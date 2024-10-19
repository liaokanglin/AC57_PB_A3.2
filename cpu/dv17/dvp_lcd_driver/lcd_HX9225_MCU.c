
#include "generic/typedef.h"
#include "asm/imb.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/pap.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "os/os_api.h"
#include "gpio.h"


#define delay2ms(t) delay_2ms(t)
#define SOFT_SIM
#define USE_9225

#ifdef LCD_ILI9225G_8BIT_MCU//LCD_9225_322_220X176_MCU

extern void clr_wdt();

//PE5
#define LCD_RST_OUT()    //gpio_direction_output(IO_PORTE_05, 0)
#define LCD_RST_H()     gpio_direction_output(IO_PORTE_05,1)//LCD_RST_OUT();PORTH_OUT |=  BIT(1)
#define LCD_RST_L()     gpio_direction_output(IO_PORTE_05, 0)//LCD_RST_OUT();PORTH_OUT &=~ BIT(1)

//PH1
#define LCD_CS_OUT()    //gpio_direction_output(IO_PORTH_01, 0)
#define LCD_CS_H()      gpio_direction_output(IO_PORTH_01, 1)
#define LCD_CS_L()      gpio_direction_output(IO_PORTH_01, 0)

//PH2
#define LCD_RS_OUT()    //gpio_direction_output(IO_PORTH_02, 0)
#define LCD_RS_H()      gpio_direction_output(IO_PORTH_02, 1)
#define LCD_RS_L()      gpio_direction_output(IO_PORTH_02, 0)

#ifdef SOFT_SIM
//PH3
#define LCD_WR_OUT()    //gpio_direction_output(IO_PORTH_03, 0)
#define LCD_WR_H()      gpio_direction_output(IO_PORTH_03, 1)
#define LCD_WR_L()      gpio_direction_output(IO_PORTH_03, 0)
//PH4
#define LCD_RD_OUT()    //gpio_direction_output(IO_PORTH_04, 0)
#define LCD_RD_H()      gpio_direction_output(IO_PORTH_04, 1)
#define LCD_RD_L()      gpio_direction_output(IO_PORTH_04, 0)

//#define LCD_PORT_OUT()  PORTG_DIR &=~ 0xff00;PORTG_PU |= 0xff00
//#define LCD_PORT_IN()   //PORTG_DIR |=  0xff00;PORTG_PU |= 0xff00
//#define LCD_PORT_W      //PORTG_OUT
//#define LCD_PORT_R      //PORTG_IN
#endif//end of SOFT_SIM

#if 1
#define LCD_PORT_OUT() 	{gpio_direction_output(IO_PORTG_08, 0);gpio_set_pull_up(IO_PORTG_08, 1);\
						 gpio_direction_output(IO_PORTG_09, 0);gpio_set_pull_up(IO_PORTG_09, 1);\
						 gpio_direction_output(IO_PORTG_10, 0);gpio_set_pull_up(IO_PORTG_10, 1);\
						gpio_direction_output(IO_PORTG_11, 0);gpio_set_pull_up(IO_PORTG_11, 1);\
						gpio_direction_output(IO_PORTG_12, 0);gpio_set_pull_up(IO_PORTG_12, 1);\
						gpio_direction_output(IO_PORTG_13, 0);gpio_set_pull_up(IO_PORTG_13, 1);\
						gpio_direction_output(IO_PORTG_14, 0);gpio_set_pull_up(IO_PORTG_14, 1);\
						gpio_direction_output(IO_PORTG_15, 0);gpio_set_pull_up(IO_PORTG_15, 1);}

#define LCD_PORT_IN()  //gpio_direction_input(IO_PORTG_08);gpio_set_pull_down(IO_PORTG_08, 1);


#define LCD_PORT_W     {gpio_direction_output(IO_PORTG_08, 0);\
						gpio_direction_output(IO_PORTG_09, 0);\
						gpio_direction_output(IO_PORTG_10, 0);\
						gpio_direction_output(IO_PORTG_11, 0);\
						gpio_direction_output(IO_PORTG_12, 0);\
						gpio_direction_output(IO_PORTG_13, 0);\
						gpio_direction_output(IO_PORTG_14, 0);\
						gpio_direction_output(IO_PORTG_15, 0);}

