#include <drivers/gfx/intelhd/intelhd.h>

void intelhd_init(list_t* pci_devs)
{
    list_foreach(pci_devs, item)
    {
        pci_dev_t* dev = (pci_dev_t*)item->val;

        //if (dev->vendor_id == 0x1234 && dev->dev_id == 0x1111)
    }
}