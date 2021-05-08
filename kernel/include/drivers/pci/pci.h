#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <acpi.h>

#define PCI_CFG_ADDR 0xcf8
#define PCI_CFG_DATA 0xcfc

#define PCI_BUS_COUNT     256
#define PCI_DEV_PER_BUS   32
#define PCI_FUNCS_PER_DEV 8

typedef struct pci_dev
{
    uint16_t dev_id;
    uint16_t vendor_id;

    uint16_t bus;
    uint8_t slot;
    uint8_t func;

    uint8_t class_code;
    uint8_t subclass;
    uint8_t progif;

} pci_dev_t;

typedef enum PCI_CFG_REGS
{
    PCI_VENDOR_ID = 0x0,
    PCI_DEVICE_ID = 0x2,
    PCI_COMMAND = 0x4,
    PCI_STATUS = 0x6,
    PCI_REVISION_ID = 0x8,
    PCI_PROGIF = 0x9,
    PCI_SUBCLASS = 0xa,
    PCI_CLASS_CODE = 0xb,
    PCI_CACHE_SIZE = 0xc,
    PCI_LATENCY_TIMER = 0xd,
    PCI_HDR_TYPE = 0xe,
    PCI_BIST = 0xf

} PCI_CFG_REGS;

#define PCI_DEVLIST_MAX 64

typedef struct pci_devlist
{
    pci_dev_t devs[PCI_DEVLIST_MAX];
    uint32_t count;

} pci_devlist_t;

extern pci_devlist_t pci_devices;

// Read PCI config word
uint16_t pci_cfg_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);
uint8_t pci_cfg_readb(uint8_t bus, uint8_t slot, uint8_t func, uint8_t off);

// Get vendor
uint16_t pci_get_vendor_id(uint16_t bus, uint8_t slot, uint8_t func);
uint16_t pci_get_dev_id(uint16_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_class_code(uint16_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_subclass(uint16_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_progif(uint16_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_hdr_type(uint16_t bus, uint8_t slot, uint8_t func);

bool pci_check_dev(uint16_t bus, uint8_t dev, uint8_t func);

void pci_add_dev(uint16_t bus, uint8_t slot, uint8_t func);
void pci_enumerate(acpi_mcfg_hdr_t* hdr);

const char* pci_class_to_str(uint8_t class_code);
const char* pci_subclass_to_str(uint8_t class_code, uint8_t subclass);
const char* pci_progif_to_str(uint8_t class_code, uint8_t subclass, uint8_t progif);