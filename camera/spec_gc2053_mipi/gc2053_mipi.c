#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "gc2053_mipi.h"
#include "asm/isp_alg.h"
#include "video/video_ioctl.h"

#include "app_config.h"
static u32 reset_gpios[2] = {-1, -1};
static u32 pwdn_gpios[2] = {-1, -1};
#define PCLK  74.25

#define LINE_LENGTH_CLK     2200
#define FRAME_LENGTH30        1125
#define FRAME_LENGTH25        1338

static u32 cur_again = -1;
static u32 cur_dgain = -1;
static u32 cur_expline = -1;
extern void *spec_GC2053_MIPI_get_ae_params();
extern void *spec_GC2053_mipi_get_awb_params();
extern void *spec_GC2053_mipi_get_iq_params();

extern void spec_GC2053_mipi_ae_ev_init(u32 fps);


typedef struct {
    u8 addr;
    u8 value;
} Sensor_reg_ini;

#if  1//GC2053_MIPI_DVP
#if  0
// 256X144  200fps
Sensor_reg_ini GC2053_MIPI_INI_REG[] = {
//window_size=256*144,mipi 2lane,
//mclk=24mhz,mipi_clock=594mhz,frame_rate=200fps,row_time=27.77us pclk =74.25M
//line_frame_total=180
    /*system*/
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x00,
    0xf2, 0x00, //[1]I2C_open_ena [0]pwd_dn
    0xf3, 0x00, //0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io
    0xf4, 0x36, //[6:4]pll_ldo_set
    0xf5, 0xc0, //[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
    0xf6, 0x44, //[7:3]wpllclk_div [2:0]refmp_div
    0xf7, 0x01, //[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
    0xf8, 0x63, //2c//[7:0]pllmp_div
    0xf9, 0x40, //[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
    0xfc, 0x8e,
    /*cisctl&analog*/
    0xfe, 0x00,
    0x87, 0x18, //[6]aec_delay_mode
    0xee, 0x30, //[5:4]dwen_sramen
    0xd0, 0xb7, //ramp_en
    0x03, 0x04,
    0x04, 0x10,
    0x05, 0x04, //05
    0x06, 0x07, //28//[11:0]hb=1031
    0x07, 0x00,
    0x08, 0x08, //vb=08

    0x09, 0x01,
    0x0a, 0xd4, //cisctl row start  //468
    0x0b, 0x03,
    0x0c, 0x40, //cisctl col start  //832

    0x0d, 0x00,
    0x0e, 0x98, //152
    0x0f, 0x01,
    0x10, 0x08, //264

    0x12, 0xe2, //vsync_ahead_mode
    0x13, 0x16,
//使能镜像
#if 0
    0x17, 0x83,
#endif
    0x19, 0x0a, //ad_pipe_num
    0x21, 0x1c, //eqc1fc_eqc2fc_sw
    0x28, 0x0a, //16//eqc2_c2clpen_sw
    0x29, 0x24, //eq_post_width
    0x2b, 0x04, //c2clpen --eqc2
    0x32, 0xf8, //[5]txh_en ->avdd28
    0x37, 0x03, //[3:2]eqc2sel=0
    0x39, 0x15, //17 //[3:0]rsgl
    0x43, 0x07, //vclamp
    0x44, 0x40, //0e//post_tx_width
    0x46, 0x0b, //txh——3.2v
    0x4b, 0x20, //rst_tx_width
    0x4e, 0x08, //12//ramp_t1_width
    0x55, 0x20, //read_tx_width_pp
    0x66, 0x05, //18//stspd_width_r1
    0x67, 0x05, //40//5//stspd_width_r
    0x77, 0x01, //dacin offset x31
    0x78, 0x00, //dacin offset
    0x7c, 0x93, //[1:0] co1comp
    0x8c, 0x12, //12 ramp_t1_ref

    0x8d, 0x92, //90
    0x90, 0x00,
    0x41, 0x00,
    0x42, 0xb4, //framelength=180
    0x9d, 0x08, //min vb=08

    0xce, 0x7c, //70//78//[4:2]c1isel
    0xd2, 0x41, //[5:3]c2clamp
    0xd3, 0xdc, //ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
    0xe6, 0x50, //ramps offset
    /*gain*/
    0xb6, 0xc0,
    0xb0, 0x70,
    0xb1, 0x01,
    0xb2, 0x00,
    0xb3, 0x00,
    0xb4, 0x00,
    0xb8, 0x01,
    0xb9, 0x00,
    /*blk*/
    0x26, 0x30, //23 //[4]写0，全n mode
    0xfe, 0x01,
    0x40, 0x23,
    0x55, 0x07,
    0x60, 0x00, //[7:0]WB_offset 0x40
    0xfe, 0x04,
    0x14, 0x78, //g1 ratio
    0x15, 0x78, //r ratio
    0x16, 0x78, //b ratio
    0x17, 0x78, //g2 ratio
    /*window*/
#if 1
    0xfe, 0x01,
    0x94, 0x01,
    0x95, 0x00,
    0x96, 0x90, //[10:0]out_height=144
    0x97, 0x01,
    0x98, 0x00, //[11:0]out_width=256
#else
    0xfe, 0x01,
    0x94, 0x00,
    0x95, 0x00,
    0x96, 0x90, //[10:0]out_height=144
    0x97, 0x01,
    0x98, 0x00, //[11:0]out_width=256
#endif
    /*ISP*/
    0xfe, 0x01,
    0x01, 0x05, //03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
    0x02, 0x89, //[7:0]BFF_sram_mode
    0x04, 0x01, //[0]DD_en
    0x07, 0xa6,
    0x08, 0xa9,
    0x09, 0xa8,
    0x0a, 0xa7,
    0x0b, 0xff,
    0x0c, 0xff,
    0x0f, 0x00,
    0x50, 0x1c,
    0x89, 0x03,
    0xfe, 0x04,
    0x28, 0x86,
    0x29, 0x86,
    0x2a, 0x86,
    0x2b, 0x68,
    0x2c, 0x68,
    0x2d, 0x68,
    0x2e, 0x68,
    0x2f, 0x68,
    0x30, 0x4f,
    0x31, 0x68,
    0x32, 0x67,
    0x33, 0x66,
    0x34, 0x66,
    0x35, 0x66,
    0x36, 0x66,
    0x37, 0x66,
    0x38, 0x62,
    0x39, 0x62,
    0x3a, 0x62,
    0x3b, 0x62,
    0x3c, 0x62,
    0x3d, 0x62,
    0x3e, 0x62,
    0x3f, 0x62,
    /*DVP & MIPI*/
    0xfe, 0x01,
    0x9a, 0x06, //[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
    0xfe, 0x00,
    0x7b, 0x2a, //[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
    0x23, 0x2d, //[3]rst_rc [2:1]drv_sync [0]pwd_rc
    0xfe, 0x03,
    0x01, 0x27, //20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
    0x02, 0x56, //[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
    0x03, 0x8e, //b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
    0x12, 0x80,
    0x13, 0x07, //LWC
    0x15, 0x12, //[1:0]clk_lane_mode
    0xfe, 0x00,
    0x3e, 0x91, //40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]

};


