#include "generic/typedef.h"
#include "asm/imb.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/pap.h"
#include "asm/lcd_config.h"
#include "os/os_api.h"
#include "device/lcd_driver.h"
#include "gpio.h"

extern void delay_2ms(int cnt);
#define delay2ms(t) delay_2ms(t)

#ifdef LCD_HX8369A_MCU

#define DBI_8BITS   0//BS[3..0]:0100  PC[15..8] <-> DB[7..0]                                                                            OK
#define DBI_9BITS   1//BS[3..0]:0101  PC[15..7] <-> DB[8..0]                                                                            OK
#define DBI_16BITS  2//BS[3..0]:0110  LE{PC[15..0]}/BE{PC[7..0],PC[15..8]} <-> DB[15..0]                                                OK
#define DBI_18BITS  3//BS[3..0]:0111  PD[1..0],PC[15..0] <-> DB[17..0]                                                                  OK
#define DBI_24BITS  4//BS[3..0]:1010  PD[1..0],PC[15..12],PD[11..10] PC[11..6],PD[9..8] PC[5..0],PD[7..6]   DB[23..0]                   OK
#define DBI_FORMAT  DBI_16BITS

#define USE_AA_GROUP
/* #define USE_AB_GROUP */
/* #define DIRECTION_HORI */
/* #define SOFT_SIM  //use pap module */

#ifdef USE_AA_GROUP
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//PF6 RESET
#define HX8369A_RST_OUT()   PORTH_DIR &=~ BIT(0)
#define HX8369A_RST_H()     HX8369A_RST_OUT();PORTH_OUT |=  BIT(0)
#define HX8369A_RST_L()     HX8369A_RST_OUT();PORTH_OUT &=~ BIT(0)

//PE6 CS
#define HX8369A_CS_OUT()    PORTH_DIR &=~ BIT(1)
#define HX8369A_CS_DIS()    HX8369A_CS_OUT();PORTH_OUT |=  BIT(1)
#define HX8369A_CS_EN()     HX8369A_CS_OUT();PORTH_OUT &=~ BIT(1)

//PH5  RS
#define HX8369A_RS_OUT()    PORTH_DIR &=~ BIT(2)
#define HX8369A_RS_H()      HX8369A_RS_OUT();PORTH_OUT |=  BIT(2)
#define HX8369A_RS_L()      HX8369A_RS_OUT();PORTH_OUT &=~ BIT(2)
/*=============================================================*/
#elif (defined USE_AB_GROUP)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//PG4 RESET
#define HX8369A_RST_OUT()   PORTG_DIR &=~ BIT(5)
#define HX8369A_RST_H()     HX8369A_RST_OUT();PORTG_OUT |=  BIT(5)
#define HX8369A_RST_L()     HX8369A_RST_OUT();PORTG_OUT &=~ BIT(5)

//PQ2 CS
#define HX8369A_CS_OUT()    PORTB_DIR &=~ BIT(2)
#define HX8369A_CS_DIS()    HX8369A_CS_OUT();PORTB_OUT |=  BIT(2)
#define HX8369A_CS_EN()     HX8369A_CS_OUT();PORTB_OUT &=~ BIT(2)

//PG7  RS
#define HX8369A_RS_OUT()    PORTG_DIR &=~ BIT(3)
#define HX8369A_RS_H()      HX8369A_RS_OUT();PORTG_OUT |=  BIT(3)
#define HX8369A_RS_L()      HX8369A_RS_OUT();PORTG_OUT &=~ BIT(3)
/*=============================================================*/
#else
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//PF6 RESET
#define HX8369A_RST_OUT()   PORTD_DIR &=~ BIT(0)
#define HX8369A_RST_H()     HX8369A_RST_OUT();PORTD_OUT |=  BIT(0)
#define HX8369A_RST_L()     HX8369A_RST_OUT();PORTD_OUT &=~ BIT(0)

