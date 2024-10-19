
#ifndef __USB_CAM_264_H_
#define __USB_CAM_264_H_

#include "system/includes.h"
#include "server/video_server.h"
#include "app_config.h"


enum USB_CAM_STA {
    USB_CAM_STA_IDLE,
    USB_CAM_STA_START,
    USB_CAM_STA_STOP,
    USB_CAM_STA_FORBIDDEN,
    USB_CAM_STA_STOPING,
};

enum ucam_err_code {
    UCAM_ERR_NONE,
    UCAM_ERR_V0_SERVER_OPEN,
    UCAM_ERR_V1_SERVER_OPEN,
    UCAM_ERR_START_FREE_SPACE,
    UCAM_ERR_SAVE_FREE_SPACE,

    UCAM_ERR_V0_REQ_START,
    UCAM_ERR_V1_REQ_START,
    UCAM_ERR_V0_REQ_STOP,
    UCAM_ERR_V1_REQ_STOP,
    UCAM_ERR_V0_REQ_SAVEFILE,
    UCAM_ERR_V1_REQ_SAVEFILE,

    UCAM_ERR_PKG,
    UCAM_ERR_MKDIR,

};
/*
  usb slave related start
*/


struct usb_app_hdl {
    u8 state;
    u8 mode;
    struct server *usb_slave;
    struct server *usb_host;
    struct server *ui;
    u8 *buf;
    u32 buf_size;
};
/*
  usb slave related end
*/

struct usb_video_config {
    u16 width;
    u16 height;
    int fmt;
    int fps;
};

struct video_stream_watermark {
    int id;
    int x;
    int y;
    int yuv_color;
    char *format;
    u16 font_w;
    u16 font_h;
    char *matrix_table;
    u8   *matrix_addr;
    int  matrix_size;
};

struct usb_cam_hdl {
    enum USB_CAM_STA h264_state;
    enum USB_CAM_STA mjpg_state;
    struct server *ui;
    struct server *video_h264;
    struct usb_video_config h264_cfg;

    u32 cycle_cnt;
    int cycle_timer;
    u32 not_cycle_cnt;

    u8 *uv_h264_fbuf;
    u8 *cap_buf;

    void *file0;
    void *file1;

    u8 osd_enable;
    int h264_abr;
    /*
      usb slave related start
    */
    struct usb_app_hdl usb;
    /*
      usb slave related end
    */
    const char *cmd_task;
};

struct usb_video_cmd_t {
    u8 id;
    u8 cmd;
    u16 data;
};

extern int usb_video_h264_open(int width, int height, int rotate, int fps);
extern int usb_video_h264_close(void);
extern int usb_video_h264_open2(int width, int height, int rotate, int fps);
extern int usb_video_h264_close2(void);


#endif  // __USB_CAM_APP_H_

