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
#ifdef CONFIG_NET_ENABLE
#include "device/wifi_dev.h"
#include "eth/eth_phy.h"
#include "eth/ethmac.h"
#endif

#if  defined CONFIG_BOARD_DEV_WIFI_5701_20240811
extern int get_video1_state();
extern int get_video2_state();
// 配置传感器类型
// #define CONFIG_SENSOR0  0       // 传感器 0 的类型配置
//                                 // 0: 使用 MIPI 接口
//                                 // 1: 使用 DVP 接口
//                                 // 2: 使用 9930 芯片（特定型号的传感器）

// #define CONFIG_SENSOR1  0       // 传感器 1 的类型配置
//                                 // 0: 使用 DEP0 传感器
//                                 // 1: 使用 DVP1 传感器



UART0_PLATFORM_DATA_BEGIN(uart0_data)
	.baudrate = 9600,//115200,
	/* .baudrate = 115200, */
	.port = PORTG_6_7,
	.output_channel = OUTPUT_CHANNEL2,
	.tx_pin = IO_PORTG_06,
	// .flags = UART_DEBUG,
     .max_continue_recv_cnt = 1024, 
	 .idle_sys_clk_cnt = 500000, 
	 .clk_src = LSB_CLK, 
UART0_PLATFORM_DATA_END();

/* UART0_PLATFORM_DATA_BEGIN(s_uart_data) */
	/* .baudrate = 460800,//115200, */
	/* [> .baudrate = 115200, <] */
	 /* //.tx_pin = IO_PORTA_07,.rx_pin = IO_PORTA_08, */
	 /* [> .tx_pin = IO_PORTG_06,.rx_pin = IO_PORTG_07, <] */
	  /* [> .tx_pin = IO_PORTH_12,.rx_pin = IO_PORTH_13, <] */
	 /* [> .tx_pin = IO_PORTB_14,.rx_pin = IO_PORTB_15, <] */
	/* .port = PORT_REMAP, */
	/* .output_channel = 2, */
	/* .input_channel = 3, */
	/* .tx_pin = IO_PORTE_02, */
	/* .tx_pin = IO_PORTE_03, */
	/* .max_continue_recv_cnt = 1024, */
	/* .idle_sys_clk_cnt = 500000, */
	/* .clk_src = LSB_CLK, */
/* UART0_PLATFORM_DATA_END(); */

UART1_PLATFORM_DATA_BEGIN(uart1_data)
	.baudrate = 1000000,                    // 设置波特率为460800
	//.port = PORTH_2_5,                   // 端口配置，已注释
	//.tx_pin = IO_PORTH_14,               // 发送引脚配置，已注释
	//.rx_pin = IO_PORTH_05,               // 接收引脚配置，已注释
	//.max_continue_recv_cnt = 1024,       // 最大连续接收计数，已注释
	//.idle_sys_clk_cnt = 500000,          // 空闲系统时钟计数，已注释
	//.flags = UART_DEBUG,                 // UART调试标志，已注释
	//.clk_src = LSB_CLK,                  // 时钟源配置，已注释
	//.port = PORT_REMAP,                   // 端口映射配置
	.output_channel = OUTPUT_CHANNEL1,    // 输出通道配置为通道3
	.tx_pin = IO_PORTH_14,                // 配置发送引脚为PORTH_14
	.flags = UART_DEBUG,                  // 设置UART调试标志
UART1_PLATFORM_DATA_END();               // 结束UART1平台数据配置


// UART2_PLATFORM_DATA_BEGIN(uart2_data)
// 	/* .baudrate = 115200, */
// 	.baudrate = 460800,//115200,
// 	 .tx_pin = IO_PORTH_10,.rx_pin = IO_PORTH_11,
// 	 /* .tx_pin = IO_PORTH_00,.rx_pin = IO_PORTH_01, */
//   	.flags = UART_DEBUG,
// UART2_PLATFORM_DATA_END();

// UART3_PLATFORM_DATA_BEGIN(uart3_data)
// 	.baudrate = 460800,//115200,
// 	/* .baudrate = 115200, */
// 	 /* .tx_pin = IO_PORTE_00,.rx_pin = IO_PORTE_01, */
// 	 /* .tx_pin = IO_PORTB_04, */
// 	 /* .rx_pin = IO_PORTB_03, */
// 	.port = PORT_REMAP,
// #ifdef CONFIG_BOARD_DEV_5711_20190809
// 	// ddr时钟输出时用output_channel0，
// 	// 此时串口改为output_channel1
// 	.output_channel = OUTPUT_CHANNEL1,
// #else
// 	.output_channel = OUTPUT_CHANNEL0,
// #endif
// 	.tx_pin = IO_PORTH_06,
//   	.flags = UART_DEBUG,
// UART3_PLATFORM_DATA_END();


#ifdef CONFIG_SD0_ENABLE

int sdmmc_0_io_detect(const struct sdmmc_platform_data *data)
{
#define SD_DET_IO IO_PORT_PR_03
    static u8 init = 0;

#ifdef CONFIG_FAST_CAPTURE
	static u8 flag = 0;
	if(flag < 50){
		flag ++;
		return 0;
	}
#endif

    if (!init) {
        init = 1;
        gpio_direction_input(SD_DET_IO);
        gpio_set_pull_up(SD_DET_IO, 1);
        gpio_set_pull_down(SD_DET_IO, 0);
    }

    return !gpio_read(SD_DET_IO);

}


