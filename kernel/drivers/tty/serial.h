#pragma once

#include <util/types.h>

#define PORT_COM1 0x3f8
#define PORT_COM2 0x2f8

// Registers (add to PORT_COM1)
#define SERIAL_DATA         (PORT_COM1 + 0)
#define SERIAL_INTR         (PORT_COM1 + 1)
#define SERIAL_DIVLO        (PORT_COM1 + 0)
#define SERIAL_DIVHI        (PORT_COM1 + 1)
#define SERIAL_FIFO         (PORT_COM1 + 2)
#define SERIAL_LINECTRL     (PORT_COM1 + 3)
#define SERIAL_MODEMCTRL    (PORT_COM1 + 4)
#define SERIAL_LINESTAT     (PORT_COM1 + 5)
#define SERIAL_MODEMSTAT    (PORT_COM1 + 6)
#define SERIAL_SCRATCH      (PORT_COM1 + 7)

#define SERIAL_SETBAUD      0x80

// Character length
#define SERIAL_CHARLEN5     0x0
#define SERIAL_CHARLEN6     0x1
#define SERIAL_CHARLEN7     0x2
#define SERIAL_CHARLEN8     0x3

// Extra stop bits
#define SERIAL_EXT_STOPBITS (1 << 2)

// Parity
#define SERIAL_PARITY_ODD   (1 << 3)
#define SERIAL_PARITY_EVEN  (2 << 3)
#define SERIAL_PARITY_MARK  (5 << 3)
#define SERIAL_PARITY_SPACE (6 << 3)

void serial_init();

bool serial_received();
char serial_read();

bool serial_transmit_empty();
void serial_write(char c);