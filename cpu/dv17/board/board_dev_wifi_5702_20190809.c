#include "system/includes.h"

#include "app_config.h"
#include "gSensor_manage.h"
#include "device/av10_spi.h"
#include "vm_api.h"
#include "asm/pwm.h"
#include "asm/iis.h"
#include "asm/plnk.h"
#include "touch_panel_manager.h"
#include "video_system.h"
// *INDENT-OFF*

#ifdef CONFIG_WIFI_ENABLE
#include "device/wifi_dev.h"
#endif
#ifdef CONFIG_BOARD_DEV_WIFI_5702_20190809


UART0_PLATFORM_DATA_BEGIN(uart0_data)
	.baudrate = 460800,//115200,
	/* .baudrate = 115200, */
	 //.tx_pin = IO_PORTA_07,.rx_pin = IO_PORTA_08,
	 /* .tx_pin = IO_PORTG_06,.rx_pin = IO_PORTG_07, */
	  .tx_pin = IO_PORTB_03,
	 /* .tx_pin = IO_PORTB_14,.rx_pin = IO_PORTB_15, */
  	.flags = UART_DEBUG,
UART0_PLATFORM_DATA_END();

UART1_PLATFORM_DATA_BEGIN(uart1_data)
	.baudrate = 460800,//115200,
	/* .baudrate = 115200, */
	 .tx_pin = IO_PORTH_02,.rx_pin = IO_PORTH_05,
	 /* .tx_pin = IO_PORTH_14,.rx_pin = IO_PORTH_15, */
  	.flags = UART_DEBUG,
UART1_PLATFORM_DATA_END();

UART2_PLATFORM_DATA_BEGIN(uart2_data)
	/* .baudrate = 115200, */
	.baudrate = 460800,//115200,
	 .tx_pin = IO_PORTH_10,.rx_pin = IO_PORTH_11,
	 /* .tx_pin = IO_PORTH_00,.rx_pin = IO_PORTH_01, */
  	.flags = UART_DEBUG,
UART2_PLATFORM_DATA_END();

UART3_PLATFORM_DATA_BEGIN(uart3_data)
	.baudrate = 460800,//115200,
	/* .baudrate = 115200, */
	 /* .tx_pin = IO_PORTE_00,.rx_pin = IO_PORTE_01, */
	 .tx_pin = IO_PORTH_13,
     .port =  PORT_REMAP,
	 /* .rx_pin = IO_PORTB_03, */
  	.flags = UART_DEBUG,
UART3_PLATFORM_DATA_END();


#ifdef CONFIG_SD0_ENABLE

int sdmmc_0_io_detect(const struct sdmmc_platform_data *data)
{
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTG_06);
        gpio_set_pull_up(IO_PORTG_06, 1);
        gpio_set_pull_down(IO_PORTG_06, 0);
    }

    return !gpio_read(IO_PORTG_06);

}
static void sdmmc_power(int on)
{
    gpio_direction_output(IO_PORTG_07, !on);
}



SD0_PLATFORM_DATA_BEGIN(sd0_data)
	/* .port 					= 'A', */
	/* .port 					= 'B', */
    .port 					= 'C',
    /* .port 					= 'D', */
	.priority 				= 3,
	.data_width 			= 4,
	/* .data_width 			= 1, */
	.speed 					= 30000000,
	/* .detect_mode 			= SD_CMD_DECT, */
	/* .detect_func 			= sdmmc_0_clk_detect, */
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_0_io_detect,
    .power                  = sdmmc_power,
SD0_PLATFORM_DATA_END()

#endif //CONFIG_SD0_ENABLE

#ifdef CONFIG_SD1_ENABLE

int sdmmc_1_io_detect(const struct sdmmc_platform_data *data)
{
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_12);
        gpio_set_pull_up(IO_PORTA_12, 1);
        gpio_set_pull_down(IO_PORTA_12, 0);
    }

    return !gpio_read(IO_PORTA_12);

}

static void sdmmc_power(int on)
{
    gpio_direction_output(IO_PORTG_14, !on);
}

SD1_PLATFORM_DATA_BEGIN(sd1_data)
	/* .port 					= 'A', */
	.port 					= 'B',
	.priority 				= 3,
	.data_width 			= 4,
	.speed 					= 30000000,
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_1_io_detect,
    /*.power                  = sdmmc_power,*/
SD1_PLATFORM_DATA_END()

#endif //CONFIG_SD1_ENABLE

#ifdef CONFIG_WIFI_ENABLE
WIFI_PLATFORM_DATA_BEGIN(wifi_data)
    .module = RTL8189E,
    .sdio_parm = SDIO_GRP_2 | SDIO_PORT_1 | SDIO_4_BIT_DATA |SDIO_DATA1_IRQ |20 * 1000000,
    .wakeup_port = IO_PORTH_06,//IO_PORTB_11,
    .cs_port = -1,
    .power_port = -1,
WIFI_PLATFORM_DATA_END()
#endif



#ifdef CONFIG_SD2_ENABLE

int sdmmc_2_io_detect(const struct sdmmc_platform_data *data)
{
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_12);
        gpio_set_pull_up(IO_PORTA_12, 1);
        gpio_set_pull_down(IO_PORTA_12, 0);
    }

    return !gpio_read(IO_PORTA_12);

}

