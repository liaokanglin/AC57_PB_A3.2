#include "pr1000.h"
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "generic/jiffies.h"
#include "timer.h"

typedef enum _pr1000_table_format {
    pr1000_format_SD480i = 0, //cvbs ntsc
    pr1000_format_SD576i,     //cvbs pal
    pr1000_format_HD720p,     //ahd 720p
    pr1000_format_HD1080p,    //ahd 1080p
    pr1000_format_CVI,        //cvi
    pr1000_format_HDT,        //hdt
    max_pr1000_format
} PR1000_table_format;

static u8 cur_input_format = pr1000_format_HD1080p; //默认1080p AHD制式

/* #define PR1000_1080  //for TP3803_F23 */
//#define SELF_ADA_ENABLE  //自适应使能

void set_remount_flag(u8 val);
u8 get_remount_flag(void);
void pr1000_init();
void pr1000_mount(PR1000_table_format input_format);


static u32 reset_gpio[4] = {-1, -1, -1, -1};

static u8 init = -1;

static u8 camera1_remount = 0;

//0xB8
//0xB9
#define PR1000_PSTA		1
#define PR1000_NSTA		2


static void *iic = NULL;
static u8 g_pn_status = -1;

#define WRCMD 0xB8
#define RDCMD 0xB9

unsigned char wrPR1000Reg(u16 regID, unsigned char regDat)
{
    u8 ret = 1;

    if (!iic) {
        puts("pr1000 iic err\n");
        return 0;
    }

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

unsigned char rdPR1000Reg(u16 regID, unsigned char *regDat)
{
    u8 ret = 1;

    if (!iic) {
        puts("pr1000 iic err\n");
        return -1;
    }

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

void pr1000_reset(u8 isp_dev)
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

    gpio_direction_output(gpio, 1);
    delay(50000);
    gpio_direction_output(gpio, 0);
    delay(50000);
    gpio_direction_output(gpio, 1);
}

s32 pr1000_id_check()
{
    u16 id = 0;
    u16 id1 = 0;

    delay(50000);

    wrPR1000Reg(0xff, 0x00);

    rdPR1000Reg(0xfc, (unsigned char *)&id);
    id <<= 8;
    rdPR1000Reg(0xfd, (unsigned char *)&id1);
    id |= id1;
    log_i("\npr1000_id %x\n", id);

    if (id == 0x1000) {
        puts("\npr1000_id_check succ\n");
        return 1;
    }

    return 0;
}


static u8 cur_sensor_type = 0xff;

s32 pr1000_check(u8 isp_dev, u32 _reset_gpio, u32 _power_gpio)
{
    if (isp_dev == ISP_DEV_0) {
        return -1;
    }

    log_i("\npr1000_id check %d\n", isp_dev);
    if (!iic) {
        if (isp_dev == ISP_DEV_0) {
            iic = dev_open("iic2", 0);
        } else {
            iic = dev_open("iic2", 0);
        }
        if (!iic) {
            log_i("open iic2 fail\n");
            return -1;
        }
    } else {
        if (cur_sensor_type != isp_dev) {
            puts("\npr1000 check iic err\n\n");
            return -1;
        }
    }

    if (_power_gpio != (u32) - 1) {
        gpio_direction_output(_power_gpio, 1);
        delay(5000);
    }

    if (isp_dev >= sizeof(reset_gpio) / sizeof(reset_gpio[0])) {
        puts("pr1000 dev overflow\n");
        while (1);
    }
    reset_gpio[isp_dev] = _reset_gpio;
    pr1000_reset(isp_dev);

    if (0 == pr1000_id_check()) {
        dev_close(iic);
        iic = NULL;
        puts("\npr1000_id check fail\n\n");
        return -1;
    }

    cur_sensor_type = isp_dev;

    return 0;
}


s32 pr1000_set_output_size(u16 *width, u16 *height, u8 *freq)
{

    /* if (avin_fps()) { */
    /* *freq = 25; */
    /* g_pn_status = PR1000_PSTA; */
    /* } else { */
    /* *freq = 30; */
    /* g_pn_status = PR1000_NSTA; */
    /* } */

    /* *freq = 25; */
    /* *width = 1280-16; */
    /* *height = 720; */

    return 0;
}

static u8 avin_valid_signal()
{
    u8 DetVideo;
    u8 LockStatus;
    u8 st, ch0, ch1;
    u8 j;
    wrPR1000Reg(0xff, 0x00);


    for (j = 0; j < 3; j++) {
        rdPR1000Reg(0x00, &DetVideo);
        rdPR1000Reg(0x01, &LockStatus);

//        printf("DetVideo=0x%x det_video=%d IFMT_STD=%d IFMT_FPS=%d IFMT_RES=%d\n",DetVideo,!!(DetVideo&BIT(7)),((DetVideo&(0x30))>>4), ((DetVideo&(0x6))>>2),DetVideo&(0x3));
//        printf("LockStatus=0x%x Lock_STD=%d det_signal=%d\n",LockStatus,!!(LockStatus&BIT(7)),!!(LockStatus&BIT(0)));
//
//        rdPR1000Reg(0x10, &st);
//        printf("0X10=0x%x\n",st );
//        rdPR1000Reg(0xf2, &st);
//        printf("0XF2=0x%x\n",st );
//        rdPR1000Reg(0xef, &st);
//        printf("0XEF=0x%x\n",st );
//        rdPR1000Reg(0xe7, &st);
//        printf("0XE7=0x%x\n",st );
//        rdPR1000Reg(0xe8, &st);
//        printf("0XE8=0x%x\n",st );

        if (LockStatus & 0xff) {
            return 1;
        }
        delay(5000);
    }
    return 0;
}
static void read_reg_test(void *p)
{
    u8 DetVideo;
    u8 LockStatus;
    u8 st;
    u16 id, id1;
#if 0
    wrPR1000Reg(0xff, 0x00);

    rdPR1000Reg(0x00, &DetVideo);
    rdPR1000Reg(0x01, &LockStatus);


    printf("DetVideo=0x%x det_video=%d IFMT_STD=%d IFMT_FPS=%d IFMT_RES=%d\n", DetVideo, !!(DetVideo & BIT(7)), ((DetVideo & (0x30)) >> 4), ((DetVideo & (0x6)) >> 2), DetVideo & (0x3));
    printf("LockStatus=0x%x Lock_STD=%d det_signal=%d\n", LockStatus, !!(LockStatus & BIT(7)), !!(LockStatus & BIT(0)));

    rdPR1000Reg(0xf2, &st);
    printf("0XF2=0x%x\n", st);
    st = 0x00;
    rdPR1000Reg(0x10, &st);
    printf("0X10=0x%x\n", st);
    rdPR1000Reg(0xE8, &st);
    printf("0XE8=0x%x\n", st);
//    delay(50000);

//    wrPR1000Reg(0xff, 0x00);

    rdPR1000Reg(0xfc, (unsigned char *)&id);
    id <<= 8;
    rdPR1000Reg(0xfd, (unsigned char *)&id1);
    id |= id1;
    puts("\nid:");
    put_u16hex(id);
    return;

#endif
    rdPR1000Reg(0x10, &st);
    printf("0X10=0x%x\n", st);
    //ch0
    static u8 cnt = 0;
    static u8 last_ifmt_res0;
    u8 ifmt_res0;
    rdPR1000Reg(0x00, &ifmt_res0);
    printf("DetVideo=0x%x det_video=%d IFMT_STD=%d IFMT_FPS=%d IFMT_RES=%d\n", ifmt_res0, !!(ifmt_res0 & BIT(7)), ((ifmt_res0 & (0x30)) >> 4), ((ifmt_res0 & (0x6)) >> 2), ifmt_res0 & (0x3));
    /* printf("ifmt_res0===%d\n",ifmt_res0&(0x3));
    */
    if (last_ifmt_res0 != ifmt_res0) {
        last_ifmt_res0 = ifmt_res0;
        cnt = 0;
    }
    if (!(ifmt_res0 & (0x3))) { // cvbs ntsc
        puts("CVBS NTSC\n");
        if (++cnt > 3) {
            cnt = 0;
            if (cur_input_format != pr1000_format_SD480i) {
                cur_input_format = pr1000_format_SD480i;
                set_remount_flag(1);
            }
        }
    } else if ((ifmt_res0 & (0x3)) == 1) { // cvbs pal
        puts("CVBS PAL\n");
        if (++cnt > 3) {
            cnt = 0;
            if (cur_input_format != pr1000_format_SD576i) {
                cur_input_format = pr1000_format_SD576i;
                set_remount_flag(1);
            }
        }
    } else if (((ifmt_res0 & (0x3)) == 3)  && (((ifmt_res0 & (0x30)) >> 4) == 3)) { // 1080p TVI
        puts("TVI\n");
        if (++cnt > 2) {
            cnt = 0;
            if (cur_input_format != pr1000_format_HDT) {
                cur_input_format = pr1000_format_HDT;
                set_remount_flag(1);
            }
        }
    } else if (((ifmt_res0 & (0x3)) == 3)  && (((ifmt_res0 & (0x30)) >> 4) == 2)) { // 1080p AHD
        puts("AHD\n");
        if (++cnt > 2) {
            cnt = 0;
            if (cur_input_format != pr1000_format_HD1080p) {
                cur_input_format = pr1000_format_HD1080p;
                set_remount_flag(1);
            }
        }

    } else if (((ifmt_res0 & (0x3)) == 3)  && (((ifmt_res0 & (0x30)) >> 4) == 1)) { // 1080p CVI
        puts("CVI\n");
        if (++cnt > 2) {
            cnt = 0;
            if (cur_input_format != pr1000_format_CVI) {
                cur_input_format = pr1000_format_CVI;
                set_remount_flag(1);
            }
        }

    }

}
static int wait_signal_valid()
{
    u32 time;

    if (avin_valid_signal()) {
        //信号有效等50ms 待信号稳定
        time = jiffies + msecs_to_jiffies(50);
        while (1) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx pr1000 valid\n");
                return 0;
            }

        }
    } else {
        //信号无效等100ms
        time = jiffies + msecs_to_jiffies(100);
        while (!avin_valid_signal()) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx pr1000 no no no no validxx \n");
                return -1;
            }
        }
    }

    return  0;
}




