#include "system/includes.h"
#include "server/ui_server.h"
#include "server/video_server.h"
#include "server/video_engine_server.h"
#include "server/usb_server.h"
#include "video_rec.h"
#include "video_system.h"
#include "gSensor_manage.h"
#include "user_isp_cfg.h"
#include "get_image_data.h"

#include "action.h"
#include "style.h"
#include "app_config.h"
#include "asm/debug.h"
#include "vrec_osd.h"
#include "vrec_icon_osd.h"
#include "app_database.h"
#include "storage_device.h"
#include "asm/lcd_config.h"

#if (APP_CASE == __WIFI_CAR_CAMERA__)
#include "net_config.h"
#include "net_video_rec.h"
#endif

#define VIDEO_REC_NO_MALLOC //统一分配内存

#ifndef CONFIG_VIDEO0_ENABLE
#undef VREC0_FBUF_SIZE
#define VREC0_FBUF_SIZE   0
#endif
#ifndef CONFIG_VIDEO1_ENABLE
#undef VREC1_FBUF_SIZE
#define VREC1_FBUF_SIZE   0
#endif
#ifndef CONFIG_VIDEO2_ENABLE
#undef VREC2_FBUF_SIZE
#define VREC2_FBUF_SIZE   0
#endif
#ifndef CONFIG_VIDEO3_ENABLE
#undef VREC3_FBUF_SIZE
#define VREC3_FBUF_SIZE   0
#endif


#define SCREEN_W        LCD_DEV_WIDTH
#define SCREEN_H        LCD_DEV_HIGHT

#define MOTION_STOP_SEC     20
#define MOTION_START_SEC    2

#define LOCK_FILE_PERCENT	40    //0~100
#define NAME_FILE_BY_DATE   1

static u8 cam_init=0;
static int video_rec_start();
static int video_rec_stop(u8 close);
static int ve_mdet_start();
static int ve_mdet_stop();
static int ve_lane_det_start(u8 fun_sel);
static int ve_lane_det_stop(u8 fun_sel);
static int ve_face_det_start(u8 fun_sel);
static int ve_face_det_stop(u8 fun_sel);
static int video_rec_start_isp_scenes();
static int video_rec_stop_isp_scenes(u8 status, u8 restart);
static int video_rec_savefile(int dev_id);
extern int video_rec_set_config(struct intent *it);
extern int video_take_photo_rec(u16 sel);
static int video0_set_audio_callback(struct server *server, int (*callback)(u8 *buf, u32 len));
void switch_camera_disp();
void camera_display_init();
void change_camera_config();
void get_ahd_yuv_init(void (*cb)(u8 *data));
static u8 disp_state=DISP_INITIAL_STATE;
u8 camera_config;//1:uvc  2:ahd 3:double ahd 4:ahd+uvc
u8 video1_is_disp=0;
char video_rec_osd_buf[64] ALIGNE(64);
struct video_rec_hdl rec_handler;
// u8 yuv_deal[1280*480*3/2]={0};
u8 yuv_deal[1280*720*3/2]={0};///////
static u16 video0_rec_width = 0;
static u16 video1_rec_width = 0;
u8 ahd_view_show_status=0;  //0---前视全屏  1---模拟后拉全屏  4---非全屏显示
u8 app_mode_flag=0;//1：代表录影模式，0：非录影模式

#define __this 	(&rec_handler)


#ifdef __CPU_DV15__
static const u16 rec_pix_w[] = {1280, 640};
static const u16 rec_pix_h[] = {720,  480};
#else
static const u16 rec_pix_w[] = {1920, 1280, 640};
static const u16 rec_pix_h[] = {1088, 720,  480};
#endif


#ifdef CONFIG_VIDEO4_ENABLE
static struct video_window disp_window[DISP_MAX_WIN][4] = {0};

static void video_set_disp_window()
{
#define DISP_W (SCREEN_W - 80)
#define DISP_H (SCREEN_H)
#define DISP_W2 (DISP_W / 2 / 16 * 16)
#define DISP_H2 (DISP_H / 2 / 16 * 16)
#define DISP_H3 (DISP_H / 3 / 16 * 16)
#define DISP_W4 (DISP_W / 4 / 16 * 16)

#define BORDER2 ((720 * DISP_W2 / 1280 - DISP_H2) / 2)

    //DISP_MAIN_WIN
    disp_window[DISP_MAIN_WIN][0].left   = (DISP_W - DISP_W2 * 2) / 2;
    disp_window[DISP_MAIN_WIN][0].top    = 0;
    disp_window[DISP_MAIN_WIN][0].width  = DISP_W2;
    disp_window[DISP_MAIN_WIN][0].height = DISP_H2;
    disp_window[DISP_MAIN_WIN][0].border_top    = BORDER2 > 0 ? BORDER2 : 0;
    disp_window[DISP_MAIN_WIN][0].border_bottom = BORDER2 > 0 ? BORDER2 : 32;

    disp_window[DISP_MAIN_WIN][1].left   = DISP_W2 + (DISP_W - DISP_W2 * 2);
    disp_window[DISP_MAIN_WIN][1].top    = 0;
    disp_window[DISP_MAIN_WIN][1].width  = DISP_W2;
    disp_window[DISP_MAIN_WIN][1].height = DISP_H2;
    disp_window[DISP_MAIN_WIN][1].border_top    = BORDER2 > 0 ? BORDER2 : 0;
    disp_window[DISP_MAIN_WIN][1].border_bottom = BORDER2 > 0 ? BORDER2 : 32;

    disp_window[DISP_MAIN_WIN][2].left   = (DISP_W - DISP_W2 * 2) / 2;
    disp_window[DISP_MAIN_WIN][2].top    = DISP_H2;
    disp_window[DISP_MAIN_WIN][2].width  = DISP_W2;
    disp_window[DISP_MAIN_WIN][2].height = DISP_H2;
    disp_window[DISP_MAIN_WIN][2].border_top    = BORDER2 > 0 ? BORDER2 : 0;
    disp_window[DISP_MAIN_WIN][2].border_bottom = BORDER2 > 0 ? BORDER2 : 32;

    disp_window[DISP_MAIN_WIN][3].left   = DISP_W2 + (DISP_W - DISP_W2 * 2);
    disp_window[DISP_MAIN_WIN][3].top    = DISP_H2;
    disp_window[DISP_MAIN_WIN][3].width  = DISP_W2;
    disp_window[DISP_MAIN_WIN][3].height = DISP_H2;
    disp_window[DISP_MAIN_WIN][3].border_top    = BORDER2 > 0 ? BORDER2 : 0;
    disp_window[DISP_MAIN_WIN][3].border_bottom = BORDER2 > 0 ? BORDER2 : 32;

    //DISP_HALF_WIN
    disp_window[DISP_HALF_WIN][0].left   = 0;
    disp_window[DISP_HALF_WIN][0].top    = 0;
    disp_window[DISP_HALF_WIN][0].width  = DISP_W - DISP_W4;
    disp_window[DISP_HALF_WIN][0].height = DISP_H;
    disp_window[DISP_HALF_WIN][0].border_top    = 0;
    disp_window[DISP_HALF_WIN][0].border_bottom = 64;

    disp_window[DISP_HALF_WIN][1].left   = DISP_W - DISP_W4;
    disp_window[DISP_HALF_WIN][1].top    = 0;
    disp_window[DISP_HALF_WIN][1].width  = DISP_W4;
    disp_window[DISP_HALF_WIN][1].height = DISP_H3;
    disp_window[DISP_HALF_WIN][1].border_top    = 0;
    disp_window[DISP_HALF_WIN][1].border_bottom = 16;

    disp_window[DISP_HALF_WIN][2].left   = DISP_W - DISP_W4;
    disp_window[DISP_HALF_WIN][2].top    = DISP_H3;
    disp_window[DISP_HALF_WIN][2].width  = DISP_W4;
    disp_window[DISP_HALF_WIN][2].height = DISP_H3;
    disp_window[DISP_HALF_WIN][2].border_top    = 0;
    disp_window[DISP_HALF_WIN][2].border_bottom = 16;

    disp_window[DISP_HALF_WIN][3].left   = DISP_W - DISP_W4;
    disp_window[DISP_HALF_WIN][3].top    = DISP_H3 * 2;
    disp_window[DISP_HALF_WIN][3].width  = DISP_W4;
    disp_window[DISP_HALF_WIN][3].height = DISP_H3;
    disp_window[DISP_HALF_WIN][3].border_top    = 0;
    disp_window[DISP_HALF_WIN][3].border_bottom = 16;

    //DISP_VIDEO0
    disp_window[DISP_VIDEO0][0].width  = DISP_W;
    disp_window[DISP_VIDEO0][0].height = 0;

    disp_window[DISP_VIDEO0][1].width  = (u16) - 1;
    disp_window[DISP_VIDEO0][1].height = 0;

    disp_window[DISP_VIDEO0][2].width  = (u16) - 1;
    disp_window[DISP_VIDEO0][2].height = 0;

    disp_window[DISP_VIDEO0][3].width  = (u16) - 1;
    disp_window[DISP_VIDEO0][3].height = 0;

    //DISP_VIDEO1
    disp_window[DISP_VIDEO1][1].width  = DISP_W;
    disp_window[DISP_VIDEO1][1].height = 0;

    disp_window[DISP_VIDEO1][0].width  = (u16) - 1;
    disp_window[DISP_VIDEO1][0].height = 0;

    disp_window[DISP_VIDEO1][2].width  = (u16) - 1;
    disp_window[DISP_VIDEO1][2].height = 0;

    disp_window[DISP_VIDEO1][3].width  = (u16) - 1;
    disp_window[DISP_VIDEO1][3].height = 0;

    //DISP_VIDEO2
    disp_window[DISP_VIDEO2][2].width  = DISP_W;
    disp_window[DISP_VIDEO2][2].height = 0;

    disp_window[DISP_VIDEO2][0].width  = (u16) - 1;
    disp_window[DISP_VIDEO2][0].height = 0;

    disp_window[DISP_VIDEO2][1].width  = (u16) - 1;
    disp_window[DISP_VIDEO2][1].height = 0;

    disp_window[DISP_VIDEO2][3].width  = (u16) - 1;
    disp_window[DISP_VIDEO2][3].height = 0;

    //DISP_VIDEO3
    disp_window[DISP_VIDEO3][3].width  = DISP_W;
    disp_window[DISP_VIDEO3][3].height = 0;

    disp_window[DISP_VIDEO3][0].width  = (u16) - 1;
    disp_window[DISP_VIDEO3][0].height = 0;

    disp_window[DISP_VIDEO3][1].width  = (u16) - 1;
    disp_window[DISP_VIDEO3][1].height = 0;

    disp_window[DISP_VIDEO3][2].width  = (u16) - 1;
    disp_window[DISP_VIDEO3][2].height = 0;

    //DISP_PARK_WIN
    disp_window[DISP_PARK_WIN][1].width  = DISP_W;
    disp_window[DISP_PARK_WIN][1].height = 0;

    disp_window[DISP_PARK_WIN][0].width  = (u16) - 1;
    disp_window[DISP_PARK_WIN][0].height = 0;

    disp_window[DISP_PARK_WIN][2].width  = (u16) - 1;
    disp_window[DISP_PARK_WIN][2].height = 0;

    disp_window[DISP_PARK_WIN][3].width  = (u16) - 1;
    disp_window[DISP_PARK_WIN][3].height = 0;
}


#else
static struct video_window disp_window[DISP_MAX_WIN][4] = {0};

static void video_set_disp_window()
{
    u16 small_screen_w;
    u16 small_screen_h;

    // 根据LCD设备的宽度选择小窗口的宽度和高度
    if (LCD_DEV_WIDTH >= 1280) {
        small_screen_w = 432; // 当LCD宽度大于1280时，小窗口宽度设置为480，16像素对齐
        small_screen_h = 256; // 高度设置为400，16像素对齐
    } else if (LCD_DEV_WIDTH > 480) {
        small_screen_w = 320; // 当LCD宽度介于480和1280之间时，小窗口宽度设置为320，16像素对齐
        small_screen_h = 240; // 高度设置为240，16像素对齐
    } else {
        small_screen_w = 192; // 当LCD宽度小于或等于480时，小窗口宽度设置为192，16像素对齐
        small_screen_h = 160; // 高度设置为160，16像素对齐
    }

    // 设置双摄同窗（DISP_MAIN_WIN）
    disp_window[DISP_MAIN_WIN][1].width  = SCREEN_W; // 小窗口宽度
    disp_window[DISP_MAIN_WIN][1].height = 720;//small_screen_h; // 小窗口高度
    disp_window[DISP_MAIN_WIN][1].top    = 40;  // 小窗口的上边距设置为0;
    disp_window[DISP_MAIN_WIN][2].width  = small_screen_w; // 第三个窗口的宽度设置为小窗口宽度
    disp_window[DISP_MAIN_WIN][2].height = small_screen_h; // 第三个窗口的高度设置为小窗口高度
    disp_window[DISP_MAIN_WIN][2].left   = SCREEN_W - small_screen_w-95; // 第三个窗口的左边距设置为屏幕宽度减去小窗口宽度
    disp_window[DISP_MAIN_WIN][2].top    = 40;  //
    disp_window[DISP_MAIN_WIN][3].width  = 16; // 小窗口宽度
    disp_window[DISP_MAIN_WIN][3].height = 16; // 小窗口高度
    disp_window[DISP_MAIN_WIN][3].top    = 0;  // 小窗口的上边距设置为0;

    //单路全屏
    disp_window[DISP_FRONT_WIN][1].width=SCREEN_W;
    disp_window[DISP_FRONT_WIN][1].height=720;
    disp_window[DISP_FRONT_WIN][1].top    = 40;
    disp_window[DISP_FRONT_WIN][2].width=(u16)-1;
    disp_window[DISP_FRONT_WIN][2].height=0;

    //other



}

#endif

// 设置显示镜像
 void video_set_disp_mirror(int id, u16 mirror)
{
    // 遍历所有显示窗口，设置指定ID的窗口的镜像属性
    for (int i = 0; i < DISP_MAX_WIN; i++) {
        disp_window[i][id].mirror = mirror; // 将镜像值赋给相应窗口
    }
}




static const char *rec_dir[][2] = {
#ifdef CONFIG_EMR_DIR_ENABLE
    {CONFIG_REC_DIR_0, CONFIG_EMR_REC_DIR_0},
    {CONFIG_REC_DIR_1, CONFIG_EMR_REC_DIR_1},
    {CONFIG_REC_DIR_2, CONFIG_EMR_REC_DIR_2},
    {CONFIG_REC_DIR_3, CONFIG_EMR_REC_DIR_3},
#else
    {CONFIG_REC_DIR_0, CONFIG_REC_DIR_0},
    {CONFIG_REC_DIR_1, CONFIG_REC_DIR_1},
    {CONFIG_REC_DIR_2, CONFIG_REC_DIR_2},
    {CONFIG_REC_DIR_3, CONFIG_REC_DIR_3},
#endif
};

static const char *rec_path[][2] = {
#ifdef CONFIG_EMR_DIR_ENABLE
    {CONFIG_REC_PATH_0, CONFIG_EMR_REC_PATH_0},
    {CONFIG_REC_PATH_1, CONFIG_EMR_REC_PATH_1},
    {CONFIG_REC_PATH_2, CONFIG_EMR_REC_PATH_2},
    {CONFIG_REC_PATH_3, CONFIG_EMR_REC_PATH_3},
#else
    {CONFIG_REC_PATH_0, CONFIG_REC_PATH_0},
    {CONFIG_REC_PATH_1, CONFIG_REC_PATH_1},
    {CONFIG_REC_PATH_2, CONFIG_REC_PATH_2},
    {CONFIG_REC_PATH_3, CONFIG_REC_PATH_3},
#endif
};


#if NAME_FILE_BY_DATE
static int __get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (fd) {
        dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
        dev_close(fd);
        return 0;
    }

    return -EINVAL;
}
#endif

static const char *rec_file_name(int format)
{
#if NAME_FILE_BY_DATE
    struct sys_time time;
    static char file_name[MAX_FILE_NAME_LEN];

    if (__get_sys_time(&time) == 0) {
        if (format == VIDEO_FMT_AVI) {
            sprintf(file_name, "VID_%d%02d%02d_%02d%02d%02d.AVI",
                    time.year, time.month, time.day, time.hour, time.min, time.sec);
        } else if (format == VIDEO_FMT_MOV) {
            sprintf(file_name, "VID_%d%02d%02d_%02d%02d%02d.MOV",
                    time.year, time.month, time.day, time.hour, time.min, time.sec);
        }
        return file_name;
    }
#endif

    if (format == VIDEO_FMT_AVI) {
        return "VID****.AVI";
    } else {
        return "VID****.MOV";
    }
}




u32 get_video_disp_state()
{
    return __this->disp_state;
}

static void video_home_post_msg(const char *msg, ...)
{
#ifdef CONFIG_UI_STYLE_JL02_ENABLE
    union uireq req;
    va_list argptr;

    va_start(argptr, msg);

    if (__this->ui) {
        req.msg.receiver = ID_WINDOW_MAIN_PAGE;
        req.msg.msg = msg;
        req.msg.exdata = argptr;

        server_request(__this->ui, UI_REQ_MSG, &req);
    }

    va_end(argptr);

#endif
}



void video_parking_post_msg(const char *msg, ...)
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;
    va_list argptr;

    va_start(argptr, msg);

    if (__this->ui) {
        req.msg.receiver = ID_WINDOW_PARKING;
        req.msg.msg = msg;
        req.msg.exdata = argptr;

        server_request(__this->ui, UI_REQ_MSG, &req);
    }

    va_end(argptr);
#endif
}

void video_rec_post_msg(const char *msg, ...)
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;
    va_list argptr;

    va_start(argptr, msg);

    if (__this->ui) {
        req.msg.receiver = ID_WINDOW_VIDEO_REC;
        req.msg.msg = msg;
        req.msg.exdata = argptr;

        server_request(__this->ui, UI_REQ_MSG, &req);
    }

    va_end(argptr);

#endif

}


static int video_rec_online_nums()
{
    u8 nums = 0;

    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        if (__this->video_online[i]) {
            nums++;
        }
    }

    return nums;
}

static int video_disp_pause(int id)
{
    union video_req req = {0};
    req.display.state = VIDEO_STATE_PAUSE;

    return server_request(__this->video_display[id], VIDEO_REQ_DISPLAY, &req);
}

static int video_disp_start(int id, const struct video_window *win)
{
    int err = 0;
    union video_req req = {0};
    static char dev_name[20];
#ifdef CONFIG_DISPLAY_ENABLE

    log_d("video_disp_start: %d, %d x %d\n", id, win->width, win->height);

    if (win->width == (u16) - 1) {
        puts("video_disp_hide\n");
        return 0;
    }

#ifdef CONFIG_VIDEO4_ENABLE
    sprintf(dev_name, "video%d.%d.%d", 4, id, 0);
#else
    sprintf(dev_name, "video%d.%d", id, id < 3 ? 0 : __this->uvc_id);
#endif

    if (!__this->video_display[id]) {
        __this->video_display[id] = server_open("video_server", (void *)dev_name);
        if (!__this->video_display[id]) {
            log_e("open video_server: faild, id = %d\n", id);
            return -EFAULT;
        }
    }


    req.display.fb 		        = "fb1";
    req.display.left  	        = win->left;
    req.display.top 	        = win->top;
    req.display.width 	        = win->width;
    req.display.height 	        = win->height;
    req.display.border_left     = win->border_left;
    req.display.border_top      = win->border_top;
    req.display.border_right    = win->border_right;
    req.display.border_bottom   = win->border_bottom;
    req.display.mirror   		= db_select("mir");
    req.display.jaggy			= 0;	// IMC 抗锯齿

    if (id == 0) {
        req.display.camera_config   = NULL;
        /* req.display.camera_config   = load_default_camera_config; */
        req.display.camera_type     = VIDEO_CAMERA_NORMAL;
    } else if ((id == 1) || (id == 2)) {
        /*if (req.display.width < 1280) {
            req.display.width 	+= 32;
            req.display.height 	+= 32;

            req.display.border_left   = 16;
            req.display.border_top    = 16;
            req.display.border_right  = 16;
            req.display.border_bottom = 16;
        }*/

        req.display.camera_config   = NULL;
        req.display.camera_type     = VIDEO_CAMERA_NORMAL;
    } else if (id == 3) {
        /* #ifdef THREE_WAY_ENABLE */
#if 0
        struct uvc_capability uvc_cap;

        req.display.three_way_type = VIDEO_THREE_WAY_JPEG;
        void *uvc_fd = dev_open("uvc", (void *)__this->uvc_id);
        if (!uvc_fd) {
            printf("uvc dev_open err!!!\n");
            return 0;
        }
        dev_ioctl(uvc_fd, UVCIOC_QUERYCAP, (unsigned int)&uvc_cap);
        int uvc_w = uvc_cap.reso[0].width;
        int uvc_h = uvc_cap.reso[0].height;
        dev_close(uvc_fd);

        if ((win->width > uvc_w * 2) || (win->height > uvc_h * 2)) {
            printf("uvc dev_open err b!!!\n");
            return 0;
        }

        req.display.width = (win->width > uvc_w) ? (uvc_w * 2) : uvc_w;
        req.display.border_left   = (req.display.width - win->width) / 2;
        req.display.border_right  = req.display.border_left;

        req.display.height = (win->height > uvc_h) ? (uvc_h * 2) : uvc_h;
        req.display.border_top = (req.display.height - win->height) / 2;
        req.display.border_bottom = req.display.border_top;
#else
        /*if (req.display.width < 1280) {
            req.display.width 	+= 32;
            req.display.height 	+= 32;

            req.display.border_left   = 16;
            req.display.border_top    = 16;
            req.display.border_right  = 16;
            req.display.border_bottom = 16;
        }*/
#endif
        req.display.uvc_id = __this->uvc_id;
        req.display.camera_config = NULL;
        req.display.camera_type = VIDEO_CAMERA_UVC;
        req.display.src_w = __this->src_width[3];
        req.display.src_h = __this->src_height[3];
        //旋转参数配置:
        //0:不旋转,不镜像 (原图)
        //1:逆时针旋转90度,不镜像
        //2:逆时针旋转270度,不镜像
        //3:逆时针旋转90度后,再垂直镜像
        //4:逆时针旋转90度后,再水平镜像
        req.display.rotate = 0; //usb后视频图像旋转显示
        req.display.rotate = win ->rotate;
    }

#ifdef CONFIG_VIDEO4_ENABLE
    req.display.camera_config   = NULL;
    req.display.camera_type     = VIDEO_CAMERA_MUX;
#endif

    req.display.state 	        = VIDEO_STATE_START;
    req.display.pctl            = NULL;

    sys_key_event_disable();
    sys_touch_event_disable();
    err = server_request(__this->video_display[id], VIDEO_REQ_DISPLAY, &req);
    if (err) {
        printf("display req err = %d!!\n", err);
        server_close(__this->video_display[id]);
        __this->video_display[id] = NULL;
    }
#ifndef CONFIG_VIDEO4_ENABLE

    video_rec_start_isp_scenes();

    if (id == 0) {
        /*rec显示重设曝光补偿*/
        __this->exposure_set = 1;
        video_rec_set_exposure(db_select("exp"));
    }
#endif
#endif

    if(win->width<=640){
        disp_state=DISP_DOUBLE_HALF;
    }else{
        if(id==1){
            disp_state=DISP_720AHD_FULL;
        }else if(id==2){
            disp_state=DISP_1080AHD_FULL;
        }else if(id==3){
            disp_state=DISP_UVC_FULL;
        }
    }
    sys_key_event_enable();
    sys_touch_event_enable();

    return err;
}


 void video_disp_stop(int id)
{
#ifdef CONFIG_DISPLAY_ENABLE
    union video_req req = {0};  // 初始化视频请求结构体，用于发送停止显示的请求

    if (__this->video_display[id]) {  // 检查指定ID的视频显示服务是否已打开
        if (id == 0) {
            video_rec_stop_isp_scenes(1, 0);  // 如果ID为0，停止ISP（图像信号处理）场景的录像
        }

        req.display.state = VIDEO_STATE_STOP;  // 设置显示状态为停止
        server_request(__this->video_display[id], VIDEO_REQ_DISPLAY, &req);  // 向视频显示服务发送停止显示请求

        server_close(__this->video_display[id]);  // 关闭视频显示服务
        __this->video_display[id] = NULL;  // 将视频显示指针置为空，表示服务已关闭

        if (id == 0) {
            video_rec_start_isp_scenes();  // 如果ID为0，重新启动ISP场景录像
        }
    }
#endif
}


static int video_disp_win_switch(int mode, int dev_id)
{
    int i;
    int err = 0;
    int next_win = 0;
    int curr_win = __this->disp_state;
    static u16 mirror = 0;

#ifdef CONFIG_DISPLAY_ENABLE
    switch (mode) {
    case DISP_WIN_SW_SHOW_PARKING:
        // 如果停车显示模式被选择且当前选择的停车视频源不可用，返回设备不存在的错误码。
        if (!__this->video_online[__this->disp_park_sel]) {
            return -ENODEV;
        }
        next_win = DISP_PARK_WIN;
        break;
    case DISP_WIN_SW_HIDE_PARKING:
        // 隐藏停车显示模式时，保持当前窗口，并将当前窗口设为停车显示窗口。
        next_win = curr_win;
        curr_win = DISP_PARK_WIN;
        break;
    case DISP_WIN_SW_SHOW_SMALL:
        // 显示小窗口时，设置当前和下一个窗口为主窗口。
        curr_win = DISP_MAIN_WIN;
        next_win = DISP_MAIN_WIN;
        break;
    case DISP_WIN_SW_SHOW_NEXT:
        // 如果只有一个摄像头在线或停车状态为1，则退出。
        if (video_rec_online_nums() < 2) {
            return 0;
        }
        if (get_parking_status() == 1) {
            curr_win = DISP_MAIN_WIN;
            next_win = DISP_MAIN_WIN;
            return 0;
        }
        // 否则，切换到下一个窗口，如果超出范围则返回主窗口。
        next_win = curr_win;
        if (++next_win >= DISP_PARK_WIN) {
            next_win = DISP_MAIN_WIN;
        }
        break;
    case DISP_WIN_SW_DEV_IN:
        // 如果当前窗口不是主窗口，退出，否则保持当前窗口。
        if (curr_win != DISP_MAIN_WIN) {
            return 0;
        }
        next_win = curr_win;
        break;
    case DISP_WIN_SW_DEV_OUT:
        // 当设备移除时，将下一个窗口设置为主窗口。
        next_win = DISP_MAIN_WIN;
        break;
    case DISP_WIN_SW_MIRROR:
        // 切换镜像模式，调用函数设置镜像，并保持当前窗口。
        mirror = !mirror;
        video_set_disp_mirror(3, mirror);
        next_win = curr_win;
        break;
    default:
        // 如果模式无效，返回错误码。
        return -EINVAL;
    }

    printf("disp_win_switch: %d, %d\n", curr_win, next_win);//当前窗口，下一个窗口

    // 停止所有显示视频
    // for (i = 1; i < CONFIG_VIDEO_REC_NUM; i++) {
    //     video_disp_stop(i);
    // }

    // 如果当前窗口与下一个窗口不同，或者切换了镜像模式，重新启动显示。
    if (curr_win != next_win || mode == DISP_WIN_SW_MIRROR) {
        video_disp_stop(0);
        err = video_disp_start(0, &disp_window[next_win][0]);
    }

    // 为在线的每个视频设备重新启动显示。
//     for (i = 1; i < CONFIG_VIDEO_REC_NUM; i++) {
//         if (__this->video_online[i]) {
// #ifdef CONFIG_VIDEO4_ENABLE
//             err = video_disp_start(i, &disp_window[next_win][i]);
// #elif defined THREE_WAY_ENABLE
//             err = video_disp_start(i, &disp_window[next_win][i]);
// #else
//             err = video_disp_start(i, &disp_window[next_win][1]);
//             if (err == 0) {
//                 break;
//             }
// #endif
//         }
//     }

    // 如果不是停车模式，将当前显示状态更新为下一个窗口。
    if (next_win != DISP_PARK_WIN) {
        __this->disp_state = next_win;
    }

#ifndef CONFIG_VIDEO4_ENABLE
    // 如果当前显示状态为后视窗口，关闭前照灯并设置屏显场景为1。
    if (__this->disp_state == DISP_BACK_WIN) {
        video_rec_post_msg("HlightOff");
        isp_scr_work_hdl(1);
    }
#endif

#endif

    return err;
}




static void rec_dev_server_event_handler(void *priv, int argc, int *argv)
{
    /*
     *该回调函数会在录像过程中，写卡出错被当前录像APP调用，例如录像过程中突然拔卡
     */
    switch (argv[0]) {
    case VIDEO_SERVER_UVM_ERR:
        log_e("APP_UVM_DEAL_ERR\n");
        break;
    case VIDEO_SERVER_PKG_ERR:
        if (__this->state == VIDREC_STA_START) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
            video_rec_err_notify("VIDEO_REC_ERR");
#endif
            video_rec_stop(0);
        }
        break;
    case VIDEO_SERVER_PKG_END:
        /* break; */
        /* video_rec_savefile((int)priv); */
        /* break; */
        if (db_select("cyc")) {
            video_rec_savefile((int)priv);
        } else {
            video_rec_stop(0);
        }
        break;
    default :
        log_e("unknow rec server cmd %x , %x!\n", argv[0], (int)priv);
        break;
    }
}



