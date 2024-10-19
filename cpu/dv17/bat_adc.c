
#include "device/device.h"
#include "device/key_driver.h"
#include "timer.h"
#include "database.h"
#include "event.h"
#include "asm/adc.h"
#include "asm/rtc.h"
#include "os/os_api.h"
#include "power_manage.h"
#include "asm/power_inf.h"
#include "system/spinlock.h"
#include "generic/ascii.h"
#include "asm/gpio.h"
#include "system/init.h"



//////////////////////////////////////////////////电池采样//////////////////////////////////////////
static void bat_updata();   // 声明一个更新函数 ldoin_updata()，具体实现如下。
REGISTER_ADC_SCAN(BET_scan1)
.channel = AD_CH09_PH12,
 .value = 0,
 .updata = bat_updata
};
// REGISTER_ADC_SCAN(ldo_vbg_scan)  // 注册一个 ADC 扫描对象 ldo_vbg_scan，使用宏 REGISTER_ADC_SCAN 初始化。
// .channel = AD_CH15_LDO_VBG,      // 设置 ADC 扫描对象的通道为 AD_CH15_LDO_VBG。
// // .channel = AD_CH09_PH12,      // 设置 ADC 扫描对象的通道为 AD_CH15_LDO_VBG。
// .value = 0,                      // 初始化通道的 ADC 值为 0。
// .updata = ldo_vbg_updata,        // 设置该通道数据更新时的回调函数为 ldo_vbg_updata。
// };

// int ntc_get1_value(struct key_driver *key)
// {
//     return BET_scan1.value; // 
// }

static int ldoin_sum = 0;      // 全局变量，累加 ldoin_scan 的 ADC 采样值。
static int ldoin_cnt = 0;      // 全局变量，记录 ldoin_scan 的采样次数。

static void bat_updata() {   // 定义 ldoin_updata 函数，该函数在采样值更新时被调用。
    spin_lock(&lock);          // 获取自旋锁，保证在多线程环境下操作 ldoin_sum 和 ldoin_cnt 的原子性。
    ldoin_sum += BET_scan1.value;  // 将新的采样值加到 ldoin_sum 中。
    ldoin_cnt++;               // 累加采样次数。
    spin_unlock(&lock);        // 释放自旋锁。
}

static void send_power_msg(int level)
{
    struct sys_event e;  // 定义一个系统事件结构体变量
    e.arg = "powerdet";  // 设置事件的参数，标识事件来自电源检测模块
    e.type = SYS_DEVICE_EVENT;  // 设置事件类型为设备事件
    e.u.dev.event = DEVICE_EVENT_CHANGE;  // 设置事件为设备状态改变事件
    e.u.dev.value = level;  // 设置事件的值为当前电源电平
    sys_event_notify(&e);  // 通过系统事件通知机制发送该事件
}

#define SCAN_CNT 10
#define FILTER_SIZE 5  // 滤波器大小

static int bat_total_value = 0;
static int bat_total_cnt = 0;
static int bat_cur_value = 0;
static int bat_cur_level = -1;
static int bat_filter_buffer[FILTER_SIZE] = {0};  // 滤波器缓冲区
static int bat_filter_index = 0;  // 当前滤波器索引

