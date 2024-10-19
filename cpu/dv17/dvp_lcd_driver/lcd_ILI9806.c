#include "asm/cpu.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"


#ifdef LCD_ILI9806

void delay_2ms(int cnt);

#define USE_AA_GROUP

#ifdef USE_AA_GROUP
#define SPI_DCX_OUT()
#define SPI_DCX_L()
#define SPI_DCX_H()

#define PIN_RST     8
#define PIN_CS      9
#define PIN_SCL     10
#define PIN_SDA     11

//RESET
#define SPI_RST_OUT()   PORTH_DIR &=~ BIT(PIN_RST)
#define SPI_RST_H()     PORTH_OUT |=  BIT(PIN_RST)
#define SPI_RST_L()     PORTH_OUT &=~ BIT(PIN_RST)

//CS
#define SPI_CSX_OUT()   PORTH_DIR &=~ BIT(PIN_CS)
#define SPI_CSX_H()     PORTH_OUT |=  BIT(PIN_CS)
#define SPI_CSX_L()     PORTH_OUT &=~ BIT(PIN_CS)

//SCL
#define SPI_SCL_OUT()   PORTH_DIR &=~ BIT(PIN_SCL)
#define SPI_SCL_H()     PORTH_OUT |=  BIT(PIN_SCL)
#define SPI_SCL_L()     PORTH_OUT &=~ BIT(PIN_SCL)

//SDA
#define SPI_SDA_OUT()   PORTH_DIR &=~ BIT(PIN_SDA)
#define SPI_SDA_IN()    PORTH_DIR |=  BIT(PIN_SDA)
#define SPI_SDA_H()     PORTH_OUT |=  BIT(PIN_SDA)
#define SPI_SDA_L()     PORTH_OUT &=~ BIT(PIN_SDA)
#define SPI_SDA_R()     PORTH_IN  &   BIT(PIN_SDA)

#else

#define SPI_DCX_OUT()
#define SPI_DCX_L()
#define SPI_DCX_H()

//RESET
#define SPI_RST_OUT()   PORTB_DIR &=~ BIT(6)
#define SPI_RST_H()     PORTB_OUT |=  BIT(6)
#define SPI_RST_L()     PORTB_OUT &=~ BIT(6)

//CS
#define SPI_CSX_OUT()   PORTD_DIR &=~ BIT(12)
#define SPI_CSX_H()     PORTD_OUT |=  BIT(12)
#define SPI_CSX_L()     PORTD_OUT &=~ BIT(12)

//SCL
#define SPI_SCL_OUT()   PORTB_DIR &=~ BIT(5)
#define SPI_SCL_H()     PORTB_OUT |=  BIT(5)
#define SPI_SCL_L()     PORTB_OUT &=~ BIT(5)

//SDA
#define SPI_SDA_OUT()   PORTB_DIR &=~ BIT(7)
#define SPI_SDA_IN()    PORTB_DIR |= BIT(7)
#define SPI_SDA_H()     PORTB_OUT |=  BIT(7)
#define SPI_SDA_L()     PORTB_OUT &=~ BIT(7)
#define SPI_SDA_R()     PORTB_IN & BIT(7)

#endif

/* #define MOVE_MODE   //部分显示 */

extern void delay_us(unsigned int us);

static void spi_io_init()
{
    SPI_DCX_OUT();
    SPI_SCL_OUT();
    SPI_CSX_OUT();
    SPI_SDA_OUT();
    SPI_RST_OUT();

    SPI_SDA_H();
    SPI_CSX_H();
    SPI_DCX_L();
    SPI_SCL_L();
}

