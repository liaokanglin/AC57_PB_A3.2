#ifndef __IMD_H__
#define __IMD_H__

#include "typedef.h"
#include "asm/cpu.h"
#include "generic/ioctl.h"
#include "asm/port_waked_up.h"
#include "asm/imd_common.h"

#define 	IMD_SET_COLOR_CFG 			_IOW('F', 0, sizeof(int))
#define 	IMD_GET_COLOR_CFG 			_IOR('F', 0, sizeof(int))
#define 	IMD_SET_COLOR_VALID 		_IOW('F', 1, sizeof(int))
#define 	IMD_GET_COLOR_CFG_SIZE 		_IOR('F', 1, sizeof(int))
#define 	IMD_SET_DEFAULT_COLOR_CFG 	_IOW('F', 2, sizeof(int))
#define 	IMD_GET_FRAMERATE		 	_IOR('F', 2, sizeof(int))
#define 	IMD_SET_REFRESH_STATUS   	_IOW('F', 3, sizeof(int))

extern const struct device_operations imd_dev_ops;

//<时钟数>
enum NCYCLE {
    CYCLE_ONE,      //RGB-24BIT、MCU-16BIT
    CYCLE_TWO,      //MCU-8BIT
    CYCLE_THREE,    //RGB-8BIT
    CYCLE_FOUR,     //RGB-8BIT+dummy
};

#define CLK_NEGATIVE    BIT(0)
#define CLK_EN          BIT(1)
#define CLK_DIS         0

#define SIGNAL_DEN      (0<<2)
#define SIGNAL_HSYNC    (1<<2)
#define SIGNAL_VSYNC    (2<<2)
#define SIGNAL_FIELD    (3<<2)

//<位宽>
enum LCD_PORT {
    PORT_1BIT   = 0x00,
    PORT_3BITS  = 0x01,
    PORT_6BITS  = 0x03,
    PORT_8BITS  = 0x07,
    PORT_9BITS  = 0x0F,
    PORT_16BITS = 0x1F,
    PORT_18BITS = 0x3F,
    PORT_24BITS = 0x7F,
};

//<lcd port select>
/*
 * PORT_GROUP_AA:
 *    DAT0~DAT23 : PG0~PG15,PH0~PH1,PH6~PH11
 *    PH2 : DCLK
 *    PH3 : DE(Default,configurable)
 *    PH4 : HSYNC(Default,configurable)
 *    PH5 : VSYNC(Default,configurable)
 *
 * PORT_GROUP_AB:
 *    DAT6~DAT23 : PG6~PG15,PH0~PH1,PH6~PH11
 *    PG2 : DE(Default,configurable)
 *    PG3 : DCLK
 *    PG4 : HSYNC(Default,configurable)
 *    PG5 : VSYNC(Default,configurable)
 *
 * PORT_GROUP_B:
 *    DAT0~DAT23 : PC0~PC15,PD0~PD1,PD6~PD11
 *    PD2 : DCLK
 *    PD3 : DE(Default,configurable)
 *    PD4 : HSYNC(Default,configurable)
 *    PD5 : VSYNC(Default,configurable)
 * */
enum LCD_GROUP {
    PORT_GROUP_AA,
    PORT_GROUP_AB,
    PORT_GROUP_B,
};

//<color type>
enum {
    IMD_COLOR_RGB,
    IMD_COLOR_YUV,
};


//<奇/偶行相位>
enum PHASE {
    PHASE_R,
    PHASE_G,
    PHASE_B,
};


//<RAW模式>
enum RAW_MODE {
    RAW_STR_MODE,//B->R->G(default)
    RAW_DLT_MODE,//R->G->B
};

//<模式>
enum LCD_MODE {
    MODE_RGB_SYNC,			// 无显存 连续帧 接HSYNC VSYNC信号
    MODE_RGB_DE_SYNC,		// 无显存 连续帧 按DE HSYNC VSYNC信号
    MODE_RGB_DE,			// 无显存 连续帧 接DE信号
    MODE_MCU,				// 有显存 单帧 SmartPanel 接DE信号
    MODE_AVOUT,				// AV OUT
    MODE_CCIR656,			// 嵌入同步信号(8BITS+DCLK、不需要HSYNC、VSYNC信号,00 FF为同步信号,传输数据不能为00 FF)
};

