
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
#include "asm/ldo.h"
#include "device/dec_stream.h"
// *INDENT-OFF*

#ifdef CONFIG_BOARD_DEV_5701C_20191024


UART0_PLATFORM_DATA_BEGIN(uart0_data)
	.baudrate = 460800,//115200,
	 //.tx_pin = IO_PORTA_07,.rx_pin = IO_PORTA_08,
	 /* .tx_pin = IO_PORTG_06,.rx_pin = IO_PORTG_07, */
	  .tx_pin = IO_PORTH_06,
	  /* .rx_pin = IO_PORTH_13, */
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
	 .tx_pin = IO_PORTB_04,
	 /* .rx_pin = IO_PORTB_03, */
  	.flags = UART_DEBUG,
UART3_PLATFORM_DATA_END();


#ifdef CONFIG_SD0_ENABLE

int sdmmc_0_io_detect(const struct sdmmc_platform_data *data)
{
    static u8 init = 0;

    return 1;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTB_11);
        gpio_set_pull_up(IO_PORTB_11, 1);
        gpio_set_pull_down(IO_PORTB_11, 0);
    }

    return !gpio_read(IO_PORTB_11);

}


SD0_PLATFORM_DATA_BEGIN(sd0_data)
	.port 					= 'A',
	/* .port 					= 'B', */
	/* .port 					= 'C', */
	/* .port 					= 'D',*/
	.priority 				= 3,
	.data_width 			= 4,
	/* .data_width 			= 1, */
	.speed 					= 40000000,
	/* .detect_mode 			= SD_CMD_DECT, */
	/* .detect_func 			= sdmmc_0_clk_detect, */
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_0_io_detect,
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
	.speed 					= 40000000,
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_1_io_detect,
    /*.power                  = sdmmc_power,*/
SD1_PLATFORM_DATA_END()

#endif //CONFIG_SD1_ENABLE

#ifdef CONFIG_SD2_ENABLE

int sdmmc_2_io_detect(const struct sdmmc_platform_data *data)
{
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTG_14);
        gpio_set_pull_up(IO_PORTG_14, 1);
        gpio_set_pull_down(IO_PORTG_14, 0);
    }

    return !gpio_read(IO_PORTG_14);

}

static void sdmmc_power(int on)
{
    gpio_direction_output(IO_PORTG_15, !on);
}


SD2_PLATFORM_DATA_BEGIN(sd2_data)
	.port 					= 'B',
	.priority 				= 3,
	.data_width 			= 4,
	.speed 					= 40000000,
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_2_io_detect,//sdmmc_2_clk_detect,
	.power                  = sdmmc_power,
SD2_PLATFORM_DATA_END()

#endif //CONFIG_SD2_ENABLE

HW_IIC0_PLATFORM_DATA_BEGIN(hw_iic0_data)
	/* .clk_pin = IO_PORTG_06,// */
	/* .dat_pin = IO_PORTG_07, */

   /*  .clk_pin = IO_PORTH_12,// */
	/* .dat_pin = IO_PORTH_14, */

	.clk_pin = IO_PORTB_04,
	.dat_pin = IO_PORTB_03,

	/* .clk_pin = IO_PORTA_05,*/
	/* .dat_pin = IO_PORTA_06,*/

    .baudrate = 0x42,//300k  0x50 250k
	/* .baudrate = 0x1f,//300k  0x50 250k */
HW_IIC1_PLATFORM_DATA_END()

HW_IIC1_PLATFORM_DATA_BEGIN(hw_iic1_data)
    .clk_pin = IO_PORTB_00,
    .dat_pin = IO_PORTB_01,

	/* .clk_pin = IO_PORTA_02,// */
	/* .dat_pin = IO_PORTA_01, */

	/* .clk_pin = IO_PORTG_00,// */
	/* .dat_pin = IO_PORTG_01, */

	/* .clk_pin = IO_PORTE_04, */
	/* .dat_pin = IO_PORTE_05, */

	/* .baudrate = 0x50,//300k  0x50 250k */
	.baudrate = 0xff,//300k  0x50 250k
HW_IIC1_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic_data)
	/* .clk_pin = IO_PORTB_00, */
	/* .dat_pin = IO_PORTB_01, */

	/* .clk_pin = IO_PORTB_14, */
	/* .dat_pin = IO_PORTB_15, */

	.clk_pin = IO_PORTB_04,
	.dat_pin = IO_PORTB_03,

	/* .clk_pin = IO_PORTE_04, */
	/* .dat_pin = IO_PORTE_05, */

	.sw_iic_delay = 200,
