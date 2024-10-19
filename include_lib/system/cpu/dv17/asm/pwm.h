#ifndef _MOTO_PWM_H
#define _MOTO_PWM_H

#include "includes.h"
#include "device/device.h"

#define PWM_MAX_NUM		8	//0-7，最多,8个通道


/***PWMCHx***/

#define PWMCH0  		0
#define PWMCH1  		1
#define PWMCH2  		2
#define PWMCH3  		3
#define PWMCH4  		4
#define PWMCH5  		5
#define PWMCH6  		6
#define PWMCH7  		7

/***ioctrl cmd***/
#define PWM_STOP                _IOW('P',0,u32)  // 停止 PWM 信号输出
#define PWM_RUN                 _IOW('P',1,u32)  // 启动 PWM 信号输出
#define PWM_FORDIRC             _IOW('P',2,u32)  // 设置 PWM 正向输出
#define PWM_REVDIRC             _IOW('P',3,u32)  // 设置 PWM 反向输出
#define PWM_SET_DUTY            _IOW('P',4,u32)  // 设置 PWM 信号的占空比
#define PWM_SLE_TIMER           _IOW('P',5,u32)  // 设置 PWM 睡眠定时器
#define PWM_SET_FREQ            _IOW('P',6,u32)  // 设置 PWM 信号的频率
#define PWM_SET_DEATH_TIME      _IOW('P',7,u32)  // 设置 PWM 信号的死区时间



#define PWM_GET_DUTY		    _IOR('P',0,u32)


#define PWM_PORTG  'A'
#define PWM_PORTH  'B'


struct pwm_reg {
    volatile u32 con;
    volatile u32 cnt;
    volatile u32 prd;
    volatile u32 cmp;
};

struct pwm_platform_data {
    struct pwm_reg *reg;
    u8  port;
    u8  output_io;
    u8  pwm_ch;
    u8  duty;
    u64 freq;
    u32 dtime_en;
    float	dtime_us;
    u8 outputchannel;
};

struct pwm_operations {
    int (*open)(struct pwm_platform_data *pwm_data);
    int (*close)(struct pwm_platform_data *pwm_data);
    /* int (*write)(struct pwm_platform_data *pwm_data, u32 duty, u32 channel); */
    /* int (*read)(struct pwm_platform_data *pwm_data, u32 channel); */
    int (*ioctl)(struct pwm_platform_data *pwm_data, u32 cmd, u32 arg);
};

struct pwm_device {
    char *name;
    struct pwm_operations *ops;
    struct device dev;
    void *priv;
};

#define PWM_PLATFORM_DATA_BEGIN(data) \
		static struct pwm_platform_data data={

#define PWM_PLATFORM_DATA_END() \
		.reg=NULL, \
	};

#define REGISTER_PWM_DEVICE(dev) \
	struct pwm_device dev sec(.pwm_dev)

extern struct pwm_device pwm_device_begin[];
extern struct pwm_device pwm_device_end[];

#define list_for_each_pwm_device(dev) \
	for(dev=pwm_device_begin;dev<=pwm_device_end;dev++)

extern const struct device_operations pwm_dev_ops;

#endif





