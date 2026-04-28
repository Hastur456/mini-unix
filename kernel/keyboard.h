#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KB_SCANCODE_PORT 0x60
#define KB_STATUS_PORT 0x64

uint8_t inb(uint16_t port);
uint8_t kb_scancode(void);
uint8_t kb_status(void);

extern const unsigned char kbdus[128];

#endif
