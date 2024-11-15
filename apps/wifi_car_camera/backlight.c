#include "device/lcd_driver.h"
#include "system/includes.h"
#include "ui/includes.h"
#include "server/ui_server.h"
#include "video_rec.h"
#include "video_system.h"
#include "asm/pwm.h"

static u8 lcd_pro_flag = 0; /* 屏保触发标志，1：已经触发 */
static u16 lcd_protect_time = 0; /* 屏保触发时间，单位秒 */
static u16 lcd_pro_cnt = 0;
static int timer = 0;

static void *pwm_dev_handle = NULL;


// 打开LCD背光
static void ui_lcd_light_on(void)
{
    // 输出一条调试信息，表明背光开启
    puts("====ui_lcd_light_on====\n");

    // 调用函数控制背光开启
    lcd_backlight_ctrl(true);
}

// 关闭LCD背光
static void ui_lcd_light_off(void)
{
    // 输出一条调试信息，表明背光关闭
    puts("====ui_lcd_light_off====\n");

    // 调用函数控制背光关闭
    lcd_backlight_ctrl(false);
}



/*
 * 屏幕保护计时器
 */
// LCD 屏幕保护功能的核心函数
static void lcd_pro_kick(void *priv)
{
    // 如果正在倒车，取消屏幕保护
    if (get_parking_status()) {
        // 倒车状态下，不启动屏幕保护
        lcd_pro_cnt = 0;  // 重置屏幕保护计数
        return;  // 退出函数，不执行后续代码
    }

    // 当屏幕保护时间已设置且屏幕保护尚未激活时
    if (lcd_protect_time && lcd_pro_flag == 0) {
        lcd_pro_cnt++;  // 屏幕保护计数器加一

        // 如果屏幕保护计数器达到设定的保护时间
        if (lcd_pro_cnt >= lcd_protect_time) {
            puts("\n\n\n********lcd_pro_kick********\n\n\n");  // 打印调试信息
            lcd_pro_cnt = 0;  // 重置屏幕保护计数器
            lcd_pro_flag = 1;  // 设置屏幕保护标志为激活状态
            ui_lcd_light_off();  // 关闭LCD背光，进入屏幕保护状态
        }
    } else {
        // 如果不满足屏幕保护条件，重置屏幕保护计数器
        lcd_pro_cnt = 0;
    }
}


// 设置LCD背光自动关闭时间的函数
void ui_lcd_light_time_set(int sec)
{
    // 打印调试信息（已被注释）
    /*printf("ui_lcd_light_time_set sec:%d\n", sec);*/

    // 如果传入的时间参数不为零
    if (sec) {
        // 将屏幕保护时间设定为传入的时间（秒）
        lcd_protect_time = sec;

        // 如果定时器尚未启动
        if (!timer) {
            // 添加一个每秒触发一次的定时器，执行lcd_pro_kick函数
            timer = sys_timer_add(NULL, lcd_pro_kick, 1000);
        }
    } else {
        // 如果传入的时间参数为零，则关闭屏幕保护功能
        lcd_protect_time = 0;
    }
}