SD0_PLATFORM_DATA_BEGIN(sd0_data)
	/* .port 					= 'A', */
	/* .port 					= 'B', */
	.port 					= 'C',
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
        gpio_direction_input(IO_PORTB_11);
        gpio_set_pull_up(IO_PORTB_11, 1);
        gpio_set_pull_down(IO_PORTB_11, 0);
    }

    return !gpio_read(IO_PORTB_11);

}

static void sdmmc_power(int on)
{
    gpio_direction_output(IO_PORTB_14, !on);
}

SD1_PLATFORM_DATA_BEGIN(sd1_data)
	/* .port 					= 'A', */
	.port 					= 'B',
	.priority 				= 3,
	.data_width 			= 4,
	.speed 					= 30000000,
	.detect_mode 			= SD_IO_DECT,
	.detect_func 			= sdmmc_1_io_detect,
    .power                  = sdmmc_power,
SD1_PLATFORM_DATA_END()

#endif //CONFIG_SD1_ENABLE

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

HW_IIC0_PLATFORM_DATA_BEGIN(hw_iic0_data)//camera iic VIDEEO1_IIC后拉
	/* IO_PORTG_06, IO_PORTG_07,  */
	/* IO_PORTH_12, IO_PORTH_14,  */
	/* IO_PORTB_04, IO_PORTB_03,  */
	/* IO_PORTA_05, IO_PORTA_06,  */
#if (CONFIG_SENSOR0 == 0)
    .clk_pin = IO_PORTB_04,
	.dat_pin = IO_PORTB_03,
#elif (CONFIG_SENSOR0 == 2)
	.clk_pin = IO_PORTB_04,
	.dat_pin = IO_PORTB_03,
#endif
	.baudrate = 0x7f,//300k  0x50 250k
HW_IIC0_PLATFORM_DATA_END()

HW_IIC1_PLATFORM_DATA_BEGIN(hw_iic1_data)//g-sensor iic//TP iic
	/* IO_PORTB_00, IO_PORTB_01,  */
	/* IO_PORTA_02, IO_PORTA_01,  */
	/* IO_PORTG_00, IO_PORTG_01,  */
	/* IO_PORTE_04, IO_PORTE_05,  */
    .clk_pin = IO_PORTB_00,
    .dat_pin = IO_PORTB_01,
	.baudrate = 0x7f,//300k  0x50 250k
HW_IIC1_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic0_data)
#if(CONFIG_SENSOR1 == 0)
	.clk_pin = -1,//IO_PORTB_00,
	.dat_pin = -1,//IO_PORTB_01,
	.sw_iic_delay = 100,
#elif(CONFIG_SENSOR1 == 1)
	// .clk_pin = IO_PORTH_12,
	// .dat_pin = IO_PORTH_14,
	// .sw_iic_delay = 100,
#endif
SW_IIC_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic1_data)
	.clk_pin = -1,//IO_PORTB_00,
	.dat_pin = -1,//O_PORTB_01,
	.sw_iic_delay = 100,
SW_IIC_PLATFORM_DATA_END()

SW_IIC_PLATFORM_DATA_BEGIN(sw_iic2_data)
	.clk_pin = -1,//IO_PORTA_05,
	.dat_pin = -1,//IO_PORTA_06,
	.sw_iic_delay = 100,
SW_IIC_PLATFORM_DATA_END()


LCD_PLATFORM_DATA_BEGIN(lcd_data)
    .lcd_name = "ek79030_v2",  // LCD 的名称，标识 LCD 型号为 "ek79030_v2"

    .lcd_io = {  // LCD 的 IO 配置
        .backlight = IO_PORTG_01,              // 背光控制引脚，-1 表示未使用
        .backlight_value = 1,          // 背光控制的默认值，1 表示打开背光

        .lcd_reset   = IO_PORTG_00,   // LCD 复位引脚，连接到端口 E 的第 3 引脚
        .lcd_standby = -1,             // LCD 待机控制引脚，-1 表示未使用
        .lcd_cs      = -1,             // LCD 片选引脚，-1 表示未使用
        .lcd_rs      = -1,             // LCD 命令/数据选择引脚，-1 表示未使用
        .lcd_spi_ck  = -1,             // LCD SPI 时钟引脚，-1 表示未使用
        .lcd_spi_di  = -1,             // LCD SPI 数据输入引脚，-1 表示未使用
        .lcd_spi_do  = -1,             // LCD SPI 数据输出引脚，-1 表示未使用
    },

    .lcd_port.mipi_mapping = {  // LCD 使用的 MIPI 接口映射配置
        .x0_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D0,
        .x1_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D1,
        .x2_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D2,
        .x4_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D3,
    },

    .lcd_port.lvds_mapping = {  // LCD 使用的 LVDS 接口映射配置
        .x0_lane = LVDS_LANE_D0,  // LVDS 通道 x0 映射到 D0 信号
        .x1_lane = LVDS_LANE_D1,  // LVDS 通道 x1 映射到 D1 信号
        .x2_lane = LVDS_LANE_D2,  // LVDS 通道 x2 映射到 D2 信号
        .x3_lane = LVDS_LANE_CLK, // LVDS 通道 x3 映射到时钟信号
        .x4_lane = LVDS_LANE_D3,  // LVDS 通道 x4 映射到 D3 信号
        .swap_dp_dn = false,     // 是否交换数据通道的正负极，false 表示不交换
    },