#else

// 512X288  100fps
Sensor_reg_ini spec_GC2053_MIPI_INI_REG[] = {
//window_size=512*288,mipi 2lane,
//mclk=24mhz,mipi_clock=594mhz,frame_rate=100fps,row_time=30.30303030us pclk =74.25M
//line_frame_total=330
    /*system*/
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x00,
    0xf2, 0x00, //[1]I2C_open_ena [0]pwd_dn
    0xf3, 0x00, //0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io
    0xf4, 0x36, //[6:4]pll_ldo_set
    0xf5, 0xc0, //[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
    0xf6, 0x44, //[7:3]wpllclk_div [2:0]refmp_div
    0xf7, 0x01, //[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
    0xf8, 0x63, //2c//[7:0]pllmp_div
    0xf9, 0x40, //[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
    0xfc, 0x8e,
    /*cisctl&analog*/
    0xfe, 0x00,
    0x87, 0x18, //[6]aec_delay_mode
    0xee, 0x30, //[5:4]dwen_sramen
    0xd0, 0xb7, //ramp_en
    0x03, 0x04,
    0x04, 0x10,
    0x05, 0x04, //05
    0x06, 0x65, //28//[11:0]hb=1125
    0x07, 0x00,
    0x08, 0x0e, //vb=14

    0x09, 0x01,
    0x0a, 0x8c, //cisctl row start  //396
    0x0b, 0x02,
    0x0c, 0xc0, //cisctl col start  //704

    0x0d, 0x01,
    0x0e, 0x28, //296
    0x0f, 0x02,
    0x10, 0x08, //520

    0x12, 0xe2, //vsync_ahead_mode
    0x13, 0x16,
//使能镜像
#if 0
    0x17, 0x83,
#endif
    0x19, 0x0a, //ad_pipe_num
    0x21, 0x1c, //eqc1fc_eqc2fc_sw
    0x28, 0x0a, //16//eqc2_c2clpen_sw
    0x29, 0x24, //eq_post_width
    0x2b, 0x04, //c2clpen --eqc2
    0x32, 0xf8, //[5]txh_en ->avdd28
    0x37, 0x03, //[3:2]eqc2sel=0
    0x39, 0x15, //17 //[3:0]rsgl
    0x43, 0x07, //vclamp
    0x44, 0x40, //0e//post_tx_width
    0x46, 0x0b, //txh——3.2v
    0x4b, 0x20, //rst_tx_width
    0x4e, 0x08, //12//ramp_t1_width
    0x55, 0x20, //read_tx_width_pp
    0x66, 0x05, //18//stspd_width_r1
    0x67, 0x05, //40//5//stspd_width_r
    0x77, 0x01, //dacin offset x31
    0x78, 0x00, //dacin offset
    0x7c, 0x93, //[1:0] co1comp
    0x8c, 0x12, //12 ramp_t1_ref

    0x8d, 0x92, //90
    0x90, 0x00,
    0x41, 0x01,
    0x42, 0x4a, //framelength=330
    0x9d, 0x0e, //min vb=14

    0xce, 0x7c, //70//78//[4:2]c1isel
    0xd2, 0x41, //[5:3]c2clamp
    0xd3, 0xdc, //ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
    0xe6, 0x50, //ramps offset
    /*gain*/
    0xb6, 0xc0,
    0xb0, 0x70,
    0xb1, 0x01,
    0xb2, 0x00,
    0xb3, 0x00,
    0xb4, 0x00,
    0xb8, 0x01,
    0xb9, 0x00,
    /*blk*/
    0x26, 0x30, //23 //[4]写0，全n mode
    0xfe, 0x01,
    0x40, 0x23,
    0x55, 0x07,
    0x60, 0x00, //[7:0]WB_offset 0x40
    0xfe, 0x04,
    0x14, 0x78, //g1 ratio
    0x15, 0x78, //r ratio
    0x16, 0x78, //b ratio
    0x17, 0x78, //g2 ratio
    /*window*/
#if 1
    0xfe, 0x01,
    0x94, 0x01,
    0x95, 0x01,
    0x96, 0x20, //[10:0]out_height=288
    0x97, 0x02,
    0x98, 0x00, //[11:0]out_width=512
#else
    0xfe, 0x01,
    0x94, 0x00,
    0x95, 0x01,
    0x96, 0x20, //[10:0]out_height=288
    0x97, 0x02,
    0x98, 0x00, //[11:0]out_width=512
#endif
    /*ISP*/
    0xfe, 0x01,
    0x01, 0x05, //03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
    0x02, 0x89, //[7:0]BFF_sram_mode
    0x04, 0x01, //[0]DD_en
    0x07, 0xa6,
    0x08, 0xa9,
    0x09, 0xa8,
    0x0a, 0xa7,
    0x0b, 0xff,
    0x0c, 0xff,
    0x0f, 0x00,
    0x50, 0x1c,
    0x89, 0x03,
    0xfe, 0x04,
    0x28, 0x86,
    0x29, 0x86,
    0x2a, 0x86,
    0x2b, 0x68,
    0x2c, 0x68,
    0x2d, 0x68,
    0x2e, 0x68,
    0x2f, 0x68,
    0x30, 0x4f,
    0x31, 0x68,
    0x32, 0x67,
    0x33, 0x66,
    0x34, 0x66,
    0x35, 0x66,
    0x36, 0x66,
    0x37, 0x66,
    0x38, 0x62,
    0x39, 0x62,
    0x3a, 0x62,
    0x3b, 0x62,
    0x3c, 0x62,
    0x3d, 0x62,
    0x3e, 0x62,
    0x3f, 0x62,
    /*DVP & MIPI*/
    0xfe, 0x01,
    0x9a, 0x06, //[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
    0xfe, 0x00,
    0x7b, 0x2a, //[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
    0x23, 0x2d, //[3]rst_rc [2:1]drv_sync [0]pwd_rc
    0xfe, 0x03,
    0x01, 0x27, //20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
    0x02, 0x56, //[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
    0x03, 0x8e, //b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
    0x12, 0x80,
    0x13, 0x07, //LWC
    0x15, 0x12, //[1:0]clk_lane_mode
    0xfe, 0x00,
    0x3e, 0x91, //40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]

};