#endif




static void lcd_io_init()
{
    LCD_RST_OUT();
    LCD_RS_OUT();
    LCD_CS_OUT();

    LCD_RST_H();
    delay2ms(60);
    LCD_RST_L();
    delay2ms(10);
    LCD_RST_H();
    delay2ms(100);
}

static void LCD_Reset()
{
    printf("reset \n");
    LCD_RST_OUT();
    LCD_RS_OUT();
    LCD_CS_OUT();

    LCD_RD_OUT();
    LCD_WR_OUT();

    LCD_RD_H();
    LCD_WR_H();
    LCD_CS_H();

    // LCD_RST_H();
    // LCD_RST_L();
    // delay_us(3);

#if 1//0//def USE_9225
    LCD_RST_H();
    delay2ms(50);
    LCD_RST_L();
    delay2ms(10);
    LCD_RST_H();
    delay2ms(50);
#else
    LCD_RST_H();
    delay2ms(60);
    LCD_RST_L();
    delay2ms(10);
    LCD_RST_H();
    delay2ms(50);
#endif
}

unsigned int LCD_data_table[] = {IO_PORTG_08, IO_PORTG_09, IO_PORTG_10, IO_PORTG_11, IO_PORTG_12, IO_PORTG_13, IO_PORTG_14, IO_PORTG_15};
//unsigned int LCD_data_table[] = {IO_PORTG_15,IO_PORTG_14,IO_PORTG_13,IO_PORTG_12,IO_PORTG_11,IO_PORTG_10,IO_PORTG_09,IO_PORTG_08};


u8 is_curr_bit_1(u8 value, u8 n)
{
    if ((((value >> n) & 1) == 0)) {
        return 0;
    } else {
        return 1;
    }
}

void mcu_write(u8 data)
{
    u8 temp = 0;
    u8 dat;
    u8 i = 0;

    for (i = 0; i < 8; ++i) {
        if (is_curr_bit_1(data, i)) {
            gpio_direction_output(LCD_data_table[i], 1);
        } else {
            gpio_direction_output(LCD_data_table[i], 0);
        }
    }
}

void Trans_Com(u16 com) //0xaa aa
{
    u8 temp = 0;

    LCD_CS_L();
    // temp = (com>>8);


    //mcu_write(temp);
    //LCD_WR_L();
    //LCD_WR_H();

    //delay2ms(30);
    temp = (com);
    LCD_RS_L();//写地址
    mcu_write(temp);
    LCD_WR_L();
    LCD_WR_H();
    LCD_CS_H();

}
void Trans_Dat(u16 dat)
{
    u8 temp = 0;

#if 1

    LCD_CS_L();

    LCD_RS_H();//写数据
    temp = (dat >> 8);
    mcu_write(temp);
    LCD_WR_L();
    LCD_WR_H();


    //LCD_RS_H();//写数据

    temp = (dat);
    mcu_write(temp);
    LCD_WR_L();
    LCD_WR_H();

    LCD_CS_H();
#endif

}


static u16 ReadData()
{
    u16 ret = 0;
    u8 i;


    LCD_CS_L();
    LCD_RS_H();
#ifndef SOFT_SIM
    dev_read(pap, &ret, sizeof(ret));
#else
    LCD_WR_H();
    //ret = LCDRead();
    for (i = 0; i < 8; ++i) {
        gpio_direction_input(LCD_data_table[i]);
    }

    ret = gpio_read(IO_PORTG_08);
    ret |= gpio_read(IO_PORTG_09) << 1;
    ret |= gpio_read(IO_PORTG_10) << 2;
    ret |= gpio_read(IO_PORTG_11) << 3;
    ret |= gpio_read(IO_PORTG_12) << 4;
    ret |= gpio_read(IO_PORTG_13) << 5;
    ret |= gpio_read(IO_PORTG_14) << 6;
    ret |= gpio_read(IO_PORTG_15) << 7;

#endif
    LCD_CS_H();

    printf("\n ReadData ret=%d ...\n", ret);

    return ret;
}


