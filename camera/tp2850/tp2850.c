#include "tp2850.h"
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "generic/jiffies.h"


/* #define tp2850k_720P */
#define tp2850k_1080P

static u8 avin_fps();

static u32 reset_gpio[2] = {-1, -1};

//0xB8
//0xB9
#define tp2850K_PSTA		1
#define tp2850K_NSTA		2

/*S_IIC_FUN tp2850k_iic_fun;*/
static void *iic = NULL;
static u8 g_pn_status = -1;

#define WRCMD 0x88
#define RDCMD 0x89

typedef struct {
    u8 addr;
    u8 value;
} tp2850_init_regs_t;

void wrtp2850KReg(u16 regID, u16 regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __wend;
    }

    delay(50);

    if (dev_ioctl(iic, IIC_IOCTL_TX, regID)) {
        ret = 0;
        goto __wend;
    }

    delay(50);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regDat)) {
        ret = 0;
        goto __wend;
    }

__wend:
    if (ret == 0) {
        log_e("iic w err!!!\n");
    }

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    /* return ret; */
}

unsigned char rdtp2850KReg(u16 regID, unsigned char *regDat)
{
    u8 ret = 1;

    dev_ioctl(iic, IIC_IOCTL_START, 0);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(50);

    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regID)) {
        ret = 0;
        goto __rend;
    }

    delay(50);


    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, RDCMD)) {
        ret = 0;
        goto __rend;
    }

    delay(50);

    dev_ioctl(iic, IIC_IOCTL_RX_WITH_STOP_BIT, (u32)regDat);

__rend:

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    return ret;
}
extern void delay_2ms(int cnt);
#define delay2ms(t) delay_2ms(t)
void tp2850k_reset(u8 isp_dev)
{
    u32 gpio;

    if (isp_dev == ISP_DEV_0) {
        gpio = reset_gpio[0];
    } else {
        gpio = reset_gpio[1];
    }
    // printf("\n gpio=====================%d\n",gpio);
    gpio_direction_output(gpio, 1);
    delay2ms(50);
    gpio_direction_output(gpio, 0);
    delay2ms(25);
    gpio_direction_output(gpio, 1);
}

u16 tp2850_dvp_rd_reg(u16 addr)
{
    u8 val;
    rdtp2850KReg((u16)addr, &val);
    return val;
}
s32 tp2850k_id_check()
{
    u16 id = 0;
    u16 id1 = 0;
    u8 id_check_cont;

    for (id_check_cont = 0; id_check_cont <= 3; id_check_cont++) {

        rdtp2850KReg(0xfe, (unsigned char *)&id);
        id <<= 8;
        rdtp2850KReg(0xff, (unsigned char *)&id1);
        id |= id1;
        puts("\nid:");
        log_d("id 0x%x\n", id);
        if (id == 0x2850) {
            puts("\n tp2850k_id_check succ\n");
            return 1;
        }

    }

    return 0;
}


/*void tp2850K_iic_set(u8 isp_dev)*/
/*{*/
/*iic_select(&tp2850k_iic_fun, isp_dev);*/
/*}*/


static u8 cur_sensor_type = 0xff;
static void av_sensor_power_ctrl(u32 _power_gpio, u32 on_off)
{
    u32 gpio = _power_gpio;
    gpio_direction_output(gpio, on_off);
    delay(5000);
}

s32 tp2850k_check(u8 isp_dev, u32 _reset_gpio, u32 _power_gpio)
{
    // printf("\n\n 0isp_dev===============%d\n\n ",isp_dev);
    if (isp_dev == ISP_DEV_0) {
        return -1;
    }
    /* if (isp_dev == ISP_DEV_1) { */
    /* return -1; */
    /* } */
    puts("\ntp2850k_id check\n");
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

    reset_gpio[isp_dev] = _reset_gpio;
    av_sensor_power_ctrl(_power_gpio, 1);
    tp2850k_reset(isp_dev);


    if (0 == tp2850k_id_check()) {
        dev_close(iic);
        iic = NULL;

        puts("\ntp2850k_id check fail\n\n");

        return -1;
    }

    cur_sensor_type = isp_dev;

    return 0;
}


