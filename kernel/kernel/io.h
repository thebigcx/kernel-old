#pragma once

#include <types.h>

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

void outsb(uint16_t port, const void* addr, uint64_t count);
void outsw(uint16_t port, const void* addr, uint64_t count);
void outsl(uint16_t port, const void* addr, uint64_t count);

void insb(uint16_t port, void* addr, uint64_t count);
void insw(uint16_t port, void* addr, uint64_t count);
void insl(uint16_t port, void* addr, uint64_t count);

void iowait();