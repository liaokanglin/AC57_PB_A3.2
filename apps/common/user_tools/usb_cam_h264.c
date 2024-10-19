#include "system/includes.h"
#include "server/ui_server.h"
#include "server/video_server.h"
#include "server/usb_server.h"
#include "video_rec.h"
#include "video_system.h"
#include "user_isp_cfg.h"


#include "action.h"
#include "app_config.h"
#include "usb_cam_h264.h"
#include "vrec_osd.h"
#include "vrec_icon_osd.h"
#include "app_database.h"
#include "storage_device.h"
#include "asm/lcd_config.h"

struct usb_cam_hdl usbcam_handler[2];
#define __this 	(&usbcam_handler[0])
#define __this2 	(&usbcam_handler[1])
#define sizeof_this     (sizeof(struct usb_cam_hdl))

#define H264_FBUF_SIZE     (2 * 1024 * 1024)

/*码率控制，根据具体分辨率设置*/
static int h264_get_abr(u32 width)
{
//    if (width <= 720) {
//        return 2000;
//    } else if (width <= 1280) {
//        return 2000;
//    } else if (width <= 1920) {
//        return 2000;
//    } else {
    return 2000;
//    }
}

static int h264_2_get_abr(u32 width)
{
//    if (width <= 720) {
//        return 3000;
//    } else if (width <= 1280) {
//        return 5000;
//    } else if (width <= 1920) {
    return 7000;
//    } else {
//        return 1000;
//    }
}

//UVC_CAMERA
int usb_video_h264_open(int width, int height, int rotate, int fps)
{
    malloc_stats();
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;
    int err;

    printf("\n\nusb_video_h264_open : %d x %d %d fps:%d\n", width, height, rotate, fps);

    if (__this->h264_state == USB_CAM_STA_START) {
        return 0;
    }
    if (!__this->uv_h264_fbuf) {
        __this->uv_h264_fbuf = malloc(H264_FBUF_SIZE);
    }
    req.rec.buf = __this->uv_h264_fbuf;
    req.rec.buf_len = H264_FBUF_SIZE;
    req.rec.rec_small_pic 	= 0;

    if (!__this->video_h264) {
        __this->video_h264 = server_open("video_server", "video0.1");

        if (!__this->video_h264) {
            return UCAM_ERR_V0_SERVER_OPEN;
        }
    }
    req.rec.cycle_time = 0xFFFFFF;
    req.rec.fsize = 0;
    //数据外引
    req.rec.target = VIDEO_TO_USB;//VIDEO_TO_OUT | VIDEO_USE_STD;

    req.rec.usb_cam_name = "usb_cam0";
    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_START;
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;
    req.rec.width 	    = width;
    req.rec.height 	    = height;
    req.rec.format 	    = VIDEO_FMT_MOV;
    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_HIG_Q;
    /* req.rec.fps 	    = 0; */
    req.rec.fps 	    = fps;
    req.rec.real_fps 	= 0;
    req.rec.abr_kbps = h264_get_abr(req.rec.width);
    req.rec.IP_interval = 0;
    req.rec.app_avc_cfg = NULL;
    req.rec.rotate = rotate;

    req.rec.camera_config = NULL;
    /* req.rec.camera_config = load_default_camera_config; */

    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

#if 1
    memcpy(video_rec_osd_buf, osd_str_buf, strlen(osd_str_buf));
#endif

    text_osd.font_w = 16;
    text_osd.font_h = 32;
    max_one_line_strnum = strlen(video_rec_osd_buf);//21;

    osd_line_num = 1;
    /* if (db_select("num")) { */
    /* osd_line_num = 2; */
    /* } */
    osd_max_heigh = (req.rec.height == 1088) ? 1080 : req.rec.height ;
    text_osd.x = (req.rec.width - max_one_line_strnum * text_osd.font_w) / 64 * 64;
    text_osd.y = (osd_max_heigh - text_osd.font_h * osd_line_num) / 16 * 16;
    /* text_osd.color[0] = 0xe20095; */
    /* text_osd.bit_mode = 1; */
    text_osd.color[0] = 0x057d88;
    text_osd.color[1] = 0xe20095;
    text_osd.color[2] = 0xe20095;
    text_osd.bit_mode = 2;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
    /* text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
    text_osd.direction = 1;

    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = (u8 *)icon_16bit_data; //icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#else
    text_osd.direction = 0;
#endif


    if (0) {
        /* if (1) { */
        req.rec.text_osd = &text_osd;
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
        req.rec.graph_osd = NULL;//&graph_osd;
#endif
    }

    err = server_request(__this->video_h264, VIDEO_REQ_REC, &req);
    if (err != 0) {
        puts("\n\n\nstart rec err\n\n\n");
        return UCAM_ERR_V0_REQ_START;
    }
    __this->h264_state = USB_CAM_STA_START;
    start_update_isp_scenes(__this->video_h264);
    malloc_stats();

    return 0;
}