//PE6 CS
#define HX8369A_CS_OUT()    PORTD_DIR &=~ BIT(1)
#define HX8369A_CS_DIS()    HX8369A_CS_OUT();PORTD_OUT |=  BIT(1)
#define HX8369A_CS_EN()     HX8369A_CS_OUT();PORTD_OUT &=~ BIT(1)

//PH5  RS
#define HX8369A_RS_OUT()    PORTD_DIR &=~ BIT(2)
#define HX8369A_RS_H()      HX8369A_RS_OUT();PORTD_OUT |=  BIT(2)
#define HX8369A_RS_L()      HX8369A_RS_OUT();PORTD_OUT &=~ BIT(2)
/*=============================================================*/
#endif

#ifdef SOFT_SIM
#define HX8369A_WR_OUT()    PORTH_DIR &=~ BIT(3)
#define HX8369A_WR_H()      HX8369A_WR_OUT();PORTH_OUT |= BIT(3)
#define HX8369A_WR_L()      HX8369A_WR_OUT();PORTH_OUT &=~BIT(3)

#define HX8369A_RD_OUT()    PORTH_DIR &=~ BIT(4)
#define HX8369A_RD_H()      HX8369A_RD_OUT();PORTH_OUT |= BIT(4)
#define HX8369A_RD_L()      HX8369A_RD_OUT();PORTH_OUT &=~BIT(4)

#ifdef DBI_16BITS
#define HX8369A_PORT_OUT()  PORTG_DIR &=~ 0xffff;PORTG_HD |= 0xffff
#define HX8369A_PORT_IN()   PORTG_DIR |=  0xffff;PORTG_PU |= 0xffff
#else
#define HX8369A_PORT_OUT()  PORTG_DIR &=~ 0xff00;PORTG_PU |= 0xff00
#define HX8369A_PORT_IN()   PORTG_DIR |=  0xff00;PORTG_PU |= 0xff00
#endif

#define HX8369A_PORT_W      PORTG_OUT
#define HX8369A_PORT_R      PORTG_IN
#endif//end of SOFT_SIM

/* #include "rgb565_800x480.h" */
/* #include "rgb565_800x480_2.h" */
/* #include "rgb565_800x480_3.h" */
/* #include "rgb565_800x480_4.h" */
/* #include "rgb565_800x480_5.h" */

static void *pap = NULL;

extern void delay(volatile u32  t);

#define REGFLAG_DELAY 0xFF

typedef struct {
    u8 cmd;
    u8 cnt;
    u8 dat[128];
} InitCode;

static const InitCode code[] = {
    {0x01, 0},//soft reset
    {REGFLAG_DELAY, 50},
    {0xB9, 3,  {0xFF, 0x83, 0x69}}, // SET password
    {0xB1, 19, {0x01, 0x00, 0x34, 0x06, 0x00, 0x11, 0x11, 0x2A, 0x32, 0x3F, 0x3F, 0x07, 0x23, 0x01, 0xE6, 0xE6, 0xE6, 0xE6, 0xE6}}, //Set Power
    {0xB2, 15, {0x00, 0x20, 0x05, 0x03, 0x70, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x01,}}, // SET Display  480x800
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
    {0x35, 1,  {0x00}},
#ifdef DIRECTION_HORI
    {0x36, 1,  {0xa0}},
#else
    {0x36, 1,  {0x00}},
#endif
#if ((DBI_FORMAT==DBI_8BITS)||(DBI_FORMAT==DBI_16BITS))
    {0x3a, 1,  {0x05}},//pixel format setting
#elif ((DBI_FORMAT==DBI_9BITS)||(DBI_FORMAT==DBI_18BITS))
    {0x3a, 1,  {0x06}},//pixel format setting
#else
    {0x3a, 1,  {0x07}},//pixel format setting
#endif
    {0x11, 0},//Sleep out
    {REGFLAG_DELAY, 150},
    {0x29, 0},
    {REGFLAG_DELAY, 100},
};

