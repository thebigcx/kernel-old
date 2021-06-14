#pragma once

#include <types.h>

#include <acpi.h>

#define PCI_CFG_ADDR 0xcf8
#define PCI_CFG_DATA 0xcfc

#define PCI_BUS_COUNT     256
#define PCI_DEV_PER_BUS   32
#define PCI_FUNCS_PER_DEV 8

#define PCI_CMD_IO_SPACE     (1 << 0 )
#define PCI_CMD_MEM_SPACE    (1 << 1 )
#define PCI_CMD_BUS_MASTER   (1 << 2 )
#define PCI_CMD_INTS_DISABLE (1 << 10)

typedef struct pci_dev
{
    uint16_t dev_id;
    uint16_t vendor_id;

    uint16_t bus;
    uint8_t slot;
    uint8_t func;

    // TODO: pack these together in 'class' field
    uint8_t class_code;
    uint8_t subclass;
    uint8_t progif;

} pci_dev_t;

#define PCI_VENDOR_ID 0x0
#define PCI_DEVICE_ID 0x2
#define PCI_COMMAND 0x4
#define PCI_STATUS 0x6
#define PCI_REVISION_ID 0x8
#define PCI_PROGIF 0x9
#define PCI_SUBCLASS 0xa
#define PCI_CLASS_CODE 0xb
#define PCI_CACHE_SIZE 0xc
#define PCI_LATENCY_TIMER 0xd
#define PCI_HDR_TYPE 0xe
#define PCI_BIST 0xf
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1c
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_CARDBUS_CIS_PTR 0x28
#define PCI_SUBSYS_VENDOR_ID 0x2c
#define PCI_SUBSYS_ID 0x2e
#define PCI_EXPAN_ROM_BASE_ADDR 0x30
#define PCI_CAPAB_PTR 0x34
#define PCI_INTERRUPT_LINE 0x3c
#define PCI_INTERRUPT_PIN 0x3d
#define PCI_MIN_GRANT 0x3e
#define PCI_MAX_LATENCY 0x3f

#define PCI_DEVLIST_MAX 64

typedef struct pci_devlist
{
    pci_dev_t devs[PCI_DEVLIST_MAX];
    uint32_t count;

} pci_devlist_t;

extern pci_devlist_t pci_devices;

void pci_enable_bus_master(pci_dev_t* dev);
void pci_enable_ints(pci_dev_t* dev);
void pci_enable_mem_space(pci_dev_t* dev);
void pci_enable_io_space(pci_dev_t* dev);

// Read PCI config data
uint32_t pci_cfg_readl(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);
uint16_t pci_cfg_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);
uint8_t pci_cfg_readb(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);

// Write PCI config data
void pci_cfg_writel(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint32_t data);
void pci_cfg_writew(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint16_t data);
void pci_cfg_writeb(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off, uint8_t data);

// Get vendor
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_get_dev_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_class_code(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_subclass(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_progif(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_hdr_type(uint8_t bus, uint8_t slot, uint8_t func);
uint64_t pci_get_base_addr(pci_dev_t* dev, uint8_t idx);

bool pci_check_dev(uint8_t bus, uint8_t dev, uint8_t func);

void pci_add_dev(uint8_t bus, uint8_t slot, uint8_t func);
void pci_enumerate();

const char* pci_class_to_str(uint8_t class_code);
const char* pci_subclass_to_str(uint8_t class_code, uint8_t subclass);
const char* pci_progif_to_str(uint8_t class_code, uint8_t subclass, uint8_t progif);