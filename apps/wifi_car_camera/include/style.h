#ifndef UI_STYLE_H
#define UI_STYLE_H

#include "app_config.h"

#ifdef CONFIG_UI_STYLE_JL02_ENABLE

#include "style_jl02.h"

#define ID_WINDOW_VIDEO_REC  PAGE_0  // 视频录制页面，关联 PAGE_1
// #define ID_WINDOW_VIDEO_TPH  PAGE_1  // 视频拍照页面，关联 PAGE_2
#define ID_WINDOW_VIDEO_REP  PAGE_1  // 视频回放页面，关联 PAGE_3
#define ID_WINDOW_USB_SLAVE  PAGE_2  // USB 从设备页面，关联 PAGE_4
// #define ID_WINDOW_VIDEO_SYS  PAGE_4  // 视频系统设置页面，关联 PAGE_5
// #define ID_WINDOW_MAIN_PAGE  PAGE_0  // 主页面，关联 PAGE_0
// #define ID_WINDOW_PARKING    PAGE_6  // 停车页面，关联 PAGE_6
// #define ID_WINDOW_LANE       PAGE_7  // 车道页面，关联 PAGE_7


#endif


#ifdef CONFIG_UI_STYLE_LY_ENABLE

#include "style_ly.h"

#define ID_WINDOW_VIDEO_REC PAGE_0
#define ID_WINDOW_VIDEO_TPH PAGE_1
#define ID_WINDOW_VIDEO_REP PAGE_2
#define ID_WINDOW_USB_SLAVE PAGE_3
#define ID_WINDOW_VIDEO_SYS PAGE_4
#define ID_WINDOW_PARKING   PAGE_5
#define ID_WINDOW_LANE      PAGE_6
#endif









#endif

