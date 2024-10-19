#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "sc2313.h"
#include "asm/isp_alg.h"
#include "asm/dv17.h"

static u32 reset_gpios[2] = {-1, -1};
static u32 pwdn_gpios[2] = {-1, -1};

extern void *sc2313_dvp_get_ae_params();
extern void *sc2313_dvp_get_awb_params();
extern void *sc2313_dvp_get_iq_params();
extern void sc2313_dvp_ae_ev_init(u32 fps);


#if 1
#define LINE_LENGTH_CLK     2250
#define FRAME_LENGTH        0xac0 //0xe10
#define ROW_TIME_NS         60000    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)
#define INPUT_CLK  24
#define PCLK  37.5//24



#define MAX_SHORT_EXP 	 	166//216  		//  (FRAME_LENGTH - 5 + 64  + 16)/17
#define MAX_SHORT_DELAY_LINE 	 84//109 // 166/2+1

#define MAX_SHORT_EXP_LINE 	 	((MAX_SHORT_EXP - 4 )*2)
#define MAX_LONG_EXP_LINE 	 	((FRAME_LENGTH - MAX_SHORT_EXP - 5 )*2)


#else

#define LINE_LENGTH_CLK     2250
#define FRAME_LENGTH        0xe10
#define ROW_TIME_NS         92250    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)
#define INPUT_CLK  24
#define PCLK  24



#define MAX_SHORT_EXP 	 	216  		//  (FRAME_LENGTH - 5 + 64  + 16)/17
#define MAX_SHORT_DELAY_LINE 	 109 // 216/2+1

#define MAX_SHORT_EXP_LINE 	 	((MAX_SHORT_EXP - 4 )*2)
#define MAX_LONG_EXP_LINE 	 	((FRAME_LENGTH - MAX_SHORT_EXP - 5 )*2)

#endif






static u32 cur_lgain = -1;
static u32 cur_sgain = -1;
static u32 cur_lexpline = -1;
static u32 cur_sexpline = -1;
static u32 cur_line_length_clk = -1;

static u32 short_g = 16;
static u32 long_g = 256;

static u32 line_length_clk = LINE_LENGTH_CLK;

static void *iic = NULL;

#define WRCMD 0x60
#define RDCMD 0x61


typedef struct {
    u16 addr;
    u8 value;
} sensor_ini_regs_t;

