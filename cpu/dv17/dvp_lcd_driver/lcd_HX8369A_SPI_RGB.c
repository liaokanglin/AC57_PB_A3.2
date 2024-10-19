#include "generic/typedef.h"
#include "asm/imb.h"
#include "asm/imd.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"

#ifdef LCD_HX8369A_SPI_RGB

extern void delay_2ms(int cnt);
#define delay2ms(t) delay_2ms(t)

#define USE_AA_GROUP

//R          G         B
#define DPI_24BITS  0//DB[23..16] DB[15..8] DB[7..0]
#define DPI_18BITS  1//DB[21..16] DB[13..8] DB[5..0]
#define DPI_16BITS  2//DB[21..17] DB[13..8] DB[5..1]  ok
#define DPI_FORMAT  DPI_24BITS

#define SPI_3WIRE   0//9bit  BS[3..0]:1101
#define SPI_4WIRE   1//8bit  BS[3..0]:1111
#define SPI_WIRE    SPI_3WIRE

#ifdef USE_AA_GROUP
// RESET(PC1)
#define SPI_RST_OUT()    PORTH_DIR &=~ BIT(11)
#define SPI_RST_H()      PORTH_OUT |=  BIT(11)
#define SPI_RST_L()      PORTH_OUT &=~ BIT(11)

#if (SPI_WIRE == SPI_4WIRE)
// DCX
#define SPI_DCX_OUT()    PORTH_DIR &=~ BIT(13)
#define SPI_DCX_H()      PORTH_OUT |=  BIT(13)
#define SPI_DCX_L()      PORTH_OUT &=~ BIT(13)
#elif(SPI_WIRE == SPI_3WIRE)
// DCX
#define SPI_DCX_OUT()    //PORTH_DIR &=~ BIT(13)
#define SPI_DCX_H()      //PORTH_OUT |=  BIT(13)
#define SPI_DCX_L()      //PORTH_OUT &=~ BIT(13)
#endif

// CS(PD1)
#define SPI_CSX_OUT()    PORTH_DIR &=~ BIT(10)
#define SPI_CSX_H()      PORTH_OUT |=  BIT(10)
#define SPI_CSX_L()      PORTH_OUT &=~ BIT(10)

// SCL(PE5)
#define SPI_SCL_OUT()    do {PORTH_DIR &=~ BIT(9);}while(0)
#define SPI_SCL_H()      do {PORTH_OUT |=  BIT(9);}while(0)
#define SPI_SCL_L()      do {PORTH_OUT &=~ BIT(9);}while(0)

// MOSI(PF5)
#define SPI_MOSI_OUT()   PORTH_DIR &=~ BIT(8)
#define SPI_MOSI_H()     PORTH_OUT |=  BIT(8)
#define SPI_MOSI_L()     PORTH_OUT &=~ BIT(8)

// MISO(PE1)
#define SPI_MISO_IN()    PORTH_DIR |= BIT(7);PORTH_PU |= BIT(7);PORTH_PD &=~BIT(7)
#define SPI_MISO_R()     PORTH_IN & BIT(7)
#else
#if 0
// RESET(PC1)
#define SPI_RST_OUT()    PORTD_DIR &=~ BIT(11)
#define SPI_RST_H()      PORTD_OUT |=  BIT(11)
#define SPI_RST_L()      PORTD_OUT &=~ BIT(11)

#if (SPI_WIRE == SPI_4WIRE)
// DCX
#define SPI_DCX_OUT()    PORTD_DIR &=~ BIT(13)
#define SPI_DCX_H()      PORTD_OUT |=  BIT(13)
#define SPI_DCX_L()      PORTD_OUT &=~ BIT(13)
#elif(SPI_WIRE == SPI_3WIRE)
// DCX
#define SPI_DCX_OUT()    //PORTD_DIR &=~ BIT(13)
#define SPI_DCX_H()      //PORTD_OUT |=  BIT(13)
#define SPI_DCX_L()      //PORTD_OUT &=~ BIT(13)
#endif

// CS(PD1)
#define SPI_CSX_OUT()    PORTD_DIR &=~ BIT(10)
#define SPI_CSX_H()      PORTD_OUT |=  BIT(10)
#define SPI_CSX_L()      PORTD_OUT &=~ BIT(10)

