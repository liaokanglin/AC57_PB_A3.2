#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_480x272_8BITS

static void lcd_480x272_8bits_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}


REGISTER_IMD_DEVICE_BEGIN(lcd_480x272_8bits_dev) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0x0000ff,
        .canvas_color    = 0xFF0000,

        .x				 = 0,
        .y				 = 0,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres     = LCD_DEV_WIDTH,
        .target_yres     = LCD_DEV_HIGHT,

        .sample          = LCD_DEV_SAMPLE,
        .format          = FORMAT_RGB888,

        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .interlaced_1st_filed = EVEN_FILED,

        .interlaced_mode = INTERLACED_NONE,
        .len 			 = LEN_256,

        .rotate_en		 = false,
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

    .dclk_set 		= CLK_EN,
    .sync0_set      = SIGNAL_HSYNC | CLK_EN,
    .sync1_set      = SIGNAL_VSYNC | CLK_EN,
    .sync2_set      = SIGNAL_DEN | CLK_EN,

    .ncycle         = CYCLE_THREE,
    .drive_mode     = MODE_RGB_SYNC,
    .port_sel       = PORT_GROUP_AA,
    .data_width     = PORT_8BITS,
    .clk_cfg    	= PLL2_CLK_480M | DIVA_7 | DIVB_2,

    .timing = {
        .hori_total 			= 625 * 3, //60Hz
        .hori_sync 				= 1 * 3,
        .hori_back_porth 		= 40 * 3,
        .hori_pixel 			= 480,

        .vert_total 			= 288,
        .vert_sync 				= 1,
        .vert_back_porth_odd 	= 8,
        .vert_back_porth_even 	= 0,
        .vert_pixel		 		= 272,
    },
},
REGISTER_IMD_DEVICE_END()

static int lcd_480x272_8bits_init(void *_data)
{
    printf("lcd 480x272 8bits init ...\n");
    return 0;
}

REGISTER_LCD_DEVICE_DRIVE(lcd_dev_8bits)  = {
    .logo   = "lcd_480x272_8bits",
    .type		= LCD_DVP_RGB,
    .init		= lcd_480x272_8bits_init,
    .dev    	= &lcd_480x272_8bits_dev,
    .bl_ctrl	= lcd_480x272_8bits_backctrl,
};


#endif
