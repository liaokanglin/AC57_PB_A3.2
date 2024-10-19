
/*******************************************************************************************
 File Name: GC1034.c

 Version: 1.00

 Discription: GC1034

 Author:xuhuajun

 Email :

 Date:2014.8.19

 Copyright:(c)JIELI  2011  @ , All Rights Reserved.
*******************************************************************************************/
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "gc1034.h"
#include "asm/isp_alg.h"

static u32 reset_gpio[2] = {-1, -1};


extern void *GC1034_get_ae_params();
extern void *gc1034_get_awb_params();
extern void *gc1034_get_iq_params();

extern void gc1034_ae_ev_init(u32 fps);


u32 gc1034_pclk;
u32 gc1034_hblank;
u32 gc1034_vblank;
static u32 gc_pll_div5b;

// hblank = (24000000*(0x09+1)/8./2./(724.+0x11+0x1+0xa)//15 - (16+(1288+24)//4))

typedef struct {
    u8 addr;
    u8 value;
} Sensor_reg_ini;

Sensor_reg_ini GC1034_INI_REG[] = {
    /////////////////////////////////////////////////////
    //////////////////////   SYS   //////////////////////
    /////////////////////////////////////////////////////
    {	0xf2, 0x00},
    {	0xf6, 0x00},
    {	0xfc, 0x04},
    {	0xf7, 0x01},
    {	0xf8, 0x0a},
    {	0xf9, 0x00},
    {	0xfa, 0x80},
    {	0xfc, 0x0e},
    ///////////////////////////////////////////////////
    //////////////   ANALOG & CISCTL   ////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x00},
    {	0x03, 0x02},
    {	0x04, 0xa3},
    {	0x05, 0x01}, //HB
    {	0x06, 0xe1},
    {	0x07, 0x00}, //VB
    {	0x08, 0x0a},
    {	0x09, 0x00},
    {	0x0a, 0x04}, //row start
    {	0x0b, 0x00},
    {	0x0c, 0x00}, //col start
    {	0x0d, 0x02},
    {	0x0e, 0xd4}, //height 724
    {	0x0f, 0x05},
    {	0x10, 0x08}, //width 1288
    {	0x17, 0xc0},
    {	0x18, 0x02},
    {	0x19, 0x08},
    {	0x1a, 0x18},
    {	0x1e, 0x50},
    {	0x1f, 0x80},
    {	0x21, 0x30},
    {	0x23, 0xf8},
    {	0x25, 0x10},
    {	0x28, 0x20},
    {	0x34, 0x08}, //data low
    {	0x3c, 0x10},
    {	0x3d, 0x0e},
    {	0xcc, 0x8e},
    {	0xcd, 0x9a},
    {	0xcf, 0x70},
    {	0xd0, 0xab},
    {	0xd1, 0xc5},
    {	0xd2, 0xed}, //data high
    {	0xd8, 0x3c}, //dacin offset
    {	0xd9, 0x7a},
    {	0xda, 0x12},
    {	0xdb, 0x50},
    {	0xde, 0x0c},
    {	0xe3, 0x60},
    {	0xe4, 0x78},
    {	0xfe, 0x01},
    {	0xe3, 0x01},
    {	0xe6, 0x10}, //ramps offset
    ///////////////////////////////////////////////////
    ////////////////////   ISP   //////////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x01},
    {	0x80, 0x50},
    {	0x88, 0x23}, //check?
    {	0x89, 0x03},
    {	0x90, 0x01},
    {	0x92, 0x02}, //crop win 2<=y<=4
    {	0x94, 0x03}, //crop win 2<=x<=5
    {	0x95, 0x02}, //crop win height
    {	0x96, 0xd0},
    {	0x97, 0x05}, //crop win width
    {	0x98, 0x00},
    ///////////////////////////////////////////////////
    ////////////////////   BLK   //////////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x01},
    {	0x40, 0x22},
    {	0x43, 0x03},
    {	0x4e, 0x3c},
    {	0x4f, 0x00},
    {	0x60, 0x00},
    {	0x61, 0x80}, //0x80 OB-G
    ///////////////////////////////////////////////////
    ////////////////////   GAIN   /////////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x01},
    {	0xb0, 0x48},
    {	0xb1, 0x01},
    {	0xb2, 0x00},
    {	0xb6, 0x00},
    {	0xfe, 0x02},
    {	0x01, 0x00},
    {	0x02, 0x01},
    {	0x03, 0x02},
    {	0x04, 0x03},
    {	0x05, 0x04},
    {	0x06, 0x05},
    {	0x07, 0x06},
    {	0x08, 0x0e},
    {	0x09, 0x16},
    {	0x0a, 0x1e},
    {	0x0b, 0x36},
    {	0x0c, 0x3e},
    {	0x0d, 0x56},
    {	0xfe, 0x02},
    {	0xb0, 0x00}, //col_gain[11:8]
    {	0xb1, 0x00},
    {	0xb2, 0x00},
    {	0xb3, 0x11},
    {	0xb4, 0x22},
    {	0xb5, 0x54},
    {	0xb6, 0xb8},
    {	0xb7, 0x60},
    {	0xb9, 0x00}, //col_gain[12]
    {	0xba, 0xc0},
    {	0xc0, 0x20}, //col_gain[7:0]
    {	0xc1, 0x2d},
    {	0xc2, 0x40},
    {	0xc3, 0x5b},
    {	0xc4, 0x80},
    {	0xc5, 0xb5},
    {	0xc6, 0x00},
    {	0xc7, 0x6a},
    {	0xc8, 0x00},
    {	0xc9, 0xd4},
    {	0xca, 0x00},
    {	0xcb, 0xa8},
    {	0xcc, 0x00},
    {	0xcd, 0x50},
    {	0xce, 0x00},
    {	0xcf, 0xa1},
    ///////////////////////////////////////////////////
    //////////////////   DARKSUN   ////////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x02},
    {	0x54, 0xf7},
    {	0x55, 0xf0},
    {	0x56, 0x00},
    {	0x57, 0x00},
    {	0x58, 0x00},
    {	0x5a, 0x04},
    ///////////////////////////////////////////////////
    /////////////////////   DD   //////////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x04},
    {	0x81, 0x8a},

    {	0x40, 0x10}, // 0x00 ob
    ///////////////////////////////////////////////////
    ////////////////////	 MIPI	/////////////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x03},
    {	0x01, 0x00},
    {	0x02, 0x00},
    {	0x03, 0x00},
    {	0x10, 0x11},
    {	0x15, 0x00},
    {	0x40, 0x01},
    {	0x41, 0x00},
    ///////////////////////////////////////////////////
    ////////////////////   pad enable   ///////////////
    ///////////////////////////////////////////////////
    {	0xfe, 0x00},
    {	0xf2, 0x0f},
};