int usb_video_h264_close(void)
{
    printf("\n[ debug ]--func=%s line=%d\n", __func__, __LINE__);
    union video_req req = {0};
    int err;
    if (__this->h264_state != USB_CAM_STA_START) {
        return 0;
    }
    /* stop_update_isp_scenes(); */
    if (__this->video_h264) {
        req.rec.channel = 0;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this->video_h264, VIDEO_REQ_REC, &req);
        if (err) {
            log_e("h.264 stream close : 0x%x\n", err);
            return UCAM_ERR_V0_REQ_STOP;
        }
        /* if (__this->uv_h264_fbuf) { */
        /* free(__this->uv_h264_fbuf); */
        /* __this->uv_h264_fbuf = NULL; */
        /* } */
        server_close(__this->video_h264);
        __this->video_h264 = NULL;
        __this->h264_state = USB_CAM_STA_STOP;
    }
    return 0;
}
//USB_CAMERA
int usb_video_h264_open2(int width, int height, int rotate, int fps)
{
    malloc_stats();
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;
    int err;

    printf("\n\nusb_video_h264_open2 : %d x %d %d fps:%d\n", width, height, rotate, fps);

    if (__this2->h264_state == USB_CAM_STA_START) {
        return 0;
    }
    if (!__this2->uv_h264_fbuf) {
        __this2->uv_h264_fbuf = malloc(H264_FBUF_SIZE);
    }
    req.rec.buf = __this2->uv_h264_fbuf;
    req.rec.buf_len = H264_FBUF_SIZE;
    req.rec.rec_small_pic 	= 0;

    if (!__this2->video_h264) {
        __this2->video_h264 = server_open("video_server", "video0.0");

        if (!__this2->video_h264) {
            return UCAM_ERR_V0_SERVER_OPEN;
        }
    }
    req.rec.cycle_time = 0xFFFFFF;
    req.rec.fsize = 0;
    //数据外引
    req.rec.target = VIDEO_TO_USB;//VIDEO_TO_OUT | VIDEO_USE_STD;

    req.rec.usb_cam_name = "usb_cam1";
    req.rec.channel = 1;
    req.rec.state 	= VIDEO_STATE_START;
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;
    req.rec.width 	    = width;
    req.rec.height 	    = height;
    req.rec.format 	    = VIDEO_FMT_MOV;
    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_HIG_Q;
    /* req.rec.fps 	    = 0; */
    req.rec.fps 	    = fps;
    req.rec.real_fps 	= 0;
    req.rec.abr_kbps = h264_2_get_abr(req.rec.width);
    req.rec.IP_interval = 0;
    req.rec.app_avc_cfg = NULL;
    req.rec.rotate = rotate;

    req.rec.camera_config = NULL;
    /* req.rec.camera_config = load_default_camera_config; */

    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

#if 1
    memcpy(video_rec_osd_buf, osd_str_buf, strlen(osd_str_buf));
#endif

    text_osd.font_w = 16;
    text_osd.font_h = 32;
    max_one_line_strnum = strlen(video_rec_osd_buf);//21;

    osd_line_num = 1;
    /* if (db_select("num")) { */
    /* osd_line_num = 2; */
    /* } */
    osd_max_heigh = (req.rec.height == 1088) ? 1080 : req.rec.height ;
    text_osd.x = (req.rec.width - max_one_line_strnum * text_osd.font_w) / 64 * 64;
    text_osd.y = (osd_max_heigh - text_osd.font_h * osd_line_num) / 16 * 16;
    /* text_osd.color[0] = 0xe20095; */
    /* text_osd.bit_mode = 1; */
    text_osd.color[0] = 0x057d88;
    text_osd.color[1] = 0xe20095;
    text_osd.color[2] = 0xe20095;
    text_osd.bit_mode = 2;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
    /* text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
    text_osd.direction = 1;

    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = (u8 *)icon_16bit_data; //icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#else
    text_osd.direction = 0;
#endif


    if (0) {
        /* if (1) { */
        req.rec.text_osd = &text_osd;
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
        req.rec.graph_osd = NULL;//&graph_osd;
#endif
    }

    err = server_request(__this2->video_h264, VIDEO_REQ_REC, &req);
    if (err != 0) {
        puts("\n\n\nstart rec err\n\n\n");
        return UCAM_ERR_V0_REQ_START;
    }
    __this2->h264_state = USB_CAM_STA_START;
    start_update_isp_scenes(__this2->video_h264);
    malloc_stats();

    return 0;
}

