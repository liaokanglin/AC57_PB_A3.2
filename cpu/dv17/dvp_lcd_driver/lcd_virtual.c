#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"


#ifdef LCD_VIRTUAL

/* #define DIRECTION_HORI */

static void lcd_virtual_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}


REGISTER_IMD_DEVICE_BEGIN(lcd_virtual_dev) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0xFF0000,
        .canvas_color = 0xFF0000,
        .x               = 0,
        .y               = 0,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres 	 = LCD_DEV_WIDTH,
        .target_yres 	 = LCD_DEV_HIGHT,
        .sw_mode         = false,
        .sw_x            = 0,
        .sw_y            = 0,
        .sw_xres         = LCD_DEV_WIDTH,
        .sw_yres         = LCD_DEV_HIGHT,
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .len 			 = LEN_256,
        .sample          = LCD_DEV_SAMPLE,
        .format          = FORMAT_RGB888,
        .interlaced_mode = INTERLACED_NONE,

#ifdef DIRECTION_HORI
        .rotate_en       = false,	    // 旋转使能
        .hori_mirror_en  = false,	    // 水平镜像使能
        .vert_mirror_en  = false,		// 垂直镜像使能
#else
        .rotate_en       = true,	    // 旋转使能
        .hori_mirror_en  = true,	    // 水平镜像使能
        .vert_mirror_en  = false,		// 垂直镜像使能
#endif

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
    .dclk_set 		 = CLK_EN,
    .sync0_set       = SIGNAL_DEN | CLK_EN, //SIGNAL_DEN;
    .sync1_set       = SIGNAL_HSYNC | CLK_EN | CLK_NEGATIVE,
    .sync2_set       = SIGNAL_VSYNC | CLK_EN | CLK_NEGATIVE,
    .data_width      = PORT_24BITS,
    .port_sel        = PORT_GROUP_AA,
    .clk_cfg    	 = PLL2_CLK | DIVA_3 | DIVB_2 | DIVC_4,

    .timing = {
        .hori_total 		    = 625,
        .hori_sync 		        = 1,
        .hori_back_porth 	    = 40,
        .hori_pixel 	        = 480,

        .vert_total 		    = 288,
        .vert_sync 		        = 1,
        .vert_back_porth_odd 	= 8,
        .vert_back_porth_even 	= 0,
        .vert_pixel 	        = 272,
    },
},
REGISTER_IMD_DEVICE_END()


REGISTER_LCD_DEVICE_DRIVE(lcd_dev)  = {
    .type   = LCD_DVP_RGB,
    .init   = NULL,
    .dev    = &lcd_virtual_dev,
    .bl_ctrl = lcd_virtual_backctrl,
};

#endif

