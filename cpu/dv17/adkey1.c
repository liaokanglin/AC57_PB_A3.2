#include "asm/adkey.h"
#include "asm/adc.h"
#include "generic/gpio.h"
#include "device/key_driver.h"
#include "device/device.h"


static struct key_driver adkey_driver;          // 定义一个静态的 `key_driver` 结构体，用于存储 ADKEY 驱动的状态和操作函数。
static struct adkey_platform_data *__this = NULL; // 定义一个静态的指针 `__this`，指向 `adkey_platform_data` 结构体，用于保存 ADKEY 的平台相关数据。

REGISTER_ADC_SCAN(adkey_scan)    // 使用 `REGISTER_ADC_SCAN` 宏注册一个名为 `adkey_scan` 的 ADC 扫描结构体。
.value = 0,                      // 初始化 `adkey_scan` 的 `value` 成员，表示当前 ADC 采样的初始值为 0。
};



static int adkey_init(struct key_driver *key, void *arg)
{
    // 将传入的 `arg` 参数转换为 `adkey_platform_data` 结构体指针，并赋值给 `__this`
    __this = (struct adkey_platform_data *)arg;
    if (!__this) {  // 如果 `__this` 为 NULL，返回错误代码 `-EINVAL`
        return -EINVAL;
    }

    // 初始化 ADKEY 的扫描通道
    adkey_scan.channel = __this->ad_channel;

    // 配置 GPIO 引脚作为输入，并设置上拉、下拉和数字功能
    gpio_direction_input(__this->io);
    gpio_set_pull_up(__this->io, 0);    // 禁用上拉
    gpio_set_pull_down(__this->io, 0);  // 禁用下拉
    gpio_set_die(__this->io, 1);        // 启用数字输入

    // 初始化 ADKEY 驱动的状态
    adkey_driver.prev_value = 0;        // 上一次按键的值初始化为 0
    adkey_driver.last_key = NO_KEY;     // 上次按键的值初始化为 NO_KEY
    adkey_driver.scan_time = 10;        // 扫描时间间隔设为 10 ms
    adkey_driver.base_cnt = __this->base_cnt; // 设置基础计数器值
    adkey_driver.long_cnt = __this->long_cnt; // 设置长按计数器值
    adkey_driver.hold_cnt = __this->hold_cnt; // 设置保持计数器值

    return 0;  // 返回 0 表示初始化成功
}


static u16 adkey_get_value(struct key_driver *key)
{
    int i;
    struct adkey_value_table *table = &__this->table;  // 获取当前键表的指针

//     printf("adkey1: %d\n", adkey_scan.value*330/0x3ff);

    for (i = 0; i < ADKEY_MAX_NUM; i++) {  // 遍历所有的 AD 键值
        if (adkey_scan.value >= table->ad_value[i]) {  // 如果当前的 ADC 值大于或等于键值表中的某个值
            return table->key_value[i];  // 返回对应的按键值
        }
        if (table->ad_value[i] == 0) {  // 如果遇到键值表中的 0 值（无效值）
            break;  // 跳出循环
        }
    }

    return NO_KEY;  // 如果没有找到匹配的键值，返回 NO_KEY 表示未检测到按键
}


// 定义一个静态常量结构体 `key_driver_ops`，用于定义 ADKEY 驱动的操作函数
static const struct key_driver_ops adkey_driver_ops = {
    .init       = adkey_init,        // 初始化函数，指向 `adkey_init`
    .get_value  = adkey_get_value,   // 获取键值函数，指向 `adkey_get_value`
};

// 使用 `REGISTER_KEY_DRIVER` 宏注册一个名为 `adkey_driver` 的键盘驱动
REGISTER_KEY_DRIVER(adkey_driver) = {
    .name = "adkey1",                 // 驱动的名称为 "adkey"
    .ops = &adkey_driver_ops,        // 指向前面定义的 `adkey_driver_ops` 结构体
};