s32 tp2850k_set_output_size(u16 *width, u16 *height, u8 *freq)
{
    /*
        if (avin_fps()) {
            *freq = 25;
            g_pn_status = tp2850K_PSTA;
        } else {
            *freq = 30;
            g_pn_status = tp2850K_NSTA;
        }
    */

#ifdef tp2850k_720P
    *freq = 25;
    *width = 1280;
    *height = 720;
#endif

    return 0;
}

static u8 avin_valid_signal()
{
    u8 DetVideo;
    u8 LockStatus;
    u8 j;

#ifdef tp2850k_1080P
    /* wrtp2850KReg(0xff, 0x00); */
    for (j = 0; j < 3; j++) {
        rdtp2850KReg(0x01, & LockStatus);
        rdtp2850KReg(0x03, &DetVideo);

        printf("\n DetVideo====%x,LockStatus====%x\n ", DetVideo, LockStatus);
        if ((LockStatus & BIT(3)) && (DetVideo & 0x07) != 0x07) {
            return 1;
        }
        delay2ms(5);
    }
    return 0;
#endif


#ifdef tp2850k_720P
    wrtp2850KReg(0x40, 0x00);
    for (j = 0; j < 3; j++) {
        rdtp2850KReg(0x01, &LockStatus);
        if ((LockStatus & 0x60) == 0X60) {
            return 1;
        }
        os_time_dly(20);
    }
    return 0;
#endif
}

static int wait_signal_valid()
{
#if 0
    u32 time;
    delay2ms(100);
    if (avin_valid_signal()) {
        //信号有效等50ms 待信号稳定
        time = jiffies + msecs_to_jiffies(50);
        while (1) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx tp2850k valid\n");
                return 0;
            }

        }
    } else {
        //信号无效等100ms
        time = jiffies + msecs_to_jiffies(100);
        while (!avin_valid_signal()) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx tp2850k no no no no validxx \n");
                return -1;
            }
        }
    }
#endif

    return  0;
}


