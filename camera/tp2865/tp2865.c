#include "tp2865.h"
#include "asm/iic.h"
#include "asm/isp_dev.h"
#include "gpio.h"
#include "generic/jiffies.h"


// #define tp2865_720P
#define tp2865_1080P
static u8 tp2865_res=3;
static u8 avin_fps();
extern unsigned char camera_is_charging();
static u32 reset_gpio[5] = {-1, -1};

//0xB8
//0xB9
#define tp2865K_PSTA		1
#define tp2865K_NSTA		2

/*S_IIC_FUN tp2865_iic_fun;*/
static void *iic = NULL;
static u8 g_pn_status = -1;

#define WRCMD 0x88
#define RDCMD 0x89

typedef struct {
    u8 addr;
    u8 value;
} tp2865_init_regs_t;

void wrtp2865KReg(u16 regID, u16 regDat)
{
    u8 ret = 1;  // 默认设置返回值为1，表示成功，若发生错误则改为0

    // 开始I2C通信
    dev_ioctl(iic, IIC_IOCTL_START, 0);

    // 发送写命令（WRCMD），并检查是否成功
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;  // 如果发送失败，将ret设置为0，表示发生错误
        goto __wend;  // 跳转到错误处理部分
    }

    // 延时50ms，给设备一定时间来处理命令
    delay(50);

    // 发送寄存器ID（regID）
    if (dev_ioctl(iic, IIC_IOCTL_TX, regID)) {
        ret = 0;  // 如果发送失败，将ret设置为0，表示发生错误
        goto __wend;  // 跳转到错误处理部分
    }

    // 延时50ms，给设备时间来处理寄存器ID
    delay(50);

    // 发送寄存器数据（regDat）
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regDat)) {
        ret = 0;  // 如果发送失败，将ret设置为0，表示发生错误
        goto __wend;  // 跳转到错误处理部分
    }

__wend:
    // 如果发生错误，打印错误信息
    if (ret == 0) {
        log_e("iic w err!!!\n");
    }

    // 停止I2C通信
    dev_ioctl(iic, IIC_IOCTL_STOP, 0);
    /* return ret;  // 如果需要返回ret值，可以取消注释这行 */
}


unsigned char rdtp2865KReg(u16 regID, unsigned char *regDat)
{
    u8 ret = 1;  // 初始化返回值为1，表示默认成功

    dev_ioctl(iic, IIC_IOCTL_START, 0);  // 启动I2C通信

    // 发送带起始位的写命令
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, WRCMD)) {
        ret = 0;  // 如果写命令失败，设置返回值为0
        goto __rend;  // 跳转到错误处理代码
    }

    delay(50);  // 等待50ms

    // 发送寄存器ID
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_STOP_BIT, regID)) {
        ret = 0;  // 如果发送寄存器ID失败，设置返回值为0
        goto __rend;  // 跳转到错误处理代码
    }

    delay(50);  // 等待50ms

    // 发送带起始位的读取命令
    if (dev_ioctl(iic, IIC_IOCTL_TX_WITH_START_BIT, RDCMD)) {
        ret = 0;  // 如果发送读取命令失败，设置返回值为0
        goto __rend;  // 跳转到错误处理代码
    }

    delay(50);  // 等待50ms

    // 读取数据并存入regDat指向的内存
    dev_ioctl(iic, IIC_IOCTL_RX_WITH_STOP_BIT, (u32)regDat);  // 从I2C设备读取数据到寄存器

__rend:
    if (ret == 0) {  // 如果发生错误
        log_e("iic r err!!!\n");  // 打印错误信息
    }

    dev_ioctl(iic, IIC_IOCTL_STOP, 0);  // 停止I2C通信

    return ret;  // 返回操作结果，0表示失败，1表示成功
}

extern void delay_2ms(int cnt);
#define delay2ms(t) delay_2ms(t)
void tp2865_reset(u8 isp_dev)
{
    u32 gpio;

    gpio = reset_gpio[isp_dev];

     printf("\n gpio=====================%d.%d,%d,%d\n",gpio,isp_dev,reset_gpio[0],reset_gpio[1]);
    gpio_direction_output(gpio, 1);
    delay2ms(50);
    gpio_direction_output(gpio, 0);
    delay2ms(50);
    gpio_direction_output(gpio, 1);
}

