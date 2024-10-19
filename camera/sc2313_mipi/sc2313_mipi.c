
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "sc2313_mipi.h"
#include "asm/isp_alg.h"

static u32 reset_gpios[2] = {-1, -1};
static u32 pwdn_gpios[2] = {-1, -1};

extern void *sc2313_mipi_get_ae_params();
extern void *sc2313_mipi_get_awb_params();
extern void *sc2313_mipi_get_iq_params();
extern void sc2313_mipi_ae_ev_init(u32 fps);

static u32 isp_dgain;
#if SC2313_HDR_EN

#define SC2313_HDR_15FPS 0

#if !SC2313_HDR_15FPS

#define INPUT_CLK  24
#define PCLK  76

#define LINE_LENGTH_CLK       0x476
#define FRAME_LENGTH          0x8aa
#define ROW_TIME_NS           15026    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)

#define MAX_SHORT_EXP 	 	134//216  		//  (FRAME_LENGTH - 5 + 64  + 16)/17
#define MAX_SHORT_DELAY_LINE 	 46//109 // 166/2+1

#define MAX_SHORT_EXP_LINE 	 	((MAX_SHORT_EXP - 4 )*2)
#define MAX_LONG_EXP_LINE 	 	((FRAME_LENGTH - MAX_SHORT_EXP - 5 )*2)

#else
#define INPUT_CLK  24
#define PCLK  40 //40.5

#define LINE_LENGTH_CLK       0x465
#define FRAME_LENGTH          0x960
#define ROW_TIME_NS           27778    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)

#define MAX_SHORT_EXP 	 	144//216  		//  (FRAME_LENGTH - 5 + 64  + 16)/17
#define MAX_SHORT_DELAY_LINE 	 73//109 // 145/2+1

#define MAX_SHORT_EXP_LINE 	 	((MAX_SHORT_EXP - 4 )*2)
#define MAX_LONG_EXP_LINE 	 	((FRAME_LENGTH - MAX_SHORT_EXP - 5 )*2)

#endif

#else

#define INPUT_CLK  24
#define PCLK  81

#define LINE_LENGTH_CLK       2250
#define FRAME_LENGTH          1200
#define ROW_TIME_NS           27778    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)
#define MAX_LONG_EXP_LINE 	 	((FRAME_LENGTH - 2 )*2)

#endif


static u32 cur_again = -1;
static u32 cur_dgain = -1;
static u32 cur_expline = -1;
static u32 cur_again_s = -1;
static u32 cur_dgain_s = -1;
static u32 cur_expline_s = -1;
static u32 cur_line_length_clk = -1;

static u32 cur_againx = 0;

static u32 line_length_clk = LINE_LENGTH_CLK;

static void *iic = NULL;

static u8 WRCMD = 0x60;
static u8 RDCMD = 0x61;


typedef struct {
    u16 addr;
    u8 value;
} Sensor_reg_ini;