static void spi_write_cmd(u8 cmd)
{
    u8 i;
    SPI_CSX_L();
    ///command
    SPI_SDA_OUT();
    SPI_SDA_L();

    SPI_SCL_L();
    delay_us(100);
    SPI_SCL_H();
    delay_us(100);
    ///command

    for (i = 0; i < 8; i++) {
        if (cmd & BIT(7)) {
            SPI_SDA_H();
        } else {
            SPI_SDA_L();
        }

        cmd <<= 1;

        SPI_SCL_L();
        delay_us(100);
        SPI_SCL_H();
        delay_us(200);
    }
    SPI_SCL_L();
    delay_us(100);
    SPI_CSX_H();
    delay_us(100);
}

static void spi_write_dat(u8 dat)
{
    u8 i;
    SPI_CSX_L();
    ///data
    SPI_SDA_OUT();
    SPI_SDA_H();

    SPI_SCL_L();
    delay_us(100);
    SPI_SCL_H();
    delay_us(100);
    ///data

    for (i = 0; i < 8; i++) {
        if (dat & BIT(7)) {
            SPI_SDA_H();
        } else {
            SPI_SDA_L();
        }

        dat <<= 1;
        SPI_SCL_L();
        delay_us(100);
        SPI_SCL_H();
        delay_us(100);
    }
    SPI_SCL_L();
    delay_us(100);
    SPI_CSX_H();
    delay_us(100);
}

static void set_display_area(unsigned int Xstart, unsigned int Xend, unsigned int Ystart, unsigned int Yend)
{
    spi_write_cmd(0x2a);
    spi_write_dat(Xstart >> 8);
    spi_write_dat(Xstart & 0xff);
    spi_write_dat(Xend >> 8);
    spi_write_dat(Xend & 0xff);

    spi_write_cmd(0x2b);
    spi_write_dat(Ystart >> 8);
    spi_write_dat(Ystart & 0xff);
    spi_write_dat(Yend >> 8);
    spi_write_dat(Yend & 0xff);

    spi_write_cmd(0x2c);
}

