#pragma once
#include <stdint.h>

extern uint8_t port_inb(uint16_t port);

extern void port_outb(uint16_t port, uint8_t value);