extern void clr_wdt();

//void  WriteRegister(unsigned char cmd,unsigned short dat)
void LCD_CtrlWrite_ILI9225G(unsigned short cmd, unsigned short dat)
{
    //Trans_Com(cmd);
    //Trans_Dat(dat);

    LCD_PORT_OUT();
    LCD_RS_L();
    LCD_CS_L();

    //LCD_PORT_W = cmd;

    mcu_write(cmd >> 8);
    LCD_WR_L();
    LCD_WR_H();

    mcu_write(cmd);
    LCD_WR_L();
    LCD_WR_H();
    LCD_RS_H();

    //LCD_PORT_W = dat>>8;
    mcu_write(dat >> 8);
    LCD_WR_L();
    LCD_WR_H();
    mcu_write(dat);

    //LCD_PORT_W = dat>>8;

    LCD_WR_L();
    LCD_WR_H();
    LCD_RS_H();
    LCD_CS_H();
}

void writelcm(unsigned int x, unsigned int cmd)
{
    LCD_PORT_OUT();

    //G=0;
    if (cmd) {
        LCD_RS_L();
    } else {
        LCD_RS_H();
    }

    LCD_CS_L();
    mcu_write(x >> 8);
    /* mcu_write(x>>8); */

    LCD_WR_L();
    LCD_WR_H();

    mcu_write(x);
    /* mcu_write(x); */
    LCD_WR_L();
    LCD_WR_H();
    LCD_RS_H();
    LCD_CS_H();
}

