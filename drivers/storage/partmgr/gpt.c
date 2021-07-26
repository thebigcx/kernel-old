#include <drivers/storage/partmgr/gpt.h>
#include <util/stdlib.h>

bool unicode_strncmp(uint16_t* s1, uint16_t* s2, uint32_t n)
{
    uint32_t i = 0;
    while (i < n)
    {
        if (*s1 != *s2) return true;

        s1++; s2++;
    }

    return false;
}

void printf_unicode(uint16_t* str)
{
    while (*str != 0)
    {
        console_putchar(*str, 255, 255, 255);
        str++;
    }
}

vfs_node_t* gpt_getpart(vfs_node_t* disk, uint16_t* name)
{
    gpt_hdr_t* hdr = kmalloc(512);
    disk->read(disk, hdr, 1, 1);

    gpt_part_ent_t* ents = kmalloc(512);
    disk->read(disk, ents, hdr->entlba, 1);
    
    for (uint32_t i = 0; i < hdr->nrentries; i++)
    {
        if (unicode_strncmp(name, ents[i].name, 36))
        {
            gpt_partdev_t* part = kmalloc(sizeof(gpt_partdev_t));
            part->disk = disk;
            part->startlba = ents[i].lba_start;
            part->endlba = ents[i].lba_end;
            strcpy(part->name, name);

            vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
            node->device = part;
            node->read = gpt_partread;
            node->write = gpt_partwrite;
            node->flags = FS_BLKDEV;

            kfree(hdr);
            kfree(ents);
            return node;
        }
    }

    kfree(hdr);
    kfree(ents);
}

size_t gpt_partread(vfs_node_t* node, void* ptr, size_t off, size_t size)
{
    gpt_partdev_t* part = (gpt_partdev_t*)node->device;
    console_printf("LBA: %d\n", 255, 255, 255, off + part->startlba);

    return part->disk->read(part->disk, ptr, off + part->startlba, size);
}

size_t gpt_partwrite(vfs_node_t* node, const void* ptr, size_t off, size_t size)
{
    gpt_partdev_t* part = (gpt_partdev_t*)node->device;

    return part->disk->write(part->disk, ptr, off + part->startlba, size);
}