#endif
#else

Sensor_reg_ini spec_GC2053_MIPI_INI_REG[] = {
//window_size=1920*1080
//mclk=24mhz,pclk=74.25mhz
//pixel_line_total=2200,line_frame_total=1125
//row_time=29.629us,frame_rate=30fps
    /****system****/
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x00,
    0xf2, 0x00, //[1]I2C_open_ena [0]pwd_dn
    0xf3, 0x0f, //00[3]Sdata_pad_io [2:0]Ssync_pad_io
    0xf4, 0x36, //[6:4]pll_ldo_set
    0xf5, 0xc0, //[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
    0xf6, 0x44, //[7:3]wpllclk_div [2:0]refmp_div
    0xf7, 0x01, //[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
    0xf8, 0x63, //38////38//[7:0]pllmp_div
    0xf9, 0x40, //82//[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
    0xfc, 0x8e,
    /****CISCTL & ANALOG****/
    0xfe, 0x00,
    0x87, 0x18, //[6]aec_delay_mode
    0xee, 0x30, //[5:4]dwen_sramen
    0xd0, 0xb7, //ramp_en
    0x03, 0x04,
    0x04, 0x60,
    0x05, 0x04, //05
    0x06, 0x4c, //60//[11:0]hb
    0x07, 0x00,
    0x08, 0x11, //19
    0x09, 0x00,
    0x0a, 0x02, //cisctl row start
    0x0b, 0x00,
    0x0c, 0x02, //cisctl col start
    0x0d, 0x04,
    0x0e, 0x40, //38
    0x12, 0xe2, //vsync_ahead_mode
    0x13, 0x16,
    0x19, 0x0a, //ad_pipe_num
    0x21, 0x1c, //eqc1fc_eqc2fc_sw
    0x28, 0x0a, //16//eqc2_c2clpen_sw
    0x29, 0x24, //eq_post_width
    0x2b, 0x04, //c2clpen --eqc2
    0x32, 0xf8, //[5]txh_en ->avdd28
    0x37, 0x03, //[3:2]eqc2sel=0
    0x39, 0x15, //17 //[3:0]rsgl
    0x43, 0x07, //vclamp
    0x44, 0x40, //0e//post_tx_width
    0x46, 0x0b,
    0x4b, 0x20, //rst_tx_width
    0x4e, 0x08, //12//ramp_t1_width
    0x55, 0x20, //read_tx_width_pp
    0x66, 0x05, //18//stspd_width_r1
    0x67, 0x05, //40//5//stspd_width_r
    0x77, 0x01, //dacin offset x31
    0x78, 0x00, //dacin offset
    0x7c, 0x93, //[1:0] co1comp
    0x8c, 0x12, //12 ramp_t1_ref
    0x8d, 0x92, //90
    0x90, 0x00, //framerate depend framelength
    0x9d, 0x10,
    0xce, 0x7c, //70//78//[4:2]c1isel
    0xd2, 0x41, //[5:3]c2clamp
    0xd3, 0xdc, //ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
    0xe6, 0x50, //ramps offset
    /*gain*/
    0xb6, 0xc0,
    0xb0, 0x70,
    0xb1, 0x01,
    0xb2, 0x00,
    0xb3, 0x00,
    0xb4, 0x00,
    0xb8, 0x01,
    0xb9, 0x00,
    /*blk*/
    0x26, 0x30, //23 //[4]写0，全n mode
    0xfe, 0x01,
    0x40, 0x23,
    0x55, 0x07,
    0x60, 0x00, //[7:0]WB_offset
    0xfe, 0x04,
    0x14, 0x78, //g1 ratio
    0x15, 0x78, //r ratio
    0x16, 0x78, //b ratio
    0x17, 0x78, //g2 ratio
    /*window*/
    0xfe, 0x01,
    0x92, 0x00, //win y1
    0x94, 0x03, //win x1
    0x95, 0x04,
    0x96, 0x38, //[10:0]out_height
    0x97, 0x07,
    0x98, 0x80, //[11:0]out_width
    /*ISP*/
    0xfe, 0x01,
    0x01, 0x05, //03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
    0x02, 0x89, //[7:0]BFF_sram_mode
    0x04, 0x01, //[0]DD_en
    0x07, 0xa6,
    0x08, 0xa9,
    0x09, 0xa8,
    0x0a, 0xa7,
    0x0b, 0xff,
    0x0c, 0xff,
    0x0f, 0x00,
    0x50, 0x1c,
    0x89, 0x03,
    0xfe, 0x04,
    0x28, 0x86, //84
    0x29, 0x86, //84
    0x2a, 0x86, //84
    0x2b, 0x68, //84
    0x2c, 0x68, //84
    0x2d, 0x68, //84
    0x2e, 0x68, //83
    0x2f, 0x68, //82
    0x30, 0x4f, //82
    0x31, 0x68, //82
    0x32, 0x67, //82
    0x33, 0x66, //82
    0x34, 0x66, //82
    0x35, 0x66, //82
    0x36, 0x66, //64
    0x37, 0x66, //68
    0x38, 0x62,
    0x39, 0x62,
    0x3a, 0x62,
    0x3b, 0x62,
    0x3c, 0x62,
    0x3d, 0x62,
    0x3e, 0x62,
    0x3f, 0x62,
    /****DVP & MIPI****/
    0xfe, 0x01,
    0x9a, 0x06, //[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
    0xfe, 0x00,
    0x7b, 0x2a, //[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
    0x23, 0x2d, //[3]rst_rc [2:1]drv_sync [0]pwd_rc
    0xfe, 0x03,
    0x01, 0x20, //27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
    0x02, 0x56, //[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
    0x03, 0xb2, //b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
    0x12, 0x80,
    0x13, 0x07, //LWC
    0xfe, 0x00,
    0x3e, 0x40, //91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]mipi_set_auto_disable [2]RAW8_mode [1]ine_sync_mode [0]double_lane_en
    0x17, 0x83,
};
#endif