extern void play_voice_file(const char *file_name);
static void ve_server_event_handler(void *priv, int argc, int *argv)
{
    switch (argv[0]) {
    case VE_MSG_MOTION_DETECT_STILL:
        /*
         *录像时，移动侦测打开的情况下，画面基本静止20秒，则进入该分支
         */
        printf("**************VE_MSG_MOTION_DETECT_STILL**********\n");
        if (!db_select("mot") || (__this->menu_inout)) {
            return;
        }
        if (__this->state == VIDREC_STA_START && __this->user_rec == 0) {
            video_rec_stop(0);
        }

        break;
    case VE_MSG_MOTION_DETECT_MOVING:
        /*
         *移动侦测打开，当检测到画面活动一段时间，则进入该分支去启动录像
         */
        printf("**************VE_MSG_MOTION_DETECT_MOVING**********\n");
        if (!db_select("mot") || (__this->menu_inout)) {
            return;
        }
        if ((__this->state == VIDREC_STA_STOP) || (__this->state == VIDREC_STA_IDLE)) {//录像停止或处于空闲
            // video_rec_start();
        }

        break;
    case VE_MSG_LANE_DETECT_WARNING:

        if (!__this->lan_det_setting) {
            if (!db_select("lan")) {
                return;
            }
        }

        play_voice_file("mnt/spiflash/audlogo/lane.adp");

        puts("==lane dete waring==\n");
        break;
    case VE_MSG_LANE_DETCET_LEFT:
        puts("==lane dete waring==l\n");
        break;
    case VE_MSG_LANE_DETCET_RIGHT:
        puts("==lane dete waring==r\n");
        break;
    case VE_MSG_VEHICLE_DETECT_WARNING:
        //printf("x = %d,y = %d,w = %d,hid = %d\n",argv[1],argv[2],argv[3],argv[4]);
        //位置
        video_rec_post_msg("carpos:p=%4", ((u32)(argv[1]) | (argv[2] << 16))); //x:x y:y
        //颜色
        if (argv[3] > 45) {
            video_rec_post_msg("carpos:w=%4", ((u32)(argv[3]) | (3 << 16)));    //w:width c:color,0:transparent, 1:green,2:yellow,3:red
        } else {
            video_rec_post_msg("carpos:w=%4", ((u32)(argv[3]) | (1 << 16)));
        }
        //隐藏
        if (argv[4] == 0) {
            video_rec_post_msg("carpos:w=%4", ((u32)(1) | (0 << 16)));
        }
        //刷新
        video_rec_post_msg("carpos:s=%4", 1);
        break;
    default :
        break;
    }
}

/*
 *智能引擎服务打开，它包括移动侦测等一些功能,在打开这些功能之前，必须要打开这个智能引擎服务
 */
static s32 ve_server_open(u8 fun_sel)
{
    //printf("-----lane start\n");
    //printf("-----fun_sel = %d\n",fun_sel);
#ifdef CONFIG_VIDEO4_ENABLE
    return 0;
#endif
    if (!__this->video_engine) {
        __this->video_engine = server_open("video_engine_server", NULL);
        if (!__this->video_engine) {
            puts("video_engine_server:faild\n");
            return -1;
        }

        // server_register_event_handler(__this->video_engine, NULL, ve_server_event_handler);

        struct video_engine_req ve_req;
        ve_req.module = 0;
        ve_req.md_mode = 0;
        ve_req.cmd = 0;
        /* ve_req.hint_info.hint = ((1 << VE_MODULE_MOTION_DETECT) | (1 << VE_MODULE_LANE_DETECT) | (1 << VE_MODULE_FACE_DETECT)); */
        ve_req.hint_info.hint = ((1 << VE_MODULE_MOTION_DETECT) | (1 << VE_MODULE_LANE_DETECT));

        //  if (fun_sel) {
        //      ve_req.hint_info.hint = 0;
        //      ve_req.hint_info.hint = (1 << VE_MODULE_LANE_DETECT);
        //  }

#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
        ve_req.hint_info.mode_hint0 = (VE_MOTION_DETECT_MODE_ISP
                                       << (VE_MODULE_MOTION_DETECT * 4));
#else
        ve_req.hint_info.mode_hint0 = (VE_MOTION_DETECT_MODE_NORMAL
                                       << (VE_MODULE_MOTION_DETECT * 4));
#endif

        ve_req.hint_info.mode_hint1 = 0;
        server_request(__this->video_engine, VE_REQ_SET_HINT, &ve_req);
    }



    if (fun_sel) {

        ve_face_det_start(1);

        //ve_mdet_start();//@kyj
        //ve_lane_det_start(1);

    } else {
        //printf("--------------server open %d\n",fun_sel);
        __this->car_head_y = 351;//db_select("lan") & 0x0000ffff;
        __this->vanish_y   = 0;//(db_select("lan") >> 16) & 0x0000ffff;
        ve_mdet_start();
        ve_lane_det_start(0);
    }

    return 0;
}

static s32 ve_server_close()
{
    if (__this->video_engine) {

        if (!__this->lan_det_setting) {
            ve_mdet_stop();
        }
        ve_lane_det_stop(0);

        server_close(__this->video_engine);

        __this->video_engine = NULL;
    }
    return 0;
}

static int ve_mdet_start()
{
    struct video_engine_req ve_req;

#ifdef CONFIG_VIDEO4_ENABLE
    return -EINVAL;
#endif
    //@kyj
    // if ((__this->video_engine == NULL) || !db_select("mot")) {
    //     return -EINVAL;
    // }

    ve_req.module = VE_MODULE_MOTION_DETECT;
#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
    ve_req.md_mode = VE_MOTION_DETECT_MODE_ISP;
#else
    ve_req.md_mode = VE_MOTION_DETECT_MODE_NORMAL;
#endif
    ve_req.cmd = 0;

    server_request(__this->video_engine, VE_REQ_MODULE_OPEN, &ve_req);


    server_request(__this->video_engine, VE_REQ_MODULE_GET_PARAM, &ve_req);


    /*
    *移动侦测的检测启动时间和检测静止的时候
    **/
    ve_req.md_params.level = 2;
    ve_req.md_params.move_delay_ms = MOTION_START_SEC * 1000;
    ve_req.md_params.still_delay_ms = MOTION_STOP_SEC * 1000;
    server_request(__this->video_engine, VE_REQ_MODULE_SET_PARAM, &ve_req);

    server_request(__this->video_engine, VE_REQ_MODULE_START, &ve_req);

    return 0;
}


static int ve_mdet_stop()
{
    struct video_engine_req ve_req;

    // 如果视频引擎为空或者数据库中未找到 "mot" 相关条目，则返回无效参数错误
    if ((__this->video_engine == NULL) || !db_select("mot")) {
        return -EINVAL;
    }

    // 设置请求的模块为运动检测模块
    ve_req.module = VE_MODULE_MOTION_DETECT;

    // 根据编译配置选择运动检测模式
#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
    ve_req.md_mode = VE_MOTION_DETECT_MODE_ISP;   // 设置为 ISP 模式
#else
    ve_req.md_mode = VE_MOTION_DETECT_MODE_NORMAL; // 设置为普通模式
#endif

    ve_req.cmd = 0; // 设置命令为 0，通常表示停止操作

    // 发送请求以停止运动检测模块
    server_request(__this->video_engine, VE_REQ_MODULE_STOP, &ve_req);

    // 发送请求以关闭运动检测模块
    server_request(__this->video_engine, VE_REQ_MODULE_CLOSE, &ve_req);

    return 0; // 返回 0 表示操作成功
}


static void ve_mdet_reset()
{
    ve_mdet_stop();
    ve_mdet_start();
}


static int ve_lane_det_start(u8 fun_sel)
{
    struct video_engine_req ve_req;

#ifdef CONFIG_VIDEO4_ENABLE
    return -EINVAL;
#endif
    if (!fun_sel) {
        if ((__this->video_engine == NULL) || !db_select("lan")) {
            return -EINVAL;
        }
    }

    ve_req.module = VE_MODULE_LANE_DETECT;
#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
    ve_req.md_mode = VE_MOTION_DETECT_MODE_ISP;
#else
    ve_req.md_mode = VE_MOTION_DETECT_MODE_NORMAL;
#endif

    ve_req.cmd = 0;
    server_request(__this->video_engine, VE_REQ_MODULE_OPEN, &ve_req);

    server_request(__this->video_engine, VE_REQ_MODULE_GET_PARAM, &ve_req);

    /**
     *轨道偏移 配置车头 位置，视线结束为止，以及车道宽度
     * */
    /* ve_req.lane_detect_params.car_head_y = 230; */
    /* ve_req.lane_detect_params.vanish_y = 170; */
    /* ve_req.lane_detect_params.len_factor = 0; */
    ve_req.lane_detect_params.car_head_y = __this->car_head_y;
    ve_req.lane_detect_params.vanish_y =  __this->vanish_y;
    ve_req.lane_detect_params.len_factor = 0;

    server_request(__this->video_engine, VE_REQ_MODULE_SET_PARAM, &ve_req);

    server_request(__this->video_engine, VE_REQ_MODULE_START, &ve_req);

    return 0;
}


static int ve_lane_det_stop(u8 fun_sel)
{
    struct video_engine_req ve_req;

    // 如果 fun_sel 为 0，则需要检查 video_engine 是否为空以及数据库中是否有 "lan" 条目
    if (!fun_sel) {
        if ((__this->video_engine == NULL) || !db_select("lan")) {
            return -EINVAL; // 如果条件不满足，则返回无效参数错误
        }
    }

    // 设置请求的模块为车道检测模块
    ve_req.module = VE_MODULE_LANE_DETECT;

    // 根据编译配置选择运动检测模式
#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
    ve_req.md_mode = VE_MOTION_DETECT_MODE_ISP;   // 设置为 ISP 模式
#else
    ve_req.md_mode = VE_MOTION_DETECT_MODE_NORMAL; // 设置为普通模式
#endif

    ve_req.cmd = 0; // 设置命令为 0，通常表示停止操作

    // 发送请求以停止车道检测模块
    server_request(__this->video_engine, VE_REQ_MODULE_STOP, &ve_req);

    // 发送请求以关闭车道检测模块
    server_request(__this->video_engine, VE_REQ_MODULE_CLOSE, &ve_req);

    return 0; // 返回 0 表示操作成功
}

void ve_lane_det_reset()
{
    // 重置车道检测功能
    ve_lane_det_stop(0);   // 停止车道检测
    ve_lane_det_start(0);  // 启动车道检测
    ve_face_det_start(0);  // 启动人脸检测
}




static int ve_face_det_start(u8 fun_sel)
{
    struct video_engine_req ve_req;

    return 0;
    if (!fun_sel) {
        if (__this->video_engine == NULL) {
            return -EINVAL;
        }
    }


    ve_req.module = VE_MODULE_FACE_DETECT;
#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
    ve_req.md_mode = VE_MOTION_DETECT_MODE_ISP;
#else
    ve_req.md_mode = VE_MOTION_DETECT_MODE_NORMAL;
#endif



    ve_req.cmd = 0;
    server_request(__this -> video_engine, VE_REQ_MODULE_OPEN, &ve_req);
    server_request(__this -> video_engine, VE_REQ_MODULE_GET_PARAM, &ve_req);

    ve_req.face_detect_params.roi_top = 0;
    ve_req.face_detect_params.roi_bottom = 351;

    server_request(__this -> video_engine, VE_REQ_MODULE_SET_PARAM, &ve_req);
    server_request(__this -> video_engine, VE_REQ_MODULE_START, &ve_req);

    return 0;

}


static int ve_face_det_stop(u8 fun_sel)
{
    struct video_engine_req ve_req;

    if (!fun_sel) {
        if ((__this -> video_engine == NULL) || !db_select("fac")) {
            return -EINVAL;
        }
    }


    ve_req.module = VE_MODULE_FACE_DETECT;
#ifdef CONFIG_VE_MOTION_DETECT_MODE_ISP
    ve_req.md_mode = VE_MOTION_DETECT_MODE_ISP;
#else
    ve_req.md_mode = VE_MOTION_DETECT_MODE_NORMAL;
#endif

    ve_req.cmd = 0;

    server_request(__this -> video_engine, VE_REQ_MODULE_STOP, &ve_req);
    server_request(__this -> video_engine, VE_REQ_MODULE_CLOSE, &ve_req);


    return 0;

}


void ve_face_det_reset()
{
    ve_face_det_stop(0);
    ve_face_det_start(0);
}



void ve_server_reopen()
{
    ve_mdet_stop();
    ve_lane_det_stop(0);

    ve_server_close();
    ve_server_open(0);
}

/*
 * 判断SD卡是否挂载成功和簇大小是否大于32K
 */
static int storage_device_available()
{
    struct vfs_partition *part;

    // 检查存储设备是否已准备好
    if (storage_device_ready() == 0) {
        // 如果设备未准备好，检查设备是否在线
        if (!dev_online(SDX_DEV)) {
            // 如果SD卡设备不在线，发送 "noCard" 消息
            video_rec_post_msg("noCard");
        } else {
            // 如果设备在线但不可用，发送 "fsErr" 消息
            video_rec_post_msg("fsErr");
        }
        return false;  // 返回false表示设备不可用
    } else {
        // 获取当前设备的分区信息
        part = fget_partition(CONFIG_ROOT_PATH);
        printf("part_fs_attr: %x\n", part->fs_attr);

        // 检查簇大小是否小于32，或者文件系统是否为只读
        if (part->clust_size < 32 || (part->fs_attr & F_ATTR_RO)) {
            // 如果条件成立，发送 "fsErr" 消息并返回false
            video_rec_post_msg("fsErr");
            return false;
        }
        // 记录总存储大小
        __this->total_size = part->total_size;
    }

    return true;  // 返回true表示设备可用
}


/*码率控制，根据具体分辨率设置*/
static int video_rec_get_abr(u32 width)
{
#if (APP_CASE ==  __CAR_CAMERA__)
    if (width <= 720) {
        return 1000;
    } else if (width <= 1280) {
        return 4000;
    } else if (width <= 1920) {
        return 10000;
    } else {
        return 10000;
    }
#endif


#if (APP_CASE ==  __WIFI_CAR_CAMERA__)
    if (width <= 720) {
        return 1000;
    } else if (width <= 1280) {
        return 2000;
    } else if (width <= 1920) {
        return 3000;
    } else {
        return 4000;
    }
#endif

#if  (APP_CASE == __WIFI_IPCAM__)
    if (width <= 720) {
        return 1000;
    } else if (width <= 1280) {
        return 1000;
    } else if (width <= 1920) {
        return 1500;
    } else {
        return 2000;
    }
#endif

#if  (APP_CASE == __WIFI_DOORBELL__)
    if (width <= 720) {
        return 1000;
    } else if (width <= 1280) {
        return 1000;
    } else if (width <= 1920) {
        return 1500;
    } else {
        return 2000;
    }
#endif


}

static void video_rec_get_remain_time(void)
{
    static char retime_buf[30];   // 缓存剩余时间的字符串
    int err;                      // 错误码
    u32 cur_space;                // 当前剩余空间
    u32 one_pic_size;             // 单帧图片的大小
    u32 one_frame_size = 0;       // 单帧视频的大小
    int second = 0;               // 剩余录像时间（秒）
    u32 gap_time = db_select("gap"); // 从数据库读取帧间隔时间

    // 如果获取的帧间隔时间为0，则默认设置为每秒30帧（33毫秒）
    if (!gap_time) {
        gap_time = 1000 / 30;
    }

    /*
     * 这里计算SD卡剩余的录像时间
     */
    if (storage_device_available()) {  // 判断存储设备是否可用
        log_d("calc_free_space...\n");
        err = fget_free_space(CONFIG_ROOT_PATH, &cur_space);  // 获取存储设备的剩余空间
        if (err) {  // 如果获取剩余空间时出错
            if (fget_err_code(CONFIG_ROOT_PATH) == -EIO) {  // 判断是否是输入输出错误
                video_rec_post_msg("fsErr");  // 发送文件系统错误消息
            }
        } else {  // 正常获取到剩余空间
            u32 res = db_select("res");  // 从数据库读取当前录像分辨率
            one_pic_size = video_rec_get_abr(rec_pix_w[res]) / 240 + 1;  // 计算单帧的大小

            /*
             * 如果有多个分辨率模式，这里可以根据具体分辨率选择对应的帧大小
             * 示例代码已经被注释掉，说明此处使用的方式可能是简单化计算
             */

            one_frame_size += one_pic_size;  // 添加单帧大小到总帧大小
#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
            // 如果支持双视频录像且第二路视频在线，计算双路视频的帧大小
            if (__this->video_online[1] && db_select("two")) {
                one_frame_size += video_rec_get_abr(rec_pix_w[VIDEO_RES_1080P]) / 240 + 1;
            }
#endif
#ifdef CONFIG_VIDEO4_ENABLE
            // 如果支持四路视频录像，遍历每一路在线视频并计算其帧大小
            for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
                if (__this->video_online[i]) {
                    one_frame_size += one_pic_size;
                }
            }
#endif
            // 计算剩余时间 = 当前剩余空间 / 每帧大小 * 帧间隔时间（秒）
            second = cur_space / one_frame_size * gap_time / 1000;

            // 将剩余时间格式化为时:分:秒并打印
            printf("retime_buf %02d:%02d:%02d", second / 3600, second % 3600 / 60, second % 60);

            // 发送剩余时间消息
            video_rec_post_msg("Remain:s=%4", ((u32)(second)));
        }
        log_d("calc_free_space_exit\n");  // 退出计算日志
    }
}


/*
 *根据录像不同的时间和分辨率，设置不同的录像文件大小
 */
static u32 video_rec_get_fsize(u8 cycle_time, u16 vid_width, int format)
{
    u32 fsize;

    if (cycle_time == 0) {
        cycle_time = 5;
    }

    fsize = video_rec_get_abr(vid_width) * cycle_time * 8250;

    if (format == VIDEO_FMT_AVI) {
        fsize = fsize + fsize / 4;
    }

    return fsize;
}

static int video_rec_cmp_fname(void *afile, void *bfile)
{
    int alen, blen;  // 用于存储两个文件名的长度
    char afname[MAX_FILE_NAME_LEN];  // 存储第一个文件的文件名
    char bfname[MAX_FILE_NAME_LEN];  // 存储第二个文件的文件名

    // 检查传入的文件指针是否为空，如果其中一个为空，返回 0 表示文件名不同
    if ((afile == NULL) || (bfile == NULL)) {
        return 0;
    }

    // 输出两个文件指针的地址，便于调试时查看传入的指针值
    printf("video_rec_cmp_fname: %p, %p\n", afile, bfile);

    // 获取第一个文件的文件名，并将其存储在 afname 中
    // alen 保存获取到的文件名长度
    alen = fget_name(afile, (u8 *)afname, MAX_FILE_NAME_LEN);

    // 如果获取文件名失败（长度小于等于 0），输出错误日志并返回 0 表示文件名不同
    if (alen <= 0) {
        log_e("fget_name: afile=%x\n", afile);
        return 0;
    }

    // 将第一个文件名的字母转换为大写，以便忽略大小写差异
    ASCII_ToUpper(afname, alen);

    // 获取第二个文件的文件名，并将其存储在 bfname 中
    // blen 保存获取到的文件名长度
    blen = fget_name(bfile, (u8 *)bfname, MAX_FILE_NAME_LEN);

    // 如果获取文件名失败（长度小于等于 0），输出错误日志并返回 0 表示文件名不同
    if (blen <= 0) {
        log_e("fget_name: bfile=%x\n", bfile);
        return 0;
    }

    // 将第二个文件名的字母转换为大写，以便忽略大小写差异
    ASCII_ToUpper(bfname, blen);

    // 输出两个文件名，便于调试时查看文件名的内容
    printf("afname: %s, bfname: %s\n", afname, bfname);

    // 比较两个文件名的长度和内容
    // 如果长度相同且文件名内容一致，返回 1 表示文件名相同
    if (alen == blen && !strcmp(afname, bfname)) {
        return 1;
    }

    // 如果文件名长度不同或内容不一致，返回 0 表示文件名不同
    return 0;
}


static void video_rec_fscan_release(int lock_dir)
{
    printf("video_rec_fscan_release: %d\n", lock_dir);
    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        if (__this->fscan[lock_dir][i]) {
            fscan_release(__this->fscan[lock_dir][i]);
            __this->fscan[lock_dir][i] = NULL;
        }
    }
}

static void video_rec_fscan_dir(int id, int lock_dir, const char *path)
{
    const char *str;
#ifdef CONFIG_EMR_DIR_ENABLE
    str = "-tMOVAVI -sn";
#else
    str = lock_dir ? "-tMOVAVI -sn -ar" : "-tMOVAVI -sn -a/r";
#endif
    if (__this->fscan[lock_dir][id]) {
        if (__this->old_file_number[lock_dir][id] == 0) {
            puts("--------delete_all_scan_file\n");
            fscan_release(__this->fscan[lock_dir][id]);
            __this->fscan[lock_dir][id] = NULL;
        }
    }

    if (!__this->fscan[lock_dir][id]) {
        __this->fscan[lock_dir][id] = fscan(path, str);
        if (!__this->fscan[lock_dir][id]) {
            __this->old_file_number[lock_dir][id] = 0;
        } else {
            __this->old_file_number[lock_dir][id] = __this->fscan[lock_dir][id]->file_number;
        }
        __this->file_number[lock_dir][id] = __this->old_file_number[lock_dir][id];
        printf("fscan_dir: %d, file_number = %d\n", id, __this->file_number[lock_dir][id]);
    }
}

static FILE *video_rec_get_first_file(int id)
{
    int max_index = -1;
    int max_file_number = 0;
    int persent = __this->lock_fsize * 100 / __this->total_size;
    int lock_dir = !!(persent > LOCK_FILE_PERCENT);
    int i;

    log_d("lock_file_persent: %d, size: %dMB\n", persent, __this->lock_fsize / 1024);

#if 1//def CONFIG_VIDEO0_ENABLE
    video_rec_fscan_dir(0, lock_dir, rec_path[0][lock_dir]);
#endif
#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    video_rec_fscan_dir(1, lock_dir, rec_path[1][lock_dir]);
#endif
#ifdef CONFIG_VIDEO3_ENABLE
    video_rec_fscan_dir(3, lock_dir, rec_path[3][lock_dir]);
#endif

#ifdef CONFIG_VIDEO4_ENABLE
    for (i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        video_rec_fscan_dir(i, lock_dir, rec_path[i][lock_dir]);
    }
#endif


    for (i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        if (__this->fscan[lock_dir][i]) {
            if (max_file_number < __this->file_number[lock_dir][i]) {
                max_file_number = __this->file_number[lock_dir][i];
                max_index = i;
            }
        }
    }

    if (max_index < 0) {
        return NULL;
    }
    if (max_index != id && id >= 0) {
        /* 查看优先删除的文件夹是否满足删除条件 */
        if (__this->file_number[lock_dir][id] + 3 > __this->file_number[lock_dir][max_index]) {
            max_index = id;
        }
    }


    log_d("fselect file from dir %d, %d\n", lock_dir, max_index);


    if (__this->fscan[lock_dir][max_index]) {
        FILE *f = fselect(__this->fscan[lock_dir][max_index], FSEL_FIRST_FILE, 0);
        if (f) {

            if (lock_dir == 0) {
                if (video_rec_cmp_fname(__this->file[max_index], f)) {
                    fclose(f);
                    return NULL;
                }
            } else {
                __this->lock_fsize -= flen(f) / 1024;
                log_d("lock fsize - = %d\n", __this->lock_fsize);
            }

            __this->file_number[lock_dir][max_index]--;
            __this->old_file_number[lock_dir][max_index]--;
            if (__this->old_file_number[lock_dir][max_index] == 0) {
                video_rec_fscan_release(lock_dir);
            }
        }
#if (APP_CASE == __WIFI_CAR_CAMERA__)
        video_rec_delect_notify(f, -1);
#endif
        return f;
    } else {
        log_e("fscan[%d][%d] err", lock_dir, max_index);
        return NULL;
    }
    return NULL;
}


static void video_rec_rename_file(int id, FILE *file, int fsize, int format)
{
    char file_name[32];

    __this->new_file[id] = NULL;

    int err = fcheck(file);
    if (err) {
        puts("fcheck fail\n");
#if (APP_CASE == __WIFI_CAR_CAMERA__)
        video_rec_delect_notify(file, -1);
#endif
        fdelete(file);
        return;
    }

    int size = flen(file);
    int persent = (size / 1024) * 100 / (fsize / 1024);

    printf("rename file: persent=%d, %d,%d\n", persent, size >> 20, fsize >> 20);

    if (persent >= 90 && persent <= 110) {
        sprintf(file_name, "%s%s", rec_dir[id][0], rec_file_name(format));

        printf("fmove: %d, %d, %s\n", id, format, file_name);
#if (APP_CASE == __WIFI_CAR_CAMERA__)
        video_rec_delect_notify(file, -1);
#endif
        int err = fmove(file, file_name, &__this->new_file[id], 1);
        if (err == 0) {
            fseek(__this->new_file[id], fsize, SEEK_SET);
            fseek(__this->new_file[id], 0, SEEK_SET);
            return;
        }
        puts("fmove_file_faild\n");
    }

#if (APP_CASE == __WIFI_CAR_CAMERA__)
    video_rec_delect_notify(file, -1);
#endif

    fdelete(file);
}




static int video_rec_create_file(int id, u32 fsize, int format, const char *path)
{
    FILE *file;
    int try_cnt = 0;
    char file_path[64];

    sprintf(file_path, "%s%s", path, rec_file_name(format));

    printf("fopen: %s, %dMB\n", file_path, fsize >> 20);

    do {
        file = fopen(file_path, "w+");
        if (!file) {
            log_e("fopen faild\n");
            break;
        }
        if (fseek(file, fsize, SEEK_SET)) {
            goto __exit;
        }
        log_e("fseek faild\n");
        fdelete(file);

    } while (++try_cnt < 2);

    return -EIO;

__exit:
    fseek(file, 0, SEEK_SET);
    __this->new_file[id] = file;

    return 0;
}



static int video_rec_del_old_file() // 定义一个函数，用于删除旧的视频文件以释放空间
{
    int i; // 循环变量
    int online_num = 0; // 当前在线视频的数量
    int err; // 用于存储错误代码
    FILE *file; // 文件指针，用于操作视频文件
    u32 cur_space; // 当前可用的存储空间
    u32 need_space = 0; // 计算所需的存储空间
    u32 gap_time = db_select("gap"); // 从数据库中选择间隔时间的值
    int cyc_time = db_select("cyc"); // 从数据库中选择周期时间的值

#ifdef THREE_WAY_ENABLE // 如果启用三路视频
    int fsize[4] = {0, 0, 0, 0}; // 存储每个视频文件的大小
    int format[4] = { VIDEO0_REC_FORMAT, VIDEO1_REC_FORMAT, VIDEO2_REC_FORMAT, VIDEO3_REC_FORMAT}; // 存储每个视频文件的格式
#else
    int fsize[4] = {0, 0, 0, 0}; // 同样用于存储文件大小

#ifdef CONFIG_VIDEO4_ENABLE // 如果启用四路视频
    int format[4] = {VIDEO4_REC_FORMAT, VIDEO4_REC_FORMAT, VIDEO4_REC_FORMAT, VIDEO4_REC_FORMAT}; // 所有视频格式都设为VIDEO4_REC_FORMAT
#else
    int format[4] = { VIDEO0_REC_FORMAT, VIDEO1_REC_FORMAT, VIDEO2_REC_FORMAT, VIDEO3_REC_FORMAT}; // 默认设置为三路视频格式
#endif
#endif

#if 1 // 判断视频0是否启用
    if (__this->video_online[2] && !__this->new_file[0]) { // 检查视频0是否在线且没有新文件
        fsize[0] = video_rec_get_fsize(cyc_time, rec_pix_w[db_select("res")], VIDEO0_REC_FORMAT); // 获取视频0的文件大小
        if (gap_time) { // 如果存在间隔时间
            fsize[0] = fsize[0] / (30 * gap_time / 1000); // 根据间隔时间调整文件大小
        }
        need_space += fsize[0]; // 累加所需空间
    }
#endif

    if (db_select("two")) { // 检查是否启用第二路视频
#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
        if (__this->video_online[1] && !__this->new_file[1]) { // 检查视频1是否在线且没有新文件
            fsize[1] =  video_rec_get_fsize(cyc_time, AVIN_WIDTH, VIDEO1_REC_FORMAT); // 获取视频1的文件大小
            if (gap_time) { // 如果存在间隔时间
                fsize[1] = fsize[1] / (30 * gap_time / 1000); // 调整文件大小
            }
            need_space += fsize[1]; // 累加所需空间
        }
#endif
    }

#ifdef CONFIG_VIDEO3_ENABLE // 检查视频3是否启用
    if (__this->video_online[3] && !__this->new_file[3]) { // 检查视频3是否在线且没有新文件
        fsize[3] =  video_rec_get_fsize(cyc_time, UVC_ENC_WIDTH, VIDEO3_REC_FORMAT); // 获取视频3的文件大小
        if (gap_time) { // 如果存在间隔时间
            fsize[3] = fsize[3] / (30 * gap_time / 1000); // 调整文件大小
        }
        need_space += fsize[3]; // 累加所需空间
    }
#endif

#ifdef CONFIG_VIDEO4_ENABLE // 如果启用四路视频
    for (i = 0; i < CONFIG_VIDEO_REC_NUM; i++) { // 循环遍历所有视频
        if (__this->video_online[i] && !__this->new_file[i]) { // 检查当前视频是否在线且没有新文件
            fsize[i] =  video_rec_get_fsize(cyc_time, rec_pix_w[VIDEO_RES_720P], VIDEO4_REC_FORMAT); // 获取文件大小
            if (gap_time) { // 如果存在间隔时间
                fsize[i] = fsize[i] / (25 * gap_time / 1000); // 调整文件大小
            }
            need_space += fsize[i]; // 累加所需空间
        }
    }
#endif

    err = fget_free_space(CONFIG_ROOT_PATH, &cur_space); // 获取当前存储路径下的可用空间
    if (err) {
        return err; // 如果出错，返回错误代码
    }

    printf("space: %dMB, %dMB\n", cur_space / 1024, need_space / 1024 / 1024); // 打印当前可用空间和所需空间

#ifdef CONFIG_VIDEO4_ENABLE
    online_num = ARRAY_SIZE(fsize); // 获取在线视频数量
#else
    online_num = video_rec_online_nums(); // 获取在线视频数量
#endif

    if (cur_space >= (need_space / 1024) * 3) { // 检查当前空间是否至少为所需空间的三倍
        for (i = 0; i < CONFIG_VIDEO_REC_NUM; i++) { // 遍历每个视频
            if (fsize[i] != 0) { // 如果文件大小有效
                err = video_rec_create_file(i, fsize[i], format[i], rec_path[i][0]); // 创建新视频文件
                if (err) {
                    return err; // 如果出错，返回错误代码
                }
            }
        }
        return 0; // 成功创建文件，返回成功
    }

    while (1) { // 循环直到空间足够
        if (cur_space >= (need_space / 1024) * 2) { // 检查当前空间是否至少为所需空间的两倍
            break; // 如果空间足够，退出循环
        }
        file = video_rec_get_first_file(-1); // 获取第一个视频文件
        if (!file) {
            return -ENOMEM; // 如果没有文件，返回内存不足错误
        }
        fdelete(file); // 删除该文件
        fget_free_space(CONFIG_ROOT_PATH, &cur_space); // 更新可用空间
        printf("space: %dMB, %dMB\n", cur_space / 1024, need_space / 1024 / 1024); // 打印空间信息
    }

    for (i = 0; i < CONFIG_VIDEO_REC_NUM; i++) { // 遍历每个视频
        if (fsize[i] != 0) { // 如果文件大小有效
            file = video_rec_get_first_file(i); // 获取该视频的第一个文件
            if (file) {
                video_rec_rename_file(i, file, fsize[i], format[i]); // 重命名文件
            }
            if (!__this->new_file[i]) { // 检查新文件是否已创建
                err = video_rec_create_file(i, fsize[i], format[i], rec_path[i][0]); // 创建新文件
                if (err) {
                    return err; // 如果出错，返回错误代码
                }
            }
        }
    }

    return 0; // 返回成功
}



