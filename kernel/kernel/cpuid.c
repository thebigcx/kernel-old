#include <cpuid.h>

cpuid_inf_t cpuid()
{
    cpuid_inf_t inf;
    uint32_t ebx, ecx, edx;

    asm volatile ("cpuid" : "=b"(ebx), "=d"(edx), "=c"(ecx) : "a"(0));

    for (int i = 0; i < 4; i++) inf.vendor_str[i]     = ebx >> (i * 8) & 0xff;
    for (int i = 0; i < 4; i++) inf.vendor_str[i + 4] = edx >> (i * 8) & 0xff;
    for (int i = 0; i < 4; i++) inf.vendor_str[i + 8] = ecx >> (i * 8) & 0xff;

    asm volatile ("cpuid" : "=d"(edx), "=c"(ecx) : "a"(1));
    inf.ecx_feats = ecx;
    inf.edx_feats = edx;

    return inf;
}