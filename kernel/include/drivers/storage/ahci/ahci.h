#pragma once

#include <stdint.h>

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

typedef struct fis_data
{
    uint8_t fis_type;

    uint8_t pmport : 4;
    uint8_t res0   : 4;

    uint8_t res1[2];

    uint32_t data[1];

} fis_data_t;

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
    uint32_t cap;      // Host capability
    uint32_t ghc;      // Global host control
    uint32_t int_stat; // Interrupt status
    uint32_t por_impl; // Port implemented
    uint32_t version;
    uint32_t ccc_ctl;  // Command completion coalescing control
    uint32_t ccc_pts;  // Command completion coalescing ports
    uint32_t em_loc;   // Enclosure management location
    uint32_t em_ctl;   // Enclosure management control
    uint32_t cap2;     // Host capabilities extended
    uint32_t bohc;     // BIOS/OS handoff control and status

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