SW_IIC_PLATFORM_DATA_END()


SW_IIC_PLATFORM_DATA_BEGIN(sw_iic1_data)
	/* .clk_pin = IO_PORTB_00, */
	/* .dat_pin = IO_PORTB_01, */

	/* .clk_pin = IO_PORTB_14, */
	/* .dat_pin = IO_PORTB_15, */

	.clk_pin = IO_PORTE_04,
	.dat_pin = IO_PORTE_05,

	/* .sw_iic_delay = 100, */
	.sw_iic_delay = 10,
SW_IIC_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic2_data)
	.clk_pin = IO_PORTH_11,
	.dat_pin = IO_PORTH_12,
	.sw_iic_delay = 50,
SW_IIC_PLATFORM_DATA_END()

LCD_PLATFORM_DATA_BEGIN(lcd_data)
    .lcd_name = "ek79030_v2",
	.lcd_io = {
        .backlight = -1,
        .backlight_value = 1,

		.lcd_reset   = IO_PORTG_00,
		.lcd_standby = -1,//IO_PORTG_01,
        .lcd_cs      = -1,
        .lcd_rs      = -1,
        .lcd_spi_ck  = -1,
        .lcd_spi_di  = -1,
        .lcd_spi_do  = -1,
	}
/* LCD_PLATFORM_DATA_ADD() */
    /* .lcd_name = "LX50FWB4001_V2", */
	/* .interface = LCD_DVP_RGB, */
	/* .lcd_io = { */
        /* .backlight = IO_PORTG_02, */
        /* .backlight_value = 1, */

		/* .lcd_reset = IO_PORTG_04, */
        /* .lcd_cs    = -1, */
        /* .lcd_rs    = -1, */
        /* .lcd_spi_ck= -1, */
        /* .lcd_spi_di= -1, */
        /* .lcd_spi_do= -1, */
	/* } */
LCD_PLATFORM_DATA_END()



#ifdef CONFIG_TOUCH_PANEL_ENABLE
extern const struct device_operations touch_panel_dev_ops;
SW_TOUCH_PANEL_PLATFORM_DATA_BEGIN(touch_panel_data)
    .enable         = 1,
    .iic_dev        = "iic2",
    .rst_pin        = IO_PORTH_10,
    .int_pin        = IO_PORT_PR_03,
    ._MAX_POINT     = 1,
    ._MAX_X         = LCD_DEV_WIDTH,
    ._MAX_Y         = LCD_DEV_HIGHT,
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
#if 1
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTE_03,
	.pwdn_gpio      = IO_PORTG_15,
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
#else
    .interface      = SEN_INTERFACE0,
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTH_07,
	.pwdn_gpio      = IO_PORTH_08,
    .dvp = {
        .pclk_gpio  = IO_PORTA_15,
        .hsync_gpio = IO_PORTE_00,
        .vsync_gpio = IO_PORTE_01,
        .io_function_sel = DVP_SENSOR0(1),
        .data_gpio  = {
          -1, // IO_PORTA_05,
          -1, // IO_PORTA_06,
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
#endif
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

    return !gpio_read(IO_PORTA_05);
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

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTG_05);
        gpio_set_pull_up(IO_PORTG_05, 0);
        gpio_set_pull_down(IO_PORTG_05, 0);
    }

    /* log_d("%s %d   io:%d\n",__func__,__LINE__,!gpio_read(IO_PORTG_05)); */
    return !gpio_read(IO_PORTG_05);
}

