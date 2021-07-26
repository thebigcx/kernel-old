#include <drivers/net/e1000/e1000.h>
#include <mem/paging.h>

#define PCIDEV_E1000        0x100e

#define REG_CTRL            0x0000
#define REG_STATUS          0x0008
#define REG_EEPROM          0x0014
#define REG_CTRLEXT         0x0018
#define REG_IMASK           0x00d0
#define REG_RCTRL           0x0100
#define REG_RXDESCLO        0x2800
#define REG_RXDESCHI        0x2804
#define REG_RXDESCLEN       0x2808
#define REG_RXDESCHEAD      0x2810
#define REG_RXDESCTAIL      0x2818

#define REG_TCTRL           0x0400
#define REG_TXDESCLO        0x3800
#define REG_TXDESCHI        0x3804
#define REG_TXDESCLEN       0x3808
#define REG_TXDESCHEAD      0x3810
#define REG_TXDESCTAIL      0x3818

#define RCTL_EN             (1 << 1 ) // Receiver enabled
#define RCTL_SBP            (1 << 2 ) // Store bad packets
#define RCTL_UPE            (1 << 3 ) // Unicast promiscuous enabled
#define RCTL_MPE            (1 << 4 ) // Multicast promiscuous enabled
#define RCTL_LPE            (1 << 5 ) // Long packet reception enable
#define RCTL_LBM_NONE       (0 << 6 ) // No loopback
#define RCTL_LBM_PHY        (3 << 6 ) // PHY or eternal SerDesc loopback
#define RCTL_RDMTS_HALF     (0 << 8 ) // Free buffer threshold 1/2 of RDLEN
#define RCTL_RDMTS_QUARTER  (1 << 8 ) // Free buffer threshold 1/4 of RDLEN
#define RCTL_RDMTS_EIGHTH   (2 << 8 ) // Free buffer threshold 1/8 of RDLEN
#define RCTL_MO36           (0 << 12) // Multicast offset - bits 47:36
#define RCTL_MO35           (1 << 12) // Multicast offset - bits 46:35
#define RCTL_MO34           (2 << 12) // Multicast offset - bits 45:34
#define RCTL_MO32           (3 << 12) // Multicast offset - bits 43:32
#define RCTL_BAM            (1 << 15) // Broadcast accept mode
#define RCTL_VFE            (1 << 18) // VLAN filter enable
#define RCTL_CFIEN          (1 << 19) // Canonical form indicator enable
#define RCTL_CFI            (1 << 20) // Canonical form indicator bit value
#define RCTL_DPF            (1 << 22) // Discard pause frames
#define RCTL_PMCF           (1 << 23) // Pass MAC control frames
#define RCTL_SECRC          (1 << 26) // Strip ethernet CRC

static uint64_t e1000_base;
static volatile uint64_t e1000_vbase;
static int bartype; // 0 = MMIO, 1 = I/O ports
static int eeprom_present;
static macaddr_t macaddr;

enum BARTYPE
{
    BAR_MMIO, BAR_IOPORT
};

// Read 8-bit value from MMIO
uint8_t e1000_read8(uint64_t addr)
{
    return *((volatile uint8_t*)(e1000_vbase + addr));
}

// Write 8-bit value to MMIO
void e1000_write8(uint64_t addr, uint8_t val)
{
    *((volatile uint8_t*)(e1000_vbase + addr)) = val;
}

// Read 16-bit value from MMIO
uint16_t e1000_read16(uint64_t addr)
{
    return *((volatile uint16_t*)(e1000_vbase + addr));
}

// Write 16-bit value to MMIO
void e1000_write16(uint64_t addr, uint16_t val)
{
    *((volatile uint16_t*)(e1000_vbase + addr)) = val;
}

// Read 32-bit value from MMIO
uint32_t e1000_read32(uint64_t addr)
{
    return *((volatile uint32_t*)(e1000_vbase + addr));
}

// Write 32-bit value to MMIO
void e1000_write32(uint64_t addr, uint32_t val)
{
    *((volatile uint32_t*)(e1000_vbase + addr)) = val;
}

// Read 64-bit value from MMIO
uint64_t e1000_read64(uint64_t addr)
{
    return *((volatile uint64_t*)(e1000_vbase + addr));
}

// Write 64-bit value to MMIO
void e1000_write64(uint64_t addr, uint64_t val)
{
    *((volatile uint64_t*)(e1000_vbase + addr)) = val;
}

// Send a command to the card
void e1000_writecmd(uint16_t addr, uint32_t val)
{
    if (bartype == BAR_MMIO)
    {
        e1000_write32(addr, val);
    }
    else
    {
        
    }
}

// Read result of command from card
uint32_t e1000_readcmd(uint16_t addr)
{
    if (bartype == BAR_MMIO)
    {
        return e1000_read32(addr);
    }
    else
    {

    }
}

// Probe the NIC's EEPROM
int e1000_probe_eeprom()
{
    int ret = 0;
    e1000_writecmd(REG_EEPROM, 0x1);

    for (int i = 0; i < 1000; i++)
    {
        uint32_t val = e1000_readcmd(REG_EEPROM);
        if (val & 0x10)
            return 1;
    }

    return 0;
}

uint32_t e1000_eeprom_read(uint8_t addr)
{
    uint32_t tmp = 0;

    if (eeprom_present)
    {
        e1000_writecmd(REG_EEPROM, 1 | (((uint32_t)addr) << 8));
        while (!((tmp = e1000_readcmd(REG_EEPROM)) & (1 << 4)));
    }
    else
    {
        e1000_writecmd(REG_EEPROM, 1 | (((uint32_t)addr) << 2));
        while (!((tmp = e1000_readcmd(REG_EEPROM)) & (1 << 1)));
    }

    return (tmp >> 16) & 0xffff;
}

// Get the MAC address from the NIC
macaddr_t e1000_read_macaddr()
{
    macaddr_t mac;

    if (eeprom_present)
    {
        uint32_t tmp;
        tmp = e1000_eeprom_read(0);
        mac.val[0] = tmp & 0xff;
        mac.val[1] = tmp >> 8;
        tmp = e1000_eeprom_read(1);
        mac.val[2] = tmp & 0xff;
        mac.val[3] = tmp >> 8;
        tmp = e1000_eeprom_read(2);
        mac.val[4] = tmp & 0xff;
        mac.val[5] = tmp >> 8;
    }
    else
    {
        if (e1000_read32(0x5400))
        {
            for (int i = 0; i < MACADDR_LEN; i++)
            {
                mac.val[i] = e1000_read8(0x5400 + i);
            }
        }
    }

    return mac;
}

// Initialize the e1000
void e1000_init(pci_dev_t* dev)
{
    e1000_base = pci_get_base_addr(dev, 0);
    e1000_vbase = page_map_mmio(e1000_base);
    bartype = pci_bar_isioport(dev, 0);

    pci_enable_bus_master(dev);

    eeprom_present = e1000_probe_eeprom();

    macaddr = e1000_read_macaddr();
    for (uint32_t i = 0; i < MACADDR_LEN - 1; i++)
    {
        serial_printf("%x:", macaddr.val[i]);
    }
    serial_printf("%x\n", macaddr.val[5]);
}