static void sdmmc_power(int on)
{
    gpio_direction_output(IO_PORTG_14, !on);
}


SD2_PLATFORM_DATA_BEGIN(sd2_data)
	.port 					= 'B',
	.priority 				= 3,
	.data_width 			= 4,
	.speed 					= 30000000,
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_2_io_detect,//sdmmc_2_clk_detect,
	.power                  = sdmmc_power,
SD2_PLATFORM_DATA_END()

#endif //CONFIG_SD2_ENABLE

HW_IIC0_PLATFORM_DATA_BEGIN(hw_iic0_data)
	/* IO_PORTG_06, IO_PORTG_07,  */
	/* IO_PORTH_12, IO_PORTH_14,  */
	/* IO_PORTB_04, IO_PORTB_03,  */
	/* IO_PORTA_05, IO_PORTA_06,  */
    .clk_pin = IO_PORTG_06,
    .dat_pin = IO_PORTG_07,
	.baudrate = 0x0f,//300k  0x50 250k
HW_IIC0_PLATFORM_DATA_END()

HW_IIC1_PLATFORM_DATA_BEGIN(hw_iic1_data)
	/* IO_PORTB_00, IO_PORTB_01,  */
	/* IO_PORTA_02, IO_PORTA_01,  */
	/* IO_PORTG_00, IO_PORTG_01,  */
	/* IO_PORTE_04, IO_PORTE_05,  */
    .clk_pin = IO_PORTG_06,
    .dat_pin = IO_PORTG_07,
	.baudrate = 0x1f,//300k  0x50 250k
HW_IIC1_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic0_data)
	.clk_pin = IO_PORTG_14,
	.dat_pin = IO_PORTG_15,
	.sw_iic_delay = 100,
SW_IIC_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic1_data)
	.clk_pin = IO_PORTG_14,
	.dat_pin = IO_PORTG_15,
	.sw_iic_delay = 100,
SW_IIC_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic2_data)
	.clk_pin = -1,
	.dat_pin = -1,
	.sw_iic_delay = 50,
SW_IIC_PLATFORM_DATA_END()


LCD_PLATFORM_DATA_BEGIN(lcd_data)
    .lcd_name = "ek79030_v2",
	.lcd_io = {
        .backlight = IO_PORTG_02,
        .backlight_value = 1,

		.lcd_reset   = IO_PORTG_00,
		.lcd_standby = IO_PORTE_02,
        .lcd_cs      = -1,
        .lcd_rs      = -1,
        .lcd_spi_ck  = -1,
        .lcd_spi_di  = -1,
        .lcd_spi_do  = -1,
	},
    .lcd_port.mipi_mapping = {
        .x0_lane = MIPI_LANE_EN  | MIPI_LANE_D3,
        .x1_lane = MIPI_LANE_EN  | MIPI_LANE_D2,
        .x2_lane = MIPI_LANE_EN  | MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_EN  | MIPI_LANE_D1,
        .x4_lane = MIPI_LANE_EN  | MIPI_LANE_D0,
    },
LCD_PLATFORM_DATA_ADD()
    .lcd_name = "LX50FWB4001_V2",
    .lcd_io = {
        .backlight = IO_PORTG_02,
        .backlight_value = 1,

        .lcd_reset = IO_PORTG_04,
        .lcd_cs    = -1,
        .lcd_rs    = -1,
        .lcd_spi_ck= -1,
        .lcd_spi_di= -1,
        .lcd_spi_do= -1,
    }
LCD_PLATFORM_DATA_END()



#ifdef CONFIG_TOUCH_PANEL_ENABLE
extern const struct device_operations touch_panel_dev_ops;
SW_TOUCH_PANEL_PLATFORM_DATA_BEGIN(touch_panel_data)
    .enable         = 1,
    .iic_dev        = "iic2",
    .rst_pin        = IO_PORTG_04,
    .int_pin        = IO_PORTG_05,
    ._MAX_POINT     = 1,
    ._MAX_X         = 1280,
    ._MAX_Y         = 480,
    ._INT_TRIGGER   = 3,
    ._X2Y_EN        = 1,
    ._X_MIRRORING   = 0,
    ._Y_MIRRORING   = 0,
    ._DEBUGP        = 0,
    ._DEBUGE        = 0,
    .points         ={
        .point_num  = 0,
    }
SW_TOUCH_PANEL_PLATFORM_DATA_END()
#endif //CONFIG_TOUCH_PANEL_ENABLE


#ifdef CONFIG_VIDEO0_ENABLE