static int video_rec_scan_lock_file()
{
#ifdef CONFIG_EMR_DIR_ENABLE
    const char *str = "-tMOVAVI -sn";
#else
    const char *str = "-tMOVAVI -sn -ar";
#endif

    __this->lock_fsize = 0;
    for (int i = 0; i < ARRAY_SIZE(rec_path); i++) {
        __this->fscan[1][i] = fscan(rec_path[i][1], str);
        if (__this->fscan[1][i] == NULL) {
            continue;
        }
        int sel_mode = FSEL_FIRST_FILE;
        while (1) {
            FILE *file = fselect(__this->fscan[1][i], sel_mode, 0);
            if (!file) {
                break;
            }
            __this->lock_fsize += (flen(file) / 1024);
            sel_mode = FSEL_NEXT_FILE;
            fclose(file);
        }
    }

    log_d("lock_file_size: %dMB\n", __this->lock_fsize / 1024);

    return 0;
}

/*
 *设置保护文件，必须要在关闭文件之前调用
 */
static int video_rec_lock_file(void *file, u8 lock)
{
    int attr;

    if (!file) {
        puts("lock file null\n");
        return -1;
    }

    fget_attr(file, &attr);

    if (lock) {
        if (attr & F_ATTR_RO) {
            return 0;
        }
        attr |= F_ATTR_RO;
    } else {
        if (!(attr & F_ATTR_RO)) {
            return 0;
        }
        attr &= ~F_ATTR_RO;
    }
    fset_attr(file, attr);

    return 0;
}

static void video_rec_close_file(int dev_id)
{
    if (!__this->file[dev_id]) {
        return;
    }
#if (APP_CASE == __WIFI_CAR_CAMERA__)
    char is_emf = 0;
    char *path = video_rec_finish_get_name(__this->file[dev_id], dev_id, is_emf);
    log_d("video_rec_close_file:%s \n", path);
#endif

    log_i("close file in\n");

    if (__this->gsen_lock & BIT(dev_id)) {
        __this->gsen_lock &= ~BIT(dev_id);
        __this->lock_fsize += flen(__this->file[dev_id]) / 1024;
        log_d("lock fsize + = %d\n", __this->lock_fsize);
        video_rec_lock_file(__this->file[dev_id], 1);
#ifdef CONFIG_EMR_DIR_ENABLE

#if (APP_CASE == __WIFI_CAR_CAMERA__)
        is_emf = TRUE;
        path = video_rec_finish_get_name(__this->file[dev_id], dev_id, is_emf);
#endif

        log_d("move_file_to_dir: %s\n", rec_dir[dev_id][1]);
        int err = fmove(__this->file[dev_id], rec_dir[dev_id][1], NULL, 0);
        if (!err) {
            __this->file[dev_id] = NULL;
#if (APP_CASE == __WIFI_CAR_CAMERA__)
            if (path) { //必须关闭文件之后才能调用，否则在读取文件信息不全！！！
                video_rec_finish_notify(path);
            }
#endif
            return;
        }
#endif
    }
    log_i("close file in1\n");
    fclose(__this->file[dev_id]);
    __this->file[dev_id] = NULL;

#if (APP_CASE == __WIFI_CAR_CAMERA__)
    if (path) { //必须关闭文件之后才能调用，否则在读取文件信息不全！！！
        video_rec_finish_notify(path);
    }
#endif

    log_i("close file finish\n");
}


static int video0_set_audio_callback(struct server *server, int (*callback)(u8 *buf, u32 len))
{
    union video_req req = {0};

    /* if (!__this->video_rec0) { */
    if (!server) {
        return -EINVAL;
    }

    req.rec.state 	= VIDEO_STATE_SET_AUDIO_CALLBACK;
    req.rec.audio_callback 	= callback;

    /* return server_request(__this->video_rec0, VIDEO_REQ_REC, &req); */
    return server_request(server, VIDEO_REQ_REC, &req);
}

#ifdef CONFIG_VIDEO1_ENABLE | CONFIG_VIDEO2_ENABLE | CONFIG_VIDEO3_ENABLE //修改原代码原 CONFIG_VIDEO0_ENABLE
/******* 不要单独调用这些子函数 ********/
static int video0_rec_start()
{
    puts("start_video_rec0\n");

    /* extern void h264_enc_manu_test(); */
    /* sys_timer_add(NULL, h264_enc_manu_test, 5000); */
    /* return 0; */

    /* extern avc_encode_test() ; */
    /* avc_encode_test() ; */
    /* return 0 ; */

    int err;
    union video_req req = {0};// 初始化录像请求结构体
    struct video_text_osd text_osd;// 用于存储文字OSD相关参数
    struct video_graph_osd graph_osd;// 用于存储图形OSD相关参数
    u16 max_one_line_strnum;// 一行最大字符数
    u16 osd_line_num;// OSD行数
    u16 osd_max_heigh; // OSD最大高度

    printf("[REC DEBUG]%s %d\n",__func__,__LINE__);
    if (!__this->video_rec0) {
        __this->video_rec0 = server_open("video_server", "video2.0");// 打开视频服务器
        /* __this->video_rec0 = server_open("video_server", "video4.0"); */
        if (!__this->video_rec0) {
            return VREC_ERR_V0_SERVER_OPEN; // 如果服务器打开失败，返回错误
        }

        server_register_event_handler(__this->video_rec0, (void *)2, rec_dev_server_event_handler);
    }
        printf("[REC DEBUG]%s %d\n",__func__,__LINE__);
//    u32 res = db_select("res");
//    printf("res=%d\n", res);
    /*
     *通道号，分辨率，封装格式，写卡的路径
     */
    req.rec.channel     = 0;
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;// 摄像头类型
    req.rec.width 	    = AVIN_WIDTH;// 录像宽度
    req.rec.height 	    = AVIN_HEIGH; // 录像高度
    req.rec.format 	    = VIDEO2_REC_FORMAT;// 录像格式
    req.rec.state 	    = VIDEO_STATE_START;// 录像状态
    req.rec.file        = __this->file[0];// 录像文件路径

    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_MID_Q; // 录像质量
    req.rec.fps 	    = 0; // 帧率
    req.rec.real_fps 	= 0; // 实际帧率
#if (APP_CASE == __WIFI_IPCAM__)
    req.rec.fps 	    = 15; // 如果是WIFI IPCAM，设置帧率为15
    req.rec.real_fps 	= 15; // 实际帧率也设置为15
#endif

    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
     req.rec.audio.fmt_format = AUDIO_FMT_PCM; // 音频格式
    req.rec.audio.sample_rate = 8000; // 采样率
    req.rec.audio.channel 	= 1; // 通道数
    req.rec.audio.volume    = 100; // 音量
    req.rec.audio.buf = __this->audio_buf[0]; // 音频缓冲区
    req.rec.audio.buf_len = AUDIO_BUF_SIZE; // 音频缓冲区长度
    req.rec.pkg_mute.aud_mute = !db_select("mic"); // 根据数据库设置是否静音

    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标,写0表示1个宏块有效
     * roio_ratio : 区域比例系数
     */
    req.rec.abr_kbps = video_rec_get_abr(req.rec.width);// 获取自适应比特率

#if defined __CPU_AC5401__
    req.rec.IP_interval = 0;// 编码参数设置
#elif defined __CPU_AC5601__
    if (req.rec.height > 720) {
        req.rec.IP_interval = 1; // 高度大于720时的编码参数
    } else {
        req.rec.IP_interval = 0;// 否则设置为0
    }
#else
    req.rec.IP_interval = 64;//128;// 默认设置
    /* req.rec.IP_interval = 0;//96; */
#endif

    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
    	感兴趣区域qp 为其他区域的 70% ，可以调整
    */
    /* req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16; */
    /* req.rec.roi.roi1_xy = (req.rec.height * 11 / 12 / 16) << 24 | (req.rec.height * 4 / 12 / 16) << 16 | (req.rec.width * 11 / 12 / 16) << 8 | (req.rec.width) * 1 / 12 / 16; */
    /* req.rec.roi.roi2_xy = 0; */
    /* req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0; */
    /* req.rec.roi.roio_ratio = 256 * 70 / 100 ; */
    /* req.rec.roi.roio_ratio1 = 256 * 90 / 100; */
    /* req.rec.roi.roio_ratio2 = 0; */
    /* req.rec.roi.roio_ratio3 = 256 * 80 / 100; */


    /* 编码参数配置，请仔细参阅说明文档后修改 */
    /* struct app_enc_info app_avc_cfg; */
    /* app_avc_cfg.pre_pskip_limit_flag = 0; 				//SKIP块提前判断限制开关 */
    /* app_avc_cfg.pre_pskip_th = 0;						//SKIP块提前判断阈值 */
    /* app_avc_cfg.common_pskip_limit_flag = 0; 			//SKIP块常规判断限制开关 */
    /* app_avc_cfg.common_pskip_th = 0;					//SKIP块常规判断阈值 */
    /* app_avc_cfg.dsw_size = 0;							//搜索窗口大小（0:3x7  1:5x7） */
    /* app_avc_cfg.fs_x_points = 32;						//水平搜索点数 */
    /* app_avc_cfg.fs_y_points = 24;						//垂直搜索点数 */
    /* app_avc_cfg.f_aq_strength = 0.6f;					//宏块QP自适应强度 */
    /* app_avc_cfg.qp_offset_low = 0;						//宏块QP偏移下限 */
    /* app_avc_cfg.qp_offset_high = 5;						//宏块QP偏移上限 */
    /* app_avc_cfg.nr_reduction = 0;						//DCT降噪开关 */
    /* app_avc_cfg.user_cfg_enable = 1;					//编码参数用户配置使能 */
    /* req.rec.app_avc_cfg = &app_avc_cfg; */
    req.rec.app_avc_cfg = NULL;

    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
#if (APP_CASE == __CAR_CAMERA__)
#ifndef CONFIG_UI_ENABLE
    memcpy(video_rec_osd_buf, osd_str_buf, strlen(osd_str_buf));// 复制OSD字符串
#endif
#endif //APP_CASE

    text_osd.font_w = 16; // 字体宽度
    text_osd.font_h = 32; // 字体高度
    max_one_line_strnum = strlen(video_rec_osd_buf); // 获取最大字符数

    /* osd_line_num = 1; */
    /* if (db_select("num")) { */
    osd_line_num = 2;// 设置OSD行数
    /* } */
    osd_max_heigh = (req.rec.height == 1088) ? 1080 : req.rec.height; // 根据录像高度设置最大高度
    text_osd.x = (req.rec.width - max_one_line_strnum * text_osd.font_w) / 64 * 64; // 计算X坐标
    text_osd.y = (osd_max_heigh - text_osd.font_h * osd_line_num) / 16 * 16; // 计算Y坐标
    /* text_osd.color[0] = 0xe20095; */
    /* text_osd.bit_mode = 1; */
    text_osd.color[0] = 0x057d88;// 设置OSD颜色
    text_osd.color[1] = 0xe20095;
    text_osd.color[2] = 0xe20095;
    text_osd.bit_mode = 2;// OSD位模式
    text_osd.text_format = video_rec_osd_buf; // 设置文本格式
    text_osd.font_matrix_table = osd_str_total; // 字体矩阵表
    /* text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;// 字体矩阵基地址
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);// 字体矩阵长度
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
    text_osd.direction = 1;// 设置文本方向

    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
    graph_osd.x = 0;// 图形X坐标
    graph_osd.y = 0;// 图形Y坐标
    graph_osd.width = 256;// 图形宽度
    graph_osd.height = 256;// 图形高度
    graph_osd.icon = icon_16bit_data;//icon_osd_buf;// 图标数据
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);// 图标大小
#else
    text_osd.direction = 0;// 默认文本方向
#endif


    if (db_select("dat")) {
        req.rec.text_osd = &text_osd;// 设置文字OSD
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
//        req.rec.graph_osd = &graph_osd;// 图形OSD设置
#endif
    }

    /*
     *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     */
     req.rec.slow_motion = 0; // 设置慢动作倍数
    req.rec.tlp_time = db_select("gap"); // 获取延时录像间隔
    if (req.rec.tlp_time) {
        req.rec.real_fps = 1000 / req.rec.tlp_time; // 根据间隔计算实际帧率
        req.rec.pkg_fps	 = 30; // 包帧率
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0; // 不录音时设置采样率为0
        req.rec.audio.channel 	= 0; // 通道数设置为0
        req.rec.audio.volume    = 0; // 音量设置为0
        req.rec.audio.buf = 0; // 不使用音频缓冲区
        req.rec.audio.buf_len = 0; // 音频缓冲区长度设置为0
    }
    req.rec.buf = __this->video_buf[0]; // 视频缓冲区
    req.rec.buf_len = VREC0_FBUF_SIZE; // 视频缓冲区长度
#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
    req.rec.rec_small_pic 	= 0; // 小图片录像设置
#else
    req.rec.rec_small_pic 	= 1; // 默认小图片录像
#endif
    /*
     *循环录像时间，文件大小
     */
    req.rec.cycle_time = db_select("cyc"); // 获取循环录像时间
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5; // 默认循环录像时间为5
    }

    req.rec.cycle_time = req.rec.cycle_time * 60; // 转换为秒
    /* if (req.rec.tlp_time) { */
    /* //此处可以使录像文件长度为设定值，需同步修改预创建大小 */
    /* req.rec.cycle_time = req.rec.cycle_time * req.rec.pkg_fps * req.rec.tlp_time / 1000; */
    /* } */

    printf("[REC DEBUG]%s %d\n",__func__,__LINE__);
    err = server_request(__this->video_rec0, VIDEO_REQ_REC, &req);// 发送录像请求
    if (err != 0) {
        puts("\n\n\nstart rec err\n\n\n");// 输出错误信息
        return VREC_ERR_V0_REQ_START;// 返回录像请求启动错误
    }

    printf("[REC DEBUG]%s %d\n",__func__,__LINE__);
//    video_rec_start_isp_scenes();// 开始ISP场景（如有需要）

    /* extern int audio_callback(u8 *buf, u32 len); */
    /* video0_set_audio_callback(__this->video_rec0,audio_callback); */
    /* extern void h264_enc_manu_test(); */
    /* sys_timer_add(NULL, h264_enc_manu_test, 5000); */
    /* sys_timeout_add(NULL, h264_enc_manu_test, 5000); */
    /* extern  void stop_and_rec() ; */
    /* sys_timeout_add(NULL, stop_and_rec, 60 * 1000 * 1); */

    return 0;
}

static int video0_rec_len()
{
    union video_req req = {0};

    if (!__this->video_rec0) {
        return -EINVAL;
    }

    req.rec.state 	= VIDEO_STATE_PKG_LEN;
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;

    return server_request(__this->video_rec0, VIDEO_REQ_REC, &req);
}

static int video0_rec_aud_mute()
{
    union video_req req = {0};

    if (!__this->video_rec0) {
        return -EINVAL;
    }

    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_PKG_MUTE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    return server_request(__this->video_rec0, VIDEO_REQ_REC, &req);
}


static int video0_rec_set_dr(u8 fps)
{
    union video_req req = {0};

    if (!__this->video_rec0) {
        return -EINVAL;
    }

    req.rec.real_fps = fps;
    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_SET_DR;


    return server_request(__this->video_rec0, VIDEO_REQ_REC, &req);

}

 int video_set_crop(u16 offset_x, u16 offset_y, u16 width, u16 height)
{
    #define SOURCE_WIDTH 1280
    #define SOURCE_HEIGHT 720
    int result = 0;  // 用来存储每次请求的结果，0表示成功，非0表示失败
    union video_req req = {0};
    static struct video_source_crop crop = {0};
    // printf("PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP");
//    if (!__this->video_rec0) {
    //      if (!__this->video_display[0]) {
    //     return -EINVAL;
    // }

    if (offset_x + width > SOURCE_WIDTH || offset_y + height > SOURCE_HEIGHT) {
        log_e("crop_overflow\n");
        return -EINVAL;
    } else if ((SOURCE_WIDTH / width > 3) || (SOURCE_HEIGHT / height > 3)) {
        log_e("crop_overflow!\n");
        return -EINVAL;
    } else {
        crop.x_offset = offset_x;
        crop.y_offset = offset_y;
        crop.width = width;
        crop.height = height;
    }
    log_i("crop : x %d, y %d, crop_w %d, crop_h %d\n", offset_x, offset_y, width, height);

//    req.rec.channel     = 0;
//    req.rec.state 	= VIDEO_STATE_SET_ENC_CROP;
//    req.rec.crop = &crop;

     req.display.state 	= VIDEO_STATE_SET_DIS_CROP;
     req.display.crop = &crop;
    // printf("PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP");
//    return server_request(__this->video_rec0, VIDEO_REQ_REC, &req);
    //  return server_request(__this->video_display[1], VIDEO_REQ_DISPLAY, &req);
    // 向 video_display[1] 发送变焦指令
    if (__this->video_display[1]) {
        result = server_request(__this->video_display[1], VIDEO_REQ_DISPLAY, &req);
        if (result != 0) {
            return result;  // 如果发送失败，立即返回错误码
        }
    }

    // 向 video_display[2] 发送变焦指令
    if (__this->video_display[2]) {
        result = server_request(__this->video_display[2], VIDEO_REQ_DISPLAY, &req);
        if (result != 0) {
            return result;  // 如果发送失败，立即返回错误码
        }
    }

    // 向 video_display[3] 发送变焦指令
    if (__this->video_display[3]) {
        result = server_request(__this->video_display[3], VIDEO_REQ_DISPLAY, &req);
        if (result != 0) {
            return result;  // 如果发送失败，立即返回错误码
        }
    }

return result;  // 全部成功返回0，任意一个失败返回错误码
}


static int video0_rec_stop(u8 close)
{
    union video_req req = {0};  // 初始化 video_req 联合体，用于存储视频请求参数
    int err;  // 定义变量，用于存储函数返回的错误代码

    log_d("video0_rec_stop\n");  // 记录调试日志，指示函数已被调用

//    video_rec_stop_isp_scenes(2, 0);  // 停止与 ISP（图像信号处理器）相关的场景

    // 检查 video_rec0 是否存在
    if (__this->video_rec0) {
        req.rec.channel = 0;  // 设置请求的通道号为 0
        req.rec.state = VIDEO_STATE_STOP;  // 设置请求的状态为停止视频录制

        // 发送停止录制请求，并将结果存储在 err 变量中
        err = server_request(__this->video_rec0, VIDEO_REQ_REC, &req);

        // 如果请求失败，处理错误
        if (err != 0) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
            video_rec_err_notify("VIDEO_REC_ERR");  // 如果是无线车载摄像机的情况，通知录制错误
#endif
            printf("\nstop rec err 0x%x\n", err);  // 打印停止录制的错误码
            return VREC_ERR_V0_REQ_STOP;  // 返回视频录制停止请求失败的错误码
        }
    }

    video_rec_close_file(0);  // 关闭视频录制文件

//    video_rec_start_isp_scenes();  // 重新启动 ISP 场景

    // 如果 close 标志为真，关闭视频录制服务器
    if (close) {
        if (__this->video_rec0) {
            server_close(__this->video_rec0);  // 关闭视频录制服务器
            __this->video_rec0 = NULL;  // 将指针置为空，表示服务器已关闭
        }
    }

    return 0;  // 返回成功
}


/*
 *注意：循环录像的时候，虽然要重新传参，但是要和start传的参数保持一致！！！
 */
static int video0_rec_savefile()
{
    union video_req req = {0}; // 初始化录像请求结构体
    int err;

    // 检查录像文件指针是否为空
    if (!__this->file[0]) {
        log_i("\n\nf0null\n\n"); // 记录错误日志
        return -ENOENT; // 返回找不到文件错误
    }

    // 设置录像请求的相关参数
    req.rec.channel = 0; // 录像通道号
    req.rec.width 	= AVIN_WIDTH; // 录像宽度
    req.rec.height 	= AVIN_HEIGH; // 录像高度
    req.rec.format 	= VIDEO2_REC_FORMAT; // 录像格式
    req.rec.state 	= VIDEO_STATE_SAVE_FILE; // 设置为保存文件状态
    req.rec.file    = __this->file[0]; // 录像文件路径
    req.rec.fps 	= 0; // 帧率
    req.rec.real_fps 	= 0; // 实际帧率

    // 设置小图片录像标志
#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
    req.rec.rec_small_pic 	= 0; // 如果启用三路或USB视频输出，设置为0
#else
    req.rec.rec_small_pic 	= 1; // 默认设置为1
#endif

    // 设置循环录像时间
    req.rec.cycle_time = db_select("cyc"); // 从数据库获取循环时间
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5; // 如果获取值为0，设置为默认值5
    }

    req.rec.cycle_time = req.rec.cycle_time * 60; // 转换为秒

    // 设置音频相关参数
    req.rec.audio.fmt_format = AUDIO_FMT_PCM; // 音频格式
    req.rec.audio.sample_rate = 8000; // 采样率
    req.rec.audio.channel 	= 1; // 通道数
    req.rec.audio.volume    = 100; // 音量
    req.rec.audio.buf = __this->audio_buf[0]; // 音频缓冲区
    req.rec.audio.buf_len = AUDIO_BUF_SIZE; // 音频缓冲区长度
    req.rec.pkg_mute.aud_mute = !db_select("mic"); // 根据数据库设置是否静音

    // 设置延时录像参数
    req.rec.tlp_time = db_select("gap"); // 获取延时录像间隔
    if (req.rec.tlp_time) {
        req.rec.real_fps = 1000 / req.rec.tlp_time; // 根据间隔计算实际帧率
        req.rec.pkg_fps	 = 30; // 包帧率
    }

    // 发送保存录像请求
    err = server_request(__this->video_rec0, VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("\nsave rec err 0x%x\n", err); // 输出错误信息
        return VREC_ERR_V0_REQ_SAVEFILE; // 返回保存文件错误
    }

    return 0; // 成功返回0
}

static void video0_rec_close()
{
    // 检查录像服务器指针是否有效
    if (__this->video_rec0) {
        server_close(__this->video_rec0); // 关闭视频服务器
        __this->video_rec0 = NULL; // 将指针设置为NULL
    }
}


/*
 *必须在启动录像之后才可调用该函数，并且确保启动录像时已经打开了osd
 *新设置的osd的整体结构要和启动录像时一样，只是内容改变!!!
 */
static int video0_rec_set_osd_str(char *str)
{
    union video_req req = {0};
    int err;
    if (!__this->video_rec0) {
        return -1;
    }

    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_SET_OSD_STR;
    req.rec.new_osd_str = str;
    err = server_request(__this->video_rec0, VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("\nset osd rec0 str err 0x%x\n", err);
        return -1;
    }

    return 0;
}

static int video0_rec_osd_ctl(u8 onoff)
{
    union video_req req = {0};
    struct video_text_osd text_osd;
    int err;

    if (__this->video_rec0) {

        u32 res = db_select("res");
        req.rec.width 	= rec_pix_w[res];
        req.rec.height 	= rec_pix_h[res];

        text_osd.font_w = 16;
        text_osd.font_h = 32;
        text_osd.x = (req.rec.width - strlen(video_rec_osd_buf) * text_osd.font_w) / 64 * 64;
        text_osd.y = (req.rec.height - text_osd.font_h) / 16 * 16;
        /* text_osd.osd_yuv = 0xe20095; */
        text_osd.color[0] = 0xe20095;
        text_osd.bit_mode = 1;
        text_osd.text_format = video_rec_osd_buf;
        text_osd.font_matrix_table = osd_str_total;
        text_osd.font_matrix_base = osd_str_matrix;
        text_osd.font_matrix_len = sizeof(osd_str_matrix);
        text_osd.direction = 1;
        req.rec.text_osd = 0;
        if (onoff) {
            req.rec.text_osd = &text_osd;
        }
        req.rec.channel = 0;
        req.rec.state 	= VIDEO_STATE_SET_OSD;

        err = server_request(__this->video_rec0, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nset osd rec0 err 0x%x\n", err);
            return -1;
        }
    }

    return 0;
}
#endif









/******* 不要单独调用这些子函数 ********/
#ifdef CONFIG_VIDEO1_ENABLE
static int video1_rec_start()
{
    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;

    puts("start_video_rec1 \n");
    if (!__this->video_rec1) {
        __this->video_rec1 = server_open("video_server", "video1.0");
        /* __this->video_rec1 = server_open("video_server", "video4.1"); */
        if (!__this->video_rec1) {
            return VREC_ERR_V1_SERVER_OPEN;
        }

        server_register_event_handler(__this->video_rec1, (void *)1, rec_dev_server_event_handler);
    }

    /* req.rec.channel = 1; */
    req.rec.channel = 0;
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;
    req.rec.width 	= AVIN_WIDTH;
    req.rec.height 	= AVIN_HEIGH;
    req.rec.format 	= VIDEO1_REC_FORMAT;
    req.rec.state 	= VIDEO_STATE_START;
    req.rec.file    = __this->file[1];
    req.rec.quality = VIDEO_LOW_Q;
    req.rec.fps 	= 0;
    req.rec.real_fps 	= 0;

    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
    req.rec.audio.sample_rate = 8000;
    req.rec.audio.channel 	= 1;
    req.rec.audio.volume    = 100;
    req.rec.audio.buf = __this->audio_buf[1];
    req.rec.audio.buf_len = AUDIO_BUF_SIZE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");
    req.rec.mirror=db_select("mir");
    req.rec.abr_kbps = video_rec_get_abr(req.rec.width);
    req.rec.IP_interval = 64;

    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
    	感兴趣区域qp 为其他区域的 70% ，可以调整
    */
    /* req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16; */
    /* req.rec.roi.roio_ratio = 256 * 70 / 100 ; */
    /* req.rec.roi.roi1_xy = 0; */
    /* req.rec.roi.roi2_xy = 0; */
    /* req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0; */
    /* req.rec.roi.roio_ratio1 = 0; */
    /* req.rec.roi.roio_ratio2 = 0; */
    /* req.rec.roi.roio_ratio3 = 256 * 80 / 100; */

    /* 编码参数配置，请仔细参阅说明文档后修改 */
    /* struct app_enc_info app_avc_cfg; */
    /* app_avc_cfg.pre_pskip_limit_flag = 0; 				//SKIP块提前判断限制开关 */
    /* app_avc_cfg.pre_pskip_th = 0;						//SKIP块提前判断阈值 */
    /* app_avc_cfg.common_pskip_limit_flag = 0; 			//SKIP块常规判断限制开关 */
    /* app_avc_cfg.common_pskip_th = 0;					//SKIP块常规判断阈值 */
    /* app_avc_cfg.dsw_size = 0;							//搜索窗口大小（0:3x7  1:5x7） */
    /* app_avc_cfg.fs_x_points = 32;						//水平搜索点数 */
    /* app_avc_cfg.fs_y_points = 24;						//垂直搜索点数 */
    /* app_avc_cfg.f_aq_strength = 0.6f;					//宏块QP自适应强度 */
    /* app_avc_cfg.qp_offset_low = 0;						//宏块QP偏移下限 */
    /* app_avc_cfg.qp_offset_high = 5;						//宏块QP偏移上限 */
    /* app_avc_cfg.nr_reduction = 0;						//DCT降噪开关 */
    /* app_avc_cfg.user_cfg_enable = 1;					//编码参数用户配置使能 */
    /* req.rec.app_avc_cfg = &app_avc_cfg; */
    req.rec.app_avc_cfg = NULL;



    text_osd.font_w = 16;
    text_osd.font_h = 32;

    max_one_line_strnum = strlen(video_rec_osd_buf);//20;
    osd_line_num = 1;
    if (db_select("num")) {
        osd_line_num = 2;
    }
    text_osd.x = (req.rec.width - max_one_line_strnum * text_osd.font_w) / 64 * 64;
    text_osd.y = (req.rec.height - text_osd.font_h * osd_line_num) / 16 * 16;

    text_osd.direction = 1;
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

    req.rec.text_osd = 0;
    if (db_select("dat")) {
        req.rec.text_osd = &text_osd;
    }

#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_osd_buf);
    req.rec.graph_osd = NULL;//&graph_osd;
#endif

    req.rec.slow_motion = 0;
    req.rec.tlp_time = db_select("gap");
    if (req.rec.tlp_time) {
        req.rec.real_fps = 1000 / req.rec.tlp_time;
        req.rec.pkg_fps	 = 30;
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel 	= 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }
    req.rec.buf = __this->video_buf[1];
    req.rec.buf_len = VREC1_FBUF_SIZE;

#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
    req.rec.rec_small_pic 	= 0;
#else
    req.rec.rec_small_pic 	= 1;
#endif


    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }

    req.rec.cycle_time = req.rec.cycle_time * 60;

    err = server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
    if (err != 0) {
        puts("\n\n\nstart rec2 err\n\n\n");
        return VREC_ERR_V1_REQ_START;
    }

    return 0;
}