u16 tp2865_dvp_rd_reg(u16 addr)
{
    u8 val;
    rdtp2865KReg((u16)addr, &val);
    return val;
}
s32 tp2865_id_check()
{
    u16 id = 0;
    u16 id1 = 0;
    u8 id_check_cont;

    for (id_check_cont = 0; id_check_cont <= 3; id_check_cont++) {

        rdtp2865KReg(0xfe, (unsigned char *)&id);
        id <<= 8;
        rdtp2865KReg(0xff, (unsigned char *)&id1);
        id |= id1;
        puts("\nid:");
        log_d("id 0x%x\n", id);
        if (id == 0x2865) {
            puts("\n tp2865_id_check succ\n");
            return 1;
        }

    }

    return 0;
}


/*void tp2865K_iic_set(u8 isp_dev)*/
/*{*/
/*iic_select(&tp2865_iic_fun, isp_dev);*/
/*}*/


static u8 cur_sensor_type = 0xff;
static void av_sensor_power_ctrl(u32 _power_gpio, u32 on_off)
{
    u32 gpio = _power_gpio;
    gpio_direction_output(gpio, on_off);
    delay(5000);
}

s32 tp2865_check(u8 isp_dev, u32 _reset_gpio, u32 _power_gpio)
{
    log_i("\n========p2865 check %d\n", isp_dev);

    if (isp_dev == ISP_DEV_0) {  // 如果当前ISP设备为ISP_DEV_0
    log_i("a0\n");  // 打印日志 "a0"
    return -1;  // 返回-1，表示错误或特殊情况
    }

    if(cur_sensor_type != 0xff){  // 如果当前传感器类型不等于0xff
        if((isp_dev == ISP_DEV_1) || (isp_dev == ISP_DEV_2)){  // 如果ISP设备是ISP_DEV_1或ISP_DEV_2
            return 0;  // 返回0，表示操作成功或者继续执行
        }
    }


    /* if (isp_dev == ISP_DEV_1) { */
    /* return -1; */
    /* } */
    if (!iic) {
        if (isp_dev == ISP_DEV_0) {
            iic = dev_open("iic1", 0);
        }else{
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
    log_i("=================tp2865 check: %d,%d\n", cur_sensor_type,isp_dev);
    reset_gpio[isp_dev] = _reset_gpio;
    av_sensor_power_ctrl(_power_gpio, 1);
    tp2865_reset(isp_dev);


    if (0 == tp2865_id_check()) {
        dev_close(iic);
        iic = NULL;

        puts("\ntp2865_id check fail\n\n");

        return -1;
    }

    cur_sensor_type = isp_dev;

    return 0;
}





static u8 tp2865_video_type[ ][10]={
"720p 60",
"720p 50",
"1080p 30",
"1080p 25",
"720p 30",
"720p 25",
"SD",
"Other",
};

s8 tp2865_avin_valid_signal(u8 channel)//channel:0:port1数据   1:port2数据
{
    u8 status = 0;
    u8 DetVideo = 0;

    if (cur_sensor_type == 0xff) {
        return -1;
    }

    if(channel == 0){
        wrtp2865KReg(0x40, 0x00);
    }else if(channel == 1){
        wrtp2865KReg(0x40, 0x01);
    }

    rdtp2865KReg(0x01, &status);
    rdtp2865KReg(0x03, &DetVideo);

    printf(">>>>>>>>>>>>>>>>>>tp2865_avin_valid_signal : %d, 0x%02X, 0x%02X\n", channel, status, DetVideo);

    if (status & 0x08) {
        printf("====eqgain : %d , %s , type : %s\n", ((DetVideo >> 4) & 0x0F), ((DetVideo & 0x08) ? "TVI v2.0" : "TVI v1.0"), tp2865_video_type[DetVideo & 0x07]);
        return 1;
    }else{
        return 0;
    }
}

void tp2865_bwhite_color_switch(u8 channel,u8 enable)//channel:0:port1数据   1:port2数据  //enable: 0:黑白效果   1:彩色效果
{
    u8 temp = 0;
    static u8 flag = 0xff;
    if (cur_sensor_type == 0xff) {
        return;
    }

    temp = channel * 2 + enable;
    if (flag == temp) {
        return;
    }

    tp2865_avin_valid_signal(channel);

    printf(">>>>>>>>>>>>>>>>>>tp2865_bwhite_color_switch : %d , %d\n", channel, enable);

    flag = temp;
    if(channel == 0){
        wrtp2865KReg(0x40, 0x00);
    }else if(channel == 1){
        wrtp2865KReg(0x40, 0x01);
    }

    if(enable == 0){
        wrtp2865KReg(0x12, 0x00);
    }else if(enable == 1){
        wrtp2865KReg(0x12, 0x40);
    }
    flag = temp;
}

s32 tp2865_set_output_size(u16 *width, u16 *height, u8 *freq)
{
    /*
        if (avin_fps()) {  // 检查输入的帧率，如果有帧率信息
            *freq = 25;  // 设置频率为25
            g_pn_status = tp2865K_PSTA;  // 设置状态为tp2865K_PSTA
        } else {
            *freq = 30;  // 如果没有帧率信息，设置频率为30
            g_pn_status = tp2865K_NSTA;  // 设置状态为tp2865K_NSTA
        }
    */

    if(db_select("reg")==1) {  // 如果数据库中“reg”值为1
        *freq = 25;  // 设置频率为25
        *width = 1280;  // 设置宽度为1280
        *height = 720;  // 设置高度为720
    } else {  // 如果数据库中“reg”值不是1（即为0）
        *freq = 25;  // 设置频率为25
        *width = 1920;  // 设置宽度为1920
        *height = 1080;  // 设置高度为1080
    }

    return 0;  // 返回0，表示操作成功
}


static u8 avin_valid_signal()
{
    u8 j;
    u8 DetVideo;
    u8 LockStatus;

#if 0//def tp2865_1080P
    /* wrtp2865KReg(0xff, 0x00); */
    for (j = 0; j < 3; j++) {  // 循环3次进行视频信号检测
        rdtp2865KReg(0x01, &LockStatus);  // 读取寄存器0x01的状态到LockStatus
        rdtp2865KReg(0x03, &DetVideo);   // 读取寄存器0x03的状态到DetVideo

        printf("\n DetVideo====%x, LockStatus====%x\n ", DetVideo, LockStatus); // 打印读取到的信号状态

        // 检查LockStatus的第3位是否为1，且DetVideo的低三位是否为0x07
        // 这表明视频信号锁定正常并且视频类型有效
        if ((LockStatus & BIT(3)) && (DetVideo & 0x07) != 0x07) {
            return 1;  // 如果不满足条件，返回1，表示信号无效
        }
        delay2ms(5);  // 延时5毫秒，等待信号稳定
    }
    return 0;  // 如果经过3次检查后条件仍然不满足，则返回0，表示信号无效
#endif


#if 1//def tp2865_720P
    wrtp2865KReg(0x40, 0x00);  // 向寄存器0x40写入0x00，配置设备为720P模式
    for (j = 0; j < 3; j++) {  // 循环3次，尝试读取设备状态
        rdtp2865KReg(0x01, &LockStatus);  // 读取寄存器0x01的状态到LockStatus
        rdtp2865KReg(0x03, &DetVideo);   // 读取寄存器0x03的状态到DetVideo

        printf("\n DetVideo====%x, LockStatus====%x\n ", DetVideo, LockStatus);  // 打印读取到的信号状态

        // 检查DetVideo的低三位是否为0x07，表示视频信号锁定且有效
        if ((DetVideo & 0x07) != 0x07) {
            return 1;  // 如果低三位不等于0x07，表示信号无效，返回1
        }
        delay2ms(5);  // 延时5毫秒，等待信号稳定
    }
    return 0;  // 如果3次检查后仍未满足条件，返回0，表示信号无效

    // 向寄存器0x40写入0x00进行设备初始化
    wrtp2865KReg(0x40, 0x00);  // 配置寄存器
    for (j = 0; j < 3; j++) {  // 循环检查3次
        rdtp2865KReg(0x01, &LockStatus);  // 读取锁定状态
        // 检查LockStatus的第6位和第5位是否都为1
        if ((LockStatus & 0x60) == 0X60) {
            return 1;  // 如果锁定状态符合预期，返回1，表示信号有效
        }
        os_time_dly(20);  // 延时20毫秒，等待设备响应
    }
    return 0;  // 如果3次检查后仍未满足条件，返回0，表示信号无效
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
                puts("\n xxxxx tp2865 valid\n");
                return 0;
            }

        }
    } else {
        //信号无效等100ms
        time = jiffies + msecs_to_jiffies(100);
        while (!avin_valid_signal()) {
            if (time_after(jiffies, time)) {
                puts("\n xxxxx tp2865 no no no no validxx \n");
                return -1;
            }
        }
    }