static const struct camera_platform_data camera0_data = {
    .xclk_gpio      = IO_PORTE_02,
	.reset_gpio     = IO_PORTA_08,
	.pwdn_gpio      = IO_PORTB_09,//IO_PORTE_01
	.interface      = SEN_INTERFACE_CSI2,
    .csi2 = {
        .data_lane_num = 2,
        .clk_rmap = CSI2_X2_LANE,
        .clk_inv = 1,
        .d0_rmap = CSI2_X4_LANE,
        .d0_inv = 1,
        .d1_rmap = CSI2_X3_LANE,
        .d1_inv = 1,
        .tval_hstt = 12,
        .tval_stto = 12,
    }

	/* .interface      = SEN_INTERFACE0, */
    /* .dvp = { */
        /* .pclk_gpio  = IO_PORTA_15, */
        /* .hsync_gpio = IO_PORTE_00, */
        /* .vsync_gpio = IO_PORTE_01, */
		/* .io_function_sel = DVP_SENSOR0(1), */
        /* .data_gpio  = { */
          /* -1, // IO_PORTA_05, */
          /* -1, // IO_PORTA_06, */
            /* IO_PORTA_07, */
            /* IO_PORTA_08, */
            /* IO_PORTA_09, */
            /* IO_PORTA_10, */
            /* IO_PORTA_11, */
            /* IO_PORTA_12, */
            /* IO_PORTA_13, */
            /* IO_PORTA_14, */
        /* }, */
    /* } */
};
static const struct video_subdevice_data video0_subdev_data[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera0_data },
};
static const struct video_platform_data video0_data = {
    .data = video0_subdev_data,
    .num = ARRAY_SIZE(video0_subdev_data),
};

#endif



#ifdef CONFIG_VIDEO1_ENABLE

static bool camera1_online_detect()
{
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_05);
        gpio_set_pull_up(IO_PORTA_05, 0);
        gpio_set_pull_down(IO_PORTA_05, 0);
    }
    u8 tmp = !gpio_read(IO_PORTA_05);
	return tmp;

    /* return !gpio_read(IO_PORTA_05); */
}

static const struct camera_platform_data camera1_data = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTH_10,
	.pwdn_gpio      = -1,//IO_PORTH_11
    .power_value    = 1,
	.interface      = SEN_INTERFACE1,
    .online_detect  = camera1_online_detect,
    .dvp = {
        .pclk_gpio  = IO_PORTH_00,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = DVP_SENSOR1(0),
        .data_gpio  = {
            -1,
            -1,
            IO_PORTH_08,
            IO_PORTH_07,
            IO_PORTH_06,
            IO_PORTH_05,
            IO_PORTH_04,
            IO_PORTH_03,
            IO_PORTH_02,
            IO_PORTH_01,
        },
    }
};

static const struct video_subdevice_data video1_subdev_data[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera1_data },
};
static const struct video_platform_data video1_data = {
    .data = video1_subdev_data,
    .num = ARRAY_SIZE(video1_subdev_data),
};
#endif


#ifdef CONFIG_VIDEO2_ENABLE

static bool camera2_online_detect()
{
    static u8 init = 0;

	return 1;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_10);
        gpio_set_pull_up(IO_PORTA_10, 0);
        gpio_set_pull_down(IO_PORTA_10, 0);
    }

    return !gpio_read(IO_PORTA_10);
}

static const struct camera_platform_data camera2_data = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTH_10,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = SEN_INTERFACE1,
    .online_detect  = camera2_online_detect,
    .dvp = {
        .pclk_gpio  = IO_PORTH_00,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = DVP_SENSOR1(0),
        .data_gpio  = {
            -1,
            -1,
            IO_PORTH_08,
            IO_PORTH_07,
            IO_PORTH_06,
            IO_PORTH_05,
            IO_PORTH_04,
            IO_PORTH_03,
            IO_PORTH_02,
            IO_PORTH_01,
        },
    }
};

static const struct video_subdevice_data video2_subdev_data[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera2_data },
};
static const struct video_platform_data video2_data = {
    .data = video2_subdev_data,
    .num = ARRAY_SIZE(video2_subdev_data),
};
#endif






#ifdef CONFIG_VIDEO4_ENABLE

static bool camera4_online_detect0()
{
    static u8 init = 0;

	return 1;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_10);
        gpio_set_pull_up(IO_PORTA_10, 0);
        gpio_set_pull_down(IO_PORTA_10, 0);
    }

    return !gpio_read(IO_PORTA_10);
}

static const struct camera_platform_data camera4_data0 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTA_06,//IO_PORTG_00,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect0,
    .dvp = {
        .pclk_gpio  = IO_PORTA_15,//IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,//DVP_SENSOR0(0),//-1,//DVP_SENSOR1(0),
        .data_gpio  = {
            -1,
            -1,
            IO_PORTA_07,
            IO_PORTA_08,
            IO_PORTA_09,
            IO_PORTA_10,
            IO_PORTA_11,
            IO_PORTA_12,
            IO_PORTA_13,
            IO_PORTA_14,
        },
    }
};

static const struct video_subdevice_data video4_subdev_data0[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera4_data0 },
};
static const struct video_platform_data video4_data0 = {
    .data = video4_subdev_data0,
    .num = ARRAY_SIZE(video4_subdev_data0),
};



static bool camera4_online_detect1()
{
    static u8 init = 0;

	return 1;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_10);
        gpio_set_pull_up(IO_PORTA_10, 0);
        gpio_set_pull_down(IO_PORTA_10, 0);
    }

    return !gpio_read(IO_PORTA_10);
}

static const struct camera_platform_data camera4_data1 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTA_06,//IO_PORTG_00,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect1,
    .dvp = {
        .pclk_gpio  = IO_PORTA_15,//IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,//DVP_SENSOR0(0),//-1,//DVP_SENSOR1(0),
        .data_gpio  = {
            -1,
            -1,
			IO_PORTA_07,
            IO_PORTA_08,
            IO_PORTA_09,
            IO_PORTA_10,
            IO_PORTA_11,
            IO_PORTA_12,
            IO_PORTA_13,
            IO_PORTA_14,
        },
    }
};