static void lcd_ili9806e_init(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;

    printf("ili9806e_init...\r\n");
    spi_io_init();

    printf("reset\r\n");
    //RESET
    SPI_RST_H();
    delay_2ms(5);
    SPI_RST_L();
    delay_2ms(5);
    SPI_RST_H();
    delay_2ms(5);

    /* spi_write_cmd(0x01); */
    /* delay_2ms(5); */

    spi_write_cmd(0xFF);
    spi_write_dat(0xFF);
    spi_write_dat(0x98);
    spi_write_dat(0x06);

    spi_write_cmd(0xBA);
    spi_write_dat(0xe0);

    spi_write_cmd(0x3A);
    spi_write_dat(0x57);

    spi_write_cmd(0xEC);
    spi_write_dat(0x24);
    spi_write_dat(0x4F);
    spi_write_dat(0x00);

    spi_write_cmd(0xBA);
    spi_write_dat(0x60);

    spi_write_cmd(0xBC);
    spi_write_dat(0x01);
    spi_write_dat(0x12);
    spi_write_dat(0x61);
    spi_write_dat(0xFF);
    spi_write_dat(0x10);
    spi_write_dat(0x10);
    spi_write_dat(0x0B);
    spi_write_dat(0x13);
    spi_write_dat(0x32);
    spi_write_dat(0x73);
    spi_write_dat(0xFF);
    spi_write_dat(0xFF);
    spi_write_dat(0x0E);
    spi_write_dat(0x0E);
    spi_write_dat(0x00);
    spi_write_dat(0x03);
    spi_write_dat(0x66);
    spi_write_dat(0x63);
    spi_write_dat(0x01);
    spi_write_dat(0x00);
    spi_write_dat(0x00);

    spi_write_cmd(0xBD);
    spi_write_dat(0x01);
    spi_write_dat(0x23);
    spi_write_dat(0x45);
    spi_write_dat(0x67);
    spi_write_dat(0x01);
    spi_write_dat(0x23);
    spi_write_dat(0x45);
    spi_write_dat(0x67);

    spi_write_cmd(0xBE);
    spi_write_dat(0x00);
    spi_write_dat(0x21);
    spi_write_dat(0xAB);
    spi_write_dat(0x60);
    spi_write_dat(0x22);
    spi_write_dat(0x22);
    spi_write_dat(0x22);
    spi_write_dat(0x22);
    spi_write_dat(0x22);

    spi_write_cmd(0xC7);
    spi_write_dat(0x6F);
    spi_write_dat(0x80);

    spi_write_cmd(0xC7);
    spi_write_dat(0x78);
    spi_write_dat(0x80);

    spi_write_cmd(0xED);
    spi_write_dat(0x7F);
    spi_write_dat(0x0F);
    spi_write_dat(0x00);

    spi_write_cmd(0xB6);
    spi_write_dat(0xA3);//RGB/SYNC mode

    spi_write_cmd(0xB0);
    spi_write_dat(0xc0);//


    u16 HBP = 40;

    //800+26+26
    u8 VFP = 0x14 - 4;
    u8 VBP = 0x14 + 4; //24;
    spi_write_cmd(0xB5);
    spi_write_dat(VFP);
    spi_write_dat(VBP);
    spi_write_dat(HBP);
    spi_write_dat(HBP >> 8);

//	spi_write_cmd(0xC0);
//    spi_write_dat(0xAB);
//    spi_write_dat(0x0B);
//    spi_write_dat(0x0A);

    spi_write_cmd(0xC0);
    spi_write_dat(0xAB);
    spi_write_dat(0x0B);
    spi_write_dat(0x0A);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x88);
    spi_write_dat(0x2C);
    spi_write_dat(0x50);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0xFF);

    spi_write_cmd(0xFC);
    spi_write_dat(0x09);

    spi_write_cmd(0xDF);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x00);
    spi_write_dat(0x20);

    spi_write_cmd(0xF3);
    spi_write_dat(0x74);

    //480x800
    spi_write_cmd(0xB1);
    spi_write_dat(0x00);
    spi_write_dat(0x12);
    spi_write_dat(0x14);

    spi_write_cmd(0xF2);
    spi_write_dat(0x00);
    spi_write_dat(0x59);
    spi_write_dat(0x40);
    spi_write_dat(0x28);

    spi_write_cmd(0xC1);
    spi_write_dat(0x07);
    spi_write_dat(0x80);
    spi_write_dat(0x80);
    spi_write_dat(0x20);

    spi_write_cmd(0xE0);
    spi_write_dat(0x04);
    spi_write_dat(0x08);
    spi_write_dat(0x0D);
    spi_write_dat(0x0C);
    spi_write_dat(0x05);
    spi_write_dat(0x09);
    spi_write_dat(0x07);
    spi_write_dat(0x04);
    spi_write_dat(0x08);
    spi_write_dat(0x0C);
    spi_write_dat(0x11);
    spi_write_dat(0x04);
    spi_write_dat(0x07);
    spi_write_dat(0x1C);
    spi_write_dat(0x15);
    spi_write_dat(0x00);

    spi_write_cmd(0xE1);
    spi_write_dat(0x00);
    spi_write_dat(0x06);
    spi_write_dat(0x0B);
    spi_write_dat(0x0A);
    spi_write_dat(0x04);
    spi_write_dat(0x07);
    spi_write_dat(0x06);
    spi_write_dat(0x04);
    spi_write_dat(0x08);
    spi_write_dat(0x0C);
    spi_write_dat(0x11);
    spi_write_dat(0x04);
    spi_write_dat(0x06);
    spi_write_dat(0x1A);
    spi_write_dat(0x13);
    spi_write_dat(0x00);

    spi_write_cmd(0xF7);
    spi_write_dat(0x82);//480*800 resolution

    spi_write_cmd(0x44);
    spi_write_dat(0x03);
    spi_write_dat(0x56);

    spi_write_cmd(0x35);
    spi_write_dat(0x00);

    spi_write_cmd(0x36);
    spi_write_dat(0x00);

