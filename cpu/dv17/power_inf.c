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

static DEFINE_SPINLOCK(lock);

extern struct power_platform_data sys_power_data;
static struct power_platform_data *__this = NULL;

extern int rtc_vdd50_enable();

/*系统上电原因打印*/
// 检查系统复位原因，并返回复位标志
static int system_reset_reason_check(void)
{
    volatile u32 reset_flag;  // 定义复位标志变量
    u8 normal;

    // 从电源控制寄存器（PWR_CON）中提取复位标志的高3位（即位5到位7）
    reset_flag = (PWR_CON & 0xe0) >> 5;

    // 根据复位标志判断复位原因
    switch (reset_flag) {
    case 0:
        log_v("=====power on reset=====\n");  // 上电复位
        break;
    case 1:
        log_v("=====VCM reset======\n");  // VCM复位（电源电压调节器）
        break;
    case 2:
        log_v("=====PR2 4s reset=====\n");  // PR2 4秒复位
        break;
    case 3:
        // 如果是LVD复位，检查是否为正常关机状态
        if (rtc_get_normal_poweroff_state()) {
            log_v("=====normal power off=====\n");  // 正常关机
            reset_flag = 6;  // 如果是正常关机，将复位标志改为6
        } else {
            log_v("=====LVD lower power reset=====\n");  // LVD低电压复位
        }
        break;
    case 4:
        log_v("=====WDT reset=====\n");  // 看门狗复位（WDT）
        break;
    case 5:
        log_v("=====software reset=====\n");  // 软件复位
        break;
    default:
        log_v("=====other reason======\n");  // 其他未知原因复位
        break;
    }

    return reset_flag;  // 返回复位标志
}



// 静态全局变量，用于标记CPU是否已经关闭
static volatile u8 cpu_poweroff_flag = 0;

// 关闭CPU电源的函数
static void cpu_poweroff(void *arg)
{
    cpu_poweroff_flag = 1;  // 设置标志位，表示CPU即将关闭
    local_irq_disable();    // 禁用本地中断，防止系统在关机过程中被中断

    // 无限循环等待，CPU进入低功耗的idle状态，等待关闭
    while (1) {
        asm("idle");  // 使用汇编指令将CPU置于idle状态
    }
}

// 系统关机函数
static void sys_poweroff(void *arg)
{
    // gpio_set_die(IO_PORT_PR_03,1);
    // gpio_set_pull_up(IO_PORT_PR_03,1);
    // gpio_set_pull_down(IO_PORT_PR_03,0);

    // gpio_set_die(IO_PORT_PR_02,1);
    // gpio_set_pull_up(IO_PORT_PR_02,1);
    // gpio_set_pull_down(IO_PORT_PR_02,0);

    // gpio_set_die(IO_PORT_PR_01,1);
    // gpio_set_pull_up(IO_PORT_PR_01,1);
    // gpio_set_pull_down(IO_PORT_PR_01,0);
    // 持续检测电源按键的状态，直到按键被释放
    while (__this->read_power_key());

    // 创建一个任务用于关闭CPU，调用cpu_poweroff函数
    task_create(cpu_poweroff, 0, "poweroff");

    // 等待CPU关闭标志位被设置（即等待cpu_poweroff函数执行）
    while (!cpu_poweroff_flag);

    // 调用RTC关机函数，关闭RTC电源
    rtc_poweroff();
}