static int video1_rec_len()
{
    union video_req req = {0};

    if (!__this->video_rec1) {
        return -EINVAL;
    }

    req.rec.state 	= VIDEO_STATE_PKG_LEN;
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;

    return server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
}

static int video1_rec_aud_mute()
{
    union video_req req = {0};

    if (!__this->video_rec1) {
        return -EINVAL;
    }

    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_PKG_MUTE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    return server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
}

static int video1_rec_set_dr()
{
    union video_req req = {0};

    if (!__this->video_rec1) {
        return -EINVAL;
    }

    req.rec.real_fps = 7;
    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_SET_DR;

    return server_request(__this->video_rec1, VIDEO_REQ_REC, &req);

}


static int video1_rec_stop(u8 close)
{
    union video_req req = {0};
    int err;

    log_d("video1_rec_stop\n");

    if (__this->video_rec1) {
        req.rec.channel = 0;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nstop rec2 err 0x%x\n", err);
            return VREC_ERR_V1_REQ_STOP;
        }
    }

    video_rec_close_file(1);

    if (close) {
        if (__this->video_rec1) {
            server_close(__this->video_rec1);
            __this->video_rec1 = NULL;
        }
    }

    return 0;
}

static int video1_rec_savefile()
{
    union video_req req = {0};
    int err;

    if (__this->video_rec1) {

        if (!__this->file[1]) {
            log_i("\n\nf1null\n\n");
            return -ENOENT;
        }

        req.rec.channel = 0;
        req.rec.width 	= AVIN_WIDTH;
        req.rec.height 	= AVIN_HEIGH;
        req.rec.format 	= VIDEO1_REC_FORMAT;
        req.rec.state 	= VIDEO_STATE_SAVE_FILE;
        req.rec.file    = __this->file[1];
        req.rec.fps 	= 0;
        req.rec.real_fps 	= 0;

#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
        req.rec.rec_small_pic 	= 0;
#else
        req.rec.rec_small_pic 	= 1;
#endif

        req.rec.cycle_time = db_select("cyc");
        if (req.rec.cycle_time == 0) {
            req.rec.cycle_time = 5;
        }
        req.rec.cycle_time = req.rec.cycle_time * 60;


        req.rec.audio.fmt_format = AUDIO_FMT_PCM;
        req.rec.audio.sample_rate = 8000;
        req.rec.audio.channel 	= 1;
        req.rec.audio.volume    = 100;
        req.rec.audio.buf = __this->audio_buf[1];
        req.rec.audio.buf_len = AUDIO_BUF_SIZE;
        req.rec.pkg_mute.aud_mute = !db_select("mic");

        req.rec.tlp_time = db_select("gap");
        if (req.rec.tlp_time) {
            req.rec.real_fps = 1000 / req.rec.tlp_time;
            req.rec.pkg_fps	 = 30;
        }

        err = server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nsave rec2 err 0x%x\n", err);
            return VREC_ERR_V1_REQ_SAVEFILE;
        }
    }

    return 0;
}

static void video1_rec_close()
{
    if (__this->video_rec1) {
        server_close(__this->video_rec1);
        __this->video_rec1 = NULL;
    }
}


/*
 *必须在启动录像之后才可调用该函数，并且确保启动录像时已经打开了osd
 *新设置的osd的整体结构要和启动录像时一样，只是内容改变!!!
 */
static int video1_rec_set_osd_str(char *str)
{
    union video_req req = {0};
    int err;
    if (!__this->video_rec1) {
        return -1;
    }

    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_SET_OSD_STR;
    req.rec.new_osd_str = str;
    err = server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("\nset osd rec1 str err 0x%x\n", err);
        return -1;
    }

    return 0;
}

static int video1_rec_osd_ctl(u8 onoff)
{
    union video_req req = {0};
    struct video_text_osd text_osd;
    int err;

    if (__this->video_rec1) {
        req.rec.width 	= AVIN_WIDTH;
        req.rec.height 	= AVIN_HEIGH;

        text_osd.font_w = 16;
        text_osd.font_h = 32;
        text_osd.x = (req.rec.width - strlen(osd_str_buf) * text_osd.font_w) / 64 * 64;
        text_osd.y = (req.rec.height - text_osd.font_h) / 16 * 16;
        /* text_osd.osd_yuv = 0xe20095; */
        text_osd.color[0] = 0xe20095;
        text_osd.bit_mode = 1;
        text_osd.text_format = osd_str_buf;
        text_osd.font_matrix_table = osd_str_total;
        text_osd.font_matrix_base = osd_str_matrix;
        text_osd.font_matrix_len = sizeof(osd_str_matrix);
        text_osd.direction = 1;
        req.rec.text_osd = 0;
        if (onoff) {
            req.rec.text_osd = &text_osd;
        }
        req.rec.channel = 0;
        req.rec.state 	= VIDEO_STATE_SET_OSD;

        err = server_request(__this->video_rec1, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nset osd rec1 err 0x%x\n", err);
            return -1;
        }
    }

    return 0;
}
#endif






#ifdef CONFIG_VIDEO3_ENABLE

static int video3_rec_start()
{
    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    char name[12];

    void *uvc_fd;
    struct uvc_capability uvc_cap;

    puts("-----start_video_rec3 \n");
    if (!__this->video_rec3) {
        sprintf(name, "video3.%d", __this->uvc_id);
        __this->video_rec3 = server_open("video_server", name);
        if (!__this->video_rec3) {
            log_e("rec3 video_server open err! \n");
            return -EINVAL;
        }

        server_register_event_handler(__this->video_rec3, (void *)3, rec_dev_server_event_handler);
    }

    req.rec.channel = 0;
    req.rec.camera_type = VIDEO_CAMERA_UVC;
#ifdef THREE_WAY_ENABLE
    req.rec.three_way_type = VIDEO_THREE_WAY_JPEG;
    req.rec.IP_interval = 99;
#else
    req.rec.three_way_type = 0;
    req.rec.IP_interval = 128;
#endif
    req.rec.format 	= VIDEO3_REC_FORMAT;
    req.rec.width 	= UVC_ENC_WIDTH;
    req.rec.height 	= UVC_ENC_HEIGH;
    req.rec.src_w   = __this->src_width[3];
    req.rec.src_h   = __this->src_height[3];
    if (req.rec.three_way_type == VIDEO_THREE_WAY_JPEG) {
        uvc_fd = dev_open("uvc", (void *)__this->uvc_id);
        if (uvc_fd) {
            dev_ioctl(uvc_fd, UVCIOC_QUERYCAP, (unsigned int)&uvc_cap);
            req.rec.width 	= uvc_cap.reso[0].width;
            req.rec.height 	= uvc_cap.reso[0].height;
            dev_close(uvc_fd);
        }
    }
    __this->uvc_width = req.rec.width;
    __this->uvc_height = req.rec.height;
    printf("\n\nuvc size %d, %d\n\n", req.rec.width, req.rec.height);
    req.rec.state 	= VIDEO_STATE_START;
    req.rec.file    = __this->file[3];
    req.rec.uvc_id = __this->uvc_id;
    req.rec.quality = VIDEO_LOW_Q;
    req.rec.fps 	= 0;
    req.rec.real_fps 	= 0;
    req.rec.mirror=db_select("mir");
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
    req.rec.audio.sample_rate = 8000;
    req.rec.audio.channel 	= 1;
    req.rec.audio.volume    = 100;
    req.rec.audio.buf = __this->audio_buf[3];
    req.rec.audio.buf_len = AUDIO_BUF_SIZE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    req.rec.abr_kbps = video_rec_get_abr(req.rec.width);


    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
    	感兴趣区域qp 为其他区域的 70% ，可以调整
    */
    /* req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16; */
    /* req.rec.roi.roio_ratio = 256 * 70 / 100 ; */
    /* req.rec.roi.roi1_xy = 0; */
    /* req.rec.roi.roi2_xy = 0; */
    /* req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0; */
    /* req.rec.roi.roio_ratio1 = 0; */
    /* req.rec.roi.roio_ratio2 = 0; */
    /* req.rec.roi.roio_ratio3 = 256 * 80 / 100; */

    /* 编码参数配置，请仔细参阅说明文档后修改 */
    /* struct app_enc_info app_avc_cfg; */
    /* app_avc_cfg.pre_pskip_limit_flag = 0; 				//SKIP块提前判断限制开关 */
    /* app_avc_cfg.pre_pskip_th = 0;						//SKIP块提前判断阈值 */
    /* app_avc_cfg.common_pskip_limit_flag = 0; 			//SKIP块常规判断限制开关 */
    /* app_avc_cfg.common_pskip_th = 0;					//SKIP块常规判断阈值 */
    /* app_avc_cfg.dsw_size = 0;							//搜索窗口大小（0:3x7  1:5x7） */
    /* app_avc_cfg.fs_x_points = 32;						//水平搜索点数 */
    /* app_avc_cfg.fs_y_points = 24;						//垂直搜索点数 */
    /* app_avc_cfg.f_aq_strength = 0.6f;					//宏块QP自适应强度 */
    /* app_avc_cfg.qp_offset_low = 0;						//宏块QP偏移下限 */
    /* app_avc_cfg.qp_offset_high = 5;						//宏块QP偏移上限 */
    /* app_avc_cfg.nr_reduction = 0;						//DCT降噪开关 */
    /* app_avc_cfg.user_cfg_enable = 1;					//编码参数用户配置使能 */
    /* req.rec.app_avc_cfg = &app_avc_cfg; */
    req.rec.app_avc_cfg = NULL;



    text_osd.font_w = 16;
    text_osd.font_h = 32;

    max_one_line_strnum = strlen(video_rec_osd_buf);//20;
    osd_line_num = 2;
    text_osd.x = (req.rec.width - max_one_line_strnum * text_osd.font_w) / 64 * 64;
    text_osd.y = (req.rec.height - text_osd.font_h * osd_line_num) / 16 * 16;

    text_osd.direction = 1;
    /* text_osd.osd_yuv = 0xe20095; */
    text_osd.color[0] = 0xe20095;
    text_osd.bit_mode = 1;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
#ifdef THREE_WAY_ENABLE
    text_osd.font_matrix_base = osd_mimc_str_matrix;
    text_osd.font_matrix_len = sizeof(osd_mimc_str_matrix);
#else
    text_osd.font_matrix_base = osd_str_matrix;
    text_osd.font_matrix_len = sizeof(osd_str_matrix);
#endif

#ifndef THREE_WAY_ENABLE
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
    text_osd.direction = 1;

    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = icon_16bit_data;//icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#endif
#endif

    if (db_select("dat")) {
        req.rec.text_osd = &text_osd;
#ifndef THREE_WAY_ENABLE
#if (defined __CPU_DV15__ || defined __CPU_DV17__ || defined __CPU_AC571X__)
        /* req.rec.graph_osd = &graph_osd; */
#endif
#endif
    }

    req.rec.slow_motion = 0;
    req.rec.tlp_time = db_select("gap");
    if (req.rec.tlp_time) {
        req.rec.real_fps = 1000 / req.rec.tlp_time;
        req.rec.pkg_fps	 = 30;
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel 	= 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }
    req.rec.buf = __this->video_buf[3];
    req.rec.buf_len = VREC3_FBUF_SIZE;
#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
    req.rec.rec_small_pic 	= 0;
#else
    req.rec.rec_small_pic 	= 1;
#endif


    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;
    err = server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
    if (err != 0) {
        log_e("rec3 rec start err! \n");
        return -EINVAL;
    }
    printf("rec3 rec start ok. \n");

    return 0;
}

static int video3_rec_len()
{
    union video_req req = {0};

    if (!__this->video_rec3) {
        return -EINVAL;
    }

    req.rec.state 	= VIDEO_STATE_PKG_LEN;
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;

    return server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
}

static int video3_rec_aud_mute()
{
    union video_req req = {0};

    if (!__this->video_rec3) {
        return -EINVAL;
    }

    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_PKG_MUTE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    return server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
}


static int video3_rec_stop(u8 close)
{
    union video_req req = {0};
    int err;

    log_d("video3_rec_stop\n");

    if (__this->video_rec3) {
        req.rec.channel = 0;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nstop rec3 err 0x%x\n", err);
            return -EINVAL;
        }
    }

    video_rec_close_file(3);

    if (close) {
        if (__this->video_rec3) {
            server_close(__this->video_rec3);
            __this->video_rec3 = NULL;
        }
    }

    return 0;
}

static int video3_rec_savefile()
{
    union video_req req = {0};
    int err;

    if (__this->video_rec3) {
        printf("video3_rec_savefile in\n");

        if (!__this->file[3]) {
            return -ENOENT;
        }

        req.rec.channel = 0;
#ifdef THREE_WAY_ENABLE
        req.rec.width 	= __this->uvc_width;
        req.rec.height 	= __this->uvc_height;
#else
        req.rec.width 	= UVC_ENC_WIDTH;
        req.rec.height 	= UVC_ENC_HEIGH;
#endif
        req.rec.format 	= VIDEO3_REC_FORMAT;
        req.rec.state 	= VIDEO_STATE_SAVE_FILE;
        req.rec.file    = __this->file[3];
        req.rec.fps 	= 0;
        req.rec.real_fps = 0;
        req.rec.uvc_id = __this->uvc_id;
#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
        req.rec.rec_small_pic 	= 0;
#else
        req.rec.rec_small_pic 	= 1;
#endif

        req.rec.cycle_time = db_select("cyc");
        if (req.rec.cycle_time == 0) {
            req.rec.cycle_time = 5;
        }
        req.rec.cycle_time = req.rec.cycle_time * 60;


        req.rec.audio.fmt_format = AUDIO_FMT_PCM;
        req.rec.audio.sample_rate = 8000;
        req.rec.audio.channel 	= 1;
        req.rec.audio.volume    = 100;
        req.rec.audio.buf = __this->audio_buf[3];
        req.rec.audio.buf_len = AUDIO_BUF_SIZE;
        req.rec.pkg_mute.aud_mute = !db_select("mic");

        req.rec.tlp_time = db_select("gap");
        if (req.rec.tlp_time) {
            req.rec.real_fps = 1000 / req.rec.tlp_time;
            req.rec.pkg_fps	 = 30;
        }

        err = server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nsave rec3 err 0x%x\n", err);
            return -EINVAL;
        }

        printf("video3_rec_savefile out\n");
    }

    return 0;
}

static void video3_rec_close()
{
    if (__this->video_rec3) {
        server_close(__this->video_rec3);
        __this->video_rec3 = NULL;
    }
}


/*
 *必须在启动录像之后才可调用该函数，并且确保启动录像时已经打开了osd
 *新设置的osd的整体结构要和启动录像时一样，只是内容改变!!!
 */
static int video3_rec_set_osd_str(char *str)
{
    union video_req req = {0};
    int err;
    if (!__this->video_rec3) {
        return -1;
    }

    req.rec.channel = 0;
    req.rec.state 	= VIDEO_STATE_SET_OSD_STR;
    req.rec.new_osd_str = str;
    err = server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("\nset osd rec3 str err 0x%x\n", err);
        return -1;
    }

    return 0;
}

static int video3_rec_osd_ctl(u8 onoff)
{
    union video_req req = {0};
    struct video_text_osd text_osd;
    int err;

    if (__this->video_rec3) {
        req.rec.width 	= UVC_ENC_WIDTH;
        req.rec.height 	= UVC_ENC_HEIGH;

        text_osd.font_w = 16;
        text_osd.font_h = 32;
        text_osd.x = (req.rec.width - strlen(osd_str_buf) * text_osd.font_w) / 64 * 64;
        text_osd.y = (req.rec.height - text_osd.font_h) / 16 * 16;
        /* text_osd.osd_yuv = 0xe20095; */
        text_osd.color[0] = 0xe20095;
        text_osd.bit_mode = 1;
        text_osd.direction = 1;
        text_osd.text_format = osd_str_buf;
        text_osd.font_matrix_table = osd_str_total;
        text_osd.font_matrix_base = osd_str_matrix;
        text_osd.font_matrix_len = sizeof(osd_str_matrix);
        req.rec.text_osd = 0;
        if (onoff) {
            req.rec.text_osd = &text_osd;
        }
        req.rec.channel = 0;
        req.rec.state 	= VIDEO_STATE_SET_OSD;

        err = server_request(__this->video_rec3, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nset osd rec3 err 0x%x\n", err);
            return -1;
        }
    }

    return 0;
}

#endif



#ifdef CONFIG_VIDEO4_ENABLE
/******* 不要单独调用这些子函数 ********/
static int video4_rec_start(u32 id)
{
    log_i("start_video4_rec%d\n", id);
    if (id >= 4) {
        return -1;
    }

    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;
    char dev_name[20];

    if (!__this->video_rec[id]) {
        sprintf(dev_name, "video4.%d.%d", id, 0);
        __this->video_rec[id] = server_open("video_server", (void *)dev_name);
        if (!__this->video_rec[id]) {
            return VREC_ERR_V0_SERVER_OPEN;
        }

        server_register_event_handler(__this->video_rec[id], (void *)id, rec_dev_server_event_handler);
    }


    u32 res = VIDEO_RES_720P;
    /*
     *通道号，分辨率，封装格式，写卡的路径
     */
    req.rec.channel     = id;
    req.rec.camera_type = VIDEO_CAMERA_MUX;
    req.rec.width 	    = rec_pix_w[res];
    req.rec.height 	    = rec_pix_h[res];
    req.rec.format 	    = VIDEO4_REC_FORMAT;
    req.rec.state 	    = VIDEO_STATE_START;
    req.rec.file        = __this->file[id];

    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_MID_Q;
    req.rec.fps 	    = 0;
    req.rec.real_fps 	= 0;

    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
    req.rec.audio.sample_rate = 8000;
    req.rec.audio.channel 	= 1;
    req.rec.audio.volume    = 100;
    req.rec.audio.buf = __this->audio_buf[id];
    req.rec.audio.buf_len = AUDIO_BUF_SIZE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标,写0表示1个宏块有效
     * roio_ratio : 区域比例系数
     */

    req.rec.abr_kbps = video_rec_get_abr(req.rec.width);
    req.rec.IP_interval = 32;//128;

    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
    	感兴趣区域qp 为其他区域的 70% ，可以调整
    */
    /* req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16; */
    /* req.rec.roi.roi1_xy = (req.rec.height * 11 / 12 / 16) << 24 | (req.rec.height * 4 / 12 / 16) << 16 | (req.rec.width * 11 / 12 / 16) << 8 | (req.rec.width) * 1 / 12 / 16; */
    /* req.rec.roi.roi2_xy = 0; */
    /* req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0; */
    /* req.rec.roi.roio_ratio = 256 * 70 / 100 ; */
    /* req.rec.roi.roio_ratio1 = 256 * 90 / 100; */
    /* req.rec.roi.roio_ratio2 = 0; */
    /* req.rec.roi.roio_ratio3 = 256 * 80 / 100; */

    /* 编码参数配置，请仔细参阅说明文档后修改 */
    /* struct app_enc_info app_avc_cfg; */
    /* app_avc_cfg.pre_pskip_limit_flag = 0; 				//SKIP块提前判断限制开关 */
    /* app_avc_cfg.pre_pskip_th = 0;						//SKIP块提前判断阈值 */
    /* app_avc_cfg.common_pskip_limit_flag = 0; 			//SKIP块常规判断限制开关 */
    /* app_avc_cfg.common_pskip_th = 0;					//SKIP块常规判断阈值 */
    /* app_avc_cfg.dsw_size = 0;							//搜索窗口大小（0:3x7  1:5x7） */
    /* app_avc_cfg.fs_x_points = 32;						//水平搜索点数 */
    /* app_avc_cfg.fs_y_points = 24;						//垂直搜索点数 */
    /* app_avc_cfg.f_aq_strength = 0.6f;					//宏块QP自适应强度 */
    /* app_avc_cfg.qp_offset_low = 0;						//宏块QP偏移下限 */
    /* app_avc_cfg.qp_offset_high = 5;						//宏块QP偏移上限 */
    /* app_avc_cfg.nr_reduction = 0;						//DCT降噪开关 */
    /* app_avc_cfg.user_cfg_enable = 1;					//编码参数用户配置使能 */
    /* req.rec.app_avc_cfg = &app_avc_cfg; */
    req.rec.app_avc_cfg = NULL;



    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
#if (APP_CASE == __CAR_CAMERA__)
#ifndef CONFIG_UI_ENABLE
    memcpy(video_rec_osd_buf, osd_str_buf, strlen(osd_str_buf));
#endif
#endif //APP_CASE

    text_osd.font_w = 16;
    text_osd.font_h = 32;
    max_one_line_strnum = strlen(video_rec_osd_buf);

    /* osd_line_num = 1; */
    /* if (db_select("num")) { */
    osd_line_num = 2;
    /* } */

    osd_max_heigh = (req.rec.height == 1088) ? 1080 : req.rec.height ;
    text_osd.x = (req.rec.width - max_one_line_strnum * text_osd.font_w) / 64 * 64;
    text_osd.y = (osd_max_heigh - text_osd.font_h * osd_line_num) / 16 * 16;
    text_osd.color[0] = 0xe20095;
    text_osd.bit_mode = 1;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
    text_osd.font_matrix_base = osd_mimc_str_matrix;//osd_str_matrix;
    text_osd.font_matrix_len = sizeof(osd_mimc_str_matrix);

    text_osd.direction = 1;

    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = (u8 *)icon_16bit_data;
    graph_osd.icon_size = sizeof(icon_16bit_data);

    req.rec.text_osd = &text_osd;
    req.rec.graph_osd = NULL;//&graph_osd;

    /*
     *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     */
    u32 gap_time = db_select("gap");
    req.rec.slow_motion = 0;

    if (gap_time) {
        /* req.rec.tlp_time = gap_time; */
        /* req.rec.real_fps = 1000 / req.rec.tlp_time; */
        req.rec.gap_fps  = gap_time / 40;
        req.rec.pkg_fps	 = 30;
    }
    if (req.rec.slow_motion || gap_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel 	= 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }
    req.rec.buf = __this->video_buf[id];
    req.rec.buf_len = VREC4_FBUF_SIZE;
#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
    req.rec.rec_small_pic 	= 0;
#else
    req.rec.rec_small_pic 	= 1;
#endif

    /*
     *循环录像时间，文件大小
     */
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;
    err = server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);
    if (err != 0) {
        log_i("\n\n\nstart video4 rec%d err\n\n\n", id);
        return VREC_ERR_V0_REQ_START;
    }

    /* extern int audio_callback(u8 *buf, u32 len); */
    /* video0_set_audio_callback(__this->video_rec[0],audio_callback); */

    return 0;

}

static int video4_rec_len(u8 id)
{
    union video_req req = {0};

    if (!__this->video_rec[id]) {
        return -EINVAL;
    }

    req.rec.channel = id;
    req.rec.state 	= VIDEO_STATE_PKG_LEN;
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;

    return server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);
}

static int video4_rec_aud_mute(u8 id)
{
    union video_req req = {0};

    if (!__this->video_rec[id]) {
        return -EINVAL;
    }

    req.rec.channel = id;
    req.rec.state 	= VIDEO_STATE_PKG_MUTE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    return server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);
}

static int video4_rec_set_dr(u8 id, u8 fps)
{
    union video_req req = {0};

    if (!__this->video_rec[id]) {
        return -EINVAL;
    }

    req.rec.real_fps = fps;
    req.rec.channel = id;
    req.rec.state 	= VIDEO_STATE_SET_DR;


    return server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);

}

static int video4_rec_stop(u8 id, u8 close)
{
    union video_req req = {0};
    int err;

    log_d("video%d_rec_stop\n", id);

    if (__this->video_rec[id]) {
        req.rec.channel = id;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nvideo4 stop rec%d err 0x%x\n", id, err);
            return VREC_ERR_V0_REQ_STOP;
        }
    }

    video_rec_close_file(id);

    if (close) {
        if (__this->video_rec[id]) {
            server_close(__this->video_rec[id]);
            __this->video_rec[id] = NULL;
        }
    }

    return 0;
}


/*
 *注意：循环录像的时候，虽然要重新传参，但是要和start传的参数保持一致！！！
 */
static int video4_rec_savefile(u8 id)
{
    union video_req req = {0};
    int err;

    if (!__this->file[id]) {
        log_i("\n\nf%dnull\n\n", id);
        return -ENOENT;
    }
    u32 res = VIDEO_RES_720P;

    req.rec.channel = id;
    req.rec.width 	= rec_pix_w[res];
    req.rec.height 	= rec_pix_h[res];
    req.rec.format 	= VIDEO4_REC_FORMAT;
    req.rec.state 	= VIDEO_STATE_SAVE_FILE;
    req.rec.file    = __this->file[id];
    req.rec.fps 	= 0;
    req.rec.real_fps 	= 0;

#if (defined THREE_WAY_ENABLE || defined CONFIG_USB_VIDEO_OUT)
    req.rec.rec_small_pic 	= 0;
#else
    req.rec.rec_small_pic 	= 1;
#endif

    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }

    req.rec.cycle_time = req.rec.cycle_time * 60;

    /*
     *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     */
    u32 gap_time = db_select("gap");
    req.rec.slow_motion = 0;

    if (gap_time) {
        /* req.rec.tlp_time = gap_time; */
        /* req.rec.real_fps = 1000 / req.rec.tlp_time; */
        req.rec.gap_fps  = gap_time / 40;
        req.rec.pkg_fps	 = 30;
    }
    if (req.rec.slow_motion || gap_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel 	= 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    } else {
        req.rec.audio.fmt_format = AUDIO_FMT_PCM;
        req.rec.audio.sample_rate = 8000;
        req.rec.audio.channel 	= 1;
        req.rec.audio.volume    = 100;
        req.rec.audio.buf = __this->audio_buf[id];
        req.rec.audio.buf_len = AUDIO_BUF_SIZE;
    }
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    err = server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("\nvideo4 save rec%d err 0x%x\n", id, err);
        return VREC_ERR_V0_REQ_SAVEFILE;
    }

    return 0;
}


static void video4_rec_close(u8 id)
{
    if (__this->video_rec[id]) {
        server_close(__this->video_rec[id]);
        __this->video_rec[id] = NULL;
    }
}

/*
 *必须在启动录像之后才可调用该函数，并且确保启动录像时已经打开了osd
 *新设置的osd的整体结构要和启动录像时一样，只是内容改变!!!
 */
static int video4_rec_set_osd_str(u8 id, char *str)
{
    union video_req req = {0};
    int err;
    if (!__this->video_rec[id]) {
        return -1;
    }

    req.rec.channel = id;
    req.rec.state 	= VIDEO_STATE_SET_OSD_STR;
    req.rec.new_osd_str = str;
    err = server_request(__this->video_rec[id], VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("\nvideo4 set osd rec%d str err 0x%x\n", id, err);
        return -1;
    }

    return 0;
}

#endif

/* const static u8 mov_user_data[] = "demo 1234567890 abcdefghijklmnopqrstuvwxyz"; */
/* int mov_pkg_insert_user_data(u8** buf) */
/* { */
/* *buf = mov_user_data; */
/* return sizeof(mov_user_data); */
/* } */

static int video_rec_start()
{
    //return 0;
    int err;                 // 用于存储函数返回的错误码
    u32 clust;               // 用于存储簇（cluster）数，用于磁盘操作
    u8 state = __this->state; // 用于存储录像模块当前的状态

    // 初始化录像模块的等待字符标志和重启录像标志
    __this->char_wait = 0;
    __this->need_restart_rec = 0;

    // 如果当前录像模块的状态是正在录像（VIDREC_STA_START），则直接返回0，表示无需重新启动录像
    if (__this->state == VIDREC_STA_START) {
        return 0;
    }

    // 输出调试日志，提示进入录像启动函数
    log_d("(((((( video_rec_start: in\n");

    // 检查存储设备是否可用，如果不可用则直接返回0，表示不能启动录像
    if (!storage_device_available()) {
        return 0;
    }

    /*
     * 申请录像所需要的音频和视频帧缓冲区（buf）
     */
    // 这里是留给后续实现的注释，说明此处的代码将会进行音视频帧缓冲区的申请操作

#ifndef VIDEO_REC_NO_MALLOC
    int abuf_size[] = {AUDIO_BUF_SIZE, AUDIO_BUF_SIZE, AUDIO_BUF_SIZE, AUDIO_BUF_SIZE}; // 定义音频缓冲区大小数组
    for (int i = 0; i < ARRAY_SIZE(abuf_size); i++) {
        if (abuf_size[i]) {
            if (!__this->audio_buf[i]) { // 检查音频缓冲区是否已分配
                __this->audio_buf[i] = malloc(abuf_size[i]); // 分配音频缓冲区
                if (__this->audio_buf[i] == NULL) { // 检查分配是否成功
                    log_i("err maloo\n"); // 输出错误信息
                    while (1); // 进入死循环，表示严重错误
                }
            }
        } else {
            __this->audio_buf[i] = NULL; // 如果大小为0，则设置为NULL
        }
    }
#endif




#ifndef VIDEO_REC_NO_MALLOC
#ifdef CONFIG_VIDEO4_ENABLE
    int buf_size[] = {VREC4_FBUF_SIZE, VREC4_FBUF_SIZE, VREC4_FBUF_SIZE, VREC4_FBUF_SIZE}; // 视频缓冲区大小
#else
#ifndef CONFIG_SINGLE_VIDEO_REC_ENABLE
    int buf_size[] = {VREC0_FBUF_SIZE, VREC1_FBUF_SIZE, VREC2_FBUF_SIZE, VREC3_FBUF_SIZE}; // 多路录像情况下的缓冲区大小
#else
    int buf_size[] = {VREC0_FBUF_SIZE}; // 单路录像情况下的缓冲区大小
#endif
#endif
    for (int i = 0; i < ARRAY_SIZE(buf_size); i++) {
        if (buf_size[i]) {
            if (!__this->video_buf[i]) { // 检查视频缓冲区是否已分配
                __this->video_buf[i] = malloc(buf_size[i]); // 分配视频缓冲区
                if (__this->video_buf[i] == NULL) { // 检查分配是否成功
                    log_i("err maloo\n"); // 输出错误信息
                    while (1); // 进入死循环，表示严重错误
                }
            }
        } else {
            __this->video_buf[i] = NULL; // 如果大小为0，则设置为NULL
        }
    }
#endif

    /*
     * 判断SD卡空间，删除旧文件并创建新文件
     */
    err = video_rec_del_old_file(); // 删除旧文件并检查空间
    if (err) {
        log_e("start free space err\n"); // 输出错误日志
        video_rec_post_msg("fsErr"); // 发送文件系统错误消息
        return VREC_ERR_START_FREE_SPACE; // 返回空间错误
    }

    // 将新文件指针赋值给文件指针
    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        __this->file[i] = __this->new_file[i];
        __this->new_file[i] = NULL; // 清空新文件指针
    }