#endif

    return  0;
}


/*#ifdef tp2865_720P
const tp2865_init_regs_t HDA_720_REG_INIT[] = {
    0x40, 0x04,
    0x02, 0xce,
    0x07, 0xc0,
    0x0b, 0xc0,
    0x0c, 0x03,
    0x0d, 0x71,
    0x15, 0x13,
    0x16, 0x15,
    0x17, 0x00,
    0x18, 0x1b,
    0x19, 0xd0,
    0x1a, 0x25,
    0x1c, 0x07,
    0x1d, 0xbc,
    0x20, 0x40,
    0x21, 0x46,
    0x22, 0x36,
    0x25, 0xfe,
    0x26, 0x01,

    0x2A, 0x30,
//    0x2A, 0x3C,//模拟输出蓝色视频信号


    0x2b, 0x60,
    0x2c, 0x3a,
    0x2d, 0x5a,
    0x2e, 0x40,
    0x30, 0x9e,
    0x31, 0x20,
    0x32, 0x10,
    0x33, 0x90,
    0x38, 0x40,
    0x35, 0x25,
    0x39, 0x08,
    0x42, 0xf0,

    0x44, 0x01,
    0x45, 0x01,

    0x51, 0x00,
    0xe7, 0xf0,
    0xe9, 0x41,
    0xea, 0x03,
    0xeb, 0x03,

    0xf6, 0x00,//11:VD1使用CVBS0  00:VD1使用CVBS1
    0xf7, 0x11,//11:VD2使用CVBS0  00:VD2使用CVBS1
//    0xf6, 0x40,
//    0xf7, 0x51,


    0x40, 0x08,
    0x02, 0x80,
    0x03, 0x80,
    0x04, 0x80,
    0x05, 0x80,
    0x06, 0x80,
    0x15, 0x04,
};
#endif
*/
/* #ifdef tp2865_1080P
const tp2865_init_regs_t HDA_1080_REG_INIT[] = {

    0x02, 0xCC,
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
 */
  #if 1//def tp2865_720P