#if   1


Sensor_reg_ini RECOVER_GC2053_MIPI_INI_REG[] = {
//window_size=1928*1080,mipi 2lane,
//mclk=24mhz,mipi_clock=300mhz,frame_rate=30fps,row_time=29.33us pclk =75M
//pixel_line_total=2200,line_frame_total=1125
    /*system*/
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x80,
    0xfe, 0x00,
    0xf2, 0x00, //[1]I2C_open_ena [0]pwd_dn
    0xf3, 0x00, //0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io
    0xf4, 0x36, //[6:4]pll_ldo_set
    0xf5, 0xc0, //[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
    0xf6, 0x44, //[7:3]wpllclk_div [2:0]refmp_div
    0xf7, 0x01, //[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
    0xf8, 0x63, //2c//[7:0]pllmp_div
    0xf9, 0x40, //[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
    0xfc, 0x8e,
    /*cisctl&analog*/
    0xfe, 0x00,
    0x87, 0x18, //[6]aec_delay_mode
    0xee, 0x30, //[5:4]dwen_sramen
    0xd0, 0xb7, //ramp_en
    0x03, 0x04,
    0x04, 0x10,
    0x05, 0x04, //05
    0x06, 0x4c, //28//[11:0]hb
    0x07, 0x00,
    0x08, 0x11,
    0x09, 0x00,
    0x0a, 0x02, //cisctl row start
    0x0b, 0x00,
    0x0c, 0x02, //cisctl col start
    0x12, 0xe2, //vsync_ahead_mode
    0x13, 0x16,
//使能镜像
#if 0
    0x17, 0x83,
#endif
    0x19, 0x0a, //ad_pipe_num
    0x21, 0x1c, //eqc1fc_eqc2fc_sw
    0x28, 0x0a, //16//eqc2_c2clpen_sw
    0x29, 0x24, //eq_post_width
    0x2b, 0x04, //c2clpen --eqc2
    0x32, 0xf8, //[5]txh_en ->avdd28
    0x37, 0x03, //[3:2]eqc2sel=0
    0x39, 0x15, //17 //[3:0]rsgl
    0x43, 0x07, //vclamp
    0x44, 0x40, //0e//post_tx_width
    0x46, 0x0b, //txh——3.2v
    0x4b, 0x20, //rst_tx_width
    0x4e, 0x08, //12//ramp_t1_width
    0x55, 0x20, //read_tx_width_pp
    0x66, 0x05, //18//stspd_width_r1
    0x67, 0x05, //40//5//stspd_width_r
    0x77, 0x01, //dacin offset x31
    0x78, 0x00, //dacin offset
    0x7c, 0x93, //[1:0] co1comp
    0x8c, 0x12, //12 ramp_t1_ref
    0x8d, 0x92, //90
    0x90, 0x00,
    0x41, 0x04,
    0x42, 0x65,

    0x9d, 0x10,
    0xce, 0x7c, //70//78//[4:2]c1isel
    0xd2, 0x41, //[5:3]c2clamp
    0xd3, 0xdc, //ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
    0xe6, 0x50, //ramps offset
    /*gain*/
    0xb6, 0xc0,
    0xb0, 0x70,
    0xb1, 0x01,
    0xb2, 0x00,
    0xb3, 0x00,
    0xb4, 0x00,
    0xb8, 0x01,
    0xb9, 0x00,
    /*blk*/
    0x26, 0x30, //23 //[4]写0，全n mode
    0xfe, 0x01,
    0x40, 0x23,
    0x55, 0x07,
    0x60, 0x00, //[7:0]WB_offset 0x40
    0xfe, 0x04,
    0x14, 0x78, //g1 ratio
    0x15, 0x78, //r ratio
    0x16, 0x78, //b ratio
    0x17, 0x78, //g2 ratio
    /*window*/
#if 1
    0xfe, 0x01,
    0x94, 0x01,
    0x95, 0x04,
    0x96, 0x38, //[10:0]out_height
    0x97, 0x07,
    0x98, 0x80, //[11:0]out_width
#else
    0xfe, 0x01,
    0x94, 0xF0,
    0x95, 0x04,
    0x96, 0x38, //[10:0]out_height
    0x97, 0x05,
    0x98, 0xA0, //[11:0]out_width
#endif
    /*ISP*/
    0xfe, 0x01,
    0x01, 0x05, //03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
    0x02, 0x89, //[7:0]BFF_sram_mode
    0x04, 0x01, //[0]DD_en
    0x07, 0xa6,
    0x08, 0xa9,
    0x09, 0xa8,
    0x0a, 0xa7,
    0x0b, 0xff,
    0x0c, 0xff,
    0x0f, 0x00,
    0x50, 0x1c,
    0x89, 0x03,
    0xfe, 0x04,
    0x28, 0x86,
    0x29, 0x86,
    0x2a, 0x86,
    0x2b, 0x68,
    0x2c, 0x68,
    0x2d, 0x68,
    0x2e, 0x68,
    0x2f, 0x68,
    0x30, 0x4f,
    0x31, 0x68,
    0x32, 0x67,
    0x33, 0x66,
    0x34, 0x66,
    0x35, 0x66,
    0x36, 0x66,
    0x37, 0x66,
    0x38, 0x62,
    0x39, 0x62,
    0x3a, 0x62,
    0x3b, 0x62,
    0x3c, 0x62,
    0x3d, 0x62,
    0x3e, 0x62,
    0x3f, 0x62,
    /*DVP & MIPI*/
    0xfe, 0x01,
    0x9a, 0x06, //[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity

    0xfe, 0x01,
    0x87, 0x50,
    //0xfe,0x00,

    0xfe, 0x00,
    0x7b, 0x2a, //[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
    0x23, 0x2d, //[3]rst_rc [2:1]drv_sync [0]pwd_rc
    0xfe, 0x03,
    0x01, 0x27, //20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
    0x02, 0x56, //[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
    0x03, 0x8e, //b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
    0x12, 0x80,
    0x13, 0x07, //LWC
    0x15, 0x12, //[1:0]clk_lane_mode
    0xfe, 0x00,
    0x3e, 0x91, //40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]
#if  0
    wrGC2053_MIPIReg(0xfe, 0x00);
    wrGC2053_MIPIReg(0x08, 0x19);
    wrGC2053_MIPIReg(0x41, 0x04);
    wrGC2053_MIPIReg(0x42, 0x65);
    printf("GC2063 30fps\n");
#endif
    0xfe, 0x00,
    0x08, 0x19,
    0x41, 0x04,
    0x42, 0x65,

};