static const struct video_subdevice_data video4_subdev_data1[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera4_data1 },
};
static const struct video_platform_data video4_data1 = {
    .data = video4_subdev_data1,
    .num = ARRAY_SIZE(video4_subdev_data1),
};


static bool camera4_online_detect2()
{
    static u8 init = 0;

	return 1;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_10);
        gpio_set_pull_up(IO_PORTA_10, 0);
        gpio_set_pull_down(IO_PORTA_10, 0);
    }

    return !gpio_read(IO_PORTA_10);
}

static const struct camera_platform_data camera4_data2 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTA_06,//IO_PORTG_00,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect2,
    .dvp = {
        .pclk_gpio  = IO_PORTA_15,//IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,//DVP_SENSOR0(0),//-1,//DVP_SENSOR1(0),
        .data_gpio  = {
            -1,
            -1,
			IO_PORTA_07,
            IO_PORTA_08,
            IO_PORTA_09,
            IO_PORTA_10,
            IO_PORTA_11,
            IO_PORTA_12,
            IO_PORTA_13,
            IO_PORTA_14,
        },
    }
};


static const struct video_subdevice_data video4_subdev_data2[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera4_data2 },
};
static const struct video_platform_data video4_data2 = {
    .data = video4_subdev_data2,
    .num = ARRAY_SIZE(video4_subdev_data2),
};

static bool camera4_online_detect3()
{
    static u8 init = 0;

	return 1;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_10);
        gpio_set_pull_up(IO_PORTA_10, 0);
        gpio_set_pull_down(IO_PORTA_10, 0);
    }

    return !gpio_read(IO_PORTA_10);
}

static const struct camera_platform_data camera4_data3 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTA_06,//IO_PORTG_00,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect3,
    .dvp = {
        .pclk_gpio  = IO_PORTA_15,//IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,//DVP_SENSOR0(0),//-1,//DVP_SENSOR1(0),
        .data_gpio  = {
            -1,
            -1,
			IO_PORTA_07,
            IO_PORTA_08,
            IO_PORTA_09,
            IO_PORTA_10,
            IO_PORTA_11,
            IO_PORTA_12,
            IO_PORTA_13,
            IO_PORTA_14,
        },
    }
};


static const struct video_subdevice_data video4_subdev_data3[] = {
    { VIDEO_TAG_CAMERA, (void *)&camera4_data3 },
};
static const struct video_platform_data video4_data3 = {
    .data = video4_subdev_data3,
    .num = ARRAY_SIZE(video4_subdev_data3),
};


#endif





#ifdef CONFIG_VIDEO3_ENABLE


UVC_PLATFORM_DATA_BEGIN(uvc_data)
    .width = 1280,//1280,
    .height = 720,//480,
    .fps = 25,
    .mem_size = 1 * 1024 * 1024,
    .timeout = 3000,//ms
    .put_msg = 0,
UVC_PLATFORM_DATA_END()

static const struct video_subdevice_data video3_subdev_data[] = {
    { VIDEO_TAG_UVC, (void *)&uvc_data },
};
static const struct video_platform_data video3_data = {
    .data = video3_subdev_data,
    .num = ARRAY_SIZE(video3_subdev_data),
};

#endif

USB_PLATFORM_DATA_BEGIN(usb0_data)
    .id = 0,
    .online_check_cnt = 3,
    .offline_check_cnt = 20,//250
    .isr_priority = 6,
    .host_ot = 20,
    .host_speed = 1,
    .slave_ot = 10,
    .ctl_irq_int = HUSB0_CTL_INT,
USB_PLATFORM_DATA_END()

USB_PLATFORM_DATA_BEGIN(usb1_data)
    .id = 1,
    .online_check_cnt = 3,
    .offline_check_cnt = 20,//250
    .isr_priority = 6,
    .host_ot = 20,
    .host_speed = 1,
    .slave_ot = 10,
    .ctl_irq_int = HUSB1_CTL_INT,
USB_PLATFORM_DATA_END()



#ifdef CONFIG_ADKEY_ENABLE
/*-------------ADKEY GROUP 1----------------*/
/* #define ADC0_33   (0x3FF) */
/* #define ADC0_30   (0x3ff*30/33) //0x3A2 */
/* #define ADC0_27   (0x3ff*27/33) //0x345 */
/* #define ADC0_23   (0x3ff*23/33) //0x2C9 */
/* #define ADC0_20   (0x3ff*20/33) //0x26C */
/* #define ADC0_17   (0x3ff*17/33) //0x1F0 */
/* #define ADC0_13   (0x3ff*13/33) //0x193 */
/* #define ADC0_07   (0x3ff*07/33) //0x136 */
/* #define ADC0_03   (0x3ff*04/33) //0xD9 */
/* #define ADC0_02   (0x3ff*02/33) //0xD9 */
/* #define ADC0_01   (0x3ff*01/33) //0x7C */
/* #define ADC0_00   (0) */