//<时钟选择以及分频>
#define IMD_INTERNAL    	0x8000 //使用单独的PLL，时钟源可配置
#define IMD_EXTERNAL    	0x0000 //[默认,可省略] 共享外部PLL时钟,目标频率为480MHz,通过分频得到所需的频率

/*
 * 注: IMD_EXTERNAL可省略
 * IMD_EXTERNAL|PLL2_CLK|DIVA_1|DIVB_2|DIVC_3         等同于 PLL2_CLK|DIVA_1|DIVB_2|DIVC_3
 */

#define PLL_SRC_SEL   		2
#define PLL_DIV1  			6
#define PLL_DIV2  			8

/*
 * @brief 时钟源选择
 */
#define PLL2_CLK_480M       (3<<PLL_SRC_SEL)
#define PLL2_CLK_192M       (4<<PLL_SRC_SEL)
#define PLL3_CLK        	(5<<PLL_SRC_SEL)
#define PLL4_CLK        	(6<<PLL_SRC_SEL)
#define PLL5_CLK        	(7<<PLL_SRC_SEL)


/**
 * @brief  时钟分频系统
 */
#define DIVA_1          	(0<<PLL_DIV1)
#define DIVA_3         		(1<<PLL_DIV1)
#define DIVA_5          	(2<<PLL_DIV1)
#define DIVA_7          	(3<<PLL_DIV1)

#define DIVB_1          	(0<<PLL_DIV2)
#define DIVB_2          	(1<<PLL_DIV2)
#define DIVB_4          	(2<<PLL_DIV2)
#define DIVB_8          	(3<<PLL_DIV2)

/**
 * @brief  颜色校正
 */

//二级命令										参数说明
#define ADJUST_EN       0x31//使能调试          不带参数
#define ADJUST_DIS      0x32//禁止调试			不带参数
#define GET_ALL         0x33//					struct color_correct
#define SET_ALL         0x34//					struct color_correct

#define SET_Y_GAIN      0x01//					u16
#define SET_U_GAIN      0x02//					u16
#define SET_V_GAIN      0x03//					u16
#define SET_Y_OFFS      0x04//					s16
#define SET_U_OFFS      0x05//					s16
#define SET_V_OFFS      0x06//					s16
#define SET_R_GAIN      0x07//					u16
#define SET_G_GAIN      0x08//					u16
#define SET_B_GAIN      0x09//					u16
#define SET_R_OFFS      0x0a//					s16
#define SET_G_OFFS      0x0b//					s16
#define SET_B_OFFS      0x0c//					s16
#define SET_R_COE0      0x0d//					u16
#define SET_R_COE1      0x0e//					s16
#define SET_R_COE2      0x0f//					s16
#define SET_G_COE0      0x10//					s16
#define SET_G_COE1      0x11//					u16
#define SET_G_COE2      0x12//					s16
#define SET_B_COE0      0x13//					s16
#define SET_B_COE1      0x14//					s16
#define SET_B_COE2      0x15//					u16
#define SET_R_GMA       0x16//					u16
#define SET_G_GMA       0x17//					u16
#define SET_B_GMA       0x18//					u16
#define SET_R_GMA_TBL   0x19//					256 bytes
#define SET_G_GMA_TBL   0x1a//					256 bytes
#define SET_B_GMA_TBL   0x1b//					256 bytes
#define SET_ISP_SCENE   0x1c//					s8
#define SET_CAMERA   	0x1d//					u8

#define CORRADJ_ALL_FLAGS     0x01fffffe
#define CORRADJ_MAIN_FLAGS    0x003ffffe

#define FORE_CAMERA     0x01
#define BACK_CAMERA     0x02
#define BOTH_CAMERA     0x03


