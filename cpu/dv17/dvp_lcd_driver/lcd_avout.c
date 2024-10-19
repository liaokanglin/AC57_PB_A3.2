#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "asm/avo_cfg.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_AVOUT

static void lcd_avout_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_IMD_DEVICE_BEGIN(lcd_avout_dev) = {
    .info = {
        .test_mode       = false,
        .test_mode_color = 0xffff00,
        .canvas_color    = 0x000000,
#if (AVOUT_MODE == NTSC_720)
        .xres 			 = 720,
        .yres 			 = 480,
        .target_xres 	 = 720,
        .target_yres 	 = 480,
#elif (AVOUT_MODE == NTSC_960)
        .xres 			 = 960,
        .yres 			 = 480,
        .target_xres 	 = 960,
        .target_yres 	 = 480,
#elif (AVOUT_MODE == PAL_720)
        .xres 			 = 720,
        .yres 			 = 576,
        .target_xres 	 = 720,
        .target_yres 	 = 576,
#elif (AVOUT_MODE == PAL_960)
        .xres 			 = 960,
        .yres 			 = 576,
        .target_xres 	 = 960,
        .target_yres 	 = 576,
#endif
        .buf_addr 		 = LCD_DEV_BUF,
        .buf_num  		 = LCD_DEV_BNUM,
        .sample          = LCD_DEV_SAMPLE,
        .format          = FORMAT_YUV422,
        .interlaced_mode = INTERLACED_ALL,
        .len 			 = LEN_256,
#if (AVOUT_MODE == NTSC_720) || (AVOUT_MODE == NTSC_960)
        .interlaced_1st_filed = ODD_FILED,
#elif (AVOUT_MODE == PAL_720) || (AVOUT_MODE == PAL_960)
        .interlaced_1st_filed = EVEN_FILED,
#endif

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
        },
    },

    .drive_mode      = MODE_AVOUT,
#if (AVOUT_MODE == NTSC_720)
    .avout_mode 	 = CVE_NTSC_720/*  | CVE_COLOR_BAR */,
    .clk_cfg    	 = IMD_INTERNAL | PLL4_CLK | DIVA_5 | DIVB_1,
    .pll4 = {
        .pll_freq = 540,
        .source = PLL4_SRC_x12M,
    },
#elif (AVOUT_MODE == NTSC_960)
    .avout_mode 	 = CVE_NTSC_960/*  | CVE_COLOR_BAR */,
    .clk_cfg    	 = IMD_INTERNAL | PLL4_CLK | DIVA_3 | DIVB_1,
    .pll4 = {
        .pll_freq = 432,
        .source = PLL4_SRC_x12M,
    },
#elif (AVOUT_MODE == PAL_720)
    .avout_mode 	 = CVE_PAL_720/*  | CVE_COLOR_BAR */,
    .clk_cfg    	 = IMD_INTERNAL | PLL4_CLK | DIVA_5 | DIVB_1,
    .pll4 = {
        .pll_freq = 540,
        .source = PLL4_SRC_x12M,
        /* .source = PLL4_SRC_XOSC1, */
    },
#elif (AVOUT_MODE == PAL_960)
    .avout_mode 	 = CVE_PAL_960 /* | CVE_COLOR_BAR */,
    .clk_cfg    	 = IMD_INTERNAL | PLL4_CLK | DIVA_3 | DIVB_1,
    .pll4 = {
        .pll_freq = 432,
        .source = PLL4_SRC_x12M,
    },
#endif
    .ncycle          = CYCLE_ONE,

    .dclk_set        = CLK_DIS | CLK_NEGATIVE,
    .sync0_set       = CLK_DIS | SIGNAL_DEN | CLK_NEGATIVE,
    .sync1_set       = CLK_DIS | SIGNAL_HSYNC | CLK_NEGATIVE,
    .sync2_set       = CLK_DIS | SIGNAL_VSYNC | CLK_NEGATIVE,

    .data_width      = PORT_1BIT,

    .timing = {
#if (AVOUT_MODE == NTSC_720)
        //NTSC 27MHz 720x480
        .hori_total 		    = 858 * 2,
        .hori_sync 		        = 138 * 2,
        .hori_back_porth 	    = 138 * 2,
        .hori_pixel 	        = 720,

        .vert_total 		    = 263,
        .vert_sync 		        = 21,
        .vert_back_porth_odd 	= 21,
        .vert_back_porth_even 	= 22,
        .vert_pixel 	        = 240,
#elif (AVOUT_MODE == NTSC_960)
        //NTSC 36MHz 960x480
        .hori_total		 		= 1144 * 2,
        .hori_sync 				= 182 * 2,
        .hori_back_porth	 	= 182 * 2,
        .hori_pixel 			= 960,

        .vert_total 			= 263,
        .vert_sync 				= 21,
        .vert_back_porth_odd 	= 21,
        .vert_back_porth_even 	= 22,
        .vert_pixel 			= 240,
#elif (AVOUT_MODE == PAL_720)
        //PAL 27MHz 720x576
        .hori_total 		    = 864 * 2,
        .hori_sync 		        = 138 * 2,
        .hori_back_porth 	    = 138 * 2,
        .hori_pixel 	        = 720,

        .vert_total 		    = 313,
        .vert_sync 		        = 21,
        .vert_back_porth_odd 	= 23,
        .vert_back_porth_even 	= 24,
        .vert_pixel 	        = 288,
#elif (AVOUT_MODE == PAL_960)
        //PAL 36MHz 960x576
        .hori_total 		    = 1152 * 2,
        .hori_sync 	            = 192 * 2,
        .hori_back_porth 	    = 192 * 2,
        .hori_pixel 		    = 960,

        .vert_total 		    = 313,
        .vert_sync 	            = 21,
        .vert_back_porth_odd    = 23,
        .vert_back_porth_even   = 24,
        .vert_pixel 		    = 288,
#endif
    },
},
REGISTER_IMD_DEVICE_END()

static int lcd_avout_init(void *_data)
{
    printf("lcd_avout_init.\n");
    return 0;
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .logo = "lcd_avout",
    .init = lcd_avout_init,
    .dev  = &lcd_avout_dev,
    .type = LCD_DVP_RGB,
    .bl_ctrl = lcd_avout_backctrl,
};
#endif