sensor_ini_regs_t sc2313_dvp_ini_regs[] = {

#if 0
    0x0103, 0x01,
    0x0100, 0x00,

    0x36e9, 0xa3, //bypass pll1	20180830
    0x36f9, 0x85, //bypass pll2	20180830

    //close mipi
    //0x3018,0x1f,
    //0x3019,0xff,
    //0x301c,0xb4,

    //0x320c,0x05,
    //0x320d,0x28,

    //0x320c,0x0a,
    //0x320d,0x50,


    0x4509, 0x10,
    0x4500, 0x39,
    0x3907, 0x00,
    0x3908, 0x44,


    0x3633, 0x87,
    0x3306, 0x7e,
    0x330b, 0x00,

    0x3635, 0x4c,
    0x330e, 0x7a,
    //0x3621,0xb0,
    0x3302, 0x1f, //3302 need be odd why????  3366  3302   3621

    0x3e01, 0x8c,
    0x3e02, 0x80,
    0x3e09, 0x1f,
    0x3e08, 0x3f,
    0x3e06, 0x03,

    0x337f, 0x03, //new auto precharge  330e in 3372   [7:6] 11: close div_rst 00:open div_rst
    0x3368, 0x04,
    0x3369, 0x00,
    0x336a, 0x00,
    0x336b, 0x00,
    0x3367, 0x08,
    0x330e, 0x30,

    0x3320, 0x06, // New ramp offset timing
    //0x3321,0x06,
    0x3326, 0x00,
    0x331e, 0x11,
    0x331f, 0x21,
    0x3303, 0x20,
    0x3309, 0x30,


    0x4501, 0xc4,
    0x3e06, 0x00,
    0x3e08, 0x03,
    0x3e09, 0x10,

    0x3366, 0x7c, // div_rst gap

    //noise
    0x3622, 0x02,

    0x3633, 0x63,

    //fpn
    0x3038, 0x88,
    0x3635, 0x45,
    0x363b, 0x04,
    0x363c, 0x06,
    0x363d, 0x05,


    //1021
    0x3633, 0x23,

    0x3301, 0x10,
    0x3306, 0x58,

    0x3622, 0x06, //blksun
    0x3631, 0x88,
    0x3630, 0x38,

    0x3633, 0x22,

    //noise test
    //0x3e08,0x3f,
    //0x3e09,0x1f,
    //0x3e06,0x0f,
    //0x5000,0x00,
    //0x5002,0x00,
    //0x3907,0x08,


    //mipi
    0x3018, 0x33, //[7:5] lane_num-1
    0x3031, 0x0c, //[3:0] bitmode
    0x3037, 0x40, //[6:5] bitsel  40:12bit
    0x3001, 0xFE, //[0] c_y

    //lane_dis of lane3~8
    //0x3018,0x12,
    //0x3019,0xfc,

    0x4603, 0x00, //[0] data_fifo mipi mode
    0x4837, 0x35, //[7:0] pclk period * 2
    //0x4827,0x48,//[7:0] hs_prepare_time[7:0]

    0x36e9, 0x83,
    //0x36ea,0x2a,
    0x36eb, 0x0f,
    0x36ec, 0x1f,


    //0x3e08,0x03,
    //0x3e09,0x10,

    0x303f, 0x01,

    //1024

    0x330b, 0x20,

    //20170307
    0x3640, 0x00,
    0x3308, 0x10,
    0x3637, 0x0a,
    0x3e09, 0x20, //3f for 2x fine gain
    0x363b, 0x08,


    //20170307
    0x3637, 0x09, // ADC range: 14.8k fullwell  blc target : 0.9k   output fullwell: 13.9k (5fps 27C  linear fullwell is 14.5K)
    0x3638, 0x14,
    0x3636, 0x65,
    0x3907, 0x01,
    0x3908, 0x01,

    0x3320, 0x01, //ramp
    0x331e, 0x15,
    0x331f, 0x25,
    0x3366, 0x80,
    0x3634, 0x34,

    0x57a4, 0xf0, //default c0,

    0x3635, 0x41, //fpn

    0x3e02, 0x30, //minimum exp 3? debug
    0x3333, 0x30, //col fpn  G >br  ?
    0x331b, 0x83,
    0x3334, 0x40,

    //30fps
    //0x320c,0x04,
    //0x320d,0x4c,
    0x3306, 0x6c,
    0x3638, 0x17,
    0x330a, 0x01,
    0x330b, 0x14,

    0x3302, 0x10,
    0x3308, 0x08,
    0x3303, 0x18,
    0x3309, 0x18,
    0x331e, 0x11,
    0x331f, 0x11,

    //sram write
    0x3f00, 0x0d, //[2]   hts/2-4
    0x3f04, 0x02,
    0x3f05, 0x22,

    0x3622, 0xe6,
    0x3633, 0x22,
    0x3630, 0xc8,
    0x3301, 0x10,

    //减少samp timing,增加330b/blksun margin

    //0319
    0x36e9, 0xa3,
    0x36eb, 0x0b,
    0x36ec, 0x0f,
    0x3638, 0x27,

    0x33aa, 0x00, //power save mode
    0x3624, 0x02,
    0x3621, 0xac,


    //aec  analog gain: 0x570 (15.75 x 2.7=41.85)   2xdgain :0xae0  4xdgain : 0x15c0  8xdgain : 0x2b80 16xdgain : 0x5700 3e0a 3e0b real ana gain  3e2c 3e2d real digital fine gain

    //0x3e03,0x03,
    //0x3e14,0xb0, //[7]:1 ana fine gain double 20~3f
    //0x3e1e,0xb5, //[7]:1 open DCG function in 0x3e03=0x03 [6]:0 DCG >2   [2] 1: dig_fine_gain_en [1:0] max fine gain  01: 3f
    //0x3e0e,0x66, //[7:3] fine gain to compsensate 2.4x DCGgain  5 : 2.3125x  6:2.375x  [2]:1 DCG gain between sa1gain 2~4  [1]:1 dcg gain in 0x3e08[5]


    0x4509, 0x40,

    //321
    0x391e, 0x00,
    0x391f, 0xc0,
    0x3635, 0x45,
    0x336c, 0x40,

    //0324
    0x3621, 0xae,
    0x3623, 0x08,

    //0326 rownoise
    0x36fa, 0xad, //charge pump
    0x3634, 0x44,

    //0327
    0x3621, 0xac, //fifo delay
    0x4500, 0x59,
    0x3623, 0x18, //for more grp rdout setup margin

    //sram write
    0x3f08, 0x04,
    0x3f00, 0x0d, //[2]   hts/2-4-{3f08} //
    0x3f04, 0x02, //0321
    0x3f05, 0x1e, //0321

    //0329
    0x336c, 0x42, //recover read timing

    //20180509
    0x5000, 0x06, //dpc enable
    0x5780, 0x7f, //auto blc setting
    0x57a0, 0x00,	//gain0 = 2x	0x0710至0x071f
    0x57a1, 0x71,
    0x57a2, 0x01,	//gain1 = 8x	0x1f10至0x1f1f
    0x57a3, 0xf1,
    0x5781, 0x06,	//white	1x
    0x5782, 0x04,	//2x
    0x5783, 0x02,	//8x
    0x5784, 0x01,	//128x
    0x5785, 0x16,	//black	1x
    0x5786, 0x12,	//2x
    0x5787, 0x08,	//8x
    0x5788, 0x02,	//128x

    //20180509 high temperature
    0x3933, 0x28,
    0x3934, 0x0a,
    0x3940, 0x1b,
    0x3941, 0x40,
    0x3942, 0x08,
    0x3943, 0x0e,

    //20180510
    //size	1928x1088
    0x3208, 0x07,
    0x3209, 0x88,
    0x320a, 0x04,
    0x320b, 0x40,
    0x3211, 0x04,
    0x3213, 0x04,

    //20180511	81M
    0x36e9, 0xa3,
    0x36ea, 0x77,
    0x36eb, 0x2b,
    0x36ec, 0x0f,
    0x36ed, 0x03,
    0x36f9, 0x85,
    0x36fa, 0x2d,
    0x36fb, 0x10,

    0x320c, 0x04, //hts=1125*2=2250
    0x320d, 0x65,
    0x320e, 0x04, //vts=1200
    0x320f, 0xb0,
    0x3235, 0x09, //vts*2-0x02
    0x3236, 0x5e,
    0x3f04, 0x02, //{0x320c,0x320d}/2-0x3f08-0x04
    0x3f05, 0x2a,

    //20180514
    0x3802, 0x00,

    //20180604
    0x3624, 0x47,
    0x3621, 0xac,

    //20180607
    0x36fa, 0x2f,

    //dvp 20180607
    //close mipi
    0x3018, 0x1f,
    0x3019, 0xff,
    0x301c, 0xb4,
    0x3018, 0x1f,
    0x3031, 0x0a,
    0x3037, 0x24,
    0x3001, 0xff,
    0x4603, 0x01,
    0x4837, 0x36,
    0x303f, 0x81,

    //size 20180607
    0x3208, 0x07,
    0x3209, 0x80,
    0x320a, 0x04,
    0x320b, 0x38,
    0x3211, 0x08,
    0x3213, 0x08,

    0x3d08, 0x00,
    0x3641, 0x01,

    //0612
    0x3637, 0x08,
    0x3638, 0x25,
    0x3635, 0x40,
    0x363b, 0x08,
    0x363c, 0x05,
    0x363d, 0x05,

    //20180619
    0x3641, 0x01,
    0x3303, 0x1c,
    0x3309, 0x1c,

    //0705
    0x3324, 0x02, //falling edge: ramp_offset_en cover ramp_integ_en
    0x3325, 0x02,
    0x333d, 0x08, //count_div_rst_width
    0x3314, 0x04,
    0x36fa, 0x28,

    //20180716
    0x3205, 0x93,
    //0x3e03,0x03, //gain map: 0x03 mode
    0x3e14, 0xb0, //[7]:1 ana fine gain double 20~3f
    0x3e1e, 0x35, //[7]:1 open DCG function in 0x3e03=0x03 [6]:0 DCG >2   [2] 1: dig_fine_gain_en [1:0] max fine gain  01: 3f
    0x3e0e, 0x66, //[7:3] fine gain to compsensate 2.4x DCGgain  5 : 2.3125x  6:2.375x  [2]:1 DCG gain between sa1gain 2~4  [1]:1 dcg gain in 0x3e08[5]

    //20180716 digital logic
    //3301 auto logic read 0x3373 for auto value	[1,2.72)=[6,1a], [2.72,4*2.72)=[8,1a], [4*2.72,+)=[8,1a] 20180717
    0x3364, 0x1d, //[4] fine gain op 1~20--3f 0~10--1f [4] ana dithring en
    0x33b6, 0x07, //gain0 when dcg off
    0x33b7, 0x07, //gain1 when dcg off
    0x33b8, 0x10, //sel0 when dcg off
    0x33b9, 0x10, //sel1 when dcg off
    0x33ba, 0x10, //sel2 when dcg off
    0x33bb, 0x07, //gain0 when dcg on
    0x33bc, 0x07, //gain1 when dcg on
    0x33bd, 0x14, //sel0 when dcg on
    0x33be, 0x14, //sel1 when dcg on
    0x33bf, 0x14, //sel2 when dcg on
    //3622 auto logic read 0x3680 for auto value
    0x360f, 0x05, //[0] 3622 auto en
    0x367a, 0x40, //gain0
    0x367b, 0x40, //gain1
    0x3671, 0xf6, //sel0
    0x3672, 0x16, //sel1
    0x3673, 0x16, //sel2
    //3630 auto logic read 0x3681 for auto value
    0x366e, 0x04, //[2] fine gain op 1~20--3f 0~10--1f
    0x3670, 0x4a, //[1] 3630 auto en, [3] 3633 auto en, [6] 363a auto en
    0x367c, 0x40, //gain0  3e08[5:2] 1000
    0x367d, 0x58, //gain1 3e08[5:2] 1011
    0x3674, 0xc8, //sel0
    0x3675, 0x54, //sel1
    0x3676, 0x18, //sel2
    //3633 auto logic read 0x3682 for auto value
    0x367e, 0x40, //gain0  3e08[5:2] 1000
    0x367f, 0x58, //gain1  3e08[5:2] 1011
    0x3677, 0x22, //sel0
    0x3678, 0x53, //sel1
    0x3679, 0x55, //sel2
    //363a auto logic read 0x3685 for auto value
    0x36a0, 0x58, //gain0  3e08[5:2] 1011
    0x36a1, 0x78, //gain1  3e08[5:2] 1111
    0x3696, 0x83, //sel0
    0x3697, 0x87, //sel1
    0x3698, 0x9f, //sel2

    //20180717
    0x3201, 0x04,
    0x3203, 0x04,
    0x3205, 0x8b,
    0x3207, 0x43,
    0x3211, 0x04,
    0x3213, 0x04,


    //20180730
    0x6000, 0x00,
    0x6002, 0x00,

    //20180822
    0x3037, 0x24, //[3:0] pump div	range [10M,20M],sclk=81/2=40.5M,div=4-->sclk/4=10.125M,duty cycle-->even number
    0x3038, 0x44, //[7:4]ppump & [3:0]npump
    0x3632, 0x18, //[5:4]idac driver
    0x5785, 0x40, //black	point 1x

    //20180824
    0x4809, 0x01, //mipi first frame, lp status

    //20181030
    0x320e, 0x07, //20fps
    0x320f, 0x08,
    //20180810 vc mode
    //0x4816,0x51,//bit[4]
    0x3220, 0x51, //bit[6]
    //0x4602,0x0f,//bit[3:0]
    0x5001, 0x20,
    0x33c0, 0x05, //bit[2]
    0x6000, 0x06,
    0x6002, 0x06,
    0x3e00, 0x00, //max long exposure = 0x1a67
    0x3e01, 0xa7,
    0x3e02, 0x00,
    0x3e03, 0x03,
    0x3e04, 0x03, //max short exposure = 0x1a7
    0x3e05, 0x70,
    0x3e06, 0x00,
    0x3e07, 0x80,
    0x3e08, 0x23,
    0x3e09, 0x3f,
    0x3e23, 0x00,
    0x3e24, 0xd8,
    0x320e, 0x0e, //double vts
    0x320f, 0x10,

    0x3641, 0x03, //dvp io driver

    //init
    //; 0x3e00,0x00,//max_exposure = vts*2-6;	min_exposure = 3;	20180712
    //; 0x3e01,0x95,
    //; 0x3e02,0xa0,
    //; 0x3e03,0x0b,//gain map 0x0b mode	gain=1x
    //; 0x3e06,0x00,
    //; 0x3e07,0x80,
    //; 0x3e08,0x03,
    //; 0x3e09,0x20,
    //0x3e03,0x03,//gain map 0x03 mode	gain=1x
    //0x3e1e,0xb5,
    //0x3e06,0x00,
    //0x3e07,0x80,
    //0x3e08,0x00,
    //0x3e09,0x20,

    0x36e9, 0x23, //enable pll1	20180830
    0x36f9, 0x05, //enable pll2	20180830


    0x0100, 0x01,

#else


    0x0103, 0x01,
    0x0100, 0x00,
    0x36e9, 0xb3,
    0x36f9, 0x85,
    0x3018, 0x1f,
    0x3019, 0xff,
    0x301c, 0xb4,
    0x337f, 0x03,
    0x3368, 0x04,
    0x3369, 0x00,
    0x336a, 0x00,
    0x336b, 0x00,
    0x3367, 0x08,
    0x330e, 0x30,
    0x3326, 0x00,
    0x3631, 0x88,
    0x3640, 0x00,
    0x3636, 0x65,
    0x3320, 0x01,
    0x3366, 0x80,
    0x57a4, 0xf0,
    0x3333, 0x30,
    0x331b, 0x83,
    0x3334, 0x40,
    0x3302, 0x10,
    0x3308, 0x08,
    0x3303, 0x18,
    0x3309, 0x18,
    0x331e, 0x11,
    0x331f, 0x11,
    0x3622, 0xe6,
    0x3633, 0x22,
    0x3630, 0xc8,
    0x3301, 0x10,
    0x33aa, 0x00,
    0x391e, 0x00,
    0x391f, 0xc0,
    0x3634, 0x44,
    0x4500, 0x59,
    0x3623, 0x18,
    0x3f08, 0x04,
    0x3f00, 0x0d,
    0x336c, 0x42,
    0x5000, 0x06,
    0x5780, 0x7f,
    0x57a0, 0x00,
    0x57a1, 0x71,
    0x57a2, 0x01,
    0x57a3, 0xf1,
    0x5781, 0x06,
    0x5782, 0x04,
    0x5783, 0x02,
    0x5784, 0x01,
    0x5786, 0x12,
    0x5787, 0x08,
    0x5788, 0x02,
    0x3933, 0x28,
    0x3934, 0x0a,
    0x3940, 0x1b,
    0x3941, 0x40,
    0x3942, 0x08,
    0x3943, 0x0e,
    0x3624, 0x47,
    0x3621, 0xac,
    0x3638, 0x25,
    0x3635, 0x40,
    0x363b, 0x08,
    0x363c, 0x05,
    0x363d, 0x05,
    0x3641, 0x01,
    0x36ea, 0x6b,
    0x36ed, 0x03,
    0x36fa, 0x58,
    0x36fb, 0x10,
    0x320c, 0x04,
    0x320d, 0x65,
    0x3f04, 0x02,
    0x3f05, 0x2a,
    0x3235, 0x08,
    0x3236, 0xeb,
    0x36ec, 0x0f,
    0x4501, 0xb4,
    0x3637, 0x10,
    0x3907, 0x00,
    0x3908, 0x91,
    0x330a, 0x01,
    0x330b, 0x00,
    0x3306, 0x80,
    0x3324, 0x02,
    0x3325, 0x02,
    0x333d, 0x08,
    0x3314, 0x04,
    0x33c0, 0x05, //bit[2]
    0x3802, 0x00,
    0x4509, 0x20,
    0x3e14, 0xb0,
    0x3e1e, 0x35,
    0x3e0e, 0x66,
    0x3364, 0x1d,
    0x33b6, 0x07,
    0x33b7, 0x07,
    0x33b8, 0x10,
    0x33b9, 0x10,
    0x33ba, 0x10,
    0x33bb, 0x07,
    0x33bc, 0x07,
    0x33bd, 0x14,
    0x33be, 0x14,
    0x33bf, 0x14,
    0x360f, 0x05,
    0x367a, 0x40,
    0x367b, 0x40,
    0x3671, 0xf6,
    0x3672, 0x16,
    0x3673, 0x16,
    0x366e, 0x04,
    0x3670, 0x4a,
    0x367c, 0x40,
    0x367d, 0x58,
    0x3674, 0xc8,
    0x3675, 0x54,
    0x3676, 0x18,
    0x367e, 0x40,
    0x367f, 0x58,
    0x3677, 0x22,
    0x3678, 0x53,
    0x3679, 0x55,
    0x36a0, 0x58,
    0x36a1, 0x78,

    0x3696, 0x83,
    0x3697, 0x87,
    0x3698, 0x9f,
    0x3201, 0x04,
    0x3203, 0x04,
    0x3205, 0x8b,
    0x3207, 0x43,
    0x3211, 0x04,
    0x3213, 0x04,
    0x6000, 0x00,
    0x6002, 0x00,
    0x3037, 0x22,
    0x3038, 0x22,
    0x3632, 0x18,
    0x5785, 0x40,
    0x4809, 0x01,
    0x3380, 0x1b,
    0x3341, 0x07,
    0x3343, 0x03,
    0x3e25, 0x03,
    0x3e26, 0x20,
    0x3e00, 0x00,
    0x3e01, 0xb8,
    0x3e02, 0x00,
    0x3e03, 0x0b,
    0x3e04, 0x0b,
    0x3e05, 0x80,
    0x3e06, 0x00,
    0x3e07, 0x80,
    0x3e08, 0x03,
    0x3e09, 0x20,
    0x3e10, 0x00,
    0x3e11, 0x80,
    0x3e12, 0x03,
    0x3e13, 0x20,
    0x3e23, 0x0,
    //0x3e24,0x60,
    0x3e24, 0xa6,
    0x36eb, 0x1f,
    0x3220, 0x50,

    0x320e, 0xa,
    0x320f, 0xc0,
    0x320a, 0x04,
    0x320b, 0x38,
    0x5001, 0x20,
    0x36e9, 0x33,
    0x36f9, 0x05,
    0x0100, 0x01,

#endif
};