LCD_PLATFORM_DATA_ADD()
    .lcd_name = "lcd_avout",  // LCD 的名称，标识 LCD 型号为 "lcd_avout"
LCD_PLATFORM_DATA_END()




#ifdef CONFIG_TOUCH_PANEL_ENABLE
extern const struct device_operations touch_panel_dev_ops;
SW_TOUCH_PANEL_PLATFORM_DATA_BEGIN(touch_panel_data)
    .enable         = 1,
    .iic_dev        = "iic2",
    .rst_pin        = IO_PORTE_00,
    .int_pin        = IO_PORTE_01,
    ._MAX_POINT     = 1,
    ._MAX_X         = 1280,
    ._MAX_Y         = 800,
    ._INT_TRIGGER   = 3,
    ._X2Y_EN        = 1,
    ._X_MIRRORING   = 1,
    ._Y_MIRRORING   = 1,
    ._DEBUGP        = 0,
    ._DEBUGE        = 0,
    .points         ={
        .point_num  = 0,
    }
SW_TOUCH_PANEL_PLATFORM_DATA_END()
#endif //CONFIG_TOUCH_PANEL_ENABLE


#ifdef CONFIG_VIDEO0_ENABLE

static const struct camera_platform_data camera0_data = {
#if (CONFIG_SENSOR0 == 0)
    .xclk_gpio      = -1,
	.reset_gpio     = -1,//IO_PORTH_11,
	.pwdn_gpio      = -1,//IO_PORTE_02,
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
#elif (CONFIG_SENSOR0 == 1)
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTH_07,
	.pwdn_gpio      = IO_PORTH_08,
	.interface      = SEN_INTERFACE0,
	.dvp = {
		.pclk_gpio  = IO_PORTA_15,
		.hsync_gpio = IO_PORTE_00,
		.vsync_gpio = IO_PORTE_01,
		.io_function_sel = DVP_SENSOR0(1),
		.data_gpio  = {
			/* IO_PORTA_05, */
			/* IO_PORTA_06, */
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
// #if(CONFIG_SENSOR1 == 0)
// 	return 1;
// #elif(CONFIG_SENSOR1 == 1)
// #define CAMERA1_DET IO_PORT_PR_03
//     static u8 init = 0;

//     if (!init) {
//         init = 1;
//         gpio_direction_input(CAMERA1_DET);
//         gpio_set_pull_up(CAMERA1_DET, 0);
//         gpio_set_pull_down(CAMERA1_DET, 0);
//     }
//     u8 tmp = !gpio_read(CAMERA1_DET);
// 	return tmp;
// #endif

//     /* return !gpio_read(IO_PORTA_05); */

	return get_video1_state();//返回摄像头是否在线
}

static const struct camera_platform_data camera1_data = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTB_13,
	.pwdn_gpio      = -1,
    .power_value    = 1,  // 摄像头供电控制值，1表示上电
	.interface      = SEN_INTERFACE0,
    .online_detect  = camera1_online_detect,  // 在线检测函数配置为 `camera1_online_detect`
	.dvp = {
		.pclk_gpio  = IO_PORTA_15,
		.hsync_gpio = -1,
		.vsync_gpio = -1,
		.io_function_sel = DVP_SENSOR0(1),
		.data_gpio  = {
			/* IO_PORTA_05, */
			/* IO_PORTA_06, */
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
    // static u8 init = 0;

	// return 1;

    // if (!init) {
    //     init = 1;
    //     gpio_direction_input(IO_PORTA_10);
    //     gpio_set_pull_up(IO_PORTA_10, 0);
    //     gpio_set_pull_down(IO_PORTA_10, 0);
    // }

    return get_video2_state();
}

static const struct camera_platform_data camera2_data = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTB_13,
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
	return 1;
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_14);
        gpio_set_pull_up(IO_PORTA_14, 0);
        gpio_set_pull_down(IO_PORTA_14, 0);
    }

    return !gpio_read(IO_PORTA_14);
}

static const struct camera_platform_data camera4_data0 = {
    .xclk_gpio      = -1,
	.reset_gpio     = IO_PORTB_15,
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
            IO_PORTB_06,
            IO_PORTB_07,
            IO_PORTB_08,
            IO_PORTB_09,
            IO_PORTB_10,
            IO_PORTB_11,
            IO_PORTB_12,
            IO_PORTB_13,
            -1,
            -1,
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
	return 1;
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_14);
        gpio_set_pull_up(IO_PORTA_14, 0);
        gpio_set_pull_down(IO_PORTA_14, 0);
    }

    return !gpio_read(IO_PORTA_14);
}

static const struct camera_platform_data camera4_data1 = {
	.xclk_gpio      = -1,
	.reset_gpio     = IO_PORTB_15,
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
	return 1;
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_14);
        gpio_set_pull_up(IO_PORTA_14, 0);
        gpio_set_pull_down(IO_PORTA_10, 0);
    }

    return !gpio_read(IO_PORTA_14);
}