// SCL(PE5)
#define SPI_SCL_OUT()    do {PORTD_DIR &=~ BIT(9);}while(0)
#define SPI_SCL_H()      do {PORTD_OUT |=  BIT(9);}while(0)
#define SPI_SCL_L()      do {PORTD_OUT &=~ BIT(9);}while(0)

// MOSI(PF5)
#define SPI_MOSI_OUT()   PORTD_DIR &=~ BIT(8)
#define SPI_MOSI_H()     PORTD_OUT |=  BIT(8)
#define SPI_MOSI_L()     PORTD_OUT &=~ BIT(8)

// MISO(PE1)
#define SPI_MISO_IN()    PORTD_DIR |= BIT(7);PORTD_PU |= BIT(7);PORTD_PD |= BIT(7)
#define SPI_MISO_R()     PORTD_IN & BIT(7)
#endif
#endif

#define DELAY_CNT       300

extern void delay(volatile u32  t);

typedef struct {
    u8 cmd;
    u8 cnt;
    u8 dat[128];
} InitCode;

#define REGFLAG_DELAY 0xFF

static const InitCode code[] = {
    /* {0x01, 0},//soft reset */
    /* {REGFLAG_DELAY, 50}, */
    {0xB9, 3,  {0xFF, 0x83, 0x69}}, // SET password
    {0xB1, 19, {0x01, 0x00, 0x34, 0x06, 0x00, 0x11, 0x11, 0x2A, 0x32, 0x3F, 0x3F, 0x07, 0x23, 0x01, 0xE6, 0xE6, 0xE6, 0xE6, 0xE6}}, //Set Power
    {0xB2, 15, {0x00, 0x2b, 0x05, 0x05, 0x70, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x01,}}, // SET Display  480x800
    {0xB3, 1,  {0x09}},
    {0xB4, 5,  {0x00, 0x0f, 0x82, 0x0c, 0x03}},
    {0xB6, 2,  {0x42, 0x42}}, // SET VCOM
    {0xD5, 26, {0x00, 0x04, 0x03, 0x00, 0x01, 0x04, 0x1A, 0xff, 0x01, 0x13, 0x00, 0x00, 0x40, 0x06, 0x51, 0x07, 0x00, 0x00, 0x41, 0x06, 0x50, 0x07, 0x07, 0x0F, 0x04, 0x00,}},
    {0xE0, 34, {0x00, 0x13, 0x19, 0x38, 0x3D, 0x3F, 0x28, 0x46, 0x07, 0x0D, 0x0E, 0x12, 0x15, 0x12, 0x14, 0x0F, 0x17, 0x00, 0x13, 0x19, 0x38, 0x3D, 0x3F, 0x28, 0x46, 0x07, 0x0D, 0x0E, 0x12, 0x15, 0x12, 0x14, 0x0F, 0x17,}},
    {
        0xC1, 127, {
            0x01,
            0x04, 0x13, 0x1a, 0x20, 0x27, 0x2c, 0x32, 0x36, 0x3f, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71, 0x7B, 0x82, 0x89, 0x91, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC1, 0xC9, 0xD0, 0xD7, 0xE0, 0xE7, 0xEF, 0xF7, 0xFE, 0xCF, 0x52, 0x34, 0xF8, 0x51, 0xF5, 0x9D, 0x75, 0x00,
            0x04, 0x13, 0x1a, 0x20, 0x27, 0x2c, 0x32, 0x36, 0x3f, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71, 0x7B, 0x82, 0x89, 0x91, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC1, 0xC9, 0xD0, 0xD7, 0xE0, 0xE7, 0xEF, 0xF7, 0xFE, 0xCF, 0x52, 0x34, 0xF8, 0x51, 0xF5, 0x9D, 0x75, 0x00,
            0x04, 0x13, 0x1a, 0x20, 0x27, 0x2c, 0x32, 0x36, 0x3f, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71, 0x7B, 0x82, 0x89, 0x91, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC1, 0xC9, 0xD0, 0xD7, 0xE0, 0xE7, 0xEF, 0xF7, 0xFE, 0xCF, 0x52, 0x34, 0xF8, 0x51, 0xF5, 0x9D, 0x75, 0x00,
        }
    },
    //{0x36, 1,  {0x10}},
#if (DPI_FORMAT==DPI_16BITS)
    //R:DB[21..17] G:DB[13..8] B:DB[5..1]
    {0x3a, 1,  {0x50}},
#elif (DPI_FORMAT==DPI_18BITS)
    //R:DB[21..16] G:DB[13..8] B:DB[5..0]
    {0x3a, 1,  {0x60}},
#elif (DPI_FORMAT==DPI_24BITS)
    //R:DB[23..16] G:DB[15..8] B:DB[7..0]
    {0x3a, 1,  {0x70}},
#endif
    {0x11, 0},//Sleep out
    {REGFLAG_DELAY, 150},
    {0x29, 0},
    {REGFLAG_DELAY, 100},
};