unsigned char wr_sc2313_reg(u16 regID, unsigned char regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __wend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX, regID >> 8)) {
        ret = 0;
        goto __wend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX, regID & 0xff)) {
        ret = 0;
        goto __wend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regDat)) {
        ret = 0;
        goto __wend;
    }

__wend:

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    return ret;

}

unsigned char rd_sc2313_reg(u16 regID, unsigned char *regDat)
{
    u8 ret = 1;
    dev_ioctl(iic, IIC_IOCTL_START, 0);
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX, regID >> 8)) {
        ret = 0;
        goto __rend;
    }

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regID & 0xff)) {
        ret = 0;
        goto __rend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, RDCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(1000);

    dev_ioctl(iic, IIC_IOCTL_RX_WITH_STOP_BIT, (u32)regDat);

__rend:

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    return ret;
}


/*************************************************************************************************
    sensor api
*************************************************************************************************/

void sc2313_dvp_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    u32 i;
    u8 v;


    sc2313_dvp_set_output_size(width, height, frame_freq);

    for (i = 0; i < sizeof(sc2313_dvp_ini_regs) / sizeof(sensor_ini_regs_t); i++) {
        wr_sc2313_reg(sc2313_dvp_ini_regs[i].addr, sc2313_dvp_ini_regs[i].value);
//        rd_sc2313_reg(sc2313_dvp_ini_regs[i].addr,&v);
//        printf("reg0x%04x =0x%02x->0x%02x\n", sc2313_dvp_ini_regs[i].addr, sc2313_dvp_ini_regs[i].value, v);
//        if (sc2313_dvp_ini_regs[i].value != v)
//            printf("Value Ch\n");
    }
