#include <drivers/pci/pci.h>
#include <mem/paging.h>
#include <sys/io.h>
#include <util/stdlib.h>

list_t* pci_devs;

uint32_t pci_cfg_readl(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off)
{
    // Create config address
    uint32_t addr = (uint32_t)( ((uint32_t)bus << 16 ) |
                                ((uint32_t)slot << 11) |
                                ((uint32_t)func << 8 ) |
                                (off & 0xfc)           |
                                ((uint32_t)0x80000000)
                              );

    outl(PCI_CFG_ADDR, addr);

    return inl(PCI_CFG_DATA);
}

uint16_t pci_cfg_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off)
{
    // Create config address
    uint32_t addr = (uint32_t)( ((uint32_t)bus << 16 ) |
                                ((uint32_t)slot << 11) |
                                ((uint32_t)func << 8 ) |
                                (off & 0xfc)           |
                                ((uint32_t)0x80000000)
                              );

    outl(PCI_CFG_ADDR, addr);

    return (uint16_t)((inl(PCI_CFG_DATA) >> ((off & 2) * 8)) & 0xffff);
}

uint8_t pci_cfg_readb(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off)
{
    // Create config address
    uint32_t addr = (uint32_t)( ((uint32_t)bus << 16 ) |
                                ((uint32_t)slot << 11) |
                                ((uint32_t)func << 8 ) |
                                (off & 0xfc)           |
                                ((uint32_t)0x80000000)
                              );

    outl(PCI_CFG_ADDR, addr);

    return (uint8_t)((inl(PCI_CFG_DATA) >> ((off & 3) * 8)) & 0xff);
}

void pci_cfg_writel(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint32_t data)
{
    uint32_t addr = (uint32_t)( ((uint32_t)bus << 16 ) |
                                ((uint32_t)slot << 11) |
                                ((uint32_t)func << 8 ) |
                                (off & 0xfc)           |
                                ((uint32_t)0x80000000)
                              );
    
    outl(PCI_CFG_ADDR, addr);
    outl(PCI_CFG_DATA, data);
}

void pci_cfg_writew(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint16_t data)
{
    uint32_t addr = (uint32_t)( ((uint32_t)bus << 16 ) |
                                ((uint32_t)slot << 11) |
                                ((uint32_t)func << 8 ) |
                                (off & 0xfc)           |
                                ((uint32_t)0x80000000)
                              );
    
    outl(PCI_CFG_ADDR, addr);
    outl(PCI_CFG_DATA, (inl(PCI_CFG_DATA) & (~(0xffff << ((off & 2) * 8)))) | ((uint32_t)data << ((off & 2) * 8)));
}

void pci_cfg_writeb(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint8_t data)
{
    uint32_t addr = (uint32_t)( ((uint32_t)bus << 16 ) |
                                ((uint32_t)slot << 11) |
                                ((uint32_t)func << 8 ) |
                                (off & 0xfc)           |
                                ((uint32_t)0x80000000)
                              );
    
    outl(PCI_CFG_ADDR, addr);
    outb(PCI_CFG_DATA, (inl(PCI_CFG_DATA) & (~(0xff << ((off & 3) * 8)))) | ((uint32_t)data << ((off & 3) * 8)));
}

void pci_enable_bus_master(pci_dev_t* dev)
{
    uint16_t data = pci_cfg_readw(dev->bus, dev->slot, dev->func, PCI_COMMAND) | PCI_CMD_BUS_MASTER;
    pci_cfg_writew(dev->bus, dev->slot, dev->func, PCI_COMMAND, data);
}

void pci_enable_ints(pci_dev_t* dev)
{
    uint16_t data = pci_cfg_readw(dev->bus, dev->slot, dev->func, PCI_COMMAND) & (~PCI_CMD_INTS_DISABLE);
    pci_cfg_writew(dev->bus, dev->slot, dev->func, PCI_COMMAND, data);
}