Sensor_reg_ini sc2313_mipi_INI_REG[] = {


#if SC2313_HDR_EN
#if !SC2313_HDR_15FPS
    0x0103, 0x01,
    0x0100, 0x00,
    0x36e9, 0xa6, //bypass pll1
    0x36f9, 0x85, //bypass pll2
    0x4509, 0x10,
    0x337f, 0x03,
    0x3368, 0x04,
    0x3369, 0x00,
    0x336a, 0x00,
    0x336b, 0x00,
    0x3367, 0x08,
    0x3326, 0x00,
    0x3631, 0x88,
    0x3018, 0x33,
    0x3031, 0x0a,
    0x3001, 0xfe,
    0x4603, 0x00,
    0x303f, 0x01,
    0x3640, 0x00,
    0x3907, 0x01,
    0x3908, 0x01,
    0x3320, 0x01,
    0x57a4, 0xf0,
    0x3333, 0x30,
    0x331b, 0x83,
    0x3334, 0x40,
    0x3302, 0x10,
    0x36eb, 0x0a,
    0x36ec, 0x0e,
    0x3f08, 0x04,
    0x4501, 0xa4,
    0x3309, 0x48,
    0x331f, 0x39,
    0x330a, 0x00,
    0x3308, 0x10,
    0x3366, 0xc0,
    0x33aa, 0x00,
    0x391e, 0x00,
    0x391f, 0xc0,
    0x3634, 0x44,
    0x4500, 0x59,
    0x3623, 0x18,
    0x3f00, 0x0d,
    0x336c, 0x42,
    0x3933, 0x28,
    0x3934, 0x0a,
    0x3940, 0x1b,
    0x3941, 0x40,
    0x3942, 0x08,
    0x3943, 0x0e,
    0x3624, 0x47,
    0x3621, 0xac,
    0x3222, 0x29,
    0x3901, 0x02,
    0x363b, 0x08,
    0x363c, 0x05,
    0x363d, 0x05,
    0x3324, 0x02,
    0x3325, 0x02,
    0x333d, 0x08,
    0x3314, 0x04,
    0x3802, 0x00,
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
    0x33bd, 0x18,
    0x33be, 0x18,
    0x33bf, 0x18,
    0x360f, 0x05,
    0x367a, 0x40,
    0x367b, 0x40,
    0x3671, 0xf6,
    0x3672, 0x16,
    0x3673, 0x16,
    0x366e, 0x04,
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
    0x3696, 0x9f,
    0x3697, 0x9f,
    0x3698, 0x9f,
    0x301c, 0x78,
    0x3037, 0x24,
    0x3038, 0x44,
    0x3632, 0x18,
    0x4809, 0x01,
    0x3625, 0x01,
    0x3670, 0x6a,
    0x369e, 0x40,
    0x369f, 0x40,
    0x3693, 0x20,
    0x3694, 0x40,
    0x3695, 0x40,
    0x5000, 0x06,
    0x5780, 0x7f,
    0x57a0, 0x00,
    0x57a1, 0x74,
    0x57a2, 0x01,
    0x57a3, 0xf4,
    0x5781, 0x06,
    0x5782, 0x04,
    0x5783, 0x02,
    0x5784, 0x01,
    0x5785, 0x16,
    0x5786, 0x12,
    0x5787, 0x08,
    0x5788, 0x02,
    0x3637, 0x0c,
    0x3638, 0x24,
    0x3200, 0x00,
    0x3201, 0x04,
    0x3202, 0x00,
    0x3203, 0x00,
    0x3204, 0x07,
    0x3205, 0x8b,
    0x3206, 0x04,
    0x3207, 0x3f,
    0x3208, 0x07,
    0x3209, 0x80,
    0x320a, 0x04,
    0x320b, 0x38,
    0x3211, 0x04,
    0x3213, 0x04,
    0x3380, 0x1b,
    0x3341, 0x07,
    0x3343, 0x03,
    0x3e25, 0x03,
    0x3e26, 0x40,
    0x391d, 0x24,
    0x36ea, 0x2d,
    0x36ed, 0x23,
    0x36fa, 0x6a,
    0x36fb, 0x20,
    0x320c, 0x04,
    0x320d, 0x76,
    0x3636, 0xa8,
    0x3f04, 0x02,
    0x3f05, 0x33,
    0x4837, 0x1a,
    0x331e, 0x21,
    0x3303, 0x30,
    0x330b, 0xb8,
    0x3306, 0x5c,
    0x330e, 0x30,
    0x4816, 0x51,
    0x3220, 0x51,
    0x4602, 0x0f,
    0x33c0, 0x05,
    0x6000, 0x06,
    0x6002, 0x06,
    0x320e, 0x08,
    0x320f, 0xaa,
    0x3e00, 0x01,
    0x3e01, 0x03,
    0x3e02, 0xe0,
    0x3e04, 0x10,
    0x3e05, 0x40,
    0x3e23, 0x00,
    0x3e24, 0x86,
    0x3e03, 0x0b,
    0x3e06, 0x00,
    0x3e07, 0x80,
    0x3e08, 0x03,
    0x3e09, 0x40,
    0x3622, 0xf6,
    0x3633, 0x22,
    0x3630, 0xc8,
    0x3301, 0x10,
    0x363a, 0x83,
    0x3635, 0x20,
    0x36e9, 0x40,
    0x36f9, 0x05,
    0x0100, 0x01,
    //add dgain_s
    0x3e10, 0x00,
    0x3e11, 0x80,
#else
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
    0x3f00, 0x0d, //[2]   hts/2-4-{3f08}
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
    0x3209, 0x80,
    0x320a, 0x04,
    0x320b, 0x38,
    0x3211, 0x08,
    0x3213, 0x08,

//20180511	81M
    0x36e9, 0xa3,
    0x36ea, 0x77,
    0x36eb, 0x0b,
    0x36ec, 0x0f,
    0x36ed, 0x03,
    0x36f9, 0x85,
    0x36fa, 0x2d,
    0x36fb, 0x10,

    0x320c, 0x04, //hts=1125*2=2250
    0x320d, 0x65,
    0x320e, 0x04, //vts=1200
    0x320f, 0xb0,
    0x3235, 0x12, //vts*2-0x02
    0x3236, 0xbe,
    0x3f04, 0x02, //{0x320c,0x320d}/2-0x3f08-0x04
    0x3f05, 0x2a,

//20180514
    0x3802, 0x00,

//20180604
    0x3624, 0x47,
    0x3621, 0xac,

//20180607
    0x36fa, 0x2f,

//0612
    0x3637, 0x08,
    0x3638, 0x25,
    0x3635, 0x40,
    0x363b, 0x08,
    0x363c, 0x05,
    0x363d, 0x05,

//20180619
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

//20170717
    0x4837, 0x31,

//20180730
    0x6000, 0x00,
    0x6002, 0x00,
    0x301c, 0x78, //close dvp

//20180822
    0x3037, 0x44, //[3:0] pump div	range [10M,20M],sclk=81/2=40.5M,div=4-->sclk/4=10.125M,duty cycle-->even number
    0x3038, 0x44, //[7:4]ppump & [3:0]npump
    0x3632, 0x18, //[5:4]idac driver
    0x5785, 0x40, //black	point 1x

//20180824
    0x4809, 0x01, //mipi first frame, lp status

//20181120	modify analog fine gain
    0x3637, 0x10,
//20181120
    0x5000, 0x06, //dpc enable
    0x5780, 0x7f, //auto blc setting
    0x57a0, 0x00,	//gain0 = 2x	0x0740至0x077f
    0x57a1, 0x74,
    0x57a2, 0x01,	//gain1 = 8x	0x1f40至0x1f7f
    0x57a3, 0xf4,
    0x5781, 0x06,	//white	1x
    0x5782, 0x04,	//2x
    0x5783, 0x02,	//8x
    0x5784, 0x01,	//128x
    0x5785, 0x16,	//black	1x
    0x5786, 0x12,	//2x
    0x5787, 0x08,	//8x
    0x5788, 0x02,	//128x

    0x4501, 0xb4, //reduce bit
    0x3637, 0x20,
    0x4509, 0x20, //blc quantification	//20181206
//0x331f,0x29,
//0x3309,0x30,
//0x330a,0x00,
//0x330b,0xc8, //[aa,e0]
//0x3306,0x60, //[34,78]
//0x330e,0x28,
////0x3933,0x30,//20181206
////0x3942,0x10,

//20181120 digital logic
//3301 auto logic read 0x3373 for auto value
    0x3364, 0x1d, //[4] fine gain op 1~20--3f 0~10--1f [4] ana dithring en
    0x33b6, 0x07, //gain0 when dcg off	gain<dcg
    0x33b7, 0x07, //gain1 when dcg off
    0x33b8, 0x10, //sel0 when dcg off
    0x33b9, 0x10, //sel1 when dcg off
    0x33ba, 0x10, //sel2 when dcg off
    0x33bb, 0x07, //gain0 when dcg on		gain>=dcg
    0x33bc, 0x07, //gain1 when dcg on
    0x33bd, 0x20, //sel0 when dcg on
    0x33be, 0x20, //sel1 when dcg on
    0x33bf, 0x20, //sel2 when dcg on
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
    0x3678, 0x33, //sel1
    0x3679, 0x44, //sel2
//363a auto logic read 0x3685 for auto value
    0x36a0, 0x58, //gain0  3e08[5:2] 1011
    0x36a1, 0x78, //gain1  3e08[5:2] 1111
    0x3696, 0x9f, //sel0
    0x3697, 0x9f, //sel1
    0x3698, 0x9f, //sel2

//20181122
    0x3637, 0x17, //fullwell 8.6k

//sysclk=81/2=40.5
//countclk=486	12:1
    0x331e, 0x11,
    0x331f, 0x21, //1d
    0x3303, 0x1c,	//[hl,to][1,1a,2e]
    0x3309, 0x3c,	//[hl,to][1,32,46]
    0x330a, 0x00,
    0x330b, 0xc8,	//[bs,to][1,a8,ec]
    0x3306, 0x68,	//[hl,bs][1,46,88]
//0x3301,0x10,0x20,//[hl,to][1,8-11,32]
//0x330e,0x30,	//[lag][8]
//0x3367,0x08,
//fullwell 8.6k
//noise 0.98e

//20181123 window	1920x1080 centered
    0x3200, 0x00,
    0x3201, 0x04,
    0x3202, 0x00,
    0x3203, 0x04,
    0x3204, 0x07,
    0x3205, 0x8b,
    0x3206, 0x04,
    0x3207, 0x43,
    0x3208, 0x07,
    0x3209, 0x80,
    0x320a, 0x04,
    0x320b, 0x38,
    0x3211, 0x04,
    0x3213, 0x04,

//20181205	debug: flip aec bug
    0x3380, 0x1b,
    0x3341, 0x07, //3318[3:0] + 2
    0x3343, 0x03, //3318[3:0] -2
    0x3e25, 0x03, //blc dithering(analog fine gain)
    0x3e26, 0x40,

//20190130
    0x3366, 0x70, //[60,78]

//init
//20190506_HDR-15FPS
    0x4816, 0x51, //bit[4]
    0x3220, 0x51, //bit[6]
    0x4602, 0x0f, //bit[3:0]
    0x33c0, 0x05, //bit[2]
    0x6000, 0x06,
    0x6002, 0x06,
    0x320e, 0x09, //double vts
    0x320f, 0x60,

//
    0x3202, 0x00, //x_start must be 0x00
    0x3203, 0x00,
    0x3206, 0x04, //1088	activeBoard=4
    0x3207, 0x3f,

//init
    0x3e00, 0x01, //max long exposure = 0x103e
    0x3e01, 0x19,
    0x3e02, 0x60,
    0x3e04, 0x11, //max short exposure = 0x104
    0x3e05, 0x80,
    0x3e23, 0x00, //max long exp : max short exp <= 16:1
    0x3e24, 0x90,
    0x3e03, 0x0b, //gain map 0x0b mode	gain=1x
    0x3e06, 0x00,
    0x3e07, 0x80,
    0x3e08, 0x03,
    0x3e09, 0x40,
//0x3e03,0x03,//gain map 0x03 mode	gain=1x
//0x3e1e,0xb5,
//0x3e06,0x00,
//0x3e07,0x80,
//0x3e08,0x00,
//0x3e09,0x40,

    0x36e9, 0x23, //enable pll1	20180830
    0x36f9, 0x05, //enable pll2	20180830

    0x0100, 0x01,
#endif

#else





    0x0103, 0x01,
    0x0100, 0x00,

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

    0x36e9, 0x03,
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
    0x36e9, 0x23,
    0x36eb, 0x0b,
    0x36ec, 0x0f,
    0x3638, 0x27,

    0x33aa, 0x00, //power save mode
    0x3624, 0x02,
    0x3621, 0xac,


//aec  analog gain: 0x570 (15.75 x 2.7=41.85)   2xdgain :0xae0  4xdgain : 0x15c0  8xdgain : 0x2b80 16xdgain : 0x5700 3e0a 3e0b real ana gain  3e2c 3e2d real digital fine gain

    0x3e03, 0x0b,
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
    0x3f00, 0x0d, //[2]   hts/2-4-{3f08}
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
    0x36e9, 0x23,
    0x36ea, 0x77,
    0x36eb, 0x0b,
    0x36ec, 0x0f,
    0x36ed, 0x03,
    0x36f9, 0x05,
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

//0612
    0x3637, 0x08,
    0x3638, 0x25,
    0x3635, 0x40,
    0x363b, 0x08,
    0x363c, 0x05,
    0x363d, 0x05,

//20180619
    0x3303, 0x1c,
    0x3309, 0x1c,

    0x0100, 0x01,

#endif
};