#endif // 0
static void *iic = NULL;

static u8 WRCMD = 0x6e;
static u8 RDCMD = 0x6f;
static unsigned char wrGC2053_MIPIReg(unsigned char regID, unsigned char regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __wend;
    }
    if (dev_ioctl(iic, IIC_IOCTL_TX, regID)) {
        ret = 0;
        goto __wend;
    }
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regDat)) {
        ret = 0;
        goto __wend;
    }
__wend:

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);

    return ret;

}

static unsigned char rdGC2053_MIPIReg(unsigned char regID, unsigned char *regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(10);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regID)) {
        ret = 0;
        goto __rend;
    }

    delay(10);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, RDCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(10);

    if (dev_ioctl(iic, IIC_IOCTL_RX_WITH_STOP_BIT, (u32)regDat)) {
        ret = 0;
        goto __rend;
    }
__rend:

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    return ret;

}

static unsigned char wrGC2053_MIPIRegArray(u8 *array, u32 size)
{

    dev_ioctl(iic, IIC_IOCTL_START, 0);
    dev_ioctl(iic, IIC_IOCTL_SET_NORMAT_RATE, 2);
    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    dev_write(iic, array, size);
    return 1;

}


/*************************************************************************************************
    sensor api
*************************************************************************************************/



static void GC2053_MIPI_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    u32 i;
    u8 val;

    for (i = 0; i < sizeof(spec_GC2053_MIPI_INI_REG) / sizeof(Sensor_reg_ini); i++) {
        wrGC2053_MIPIReg(spec_GC2053_MIPI_INI_REG[i].addr, spec_GC2053_MIPI_INI_REG[i].value);
    }