s32 pr1000_initialize(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    pr1000_reset(isp_dev);
    log_i("\npr1000_init \n");

    pr1000_init();

    if (wait_signal_valid() != 0) {
        log_i("\npr1000_init wait_signal fail\n");
        return -1;
    }
    pr1000_set_output_size(width, height, frame_freq);

    return 0;
}
u8 get_remount_flag(void)
{
    return camera1_remount ;
}
void set_remount_flag(u8 val)
{
    camera1_remount = val;
}

void pr1000_mount(PR1000_table_format input_format)
{
    printf("====Init [%d]", input_format);
    isp_sen_t *c;
    u16 width;
    u16 height;

    switch (input_format) {
    case pr1000_format_SD480i:
        width = 960;
        height = 480;
        list_for_each_camera(c) {
            if (!strcmp((char *)c->logo, "PR1000")) {
                puts("pr1000\n");
                c->mbus_config = SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING | SEN_MBUS_FIELDS;
                c->sen_size.w =  	width;
                c->sen_size.h =  	height;
                c->isp_size.w = 	width;
                c->isp_size.h = 	height;
            }
        }

        for (int i = 0; i < sizeof(PR1000_960x480i60_NTSC_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
            putchar('A');
            wrPR1000Reg(PR1000_960x480i60_NTSC_INI[i].addr, PR1000_960x480i60_NTSC_INI[i].value);
        }

        break;
    case pr1000_format_SD576i:
        width = 960;
        height = 576;
        list_for_each_camera(c) {
            if (!strcmp((char *)c->logo, "PR1000")) {
                puts("pr1000\n");
                c->mbus_config = SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING | SEN_MBUS_FIELDS;
                c->sen_size.w =  	width;
                c->sen_size.h =  	height;
                c->isp_size.w = 	width;
                c->isp_size.h = 	height;
            }
        }
        for (int i = 0; i < sizeof(PR1000_960x576i50_PAL_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
            putchar('A');
            wrPR1000Reg(PR1000_960x576i50_PAL_INI[i].addr, PR1000_960x576i50_PAL_INI[i].value);
        }

        break;
    case pr1000_format_HD1080p:
        width = 1920;
        height = 1080;
        list_for_each_camera(c) {
            if (!strcmp((char *)c->logo, "PR1000")) {
                puts("\npr1000\n");
                c->mbus_config = SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING ;
                c->sen_size.w =  	width;
                c->sen_size.h =  	height;
                c->isp_size.w = 	width;
                c->isp_size.h = 	height;
            }
        }
        for (int i = 0; i < sizeof(PR1000_1080_REG_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
            putchar('A');
            wrPR1000Reg(PR1000_1080_REG_INI[i].addr, PR1000_1080_REG_INI[i].value);
        }
        break;
    case pr1000_format_HD720p:
        width = 1280;
        height = 720;
        list_for_each_camera(c) {
            if (!strcmp((char *)c->logo, "PR1000")) {
                puts("\npr1000\n");
                c->mbus_config = SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING ;
                c->sen_size.w =  	width;
                c->sen_size.h =  	height;
                c->isp_size.w = 	width;
                c->isp_size.h = 	height;
            }
        }
        for (int i = 0; i < sizeof(PR1000_720_REG_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
            wrPR1000Reg(PR1000_720_REG_INI[i].addr, PR1000_720_REG_INI[i].value);
        }
        break;
    case pr1000_format_HDT:
        width = 1920;
        height = 1080;
        list_for_each_camera(c) {
            if (!strcmp((char *)c->logo, "PR1000")) {
                puts("\npr1000\n");
                c->mbus_config = SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING ;
                c->sen_size.w =  	width;
                c->sen_size.h =  	height;
                c->isp_size.w = 	width;
                c->isp_size.h = 	height;
            }
        }
        for (int i = 0; i < sizeof(PR1000_HDT_REG_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
            putchar('A');
            wrPR1000Reg(PR1000_HDT_REG_INI[i].addr, PR1000_HDT_REG_INI[i].value);
        }
        break;
    case pr1000_format_CVI:
        width = 1920;
        height = 1080;
        list_for_each_camera(c) {
            if (!strcmp((char *)c->logo, "PR1000")) {
                puts("\npr1000\n");
                c->mbus_config = SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING ;
                c->sen_size.w =  	width;
                c->sen_size.h =  	height;
                c->isp_size.w = 	width;
                c->isp_size.h = 	height;
            }
        }
        for (int i = 0; i < sizeof(PR1000_CVI_REG_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
            putchar('A');
            wrPR1000Reg(PR1000_CVI_REG_INI[i].addr, PR1000_CVI_REG_INI[i].value);
        }
        break;
    default:
        puts("err input format!\n");
        return;
    }

    delay(50000);

}


void pr1000_init()
{
#ifdef PR1000_1080
    static u32 timer_id = 0;
    int i = 0;
    pr1000_mount(cur_input_format);

#ifdef SELF_ADA_ENABLE
    if (timer_id == 0) {
        timer_id = sys_timer_add(NULL, read_reg_test, 1000);
    }
#endif

#else
    int i = 0;
    /* for (i = 0; i < sizeof(PR1000_720_REG_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) { */
    /* wrPR1000Reg(PR1000_720_REG_INI[i].addr, PR1000_720_REG_INI[i].value); */
    /* } */

    for (i = 0; i < sizeof(PR1000_960x480i60_NTSC_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) {
        wrPR1000Reg(PR1000_960x480i60_NTSC_INI[i].addr, PR1000_960x480i60_NTSC_INI[i].value);
    }


    //mux
    /* for (i = 0; i < sizeof(PR1000_960x480i60_BT656MUX_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) { */
    /* wrPR1000Reg(PR1000_960x480i60_BT656MUX_INI[i].addr, PR1000_960x480i60_BT656MUX_INI[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(PR1000_960x576i50_BT656MUX_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) { */
    /* wrPR1000Reg(PR1000_960x576i50_BT656MUX_INI[i].addr, PR1000_960x576i50_BT656MUX_INI[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(PR1000_720P25_BT656MUX_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) { */
    /* wrPR1000Reg(PR1000_720P25_BT656MUX_INI[i].addr, PR1000_720P25_BT656MUX_INI[i].value); */
    /* } */

    /* for (i = 0; i < sizeof(PR1000_720P30_BT656MUX_INI) / sizeof(_PR1000_REG_TABLE_COMMON); i++) { */
    /* wrPR1000Reg(PR1000_720P30_BT656MUX_INI[i].addr, PR1000_720P30_BT656MUX_INI[i].value); */
    /* } */

#endif

#if 1

//open color bar
    u8 temp ;
    /* wrPR1000Reg(0xff, 0x01); */
    /* rdPR1000Reg(0x4f, &temp); */
    /* temp = (temp | 0x10); */
    /* wrPR1000Reg(0x4f, temp); */

    /* wrPR1000Reg(0xff, 0x02); */
    /* rdPR1000Reg(0x80, &temp); */
    /* temp = (temp | 0xb0); */
    /* wrPR1000Reg(0x80, temp); */

    wrPR1000Reg(0xff, 0x01);
    rdPR1000Reg(0x00, &temp);
    temp |= (BIT(0) | BIT(1));
    wrPR1000Reg(0x00, temp);

    rdPR1000Reg(0x80, &temp);
    temp |= (BIT(0) | BIT(1));
    wrPR1000Reg(0x80, temp);

    wrPR1000Reg(0xff, 0x02);
    rdPR1000Reg(0x00, &temp);
    temp |= (BIT(0) | BIT(1));
    wrPR1000Reg(0x00, temp);

    rdPR1000Reg(0x80, &temp);
    temp |= (BIT(0) | BIT(1));
    wrPR1000Reg(0x80, temp);
#endif

    delay(50000);

}

s32 pr1000_power_ctl(u8 isp_dev, u8 is_work)
{
    return 0;
}

static u8 avin_fps()
{
    u8 status ;

    wrPR1000Reg(0xff, 0x00);

    rdPR1000Reg(0x00, (unsigned char *)&status);
    rdPR1000Reg(0x00, (unsigned char *)&status);
    rdPR1000Reg(0x00, (unsigned char *)&status);

    log_i("\n\n\n\n\n\nch0 : 0x%x\n\n\n\n\n", status);
    rdPR1000Reg(0x20, (unsigned char *)&status);
    rdPR1000Reg(0x20, (unsigned char *)&status);
    rdPR1000Reg(0x20, (unsigned char *)&status);
    log_i("\n\n\n\n\n\nch1 : 0x%x\n\n\n\n\n", status);
    rdPR1000Reg(0x40, (unsigned char *)&status);
    rdPR1000Reg(0x40, (unsigned char *)&status);
    rdPR1000Reg(0x40, (unsigned char *)&status);
    log_i("\n\n\n\n\n\nch2 : 0x%x\n\n\n\n\n", status);
    rdPR1000Reg(0x60, (unsigned char *)&status);
    rdPR1000Reg(0x60, (unsigned char *)&status);
    rdPR1000Reg(0x60, (unsigned char *)&status);
    log_i("\n\n\n\n\n\nch3 : 0x%x\n\n\n\n\n", status);


    if (status & 0x0c) {
        return 0;//30hz
    }

    return 1;//25hz
}



static u8 avin_mode_det(void *parm)
{
    u8 new_status;
    puts("31313131223\n");
    /*
        if (avin_fps()) {
            new_status = PR1000_PSTA;
        } else {
            new_status = PR1000_NSTA;
        }

        if (g_pn_status != new_status) {
            return 1;
        }
    */
    return 0;
}

u8 pr1000_avin_det()
{
    u8 dat = 0;

    if (init == 0) {
        puts("error : pr1000 not init\n");
        return -1;
    }

    if (!iic) {
        iic = dev_open("iic2", 0);
        return -1;
    }


    wrPR1000Reg(0xff, 0x00);

    rdPR1000Reg(0x00, (unsigned char *)&dat);

    if (dat == 0) {
        return -1;
    }

//    printf("pr1000 det %d %x",(!!(dat & BIT(3))),dat);

    return (!!(dat & BIT(3)));
}

REGISTER_CAMERA(PR1000) = {
    .logo 				= 	"PR1000",
    .isp_dev 			= 	ISP_DEV_NONE,
    .in_format 			= 	SEN_IN_FORMAT_UYVY,
    .out_format 		= 	0xff,
    .mbus_type          =   SEN_MBUS_BT656,//SEN_MBUS_BT656,
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING,

    .fps         		= 	25,

#ifdef PR1000_1080
    .sen_size 			= 	{960, 480},
    .isp_size 			= 	{960, 480},
#else
    .sen_size 			= 	{960, 480},
    .isp_size 			= 	{960, 480},
    /* .sen_size 			= 	{1280, 720}, */
    /* .isp_size 			= 	{1280, 720}, */
#endif
    .ops                =   {
        .avin_fps           =   avin_fps,
        .avin_valid_signal  =   avin_valid_signal,
        .avin_mode_det      =   avin_mode_det,
        .sensor_check 		= 	pr1000_check,
        .init 		        = 	pr1000_initialize,
        .set_size_fps 		=	pr1000_set_output_size,
        .power_ctrl         =   pr1000_power_ctl,


        .sleep 		        =	NULL,
        .wakeup 		    =	NULL,
        .write_reg 		    =	NULL,
        .read_reg 		    =	NULL,
    }
};