#ifdef tp2850k_720P
const tp2850_init_regs_t HDA_720_REG_INIT[] = {
    0x02, 0xCE,
    0x05, 0x00,
    0x06, 0x32,
    0x07, 0xC0,
    0x08, 0x00,
    0x09, 0x24,
    0x0A, 0x48,
    0x0B, 0xC0,
    0x0C, 0x03,
    0x0D, 0x71,
    0x0E, 0x00,
    0x0F, 0x00,
    0x10, 0x70,
    0x11, 0x48,
    0x12, 0x58,
    0x13, 0x00,
    0x14, 0x00,
    0x15, 0x13,
    0x16, 0x16,
    0x17, 0x00,
    0x18, 0x19,
    0x19, 0xD0,
    0x1A, 0x25,
    0x1B, 0x00,
    0x1C, 0x07,
    0x1D, 0xBC,
    0x1E, 0x80,
    0x1F, 0x80,
    0x20, 0x40,
    0x21, 0x46,
    0x22, 0x36,
    0x23, 0x3C,
    0x24, 0x04,
    0x25, 0xFE,
    0x26, 0x01,
    0x27, 0x2D,
    0x28, 0x00,
    0x29, 0x48,
    0x2A, 0x30,
    0x2B, 0x60,
    0x2C, 0x2A,
    0x2D, 0x5A,
    0x2E, 0x40,
    0x2F, 0x00,
    0x30, 0x9E,
    0x31, 0x20,
    0x32, 0x10,
    0x33, 0x90,
    0x34, 0x00,
    0x35, 0x25,
    0x36, 0xDC,
    0x37, 0x00,
    0x38, 0x00,
    0x39, 0x18,
    0x3A, 0x32,
    0x3B, 0x26,
    0x3C, 0x00,
    0x3D, 0x60,
    0x3E, 0x00,
    0x3F, 0x00,
    0x40, 0x00,
    0x41, 0x00,
    0x42, 0x00,
    0x43, 0x00,
    0x44, 0x00,
    0x45, 0x00,
    0x46, 0x00,
    0x47, 0x00,
    0x48, 0x00,
    0x49, 0x00,
    0x4A, 0x00,
    0x4B, 0x00,
    0x4C, 0x43,
    0x4D, 0x00,
    0x4E, 0x17,
    0x4F, 0x00,
    0x50, 0x00,
    0x51, 0x00,
    0x52, 0x00,
    0x53, 0x00,
    0x54, 0x00,
    0x55, 0x00,

    0xB3, 0xFA,
    0xB4, 0x00,
    0xB5, 0x00,
    0xB6, 0x00,
    0xB7, 0x00,
    0xB8, 0x00,
    0xB9, 0x00,
    0xBA, 0x00,
    0xBB, 0x00,
    0xBC, 0x00,
    0xBD, 0x00,
    0xBE, 0x00,
    0xBF, 0x00,
    0xC0, 0x00,
    0xC1, 0x00,
    0xC2, 0x0B,
    0xC3, 0x0C,
    0xC4, 0x00,
    0xC5, 0x00,
    0xC6, 0x1F,
    0xC7, 0x78,
    0xC8, 0x27,
    0xC9, 0x00,
    0xCA, 0x00,
    0xCB, 0x07,
    0xCC, 0x08,
    0xCD, 0x00,
    0xCE, 0x00,
    0xCF, 0x04,
    0xD0, 0x00,
    0xD1, 0x00,
    0xD2, 0x60,
    0xD3, 0x10,
    0xD4, 0x06,
    0xD5, 0xBE,
    0xD6, 0x39,
    0xD7, 0x27,
    0xD8, 0x00,
    0xD9, 0x00,
    0xDA, 0x00,
    0xDB, 0x00,
    0xDC, 0x00,
    0xDD, 0x00,
    0xDE, 0x00,
    0xDF, 0x00,
    0xE0, 0x00,
    0xE1, 0x00,
    0xE2, 0x00,
    0xE3, 0x00,
    0xE4, 0x00,
    0xE5, 0x00,
    0xE6, 0x00,
    0xE7, 0x13,
    0xE8, 0x03,
    0xE9, 0x00,
    0xEA, 0x00,
    0xEB, 0x00,
    0xEC, 0x00,
    0xED, 0x00,
    0xEE, 0x00,
    0xEF, 0x00,
    0xF0, 0x00,
    0xF1, 0x00,
    0xF2, 0x00,
    0xF3, 0x00,
    0xF4, 0x20,
    0xF5, 0x10,
    0xF6, 0x00,
    0xF7, 0x00,
    0xF8, 0x00,
    0xF9, 0x00,
    0xFA, 0x88,
    0xFB, 0x00,
    0xFC, 0x00,

    0x40, 0x08,
    0x01, 0xf8,
    0x02, 0x01,
    0x08, 0x0f,
    0x13, 0x24,
    0x14, 0x46,
    0x15, 0x08,
    0x20, 0x12,
    0x34, 0x1b,
    0x23, 0x02,
    0x23, 0x00,

    0x40, 0x00,
};
#endif