static void LCD_Reset()
{
    printf("lcd HX8369A reset \n");

    /* PORTG_DIR = 0x0000; */
    /* PORTG_OUT = 0x0000; */
    /* PORTH_DIR &=~(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)); */
    /* PORTH_OUT &=~(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)); */
    /* while(1) { */
    /* PORTG_OUT ^= 0xffff; */
    /* printf("PORTH_OUT : 0x%x\n",PORTH_OUT); */
    /* } */

    HX8369A_RST_OUT();
    HX8369A_RS_OUT();
    HX8369A_CS_OUT();
#ifdef SOFT_SIM
    HX8369A_RD_OUT();
    HX8369A_WR_OUT();

    HX8369A_RD_H();
    HX8369A_WR_H();
#else //end of SOFT_SIM
#if (DBI_FORMAT == DBI_8BITS)
    struct pap_info pap_info_t;
    pap_info_t.datawidth 	= PAP_PORT_8BITS;
    pap_info_t.endian    	= PAP_BE;//8bit必须为大端
    pap_info_t.cycle     	= PAP_CYCLE_ONE;
#if (defined USE_AA_GROUP)
    pap_info_t.group_sel 	= PAP_GROUP_PG_PH;
    pap_info_t.wr_rd_sel    = PAP_PH3_PH4;
#elif (defined USE_AB_GROUP)
    pap_info_t.group_sel 	= PAP_GROUP_PG_PH;
    pap_info_t.wr_rd_sel    = PAP_PG2_PG4;
#else
    pap_info_t.group_sel 	= PAP_GROUP_PC_PD;
#endif
    pap_info_t.timing_setup = 2;
    pap_info_t.timing_hold  = 2;
    pap_info_t.timing_width = 5;
    pap = dev_open("pap", &pap_info_t);
#elif (DBI_FORMAT == DBI_16BITS)||(DBI_FORMAT == DBI_18BITS)
    struct pap_info pap_info_t;
    pap_info_t.datawidth 	= PAP_PORT_16BITS;
    pap_info_t.endian    	= PAP_LE;
    pap_info_t.cycle     	= PAP_CYCLE_ONE;
#ifdef USE_AA_GROUP
    pap_info_t.group_sel 	= PAP_GROUP_PG_PH;
    pap_info_t.wr_rd_sel    = PAP_PH3_PH4;
#else
    pap_info_t.group_sel 	= PAP_GROUP_PC_PD;
#endif
    pap_info_t.timing_setup = 2;
    pap_info_t.timing_hold  = 2;
    pap_info_t.timing_width = 5;
    pap = dev_open("pap", &pap_info_t);
#endif
    ASSERT(pap, "open pap dev fail!");
#endif


    HX8369A_RST_H();
    delay_2ms(60);
    HX8369A_RST_L();
    delay_2ms(10);
    HX8369A_RST_H();
    delay_2ms(100);
}

#ifdef SOFT_SIM

#if (DBI_FORMAT == DBI_16BITS)||(DBI_FORMAT == DBI_18BITS)
static void LCDWrite(u16 dat)
{
    HX8369A_PORT_OUT();
    HX8369A_WR_L();

    HX8369A_PORT_W = dat;
    HX8369A_WR_H();
}

static u16 LCDRead()
{
    u16 ret;
    HX8369A_PORT_IN();
    HX8369A_RD_L();
    HX8369A_RD_H();

    ret = HX8369A_PORT_R;

    return ret;
}
#elif (DBI_FORMAT == DBI_8BITS)

static void LCDWrite(u16 dat)
{
    HX8369A_PORT_OUT();
    HX8369A_WR_L();
    HX8369A_PORT_W &= ~0xff00;
    HX8369A_PORT_W |= (dat & 0xff) << 8;
    HX8369A_WR_H();
}

static u16 LCDRead()
{
    HX8369A_PORT_IN();
    HX8369A_RD_L();
    HX8369A_RD_H();

    return (HX8369A_PORT_R >> 8) & 0xff;
}
#elif (DBI_FORMAT == DBI_9BITS)
static void LCDWrite(u16 dat)
{
    HX8369A_PORT_OUT();
    HX8369A_WR_L();
    HX8369A_PORT_W = (dat & 0x1ff) << 7;
    //delay(5);
    HX8369A_WR_H();
    //delay(5);
}