static void ntc_scan1_process(void *p)
{   printf("ntc_get1_value%d\n", ntc_get1_value(NULL));
    printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW\n");
    // 第一次读取电池电量
    if (bat_cur_level == -1) {
        bat_cur_value = ntc_get1_value(NULL);
        if (bat_cur_value < 834) {
            bat_cur_level = 0;
        } else if (bat_cur_value < 850) {
            bat_cur_level = 1;
        } else if (bat_cur_value < 874) {
            bat_cur_level = 2;
        } else if (bat_cur_value < 900) {
            bat_cur_level = 3;
        } else if (bat_cur_value < 924) {
            bat_cur_level = 4;
        }
        return;       
    }
    printf("ntc_get1_value%d\n", ntc_get1_value(NULL));
    // 获取当前电池电量并进行滤波
    int current_value = ntc_get1_value(NULL);
    bat_total_value += current_value;
    bat_total_cnt++;
    printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW1\n");
    // 更新滤波器
    bat_filter_buffer[bat_filter_index] = current_value;  // 将当前值加入滤波器
    bat_filter_index = (bat_filter_index + 1) % FILTER_SIZE;  // 更新索引
    printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW2\n");
    // 计算滤波后的电池值
    int filtered_value = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        filtered_value += bat_filter_buffer[i];
    }
    filtered_value /= FILTER_SIZE;  // 计算平均值
    printf("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW3\n");
    // 达到采样次数，计算电池电量
    if (bat_total_cnt >= SCAN_CNT) {
        bat_cur_value = bat_total_value / SCAN_CNT;  // 计算总的电池值
        bat_total_value = 0;  // 重置总值
        bat_total_cnt = 0;  // 重置计数
        printf("bat_cur_value: %d\n", bat_cur_value);
        // 确定电池电量级别
        int level = 0;
        // if (filtered_value < 834) {
        //     level = 0;
        // } else
         if (filtered_value < 838) { //850
            level = 0;
        } else if (filtered_value < 874) {
            level = 2;
        } else if (filtered_value < 900) {
            level = 3;
        } else if (filtered_value < 924) {
            level = 4;
        } else {
            level = 5;
        }

        // 发送电量变化消息
        if (level != bat_cur_level) {
            bat_cur_level = level;  // 更新当前电量等级
            send_power_msg(bat_cur_level);  // 发送当前电量等级
            printf("bat_cur_level: %d\n", bat_cur_level);
        }
    }
}


int get_bat_level()
{
    return bat_cur_level;
}

int ntc_scan1_init()
{
    gpio_direction_input(IO_PORTH_12);
    gpio_set_pull_up(IO_PORTH_12, 0);
    gpio_set_pull_down(IO_PORTH_12, 0);
    gpio_set_die(IO_PORTH_12, 1);

    // sys_timer_add(0, ntc_scan1_process, 2000); 
    sys_timer_add(0, ntc_scan1_process, 500); 
    printf("BTC_scan1_init\n");
    return 0; // 初始化成功
}

// 将ntc_scan_init函数注册为平台初始化调用函数，
// 当平台初始化时，这个函数会被自动调用，进行NTC扫描的初始化设置。
late_initcall(ntc_scan1_init);
///////////////////////////////////////////////////////////////////////////////////


// #define SAMPLES 100            // 每次采样的数量
// #define MIN_CHANGE 5           // 最小变化门限，防止频繁变化
// #define MAX_CHANGE_RATE 2      // 最大每次允许的电量变化，限制变化速率
// #define HYSTERESIS 1          // 滞后区间，防止临界点抖动

// // extern int ntc_get1_value();   // 电池采样函数，获取ADC采样值

// // 全局变量存储电池电量和历史电量
// int battery_level = 0;
// int last_battery_level = 0;    // 上一次的电量值

// // 滤波和采样算法，加入滞后机制
// int filter_and_classify_battery_value() {
//     int total_value = 0;
//     int avg_value;
//     int sample_value;

//     // 多次采样取平均值
//     for (int i = 0; i < SAMPLES; i++) {
//         sample_value = ntc_get1_value(NULL);  // 获取一次采样值
//         total_value += sample_value;
//     }
//     avg_value = total_value / SAMPLES;

//     // 根据平均采样值进行分档
//     int new_battery_level = 0;
//     if (avg_value < 834) {
//         new_battery_level = 0;  // 0档
//     } else if (avg_value < 850) {
//         new_battery_level = 1;  // 1档  3.7V
//     } else if (avg_value < 874) {
//         new_battery_level = 2;  // 2档  3.8V
//     } else if (avg_value < 900) {
//         new_battery_level = 3;  // 3档  3.9V
//     } else if (avg_value < 924) {
//         new_battery_level = 4;  // 4档  4.0V
//     } else {
//         new_battery_level = 5;  // 5档
//     }

//     // 引入滞后机制，防止在临界值附近频繁跳动
//     // if (abs(new_battery_level - last_battery_level) >= HYSTERESIS) {
//     //     // 电量变化超过滞后区间，允许更新电量
//     //     last_battery_level = new_battery_level;
//     // } else {
//     //     // 电量变化未超过滞后区间，不更新
//     //     new_battery_level = last_battery_level;
//     // }

//     printf("Battery level: %d (Avg: %d)\n", new_battery_level, avg_value);
    
//     // 返回最终分档结果
//     return new_battery_level;
// }