#ifdef tp2850k_1080P
const tp2850_init_regs_t HDA_1080_REG_INIT[] = {

    /* 0x02, 0xCC, */
    0x02, 0xC4,
    0x05, 0x00,
    0x06, 0x32,
    0x07, 0xC0,
    0x08, 0x00,
    0x09, 0x24,
    0x0A, 0x48,
    0x0B, 0xC0,
    0x0C, 0x03,
    0x0D, 0x73,
    0x0E, 0x00,
    0x0F, 0x00,
    0x10, 0x00,
    0x11, 0x40,
    0x12, 0x60,
    0x13, 0x00,
    0x14, 0x00,
    0x15, 0x01,
    0x16, 0xF0,
    0x17, 0x80,
    0x18, 0x29,
    0x19, 0x38,
    0x1A, 0x47,
    0x1B, 0x01,
    0x1C, 0x0A,
    0x1D, 0x50,
    0x1E, 0x80,
    0x1F, 0x80,
    0x20, 0x3C,
    0x21, 0x46,
    0x22, 0x36,
    0x23, 0x3C,
    0x24, 0x04,
    0x25, 0xFE,
    0x26, 0x0D,
    0x27, 0x2D,
    0x28, 0x00,
    0x29, 0x48,
    0x2A, 0x30,
    0x2B, 0x60,
    0x2C, 0x1A,
    0x2D, 0x54,
    0x2E, 0x40,
    0x2F, 0x00,
    0x30, 0xA5,
    0x31, 0x86,
    0x32, 0xFB,
    0x33, 0x60,
    0x34, 0x00,
    0x35, 0x05,
    0x36, 0xDC,
    0x37, 0x00,
    0x38, 0x00,
    0x39, 0x1C,
    0x3A, 0x32,
    0x3B, 0x26,
    0x3C, 0x00,
    0x3D, 0x60,
    0x3E, 0x00,
    0x3F, 0x00,
    0x40, 0x00,
    0x41, 0x00,
    0x42, 0x00,
    0x43, 0x00,
    0x44, 0x00,
    0x45, 0x00,
    0x46, 0x00,
    0x47, 0x00,
    0x48, 0x00,
    0x49, 0x00,
    0x4A, 0x00,
    0x4B, 0x00,
    0x4C, 0x43,
    0x4D, 0x00,
    0x4E, 0x17,
    0x4F, 0x00,
    0x50, 0x00,
    0x51, 0x00,
    0x52, 0x00,
    0x53, 0x00,
    0x54, 0x00,

    0xB3, 0xFA,
    0xB4, 0x00,
    0xB5, 0x00,
    0xB6, 0x00,
    0xB7, 0x00,
    0xB8, 0x00,
    0xB9, 0x00,
    0xBA, 0x00,
    0xBB, 0x00,
    0xBC, 0x00,
    0xBD, 0x00,
    0xBE, 0x00,
    0xBF, 0x00,
    0xC0, 0x00,
    0xC1, 0x00,
    0xC2, 0x0B,
    0xC3, 0x0C,
    0xC4, 0x00,
    0xC5, 0x00,
    0xC6, 0x1F,
    0xC7, 0x78,
    0xC8, 0x27,
    0xC9, 0x00,
    0xCA, 0x00,
    0xCB, 0x07,
    0xCC, 0x08,
    0xCD, 0x00,
    0xCE, 0x00,
    0xCF, 0x04,
    0xD0, 0x00,
    0xD1, 0x00,
    0xD2, 0x60,
    0xD3, 0x10,
    0xD4, 0x06,
    0xD5, 0xBE,
    0xD6, 0x39,
    0xD7, 0x27,
    0xD8, 0x00,
    0xD9, 0x00,
    0xDA, 0x00,
    0xDB, 0x00,
    0xDC, 0x00,
    0xDD, 0x00,
    0xDE, 0x00,
    0xDF, 0x00,
    0xE0, 0x00,
    0xE1, 0x00,
    0xE2, 0x00,
    0xE3, 0x00,
    0xE4, 0x00,
    0xE5, 0x00,
    0xE6, 0x00,
    0xE7, 0x13,
    0xE8, 0x03,
    0xE9, 0x00,
    0xEA, 0x00,
    0xEB, 0x00,
    0xEC, 0x00,
    0xED, 0x00,
    0xEE, 0x00,
    0xEF, 0x00,
    0xF0, 0x00,
    0xF1, 0x00,
    0xF2, 0x00,
    0xF3, 0x00,
    0xF4, 0x20,
    0xF5, 0x10,
    0xF6, 0x00,
    0xF7, 0x00,
    0xF8, 0x00,
    0xF9, 0x00,
    0xFA, 0x03,
    0xFB, 0x00,
    0xFC, 0x00,

    0x40, 0x08,
    0x00, 0x00,
    0x01, 0xf8,
    0x02, 0x01,
    0x08, 0xF0,
    0x13, 0x04,
    0x14, 0x73,
    0x15, 0x08,
    0x20, 0x12,
    0x34, 0x1b,
    0x23, 0x02,
    0x23, 0x00,

    0x40, 0x00,

};
#endif



