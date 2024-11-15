#ifndef DEVICE_CAMERA_H
#define DEVICE_CAMERA_H




#include "device/device.h"
#include "asm/isp_dev.h"
#include "device/video.h"


#define VIDEO_TAG_CAMERA        VIDEO_TAG('c', 'a', 'm', 'e')
#define VIDEO_TAG_UVC           VIDEO_TAG('u', 'v', 'c', ' ')
#define VIDEO_TAG_MASS           VIDEO_TAG('m', 'a', 's', 's')


#define CAMERA_DEVICE_NUM  3

#define CSI2_X0_LANE    0
#define CSI2_X1_LANE    1
#define CSI2_X2_LANE    2
#define CSI2_X3_LANE    3
#define CSI2_X4_LANE    4

// 定义相机平台数据结构
struct camera_platform_data {
    u8 xclk_gpio;        // 外部时钟信号 (xclk) 引脚
    u8 reset_gpio;       // 重置引脚 (用于重置相机)
    u8 pwdn_gpio;        // 电源关闭引脚 (用于关闭相机电源)
    u8 power_value;      // 电源控制值 (表示电源电压等)
    u32 interface;       // 接口类型 (例如 DVP 或 CSI2)
    bool (*online_detect)();  // 函数指针，检查相机是否在线的函数

    union {  // 使用联合体，根据不同接口类型的设置包含不同字段
        struct {  // DVP接口相关的配置
            u32 pclk_gpio;    // 像素时钟 (PCLK) 引脚
            u32 hsync_gpio;   // 行同步信号 (HSYNC) 引脚
            u32 vsync_gpio;   // 垂直同步信号 (VSYNC) 引脚
            u32 io_function_sel;  // I/O功能选择（可能用于配置信号类型或方向）
            u32 data_gpio[10]; // 数据引脚数组（最多10个数据引脚）
        } dvp;  // DVP（数字视频接口）配置

        struct {  // CSI2接口相关的配置
            u8 data_lane_num;   // 数据通道数（通常为4或2）
            u8 clk_rmap;         // 时钟信号映射（配置时钟信号的GPIO引脚）
            u8 clk_inv;          // 时钟信号是否反转（用于时钟信号的反向逻辑）
            u8 d0_rmap;          // 数据通道0的引脚映射
            u8 d0_inv;           // 数据通道0信号是否反转
            u8 d1_rmap;          // 数据通道1的引脚映射
            u8 d1_inv;           // 数据通道1信号是否反转
            u8 d2_rmap;          // 数据通道2的引脚映射
            u8 d2_inv;           // 数据通道2信号是否反转
            u8 d3_rmap;          // 数据通道3的引脚映射
            u8 d3_inv;           // 数据通道3信号是否反转
            u8 tval_hstt;        // 高速数据起始时钟周期（用于CSI2协议中的数据时序）
            u8 tval_stto;        // 数据稳定结束时钟周期（用于CSI2协议中的数据时序）
        } csi2;  // CSI2（摄像头串行接口2）配置
    };
};


struct camera_device_info {
    u16 fps;
    u16 sen_width;
    u16 sen_height;
    u16 width;
    u16 height;
    u32 real_fps;
};


#define CAMERA_PLATFORM_DATA_BEGIN(data) \
	static const struct camera_platform_data data = { \


#define CAMERA_PLATFORM_DATA_END() \
	};

#define CAMERA_CMD_BASE		0x00400000
#define CAMERA_GET_ISP_SRC_SIZE		(CAMERA_CMD_BASE + 1)
#define CAMERA_GET_ISP_SIZE		(CAMERA_CMD_BASE + 2)
#define CAMERA_SET_CROP_SIZE		(CAMERA_CMD_BASE + 3)
#define CAMERA_CROP_TRIG		(CAMERA_CMD_BASE + 4)
#define CAMERA_NEED_REMOUNT		(CAMERA_CMD_BASE + 5)
#define CAMERA_GET_SENSOR_ID    (CAMERA_CMD_BASE + 6)
#define CAMERA_GET_CAPTURE_RAW  (CAMERA_CMD_BASE + 7)
#define CAMERA_SET_TEST_KICK    (CAMERA_CMD_BASE + 8)
#define CAMERA_GET_TEST_DONE    (CAMERA_CMD_BASE + 9)

extern const struct device_operations camera_dev_ops;

//dvp sensor io sel
#define DVP_SENSOR0(sel)   (((sel?1:0) << 24) | (22))//sel 0 PC3~PC13  1 PA6~PA15
#define DVP_SENSOR1(sel)   (((sel?1:0) << 24) | (23))//sel 0 PH1~PH11  1 PD0~PD10


int camera_init(const char *name, const struct video_platform_data *data);

void *camera_driver_open(int id, struct camera_device_info *info);
int camera_driver_ioctl(void *_camera, u32 cmd, void *arg);

int camera_driver_close(void *_camera);
int camera_driver_force_close(void *_camera);




int camera_mux_init(const char *name, const struct video_platform_data *data);
bool camera_mux_online(const char *name);
int camera_mux_close(void *_camera);
void *camera_mux_open(int id, struct camera_device_info *info);


#endif