void FillColour(unsigned int Colour)
{
    unsigned char x, y;
    for (y = 220; y > 0; y--) {
        for (x = 176; x > 0; x--) {
            writelcm(Colour, 0);
        }

    }
}
static void lcd_9225s_setxy(u16 Xstart, u16 Xend, u16 Ystart, u16 Yend)
{
    //HX8352-C
    /* printf("%x:%x:%x:%x\n",Xstart,Xend,Ystart,Yend); */
//    LCD_CtrlWrite_ILI9225G(0x50, 0x0000);
//    LCD_CtrlWrite_ILI9225G(0x51, 0x00EF);
//    LCD_CtrlWrite_ILI9225G(0x52, 0x0000);
//    LCD_CtrlWrite_ILI9225G(0x53, 0x013F);

//    LCD_CtrlWrite_ILI9225G(0x50, Xstart);
//    LCD_CtrlWrite_ILI9225G(0x51, Xend);
//    LCD_CtrlWrite_ILI9225G(0x52, Ystart);
//    LCD_CtrlWrite_ILI9225G(0x53, Yend);
//

    LCD_CtrlWrite_ILI9225G(0x36, Xend);
    LCD_CtrlWrite_ILI9225G(0x37, Xstart);
    /* LCD_CtrlWrite_ILI9225G(0x38, 0x00DB); */
    LCD_CtrlWrite_ILI9225G(0x38, Yend);
    LCD_CtrlWrite_ILI9225G(0x39, Ystart);


    writelcm(0x0022, 1);
    /* WriteComm(0x2a); */
    /* WriteData(Xstart >> 8); */
    /* WriteData(Xstart & 0xff); */
    /* WriteData(Xend >> 8); */
    /* WriteData(Xend & 0xff); */

    /* WriteComm(0x2b); */
    /* WriteData(Ystart >> 8); */
    /* WriteData(Ystart & 0xff); */
    /* WriteData(Yend >> 8); */
    /* WriteData(Yend & 0xff); */

    /* WriteComm(0x2c); */

    LCD_CS_L();
    LCD_RS_H();
}
static void LCD_R69419_init(void *_data)
{
    u16 para = 0;
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;
    printf(" 9325 initial....................................\n");
    //gpio_direction_output(IO_PORTH_01, 1);
    LCD_Reset();

#ifdef USE_9225

//------------------------------------------------------------------------
#if 1 //9325  AU  2.4   //Îâ

#if 1
    //************* Start Initial Sequence **********//
    LCD_CtrlWrite_ILI9225G(0x00D0, 0x0003);
    LCD_CtrlWrite_ILI9225G(0x00EB, 0x0B00);
    LCD_CtrlWrite_ILI9225G(0x00EC, 0x004F);
    LCD_CtrlWrite_ILI9225G(0x00C7, 0x030F); //0305

    LCD_CtrlWrite_ILI9225G(0x0001, 0x011C);
    LCD_CtrlWrite_ILI9225G(0x0002, 0x0100);
    LCD_CtrlWrite_ILI9225G(0x0003, 0x1030);
    //LCD_CtrlWrite_ILI9225G(0x03, 0x1300);
    LCD_CtrlWrite_ILI9225G(0x0007, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0008, 0x0808);
    LCD_CtrlWrite_ILI9225G(0x000F, 0x0A01);
    LCD_CtrlWrite_ILI9225G(0x0010, 0x0A00);
    LCD_CtrlWrite_ILI9225G(0x0011, 0x1038); //The register setting is suitable for VCI=2.8V
    LCD_CtrlWrite_ILI9225G(0x0012, 0x1121);//The register setting is suitable for VCI=2.8V
    LCD_CtrlWrite_ILI9225G(0x0013, 0x0052); //The register setting is suitable for VCI=2.8V
    LCD_CtrlWrite_ILI9225G(0x0014, 0x5560); //The register setting is suitable for VCI=2.8V

    LCD_CtrlWrite_ILI9225G(0x0030, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0031, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0032, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0033, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0034, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0035, 0x0000);

    /* LCD_CtrlWrite_ILI9225G(0x36, 0x00AF); */
    /* LCD_CtrlWrite_ILI9225G(0x37, 0x0000); */
    /* LCD_CtrlWrite_ILI9225G(0x38, 0x00db); */
    /* LCD_CtrlWrite_ILI9225G(0x39, 0x0000); */

    LCD_CtrlWrite_ILI9225G(0x0050, 0x0400);
    LCD_CtrlWrite_ILI9225G(0x0051, 0x060B);
    LCD_CtrlWrite_ILI9225G(0x0052, 0x0C0A);
    LCD_CtrlWrite_ILI9225G(0x0053, 0x0105);
    LCD_CtrlWrite_ILI9225G(0x0054, 0x0A0C);
    LCD_CtrlWrite_ILI9225G(0x0055, 0x0B05);
    LCD_CtrlWrite_ILI9225G(0x0056, 0x0004);
    LCD_CtrlWrite_ILI9225G(0x0057, 0x0501);
    LCD_CtrlWrite_ILI9225G(0x0058, 0x0E00);
    LCD_CtrlWrite_ILI9225G(0x0059, 0x000E);;

    LCD_CtrlWrite_ILI9225G(0x0020, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0021, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0007, 0x1017);//bit12 te 0x1017

#else
    //************* Start Initial Sequence **********//
    LCD_CtrlWrite_ILI9225G(0x00D0, 0x0003);
    LCD_CtrlWrite_ILI9225G(0x00EB, 0x0B00);
    LCD_CtrlWrite_ILI9225G(0x00EC, 0x004F);
    LCD_CtrlWrite_ILI9225G(0x00C7, 0x030F);

    LCD_CtrlWrite_ILI9225G(0x0001, 0x011C);//0x011C  倒立;  0x001C 横屏
    LCD_CtrlWrite_ILI9225G(0x0002, 0x0100);// LCD driver control
    LCD_CtrlWrite_ILI9225G(0x0003, 0x1030);//1030// 16-bit mode  0x1030 竖屏; 0x0138 横屏
    LCD_CtrlWrite_ILI9225G(0x0008, 0x0808);
    LCD_CtrlWrite_ILI9225G(0x000F, 0x0901);
    delay2ms(5);
    LCD_CtrlWrite_ILI9225G(0x0010, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0011, 0x1B41); //The register setting is suitable for VCI=2.8V
    delay2ms(60);
    LCD_CtrlWrite_ILI9225G(0x0012, 0x200E);
    LCD_CtrlWrite_ILI9225G(0x0013, 0x0052);
    LCD_CtrlWrite_ILI9225G(0x0014, 0x5A66);

    LCD_CtrlWrite_ILI9225G(0x0030, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0031, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0032, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0033, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0034, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0035, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0036, 0x00AF);
    LCD_CtrlWrite_ILI9225G(0x0037, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0038, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0039, 0x0000);

    LCD_CtrlWrite_ILI9225G(0x0050, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0051, 0x010C);
    LCD_CtrlWrite_ILI9225G(0x0052, 0x0A01);
    LCD_CtrlWrite_ILI9225G(0x0053, 0x0401);
    LCD_CtrlWrite_ILI9225G(0x0054, 0x020A);
    LCD_CtrlWrite_ILI9225G(0x0055, 0x0B00);
    LCD_CtrlWrite_ILI9225G(0x0056, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0057, 0x0104);
    LCD_CtrlWrite_ILI9225G(0x0058, 0x0E05);
    LCD_CtrlWrite_ILI9225G(0x0059, 0x050E);

    LCD_CtrlWrite_ILI9225G(0x0020, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0021, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0010, 0x0000);
    delay2ms(60);
    LCD_CtrlWrite_ILI9225G(0x0007, 0x1017);
//    while(1){
//    gpio_direction_output(IO_PORTG_08, 1);
//    gpio_direction_output(IO_PORTG_09, 1);
//    gpio_direction_output(IO_PORTG_10, 1);
//    delay2ms(500);
//    gpio_direction_output(IO_PORTG_08, 0);
//    gpio_direction_output(IO_PORTG_09, 0);
//    gpio_direction_output(IO_PORTG_10, 0);
//    delay2ms(500);
//    }



#endif // 0


#else
    //************* Start Initial Sequence **********//
    LCD_CtrlWrite_ILI9225G(0x00D0, 0x0003);
    LCD_CtrlWrite_ILI9225G(0x00EB, 0x0B00);
    LCD_CtrlWrite_ILI9225G(0x00EC, 0x000F);
    LCD_CtrlWrite_ILI9225G(0x00C7, 0x030F);

    LCD_CtrlWrite_ILI9225G(0x0001, 0x011C);
    LCD_CtrlWrite_ILI9225G(0x0002, 0x0100);
    LCD_CtrlWrite_ILI9225G(0x0003, 0x1030);
    LCD_CtrlWrite_ILI9225G(0x0008, 0x0808);
    LCD_CtrlWrite_ILI9225G(0x000F, 0x0901);
    delay_2ms(5);
    LCD_CtrlWrite_ILI9225G(0x0010, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0011, 0x1B41); //The register setting is suitable for VCI=2.8V
    delay_2ms(60);
    LCD_CtrlWrite_ILI9225G(0x0012, 0x200E);//The register setting is suitable for VCI=2.8V
    LCD_CtrlWrite_ILI9225G(0x0013, 0x0052); //The register setting is suitable for VCI=2.8V
    LCD_CtrlWrite_ILI9225G(0x0014, 0x4B5C); //The register setting is suitable for VCI=2.8V
    LCD_CtrlWrite_ILI9225G(0x0030, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0031, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0032, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0033, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0034, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0035, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0036, 0x00AF);
    LCD_CtrlWrite_ILI9225G(0x0037, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0038, 0x00DB);
    LCD_CtrlWrite_ILI9225G(0x0039, 0x0000);

    LCD_CtrlWrite_ILI9225G(0x0050, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0051, 0x0705);
    LCD_CtrlWrite_ILI9225G(0x0052, 0x0C0A);
    LCD_CtrlWrite_ILI9225G(0x0053, 0x0401);
    LCD_CtrlWrite_ILI9225G(0x0054, 0x040C);
    LCD_CtrlWrite_ILI9225G(0x0055, 0x0608);
    LCD_CtrlWrite_ILI9225G(0x0056, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0057, 0x0104);
    LCD_CtrlWrite_ILI9225G(0x0058, 0x0E06);
    LCD_CtrlWrite_ILI9225G(0x0059, 0x060E);;

    LCD_CtrlWrite_ILI9225G(0x0020, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0021, 0x0000);
    LCD_CtrlWrite_ILI9225G(0x0007, 0x1017);////bit12 te 0x1017

#endif//0
    writelcm(0x0022, 1);	//��Write to GRAMʱ��ͼ��д0x0022��index�Ĵ���
    delay_2ms(100);
#endif // USE_9225

    LCD_RS_H();//Ð´Êý¾Ý
    LCD_WR_H();
    LCD_CS_L();
    LCD_RD_H();

}