//    u16 ii;
//    for (ii = 0x3208;ii<0x3214;ii++)
//    {
//        rd_sc2313_reg(ii,&v);
//        printf("reg0x%4x:0x%02x\n", ii, v);
//    }
    sc2313_dvp_ae_ev_init(*frame_freq);

    cur_lgain = -1;
    cur_sgain = -1;
    cur_lexpline = -1;
    cur_sexpline = -1;
    cur_line_length_clk = -1;

    //u8 val;
    //rd_sc2313_reg(0x5001, &val);
    //printf("0x5001=0x%x\n", val);
    return;
}


s32 sc2313_dvp_set_output_size(u16 *width, u16 *height, u8 *frame_freq)
{
    return 0;
}


s32 sc2313_dvp_power_ctl(u8 isp_dev, u8 is_work)
{
    return 0;
}

s32 sc2313_dvp_ID_check(void)
{
    u8 pid = 0x00;
    u8 ver = 0x00;
    u8 i ;

    for (i = 0; i < 3; i++) { //
        rd_sc2313_reg(0x3107, &pid);
        rd_sc2313_reg(0x3108, &ver);
    }

    puts("Sensor PID \n");
    put_u32hex(pid);
    put_u32hex(ver);
    puts("\n");

    // if (pid != 0x22 && ver != 0x35)
    if (pid != 0x23 || ver != 0x11) {
        puts("----not sc2313_dvp-----\n");
        return -1;
    }
    puts("----hello sc2313_dvp-----\n");
    return 0;
}

