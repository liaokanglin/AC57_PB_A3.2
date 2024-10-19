#include "asm/cpu.h"
#include "asm/adc.h"
#include "system/init.h"
#include "system/timer.h"

#define FIRST_CHANNEL() \
	adc_scan_begin  // 宏定义 FIRST_CHANNEL，表示扫描的第一个通道，返回 adc_scan_begin

#define NEXT_CHANNEL(ch) \
	++ch >= adc_scan_end ? adc_scan_begin : ch  // 宏定义 NEXT_CHANNEL，表示获取下一个通道，若当前通道超过了最后一个通道 
                                                 // adc_scan_end，则返回第一个通道 adc_scan_begin，否则返回下一个通道

static struct adc_scan *channel = FIRST_CHANNEL();  // 定义并初始化一个静态指针变量 channel，指向扫描的第一个 ADC 通道
static u8 manual_flag = 0;  // 定义并初始化一个静态变量 manual_flag，表示手动标志，初始值为 0
static u8 init = 0;  // 定义并初始化一个静态变量 init，用于标记初始化状态，初始值为 0



		// sys_adc_value = {                    // ADC值与对应按键的映射表
		// 	ADC0_33,                   // 对应 NO_KEY，未按下
		// 	ADC0_30,                   // 对应 NO_KEY，未按下
		// 	ADC0_27,                   // 对应 NO_KEY，未按下
		// 	ADC0_23,                   // 对应 NO_KEY，未按下
		// 	ADC0_20,                   // 对应 NO_KEY，未按下
		// 	ADC0_17,                   // 对应 NO_KEY，未按下
		// 	ADC0_13,                   // 对应 KEY_OK，按下 OK 键
		// 	ADC0_09,                   // 对应 KEY_DOWN，按下 DOWN 键
		// 	ADC0_06,                   // 对应 KEY_UP，按下 UP 键
		// 	ADC0_02,                   // 对应 KEY_MODE，按下 MODE 键
		// 	ADC0_00,                   // 对应 KEY_MENU，按下 MENU 键   11
		// },
		// sys_key_value = {                  // 对应的按键值
		// 	NO_KEY,    /*0*/            // 未按下
		// 	NO_KEY,                     // 未按下
		// 	KEY_LED,                     // 2F5  757
		// 	NO_KEY,                     // 未按下
		// 	NO_KEY,                     // 未按下
		// 	NO_KEY,                     // 未按下
		// 	KEY_OK,                     // OK 键
		// 	KEY_DOWN,                   // DOWN 键
		// 	KEY_UP,                     // UP 键
		// 	KEY_MODE,                   // MODE 键
		// 	KEY_MENU,                   // MENU 键
		// },

extern void adc_spin_lck();
extern void adc_spin_unlck();
static void adc_scan_process(void *p)
{
#if 0
    adc_spin_lck();
    if (!manual_flag) {
        channel = NEXT_CHANNEL(channel);
    }
    ADCSEL(channel->channel);
    KITSTART();
    while (!ADC_PND());
    if (!manual_flag) {
        channel->value = GPADC_RES;
    } else {
        manual_flag = 0;
    }
    /* printf(">>>>>>>>>>>>>>>>> ch: %d val:0x%x\n",channel->channel,channel->value); */
    adc_spin_unlck();
#else
   while (!ADC_PND());  // 等待 ADC 完成转换，即等待 ADC 中断挂起标志 (ADC_PND) 为真

if (!manual_flag) {  // 如果 manual_flag 未设置
    channel->value = GPADC_RES;  // 将 ADC 转换结果 (GPADC_RES) 存储到当前通道的 value 字段中
    if (channel->updata) {  // 如果通道的 updata 回调函数存在
        channel->updata();  // 调用 updata 回调函数进行更新
    }
    channel = NEXT_CHANNEL(channel);  // 切换到下一个通道
} else {
    manual_flag = 0;  // 如果 manual_flag 设置了，则手动标志重置为 0
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// if (channel == 6) {  // 如果当前通道编号为 6
//     for (i = 0; i < ADKEY_MAX_NUM; i++) {  // 遍历 ADKEY 最大数量的键值
//         if (channel->value >= sys_adc_value[i]) {  // 如果当前通道的值大于等于系统的 ADC 对应键值
//             sys_key_value[i];  // 对应的键值处理（此处缺少操作逻辑，需根据实际需求补充）
//         }
//         if (channel->value[i] == 0) {  // 如果当前通道的值为 0
//             break;  // 跳出循环
//         }
//     }
// }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// printf(">>>>>>>>>>>>>>>>> ch: %d val:0x%x\n",channel->channel,channel->value);  // 打印当前通道编号和值，用于调试
ADCSEL(channel->channel);  // 选择 ADC 下一个要转换的通道
KITSTART();  // 启动 ADC 转换

#endif
}


static int adc_scan_init() {
    ADCSEL(channel->channel);    // 选择当前通道，将 `channel->channel` 的值写入 ADC 通道选择寄存器。
    ADC_EN(1);                   // 启用 ADC 模块。
    ADCEN(1);                    // 使能 ADC。
    ADC_BAUD(0x7);               // 设置 ADC 时钟分频因子，`0x7` 表示较低的时钟频率。
    ADC_WTIME(0x1);              // 设置 ADC 采样等待时间，`0x1` 表示最短等待时间。
    KITSTART();                  // 开始 ADC 采样。

    // LDO_CON |= BIT(11);       // （已注释）可能用于启用低压差稳压器（LDO）的相关配置。

    sys_hi_timer_add(0, adc_scan_process, 4);  // 添加一个高精度定时器，间隔4ms调用 `adc_scan_process` 函数。
    init = 1;                    // 设置初始化标志 `init` 为 1，表示 ADC 扫描已初始化。

    return 0;                    // 返回 0，表示初始化成功。
}
platform_initcall(adc_scan_init);  // 注册 `adc_scan_init` 函数为系统初始化调用函数。

u16 adc_scan_manual(u8 ch)
 {
    if (!init) {                 // 如果 ADC 扫描尚未初始化，返回 -1 表示失败。
        return -1;
    }
    u16 val = 0;
    manual_flag = 1;             // 设置 `manual_flag` 为 1，表示手动模式。
    while (!ADC_PND());          // 等待上一次 ADC 转换完成。
    /* log_d("adc_scan_manual ch%d\n",ch); */
    ADCSEL(ch);                  // 选择指定的 ADC 通道。
    KITSTART();                  // 开始 ADC 采样。
    while (!ADC_PND());          // 等待 ADC 采样完成。
    val = GPADC_RES;             // 获取 ADC 采样结果。
    return val;                  // 返回采样值。
}


