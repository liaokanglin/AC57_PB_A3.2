#ifndef DEVICE_IIC_H
#define DEVICE_IIC_H

#include "typedef.h"
#include "device/device.h"
#include "generic/ioctl.h"
#include "system/task.h"




#define IIC_IOCTL_TX_START_BIT 				_IOW('I', 0,  0)  // 发送带起始位的I2C传输命令
#define IIC_IOCTL_TX_WITH_START_BIT 		_IOW('I', 1,  1)  // 发送带起始位的I2C写命令
#define IIC_IOCTL_TX_STOP_BIT 				_IOW('I', 2,  1)  // 发送停止位的I2C写命令
#define IIC_IOCTL_TX 						_IOW('I', 3,  8)  // 发送一个字节的数据到I2C设备
#define IIC_IOCTL_TX_WITH_STOP_BIT 			_IOW('I', 4,  9)  // 发送带停止位的I2C写命令
#define IIC_IOCTL_RX 						_IOR('I', 5,  8)  // 从I2C设备读取一个字节的数据
#define IIC_IOCTL_RX_WITH_STOP_BIT 			_IOR('I', 6,  9)  // 从I2C设备读取数据并包含停止位
#define IIC_IOCTL_RX_WITH_NOACK 			_IOR('I', 7,  9)  // 从I2C设备读取数据并不发送ACK
#define IIC_IOCTL_RX_WITH_ACK 				_IOR('I', 8,  9)  // 从I2C设备读取数据并要求ACK确认
#define IIC_IOCTL_SET_NORMAT_RATE 			_IOW('I', 9,  0)  // 设置I2C设备的正常传输速率
#define IIC_IOCTL_START 					_IOW('I', 10,  0)  // 启动I2C通信
#define IIC_IOCTL_STOP 						_IOW('I', 11,  0)  // 停止I2C通信



struct iic_device;



enum iic_device_type {
    IIC_TYPE_HW, 			//hardware iic
    IIC_TYPE_SW, 			//software iic
};

struct software_iic {
    u8 clk_pin;
    u8 dat_pin;
    u32 sw_iic_delay;
};


struct iic_platform_data {
    enum iic_device_type type;
    u32 data[0];
};

struct sw_iic_platform_data {
    struct iic_platform_data head;
    struct  software_iic iic;
};



#define SW_IIC_PLATFORM_DATA_BEGIN(data) \
	static const struct sw_iic_platform_data data = { \
		.head = { \
			.type = IIC_TYPE_SW, \
		}, \
		.iic = {



#define SW_IIC_PLATFORM_DATA_END() \
		}, \
	};





struct iic_operations {
    enum iic_device_type type;
    int (*open)(struct iic_device *);
    int (*read)(struct iic_device *, void *buf, int len);
    int (*write)(struct iic_device *, void *buf, int len);
    int (*ioctl)(struct iic_device *, int cmd, int arg);
};


struct iic_device {
    u8 id;
    u8 type;
    u8 nrate;
    u8 open_status;
    struct list_head entry;
    const void *hw;
    struct device dev;
    const struct iic_operations *ops;
    OS_MUTEX mutex;
    struct device *curr_device;
};


#define REGISTER_IIC_DEVICE(name) \
	static const struct iic_operations name sec(.iic)

extern const struct iic_operations iic_device_begin[], iic_device_end[];

#define list_for_each_iic_device_ops(p) \
	for (p=iic_device_begin; p<iic_device_end; p++)



extern const struct device_operations iic_dev_ops;



#endif