//配置指定端口的唤醒设置
static int port_wakeup_set_config(const char *port, int enable)
{
    struct wkup_io_map *p;  // 唤醒IO映射结构指针
    int cnt = 0;  // 记录启用的端口计数
    static struct rtc_wkup_cfg rtc_cfg = {0};  // 静态RTC唤醒配置结构体，初始值为0

    // 检查端口名是否为空
    if (!port) {
        log_e("%s() port invalid\n", __FUNCTION__);  // 记录错误日志，端口无效
        return -EINVAL;  // 返回无效参数错误码
    }

    // 遍历唤醒端口映射表，寻找匹配的端口
    for (p = __this->wkup_map; p->wkup_port != 0; p++) {
        if (!strcmp(port, p->wkup_port)) {  // 如果找到匹配的端口
            // 根据唤醒端口类型配置唤醒边沿和启用状态
            switch (p->portr) {
            case WKUP_IO_PR1:  // 处理PR1端口
                rtc_cfg.pr1.edge = p->edge;  // 设置PR1的边沿触发模式
                rtc_cfg.pr1.port_en = enable;  // 启用或禁用PR1端口
                break;
            case WKUP_IO_PR2:  // 处理PR2端口
                rtc_cfg.pr2.edge = p->edge;  // 设置PR2的边沿触发模式
                rtc_cfg.pr2.port_en = enable;  // 启用或禁用PR2端口
                break;
            case WKUP_IO_PR3:  // 处理PR3端口
                rtc_cfg.pr3.edge = p->edge;  // 设置PR3的边沿触发模式
                rtc_cfg.pr3.port_en = enable;  // 启用或禁用PR3端口
                break;
            }
            break;  // 找到端口后，跳出循环
        }
    }

    // 如果没有找到匹配的端口，记录错误日志
    if (p->wkup_port == 0) {
        log_e("%s() port not found\n", __FUNCTION__);  // 记录错误日志，端口未找到
        return -EINVAL;  // 返回无效参数错误码
    }

    // 计算启用的端口数量
    cnt = rtc_cfg.pr1.port_en + rtc_cfg.pr2.port_en + rtc_cfg.pr3.port_en;

    // 根据启用的端口数量设置唤醒总开关
    rtc_cfg.wkup_en = cnt ? 1 : 0;

    // 调用函数配置RTC唤醒控制
    rtc_wkup_ctrl(&rtc_cfg);

    // 重新启用RTC电压检测功能
    rtc_vdd50_enable();  // 重新启用RTC的v50 ADC检测功能
    return 0;  // 返回0表示成功
}


//启用USB唤醒功能
static void usb_wakeup_enable()
{
    struct rtc_wkup_cfg rtc_cfg = {0};  // 初始化RTC唤醒配置结构体
    struct wkup_io_map *p;  // 唤醒IO映射结构指针
    const char *ch;  // 字符指针，用于遍历端口名称
    int cnt = 0;  // 计数器，用于标记是否找到USB相关端口

    // 遍历唤醒端口映射表，查找包含“usb”的端口
    for (p = __this->wkup_map; p->wkup_port != 0 && !cnt; p++) {
        ch = p->wkup_port;
        while (*ch && !cnt) {
            // 检查端口名中是否有“u”或“U”开头的
            if (*ch == 'u' || *ch == 'U') {
                // 比较是否为"usb"字符串
                if (!ASCII_StrCmpNoCase(ch, "usb", 3)) {
                    // 根据端口号设置相应的RTC配置
                    switch (p->portr) {
                    case WKUP_IO_PR1:
                        rtc_cfg.pr1.edge = p->edge;  // 设置PR1的边沿触发模式
                        rtc_cfg.pr1.port_en = 1;  // 启用PR1端口
                        break;
                    case WKUP_IO_PR2:
                        rtc_cfg.pr2.edge = p->edge;  // 设置PR2的边沿触发模式
                        rtc_cfg.pr2.port_en = 1;  // 启用PR2端口
                        break;
                    case WKUP_IO_PR3:
                        rtc_cfg.pr3.edge = p->edge;  // 设置PR3的边沿触发模式
                        rtc_cfg.pr3.port_en = 1;  // 启用PR3端口
                        break;
                    }
                    cnt = 1;  // 找到USB相关端口后，标记为已找到
                }
            }
            ch++;  // 移动到下一个字符
        }
    }

    // 计算启用的端口数量
    cnt = rtc_cfg.pr1.port_en + rtc_cfg.pr2.port_en + rtc_cfg.pr3.port_en;
    rtc_cfg.wkup_en = cnt ? 1 : 0;  // 如果至少有一个端口启用了，则启用唤醒功能

    // 调用RTC唤醒控制函数
    rtc_wkup_ctrl(&rtc_cfg);

    // 重新启用RTC的V50电压检测功能
    rtc_vdd50_enable();  // 重新启用RTC v50 ADC检测
}

// 设置闹钟唤醒功能的配置
static int alarm_wakeup_set_config(u32 sec, int enable)
{
    // 启用或禁用闹钟唤醒功能，并设置秒数
    alarm_wkup_ctrl(enable, sec);

    // 重新启用RTC的V50电压检测功能
    rtc_vdd50_enable();  // 重新启用RTC v50 ADC检测
    return 0;
}


