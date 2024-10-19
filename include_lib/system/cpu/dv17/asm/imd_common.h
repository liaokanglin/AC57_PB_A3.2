#ifndef __IMD_DMM_H__
#define __IMD_DMM_H__

#include "typedef.h"

//<帧模式>
enum BUFFER_MODE {
    BUFFER_FRAME,
    BUFFER_DOUBLE,
};

//<扫描方式>
enum INTERLACED {
    INTERLACED_NONE,		//非隔行
    INTERLACED_TIMING,		//时序隔行
    INTERLACED_DATA,		//数据隔行
    INTERLACED_ALL,			//(INTERLACED_DATA|INTERLACED_TIMING)时序数据隔行
};

//<input format>
enum IN_FORMAT {
    SAMP_YUV420,
    SAMP_YUV422,
    SAMP_YUV444,
    SAMP_YUV422_SPECIAL,
    SAMP_RGB_TO_MIPI,
};

//<输出数据格式>
enum OUT_FORMAT {
    FORMAT_RGB333 = 2,
    FORMAT_RGB666,
    FORMAT_RGB888,
    FORMAT_RGB565,
    FORMAT_YUV888,
    FORMAT_YUV422,
    FORMAT_ITU656, //YUV422同时使能
};

//<访问长度>
enum LEN_CFG {
    LEN_512,
    LEN_256,
};

enum ODD_EVEN_FILED {
    ODD_FILED, /*默认*/
    EVEN_FILED,
    SAME_FILED,
};

struct color_correct {
    u16 y_gain;
    u16 u_gain;
    u16 v_gain;
    s16 y_offs;
    s16 u_offs;
    s16 v_offs;

    u16 r_gain;
    u16 g_gain;
    u16 b_gain;
    s16 r_offs;
    s16 g_offs;
    s16 b_offs;

    u16 r_coe0;
    s16 r_coe1;
    s16 r_coe2;

    s16 g_coe0;
    u16 g_coe1;
    s16 g_coe2;

    s16 b_coe0;
    s16 b_coe1;
    u16 b_coe2;

    u16 r_gma;
    u16 g_gma;
    u16 b_gma;

    u8 r_gamma_tab[256];
    u8 g_gamma_tab[256];
    u8 b_gamma_tab[256];
};

/*
 *TIPS:由于avout对时钟的要求比较高,想要比较好的效果,建议使用12M的晶振
 * */
enum PLL4_SRC {
    PLL4_SRC_XOSC0 = 1, /*PG14、PG15脚挂12M晶振*/
    PLL4_SRC_XOSC1,     /*PB14、PB15脚挂12M晶振*/
    PLL4_SRC_RTCOSC,
    PLL4_SRC_HTC,
    PLL4_SRC_OSC,
    PLL4_SRC_x12M,
};

struct pll4_info {
    u8  source;
    u16 pll_freq;
};

struct imd_dmm_info {
    u16 x;                      // 显存在画布中的x坐标
    u16 y;                      // 显存在画布中的y坐标
    u16 xres;					// 显存水平分辨率
    u16 yres;					// 显存垂直分辨率
    u16 target_xres;            // 屏水平分辨率
    u16 target_yres;            // 屏垂直分辨率
    u8  sw_mode;                // 窗口移动模式
    u16 sw_x;
    u16 sw_y;
    u16 sw_xres;                // 窗口宽度
    u16 sw_yres;                // 窗口高度

    u8  buf_num;				// 显示缓存数量
    u32 buf_addr;				// 显示缓存
    enum BUFFER_MODE mode;
    enum INTERLACED interlaced_mode;         // 隔行模式

    u8  test_mode;				// 测试模式(纯色)使能
    u32 test_mode_color;		// 测试模式颜色设置
    u32 canvas_color;           // 画布颜色

    enum IN_FORMAT sample;		// 采样方式 YUV420/YUV422/YUV444
    enum OUT_FORMAT format;     // 输出数据格式
    struct color_correct adjust;// 颜色校正参数

    enum LEN_CFG len;		    // 访问的块长度
    enum ODD_EVEN_FILED interlaced_1st_filed;  	// 首场是奇场or偶场,INTERLACED_DATA 时有效

    u8  rotate_en;				// 旋转使能
    u8  hori_mirror_en;			// 水平镜像使能
    u8  vert_mirror_en;			// 垂直镜像使能
};

int imb_init(void *arg);
void imb_uninit();
void *imd_dmm_init(struct imd_dmm_info *info);
void *imd_dmm_uninit(struct imd_dmm_info *info);
void imd_dmm_color_adj(struct color_correct *adj);
struct color_correct *imd_dmm_get_color_adj();
void pll4_init(u16 freq, u8 source);

#endif
