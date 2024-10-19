#include "tp9930.h"
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "generic/jiffies.h"

/* #define CONFIG_9930_CLKOUT_27M */

static u8 avin_fps();

static u32 reset_gpio[4] = {-1, -1, -1. - 1};

//0x88
//0x89
#define tp9930K_PSTA		1
#define tp9930K_NSTA		2

static void *iic = NULL;
static u8 g_pn_status = -1;

#define WRCMD 0x88
#define RDCMD 0x89

unsigned char wrtp9930KReg(u16 regID, u16 regDat)
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

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    return ret;
}

unsigned char rdtp9930KReg(u16 regID, unsigned char *regDat)
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



#ifdef CONFIG_9930_CLKOUT_27M
void output_clk(u8 gpio)
{
    CLK_CON0 &= ~(0xf << 14);
    CLK_CON0 |= (0 << 14); //clk out 1
    IOMC1 &= ~(0x1f << 18);//op ch 2
    IOMC1 |= (0x1 << 18); //clk out 1

    gpio_direction_output(gpio, 1);
    gpio_set_pull_up(gpio, 1);
    gpio_set_pull_down(gpio, 1);
    gpio_set_die(gpio, 0);
    gpio_set_hd(gpio, 1);
}
#endif

extern void delay_2ms(int cnt);
#define delay2ms(t) delay_2ms(t)
void tp9930k_reset(u8 isp_dev)
{
    u32 gpio;

    gpio = reset_gpio[isp_dev];
    /* if (isp_dev == ISP_DEV_0) { */
    /* gpio = reset_gpio[0]; */
    /* } else if (isp_dev == ISP_DEV_1) { */
    /* gpio = reset_gpio[1]; */
    /* } else { */
    /* gpio = reset_gpio[2]; */
    /* } */
#ifdef CONFIG_9930_CLKOUT_27M
    output_clk(IO_PORTE_02);	//IO_PORTE_02
#endif

    // printf("\n gpio=====================%d\n",gpio);
    gpio_direction_output(gpio, 1);
    /* delay2ms(50); */
    delay2ms(100);
    gpio_direction_output(gpio, 0);
    /* delay2ms(25); */
    delay2ms(100);
    gpio_direction_output(gpio, 1);
}

s32 tp9930k_id_check()
{
    u16 id = 0;
    u16 id1 = 0;
    u8 id_check_cont;

    for (id_check_cont = 0; id_check_cont <= 3; id_check_cont++) {
        rdtp9930KReg(0xfe, (unsigned char *)&id);
        id <<= 8;
        rdtp9930KReg(0xff, (unsigned char *)&id1);
        id |= id1;
        log_i("id %x\n", id);
        if (id == 0x2832) {
            puts("\n tp9930k_id_check succ\n");
            return 1;
        }
    }

    return 0;
}


/*void tp9930K_iic_set(u8 isp_dev)*/
/*{*/
/*iic_select(&tp9930k_iic_fun, isp_dev);*/
/*}*/


static u8 cur_sensor_type = 0xff;

s32 tp9930k_check(u8 isp_dev, u32 _reset_gpio, u32 _power_gpio)
{
    log_i("\ntp9930k_id check %d\n", isp_dev);

    if (isp_dev == ISP_DEV_0) {
        log_i("a0\n");
        return -1;
    }
    if (!iic) {
        if (isp_dev == ISP_DEV_0) {
            iic = dev_open("iic4", 0);
        } else {
            iic = dev_open("iic4", 0);
        }
        if (!iic) {
            log_i("a1\n");
            return -1;
        }
    } else {
        if (cur_sensor_type != isp_dev) {
            log_i("a2\n");
            return -1;
        }
    }

    if (_power_gpio != (u8) - 1) {
        gpio_direction_output(_power_gpio, 1);
        delay(5000);
    }

    if (isp_dev >= sizeof(reset_gpio) / sizeof(reset_gpio[0])) {
        puts("tp9930 dev overflow\n");
        while (1);
    }

    if (cur_sensor_type == 0xff) {
        reset_gpio[isp_dev] = _reset_gpio;
        tp9930k_reset(isp_dev);
    }

    if (0 == tp9930k_id_check()) {
        dev_close(iic);
        iic = NULL;
        puts("\ntp9930k_id check fail\n\n");
        return -1;
    }

    /* extern void tp9930k_init(); */
    /* tp9930k_init(); */

    cur_sensor_type = isp_dev;

    return 0;
}


