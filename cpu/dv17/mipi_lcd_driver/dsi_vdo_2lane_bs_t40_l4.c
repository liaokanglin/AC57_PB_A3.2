#include "generic/typedef.h"
#include "asm/dsi.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"
#include "asm/dsi.h"
#include "asm/port_waked_up.h"


#ifdef LCD_DSI_DCS_2LANE_24BIT_BS_T40_L4
//------------------------------------------------------//
// lcd command initial
//------------------------------------------------------//
const static u8 init_cmd_list[] = {
    _W, DELAY(0), PACKET_GENERIC, SIZE(2), 0xb0, 0x04,
    _W, DELAY(0), PACKET_GENERIC, SIZE(2), 0xd6, 0x28,

//20ms
    _W, DELAY(0), PACKET_GENERIC, SIZE(16), 0xc1, 0x63, 0x31, 0x00, 0x27, 0x27, 0x32, 0x12, 0x28,  0xde/* 0xc2~0xC8*/, 0x9e,/*0x82~0x88*//*0x46,0x10,*/ 0xa5, 0x0f, 0x58, 0x21, 0x01,
    _W, DELAY(0), PACKET_GENERIC, SIZE(2), 0xFA, 0x03,


    _W, DELAY(0), PACKET_DCS, SIZE(5), 0x2A, 0x00, 0x00, 0x01, 0xDF,
    _W, DELAY(0), PACKET_DCS, SIZE(5), 0x2B, 0x00, 0x00, 0x03, 0x1f,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x3A, 0x77,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x36, 0x11,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x35, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(3), 0x44, 0x00, 0x00,
    _W, DELAY(120), PACKET_DCS, SIZE(1), 0x11,
    _W, DELAY(50), PACKET_DCS, SIZE(1), 0x29,
};

/* #define freq 321*///49fps-->需要Te 1000/49 = 20.4
#define freq 348    //53fps-->需要Te  1000/53 = 18.8


#define lane_num 2

#define bpp_num  24

//垂直时序要求比较严
#define vsa_line 4//1
#define vbp_line 8//23
#define vda_line 800
#define vfp_line 6

#define hsa_pixel  80
#define hbp_pixel  70
#define hda_pixel  480
#define hfp_pixel  120

REGISTER_MIPI_DEVICE_BEGIN(mipi_dev_t) = {
    .info = {
        .xres 			= LCD_DEV_WIDTH,
        .yres 			= LCD_DEV_HIGHT,
        .target_xres 	= LCD_DEV_WIDTH,
        .target_yres 	= LCD_DEV_HIGHT,
        .buf_addr 		= LCD_DEV_BUF,
        .buf_num 		= LCD_DEV_BNUM,
        .sample         = LCD_DEV_SAMPLE,
        .test_mode 		= false,
        .test_mode_color = 0x0000ff,
        .canvas_color   = 0x000000,
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
        .x0_lane = MIPI_LANE_EN | MIPI_LANE_CLK,
        .x1_lane = MIPI_LANE_EN | MIPI_LANE_D0,
        .x2_lane = MIPI_LANE_EN | MIPI_LANE_D1,
        .x3_lane = MIPI_LANE_DIS,
        .x4_lane = MIPI_LANE_DIS,

        /*      .x0_lane = MIPI_LANE_DIS,//MIPI_LANE_EN | MIPI_LANE_CLK, */
        /* .x1_lane = MIPI_LANE_DIS, */
        /* .x2_lane = MIPI_LANE_EN | MIPI_LANE_CLK, */
        /* .x3_lane = MIPI_LANE_EN | MIPI_LANE_D1, */
        /* .x4_lane = MIPI_LANE_EN | MIPI_LANE_D0, */


    },
    .video_timing = {

        .video_mode = VIDEO_STREAM_COMMAND,//视频模式 类似于dump panel
        .sync_mode = SYNC_PULSE_MODE,//同步事件
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
    .te_mode = {
        .te_mode_en = true,
        /* .te_mode_en = false, */
        .event = EVENT_IO_2,
        .edge = EDGE_POSITIVE,
        /* .edge = EDGE_NEGATIVE, */
        .gpio = IO_PORTG_03,
    },
    .pll4 = {
        .pll_freq = freq,
        .source = PLL4_SRC_x12M,
    },

    .cmd_list = init_cmd_list,
    .cmd_list_item = sizeof(init_cmd_list),
    .debug_mode = false,
},
REGISTER_MIPI_DEVICE_END()

static int dsi_vdo_init(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;

    /*
     * lcd reset
     */
    if ((u8) - 1 != lcd_reset) {
        gpio_direction_output(lcd_reset, 0);
        delay_2ms(100);
        gpio_direction_output(lcd_reset, 1);
        delay_2ms(100);
    }


    return 0;
}
#include "os/os_api.h"
static void mipi_backlight_ctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if ((u8) - 1 == data->lcd_io.backlight) {
        return;
    }
    /* os_time_dly(50); */
    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type 	 = LCD_MIPI,
    .dev  	 = &mipi_dev_t,
    .init 	 = dsi_vdo_init,
    .bl_ctrl = mipi_backlight_ctrl,
    .bl_ctrl_flags = BL_CTRL_BACKLIGHT,
    /* .te_mode_dbug = true, */
};


#endif

u32  set_retry_cnt()
{
    return 20;
}
