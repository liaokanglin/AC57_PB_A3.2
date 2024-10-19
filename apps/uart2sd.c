#include "os/os_api.h"
#include "asm/uart.h"
#include "generic/gpio.h"
#include "spinlock.h"
#include "init.h"
#include "wait.h"
#include "storage_device.h"
#include "event.h"
#include "generic/circular_buf.h"
#include "fs/fs.h"
#include "timer.h"
#include "app_config.h"

/* #define UART_DEBUG_SDCARD_EN */

#ifdef UART_DEBUG_SDCARD_EN

#define UART_CBUF_SIZE    (16*1024)
#define UART_TIMEOUT      (250)

enum {
    STA_UART2SD_INIT = 0,
    STA_UART2SD_OPEN,
    STA_UART2SD_START,
    STA_UART2SD_STOP,

};
struct uart_debug_sd_t {
    u8 state;
    u8 *buf;
    u8 *wbuf;
    cbuffer_t cbuf;
    FILE *file;
    int sd_wait;
};
static struct uart_debug_sd_t u_sd_t = {0};
#define __this 	(&u_sd_t)

int uart_2_sd_start(void);
int uart_2_sd_stop(void);

static void uart_2_sd_task(void *p)
{
    u32 rlen = 0;
    u32 time = jiffies + msecs_to_jiffies(UART_TIMEOUT);
    __this->wbuf = malloc(UART_CBUF_SIZE);
    u8 fname[64];
    if (!__this->file) {
        __this->file = fopen(CONFIG_ROOT_PATH"uart***.txt", "w+");
    }
    sys_timeout_add(NULL, uart_2_sd_stop, 16000); //时间到停止写卡
    while (1) {

        if (__this->state != STA_UART2SD_START) {
            os_time_dly(1);
            continue;
        }

        if (time_after(jiffies, time)) {
            time = jiffies + msecs_to_jiffies(UART_TIMEOUT);
            cbuf_read_alloc(&__this->cbuf, &rlen);
            if (rlen) {
                //写卡
                cbuf_read(&__this->cbuf, __this->wbuf, rlen);
                if (__this->file) {
                    /* log_hexdump("data",16,__this->wbuf,rlen); */
                    fwrite(__this->file, __this->wbuf, rlen);
                }
            }
        }

    }
}
static int uart_2_sd_task_create(void *p)
{
    __this->sd_wait = 0;

    os_task_create(uart_2_sd_task, NULL, 20, 1024, 0, "uart2sd_task");
    return 0;
}

int uart_2_sd_stop(void)
{
    if (__this->state == STA_UART2SD_START) {
        __this->state = STA_UART2SD_STOP;
        if (__this->file) {
            fclose(__this->file);
            __this->file = NULL;
        }
        if (__this->wbuf) {
            free(__this->wbuf);
            __this->wbuf = NULL;
        }
        cbuf_clear(&__this->cbuf);
        task_kill("uart2sd_task");
    }

    return 0;
}
int uart_2_sd_start(void)
{
    if (__this->state == STA_UART2SD_START) {
        return 0;
    }
    __this->sd_wait = wait_completion(storage_device_ready, uart_2_sd_task_create, NULL);
    __this->state = STA_UART2SD_START;

    return 0;
}

void uart_output(char a)
{
    u32 wlen = 0;
    char *wptr;

    if (__this->state == STA_UART2SD_INIT) {
        __this->state = STA_UART2SD_OPEN;
        __this->buf = malloc(UART_CBUF_SIZE);
        cbuf_init(&__this->cbuf, __this->buf, UART_CBUF_SIZE);
    }

    wptr = cbuf_write_alloc(&__this->cbuf, &wlen);
    if (wlen) {
        *wptr = a;
        cbuf_write_updata(&__this->cbuf, 1);
    }
}

static void uart_2_sd_event_handler(struct sys_event *event)
{
    if (!strcmp(event->arg, "sd0") || !strcmp(event->arg, "sd1") || !strcmp(event->arg, "sd2")) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:
        case DEVICE_EVENT_ONLINE:
            log_d(">>> uart 2 sd event online\n");
            uart_2_sd_start();
            break;
        case DEVICE_EVENT_OUT:
            log_d(">>> uart 2 sd event offline\n");
            uart_2_sd_stop();
            break;
        default:
            break;
        }
    }
}
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, uart_2_sd_event_handler, 0);

#endif
