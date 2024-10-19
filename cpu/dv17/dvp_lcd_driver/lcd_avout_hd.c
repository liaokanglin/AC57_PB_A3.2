#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "asm/avo_cfg.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_AVOUT_HD

static void lcd_ahd_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_IMD_DEVICE_BEGIN(lcd_ahd_dev) = {
    .info = {
        .test_mode       = false,
        .test_mode_color = 0x008080,
        .canvas_color    = 0x000000,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres     = LCD_DEV_WIDTH,
        .target_yres     = LCD_DEV_HIGHT,
        .format          = FORMAT_YUV422,
        .interlaced_mode = INTERLACED_NONE,
        .sample          = LCD_DEV_SAMPLE,
        .buf_num         = LCD_DEV_BNUM,
        .buf_addr        = LCD_DEV_BUF,
        .len 			 = LEN_256,

        .rotate_en 		 = false,		// 旋转使能
        .hori_mirror_en  = false,		// 水平镜像使能
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
            //~~~~~~~~~~~~~~~~~~//
            /* .y_gain = 0x100, */
            /* .u_gain = 0x100 + 32, */
            /* .v_gain = 0x100 + 32, */
            /* .y_offs = -2, */
            /* .u_offs = 1, */
            /* .v_offs = 1, */
            /* .r_gain = 0x80, */
            /* .g_gain = 0x80, */
            /* .b_gain = 0x80 + 4, */

            /* .r_coe0 = 0x80 + 8, */
            /* .r_coe1 = 0 - 4, */
            /* .r_coe2 = 0 - 4, */

            /* .g_coe0 = 0 - 4, */
            /* .g_coe1 = 0x80 + 8, */
            /* .g_coe2 = 0 - 4, */

            /* .b_coe0 = 0 + 4, */
            /* .b_coe1 = 0 - 4, */
            /* .b_coe2 = 0x80 + 4, */

            /* .r_gma  = 100, */
            /* .g_gma  = 100, */
            /* .b_gma  = 100, */
        },
    },

    .drive_mode      = MODE_AVOUT,
    .avout_mode 	 = CVE_PAL_1280 /* | CVE_COLOR_BAR */,
    .clk_cfg         = IMD_INTERNAL | PLL4_CLK | DIVA_1 | DIVB_1,
    .pll4 = {
        .pll_freq = 144,
        .source = PLL4_SRC_XOSC1,
    },
    .ncycle          = CYCLE_ONE,

    .dclk_set        = CLK_DIS | CLK_NEGATIVE,
    .sync0_set       = CLK_DIS | SIGNAL_DEN | CLK_NEGATIVE,
    .sync1_set       = CLK_DIS | SIGNAL_HSYNC | CLK_NEGATIVE,
    .sync2_set       = CLK_DIS | SIGNAL_VSYNC | CLK_NEGATIVE,
    .data_width      = PORT_1BIT,
    .timing = {
        .hori_total 			= 864 * 2,
        .hori_sync 	        	= 138 * 2,
        .hori_back_porth 		= 138 * 2,
        .hori_pixel 			= 1280,

        .vert_total 			= 313,
        .vert_sync 		        = 21,
        .vert_back_porth_odd 	= 23,
        .vert_back_porth_even 	= 24,
        .vert_pixel 			= 720,
    },
},
REGISTER_IMD_DEVICE_END()

static int lcd_avout_hd_init(void *_data)
{
    printf("lcd_avout_hd_init.\n");
    return 0;
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .logo = "lcd_avout_hd",
    .init = lcd_avout_hd_init,
    .dev  = (void *) &lcd_ahd_dev,
    .type = LCD_DVP_RGB,
    .bl_ctrl = lcd_ahd_backctrl,
};
#endif