//唤醒原因记录
// 检查唤醒原因，并将唤醒原因字符串填充到参数 'reason' 中，返回字符串长度
static int wkup_reason_check(char *reason, int max_len)
{
    u32 tmp;
    u32 len;
    int cnt = 0;
    struct wkup_io_map *p;

    // 检查输入参数的有效性
    if (!reason || !max_len) {
        log_e("%s() string buffer invalid\n", __FUNCTION__); // 错误日志输出
        return -EINVAL;  // 返回无效参数错误
    }

    // 检查系统复位原因是否为LVD复位（低电压检测复位）
    if (system_reset_reason_check() == 3) {
        // 如果是LVD复位并且没有充电器在线，则关机
        if (!__this->charger_online()) {
            log_v("need charge after LVD reset\n");  // 输出提示需要充电的日志
            rtc_pin_reset_ctrl(0);  // 关闭RTC引脚复位控制
            // usb_wakeup_enable();    // 启用USB唤醒
            sys_poweroff(0);        // 系统关机
            while (1);              // 进入死循环，防止系统继续运行
        }
    }

    // 检测电源键是否被按下
    for (int i = 0; i < 5; i++) {
        if (__this->read_power_key()) {  // 如果读取到电源键按下
            cnt++;  // 计数按键按下次数
        }
    }

    // 如果电源键按下次数大于等于4，认为是电源键唤醒
    if (cnt >= 4) {
        log_v("power key press wake up\n");  // 日志记录电源键唤醒
        // 根据最大长度填充唤醒原因字符串
        len = max_len > strlen(PWR_WKUP_PWR_ON) ? strlen(PWR_WKUP_PWR_ON) : max_len - 1;
        strncpy(reason, PWR_WKUP_PWR_ON, len);  // 复制唤醒原因到 reason
    }
    else {
        //否则，读取RTC的唤醒原因
        tmp = rtc_wkup_reason();

        //检查是否是I/O端口唤醒
        if (tmp & (WKUP_IO_PR1 | WKUP_IO_PR2 | WKUP_IO_PR3)) {
            len = max_len > strlen(PWR_WKUP_PORT) ? strlen(PWR_WKUP_PORT) : max_len - 1;
            for (p = __this->wkup_map; p->wkup_port != 0; p++) {  // 遍历唤醒端口映射表
                if (tmp & p->portr) {  // 如果匹配到端口唤醒
                    strncpy(reason, PWR_WKUP_PORT, len);  // 填充端口唤醒原因
                    max_len -= len;  // 更新剩余最大长度

                    // 检查是否有足够的空间继续填充字符串
                    if (max_len > 1) {
                        reason[len] = ':';  // 添加冒号
                        reason[len + 1] = '\0';  // 添加终止符
                        max_len -= 1;
                        // 填充具体的唤醒端口信息
                        len = max_len > strlen(p->wkup_port) ? strlen(p->wkup_port) : max_len - 1;
                        strncpy(reason + strlen(reason), p->wkup_port, len);
                        log_v("portr wakeup: %s\n", p->wkup_port);  // 输出日志，显示具体唤醒端口
                        break;
                    }
                }
            }
        }
        //检查是否是闹钟唤醒
        else if (tmp & WKUP_ALARM) {
            log_v("alarm wakeup\n");  // 输出日志
            len = max_len > strlen(PWR_WKUP_ALARM) ? strlen(PWR_WKUP_ALARM) : max_len - 1;
            strncpy(reason, PWR_WKUP_ALARM, len);  // 复制唤醒原因到 reason
        }
        //检查是否是异常复位唤醒
        else if (tmp & ABNORMAL_RESET) {
            log_v("abnormal wakeup\n");  // 输出异常复位日志
            len = max_len > strlen(PWR_WKUP_ABNORMAL) ? strlen(PWR_WKUP_ABNORMAL) : max_len - 1;
            strncpy(reason, PWR_WKUP_ABNORMAL, len);  // 复制唤醒原因到 reason

            // 如果没有充电器在线，则关机
            if (!__this->charger_online()) {
                rtc_pin_reset_ctrl(0);  // 关闭RTC引脚复位控制
                usb_wakeup_enable();    // 启用USB唤醒
                sys_poweroff(0);        // 系统关机
            }
        }
        //检查是否是电池首次接入唤醒
        else
        if (tmp & BAT_POWER_FIRST) {
            log_i("\n\n\nfirst power on\n\n\n");  // 输出首次上电日志
            len = max_len > strlen(PWR_WKUP_PWR_ON) ? strlen(PWR_WKUP_PWR_ON) : max_len - 1;
            strncpy(reason, PWR_WKUP_PWR_ON, len);  // 复制唤醒原因到 reason

            // 如果没有充电器在线，则关机
            if (!__this->charger_online()) {
                rtc_pin_reset_ctrl(0);  // 关闭RTC引脚复位控制
                usb_wakeup_enable();    // 启用USB唤醒
                sys_poweroff(0);        // 系统关机
            }
        }
    }

    //日志记录
    log_i("\n\n\nb6\n\n\n");
    usb_wakeup_enable();  // 启用USB唤醒
    __this->pwr_ctl(1);   // 控制电源开启
    return strlen(reason); // 返回唤醒原因字符串的长度
}