#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
static u8 ch0_duty = 100;// 全局变量，表示通道0的初始占空比，默认值为100（100%亮度）
// 初始化通道0的PWM，用于控制背光亮度
void pwm_ch0_backlight_init(u8 backlight_io)
{
    struct pwm_platform_data pwm_data = {0}; // 定义并初始化PWM配置数据结构
    int ret = 0;
    u8 find_pwm_io = 0;  // 定义一个用于查找PWM I/O口的变量，初始值为0

    // 如果PWM设备句柄尚未初始化
    if (!pwm_dev_handle) {
        // 打开PWM设备，并获取设备句柄
        pwm_dev_handle = dev_open("pwm", NULL);

        // 如果成功获取到PWM设备句柄
        if (pwm_dev_handle) {
            // 设置PWM通道为通道0（PWMCH0）
            pwm_data.pwm_ch = PWMCH0;

            // 设置PWM频率为100kHz
            pwm_data.freq = 100000;
            // 调用设备控制接口设置PWM频率
            dev_ioctl(pwm_dev_handle, PWM_SET_FREQ, (u32)&pwm_data);

            // 再次设置PWM通道为通道0
            pwm_data.pwm_ch = PWMCH0;

            // 设置PWM占空比为0（即初始关闭背光）
            pwm_data.duty = 0;
            // 调用设备控制接口设置PWM占空比
            dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data);

            // 再次设置PWM通道为通道0
            pwm_data.pwm_ch = PWMCH0;

            // 启用死区时间，防止PWM信号冲突
            pwm_data.dtime_en = true;
            // 设置死区时间为1.5微秒
            pwm_data.dtime_us = 1.5;
            /* dev_ioctl(pwm_dev_handle, PWM_SET_DEATH_TIME, (u32)&pwm_data); */ // 设置死区时间（注释掉的代码）

            // 再次设置PWM通道为通道0
            pwm_data.pwm_ch = PWMCH0;

            // 将PWM输出I/O口设置为传入的backlight_io
            pwm_data.output_io = backlight_io;

            // 设置PWM输出通道（这里选择通道3）
            pwm_data.outputchannel = 3;
            // 如果不想使用某个通道，可以通过设置outputchannel为-1来取消
            // pwm_data.outputchannel = -1;

            // 启动PWM，输出控制信号
            dev_ioctl(pwm_dev_handle, PWM_RUN, (u32)&pwm_data);

            //前照灯1设置
            pwm_data.pwm_ch = PWMCH7;  //IR红外 PWM_PORTG端口的通道0, 对应引脚PG8
            pwm_data.freq = 55000;  //1000;//1k
            ret = dev_ioctl(pwm_dev_handle, PWM_SET_FREQ, (u32)&pwm_data);

            pwm_data.pwm_ch = PWMCH7;
            pwm_data.duty = 0;
            ret = dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data);
            /* 死区设置，不用可注释掉 */
            pwm_data.pwm_ch = PWMCH7;
            pwm_data.dtime_en = true;
            pwm_data.dtime_us = 2;//1.5us
            /* dev_ioctl(pwm_dev_handle, PWM_SET_DEATH_TIME, (u32)&pwm_data); */

            pwm_data.pwm_ch = PWMCH7;
            pwm_data.outputchannel = -1;  //-1为取消outputchannel
            ret = dev_ioctl(pwm_dev_handle, PWM_RUN, (u32)&pwm_data);

            //前照灯2设置
            pwm_data.pwm_ch = PWMCH6;  //IR红外 PWM_PORTG端口的通道0, 对应引脚PG8
            pwm_data.freq = 55000;  //1000;//1k
            ret = dev_ioctl(pwm_dev_handle, PWM_SET_FREQ, (u32)&pwm_data);

            pwm_data.pwm_ch = PWMCH6;
            pwm_data.duty = 0;
            ret = dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data);
            /* 死区设置，不用可注释掉 */
            pwm_data.pwm_ch = PWMCH6;
            pwm_data.dtime_en = true;
            pwm_data.dtime_us = 1.5;//1.5us
            /* dev_ioctl(pwm_dev_handle, PWM_SET_DEATH_TIME, (u32)&pwm_data); */

            pwm_data.pwm_ch = PWMCH6;
            pwm_data.outputchannel = -1;  //-1为取消outputchannel
            ret = dev_ioctl(pwm_dev_handle, PWM_RUN, (u32)&pwm_data);
        }
    }
}


// 开启通道0的PWM背光
void pwm_ch0_backlight_on(void)
{
    struct pwm_platform_data pwm_data; // 定义PWM配置数据结构
    if (pwm_dev_handle) { // 检查PWM设备是否已打开
        pwm_data.pwm_ch = PWMCH0; // 设置PWM通道为通道0
        pwm_data.duty = ch0_duty; // 将占空比设置为全局变量ch0_duty的值
        dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data); // 调用设备控制接口设置PWM占空比
    }
}

// 关闭通道0的PWM背光
void pwm_ch0_backlight_off(void)
{
    struct pwm_platform_data pwm_data; // 定义PWM配置数据结构
    if (pwm_dev_handle) { // 检查PWM设备是否已打开
        pwm_data.pwm_ch = PWMCH0; // 设置PWM通道为通道0
        pwm_data.duty = 0; // 将占空比设置为0，关闭背光
        dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data); // 调用设备控制接口设置PWM占空比
    }
}

// 关闭并释放PWM背光控制资源
void pwm_ch0_backlight_close(void)
{
    if (pwm_dev_handle) { // 检查PWM设备是否已打开
        dev_close(pwm_dev_handle); // 关闭PWM设备
        pwm_dev_handle = NULL; // 将设备句柄设置为NULL，表示设备已关闭
    }
}