static InitCode code1[] = {
    //{0x01, 0},//soft reset
    //{REGFLAG_DELAY, 50},
    {0xB9, 3,  {0xFF, 0x83, 0x69}}, // SET password
    {0xB1, 19, {0x85, 0x00, 0x34, 0x07, 0x00, 0x11, 0x11, 0x2A, 0x32, 0x19, 0x19, 0x07, 0x23, 0x01, 0xE6, 0xE6, 0xE6, 0xE6, 0xE6}}, //Set Power
    {0xB2, 15, {0x00, 0x2b, 0x05, 0x05, 0x70, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x01,}}, // SET Display  480x800
    {0xB4, 5,  {0x00, 0x0f, 0x82, 0x0c, 0x03}},
    {0xB6, 2,  {0x10, 0x10}}, // SET VCOM
    {0xD5, 26, {0x00, 0x04, 0x03, 0x00, 0x01, 0x04, 0x1A, 0xff, 0x01, 0x13, 0x00, 0x00, 0x40, 0x06, 0x51, 0x07, 0x00, 0x00, 0x41, 0x06, 0x50, 0x07, 0x07, 0x0F, 0x04, 0x00,}},
    {0xE0, 34, {0x00, 0x13, 0x19, 0x38, 0x3D, 0x3F, 0x28, 0x46, 0x07, 0x0D, 0x0E, 0x12, 0x15, 0x12, 0x14, 0x0F, 0x17, 0x00, 0x13, 0x19, 0x38, 0x3D, 0x3F, 0x28, 0x46, 0x07, 0x0D, 0x0E, 0x12, 0x15, 0x12, 0x14, 0x0F, 0x17,}},
    {
        0xC1, 127, {
            0x01,
            0x04, 0x13, 0x1a, 0x20, 0x27, 0x2c, 0x32, 0x36, 0x3f, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71, 0x7B, 0x82, 0x89, 0x91, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC1, 0xC9, 0xD0, 0xD7, 0xE0, 0xE7, 0xEF, 0xF7, 0xFE, 0xCF, 0x52, 0x34, 0xF8, 0x51, 0xF5, 0x9D, 0x75, 0x00,
            0x04, 0x13, 0x1a, 0x20, 0x27, 0x2c, 0x32, 0x36, 0x3f, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71, 0x7B, 0x82, 0x89, 0x91, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC1, 0xC9, 0xD0, 0xD7, 0xE0, 0xE7, 0xEF, 0xF7, 0xFE, 0xCF, 0x52, 0x34, 0xF8, 0x51, 0xF5, 0x9D, 0x75, 0x00,
            0x04, 0x13, 0x1a, 0x20, 0x27, 0x2c, 0x32, 0x36, 0x3f, 0x47, 0x50, 0x59, 0x60, 0x68, 0x71, 0x7B, 0x82, 0x89, 0x91, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC1, 0xC9, 0xD0, 0xD7, 0xE0, 0xE7, 0xEF, 0xF7, 0xFE, 0xCF, 0x52, 0x34, 0xF8, 0x51, 0xF5, 0x9D, 0x75, 0x00,
        }
    },
    //{0x36, 1,  {0x10}},
#if (DPI_FORMAT==DPI_16BITS)
    //R:DB[21..17] G:DB[13..8] B:DB[5..1]
    {0x3a, 1,  {0x50}},
#elif (DPI_FORMAT==DPI_18BITS)
    //R:DB[21..16] G:DB[13..8] B:DB[5..0]
    {0x3a, 1,  {0x60}},
#elif (DPI_FORMAT==DPI_24BITS)
    //R:DB[23..16] G:DB[15..8] B:DB[7..0]
    {0x3a, 1,  {0x70}},
#endif
    {0x11, 0},//Sleep out
    {REGFLAG_DELAY, 150},
    {0x29, 0},
    {REGFLAG_DELAY, 100},
};