s32 tp9930k_set_output_size(u16 *width, u16 *height, u8 *freq)
{
    /*
        if (avin_fps()) {
            *freq = 25;
            g_pn_status = tp9930K_PSTA;
        } else {
            *freq = 30;
            g_pn_status = tp9930K_NSTA;
        }
    */
    /* *freq = 25; */
    /* *width = 1280; */
    /* *height = 720; */

    return 0;
}

static u8 avin_valid_signal()
{
    u8 DetVideo;
    u8 LockStatus;
    u8 j;

    /* wrtp9930KReg(0xff, 0x00); */
    for (j = 0; j < 3; j++) {
        rdtp9930KReg(0x01, & LockStatus);
        rdtp9930KReg(0x03, &DetVideo);

        printf("\n DetVideo====%x,LockStatus====%x\n ", DetVideo, LockStatus);
        if (LockStatus & 0x70) {
            return 1;
        }
        delay2ms(5);
    }

    log_i("not valid, but test!~!!!!!!\n");
    return 1;
    /* return 0; */
}

static int wait_signal_valid()
{
    u32 time;
    delay2ms(100);
    if (avin_valid_signal()) {
        //信号有效等50ms 待信号稳定
        time = jiffies + msecs_to_jiffies(50);
        while (1) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx tp9930k valid\n");
                return 0;
            }

        }
    } else {
        //信号无效等100ms
        time = jiffies + msecs_to_jiffies(100);
        while (!avin_valid_signal()) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx tp9930k no no no no validxx \n");
                return -1;
            }
        }
    }

    return  0;
}

void tp9930k_init();

s32 tp9930k_initialize(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    tp9930k_reset(isp_dev);
    puts("\ntp9930k_init \n");

    tp9930k_init();

    if (wait_signal_valid() != 0) {
        puts("\ntp9930k_init signal valid fail\n");
        return -1;
    }
    *format = SEN_IN_FORMAT_UYVY;
    tp9930k_set_output_size(width, height, frame_freq);

    return 0;
}


