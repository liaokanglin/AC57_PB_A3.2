#include "generic/typedef.h"
#include "asm/cpu.h"
#include "asm/dsi.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_DSI_DCS_2LANE_24BIT
//------------------------------------------------------//
// lcd command initial
//------------------------------------------------------//
const static u8 init_cmd_list[] = {
    _W, DELAY(1), PACKET_DCS, SIZE(4),  0xB9, 0xFF, 0x83, 0x79,
    _W, DELAY(1), PACKET_DCS, SIZE(21), 0xB1, 0x44, 0x12, 0x12, 0x31, 0x31, 0x50, 0xd0, 0xee, 0x54, 0x80, 0x38, 0x38, 0xf8, 0x33, 0x32, 0x22, 0x00, 0x80, 0x30, 0x00,
    _W, DELAY(1), PACKET_DCS, SIZE(10), 0xB2, 0x80, 0xFE, 0x0A, 0x04, 0x00, 0x50, 0x11, 0x42, 0x1D,
    _W, DELAY(1), PACKET_DCS, SIZE(11), 0xB4, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x22, 0x80, 0x23, 0x80,
    _W, DELAY(1), PACKET_DCS, SIZE(2),  0xCC, 0x02,
    _W, DELAY(1), PACKET_DCS, SIZE(2),  0xD2, 0x11,
    _W, DELAY(1), PACKET_DCS, SIZE(30), 0xD3, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x10, 0x03, 0x00, 0x03, 0x03, 0x60, 0x03, 0x60, 0x00, 0x08, 0x00, 0x08, 0x45, 0x44, 0x08, 0x08, 0x37, 0x08, 0x08, 0x37, 0x09,
    _W, DELAY(1), PACKET_DCS, SIZE(33), 0xD5, 0x18, 0x18, 0x19, 0x19, 0x18, 0x18, 0x20, 0x21, 0x24, 0x25, 0x18, 0x18, 0x18, 0x18, 0x00, 0x01, 0x04, 0x05, 0x02, 0x03, 0x06, 0x07, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    _W, DELAY(1), PACKET_DCS, SIZE(33), 0xD6, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x25, 0x24, 0x21, 0x20, 0x18, 0x18, 0x18, 0x18, 0x05, 0x04, 0x01, 0x00, 0x03, 0x02, 0x07, 0x06, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    _W, DELAY(1), PACKET_DCS, SIZE(43), 0xE0, 0x00, 0x00, 0x00, 0x06, 0x08, 0x3f, 0x11, 0x29, 0x05, 0x09, 0x0c, 0x17, 0x10, 0x14, 0x16, 0x15, 0x15, 0x08, 0x13, 0x14, 0x18, 0x00, 0x00, 0x00, 0x06, 0x08, 0x3f, 0x11, 0x29, 0x05, 0x09, 0x0c, 0x17, 0x10, 0x14, 0x16, 0x15, 0x15, 0x08, 0x13, 0x14, 0x18,
    _W, DELAY(1), PACKET_DCS, SIZE(3),  0xB6, 0x42, 0x42,
    _W, DELAY(1), PACKET_DCS, SIZE(2),  0x35, 0x00,
    _W, DELAY(1), PACKET_DCS, SIZE(2),  0x51, 0xFF,
    _W, DELAY(1), PACKET_DCS, SIZE(2),  0x53, 0x24,
    _W, DELAY(1), PACKET_DCS, SIZE(2),  0x55, 0x01,
    /* _W,DELAY(1),PACKET_DCS, SIZE(1), 0x3A,0x70, */
    /* _W,DELAY(1),PACKET_DCS, SIZE(1), 0x26,0x01, */
    _W, DELAY(20), PACKET_DCS, SIZE(1), 0x11,
    _W, DELAY(4), PACKET_DCS, SIZE(1),  0x29,
};

#define freq 321

#define lane_num 2
#define bpp_num  24

//垂直时序要求比较严
#define vsa_line 1
#define vbp_line 10
#define vda_line 854
#define vfp_line 16

#define hsa_pixel  8
#define hbp_pixel  16
#define hda_pixel  480
#define hfp_pixel  16

REGISTER_MIPI_DEVICE_BEGIN(mipi_dev_t) = {
    .info = {
        .xres 			= LCD_DEV_WIDTH,
        .yres 			= LCD_DEV_HIGHT,
        .target_xres 	 = LCD_DEV_WIDTH,
        .target_yres 	 = LCD_DEV_HIGHT,
        .buf_addr 		= LCD_DEV_BUF,
        .buf_num 		= LCD_DEV_BNUM,
        .sample         = LCD_DEV_SAMPLE,
        .test_mode 		= false,
        .test_mode_color = 0x0000ff,
        .canvas_color = 0x000000,
        .format 		= FORMAT_RGB888,
        .len 			= LEN_256,

        .rotate_en 		= true,			// 旋转使能
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
        .x0_lane = MIPI_LANE_EN | MIPI_LANE_D0,
        .x1_lane = MIPI_LANE_EN | MIPI_LANE_D1,
        .x2_lane = MIPI_LANE_EN | MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_DIS,
        .x4_lane = MIPI_LANE_DIS,
    },
    .video_timing = {
        .video_mode = VIDEO_STREAM_VIDEO,//视频模式 类似于dump panel
        .sync_mode  = SYNC_PULSE_MODE,
        .color_mode = COLOR_FORMAT_RGB888,
        .pixel_type = PIXEL_RGB888,
        .virtual_ch   = 0,
        .hs_eotp_en = false,

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
        .pll_freq = 321,
        .source = PLL4_SRC_x12M,
    },

    .cmd_list = init_cmd_list,
    .cmd_list_item = sizeof(init_cmd_list),
    .debug_mode = false,
},
REGISTER_MIPI_DEVICE_END()

//------------------------------------------------------//
// dsi run
//------------------------------------------------------//
static int dsi_init(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;

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

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type = LCD_MIPI,
    .init = dsi_init,
    .dev  = &mipi_dev_t,
    .bl_ctrl = mipi_backlight_ctrl,
};
#endif
