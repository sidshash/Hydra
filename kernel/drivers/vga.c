#include <stdint.h>
#include <arch/port.h>

#define VGA_CTRL_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5
#define VGA_WIDTH     80

uint16_t read_cursor(void){
    //write 0x0F to read first byte 
    port_outb(VGA_CTRL_PORT, 0x0F);
    uint8_t high = port_inb(VGA_DATA_PORT);

    port_outb(VGA_CTRL_PORT, 0x0e);
    uint8_t low = port_inb(VGA_DATA_PORT);

    return (uint16_t) (high << 8) | low;
} 