// 设置通道0的PWM背光占空比
void pwm_ch0_backlight_set_duty(u8 duty)
{
    ch0_duty = duty; // 更新全局变量ch0_duty的值
    pwm_ch0_backlight_on(); // 调用函数以更新PWM背光的占空比
}

#endif

// 背光事件处理函数
static void backlight_event_handler(struct sys_event *event)
{
    lcd_pro_cnt = 0; // 重置屏保计数器
    if (lcd_pro_flag) { // 如果屏幕当前处于关闭状态
        if (event->type == SYS_KEY_EVENT) { // 如果发生了按键事件
            ui_lcd_light_on(); // 打开背光
            lcd_pro_flag = 0; // 重置屏保标志，表示屏幕已被唤醒
            sys_key_event_consume(&(event->u.key)); // 消耗按键事件，表示该事件已处理完毕，防止进一步传递
        } else if (event->type == SYS_TOUCH_EVENT) { // 如果发生了触摸事件
            sys_touch_event_consume(&(event->u.touch)); // 消耗触摸事件，表示该事件已处理完毕
            if (event->u.touch.event == ELM_EVENT_TOUCH_UP) { // 如果是触摸抬起事件
                ui_lcd_light_on(); // 打开背光
                lcd_pro_flag = 0; // 重置屏保标志
            }
        }
    } else if (event->type == SYS_KEY_EVENT // 如果背光已经开启并且事件类型为按键事件
               && event->u.key.event == KEY_EVENT_CLICK // 按键事件类型为单击
               && event->u.key.value == KEY_POWER) { // 按键的值是电源键
        lcd_pro_flag = 1; // 设置屏保标志，表示屏幕进入保护状态
        ui_lcd_light_off(); // 关闭背光
        sys_key_event_consume(&(event->u.key)); // 消耗按键事件，防止进一步传递
    }
}

// 注册背光事件处理程序，处理按键和触摸事件，优先级为4
SYS_EVENT_HANDLER(SYS_KEY_EVENT | SYS_TOUCH_EVENT, backlight_event_handler, 4);



// 处理与背光相关的充电事件和停车事件的事件处理函数
static void backlight_charge_event_handler(struct sys_event *event)
{
    // 判断事件参数是否为 "parking"，并且长度为7
    if (!ASCII_StrCmp(event->arg, "parking", 7)) {
        if (event->u.dev.event == DEVICE_EVENT_IN) { // 如果是停车设备插入事件
            if (lcd_pro_flag) { // 如果背光关闭
                ui_lcd_light_on(); // 打开背光
                lcd_pro_flag = 0; // 重置屏保标志
            }
        } else if (event->u.dev.event == DEVICE_EVENT_OUT) { // 如果是停车设备拔出事件
            if (lcd_pro_flag) { // 如果背光关闭
                ui_lcd_light_on(); // 打开背光
                lcd_pro_flag = 0; // 重置屏保标志
            }
        }
    // 判断事件参数是否为 "sys_power"，并且长度为9
    } else if (!ASCII_StrCmp(event->arg, "sys_power", 9)) {
        if (event->u.dev.event == DEVICE_EVENT_POWER_CHARGER_IN) { // 如果是充电器插入事件
            if (lcd_pro_flag) { // 如果背光关闭
                ui_lcd_light_on(); // 打开背光
                lcd_pro_flag = 0; // 重置屏保标志
            }
        } else if (event->u.dev.event == DEVICE_EVENT_POWER_CHARGER_OUT) { // 如果是充电器拔出事件
            if (lcd_pro_flag) { // 如果背光关闭
                ui_lcd_light_on(); // 打开背光
                lcd_pro_flag = 0; // 重置屏保标志
            }
        }
    }

    // 额外处理停车事件，避免重复判断
    if (!strncmp(event->arg, "parking", 7)) {
        if (event->u.dev.event == DEVICE_EVENT_IN) { // 停车设备插入事件
            ui_lcd_light_on(); // 打开背光
            lcd_pro_flag = 0; // 重置屏保标志
        }
    }
}

// 注册背光和充电事件处理程序，处理设备相关事件，优先级为4
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, backlight_charge_event_handler, 4);



// void pwm_ch0_init(void)
// {
//     int ret = 0;
//     struct pwm_platform_data pwm_data = {0};
    

//     // 如果PWM设备句柄尚未初始化
//     if (!pwm_dev_handle) {
//         // 打开PWM设备，并获取设备句柄
//         pwm_dev_handle = dev_open("pwm", NULL);

