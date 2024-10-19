
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "F37_mipi.h"
#include "asm/isp_alg.h"

static u32 reset_gpios[2] = {-1, -1};
static u32 pwdn_gpios[2] = {-1, -1};

extern void *F37_mipi_get_ae_params();
extern void *F37_mipi_get_awb_params();
extern void *F37_mipi_get_iq_params();
extern void F37_mipi_ae_ev_init(u32 fps);

/* #define F37_30FPS */

#ifdef  F37_30FPS
#define INPUT_CLK  24
#define PCLK  129.6
#define LINE_LENGTH_CLK       3840
#define FRAME_LENGTH          1125
#define ROW_TIME_NS           29630    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)
#define MAX_LONG_EXP_LINE 	 	((FRAME_LENGTH - 2 )*2)
#else
#define INPUT_CLK  24
#define PCLK  129.6
#define LINE_LENGTH_CLK       3458
#define FRAME_LENGTH          1125
#define ROW_TIME_NS           29630    //1*10^9 * LINE_LENGTH_CLK/ (PCLK*10^6)
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

static u8 WRCMD = 0x80;
static u8 RDCMD = 0x81;


typedef struct {
    u8 addr;
    u8 value;
} Sensor_reg_ini;


Sensor_reg_ini F37_mipi_INI_REG[] = {
#ifdef  F37_30FPS
    0x12, 0x60,
    0x48, 0x8A,
    0x48, 0x0A,
    0x0E, 0x11,
    0x0F, 0x14,
    0x10, 0x36,
    0x11, 0x80,
    0x0D, 0xF0,
    0x5F, 0x41,
    0x60, 0x20,
    0x58, 0x12,
    0x57, 0x60,
    0x9D, 0x00,
    0x20, 0x80,
    0x21, 0x07,
    0x22, 0x65,
    0x23, 0x04,
    0x24, 0xC0,
    0x25, 0x38,
    0x26, 0x43,
    0x27, 0x17,
    0x28, 0x1D,
    0x29, 0x07,
    0x2A, 0x0A,
    0x2B, 0x17,
    0x2C, 0x00,
    0x2D, 0x00,
    0x2E, 0x16,
    0x2F, 0x44,
    0x41, 0xC5,
    0x42, 0x3B,
    0x47, 0x62,
    0x76, 0x60,
    0x77, 0x09,
    0x1D, 0x00,
    0x1E, 0x04,
    0x6C, 0x40,
    0x6E, 0x2C,
    0x70, 0xDC,
    0x71, 0xD3,
    0x72, 0xD4,
    0x73, 0x58,
    0x74, 0x02,
    0x78, 0x96,
    0x89, 0x01,
    0x6B, 0x20,
    0x86, 0x40,
    0x31, 0x0C,
    0x32, 0x38,
    0x33, 0x6C,
    0x34, 0x88,
    0x35, 0x88,
    0x3A, 0xAF,
    0x56, 0xB2,
    0x59, 0xE8,
    0x5A, 0x04,
    0x85, 0x70,
    0x8A, 0x04,
    0x91, 0x13,
    0x9B, 0x03,
    0xA9, 0x78,
    0x5B, 0xB0,
    0x5C, 0x71,
    0x5D, 0x46,
    0x5E, 0x14,
    0x62, 0x01,
    0x63, 0x0F,
    0x64, 0xC0,
    0x65, 0x06,
    0x67, 0x65,
    0x66, 0x04,
    0x68, 0x00,
    0x69, 0x7C,
    0x6A, 0x18,
    0x7A, 0x80,
    0x82, 0x20,
    0x8F, 0x91,
    0xAE, 0x30,
    0x13, 0x81,
    0x96, 0x04,
    0x4A, 0x05,
    0x7E, 0xCD,
    0x50, 0x02,
    0x49, 0x10,
    0xAF, 0x12,
    0x80, 0x41,
    0x7B, 0x4A,
    0x7C, 0x08,
    0x7F, 0x57,
    0x90, 0x00,
    0x8C, 0xFF,
    0x8D, 0xC7,
    0x8E, 0x00,
    0x8B, 0x01,
    0x0C, 0x00,
    0x19, 0x20,
    0x46, 0x00,
    0x12, 0x20,
    0x48, 0x8A,
    0x48, 0x0A,
#else
    0x12, 0x60,
    0x48, 0x8A,
    0x48, 0x0A,
    0x0E, 0x11,
    0x0F, 0x14,
    0x10, 0x36,
    0x11, 0x80,
    0x0D, 0xF0,
    0x5F, 0x41,
    0x60, 0x20,
    0x58, 0x12,
    0x57, 0x60,
    0x9D, 0x00,
    0x20, 0xC1,
    0x21, 0x06,
    0x22, 0x65,
    0x23, 0x04,
    0x24, 0xC0,
    0x25, 0x38,
    0x26, 0x43,
    0x27, 0x5B,
    0x28, 0x1D,
    0x29, 0x06,
    0x2A, 0x4B,
    0x2B, 0x16,
    0x2C, 0x00,
    0x2D, 0x00,
    0x2E, 0x16,
    0x2F, 0x44,
    0x41, 0xC8,
    0x42, 0x3B,
    0x47, 0x62,
    0x76, 0x60,
    0x77, 0x09,
    0x1D, 0x00,
    0x1E, 0x04,
    0x6C, 0x40,
    0x6E, 0x2C,
    0x70, 0xDC,
    0x71, 0xD3,
    0x72, 0xD4,
    0x73, 0x58,
    0x74, 0x02,
    0x78, 0x96,
    0x89, 0x01,
    0x6B, 0x20,
    0x86, 0x40,
    0x31, 0x0C,
    0x32, 0x38,
    0x33, 0x6C,
    0x34, 0x88,
    0x35, 0x88,
    0x3A, 0xAF,
    0x3B, 0x00,
    0x3C, 0x57,
    0x3D, 0x78,
    0x3E, 0xFF,
    0x3F, 0xF8,
    0x40, 0xFF,
    0x56, 0xB2,
    0x59, 0xE8,
    0x5A, 0x04,
    0x85, 0x70,
    0x8A, 0x04,
    0x91, 0x13,
    0x9B, 0x03,
    0x9C, 0xE1,
    0xA9, 0x78,
    0x5B, 0xB0,
    0x5C, 0x71,
    0x5D, 0x46,
    0x5E, 0x14,
    0x62, 0x01,
    0x63, 0x0F,
    0x64, 0xC0,
    0x65, 0x06,
    0x67, 0x65,
    0x66, 0x04,
    0x68, 0x00,
    0x69, 0x7C,
    0x6A, 0x18,
    0x7A, 0x80,
    0x82, 0x20,
    0x8F, 0x91,
    0xAE, 0x30,
    0x13, 0x81,
    0x96, 0x04,
    0x4A, 0x05,
    0x7E, 0xCD,
    0x50, 0x02,
    0x49, 0x10,
    0xAF, 0x12,
    0x80, 0x41,
    0x7B, 0x4A,
    0x7C, 0x08,
    0x7F, 0x57,
    0x90, 0x00,
    0x8C, 0xFF,
    0x8D, 0xC7,
    0x8E, 0x00,
    0x8B, 0x01,
    0x0C, 0x00,
    0x19, 0x20,
    0x46, 0x00,
    0x12, 0x20,
    0x48, 0x8A,
    0x48, 0x0A,
#endif
};


