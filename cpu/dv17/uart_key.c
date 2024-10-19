#include "key_driver.h"
#include "system/event.h"
#include "asm/uart.h"

extern int getbyte(char *c);

static int uart_key_init(struct key_driver *_key, void *arg)
{
    return 0;
}

static u8 uartkey_table[] = {
    'i', 'e', 'd', 'd', 'd', 'd', 'u', 'u', 'u',
    'e', 'e', 'm', 'm',
};

extern void imd_sw_mode_move(char c);
extern void imb_sw_mode_move(char c);
static u16 uart_get_key_value(struct key_driver *_key)
{
    char c;
    u8 key_value;
    static int i = 0;

#if 0//Auto Test
    i++;
    if (i / (30 * 5) == sizeof(uartkey_table) / sizeof(uartkey_table[0])) {
        i = 0;
    }
    if (i % (30 * 5) == 0) {
        printf("key : %c\n", uartkey_table[i / (30 * 5)]);
        c = uartkey_table[i / (30 * 5)];
    } else {
        return NO_KEY;
    }
#else
    if (getbyte(&c) == 0) {
        return NO_KEY;
    }
    putchar(c);
#endif

    switch (c) {
    case 'm':
        key_value = KEY_MODE;
        break;
    case 'u':
        key_value = KEY_UP;
        break;
    case 'd':
        key_value = KEY_DOWN;
        break;
    case 'o':
        key_value = KEY_OK;
        break;
    case 'e':
        key_value = KEY_MENU;
        break;
    default:
#if 0
        imd_sw_mode_move(c);
        imb_sw_mode_move(c);
#endif
        key_value = NO_KEY;
        break;
    }

    return key_value;
}

struct key_driver_ops uart_key_ops = {
    .init = uart_key_init,
    .get_value = uart_get_key_value,
};

REGISTER_KEY_DRIVER(uart_key) = {
    .name = "uart_key",
    .prev_value = 0,
    .last_key = NO_KEY,
    .filter_cnt = 0,
    .base_cnt   = 0,
    .long_cnt   = 75,
    .hold_cnt   = (75 + 15),
    .scan_time  = 40,
    .ops        = &uart_key_ops,
};


