#include "device/uart.h"
#include "os/os_compat.h"


/*****************************************************************************
 Description:特殊串口初始化函数
******************************************************************************/
static void *uart_dev_handle;  // 串口设备句柄，用于后续操作

// 接收数据函数
// int spec_uart_recv(char *buf, u32 len)
// {
//     return dev_read(uart_dev_handle, buf, len);  // 从串口设备读取数据
// }

// 发送数据函数
int spec_uart_send(char *buf, u32 len)
{
    return dev_write(uart_dev_handle, buf, len);  // 向串口设备发送数据
}

// 串口测试线程初始化
void spec_uart_test_thread(void *priv)
{
    static char uart_circlebuf[1 * 1024] __attribute__((aligned(32))); // 串口循环数据缓冲区，根据需要设置大小

    int parm;
    // printf("uart_dev_handle start\n",uart_dev_handle);
    // 打开串口设备 "uart3"
    uart_dev_handle = dev_open("uart0", 0);
    ASSERT(uart_dev_handle != NULL, "open uart err");  // 确保串口打开成功
    dev_ioctl(uart_dev_handle, UART_SET_CIRCULAR_BUFF_ADDR, (int)uart_circlebuf);  // 设置循环缓冲区地址

    parm = sizeof(uart_circlebuf);
    dev_ioctl(uart_dev_handle, UART_SET_CIRCULAR_BUFF_LENTH, (int)&parm);  // 设置循环缓冲区长度

#if 0 // 是否设置为接收完指定长度数据，spec_uart_recv才出来
    parm = 1;
    dev_ioctl(uart_dev_handle, UART_SET_RECV_ALL, (int)&parm);  // 设置接收模式为接收所有数据
#endif

#if 1 // 是否设置为阻塞方式读
    parm = 1;
    dev_ioctl(uart_dev_handle, UART_SET_RECV_BLOCK, (int)&parm);  // 设置接收为阻塞方式
#endif

    dev_ioctl(uart_dev_handle, UART_START, (int)0);  // 启动串口

    while (1) {
#if 0
        int ret;
        char c;
        ret = spec_uart_recv(&c, sizeof(c));  // 接收一个字符
        if (ret > 0) {
            if (c == 'a') {
                cpu_reset();  // 如果接收到字符 'a'，重置 CPU
            }
        }
#endif

#if 1
        // char buf[200];  // 定义发送缓冲区
        // strcpy(buf, "spec_uart_test\n");  // 将测试信息复制到缓冲区
        // spec_uart_send(buf, strlen(buf));  // 发送测试信息
#endif

        os_time_dly(100);  // 延迟100毫秒
    }
}

/// 串口测试线程初始化
int spec_uart_test_main(void)
{
    return thread_fork("spec_uart_test_main", 2, 0x1000, 0, 0, spec_uart_test_thread, NULL);  // 创建串口测试线程
}