static void spi_io_init()
{
    printf("reset \r\n");

    SPI_RST_OUT();
    SPI_DCX_OUT();
    SPI_CSX_OUT();
    SPI_SCL_OUT();
    SPI_MOSI_OUT();
    SPI_MISO_IN();

    SPI_CSX_H();
    SPI_DCX_H();
    SPI_RST_H();
    SPI_MOSI_L();
    SPI_SCL_L();


    SPI_RST_H();
    delay_2ms(50);
    SPI_RST_L();
    delay_2ms(50);
    SPI_RST_H();
}

static void spi_tx_byte(u16 dat)
{
    u8 i;
#if (SPI_WIRE == SPI_3WIRE)
    for (i = 0; i < 9; i++) {
        if (dat & BIT(8)) {
            SPI_MOSI_H();
        } else {
            SPI_MOSI_L();
        }
        dat <<= 1;

        SPI_SCL_L();
        delay(DELAY_CNT);
        SPI_SCL_H();
        delay(DELAY_CNT);
    }
#elif (SPI_WIRE == SPI_4WIRE)

    for (i = 0; i < 8; i++) {
        if (dat & BIT(7)) {
            SPI_MOSI_H();
        } else {
            SPI_MOSI_L();
        }
        dat <<= 1;

        SPI_SCL_L();
        delay(DELAY_CNT);
        SPI_SCL_H();
        delay(DELAY_CNT);
    }

#endif
    SPI_SCL_L();
}

static u8 spi_rx_byte()
{
    u8 dat = 0;
    u8 i;

    SPI_MISO_IN() ;
    for (i = 0; i < 8; i++) {

        SPI_SCL_L();
        delay(DELAY_CNT);

        SPI_SCL_H();
        delay(DELAY_CNT);

        dat <<= 1;
        if (SPI_MISO_R()) {
            dat |= BIT(0);
        }
    }
    SPI_SCL_L();

    return dat;
}


static void WriteComm(u8 cmd)
{
    SPI_CSX_L();
    SPI_DCX_L();

    spi_tx_byte(cmd);

    SPI_CSX_H();
}

static void WriteData(u8 dat)
{
    SPI_CSX_L();
    SPI_DCX_H();
#if (SPI_WIRE == SPI_3WIRE)
    spi_tx_byte(BIT(8) | dat);
#elif (SPI_WIRE == SPI_4WIRE)
    spi_tx_byte(dat);
#endif
    SPI_CSX_H();
}

static u8 ReadData()
{
    u8 ret;
    SPI_CSX_L();
    SPI_DCX_H();
    ret = spi_rx_byte();
    SPI_CSX_H();

    return ret;
}

static void ReadReg(u8 cmd, u8 *buf, u8 len)
{
    u8 i;
    WriteComm(cmd);
    for (i = 0; i < len; i++) {
        buf[i] = ReadData();
    }
}

static void hx8369a_init_code(InitCode *code, u8 cnt)
{
    u8 i, j;

    for (i = 0; i < cnt; i++) {
        if (code[i].cmd == REGFLAG_DELAY) {
            delay_2ms(code[i].cnt);
        } else {
            WriteComm(code[i].cmd);
            /* printf("cmd:%x ", code[i].cmd); */
            //printf("cnt:%x ",code[i].cnt);
            //printf("dat:%x ",code[i].dat);
            for (j = 0; j < code[i].cnt; j++) {
                WriteData(code[i].dat[j]);
                /* printf("%02x ", code[i].dat[j]); */
            }
            /* printf("\n"); */
        }
    }
}



static void setxy(u16 Xstart, u16 Xend, u16 Ystart, u16 Yend)
{
    //HX8352-C
    printf("setxy %d %d %d %d\n", Xstart, Xend, Ystart, Yend);

    WriteComm(0x2a);
    WriteData(Xstart >> 8);
    WriteData(Xstart & 0xff);
    WriteData(Xend >> 8);
    WriteData(Xend & 0xff);

    WriteComm(0x2b);
    WriteData(Ystart >> 8);
    WriteData(Ystart & 0xff);
    WriteData(Yend >> 8);
    WriteData(Yend & 0xff);

    WriteComm(0x2c);

    SPI_CSX_L();
    SPI_DCX_H();
}

