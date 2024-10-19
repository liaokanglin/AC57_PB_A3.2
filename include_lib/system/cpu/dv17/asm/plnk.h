#ifndef __PLNK_H__
#define __PLNK_H__

#include "typedef.h"

/*! \addtogroup PLNK
 *  @ingroup HAL
 *  @brief	PLNK device api
 *  @{
 */

/**
 * \name PLNK采样率设置
 * \{
 */
#define PLNK_SAMPRATE_44_1KHZ           44100
#define PLNK_SAMPRATE_48KHZ             48000
#define PLNK_SAMPRATE_32KHZ             32000
#define PLNK_SAMPRATE_22_05KHZ          22050
#define PLNK_SAMPRATE_24KHZ             24000
#define PLNK_SAMPRATE_16KHZ             16000
#define PLNK_SAMPRATE_11_025KHZ         11025
#define PLNK_SAMPRATE_12KHZ             12000
#define PLNK_SAMPRATE_8KHZ              8000
/* \} name */


/**
 * \name 输入通道选择
 * \{
 */
//CLK: pb0 D0:pb1 D1:pe1
#define PLNK_MIC_MASK					(BIT(0) | BIT(1))
#define PLNK_CH_MIC_L					BIT(0)//PB1
#define PLNK_CH_MIC_R					BIT(1)//PE1
#define PLNK_CH_MIC_DOUBLE				(BIT(0) | BIT(1))
/* \} name */
/*plnk默认io*/

struct plnk_platform_data {
    u8 plnk_clk_io;
    u8 plnk_d0_io;
    u8 plnk_d1_io;
    void (*init)(const struct plnk_platform_data *, int channel);
};


/**
 * @brief 初始化PLNK
 *
 * @param sample_rate 采样率
 * @param channel 输入通道
 *
 * @return 0: 成功
 * @return -1: 失败
 */
int plnk_open(int sample_rate, int channel, struct plnk_platform_data *pd);

/**
 * @brief 启动PLNK
 *
 * @return 0: 成功
 * @return -1: 失败
 */
int plnk_start(void);

/**
 * @brief 停止PLNK
 *
 * @return 0: 成功
 * @return -1: 失败
 */
int plnk_stop(void);

/**
 * @brief 关闭PLNK
 *
 * @return 0: 成功
 * @return -1: 失败
 */
int plnk_close(void);

/**
 * @brief 设置PLNK采样率
 *
 * @param sample_rate 采样率
 *
 * @return 0: 成功
 * @return -1: 失败
 */
int plnk_set_sample_rate(int sample_rate);

/**
 * @brief 设置PLNK中断处理函数中的回调函数，用于处理采集到的数据
 *
 * @param priv The pointer to the private data of the callback function
 * @param cb 回调函数指针
 */
void plnk_set_handler_cb(void *priv, void (*cb)(void *, u8 *data, int len));

/**
 * @brief PLNK中断处理函数
 * @note  该函数用于注册中断
 */
void plnk_irq_handler(void);

/*! @}*/

/**
 * @brief PLNK IO占用初始化函数
 * @note
 * */

void plnk_iomc_init(const struct plnk_platform_data *pd, int channel);

extern void OutputChannelx(u32 ch, u32 io_signal, int gpio);
#endif


