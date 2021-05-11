#include <acpi.h>
#include <stddef.h>
#include <string.h>

acpi_mcfg_hdr_t* acpi_find_facp(acpi_sdt_hdr_t* sdt)
{
    uint32_t entries = (sdt->len - sizeof(acpi_sdt_hdr_t)) / 4;

    for (uint32_t i = 0; i < entries; i++)
    {
        acpi_sdt_hdr_t* hdr = (acpi_sdt_hdr_t*)*(uint64_t*)((uint64_t)sdt + sizeof(acpi_sdt_hdr_t) + (i * 8));
        
        if (!strncmp((char*)hdr->sig, "FACP", 4))
            return (acpi_mcfg_hdr_t*)hdr;
    }

    return NULL;
}