extern void tp2850k_init();
int tp2850K_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    tp2850k_init();

    if (wait_signal_valid() != 0) {
        return -1;
    }

#ifdef tp2850k_720P
    *format = SEN_IN_FORMAT_UYVY;
#endif

    tp2850k_set_output_size(width, height, frame_freq);

    return 0;
}

s32 tp2850k_initialize(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    tp2850k_reset(isp_dev);
    puts("\ntp2850k_init \n");

    if (tp2850K_config_SENSOR(width, height, format, frame_freq) != 0) {
        puts("\ntp2850k_init fail\n");
        return -1;
    }
    return 0;
}


void tp2850k_init()
{

    u8 v = 0;
    puts("\ntp2850k_init========================================================================= \n");
#ifdef tp2850k_720P
    for (int i = 0; i < sizeof(HDA_720_REG_INIT) / sizeof(tp2850_init_regs_t); i++) {
        wrtp2850KReg(HDA_720_REG_INIT[i].addr, HDA_720_REG_INIT[i].value);
    }
#else
    for (int i = 0; i < sizeof(HDA_1080_REG_INIT) / sizeof(tp2850_init_regs_t); i++) {
        wrtp2850KReg(HDA_1080_REG_INIT[i].addr, HDA_1080_REG_INIT[i].value);
    }
#endif

    /* for (int ii = 0;ii < sizeof(HDA_720_REG_INIT) / sizeof(tp2850_init_regs_t);ii++) */
    /* { */
    /* v = 0x00; */
    /* rdtp2850KReg(HDA_720_REG_INIT[ii].addr,&v); */
    /* printf("reg0x%04x =0x%02x->0x%02x\n", HDA_720_REG_INIT[ii].addr, HDA_720_REG_INIT[ii].value, v); */
    /* if (HDA_720_REG_INIT[ii].value != v) */
    /* printf("Value Ch\n"); */
    /* } */
}

s32 tp2850k_power_ctl(u8 isp_dev, u8 is_work)
{
    return 0;
}

static u8 avin_fps()
{

#ifdef tp2850k_1080P
    return 1;
#else
    u8 status ;
    rdtp2850KReg(0x88, &status);
    if (status & BIT(5)) {
        return 1;
    }
    return 0;
#endif

}



static u8 avin_mode_det(void *parm)
{
    u8 new_status;
    /*
        if (avin_fps()) {
            new_status = tp2850K_PSTA;
        } else {
            new_status = tp2850K_NSTA;
        }

        if (g_pn_status != new_status) {
            return 1;
        }
    */
    return 0;
}


REGISTER_CAMERA(tp2850K) = {
    .logo 				= 	"tp2850",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			=   SEN_IN_FORMAT_UYVY,//	SEN_IN_FORMAT_UYVY,
    .out_format 		= 	ISP_OUT_FORMAT_YUV,
#ifdef tp2850k_720P
    .mbus_type          =   SEN_MBUS_BT656,
#else
    .mbus_type          =   SEN_MBUS_BT1120,
#endif
    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING | SEN_MBUS_DATA_REVERSE, */
    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_RISING| SEN_MBUS_DATA_REVERSE, */
    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_RISING, */
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING,

    .fps         		= 	25,

#ifdef tp2850k_720P
    .sen_size 			= 	{1280, 720},
    .isp_size 			= 	{1280, 720},
#else
    .sen_size 			= 	{1920, 1080},
    .isp_size 			= 	{1920, 1080},
#endif

    .ops                =   {
        .avin_fps           =   avin_fps,
        .avin_valid_signal  =   avin_valid_signal,
        .avin_mode_det      =   avin_mode_det,
        .sensor_check 		= 	tp2850k_check,
        .init 		        = 	tp2850k_initialize,
        .set_size_fps 		=	tp2850k_set_output_size,
        .power_ctrl         =   tp2850k_power_ctl,


        .sleep 		        =	NULL,
        .wakeup 		    =	NULL,
        .write_reg 		    =	wrtp2850KReg,
        .read_reg 		    =	tp2850_dvp_rd_reg,
    }
};