static void *iic = NULL;


unsigned char wrGC1034Reg(unsigned char regID, unsigned char regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, 0x42)) {
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

unsigned char rdGC1034Reg(unsigned char regID, unsigned char *regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, 0x42)) {
        ret = 0;
        goto __rend;
    }

    delay(10);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regID)) {
        ret = 0;
        goto __rend;
    }

    delay(10);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, 0x43)) {
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

unsigned char wrGC1034RegArray(u8 *array, u32 size)
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



void GC1034_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    u32 i;


    GC1034_set_output_size(width, height, frame_freq);

    for (i = 0; i < sizeof(GC1034_INI_REG) / sizeof(Sensor_reg_ini); i++) {
        wrGC1034Reg(GC1034_INI_REG[i].addr, GC1034_INI_REG[i].value);
    }

    printf("framerate:%d\n", *frame_freq);
    if (*frame_freq == 30) {
        gc1034_hblank = 0x1c8;
        gc1034_vblank = 0x0a;
        gc_pll_div5b = 0x0b;  //0xf8
    } else if (*frame_freq == 25) {
        gc1034_hblank = 0x218;
        gc1034_vblank = 0x0a;
        gc_pll_div5b = 0x0a;  //0xf8
    } else if (*frame_freq == 15) {
        /* gc1034_hblank = 0x340 + 0x340 + 0x340 + 0x340; */
        /* gc1034_hblank = 0x340 + 0x340 + 0x340; */
        /* gc1034_hblank = 0x340 - 0x340 / 2 + 0xf0;  */
        /* gc1034_hblank = 0x340 - 0x340 / 2;  */
        gc1034_hblank = 0x340;
        /* gc1034_hblank = 0x4c8 - 0xf0; */
        gc1034_vblank = 0x0a;
        gc_pll_div5b = 0x08;

        /* gc1034_hblank = 0x238; */
        /* gc1034_vblank = 0x0a; */
        /* gc_pll_div5b = 0x0b; */
    } else {
        gc1034_hblank = 0x4c8;
        gc1034_vblank = 0x0a;
        gc_pll_div5b = 0x08;  //0xf8
    }
    gc1034_pclk = (HOST_MCLK * (gc_pll_div5b + 1) / 8);

    printf("pclk:%d\n", gc1034_pclk);

#if 1
    wrGC1034Reg(0xf8, (gc_pll_div5b & 0x1f));
    wrGC1034Reg(0x05, gc1034_hblank >> 8);
    wrGC1034Reg(0x06, gc1034_hblank & 0xff);
    wrGC1034Reg(0x07, gc1034_vblank >> 8);
    wrGC1034Reg(0x08, gc1034_vblank & 0xff);
#endif
    *format = SEN_IN_FORMAT_RGGB;

    gc1034_ae_ev_init(*frame_freq);


    return;
}