#if 1 // 判断是否启用视频0
    if(__this->video_online[2]) { // 检查视频0是否在线
        err = video0_rec_start(); // 启动视频0录制
    }
    if (err) { // 检查启动是否成功
        video0_rec_stop(0); // 停止录像
        return err; // 返回错误
    }
#endif



#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE) // 检查视频1是否启用
    if (__this->video_online[1]) {
        printf(">>>>>>>>%s %d\n",__func__,__LINE__); // 输出调试信息
        err = video1_rec_start(); // 启动视频1录制
    }
    if (err) {
        video1_rec_stop(0); // 停止录像
        return err; // 返回错误
    }
#endif


#ifdef CONFIG_VIDEO3_ENABLE // 检查视频3是否启用
    if (__this->video_online[3]) {
        err = video3_rec_start(); // 启动视频3录制
    }
    if (err) {
        video3_rec_stop(0); // 停止录像
        return err; // 返回错误
    }
#endif
    video_rec_post_msg("onREC");
    video_parking_post_msg("onREC");
#ifdef CONFIG_VIDEO4_ENABLE
    int id; // 视频4的ID
    for (id = 0; id < CONFIG_VIDEO_REC_NUM; id++) {
        if (__this->video_online[id]) { // 检查视频是否在线
            err = video4_rec_start(id); // 启动视频4录制
            if (err != 0) {
                log_i("video4 start err out\n"); // 输出错误信息
                return -1; // 返回错误
            }
        }
    }
#endif
    // 检查传感器锁定状态
    if (__this->gsen_lock == 0xff) {
        video_rec_post_msg("lockREC"); // 发送锁定录像消息
    }

#ifndef CONFIG_VIDEO4_ENABLE
#ifndef CONFIG_DISPLAY_ENABLE
    // video_rec_set_white_balance(); // 可选：设置白平衡
    video_rec_set_exposure(db_select("exp")); // 设置曝光
#endif
#endif

    sys_power_auto_shutdown_pause(); // 暂停自动关机

    __this->state = VIDREC_STA_START; // 更新状态为正在录像

    log_d("video_rec_start: out )))))))\n"); // 输出调试日志
    /* malloc_dump(); */
    malloc_stats(); // 输出内存统计信息

    return 0;
}


static int video_rec_len()
{

    /* if (db_select("mic")) { */
    /* puts("mic on\n"); */
    /* video_rec_post_msg("onMIC"); */
    /* } else { */
    /* puts("mic off\n"); */
    /* video_rec_post_msg("offMIC"); */
    /* } */

    if (__this->state != VIDREC_STA_START) {
        return 0;
    }

#if 1//def CONFIG_VIDEO0_ENABLE
    video0_rec_len();
#endif

#ifdef CONFIG_VIDEO1_ENABLE
    video1_rec_len();
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    video3_rec_len();
#endif

#ifdef CONFIG_VIDEO4_ENABLE
    int id;
    for (id = 0; id < CONFIG_VIDEO_REC_NUM; id++) {
        video4_rec_len(id);
    }
#endif

    return 0;
}

static int video_rec_aud_mute()
{

    /* if (db_select("mic")) { */
    /* puts("mic on\n"); */
    /* video_rec_post_msg("onMIC"); */
    /* } else { */
    /* puts("mic off\n"); */
    /* video_rec_post_msg("offMIC"); */
    /* } */

    if (__this->state != VIDREC_STA_START) {
        return 0;
    }

#if 1//def CONFIG_VIDEO0_ENABLE
    video0_rec_aud_mute();
#endif

#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    video1_rec_aud_mute();
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    video3_rec_aud_mute();
#endif

#ifdef CONFIG_VIDEO4_ENABLE
    int id;
    for (id = 0; id < CONFIG_VIDEO_REC_NUM; id++) {
        video4_rec_aud_mute(id);
    }
#endif

    return 0;
}


static int video_rec_stop(u8 close)
{
    int err;
    __this->need_restart_rec = 0;

    if (__this->state != VIDREC_STA_START) {
        return 0;
    }

    puts("\nvideo_rec_stop\n");

    __this->state = VIDREC_STA_STOPING;

#if 1//def CONFIG_VIDEO0_ENABLE
    err = video0_rec_stop(close);
    if (err) {
        puts("\nstop0 err\n");
    }
#endif

#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    err = video1_rec_stop(close);
    if (err) {
        puts("\nstop1 err\n");
    }
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    err = video3_rec_stop(close);
    if (err) {
        puts("\nstop3 err\n");
    }
#endif

#ifdef CONFIG_VIDEO4_ENABLE
    int id;
    int other_id;
    union video_req req = {0};
    for (id = 0; id < CONFIG_VIDEO_REC_NUM; id++) {

        /* if ((id == 0) || (id == 1)) { */
        if (id == 0) {
            for (other_id = id + 1; other_id < CONFIG_VIDEO_REC_NUM; other_id++) {
                if (__this->video_rec[other_id]) {
                    req.rec.IP_interval = 1;
                    req.rec.channel = other_id;
                    req.rec.state = VIDEO_STATE_V4_PAUSE_RUN;
                    err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req);
                }
            }
        }
        if (id == 0) {
            for (other_id = id + 1; other_id < CONFIG_VIDEO_REC_NUM; other_id++) {
                if (__this->video_rec[other_id]) {
                    req.rec.pkg_mute.aud_mute = 1;
                    req.rec.channel = other_id;
                    req.rec.state = VIDEO_STATE_PKG_PAUSE_RUN;
                    err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req);
                }
            }
        }

        err = video4_rec_stop(id, close);
        if (err) {
            log_i("\nvideo4 stop%d err\n", id);
        }

        /* if ((id == 0) || (id == 1)) { */
        if (1) {
            /* for (other_id = id + 1; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
            other_id = id + 1;
            if (other_id < CONFIG_VIDEO_REC_NUM) {
                if (__this->video_rec[other_id]) {
                    req.rec.IP_interval = 0;
                    req.rec.channel = other_id;
                    req.rec.state = VIDEO_STATE_V4_PAUSE_RUN;
                    err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req);
                }
            }
        }
        /* if (id == 0) { */
        /* [> for (other_id = id + 1; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { <] */
        /* for (other_id = id; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
        /* if (__this->video_rec[other_id]) { */
        /* req.rec.pkg_mute.aud_mute = 0; */
        /* req.rec.channel = other_id; */
        /* req.rec.state = VIDEO_STATE_PKG_PAUSE_RUN; */
        /* err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req); */
        /* } */
        /* } */
        /* } */

    }

#endif

#ifndef CONFIG_VIDEO4_ENABLE
    if (__this->disp_state == DISP_BACK_WIN) {
        video_rec_post_msg("HlightOff"); //后视停录像关闭前照灯
    }
#endif

    __this->state = VIDREC_STA_STOP;
    __this->gsen_lock = 0;
    sys_power_auto_shutdown_resume();

    video_rec_get_remain_time();
    video_rec_post_msg("offREC");
    video_home_post_msg("offREC");
    video_parking_post_msg("offREC");

    puts("video_rec_stop: exit\n");
    return 0;
}




static int video_rec_close()
{
#if 1//def CONFIG_VIDEO0_ENABLE
    video0_rec_close();
#endif

#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    video1_rec_close();
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    video3_rec_close();
#endif

#ifdef CONFIG_VIDEO4_ENABLE
    for (int id = 0; id < CONFIG_VIDEO_REC_NUM; id++) {
        video4_rec_close(id);
    }
#endif


    return 0;
}

static int video_rec_change_source_reso(int dev_id, u16 width, u16 height)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    if (dev_id == 0) {

    } else
#endif
#ifdef CONFIG_VIDEO1_ENABLE
        if (dev_id == 1) {

        } else
#endif
#ifdef CONFIG_VIDEO3_ENABLE  // 如果启用了 CONFIG_VIDEO3 配置项
            if (dev_id == 3) {  // 检查设备 ID 是否为 3
                __this->src_width[3] = width;  // 设置视频源宽度
                __this->src_height[3] = height;  // 设置视频源高度
                if (__this->video_online[3]) {  // 如果视频3在线
                    log_d("video3.* change source reso to %d x %d\n", width, height);  // 输出日志，记录视频3分辨率更改
                    int rec_state = __this->state;  // 获取当前录像状态
                    int disp_state = __this->disp_state;  // 获取当前显示状态
                    void *video3_disp = __this->video_display[3];  // 获取视频3的显示指针
                    if (rec_state == VIDREC_STA_START) {  // 如果录像状态为开始
                        video_rec_stop(0);  // 停止视频录制
                    }
                    if (disp_state == DISP_MAIN_WIN ||  // 如果显示状态为主窗口
                        disp_state == DISP_HALF_WIN ||  // 或者显示状态为半窗口
#ifndef CONFIG_VIDEO4_ENABLE  // 如果没有启用 CONFIG_VIDEO4 配置项
                        disp_state == DISP_BACK_WIN ||  // 或者显示状态为后窗显示

#else
                        disp_state == DISP_VIDEO3 ||
#endif
                        disp_state == DISP_PARK_WIN) {  // 检查当前显示状态是否为停车窗口
                        if (video3_disp != NULL) {  // 如果 video3_disp 不为空（表示视频显示服务已打开）
                            video_disp_stop(1);  // 停止ID为3的视频显示服务
                            // video_disp_stop(1);  // 停止ID为3的视频显示服务
                        }
                    }
                    // 在更改视频源分辨率之前，必须关闭 video3.* 的录制和显示
                    if (rec_state == VIDREC_STA_START) {  // 如果录像状态为开始
                        // video_rec_start();  // 开始视频录制
                    }
                    if (disp_state == DISP_MAIN_WIN ||  // 如果显示状态为主窗口
                        disp_state == DISP_HALF_WIN ||  // 或者显示状态为半窗口

#ifndef CONFIG_VIDEO4_ENABLE  // 如果没有启用 CONFIG_VIDEO4 配置项
                        disp_state == DISP_BACK_WIN ||  // 如果显示状态为后窗显示
#else  // 否则
                        disp_state == DISP_VIDEO3 ||  // 如果启用了 CONFIG_VIDEO4，则显示状态为视频3窗口
#endif
                        disp_state == DISP_PARK_WIN) {  // 或者显示状态为停车窗口
                        if (video3_disp != NULL) {  // 如果视频3显示对象不为空
#ifdef CONFIG_VIDEO4_ENABLE  // 如果启用了 CONFIG_VIDEO4 配置项
                            video_disp_start(3, &disp_window[disp_state][3]);  // 启动视频3显示，使用对应状态的显示窗口参数
#elif defined THREE_WAY_ENABLE  // 否则如果启用了 THREE_WAY 配置项
                            video_disp_start(3, &disp_window[disp_state][3]);  // 启动视频3显示，使用对应状态的显示窗口参数
#else  // 如果都没有启用
                            video_disp_start(3, &disp_window[disp_state][1]);  // 启动视频3显示，使用主窗口参数
#endif
                        }
                    }
                }
            } else  // 如果不是设备3
#endif
            {
                // 保留这个括号以避免语法错误，即使没有实际操作
            }
    return 0;  // 返回0，表示函数执行成功
}
// 设置显示旋转
 void video_set_disp_rotate(int id, u16 rotate)
{
    // 遍历所有显示窗口，设置指定ID的窗口的旋转属性
    for (int i = 0; i < DISP_MAX_WIN; i++) {
        disp_window[i][id].rotate = rotate; // 将旋转值赋给相应窗口
        video_disp_stop(0);
        video_disp_start(0, &disp_window[3][3]);
    }
}

static int video_rec_savefile(int dev_id)
{
    int i;
    int err;
    int post_msg = 0;
    union video_req req = {0};

    // 如果当前状态不是正在录像，则返回0
    if (__this->state != VIDREC_STA_START) {
        return 0;
    }

    // 禁用系统按键事件
    sys_key_event_disable();
    // 禁用触摸事件
    sys_touch_event_disable();

    // 如果需要重新开始录像
    if (__this->need_restart_rec) {
        log_d("need restart rec");
        // 停止录像
        video_rec_stop(0);
        // 重新开始录像
        // video_rec_start();
        // 重新启用按键事件
        sys_key_event_enable();
        // 重新启用触摸事件
        sys_touch_event_enable();
        return 0;
    }

    // 打印日志，开始新文件录像
    log_d("\nvideo_rec_start_new_file: %d\n", dev_id);


    /* #ifdef CONFIG_VIDEO4_ENABLE */
    /* int other_id; */
    /* for (other_id = 0; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
    /* [> if ((other_id != dev_id) && __this->video_online[other_id] && __this->video_rec[other_id]) { <] */
    /* if (__this->video_online[other_id] && __this->video_rec[other_id]) { */
    /* req.rec.IP_interval = 1; */
    /* req.rec.channel = other_id; */
    /* req.rec.state = VIDEO_STATE_V4_PAUSE_RUN; */
    /* err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req); */
    /* } */
    /* } */
    /* #endif */

    video_rec_close_file(dev_id);

    /* #ifdef CONFIG_VIDEO4_ENABLE */
    /* for (other_id = 0; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
    /* [> if ((other_id != dev_id) && __this->video_online[other_id] && __this->video_rec[other_id]) { <] */
    /* if (__this->video_online[other_id] && __this->video_rec[other_id]) { */
    /* req.rec.IP_interval = 0; */
    /* req.rec.channel = other_id; */
    /* req.rec.state = VIDEO_STATE_V4_PAUSE_RUN; */
    /* err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req); */
    /* } */
    /* } */
    /* #endif */


    if (__this->new_file[dev_id] == NULL) {
        err = video_rec_del_old_file();
        if (err) {
            video_rec_post_msg("fsErr");
            goto __err;
        }
        post_msg = 1;
    }
    __this->file[dev_id]     = __this->new_file[dev_id];
    __this->new_file[dev_id] = NULL;

#if 1//def CONFIG_VIDEO0_ENABLE
    if (dev_id == 0) {
        err = video0_rec_savefile();
        if (err) {
            log_i("\n\n\nv0_serr %x\n\n", err);
            goto __err;
        }
    }
#endif

    if (post_msg) {
        video_rec_post_msg("saveREC");
        video_home_post_msg("saveREC");//录像切到后台,ui消息由主界面响应
        video_parking_post_msg("saveREC");
    }

#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    if (__this->video_online[1] && (dev_id == 1)) {
        err = video1_rec_savefile();
        if (err) {
            log_i("\n\n\nv1_serr %x\n\n", err);
            goto __err;
        }
    }
#endif

    /* puts("\n\n------save2\n\n"); */
#ifdef CONFIG_VIDEO3_ENABLE
    if (__this->video_online[3] && (dev_id == 3)) {
        err = video3_rec_savefile();
        if (err) {
            goto __err;
        }
    }
#endif

#ifdef CONFIG_VIDEO4_ENABLE

    /* for (other_id = 0; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
    /* if ((other_id != dev_id) && __this->video_online[other_id] && __this->video_rec[other_id]) { */
    /* req.rec.channel = other_id; */
    /* req.rec.state = VIDEO_STATE_PKG_PAUSE_RUN; */
    /* err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req); */
    /* } */
    /* } */

    if (__this->video_online[dev_id]) {
        err = video4_rec_savefile(dev_id);
        if (err) {
            log_i("\n\n\nv%d_serr %x\n\n", dev_id, err);
            goto __err;
        }
    }

    /* for (other_id = 0; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
    /* if ((other_id != dev_id) && __this->video_online[other_id] && __this->video_rec[other_id]) { */
    /* req.rec.channel = other_id; */
    /* req.rec.state = VIDEO_STATE_PKG_PAUSE_RUN; */
    /* err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req); */
    /* } */
    /* } */

#endif
    __this->state = VIDREC_STA_START;

#if (APP_CASE == __WIFI_CAR_CAMERA__)
    video_rec_state_notify();
#endif
    sys_key_event_enable();
    sys_touch_event_enable();

    malloc_stats();


    return 0;


__err:


#ifdef CONFIG_VIDEO4_ENABLE

    /* for (other_id = 0; other_id < CONFIG_VIDEO_REC_NUM; other_id++) { */
    /* [> if ((other_id != dev_id) && __this->video_online[other_id] && __this->video_rec[other_id]) { <] */
    /* if (__this->video_online[other_id] && __this->video_rec[other_id]) { */
    /* req.rec.IP_interval = 0; */
    /* req.rec.channel = other_id; */
    /* req.rec.state = VIDEO_STATE_V4_PAUSE_RUN; */
    /* err = server_request(__this->video_rec[other_id], VIDEO_REQ_REC, &req); */
    /* } */
    /* } */

    for (dev_id = 0; dev_id < CONFIG_VIDEO_REC_NUM; dev_id++) {
        err = video4_rec_stop(dev_id, 0);
        if (err) {
            printf("\nsave wrong%d %x\n", dev_id, err);
        }
    }
#endif


#ifdef CONFIG_VIDEO3_ENABLE
    err = video3_rec_stop(0);
    if (err) {
        printf("\nsave wrong3 %x\n", err);
    }
#endif

#if (defined CONFIG_VIDEO1_ENABLE  && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    err = video1_rec_stop(0);
    if (err) {
        printf("\nsave wrong1 %x\n", err);
    }
#endif

#if 1//def CONFIG_VIDEO0_ENABLE
    err = video0_rec_stop(0);
    if (err) {
        printf("\nsave wrong0 %x\n", err);
    }
#endif


    video_rec_post_msg("offREC");
    video_home_post_msg("offREC");//录像切到后台,ui消息由主界面响应
    video_parking_post_msg("offREC");
    __this->state = VIDREC_STA_STOP;

    sys_key_event_enable();
    sys_touch_event_enable();

    return -EFAULT;

}