/* #define ADKEY_V_0      	((ADC0_33 + ADC0_30)/2) */
/* #define ADKEY_V_1 		((ADC0_30 + ADC0_27)/2) */
/* #define ADKEY_V_2 		((ADC0_27 + ADC0_23)/2) */
/* #define ADKEY_V_3 		((ADC0_23 + ADC0_20)/2) */
/* #define ADKEY_V_4 		((ADC0_20 + ADC0_17)/2) */
/* #define ADKEY_V_5 		((ADC0_17 + ADC0_13)/2) */
/* #define ADKEY_V_6 		((ADC0_13 + ADC0_00)/2)//up 0.7 */
/* #define ADKEY_V_7 		((ADC0_03 + ADC0_03)/2)//down 0.4 */
/* #define ADKEY_V_8 		((ADC0_01 + ADC0_01)/2)//ok 0.2 */
/* #define ADKEY_V_9 		((ADC0_01 + ADC0_00)/2)//menu/mode 0.1 */
/* #define ADKEY_V_10 		(ADC0_00) */


/*-------------ADKEY GROUP 2----------------*/
#define ADC0_33   (0x3FF)
#define ADC0_30   (0x3ff*30/33) //0x3A2
#define ADC0_27   (0x3ff*27/33) //0x345
#define ADC0_23   (0x3ff*23/33) //0x2C9
#define ADC0_20   (0x3ff*20/33) //0x26C
#define ADC0_17   (0x3ff*17/33) //0x20F
#define ADC0_13   (0x3ff*13/33) //0x193
#define ADC0_09   (0x3ff*9/33) //0x117
#define ADC0_07   (0x3ff*07/33) //0xD9
#define ADC0_06   (0x3ff*06/33) //0xC6
#define ADC0_03   (0x3ff*04/33) //0x7C
#define ADC0_02   (0x3ff*02/33) //0x3E
#define ADC0_01   (0x3ff*01/33) //0x1F
#define ADC0_00   (0)

#define ADKEY_V_0       ((ADC0_33 + ADC0_30)/2)
#define ADKEY_V_1       ((ADC0_30 + ADC0_27)/2)
#define ADKEY_V_2       ((ADC0_27 + ADC0_23)/2)
#define ADKEY_V_3       ((ADC0_23 + ADC0_20)/2)
#define ADKEY_V_4       ((ADC0_20 + ADC0_17)/2)
#define ADKEY_V_5       ((ADC0_17 + ADC0_13)/2)
#define ADKEY_V_6       ((ADC0_13 + ADC0_09)/2)//up 0.7
#define ADKEY_V_7       ((ADC0_09 + ADC0_06)/2)//down 0.4
#define ADKEY_V_8       ((ADC0_06 + ADC0_03)/2)//ok 0.2
#define ADKEY_V_9       ((ADC0_03 + ADC0_01)/2)//menu/mode 0.1
#define ADKEY_V_10      (ADC0_00)



//五个按键：OK ,  MEN/MODE, POWER,  UP,  DOWN
ADKEY_PLATFORM_DATA_BEGIN(adkey_data)
	/* .io 		= IO_PORTB_14, */
	.io 		= IO_PORTE_03,
	.ad_channel = 5,
	.table 	= {
		.ad_value = {
			ADKEY_V_0,
			ADKEY_V_1,
			ADKEY_V_2,
			ADKEY_V_3,
			ADKEY_V_4,
			ADKEY_V_5,
			ADKEY_V_6,
			ADKEY_V_7,
			ADKEY_V_8,
			ADKEY_V_9,
			ADKEY_V_10,
		},
		.key_value = {
			NO_KEY,    /*0*/
			NO_KEY,
			NO_KEY,
			NO_KEY,
			NO_KEY,
			NO_KEY,  /*5*/
			NO_KEY,
			KEY_DOWN,
			KEY_OK,
			KEY_UP,/*KEY_MODE,*/
			KEY_MODE,
		},
	},
ADKEY_PLATFORM_DATA_END()
int key_event_remap(struct sys_event *e)
{
	static u8 m_flag = 0;

    if (e->u.key.value == KEY_MODE) {
	   	if(e->u.key.event == KEY_EVENT_HOLD) {
			return false;
		}

		/* if (!m_flag) */
		/* { */
   		 	if (e->u.key.event == KEY_EVENT_LONG) {
				m_flag = 1;
				e->u.key.value = KEY_MENU;
				e->u.key.event = KEY_EVENT_CLICK;
			}
		/* } */
	   /* else{ */
		/*    if (m_flag) */
		/*    { */
		/* 	   if (m_flag) */
		/* 		   m_flag = 0; */
       /*  */
		/* 	   e->u.key.value = KEY_MENU; */
		/*    	   e->u.key.event = KEY_EVENT_CLICK; */
       /*  */
		/* 		if (m_flag == 1) */
		/* 			m_flag = 2; */
		/*    } */
	   /* } */
    }
	return true;
}


#endif


#ifdef CONFIG_IOKEY_ENABLE
/*
 * power键
 */
const struct iokey_port iokey_list[] = {
    {
        .port = IO_PORTA_07,
        .press_value = 0,
        .key_value = KEY_POWER,
    }
};

const struct iokey_platform_data iokey_data = {
    .num = ARRAY_SIZE(iokey_list),
    .port = iokey_list,
};


unsigned char read_power_key()
{
	gpio_direction_input(IO_PORTA_07);
	return (gpio_read(IO_PORTA_07));
}
#else

unsigned char read_power_key()
{
    return 0;
}

#endif


/*
 * spi0接falsh
 */