unsigned char wrsc2313_mipi_Reg(u16 regID, unsigned char regDat)
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
    if (ret == 0) {
        printf("wreg iic fail\n");
    }
    if (regID == 0x3e01) {
        //printf("wr 0x3e01 with %02x\n", regDat);
    }
    return ret;

}

unsigned char rdsc2313_mipi_Reg(u16 regID, unsigned char *regDat)
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

void sc2313_mipi_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    u32 i;
    u8 v;


    sc2313_mipi_set_output_size(width, height, frame_freq);

    for (i = 0; i < sizeof(sc2313_mipi_INI_REG) / sizeof(Sensor_reg_ini); i++) {
        wrsc2313_mipi_Reg(sc2313_mipi_INI_REG[i].addr, sc2313_mipi_INI_REG[i].value);
    }

    sc2313_mipi_ae_ev_init(*frame_freq);



    *format = SEN_IN_FORMAT_BGGR;
    return;
}


s32 sc2313_mipi_set_output_size(u16 *width, u16 *height, u8 *frame_freq)
{
    *width = sc2313_MIPI_OUTPUT_W;
    *height = sc2313_MIPI_OUTPUT_H;

    return 0;
}


s32 sc2313_mipi_power_ctl(u8 isp_dev, u8 is_work)
{

    return 0;
}