unsigned char wrF37_mipi_Reg(u8 regID, unsigned char regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __wend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX, regID)) {
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
    return ret;

}

unsigned char rdF37_mipi_Reg(u8 regID, unsigned char *regDat)
{
    u8 ret = 1;
    dev_ioctl(iic, IIC_IOCTL_START, 0);
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(100);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regID)) {
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

void F37_mipi_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    u32 i;
    u8 v;


    F37_mipi_set_output_size(width, height, frame_freq);

    for (i = 0; i < sizeof(F37_mipi_INI_REG) / sizeof(Sensor_reg_ini); i++) {
        wrF37_mipi_Reg(F37_mipi_INI_REG[i].addr, F37_mipi_INI_REG[i].value);
    }

    F37_mipi_ae_ev_init(*frame_freq);



    *format = SEN_IN_FORMAT_BGGR;
    return;
}


s32 F37_mipi_set_output_size(u16 *width, u16 *height, u8 *frame_freq)
{
    *width = F37_MIPI_OUTPUT_W;
    *height = F37_MIPI_OUTPUT_H;

    return 0;
}


s32 F37_mipi_power_ctl(u8 isp_dev, u8 is_work)
{

    return 0;
}

s32 F37_mipi_ID_check(void)
{
    u8 pid = 0x00;
    u8 ver = 0x00;
    u8 i = 0;
    u8 slave_id[4] = {0x80, 0x84, 0x88, 0x8C};

    for (i = 0; i < 4; i ++) {
        WRCMD = slave_id[i];
        RDCMD = slave_id[i] + 1;
        rdF37_mipi_Reg(0x0A, &pid);
        rdF37_mipi_Reg(0x0B, &ver);

        printf("Sensor PID %02x %02x\n", pid, ver);
        if (pid == 0x0F && ver == 0x37) {
            puts("----hello F37_mipi-----\n");
            return 0;
        }
    }

    puts("----not F37_mipi-----\n");
    return -1;
}

