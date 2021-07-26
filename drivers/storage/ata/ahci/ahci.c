#include <drivers/storage/ata/ahci/ahci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ata/ata.h>
#include <mem/paging.h>
#include <util/stdlib.h>
#include <mem/kheap.h>
#include <sys/console.h>
#include <mem/pmm.h>

hba_memory_t* abar;
hba_memory_t* vabar;
list_t* ahci_ports;

static int check_type(hba_port_t* port)
{
    uint32_t sata_stat = port->sata_stat;

    uint8_t ipm = (sata_stat >> 8) & 0xf;
    uint8_t det = sata_stat & 0xf;

    if (det != HBA_PORT_DET_PRESENT)
        return AHCI_PORT_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_PORT_NULL;

    switch (port->sig)
    {
        case SATA_SIG_ATA:
            return AHCI_PORT_SATA;
        case SATA_SIG_ATAPI:
            return AHCI_PORT_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_PORT_SEMB;
        case SATA_SIG_PM:
            return AHCI_PORT_PM;
        default:
            return AHCI_PORT_NULL;
    }
}

void ahci_probe_ports()
{
    ahci_ports = list_create();

    uint32_t ports_impl = vabar->ports_impl;
    for (uint32_t i = 0; i < 32; i++)
    {
        if (ports_impl & (1 << i))
        {
            int pt = check_type(&vabar->ports[i]);

            if (pt == AHCI_PORT_SATA || pt == AHCI_PORT_SATAPI)
            {
                serial_writestr("Found AHCI port\n");
                ahci_port_t* port = (ahci_port_t*)kmalloc(sizeof(ahci_port_t));
                port->hba_port = &vabar->ports[i];
                port->type = pt;
                port->num = ahci_ports->cnt;
                port->phys_buf = pmm_request();
                port->buf = page_kernel_alloc4k(1);
                page_kernel_map_memory(port->buf, port->phys_buf, 1);
                list_push_back(ahci_ports, port);
            }
        }
    }
}

void ahci_init_dev(pci_dev_t* pci_dev)
{
    pci_enable_bus_master(pci_dev);
    pci_enable_ints(pci_dev);
    pci_enable_mem_space(pci_dev);

    abar = (hba_memory_t*)pci_get_base_addr(pci_dev, 5);
    vabar = page_map_mmio(abar);

    ahci_probe_ports();

    list_foreach(ahci_ports, port)
    {
        ahci_port_rebase((ahci_port_t*)port->val);
    }
}

void ahci_port_rebase(ahci_port_t* ahci_port)
{
    hba_port_t* port = ahci_port->hba_port;

    ahci_stop_cmd(ahci_port);

    uint64_t phys;

    phys = pmm_request();

    port->com_base_addr = (uint32_t)((uint64_t)phys & 0xffffffff);
    port->com_base_addr_u = (uint32_t)((uint64_t)phys >> 32); // Upper 32 bits
    ahci_port->comlist = page_map_mmio(phys);

    memset(ahci_port->comlist, 0, PAGE_SIZE_4K);

    phys = pmm_request();
    port->fis_base = (uint32_t)((uint64_t)phys & 0xffffffff);
    port->fis_base_u = (uint32_t)((uint64_t)phys >> 32); // Upper 32 bit
    ahci_port->fis = page_map_mmio(phys);
    memset(ahci_port->fis, 0, PAGE_SIZE_4K);

    ahci_port->fis->dsfis.fis_type = FIS_TYPE_DMA_SETUP;
    ahci_port->fis->psfis.fis_type = FIS_TYPE_PIO_SETUP;
    ahci_port->fis->rfis.fis_type = FIS_TYPE_REG_D2H;
    ahci_port->fis->sdbfis[0] = FIS_TYPE_DEV_BITS;

    for (uint32_t i = 0; i < 8; i++)
    {
        ahci_port->comlist[i].prdt_len = 1;
        
        phys = pmm_request();
        ahci_port->comlist[i].cmd_tbl_base_addr = (uint32_t)(uint64_t)phys;
        ahci_port->comlist[i].cmd_tbl_base_addr_u = (uint32_t)((uint64_t)phys >> 32);

        ahci_port->comtables[i] = page_map_mmio(phys);
        memset(ahci_port->comtables[i], 0, PAGE_SIZE_4K);
    }

    ahci_start_cmd(ahci_port);
}

void ahci_start_cmd(ahci_port_t* port)
{
    while (port->hba_port->cmd_stat & HBA_PXCMD_CR);

    port->hba_port->cmd_stat |= HBA_PXCMD_FRE;
    port->hba_port->cmd_stat |= HBA_PXCMD_ST;
}

