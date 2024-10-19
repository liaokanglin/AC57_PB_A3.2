#ifndef __POWER_MANAGE_H_
#define __POWER_MANAGE_H_

#include "generic/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    DEVICE_EVENT_POWER_SHUTDOWN = 0x10,  // 设备关机事件
    DEVICE_EVENT_POWER_STARTUP,           // 设备启动事件
    DEVICE_EVENT_POWER_PERCENT,           // 电池电量百分比事件
    DEVICE_EVENT_POWER_CHARGER_IN,       // 充电器插入事件
    DEVICE_EVENT_POWER_CHARGER_OUT,      // 充电器拔出事件
    DEVICE_EVENT_STD_POWER_SHUTDOWN,      // 自定义标准关机流程
    DEVICE_EVENT_SPECIAL_POWER_SHUTDOWN,  // 自定义特殊关机流程，针对低电量
};


#define PWR_SCAN_TIMES 	   3

#define PWR_DELAY_INFINITE      0xffffffff

#define PWR_WKUP_PORT           "wkup_port"        // 唤醒端口事件
#define PWR_WKUP_ALARM          "wkup_alarm"       // 唤醒闹钟事件
#define PWR_WKUP_PWR_ON         "wkup_pwr_on"      // 电源开启唤醒事件
#define PWR_WKUP_ABNORMAL       "wkup_abnormal"    // 异常唤醒事件
#define PWR_WKUP_SHORT_KEY      "wkup_short_key"   // 短按键唤醒事件XC


struct sys_power_hal_ops {
    void (*init)(void);                      // 初始化电源管理系统
    void (*poweroff)(void *arg);             // 关闭系统电源，接受一个参数
    int (*wakeup_check)(char *reason, int max_len); // 检查唤醒原因，返回唤醒状态
    int (*port_wakeup_config)(const char *port, int enable); // 配置端口唤醒功能
    int (*alarm_wakeup_config)(u32 sec, int enable); // 配置定时器唤醒
    int (*get_battery_voltage)(void);        // 获取电池电压
    int (*get_battery_percent)(void);        // 获取电池电量百分比
    int (*charger_online)(void);              // 检查充电器是否连接
};


extern const struct sys_power_hal_ops sys_power_hal_ops_begin[];
extern const struct sys_power_hal_ops sys_power_hal_ops_end[];

#define REGISTER_SYS_POWER_HAL_OPS(ops) \
    static const struct sys_power_hal_ops ops sec(.sys_power_hal_ops)


int sys_power_init(void);  // 初始化电源管理系统

void sys_power_early_init();  // 早期初始化电源管理系统

/*
 * @brief 断电关机，不释放资源
 */
void sys_power_poweroff();  // 直接断电关机，不进行资源释放

/*
 * @brief 软关机，触发DEVICE_EVENT_POWER_SHUTDOWN事件，
 * app捕获事件释放资源再调用sys_power_poweroff()
 */
void sys_power_shutdown();  // 软关机，允许应用释放资源后关机

int sys_power_set_port_wakeup(const char *port, int enable);  // 设置指定端口的唤醒功能

int sys_power_set_alarm_wakeup(u32 sec, int enable);  // 设置定时唤醒功能

const char *sys_power_get_wakeup_reason();  // 获取唤醒原因

void sys_power_clr_wakeup_reason(const char *str);  // 清除指定的唤醒原因

int sys_power_get_battery_voltage();  // 获取电池电压

int sys_power_get_battery_persent();  // 获取电池百分比

int sys_power_is_charging();  // 检查设备是否正在充电

int sys_power_charger_online(void);  // 检查充电器是否在线

/*
 * @brief 倒计时自动关机
 * @parm dly_secs 延时关机时间，赋值0为永不关机
 * @return none
 */
void sys_power_auto_shutdown_start(u32 dly_secs);
void sys_power_auto_shutdown_pause();
void sys_power_auto_shutdown_resume();
void sys_power_auto_shutdown_clear();
void sys_power_auto_shutdown_stop();


int sys_power_low_voltage(u32 voltage);

/*
 * @brief 低电延时关机
 * @parm p_low_percent 低电电量百分比
 * @parm dly_secs 延时关机时间，赋值0为立即关机，赋值PWR_DELAY_INFINITE为永不关机
 * @return none
 */
void sys_power_low_voltage_shutdown(u32 voltage, u32 dly_secs);
/*
 * @brief 插拔延时关机
 * @parm dly_secs 延时关机时间，赋值0为立即关机，赋值PWR_DELAY_INFINITE为永不关机
 * @parm cancel   允许取消，赋值0为不可取消，赋值1为按键或触屏取消关机
 * @return none
 */
void sys_power_charger_off_shutdown(u32 dly_secs, u8 cancel);


#ifdef __cplusplus
}
#endif
#endif
