#include <drivers/storage/ahci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ata.h>
#include <drivers/storage/dev.h>

#include <stdlib.h>
#include <paging/paging.h>
#include <string.h>
#include <stdio.h>

hba_memory_t* abar;
ahci_portlist_t ahci_portlist;

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
    memset(ahci_portlist.ports, 0, sizeof(ahci_port_t*) * AHCI_PORTLIST_MAX);
    ahci_portlist.count = 0;

    uint32_t ports_impl = abar->ports_impl;
    for (uint32_t i = 0; i < 32; i++) // TEMP: 1 port implemented
    {
        if (ports_impl & (1 << i))
        {
            int pt = check_type(&abar->ports[i]);

            if (pt == AHCI_PORT_SATA || pt == AHCI_PORT_SATAPI)
            {
                ahci_port_t* port = (ahci_port_t*)kmalloc(sizeof(ahci_port_t));
                port->hba_port = &abar->ports[i];
                port->type = pt;
                port->num = ahci_portlist.count;
                ahci_portlist.ports[ahci_portlist.count++] = port;
            }
        }
    }
}

void ahci_init_dev(pci_dev_t* pci_base_addr)
{
    pci_enable_bus_master(pci_base_addr);
    pci_enable_ints(pci_base_addr);
    pci_enable_mem_space(pci_base_addr);

    abar = (hba_memory_t*)pci_get_base_addr(pci_base_addr, 5);
    page_kernel_map_memory((void*)abar, (void*)abar);

    ahci_probe_ports();

    for (uint32_t i = 0; i < ahci_portlist.count; i++)
    {
        ahci_port_rebase(ahci_portlist.ports[i]);
    }

    puts("AHCI initialized\n");
}