static int set_label_config(u16 image_width, u16 image_height, u32 font_color,struct video_text_osd *label)
{
    static char label_format[128] ALIGNE(64) = "yyyy-nn-dd hh:mm:ss";
    if (!label) {
        return 0;
    }

    /*
     *日期标签设置
     *1920以下使用 16x32字体大小，以上使用32x64字体
     */
#ifdef __CPU_AC521x__
    label->direction = 1;
#else
    label->direction = 0;
#endif
    if (image_width > 1920) {
        return -1;

    } else {
        label->font_w = 16;
        label->font_h = 32;
        label->text_format = label_format;
        label->font_matrix_table = osd_str_total;
        label->font_matrix_base = osd_str_matrix;
        label->font_matrix_len = sizeof(osd_str_matrix);
    }
    label->color[0] = font_color;

    label->x = 192;//(image_width - strlen(label_format) * label->font_w-128) / 64 * 64;
    label->y = (image_height - label->font_h - 32) / 16 * 16-64;


    return 0;
}
/* 用于录像模式下拍照  sel: 0表示前视 1表示后视 */
int shot_flag=0;
static int video_take_photo(u8 sel)
{
    u8 date_name_flag = 0;
    char name_buf[64]={0};
    char file_name[64]={0};
    struct sys_time time;
    int camera0_reso[][2] = {
        {640,  480},  //                VGA
        {1280, 720},  // {1024, 768}    1M
        {1920, 1088}, // {1600, 1200}   2M
        {2048, 1536}, //                3M
        {2560, 1936}, //                4M
        {2592, 1944}, //                5M
        {3072, 1944}, //                6M
        {3456, 1944}, //                7M
        {3456, 2448}, //                8M
        {3456, 2592}, //                9M
        {3648, 2736}, //                10M
        {4032, 2736}, //                11M
        {4032, 3024}, //                12M
    };
    int err = 0;
//    int res = db_select("pres");
    union video_req req = {0};
    struct icap_auxiliary_mem aux_mem;
    struct jpg_thumbnail thumbnails;
    struct server *server;
    char video_name[8];
    int i = 1;
    struct vfs_partition *part;
    void *cap_buf=NULL;
    void *aux_buf=NULL;
    void *thumbnails_buf=NULL;
    char buf[128] = {0};
    i = db_select("cyt");
    if(i == 0){
        i = 1;
    }
    if (!storage_device_ready()) {
        if (!dev_online(SDX_DEV)) {
            video_rec_post_msg("noCard");
        } else {
            video_rec_post_msg("fsErr");
        }
        return -ENODEV;
    } else {
        part = fget_partition(CONFIG_ROOT_PATH);
        if (part->clust_size < 32 || (part->fs_attr & F_ATTR_RO)) {
            video_rec_post_msg("fsErr");
            return -ENODEV;
        }
    }

    /*
     *打开相机对应的video服务
     */
#ifdef CONFIG_VIDEO4_ENABLE
    sprintf(video_name, "video4.%d.0", (sel == 1) ? 3 : 0);
#else
    sprintf(video_name, "video%d.0", sel);
#endif
    if (!server) {
        server = server_open("video_server", video_name);
    }

    if (!server) {
        return -EFAULT;
    }
    printf("video_server:%p\n", server);
    #if (SDRAM_SIZE == (64 * 1024 * 1024))
    req.icap.width = rec_pix_w[VIDEO_RES_1080P];
    req.icap.height = rec_pix_h[VIDEO_RES_1080P];
    #else
    req.icap.width = 1280;
    req.icap.height = 720;
    #endif

    /*
     *设置拍照所需要的buffer
     */
    cap_buf = (u8 *)malloc(2 * 1024 * 1024);

    if (req.icap.width > 1920 && !aux_buf) {
        /*
         *设置尺寸较大时缩放所需要的buffer
         */
        aux_buf = (u8 *)malloc(2 * 1024 * 1024 + 160 * 1024);
        thumbnails_buf = (u8 *)malloc(64 * 1024);
        if (!aux_buf || !thumbnails_buf) {
            err = -ENOMEM;
            goto __err;
        }
    }
    if(sel==1){
      __this->photo_camera_sel=1;
    }else{
     __this->photo_camera_sel=0;
    }

    aux_mem.addr = aux_buf;
    aux_mem.size = 2 * 1024 * 1024 + 160 * 1024;

    /*
     *配置拍照服务参数
     *尺寸、buffer、质量、日期标签
     */

    req.icap.quality = db_select("qua");

    if (__get_sys_time(&time) == 0) {
        date_name_flag=1;
        if(__this->photo_camera_sel == 0){
            sprintf(file_name, CAMERA0_CAP_PATH"IMG_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        } else if (__this->photo_camera_sel == 1) {
            sprintf(file_name, CAMERA1_CAP_PATH"IMG_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        } else if (__this->photo_camera_sel == 2) {
            sprintf(file_name, CAMERA1_CAP_PATH"IMG_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        }else{
            sprintf(file_name, CAMERA1_CAP_PATH"IMG_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        }
        req.icap.path = file_name;
    }else{
        date_name_flag=0;
        if (__this->photo_camera_sel == 0) {
            req.icap.path = CAMERA0_CAP_PATH"IMG_****.JPG";
        } else if (__this->photo_camera_sel == 1) {
            req.icap.path = CAMERA1_CAP_PATH"IMG_****.JPG";
        } else if (__this->photo_camera_sel == 2) {
            req.icap.path = CAMERA1_CAP_PATH"IMG_****.JPG";
        } else {
            req.icap.path = CAMERA1_CAP_PATH"IMG_****.JPG";
        }
    }

//    req.icap.path = sel == 0 ? CAMERA0_CAP_PATH"IMG_***.JPG" : CAMERA1_CAP_PATH"IMG_***.JPG";
    req.icap.buf = cap_buf;
    req.icap.buf_size = 2 * 1024 * 1024;
    req.icap.aux_mem = &aux_mem;
    req.icap.camera_type = VIDEO_CAMERA_NORMAL;
#ifdef JPG_THUMBNAILS_ENABLE
    if (req.icap.width > 1920) {
        thumbnails.enable = 1;
        thumbnails.quality = 10;
        thumbnails.width = 480;
        thumbnails.height = 320;
        thumbnails.buf = thumbnails_buf;
        thumbnails.len = 64 * 1024;
        req.icap.thumbnails = &thumbnails;
    }
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    if (sel == 1) {
        req.icap.camera_type = VIDEO_CAMERA_UVC;
        req.icap.uvc_id = __this->uvc_id;
    }
#endif

    /* req.icap.label = NULL;  */
    struct video_text_osd label;
    struct video_graph_osd graph_label;

        req.icap.text_label = db_select("pdat") ? &label : NULL;
#ifdef CONFIG_OSD_LOGO
         req.icap.graph_label =db_select("pdat") ? &graph_label : NULL;
#endif // CONFIG_OSD_LOGO
    set_label_config(req.icap.width, req.icap.height, 0xe20095, req.icap.text_label);
    #if 0
        graph_label.bit_mode = 16;//2bit的osd需要配置3个color
        if(req.icap.height>=1080)
        {
        graph_label.x = 288;
        graph_label.y = 800;
        }else if(req.icap.height==720)
        {
        graph_label.x = 192;
        graph_label.y = 504;
        }else {
        graph_label.x = 98;
        graph_label.y = 296;
        }
        graph_label.width = 128;
        graph_label.height = 32;

        graph_label.icon = (u8 *)icon_16bit_data; //icon_osd_buf;
        graph_label.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
    #endif
if(shot_flag==1)
{
     while (i--) {
        /*
         *发送拍照请求
         */
        err = server_request(server, VIDEO_REQ_IMAGE_CAPTURE, &req);
        delay_2ms(500);
        if (err != 0) {
            puts("take photo err.\n");
            goto __err;
        }
        key_voice_start(1);
    }

}else {
        /*
         *发送拍照请求
         */
        err = server_request(server, VIDEO_REQ_IMAGE_CAPTURE, &req);
        if (err != 0) {
            puts("take photo err.\n");
            goto __err;
        }
        key_voice_start(1);
    }
 #if (APP_CASE == __WIFI_CAR_CAMERA__)
    if(date_name_flag){
        sprintf(buf, "%s", req.icap.path);
    }else{
        sprintf(buf, "%s", req.icap.file_name);
    }
    printf("*********rec_photo_user:%s,%s\n",buf,req.icap.path);
    FILE_LIST_ADD(0, buf, 0);
#endif

    /* return 0; */
__err:
    if (server) {
        server_close(server);
    }
    if (cap_buf) {
        free(cap_buf);
    }
    if (aux_buf) {
        free(aux_buf);
    }
    if(thumbnails_buf){
        free(thumbnails_buf);
    }

    /* malloc_stats(); */
    return err;

}
int delay_timeout_flag = 0;         //延时拍照定时器
int temp_camera = 0;
static void delay_take_photo_now(void)
{
    video_take_photo(temp_camera);

}

static int video_rec_delay_photo(u8 sel)
{
    int delay_time = 0;     //拍照时间
    temp_camera = sel;
    if (!storage_device_ready()) {
        if (!dev_online(SDX_DEV)) {
            video_rec_post_msg("noCard");
        } else {
            video_rec_post_msg("fsErr");
        }
        return -ENODEV;
    }

    if(delay_time > 0){
        printf("delay_time ============== %d ,line == %d\n",delay_time,__LINE__);
        return 0;
    }
    delay_time = db_select("phm") * 1000;
    printf("delay_time ============== %d ,line == %d\n",delay_time,__LINE__);
    if(delay_timeout_flag){
        sys_timeout_del(delay_timeout_flag);
        delay_timeout_flag = 0;
    }

    sys_key_event_disable();
    sys_touch_event_disable();
    video_rec_post_msg("dlyREC:ms=%4", delay_time);

    delay_timeout_flag = sys_timeout_add(NULL,delay_take_photo_now,delay_time);

    if (!delay_timeout_flag) {
        sys_key_event_enable();
        sys_touch_event_enable();
        return -EFAULT;
    }

    return 0;
}
/*
 * 录像时拍照的控制函数, 不能单独调用，必须录像时才可以调用，实际的调用地方已有
 * 录像时拍照会需要至少1.5M + 400K的空间，请根据实际情况来使用
 */
 int video_rec_take_photo(void)
{
    struct server *server;
    union video_req req = {0};  // 视频请求结构体初始化
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;
    u8 date_name_flag = 0;
    char name_buf[64]={0};  // 文件名缓冲区初始化
    char file_name[64]={0}; // 另一个文件名缓冲区初始化
    char buf[128] = {0};    // 临时缓冲区初始化
    struct sys_time time;
    int err;

    // 打印调试信息，显示当前函数名称、行号及状态信息
    printf("====%s:%d   %d,%d\n",__func__,__LINE__,__this->state,disp_state);

    // 判断当前显示状态，选择摄像头
    switch(disp_state){
        case DISP_720AHD_FULL://VIDEO1
        __this->photo_camera_sel=1;  // 选择摄像头1
        break;
        case DISP_1080AHD_FULL://VIDEO2
            __this->photo_camera_sel=2;
        break;
        case DISP_UVC_FULL://VIDEO3
            __this->photo_camera_sel=3;
        break;
    }

#ifdef CONFIG_VIDEO4_ENABLE
    server = __this->video_rec[__this->photo_camera_sel];  // 根据摄像头选择视频录制服务
#else
    printf("photo_camera_sel = %d",__this->photo_camera_sel);
    if (__this->photo_camera_sel == 0) {
        server = __this->video_rec0;  // 摄像头0
    } else if (__this->photo_camera_sel == 1) {
        server = __this->video_rec1;  // 摄像头1
    } else if (__this->photo_camera_sel == 2) {
        server = __this->video_rec0;  // 摄像头2
    } else if(__this->photo_camera_sel==3){
        server = __this->video_rec3;
    } else {
        server = NULL;
    }

    printf("server = %p\n",server);
#endif
    // printf("99999999999999999999999999999999999999999999999999999999999999999999");
    // 如果状态不对或者 server 为空，返回错误码
    if ((__this->state != VIDREC_STA_START) || (server == NULL)) {
        return -EINVAL;  // 无效参数错误
    }
    // printf("8888888888888888888888888888888888888888888888888888888888888888");
    // 分配捕获缓冲区
    if (__this->cap_buf == NULL) {
#ifndef VIDEO_REC_NO_MALLOC
        __this->cap_buf = (u8 *)malloc(CAMERA_CAP_BUF_SIZE);  // 动态分配内存
#endif
        if (!__this->cap_buf) {
            puts("\ntake photo no mem\n");  // 打印内存不足信息
            return -ENOMEM;  // 返回内存不足错误
        }
    }
    // printf("77777777777777777777777777777777777777777777777777777777777777");
    // 根据不同的摄像头选择分辨率
    if(__this->photo_camera_sel == 0){
        if(video0_rec_width >= 1920){
            req.icap.width = 1920;
            req.icap.height = 1080;  // 设置摄像头0为1080p
        }else if(video0_rec_width>=1280){
            req.icap.width = 1280;
            req.icap.height = 720;   // 设置摄像头0为720p
        }else if(video0_rec_width>=640){
            req.icap.width = 640;
            req.icap.height = 480;   // 设置摄像头0为480p
        }else{
            req.icap.width = 1280;
            req.icap.height = 720;   // 默认设置为720p
        }
    }else{
        if(video1_rec_width >= 1920){
            req.icap.width = 1920;
            req.icap.height = 1080;  // 设置摄像头1为1080p
        }else if(video1_rec_width>=1280){
            req.icap.width = 1280;
            req.icap.height = 720;   // 设置摄像头1为720p
        }else if(video1_rec_width>=640){
            req.icap.width = 640;
            req.icap.height = 480;   // 设置摄像头1为480p
        }else{
            req.icap.width = 1280;
            req.icap.height = 720;   // 默认设置为720p
        }
    }
    // printf("666666666666666666666666666666666666666666666666666666666");
    // 内存大小检查，针对不同的SDRAM大小调整分辨率
#if (SDRAM_SIZE == (64 * 1024 * 1024))
    if(__this->photo_camera_sel == 0){
        if(req.icap.width>=1920){
            req.icap.width = 1920;
            req.icap.height = 1080;  // 限制为1080p
        }
    }else{
        if(req.icap.width>=AVIN_WIDTH){
            req.icap.width = AVIN_WIDTH;
            req.icap.height = AVIN_HEIGH;  // 设置为AVIN输入的最大分辨率
        }
    }
#else
    if(__this->photo_camera_sel == 0){
        if(req.icap.width>=1280){
            req.icap.width = 1280;
            req.icap.height = 720;   // 限制为720p
        }
    }else{
        if(req.icap.width>=1280){
            req.icap.width = 1280;
            req.icap.height = 720;   // 限制为720p
        }
    }
#endif

#ifdef VIDEO0_USE_1296P
    req.icap.width = 1280;
    req.icap.height = 720;  // 特殊宏定义情况下，强制为720p
#endif

    req.icap.quality = VIDEO_MID_Q;  // 设置照片质量为中等
    req.icap.text_label = NULL;      // 不设置文本标签
    req.icap.buf = __this->cap_buf;  // 使用分配的捕获缓冲区
    req.icap.buf_size = CAMERA_CAP_BUF_SIZE;  // 设置缓冲区大小
    req.icap.file_name = name_buf;   // 文件名缓冲区
#ifdef CONFIG_VIDEO4_ENABLE
    req.icap.camera_type = VIDEO_CAMERA_MUX;
#endif
    // printf("5555555555555555555555555555555555555555555555555555555555");
    if (__get_sys_time(&time) == 0) {
        date_name_flag=1;
        if(__this->photo_camera_sel == 0){
            sprintf(file_name, CAMERA0_CAP_PATH"CAP_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        } else if (__this->photo_camera_sel == 1) {
            sprintf(file_name, CAMERA1_CAP_PATH"CAP_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        } else if (__this->photo_camera_sel == 2) {
            sprintf(file_name, CAMERA1_CAP_PATH"CAP_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        }else{
            sprintf(file_name, CAMERA1_CAP_PATH"CAP_%0d%02d%02d%02d%02d%02d.JPG",time.year, time.month, time.day, time.hour, time.min, time.sec);
        }
        req.icap.path = file_name;
    }else{
        date_name_flag=0;
        if (__this->photo_camera_sel == 0) {
            req.icap.path = CAMERA0_CAP_PATH"CAP_****.JPG";
        } else if (__this->photo_camera_sel == 1) {
            req.icap.path = CAMERA1_CAP_PATH"CAP_****.JPG";
        } else if (__this->photo_camera_sel == 2) {
            req.icap.path = CAMERA1_CAP_PATH"CAP_****.JPG";
        } else {
            req.icap.path = CAMERA1_CAP_PATH"CAP_****.JPG";
        }
    }

    req.icap.src_w = __this->src_width[__this->photo_camera_sel];
    req.icap.src_h = __this->src_height[__this->photo_camera_sel];
    // printf("444444444444444444444444444444444444444444444444444444");
    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
//    memset(video_photo_osd_buf,0,64);
//    memcpy(video_photo_osd_buf,osd_str_buf,sizeof(osd_str_buf));
//    text_osd.font_w = 16;
//    text_osd.font_h = 32;
//    max_one_line_strnum = strlen(video_photo_osd_buf);
//
//     osd_line_num = 1;
//     if (db_select("num")) {
//        osd_line_num = 2;
//     }
//    osd_max_heigh = (req.icap.height == 1088) ? 1080 : req.icap.height ;
//    text_osd.x = (req.icap.width - max_one_line_strnum * text_osd.font_w) / 64 * 64;
//    text_osd.y = (osd_max_heigh - text_osd.font_h * osd_line_num) / 16 * 16-160;
//    text_osd.color[0] = 0xe20095;
//    text_osd.bit_mode = 1;
//    text_osd.text_format = video_photo_osd_buf;
//    text_osd.font_matrix_table = osd_str_total;
//    text_osd.font_matrix_base = osd_mimc_str_matrix;//osd_str_matrix;
//    text_osd.font_matrix_len = sizeof(osd_mimc_str_matrix);
//
//    text_osd.direction = 1;

//    graph_osd.bit_mode = 16;//2bit的osd需要配置3个color
//    graph_osd.x = 0;
//    graph_osd.y = 0;
//    graph_osd.width = 256;
//    graph_osd.height = 256;
//    graph_osd.icon = (u8 *)icon_16bit_data;
//    graph_osd.icon_size = sizeof(icon_16bit_data);

    req.icap.text_label = NULL;//&text_osd;
    req.icap.graph_label = NULL;//&graph_osd;

    err = server_request(server, VIDEO_REQ_IMAGE_CAPTURE, &req);
    if (err != 0) {
        puts("\n\n\ntake photo err\n\n\n");
        if (__this->cap_buf) {
            free(__this->cap_buf);
            __this->cap_buf = NULL;
        }
        return -EINVAL;
    }
     key_voice_start(1);
#ifndef VIDEO_REC_NO_MALLOC
    if (__this->cap_buf) {
        free(__this->cap_buf);
        __this->cap_buf = NULL;
    }
#endif
    // printf("3333333333333333333333333333333333333333333333333333333333333333333333333333");
#if (APP_CASE == __WIFI_CAR_CAMERA__)
    if(date_name_flag){
        sprintf(buf, "%s", req.icap.path);
    }else{
        sprintf(buf, "%s", req.icap.file_name);
    }
    printf("*********rec_photo_user:%s,%s\n",buf,req.icap.path);
    FILE_LIST_ADD(0, buf, 0);
#endif
    // printf("2222222222222222222222222222222222222222222222222222222222222222222222222222");
    return 0;
}
#if 0
int video_rec_osd_ctl(u8 onoff)
{
    int err;

    if (__this->state == VIDREC_STA_START) {
        return -EFAULT;
    }

    __this->rec_info->osd_on = onoff;
#ifdef CONFIG_VIDEO0_ENABLE
    err = video0_rec_osd_ctl(__this->rec_info->osd_on);
#endif // VREC0_EN

#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
    err = video1_rec_osd_ctl(__this->rec_info->osd_on);
#endif

    return err;
}

int video_rec_set_white_balance()
{
    union video_req req = {0};

    if (!__this->white_balance_set) {
        return 0;
    }

    req.camera.mode = ISP_MODE_IMAGE_CAPTURE;
    req.camera.white_blance = __this->rec_info->wb_val;
    req.camera.cmd = SET_CAMERA_WB ;

    if (__this->video_display[0]) {
        server_request(__this->video_display[0], VIDEO_REQ_CAMERA_EFFECT, &req);
    } else if (__this->video_rec0 && (__this->state == VIDREC_STA_START)) {
        server_request(__this->video_rec0, VIDEO_REQ_CAMERA_EFFECT, &req);
    } else {
        puts("\nvrec set wb fail\n");
        return 1;
    }

    __this->white_balance_set = 0;

    return 0;
}
#endif


int video_rec_set_exposure(u32 exp)
{
    union video_req req = {0};  // 初始化请求参数

    if (!__this->exposure_set) {  // 检查曝光设置标志
        return 0;  // 如果未设置，直接返回
    }

    req.camera.mode = ISP_MODE_IMAGE_CAPTURE;  // 设置相机模式为图像捕获
    req.camera.ev = exp;  // 设置曝光值
    req.camera.cmd = SET_CAMERA_EV;  // 设置命令为设置曝光

    if (__this->video_display[0]) {  // 检查第一个视频显示是否有效
        server_request(__this->video_display[0], VIDEO_REQ_CAMERA_EFFECT, &req);  // 发送曝光设置请求
    } else if (__this->video_rec0 && (__this->state == VIDREC_STA_START)) {  // 检查视频录制状态
        server_request(__this->video_rec0, VIDEO_REQ_CAMERA_EFFECT, &req);  // 发送曝光设置请求
    } else {
        return 1;  // 如果都不满足，返回1表示失败
    }

    __this->exposure_set = 0;  // 重置曝光设置标志

    return 0;  // 返回成功
}


/*
 *场景切换使能函数，如果显示打开就用显示句柄控制，否则再尝试用录像句柄控制
 */
static int video_rec_start_isp_scenes()
{
#ifdef CONFIG_VIDEO4_ENABLE
    return 0;
#endif
    if (__this->isp_scenes_status) {
        return 0;
    }

    stop_update_isp_scenes();

    if (__this->video_display[0]) {
        __this->isp_scenes_status = 1;
        return start_update_isp_scenes(__this->video_display[0]);
    } else if (__this->video_rec0 && ((__this->state == VIDREC_STA_START) ||
                                      (__this->state == VIDREC_STA_STARTING))) {
        __this->isp_scenes_status = 2;
        return start_update_isp_scenes(__this->video_rec0);
    }

    __this->isp_scenes_status = 0;

    return 1;
}

static int video_rec_stop_isp_scenes(u8 status, u8 restart)
{

    if (__this->isp_scenes_status == 0) {
        return 0;
    }

    if ((status != __this->isp_scenes_status) && (status != 3)) {
        return 0;
    }

    __this->isp_scenes_status = 0;
    stop_update_isp_scenes();

    if (restart) {
        video_rec_start_isp_scenes();
    }

    return 0;
}


static u8 page_main_flag = 0;
static u8 page_park_flag = 0;
void set_page_main_flag(u8 flag)
{
    page_main_flag = flag;
}
static int show_main_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (page_main_flag) {
        return 0;
    }
    if (!__this->ui) {
        return -1;
    }

    puts("show_main_ui\n");
    req.show.id = ID_WINDOW_VIDEO_REC;
    server_request_async(__this->ui, UI_REQ_SHOW, &req);
    page_main_flag = 1;
#endif

    return 0;
}


static int show_park_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (page_park_flag) {
        return 0;
    }
    if (!__this->ui) {
        return -1;
    }

    puts("show_park_ui\n");
    req.show.id = ID_WINDOW_PARKING;
    server_request_async(__this->ui, UI_REQ_SHOW, &req);
    page_park_flag = 1;
#endif

    return 0;
}

static int show_lane_set_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (!__this->ui) {
        return -1;
    }

    req.show.id = ID_WINDOW_LANE;
    server_request_async(__this->ui, UI_REQ_SHOW, &req);
#endif

    return 0;
}

static void hide_main_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (page_main_flag == 0) {
        return;
    }
    if (!__this->ui) {
        puts("__this->ui == NULL!!!!\n");
        return;
    }

    puts("hide_main_ui\n");

    req.hide.id = ID_WINDOW_VIDEO_REC;
    server_request(__this->ui, UI_REQ_HIDE, &req);
    page_main_flag = 0;
#endif
}

static void hide_home_main_ui()
{
#ifdef CONFIG_UI_STYLE_JL02_ENABLE
    union uireq req;

    if (!__this->ui) {
        puts("__this->ui == NULL!!!!\n");
        return;
    }

    puts("hide_home_main_ui\n");

    req.hide.id = ID_WINDOW_MAIN_PAGE;
    server_request(__this->ui, UI_REQ_HIDE, &req);
#endif
}

static void hide_park_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (page_park_flag == 0) {
        video_rec_get_remain_time();
        return;
    }
    if (!__this->ui) {
        puts("__this->ui == NULL!!!!\n");
        return;
    }

    puts("hide_park_ui\n");

    req.hide.id = ID_WINDOW_PARKING;
    server_request(__this->ui, UI_REQ_HIDE, &req);
    page_park_flag = 0;
#endif
}

static int video_rec_storage_device_ready(void *p)
{
    __this->sd_wait = 0;

    video_rec_scan_lock_file();

    /* #if (APP_CASE == __WIFI_CAR_CAMERA__) */
    /* FILE_LIST_SCAN(); */
    /* #endif */
    if ((int)p == 1) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
        video_rec_start_notify();//先停止网络实时流再录像,录像完毕再通知APP
#else
        /* video_rec_start(); */
#endif

    }

    return 0;
}


static int video_rec_sd_in()
{
    /*
     * 如果APP应用场景是无线车载摄像头，则发送SD卡插入事件通知
     */
#if (APP_CASE == __WIFI_CAR_CAMERA__)
    video_rec_sd_event_ctp_notify(1);
#endif

    /*
     * 如果当前录像状态是空闲状态（VIDREC_STA_IDLE），则：
     * 1. 获取剩余录像时间
     * 2. 停止运动检测
     * 3. 停止车道检测
     */
    if (__this->state == VIDREC_STA_IDLE) {
        video_rec_get_remain_time();  // 获取剩余录像时间
        ve_mdet_stop();               // 停止运动检测
        ve_lane_det_stop(0);          // 停止车道检测
    }

    /*
     * 如果菜单没有弹出（menu_inout == 0），则启动以下检测功能：
     * 1. 启动运动检测
     * 2. 启动车道检测
     * 3. 启动人脸检测
     */
    if (__this->menu_inout == 0) {
        ve_mdet_start();              // 启动运动检测
        ve_lane_det_start(0);         // 启动车道检测
        ve_face_det_start(0);         // 启动人脸检测
    }

    /*
     * 如果APP应用场景是无线车载摄像头，则通知网络录像状态
     */
#if (APP_CASE == __WIFI_CAR_CAMERA__)
    net_video_rec_status_notify();
#endif

    /*
     * 重置锁定文件大小计数器
     */
    __this->lock_fsize_count = 0;

    return 0;  // 返回0，表示成功
}


static int video_rec_sd_out()
{
    ve_mdet_stop();
    ve_lane_det_stop(0);

    video_rec_fscan_release(0);
    video_rec_fscan_release(1);

    if (__this->sd_wait == 0) {
        __this->sd_wait = wait_completion(storage_device_ready,
                                          video_rec_storage_device_ready, (void *)1);
    }

#if (APP_CASE == __WIFI_CAR_CAMERA__)
    video_rec_sd_event_ctp_notify(0);
#endif

    return 0;
}

static void video_rec_park_call_back(void *priv)
{
    if (__this->state == VIDREC_STA_START) {
        video_rec_stop(0);
        if (usb_is_charging() && (__this->state == VIDREC_STA_STOP)) {
            // video_rec_start();
        } else {
            puts("park rec off power close\n");
            sys_power_shutdown();
        }
    }
}

static int video_rec_park_wait(void *priv)
{
    int err = 0;

    puts("video_rec_park_wait\n");

    if (__this->state != VIDREC_STA_START) {
        puts("park_rec_start\n");
        // err = video_rec_start();
    }

    if (err == 0) {
        sys_timeout_add(NULL, video_rec_park_call_back, 30 * 1000);
        if (__this->park_wait_timeout) {
            sys_timeout_del(__this->park_wait_timeout);
            __this->park_wait_timeout = 0;
        }
    }

    return 0;
}

static void video_rec_park_wait_timeout(void *priv)
{
    if (__this->state == VIDREC_STA_START) {
        return;
    }
    puts("park wait timeout power close\n");
    sys_power_shutdown();
}

int lane_det_setting_disp()
{
    u32 err = 0;
#ifdef CONFIG_VIDEO0_ENABLE
    struct video_window win = {0};

    video_disp_stop(1);

    u16 dis_w = 640 * SCREEN_H / 352 / 16 * 16;
    dis_w = dis_w > SCREEN_W ? SCREEN_W : dis_w;

    printf("lane dis %d x %d\n", dis_w, SCREEN_H);

    win.top             = 0;
    win.left            = (SCREEN_W - dis_w) / 2 / 16 * 16;
    win.width           = dis_w;
    win.height          = SCREEN_H;
    win.border_left     = 0;
    win.border_right    = 0;
    win.border_top      = 0;
    win.border_bottom   = 0;
    err = video_disp_start(0, &win);
    show_lane_set_ui();
#endif
    return err;
}

static int video_rec_usb_device_ready(void *p)
{
    __this->usb_wait = 0;

    extern int usb_connect(u8 state);
    usb_connect(USB_CAMERA);

    return 0;
}

extern int get_video1_state();

extern int get_video2_state();
static int video_rec_init()
{
    int err = 0;

    void sd1_out_timeout(u8 timeout);
    sd1_out_timeout(100);  // 设置SD卡超时时间为100

    ve_server_open(0);  // 打开视频服务器
    db_update("cyc", 1);  // 更新数据库记录，设置循环记录

#if (CONFIG_VIDEO_PARK_DECT == 1)
    __this->disp_park_sel = 1;  // 配置停车检测类型为1
#elif (CONFIG_VIDEO_PARK_DECT == 3)
    __this->disp_park_sel = 2;  // 配置停车检测类型为3
#elif (CONFIG_VIDEO_PARK_DECT == 4)
    __this->disp_park_sel = 3;  // 配置停车检测类型为4
#else
    __this->disp_park_sel = 0;  // 默认停车检测类型为0
#endif

#ifdef VIDEO_REC_NO_MALLOC
#ifdef CONFIG_VIDEO4_ENABLE
    int buf_size[] = {VREC4_FBUF_SIZE, VREC4_FBUF_SIZE, VREC4_FBUF_SIZE, VREC4_FBUF_SIZE};  // 配置四路视频缓存大小
#else
#ifndef CONFIG_SINGLE_VIDEO_REC_ENABLE
    int buf_size[] = {VREC0_FBUF_SIZE, VREC1_FBUF_SIZE, VREC2_FBUF_SIZE, VREC3_FBUF_SIZE};  // 配置多路视频缓存大小
#else
    int buf_size[] = {VREC0_FBUF_SIZE};  // 配置单路视频缓存大小
#endif
#endif
    for (int i = 0; i < ARRAY_SIZE(buf_size); i++) {  // 为每个视频通道分配内存
        if (buf_size[i]) {
            if (!__this->video_buf[i]) {
                __this->video_buf[i] = malloc(buf_size[i]);  // 分配视频缓存内存
                if (__this->video_buf[i] == NULL) {
                    log_i("err maloo\n");  // 分配失败，打印错误信息
                    while (1);
                }
            }
        } else {
            __this->video_buf[i] = NULL;  // 如果缓存大小为0，设置缓存指针为空
        }
    }
    if (!__this->cap_buf) {
        __this->cap_buf = (u8 *)malloc(CAMERA_CAP_BUF_SIZE);  // 分配摄像头捕捉缓存
    }
#endif
    __this->video_online[1]=get_video1_state();
    __this->video_online[2]=get_video2_state();
    printf(">>>>>>>>>>>>__this->video_online[1]=%d  __this->video_online[2]=%d\n",__this->video_online[1],__this->video_online[2]);
#ifdef CONFIG_VIDEO0_ENABLE
    __this->video_online[0] = 1;
    err = video_disp_start(0, &disp_window[DISP_MAIN_WIN][0]);
#endif

#ifdef CONFIG_VIDEO1_ENABLE
//    __this->video_online[1] = dev_online("video1.*");
    if (__this->video_online[1]) {
        if(camera_config==2){
            video_disp_start(1, &disp_window[DISP_FRONT_WIN][1]);
        }else if(camera_config==3||camera_config==4){
            video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);
        }
    }else{
        video_disp_start(1, &disp_window[DISP_MAIN_WIN][3]);
    }
#endif

#ifdef CONFIG_VIDEO2_ENABLE
     if(__this->video_online[2]){
        video_disp_stop(1);
        if(camera_config==2){
            video_disp_start(2, &disp_window[DISP_FRONT_WIN][1]);
        }else if(camera_config==3){
            video_disp_start(2, &disp_window[DISP_MAIN_WIN][2]);
        }else if(camera_config==4){
            video_disp_start(2, &disp_window[DISP_MAIN_WIN][1]);
        }
     }
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    __this->video_online[3] = dev_online("uvc");  // 检查通道3设备是否在线
    /* #if 1 */
    /*     if (__this->video_online[3]) { */
    /*         err = video_disp_start(3, &disp_window[DISP_MAIN_WIN][1]); */  // 启动视频显示，通道3，主窗口
    /*     } */
    /* #endif */
#endif

#ifdef CONFIG_VIDEO4_ENABLE
    __this->video_online[0] = 1;
    __this->video_online[1] = 1;
    __this->video_online[2] = 1;
    __this->video_online[3] = 1;
    err = video_disp_start(0, &disp_window[DISP_MAIN_WIN][0]);  // 启动视频显示，通道0，主窗口
    /* err = video_disp_start(0, &disp_window[DISP_HALF_WIN][0]); */  // 启动半屏显示
    /* err = video_disp_start(1, &disp_window[DISP_HALF_WIN][1]); */  // 启动半屏显示
#endif

    __this->disp_state = DISP_MAIN_WIN;  // 设置显示状态为主窗口
    __this->second_disp_dev = 0;  // 没有第二个显示设备

#ifdef CONFIG_PARK_ENABLE
    if (get_parking_status()) {
        show_park_ui();  // 如果检测到停车状态，显示停车UI
    } else {
        show_main_ui();  // 否则显示主界面UI
    }
#else
    show_main_ui();  // 如果没有启用停车功能，直接显示主界面UI
#endif

    video_rec_get_remain_time();  // 获取视频录制剩余时间

#ifdef CONFIG_GSENSOR_ENABLE
    if (!strcmp(sys_power_get_wakeup_reason(), "wkup_port:wkup_gsen")) {
        if (db_select("par")) {
            __this->gsen_lock = 0xff;  // 设置G-sensor锁定状态
            __this->park_wait_timeout = sys_timeout_add(NULL, video_rec_park_wait_timeout, 10 * 1000);  // 设置停车等待超时时间
            __this->park_wait = wait_completion(storage_device_ready,
                                                video_rec_park_wait, NULL);  // 等待存储设备准备好
        }
        sys_power_clr_wakeup_reason("wkup_port:wkup_gsen");  // 清除G-sensor唤醒原因
    } else {
        __this->sd_wait = wait_completion(storage_device_ready,
                                          video_rec_storage_device_ready, 0);  // 等待存储设备准备好
    }
#else
    __this->sd_wait = wait_completion(storage_device_ready,
                                      video_rec_storage_device_ready, (void *)1);  // 等待存储设备准备好，带参数1
#endif

    if (get_parking_status()) {
        video_disp_win_switch(DISP_WIN_SW_SHOW_PARKING, 0);  // 显示停车窗口
    } else {
        video_disp_win_switch(DISP_WIN_SW_SHOW_SMALL, 0);  // 显示小窗口
    }

#ifdef CONFIG_USB_VIDEO_OUT
    extern int usb_device_ready();
    __this->usb_wait = wait_completion(usb_device_ready,
                                       video_rec_usb_device_ready, 0);  // 等待USB设备准备好
#endif

    return err;  // 返回错误码
}



static int video_rec_uninit()
{
    int err;
    union video_req req = {0};

    if (__this->state == VIDREC_STA_START) {
        return -EFAULT;
    }
    if (__this->park_wait) {
        wait_completion_del(__this->park_wait);
        __this->park_wait = 0;
    }
    if (__this->sd_wait) {
        wait_completion_del(__this->sd_wait);
        __this->sd_wait = 0;
    }
    if (__this->char_wait) {
        wait_completion_del(__this->char_wait);
        __this->char_wait = 0;
    }
    if (__this->park_wait_timeout) {
        sys_timeout_del(__this->park_wait_timeout);
        __this->park_wait_timeout = 0;
    }
    if (__this->usb_wait) {
        wait_completion_del(__this->usb_wait);
        __this->usb_wait = 0;
    }

    video_rec_stop_isp_scenes(3, 0);

    ve_server_close();

    if (__this->state == VIDREC_STA_START) {
        err = video_rec_stop(1);
    }
    video_rec_close();


    video_rec_fscan_release(0);
    video_rec_fscan_release(1);

    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        video_disp_stop(i);
    }

#ifdef VIDEO_REC_NO_MALLOC
    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        if (__this->video_buf[i]) {
            free(__this->video_buf[i]);
            __this->video_buf[i] = NULL;
        }
    }
    if (__this->cap_buf) {
        free(__this->cap_buf);
        __this->cap_buf = NULL;
    }
#endif

    __this->disp_state = DISP_FORBIDDEN;
    __this->state = VIDREC_STA_FORBIDDEN;
    __this->lan_det_setting = 0;

    void sd1_out_timeout(u8 timeout);
    sd1_out_timeout(0);

#ifdef CONFIG_USB_VIDEO_OUT
    extern int usb_disconnect();
    usb_disconnect();
#endif

    return 0;

}


static int video_rec_mode_sw()
{
    if (__this->state != VIDREC_STA_FORBIDDEN) {
        return -EFAULT;
    }

#ifndef VIDEO_REC_NO_MALLOC
    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        if (__this->video_buf[i]) {
            free(__this->video_buf[i]);
            __this->video_buf[i] = NULL;
        }
    }
#endif



    for (int i = 0; i < CONFIG_VIDEO_REC_NUM; i++) {
        if (__this->audio_buf[i]) {
            free(__this->audio_buf[i]);
            __this->audio_buf[i] = NULL;
        }
    }

    if (__this->cap_buf) {
        free(__this->cap_buf);
        __this->cap_buf = NULL;
    }

    return 0;
}


/*
 *菜单相关的函数
 */
static int video_rec_change_status(struct intent *it)
{
    if (!strcmp(it->data, "opMENU:")) { /* ui要求打开rec菜单 */
        puts("ui ask me to opMENU:.\n");

        if ((__this->state != VIDREC_STA_START) && (__this->state != VIDREC_STA_FORBIDDEN)) { /* 允许ui打开菜单 */
            __this->menu_inout = 1;
            if (db_select("mot")) {
                ve_mdet_stop();
            }
            if (db_select("lan")) {
                ve_lane_det_stop(0);
            }

            it->data = "opMENU:en";
        } else { /* 禁止ui打开菜单 */
            it->data = "opMENU:dis";
        }

    } else if (!strcmp(it->data, "exitMENU")) { /* ui已经关闭rec菜单 */
        puts("ui tell me exitMENU.\n");
        __this->menu_inout = 0;

        video_rec_get_remain_time();
        video_rec_fun_restore();
        if (db_select("mot")) {
            ve_mdet_start();
        }
        if (db_select("lan")) {
            ve_lane_det_start(0);
        }
        if (db_select("fac")) {
            ve_face_det_start(0);
        }
    } else if (!strcmp(it->data, "sdCard:")) {
        video_rec_get_remain_time();
        if (storage_device_ready() == 0) {
            it->data = "offline";
        } else {
            it->data = "online";
        }
    } else {
        puts("unknow status ask by ui.\n");
    }

    return 0;
}

static int video_disp_move(u16 width, u16 height, u16 xoff, u16 yoff,
                           u16 b_left, u16 b_top, u16 b_right, u16 b_bottom)
{
    union video_req req = {0};
    req.display.fb 		= "fb1";

    req.display.left  	= xoff;
    req.display.top 	= yoff;

    req.display.width 	= width;
    req.display.height 	= height;

    req.display.border_left   = b_left;
    req.display.border_top    = b_top;
    req.display.border_right  = b_right;
    req.display.border_bottom = b_bottom;

    req.display.uvc_id = 0;
    req.display.camera_config = NULL;
    req.display.camera_type     = VIDEO_CAMERA_NORMAL;

    req.display.state 	= VIDEO_STATE_CFG;
    req.display.pctl = NULL;

    server_request(__this->video_display[0], VIDEO_REQ_DISPLAY, &req);


    return 0;
}

static u8 reverse = 1;
/* static u8 g_fps = 30; */
/*
 *录像的状态机,进入录像app后就是跑这里
 */