static u16 LCDRead()
{
    HX8369A_PORT_IN();
    HX8369A_RD_L();
    //delay(5);
    HX8369A_RD_H();
    //delay(5);
    return (HX8369A_PORT_R >> 7) & 0x1ff;
}
#elif (DBI_FORMAT == DBI_24BITS)
static void LCDWrite(u16 dat)
{
    u8 dath, datl;

    datl = dat & 0xff;
    dath = (dat >> 8) & 0xff;

    HX8369A_PORT_OUT();

    HX8369A_WR_L();
    //[1..0]
    PORTD_DIR &= ~ 0xc0;
    PORTD_OUT &= ~ 0xc0;
    PORTD_OUT |= (datl & 0x03) << 6;
    //[7..2]
    HX8369A_PORT_W &= ~ 0x3f;
    HX8369A_PORT_W &= ~ 0x3f;
    HX8369A_PORT_W |=  datl >> 2;

    //9..8
    PORTD_DIR &= ~ 0x300;
    PORTD_OUT &= ~ 0x300;
    PORTD_OUT |= (dath & 0x03) << 8;
    //[15..10]
    HX8369A_PORT_W &= ~ 0xfc0;
    HX8369A_PORT_W &= ~ 0xfc0;
    HX8369A_PORT_W |= (dath >> 2) << 6;

    HX8369A_WR_H();
}

static u16 LCDRead()
{
    u16 ret;
    HX8369A_PORT_IN();
    HX8369A_RD_L();
    //delay(100);
    HX8369A_RD_H();

    ret = (HX8369A_PORT_R & 0x3f) << 2;

    PORTD_DIR |= 0xc0;
    ret |= (PORTD_IN & 0xc0) >> 6;

    return ret & 0xff;
}
#endif
#endif

static void WriteComm(u16 cmd)
{
    HX8369A_CS_EN();
    HX8369A_RS_L();
#ifndef SOFT_SIM
    dev_write(pap, &cmd, sizeof(cmd));
#else
    HX8369A_RD_H();
    //delay(100);
    LCDWrite(cmd);
#endif
    HX8369A_CS_DIS();
}

static void WriteData(u16 dat)
{
    HX8369A_CS_EN();
    HX8369A_RS_H();
#ifndef SOFT_SIM
    dev_write(pap, &dat, sizeof(dat));
#else
    HX8369A_RD_H();
    //delay(100);
    LCDWrite(dat);
#endif
    HX8369A_CS_DIS();
}

static u16 ReadData()
{
    u16 ret = 0;

    HX8369A_CS_EN();
    HX8369A_RS_H();
#ifndef SOFT_SIM
    dev_read(pap, &ret, sizeof(ret));
#else
    HX8369A_WR_H();
    ret = LCDRead();
#endif
    HX8369A_CS_DIS();

    return ret;
}


static void hx8369a_init_code(const InitCode *code, u8 cnt)
{
    u8 i, j;

    for (i = 0; i < cnt; i++) {
        if (code[i].cmd == REGFLAG_DELAY) {
            delay_2ms(code[i].cnt);
        } else {
            WriteComm(code[i].cmd);
            /* printf("cmd:%x ", code[i].cmd); */
            /* printf("cnt:%x ",code[i].cnt); */
            /* printf("dat:%x ",code[i].dat); */
            for (j = 0; j < code[i].cnt; j++) {
                WriteData(code[i].dat[j]);
                /* printf("%02x ", code[i].dat[j]); */
            }
            /* printf("\n"); */
        }
    }
}

