// TODO: rewrite limine
#include <util/types.h>
#include <arch/x86_64/stivale2.h>
#include <util/printf.h>

uint8_t stack[4096];

static st2_tagterm_t termtag =
{
    .tag =
    {
        .id = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },

    .flags = 0
};

static st2_tagfb_t fbtag =
{
    .tag =
    {
        .id = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&termtag
    },

    .width = 0,
    .height = 0,
    .depth = 32
};

__attribute__((section(".stivale2hdr"), used))
static st2_header_t stivale_hdr =
{
    .entry_point = 0,
    .stack = (uint64_t)stack + sizeof(stack),
    .flags = 0, // We don't need Limine to supply higher-half pointers, as we map the kernel ourselves
    .tags = (uint64_t)&fbtag
};

void _start(struct stivale2_struct* st2)
{
    init_stivale2(st2);
}