SPI0_PLATFORM_DATA_BEGIN(spi0_data)//ok
	/* .clk    = 1000000, */
	.clk    = 20000000,
    /* .clk    = 1000000, */
    /* .mode   = SPI_QUAD_MODE, */
    .mode   = SPI_DUAL_MODE,
    /* .mode   = SPI_ODD_MODE,//SPI_DUAL_MODE, */
    .port   = 'A',
SPI0_PLATFORM_DATA_END()

SPI1_PLATFORM_DATA_BEGIN(spi1_data)
    .clk    = 1000000,
	.mode   = SPI_DUAL_MODE,//SPI_DUAL_MODE,
	/* .mode   = SPI_ODD_MODE,//SPI_DUAL_MODE, */
/* .port   = 'A', */
	.port   = 'B',//
	/* .port   = 'C',// */
SPI1_PLATFORM_DATA_END()

SPI2_PLATFORM_DATA_BEGIN(spi2_data)//ok
    .clk    = 1000000,
	.mode   = SPI_QUAD_MODE,//SPI_DUAL_MODE,
    /* .mode   = SPI_ODD_MODE,//SPI_DUAL_MODE, */
	/* .port   = 'A', */
	.port   = 'B',
SPI2_PLATFORM_DATA_END()


const struct spiflash_platform_data spiflash_data = {
     .name           = "spi0",
	/* .name           = "spi1", */
    /* .name           = "spi2", */
    .mode           = FAST_READ_OUTPUT_MODE,//FAST_READ_IO_MODE,
	.sfc_run_mode   = SFC_FAST_READ_DUAL_OUTPUT_MODE,
};



const struct dac_platform_data dac_data = {
    .ldo_id = 1,
	.pa_mute_port = 0xff,
	.pa_mute_value = 0,
	.differ_output = 1,
};

const struct adc_platform_data adc_data = {
    .mic_channel = LADC_CH_MIC_R,
	.linein_channel = LADC_LINE0_MASK,
	.ldo_sel = 1,
};

const struct iis_platform_data iis_data = {
	.channel_in = BIT(0),
	/* .channel_in = BIT(1), */
	/* .channel_in = BIT(2), */
	/* .channel_in = BIT(3), */
	/* .channel_in = 0, */
	.channel_out = 0,
	/* .channel_out = BIT(0), */
	/* .channel_out = BIT(1), */
	/* .channel_out = BIT(2), */
	/* .channel_out = BIT(3), */
	/* .port_sel = IIS_PORTA, */
	.port_sel = IIS_PORTG,
	/* .data_width = BIT(0),//24bit模式， 读取32bit数据 */
	.data_width = 0,
	.mclk_output = 0,
	.slave_mode = 0,
};

const struct plnk_platform_data plnk_data = {
	.plnk_clk_io = IO_PORTB_00,
	.plnk_d0_io  = IO_PORTB_01,
	.plnk_d1_io  = IO_PORTE_01,
	.init = plnk_iomc_init,
};
const struct audio_pf_data audio_pf_d = {
	.adc_pf_data = &adc_data,
	.dac_pf_data = &dac_data,
	.iis_pf_data = &iis_data,
	.plnk_pf_data = &plnk_data,
};
const struct audio_platform_data audio_data = {
	.private_data = (void *)&audio_pf_d,
};

USB_CAMERA_PLATFORM_DATA_BEGIN(usb_camera0_data)
    .open_log = 1,
USB_CAMERA_PLATFORM_DATA_END()

USB_CAMERA_PLATFORM_DATA_BEGIN(usb_camera1_data)
    .open_log = 1,
USB_CAMERA_PLATFORM_DATA_END()

#ifdef CONFIG_GSENSOR_ENABLE
extern const struct device_operations gsensor_dev_ops;
#endif // CONFIG_GSENSOR_ENABLE


#ifdef CONFIG_AV10_SPI_ENABLE
extern const struct device_operations _spi_dev_ops;
//以下io为临时配置，还需根据原理图来调整
SW_SPI_PLATFORM_DATA_BEGIN(sw_spi_data)
	.pin_cs = IO_PORTG_15,
	.pin_clk = IO_PORTB_00,
	.pin_in  = IO_PORTB_01,
	.pin_out = IO_PORTB_01,
SW_SPI_PLATFORM_DATA_END()
#endif // CONFIG_AV10_SPI_ENABLE


#if 1
#include "eth/eth_phy.h"
#include "eth/ethmac.h"



NET_PLATFORM_DATA_BEGIN(net_phy_data)
    .name = (u8 *)"rtl8201f",
    .speed = PHY_FULLDUPLEX_100M,
    .mode  = RMII_MODE,
    .irq   = 1,  //-1为查询  0-7 使用中断的优先级
    .check_link_time = 100, //100ms  // 检测网络连接状态时间间隔
    .rmii_bus = {
    .phy_addr = 0x1,
    .clk_pin = IO_PORTB_04,
    .dat_pin = IO_PORTB_03,
    },
NET_PLATFORM_DATA_END()
#endif


#if defined CONFIG_IR_ENABLE
void ir_on() // 红外灯开
{
    gpio_direction_output(IO_PORT_PR_03,1);
}

void ir_off() //红外灯关
{
    gpio_direction_output(IO_PORT_PR_03,0);

}

