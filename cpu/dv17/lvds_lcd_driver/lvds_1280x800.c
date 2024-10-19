#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lvds.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_LVDS_1280x800

static void lcd_lvds_1280x800_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_LVDS_DEVICE_BEGIN(lvds_1280x800_dev_t) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0x00ff00,
        .canvas_color    = 0xff0000,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres 	 = LCD_DEV_WIDTH,
        .target_yres 	 = LCD_DEV_HIGHT,
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .len 			 = LEN_256,
        .sample          = LCD_DEV_SAMPLE,
        .format          = FORMAT_RGB888,

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
    .mapping = {
        .x0_lane = LVDS_LANE_D0,
        .x1_lane = LVDS_LANE_D1,
        .x2_lane = LVDS_LANE_D2,
        .x3_lane = LVDS_LANE_CLK,
        .x4_lane = LVDS_LANE_D3,
        .swap_dp_dn = false;
    },

    .frame_mode   = LVDS_MODE_LOOP,
    .singal_polar = LVDS_POLAR_DEFAULT,
    .unit         = LVDS_UNIT_LINE,
    .clk_phase    = 2,
    .interface    = LVDS_SINGAL_8BIT,
    .data_format  = LVDS_DATA_VSEA,
    .pll4 = {
        .pll_freq = 350,
        .source = PLL4_SRC_x12M,
    },
    .lvds_div     = LVDS_PLL_DIVB_1 | LVDS_PLL_DIVA_1,

    .timing = {
        .hori_total 	 = 1344,
        .hori_sync 		 = 10,
        .hori_back_porth = 100,
        .hori_pixel 	 = 1024,

        .vert_total 	 = 635,
        .vert_sync 		 = 10,
        .vert_back_porth = 15,
        .vert_pixel 	 = 600,
    },
},
REGISTER_LVDS_DEVICE_END()

static int lcd_lvds_1280x800_init(void *_data)
{
    printf("lvds 1280x800 init.\n");
    return 0;
}

REGISTER_LCD_DEVICE_DRIVE(lcd_dev)  = {
    .type    = LCD_LVDS,
    .init    = lcd_lvds_1280x800_init,
    .dev     = &lvds_1280x800_dev_t,
    .bl_ctrl = lcd_lvds_1280x800_backctrl,
};

#endif

