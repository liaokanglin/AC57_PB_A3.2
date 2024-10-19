#ifndef _PRODUCT_DATATYPE_H_
#define _PRODUCT_DATATYPE_H_

#include "typedef.h"

enum OPC_GROUP {
    OPC_GET_PROFILE = 0,
    OPC_DEV_CTL,
    OPC_WRITE_UUID,
    OPC_WRITE_MAC_EDR,
    OPC_WRITE_MAC_BLE,
    OPC_WRITE_MAC_WIFI,
    OPC_WRITE_SN,
    OPC_WRITE_OPTIONS,
    OPC_WRITE_FILE_INFO,
    OPC_READ_UUID,
    OPC_READ_MAC_EDR,
    OPC_READ_MAC_BLE,
    OPC_READ_MAC_WIFI,
    OPC_READ_SN,
    OPC_GET_LICENSE_INFO,
    OPC_RTC_DEF_TIME_WRITE,
    OPC_RTC_DEF_TIME_READ,
    OPC_ERASE_BOOTSCREENS,
};


enum DEV_CTL_GROUP {
    CTL_READ_REG = 0,
    CTL_WRITE_REG,
    CTL_GET_DEV_INFO,
    CTL_WIFI_CHANGE_MODE,
    CTL_WIFI_LAUNCH_SCAN,
    CTL_WIFI_GET_SCAN_RES,
    CTL_WIFI_GET_STA_ENTRY_INFO,
    CTL_LCD_COLOR_SWITCH_ON,
    CTL_LCD_COLOR_SWITCH_OFF,
    CTL_CAMERA_NVG_ON,
    CTL_CAMERA_NVG_OFF,
    CTL_CAMERA_LIGHT_ON,
    CTL_CAMERA_LIGHT_OFF,
    CTL_SD_FILE_WR,
    CTL_NETWORK_PING_START,
    CTL_NETWORK_PING_RSP,
    CTL_DEV_MONITOR_START,
    CTL_DEV_MONITOR_STOP,
    CTL_DEV_MONITOR_RSP,
    CTL_WIFI_GET_STA_CONN_INFO,
    CTL_MOTOR_LEFT,
    CTL_MOTOR_RIGHT,
    CTL_MOTOR_STOP,
};


enum ERR_GROUP {
    ERR_NULL = 0,
    ERR_PARAMS,
    ERR_NO_SUPPORT_DEV,
    ERR_NO_SUPPORT_DEV_CMD,
    ERR_DEV_FAULT,
    ERR_MALLOC_FAIL,
    ERR_FILE_WR,
    ERR_FILE_CHECK,
    ERR_SAME_DATA,
    ERR_NETWORK_DISCONNECT,
    ERR_FILE_WR_NOSPACE,
    ERR_ALREADY_EXIST
};


//DEV_TYPE_GROUP
#define DEV_TYPE_SD 0
#define DEV_TYPE_LCD 1
#define DEV_TYPE_MIC 2
#define DEV_TYPE_SPEAKER 3
#define DEV_TYPE_CAMERA 4
#define DEV_TYPE_KEYPAD 5
#define DEV_TYPE_BATTERY 6
#define DEV_TYPE_WIFI 7
#define DEV_TYPE_PIR 8
#define	DEV_TYPE_MOTOR		9
#define	DEV_TYPE_GSENSOR    10
#define	DEV_TYPE_TOUCHPANEL 11

enum DATA_TYPE_GROUP {
    DATA_TYPE_OPCODE,
    DATA_TYPE_LICENSE_WRITE,
    DATA_TYPE_STARTUP_SCREENS,
    DATA_TYPE_LICENSE_READ,
    DATA_TYPE_LICENSE_ERASE,
    DATA_TYPE_SHUTDOWN_SCREENS,
};


struct comm_head {
    u8 mark[2];
    u8 idx;
    u8 type;
    u16 len;
    u8 reserved[2];
};


struct comm_msg {
    u16 len;
    u8 *data;
    struct comm_msg *self;
    struct list_head entry;
};


struct procudt_camera_info {
    u8 name[8];
    u32 fps;
    u32 width;
    u32 height;
};


struct product_rtc_time {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
};

#endif





