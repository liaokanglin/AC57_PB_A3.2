#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"



#ifdef LCD_VGA

#define RES_640x480   1
#define RES_800x600   2
#define RES_1280x720  3
#define RES_1920x1080 4

#define RES RES_1280x720

static void lcd_vga_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}


REGISTER_IMD_DEVICE_BEGIN(lcd_vga_dev) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0x00FF00,
        .canvas_color    = 0x000000,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
#if (RES == RES_640x480)
        .target_xres     = 640,
        .target_yres     = 480,
#elif (RES == RES_800x600)
        .target_xres     = 800,
        .target_yres     = 600,
#elif (RES == RES_1280x720)
        .target_xres     = 1280,
        .target_yres     = 720,
#elif (RES == RES_1920x1080)
        .target_xres     = 1920,
        .target_yres     = 1080,
#endif
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .len 			 = LEN_256,
        .sample          = LCD_DEV_SAMPLE,
        .format          = FORMAT_RGB888,
        .interlaced_mode = INTERLACED_NONE,

        .rotate_en       = false,	    // 旋转使能
        .hori_mirror_en  = false,	    // 水平镜像使能
        .vert_mirror_en  = false,		// 垂直镜像使能

        .adjust = {
            .y_gain = 0x100,
            .u_gain = 0x100,
            .v_gain = 0x100,
            .r_gain = 0x80,
            .g_gain = 0x80,
            .b_gain = 0x80,
            .r_coe0 = 0x80,
            .g_coe1 = 0x80,
            .b_coe2 = 0x80,
            .r_gma  = 100,
            .g_gma  = 100,
            .b_gma  = 100,
        },
    },

    .drive_mode      = MODE_RGB_DE_SYNC,
    .ncycle          = CYCLE_ONE,
    .dclk_set 		 = CLK_EN | CLK_NEGATIVE,
    .sync0_set       = SIGNAL_DEN | CLK_EN, //SIGNAL_DEN;
    .sync1_set       = SIGNAL_HSYNC | CLK_EN | CLK_NEGATIVE,
    .sync2_set       = SIGNAL_VSYNC | CLK_EN | CLK_NEGATIVE,
    .data_width      = PORT_24BITS,
    .port_sel        = PORT_GROUP_AA,

#if (RES == RES_640x480)
    .clk_cfg    	 = PLL2_CLK_480M | DIVA_5 | DIVB_4,
#elif (RES == RES_800x600)
    .clk_cfg    	 = PLL2_CLK_480M | DIVA_3 | DIVB_4,
#elif (RES == RES_1280x720)
    .clk_cfg    	 = PLL2_CLK_480M | DIVA_3 | DIVB_2,
#elif (RES == RES_1920x1080)
    .clk_cfg    	 = PLL2_CLK_480M | DIVA_3 | DIVB_1,
#endif

#if (RES == RES_640x480)
    .timing = {
        .hori_total 			= 800,
        .hori_sync 		        = 96,
        .hori_back_porth 		= 48,
        .hori_pixel 			= 640,

        .vert_total 			= 525,
        .vert_sync 		        = 2,
        .vert_back_porth_odd 	= 33,
        .vert_back_porth_even 	= 33,
        .vert_pixel		 		= 480,
    },
#elif (RES == RES_800x600)
    .timing = {
        .hori_total 			= 1056,
        .hori_sync 		        = 128,
        .hori_back_porth 		= 88,
        .hori_pixel 			= 800,

        .vert_total 			= 628,
        .vert_sync 		        = 4,
        .vert_back_porth_odd 	= 23,
        .vert_back_porth_even 	= 0,
        .vert_pixel		 		= 600,
    },
#elif (RES == RES_1280x720)
    .timing = {
        .hori_total 			= 1688,
        .hori_sync 		        = 112,
        .hori_back_porth 		= 248,
        .hori_pixel 			= 1280,

        .vert_total 			= 768,
        .vert_sync 		        = 6,
        .vert_back_porth_odd 	= 28,
        .vert_back_porth_even 	= 0,
        .vert_pixel		 		= 720,
    },
#elif (RES == RES_1920x1080)
    .timing = {
        .hori_total 			= 2200,
        .hori_sync 		        = 44,
        .hori_back_porth 		= 148,
        .hori_pixel 			= 1920,

        .vert_total 			= 1125,
        .vert_sync 		        = 5,
        .vert_back_porth_odd 	= 36,
        .vert_back_porth_even 	= 0,
        .vert_pixel		 		= 1080,
    },
#endif
},
REGISTER_IMD_DEVICE_END()

REGISTER_LCD_DEVICE_DRIVE(lcd_dev)  = {
    .type   = LCD_DVP_RGB,
    .init   = NULL,
    .dev    = &lcd_vga_dev,
    .bl_ctrl = lcd_vga_backctrl,
};

#endif