//    spi_write_cmd(0xb9);
//    spi_write_dat(0x00);
//    spi_write_dat(0x00);

    spi_write_cmd(0x51);
    spi_write_dat(0x83);

    set_display_area(0, 480 - 1, 0, 800 - 1);

    spi_write_cmd(0x13);
    spi_write_cmd(0x11);//Sleep Out
    spi_write_cmd(0x29);//Display ON

    printf("initial ok.\r\n");

}


static void lcd_ili9806e_backctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    /* if (on) { */
    /* gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value); */
    /* } else { */
    /* gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value); */
    /* } */
}


REGISTER_IMD_DEVICE_BEGIN(lcd_ili9806e_dev) = {
    .info = {
        .test_mode 	     = false,
        .test_mode_color = 0xFF0000,
        .canvas_color    = 0x0000FF,
        .x               = 0,
        .y               = 0,
        .xres 			 = LCD_DEV_WIDTH,
        .yres 			 = LCD_DEV_HIGHT,
        .target_xres     = LCD_DEV_WIDTH,
        .target_yres     = LCD_DEV_HIGHT,
        .buf_num  		 = LCD_DEV_BNUM,
        .buf_addr 		 = LCD_DEV_BUF,
        .sample          = LCD_DEV_SAMPLE,
        .format          = FORMAT_RGB666,
        .len 			 = LEN_256,
        .interlaced_mode = INTERLACED_NONE,

        .rotate_en       = true,	    // 旋转使能
        .hori_mirror_en  = true,	    // 水平镜像使能
        .vert_mirror_en  = false,		// 垂直镜像使能

        .adjust = {
            .y_gain = 0x100,
            .u_gain = 0x100,
            .v_gain = 0x100,
            .r_gain = 0x80 * 0.9,
            .g_gain = 0x80 * 0.9,
            .b_gain = 0x80 * 0.9,
            .r_offs = 6,
            .g_offs = 6,
            .b_offs = 6,
            .r_coe0 = 0x80,
            .g_coe1 = 0x80,
            .b_coe2 = 0x80,
            .r_gma  = 100,
            .g_gma  = 100,
            .b_gma  = 100,
        },
    },

    .drive_mode      = MODE_RGB_SYNC,
    .ncycle          = CYCLE_ONE,
    .dclk_set        = CLK_EN | CLK_NEGATIVE,
    .sync0_set       = SIGNAL_DEN  | CLK_EN | CLK_NEGATIVE,
    .sync1_set       = SIGNAL_HSYNC | CLK_EN | CLK_NEGATIVE,
    .sync2_set       = SIGNAL_VSYNC | CLK_EN | CLK_NEGATIVE,
    .data_width      = PORT_18BITS,
#ifdef USE_AA_GROUP
    .port_sel        = PORT_GROUP_AA,
#else
    .port_sel        = PORT_GROUP_B,
#endif
    /* .clk_cfg    	 = PLL2_CLK | DIVA_3 | DIVB_2 | DIVC_2, */
    .clk_cfg    	 = PLL2_CLK | DIVA_1 | DIVB_2 | DIVC_1, /* FPGA */

    .timing = {
        .hori_total 		    =    646,
        .hori_sync 		        =    2,
        .hori_back_porth 	    =    40,
        .hori_pixel 	        =    480,

        .vert_total 		    =    848,
        .vert_sync 		        =    1,//14,
        .vert_back_porth_odd 	=    24,
        .vert_back_porth_even 	=    0,
        .vert_pixel 	        =    800,
    },
},
REGISTER_IMD_DEVICE_END()


REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .type 	 = LCD_DVP_RGB,
    .init 	 = lcd_ili9806e_init,
    .dev  	 = &lcd_ili9806e_dev,
    .bl_ctrl = lcd_ili9806e_backctrl,
};
#endif