//         // 如果成功获取到PWM设备句柄
//         if (pwm_dev_handle) {
//             // 设置PWM通道为通道0（PWMCH0）
//             pwm_data.pwm_ch = PWMCH7;

//             // 设置PWM频率为100kHz
//             pwm_data.freq = 100000;
//             // 调用设备控制接口设置PWM频率
//             ret = dev_ioctl(pwm_dev_handle, PWM_SET_FREQ, (u32)&pwm_data);

//             // 再次设置PWM通道为通道0
//             pwm_data.pwm_ch = PWMCH7;

//             // 设置PWM占空比为0（即初始关闭背光）
//             pwm_data.duty = 50;
//             // 调用设备控制接口设置PWM占空比
//             dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data);

//             // 再次设置PWM通道为通道0
//             pwm_data.pwm_ch = PWMCH7;

//             // 启用死区时间，防止PWM信号冲突
//             pwm_data.dtime_en = true;
//             // 设置死区时间为1.5微秒
//             pwm_data.dtime_us = 1.5;
//             /* dev_ioctl(pwm_dev_handle, PWM_SET_DEATH_TIME, (u32)&pwm_data); */ // 设置死区时间（注释掉的代码）

//             // 再次设置PWM通道为通道0
//            // pwm_data.pwm_ch = PWMCH7;

//             // 将PWM输出I/O口设置为传入的backlight_io
//            //pwm_data.output_io = backlight_io;

//             // 设置PWM输出通道（这里选择通道3）
//             pwm_data.outputchannel = PWMCH7;
//             // 如果不想使用某个通道，可以通过设置outputchannel为-1来取消
//              pwm_data.outputchannel = -1;

//             // 启动PWM，输出控制信号
//             dev_ioctl(pwm_dev_handle, PWM_RUN, (u32)&pwm_data);
//             printf("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
            
//         }
//     }
// }
// // 控制PWM通道0的占空比
// /**
//  * @brief 控制 PWM 通道 7 的占空比
//  *
//  * 该函数通过设置 PWM 通道 7 的占空比来控制其输出。
//  *
//  * @param data_val 占空比值，范围通常为 0 到 100，表示输出的百分比。
//  */
// void pwm_ch0_ctrl(u8 data_val)
// {
//     // 定义 PWM 平台数据结构
//     struct pwm_platform_data pwm_data;
    
//     // 检查 PWM 设备句柄是否有效
//     if(pwm_dev_handle){
//         pwm_data.pwm_ch = PWMCH7;  // 指定 PWM 通道 7
//         pwm_data.duty = duty_val;  // 设置占空比
//         //pwm_data.duty = data_val;  // 设置占空比
        
//         // 调用 ioctl 函数设置占空比
//         dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data);
//     }
// }


// /**
//  * @brief 控制 PWM 通道 7 的频率
//  *
//  * 该函数通过设置 PWM 通道 7 的频率来控制其输出。
//  *
//  * @param freq 频率值，以赫兹 (Hz) 为单位。
//  */
// void pwm_ch0_freq_ctrl(u64 freq)
// {
//     // 定义 PWM 平台数据结构
//     struct pwm_platform_data pwm_data;
    
//     // 检查 PWM 设备句柄是否有效
//     if(pwm_dev_handle){
//         pwm_data.pwm_ch = PWMCH7;  // 指定 PWM 通道 7
//         pwm_data.freq = 5000;      // 设置频率
//         pwm_data.freq = freq;      // 设置频率
//         printf("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
//         // 调用 ioctl 函数设置频率
//         dev_ioctl(pwm_dev_handle, PWM_SET_FREQ, (u32)&pwm_data);
//     }
// }



/* 占空比设置 */
void sys_pwm_ctrl(u8 ch, u8 duty_val)
{
    struct pwm_platform_data pwm_data;
    if (pwm_dev_handle) {
        pwm_data.pwm_ch = ch;
        pwm_data.duty = duty_val;
        dev_ioctl(pwm_dev_handle, PWM_SET_DUTY, (u32)&pwm_data);
    }
}

/* 频率设置 */
void sys_pwm_freq_ctrl(u8 ch, u64 freq)
{
    struct pwm_platform_data pwm_data;
    if(pwm_dev_handle){
        pwm_data.pwm_ch = ch;
        pwm_data.freq = freq;
        dev_ioctl(pwm_dev_handle, PWM_SET_FREQ, (u32)&pwm_data);
    }
}