const tp2865_init_regs_t HDA_720_REG_INIT[] = {
    0x40, 0x04,
    0x02, 0xce,
    0x07, 0xc0,
    0x0b, 0xc0,
    0x0c, 0x03,
    0x0d, 0x71,
    0x15, 0x13,
    0x16, 0x15,
    0x17, 0x00,
    0x18, 0x1b,
    0x19, 0xd0,
    0x1a, 0x25,
    0x1c, 0x07,
    0x1d, 0xbc,
    0x20, 0x40,
    0x21, 0x46,
    0x22, 0x36,
    0x25, 0xfe,
    0x26, 0x01,

    0x2A, 0x30,
//    0x2A, 0x3C,//模拟输出蓝色视频信号


    0x2b, 0x60,
    0x2c, 0x3a,
    0x2d, 0x5a,
    0x2e, 0x40,
    0x30, 0x9e,
    0x31, 0x20,
    0x32, 0x10,
    0x33, 0x90,
    0x38, 0x40,
    0x35, 0x25,
    0x39, 0x08,
    0x42, 0xf0,

    0x44, 0x01,
    0x45, 0x01,
    0x51, 0x00,
    0xe7, 0xf0,
    0xe9, 0x41,
    0xea, 0x03,
    0xeb, 0x03,

    // 0xf6, 0x00,//11:VD1使用CVBS0  00:VD1使用CVBS1
    // 0xf7, 0x11,//11:VD2使用CVBS0  00:VD2使用CVBS1
//    0xf6, 0x40,
//    0xf7, 0x51,

    0xf6, 0x00,
    0xf7, 0x11,


    0x40, 0x08,
    0x02, 0x80,
    0x03, 0x80,
    0x04, 0x80,
    0x05, 0x80,
    0x06, 0x80,
    0x15, 0x04,
};
#endif
#ifdef tp2865_1080P

const tp2865_init_regs_t HDA_1080_REG_INIT[] = {

    0x40, 0x04,
    0x02, 0xc8,
    0x07, 0xc0,
    0x0b, 0xc0,
    0x0c, 0x03,
    0x0d, 0x70,
    0x15, 0x03,
    0x16, 0xd0,
    0x17, 0x80,
    0x18, 0x2a,
    0x19, 0x38,
    0x1a, 0x47,
    0x1c, 0x8a,
    0x1d, 0x4e,
    0x20, 0x3c,
    0x21, 0x46,
    0x22, 0x36,
    0x25, 0xfe,
    0x26, 0x01,

    0x27, 0xad,
//    0x2A, 0x30,
//    0x2A, 0x3C,//模拟输出蓝色视频信号


    0x2b, 0x60,
    0x2c, 0x3a,
    0x2d, 0x48,
    0x2e, 0x40,
    0x30, 0x52,
    0x31, 0xc3,
    0x32, 0x7d,
    0x33, 0xa0,
    0x35, 0x25,
    0x39, 0x0c,
    0x42, 0xf0,
    0x44, 0x03,
    0x45, 0x03,
    0x51, 0x00,
    0xe7, 0xf0,
    0xe8, 0x00,
    0xe9, 0x41,
    0xea, 0x03,
    0xeb, 0x03,
    0xf1, 0x30,
    0xf6, 0x00,//11:VD1使用VIN1  00:VD1使用VIN2
    0xf7, 0x11,//11:VD2使用VIN1  00:VD2使用VIN2
//    0xf6, 0x40,
//    0xf7, 0x51,


    0x40, 0x08,
    0x02, 0x80,
    0x03, 0x80,
    0x04, 0x80,
    0x05, 0x80,
    0x06, 0x80,
    0x15, 0x04,

};
#endif