void F37_mipi_reset(u8 isp_dev)
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
    gpio_direction_output(pwdn_gpio, 1);
    delay(40000);
    gpio_direction_output(reset_gpio, 1);
    gpio_direction_output(pwdn_gpio, 0);
    delay(40000);
}


static u8 cur_sensor_type = 0xff;

s32 F37_mipi_check(u8 isp_dev, u32 reset_gpio, u32 pwdn_gpio)
{

    puts("\n\n F37_mipi_check \n\n");
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

    F37_mipi_reset(isp_dev);

    if (0 != F37_mipi_ID_check()) {
        dev_close(iic);
        iic = NULL;
        return -1;
    }


//    strcpy(sensor_logo, "SR013");
    cur_sensor_type = isp_dev;

    return 0;
}


s32 F37_mipi_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    F37_mipi_reset(isp_dev);
    puts("\n\n F37_mipi_init22 \n\n");

    F37_mipi_config_SENSOR(width, height, format, frame_freq);

    return 0;
}

static void set_again(u32 again)
{
    /* if (cur_again == again) { */
    /* return; */
    /* } */
    /* cur_again  = again; */
    /* //printf("again = %04x\n",again); */
    /* wrF37_mipi_Reg(0x3e08, ((again >> 8) & 0xfc) | 0x03); */
    /* wrF37_mipi_Reg(0x3e09, again & 0xff); */
    /* return; */
}

static void set_dgain(u32 dgain)
{
    if (cur_dgain == dgain) {
        return;
    }
    cur_dgain  = dgain;
    //printf("dgain = %04x\n", dgain);
    wrF37_mipi_Reg(0x00, dgain);
}

static void calc_gain(u32 gain, u32 *_again, u32 *_dgain)
{
    u32 gain0;
    u32 reg0, reg1;

    if (gain < 1024) {
        gain = 1024;
    }
    if (gain > 255 * 1024) {
        gain = 255 * 1024;
    }

    gain0 = gain;
    reg0 = 0;
    while (gain0 > 2048) {
        reg0++;
        gain0 /= 2;
    }
    reg1 = ((gain0 - 1024) >> 6) & 0x0F;

    *_again = 0;
    *_dgain = (reg0 << 4) | reg1;

    /* printf(" gain = %d; reg =%d %x %x %02x; \n ", gain,gain0,reg0,reg1, *_dgain); */
}


static void set_shutter(u32 texp)
{
    u8 wval, rval;
    if (cur_expline == texp) {
        return;
    }
    cur_expline  = texp;
    /* printf("texp = %d\n", texp); */

    wval = (u8)(texp >> 8);
    wrF37_mipi_Reg(0x02, wval);
    wval = (u8)texp;
    wrF37_mipi_Reg(0x01, wval);
}

