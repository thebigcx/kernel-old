#pragma once

#include <stdint.h>

typedef struct acpi_rsdt2
{
    unsigned char sig[8];
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;
    uint32_t len;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t res[3];

} __attribute__((packed)) acpi_rsdt2_t;

typedef struct acpi_rsdp_desc
{
    unsigned char sig[8];
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t revision;
    uint32_t rsdt_addr;

    uint32_t len;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t res[3];

} __attribute__((packed)) acpi_rsdp_desc_t;

typedef struct acpi_sdt_hdr
{
    unsigned char sig[4];
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_tbl_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_rev;

} __attribute__((packed)) acpi_sdt_hdr_t;

typedef struct acpi_mcfg_hdr
{
    acpi_sdt_hdr_t hdr;
    uint64_t res;

} __attribute__((packed)) acpi_mcfg_hdr_t;

void* acpi_find_tbl(acpi_sdt_hdr_t* sdt, const char* tbl);