int usb_video_h264_close2(void)
{
    printf("\n[ debug ]--func=%s line=%d\n", __func__, __LINE__);
    union video_req req = {0};
    int err;
    if (__this2->h264_state != USB_CAM_STA_START) {
        return 0;
    }
    /* stop_update_isp_scenes(); */
    if (__this2->video_h264) {
        req.rec.channel = 1;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this2->video_h264, VIDEO_REQ_REC, &req);
        if (err) {
            log_e("h.264 stream close : 0x%x\n", err);
            return UCAM_ERR_V0_REQ_STOP;
        }
        /* if (__this->uv_h264_fbuf) { */
        /* free(__this->uv_h264_fbuf); */
        /* __this->uv_h264_fbuf = NULL; */
        /* } */
        server_close(__this2->video_h264);
        __this2->video_h264 = NULL;
        __this2->h264_state = USB_CAM_STA_STOP;
    }
    return 0;
}
#ifdef CONFIG_UVC_SLAVE_ENABLE
void video_write_err(u8 id, int drop_fcnt)
{}


static void usb_video_h264_event_handler(struct sys_event *event)
{
    enum {
        CMD_NONE = 0,
        CMD_FORCE_IFRAME,
        CMD_SET_FPS,
        CMD_SET_KBPS,
    };
    struct usb_video_cmd_t _cmd;
    if (!strcmp((char *)event->arg, "uvc h264")) {
        if (event->u.dev.event == DEVICE_EVENT_CHANGE) {
            memcpy(&_cmd, (void *)event->u.dev.value, 4);
            printf("id: %d\n", _cmd.id);
            printf("cmd: %d\n", _cmd.cmd);
            printf("data: %d\n", _cmd.data);
            switch (_cmd.cmd) {
            case CMD_FORCE_IFRAME:
                //TODO
                break;
            case CMD_SET_FPS:
                //TODO
                break;
            case CMD_SET_KBPS:
                //TODO
                break;
            default:
                break;
            }

        }
    }
}
/*
 * 静态注册设备事件回调函数，优先级为0
 */
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, usb_video_h264_event_handler, 0);

#endif


int video0_rec_get_iframe1(void)
{
    union video_req req = {0};
    if (!__this->video_h264) {
        return -1;
    }
    req.rec.channel = 0;
    req.rec.buf = NULL; //如果想要I帧数据，malloc buf
    req.rec.buf_len = 0;
    req.rec.state   = VIDEO_STATE_CAP_IFRAME;
    req.rec.format = VIDEO_FMT_MOV;
    return server_request(__this->video_h264, VIDEO_REQ_REC, &req);
}

int video0_rec_get_iframe2(void)
{
    union video_req req = {0};
    if (!__this2->video_h264) {
        return -1;
    }
    req.rec.channel = 1;
    req.rec.buf = NULL; //如果想要I帧数据，malloc buf
    req.rec.buf_len = 0;
    req.rec.state   = VIDEO_STATE_CAP_IFRAME;
    req.rec.format = VIDEO_FMT_MOV;
    return server_request(__this2->video_h264, VIDEO_REQ_REC, &req);
}


int video0_rec_set_dr(u8 fps)
{
    union video_req req = {0};

    if (!__this->video_h264) {
        return -EINVAL;
    }

    req.rec.real_fps = fps;
    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_SET_DR;


    return server_request(__this->video_h264, VIDEO_REQ_REC, &req);

}

int video0_rec_set_dr2(u8 fps)
{
    union video_req req = {0};

    if (!__this2->video_h264) {
        return -EINVAL;
    }

    req.rec.real_fps = fps;
    req.rec.channel = 1;
    req.rec.state 	= VIDEO_STATE_SET_DR;


    return server_request(__this2->video_h264, VIDEO_REQ_REC, &req);

}



int video0_rec_set_abr(int bits_rate)
{
    union video_req req = {0};

    if (!__this->video_h264) {
        return -EINVAL;
    }
    req.rec.channel = 0;//type == 0 ? 1 : 0;
    /* printf("%s bits rate : %d\n", type == 0 ? "MJPG" : "H.264", bits_rate); */
    req.rec.state = VIDEO_STATE_RESET_BITS_RATE;
    req.rec.abr_kbps = bits_rate;
    return server_request(__this->video_h264, VIDEO_REQ_REC, &req);
}


int video0_rec_set_abr2(int bits_rate)
{
    union video_req req = {0};

    if (!__this2->video_h264) {
        return -EINVAL;
    }
    req.rec.channel = 1;//type == 0 ? 1 : 0;
    /* printf("%s bits rate : %d\n", type == 0 ? "MJPG" : "H.264", bits_rate); */
    req.rec.state = VIDEO_STATE_RESET_BITS_RATE;
    req.rec.abr_kbps = bits_rate;
    return server_request(__this2->video_h264, VIDEO_REQ_REC, &req);
}
