#include <stddef.h>
#include <stdint.h>
#include <arch/x86/gdt.h>

extern unsigned char _binary_build_user_shell_bin_start[];
extern unsigned char _binary_build_user_shell_bin_end[];

#define USER_SHELL_ADDR  0x00200000
#define USER_SHELL_ENTRY USER_SHELL_ADDR

#define USER_STACK_ADDR  0x00300000
#define USER_ENTRY 0x00200000
 
#define USER_CS GDT_UCODE_SELECTOR
#define USER_DS GDT_UDATA_SELECTOR
#define USER_SS GDT_USTACK_SELECTOR

__attribute__((noreturn))
void enter_user_mode(uint32_t entry)
{
    __asm__ volatile (
        /*
         * Load user data segments.
         */
        "movw %[ds], %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"

        /*
         * Build iret frame:
         *
         *     SS
         *     ESP
         *     EFLAGS
         *     CS
         *     EIP
         */
        "pushl %[ss]\n"
        "pushl %[esp]\n"

        "pushfl\n"
        "orl $0x200, (%%esp)\n"

        "pushl %[cs]\n"
        "pushl %[eip]\n"

        /*
         * Ring 0 -> Ring 3.
         */
        "iret\n"

        :
        : [ds]  "i"((uint16_t)USER_DS),
          [ss]  "i"((uint32_t)USER_SS),
          [esp] "i"((uint32_t)USER_STACK_ADDR),
          [cs]  "i"((uint32_t)USER_CS),
          [eip] "r"(entry)
        : "eax", "memory"
    );

    __builtin_unreachable();
}


void user_init(void)
{
    size_t size =
        (size_t)(
            _binary_build_user_shell_bin_end -
            _binary_build_user_shell_bin_start
        );

    unsigned char *dst =
        (unsigned char *)USER_ENTRY;

    const unsigned char *src =
        _binary_build_user_shell_bin_start;

    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
}