void sc2313_dvp_reset(u8 isp_dev)
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

    /*printf("pwdn_gpio=%d\n", pwdn_gpio);*/
    gpio_direction_output(pwdn_gpio, 1);

    gpio_direction_output(reset_gpio, 0);
    delay(40000);
    gpio_direction_output(reset_gpio, 1);
    //gpio_direction_output(pwdn_gpio, 0);
    delay(40000);
}


static u8 cur_sensor_type = 0xff;

s32 sc2313_dvp_check(u8 isp_dev, u32 reset_gpio, u32 pwdn_gpio)
{

    puts("\n\n sc2313_dvp_check \n\n");
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
    // printf("\n\n isp_dev =%d\n\n", isp_dev);

    reset_gpios[isp_dev] = reset_gpio;
    pwdn_gpios[isp_dev] = pwdn_gpio;

    sc2313_dvp_reset(isp_dev);

    if (0 != sc2313_dvp_ID_check()) {
        dev_close(iic);
        iic = NULL;
        return -1;
    }

    cur_sensor_type = isp_dev;

    return 0;
}

s32 sc2313_dvp_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    sc2313_dvp_reset(isp_dev);
    puts("\n\n sc2313_dvp_init22 \n\n");

    sc2313_dvp_config_SENSOR(width, height, format, frame_freq);

    return 0;
}