static void LCD_R69419_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    //puts("\n\enter 9325G lcd on............*********************............n");
    if (on) {
        // puts("\n\enter lcd on...............n");
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        // puts("\n\enter lcd off...............n");
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_IMD_DEVICE_BEGIN(LCD_R69419_dev) = {
    .info = {
        .test_mode       = 0,//true,//false,//
        .test_mode_color = 0xFF0000,
        .canvas_color    = 0x000000,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres     = 224,
        .target_yres     = 176,
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .sample          = LCD_DEV_SAMPLE,
        .len 			 = LEN_256,
        .format          = FORMAT_RGB565,
        .interlaced_mode = INTERLACED_NONE,

        .rotate_en 		 = true,		// 旋转使能
        .hori_mirror_en  = false,		// 水平镜像使能
        .vert_mirror_en  = true,//false,		// 垂直镜像使能

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
//        .sw_mode = true,
//        .sw_x = 0,
//        .sw_y = 0,
//        .sw_xres = 220,
//        .sw_yres = 176,
    },

#if 0
    .te_mode = {
        .te_mode_en = true,
//		.te_mode_en = false,
        .event = EVENT_PH7,//EVENT_IO_2,
        .edge = EDGE_POSITIVE,
//		.edge = EDGE_NEGATIVE,
        .gpio = IO_PORTH_06,
    },
#endif

    .drive_mode      = MODE_MCU,
    .ncycle          = CYCLE_TWO,
    .data_width      = PORT_8BITS,

    .dclk_set        = CLK_DIS | CLK_NEGATIVE,
    .sync0_set       = CLK_EN  | SIGNAL_DEN,  //| CLK_POSITIVE ,
    .sync1_set       = CLK_DIS | SIGNAL_HSYNC | CLK_NEGATIVE,
    .sync2_set       = CLK_DIS | SIGNAL_VSYNC | CLK_NEGATIVE,

    .port_sel        = PORT_GROUP_AA,
#if 1
//    .clk_cfg	 	 = PLL2_CLK | DIVA_3 | DIVB_2 | DIVC_2,
//    .clk_cfg	 	 = PLL3_CLK | DIVA_5 | DIVB_4,
    .clk_cfg	 	 = PLL2_CLK_480M | DIVA_7 | DIVB_4,
#else
    .clk_cfg	 	 = IMD_INTERNAL | OSC_32K | PLL1_CLK | DIVA_3 | DIVB_1 | DIVC_1,
    .pll1_nr         = 3662,/* 3662*0.032768=120M */
    .pll1_nf         = 1,
#endif
    .timing = {
        .hori_total 		    = 400,//400,
        .hori_sync 		        = 20,//8,
        .hori_back_porth 	    = 20,//8,
        .hori_pixel 	        = 176,

        .vert_total 		    = 256,
        .vert_sync 		        = 10,//8,
        .vert_back_porth_odd 	= 8,//8
        .vert_back_porth_even 	= 0,
        .vert_pixel 	        = 220,

    },

},
REGISTER_IMD_DEVICE_END()

/* void (*setxy)(int x0, int x1, int y0, int y1); */
REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .logo = "9225_mcu",
    .init = LCD_R69419_init,
    .type = LCD_DVP_MCU,
    .dev  = &LCD_R69419_dev,
    .bl_ctrl = LCD_R69419_backctrl,
    .te_mode_dbug = 0,//true,false
    .setxy   = lcd_9225s_setxy,
};

#endif
