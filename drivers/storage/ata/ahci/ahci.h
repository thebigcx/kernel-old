#pragma once

#include <util/types.h>
#include <util/list.h>
#include <fs/vfs/vfs.h>
#include <drivers/pci/pci.h>

#define AHCI_PORT_NULL 0
#define AHCI_PORT_SATA 1
#define AHCI_PORT_SEMB 2
#define AHCI_PORT_PM 3
#define AHCI_PORT_SATAPI 4

#define AHCI_CMD_SLOTS 32

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define	SATA_SIG_ATA   0x00000101 // SATA drive
#define	SATA_SIG_ATAPI 0xeb140101 // SATAPI drive
#define	SATA_SIG_SEMB  0xc33c0101 // Enclosure management bridge
#define	SATA_SIG_PM	   0x96690101 // Port multiplier

#define HBA_PXCMD_ST  0x0001
#define HBA_PXCMD_FRE 0x0010
#define HBA_PXCMD_FR  0x4000
#define HBA_PXCMD_CR  0x8000

#define HBA_PXIS_TFES (1 << 30)

#define SECTOR_SIZE 512

typedef enum FIS_TYPE
{
    FIS_TYPE_REG_H2D   = 0x27, // Host to device
    FIS_TYPE_REG_D2H   = 0x34, // Device to host
    FIS_TYPE_DMA_ACT   = 0x39, // DMA activate FIS
    FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS
    FIS_TYPE_DATA      = 0x46, // Data FIS
    FIS_TYPE_BIST      = 0x58, // BIST activate FIS
    FIS_TYPE_PIO_SETUP = 0x5f, // PIO setup FIS
    FIS_TYPE_DEV_BITS  = 0xa1  // Set device bits FIS
} FIS_TYPE;

typedef struct fis_reg_h2d
{
    uint8_t fis_type;

    uint8_t pmport   : 4;
    uint8_t res0     : 3; // Reserved
    uint8_t com_ctrl : 1;

    uint8_t command;
    uint8_t featurel; // Feature low byte

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t featureh; // Feature high byte

    uint8_t countl; // Count low byte
    uint8_t counth; // Count high byte
    uint8_t icc;    // Isochronous command completion
    uint8_t control;

    uint8_t res1[4]; // Reserved

} fis_reg_h2d_t;

typedef struct fis_reg_d2h
{
    uint8_t fis_type;

    uint8_t pmport   : 4;
    uint8_t res0     : 2; // Reserved
    uint8_t intr     : 1;
    uint8_t res1     : 1;

    uint8_t stat;
    uint8_t err;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t res2;

    uint8_t countl; // Count low byte
    uint8_t counth; // Count high byte
    uint8_t res3[2];

    uint8_t res4[4]; // Reserved

} fis_reg_d2h_t;

typedef struct fis_data
{
    uint8_t fis_type;

    uint8_t pmport : 4;
    uint8_t res0   : 4;

    uint8_t res1[2];

    uint32_t data[1];

} fis_data_t;

typedef struct fis_pio_setup
{
    uint8_t fis_type;

    uint8_t pmport   : 4;
    uint8_t res0     : 1; // Reserved
    uint8_t datadir  : 1; // Data direction
    uint8_t intr     : 1;
    uint8_t res1     : 1;

    uint8_t stat;
    uint8_t err;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t res2;

    uint8_t countl; // Count low byte
    uint8_t counth; // Count high byte
    uint8_t res3;
    uint8_t estat;

    uint16_t transcnt;  // Transfer count    
    uint8_t res4[2]; // Reserved

} fis_pio_setup_t;

typedef struct fis_dma_setup
{
    uint8_t fis_type;

    uint8_t pmport   : 4;
    uint8_t res0     : 1; // Reserved
    uint8_t datadir  : 1; // Data direction
    uint8_t intr     : 1;
    uint8_t autoact  : 1;

    uint8_t res1[2];

    uint64_t dmabufid; // DMA buffer ID

    uint32_t res2;

    uint32_t dmabufoff; // Byte offset into the buffer

    uint32_t transcnt;  // Transfer count    

    uint32_t res3[2]; // Reserved

} fis_dma_setup_t;

typedef struct hba_fis
{
    fis_dma_setup_t dsfis; // DMA setup FIS
    uint8_t         pad0[4];

    fis_pio_setup_t psfis; // PIO setup FIS
    uint8_t         pad1[12];

    fis_reg_d2h_t   rfis; // Register FIS
    uint8_t         pad[4];

    uint8_t         sdbfis[8]; // Set Device Bit FIS

    uint8_t         ufis[64];
    uint8_t         res[0x100 - 0xa0];

} hba_fis_t;