s32 sc2313_mipi_ID_check(void)
{
    u8 pid = 0x00;
    u8 ver = 0x00;
    u8 i ;

    for (i = 0; i < 3; i++) { //
        rdsc2313_mipi_Reg(0x3107, &pid);
        rdsc2313_mipi_Reg(0x3108, &ver);
    }

    puts("Sensor PID \n");
    put_u32hex(pid);
    put_u32hex(ver);
    puts("\n");

    if (pid != 0x23 || ver != 0x11) {
        puts("\n----not sc2313_mipi-----\n");
        return -1;
    }
    puts("\n----hello sc2313_mipi-----\n");
    return 0;
}

void sc2313_mipi_reset(u8 isp_dev)
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


static u8 cur_sensor_type = 0xff;

s32 sc2313_mipi_check(u8 isp_dev, u32 reset_gpio, u32 pwdn_gpio)
{

    puts("\n\n sc2313_mipi_check \n\n");
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

    sc2313_mipi_reset(isp_dev);

    if (0 != sc2313_mipi_ID_check()) {
        if (0x60 == WRCMD) {
            WRCMD = 0x64;
            RDCMD = 0x65;
        } else {
            WRCMD = 0x60;
            RDCMD = 0x61;
        }
        if (0 != sc2313_mipi_ID_check()) {
            dev_close(iic);
            iic = NULL;
            return -1;
        }

    }

//    strcpy(sensor_logo, "SR013");
    cur_sensor_type = isp_dev;

    return 0;
}


