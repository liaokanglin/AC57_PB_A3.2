#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__

#include "app_config.h"

#if (defined LCD_AVOUT)

#define NTSC_720		0
#define NTSC_960		1
#define PAL_720			2
#define PAL_960  		3

#define AVOUT_MODE      PAL_960
#endif


#ifndef MULTI_LCD_EN
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~beautiful line~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if (defined LCD_AVOUT)


#if (AVOUT_MODE == NTSC_720)
#define LCD_DEV_WIDTH	720
#define LCD_DEV_HIGHT	480
#elif (AVOUT_MODE == NTSC_960)
#define LCD_DEV_WIDTH	960
#define LCD_DEV_HIGHT	480
#elif (AVOUT_MODE == PAL_720)
#define LCD_DEV_WIDTH	720
#define LCD_DEV_HIGHT	576
#elif (AVOUT_MODE == PAL_960)
#define LCD_DEV_WIDTH	960
#define LCD_DEV_HIGHT	576
#endif

#endif


#if (defined LCD_DSI_DCS_1LANE_16BIT) ||\
    (defined LCD_DSI_DCS_1LANE_24BIT)
#define LCD_DEV_WIDTH	640
#define LCD_DEV_HIGHT	360
#endif


#if (defined LCD_DSI_DCS_2LANE_24BIT)
#define LCD_DEV_WIDTH	854
#define LCD_DEV_HIGHT	480
#endif

#if (defined LCD_DSI_VDO_4LANE_1280x800_RP)
#define LCD_DEV_WIDTH   1280
#define LCD_DEV_HIGHT   800
#endif

#if (defined LCD_DSI_VDO_4LANE_1280x800_RP_CH)
#define LCD_DEV_WIDTH   1280
#define LCD_DEV_HIGHT   800
#endif

#if (defined LCD_DSI_DCS_4LANE_1080P)
/*插值*/
#define LCD_DEV_WIDTH	1280//1920
#define LCD_DEV_HIGHT	720//1080
#endif

#if (defined LCD_DSI_DCS_4LANE_720P)
#define LCD_DEV_WIDTH	1280
#define LCD_DEV_HIGHT	720
#endif



#if (defined LCD_DSI_VDO_4LANE_720P_RM68200GA1)
#define LCD_DEV_WIDTH	1280
#define LCD_DEV_HIGHT	720
#endif

#if (defined LCD_480x272) || \
    (defined LCD_480x272_8BITS) || \
    (defined LCD_480x272_18BITS)
#define LCD_DEV_WIDTH	480
#define LCD_DEV_HIGHT	272
#endif

#if (defined LCD_VGA)
#define LCD_DEV_WIDTH	1280
#define LCD_DEV_HIGHT	720
#endif

#if (defined LCD_DSI_VDO_4LANE_MIPI) || \
    (defined LCD_DSI_VDO_2LANE_MIPI_EK79030) || \
    (defined LCD_DSI_VDO_4LANE_MIPI_EK79030) || \
    (defined LCD_DSI_VDO_4LANE_MIPI_EK79030_V2) || \
    (defined LCD_DSI_VDO_4LANE_MIPI_ICN9706)

#define LCD_DEV_WIDTH	1280
#define LCD_DEV_HIGHT	480
#endif

#if (defined LCD_HX8369A_MCU)|| \
    (defined LCD_HX8369A_SPI_RGB)
// #define LCD_DEV_WIDTH	320
// #define LCD_DEV_HIGHT	240
#define LCD_DEV_WIDTH	480
#define LCD_DEV_HIGHT	272
// #define NN 13
// #define LCD_DEV_WIDTH	(800-NN*16)
// #define LCD_DEV_HIGHT	(480-NN*16)
// #define LCD_DEV_WIDTH	1152
// #define LCD_DEV_HIGHT	720
// #define LCD_DEV_WIDTH	800
// #define LCD_DEV_HIGHT	480
// #define LCD_DEV_WIDTH	800
// #define LCD_DEV_HIGHT	480
// #define LCD_DEV_WIDTH	1920
// #define LCD_DEV_HIGHT	1088

