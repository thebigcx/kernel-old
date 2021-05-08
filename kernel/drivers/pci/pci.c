#include <drivers/pci/pci.h>
#include <io.h>

pci_devlist_t pci_devices;

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

uint16_t pci_get_vendor_id(uint16_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readw(bus, slot, func, PCI_VENDOR_ID);
}

uint16_t pci_get_dev_id(uint16_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readw(bus, slot, func, PCI_DEVICE_ID);
}

uint8_t pci_get_class_code(uint16_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_CLASS_CODE);
}

uint8_t pci_get_subclass(uint16_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_SUBCLASS);
}

uint8_t pci_get_progif(uint16_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_PROGIF);
}

uint8_t pci_get_hdr_type(uint16_t bus, uint8_t slot, uint8_t func)
{
    return pci_cfg_readb(bus, slot, func, PCI_HDR_TYPE);
}

bool pci_check_dev(uint16_t bus, uint8_t dev, uint8_t func)
{
    if (pci_get_vendor_id(bus, dev, func) == 0xffff)
        return false;

    return true;
}

void pci_add_dev(uint16_t bus, uint8_t slot, uint8_t func)
{
    pci_dev_t dev;

    dev.vendor_id = pci_get_vendor_id(bus, slot, func);
    dev.dev_id = pci_get_dev_id(bus, slot, func);

    dev.bus = bus;
    dev.slot = slot;
    dev.func = func;

    dev.class_code = pci_get_class_code(bus, slot, func);
    dev.subclass = pci_get_subclass(bus, slot, func);
    dev.progif = pci_get_progif(bus, slot, func);

    pci_devices.devs[pci_devices.count++] = dev;
}

void pci_enumerate(acpi_mcfg_hdr_t* hdr)
{
    memset(pci_devices.devs, 0, sizeof(pci_dev_t) * PCI_DEVLIST_MAX);
    pci_devices.count = 0;

    uint8_t func = 0;

    // Use brute-force method.
    // TODO: use recursion (somehow)
    for (uint16_t bus = 0; bus < PCI_BUS_COUNT; bus++)
    for (uint8_t dev = 0; dev < PCI_DEV_PER_BUS; dev++)
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

    char buffer[100];
    for (uint32_t i = 0; i < pci_devices.count; i++)
    {
        puts(pci_class_to_str(pci_devices.devs[i].class_code));
        puts(" / ");
        puts(pci_subclass_to_str(pci_devices.devs[i].class_code, pci_devices.devs[i].subclass));
        puts(" / ");
        puts(pci_progif_to_str(pci_devices.devs[i].class_code, pci_devices.devs[i].subclass, pci_devices.devs[i].progif));
        puts("\n");
    }

    while (1);
}

const char* pci_class_to_str(uint8_t class_code)
{
    switch (class_code)
    {
        case 0x0: return "Unclassified";
        case 0x1: return "Mass Storage Controller";
        case 0x2: return "Network Controller";
        case 0x3: return "Display Controller";
        case 0x4: return "Multimedia Controller";
        case 0x5: return "Memory Controller";
        case 0x6: return "Bridge Device";
        case 0x7: return "Simple Communication Controller";
        case 0x8: return "Base System Peripheral";
    }

    return "Invalid";
}

const char* pci_subclass_to_str(uint8_t class_code, uint8_t subclass)
{
    switch (class_code)
    {
        case 0x0:
            switch (subclass)
            {
                case 0x0: return "Non-VGA-Compatible Device";
                case 0x1: return "VGA-Compatible Device";
            }
            break;

        case 0x1:
            switch (subclass)
            {
                case 0x0: return "SCSI Bus Controller";
                case 0x1: return "IDE Controller";
                case 0x2: return "Floppy Disk Controller";
                case 0x3: return "IPI Bus Controller";
                case 0x4: return "RAID Controller";
                case 0x5: return "ATA Controller";
                case 0x6: return "Serial ATA";
                case 0x7: return "Serial Attached SCSI";
                case 0x8: return "Non-Volatile Memory Controller";
                case 0x80: return "Other";
            }
            break;
    }

    return "Invalid";
}

const char* pci_progif_to_str(uint8_t class_code, uint8_t subclass, uint8_t progif)
{
    return "Invalid";
}