void tp9930k_init()
{
    int i = 0;

    /* for (i = 0; i < sizeof(TP9930_720P25_BT656_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_720P25_BT656_TAB[i].addr, TP9930_720P25_BT656_TAB[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(TP9930_720P25_BT656MUX2_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_720P25_BT656MUX2_TAB[i].addr, TP9930_720P25_BT656MUX2_TAB[i].value); */
    /* } */

    for (i = 0; i < sizeof(TP9930_720P25_DDR8bitBT656MUX4_TAB) / sizeof(tp9930_reg_t); i++) {
        wrtp9930KReg(TP9930_720P25_DDR8bitBT656MUX4_TAB[i].addr, TP9930_720P25_DDR8bitBT656MUX4_TAB[i].value);
    }



    /* for (i = 0; i < sizeof(TP9930_720P25DDR_BT656MUX2_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_720P25DDR_BT656MUX2_TAB[i].addr, TP9930_720P25DDR_BT656MUX2_TAB[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(TP9930_720P25_BT1120MUX4_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_720P25_BT1120MUX4_TAB[i].addr, TP9930_720P25_BT1120MUX4_TAB[i].value); */
    /* } */


    /* for (i = 0; i < sizeof(TP9930_cvbs_BT656MUX2_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_cvbs_BT656MUX2_TAB[i].addr, TP9930_cvbs_BT656MUX2_TAB[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(TP9930_cvbs_BT656MUX2_DDR1980x480_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_cvbs_BT656MUX2_DDR1980x480_TAB[i].addr, TP9930_cvbs_BT656MUX2_DDR1980x480_TAB[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(TP9930_cvbs_PAL_BT656MUX2_DDR1980x480_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_cvbs_PAL_BT656MUX2_DDR1980x480_TAB[i].addr, TP9930_cvbs_PAL_BT656MUX2_DDR1980x480_TAB[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(TP9930_cvbs_PAL_BT656MUX2_DDR990x480_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_cvbs_PAL_BT656MUX2_DDR990x480_TAB[i].addr, TP9930_cvbs_PAL_BT656MUX2_DDR990x480_TAB[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(TP9930_cvbs_PAL_BT656MUX2_SDR990x480_TAB) / sizeof(tp9930_reg_t); i++) { */
    /* wrtp9930KReg(TP9930_cvbs_PAL_BT656MUX2_SDR990x480_TAB[i].addr, TP9930_cvbs_PAL_BT656MUX2_SDR990x480_TAB[i].value); */
    /* } */

    delay2ms(10);
/////////////////////////////test/////////////////////////////////////////////////
//open color bar

    /* u8 temp ; */
    /* wrtp9930KReg(0xff, 0x01); */
    /* rdtp9930KReg(0x4f, &temp); */
    /* temp = (temp | 0x10); */
    /* wrtp9930KReg(0x4f, temp); */

    /* wrtp9930KReg(0xff, 0x02); */
    /* rdtp9930KReg(0x80, &temp); */
    /* temp = (temp | 0xb0); */
    /* wrtp9930KReg(0x80, temp); */


    /* u8 temp ; */
    /* rdtp9930KReg(0x03, &temp); */
    /* printf("reg:0x03 = %x\n",temp); */

    /* wrtp9930KReg(0x40, 0x01); */
    /* u8 data,temp; */
    /* for(data=0x01;data<=0x3f;data++) */
    /* { */

    /* rdtp9930KReg(data, &temp); */
    /* printf("reg:0x%x=%x\n",data,temp); */
    /* } */

}

s32 tp9930k_power_ctl(u8 isp_dev, u8 is_work)
{
    return 0;
}

static u8 avin_fps()
{
    u8 status ;
    return 1;//25hz
    // rdtp9930KReg(0x88, &status);
    if (status & BIT(5)) {
        return 1;
    }

    return 0;//30hz

}



static u8 avin_mode_det(void *parm)
{
    u8 new_status;
    /*
        if (avin_fps()) {
            new_status = tp9930K_PSTA;
        } else {
            new_status = tp9930K_NSTA;
        }

        if (g_pn_status != new_status) {
            return 1;
        }
    */
    return 0;
}


REGISTER_CAMERA(tp9930K) = {
    .logo 				= 	"tp9930K",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			=   SEN_IN_FORMAT_UYVY,//	SEN_IN_FORMAT_UYVY,
    .out_format 		= 	0xff,//ISP_OUT_FORMAT_YUV,
    /* .mbus_type          =   SEN_MBUS_BT656, */
    .mbus_type          =   SEN_MBUS_BT1120,
    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING, */
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_DATA_REVERSE | SEN_MBUS_PCLK_SAMPLE_FALLING,

    .fps         		= 	25,

    .sen_size 			= 	{1280, 720},
    .isp_size 			= 	{1280, 720},
    /* .sen_size 			= 	{720, 480}, */
    /* .isp_size 			= 	{720, 480}, */

    .ops                =   {
        .avin_fps           =   avin_fps,
        .avin_valid_signal  =   avin_valid_signal,
        .avin_mode_det      =   avin_mode_det,
        .sensor_check 		= 	tp9930k_check,
        .init 		        = 	tp9930k_initialize,
        .set_size_fps 		=	tp9930k_set_output_size,
        .power_ctrl         =   tp9930k_power_ctl,


        .sleep 		        =	NULL,
        .wakeup 		    =	NULL,
        .write_reg 		    =	NULL,
        .read_reg 		    =	NULL,
    }
};



