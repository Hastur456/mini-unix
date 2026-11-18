#include <arch/x86/tss.h>
#include <arch/x86/gdt.h>
#include <lib/string.h>

static tss_t tss;

static void tss_flush(void)
{
    __asm__ volatile (
        "movw $0x38, %%ax\n"
        "ltr %%ax\n"
        :
        :
        : "ax"
    );
}

void tss_init(uint32_t kernel_stack)
{
    memset(&tss, 0, sizeof(tss));

    /*
     * Stack used when CPU enters ring 0 from ring 3.
     */
    tss.ss0 = GDT_KDATA_SELECTOR;
    tss.esp0 = kernel_stack;

    /*
     * These fields are not used for an iret-based
     * ring 3 entry, but initialize them consistently.
     */
    tss.cs = GDT_KCODE_SELECTOR | 0x03;
    tss.ss = GDT_UDATA_SELECTOR | 0x03;
    tss.ds = GDT_UDATA_SELECTOR | 0x03;
    tss.es = GDT_UDATA_SELECTOR | 0x03;
    tss.fs = GDT_UDATA_SELECTOR | 0x03;
    tss.gs = GDT_UDATA_SELECTOR | 0x03;

    /*
     * Disable the I/O permission bitmap.
     */
    tss.iomap_base = sizeof(tss);

    /*
     * GDT entry 7 = selector 0x38.
     *
     * TSS descriptor:
     *   base   = &tss
     *   limit  = sizeof(tss) - 1
     *   access = 0x89 (present, ring 0, available 32-bit TSS)
     *   flags  = 0
     */
    gdt_set_gate(
        GDT_TSS_SEGMENT,
        (uint32_t)&tss,
        sizeof(tss) - 1,
        0x89,
        0x0
    );

    tss_flush();
}

void tss_set_kernel_stack(uint32_t kernel_stack)
{
    tss.esp0 = kernel_stack;
}
