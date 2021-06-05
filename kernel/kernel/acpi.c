#include <acpi.h>
#include <stddef.h>
#include <string.h>

void* acpi_find_tbl(acpi_sdt_hdr_t* sdt, const char* tbl)
{
    uint32_t entries = (sdt->len - sizeof(acpi_sdt_hdr_t)) / 4;

    for (uint32_t i = 0; i < entries; i++)
    {
        acpi_sdt_hdr_t* hdr = (acpi_sdt_hdr_t*)*(uint64_t*)((uint64_t)sdt + sizeof(acpi_sdt_hdr_t) + (i * 8));
        
        if (!strncmp((char*)hdr->sig, tbl, 4))
            return hdr;
    }

    return NULL;
}