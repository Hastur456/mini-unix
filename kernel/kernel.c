#include <stdint.h>

volatile char* video = (volatile char*)0xb8000;

void print(const char* str) {
    int i = 0;

    while(str[i]) {
        video[i * 2] = str[i];
        video[i * 2 + 1] = 0x07;
        i++;
    }
}