typedef volatile struct hba_port
{
    uint32_t com_base_addr;   // Command list base address
    uint32_t com_base_addr_u; // Command list base address upper
    uint32_t fis_base;        // FIS base address
    uint32_t fis_base_u;      // FIS base address upper
    uint32_t int_stat;        // Interrupt status
    uint32_t int_enable;      // Interrupt enable
    uint32_t cmd_stat;        // Command and status
    uint32_t res0;            // Reserved
    uint32_t task_file_dat;   // Task file data
    uint32_t sig;             // Signature
    uint32_t sata_stat;       // SATA status
    uint32_t sata_ctrl;       // SATA control
    uint32_t sata_err;        // SATA error
    uint32_t sata_active;     // SATA active
    uint32_t cmd_issue;       // Command issue
    uint32_t sata_notif;      // SATA notification
    uint32_t fis_switch_ctrl; // FIS switch control
    uint32_t res1[11];        // Reserved
    uint32_t vendor[4];       // Vendor specific

} hba_port_t;

typedef volatile struct hba_memory
{
    uint32_t cap;        // Host capability
    uint32_t ghc;        // Global host control
    uint32_t int_stat;   // Interrupt status
    uint32_t ports_impl; // Port implemented
    uint32_t version;
    uint32_t ccc_ctl;    // Command completion coalescing control
    uint32_t ccc_pts;    // Command completion coalescing ports
    uint32_t em_loc;     // Enclosure management location
    uint32_t em_ctl;     // Enclosure management control
    uint32_t cap2;       // Host capabilities extended
    uint32_t bohc;       // BIOS/OS handoff control and status

    uint8_t res[0x74];

    uint8_t vendor[0x60];

    hba_port_t ports[1];

} hba_memory_t;

typedef struct hba_cmd_header
{
    uint8_t cmd_fis_len : 5;      // Command FIS length
    uint8_t atapi       : 1;      // ATAPI
    uint8_t write       : 1;      // Write
    uint8_t prefetch    : 1;      // Prefetchable

    uint8_t reset       : 1;      // Reset
    uint8_t bist        : 1;      // BIST
    uint8_t clear_busy  : 1;      // Clear busy upon R_OK
    uint8_t res0        : 1;      // Reserved
    uint8_t port_mul    : 4;      // Port multiplier port

    uint16_t prdt_len;            // PRD table length (in entries)

    volatile uint32_t prdb_cnt;   // PRD byte count

    uint32_t cmd_tbl_base_addr;   // Command table base address
    uint32_t cmd_tbl_base_addr_u; // Command table base address upper

    uint32_t res1[4];             // Reserved

} hba_cmd_header_t;

typedef struct hba_prdt_entry
{
    uint32_t data_base_addr;   // Data base address
    uint32_t data_base_addr_u; // Data base address upper
    uint32_t res0;             // Reserved

    uint32_t byte_cnt    : 22; // Byte count
    uint32_t res1        : 9;  // Reserved
    uint32_t int_on_cmpl : 1;  // Interrupt on completion

} hba_prdt_entry_t;

typedef struct hba_cmd_tbl
{
    uint8_t cmd_fis[64];            // Command FIS
    uint8_t atapi_cmd[16];          // ATAPI command
    uint8_t res[48];                // Reserved
    hba_prdt_entry_t prdt_entry[1]; // PRDT entries 0 ~ 65535

} hba_cmd_tbl_t;

typedef struct ahci_port
{
    hba_port_t* hba_port;
    uint32_t type;
    uint8_t num;

    hba_cmd_header_t* comlist;
    hba_cmd_tbl_t* comtables[32];
    hba_fis_t* fis;
    void* buf;
    uint64_t phys_buf;

} ahci_port_t;

typedef struct ahci_dev
{
    ahci_port_t* port;

} ahci_dev_t;

extern list_t* ahci_ports;

void ahci_probe_ports();
void ahci_port_rebase(ahci_port_t* port);
void ahci_init_dev(pci_dev_t* pci_dev);
void ahci_start_cmd(ahci_port_t* port);
void ahci_stop_cmd(ahci_port_t* port);

bool ahci_access(ahci_port_t* ahciport, uint64_t sector, uint32_t cnt, void* buffer, int write);
size_t ahci_read(vfs_node_t* node, void* ptr, uint64_t off, uint32_t len);
size_t ahci_write(vfs_node_t* node, void* ptr, uint64_t off, uint32_t len);

vfs_node_t* ahci_get_dev(int idx);

// Initialize AHCI, devs is a list of all PCI devices
void ahci_init(list_t* devs);