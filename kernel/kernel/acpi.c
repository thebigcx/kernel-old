#include <acpi.h>
#include <stddef.h>
#include <string.h>
#include <console.h>
#include <apic.h>
#include <paging/paging.h>

acpi_rsdt_t* rsdt;
acpi_xsdt_t* xsdt;
acpi_rsdp_t* desc;

void acpi_init(acpi_rsdp_t* rsdp)
{
    desc = rsdp;
    xsdt = rsdp->xsdt_addr;
    rsdt = rsdp->rsdt_addr;

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
        console_write("[ACPI] Could not find MADT", 255, 0, 0);
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
            case ACPI_MADT_TYPE_APICIO:
            {
                apicio_t* apicio = (apicio_t*)ent;

                if (!apicio->gsib)
                    apicio_set_base(apicio->addr);
            }
            break;
        }

        madt_ent += ent->len;
    }
}