#endif

#if (defined LCD_ST7789S_MCU)
#define LCD_DEV_WIDTH	320
#define LCD_DEV_HIGHT	240
#endif


#if (defined LCD_MIPI_4LANE_S8500L0)
/*插值*/
#define LCD_DEV_WIDTH	864//1296(被16整除)
#define LCD_DEV_HIGHT	224//336（被16整除）
#endif
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~beautiful line~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if (defined LCD_DSI_VDO_4LANE_1280x400)
#define LCD_DEV_WIDTH	1280
#define LCD_DEV_HIGHT	400
#endif

#if (defined LCD_LX50FWB4001_V2)
#define LCD_DEV_WIDTH	848/* 864 */
#define LCD_DEV_HIGHT	480/* 480 */
#endif

#if (defined LCD_ILI8960)
#define LCD_DEV_WIDTH   720
#define LCD_DEV_HIGHT   480
#endif

#if (defined LCD_ILI9806) || \
	(defined LCD_DSI_VDO_2LANE_MIPI_ST7701S)
#define LCD_DEV_WIDTH   800
#define LCD_DEV_HIGHT   480
#endif

#if (defined LCD_LYWS030GQ)
#define LCD_DEV_WIDTH   720
#define LCD_DEV_HIGHT   480
#endif

#if (defined LCD_OTA5182A)
#define LCD_DEV_WIDTH   320
#define LCD_DEV_HIGHT   240
#endif

#if (defined LCD_VIRTUAL)
// #define LCD_DEV_WIDTH   1280
// #define LCD_DEV_HIGHT   720
#define LCD_DEV_WIDTH   1920
#define LCD_DEV_HIGHT   1088
#endif

#if (defined LCD_LVDS_1024x600)
#define LCD_DEV_WIDTH   1024
#define LCD_DEV_HIGHT   600
#endif

#if (defined LCD_LVDS_1280x800)
#define LCD_DEV_WIDTH   1280
#define LCD_DEV_HIGHT   800
#endif

#if (defined MIPI_LCD_1920x1080_HX8399C)
#define LCD_DEV_WIDTH   1920
#define LCD_DEV_HIGHT   1088
#endif

#if (defined LCD_AVOUT_HD)
#define LCD_DEV_WIDTH   1280
#define LCD_DEV_HIGHT   720
#endif

#if (defined MIPI_LCD_1280x720_RM68200GA1)
#define LCD_DEV_WIDTH   1280
#define LCD_DEV_HIGHT   720
#endif

#if (defined LCD_LVDS_1920x480)
#define LCD_DEV_WIDTH   1920
#define LCD_DEV_HIGHT   480
#endif

#if (defined LCD_1920x480_OTA7290B)
#define LCD_DEV_WIDTH   1920
#define LCD_DEV_HIGHT   480
#endif

#if (defined LCD_1920x380_OTA7290B)
#define LCD_DEV_WIDTH   1920
#define LCD_DEV_HIGHT   384
#endif

#define LCD_DEV_SAMPLE	SAMP_YUV420
#define LCD_DEV_BNUM	2//同步显示BUF数量
#define LCD_DEV_BUF     (u32)lcd_dev_buf_addr

#ifndef LCD_DEV_WIDTH
// #error LCD_DEV_WIDTH isn't definition!!!
#define LCD_DEV_WIDTH 1
#endif
#ifndef LCD_DEV_HIGHT
#define LCD_DEV_HIGHT 1
// #error LCD_DEV_HIGHT isn't definition!!!
#endif

#else
#ifdef  CONFIG_HUNTING_CAMERA_ENABLE
#define LCD_DEV_HIGHT   224//224//lcd_get_height()
#define LCD_DEV_WIDTH   176//176//lcd_get_width()
#else

#define LCD_DEV_HIGHT   lcd_get_height()
#define LCD_DEV_WIDTH   lcd_get_width()
#endif
#define LCD_DEV_SAMPLE  SAMP_YUV420
#define LCD_DEV_BNUM    2
#define LCD_DEV_BUF     0

#endif

#endif // __LCD_CONFIG_HH

