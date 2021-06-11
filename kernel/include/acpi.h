#pragma once

#include <stdint.h>

typedef struct acpi_rsdp
{
    uint8_t sig[8];
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t rev;
    uint32_t rsdt_addr;
    uint32_t len;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t res[3];

} __attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_sdt_hdr
{
    uint8_t sig[4];
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_tbl_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_rev;

} __attribute__((packed)) acpi_sdt_hdr_t;

typedef struct acpi_rsdt
{
    acpi_sdt_hdr_t hdr;

} __attribute__((packed)) acpi_rsdt_t;

typedef struct acpi_xsdt
{
    acpi_sdt_hdr_t hdr;
    uint64_t* sdts;

} __attribute__((packed)) acpi_xsdt_t;

typedef struct acpi_madt
{
    acpi_sdt_hdr_t hdr;
    uint32_t loc_apic_addr;
    uint32_t flags;

} __attribute__((packed)) acpi_madt_t;

typedef struct acpi_madt_ent
{
    uint8_t type;
    uint8_t len;

} __attribute__((packed)) acpi_madt_ent_t;

#define ACPI_MADT_TYPE_APICLOC          0
#define ACPI_MADT_TYPE_APICIO           1
#define ACPI_MADT_TYPE_APICIO_ISO       2
#define ACPI_MADT_TYPE_APICIO_NONMASK   3
#define ACPI_MADT_TYPE_APICLOC_NONMASK  4
#define ACPI_MADT_TYPE_APICLOC_ADDR     5
#define ACPI_MADT_TYPE_APICLOC_X2       9

typedef struct acpi_mcfg_hdr
{
    acpi_sdt_hdr_t hdr;
    uint64_t res;

} __attribute__((packed)) acpi_mcfg_hdr_t;

typedef struct apicio
{
    acpi_madt_ent_t* ent;
    uint8_t apicid;
    uint8_t res;
    uint32_t addr;
    uint32_t gsib;

} __attribute__((packed)) apicio_t;

void acpi_init(acpi_xsdt_t* xsdt_hdr);
void* acpi_find_tbl(const char* tbl);
void acpi_read_madt();