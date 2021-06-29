#include <acpi/acpi.h>
#include <util/stdlib.h>
#include <sys/console.h>
#include <intr/apic.h>
#include <mem/paging.h>

acpi_rsdt_t* rsdt;
acpi_xsdt_t* xsdt;
acpi_rsdp_t* desc;

iso_lst_t acpi_isos;

uint32_t acpi_cpus[64];
uint32_t acpi_cpu_cnt = 0;

void acpi_init(acpi_rsdp_t* rsdp)
{
    desc = rsdp;
    xsdt = rsdp->xsdt_addr;
    rsdt = rsdp->rsdt_addr;

    acpi_isos.cnt = 0;

    acpi_read_madt();
}

void* acpi_find_tbl(const char* tbl)
{
    uint32_t entries = (xsdt->hdr.len - sizeof(acpi_sdt_hdr_t)) / sizeof(uint64_t);

    for (uint32_t i = 0; i < entries; i++)
    {
        acpi_sdt_hdr_t* hdr = (acpi_sdt_hdr_t*)xsdt->sdts[i];

        page_kernel_map_memory(hdr, hdr);
        
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
        console_write(ANSI_YELLOW "ACPI: " ANSI_RED "Could not find MADT\n" ANSI_WHITE, 255, 0, 0);
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
                console_printf("[ACPI] Invalid MADT entry %d\n", 255, 0, 0, ent->type);
                break;
        }

        madt_ent += ent->len;
    }
}