void ir_cut_on()//绿光片 开
{

    gpio_direction_output(IO_PORTB_00,1);
    gpio_direction_output(IO_PORTB_01,0);


}

void ir_cut_off()//绿光片 关
{

    gpio_direction_output(IO_PORTB_00,0);
    gpio_direction_output(IO_PORTB_01,1);

}

#endif





void av_parking_det_init()
{
    /* gpio_direction_input(IO_PORTA_06); */
}

unsigned char av_parking_det_status()
{
    return -1;
    /* return (!gpio_read(IO_PORTA_06)); */
}
unsigned char PWR_CTL(unsigned char on_off)
{
    return 0;
}

#define USB_WKUP_IO     -1
#define GSEN_WKUP_IO 	-1
unsigned char usb_is_charging()
{
#if 0
	static unsigned char init = 0;
	if (!init){
		init = 1;
		gpio_direction_input(USB_WKUP_IO);
		gpio_set_pull_up(USB_WKUP_IO, 0);
		gpio_set_pull_down(USB_WKUP_IO, 0);
		gpio_set_die(USB_WKUP_IO, 1);
		delay(10);
	}

	return (gpio_read(USB_WKUP_IO));//no usb charing == false
#else
	return 1;
#endif
}

unsigned int get_usb_wkup_gpio()
{
	return (USB_WKUP_IO);
}

POWER_PLATFORM_DATA_BEGIN(sys_power_data)
    .wkup_map = {
        {"wkup_gsen", WKUP_IO_PR1, 0},
        {"wkup_usb", WKUP_IO_PR3, 0},
        {0, 0, 0}
    },
    .voltage_table = {
        {365, 10},
        {370, 20},
        {373, 30},
        {376, 40},
        {379, 50},
        {387, 60},
        {390, 70},
        {397, 80},
        {408, 90},
        {422, 100},
    },
    .min_bat_power_val = 350,
    .max_bat_power_val = 420,
    .charger_online = usb_is_charging,
    .charger_gpio  = get_usb_wkup_gpio,
    .read_power_key = read_power_key,
    .pwr_ctl = PWR_CTL,
POWER_PLATFORM_DATA_END()


#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
PWM_PLATFORM_DATA_BEGIN(pwm_data)
	.port = PWM_PORTG,
	/* .port = PWM_PORTH, */
PWM_PLATFORM_DATA_END()
#endif



#if 1

#include "step_motor.h"

TIMER_MOTO_PORT_BEGIN(timer_moto1)
    .PORT_A = IO_PORTH_01,
    .PORT_B = IO_PORTH_00,
    .PORT_C = IO_PORTE_05,
    .PORT_D = IO_PORTE_04,

    /* .PORT_A = IO_PORTE_04, */
    /* .PORT_B = IO_PORTE_05, */
    /* .PORT_C = IO_PORTH_00, */
    /* .PORT_D = IO_PORTH_01, */
    .PORT_CHECK_A = IO_PORTA_06,
TIMER_MOTO_PORT_END()
TIMER_MOTO_PORT_BEGIN(timer_moto2)
/*     .PORT_A = IO_PORTH_02, */
    /* .PORT_B = IO_PORTH_03, */
    /* .PORT_C = IO_PORTH_04, */
    /* .PORT_D = IO_PORTH_05, */
    .PORT_A = IO_PORTH_05,
    .PORT_B = IO_PORTH_04,
    .PORT_C = IO_PORTH_03,
    .PORT_D = IO_PORTH_02,


    .PORT_CHECK_A = -1,
TIMER_MOTO_PORT_END()
#endif




