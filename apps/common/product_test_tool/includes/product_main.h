#ifndef _PRODUCT_MAIN_H_
#define _PRODUCT_MAIN_H_

#include "app_config.h"

#ifdef PRODUCT_TEST_ENABLE

#include "system/includes.h"
#include "json_c/json.h"
#include "json_c/json_object.h"
#include "json_c/json_tokener.h"
#include "usb_ep_cfg.h"
#include "ioctl_cmds.h"
#ifdef CONFIG_WIFI_ENABLE
#include "lwip.h"
#include "device/wifi_dev.h"
#endif
#include "camera.h"
#include "gpio.h"
#include "fs/fs.h"

#include "datatype.h"
#include "product_port.h"
#include "communication.h"
#include "devices_control.h"

//*********************************************************************************//
//                                  PRODUCT测试相关配置                            //
//*********************************************************************************//
#define 	PRODUCT_TEST_DEBUG
//#define 	MSG_TEST_DEBUG

#define 	DATA_BUFFER_ALIGN	(4)
#define 	TEST_BUFFER_SIZE	(sizeof(struct comm_head) + sizeof(u16) + 1024 * 1)
#define 	RESP_BUFFER_SIZE	(sizeof(struct comm_head) + sizeof(u16) + 1024 * 3)
#define 	RECV_BUFFER_SIZE	(sizeof(struct comm_head) + sizeof(u16) + 1024 * 3)

#define 	PRODUCT_MAC_SIZE				(6)
#define 	PRODUCT_UUID_SIZE				(16)
#define 	PRODUCT_SN_SIZE					(16)
#define 	PRODUCT_LICENSE_INFO_SIZE		(512)
#define 	PRODUCT_OPTION_SIZE				(512)
#define 	PRODUCT_OPTION_STRING_SIZE		(512)
#define 	PRODUCT_BOOT_SCREEN_INFO_SIZE	(16)
#define 	PRODUCT_BOOT_SCREEN_SIZE		(500 * 1024)
#define 	PRODUCT_RTC_INFO_SIZE	    (sizeof(struct product_rtc_time))

#define 	IO_TRIGGER_MODE      (0) //"IO检测"方式触发进入测试模式
#define 	USER_DEF_MODE        (1) //"用户定义"方式触发进入测试模式
#define 	AUTO_STARTUP_MODE    (2) //上电进入测试模式



#define CONFIG_PD_TRIGGER_MODE   AUTO_STARTUP_MODE //触发方式配置

#define CONFIG_PD_TRIGGER_IO_PORT   IO_PORTH_10    //"IO检测"IO配置
#define CONFIG_PD_TRIGGER_IO_STATE  (0)        	   //"IO检测"IO的状态配置，0:低电平触发，1:高电平触发
#define CONFIG_PD_TRIGGER_IO_CNT    (10)           //"IO检测"IO的检测次数
#define DEV_STATUS_ON	(1)
#define DEV_STATUS_OFF	(0)
#define	DEV_MONITOR_PERIOD	(50) //ticks


//配置设备列表
#define _STR(x) #x
#define STR(x) _STR(x)
#define DEVICE_TYPE(id, dev) "{\"id\" : \""STR(id)"\", \"type\" : \""STR(dev)"\"}"
#ifdef CONFIG_WIFI_ENABLE
#define DEVICES_LIST "\"devs_list\" : [" \
	DEVICE_TYPE(0,DEV_TYPE_SD)","\
	DEVICE_TYPE(0,DEV_TYPE_LCD)","\
	DEVICE_TYPE(0,DEV_TYPE_MIC)","\
	DEVICE_TYPE(0,DEV_TYPE_SPEAKER)","\
	DEVICE_TYPE(0,DEV_TYPE_CAMERA)","\
	DEVICE_TYPE(0,DEV_TYPE_BATTERY)","\
	DEVICE_TYPE(0,DEV_TYPE_WIFI)","\
	DEVICE_TYPE(0,DEV_TYPE_PIR)","\
	DEVICE_TYPE(0,DEV_TYPE_MOTOR)","\
	DEVICE_TYPE(0,DEV_TYPE_GSENSOR)","\
	DEVICE_TYPE(0,DEV_TYPE_TOUCHPANEL)\
	"]"
#else
#define DEVICES_LIST "\"devs_list\" : [" \
	DEVICE_TYPE(0,DEV_TYPE_SD)","\
	DEVICE_TYPE(0,DEV_TYPE_LCD)","\
	DEVICE_TYPE(0,DEV_TYPE_MIC)","\
	DEVICE_TYPE(0,DEV_TYPE_SPEAKER)","\
	DEVICE_TYPE(0,DEV_TYPE_CAMERA)","\
	DEVICE_TYPE(0,DEV_TYPE_BATTERY)","\
	DEVICE_TYPE(0,DEV_TYPE_PIR)","\
	DEVICE_TYPE(0,DEV_TYPE_MOTOR)","\
	DEVICE_TYPE(0,DEV_TYPE_GSENSOR)","\
	DEVICE_TYPE(0,DEV_TYPE_TOUCHPANEL)\
	"]"
#endif


//配置固件信息
#define PD_TOOL_VERSION		("v1.0.0")
#define PD_CHIP_VERSION		("AC57xx")
#define PD_SDK_VERSION		("v3.0.3")//(sdk_version())


#ifdef PRODUCT_TEST_DEBUG
#define     log_info(x, ...)     printf("[PRODUCT_TEST][INFO] " x " ", ## __VA_ARGS__)
#define     log_err(x, ...)      printf("[PRODUCT_TEST][ERR] " x " ", ## __VA_ARGS__)
#else
#define     log_info(...)
#define     log_err(...)
#endif
//*********************************************************************************//
//                                  PRODUCT测试相关接口                            //
//*********************************************************************************//

/**
 * @brief 产测测试初始化
 *
 * @param
 *
 * @note
 */
u8 product_main(void);


u8 is_product_mode(void);
void data_respond(u8 idx, u8 type, u8 *data, u32 len);
#endif //PRODUCT_TEST_ENABLE

#endif