static void ldoin_updata();   // 声明一个更新函数 ldoin_updata()，具体实现如下。

REGISTER_ADC_SCAN(ldoin_scan)  // 注册一个 ADC 扫描对象 ldoin_scan，使用宏 REGISTER_ADC_SCAN 初始化。
// .channel = AD_CH13_RTC_2_ADC,  // 设置 ADC 扫描对象的通道为 AD_CH13_RTC_2_ADC。
.channel = AD_CH09_PH12,  // 设置 ADC 扫描对象的通道为 AD_CH13_RTC_2_ADC。
.value = 0,                    // 初始化通道的 ADC 值为 0。
.updata = ldoin_updata,        // 设置该通道数据更新时的回调函数为 ldoin_updata。
};

static int ldoin_sum = 0;      // 全局变量，累加 ldoin_scan 的 ADC 采样值。
static int ldoin_cnt = 0;      // 全局变量，记录 ldoin_scan 的采样次数。

static void ldoin_updata() {   // 定义 ldoin_updata 函数，该函数在采样值更新时被调用。
    spin_lock(&lock);          // 获取自旋锁，保证在多线程环境下操作 ldoin_sum 和 ldoin_cnt 的原子性。
    ldoin_sum += ldoin_scan.value;  // 将新的采样值加到 ldoin_sum 中。
    ldoin_cnt++;               // 累加采样次数。
    spin_unlock(&lock);        // 释放自旋锁。
}

static void ldo_vbg_updata();  // 声明一个更新函数 ldo_vbg_updata()，具体实现如下。

REGISTER_ADC_SCAN(ldo_vbg_scan)  // 注册一个 ADC 扫描对象 ldo_vbg_scan，使用宏 REGISTER_ADC_SCAN 初始化。
.channel = AD_CH15_LDO_VBG,      // 设置 ADC 扫描对象的通道为 AD_CH15_LDO_VBG。
// .channel = AD_CH09_PH12,      // 设置 ADC 扫描对象的通道为 AD_CH15_LDO_VBG。
.value = 0,                      // 初始化通道的 ADC 值为 0。
.updata = ldo_vbg_updata,        // 设置该通道数据更新时的回调函数为 ldo_vbg_updata。
};

static int ldo_vbg_sum = 0;      // 全局变量，累加 ldo_vbg_scan 的 ADC 采样值。
static int ldo_vbg_cnt = 0;      // 全局变量，记录 ldo_vbg_scan 的采样次数。

static void ldo_vbg_updata() {   // 定义 ldo_vbg_updata 函数，该函数在采样值更新时被调用。
    spin_lock(&lock);            // 获取自旋锁，保证在多线程环境下操作 ldo_vbg_sum 和 ldo_vbg_cnt 的原子性。
    ldo_vbg_sum += ldo_vbg_scan.value;  // 将新的采样值加到 ldo_vbg_sum 中。
    ldo_vbg_cnt++;               // 累加采样次数。
    spin_unlock(&lock);          // 释放自旋锁。
}


// static void battery_update();  // 声明电池更新函数

// REGISTER_ADC_SCAN(battery_scan)  // 注册一个 ADC 扫描对象 battery_scan
// .channel = AD_CH09_PH12,       // 设置 ADC 扫描对象的通道为 AD_CHXX_BATTERY（请根据具体通道替换）
// .value = 0,                       // 初始化通道的 ADC 值为 0。
// .updata = battery_update,         // 设置该通道数据更新时的回调函数为 battery_update。
// };

// static int battery_sum = 0;       // 全局变量，累加 battery_scan 的 ADC 采样值。
// static int battery_cnt = 0;       // 全局变量，记录 battery_scan 的采样次数。