void ahci_port_rebase(ahci_port_t* ahci_port)
{
    hba_port_t* port = ahci_port->hba_port;

    ahci_stop_cmd(ahci_port);

    void* new_base = page_request();
    port->com_base_addr = (uint32_t)((uint64_t)new_base & 0xffffffff);
    port->com_base_addr_u = (uint32_t)((uint64_t)new_base >> 32); // Upper 32 bits
    memset((void*)(uint64_t)port->com_base_addr, 0, PAGE_SIZE);

    void* fis_base = kmalloc(256);
    port->fis_base = (uint32_t)((uint64_t)fis_base & 0xffffffff);
    port->fis_base_u = (uint32_t)((uint64_t)fis_base >> 32); // Upper 32 bits
    memset((void*)(uint64_t)port->fis_base, 0, 256);

    hba_cmd_header_t* cmd_hdr = (hba_cmd_header_t*)((uint64_t)port->com_base_addr + ((uint64_t)port->com_base_addr_u << 32));
    for (uint32_t i = 0; i < 32; i++)
    {
        cmd_hdr[i].prdt_len = 8;
        
        void* cmd_tbl = page_request();
        uint64_t addr = (uint64_t)cmd_tbl + (i << 8);
        cmd_hdr[i].cmd_tbl_base_addr = (uint32_t)(uint64_t)addr;
        cmd_hdr[i].cmd_tbl_base_addr_u = (uint32_t)((uint64_t)addr >> 32);
        memset(cmd_tbl, 0, 256);
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

    puts("[ERROR] AHCI : Could not find free command list entry.\n");
    return -1;
}

bool ahci_read(ahci_port_t* ahciport, uint64_t sector, uint32_t cnt, void* buffer)
{
    return ahci_access(ahciport, sector, cnt, buffer, 0);
}

bool ahci_write(ahci_port_t* ahciport, uint64_t sector, uint32_t cnt, void* buffer)
{
    return ahci_access(ahciport, sector, cnt, buffer, 1);
}

bool ahci_access(ahci_port_t* ahciport, uint64_t sector, uint32_t cnt, void* buffer, int write)
{
    hba_port_t* port = ahciport->hba_port;

    port->int_stat = (uint32_t) -1;
    
    int slot = find_cmd_slot(port);
    if (slot == -1)
        return false;

    hba_cmd_header_t* cmd_hdr = (hba_cmd_header_t*)(uint64_t)port->com_base_addr;
    cmd_hdr += slot;
    cmd_hdr->cmd_fis_len = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    cmd_hdr->write = write;
    cmd_hdr->prdt_len = (uint16_t)((cnt - 1) >> 4) + 1;

    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t*)(uint64_t)(cmd_hdr->cmd_tbl_base_addr);
    memset(cmd_tbl, 0, sizeof(hba_cmd_tbl_t) + (cmd_hdr->prdt_len - 1) * sizeof(hba_prdt_entry_t));

    uint16_t i = 0;
    for (; i < cmd_hdr->prdt_len - 1; i++)
    {
        cmd_tbl->prdt_entry[i].data_base_addr = (uint32_t)(uint64_t)buffer;
        cmd_tbl->prdt_entry[i].data_base_addr_u = (uint32_t)((uint64_t)buffer >> 32);
        cmd_tbl->prdt_entry[i].byte_cnt = 8 * 1024 - 1;
        cmd_tbl->prdt_entry[i].int_on_cmpl = 1;

        buffer = (void*)((uint64_t)buffer + 8 * 1024);
    }

    cmd_tbl->prdt_entry[i].data_base_addr = (uint32_t)(uint64_t)buffer;
    cmd_tbl->prdt_entry[i].data_base_addr_u = (uint32_t)((uint64_t)buffer >> 32);
    cmd_tbl->prdt_entry[i].byte_cnt = (cnt << 9) - 1;
    cmd_tbl->prdt_entry[i].int_on_cmpl = 1;

    fis_reg_h2d_t* cmd_fis = (fis_reg_h2d_t*)(&cmd_tbl->cmd_fis);

    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->com_ctrl = 1; // Command
    cmd_fis->command = write ? ATA_CMD_WRITE_DMA_EX : ATA_CMD_READ_DMA_EX;

    cmd_fis->lba0 = (uint8_t)sector;
    cmd_fis->lba1 = (uint8_t)(sector >> 8);
    cmd_fis->lba2 = (uint8_t)(sector >> 16);
    cmd_fis->lba3 = (uint8_t)(sector >> 24);
    cmd_fis->lba4 = (uint8_t)(sector >> 32);
    cmd_fis->lba5 = (uint8_t)(sector >> 40);

    cmd_fis->device = 1 << 6; // LBA mode

    cmd_fis->countl = cnt & 0xff;
    cmd_fis->counth = (cnt >> 8) & 0xff;

    uint64_t spin = 0;

    while ((port->task_file_dat & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        puts("[ERROR] AHCI : Port has hung.\n");
        return false;
    }

    port->cmd_issue = 1 << slot;

    // Wait for completion
    while (1)
    {
        if ((port->cmd_issue & (1 << slot)) == 0)
            break;
        if (port->int_stat & HBA_PXIS_TFES) // Task file error
        {
            puts("[ERROR] AHCI : Read disk error.\n");
            return false;
        }
    }

    // Check again
    if (port->int_stat & HBA_PXIS_TFES)
    {
        puts("[ERROR] AHCI : Read disk error.\n");
        return false;
    }

    return true;
}

int ahci_storage_dev_read(dev_t* dev, uint64_t offset, uint32_t len, void* buffer)
{
    return ahci_read(((disk_dev_t*)dev->derived)->port, offset, len, buffer);
}

int ahci_storage_dev_write(dev_t* dev, uint64_t offset, uint32_t len, void* buffer)
{
    return ahci_write(((disk_dev_t*)dev->derived)->port, offset, len, buffer);
}

dev_t ahci_get_dev(int idx)
{
    dev_t dev;
    dev.read = ahci_storage_dev_read;
    dev.write = ahci_storage_dev_write;
    dev.derived = kmalloc(sizeof(disk_dev_t));
    ((disk_dev_t*)dev.derived)->port = ahci_portlist.ports[idx];
    return dev;
}

void ahci_init(pci_devlist_t* devs)
{
    for (uint32_t i = 0; i < pci_devices.count; i++)
    {
        pci_dev_t* dev = &pci_devices.devs[i];
        
        if (dev->class_code == PCI_CLASS_STORAGE && dev->subclass == PCI_SUBCLASS_SATA && dev->progif == PCI_PROGIF_AHCI)
        {
            ahci_init_dev(dev);
        }
    }
}