#if  0
    if (*frame_freq == 25) {
        wrGC2053_MIPIReg(0xfe, 0x00);
        wrGC2053_MIPIReg(0x08, 0xfc);
        wrGC2053_MIPIReg(0x41, 0x05);
        wrGC2053_MIPIReg(0x42, 0x3A);
        printf("GC2063 25fps\n");
    } else {
        wrGC2053_MIPIReg(0xfe, 0x00);
        wrGC2053_MIPIReg(0x08, 0x19);
        wrGC2053_MIPIReg(0x41, 0x04);
        wrGC2053_MIPIReg(0x42, 0x65);
        printf("GC2063 30fps\n");
    }

#endif

    return;
}


static s32 GC2053_MIPI_set_output_size(u16 *width, u16 *height, u8 *frame_freq)
{
    return 0;
}


static s32 GC2053_MIPI_power_ctl(u8 isp_dev, u8 is_work)
{

    return 0;
}

static s32 GC2053_MIPI_ID_check(void)
{
    u8 pid = 0x00;
    u8 ver = 0x00;
    u8 i ;

    for (i = 0; i < 3; i++) { //
        rdGC2053_MIPIReg(0xf0, &pid);
        rdGC2053_MIPIReg(0xf1, &ver);
    }

    puts("Sensor PID \n");
    put_u32hex(pid);
    put_u32hex(ver);
    puts("\n");

    if (pid != 0x20 || ver != 0x53) {
        puts("\n----not GC2053_MIPI-----\n");
        return -1;
    }
    puts("\n----hello GC2053_MIPI-----\n");
    return 0;
}


static u8 cur_sensor_type = 0xff;
static void GC2053_mipi_reset(u8 isp_dev)
{
    u32 reset_gpio;
    u32 pwdn_gpio;

    if (isp_dev == ISP_DEV_0) {
        reset_gpio = reset_gpios[0];
        pwdn_gpio = pwdn_gpios[0];
    } else {
        reset_gpio = reset_gpios[1];
        pwdn_gpio = pwdn_gpios[1];
    }
    gpio_direction_output(reset_gpio, 0);
    gpio_direction_output(pwdn_gpio, 0);

    delay(40000);
    gpio_direction_output(reset_gpio, 1);
    delay(40000);

    printf("pwdn_gpio=%d\n", pwdn_gpio);
    gpio_direction_output(pwdn_gpio, 1);
}
static s32 GC2053_MIPI_check(u8 isp_dev, u32 reset_gpio, u32 pwdn_gpio)
{
    puts("\n\n GC2053_MIPI_check \n\n");



    if (!iic) {
        if (isp_dev == ISP_DEV_0) {
            iic = dev_open("iic0", 0);
        } else {
            iic = dev_open("iic1", 0);
        }
        if (!iic) {
            return -1;
        }
    } else {
        if (cur_sensor_type != isp_dev) {
            return -1;
        }
    }
    printf("\n\n isp_dev =%d\n\n", isp_dev);

    reset_gpios[isp_dev] = reset_gpio;
    pwdn_gpios[isp_dev] = pwdn_gpio;
    GC2053_mipi_reset(isp_dev);
    puts("GC2053_mipi_id_check\n");
    if (0 != GC2053_MIPI_ID_check()) {
        dev_close(iic);
        iic = NULL;
        return -1;
    }

    cur_sensor_type = isp_dev;

    return 0;
}




static void gc2053_set_isp_size(u32 width, u32 height, u32 fps)
{
    isp_sen_t *c;
    list_for_each_camera(c) {

        printf("\n  (const char *)(c->logo) :::%s\n", (const char *)(c->logo));
        if (!strcmp((const char *)(c->logo), "GC2053")) {
            c->sen_size.w =  	width;
            c->sen_size.h =  	height;
            c->isp_size.w = 	width;
            c->isp_size.h = 	height;


            c->sen_cap_size.w =  	width;
            c->sen_cap_size.h =  	height;
            c->isp_cap_size.w = 	width;
            c->isp_cap_size.h = 	height;

            c->cap_fps = fps;
            break;
        }
    }
}

extern  void video_disp_stop(int id);
extern struct video_window disp_window[7][4];
extern  int video_disp_start(int id, const struct video_window *win);
extern void set_imc_size();
extern void  isp_test();
static  u32 t1;

static s32 GC2053_MIPI_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    // puts("\n\n GC2053_mipi_init22 \n\n");
    u32 i;
#ifdef  ENABLE_SPECIAL_SMALL_PICTURE

    static u8  init_once = 0;

    t1 = timer_get_ms();


    printf("\n  init_once============================%d\n", init_once);
    if (!init_once) {

        init_once = 1;
        printf("\n  init_256x144_200FPS\n");
        GC2053_MIPI_config_SENSOR(width, height, format, frame_freq);
#ifdef CONFIG_IO_MEASURE_TIMER_ENABLE
        set_io_measure_state(0);
#endif
        // sys_timeout_add(NULL, isp_test, 6 * 1000);
    } else {

        printf("\n  init_1080P_30FPS\n");

        for (i = 0; i < sizeof(RECOVER_GC2053_MIPI_INI_REG) / sizeof(Sensor_reg_ini); i++) {
            wrGC2053_MIPIReg(RECOVER_GC2053_MIPI_INI_REG[i].addr, RECOVER_GC2053_MIPI_INI_REG[i].value);
        }


#ifdef CONFIG_IO_MEASURE_TIMER_ENABLE
        set_io_measure_state(0);

#endif
        //  gc2053_set_isp_size(1920,1080,30);




        printf("time1::::::::%d\n", timer_get_ms() - t1);



    }

#else

    //  GC2053_MIPI_config_SENSOR(width, height, format, frame_freq);// 512X288

    for (i = 0; i < sizeof(RECOVER_GC2053_MIPI_INI_REG) / sizeof(Sensor_reg_ini); i++) {
        wrGC2053_MIPIReg(RECOVER_GC2053_MIPI_INI_REG[i].addr, RECOVER_GC2053_MIPI_INI_REG[i].value); // 1080P
    }