static void set_again(u32 again)
{
    return;
}

static void set_dgain(u32 lgain, u32 sgain)
{

    if (cur_lgain == lgain
        && cur_sgain == sgain) {
        //return;
    }
    cur_lgain  = lgain;
    cur_sgain  = sgain;

#if 0
    wr_sc2313_reg(0x3e08, ((lgain >> 24) & 0xff));
    wr_sc2313_reg(0x3e09, (lgain >> 16) & 0xff);
    //wr_sc2313_reg(0x3e06, (lgain>>8) & 0xff);
    //wr_sc2313_reg(0x3e07, (lgain>>0) & 0xff);

    wr_sc2313_reg(0x3e12, ((sgain >> 24) & 0xff));
    wr_sc2313_reg(0x3e13, (sgain >> 16) & 0xff);
    //wr_sc2313_reg(0x3e10, (sgain>>8) & 0xff);
    //wr_sc2313_reg(0x3e11, (sgain>>0) & 0xff);
#else
    {
//    	static u32 prev_gain = -1;
//
//    	//printf("lgain:%d\n",prev_gain);
//    	if(prev_gain>= 512)
//    	{
//    		isp0_hdr_merge_gain = prev_gain;
//    	}
//    	prev_gain = (lgain>>1);

        isp0_hdr_merge_gain = (lgain >> 1);

    }
#endif

}




#if 0
static void calc_gain(u32 lgain, u32 sgain, u32 *_lgain, u32 *_sgain)
{
    *_lgain = (lgain + 16) >> 5;
    *_sgain = (sgain + 16) >> 5;

}
#else