u32 F37_mipi_calc_shutter(isp_ae_shutter_t *shutter, u32 exp_time_us, u32 gain)
{
    u32 texp;
    u32 texp_align;
    u32 ratio;
    u32 fLen;
    /* static bLow = 0; */

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

    shutter->ae_exp_line =  texp;
    shutter->ae_gain = (gain * ratio) >> 10;
    shutter->ae_exp_clk = 0;

    shutter->ae_exp_line_short =  texp / 16;
    shutter->ae_gain_short = shutter->ae_gain * texp / (texp & (~0xf));
    shutter->ae_exp_clk_short = 0;

    //printf("exp_time_us=%d, texp=%d, gain=%d->%d\n", exp_time_us, texp, gain,shutter->ae_gain);
    return 0;

}

u32 F37_mipi_set_shutter(isp_ae_shutter_t *shutter)
{
    u32 again, dgain;
    u32 again_s, dgain_s;

    calc_gain((shutter->ae_gain), &again, &dgain);

    set_shutter(shutter->ae_exp_line);
    set_again(again);
    set_dgain(dgain);

    return 0;
}


void F37_mipi_sleep()
{


}

void F37_mipi_wakeup()
{


}

void F37_mipi_W_Reg(u16 addr, u16 val)
{
    /*printf("update reg%x with %x\n", addr, val);*/
    wrF37_mipi_Reg((u8)addr, (u8)val);
}
u16 F37_mipi_R_Reg(u16 addr)
{
    u8 val;
    rdF37_mipi_Reg((u8)addr, &val);
    printf("read reg%x val %x\n", addr, val);
    return val;
}

REGISTER_CAMERA(F37_mipi) = {
    .logo 				= 	"F37m",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_BGGR,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
    .mbus_type          =   SEN_MBUS_CSI2,
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_10B | SEN_MBUS_CSI2_2_LANE,
#ifdef F37_30FPS
    .real_fps			= (30.0 * 65536),
#else
    .real_fps			= (33.3 * 65536),
#endif
    .fps         		= 	30,

    .sen_size 			= 	{F37_MIPI_OUTPUT_W, F37_MIPI_OUTPUT_H},
    .isp_size 			= 	{F37_MIPI_OUTPUT_W, F37_MIPI_OUTPUT_H},

    .cap_fps         		= 	30,
    .sen_cap_size 			= 	{F37_MIPI_OUTPUT_W, F37_MIPI_OUTPUT_H},
    .isp_cap_size 			= 	{F37_MIPI_OUTPUT_W, F37_MIPI_OUTPUT_H},

#if 0
    .hdr_mode               =  ISP_HDR_VC_SS,
    .hdr_prefetch_lines     =  MAX_SHORT_DELAY_LINE - 5,
    .hdr_short_delay_lines  =  MAX_SHORT_DELAY_LINE,
    .hdr_short_gain         =  0x1000,  // 1 times
    .hdr_long_gain          =  0x100,   // 1/16 times
#endif

    .ops                =   {
        .avin_fps           =   NULL,
        .avin_valid_signal  =   NULL,
        .avin_mode_det      =   NULL,
        .sensor_check 		= 	F37_mipi_check,
        .init 		        = 	F37_mipi_init,
        .set_size_fps 		=	F37_mipi_set_output_size,
        .power_ctrl         =   F37_mipi_power_ctl,

        .get_ae_params  	=	F37_mipi_get_ae_params,
        .get_awb_params 	=	F37_mipi_get_awb_params,
        .get_iq_params 	   	=	F37_mipi_get_iq_params,

        .sleep 		        =	F37_mipi_sleep,
        .wakeup 		    =	F37_mipi_wakeup,
        .write_reg 		    =	F37_mipi_W_Reg,
        .read_reg 		    =	F37_mipi_R_Reg,

    }
};