void pci_enable_mem_space(pci_dev_t* dev)
{
    uint16_t data = pci_cfg_readw(dev->bus, dev->slot, dev->func, PCI_COMMAND) | PCI_CMD_MEM_SPACE;
    pci_cfg_writew(dev->bus, dev->slot, dev->func, PCI_COMMAND, data);
}

void pci_enable_io_space(pci_dev_t* dev)
{
    uint16_t data = pci_cfg_readw(dev->bus, dev->slot, dev->func, PCI_COMMAND) | PCI_CMD_IO_SPACE;
    pci_cfg_writew(dev->bus, dev->slot, dev->func, PCI_COMMAND, data);
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readw(bus, slot, func, PCI_VENDOR_ID);
}

uint16_t pci_get_dev_id(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readw(bus, slot, func, PCI_DEVICE_ID);
}

uint8_t pci_get_class_code(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_CLASS_CODE);
}

uint8_t pci_get_subclass(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_SUBCLASS);
}

uint8_t pci_get_progif(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_PROGIF);
}

uint8_t pci_get_hdr_type(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_HDR_TYPE);
}

uint64_t pci_get_base_addr(pci_dev_t* dev, uint8_t idx)
{
    uint64_t bar = pci_cfg_readl(dev->bus, dev->slot, dev->func, PCI_BAR0 + (idx * sizeof(uint32_t)));
    return (bar & 0x1) ? (bar & 0xFFFFFFFFFFFFFFFC) : (bar & 0xFFFFFFFFFFFFFFF0);
}

int pci_bar_isioport(pci_dev_t* dev, uint8_t idx)
{
    uint64_t bar = pci_cfg_readl(dev->bus, dev->slot, dev->func, PCI_BAR0 + (idx * sizeof(uint32_t)));
    return bar & 0x1;
}

bool pci_check_dev(uint8_t bus, uint8_t dev, uint8_t func)
{
    if (pci_get_vendor_id(bus, dev, func) == 0xffff)
        return false;

    return true;
}

void pci_add_dev(uint8_t bus, uint8_t slot, uint8_t func)
{
    pci_dev_t* dev = kmalloc(sizeof(pci_dev_t));

    dev->vendor_id = pci_get_vendor_id(bus, slot, func);
    dev->dev_id = pci_get_dev_id(bus, slot, func);

    dev->bus = bus;
    dev->slot = slot;
    dev->func = func;

    dev->class_code = pci_get_class_code(bus, slot, func);
    dev->subclass = pci_get_subclass(bus, slot, func);
    dev->progif = pci_get_progif(bus, slot, func);

    list_push_back(pci_devs, dev);
}

void pci_enumerate()
{
    pci_devs = list_create();

    uint8_t func = 0;

    // Use brute-force method.
    // TODO: use recursion (somehow)
    for (uint16_t bus = 0; bus < PCI_BUS_COUNT; bus++)
    for (uint16_t dev = 0; dev < PCI_DEV_PER_BUS; dev++)
    {
        if (pci_check_dev(bus, dev, 0))
        {
            pci_add_dev(bus, dev, 0);
            if ((pci_get_hdr_type(bus, dev, func) & 0x80) != 0)
            {
                for (func = 1; func < PCI_FUNCS_PER_DEV; func++)
                {
                    if (pci_check_dev(bus, dev, 0))
                    {
                        pci_add_dev(bus, dev, func);
                    }
                }
            }
        }
    }

    /*list_foreach(pci_devs, item)
    {
        pci_dev_t* dev = (pci_dev_t*)item->val;
        console_printf("[%x, %x, %x]\n", 255, 255, 255, dev->class_code, dev->subclass, dev->progif);
        console_printf("%x %x\n", 255, 255, 255, dev->vendor_id, dev->dev_id);
        console_printf("[%d, %d, %d]\n\n", 255, 255, 255, dev->bus, dev->slot, dev->func);
    }
    
    while (1);*/
}

const char* pci_getinf(pci_dev_t* dev)
{
    switch (dev->class_code)
    {

    }
}