s32 sc2313_mipi_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    sc2313_mipi_reset(isp_dev);
    puts("\n\n sc2313_mipi_init22 \n\n");

    sc2313_mipi_config_SENSOR(width, height, format, frame_freq);
    //add init isp_dgain
    isp_dgain = 256;
    return 0;
}

static void set_again(u32 again)
{
    //printf("again=%d",again);
    if (cur_again == again) {
        //return;
    }
    cur_again  = again;
    //printf("again = %04x\n",again);
    wrsc2313_mipi_Reg(0x3e08, ((again >> 8) & 0xfc) | 0x03);
    wrsc2313_mipi_Reg(0x3e09, again & 0xff);
    return;
}

static void set_dgain(u32 dgain)
{
    //printf("dgain=%d",dgain);
    if (cur_dgain == dgain) {
        //return;
    }
    cur_dgain  = dgain;
    //printf("dgain = %04x\n", dgain);
    wrsc2313_mipi_Reg(0x3e06, (dgain >> 8) & 0xff);
    wrsc2313_mipi_Reg(0x3e07, (dgain >> 0) & 0xff);
}


static void set_again_s(u32 again)
{
    //printf("again_s=%d",again);
    if (cur_again_s == again) {
        //return;
    }
    cur_again_s  = again;
    //printf("again_s = %04x\n",again);
    wrsc2313_mipi_Reg(0x3e12, ((again >> 8) & 0xfc) | 0x03);
    wrsc2313_mipi_Reg(0x3e13, again & 0xff);
    return;
}

static void set_dgain_s(u32 dgain)
{
    //printf("dgain_s=%d",dgain);
    if (cur_dgain_s == dgain) {
        //return;
    }
    cur_dgain_s  = dgain;
    //printf("dgain_s = %04x\n", dgain);
    wrsc2313_mipi_Reg(0x3e10, (dgain >> 8) & 0xff);
    wrsc2313_mipi_Reg(0x3e11, (dgain >> 0) & 0xff);
}



