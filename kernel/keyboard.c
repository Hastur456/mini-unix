#include <stdint.h>
#include "keyboard.h"
#include "interrupts.h"
#include "io.h"
#include "pic.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_VECTOR (IRQ_BASE + KEYBOARD_IRQ)
#define KEYBOARD_BUFFER_SIZE 128

static volatile unsigned char key_buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint32_t key_buffer_head = 0;
static volatile uint32_t key_buffer_tail = 0;

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

static void keyboard_buffer_push(unsigned char c) {
    uint32_t next = (key_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

    if (next == key_buffer_tail) {
        return;
    }

    key_buffer[key_buffer_head] = c;
    key_buffer_head = next;
}

static int keyboard_decode_scancode(uint8_t scancode) {
    if (scancode & 0x80) {
        return -1;
    }

    if (scancode >= sizeof(kbdus)) {
        return -1;
    }

    if (kbdus[scancode] == 0) {
        return -1;
    }

    return kbdus[scancode];
}

static void keyboard_irq_handler(struct interrupt_frame* frame) {
    (void)frame;

    if (!(kb_status() & 1)) {
        return;
    }

    uint8_t scancode = kb_scancode();
    int c = keyboard_decode_scancode(scancode);

    if (c != -1) {
        keyboard_buffer_push((unsigned char)c);
    }
}

void keyboard_init(void) {
    key_buffer_head = 0;
    key_buffer_tail = 0;

    interrupt_register_handler(KEYBOARD_VECTOR, keyboard_irq_handler);
    pic_unmask_irq(KEYBOARD_IRQ);
}

int keyboard_getchar(void) {
    if (key_buffer_head == key_buffer_tail) {
        return -1;
    }

    unsigned char c = key_buffer[key_buffer_tail];
    key_buffer_tail = (key_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;

    return c;
}