#ifdef CONFIG_IO_MEASURE_TIMER_ENABLE
    set_io_measure_state(0);
#endif
#endif

    return 0;
}

static s32 GC2053_mipi_get_mipi_clk(u32 *mipi_clk,  u32 *tval_hstt, u32 *tval_stto, u16 width, u16 height, u8 frame_freq)
{
    *mipi_clk = 297;
    *tval_hstt = 0;
    *tval_stto = 30;
    return 0;
}


u32 spec_GC2053_mipi_calc_shutter(isp_ae_shutter_t *shutter, u32 exp_time_us, u32 gain)
{
    u32 texp;
    u32 texp_align;
    u32 ratio;

    texp = exp_time_us * PCLK / LINE_LENGTH_CLK;


    texp_align = (texp) * LINE_LENGTH_CLK / PCLK;

    if (texp_align < exp_time_us) {
        ratio = (exp_time_us) * (1 << 10) / texp_align;
        //printf("ratio = %d\n",ratio);
    } else {
        ratio = (1 << 10);
    }

    shutter->ae_exp_line =  texp;
    shutter->ae_gain = (gain * ratio) >> 10;
    shutter->ae_exp_clk = 0;

    //printf("exp_time_us=%d, texp=%d, gain=%d->%d\n", exp_time_us, texp, gain,shutter->ae_gain);
    return 0;

    return 0;

}
static u32 regValTable[29][4] = {
    {0x00, 0x00, 0x01, 0x00},
    {0x00, 0x10, 0x01, 0x0c},
    {0x00, 0x20, 0x01, 0x1b},
    {0x00, 0x30, 0x01, 0x2c},
    {0x00, 0x40, 0x01, 0x3f},
    {0x00, 0x50, 0x02, 0x16},
    {0x00, 0x60, 0x02, 0x35},
    {0x00, 0x70, 0x03, 0x16},
    {0x00, 0x80, 0x04, 0x02},
    {0x00, 0x90, 0x04, 0x31},
    {0x00, 0xa0, 0x05, 0x32},
    {0x00, 0xb0, 0x06, 0x35},
    {0x00, 0xc0, 0x08, 0x04},
    {0x00, 0x5a, 0x09, 0x19},
    {0x00, 0x83, 0x0b, 0x0f},
    {0x00, 0x93, 0x0d, 0x12},
    {0x00, 0x84, 0x10, 0x00},
    {0x00, 0x94, 0x12, 0x3a},
    {0x01, 0x2c, 0x1a, 0x02},
    {0x01, 0x3c, 0x1b, 0x20},
    {0x00, 0x8c, 0x20, 0x0f},
    {0x00, 0x9c, 0x26, 0x07},
    {0x02, 0x64, 0x36, 0x21},
    {0x02, 0x74, 0x37, 0x3a},
    {0x00, 0xc6, 0x3d, 0x02},
    {0x00, 0xdc, 0x3f, 0x3f},
    {0x02, 0x85, 0x3f, 0x3f},
    {0x02, 0x95, 0x3f, 0x3f},
    {0x00, 0xce, 0x3f, 0x3f},
};
static u32 Analog_Multiple[29] = {
    1024,
    1184,
    1424,
    1632,
    2032,
    2352,
    2832,
    3248,
    4160,
    4800,
    5776,
    6640,
    8064,
    9296,
    11552,
    13312,
    16432,
    18912,
    22528,
    25936,
    31840,
    36656,
    45600,
    52512,
    64768,
    82880,
    88000,
    107904,
    113168,
};

static void calc_gain(u32 gain, u32 *_again, u32 *_dgain)
{
    int i;
    u32 Decimal;
    u32 reg0, reg1, reg2, Analog_Index;
    u32 dgain, again, dcggain = 0;

    if (gain < 1024) {
        gain = 1024;
    }
    if (gain > 127 * 1024) {
        gain = 127 * 1024;
    }
    //printf("gain = %d ",gain);


    Analog_Index = 0;

    while (Analog_Index < 29) {
        if (gain < Analog_Multiple[Analog_Index]) {
            break;
        } else {
            Analog_Index++;
        }
    }
    dgain = gain * 1000 / Analog_Multiple[Analog_Index - 1];
    Decimal = (dgain * 64) / 1000;


    *_dgain = (Decimal << 2) & 0xfffc;
    *_again = Analog_Index - 1;

    // printf("gain = %d  dg = %d; ag= %d; \n ",gain,dgain,Analog_Multiple[Analog_Index-1]);
}


static void set_shutter(u32 texp)
{
    u8 wval, rval;
    if (cur_expline == texp) {
        return;
    }
    cur_expline  = texp;

    wrGC2053_MIPIReg(0xfe, 0x0);
    wval = (u8)(texp >> 8);
    wrGC2053_MIPIReg(0x03, wval);
    wval = (u8)texp;
    wrGC2053_MIPIReg(0x04, wval);
}
static void set_again(u32 again)
{
    u8 pid = 0x00;
    u8 ver = 0x00;

    if (cur_again == again) {
        return;
    }
    cur_again  = again;
    wrGC2053_MIPIReg(0xb4, regValTable[again][0]);
    wrGC2053_MIPIReg(0xb3, regValTable[again][1]);
    wrGC2053_MIPIReg(0xb8, regValTable[again][2]);
    wrGC2053_MIPIReg(0xb9, regValTable[again][3]);


    rdGC2053_MIPIReg(0xb4, &pid);
    rdGC2053_MIPIReg(0xb3, &ver);

    //  printf("(%x,%x) ",pid,ver);
    return;
}

