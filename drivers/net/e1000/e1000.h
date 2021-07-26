#pragma once

#include <util/types.h>
#include <drivers/pci/pci.h>
#include <net/net.h>

#define E1000_NUM_RXDESC 32
#define E1000_NUM_TXDESC 8

typedef struct e1000_rxdesc
{
    volatile uint64_t addr;
    volatile uint16_t len;
    volatile uint16_t checksum;
    volatile uint8_t  stat;
    volatile uint8_t  err;
    volatile uint16_t magic;

} __attribute__((packed)) e1000_rxdesc_t;

uint8_t e1000_read8(uint64_t addr);
void e1000_write8(uint64_t addr, uint8_t val);

uint16_t e1000_read16(uint64_t addr);
void e1000_write16(uint64_t addr, uint16_t val);

uint32_t e1000_read32(uint64_t addr);
void e1000_write32(uint64_t addr, uint32_t val);

uint64_t e1000_read64(uint64_t addr);
void e1000_write64(uint64_t addr, uint64_t val);

void e1000_writecmd(uint16_t addr, uint32_t val);
uint32_t e1000_readcmd(uint16_t addr);

int e1000_probe_eeprom();
uint32_t e1000_eeprom_read(uint8_t addr);

macaddr_t e1000_read_macaddr();

void e1000_init(pci_dev_t* dev);