s32 GC1034_set_output_size(u16 *width, u16 *height, u8 *frame_freq)
{

    *width = GC1034_OUTPUT_W;
    *height = GC1034_OUTPUT_H;
    return 0;
}


s32 GC1034_power_ctl(u8 isp_dev, u8 is_work)
{

    return 0;
}

s32 GC1034_ID_check(void)
{
    u8 pid = 0x00;
    u8 ver = 0x00;
    u8 i ;

    for (i = 0; i < 3; i++) { //
        rdGC1034Reg(0xf0, &pid);
        rdGC1034Reg(0xf1, &ver);
    }

    puts("Sensor PID \n");
    put_u32hex(pid);
    put_u32hex(ver);
    puts("\n");

    if (pid != 0x10 || ver != 0x34) {
        puts("\n----not GC1034-----\n");
        return -1;
    }
    puts("\n----hello GC1034-----\n");
    return 0;
}

void GC1034_reset(u8 isp_dev)
{
    u32 gpio;

    if (isp_dev == ISP_DEV_0) {
        gpio = reset_gpio[0];
    } else {
        gpio = reset_gpio[1];
    }

    gpio_direction_output(gpio, 0);
    delay(40000);
    gpio_direction_output(gpio, 1);
    delay(40000);
}


static u8 cur_sensor_type = 0xff;

s32 GC1034_check(u8 isp_dev, u32 _reset_gpio, u32 pwdn_gpio)
{
    puts("\n\n GC1034_check \n\n");
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

    reset_gpio[isp_dev] = _reset_gpio;

    GC1034_reset(isp_dev);

    puts("gc1034_id_check\n");
    if (0 != GC1034_ID_check()) {
        dev_close(iic);
        iic = NULL;
        return -1;
    }

    cur_sensor_type = isp_dev;

    return 0;
}

s32 GC1034_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    GC1034_reset(isp_dev);
    puts("\n\n gc1034_init22 \n\n");

    GC1034_config_SENSOR(width, height, format, frame_freq);

    return 0;
}


u32 gc1034_calc_shutter(isp_ae_shutter_t *shutter, u32 exp_time_us, u32 gain)
{
    u32 line = gc1034_pclk * exp_time_us / (gc1034_hblank + (REG_SH_DELAY + REG_WIN_WIDTH) / 4 + 16) / 2;
    u32 line_time = line * (gc1034_hblank + (REG_SH_DELAY + REG_WIN_WIDTH / 4) + 16) * 2 / gc1034_pclk;
    u32 ratio = (exp_time_us) * (1 << 10) / line_time;

    shutter->ae_exp_line =  line;
    shutter->ae_gain = (((gain * ratio) >> 10) & (~0xf));
    shutter->ae_exp_clk = 0;

    return 0;

}

#define GC1034_ANALOG_GAIN_1 64 // 1.00x
#define GC1034_ANALOG_GAIN_2 90 // 1.4x
#define GC1034_ANALOG_GAIN_3 118 // 1.8x
#define GC1034_ANALOG_GAIN_4 163 // 2.56x
#define GC1034_ANALOG_GAIN_5 218 // 3.40x
#define GC1034_ANALOG_GAIN_6 304 // 4.7x
#define GC1034_ANALOG_GAIN_7 438 // 6.84x
#define GC1034_ANALOG_GAIN_8 602 // 9.4x
#define GC1034_ANALOG_GAIN_9 851 // 13.2x

#define  GC1034_DGAIN_PRECISE  64

