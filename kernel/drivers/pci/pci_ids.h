#pragma once

#define PCI_CLASS_STORAGE       0x01
#define PCI_CLASS_NET           0x02
#define PCI_CLASS_DISPLAY       0x03
#define PCI_CLASS_MULTIMEDIA    0x04
#define PCI_CLASS_MEMORY        0x05
#define PCI_CLASS_BRIDGE        0x06
#define PCI_CLASS_COM           0x07
#define PCI_CLASS_BASESYS       0x08
#define PCI_CLASS_INPUTDEV      0x09
#define PCI_CLASS_DOCK          0x0a
#define PCI_CLASS_PROCESSOR     0x0b
#define PCI_CLASS_SERIALBUS     0x0c
#define PCI_CLASS_WIRELESS      0x0d
#define PCI_CLASS_INTELLIG      0x0e
#define PCI_CLASS_SATTELITE     0x0f
#define PCI_CLASS_CRYPT         0x10
#define PCI_CLASS_SIGPROC       0x11
#define PCI_CLASS_PROCACCEL     0x12
#define PCI_CLASS_INSTRUM       0x13
#define PCI_CLASS_COPROC        0x40

// Subclasses
#define PCI_SC_SCSI             0x00
#define PCI_SC_IDE              0x01
#define PCI_SC_FLOPPY           0x02
#define PCI_SC_IPIBUS           0x03
#define PCI_SC_RAID             0x04
#define PCI_SC_ATA              0x05
#define PCI_SC_SATA             0x06
#define PCI_SC_SERIAL_SCSI      0x07
#define PCI_SC_MEMCONT          0x08

#define PCI_PI_AHCI             0x01