static void lcd_hx8369a_setxy(int Xstart, int Xend, int Ystart, int Yend)
{
    //HX8352-C
    /* printf("[%d,%d,%d,%d]\n",Xstart,Xend,Ystart,Yend); */

    /* #ifdef DIRECTION_HORI */
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
    /* #else */
    /* WriteComm(0x2a); */
    /* WriteData(Ystart >> 8); */
    /* WriteData(Ystart & 0xff); */
    /* WriteData(Yend >> 8); */
    /* WriteData(Yend & 0xff); */

    /* WriteComm(0x2b); */
    /* WriteData(Xstart >> 8); */
    /* WriteData(Xstart & 0xff); */
    /* WriteData(Xend >> 8); */
    /* WriteData(Xend & 0xff); */
    /* #endif */
    WriteComm(0x2c);

    HX8369A_CS_EN();
    HX8369A_RS_H();
}

static void lcd_hx8369a_clear(u16 w, u16 h, u32 color)
{
    u16 i, j;
    u32 dat;
    u32 len;

    len = w * h;
#ifdef DIRECTION_HORI
    lcd_hx8369a_setxy(0, w - 1, 0, h - 1);
#else
    lcd_hx8369a_setxy(0, h - 1, 0, w - 1);
#endif

#if (DBI_FORMAT == DBI_8BITS)
    dat = (((color >> 19) << 11) & 0xf800) | (((color >> 10) << 5) & 0x07e0) | ((color >> 3) & 0x001f);
#elif (DBI_FORMAT == DBI_9BITS)
    dat = (((color >> 19) << 12) & 0x3f000) | (((color >> 10) << 6) & 0x0fc0) | ((color >> 3) & 0x003f);
#elif (DBI_FORMAT == DBI_16BITS)
    dat = (((color >> 19) << 11) & 0xf800) | (((color >> 10) << 5) & 0x07e0) | ((color >> 3) & 0x001f);
#elif (DBI_FORMAT == DBI_18BITS)
    dat = (((color >> 19) << 12) & 0x3f000) | (((color >> 10) << 6) & 0x0fc0) | ((color >> 3) & 0x003f);
#endif

    while (len--) {
#if (DBI_FORMAT == DBI_8BITS)
        WriteData(dat >> 8);
        WriteData(dat);
#elif (DBI_FORMAT == DBI_9BITS)
        WriteData(dat >> 9);
        WriteData(dat);
#elif (DBI_FORMAT == DBI_16BITS)
        WriteData(dat);
#elif (DBI_FORMAT == DBI_18BITS)
        WriteData(dat);
#elif (DBI_FORMAT == DBI_24BITS)
        WriteData(color);
#endif
    }
}

static void lcd_hx8369a_send_image(u16 w, u16 h, u8 *image)
{
    u32 len;
    u8 *p;
    u16 dat0, dat1;
    u16 i, j;

#ifdef DIRECTION_HORI
    lcd_hx8369a_setxy(0, w - 1, 0, h - 1);
#else
    lcd_hx8369a_setxy(0, h - 1, 0, w - 1);
#endif

    len = w * h;
    p = image;

    while (len--) {
#if (DBI_FORMAT == DBI_8BITS)
        dat0 = *image++;
        dat1 = *image++;
        WriteData(dat1);
        WriteData(dat0);
#elif (DBI_FORMAT == DBI_9BITS)
        /* WriteData(dat >> 9); */
        /* WriteData(dat); */
#elif (DBI_FORMAT == DBI_16BITS)
        dat0 = *image++;
        dat1 = *image++;
        WriteData(dat1 << 8 | dat0);
#elif (DBI_FORMAT == DBI_18BITS)
        /* WriteData(dat); */
#elif (DBI_FORMAT == DBI_24BITS)
        /* WriteData(color); */
#endif
    }
}