#if 0
static u32 gain2base(u32 val)
{
    if (val >= 16) {
        return 16;
    }
    if (val >= 8) {
        return 8;
    }
    if (val >= 4) {
        return 4;
    }
    if (val >= 2) {
        return 2;
    }
    return 1;
}

static u32 base2reg(u32 val)
{
    if (val == 16) {
        return 0x0f;
    }
    if (val == 8) {
        return 0x07;
    }
    if (val == 4) {
        return 0x03;
    }
    if (val == 2) {
        return 0x01;
    }
    return 0;
}

static unsigned int calc_one_gain(u32 gain)
{
    int i;
    unsigned int out;
    u32 dcg, coarse, dgain;
    u32 reg0, reg1, reg2, reg3;


    if (gain < 1024) {
        gain = 1024;
    }

    if (gain > 56 * 1024) {
        gain = 56 * 1024;
    }

    if (gain < 2785) {
        dcg = 0;
    } else {
        dcg = 1;
        gain = (gain * 1024 + 2785 / 2) / 2785;
    }

    coarse = (gain + 16) >> 5;

    reg0 = gain2base(coarse >> 5); //base

    reg1 = coarse / reg0;

    dgain = (gain << 5) / reg0 / reg1;
    reg2 = gain2base(dgain >> 10); //base
    reg3 = (dgain >> 3) / reg2;

    reg0 = base2reg(reg0);
    reg2 = base2reg(reg2);

    out = (((dcg << 5) | (reg0 << 2)) << 24)  | (reg1 << 16) | (reg2 << 8) | (reg3);
    //printf("dcg=%d, reg0=%d, reg1=%d ,reg2=%d, reg3=%d\n", dcg, reg0, reg1,reg2,reg3);
    return out;
}

static void calc_gain(u32 lgain, u32 sgain, u32 *_lgain, u32 *_sgain)
{

    *_lgain = calc_one_gain(lgain);
    *_sgain = calc_one_gain(sgain);


}
#else

static void calc_gain(u32 lgain, u32 sgain, u32 *_lgain, u32 *_sgain)
{

    *_lgain = (lgain);
    *_sgain = (sgain);
}

#endif
#endif

static void set_shutter(u32 lexp, u32 sexp)
{
    if (cur_lexpline == lexp && cur_sexpline == sexp) {
        return;
    }
    cur_lexpline  = lexp;
    cur_sexpline  = sexp;


    wr_sc2313_reg(0x3e00, (lexp >> 12) & 0xff);
    wr_sc2313_reg(0x3e01, (lexp >> 4) & 0xff);
    wr_sc2313_reg(0x3e02, (lexp << 4) & 0xf0);

    wr_sc2313_reg(0x3e04, sexp >> 4);
    wr_sc2313_reg(0x3e05, (sexp << 4) & 0xf0);
}


u32 sc2313_dvp_calc_shutter(isp_ae_shutter_t *shutter, u32 exp_time_us, u32 gain)
{
    u32 texp;
    u32 texp_align;
    u32 ratio;


    texp = exp_time_us * 1000 * 2 / ROW_TIME_NS;

    if (texp < 1) {
        texp = 1;
    }
    if (texp > MAX_LONG_EXP_LINE) {
        texp = MAX_LONG_EXP_LINE;
    }

    texp_align = (texp) * ROW_TIME_NS / (1000 * 2);

    if (texp_align < exp_time_us) {
        ratio = (exp_time_us) * (1 << 10) / texp_align;
        //printf("ratio = %d\n",ratio);
    } else {
        ratio = (1 << 10);
    }

    if (texp < 8 * 16) {
        texp = 8 * 16;
    }

    shutter->ae_exp_line =  texp;
    shutter->ae_gain = (gain * ratio) >> 10;
    shutter->ae_exp_clk = 0;

    if (shutter->ae_gain < 1024) {
        shutter->ae_gain = 1024;
    }

    shutter->ae_exp_line_short =  texp / 16;
    shutter->ae_gain_short = shutter->ae_gain * texp / (texp & (~0xf));
    shutter->ae_exp_clk_short = 0;

    printf("l:%d, s:%d\n", shutter->ae_exp_line, shutter->ae_exp_line_short);

#if 0
    shutter->ae_exp_line =  long_g;
    shutter->ae_gain = 1024;
    shutter->ae_exp_clk = 0;

    shutter->ae_exp_line_short = short_g;
    shutter->ae_gain_short = 1024;
    shutter->ae_exp_clk_short = 0;

#endif

    printf("exp_time_us=%d, texp=%d, gain=%d->%d\n", exp_time_us, texp, gain, shutter->ae_gain);
    return 0;

}