static void gc1034_decompose_gain(int total_gain/*q10*/, u32 *aG, u32 *dG)
{
    int iReg = (total_gain + (1 << 3)) >> 4;


    if (iReg < GC1034_ANALOG_GAIN_1) {
        iReg = GC1034_ANALOG_GAIN_1;
    }

    if ((GC1034_ANALOG_GAIN_1 <= iReg) && (iReg < GC1034_ANALOG_GAIN_2)) {
        *aG = 0;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_1;

    } else if ((GC1034_ANALOG_GAIN_2 <= iReg) && (iReg < GC1034_ANALOG_GAIN_3)) {
        *aG = 1;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_2;

    } else if ((GC1034_ANALOG_GAIN_3 <= iReg) && (iReg < GC1034_ANALOG_GAIN_4)) {
        *aG = 2;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_3;

    } else if ((GC1034_ANALOG_GAIN_4 <= iReg) && (iReg < GC1034_ANALOG_GAIN_5)) {
        *aG = 3;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_4;

    } else if ((GC1034_ANALOG_GAIN_5 <= iReg) && (iReg < GC1034_ANALOG_GAIN_6)) {
        *aG = 4;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_5;

    } else if ((GC1034_ANALOG_GAIN_6 <= iReg) && (iReg < GC1034_ANALOG_GAIN_7)) {
        *aG = 5;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_6;

    } else if ((GC1034_ANALOG_GAIN_7 <= iReg) && (iReg < GC1034_ANALOG_GAIN_8)) {
        *aG = 6;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_7;

    } else if ((GC1034_ANALOG_GAIN_8 <= iReg) && (iReg < GC1034_ANALOG_GAIN_9)) {
        *aG = 7;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_8;

    } else {
        *aG = 8;
        *dG = GC1034_DGAIN_PRECISE * iReg / GC1034_ANALOG_GAIN_9;
    }
}

static u8 gc1034_shutter_cmd[7][3] = {
    {0x42, 0xfe, 0x01},  //page1
    {0x42, 0xb6, 0x00},  //again
    {0x42, 0xb1, 0x01},  //dgainL
    {0x42, 0xb2, 0x00},  //dgainH
    {0x42, 0xfe, 0x00},  //page0
    {0x42, 0x04, 0x00},  //exptime L
    {0x42, 0x03, 0x00},   //exptime H
} ;


u32 gc1034_set_shutter(isp_ae_shutter_t *shutter)
{
    u32 gain;
    u32 again, dgain;

    gain = (shutter->ae_gain);

    gc1034_decompose_gain(gain, &again, &dgain);

    //printf("time=%d, again=%d, dgain=%d\n", shutter->ae_exp_line, again, dgain);

    gc1034_shutter_cmd[1][2] = again ;
    gc1034_shutter_cmd[2][2] = dgain >> 6 ;

    gc1034_shutter_cmd[3][2] = (dgain << 2) & 0xfc;

    gc1034_shutter_cmd[5][2] = shutter->ae_exp_line & 0xFF ;
    gc1034_shutter_cmd[6][2] = (shutter->ae_exp_line >> 8) & 0x1F;

    wrGC1034RegArray(gc1034_shutter_cmd[0], 21);



    return 0;
}

void GC1034_sleep()
{

}

void GC1034_wakeup()
{
}

void GC1034_W_Reg(u16 addr, u16 val)
{
    printf("update reg%x with %x\n", addr, val);
    wrGC1034Reg((u8)addr, (u8)val);
}
u16 GC1034_R_Reg(u16 addr)
{
    u8 val;
    rdGC1034Reg((u8)addr, &val);
    return val;
}




REGISTER_CAMERA(GC1034) = {
    .logo 				= 	"GC1034",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_RGGB,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
    .mbus_type          =   SEN_MBUS_PARALLEL,
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING,
    /* .fps         		= 	15, */
    .fps         		= 	30,

    .sen_size 			= 	{GC1034_OUTPUT_W, GC1034_OUTPUT_H},
    .isp_size 			= 	{GC1034_OUTPUT_W, GC1034_OUTPUT_H},

    /* .cap_fps         		= 	15, */
    .cap_fps         		= 	30,
    .sen_cap_size 			= 	{GC1034_OUTPUT_W, GC1034_OUTPUT_H},
    .isp_cap_size 			= 	{GC1034_OUTPUT_W, GC1034_OUTPUT_H},

    .ops                =   {
        .avin_fps           =   NULL,
        .avin_valid_signal  =   NULL,
        .avin_mode_det      =   NULL,
        .sensor_check 		= 	GC1034_check,
        .init 		        = 	GC1034_init,
        .set_size_fps 		=	GC1034_set_output_size,
        .power_ctrl         =   GC1034_power_ctl,

        .get_ae_params  	=	GC1034_get_ae_params,
        .get_awb_params 	=	gc1034_get_awb_params,
        .get_iq_params 	    =	gc1034_get_iq_params,

        .sleep 		        =	GC1034_sleep,
        .wakeup 		    =	GC1034_wakeup,
        .write_reg 		    =	GC1034_W_Reg,
        .read_reg 		    =	GC1034_R_Reg,

    }
};