static int lcd_hx8369a_mcu_init(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u16 ret;
    u8 i;


    printf("\nlcd hx8369 init...\n");

    LCD_Reset();

    //WriteComm(0x01);
    //delay2ms(100);

    WriteComm(0x11);
    delay_2ms(120);
#if ((DBI_FORMAT==DBI_8BITS)||(DBI_FORMAT==DBI_16BITS))
    WriteComm(0x2D);
    for (i = 0; i <= 63; i++) {
        WriteData(i * 8);
    }

    for (i = 0; i <= 63; i++) {
        WriteData(i * 4);
    }

    for (i = 0; i <= 63; i++) {
        WriteData(i * 8);
    }
#elif ((DBI_FORMAT==DBI_9BITS)||(DBI_FORMAT==DBI_18BITS))
    WriteComm(0x2D);
    for (i = 0; i <= 63; i++) {
        WriteData(i * 4);
    }

    for (i = 0; i <= 63; i++) {
        WriteData(i * 4);
    }

    for (i = 0; i <= 63; i++) {
        WriteData(i * 4);
    }
#endif

    hx8369a_init_code(code, sizeof(code) / sizeof(code[0]));

    WriteComm(0x0C);
    ReadData();
    ret = ReadData();//0x05
    printf("0x0C :  0x%x\n", ret);

#if 1
    WriteComm(0xF4);
    ret = ReadData();
    printf("ret = %x\n", ret);
    ret = ReadData();//0x69
    printf("ret = %x\n", ret);
    ASSERT((ret & 0xff) == 0x69);
    ret = ReadData();//0x02
    printf("ret = %x\n", ret);
    ASSERT((ret & 0xff) == 0x02);
#endif

    /* lcd_hx8369a_send_image(800, 480, rgb565_800x480); */
    /* delay2ms(500); */
    /* lcd_hx8369a_send_image(800, 480, rgb565_800x480_4); */
    /* delay2ms(500); */
    /* lcd_hx8369a_send_image(800, 480, rgb565_800x480_5); */
    /* delay2ms(500); */
    /* lcd_hx8369a_clear(800, 480, 0xff0000); */
    /* delay2ms(500); */
    /* lcd_hx8369a_clear(800, 480, 0x00ff00); */
    /* delay2ms(500); */
    /* lcd_hx8369a_clear(800, 480, 0x0000ff); */
    /* delay2ms(500); */
    /* lcd_hx8369a_clear(800, 480, 0x000000); */
    /* delay2ms(500); */
    /* lcd_hx8369a_clear(800, 480, 0xffffff); */
    /* delay2ms(500); */

#ifdef DIRECTION_HORI
    lcd_hx8369a_setxy(0, 800 - 1, 0, 480 - 1);
#else
    lcd_hx8369a_setxy(0, 480 - 1, 0, 800 - 1);
#endif


    return 0;
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

REGISTER_IMD_DEVICE_BEGIN(lcd_hx8369a_mcu_dev) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0x0000FF,
        .canvas_color    = 0x0000FF,
        .x               = 0,
        .y               = 0,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .sw_mode         = false,
        .sw_x            = 352,
        .sw_y            = 0,
        .sw_xres         = 800,
        .sw_yres         = 480,
        .target_xres     = 800/* LCD_DEV_WIDTH */,
        .target_yres     = 480/* LCD_DEV_HIGHT */,
        /* .layer_sw_mode   = true, */
        /* .layer_sw_xres   = 1280, */
        /* .layer_sw_yres   = 480, */
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .sample          = LCD_DEV_SAMPLE,

        .len 			 = LEN_256,
#ifdef DIRECTION_HORI
        .rotate_en       = false,	    // 旋转使能
        .hori_mirror_en  = true,	    // 水平镜像使能
        .vert_mirror_en  = true,		// 垂直镜像使能
#else
        .rotate_en       = true,
        .hori_mirror_en  = false,
        .vert_mirror_en  = true,
#endif

#if (DBI_FORMAT == DBI_8BITS)
        .format          = FORMAT_RGB565,
#elif (DBI_FORMAT == DBI_9BITS)
        .format          = FORMAT_RGB666,
#elif (DBI_FORMAT == DBI_16BITS)
        .format          = FORMAT_RGB565,
#elif (DBI_FORMAT == DBI_18BITS)
        .format          = FORMAT_RGB666,
#elif (DBI_FORMAT == DBI_24BITS)
        .format          = FORMAT_RGB888,
#endif
        .interlaced_mode = INTERLACED_NONE,

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

    .drive_mode      = MODE_MCU,
#if (DBI_FORMAT == DBI_8BITS)
    .ncycle          = CYCLE_TWO,
    .data_width      = PORT_8BITS,
#elif (DBI_FORMAT == DBI_9BITS)
    .ncycle          = CYCLE_TWO,
    .data_width      = PORT_9BITS,
#elif (DBI_FORMAT == DBI_16BITS)
    .ncycle          = CYCLE_ONE,
    .data_width      = PORT_16BITS,
#elif (DBI_FORMAT == DBI_18BITS)
    .ncycle          = CYCLE_ONE,
    .data_width      = PORT_18BITS,
#elif (DBI_FORMAT == DBI_24BITS)
    .ncycle          = CYCLE_ONE,
    .data_width      = PORT_24BITS,
#endif

    .dclk_set        = /*CLK_EN |*/CLK_NEGATIVE,
    .sync0_set       =  CLK_EN | SIGNAL_DEN  | CLK_NEGATIVE,
    .sync1_set       = /*CLK_EN|*/SIGNAL_HSYNC | CLK_NEGATIVE,
    .sync2_set       = /*CLK_EN|*/SIGNAL_VSYNC | CLK_NEGATIVE,

#if (defined USE_AA_GROUP)
    .port_sel        = PORT_GROUP_AA,
#elif (defined USE_AB_GROUP)
    .port_sel        = PORT_GROUP_AB,
#else
    .port_sel        = PORT_GROUP_B,
#endif
    /* .clk_cfg    	 = PLL2_CLK | DIVA_3 | DIVB_2 | DIVC_1, */
    .clk_cfg    	 = PLL2_CLK | DIVA_1 | DIVB_1 | DIVC_1,

    .timing = {
#if (DBI_FORMAT == DBI_16BITS)||(DBI_FORMAT == DBI_18BITS)||(DBI_FORMAT == DBI_24BITS)
#ifdef DIRECTION_HORI
        .hori_total 		    = 800 + 40,
        .hori_sync 		        = 20,
        .hori_back_porth 	    = 20,
        .hori_pixel 	        = 800,

        .vert_total 		    = 480 + 80,
        .vert_sync 		        = 40,
        .vert_back_porth_odd 	= 40,
        .vert_back_porth_even 	= 0,
        .vert_pixel 	        = 480,
#else
        .hori_total 		    = 480 + 40,
        .hori_sync 		        = 20,
        .hori_back_porth 	    = 20,
        .hori_pixel 	        = 480,

        .vert_total 		    = 800 + 40,
        .vert_sync 		        = 20,
        .vert_back_porth_odd 	= 20,
        .vert_back_porth_even 	= 0,
        .vert_pixel 	        = 800,
#endif
#else
#ifdef DIRECTION_HORI
        .hori_total 		    = (800 + 20) * 2,
        .hori_sync 		        = 10 * 2,
        .hori_back_porth 	    = 10 * 2,
        .hori_pixel 	        = 800,

        .vert_total 		    = 480 + 80,
        .vert_sync 		        = 40,
        .vert_back_porth_odd 	= 40,
        .vert_back_porth_even 	= 0,
        .vert_pixel 	        = 480,
#else
        .hori_total 		    = (480 + 40) * 2,
        .hori_sync 		        = 10 * 2,
        .hori_back_porth 	    = 10 * 2,
        .hori_pixel 	        = 480,

        .vert_total 		    = 880,
        .vert_sync 		        = 20,
        .vert_back_porth_odd 	= 20,
        .vert_back_porth_even 	= 0,
        .vert_pixel 	        = 800,
#endif
#endif
    },
},
REGISTER_IMD_DEVICE_END()

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type    = LCD_DVP_MCU,
    .init    = lcd_hx8369a_mcu_init,
    .dev     = &lcd_hx8369a_mcu_dev,
    .setxy   = lcd_hx8369a_setxy,
    .bl_ctrl = lcd_hx8369a_backctrl,
};

#endif
