#include "generic/typedef.h"
#include "asm/dsi.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"
#ifdef LCD_DSI_VDO_4LANE_MIPI

#define freq 300

#define bpp_num  24

#define vsa_line 20
#define vbp_line 30
#define vda_line 1280
#define vfp_line 30

#define hsa_pixel  30
#define hbp_pixel  40
#define hda_pixel  480
#define hfp_pixel  40

REGISTER_MIPI_DEVICE_BEGIN(mipi_dev_t) = {
    .info = {
        .xres = LCD_DEV_WIDTH,
        .yres = LCD_DEV_HIGHT,
        .target_xres 	 = LCD_DEV_WIDTH,
        .target_yres 	 = LCD_DEV_HIGHT,
        .buf_addr = LCD_DEV_BUF,
        .buf_num = LCD_DEV_BNUM,
        .sample = LCD_DEV_SAMPLE,
        .test_mode = false,//测试模式
        .test_mode_color = 0x0000ff,
        .canvas_color = 0x000000,
        .format = FORMAT_RGB888,

        .rotate_en = true,				// 旋转使能
        .hori_mirror_en = true,			// 水平镜像使能
        .vert_mirror_en = false,		// 垂直镜像使能

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
    .lane_mapping = {
        .x0_lane = MIPI_LANE_EN | MIPI_LANE_D3,
        .x1_lane = MIPI_LANE_EN | MIPI_LANE_D2,
        .x2_lane = MIPI_LANE_EN | MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_EN | MIPI_LANE_D1,
        .x4_lane = MIPI_LANE_EN | MIPI_LANE_D0,
    },
    .video_timing = {
        .video_mode = VIDEO_STREAM_VIDEO,
        .sync_mode  = SYNC_PULSE_MODE,
        .color_mode = COLOR_FORMAT_RGB888,
        .pixel_type = PIXEL_RGB888,
        .virtual_ch	= 0,
        .hs_eotp_en = true,

        .dsi_vdo_vsa_v  = vsa_line,
        .dsi_vdo_vbp_v  = vbp_line,
        .dsi_vdo_vact_v = vda_line,
        .dsi_vdo_vfp_v  = vfp_line,

        .dsi_vdo_hsa_v   = ((bpp_num * hsa_pixel) / 8) - 10,
        .dsi_vdo_hbp_v   = ((bpp_num * hbp_pixel) / 8) - 10,
        .dsi_vdo_hact_v  = ((bpp_num * hda_pixel) / 8),
        .dsi_vdo_hfp_v   = ((bpp_num * hfp_pixel) / 8) - 6,
        .dsi_vdo_bllp0_v = ((bpp_num * (hbp_pixel + hda_pixel + hfp_pixel) / 8) - 10),
        .dsi_vdo_bllp1_v = ((bpp_num * hda_pixel) / 8),
    },
    .timing = {
        .tval_lpx   = ((80     * freq / 1000) / 2 - 1),
        .tval_wkup  = ((100000 * freq / 1000) / 8 - 1),
        .tval_c_pre = ((40     * freq / 1000) / 2 - 1),
        .tval_c_sot = ((300    * freq / 1000) / 2 - 1),
        .tval_c_eot = ((100    * freq / 1000) / 2 - 1),
        .tval_c_brk = ((150    * freq / 1000) / 2 - 1),
        .tval_d_pre = ((60     * freq / 1000) / 2 - 1),
        .tval_d_sot = ((160    * freq / 1000) / 2 - 1),
        .tval_d_eot = ((100    * freq / 1000) / 2 - 1),
        .tval_d_brk = ((150    * freq / 1000) / 2 - 1),
        .tval_c_rdy = 400/* 64 */,
    },
    .pll4 = {
        .pll_freq = 300,
        .source = PLL4_SRC_x12M,
    },

    .cmd_list = NULL,
    .cmd_list_item = 0,
    .debug_mode = false,
},
REGISTER_MIPI_DEVICE_END()

static int dsi_vdo_mipi_init(void *_data)
{
    printf("dsi_vdo_mipi init...\n");
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u32 lcd_reset = data->lcd_io.lcd_reset;
    printf("lcd_reset : %d\n", lcd_reset);
    //reset pin
    gpio_direction_output(lcd_reset, 0);
    delay(0xffff);
    gpio_direction_output(lcd_reset, 1);
    delay(0xffff);
    delay(0xffff);
    delay(0xffff);
    return 0;
}

static void mipi_backlight_ctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type = LCD_MIPI,
    .init = dsi_vdo_mipi_init,
    .bl_ctrl = mipi_backlight_ctrl,
    .dev  = &mipi_dev_t,
};

#endif