void tp2865_dump_iic_read()
{
    u16 i = 0;      // 循环计数器，遍历寄存器地址
    u8 page = 0;    // 当前的页码
    u8 status = 0;  // 存储读取的寄存器值

    page = 0;  // 设置初始页码为 0
    wrtp2865KReg(0x40, page);  // 向寄存器 0x40 写入当前页码
    printf("\n\n>>>>>>>>>>>>>>>page %d\n", page);  // 打印当前页码

    // 读取并打印当前页的所有寄存器值
    for(i = 0; i <= 0xff; i++) {  // 遍历所有 0x00 到 0xFF 的寄存器地址
        rdtp2865KReg(i, &status);  // 读取寄存器 i 的值到 status 中
        printf("0x%02X read: 0x%02X\n", i, status);  // 打印寄存器地址和对应的值
    }

    page = 1;  // 设置页码为 1
    wrtp2865KReg(0x40, page);  // 向寄存器 0x40 写入页码 1
    printf("\n\n>>>>>>>>>>>>>>>page %d\n", page);  // 打印当前页码

    // 读取并打印当前页的所有寄存器值
    for(i = 0; i <= 0xff; i++) {
        rdtp2865KReg(i, &status);  // 读取寄存器 i 的值到 status 中
        printf("0x%02X read: 0x%02X\n", i, status);  // 打印寄存器地址和对应的值
    }

    page = 4;  // 设置页码为 4
    wrtp2865KReg(0x40, page);  // 向寄存器 0x40 写入页码 4
    printf("\n\n>>>>>>>>>>>>>>>page %d\n", page);  // 打印当前页码

    // 读取并打印当前页的所有寄存器值
    for(i = 0; i <= 0xff; i++) {
        rdtp2865KReg(i, &status);  // 读取寄存器 i 的值到 status 中
        printf("0x%02X read: 0x%02X\n", i, status);  // 打印寄存器地址和对应的值
    }

    page = 8;  // 设置页码为 8
    wrtp2865KReg(0x40, page);  // 向寄存器 0x40 写入页码 8
    printf("\n\n>>>>>>>>>>>>>>>page %d\n", page);  // 打印当前页码

    // 读取并打印当前页的所有寄存器值
    for(i = 0; i <= 0xff; i++) {
        rdtp2865KReg(i, &status);  // 读取寄存器 i 的值到 status 中
        printf("0x%02X read: 0x%02X\n", i, status);  // 打印寄存器地址和对应的值
    }
}


