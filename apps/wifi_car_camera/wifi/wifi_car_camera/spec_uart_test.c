#include "device/uart.h"
#include "os/os_compat.h"
/*****************************************************************************
 Description:特殊串口初始化函数
******************************************************************************/
static void *uart_dev_handle;

int spec_uart_recv(char *buf, u32 len)
{
    return dev_read(uart_dev_handle, buf, len);
}

int spec_uart_send(char *buf, u32 len)
{
    return dev_write(uart_dev_handle, buf, len);
}

void spec_uart_test_thread(void *priv)
{
    static char uart_circlebuf[1 * 1024] __attribute__((aligned(32))); //串口循环数据buf,根据需要设置大小

    int parm;

    uart_dev_handle = dev_open("uart3", 0);
    ASSERT(uart_dev_handle != NULL, "open uart err");
    dev_ioctl(uart_dev_handle, UART_SET_CIRCULAR_BUFF_ADDR, (int)uart_circlebuf);


    parm = sizeof(uart_circlebuf);

    dev_ioctl(uart_dev_handle, UART_SET_CIRCULAR_BUFF_LENTH, (int)&parm);


#if 0 // 是否设置为 接收完指定长度数据, spec_uart_recv才出来
    parm = 1;
    dev_ioctl(uart_dev_handle, UART_SET_RECV_ALL, (int)&parm);
#endif

#if 1 // 是否设置为阻塞方式读
    parm = 1;
    dev_ioctl(uart_dev_handle, UART_SET_RECV_BLOCK, (int)&parm);
#endif

    dev_ioctl(uart_dev_handle, UART_START, (int)0);


    while (1) {
#if 0
        int ret;
        char c;
        ret = spec_uart_recv(&c, sizeof(c));
        if (ret > 0) {
            if (c == 'a') {
                cpu_reset();
            }
        }
#endif

#if 1
        char buf[200];
        strcpy(buf, "spec_uart_test\n");
        spec_uart_send(buf, strlen(buf));
#endif

        os_time_dly(100);
    }
}

int spec_uart_test_main(void)
{
    return thread_fork("spec_uart_test_main", 2, 0x1000, 0, 0, spec_uart_test_thread, NULL);
}
