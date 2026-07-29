#include <lib/print.h>
#include <lib/stdarg.h>


static void print_unsigned(unsigned int value, unsigned int base)
{
    char buffer[32];
    int i = 0;

    do {
        unsigned digit = value % base;

        buffer[i++] =
            (digit < 10) ? ('0' + digit)
                         : ('a' + digit - 10);

        value /= base;
    } while (value);

    while (i--)
        tty_putchar(buffer[i]);
}

static void print_signed(int value)
{
    if (value < 0) {
        tty_putchar('-');
        print_unsigned((unsigned int)(-(value + 1)) + 1, 10);
    } else {
        print_unsigned((unsigned int)value, 10);
    }
}

void kprint(const char *str) {
    if (str) {
        tty_write(str);
    }
}


void vkprintf(const char *fmt, va_list args) {
    while (*fmt) {
        if (*fmt != '%') {
            tty_putchar(*fmt);
            fmt++;
            continue;
        }   

        fmt++;

        switch (*fmt) {
            case '%':
                tty_putchar('%');
                break;

            case 'c':
                tty_putchar((char)va_arg(args, int));
                break;

            case 's': {
                const char *str = va_arg(args, const char *);

                if (!str)
                    str = "(null)";

                tty_write(str);
                break;
            }

            case 'd':
                print_signed(va_arg(args, int));
                break;

            case 'u':
                print_unsigned(va_arg(args, unsigned int), 10);
                break;

            case 'x':
                print_unsigned(va_arg(args, unsigned int), 16);
                break;

            default:
                tty_putchar('%');
                tty_putchar(*fmt);
                break;
        }

        fmt++;
    }
}


void kprintf(const char *fmt, ...) {
    if (*fmt == '\0') return;

    va_list args;

    va_start(args, fmt);
    vkprintf(fmt, args);
    va_end(args);
}
