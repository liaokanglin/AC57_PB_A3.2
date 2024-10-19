#ifndef DEVICE_UART_H
#define DEVICE_UART_H

#include "typedef.h"
#include "device/device.h"
#include "generic/ioctl.h"
#include "system/task.h"

#define UART_DMA_SUPPORT 	0x00000001
#define UART_TX_USE_DMA 	0x00000003
#define UART_RX_USE_DMA 	0x00000005
#define UART_DEBUG 			0x00000008

struct uart_outport {
    u8  tx_pin;
    u8  rx_pin;
    u16 value;
};

extern void putbyte(char a);

enum uart_clk_src {
    LSB_CLK,
    OSC_CLK,
    PLL_48M,
};

enum __UART_PORT {
    //UART0 PORTX_TX_RX
    PORTA_7_8 = 0x10000,
    PORTG_6_7,
    PORTH_12_13,
    PORTB_14_15,

    //UART1 PORTX_TX_RX
    PORTH_2_5,
    PORTH_14_15,

    //UART3 PORTX_TX_RX
    PORTE_0_1,
    PORTB_4_3,


    PORT_REMAP = 0X20000,
};

enum __OUTPUT_CHANNEL {
    OUTPUT_CHANNEL0,  // 输出通道0
    OUTPUT_CHANNEL1,  // 输出通道1
    OUTPUT_CHANNEL2,  // 输出通道2
    OUTPUT_CHANNEL3,  // 输出通道3
};



struct uart_platform_data {
    u8 *name;

    u8  irq;
    int tx_pin;//不配置需设置-1
    int rx_pin;//不配置需设置-1
    int flags;
    u32 baudrate;

    int port;//enum _uart_port0-3的值
    int output_channel;
    int input_channel;
    u32 max_continue_recv_cnt;
    u32 idle_sys_clk_cnt;
    enum uart_clk_src clk_src;
};

enum {
    UART_CIRCULAR_BUFFER_WRITE_OVERLAY = -1,
    UART_RECV_TIMEOUT = -2,
    UART_RECV_EXIT = -3,
};

#define UART_MAGIC                          'U'
#define UART_FLUSH                          _IO(UART_MAGIC,1)
#define UART_SET_RECV_ALL                   _IOW(UART_MAGIC,2,bool)
#define UART_SET_RECV_BLOCK                 _IOW(UART_MAGIC,3,bool)
#define UART_SET_RECV_TIMEOUT               _IOW(UART_MAGIC,4,u32)
#define UART_SET_RECV_TIMEOUT_CB            _IOW(UART_MAGIC,5,int (*)(void))
#define UART_GET_RECV_CNT                   _IOR(UART_MAGIC,6,u32)
#define UART_START                          _IO(UART_MAGIC,7)
#define UART_SET_CIRCULAR_BUFF_ADDR         _IOW(UART_MAGIC,8,void *)
#define UART_SET_CIRCULAR_BUFF_LENTH        _IOW(UART_MAGIC,9,u32)



struct uart_device {
    char *name;
    const struct uart_operations *ops;
    struct device dev;
    const struct uart_platform_data *priv;
    OS_MUTEX mutex;
};




struct uart_operations {
    int (*init)(struct uart_device *);
    int (*read)(struct uart_device *, void *buf, u32 len);
    int (*write)(struct uart_device *, void *buf, u16 len);
    int (*ioctl)(struct uart_device *, u32 cmd, u32 arg);
    int (*close)(struct uart_device *);
};



#define REGISTER_UART_DEVICE(dev) \
    static struct uart_device dev sec(.uart)

extern struct uart_device uart_device_begin[], uart_device_end[];

#define list_for_each_uart_device(p) \
    for (p=uart_device_begin; p<uart_device_end; p++)



extern const struct device_operations uart_dev_ops;



#endif
