#include <stdint.h>
#include "keyboard.h"

const unsigned char kbdus[128] =
{
  0,   27,  '1', '2', '3', '4', '5', '6', '7', '8',
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',                   /* Tab */
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i', 'o', 'p',
  '[', ']', '\n',        /* Enter */
  0,                     /* Ctrl */
  'a', 's', 'd', 'f', 'g',
  'h', 'j', 'k', 'l', ';', '\'',
  '`',
  0,                     /* Left shift */
  '\\', 'z', 'x', 'c', 'v', 'b',
  'n', 'm', ',', '.', '/',
  0,                     /* Right shift */
  '*',
  0,                     /* Alt */
  ' ',                   /* Space */
  0,                     /* Caps lock */

  /* F1-F10 */
  0,0,0,0,0,0,0,0,0,0,

  /* NumLock, ScrollLock */
  0,0,

  /* Keypad */
  '7','8','9','-',
  '4','5','6','+',
  '1','2','3','0','.'
};

uint8_t kb_scancode(void) {
    return inb(KB_SCANCODE_PORT);
}

uint8_t kb_status(void) {
    return inb(KB_STATUS_PORT);
}
