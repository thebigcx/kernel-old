#pragma once

#include <util/types.h>

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

} __attribute__((packed)) acpi_xsdp_t;

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
    uint32_t sdts[100];

} __attribute__((packed)) acpi_rsdt_t;

typedef struct acpi_xsdt
{
    acpi_sdt_hdr_t hdr;
    uint64_t sdts[100];

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

typedef struct gen_addr
{
    uint8_t addr_space;
    uint8_t bit_width;
    uint8_t bit_off;
    uint8_t acc_size;
    uint64_t addr;

} __attribute__((packed)) gen_addr_t;

typedef struct acpi_fadt
{
    acpi_sdt_hdr_t hdr;

    uint32_t    firmware_ctrl;
    uint32_t    dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t     res;
 
    uint8_t     pref_pm_prof;
    uint16_t    sci_int;
    uint32_t    smi_cmd_port;
    uint8_t     acpi_enable;
    uint8_t     acpi_disable;
    uint8_t     s4bios_req;
    uint8_t     pstate_ctrl;
    uint32_t    pm1a_evt_blk;
    uint32_t    pm1b_evt_blk;
    uint32_t    pm1a_ctrl_blk;
    uint32_t    pm1b_ctrl_blk;
    uint32_t    pm2_ctrl_blk;
    uint32_t    pm_time_blk;
    uint32_t    gpe0_blk;
    uint32_t    gpe1_blk;
    uint8_t     pm1_evt_len;
    uint8_t     pm1_ctrl_len;
    uint8_t     pm2_ctrl_len;
    uint8_t     pm_time_len;
    uint8_t     gpe0_len;
    uint8_t     gpe1_len;
    uint8_t     gpe1_base;
    uint8_t     cstate_ctrl;
    uint16_t    worst_c2_lat;
    uint16_t    worst_c3_lat;
    uint16_t    flush_size;
    uint16_t    flush_stride;
    uint8_t     duty_off;
    uint8_t     duty_width;
    uint8_t     day_alrm;
    uint8_t     month_alrm;
    uint8_t     cent;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t    boot_arch_flags;
 
    uint8_t     res2;
    uint32_t    flags;

    gen_addr_t  reset_reg;
 
    uint8_t     reset_val;
    uint8_t     res3[3];
 
    // 64-bit pointers - available on ACPI 2.0+
    uint64_t    x_fw_ctrl;
    uint64_t    x_dsdt;
 
    gen_addr_t  x_pm1a_evt_blk;
    gen_addr_t  x_pm1abevt_blk;
    gen_addr_t  x_pm1a_ctrl_blk;
    gen_addr_t  x_pm1b_ctrl_blk;
    gen_addr_t  x_pm2_ctrl_blk;
    gen_addr_t  x_pm_time_blk;
    gen_addr_t  x_gpe0_blk;
    gen_addr_t  x_gpe1_blk;

} __attribute__((packed)) acpi_fadt_t;

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

typedef struct ioapic
{
    acpi_madt_ent_t ent;
    uint8_t apicid;
    uint8_t res;
    uint32_t addr;
    uint32_t gsib;

} __attribute__((packed)) ioapic_t;

typedef struct lapic
{
    acpi_madt_ent_t ent;
    uint8_t id;
    uint8_t apic_id;
    uint32_t flags;

} __attribute__((packed)) lapic_t;

#define ACPI_APICLOC_ENABLE         (1 << 0)
#define ACPI_APICLOC_ONLINE_CAPABLE (1 << 2)

typedef struct apic_iso
{
    acpi_madt_ent_t ent;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;

} __attribute__((packed)) apic_iso_t;

typedef struct iso_lst
{
    apic_iso_t* data[200];
    uint32_t cnt;

} iso_lst_t;

extern iso_lst_t acpi_isos;
extern uint32_t acpi_cpus[64]; // Max 64 cores
extern uint32_t acpi_cpu_cnt;

void acpi_setrsdp(acpi_xsdp_t* rsdp);
void acpi_init();
void* acpi_find_tbl(const char* tbl);
void acpi_read_madt();