// static void battery_update() {     // 定义 battery_update 函数
//     spin_lock(&lock);              // 获取自旋锁，保证在多线程环境下操作 battery_sum 和 battery_cnt 的原子性。
//     battery_sum += battery_scan.value;  // 将新的采样值加到 battery_sum 中。
//     battery_cnt++;                 // 累加采样次数。
//     spin_unlock(&lock);            // 释放自旋锁。
// }


/// @brief ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static void OTHER_KEY_updata();  // 声明一个更新函数 OTHER_KEY_updata()，具体实现不在本段代码中。

// REGISTER_ADC_SCAN(ldo_vbg_scan)  // 注册一个 ADC 扫描对象 ldo_vbg_scan，使用宏 REGISTER_ADC_SCAN 初始化。
// .channel = AD_CH05_PE03,      // 设置 ADC 扫描对象的通道为 AD_CH15_LDO_VBG。
// .value = 0,                      // 初始化通道的 ADC 值为 0。
// .updata = ldo_vbg_updata,        // 设置该通道数据更新时的回调函数为 ldo_vbg_updata。
// };

// static int ldo_vbg_sum = 0;      // 全局变量，累加 ldo_vbg_scan 的 ADC 采样值。
// static int ldo_vbg_cnt = 0;      // 全局变量，记录 ldo_vbg_scan 的采样次数。

// static void ldo_vbg_updata() {   // 定义 ldo_vbg_updata 函数，该函数在采样值更新时被调用。
//     spin_lock(&lock);            // 获取自旋锁，保证在多线程环境下操作 ldo_vbg_sum 和 ldo_vbg_cnt 的原子性。
//     ldo_vbg_sum += ldo_vbg_scan.value;  // 将新的采样值加到 ldo_vbg_sum 中。
//     ldo_vbg_cnt++;               // 累加采样次数。
//     spin_unlock(&lock);          // 释放自旋锁。
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
static struct adc_scan ldo_vbg_scan;

static int ldo_vbg_init(struct key_driver *key, void *arg)
{
    LDO_CON |= BIT(11);//VBG_EN
    LDO_CON |= BIT(15);//ADCOE

    return 0;
}

static u16 ldo_vbg_get_value(struct key_driver *key)
{
    return ldo_vbg_scan.value;
}

static const struct key_driver_ops ldo_vbg_driver_ops = {
    .init       = ldo_vbg_init,
    .get_value  = ldo_vbg_get_value,
};

REGISTER_KEY_DRIVER(ldo_vbg_driver) = {
    .name = "ad_ldo",
    .ops  = &ldo_vbg_driver_ops,
};
#endif


static int adc_scan_init()
{
    ADC_EN(1);        // 启用ADC，传递参数1表示启用
    ADCEN(1);         // 启用ADC的另一个相关功能，参数1表示启用
    ADC_BAUD(0x7);    // 设置ADC的波特率或采样速度为 0x7
    ADC_WTIME(0x1);   // 设置ADC的等待时间为 0x1，可能与采样时间相关
    return 0;         // 返回 0，表示初始化成功
}


static void adc_scan_process(void)
{
    ADCSEL(AD_CH15_LDO_VBG);    // 选择通道AD_CH15_LDO_VBG进行采样
    // ADCSEL(AD_CH09_PH12);    // 选择通道AD_CH15_LDO_VBG进行采样
    KITSTART();                 // 启动ADC采样
    while (!ADC_PND());          // 等待ADC采样完成
    ldo_vbg_scan.value = GPADC_RES; // 获取ADC结果并存储在ldo_vbg_scan.value中

    // ADCSEL(AD_CH13_RTC_2_ADC);  // 选择通道AD_CH13_RTC_2_ADC进行采样
    ADCSEL(AD_CH09_PH12);  // 选择通道AD_CH13_RTC_2_ADC进行采样
    KITSTART();                 // 启动ADC采样
    while (!ADC_PND());          // 等待ADC采样完成
    ldoin_scan.value = GPADC_RES; // 获取ADC结果并存储在ldoin_scan.value中

    // ADCSEL(AD_CH09_PH12);       // 选择通道AD_CHXX_BATTERY进行采样
    // KITSTART();                    // 启动ADC采样
    // while (!ADC_PND());            // 等待ADC采样完成
    // battery_scan.value = GPADC_RES; // 获取ADC结果并存储在battery_scan.value中
}