struct color_effect_cfg {
    const char *fname;
    struct color_correct *adj;
};

/**
 * 硬件时序
 */
struct imd_dpi_timing {
    u16 hori_total; 		    /*水平时钟总数(Horizontal Line/HSD period)*/
    u16 hori_sync;		        /*水平同步时钟(HSYNC pulse width/HSD pulse width)*/
    u16 hori_back_porth;		/*水平起始时钟(HSYNC blanking/HSD back porth)*/
    u16 hori_pixel;	            /*水平像素(Horizontal display area/HSD display period)*/

    u16 vert_total;		        /*垂直同步总数(Vertical display area/VSD period time)*/
    u16 vert_sync;		        /*垂直同步时钟(VSYNC pulse width)*/
    u16 vert_back_porth_odd;	/*垂直起始时钟(VSYNC Blanking Odd field/VSD back porch Odd field)*/
    u16 vert_back_porth_even;	/*垂直起始时钟(隔行模式)(VSYNC Blanking Even field/VSD back porch Even field)*/
    u16 vert_pixel;	            /*垂直像素(Vertical display area)*/
};

/*
 * 屏接口类型
 */
enum LCD_IF {
    LCD_MIPI,
    LCD_DVP_MCU,
    LCD_DVP_RGB,
    LCD_LVDS,
};

////显示相关////

struct imd_dev {
    struct imd_dmm_info info;
    struct te_mode_ctrl te_mode;
    struct pll4_info pll4;
    enum LCD_MODE drive_mode;       // 驱动模式
    enum NCYCLE ncycle;             // 每像素时钟数
    enum PHASE raw_odd_phase;	    // 奇行相位
    enum PHASE raw_even_phase;      // 偶行相位
    enum RAW_MODE raw_mode;         // RAW模式选择
    enum LCD_PORT data_width;       // 数据位宽
    enum LCD_GROUP port_sel;        // 输出端口选择
    u8 avout_mode;				    // AVOUT制式(PAL/NTSC/TESTMODE)
    u8 dclk_set;                    // dclk使能以及极性配置
    u8 sync0_set;                   // AA:PH3/AB:PG2/B:PD3 (DE/HSYNC/VSYNC)
    u8 sync1_set;                   // AA:PH4/AB:PG4/B:PD4 (DE/HSYNC/VSYNC)
    u8 sync2_set;                   // AA:PH5/AB:PG5/B:PD5 (DE/HSYNC/VSYNC)
    u8 sync3_set;                   // Reversed
    u32 clk_cfg;                    // clk时钟分频配置
    struct imd_dpi_timing timing;   // 时序参数
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~beautiful line~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct imd_dev *imd_dpi_start();
void imd_dpi_kick_start();
void *imd_open();
u32 *imd_dmm_get_update_flags();
void imd_dmm_color_adj(struct color_correct *adj);
struct color_correct *imd_dmm_get_color_adj();
void imd_dmm_r_gamma_set(u8 *r_tab);
void imd_dmm_g_gamma_set(u8 *g_tab);
void imd_dmm_b_gamma_set(u8 *b_tab);
void imd_dmm_r_gamma_update();
void imd_dmm_g_gamma_update();
void imd_dmm_b_gamma_update();
void imd_dmm_r_gamma_cfg();
void imd_dmm_g_gamma_cfg();
void imd_dmm_b_gamma_cfg();


#define LCD_BUF_SIZE(w,h,s,n) (w*h*n*(((s)==SAMP_YUV420)?(3):(((s)==SAMP_YUV422)?4:6))/2)

extern u8 lcd_dev_buf_addr[];
#define REGISTER_LCD_DEVICE_BUF(w,h,s,n) \
	u8 lcd_dev_buf[LCD_BUF_SIZE(w, h, s, n)] sec(.lcd_dev_buf);

extern struct imd_dev lcd_device_begin[];
extern struct imd_dev lcd_device_end[];

#define list_for_each_lcd_device(p) \
	for (p=lcd_device_begin; p < lcd_device_end; p++)

#endif