static int video_rec_state_machine(struct application *app, enum app_state state, struct intent *it)
{
    int err = 0;
    int len;

    switch (state) {
    case APP_STA_CREATE:
        puts("--------app_rec: APP_STA_CREATE\n");

        // 初始化视频录制处理句柄
        memset(__this, 0, sizeof(struct video_rec_hdl));
        // 分配视频录制缓冲区
        //video_rec_buf_alloc();
        // 加载视频服务器
        server_load(video_server);
#ifdef CONFIG_UI_ENABLE
        // 打开UI服务器
        __this->ui = server_open("ui_server", NULL);
        if (!__this->ui) {
            return -EINVAL;  // 如果UI服务器打开失败，返回错误码
        }
#endif
        // 设置视频显示窗口
        video_set_disp_window();
        // 初始化视频录制配置
        video_rec_config_init();
        // 设置视频录制状态为空闲
        __this->state = VIDREC_STA_IDLE;

        break;
    case APP_STA_START:
        puts("--------app_rec: APP_STA_START\n");
        if (!it) {
            break;  // 如果意图为空，则直接跳出
        }
        switch (it->action) {
        case ACTION_VIDEO_REC_MAIN:
            puts("ACTION_VIDEO_REC_MAIN\n");
            if (it->data && !strcmp(it->data, "lan_setting")) {
                __this->lan_det_setting = 1;  // 设置车道检测配置标志
                ve_server_open(1);  // 打开视频服务器并开启车道检测
                lane_det_setting_disp();  // 显示车道检测设置界面
                // printf("ACTION_VIDEO_REC_MAIN:lan_settingGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
            } else {
                //  camera_display_init();
                video_rec_init();  // 初始化视频录制功能/////////////////
                //  camera_display_init();
                //  video_disp_start(0, &disp_window[DISP_FRONT_WIN][1]);
                app_mode_flag=1;
            }
            break;
        case ACTION_VIDEO_REC_SET_CONFIG:
            if (!__this->ui) {
                return -EINVAL;  // 如果UI服务器未打开，返回错误码
            }
            video_rec_set_config(it);  // 设置视频录制配置
            db_flush();  // 刷新数据库
            if (it->data && !strcmp(it->data, "res")) {
                video_rec_get_remain_time();  // 获取剩余录制时间
            }
            if (it->data && !strcmp(it->data, "gap")) {
                __this->need_restart_rec = 1;  // 需要重新启动录制
                video_rec_get_remain_time();  // 获取剩余录制时间
            }
            if (it->data && !strcmp(it->data, "mic")) {
                video_rec_aud_mute();  // 静音音频录制
            }
            if (it->data && !strcmp(it->data, "cyc")) {
                video_rec_len();  // 获取视频录制长度
            }
            break;
        case ACTION_VIDEO_REC_CHANGE_STATUS:
            video_rec_change_status(it);  // 改变视频录制状态
            break;
        // case ACTION_VIDEO_REC_CONTROL:
        //     printf("ACTION_VIDEO_REC_CONTROL\n");
       //  switch_camera_disp();//此处修改用于验证切换功能，可不同步
        //     break;

        case ACTION_REC_TAKE_PHOTO:  // 处理拍照动作
            if(app_mode_flag != 1) {  // 检查应用模式标志是否为1
                printf("===========ACTION_REC_TAKE_PHOTO is not video_rec app\n\n");  // 如果不是视频录制应用，输出提示信息
                break;  // 结束此case
            }
            if(db_select("phm") == 0) {  // 查询数据库，选择"phm"
                printf("__this->video_online[1] === %d , __this->disp_state === %d \n", __this->video_online[1], __this->disp_state);  // 输出视频在线状态和显示状态
                if(__this->video_online[1]) {  // 检查视频是否在线
                    if(__this->disp_state == DISP_FRONT_WIN) {  // 如果显示状态为前窗
                        video_take_photo(0);  // 进行拍照，参数为0
                    } else {
                        video_take_photo(1);  // 否则，进行拍照，参数为1
                        printf("video_take_photo(1)\n");
                    }
                } else {
                    video_take_photo(0);  // 如果视频不在线，默认拍照，参数为0
                }
            } else {
                if(__this->video_online[1]) {  // 如果查询数据库失败，但视频仍在线
                    // 延时拍照
                    if(__this->disp_state == DISP_FRONT_WIN) {  // 如果显示状态为前窗
                        video_rec_delay_photo(0);  // 进行延时拍照，参数为0
                    } else {
                        video_rec_delay_photo(1);  // 否则，进行延时拍照，参数为1
                    }
                } else {
                    video_rec_delay_photo(0);  // 如果视频不在线，默认延时拍照，参数为0
                }
            }
            ui_hide(ENC_PO_PT);  // 隐藏界面元素ENC_PO_PT
        break;
        case ACTION_REC_CAP_TAKE_PHOTO:
            video_rec_take_photo();
            ui_hide(ENC_PO_PT);
        break;
#ifdef CONFIG_UI_STYLE_JL02_ENABLE
        case ACTION_VIDEO_REC_CONTROL:
            // 如果当前状态为视频录制开始状态 (VIDREC_STA_START)，则停止视频录制，
            // 并重置运动检测和车道偏离检测功能。
            if (__this->state == VIDREC_STA_START) {
                video_rec_stop(0);
                ve_mdet_reset();
                ve_lane_det_reset();
            } else {
                // 否则，启动视频录制。
                video_rec_start();
            }
            break;
        case ACTION_VIDEO_REC_LOCK_FILE:
            // // 如果数据 (it->data) 存在且为 "get_lock_statu"，则获取当前的重力感应锁定状态，
            // // 并将其存储到 it->exdata 中。
            // if (it->data && !strcmp(it->data, "get_lock_statu")) {
            //     it->exdata = !!__this->gsen_lock;
            //     break;
            // }

            // // 如果当前状态为视频录制开始状态 (VIDREC_STA_START)，根据 it->exdata 的值
            // // 设置重力感应锁定状态 (gsen_lock)。0xff 表示锁定，0 表示不锁定。
            // if (__this->state == VIDREC_STA_START) {
            //     __this->gsen_lock = it->exdata ? 0xff : 0;
            // }
            // break;
        case ACTION_VIDEO_REC_SWITCH_WIN:
            // 切换到下一个显示窗口 (DISP_WIN_SW_SHOW_NEXT)。
            // video_disp_win_switch(DISP_WIN_SW_SHOW_NEXT, 0);
            printf("switch winNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN\n");


            /* if (--g_fps <= 2) { */
            /* g_fps = 30; */
            /* } */
            /* video0_rec_set_dr(g_fps); */

            /* reverse = !reverse; */
            /* if (reverse) { */
            /* video_set_crop(100, 100, 1720, 880); */
            /* } else { */
            /* video_set_crop(0, 0, 1920, 1080); */
            /* } */

            /* video_rec_take_photo(); */

            /* video_disp_win_switch(DISP_WIN_SW_MIRROR, 0); */

            /* { */
            /* static u8 c = 0; */
            /* c++; */
            /* c = c < 4 ? c : 0; */
            /* int switch_usb_camera(int id); */
            /* switch_usb_camera(c); */
            /* } */

            /* { */
            /* video_disp_pause(0); */
            /* video_rec_take_photo(); */
            /* video_disp_start(0, &disp_window[DISP_MAIN_WIN][0]); */
            /* } */
            break;
#endif
        }
        break;
    case APP_STA_PAUSE:
        puts("--------app_rec: APP_STA_PAUSE\n");
        video_rec_fscan_release(0);
        video_rec_fscan_release(1);
        break;
    case APP_STA_RESUME:
        puts("--------app_rec: APP_STA_RESUME\n");
        break;
    case APP_STA_STOP:
        puts("--------app_rec: APP_STA_STOP\n");

#if (APP_CASE == __WIFI_CAR_CAMERA__)
        video_rec_all_stop_notify();
#endif
        if (__this->state == VIDREC_STA_START) {
            video_rec_stop(0);
            ve_mdet_stop();
            ve_lane_det_stop(0);
        }

        if (video_rec_uninit()) {
            err = 1;
            break;
        }

#ifdef CONFIG_UI_ENABLE
        puts("--------rec hide ui\n");
        hide_main_ui();
#endif
        break;
    case APP_STA_DESTROY:
        puts("--------app_rec: APP_STA_DESTROY\n");

#if (APP_CASE == __WIFI_CAR_CAMERA__)
        printf("\n video_rec_all_stop_notify \n");
        video_rec_all_stop_notify();
#endif
        if (video_rec_mode_sw()) {
            err = 2;
            break;
        }
#ifdef CONFIG_UI_ENABLE
        puts("--------rec close ui\n");
        if (__this->ui) {
            server_close(__this->ui);
            __this->ui = NULL;
        }
#endif
        f_free_cache(CONFIG_ROOT_PATH);
        malloc_stats();
        log_d("<<<<<<< video_rec: destroy\n");
        break;
    }

    return err;
}


/*
 *录像app的按键响应函数
 */
static int video_rec_key_event_handler(struct key_event *key)
{
    int err;

    switch (key->event) {
    case KEY_EVENT_CLICK:
        switch (key->value) {
        case KEY_OK:
            printf("video_rec_key_ok: %d\n", __this->state);
            // if (__this->state == VIDREC_STA_START) {
            //     err = video_rec_stop(0);
            //     ve_mdet_reset();
            //     ve_lane_det_reset();
            //     __this->user_rec = 0;
            // } else {
            //     err = video_rec_start();
            //     __this->user_rec = 1;
            // }
            break;
        case KEY_MENU:
            break;
        case KEY_MODE:
            puts("rec key mode\n");
            if ((__this->state != VIDREC_STA_STOP) && (__this->state != VIDREC_STA_IDLE)) {
                if (__this->state == VIDREC_STA_START) {
                    if (!__this->gsen_lock) {
                        __this->gsen_lock = 0xff;
                        video_rec_post_msg("lockREC");
                    } else {
                        __this->gsen_lock = 0;
                        video_rec_post_msg("unlockREC");
                    }
                }

                return true;
            }
            break;
        case KEY_UP:
            video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
//            video_disp_start(2, &disp_window[DISP_MAIN_WIN][1]);
//            video_disp_win_switch(DISP_WIN_SW_SHOW_NEXT, 0);
            printf("key up \n");
            break;
        case KEY_DOWN:
            mic_set_toggle();
            video_rec_aud_mute();
            break;
        default:
            break;
        case KEY_1:
        if(app_mode_flag!=1){
                printf("===========ACTION_REC_TAKE_PHOTO is not video_rec appkey 1\n\n");
                break;
            }
            if(db_select("phm") == 0){
                printf("__this->video_online[1] === %d , __this->disp_state === %d \n",__this->video_online[1],__this->disp_state);
                if(__this->video_online[1]){
                    if( __this->disp_state == DISP_FRONT_WIN){
                        video_take_photo(0);
                    }else{
                        video_take_photo(1);
                    }
                }else{
                    video_take_photo(0);
                }
            }else{
                if(__this->video_online[1]){         //延时拍照
                    if( __this->disp_state == DISP_FRONT_WIN){
                        video_rec_delay_photo(0);
                    }else{
                        video_rec_delay_photo(1);
                    }
                }else{
                    video_rec_delay_photo(0);
                }
            }
        }
        break;
    default:
        break;
    }

    return false;
}



void test_timer(void *priv)
{

    msleep(20 * 1000);


#if 1
    while (1) {
//循环测试双路码流出帧

        int user_net_video3_recx_start(u8 uvc_id, u8 is_stream);
        user_net_video3_recx_start(0, 1);
        user_net_video3_recx_start(1, 1);
//        user_net_video3_recx_start(2,1);
//        user_net_video3_recx_start(3,1);
        msleep(10 * 1000);


#if CONFIG_SHARED_REC_HDL_ENABLE
        user_net_video3_recx_stop(0, 0);
#else
        user_net_video3_recx_stop(0, 1);
#endif
#if CONFIG_SHARED_REC_HDL_ENABLE
        user_net_video3_recx_stop(1, 0);
#else
        user_net_video3_recx_stop(1, 1);
#endif
//#if CONFIG_SHARED_REC_HDL_ENABLE
//       user_net_video3_recx_stop(2,0);
//#else
//       user_net_video3_recx_stop(2,1);
//#endif
//#if CONFIG_SHARED_REC_HDL_ENABLE
//       user_net_video3_recx_stop(3,0);
//#else
//       user_net_video3_recx_stop(3,1);
//#endif
        msleep(10 * 1000);

    }
#endif // 0



#if 0
    while (1) {
//循环测试本地写卡
        user_net_video3_recx_start(0, 0);
        user_net_video3_recx_start(1, 0);
        user_net_video3_recx_start(2, 0);
//        user_net_video3_recx_start(3,0);
        msleep(10 * 1000);
#if CONFIG_SHARED_REC_HDL_ENABLE
        user_net_video3_recx_stop(0, 0);
        user_net_video3_recx_stop(1, 0);
        user_net_video3_recx_stop(2, 0);
//        user_net_video3_recx_stop(3,0);
#else
        user_net_video3_recx_stop(0, 1);
        user_net_video3_recx_stop(1, 1);
        user_net_video3_recx_stop(2, 1);
//        user_net_video3_recx_stop(3,1);
#endif
        msleep(10 * 1000);
    }

#endif



#if 1
    static int count;
    while (1) {
//新开server拍照试验
        printf("\n\n >>>>>>>>>>>>>>>count = %d\n\n", count++);

        video3_norec_take_photo_save_file(0);
        video3_norec_take_photo_save_file(1);
        video3_norec_take_photo_save_file(2);
//        video3_norec_take_photo_save_file(3);
        msleep(10 * 1000);
    }

#endif



#if 0

    while (1) {
        //录像过程中抓拍试验
        //由于imc不足，最多只能同时启用2个测试
//        video3_recining_take_photo_save_file(0);
//        video3_recining_take_photo_save_file(1);
        video3_recining_take_photo_save_file(2);
        video3_recining_take_photo_save_file(3);
        msleep(5000);
    }

#endif


//获取uvcx对应接入的hub的port号
#if 0
    while (1) {
        u32 get_uvc_usb_port(u8 uvc_id);
        printf("\nuvc%d port = %d\n", 0, get_uvc_usb_port(0));
        printf("\nuvc%d port = %d\n", 1, get_uvc_usb_port(1));
        printf("\nuvc%d port = %d\n", 2, get_uvc_usb_port(2));
//        printf("\nuvc%d port = %d\n",3,get_uvc_usb_port(3));
        msleep(10 * 1000);
    }
#endif // 0
}
/**
 * @brief       video3 指定uvc 显示
 *
 * @param: sub_id: uvc id号
 * @param: win:显示窗口配置
 *
 * @return:
 **/
int video3_disp_start(int sub_id, const struct video_window *win)
{
    int err = 0;  // 初始化错误码为0
    union video_req req = {0};  // 初始化视频请求结构体，所有字段清零
    static char dev_name[20];  // 静态字符数组，用于存储设备名称
#ifdef CONFIG_DISPLAY_ENABLE

    u8 id = sub_id;  // 将传入的子ID赋值给局部变量id

    // 如果窗口宽度为-1，表示隐藏视频显示
    if (win->width == (u16) -1) {
        puts("video_disp_hide\n");  // 打印隐藏信息
        return 0;  // 返回0表示成功
    }

    // 根据子ID生成设备名称
    sprintf(dev_name, "video3.%d", sub_id * 5);
    if (sub_id >= 2) {  // 如果子ID大于等于2，使用不同的名称格式
        sprintf(dev_name, "video3.%d", sub_id + 6);
    }
    log_d("video_disp_start: %s, %d x %d\n", dev_name, win->width, win->height);  // 打印调试信息，显示设备名称和窗口尺寸

    // 检查视频显示服务器是否已打开，如果没有打开，则打开它
    if (!__this->video_display[id]) {
        __this->video_display[id] = server_open("video_server", (void *)dev_name);
        if (!__this->video_display[id]) {  // 如果打开失败，记录错误信息并返回错误码
            log_e("open video_server: faild, id = %d\n", id);
            return -EFAULT;  // 返回错误码，表示打开失败
        }
    }

    // 设置视频显示参数
    req.display.fb 		        = "fb1";  // 使用的帧缓冲设备
    req.display.left  	        = win->left;  // 显示窗口左上角X坐标
    req.display.top 	        = win->top;  // 显示窗口左上角Y坐标
    req.display.width 	        = win->width;  // 显示窗口宽度
    req.display.height 	        = win->height;  // 显示窗口高度
    req.display.border_left     = win->border_left;  // 左边界
    req.display.border_top      = win->border_top;  // 上边界
    req.display.border_right    = win->border_right;  // 右边界
    req.display.border_bottom   = win->border_bottom;  // 下边界
    req.display.mirror          = win->mirror;  // 镜像设置
    req.display.jaggy           = 0;  // 设置抗锯齿参数

    req.display.uvc_id = sub_id;  // 设置UVC ID
    req.display.camera_config = NULL;  // 相机配置
    req.display.camera_type = VIDEO_CAMERA_UVC;  // 相机类型为UVC
    req.display.src_w = __this->src_width[3];  // 源图像宽度
    req.display.src_h = __this->src_height[3];  // 源图像高度

    // 旋转参数配置
    // 0: 不旋转, 不镜像 (原图)
    // 1: 逆时针旋转90度, 不镜像
    // 2: 逆时针旋转270度, 不镜像
    // 3: 逆时针旋转90度后, 再垂直镜像
    // 4: 逆时针旋转90度后, 再水平镜像
    req.display.rotate = 0;  // USB视频图像旋转显示

    req.display.state 	        = VIDEO_STATE_START;  // 设置视频状态为开始
    req.display.pctl            = NULL;  // 控制器指针为空

    // 禁用系统按键和触摸事件
    sys_key_event_disable();
    sys_touch_event_disable();

    // 向视频显示服务器发送显示请求
    err = server_request(__this->video_display[id], VIDEO_REQ_DISPLAY, &req);
    if (err) {  // 如果请求失败，打印错误信息并关闭服务器
        printf("display req err = %d!!\n", err);
        server_close(__this->video_display[id]);
        __this->video_display[id] = NULL;
    }
#ifndef CONFIG_VIDEO4_ENABLE

    video_rec_start_isp_scenes();  // 启动ISP场景

#endif
#endif
    // 启用系统按键和触摸事件
    sys_key_event_enable();
    sys_touch_event_enable();

    return err;  // 返回错误码，0表示成功
}

void video3_disp_stop(int id)
{
#ifdef CONFIG_DISPLAY_ENABLE
    union video_req req = {0};  // 初始化视频请求结构体，所有字段清零

    if (__this->video_display[id]) {

        req.display.state 	= VIDEO_STATE_STOP;  // 设置视频状态为停止
        server_request(__this->video_display[id], VIDEO_REQ_DISPLAY, &req);  // 向服务器发送停止显示请求

        server_close(__this->video_display[id]);  // 关闭视频显示服务器
        __this->video_display[id] = NULL;  // 将服务器指针置空

    }
#endif
}

/*
 *录像app的设备响应函数
 */
static int video_rec_device_event_handler(struct sys_event *event)
{
    int err;  // 定义错误码变量
    struct intent it;  // 定义意图结构体

    // 判断事件参数是否以 "sd*" 开头，表示SD卡相关事件
    if (!ASCII_StrCmp(event->arg, "sd*", 4)) {
        switch (event->u.dev.event) {
            case DEVICE_EVENT_IN:  // 处理SD卡插入事件
                video_rec_sd_in();  // 调用SD卡插入处理函数

                break;
            case DEVICE_EVENT_OUT:  // 处理SD卡拔出事件
                if (!fdir_exist(CONFIG_STORAGE_PATH)) {  // 如果存储路径不存在，处理SD卡拔出事件
                    video_rec_sd_out();  // 调用SD卡拔出处理函数
                }

                // 定义并调用 `sd1_out_finish` 函数，用于完成SD卡拔出后的操作
                void sd1_out_finish(void);
                sd1_out_finish();
                break;
        }
    } else if (!ASCII_StrCmp(event->arg, "sys_power", 7)) {  // 判断事件参数是否为 "sys_power"，表示系统电源相关事件
        switch (event->u.dev.event) {
            case DEVICE_EVENT_POWER_CHARGER_IN:  // 处理充电器插入事件
                puts("\n\ncharger in\n\n");  // 打印充电器插入信息
                // 如果当前录像状态为空闲或停止状态，准备启动录像
                if ((__this->state == VIDREC_STA_IDLE) || (__this->state == VIDREC_STA_STOP)) {
                    if (__this->char_wait == 0) {  // 如果没有等待任务，开始等待存储设备准备好
                        // __this->char_wait = wait_completion(storage_device_ready,video_rec_start, (void *)0);
                        /* 可选代码: video_rec_storage_device_ready, (void*)1); */
                        /* 可选代码: video_rec_start(); */
                    }
                }
                break;
            case DEVICE_EVENT_POWER_CHARGER_OUT:  // 处理充电器拔出事件
                puts("charger out\n");  // 打印充电器拔出信息
                /* 注释代码: 如果当前录像状态为开始，停止录像 */
                /* if (__this->state == VIDREC_STA_START) {
                    video_rec_stop(0);
                } */
                break;
        }
    } else if (!ASCII_StrCmp(event->arg, "parking", 7)) {  // 判断事件参数是否为 "parking"，表示停车相关事件
        switch (event->u.dev.event) {
            case DEVICE_EVENT_IN:  // 处理停车事件
                puts("parking on\n");  // 打印停车事件信息
                hide_main_ui();  // 隐藏主界面，切换到停车界面
#ifdef CONFIG_UI_STYLE_JL02_ENABLE
            hide_home_main_ui();//录像在后台进入倒车隐藏主界面
#endif
            show_park_ui();  // 显示停车界面
            // video_disp_win_switch(DISP_WIN_SW_SHOW_PARKING, 0);  // 切换显示窗口到停车界面
            sys_power_auto_shutdown_pause();  // 暂停自动关机功能

            return true;

        case DEVICE_EVENT_OUT:  // 停车功能关闭事件
        hide_park_ui();  // 隐藏停车界面
        show_main_ui();  // 显示主界面
        puts("parking off\n");  // 输出停车关闭的消息
        // video_disp_win_switch(DISP_WIN_SW_HIDE_PARKING, 0);  // 切换显示窗口，隐藏停车界面
        video_rec_get_remain_time();  // 获取视频录制的剩余时间
        if (__this->state == VIDREC_STA_START) {
            sys_power_auto_shutdown_pause();  // 如果正在录制，暂停自动关机功能
        }
        return true;
}
    }
#if (defined CONFIG_VIDEO1_ENABLE && !defined CONFIG_SINGLE_VIDEO_REC_ENABLE)
// 检查是否启用了 video1 并且不处于单视频录制模式
    else if (!strncmp(event->arg, "video1", 6)) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
        net_video_rec_event_notify();// 如果是 WiFi 车载摄像头应用场景，通知网络视频录制事件
#endif
        switch (event->u.dev.event) {
    case DEVICE_EVENT_IN:
    case DEVICE_EVENT_ONLINE:
        puts("设备事件上线：video1\n");
        // 如果第2个视频设备还未上线
        if (!__this->video_online[1]) {
            // 标记第2个视频设备为已上线
            __this->video_online[1] = true;

            // 如果摄像头配置为2
            if(camera_config == 2){
                // 停止显示第1路视频
                video_disp_stop(1);

                // 如果第3个视频设备在线，则停止显示
                if(__this->video_online[2]){
                    video_disp_stop(2);
                }
                // 启动第1路视频显示，使用前置窗口配置
                video_disp_start(1, &disp_window[DISP_FRONT_WIN][1]);

            } else if (camera_config == 3) {
                // 停止第1路视频显示
                video_disp_stop(1);

                // 如果第3个视频设备在线
                if(__this->video_online[2]){
                    // 停止显示第2路视频
                    video_disp_stop(2);
                     // 启动第1路视频显示，使用主窗口配置
                    video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);

                    // 启动第2路视频显示，使用主窗口配置
                    video_disp_start(2, &disp_window[DISP_MAIN_WIN][2]);


                } else {
                    // 启动第1路视频显示，使用前置窗口配置
                    video_disp_start(1, &disp_window[DISP_FRONT_WIN][1]);
                }

            } else if (camera_config == 4) {
                // 停止第1路视频显示
                video_disp_stop(1);

                // 如果第4个视频设备在线
                if(__this->video_online[3]){
                    // 停止显示第3路视频
                    video_disp_stop(3);

                    // 启动第3路视频显示，使用主窗口配置
                    video_disp_start(3, &disp_window[DISP_MAIN_WIN][2]);

                    // 启动第1路视频显示，使用主窗口配置
                    video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);
                } else {
                    // 启动第1路视频显示，使用前置窗口配置
                    video_disp_start(1, &disp_window[DISP_FRONT_WIN][1]);
                }
            }

            // 获取视频录制的剩余时间
            video_rec_get_remain_time();

            // 如果当前视频录制状态为开始
            if (__this->state == VIDREC_STA_START) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                // 停止网络视频录制事件
                net_video_rec_event_stop();
#endif

                // 停止并重新开始视频录制
                // video_rec_stop(0);
                // video_rec_start();

#if (APP_CASE == __WIFI_CAR_CAMERA__)
                // 开始网络视频录制事件
                net_video_rec_event_start();
#endif
            }
        }
        break;

        case DEVICE_EVENT_OUT:
            puts("DEVICE_EVENT_OUT: video1\n");
            if (__this->video_online[1]) {
                __this->video_online[1] = false;
                video_disp_stop(1);


                video_rec_get_remain_time();

                if (__this->state == VIDREC_STA_START) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                    net_video_rec_event_stop();
#endif
                    // video_rec_stop(0);
                    // video_rec_start();
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                    net_video_rec_event_start();
#endif

                }
            }
            if(camera_config==3){
                if(__this->video_online[2]){
                    video_disp_stop(2);
                    video_disp_start(2,&disp_window[DISP_FRONT_WIN][1]);
                }
            }else if(camera_config==4){
                if(__this->video_online[3]){
                    video_disp_stop(3);
                    video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);
                }
            }
            break;
        }
    }
#endif

#ifdef CONFIG_VIDEO2_ENABLE
    // 检查是否启用了 video2 设备
    else if (!strncmp(event->arg, "video2", 6)) {
        // 根据 video2 设备的事件类型来处理
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:
        case DEVICE_EVENT_ONLINE:
            puts("设备事件上线：video2\n");
            // 如果 video2 当前不在线
            if (!__this->video_online[2]) {
                __this->video_online[2] = true;  // 标记 video2 为已上线

                // 根据 camera_config 的不同值执行不同操作
                if (camera_config == 2) {
                    video_disp_stop(1);  // 停止显示 video1
                    video_disp_start(2, &disp_window[DISP_FRONT_WIN][1]);  // 显示 video2

                } else if (camera_config == 3) {
                    video_disp_stop(1);  // 停止显示 video1
                    if (__this->video_online[1]) {
                        // 如果 video1 在线，则同时显示 video1 和 video2
                        video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);
                        video_disp_start(2, &disp_window[DISP_MAIN_WIN][2]);
                    } else {
                        video_disp_start(2, &disp_window[DISP_FRONT_WIN][1]);  // 仅显示 video2
                    }

                } else if (camera_config == 4) {
                    video_disp_stop(1);  // 停止显示 video1
                    if (__this->video_online[3]) {
                        // 如果 video3 在线，停止显示 video3，显示 video2 和 video3
                        video_disp_stop(3);
                        video_disp_start(3, &disp_window[DISP_MAIN_WIN][2]);
                        video_disp_start(2, &disp_window[DISP_MAIN_WIN][1]);
                    } else {
                        video_disp_start(2, &disp_window[DISP_FRONT_WIN][1]);  // 仅显示 video2
                    }
                }
            }
            break;

        case DEVICE_EVENT_OUT:
            puts("设备事件下线：video2\n");
            // 如果 video2 当前在线
            if (__this->video_online[2]) {
                __this->video_online[2] = false;  // 标记 video2 为下线
                video_disp_stop(2);  // 停止显示 video2
            }

            // 如果 camera_config 为 3 或 4，重新设置视频显示
            if (camera_config == 3) {
                video_disp_stop(1);
                video_disp_start(1, &disp_window[DISP_FRONT_WIN][1]);  // 显示 video1

            } else if (camera_config == 4) {
                video_disp_stop(3);
                video_disp_start(3, &disp_window[DISP_FRONT_WIN][1]);  // 显示 video3
            }
            break;
        }
    }
#endif // CONFIG_VIDEO2_ENABLE

#ifdef CONFIG_VIDEO3_ENABLE
    else if (!strncmp((char *)event->arg, "uvc", 3)) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
        net_video_rec_event_notify();
#endif

        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:
        case DEVICE_EVENT_ONLINE:
            puts("DEVICE_EVENT_ONLINE: uvc\n");
            printf("DEVICE_EVENT_ONLINE: uvc_id = %d\n", ((char *)event->arg)[3] - '0');
            printf("\n event->arg = %s __this->video_online[3] = %d \n", event->arg, __this->video_online[3]);
#ifndef CONFIG_UVCX_ENABLE
            if (!__this->video_online[3]) {
//                if (!usb_is_charging()) {
//                    break;
//                }

                __this->video_online[3] = true;
                __this->uvc_id = ((char *)event->arg)[3] - '0';
                if(camera_config==1){
                     video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);

                }else if(camera_config==4){
                    if(__this->video_online[1]){
                        video_disp_stop(1);
                        video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
                        video_disp_start(3,&disp_window[DISP_MAIN_WIN][2]);
                    }else if(__this->video_online[2]){
                        video_disp_stop(2);
                        video_disp_start(2,&disp_window[DISP_MAIN_WIN][1]);
                        video_disp_start(3,&disp_window[DISP_MAIN_WIN][2]);
                    }else{
                        video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);
                    }


                }
                printf("DEVICE_EVENT_ONLINE: uvc_id = %d\n", __this->uvc_id);
//                video_disp_win_switch(DISP_WIN_SW_DEV_IN, 3);
                video_rec_get_remain_time();

                if (__this->state == VIDREC_STA_START) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                    net_video_rec_event_stop();
#endif

                    // video_rec_stop(0);
                    // video_rec_start();
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                    net_video_rec_event_start();
#endif
                }




                /* extern  int UVC_DETECT_TIME; */
                /* printf("\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>UVC_DETECT_TIME = %d\n", timer_get_ms() - UVC_DETECT_TIME); */
            }
