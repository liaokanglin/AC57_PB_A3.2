#include "asm/clock.h"
#include "os/os_api.h"
#include "os/os_compat.h"
#include "asm/uart.h"
#include "generic/gpio.h"
#include "spinlock.h"
#include "init.h"
#include "asm/cpu.h"
#include "device/uart.h"
#include "asm/hwi.h"
#include "asm/device_drive.h"
#include "wf_uart_protocal.h"

static u8 cbuf[1 * 128] __attribute__((aligned(32))); //用于串口接收缓存数据的循环cbuf
static void *g_hdl = NULL;
static uart_protocal_frame g_pf;
static u8 uart_dev_init_ok = 0;
static OS_MUTEX g_mutex;

static void uart_dev_init(void *priv)
{
    u8 recv_buf[64];
    int send_buf[64];	//DMA发送需要4字节对齐
    int len;
    int cnt = 0;

    os_mutex_create(&g_mutex);
    g_hdl = dev_open("uart1", NULL);
    if (!g_hdl) {
        printf("open uart err !!!\n");
        return;
    }

    dev_ioctl(g_hdl, UART_SET_CIRCULAR_BUFF_ADDR, (int)cbuf);
    dev_ioctl(g_hdl, UART_SET_CIRCULAR_BUFF_LENTH, sizeof(cbuf));
    dev_ioctl(g_hdl, UART_SET_RECV_BLOCK, 1);

    u32 parm = 1000;
    dev_ioctl(g_hdl, UART_SET_RECV_TIMEOUT, (u32)parm); //超时设置

    dev_ioctl(g_hdl, UART_START, 0);

    uart_dev_init_ok = 1;
    printf("uart_dev_init OK!\n");
}


static uart_err_t uart_action(uart_protocal_frame *pf)
{
    uart_err_t err = UART_ERR_OK;

    switch (pf->data.optcode) {
    case WIFI_SET_CHANNEL:
        printf("wifi channel set : %d\n", pf->data.data[0]);
        break;

    case PARM_SET_RSP:
        if (pf->data.data[0] == PARM_SET_OK) {
            puts("PARM_SET_OK\n");
        } else {
            puts("UART_PARAM_SET_ERR\n");
            err = UART_PARAM_SET_ERR;
        }

        break;

    default:
        puts("UART_OPTCODE_NOT_DEFAULT\n");
        err =  UART_OPTCODE_NOT_DEFAULT;
        break;
    }

    return err;
}

static uart_err_t uart_recv_parse(u8 *recv_data, u32 len)
{
    uart_protocal_frame *pt;
    uart_err_t err = UART_ERR_OK;

    if (len < PROTOCAL_FRAME_SIZE) {
        puts("len < PROTOCAL_FRAME_SIZE");
        return UART_FRAME_ERR;
    }

    pt = (uart_protocal_frame *)(recv_data);
    if (pt->data.mark0 == SYNC_MARK0 && pt->data.mark1 == SYNC_MARK1) {
        u16 c_crc = CRC16(pt, sizeof(uart_protocal_frame) - 2);
        if (c_crc == pt->data.crc) {
            err = uart_action(pt);
        } else {
            puts("UART_FRAME_CRC_ERR\n");
            err = UART_FRAME_CRC_ERR;
        }
    } else {
        puts("UART_FRAME_ERR\n");
        err = UART_FRAME_ERR;
    }

    return err;
}

uart_err_t uart_dev_send_cmd(u8 cmd, u8 *data, u32 data_len)
{
    u8 recv_buf[64];
    int len;
    int err;

    if (!uart_dev_init_ok) {
        puts("uart_dev not init!");
        return UART_DEV_NOT_INIT;
    }

    if (data_len > 4 || data == NULL) {
        puts("uart_dev_send_cmd err!");
        return UART_PARAM_INVAILD;
    }

    os_mutex_pend(&g_mutex, 0);
    memset(&g_pf, 0, sizeof(uart_protocal_frame));
    g_pf.data.mark0 = SYNC_MARK0;
    g_pf.data.mark1 = SYNC_MARK1;
    g_pf.data.length = data_len;
    g_pf.data.optcode = cmd;
    memcpy(g_pf.data.data, data, data_len);
    g_pf.data.crc = CRC16(&g_pf, sizeof(uart_protocal_frame) - 2);
    dev_write(g_hdl, &g_pf, sizeof(uart_protocal_frame));

    len = dev_read(g_hdl, recv_buf, 64);
    if (len <= 0) {
        if (len == UART_CIRCULAR_BUFFER_WRITE_OVERLAY) {
            puts("\n UART_CIRCULAR_BUFFER_WRITE_OVERLAY err\n");
            dev_ioctl(g_hdl, UART_FLUSH, 0);
        } else if (len == UART_RECV_TIMEOUT) {
            puts("UART_RECV_TIMEOUT...\r\n");
        }

        os_mutex_post(&g_mutex);
        return -1;
    }

    err = uart_recv_parse(recv_buf, len);
    os_mutex_post(&g_mutex);

    return err;
}

int uart_set_wifi_channel(u8 channel)
{
    printf("switch channel : %d \n", channel);
    uart_dev_send_cmd(WIFI_SET_CHANNEL, &channel, 1);
}

static void uart_main(void)
{
    if (thread_fork("uart_dev_init", 10, 512, 0, NULL, uart_dev_init, NULL) != OS_NO_ERR) {
        printf("thread fork fail\n");
    }
}

#ifdef CONFIG_WL82_ENABLE
late_initcall(uart_main);
#endif