static void lcd_hx8369a_init(void *_data)
{
    u8 i;
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;

    printf("lcd hx8369 init...\r\n");

    spi_io_init();

    WriteComm(0x01);
    delay_2ms(100);

    WriteComm(0x11);
    delay_2ms(120);


    /* WriteComm(0x2D); */
    /* for (i = 0; i <= 63; i++) { */
    /* WriteData(i * 4); */
    /* } */

    /* for (i = 0; i <= 63; i++) { */
    /* WriteData(i * 4); */
    /* } */

    /* for (i = 0; i <= 63; i++) { */
    /* WriteData(i * 4); */
    /* } */

    hx8369a_init_code(code, sizeof(code) / sizeof(code[0]));
    /* hx8369a_init_code(code1, sizeof(code1) / sizeof(code1[0])); */

    /* u8 buf[3]; */
    /* ReadReg(0xF4,buf,3); */
    /* printf("1st:0x%x\n",buf[0]); */
    /* printf("2nd:0x%x\n",buf[1]); */
    /* printf("3rd:0x%x\n",buf[2]); */


    setxy(0, 479, 0, 799);
}

static void lcd_hx8369a_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if (on) {
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
}

REGISTER_IMD_DEVICE_BEGIN(lcd_hx8369a_spi_rgb_dev) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0x0000FF,
        .canvas_color    = 0x000000,
        .x               = 0,
        .y               = 0,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres     = 800,
        .target_yres     = 480,
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .sample          = LCD_DEV_SAMPLE,
        .len 			 = LEN_512,
#if (DPI_FORMAT==DPI_16BITS)
        .format          = FORMAT_RGB565,
#elif (DPI_FORMAT==DPI_18BITS)
        .format          = FORMAT_RGB666,
#elif (DPI_FORMAT==DPI_24BITS)
        .format          = /* FORMAT_RGB888 */FORMAT_RGB666,
#endif
        .interlaced_mode = INTERLACED_NONE,

        .rotate_en       = true,	    // 旋转使能
        .hori_mirror_en  = true,	    // 水平镜像使能
        .vert_mirror_en  = false,		// 垂直镜像使能

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
    },

    .drive_mode      = MODE_RGB_DE_SYNC,
    .ncycle          = CYCLE_ONE,
#if (DPI_FORMAT==DPI_16BITS)
    .data_width      = PORT_16BITS,
#elif (DPI_FORMAT==DPI_18BITS)
    .data_width      = PORT_18BITS,
#elif (DPI_FORMAT==DPI_24BITS)
    .data_width      = /* PORT_24BITS */PORT_18BITS,
#endif

    .dclk_set        = CLK_EN /* | CLK_NEGATIVE */,
    .sync0_set       = SIGNAL_DEN  | CLK_EN/*|CLK_NEGATIVE*/,
    .sync1_set       = SIGNAL_HSYNC | CLK_EN | CLK_NEGATIVE,
    .sync2_set       = SIGNAL_VSYNC | CLK_EN | CLK_NEGATIVE,

#ifdef USE_AA_GROUP
    .port_sel        = PORT_GROUP_AA,
#else
    .port_sel        = PORT_GROUP_B,
#endif
    /* .clk_cfg    	 = PLL2_CLK | DIVA_3 | DIVB_2 | DIVC_2, */
    .clk_cfg    	 = PLL2_CLK_480M | DIVA_3 | DIVB_4, /* FPGA */

    .timing = {
        /* .hori_total 		        = 525,//12000000/60/288,// */
        /* .hori_sync 		        = 5, */
        /* .hori_back_porth 	    = 40, */
        /* .hori_pixel 	            = 480, */
        /* .vert_total 		        = 810, */
        /* .vert_sync 		        = 5, */
        /* .vert_back_porth_odd 	= 5, */
        /* .vert_back_porth_even 	= 0, */
        /* .vert_pixel 	            = 800, */

        .hori_total 	        	= 524,
        .hori_sync 		            = 2,
        .hori_back_porth 	        = 14,
        .hori_pixel 	            = 480,

        .vert_total 		        = 820,
        .vert_sync 		            = 2,
        .vert_back_porth_odd 	    = 10,
        .vert_back_porth_even 	    = 0,
        .vert_pixel 	            = 800,
    },
},
REGISTER_IMD_DEVICE_END()

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type 	 = LCD_DVP_RGB,
    .init 	 = lcd_hx8369a_init,
    .dev     = &lcd_hx8369a_spi_rgb_dev,
    .bl_ctrl = lcd_hx8369a_backctrl,
    .init_order = INIT_LAST,
};

#endif
