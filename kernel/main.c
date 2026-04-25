void print(const char* str);

void kernel_main() {

    print("Mini Unix Kernel");

    while(1) {
        __asm__("hlt");
    }
}