static int get_battery_voltage()
{
    static u16 in_val = 0;       // 电池输入电压的ADC值
    static u16 refer_val = 0;    // 参考电压的ADC值
    static u16 val = 0;          // 最终计算的电池电压
    static u16 battery_cnt_1 = 0;  // 电池电压采样计数器

    // 检查CPU是否禁止中断
    if (cpu_irq_disabled()) {
        // 如果中断被禁用，执行ADC扫描，获取参考电压和输入电压
        adc_scan_process();
        refer_val = ldo_vbg_scan.value;  // 获取参考电压ADC值
        in_val = ldoin_scan.value;       // 获取输入电压ADC值
       // battery_cnt_1 = battery_scan.value;  // 获取电池电压ADC值
    } else {
        // 启用自旋锁，防止多线程竞争资源
        spin_lock(&lock);
        // 如果参考电压和输入电压采样计数器非零，计算平均值
        if (ldo_vbg_cnt && ldoin_cnt) {
            refer_val = ldo_vbg_sum / ldo_vbg_cnt;  // 计算参考电压的平均值
            in_val = ldoin_sum / ldoin_cnt;         // 计算输入电压的平均值
            // 重置计数器和累加值
            ldo_vbg_sum = 0;
            ldo_vbg_cnt = 0;
            ldoin_sum = 0;
            ldoin_cnt = 0;
        }
        spin_unlock(&lock);  // 释放自旋锁
    }

    // 如果参考电压有效，开始计算电池电压
    if (refer_val) {
#if ENABLE_SAMPLE_VAL
        // 如果启用采样值校准，使用vbg_volt值进行计算
        const u16 vbg_volt = 129;  // 带隙电压校准值（需要通过测量校准）
        val = in_val * vbg_volt / refer_val * 3;  // 计算电池电压，乘以3因硬件电路原因
#else
        // 使用默认的参考电压常量进行计算
        val = (in_val * 3 * LDO_REFERENCE_VOL + 0x181 * 2) / refer_val;
#endif
    }

     /*输出调试信息：显示输入电压、参考电压和计算出的电池电压 */
     printf("%d : %d : %d \n", in_val, refer_val, val); 
    //  printf("%d ", battery_cnt_1); 
     printf("0x%x\n", val); 

    return val;  // 返回计算得到的电池电压值
}


/*static int ad_filter(void)
{
    int i = 0;
    int sum = 0;

    [>do {<]
    sum += get_battery_val();
    [>} while (++i < 10);<]
    [>sum = sum / 10;<]

    return sum;
}*/

// 这是一个弱符号函数，允许在其他地方重定义此函数
int __attribute__((weak)) voltage_to_persent(int voltage)
{
    // 遍历电压表，查找与输入电压对应的电池百分比
    for (int i = 0; i < 10; i++) {
        if (voltage <= __this->voltage_table[i][0]) {
            return __this->voltage_table[i][1]; // 返回对应的电池百分比
        }
    }
    return 100; // 如果没有匹配的电压，返回100%
}

// 这是一个弱符号函数，允许在其他地方重定义此函数
u16 __attribute__((weak)) voltage_get_min_val(void)
{
    return __this->min_bat_power_val; // 返回最低电池电压值
}

// 检查充电器是否在线
static int charger_online(void)
{
    return __this->charger_online(); // 调用__this对象的方法检查充电器状态
}

// 电源早期初始化
static void power_early_init()
{
    __this = &sys_power_data; // 将__this指针指向系统电源数据结构

    LDO_CON |= BIT(12); // 设置LDO控制寄存器的第12位
    LDO_CON |= BIT(13); // 设置LDO控制寄存器的第13位
    LDO_CON &= ~BIT(14); // 清除LDO控制寄存器的第14位
    /*ldo_vbg_init(NULL, NULL); // 注释掉的电压基准初始化函数
    ldoin_init(NULL, NULL); // 注释掉的低压输入初始化函数*/
    rtc_vdd50_enable(); // 启用RTC（实时时钟）的5V电源

    adc_scan_init(); // 初始化ADC扫描
}



REGISTER_SYS_POWER_HAL_OPS(sys_power) = {
    .init = power_early_init,             // 初始化电源管理系统
    .poweroff = sys_poweroff,             // 关闭系统电源
    .wakeup_check = wkup_reason_check,    // 检查唤醒原因
    .port_wakeup_config = port_wakeup_set_config, // 配置端口唤醒功能
    .alarm_wakeup_config = alarm_wakeup_set_config, // 配置定时唤醒功能
    .get_battery_voltage = get_battery_voltage, // 获取电池电压
    .charger_online = charger_online,      // 检查充电器是否连接
};

