#ifndef CPU_ADC_H
#define CPU_ADC_H  // 防止头文件重复包含

#include "typedef.h"  // 包含基本类型定义文件

/*
 * ADC通道选择
 */
enum {
    AD_CH00_PA02,         // 通道 0，连接到 PA02 引脚
    AD_CH01_PB00,         // 通道 1，连接到 PB00 引脚
    AD_CH02_PB03,         // 通道 2，连接到 PB03 引脚
    AD_CH03_PB12,         // 通道 3，连接到 PB12 引脚
    AD_CH04_PB14,         // 通道 4，连接到 PB14 引脚
    AD_CH05_PE03,         // 通道 5，连接到 PE03 引脚
    AD_CH06_PG00,         // 通道 6，连接到 PG00 引脚
    AD_CH07_PG07,         // 通道 7，连接到 PG07 引脚
    AD_CH08_PE05,         // 通道 8，连接到 PE05 引脚
    AD_CH09_PH12,         // 通道 9，连接到 PH12 引脚
    AD_CH10_PH14,         // 通道 10，连接到 PH14 引脚
    AD_CH11_PA05,         // 通道 11，连接到 PA05 引脚
    AD_CH12_PH10,         // 通道 12，连接到 PH10 引脚
    AD_CH13_RTC_2_ADC,    // 通道 13，连接到 RTC_2_ADC
    AD_CH14_ADC_ANA_DET,  // 通道 14，连接到 ADC_ANA_DET
    AD_CH15_LDO_VBG,      // 通道 15，连接到 LDO_VBG
};

/*
 * ADC 控制寄存器的宏定义
 */
#define ADC_WTIME(x)    GPADC_CON = (GPADC_CON & ~(0xf<<12)) | (x<<12)  // 设置等待时间（采样周期）
#define ADCSEL(x)       GPADC_CON = (GPADC_CON & ~(0xf<<8))  | (x<<8)   // 选择 ADC 通道
#define ADC_PND()       (GPADC_CON & BIT(7))                             // 检查是否有挂起的 ADC 中断
#define KITSTART()      GPADC_CON |= BIT(6)                              // 启动 ADC 转换
#define ADC_IE(x)       GPADC_CON = (GPADC_CON & ~BIT(5)) | (x<<5)       // 启用或禁用 ADC 中断
#define ADC_EN(x)       GPADC_CON = (GPADC_CON & ~BIT(4)) | (x<<4)       // 启用或禁用 ADC 功能
#define ADCEN(x)        GPADC_CON = (GPADC_CON & ~BIT(3)) | (x<<3)       // 启用或禁用 ADC 模块
#define ADC_BAUD(x)     GPADC_CON = (GPADC_CON & ~(BIT(2)| BIT(1)| BIT(0))) | (x)  // 设置 ADC 时钟速率

/*
 * 定义一个结构体，用于表示 ADC 通道的扫描信息
 */
struct adc_scan {
    u8 channel;  // 通道号
    u16 value;   // 通道的采样值
    void (*updata)();  // 更新回调函数
};

extern struct adc_scan adc_scan_begin[];  // 声明 ADC 扫描开始的数组
extern struct adc_scan adc_scan_end[];    // 声明 ADC 扫描结束的数组

/*
 * 定义宏用于注册一个 ADC 扫描通道
 */
#define REGISTER_ADC_SCAN(scan) \
	static struct adc_scan scan sec(.adc_scan) = {

/*
 * 函数声明，用于手动扫描 ADC 通道并返回结果
 */
u16 adc_scan_manual(u8 ch);

#endif  // 结束头文件的条件编译块