static u32 convertagain2cfg(u32 val)
{
    u32 course = 1;
    u32 finegain;

    while (val >= 2048) {
        course *= 2;
        val /= 2;
    }
    finegain = val / 32;
    return ((course - 1) << 10) + finegain;
}
static u32 convertdgain2cfg(u32 val)
{
    u32 course = 1;
    u32 finegain;

    while (val >= 2048) {
        course *= 2;
        val /= 2;
    }
    val -= 1024;
    finegain = 0x80 + val / 8;
    return ((course - 1) << 8) + finegain;
}

#define MAX_AGAIN (16*1024 -512)
#define MAX_DGAIN (32*1024 -512)
#define DCG_GAIN 2786


static void calc_gain(u32 gain, u32 *_again, u32 *_dgain)
{
    int i;
    u32 reg0, reg1, reg2;
    u32 dgain, again, dcggain = 0;

    if (gain < 1024) {
        gain = 1024;
    }
    if (gain > 260 * 1024) {
        gain = 260 * 1024;
    }
    //printf("gain = %d ", gain);
    if (gain >= DCG_GAIN) { //dcg gain on/off
        dcggain = 1;
        gain *= 1024;
        gain /= DCG_GAIN;
    }
    dgain = gain * 1024 / MAX_AGAIN;
    if (dgain < 1024) {
        dgain = 1024;
    }
    if (dgain > MAX_DGAIN) {
        dgain = MAX_DGAIN;
    }
    //////////////////////////
    //isp0_hdr_merge_gain=dgain/4;//q8
    //add
    isp_dgain = dgain / 4;
    ///////////////////////////
    again = gain * 1024 / dgain;

    if (again < 1024) {
        again = 1024;
    }
    if (again >= MAX_AGAIN) {
        again = MAX_AGAIN;
    }
    cur_againx = again;
    *_again = (dcggain << 13) + convertagain2cfg(again);
    *_dgain = convertdgain2cfg(dgain);;

    //printf(" dg = %d; ag= %d; \n ", dgain, again);
}


static void set_shutter(u32 texp)
{
    /*printf("texp=%d",texp);
    u8 wval, rval;
    if (cur_expline == texp) {
        //return;
    }
    cur_expline  = texp;
    //texp <<= 4;
    //printf("texp = %d\n", texp);

    wval = (u8)((texp >> 12)&0x0f);
    printf("wval1=%d",wval);
    wrsc2313_mipi_Reg(0x3e00, wval);
    wval = (u8)((texp >> 4)&0xff);
    printf("wval2=%d",wval);
    wrsc2313_mipi_Reg(0x3e01, wval);
    wval = (u8)((texp<<4)&0xff);
    printf("wval3=%d",wval);
    wrsc2313_mipi_Reg(0x3e02, wval);*/
    //printf("texp=%d",texp);
    u8 wval, rval;
    if (cur_expline == texp) {
        //return;
    }
    cur_expline  = texp;
    texp <<= 4;
    //printf("texp = %d\n", texp);

    wval = (u8)((texp >> 16) & 0x0f);
    //printf("wval1=%d",wval);
    wrsc2313_mipi_Reg(0x3e00, wval);
    wval = (u8)((texp >> 8) & 0xff);
    //printf("wval2=%d",wval);
    wrsc2313_mipi_Reg(0x3e01, wval);
    wval = (u8)(texp & 0xff);
    //printf("wval3=%d",wval);
    wrsc2313_mipi_Reg(0x3e02, wval);
}

static void set_shutter_s(u32 texp)
{
    /*printf("texp_s=%d",texp);
    u8 wval, rval;
    if (cur_expline_s == texp) {
        //return;
    }
    cur_expline_s  = texp;
    //texp <<= 4;
    //printf("texp = %d\n", texp);


    wval = (u8)((texp >> 4)&0xff);
    wrsc2313_mipi_Reg(0x3e04, wval);
    wval = (u8)((texp<<4)&0xff);
    wrsc2313_mipi_Reg(0x3e05, wval);*/
    //printf("texp_s=%d",texp);
    u8 wval, rval;
    if (cur_expline_s == texp) {
        //return;
    }
    cur_expline_s  = texp;
    texp <<= 4;
    //printf("texp = %d\n", texp);


    wval = (u8)((texp >> 8) & 0xff);
    wrsc2313_mipi_Reg(0x3e04, wval);
    wval = (u8)(texp & 0xff);
    wrsc2313_mipi_Reg(0x3e05, wval);
}