u32 sc2313_dvp_set_shutter(isp_ae_shutter_t *shutter)
{

    u32 lgain, sgain;
    //return 0;

    calc_gain(shutter->ae_gain, shutter->ae_gain_short, &lgain, &sgain);


    wr_sc2313_reg(0x3812, 0x00);

    //set_again(again);
    set_dgain(lgain, sgain);

    set_shutter(shutter->ae_exp_line, shutter->ae_exp_line_short);
    //printf("s=%d\n",shutter->ae_exp_line);

    wr_sc2313_reg(0x3812, 0x30);

//    printf("lg:%d,sg=%d\n",lgain,sgain);
//    u8 gain_s,gain_s1,gain_s2,gain_s3,gain_s4,gain_l,gain_l1,gain_l2,gain_l3,gain_l4;
//	u8 exp_s1,exp_s2,exp_l1,exp_l2,exp_l3;
//    rd_sc2313_reg((u16)0x3e13, &gain_s1);
//    rd_sc2313_reg((u16)0x3e12, &gain_s2);
//    rd_sc2313_reg((u16)0x3e11, &gain_s3);
//    rd_sc2313_reg((u16)0x3e10, &gain_s4);
//
//
//    rd_sc2313_reg((u16)0x3e09, &gain_l1);
//    rd_sc2313_reg((u16)0x3e08, &gain_l2);
//    rd_sc2313_reg((u16)0x3e07, &gain_l3);
//    rd_sc2313_reg((u16)0x3e06, &gain_l4);
//
//
//
//    rd_sc2313_reg((u16)0x3e05, &exp_s1);
//    rd_sc2313_reg((u16)0x3e04, &exp_s2);
//
//
//
//    rd_sc2313_reg((u16)0x3e02, &exp_l1);
//    rd_sc2313_reg((u16)0x3e01, &exp_l2);
//    rd_sc2313_reg((u16)0x3e00, &exp_l3);
//
//    printf("0x3e13:%x 0x3e12:%x 0x3e11:%x 0x3e10:%x\n",gain_s1,gain_s2,gain_s3,gain_s4);
//    printf("0x3e09:%x 0x3e08:%x 0x3e07:%x 0x3e06:%x\n",gain_l1,gain_l2,gain_l3,gain_l4);
//    printf("0x3e05:%x 0x3e04:%x\n",exp_s1,exp_s2);
//    printf("0x3e02:%x 0x3e01:%x 0x3e00:%x\n",exp_l1,exp_l2,exp_l3);

    return -1;
}

void sc2313_dvp_sleep()
{


}

void sc2313_dvp_wakeup()
{


}

void sc2313_dvp_wr_reg(u16 addr, u16 val)
{
    printf("update reg%x with %x\n", addr, val);
    wr_sc2313_reg((u16)addr, (u8)val);
}
u16 sc2313_dvp_rd_reg(u16 addr)
{
    u8 val;
    rd_sc2313_reg((u16)addr, &val);
    return val;
}

REGISTER_CAMERA(sc2313_dvp) = {
    .logo 				= 	"sc2313d",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_BGGR,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
    .mbus_type          =   SEN_MBUS_PARALLEL,
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B,// | SEN_MBUS_PCLK_SAMPLE_FALLING,
    .fps         		= 	30, // 25

    .sen_size 			= 	{SC2313_DVP_OUTPUT_W, SC2313_DVP_OUTPUT_H},
    .isp_size 			= 	{SC2313_DVP_OUTPUT_W, SC2313_DVP_OUTPUT_H},

    .cap_fps         		= 	30, // 25
    .sen_cap_size 			= 	{SC2313_DVP_OUTPUT_W, SC2313_DVP_OUTPUT_H},
    .isp_cap_size 			= 	{SC2313_DVP_OUTPUT_W, SC2313_DVP_OUTPUT_H},

    .hdr_mode               =  ISP_HDR_NVC_SS,
    .hdr_prefetch_lines     =  MAX_SHORT_DELAY_LINE - 5,
    .hdr_short_delay_lines  =  MAX_SHORT_DELAY_LINE,
    .hdr_short_gain         =  0x1000,  // 1 times
    .hdr_long_gain          =  0x100,   // 1/16 times

    .ops                =   {
        .avin_fps           =   NULL,
        .avin_valid_signal  =   NULL,
        .avin_mode_det      =   NULL,
        .sensor_check 		= 	sc2313_dvp_check,
        .init 		        = 	sc2313_dvp_init,
        .set_size_fps 		=	sc2313_dvp_set_output_size,
        .power_ctrl         =   sc2313_dvp_power_ctl,

        .get_ae_params  	=	sc2313_dvp_get_ae_params,
        .get_awb_params 	=	sc2313_dvp_get_awb_params,
        .get_iq_params 	    	=	sc2313_dvp_get_iq_params,

        .sleep 		        =	sc2313_dvp_sleep,
        .wakeup 		    =	sc2313_dvp_wakeup,
        .write_reg 		    =	sc2313_dvp_wr_reg,
        .read_reg 		    =	sc2313_dvp_rd_reg,

    }
};