void tp2865_dump_timeout(void *priv)
{
    int flag = 0;
    static u8 cnt = 0;  // 计数器，用于切换操作
    static u32 num = 0; // 计数器，用于超时计数

    flag = (int)priv;  // 获取传入的标志参数
    if (flag) {  // 如果 flag 为非零，表示需要进行 I2C 数据读取
        tp2865_dump_iic_read();  // 调用函数进行 I2C 数据读取
        return;  // 读取后返回
    }

    num++;  // 每次函数调用，超时计数增加

    printf("==========tp2865_dump_timeout : %d , %d\n", cnt, num);  // 输出当前的 cnt 和 num 值

    // 每隔21次超时，进行切换摄像头操作
    if ((num % 21) != 20) {
        return;  // 如果没有达到21次，直接返回，不进行摄像头切换
    }

    puts(">>>>>>>>>>>>switch camram\n");  // 输出切换摄像头的提示信息

    wrtp2865KReg(0x40, 0x04);  // 向寄存器0x40写入0x04，可能用于切换操作

    // 根据 cnt 的值来控制不同的操作，循环切换不同的配置
    if (cnt == 0) {  // 当 cnt 为 0 时，进行第一次配置
        cnt = 1;  // 更新 cnt 为 1
        wrtp2865KReg(0xf6, 0x11);  // 向寄存器 0xf6 写入 0x11
        printf("0x%02X write : 0x%02X\n", 0xf6, 0x11);  // 输出写入操作的寄存器和数据
        wrtp2865KReg(0xf7, 0x00);  // 向寄存器 0xf7 写入 0x00
        printf("0x%02X write : 0x%02X\n", 0xf7, 0x00);  // 输出写入操作的寄存器和数据
    } else if (cnt == 1) {  // 当 cnt 为 1 时，进行第二次配置
        cnt = 2;  // 更新 cnt 为 2
        wrtp2865KReg(0xf6, 0x11);  // 向寄存器 0xf6 写入 0x11
        printf("0x%02X write : 0x%02X\n", 0xf6, 0x11);  // 输出写入操作的寄存器和数据
        wrtp2865KReg(0xf7, 0x11);  // 向寄存器 0xf7 写入 0x11
        printf("0x%02X write : 0x%02X\n", 0xf7, 0x11);  // 输出写入操作的寄存器和数据
    } else if (cnt == 2) {  // 当 cnt 为 2 时，进行第三次配置
        cnt = 3;  // 更新 cnt 为 3
        wrtp2865KReg(0xf6, 0x00);  // 向寄存器 0xf6 写入 0x00
        printf("0x%02X write : 0x%02X\n", 0xf6, 0x00);  // 输出写入操作的寄存器和数据
        wrtp2865KReg(0xf7, 0x00);  // 向寄存器 0xf7 写入 0x00
        printf("0x%02X write : 0x%02X\n", 0xf7, 0x00);  // 输出写入操作的寄存器和数据
    } else if (cnt == 3) {  // 当 cnt 为 3 时，进行第四次配置
        cnt = 0;  // 更新 cnt 为 0，重新开始
        wrtp2865KReg(0xf6, 0x00);  // 向寄存器 0xf6 写入 0x00
        printf("0x%02X write : 0x%02X\n", 0xf6, 0x00);  // 输出写入操作的寄存器和数据
        wrtp2865KReg(0xf7, 0x11);  // 向寄存器 0xf7 写入 0x11
        printf("0x%02X write : 0x%02X\n", 0xf7, 0x11);  // 输出写入操作的寄存器和数据
    }
}




extern void tp2865_init();
int tp2865K_config_SENSOR(u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    static u8 tp2865K_init_flag=0;

    if(tp2865K_init_flag){
       return 0;
    }

    tp2865_init();
    tp2865K_init_flag=1;

/******************************************/
//port2输出蓝屏   port1输出黑屏
//    wrtp2865KReg(0x40, 0x00);
//    wrtp2865KReg(0x2A, 0x38);//黑屏
//    wrtp2865KReg(0x40, 0x01);
//    wrtp2865KReg(0x2A, 0x3C);//蓝屏
/******************************************/



    if (wait_signal_valid() != 0) {
        return -1;
    }

#if 1//def tp2865_720P
    *format = SEN_IN_FORMAT_UYVY;
#endif

    tp2865_set_output_size(width, height, frame_freq);
    tp2865_bwhite_color_switch(0,1);
    return 0;
}
u8 valid_vin1_signal()
{
    u8 j;
    u8 DetVideo;
    u8 LockStatus;

    // 设置TP2865K寄存器，可能是初始化配置或清除某个状态
    wrtp2865KReg(0x40, 0x00);

    // 重复检测视频信号10次
    for (j = 0; j < 10; j++) {
        // 读取锁定状态寄存器，LockStatus表示当前信号是否锁定
        rdtp2865KReg(0x01, &LockStatus);
        // 读取检测到的视频信号状态寄存器，DetVideo表示当前视频信号的检测状态
        rdtp2865KReg(0x03, &DetVideo);

        // 调试输出：打印DetVideo和LockStatus的值
        // printf("\n DetVideo====%x, LockStatus====%x\n ", DetVideo, LockStatus);
        
        // 如果检测到的视频信号为0x0b，且tp2865_res为1，则进行数据库更新并延时2秒
        if(DetVideo == 0x0b && tp2865_res == 1 && db_select("sxt") != 1){
            db_update("reg", 0);  // 更新数据库中的"reg"值为0
            db_flush();  // 将更新的数据写入数据库
            os_time_dly(2);  // 延时2秒，可能是等待硬件反应
            cpu_reset();  // 重置CPU（注释掉的代码）
        }
        // 如果检测到的视频信号为0x0d，且tp2865_res为0，则进行数据库更新并延时2秒
        else if(DetVideo == 0x0d && tp2865_res == 0 && db_select("sxt") != 1){
            db_update("reg", 1);  // 更新数据库中的"reg"值为1
            db_flush();  // 将更新的数据写入数据库
            os_time_dly(2);  // 延时2秒，可能是等待硬件反应
            cpu_reset();  // 重置CPU（注释掉的代码）
        }

        // 如果DetVideo的低3位不是0x07，表示信号不正常，返回1表示无效信号
        if ((DetVideo & 0x07) != 0x07 ) {
            return 1;
        }

        // 延时5毫秒，防止快速重复读取
        delay2ms(5);
    }

    // 如果通过了10次检测，表示信号有效，返回0
    return 0;
}

