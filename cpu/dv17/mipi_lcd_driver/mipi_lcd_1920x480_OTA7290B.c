#include "generic/typedef.h"
#include "asm/dsi.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"
#include "os/os_compat.h"

#ifdef LCD_1920x480_OTA7290B
//------------------------------------------------------//
// lcd command initial
//------------------------------------------------------//
//command list


#define freq (400)

#define lane_num 4
/*
 *  bpp_num
 *  16: PIXEL_RGB565_COMMAND/PIXEL_RGB565_VIDEO
 *  18: PIXEL_RGB666/PIXEL_RGB666_LOOSELY
 *  24: PIXEL_RGB888
 */
#define bpp_num  24
#if 0
#define vsa_line 2
#define vbp_line 40
#define vda_line 1920
#define vfp_line 40

#define hsa_pixel  8
#define hbp_pixel  42
#define hda_pixel  480
#define hfp_pixel  44
#elif 0
#define vsa_line 2
#define vbp_line 40
#define vda_line 1920
#define vfp_line 100

#define hsa_pixel  8
#define hbp_pixel  (42+40)
#define hda_pixel  480
#define hfp_pixel  (44+240)
#else

#define vsa_line 4
#define vbp_line 56
#define vda_line 480
#define vfp_line 40

#define hsa_pixel  4
#define hbp_pixel  56
#define hda_pixel  1920
#define hfp_pixel  70

#endif
REGISTER_MIPI_DEVICE_BEGIN(mipi_dev_t) = {
    .info = {
        .xres           = LCD_DEV_WIDTH,
        .yres           = LCD_DEV_HIGHT,
        .target_xres 	= LCD_DEV_WIDTH,
        .target_yres 	= LCD_DEV_HIGHT,
        .buf_addr       = LCD_DEV_BUF,
        .buf_num        = LCD_DEV_BNUM,
        .sample         = LCD_DEV_SAMPLE,
        .test_mode      = false,
        .test_mode_color = 0xff0000,
        .canvas_color   = 0x0000ff,
        .format         = FORMAT_RGB888,
        .len            = LEN_256,
        .interlaced_mode = INTERLACED_NONE,

        .rotate_en      = false,             // 旋转使能
        .hori_mirror_en = false,             // 水平镜像使能
        .vert_mirror_en = false,            // 垂直镜像使能

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
        .x0_lane = MIPI_LANE_EN  | MIPI_LANE_D3,
        .x1_lane = MIPI_LANE_EN  | MIPI_LANE_D2,
        .x2_lane = MIPI_LANE_EN  | MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_EN  | MIPI_LANE_D1,
        .x4_lane = MIPI_LANE_EN  | MIPI_LANE_D0,
    },
    .video_timing = {
        .video_mode = VIDEO_STREAM_VIDEO,
        .sync_mode  = SYNC_PULSE_MODE,
        .color_mode = COLOR_FORMAT_RGB888,
        .pixel_type = PIXEL_RGB888,
        .virtual_ch = 0,
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
        /* 以下参数只需修改freq */
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
        .pll_freq = freq,
        .source = PLL4_SRC_x12M,
    },
    .cmd_list = NULL,
    .cmd_list_item = 0,
    .debug_mode = false,
},
REGISTER_MIPI_DEVICE_END()

static int dsi_vdo_ek79030_init(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;
    u8 lcd_standby  = data->lcd_io.lcd_standby;

    printf("dsi_vdo_ek79030_init...\n");

    if ((u8) - 1 != lcd_standby) { /*4 lane*/
        printf("lcd_standby = %d\n", lcd_standby);
        gpio_direction_output(lcd_standby, 0);
    }

    /*
     * lcd reset
     */

    if ((u8) - 1 != lcd_reset) {
        gpio_direction_output(lcd_reset, 0);
        delay_2ms(5);
        gpio_direction_output(lcd_reset, 1);
        delay_2ms(5);
    }

    return 0;
}
static void mipi_backlight_ctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;


    if ((u8) - 1 == data->lcd_io.backlight) {
        return;
    }
    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type      = LCD_MIPI,
    .dev       = &mipi_dev_t,
    .init      = dsi_vdo_ek79030_init,
    .bl_ctrl   = mipi_backlight_ctrl,
    .esd = {
        /* .interval = 1000, */
        /* .esd_check_isr = ek79030_esd_check, */
    }
};

#endif