REGISTER_DEVICES(device_table) = {

#ifdef CONFIG_PAP_ENABLE
    { "pap",   &pap_dev_ops, NULL},
#endif
	{ "imr",   &imr_dev_ops, NULL},
	{ "imd",   &imd_dev_ops, NULL},
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    { "pwm",   &pwm_dev_ops, (void *)&pwm_data},
#endif
	/* { "lcd",   &lcd_dev_ops, (void*)&lcd_data}, */

	/* { "iic0",  &iic_dev_ops, (void *)&hw_iic1_data }, */
    { "iic1",  &iic_dev_ops, (void *)&sw_iic0_data},
    { "iic0",  &iic_dev_ops, (void *)&sw_iic1_data },
    { "iic3",  &iic_dev_ops, (void *)&sw_iic2_data},

	{ "audio", &audio_dev_ops, (void *)&audio_data },

#ifdef CONFIG_TOUCH_PANEL_ENABLE
    { "touch_panel", &touch_panel_dev_ops, (void *)&touch_panel_data},
#endif //CONFIG_TOUCH_PANEL_ENABLE

#ifdef CONFIG_AV10_SPI_ENABLE
    { "avin_spi",  &_spi_dev_ops, (void *)&sw_spi_data },
#endif

#ifdef CONFIG_SD0_ENABLE
    { "sd0",  &sd_dev_ops, (void *)&sd0_data },
#endif

#ifdef CONFIG_SD1_ENABLE
    { "sd1",  &sd_dev_ops, (void *)&sd1_data },
#endif

#ifdef CONFIG_SD2_ENABLE
    { "sd2",  &sd_dev_ops, (void *)&sd2_data },
#endif

#ifdef CONFIG_ADKEY_ENABLE
    /* { "adkey", &key_dev_ops, (void *)&adkey_data }, */
#endif
#ifdef CONFIG_IOKEY_ENABLE
    { "iokey", &key_dev_ops, (void *)&iokey_data },
#endif
    /* { "uart_key", &key_dev_ops, NULL }, */

#ifdef CONFIG_VIDEO0_ENABLE
    { "video0.*",  &video_dev_ops, (void *)&video0_data},
#endif

#ifdef CONFIG_VIDEO1_ENABLE
   { "video1.*",  &video_dev_ops, (void *)&video1_data },
#endif

#ifdef CONFIG_VIDEO2_ENABLE
   { "video2.*",  &video_dev_ops, (void *)&video2_data },
#endif

#ifdef CONFIG_VIDEO_DEC_ENABLE
	{ "video_dec",  &video_dev_ops, NULL },
#endif

#ifdef CONFIG_VIDEO3_ENABLE
	{ "video3.*",  &video_dev_ops, (void *)&video3_data },
#endif

#ifdef CONFIG_VIDEO4_ENABLE
   { "video4.0.*",  &video_dev_ops, (void *)&video4_data0 },
   { "video4.1.*",  &video_dev_ops, (void *)&video4_data1 },
   { "video4.2.*",  &video_dev_ops, (void *)&video4_data2 },
   { "video4.3.*",  &video_dev_ops, (void *)&video4_data3 },
   /* { "video4.*",  &video_dev_ops, (void *)&video0_data }, */
#endif

    { "fb0",  &fb_dev_ops, NULL },
    { "fb1",  &fb_dev_ops, NULL },
    { "fb2",  &fb_dev_ops, NULL },
    /* { "fb5",  &fb_dev_ops, NULL }, */

	{ "videoengine",  &video_engine_ops, NULL },
#ifndef CONFIG_SFC_ENABLE
   { "spi0", &spi_dev_ops, (void *)&spi0_data },
   { "spiflash", &spiflash_dev_ops, (void *)&spiflash_data },
#else
   { "spiflash", &sfcflash_dev_ops, (void *)&spiflash_data },
#endif

#ifdef CONFIG_ETH_ENABLE
    { "eth0",  &eth_phy_dev_ops, (void *) &net_phy_data},
#endif

//	{ "usb_cam0",  &usb_cam_dev_ops, (void *)&usb_camera0_data },
//	{ "usb_cam1",  &usb_cam_dev_ops, (void *)&usb_camera1_data },

#ifdef CONFIG_GSENSOR_ENABLE
	{"gsensor", &gsensor_dev_ops, NULL},
#endif

    {"rtc", &rtc_dev_ops, NULL},


#ifdef CONFIG_WIFI_ENABLE
        { "wifi",  &wifi_dev_ops, (void *) &wifi_data},
#endif


#ifdef CONFIG_LTE_ENABLE
    { "lte0",  &usbwifi_dev_ops, (void *) &usb0_data},
#endif



 //   {"vm",  &vm_dev_ops, NULL},
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    {"pwm", &pwm_dev_ops, (void *)&pwm_data},
#endif
   {"vm",  &vm_dev_ops, NULL},
   /*  {"pwm", &pwm_dev_ops, (void *)&pwm_data}, */
	/* {"tpwm", &pwm_dev_ops, NULL}, */
	/* {"pwm8", &pwm_dev_ops, NULL}, */
    {"uvc", &uvc_dev_ops, NULL},
    {"usb0", &usb_dev_ops, (void *)&usb0_data},
    {"usb1", &usb_dev_ops, (void *)&usb1_data},
};

// *INDENT-ON*


#ifdef CONFIG_DEBUG_ENABLE

void debug_uart_init()
{

    uart_init(&uart3_data);
    /*     if (uart0_data.tx_pin == IO_PORTB_03) { */
    /* IOMC1 &= ~(0x0f << 8); */
    /* gpio_direction_output(IO_PORTB_03, 0); */
    /* gpio_set_pull_up(IO_PORTB_03, 1); */
    /* gpio_set_pull_down(IO_PORTB_03, 1); */
    /* gpio_set_die(IO_PORTB_03, 0); */
    /* } */
    /* IOMC3 &= ~BIT(25);//解除串口uart3占用io PB3 and PB4 */
}
#endif

#define ISP_XCLK_MAPOUT()   \
    do{ \
        CLK_CON0 &=~(BIT(10)|BIT(11)|BIT(12)|BIT(13)); \
        CLK_CON0 |= 0x09 << 10 ;\
    } while(0)

void board_init()
{
//   CLK_CON0 &=  ~BIT(13);//uboot某测试时钟io pe2输出关断
    /* EVA_CON |= BIT(0); */
    /* delay(10); */
    /* EVA_CON |= BIT(1); */
    /* delay(10); */
    /* ISP_XCLK_MAPOUT(); */
    //gpio_direction_output(IO_PORT_12, 0);

    gpio_direction_output(IO_PORTH_12, 1);


    gpio_direction_output(IO_PORTB_05, 1);


    // sd power off
    sdmmc_power(0);


//    mipi_phy_con0 &= ~BIT(23);//增加这一句 关闭mipi ldo

    puts("board_init\n");
    devices_init();
    puts("board_int: over\n");

    // sd power on
    sdmmc_power(1);

}

#endif