u32 sc2313_mipi_calc_shutter(isp_ae_shutter_t *shutter, u32 exp_time_us, u32 gain)
{
    u32 texp;
    u32 texp_align;
    u32 ratio;
    u32 fLen;
    static int bLow = 0;

    texp = exp_time_us * 1000 * 2 / ROW_TIME_NS;

#if SC2313_HDR_EN
    if (texp < 16 * 5) {
        texp = 16 * 5;
    }
#else
    if (texp < 5) {
        texp = 5;
    }
#endif
    //printf("texp=%d",texp);
    if (texp > MAX_LONG_EXP_LINE) {
        texp = MAX_LONG_EXP_LINE - 1;
    }

    texp_align = (texp) * ROW_TIME_NS / (1000 * 2);

    if (texp_align < exp_time_us) {
        ratio = (exp_time_us) * (1 << 10) / texp_align;
        //printf("ratio = %d\n",ratio);
    } else {
        ratio = (1 << 10);
    }
    if (ratio < (1 << 10)) {
        ratio = (1 << 10);
    }

    gain = (gain * ratio) >> 10;

    if (gain > 30 * 1024) {
        gain = 30 * 1024;
    }

#if 0
    static int flag = 0;
    if (flag) {
        texp = 1296;
        gain = 1024;
        flag = 0;
    } else {
        flag = 1;
        texp = 1328;
        gain = 1036;
    }
#endif
#if SC2313_HDR_EN
    shutter->ae_exp_line =  texp & (~0xf);
#else
    shutter->ae_exp_line =  texp;
#endif
    shutter->ae_gain = gain;//1024;//gain;
    shutter->ae_exp_clk = 0;
    shutter->ae_exp_line_short =  texp / 16;
    shutter->ae_gain_short = gain;//gain;// * texp / (texp & (~0xf));
    shutter->ae_exp_clk_short = 0;

    //add
    shutter->isp_d_gain = isp_dgain;

    //printf("exp_time_us=%d, texp=%d, gain=%d->%d\n", exp_time_us, texp, gain,shutter->ae_gain);
    return 0;

}
static void set_log()
{
    /*
    [gain<2]
    0x3622,0xf6, //20180612
    0x3633,0x22,
    0x3630,0xc8,
    0x3301,0x11, //[6,14] //0611

    [gain>=2]
    0x3622,0x16, //20180612
    0x3633,0x55, //20180619
    0x3630,0x08,
    0x3301,0x14,//[9,14] //0326
    */
    u32 gainx = cur_againx / 1024;
    if (gainx < 2) {
        wrsc2313_mipi_Reg(0x3622, 0xf6);
        wrsc2313_mipi_Reg(0x3633, 0x22);
        wrsc2313_mipi_Reg(0x3630, 0xc8);
        wrsc2313_mipi_Reg(0x3301, 0x11);
    } else if (gainx >= 2) {
        wrsc2313_mipi_Reg(0x3622, 0x16);
        wrsc2313_mipi_Reg(0x3633, 0x55);
        wrsc2313_mipi_Reg(0x3630, 0x08);
        wrsc2313_mipi_Reg(0x3301, 0x14);
    }
}

u32 sc2313_mipi_set_shutter(isp_ae_shutter_t *shutter)
{
    u32 again, dgain;
    u32 again_s, dgain_s;

#if SC2313_HDR_EN
    calc_gain((shutter->ae_gain_short), &again_s, &dgain_s);
    calc_gain((shutter->ae_gain), &again, &dgain);
    wrsc2313_mipi_Reg(0x3812, 0x00);

    /*if (cur_expline != shutter->ae_exp_line
            || cur_expline_s != shutter->ae_exp_line_short
            || cur_again != again
            || cur_again_s != again_s
            || cur_dgain != dgain
            || cur_dgain_s != dgain_s) {
        set_shutter(shutter->ae_exp_line);
        set_shutter_s(shutter->ae_exp_line_short);
        set_again(again);
        set_again_s(again_s);
        set_dgain(dgain);
        set_dgain_s(dgain_s);
    }*/

    set_shutter(shutter->ae_exp_line);
    set_shutter_s(shutter->ae_exp_line_short);
    set_again(again);
    set_again_s(again_s);
    //set_dgain(dgain);
    //set_dgain_s(dgain_s);

    //set_log();
    wrsc2313_mipi_Reg(0x3812, 0x30);
    //if (shutter->ae_exp_line > 160) {
    //    wrsc2313_mipi_Reg(0x3314, 0x04);
    //}
    //if (shutter->ae_exp_line < 80) {
    //    wrsc2313_mipi_Reg(0x3314, 0x14);
    //}

#else

    calc_gain((shutter->ae_gain), &again, &dgain);
    wrsc2313_mipi_Reg(0x3812, 0x00);
    //shutter->ae_exp_line=100;

    //if (cur_expline != shutter->ae_exp_line) {
    set_shutter(shutter->ae_exp_line);
    //}

    //if (cur_again != again
    //      || cur_dgain != dgain) {
    set_again(again);
    set_dgain(dgain);
    //}



    wrsc2313_mipi_Reg(0x3812, 0x30);

    set_log();

    if (shutter->ae_exp_line > 160) {
        wrsc2313_mipi_Reg(0x3314, 0x04);
    }
    if (shutter->ae_exp_line < 80) {
        wrsc2313_mipi_Reg(0x3314, 0x14);
    }
#endif
    //printf("shutter->ae_exp_line=%d",shutter->ae_exp_line);
    //printf("again=%d",again);
    //printf("dgain=%d",dgain);
    return 0;
}