void ahci_stop_cmd(ahci_port_t* port)
{
    port->hba_port->cmd_stat &= ~HBA_PXCMD_ST;
    port->hba_port->cmd_stat &= ~HBA_PXCMD_FRE;

    while (1)
    {
        if (port->hba_port->cmd_stat & HBA_PXCMD_FR)
            continue;
        if (port->hba_port->cmd_stat & HBA_PXCMD_CR)
            continue;
        break;
    }
}

static int32_t find_cmd_slot(hba_port_t* port)
{
    uint32_t slots = (port->sata_active | port->cmd_issue);

    for (int32_t i = 0; i < AHCI_CMD_SLOTS; i++)
    {
        if ((slots & i) == 0)
            return i;

        slots >>= 1;
    }

    serial_writestr("[AHCI] Could not find free command list entry\n");
    return -1;
}

bool ahci_access(ahci_port_t* ahciport, uint64_t sector, uint32_t cnt, void* buffer, int write)
{
    hba_port_t* port = ahciport->hba_port;

    port->int_stat = 0xffffffff;
    
    int slot = find_cmd_slot(port);
    if (slot == -1)
        return false;

    hba_cmd_header_t* cmdhdr = &ahciport->comlist[slot];
    cmdhdr->cmd_fis_len = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    cmdhdr->write = write;
    cmdhdr->prdt_len = (uint16_t)((cnt - 1) >> 4) + 1;

    hba_cmd_tbl_t* cmdtbl = ahciport->comtables[slot];
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t));

    cmdtbl->prdt_entry[0].data_base_addr = (uint32_t)(uint64_t)buffer;
    cmdtbl->prdt_entry[0].data_base_addr_u = (uint32_t)((uint64_t)buffer >> 32);
    cmdtbl->prdt_entry[0].byte_cnt = cnt * 512 - 1;
    cmdtbl->prdt_entry[0].int_on_cmpl = 1;

    fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cmd_fis);
    memset(cmdfis, 0, sizeof(fis_reg_h2d_t));

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->com_ctrl = 1; // Command
    cmdfis->command = write ? ATA_CMD_WRITE_DMA_EX : ATA_CMD_READ_DMA_EX;

    cmdfis->lba0 = (uint8_t)sector;
    cmdfis->lba1 = (uint8_t)(sector >> 8);
    cmdfis->lba2 = (uint8_t)(sector >> 16);
    cmdfis->lba3 = (uint8_t)(sector >> 24);
    cmdfis->lba4 = (uint8_t)(sector >> 32);
    cmdfis->lba5 = (uint8_t)(sector >> 40);

    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->countl = cnt & 0xff;
    cmdfis->counth = (cnt >> 8) & 0xff;

    uint64_t spin = 0;

    while ((port->task_file_dat & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        serial_writestr("[AHCI] Port has hung\n");
        return false;
    }

    port->cmd_issue = 1 << slot;

    // Wait for completion
    while (port->cmd_issue & (1 << slot))
    {
        if (port->int_stat & HBA_PXIS_TFES) // Task file error
        {
            serial_writestr("[AHCI] Read disk error\n");
            return false;
        }
    }

    // Check again
    if (port->int_stat & HBA_PXIS_TFES)
    {
        serial_writestr("[AHCI] Read disk error\n");
        return false;
    }

    return true;
}

size_t ahci_read(vfs_node_t* node, void* ptr, uint64_t off, uint32_t len)
{
    ahci_port_t* port = ((ahci_dev_t*)node->device)->port;

    while (len)
    {
        ahci_access(port, off, len, port->phys_buf, 0);
        memcpy(ptr, port->buf, 512);
        ptr += 512;
        len--;
        off++;
    }

    return len;
}

size_t ahci_write(vfs_node_t* node, void* ptr, uint64_t off, uint32_t len)
{
    ahci_port_t* port = ((ahci_dev_t*)node->device)->port;

    while (len)
    {
        memcpy(port->buf, ptr, 512);
        ahci_access(port, off, len, port->phys_buf, 1);
        ptr += 512;
        len--;
        off++;
    }

    return len;
}

vfs_node_t* ahci_get_dev(int idx)
{
    if (idx >= ahci_ports->cnt)
    {
        serial_printf("[AHCI] Invalid AHCI port number: %d\n", idx);
        return NULL;
    }

    vfs_node_t* dev = kmalloc(sizeof(vfs_node_t));

    dev->read = ahci_read;
    dev->write = ahci_write;
    dev->device = kmalloc(sizeof(ahci_dev_t));
    dev->flags = FS_BLKDEV;

    ((ahci_dev_t*)dev->device)->port = (ahci_port_t*)list_get(ahci_ports, idx)->val;

    return dev;
}

void ahci_init(list_t* devs)
{
    list_foreach(devs, item)
    {
        pci_dev_t* dev = (pci_dev_t*)item->val;

        if (dev->class_code == PCI_CLASS_STORAGE && dev->subclass == PCI_SC_SATA && dev->progif == PCI_PI_AHCI)
        {
            ahci_init_dev(dev);
        }
    }
}