static const struct camera_platform_data camera4_data2 = {
	.xclk_gpio      = -1,
	.reset_gpio     = IO_PORTB_15,
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
	return 1;
    static u8 init = 0;

    if (!init) {
        init = 1;
        gpio_direction_input(IO_PORTA_14);
        gpio_set_pull_up(IO_PORTA_14, 0);
        gpio_set_pull_down(IO_PORTA_14, 0);
    }

    return !gpio_read(IO_PORTA_14);
}

static const struct camera_platform_data camera4_data3 = {
	.xclk_gpio      = -1,
	.reset_gpio     = IO_PORTB_15,
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

UVC_PLATFORM_DATA_BEGIN(uvc1_data)
    .width = 1280,//1280,
    .height = 720,//480,
    .fps = 20,
    .mem_size = 1 * 1024 * 1024,
    .timeout = 3000,//ms
    .put_msg = 0,
UVC_PLATFORM_DATA_END()


UVC_PLATFORM_DATA_BEGIN(uvc2_data)
    .width = 1280,//1280,
    .height = 720,//480,
    .fps = 20,
    .mem_size = 1 * 1024 * 1024,
    .timeout = 3000,//ms
    .put_msg = 0,
UVC_PLATFORM_DATA_END()

UVC_PLATFORM_DATA_BEGIN(uvc3_data)
    .width = 1280,//1280,
    .height = 720,//480,
    .fps = 20,
    .mem_size = 1 * 1024 * 1024,
    .timeout = 3000,//ms
    .put_msg = 0,
UVC_PLATFORM_DATA_END()

static const struct video_subdevice_data video3_subdev_data[] = {
    { VIDEO_TAG_UVC, (void *)&uvc_data },
    { VIDEO_TAG_UVC, (void *)&uvc1_data },
    { VIDEO_TAG_UVC, (void *)&uvc2_data },
    { VIDEO_TAG_UVC, (void *)&uvc3_data },
};
static const struct video_platform_data video3_data = {
    .data = video3_subdev_data,
    .num = ARRAY_SIZE(video3_subdev_data),
};

#endif

USB_PLATFORM_DATA_BEGIN(usb0_data)
    .id = 0,                      // USB0 设备 ID
    .online_check_cnt = 3,        // 在线检测计数
    .offline_check_cnt = 20,      // 离线检测计数，默认值为250
    .isr_priority = 6,            // 中断服务优先级
    .host_ot = 20,                // 主机超时时间
    .host_speed = 0,              // 主机速度配置，0表示全速或低速
    .slave_ot = 10,               // 从机超时时间
    .ctl_irq_int = HUSB0_CTL_INT, // 控制中断号
USB_PLATFORM_DATA_END()

USB_PLATFORM_DATA_BEGIN(usb1_data)
    .id = 1,                      // USB1 设备 ID
    .online_check_cnt = 1,        // 在线检测计数
    .offline_check_cnt = 20,      // 离线检测计数，默认值为250
    .isr_priority = 6,            // 中断服务优先级
    .host_ot = 20,                // 主机超时时间
    .host_speed = 1,              // 主机速度配置，1表示高速
    .slave_ot = 10,               // 从机超时时间
    .ctl_irq_int = HUSB1_CTL_INT, // 控制中断号
USB_PLATFORM_DATA_END()




#ifdef CONFIG_ADKEY_ENABLE
/*-------------ADKEY GROUP 2----------------*/
// 定义ADC键值范围
#define ADC0_33   (0x3FF)               // 1023，最大值，对应按键未按下时的电压
#define ADC0_30   (0x3ff*30/33) //0x3A2  // 930，约等于3.3V的电压值
#define ADC0_27   (0x3ff*27/33) //0x345  // 837，约等于3.0V的电压值
#define ADC0_26   (0x3ff*26/33) //0x345  // 837，约等于3.0V的电压值
#define ADC0_25   (0x3ff*25/33) //0x345  // 837，约等于3.0V的电压值
#define ADC0_23   (0x3ff*23/33) //0x2C9  // 713，约等于2.3V的电压值
#define ADC0_22   (0x3ff*22/33) //0x2C9  // 713，约等于2.3V的电压值
#define ADC0_20   (0x3ff*20/33) //0x26C  // 620，约等于2.0V的电压值
#define ADC0_19   (0x3ff*19/33) //0x20F  // 527，约等于1.7V的电压值
#define ADC0_17   (0x3ff*17/33) //0x20F  // 527，约等于1.7V的电压值
#define ADC0_14   (0x3ff*14/33) //0x193  // 403，约等于1.3V的电压值
#define ADC0_13   (0x3ff*13/33) //0x193  // 403，约等于1.3V的电压值
#define ADC0_11   (0x3ff*11/33) //0x193  // 403，约等于1.3V的电压值
#define ADC0_09   (0x3ff*9/33)  //0x117  // 279，约等于0.9V的电压值
#define ADC0_07   (0x3ff*07/33) //0xD9   // 217，约等于0.7V的电压值
#define ADC0_06   (0x3ff*06/33) //0xC6   // 186，约等于0.6V的电压值
#define ADC0_03   (0x3ff*04/33) //0x7C   // 124，约等于0.3V的电压值
#define ADC0_02   (0x3ff*02/33) //0x3E   // 62，约等于0.2V的电压值
#define ADC0_01   (0x3ff*01/33) //0x1F   // 31，约等于0.1V的电压值
#define ADC0_00   (0)                    // 0，最小值，对应按键完全按下时的电压


// 五个按键：OK , MEN/MODE, POWER, UP, DOWN
ADKEY_PLATFORM_DATA_BEGIN(adkey_data)
	 .io 		= IO_PORTE_03,             // IO端口：指定按键输入的IO口
	 .ad_channel = AD_CH05_PE03,            // ADC通道：指定用于读取按键的ADC通道
	.table 	= {
		.ad_value = {                    //
			ADC0_33,                   //
			ADC0_30,                   //
			ADC0_27,                   //
			ADC0_25,                   //
			ADC0_22,                   //
			ADC0_19,                   //
			ADC0_17,                   //
			ADC0_14,                   //
			ADC0_11,                   //
			ADC0_07,                   //
			ADC0_00,                   //
		},
		.key_value = {                  //
			NO_KEY,    /*0*/            //
			NO_KEY,                     //
			NO_KEY,                    //
			NO_KEY,                     //
			KEY_6,                     // LED
			KEY_5,                     // 调焦-
			KEY_4,                     // 上
			KEY_3,                   // 右
			KEY_2,                     // 调焦+
			KEY_1,                   // 拍照键
			NO_KEY,                   //
		},
	},
ADKEY_PLATFORM_DATA_END()

// 五个按键：OK , MEN/MODE, POWER, UP, DOWN
ADKEY_PLATFORM_DATA_BEGIN(adkey_data1)

    .io 		= IO_PORTA_05,             // IO端口：指定按键输入的IO口
	.ad_channel = AD_CH11_PA05,            // ADC通道：指定用于读取按键的ADC通道
	.table 	= {
		.ad_value = {                    // ADC值与对应按键的映射表
			ADC0_33,                   //
			ADC0_30,                   //
			ADC0_27,                   //
			ADC0_26,                   //
			ADC0_22,                   //
			ADC0_19,                   //
			ADC0_17,                   //
			ADC0_14,                   //
			ADC0_11,                   //
			ADC0_07,                   //
			ADC0_00,                   //
		},
		.key_value = {                  //
			NO_KEY,    /*0*/            //
			NO_KEY,                     //
			NO_KEY,                    //
			NO_KEY,                     //
			KEY_OK,                     //下
			KEY_11,                     //左
			KEY_10,                     //缩放
			KEY_9,                   //旋转
			KEY_8,                     //返回
			KEY_7,                   // 菜单
			NO_KEY,                   //
		},
	},
ADKEY_PLATFORM_DATA_END()

int key_event_remap(struct sys_event *e)
{
    static u8 m_flag = 0;  // 定义一个静态变量 m_flag，初始值为 0，用于跟踪模式状态

    if (e->u.key.value == KEY_MODE) {  // 如果接收到的按键值是 KEY_MODE
        if(e->u.key.event == KEY_EVENT_HOLD) {  // 如果按键事件是按住（HOLD）
            return false;  // 不做任何处理，直接返回 false
        }

        /* if (!m_flag) */
        /* { */
            if (e->u.key.event == KEY_EVENT_LONG) {  // 如果按键事件是长按（LONG）
                m_flag = 1;  // 设置 m_flag 为 1，表示进入菜单模式
                e->u.key.value = KEY_MENU;  // 将按键值重映射为 KEY_MENU
                e->u.key.event = KEY_EVENT_CLICK;  // 将按键事件重映射为单击（CLICK）
            }
        /* } */
        /* else{ */
        /*    if (m_flag) */
        /*    { */
        /*        if (m_flag) */
        /*            m_flag = 0; */
        /* */
        /*        e->u.key.value = KEY_MENU; */
        /*        e->u.key.event = KEY_EVENT_CLICK; */
        /* */
        /*        if (m_flag == 1) */
        /*            m_flag = 2; */
        /*    } */
        /* } */
    }
    return true;  // 默认返回 true，表示事件已处理
}



#endif


#ifdef CONFIG_IOKEY_ENABLE
/*
 * power键
 */
// 定义一个iokey_port结构体数组，用于配置按键端口
const struct iokey_port iokey_list[] = {
    {
        .port = IO_PORTA_06,   // 按键所在的端口 (IO_PORTA_06)
        .press_value = 1,      // 按键按下时的值（例如1表示按下）
        .key_value = KEY_POWER, // 对应的按键功能，这里为电源键(KEY_POWER)
    }
};

// 定义iokey_platform_data结构体，用于描述按键平台相关数据
const struct iokey_platform_data iokey_data = {
    .num = ARRAY_SIZE(iokey_list), // 按键数量（通过数组大小计算）
    .port = iokey_list,            // 按键端口列表
};

// 读取电源键的状态
unsigned char read_power_key()
{
    // 将IO_PORTA_06设置为输入模式
    gpio_direction_input(IO_PORTA_06);

    // 读取IO_PORTA_06的电平值并返回（返回值可能是0或1）
    return gpio_read(IO_PORTA_06);
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

#ifdef CONFIG_BOARD_DEV_5711_20190809   
	// 5711使用的flash与5701不同，如果mode一样
	// 会导致ui资源无法打开，打印----show err
	.mode   = SPI_DUAL_MODE,
#else
	.mode   = SPI_QUAD_MODE,
#endif
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


const struct spiflash_platform_data extflash_data = {
     .name           = "spi0",
	/* .name           = "spi1", */
    /* .name           = "spi2", */
    .mode           = FAST_READ_OUTPUT_MODE,//FAST_READ_IO_MODE,
	.sfc_run_mode   = SFC_FAST_READ_DUAL_OUTPUT_MODE,
    .ext_addr = 3 * 1024 * 1024,
    .ext_space = 1 * 1024 * 1024,

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
	.plnk_clk_io = -1,//IO_PORTB_00,
	.plnk_d0_io  = -1,//IO_PORTB_01,
	.plnk_d1_io  = -1,//IO_PORTE_01,
	.init = plnk_iomc_init,
};
const struct audio_pf_data audio_pf_d = {
	.adc_pf_data = &adc_data,
	.dac_pf_data = &dac_data,
	/* .iis_pf_data = &iis_data, */
	/* .plnk_pf_data = &plnk_data, */
};
const struct audio_platform_data audio_data = {
	.private_data = (void *)&audio_pf_d,
};

// USB_CAMERA_PLATFORM_DATA_BEGIN(usb_camera0_data)
//     .open_log = 1,  // 设置打开日志标志位。该参数用于控制是否在相机启动时记录日志信息。值为1表示开启日志记录，值为0表示关闭日志记录。
// USB_CAMERA_PLATFORM_DATA_END()


// USB_CAMERA_PLATFORM_DATA_BEGIN(usb_camera1_data)
//     .open_log = 1,
// USB_CAMERA_PLATFORM_DATA_END()

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
   // gpio_direction_input(IO_PORTA_06);
}

unsigned char av_parking_det_status()
{
	return 0;
    //return (!gpio_read(IO_PORTA_06));
}
unsigned char PWR_CTL(unsigned char on_off)
{
    return 0;
}

#define USB_WKUP_IO 	IO_PORT_PR_01
#define GSEN_WKUP_IO 	-1//IO_PORT_PR_02
unsigned char usb_is_charging()
{
#if 1
	static unsigned char init = 0;
	if (!init){
		init = 1;
		gpio_direction_input(USB_WKUP_IO);//将引脚配置为输入模式，用于检测外部信号
		gpio_set_pull_up(USB_WKUP_IO, 0);//关闭引脚的上拉电阻
		gpio_set_pull_down(USB_WKUP_IO, 0);//关闭引脚的下拉电阻
		gpio_set_die(USB_WKUP_IO, 1);//设置引脚的数字输入特性，使其能正确读取外部信号
		delay(10);
	}

	return (gpio_read(USB_WKUP_IO));//no usb charing == false
    // return 1;
#else
	return 1;
#endif
}

unsigned char camera_is_charging()//AHD2上线判断
{

	static unsigned char init = 0;
	if (!init){
		init = 1;
		gpio_direction_input(IO_PORTH_10);//将引脚配置为输入模式，用于检测外部信号
		gpio_set_pull_up(IO_PORTH_10, 0);//关闭引脚的上拉电阻
		gpio_set_pull_down(IO_PORTH_10, 0);//关闭引脚的下拉电阻
		gpio_set_die(IO_PORTH_10, 1);//设置引脚的数字输入特性，使其能正确读取外部信号
		delay(10);
	}

	return (gpio_read(IO_PORTH_10));//no usb charing == false
}


unsigned char avout_is_charging()//avout上线判断
{

	static unsigned char init = 0;
	if (!init){
		init = 1;
		gpio_direction_input(IO_PORTH_11);//将引脚配置为输入模式，用于检测外部信号
		gpio_set_pull_up(IO_PORTH_11, 0);//关闭引脚的上拉电阻
		gpio_set_pull_down(IO_PORTH_11, 0);//关闭引脚的下拉电阻
		gpio_set_die(IO_PORTH_11, 1);//设置引脚的数字输入特性，使其能正确读取外部信号
		delay(10);
	}

	return (gpio_read(IO_PORTH_11));//no usb charing == false
}



unsigned int get_usb_wkup_gpio()
{
	return (USB_WKUP_IO);
}



POWER_PLATFORM_DATA_BEGIN(sys_power_data)
    .wkup_map = {
        // {"wkup_gsen", WKUP_IO_PR2, 0},
        {"wkup_usb", WKUP_IO_PR1, 0},
        {0, 0, 0}
    },
    // .voltage_table = {
    //     {365, 10},
    //     {370, 20},
    //     {373, 30},
    //     {376, 40},
    //     {379, 50},
    //     {387, 60},
    //     {390, 70},
    //     {397, 80},
    //     {408, 90},
    //     {422, 100},
    // },
    // .min_bat_power_val = 350,
    // .max_bat_power_val = 420,
    .voltage_table = {
        {730, 10},
        {755, 20},  //6.5v
        {782, 30},
        {813, 40},  //6.95v
        {842, 50},
        {874, 60}, //7.45v
        {901, 70},
        {929, 80}, //7.925v
        {960, 90},
        {988, 100},//8.4v
    },
    .min_bat_power_val = 732,
    .max_bat_power_val = 986,
    .charger_online = usb_is_charging,
    .charger_gpio  = get_usb_wkup_gpio,
    .read_power_key = read_power_key,
    .pwr_ctl = PWR_CTL,
POWER_PLATFORM_DATA_END()


#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
PWM_PLATFORM_DATA_BEGIN(pwm_data)
	.port = PWM_PORTG,  //"A"
	 .port = PWM_PORTH, //"B"
PWM_PLATFORM_DATA_END()
#endif


#ifdef CONFIG_WIFI_ENABLE
WIFI_PLATFORM_DATA_BEGIN(wifi_data)
    .module = RTL8189F,
    .sdio_parm = SDIO_GRP_2 | SDIO_PORT_1 | SDIO_4_BIT_DATA |/*SDIO_DATA1_IRQ |*/25 * 1000000,
//    .sdio_parm = SDIO_GRP_1 | SDIO_PORT_1 | SDIO_4_BIT_DATA |/*SDIO_DATA1_IRQ |*/SDIO_CLOCK_8M,
    .wakeup_port = -1,//IO_PORTB_11,
    .cs_port = IO_PORTG_05,
    .power_port = IO_PORTG_04,//IO_PORTG_07,
WIFI_PLATFORM_DATA_END()
#endif

#ifdef CONFIG_ETH_ENABLE

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





REGISTER_DEVICES(device_table) = {

#ifdef CONFIG_PAP_ENABLE
    { "pap",   &pap_dev_ops, NULL},
#endif
	{ "imr",   &imr_dev_ops, NULL},
	{ "imd",   &imd_dev_ops, NULL},
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    { "pwm",   &pwm_dev_ops, (void *)&pwm_data},
#endif

#ifdef CONFIG_DISPLAY_ENABLE
	{ "lcd",   &lcd_dev_ops, (void*)&lcd_data},
#endif

// #if (CONFIG_SENSOR0 == 0)
	{ "iic1",  &iic_dev_ops, (void *)&hw_iic0_data },
// #elif (CONFIG_SENSOR0 == 1)
// 	{ "iic0",  &iic_dev_ops, (void *)&hw_iic1_data },
// #endif
// #ifndef CONFIG_FAST_CAPTURE
	// { "iic1",  &iic_dev_ops, (void *)&sw_iic0_data},
	// { "iic2",  &iic_dev_ops, (void *)&sw_iic2_data },
	// { "iic3",  &iic_dev_ops, (void *)&sw_iic1_data},
// #endif
// #if (CONFIG_SENSOR0 == 2)
// 	{ "iic4",  &iic_dev_ops, (void *)&hw_iic0_data },
// #endif

	{ "audio", &audio_dev_ops, (void *)&audio_data },

#ifdef CONFIG_TOUCH_PANEL_ENABLE
    { "iic2",  &iic_dev_ops, (void *)&hw_iic1_data}, // 如果启用了触摸屏，注册IIC2设备
    { "touch_panel", &touch_panel_dev_ops, (void *)&touch_panel_data}, // 注册触摸屏设备
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
    { "adkey1", &key_dev_ops, (void *)&adkey_data1 },
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

#ifndef CONFIG_FAST_CAPTURE
	{ "videoengine",  &video_engine_ops, NULL },
#endif
#ifndef CONFIG_SFC_ENABLE
   { "spi0", &spi_dev_ops, (void *)&spi0_data },   // 如果未启用SFC（Serial Flash Controller），则初始化SPI0设备。 "spi0" 是设备名称，&spi_dev_ops 是 SPI 设备的操作函数指针，spi0_data 是传递给设备的配置信息。
   { "spiflash", &spiflash_dev_ops, (void *)&spiflash_data }, // 同样，如果未启用SFC，这里初始化 SPI Flash 设备。 "spiflash" 是设备名称，&spiflash_dev_ops 是 SPI Flash 设备的操作函数指针，spiflash_data 是传递给设备的配置信息。

#else
   { "spi0", &spi_dev_ops, (void *)&spi0_data },
   { "spiflash", &sfcflash_dev_ops, (void *)&spiflash_data },
#endif

   { "extflash", &extflash_dev_ops, (void *)&extflash_data },

	// { "usb_cam0",  &usb_cam_dev_ops, (void *)&usb_camera0_data },
	// { "usb_cam1",  &usb_cam_dev_ops, (void *)&usb_camera1_data },

#ifdef CONFIG_GSENSOR_ENABLE
	{"gsensor", &gsensor_dev_ops, NULL},
#endif

    {"rtc", &rtc_dev_ops, NULL},
	{"vm",  &vm_dev_ops, NULL},
	/* {"tpwm", &pwm_dev_ops, NULL}, */
	/* {"pwm8", &pwm_dev_ops, NULL}, */
//#ifndef CONFIG_FAST_CAPTURE
    {"uvc", &uvc_dev_ops, NULL},
    // {"usb0", &usb_dev_ops, (void *)&usb0_data},
    // {"usb1", &usb_dev_ops, (void *)&usb1_data},
//#endif
#ifdef CONFIG_USE_UDISK_0
    {"udisk0", &mass_storage_ops, NULL},
#endif
#ifdef CONFIG_WIFI_ENABLE
    { "wifi",  &wifi_dev_ops, (void *) &wifi_data},
#endif

#ifdef CONFIG_ETH_ENABLE
    { "eth0",  &eth_phy_dev_ops, (void *) &net_phy_data},
#endif

#ifdef CONFIG_LTE_ENABLE
    { "lte0",  &usbwifi_dev_ops, (void *) &usb0_data},
#endif

	{"uart1", &uart_dev_ops, (void *)&uart1_data},
    {"uart0", &uart_dev_ops, (void *)&uart0_data},
};

// *INDENT-ON*


#ifdef CONFIG_DEBUG_ENABLE
void debug_uart_init()
{
    uart_init(&uart1_data);
}

// void pro_uart_init()
// {
//     uart_init(&uart0_data);
// }

#endif

#define ISP_XCLK_MAPOUT()   \
    do { \
        CLK_CON0 &= ~(BIT(10) | BIT(11) | BIT(12) | BIT(13)); /* 清除位10到13的值 */ \
        CLK_CON0 |= (0x09 << 10); /* 将值0x09映射到位10到13 */ \
    } while(0)


// #define PWM_PRD 0x300

// void pwm_duty_cycle(u8 per)
// {
//     u32 val;

//      val = (PWM_PRD * (100 - per)) / 100;
//     //val = PWM_PRD*per/100;

//     // 设置PWM的高电平时间
//     T3_PWM = val;

//     // 打印当前的占空比，用于调试
//     printf("====pwm_duty_cycle:%d\n", per);
// }

//  int pwm_backlight_init()
// {

//     IOMC1 &= ~(BIT(12)|BIT(11)|BIT(10)|BIT(9)|BIT(8));//0
//     // IOMC1 |= BIT(26)|BIT(25); //1
//     IOMC1 |= BIT(11)|BIT(8);

//   gpio_direction_output(IO_PORTE_05, 1);
//   gpio_set_pull_up(IO_PORTE_05, 1);
//   gpio_set_pull_down(IO_PORTE_05, 1);
//   gpio_set_die(IO_PORTE_05, 1);

//   T0_CNT = 0;
//   T0_CON &= ~(BIT(5)|BIT(4));
//   T0_CON |=  BIT(4);
//   T0_PRD = PWM_PRD;
//   T0_PWM = 0;
//   T0_CON |= BIT(0);
//    //pwm_duty_cycle(50); //0.79V.
//       //pwm_duty_cycle(10);

//   //pwm_duty_cycle(0.005); //1.21V

//   printf("pwm_backlight_initwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");

// }









void board_init()
{

#ifdef CONFIG_BOARD_DEV_5711_20190809
    // 5711 ddr时钟“pll3”输出配置
#if 0
    // PLL2 / 5 / 8
    PLL_CON1 &= ~(0b111 << 11);
    PLL_CON1 |= (3 << 11);
    PLL_CON1 &= ~(0b1111 << 14);
    PLL_CON1 |= (0b1110 << 14);
#else
    // PLL3 / 5 / 4
    PLL_CON1 &= ~(0b111 << 11);
    PLL_CON1 |= (5 << 11);
    PLL_CON1 &= ~(0b1111 << 14);
    PLL_CON1 |= (0b1010 << 14);
#endif

    CLK_CON0 &= ~(0xf << 10);
    CLK_CON0 |= 0b101 << 10;

    IOMC1 &= ~(0x1f << 8);
    IOMC1 |= 0b00000 << 8;
    // IOMC1 |= 3 << 8;	// pll24

    // PH12 sel output ch0
    PORTA_PU  |=  BIT(15);
    PORTA_PD  |=  BIT(15);
    PORTA_DIR &= ~BIT(15);
    PORTA_OUT &= ~BIT(15);
    PORTA_DIE &= ~BIT(15);

#endif


//   CLK_CON0 &=  ~BIT(13);//uboot某测试时钟io pe2输出关断
    /* EVA_CON |= BIT(0); */
    /* delay(10); */
    /* EVA_CON |= BIT(1); */
    /* delay(10); */
    /* ISP_XCLK_MAPOUT(); */
    //gpio_direction_output(IO_PORT_12, 0);

//		avin_power
    // gpio_direction_output(IO_PORTH_11, 1);
    //printf("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
    gpio_direction_output(IO_PORTG_02, 1); //按键版灯

    gpio_direction_output(IO_PORTG_15, 0);//喇叭

#ifdef CONFIG_ETH_ENABLE
    gpio_direction_output(IO_PORTB_05, 0);
    os_time_dly(20);
    gpio_direction_output(IO_PORTB_05, 1);
#endif


    /* while(1){ */
    /* gpio_direction_output(IO_PORTG_06, 0); */
    /* gpio_direction_output(IO_PORTG_07, 0); */
    /* delay(10000); */
    /* gpio_direction_output(IO_PORTG_06, 1); */
    /* gpio_direction_output(IO_PORTG_07, 1); */
    /* delay(10000); */
    /* } */


    // sd power off
    /* sdmmc_power(0); */


//    mipi_phy_con0 &= ~BIT(23);//增加这一句 关闭mipi ldo

    puts("board_init\n");
    devices_init();
    puts("board_int: over\n");
    gpio_direction_output(IO_PORTG_05, 1);
    // sd power on
    /* sdmmc_power(1); */

}

#endif


