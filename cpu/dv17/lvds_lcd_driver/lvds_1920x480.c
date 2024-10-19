#include "generic/typedef.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lvds.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_LVDS_1920x480

static void lcd_lvds_1920x480_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_LVDS_DEVICE_BEGIN(lvds_1920x480_dev_t) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0xff0000,
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
#if (defined CONFIG_BOARD_AC5701L_LVDS_TP2850_20190525)
    .mapping = {
        .x0_lane = LVDS_LANE_D0,
        .x1_lane = LVDS_LANE_D1,
        .x2_lane = LVDS_LANE_D2,
        .x3_lane = LVDS_LANE_CLK,
        .x4_lane = LVDS_LANE_D3,
        .swap_dp_dn = false,
    },
#elif (defined CONFIG_BOARD_TEST_DDR1)
    .mapping = {
        .x0_lane = LVDS_LANE_D3,
        .x1_lane = LVDS_LANE_CLK,
        .x2_lane = LVDS_LANE_D2,
        .x3_lane = LVDS_LANE_D1,
        .x4_lane = LVDS_LANE_D0,
        .swap_dp_dn = true,
    },
#endif

    .frame_mode   = LVDS_MODE_LOOP,
    .singal_polar = LVDS_POLAR_DEFAULT,
    .unit         = LVDS_UNIT_LINE,
    .clk_phase    = 2,
    .interface    = LVDS_SINGAL_8BIT,
    .data_format  = LVDS_DATA_VSEA,
    .pll4 = {
        /* .pll_freq = 1150, */
        .pll_freq = 500,
        .source = PLL4_SRC_x12M,
    },
    .lvds_div     = LVDS_PLL_DIVB_1 | LVDS_PLL_DIVA_1,

    .timing = {
        .hori_total 	 = 2050,
        .hori_sync 		 = 4,
        .hori_back_porth = 60,
        .hori_pixel 	 = 1920,

        .vert_total 	 = 580,
        .vert_sync 		 = 4,
        .vert_back_porth = 60,
        .vert_pixel 	 = 480,
    },
},
REGISTER_LVDS_DEVICE_END()

static int lcd_lvds_1920x480_init(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;

    printf("lvds 1920x480 init...\n");

#if (defined CONFIG_BOARD_AC5701L_LVDS_TP2850_20190525)
    PORTG_DIR |= (BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8) | BIT(7) | BIT(6));
    PORTG_DIE &= ~(BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8) | BIT(7) | BIT(6));
    PORTG_PU  &= ~(BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8) | BIT(7) | BIT(6));
    PORTG_PD  &= ~(BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8) | BIT(7) | BIT(6));
#endif

    /*
     * lcd reset
     */
    if (-1 != lcd_reset) {
        gpio_direction_input(lcd_reset);
        gpio_set_pull_up(lcd_reset, 0);
        gpio_set_pull_down(lcd_reset, 1);
        delay(500);

        gpio_direction_input(lcd_reset);
        gpio_set_pull_up(lcd_reset, 1);
        gpio_set_pull_down(lcd_reset, 0);
        delay(500);
    }

    return 0;
}

REGISTER_LCD_DEVICE_DRIVE(lcd_dev)  = {
    .logo    = "lvds_1920x480",
    .type    = LCD_LVDS,
    .init    = lcd_lvds_1920x480_init,
    .dev     = &lvds_1920x480_dev_t,
    .bl_ctrl = lcd_lvds_1920x480_backctrl,
};

#endif