u8 valid_vin2_signal()
{
    u8 j;
    u8 DetVideo;
    u8 LockStatus;

    // 设置TP2865K寄存器，可能是初始化配置或清除某个状态
    wrtp2865KReg(0x40, 0x01);

    // 重复检测视频信号10次
    for (j = 0; j < 10; j++) {
        // 读取锁定状态寄存器，LockStatus表示当前信号是否锁定
        rdtp2865KReg(0x01, &LockStatus);
        // 读取检测到的视频信号状态寄存器，DetVideo表示当前视频信号的检测状态
        rdtp2865KReg(0x03, &DetVideo);

        // 调试输出：打印DetVideo和LockStatus的值
        // printf("\n DetVideo2====%x, LockStatus2====%x\n ", DetVideo, LockStatus);

        // 如果检测到的视频信号为0x0b，且tp2865_res为1，则进行数据库更新并延时2秒
        // if(DetVideo == 0x0b && tp2865_res == 1 && camera_is_charging() == 1){
        //     db_update("reg", 0);  // 更新数据库中的"reg"值为0
        //     db_flush();  // 将更新的数据写入数据库
        //     os_time_dly(2);  // 延时2秒，可能是等待硬件反应
        //     cpu_reset();  // 重置CPU（注释掉的代码）
        // }
        // // 如果检测到的视频信号为0x0d，且tp2865_res为0，则进行数据库更新并延时2秒
        // else if(DetVideo == 0x0d && tp2865_res == 0 && camera_is_charging() == 1){
        //     db_update("reg", 1);  // 更新数据库中的"reg"值为1
        //     db_flush();  // 将更新的数据写入数据库
        //     os_time_dly(2);  // 延时2秒，可能是等待硬件反应
        //     cpu_reset();  // 重置CPU（注释掉的代码）
        // }

        // 如果DetVideo的低3位不是0x07，表示信号不正常，返回1表示无效信号
        if ((DetVideo & 0x07) != 0x07 && camera_is_charging() == 0) {
            return 1;
        }

        // 延时5毫秒，防止快速重复读取
        delay2ms(5);
    }

    // 如果通过了10次检测，表示信号有效，返回0
    return 0;
}

s32 tp2865_initialize(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq)
{
    puts("\ntp2865_init \n");

    if (tp2865K_config_SENSOR(width, height, format, frame_freq) != 0) {
        puts("\ntp2865_init fail\n");
        return -1;
    }

//    sys_timer_add(NULL,tp2865_dump_timeout,1000);
//    sys_timeout_add((void *)1,tp2865_dump_timeout,10*1000);

    return 0;
}




void tp2865_init()
{
    u8 v = 0;
    puts("\ntp2865_init========================== \n");  // 输出初始化提示信息

    tp2865_res = db_select("reg");  // 从数据库中读取配置值（1 或 0），决定使用哪一组初始化寄存器
    if (tp2865_res == 1) {  // 如果tp2865_res为1，使用720P配置
        for (int i = 0; i < sizeof(HDA_720_REG_INIT) / sizeof(tp2865_init_regs_t); i++) {  // 遍历HDA_720_REG_INIT数组
            wrtp2865KReg(HDA_720_REG_INIT[i].addr, HDA_720_REG_INIT[i].value);  // 通过I2C写入每个寄存器的值
        }
    } else {  // 如果tp2865_res不为1，使用1080P配置
        for (int i = 0; i < sizeof(HDA_1080_REG_INIT) / sizeof(tp2865_init_regs_t); i++) {  // 遍历HDA_1080_REG_INIT数组
            wrtp2865KReg(HDA_1080_REG_INIT[i].addr, HDA_1080_REG_INIT[i].value);  // 通过I2C写入每个寄存器的值
        }
    }

    // 以下代码块已注释掉，可能用于调试验证寄存器值是否正确
//     for (int ii = 0; ii < sizeof(HDA_720_REG_INIT) / sizeof(tp2865_init_regs_t); ii++) {
//         v = 0x00;
//         rdtp2865KReg(HDA_720_REG_INIT[ii].addr, &v);  // 读取寄存器值
//         printf("reg0x%04x =0x%02x->0x%02x\n", HDA_720_REG_INIT[ii].addr, HDA_720_REG_INIT[ii].value, v);  // 输出寄存器地址和值
//         if (HDA_720_REG_INIT[ii].value != v)  // 如果寄存器值不匹配，输出提示
//             printf("Value Ch\n");
//     }
}