static void set_dgain(u32 dgain)
{
    if (cur_dgain == dgain) {
        return;
    }
    cur_dgain  = dgain;

    wrGC2053_MIPIReg(0xb1, (dgain >> 8) & 0xff);
    wrGC2053_MIPIReg(0xb2, (dgain >> 0) & 0xff);
}
u32 spec_GC2053_mipi_set_shutter(isp_ae_shutter_t *shutter)
{
    u32 gain;
    u32 again, dgain;

    gain = (shutter->ae_gain);
    calc_gain(gain, &again, &dgain);
    set_shutter(shutter->ae_exp_line);
    set_again(again);
    set_dgain(dgain);
    return 0;
}

static void GC2053_MIPI_sleep()
{
    wrGC2053_MIPIReg(0x3e, 0x90);
}

static void GC2053_MIPI_wakeup()
{

    wrGC2053_MIPIReg(0x3e, 0x91);
}

static void GC2053_MIPI_W_Reg(u16 addr, u16 val)
{
    //printf("update reg%x with %x\n", addr, val);
    wrGC2053_MIPIReg((u8)addr, (u8)val);
}

static u16 GC2053_MIPI_R_Reg(u16 addr)
{
    u8 val;
    rdGC2053_MIPIReg((u8)addr, &val);
    return val;
}





#if  0 //GC2053_MIPI_DVP
REGISTER_CAMERA(GC2053_MIPI) = {
    .logo 				= 	"spec_GC2053",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_RGGB,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
    .mbus_type          =   SEN_MBUS_CSI2,
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_10B | SEN_MBUS_CSI2_2_LANE,

#ifdef  ENABLE_SPECIAL_SMALL_PICTURE

#if  0
    .fps         		= 	200,

    .sen_size 			= 	{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_size 			= 	{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},


    .cap_fps         		= 30,
    .sen_cap_size 			= {1920, 1080},	//{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_cap_size 			= {1920, 1080},	//{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

#else

    .fps         		= 	100,

    .sen_size 			= 	{512, 288}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_size 			= 	{512, 288}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},


    .cap_fps         		= 30,
    .sen_cap_size 			= {1920, 1080},	//{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_cap_size 			= {1920, 1080},	//{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},


#endif

#else

#if  1
    .fps         		= 	30,

    .sen_size 			= 	{GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_size 			= 	{GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},


    .cap_fps         		= 	30,
    .sen_cap_size 			= {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_cap_size 			= {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},
#else
    .fps         		= 	200,

    .sen_size 			= 	{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_size 			= 	{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},


    .cap_fps         		= 200,//30,
    .sen_cap_size 			= {256, 144}, // {1920,1080},	//{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .isp_cap_size 			= {256, 144}, //{1920,1080},	//{256, 144}, // {GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},


#endif


#endif

    .ops                =   {
        .avin_fps           =   NULL,
        .avin_valid_signal  =   NULL,
        .avin_mode_det      =   NULL,
        .sensor_check 		= 	GC2053_MIPI_check,
        .init 		        = 	GC2053_MIPI_init,
        .set_size_fps 		=	GC2053_MIPI_set_output_size,
        //.get_mipi_clk       =   GC2053_mipi_get_mipi_clk,
        .power_ctrl         =   GC2053_MIPI_power_ctl,

        .get_ae_params  	=	spec_GC2053_MIPI_get_ae_params,
        .get_awb_params 	=	spec_GC2053_mipi_get_awb_params,
        .get_iq_params 	    =	spec_GC2053_mipi_get_iq_params,

        .sleep 		        =	GC2053_MIPI_sleep,
        .wakeup 		    =	GC2053_MIPI_wakeup,
        .write_reg 		    =	GC2053_MIPI_W_Reg,
        .read_reg 		    =	GC2053_MIPI_R_Reg,

    }
};
#else
REGISTER_CAMERA(GC2053_MIPI) = {
    .logo 				= 	"spec_GC2053",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_RGGB,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
    .mbus_type          =   SEN_MBUS_PARALLEL,
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_10B | SEN_MBUS_PCLK_SAMPLE_FALLING,
    .fps         		= 	25,

    .sen_size 			= 	{GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},
    .isp_size 			= 	{GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .cap_fps         		= 	25,
    .sen_cap_size 			= 	{GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},
    .isp_cap_size 			= 	{GC2053_MIPI_OUTPUT_W, GC2053_MIPI_OUTPUT_H},

    .ops                =   {
        .avin_fps           =   NULL,
        .avin_valid_signal  =   NULL,
        .avin_mode_det      =   NULL,
        .sensor_check 		= 	GC2053_MIPI_check,
        .init 		        = 	GC2053_MIPI_init,
        .set_size_fps 		=	GC2053_MIPI_set_output_size,
        //.get_mipi_clk       =   GC2053_mipi_get_mipi_clk,
        .power_ctrl         =   GC2053_MIPI_power_ctl,

        .get_ae_params  	=	spec_GC2053_MIPI_get_ae_params,
        .get_awb_params 	=	spec_GC2053_mipi_get_awb_params,
        .get_iq_params 	    =	spec_GC2053_mipi_get_iq_params,

        .sleep 		        =	GC2053_MIPI_sleep,
        .wakeup 		    =	GC2053_MIPI_wakeup,
        .write_reg 		    =	GC2053_MIPI_W_Reg,
        .read_reg 		    =	GC2053_MIPI_R_Reg,

    }
};
#endif


