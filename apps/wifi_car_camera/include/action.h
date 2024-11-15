#ifndef APP_ACTION_H
#define APP_ACTION_H
#include "app_config.h"


#define ACTION_HOME_MAIN 				0x00001001  // 主页主动作
#define ACTION_DEVICE_OUT 				0x00001002  // 设备退出动作

#define ACTION_VIDEO_REC_MAIN 			0x00002001  // 视频录制主动作
#define ACTION_VIDEO_REC_SET_CONFIG 	0x00002002  // 设置视频录制配置
#define ACTION_VIDEO_REC_GET_CONFIG 	0x00002003  // 获取视频录制配置
#define ACTION_VIDEO_REC_CHANGE_STATUS 	0x00002004  // 改变视频录制状态
#define ACTION_VIDEO_REC_CONTROL        0x00002005  // 控制视频录制
#define ACTION_VIDEO_REC_SWITCH_WIN     0x00002006  // 切换视频录制窗口
#define ACTION_VIDEO_REC_LOCK_FILE      0x00002007  // 锁定视频录制文件
#define ACTION_VIDEO_REC_GET_APP_STATUS 0x00002008  // 获取视频录制应用状态
#define ACTION_VIDEO_REC_GET_PATH       0x00002009  // 获取视频录制路径
#define ACTION_VIDEO0_OPEN_RT_STREAM    0x0000200b  // 打开视频0实时流
#define ACTION_VIDEO0_CLOSE_RT_STREAM   0x0000200c  // 关闭视频0实时流
#define ACTION_VIDEO1_OPEN_RT_STREAM    0x0000200d  // 打开视频1实时流
#define ACTION_VIDEO1_CLOSE_RT_STREAM   0x0000200e  // 关闭视频1实时流
#define ACTION_VIDEO_TAKE_PHOTO         0x0000200f  // 视频拍照
#define ACTION_VIDEO_REC_CONCTRL        0x00002010  // 控制视频录制（注意拼写错误，应为 "CONTROL"）
#define ACTION_VIDEO_CYC_SAVEFILE       0x00002011  // 循环保存视频文件
#define ACTION_VIDEO_REC_SWITCH_VIEW    0x00002012  //可能用于在视频记录过程中切换视图模式或摄像头。
#define ACTION_REC_TAKE_PHOTO           0x00002013  //触发拍照的动作，通常在用户点击拍照按钮时调用。
#define ACTION_REC_CAP_TAKE_PHOTO       0x00002014  //可能用于捕捉照片的特定状态或模式，比如在视频录制过程中同时进行拍照。

#define ACTION_VIDEO_DEC_MAIN 			0x00004001  // 视频解码主动作
#define ACTION_VIDEO_DEC_SET_CONFIG 	0x00004002  // 设置视频解码配置
#define ACTION_VIDEO_DEC_GET_CONFIG 	0x00004003  // 获取视频解码配置
#define ACTION_VIDEO_DEC_CHANGE_STATUS 	0x00004004  // 改变视频解码状态
#define ACTION_VIDEO_DEC_OPEN_FILE    	0x00004005  // 打开视频文件解码
#define ACTION_VIDEO_DEC_CONTROL        0x00004006  // 控制视频解码
#define ACTION_VIDEO_DEC_CUR_PAGE       0x00004007  // 获取视频解码当前页面
#define ACTION_VIDEO_DEC_SWITCH         0x00004008  // 切换视频解码页面


#define ACTION_PHOTO_TAKE_MAIN 			0x00008001
#define ACTION_PHOTO_TAKE_SET_CONFIG 	0x00008002
#define ACTION_PHOTO_TAKE_GET_CONFIG 	0x00008003
#define ACTION_PHOTO_TAKE_CHANGE_STATUS	0x00008004
#define ACTION_PHOTO_TAKE_GET_CAMERAID 	0x00008005
#define ACTION_PHOTO_TAKE_CONTROL       0x00008006
#define ACTION_PHOTO_TAKE_SWITCH_WIN    0x00008007

#define ACTION_MUSIC_PLAY_MAIN 			0x00009001
#define ACTION_MUSIC_PLAY_SET_CONFIG 	0x00009002
#define ACTION_MUSIC_PLAY_GET_CONFIG 	0x00009003
#define ACTION_MUSIC_PLAY_CHANGE_STATUS 0x00009004


#define ACTION_AUDIO_REC_MAIN 			0x00010001
#define ACTION_AUDIO_REC_SET_CONFIG 	0x00010002
#define ACTION_AUDIO_REC_GET_CONFIG 	0x00010003
#define ACTION_AUDIO_REC_CHANGE_STATUS  0x00010004


#define ACTION_USB_SLAVE_MAIN 			0x00020001
#define ACTION_USB_SLAVE_SET_CONFIG 	0x00020002
#define ACTION_USB_SLAVE_GET_CONFIG 	0x00020003

#define ACTION_UPGRADE_MAIN             0x00030001
#define ACTION_UPGRADE_SET_CONFIRM      0x00030002

#define ACTION_SYSTEM_MAIN              0x00FFF001  // 系统主操作标识
#define ACTION_SYSTEM_SET_CONFIG        0x00FFF002  // 设置系统配置的操作标识
#define ACTION_SYSTEM_GET_CONFIG        0x00FFF003  // 获取系统配置的操作标识



#define ACTION_AUDIO_MAIN 	     		0x00011001
#define ACTION_AUDIO_SET_CONFIG     	0x00011002
#define ACTION_AUDIO_GET_CONFIG     	0x00011003
#define ACTION_AUDIO_CHANGE_STATUS      0x00011004

#define ACTION_NET_SCR_REC_CLOSE        0X00888001
#define ACTION_NET_SCR_REC_OPEN         0X00888002


#endif