s32 tp2865_power_ctl(u8 isp_dev, u8 is_work)
{
    return 0;
}

static u8 avin_fps()
{
    return 1;
#ifdef tp2865_1080P
    return 1;
#else
    u8 status ;
    rdtp2865KReg(0x88, &status);
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
            new_status = tp2865K_PSTA;
        } else {
            new_status = tp2865K_NSTA;
        }

        if (g_pn_status != new_status) {
            return 1;
        }
    */
    return 0;
}
static const u16 rec_pix_w[] = {1920, 1280};
static const u16 rec_pix_h[] = {1088, 720};

REGISTER_CAMERA(tp2865K) = {
    .logo               =   "tp2865",              // 相机的标识符，这里是 "tp2865"
    .isp_dev            =   ISP_DEV_NONE,           // 选择的ISP设备，这里表示没有使用ISP
    .in_format          =   SEN_IN_FORMAT_UYVY,    // 输入格式为 UYVY（通常是YCbCr 4:2:2格式）
    .out_format         =   ISP_OUT_FORMAT_YUV,    // 输出格式为 YUV 格式（标准的颜色空间格式）

#if 1 // 条件编译，选择720P格式
    .mbus_type          =   SEN_MBUS_BT656,        // 选择 BT656 协议作为数据总线类型，常用于高清视频流
#else
    .mbus_type          =   SEN_MBUS_BT1120,        // 备用选择 BT1120 协议
#endif

    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING | SEN_MBUS_DATA_REVERSE, */
    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_RISING| SEN_MBUS_DATA_REVERSE, */
    /* .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_RISING, */
    .mbus_config        =   SEN_MBUS_DATA_WIDTH_8B | SEN_MBUS_PCLK_SAMPLE_FALLING,  // 配置数据宽度为8位，时钟上升沿采样

    .fps                =   25,                       // 设置帧率为25帧每秒

#if 0 // 条件编译，选择720P解析度
    .sen_size           =   {1280, 720},             // 设置传感器分辨率为1280x720（HD）
    .isp_size           =   {1280, 720},             // 设置ISP的输出分辨率为1280x720（HD）
#else
    .sen_size           =   {1920, 1080},            // 设置传感器分辨率为1920x1080（Full HD）
    .isp_size           =   {1920, 1080},            // 设置ISP的输出分辨率为1920x1080（Full HD）
#endif

    .ops                =   {
        .avin_fps           =   avin_fps,               // 设置模拟输入的视频帧率，`avin_fps`是一个外部函数或变量
        .avin_valid_signal  =   avin_valid_signal,      // 设置模拟输入信号有效性的判断，`avin_valid_signal`是一个外部函数或变量
        .avin_mode_det      =   avin_mode_det,          // 设置模拟输入模式的检测，`avin_mode_det`是一个外部函数或变量
        .sensor_check       =   tp2865_check,           // 设置传感器检查函数，`tp2865_check` 是该传感器的初始化检查函数
        .init               =   tp2865_initialize,      // 传感器初始化函数，`tp2865_initialize` 用于初始化TP2865传感器
        .set_size_fps       =   tp2865_set_output_size, // 设置输出的尺寸和帧率，`tp2865_set_output_size` 用于设置输出的分辨率和帧率
        .power_ctrl         =   tp2865_power_ctl,       // 设置电源控制函数，`tp2865_power_ctl` 用于控制传感器的电源

        .sleep              =   NULL,                   // 设置休眠函数，当前未实现
        .wakeup             =   NULL,                   // 设置唤醒函数，当前未实现
        .write_reg          =   wrtp2865KReg,           // 设置写寄存器函数，`wrtp2865KReg` 用于写寄存器值到传感器
        .read_reg           =   tp2865_dvp_rd_reg,      // 设置读寄存器函数，`tp2865_dvp_rd_reg` 用于从传感器读取寄存器值
    }
};




