#pragma once

#define ATA_DEV_BUSY 0x80 // Busy
#define ATA_DEV_DRDY 0x40 // Drive ready
#define ATA_DEV_DF   0x20 // Drive write fault
#define ATA_DEV_DSC  0x10 // Drive seek complete
#define ATA_DEV_DRQ  0x08 // Data request ready
#define ATA_DEV_CORR 0x04 // Corrected data
#define ATA_DEV_IDX  0x02 // Index
#define ATA_DEV_ERR  0x01 // Error

#define ATA_CMD_READ_DMA_EX  0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_IDENTIFY     0xec