static const struct camera_platform_data camera4_data0 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTE_02,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect0,
    .dvp = {
        .pclk_gpio  = IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,
        .data_gpio  = {
            -1,
            -1,
            IO_PORTB_06,
            IO_PORTB_07,
            IO_PORTB_08,
            IO_PORTB_09,
            IO_PORTB_10,
            IO_PORTB_11,
            IO_PORTB_12,
            IO_PORTB_13,
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

    if (!init) {
        gpio_direction_input(IO_PORTG_04);
        gpio_set_pull_up(IO_PORTG_04, 0);
        gpio_set_pull_down(IO_PORTG_04, 0);
    }

    return !gpio_read(IO_PORTG_04);
}

static const struct camera_platform_data camera4_data1 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTE_02,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect1,
    .dvp = {
        .pclk_gpio  = IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,
        .data_gpio  = {
            -1,
            -1,
            IO_PORTB_06,
            IO_PORTB_07,
            IO_PORTB_08,
            IO_PORTB_09,
            IO_PORTB_10,
            IO_PORTB_11,
            IO_PORTB_12,
            IO_PORTB_13,
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

    if (!init) {
        gpio_direction_input(IO_PORTG_03);
        gpio_set_pull_up(IO_PORTG_03, 0);
        gpio_set_pull_down(IO_PORTG_03, 0);
    }

    return !gpio_read(IO_PORTG_03);
}

static const struct camera_platform_data camera4_data2 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTE_02,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect2,
    .dvp = {
        .pclk_gpio  = IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,
        .data_gpio  = {
            -1,
            -1,
            IO_PORTB_06,
            IO_PORTB_07,
            IO_PORTB_08,
            IO_PORTB_09,
            IO_PORTB_10,
            IO_PORTB_11,
            IO_PORTB_12,
            IO_PORTB_13,
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

    if (!init) {
        gpio_direction_input(IO_PORTG_02);
        gpio_set_pull_up(IO_PORTG_02, 0);
        gpio_set_pull_down(IO_PORTG_02, 0);
    }

    return !gpio_read(IO_PORTG_02);
}

static const struct camera_platform_data camera4_data3 = {
	.xclk_gpio      = -1,
	.reset_gpio     = IO_PORTE_02,
	.pwdn_gpio      = -1,
    .power_value    = 1,
	.interface      = -1,
    .online_detect  = camera4_online_detect3,
    .dvp = {
        .pclk_gpio  = IO_PORTB_05,
        .hsync_gpio = -1,
        .vsync_gpio = -1,
		.io_function_sel = -1,
        .data_gpio  = {
            -1,
            -1,
            IO_PORTB_06,
            IO_PORTB_07,
            IO_PORTB_08,
            IO_PORTB_09,
            IO_PORTB_10,
            IO_PORTB_11,
            IO_PORTB_12,
            IO_PORTB_13,
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
#define ADC0_33   (0x3FF)
#define ADC0_30   (0x3ff*30/33) //0x3A2
#define ADC0_27   (0x3ff*27/33) //0x345
#define ADC0_23   (0x3ff*23/33) //0x2C9
#define ADC0_20   (0x3ff*20/33) //0x26C
#define ADC0_17   (0x3ff*17/33) //0x20F
#define ADC0_12   (0x3ff*12/33) //0x193
#define ADC0_09   (0x3ff*9/33) //0x117
#define ADC0_07   (0x3ff*07/33) //0xD9
#define ADC0_06   (0x3ff*06/33) //0xC6
#define ADC0_03   (0x3ff*04/33) //0x7C
#define ADC0_02   (0x3ff*02/33) //0x3E
#define ADC0_01   (0x3ff*01/33) //0x1F
#define ADC0_00   (0)

//五个按键：OK ,  MEN/MODE, POWER,  UP,  DOWN
// 0, 0.398, 0.785, 1.038, 1.352,
ADKEY_PLATFORM_DATA_BEGIN(adkey_data)
	.io 		= IO_PORTH_14,
	.ad_channel = 10,
	.table 	= {
		.ad_value = {
			ADC0_33,
			ADC0_30,
			ADC0_27,
			ADC0_23,
			ADC0_20,
			ADC0_17,
			ADC0_12,
			ADC0_09,
			ADC0_06,
			ADC0_02,
			ADC0_00,
		},
		.key_value = {
			NO_KEY,    /*0*/
			NO_KEY,
			NO_KEY,
			NO_KEY,
			NO_KEY,
			NO_KEY,  /*5*/
			KEY_OK,
			KEY_DOWN,
			KEY_UP,
			KEY_MODE,
			KEY_MENU,
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
#define IO_POWER_KEY     IO_PORTG_01
const struct iokey_port iokey_list[] = {
    {
        .port = IO_POWER_KEY,
        .press_value = 1,
        .key_value = KEY_POWER,
    }
};

const struct iokey_platform_data iokey_data = {
    .num = ARRAY_SIZE(iokey_list),
    .port = iokey_list,
};


unsigned char read_power_key()
{
	gpio_direction_input(IO_POWER_KEY);
	return (gpio_read(IO_POWER_KEY));
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



void av_parking_det_init()
{
    /* gpio_direction_input(IO_PORTA_06); */
}

unsigned char av_parking_det_status()
{
	return 0;
    /* return (!gpio_read(IO_PORTA_06)); */
}
unsigned char PWR_CTL(unsigned char on_off)
{
    return 0;
}

#define USB_WKUP_IO 	IO_PORT_PR_01
#define GSEN_WKUP_IO 	IO_PORT_PR_02
unsigned char usb_is_charging()
{
#if 1
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

REGISTER_DEVICES(device_table) = {

#ifdef CONFIG_PAP_ENABLE
    { "pap",   &pap_dev_ops, NULL},
#endif
	{ "imr",   &imr_dev_ops, NULL},
	{ "imd",   &imd_dev_ops, NULL},
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    { "pwm",   &pwm_dev_ops, (void *)&pwm_data},
#endif
	{ "lcd",   &lcd_dev_ops, (void*)&lcd_data},

    /* { "iic4",  &iic_dev_ops, (void *)&hw_iic0_data }, */
	/* { "iic0",  &iic_dev_ops, (void *)&hw_iic1_data }, */
	{ "iic0",  &iic_dev_ops, (void *)&sw_iic1_data },
	{ "iic2",  &iic_dev_ops, (void *)&sw_iic2_data },
	/* { "iic3",  &iic_dev_ops, (void *)&sw_iic1_data }, */
    { "iic4",  &iic_dev_ops, (void *)&sw_iic_data },
	/* { "iic2",  &iic_dev_ops, (void *)&sw_iic_data }, */

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
    { "adkey", &key_dev_ops, (void *)&adkey_data },
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
#endif

    { "fb0",  &fb_dev_ops, NULL },
    { "fb1",  &fb_dev_ops, NULL },
	{ "fb2",  &fb_dev_ops, NULL },
    /* { "fb5",  &fb_dev_ops, NULL }, */

	{ "video_dec_stream",  &vdec_stream_ops, NULL },
	{ "audio_dec_stream",  &adec_stream_ops, NULL },
	{ "videoengine",  &video_engine_ops, NULL },
#ifndef CONFIG_SFC_ENABLE
   { "spi0", &spi_dev_ops, (void *)&spi0_data },
   { "spiflash", &spiflash_dev_ops, (void *)&spiflash_data },
#else
   /* { "spiflash", &sfcflash_dev_ops, (void *)&spiflash_data }, */
#endif

#ifdef CONFIG_ETH_PHY_ENABLE
    /* { "net-phy",  &eth_phy_dev_ops, (void *) &net_phy_data}, */
#endif

//	{ "usb_cam0",  &usb_cam_dev_ops, (void *)&usb_camera0_data },
//	{ "usb_cam1",  &usb_cam_dev_ops, (void *)&usb_camera1_data },

#ifdef CONFIG_GSENSOR_ENABLE
	{"gsensor", &gsensor_dev_ops, NULL},
#endif

    {"rtc", &rtc_dev_ops, NULL},
	{"vm",  &vm_dev_ops, NULL},
	/* {"tpwm", &pwm_dev_ops, NULL}, */
	/* {"pwm8", &pwm_dev_ops, NULL}, */
    {"uvc", &uvc_dev_ops, NULL},
    {"usb0", &usb_dev_ops, (void *)&usb0_data},
    {"usb1", &usb_dev_ops, (void *)&usb1_data},
#ifdef CONFIG_USE_UDISK_0
    {"udisk0", &mass_storage_ops, NULL},
#endif
};

// *INDENT-ON*

#ifdef CONFIG_DEBUG_ENABLE

void debug_uart_init()
{
    uart_init(&uart0_data);
    IOMC1 &= ~(0x0f << 8);
    IOMC1 |= (0x05 << 8);
    gpio_direction_output(uart0_data.tx_pin, 0);
    gpio_set_pull_up(uart0_data.tx_pin, 1);
    gpio_set_pull_down(uart0_data.tx_pin, 1);
    gpio_set_die(uart0_data.tx_pin, 0);
}

#endif


void board_init()
{
//		avin_power
    /* gpio_direction_output(IO_PORTH_11, 1); */

    //avdd18_ctrl
    /* avdd18_crtl(AVDD18_18, true); */

    // sd power off
    sdmmc_power(0);

    puts("board_init\n");
    devices_init();
    puts("board_int: over\n");

    // sd power on
    sdmmc_power(1);

    /* while(1){ */
    /* gpio_direction_output(IO_PORTE_04,1); */
    /* gpio_direction_output(IO_PORTE_05,1); */
    /* delay(10000); */
    /* gpio_direction_output(IO_PORTE_04,0); */
    /* gpio_direction_output(IO_PORTE_05,0); */
    /* delay(10000); */
    /* WDT_CON |= BIT(6); */
    /* } */

}

#endif