void sc2313_mipi_sleep()
{


}

void sc2313_mipi_wakeup()
{


}

void sc2313_mipi_W_Reg(u16 addr, u16 val)
{
    /*printf("update reg%x with %x\n", addr, val);*/
    wrsc2313_mipi_Reg((u16)addr, (u8)val);
}
u16 sc2313_mipi_R_Reg(u16 addr)
{
    u8 val;
    rdsc2313_mipi_Reg((u16)addr, &val);
    printf("read reg%x val %x\n", addr, val);
    return val;
}

REGISTER_CAMERA(sc2313_mipi) = {
    .logo 				= 	"sc2313m",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_BGGR,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
    .mbus_type          =   SEN_MBUS_CSI2,
#if SC2313_HDR_EN
#if SC2313_HDR_15FPS
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_12B | SEN_MBUS_CSI2_2_LANE,// | SEN_MBUS_CSI2_HDR_VC_INV,
#else
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_10B | SEN_MBUS_CSI2_2_LANE,// | SEN_MBUS_CSI2_HDR_VC_INV,
#endif
#else
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_12B | SEN_MBUS_CSI2_2_LANE,
#endif
    .fps         		= 	30,

    .sen_size 			= 	{sc2313_MIPI_OUTPUT_W, sc2313_MIPI_OUTPUT_H},
    .isp_size 			= 	{sc2313_MIPI_OUTPUT_W, sc2313_MIPI_OUTPUT_H},

    .cap_fps         		= 	30,
    .sen_cap_size 			= 	{sc2313_MIPI_OUTPUT_W, sc2313_MIPI_OUTPUT_H},
    .isp_cap_size 			= 	{sc2313_MIPI_OUTPUT_W, sc2313_MIPI_OUTPUT_H},

#if SC2313_HDR_EN
    .hdr_mode               =  ISP_HDR_VC_SS,
    .hdr_prefetch_lines     =  MAX_SHORT_DELAY_LINE - 5,
    .hdr_short_delay_lines  =  MAX_SHORT_DELAY_LINE,
    .hdr_short_gain         =  0x1000,  // 1 times
    .hdr_long_gain          =  0x100,   // 1/16 times
#endif
    //add
    .isp_d_gain_en      = 0,
    .delay_frame        = 4,

    .ops                =   {
        .avin_fps           =   NULL,
        .avin_valid_signal  =   NULL,
        .avin_mode_det      =   NULL,
        .sensor_check 		= 	sc2313_mipi_check,
        .init 		        = 	sc2313_mipi_init,
        .set_size_fps 		=	sc2313_mipi_set_output_size,
        .power_ctrl         =   sc2313_mipi_power_ctl,

        .get_ae_params  	=	sc2313_mipi_get_ae_params,
        .get_awb_params 	=	sc2313_mipi_get_awb_params,
        .get_iq_params 	    	=	sc2313_mipi_get_iq_params,

        .sleep 		        =	sc2313_mipi_sleep,
        .wakeup 		    =	sc2313_mipi_wakeup,
        .write_reg 		    =	sc2313_mipi_W_Reg,
        .read_reg 		    =	sc2313_mipi_R_Reg,

    }
};



