#include <acpi/acpi.h>
#include <util/stdlib.h>
#include <sys/console.h>
#include <intr/apic.h>
#include <mem/paging.h>
#include <drivers/tty/serial.h>

acpi_xsdp_t* desc;
acpi_rsdt_t* rsdt;
acpi_xsdt_t* xsdt;
acpi_xsdp_t* rsdp;

iso_lst_t acpi_isos;

uint32_t acpi_cpus[64];
uint32_t acpi_cpu_cnt = 0;

void acpi_setrsdp(acpi_xsdp_t* rsdp_)
{
    rsdp = rsdp_;
}

void acpi_init()
{
    desc = rsdp;

    if (desc->rev == 0)
    {
        rsdt = page_map_mmio(desc->rsdt_addr);
    }
    else
    {
        xsdt = page_map_mmio(desc->xsdt_addr);
        rsdt = page_map_mmio(desc->xsdt_addr);
    }

    acpi_isos.cnt = 0;

    acpi_read_madt();
}

void* acpi_find_tbl(const char* tbl)
{
    uint32_t entries = 0;

    if (desc->rev == 0)
    {
        entries = (rsdt->hdr.len - sizeof(acpi_sdt_hdr_t)) / sizeof(uint32_t);
    }
    else
    {
        entries = (xsdt->hdr.len - sizeof(acpi_sdt_hdr_t)) / sizeof(uint64_t);
    }
    char buf[100];

    for (uint32_t i = 0; i < entries; i++)
    {
        acpi_sdt_hdr_t* hdr = page_map_mmio(rsdt->sdts[i]);

        if (!strncmp((char*)hdr->sig, tbl, 4))
            return hdr;
    }

    return NULL;
}

void acpi_read_madt()
{
    void* madt = acpi_find_tbl("APIC");

    if (!madt)
    {
        serial_writestr("[ACPI] Could not find MADT\n" ANSI_WHITE);
        return;
    }

    acpi_madt_t* madt_hdr = (acpi_madt_t*)madt;
    uint64_t madt_end = (uint64_t)madt + madt_hdr->hdr.len;
    uint64_t madt_ent = (uint64_t)madt + sizeof(acpi_madt_t);

    while (madt_ent < madt_end)
    {
        acpi_madt_ent_t* ent = (acpi_madt_ent_t*)madt_ent;

        switch (ent->type)
        {
            case ACPI_MADT_TYPE_APICLOC:
            {
                lapic_t* lapic = (lapic_t*)ent;

                if (lapic->flags & (ACPI_APICLOC_ENABLE | ACPI_APICLOC_ONLINE_CAPABLE))
                {
                    if (lapic->apic_id == 0) break; // BSP

                    acpi_cpus[acpi_cpu_cnt++] = lapic->apic_id;
                }
            }
            break;

            case ACPI_MADT_TYPE_APICIO:
            {
                ioapic_t* ioapic = (ioapic_t*)ent;

                if (!ioapic->gsib)
                    ioapic_set_base(ioapic->addr);
            }
            break;

            case ACPI_MADT_TYPE_APICIO_ISO:
            {
                apic_iso_t* iso = (apic_iso_t*)ent;
                acpi_isos.data[acpi_isos.cnt++] = iso;
            }
            break;

            case ACPI_MADT_TYPE_APICLOC_NONMASK:
            {

            }
            break;

            case ACPI_MADT_TYPE_APICLOC_ADDR:
            {

            }
            break;

            default:
                serial_printf("[ACPI] Invalid MADT entry %d\n", 255, 0, 0, ent->type);
                break;
        }

        madt_ent += ent->len;
    }
}