#else

            if (!strncmp((char *)event->arg, "uvc3", 4)) {
#if (CONFIG_DISP_DEVICE & CONFIG_UVC3_DEVICE)
                struct video_window win = {0};
                win.left   = LCD_DEV_WIDTH / 2;
                win.top    = 0;
                win.width  = LCD_DEV_WIDTH / 2;
                win.height = LCD_DEV_HIGHT;
                video3_disp_start(3, &win);
#endif
            }

            if (!strncmp((char *)event->arg, "uvc2", 4)) {
#if (CONFIG_DISP_DEVICE & CONFIG_UVC2_DEVICE)

                struct video_window win = {0};
                win.left   = LCD_DEV_WIDTH / 2;
                win.top    = 0;
                win.width  = LCD_DEV_WIDTH / 2;
                win.height = LCD_DEV_HIGHT;
                video3_disp_start(2, &win);
#endif
            }

            if (!strncmp((char *)event->arg, "uvc1", 4)) {
                printf("\n>>>>>>>>>>>>>>>>>>> %s %d\n", __func__, __LINE__);
                printf("DEVICE_EVENT_ONLINE: uvc_id = %d\n", ((char *)event->arg)[3] - '0');
                printf("\n>>>>>>>>>>>>>>>>>>> %s %d\n", __func__, __LINE__);
#if (CONFIG_DISP_DEVICE & CONFIG_UVC1_DEVICE)
                struct video_window win = {0};
                win.left   = LCD_DEV_WIDTH / 2;
                win.top    = 0;
                win.width  = LCD_DEV_WIDTH / 2;
                win.height = LCD_DEV_HIGHT;
                video3_disp_start(1, &win);
#endif

//                video_disp_win_switch(DISP_WIN_SW_DEV_IN, 3);



            }


            if (!__this->video_online[3]) {
                if (!usb_is_charging()) {
                    break;
                }
                __this->video_online[3] = true;
                __this->uvc_id = ((char *)event->arg)[3] - '0';

                printf("DEVICE_EVENT_ONLINE: uvc_id = %d\n", __this->uvc_id);

#if (CONFIG_DISP_DEVICE & CONFIG_UVC0_DEVICE)

                struct video_window win = {0};
                win.left   = 0;
                win.top    = 0;
                win.width  = LCD_DEV_WIDTH  / 2;
                win.height = LCD_DEV_HIGHT;
                video3_disp_start(0, &win);
#endif
                video_rec_get_remain_time();




                /* extern  int UVC_DETECT_TIME; */
                /* printf("\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>UVC_DETECT_TIME = %d\n", timer_get_ms() - UVC_DETECT_TIME); */
            }

#ifdef CONFIG_UVCX_TEST_ENABLE
            static int flag;
            if (!flag) {
                flag = 1;
                thread_fork("test_timer", 17, 0x2E00, 256, 0, test_timer, NULL);
            }

#endif

#endif // CONFIG_UVCX_ENABLE

            break;
        case DEVICE_EVENT_OUT:
            puts("DEVICE_EVENT_OUT: uvc\n");
            printf("DEVICE_EVENT_ONLINE: uvc_id = %d\n", ((char *)event->arg)[3] - '0');
#ifndef CONFIG_UVCX_ENABLE
            if (__this->video_online[3]) {
                __this->video_online[3] = false;
                video_disp_stop(3);
//                video_disp_win_switch(DISP_WIN_SW_DEV_OUT, 3);
                video_rec_get_remain_time();

                if (__this->state == VIDREC_STA_START) {
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                    net_video_rec_event_stop();
#endif
                    // video_rec_stop(0);
                    // video_rec_start();
#if (APP_CASE == __WIFI_CAR_CAMERA__)
                    net_video_rec_event_start();
#endif

                }


                /* post_msg_doorbell_task("doorbell_event_task", 2, DOORBELL_EVENT_CLOSE_RT_STREAM, 0); */

            }
            if(camera_config==4){
                if(__this->video_online[1]){
                    video_disp_stop(1);
                    video_disp_start(1,&disp_window[DISP_FRONT_WIN][1]);
                }else if(__this->video_online[2]){
                    video_disp_stop(2);
                    video_disp_start(2,&disp_window[DISP_FRONT_WIN][1]);
                }
            }
#else
            printf("\n 2event->arg = %s __this->video_online[3] = %d \n", event->arg, __this->video_online[3]);
            if (!strncmp((char *)event->arg, "uvc3", 4)) {

#if (CONFIG_DISP_DEVICE & CONFIG_UVC3_DEVICE)
                video3_disp_stop(3);
#endif
            }

            if (!strncmp((char *)event->arg, "uvc2", 4)) {

#if (CONFIG_DISP_DEVICE & CONFIG_UVC2_DEVICE)
                video3_disp_stop(2);
#endif
            }

            if (!strncmp((char *)event->arg, "uvc1", 4)) {
                printf("\n>>>>>>>>>>>>>>>>>>> %s %d\n", __func__, __LINE__);
                printf("DEVICE_EVENT_OUT: uvc_id = %d\n", ((char *)event->arg)[3] - '0');
                printf("\n>>>>>>>>>>>>>>>>>>> %s %d\n", __func__, __LINE__);
#if (CONFIG_DISP_DEVICE & CONFIG_UVC1_DEVICE)
                video3_disp_stop(1);
#endif

#if (CONFIG_NET_STREAM_DEVICE & CONFIG_UVC1_DEVICE)
                post_msg_doorbell_task("doorbell_event_task", 2, DOORBELL_EVENT_CLOSE_RT_STREAM, 1);
#endif

            }

            if (__this->video_online[3] && !strncmp((char *)event->arg, "uvc0", 4)) {
                __this->video_online[3] = false;
#if (CONFIG_DISP_DEVICE & CONFIG_UVC0_DEVICE)
                video3_disp_stop(0);
#endif


            }

#endif // CONFIG_UVCX_ENABLE

            break;
        }
    }
#endif



#ifdef CONFIG_GSENSOR_ENABLE
    else if (!strcmp(event->arg, "gsen_lock")) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_CHANGE:
            if (__this->state == VIDREC_STA_START) {
                if (db_select("gra")) {
                    __this->gsen_lock = 0xff;
                    video_rec_post_msg("lockREC");
                }
            }
            break;
        }
    }
#endif

    else if (!strncmp(event->arg, "lane_set_open", strlen("lane_set_open"))) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_CHANGE: {
            u32 aToint;
            ASCII_StrToInt(event->arg + strlen("lane_set_open"), &aToint, strlen(event->arg) - strlen("lane_set_open"));
            __this->car_head_y = aToint & 0x0000ffff;
            __this->vanish_y   = (aToint >> 16) & 0x0000ffff;
            ve_lane_det_start(1);
        }
        break;
        }
    } else if (!strncmp(event->arg, "lane_set_close", strlen("lane_set_close"))) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_CHANGE:
            ve_lane_det_stop(1);
            break;
        }
    } else if (!strcmp(event->arg, "camera0_err")) {
        // video_disp_win_switch(DISP_WIN_SW_DEV_OUT, 0);

        if (__this->state == VIDREC_STA_START) {
            video_rec_stop(0);
            ve_mdet_reset();
            ve_lane_det_reset();
            // video_rec_start();
        }
    } else if (!strncmp((char *)event->arg, "rec", 3)) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_OFFLINE:
            if (__this->state == VIDREC_STA_START) {
                log_e("video rec write error");
                video_rec_stop(0);
            }
            break;
        }
    }

    return false;
}

/*录像app的事件总入口*/
static int video_rec_event_handler(struct application *app, struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        return video_rec_key_event_handler(&event->u.key);
    case SYS_DEVICE_EVENT:
        return video_rec_device_event_handler(event);
    default:
        return false;
    }
}

static const struct application_operation video_rec_ops = {
    .state_machine  = video_rec_state_machine,
    .event_handler 	= video_rec_event_handler,
};

REGISTER_APPLICATION(app_video_rec) = {
    .name 	= "video_rec",
    .action	= ACTION_VIDEO_REC_MAIN,
    .ops 	= &video_rec_ops,
    .state  = APP_STA_DESTROY,
};




#if (APP_CASE == __WIFI_CAR_CAMERA__)
/******************************用于网络实时流*************************************/
static void ve_mdet_reset();
static int video_rec_sd_in();
static int video_rec_sd_out();
static int video_rec_device_event_handler(struct sys_event *event);
void ve_lane_det_reset();
extern char *video_rec_finish_get_name(FILE *fd, int index, u8 is_emf); //index：video0则0，video1则1，video2则2
extern int video_rec_finish_notify(char *path);
extern int video_rec_delect_notify(FILE *fd, int id);
extern int video_rec_err_notify(const char *method);
extern int video_rec_state_notify(void);
extern int video_rec_all_stop_notify(void);
extern int net_video_rec_event_notify(void);
extern int net_video_rec_event_stop(void);
extern int net_video_rec_event_start(void);

static int video_rec_get_abr(u32 width);
static void video_disp_stop(int id);
static int video_disp_start(int id, const struct video_window *win);
static int show_main_ui();
static void hide_main_ui();

void *get_video_rec_handler(void)
{
    return (void *)&rec_handler;
}
int video_rec_get_fps()
{
#ifdef LOCAL_VIDEO_REC_FPS
    return LOCAL_VIDEO_REC_FPS;
#else
    return 0;
#endif
}

int video_rec_get_audio_sampel_rate(void)
{
#ifdef  VIDEO_REC_AUDIO_SAMPLE_RATE
    return VIDEO_REC_AUDIO_SAMPLE_RATE;
#else
    return 8000;
#endif
}
int video_rec_control_start(void)
{
    int err; // 用于存储返回的错误码
    // err = video_rec_start(); // 启动视频录制
    return err; // 返回启动结果
}

int video_rec_control_doing(void)
{
    int err;
    if (__this->state == VIDREC_STA_START) {
        err = video_rec_stop(0);
        ve_mdet_reset();
        ve_lane_det_reset();
    } else {
        // err = video_rec_start();
    }
    return err;
}

int video_rec_device_event_action(struct sys_event *event)
{
    return video_rec_device_event_handler(event);
}

int video_rec_sd_in_notify(void)
{
    return video_rec_sd_in();
}

int video_rec_sd_out_notify(void)
{
    return video_rec_sd_out();
}

int video_rec_get_abr_from(u32 width)
{
    return video_rec_get_abr(width);
}

int net_video_disp_stop(int id)
{
    video_disp_stop(id);
    return 0;
}
int net_video_disp_start(int id)
{
    video_disp_start(id, &disp_window[0][0]);
    return 0;
}
int net_hide_main_ui(void)
{
    hide_main_ui();
    return 0;
}
int net_show_main_ui(void)
{
    show_main_ui();
    return 0;
}

/******************************************************************/
#endif





u8 is_video_rec_mode(void)
{
    struct intent it;
    struct application *app;
    app = get_current_app();
    if (app && !strcmp(app->name, "video_rec")) {
        return 1;
    }
    return 0;
}

u8 is_usb_mode(void)
{
    struct intent it;
    struct application *app;
    app = get_current_app();
    if (app && !strcmp(app->name, "usb_app")) {
        return 1;
    }
    return 0;
}



/* uvc主机发命令(PU)给从机demo示例 */
void video3_stream_cmd_test(void)
{
    enum {
        ID_USB_CAM0 = 0,
        ID_USB_CAM1 = 1,
    };
    enum {
        CMD_NONE = 0,
        CMD_FORCE_IFRAME,
        CMD_SET_FPS,
        CMD_SET_KBPS,
    };
    struct usb_video_cmd_t {
        u8 id;
        u8 cmd;
        u16 data;
    };

    int err = 0 ;
    struct usb_video_cmd_t usb_video_cmd;
    struct uvc_processing_unit uvc_pu;
    uvc_pu.index = 0x0200;
    uvc_pu.request = 0x01;
    uvc_pu.type = 0x21;
    uvc_pu.value = 0x0100;
    uvc_pu.len = 4;

    if (!dev_online("uvc")) {
        log_e("uvc offline!\n");
        return ;
    }
    void *fd = dev_open("uvc", (void *)0);
    if (!fd) {
        puts("fd uvc open faild!\n");
        return ;
    }

    //set fps
    usb_video_cmd.id = ID_USB_CAM1;
    usb_video_cmd.cmd = CMD_SET_FPS;
    u16 fps = 15;
    usb_video_cmd.data = fps;
    memcpy(uvc_pu.buf, &usb_video_cmd, sizeof(struct usb_video_cmd_t));

    err = dev_ioctl(fd, UVCIOC_REQ_PROCESSING_UNIT, (unsigned int)&uvc_pu);
    printf("err=%d\n", err);

    os_time_dly(20);

    //set kbps
    usb_video_cmd.id = ID_USB_CAM1;
    usb_video_cmd.cmd = CMD_SET_KBPS;
    u16 kbps = 2000;
    usb_video_cmd.data = kbps;
    memcpy(uvc_pu.buf, &usb_video_cmd, sizeof(struct usb_video_cmd_t));

    err = dev_ioctl(fd, UVCIOC_REQ_PROCESSING_UNIT, (unsigned int)&uvc_pu);
    printf("err=%d\n", err);

    dev_close(fd); //记得关闭uvc设备句柄
}


int get_video3_online_flag(void)
{
    return  __this->video_online[3];
}
void change_camera_config(int value)
{
    camera_config=value;
    camera_display_init();//更改配置后重新初始化窗口，可不调
}

void camera_display_init() {
    // 停止所有视频显示
    video_disp_stop(1);
    video_disp_stop(2);
    video_disp_stop(3);

    // 根据 camera_config 的值来选择如何显示摄像头的内容
    switch(camera_config) {
        case 1:
            // 如果 camera_config 为 1，且 video3 在线，启动 video3 的显示
            if (__this->video_online[3]) {
                video_disp_start(3, &disp_window[DISP_FRONT_WIN][1]);
            }
        break;

        case 2:
            // 如果 camera_config 为 2，优先显示 video2（若在线），否则显示 video1
            if (__this->video_online[2]) {
                video_disp_start(2, &disp_window[DISP_FRONT_WIN][1]);
            } else if (__this->video_online[1]) {
                video_disp_start(1, &disp_window[DISP_FRONT_WIN][1]);
            }
        break;

        case 3:
            // 如果 camera_config 为 3，若 video2 在线，则显示在主窗口2；同时若 video1 在线，则显示在主窗口1
            if (__this->video_online[1]) {
                video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);
            }
            if (__this->video_online[2]) {
                video_disp_start(2, &disp_window[DISP_MAIN_WIN][2]);
            }
        break;

        case 4:
            // 如果 camera_config 为 4，若 video2 在线，显示在主窗口1；若 video1 在线，也显示在主窗口1
            // 同时，如果 video3 在线，显示在主窗口2
            if (__this->video_online[2]) {
                video_disp_start(2, &disp_window[DISP_MAIN_WIN][1]);
            } else if (__this->video_online[1]) {
                video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);
            }
            if (__this->video_online[3]) {
                video_disp_start(3, &disp_window[DISP_MAIN_WIN][2]);
            }
        break;
    }

    return;
}

void video_display_mirror()//屏显镜像执行函数。注意，由于录像文件也要镜像所以，还需要暂停和重新开始录像的操作。开始录像需在mic参数改变之后
{
    if(db_select("mir")){
        db_update("mir",0);
    }else{
        db_update("mir",1);
    }
//    db_flush();//参数是否保存看功能设计

    video_disp_stop(1);
    video_disp_stop(2);
    video_disp_stop(3);
    switch(disp_state){
        case DISP_DOUBLE_HALF:
            if(camera_config==3){
                video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
                video_disp_start(2,&disp_window[DISP_MAIN_WIN][2]);
            }else if(camera_config==4){
                if(__this->video_online[1]){
                    video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
                }else{
                    video_disp_start(2,&disp_window[DISP_MAIN_WIN][1]);
                }
                video_disp_start(3,&disp_window[DISP_MAIN_WIN][2]);
            }
        break;
        case DISP_720AHD_FULL:
            video_disp_start(1,&disp_window[DISP_FRONT_WIN][1]);
        break;
        case DISP_UVC_FULL:
            video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);
        break;
        case DISP_1080AHD_FULL:
            video_disp_start(2,&disp_window[DISP_FRONT_WIN][1]);
        break;
    }

}
void switch_camera_disp()//镜头窗口切换函数
{

    if((camera_config==1)||(camera_config==2)){
        /**单路配置不做切换*/
        return ;
    }
    video_disp_stop(1);
    video_disp_stop(2);
    video_disp_stop(3);
    switch(disp_state){
        case DISP_DOUBLE_HALF:
            if(camera_config==3){
                if(__this->video_online[1]){
                    video_disp_start(1,&disp_window[DISP_FRONT_WIN][1]);
                }else if(__this->video_online[2]){
                    video_disp_start(2,&disp_window[DISP_FRONT_WIN][1]);
                }
            }else if(camera_config==4){
                if(__this->video_online[1]){
                    video_disp_start(1,&disp_window[DISP_FRONT_WIN][1]);
                }else if(__this->video_online[2]){
                    video_disp_start(2,&disp_window[DISP_FRONT_WIN][1]);
                }else if(__this->video_online[3]){
                    video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);
                }
            }
        break;
        case DISP_720AHD_FULL:
            if(camera_config==3){
                if(__this->video_online[2]){
                    video_disp_start(2,&disp_window[DISP_FRONT_WIN][1]);
                }else{
                    video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
                }
            }else if(camera_config==4){
                if(__this->video_online[3]){
                    video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);
                }else{
                     video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
                }
            }
        break;
        case DISP_UVC_FULL:
            if(__this->video_online[1]){
                video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
            }else if(__this->video_online[2]){
                video_disp_start(2,&disp_window[DISP_MAIN_WIN][1]);
            }
            video_disp_start(3,&disp_window[DISP_MAIN_WIN][2]);
        break;
        case DISP_1080AHD_FULL:
            if(camera_config==3){
                if(__this->video_online[1]){
                     video_disp_start(1,&disp_window[DISP_MAIN_WIN][1]);
                }
                video_disp_start(2,&disp_window[DISP_MAIN_WIN][2]);
            }else if(camera_config==4){
                if(__this->video_online[3]){
                    video_disp_start(3,&disp_window[DISP_FRONT_WIN][1]);
                }else{
                     video_disp_start(2,&disp_window[DISP_MAIN_WIN][1]);
                }
            }
        break;
    }
}

/**获取负片效果数据*/
/*
参数1：源YUV数据
参数2：处理后的数据
参数3：源YUV数据的宽
参数4：源YUV数据的高
*/
void convert_yuv_to_grayscale(unsigned char *input_yuv, unsigned char *output_gray, int width, int height)
{
    // int n=0; 用于存储输出灰度图像的当前索引
    int n = 0;
    int y_size = width * height; // Y分量的大小
    int uv_size = y_size / 4; // U和V分量的大小

    // 处理Y分量
    for (int i = 0; i < y_size; i++) {
        output_gray[n++] = 255 - input_yuv[i];  // Y分量取补码，生成灰度值
    }

    // 处理U分量
    unsigned char *ptemp = input_yuv + y_size; // U分量的起始地址
    for (int i = 0; i < uv_size; i++) {
        output_gray[n++] = 255 - ptemp[i];   // U分量取补码
    }

    // 处理V分量
    ptemp += uv_size; // 移动到V分量的起始地址
    for (int i = 0; i < uv_size; i++) {
        output_gray[n++] = 255 - ptemp[i];   // V分量取补码
    }
}
static get_yuv_cfg  __info_ahd;
/**get yuv data*/
//拿VIDEO1举例，在不影响屏显的情况下获取VIDEO1的YUV数据，那么dev_name则为video1.4（若是VIDEO2的话则为video2.4以此类推）。（如下图）
void get_ahd_yuv_task()
{
    int ret;
    void *video_dev_fd = NULL, *fb2 = NULL; // 视频设备和帧缓冲指针
    void *fb = NULL;
    struct fb_map_user map1; // 用于帧缓冲的内存映射
    struct video_format f = {0}; // 视频格式结构体
    struct fb_var_screeninfo fb2_info = {0}; // 帧缓冲信息
    const char *dev_name = "video1.4"; // 对应的摄像头设备名称
    const char *fb_name = "fb2"; // 帧缓冲名称

    // 设置视频格式
    f.type = VIDEO_BUF_TYPE_VIDEO_OVERLAY; // 设置为视频叠加类型
    f.win.left = 0;
    f.win.top = 0;
    f.win.width = 1280; // 设置需要获取YUV数据的宽度
    f.win.height = 720; // 设置需要获取YUV数据的高度
    f.win.border_left = 0;
    f.win.border_top = 0;
    f.win.border_right = 0;
    f.win.border_bottom = 0;
    f.private_data = fb_name; // 记录帧缓冲名称

    // 打开视频设备
    video_dev_fd = dev_open(dev_name, NULL);
    if (video_dev_fd == NULL) {
        printf("%s test open device %s failed\n", fb_name, dev_name); // 打开失败的提示
        return;
    }
    printf("dev_name open success\n");

    // 设置视频格式
    ret = dev_ioctl(video_dev_fd, VIDIOC_SET_FMT, (u32)&f);
    if (ret) {
        printf("VIDIOC_SET_FMT failed\n"); // 设置格式失败的提示
        dev_close(video_dev_fd);
        return;
    }
    printf("VIDIOC_SET_FMT success\n");

    // 启用视频叠加
    ret = dev_ioctl(video_dev_fd, VIDIOC_OVERLAY, 1);
    if (ret) {
        printf("VIDIOC_OVERLAY failed\n"); // 启用叠加失败的提示
        dev_close(video_dev_fd);
        return;
    }
    printf("VIDIOC_OVERLAY success\n");

    // 打开显示通道
    fb2 = dev_open(f.private_data, (void *)FB_COLOR_FORMAT_YUV420);
    if (!fb2) {
        printf(" ||| %s ,,, %d \r\n", __FUNCTION__, __LINE__); // 打开帧缓冲失败的提示
        dev_close(video_dev_fd);
        return;
    }
    printf(" ||| ||| %s ,,, %d \r\n", __FUNCTION__, __LINE__);
#if 0//YUV_SAVE_TEST
    FILE *fd = fopen(CONFIG_ROOT_PATH"yuv_***.yuv", "w+");
    u32 yuv_size = 1280 * 480 * 3 / 2;
    int yuv_cnt = 0;
#endif
//获取到YUV数据后进行数据处理达到负片效果。（如下图）处理后的数据使用全局数组进行储存，此
//例子中使用u8 yuv_deal[1280*480*3/2]={0};数组的大小根据实际获取的YUV宽高相乘的1.5倍。
while (1) {
    // 检查是否需要退出循环
    if (__info_ahd.exit_state) {
        __info_ahd.exit_state = 0; // 重置退出状态
        break; // 退出循环
    }
    // 获取帧缓冲中已使用的内存
    dev_ioctl(fb2, FBIOGET_FBUFFER_INUSED, (int)&map1);
    // 如果存在有效的缓冲区地址
    if ((u32)map1.baddr) {
        extern u8 display_state; // 声明外部变量，表示显示状态
        if (!display_state) { // 如果当前不在显示状态
            // 进行 YUV 到灰度的转换处理
            convert_yuv_to_grayscale(map1.baddr, yuv_deal, 1280, 720); // 数据负片处理
            display_state = 1; // 更新显示状态为已显示
        }




            /**数据写卡测试*/
        #if 0//YUV_SAVE_TEST    /*保存YUV数据测试*/
        if (fd) { // 如果文件指针fd有效
            yuv_cnt++; // 增加YUV计数器
            fwrite(fd, yuv_deal, yuv_size); // 将YUV数据写入文件

            if (yuv_cnt > 30 * 5) { // 如果计数器超过150（30帧，每帧5次）
                fclose(fd); // 关闭文件
                fd = NULL; // 将文件指针置为NULL
            }
            printf("write file ok : %d \n", yuv_cnt); // 输出写入文件的计数
        } else if (fd) { // 如果fd依然有效（这个条件是多余的，因为上面已验证fd）
            fclose(fd); // 关闭文件
            fd = NULL; // 将文件指针置为NULL
        }


        #endif
            //需要使用YUV数据的任务的回调
            //printf("\nmap1.baddr is 0x%x\n",map1.baddr);
//            __info_ahd.cb(map1.baddr);
            //获取到才释放


            dev_ioctl(fb2, FBIOPUT_FBUFFER_INUSED, (int)&map1);

        } else {
            //获取不到重试
            continue;
        }

    }

    dev_close(fb2); // 关闭显示缓冲区fb2
    dev_ioctl(video_dev_fd, VIDIOC_OVERLAY, 0); // 禁用视频叠加
    dev_close(video_dev_fd); // 关闭视频设备
    printf("get_yuv_task exit\n\n "); // 输出任务退出信息
}
static u8 get_ahd_yuv_init_flag=0;
//首先创建任务获取对应摄像头的YUV数据。（如下图）
//然后在需要使用YUV数据的地方调用get_ahd_yuv_init()函数，传入回调函数，即可获取到对应摄像头的YUV数据。
void get_ahd_yuv_init(void (*cb)(u8 *data)) // 获取对应摄像头的数据
{
    if (get_ahd_yuv_init_flag == 1) {
        return; // 如果已经初始化过，则直接返回
    }
    get_ahd_yuv_init_flag = 1; // 标记为已初始化
    __info_ahd.cb = cb; // 设置回调函数
    __info_ahd.exit_state = 0; // 初始化退出状态
    thread_fork("GET_YUV_TASK_AHD", 10, 0x1000, 0, &__info_ahd.pid, get_ahd_yuv_task, NULL); // 创建新的线程以获取YUV数据
}

void get_ahd_yuv_uninit(void)
{
    if (get_ahd_yuv_init_flag == 0) { // 如果初始化标志为0，表示未初始化
        return; // 直接返回
    }
    get_ahd_yuv_init_flag = 0; // 将初始化标志设为0，表示已取消初始化
    __info_ahd.exit_state = 1; // 设置退出状态，指示线程应终止
    thread_kill(&__info_ahd.pid, KILL_WAIT); // 杀死获取YUV数据的线程，等待其退出
}

//图像比例
// void Fvideo_set_disp_window_with_aspect_ratio(float aspect_ratio) {
//     u16 display_width = SCREEN_W;  // 屏幕宽度
//     u16 display_height = 720;      // 屏幕高度，固定720
//     u16 crop_width, crop_height;
//     u16 offset_x = 0, offset_y = 0;

//     // 根据输入的宽高比（aspect_ratio）计算裁剪宽高
//     if (aspect_ratio == 16.0f / 9.0f) {
//         // 16:9 显示，无需裁剪
//         crop_width = display_width;
//         crop_height = display_height;
//     } else if (aspect_ratio == 4.0f / 3.0f) {
//         // 4:3 显示，计算裁剪尺寸
//         crop_height = display_height;
//         crop_width = (crop_height * 4) / 3;  // 宽度保持4:3
//         offset_x = (display_width - crop_width) / 2;  // 横向居中裁剪
//     } else if (aspect_ratio == 1.0f) {
//         // 1:1 显示，正方形显示
//         crop_height = display_height;
//         crop_width = crop_height;  // 宽高相等
//         offset_x = (display_width - crop_width) / 2;  // 横向居中裁剪
//     } else {
//         // 默认16:9
//         crop_width = display_width;
//         crop_height = display_height;
//     }

//     // 调用裁剪函数，调整画面显示比例
//     video_set_crop(offset_x, offset_y, crop_width, crop_height);

//     // 设置全屏窗口显示
//     disp_window[DISP_FRONT_WIN][1].width = crop_width;   // 设置裁剪后的宽度
//     disp_window[DISP_FRONT_WIN][1].height = crop_height; // 设置裁剪后的高度
//     disp_window[DISP_FRONT_WIN][1].top = 40;             // 上边距保持为40
//     disp_window[DISP_FRONT_WIN][1].left = offset_x;      // 左边距，根据裁剪结果调整

//     // 禁用其他窗口
//     disp_window[DISP_FRONT_WIN][2].width = (u16)-1;
//     disp_window[DISP_FRONT_WIN][2].height = 0;

//     // 更新显示
//     // update_display();
// }
void set_display_crop(int aspect_ratio) {
    u16 offset_x, offset_y, width, height;

    switch (aspect_ratio) {
        case 0: // 16:9 保持原始分辨率不裁剪
            offset_x = 0;
            offset_y = 0;
            width = 1280;
            height = 720;
            break;
        case 1: // 4:3 裁剪为960x720
            offset_x = 160;  // 保证居中裁剪
            offset_y = 0;
            width = 960;
            height = 720;
            break;
        case 2: // 1:1 裁剪为720x720
            offset_x = 280;  // 保证居中裁剪
            offset_y = 0;
            width = 720;
            height = 720;
            break;
        default:
            return;  // 无效比例直接返回
    }

    // 调用裁剪函数，传入裁剪区域
    video_set_crop(offset_x, offset_y, width, height);
}
void set_display_window(int aspect_ratio) {
    u16 width, height, left, top;

    // 根据不同的裁剪比例设置显示窗口的宽度和高度
    switch (aspect_ratio) {
        case 0: // 16:9
            width = 1280;
            height = 720;
            break;
        case 1: // 4:3
            width = 960;
            height = 720;
            break;
        case 2: // 1:1
            width = 720;
            height = 720;
            break;
        default:
            return; // 无效比例返回
    }

    // 居中显示
    left = (1280 - width) / 2;  // 屏幕居中
    // top = (720 - height) / 2;   // 屏幕居中

    // 设置显示窗口的参数
    disp_window[DISP_MAIN_WIN][1].width  = width;
    disp_window[DISP_MAIN_WIN][1].height = height;
    disp_window[DISP_MAIN_WIN][1].left   = left;
    disp_window[DISP_MAIN_WIN][1].top    = 40;

    // 开始显示裁剪后的画面
    video_disp_start(1, &disp_window[DISP_MAIN_WIN][1]);
}


