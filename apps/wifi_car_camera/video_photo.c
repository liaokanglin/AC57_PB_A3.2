/*************************************************************************
	> File Name: video_photo.c
	> Author:
	> Mail:
	> Created Time: Thu 16 Feb 2017 09:35:08 AM HKT
 ************************************************************************/

#include "system/includes.h"
#include "server/video_server.h"
#include "server/video_dec_server.h"
#include "server/ui_server.h"
#include "action.h"
#include "style.h"
#include "app_config.h"
#include "storage_device.h"
#include "video_photo.h"
#include "video_system.h"
#include "vrec_osd.h"
#include "user_isp_cfg.h"
#include "key_voice.h"
#include "app_database.h"
#include "asm/lcd_config.h"


#define PHOTO_MODE_UNINIT   0x0
#define PHOTO_MODE_ACTIVE   0x1
#define PHOTO_MODE_STOP     0x2
#define PHOTO_MODE_QSCAN    0x4


#define CAMERA_OFFLINE          0x0
#define CAMERA_ONLINE           0x1
#define CAMERA_BUSY             0x2
#define MAX_PHOTO_NUMBER        4096

struct photo_resolution {
    u16 width;
    u16 height;
};

static const char *photo_path[4] = {
    CAMERA0_CAP_PATH"img_***.jpg",
    CAMERA1_CAP_PATH"img_***.jpg",
    CAMERA2_CAP_PATH"img_***.jpg",
    CAMERA3_CAP_PATH"img_***.jpg",
};

struct video_photo_handle tph_handler = {
    .state = PHOTO_MODE_UNINIT,
};

static struct photo_resolution camera0_reso[] = {
    {640,  480},  //                VGA
    {1280, 720},  // {1024, 768}    1M
    {1920, 1080}, // {1600, 1200}   2M
    {2048, 1536}, //                3M
    {2560, 1936}, //                4M
    {2592, 1944}, //                5M
    {3072, 1944}, //                6M
    {3456, 1944}, //                7M
    {3456, 2448}, //                8M
    {3456, 2592}, //                9M
    {3648, 2736}, //                10M

    /* 新增加的分辨率，默认快速插值，
     * 用线性插值需将linear_scale置1
     * */
    /* {5120, 2880}, //                14M */
    /* {5504, 2944}, //                16M */
    /* {5632, 3072}, //                17M */
    /* {5760, 3200}, //                18M */
    /* {6016, 4096}, //                24M */
    /* {7680, 4352}, //                32M */
    /* {7936, 5504}, //                43M */
    /* {9600, 5120}, //                48M */
    /* {10240, 5760}, //               58M(64M)目前最大为720P纵横扩大8倍 */

    {4032, 2736}, //                11M
    {4032, 3024}, //                12M
};

enum {
    CAMERA_RESO_VGA = 0x0,
    CAMERA_RESO_1M,
    CAMERA_RESO_2M,
    CAMERA_RESO_3M,
    CAMERA_RESO_4M,
    CAMERA_RESO_5M,
    CAMERA_RESO_6M,
    CAMERA_RESO_7M,
    CAMERA_RESO_8M,
    CAMERA_RESO_9M,
    CAMERA_RES0_10M,
    CAMERA_RESO_11M,
    CAMERA_RESO_12M,
};

#define __this  (&tph_handler)

//#define CAMERA0_CAP_PATH        CONFIG_REC_PATH"/C/1/"
//#define CAMERA1_CAP_PATH        CONFIG_REC_PATH"/C/2/"

/*
const u16 video_photo_reso_w[] = {800, 1280, 1600, 1920, 2208, 2560, 3072, 3712};
const u16 video_photo_reso_h[] = {600, 720, 912, 1088, 1536, 1936, 2208, 1952};
*/

static int photo_switch_camera(void);
static int video_take_photo(struct photo_camera *camera);
static void camera_close(struct photo_camera *camera);
static int photo_camera_stop_display(void);
static int photo_camera_display(void);

struct imc_presca_ctl *video_disp_get_pctl();
extern int wbl_set_function_camera0(u32 parm);


void video_photo_post_msg(const char *msg, ...)
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;
    va_list argptr;

    va_start(argptr, msg);

    if (__this->ui) {
//        req.msg.receiver = ID_WINDOW_VIDEO_TPH;
        req.msg.msg = msg;
        req.msg.exdata = argptr;
        server_request(__this->ui, UI_REQ_MSG, &req);
    }

    va_end(argptr);
#endif

}

static u32 photo_db_select(const char *table)
{
    if (__this->camera_id == 0) {
        return db_select(table);
    } else {
        if ((strcmp(table, "cyt") == 0) ||
            (strcmp(table, "pdat") == 0) ||
            (strcmp(table, "sca") == 0) ||
            (strcmp(table, "qua") == 0) ||
            (strcmp(table, "phm") == 0)) {
            return db_select(table);
        }
    }
    return 0;
}


int set_camera0_reso(u8 res)
{
#ifdef CONFIG_VIDEO4_ENABLE
    for (int i = 0; i < 4; i++) {
        __this->camera[i].id = i;
        __this->camera[i].state = CAMERA_ONLINE;
        __this->camera[i].width = camera0_reso[res].width;
        __this->camera[i].height = camera0_reso[res].height;
    }
#else
    __this->camera[0].width = camera0_reso[res].width;
    __this->camera[0].height = camera0_reso[res].height;
#endif

    return 0;
}


static int set_label_config(u8 camera_id, u16 image_width, u16 image_height, u32 font_color,
                            struct video_text_osd *label)
{
    if (!label) {
        return 0;
    }

    /*
     *日期标签设置
     *1920以下使用 16x32字体大小，以上使用32x64字体
     */
    label->direction = 0;
    /* if (image_width > 1920) { */
    if (image_width > 2304) {
        label->font_w = 32;
        label->font_h = 64;
        label->text_format = label_format;
        label->font_matrix_table = (char *)label_index_tbl;
        label->font_matrix_base = (u8 *)label_font_32x64;
        label->font_matrix_len = sizeof(label_font_32x64);
    } else {
        label->font_w = 16;
        label->font_h = 32;
        label->text_format = label_format;
        label->font_matrix_table = osd_str_total;
        /* if (camera_id == 3) { */
        /* label->font_matrix_base = osd_mimc_str_matrix; */
        /* label->font_matrix_len = sizeof(osd_mimc_str_matrix); */
        /* } else { */
        label->font_matrix_base = osd_str_matrix;
        label->font_matrix_len = sizeof(osd_str_matrix);
        /* } */
    }
    /* label->osd_yuv = font_color; */
    label->color[0] = font_color;
    label->bit_mode = 1;

    label->x = (image_width - strlen(label_format) * label->font_w) / 64 * 64;
    label->y = (image_height - label->font_h - 16) / 16 * 16;

    /* 大尺寸拍照的时候，用四倍或八倍源yuv插值 */
    if (image_width > 4608) {
        if (image_width <= 5120) {
            label->x = (image_width / 4 - strlen(label_format) * label->font_w) / 64 * 64;
            label->y = (image_height / 4 - label->font_h - 16) / 16 * 16;
        } else {
            label->x = (image_width / 8 - strlen(label_format) * label->font_w) / 64 * 64;
            label->y = (image_height / 8 - label->font_h - 16) / 16 * 16;
        }
    }
    /* if (image_width >= 5120) { */
    /* label->x = (1280 - strlen(label_format) * label->font_w) / 64 * 64; */
    /* label->y = (720 - label->font_h - 16) / 16 * 16; */
    /* } */

    return 0;
}



/*
 *停止快速预览
 */
static void stop_quick_scan(void *p)
{
    union video_dec_req dec_req;

    if (!(__this->state & PHOTO_MODE_QSCAN)) {
        return;
    }

    /*
     *删除timeout超时
     */
    if (__this->qscan_timeout) {
        sys_timeout_del(__this->qscan_timeout);
        __this->qscan_timeout = 0;
    }

    /*
     *请求停止解码
     */
    if (__this->video_dec) {
        server_request(__this->video_dec, VIDEO_REQ_DEC_STOP, &dec_req);
    }

    __this->state &= ~PHOTO_MODE_QSCAN;

    if (!(__this->state & PHOTO_MODE_STOP)) {
        /*
         *恢复摄像头显示
         */
        photo_camera_display();
    }
    sys_key_event_enable();
    sys_touch_event_enable();

}


/*
 *快速预览功能
 */
static int photo_quick_scan(struct photo_camera *camera)
{
    int err = 0;
    int timeout;
    union video_req req = {0};
    union video_dec_req dec_req;

    memset(&dec_req, 0x0, sizeof(dec_req));

    /*
     *获取延时秒数
     */
    timeout = photo_db_select("sca") * 1000;
    log_i("scan time %d\n", timeout);
    if (timeout == 0) {
        return 0;
    }

    /*
     *获取拍摄的IMAGE
     */
    err = server_request(camera->server, VIDEO_REQ_GET_IMAGE, &req);
    if (!err) {
        photo_camera_stop_display();

        if (camera->server) {
            server_close(camera->server);
            camera->server = NULL;
        }

        if (!__this->video_dec) {
            struct video_dec_arg arg = {0};

            arg.dev_name = "video_dec";
            arg.audio_buf_size = 512;
            arg.video_buf_size = 512;
            __this->video_dec = server_open("video_dec_server", (void *)&arg);
            if (!__this->video_dec) {
                return -EFAULT;
            }
        }

        /*
         *预览参数设置
         */
        dec_req.dec.fb = "fb1";
        dec_req.dec.left = 0;
        dec_req.dec.top = 0;
        dec_req.dec.width = 0;
        dec_req.dec.height = 0;

        dec_req.dec.pctl = NULL;
        dec_req.dec.thm_first = 1;
        dec_req.dec.preview = 1;
        dec_req.dec.image.buf = req.image.buf;
        dec_req.dec.image.size = req.image.size;
        if (__this->aux_buf) {
            dec_req.dec.image.aux_buf = __this->aux_buf;
            dec_req.dec.image.aux_buf_size = IMAGE_AUX_BUF_SIZE;
        }

        /*
         *解码拍摄的IMAGE
         */
        err = server_request_async(__this->video_dec, VIDEO_REQ_DEC_IMAGE, &dec_req);

        __this->state |= PHOTO_MODE_QSCAN;
        __this->qscan_timeout = sys_timeout_add(NULL, stop_quick_scan, timeout);

    }

    return err;
}

/*
 *关闭camera
 */
static void camera_close(struct photo_camera *camera)
{
    if (camera->server) {
        server_close(camera->server);
        camera->server = NULL;
    }

    if (__this->cap_buf) {
        free(__this->cap_buf);
        __this->cap_buf = NULL;
    }

    if (__this->aux_buf) {
        free(__this->aux_buf);
        __this->aux_buf = NULL;
    }

    if (__this->thumbnails_buf) {
        free(__this->thumbnails_buf);
        __this->thumbnails_buf = NULL;
    }
}

static int storage_device_available()
{
    struct vfs_partition *part;

    if (!storage_device_ready()) {
        if (!dev_online(SDX_DEV)) {
            video_photo_post_msg("noCard");
        } else {
            video_photo_post_msg("fsErr");
        }
        return false;
    } else {
        part = fget_partition(CONFIG_ROOT_PATH);
        if (part->clust_size < 32 || (part->fs_attr & F_ATTR_RO)) {
            video_photo_post_msg("fsErr");
            return false;
        }
    }
    return true;
}

static int take_photo_num_calc(void *p)
{
    int err;
    int file_num;
    int coef;
    u32 free_space = 0;

    puts("take_photo_num_calc\n");

    if (!storage_device_available()) {
        video_photo_post_msg("reNum:num=%p", "00000");
        return -EINVAL;
    }

    err = fget_free_space(CONFIG_ROOT_PATH, &free_space);
    if (err) {
        return err;
    } else {
#ifdef __CPU_AC521x__
        /*****720P的镜头*****/
        /* coef = photo_db_select("pres") > PHOTO_RES_5M ? 96 : 36; */
        coef = photo_db_select("pres") > PHOTO_RES_5M ? 96 : 61;
#else
        /* coef = photo_db_select("pres") > PHOTO_RES_8M ? 10 : 30; */
        coef = photo_db_select("pres") > PHOTO_RES_5M ? 60 : 40;
#endif
        coef = coef / (photo_db_select("qua") + 1);
        file_num = free_space / (__this->camera[__this->camera_id].width *
                                 __this->camera[__this->camera_id].height / coef / 1024);
        printf("file_num = %d", file_num);
        if (file_num > 99999) {
            file_num = 99999;
        }
    }
    if (__this->camera_id == 0) {
        sprintf(__this->file_str, "%05d", file_num);
    } else {
#ifdef CONFIG_VIDEO1_ENABLE
        sprintf(__this->file_str, "%05d", file_num);
#elif defined CONFIG_VIDEO3_ENABLE
        sprintf(__this->file_str, "%s", "N/A");
#endif
    }

    log_d("take_photo_num_calc: %s\n", __this->file_str);

    video_photo_post_msg("reNum:num=%p", __this->file_str);

    return 0;
}

static int photo_quick_save(struct photo_camera *camera)
{
    int err = 0;                           // 定义一个整型变量 err，初始值为 0，用于存储函数调用的返回值
    struct vfs_partition *part;            // 定义一个指向 vfs_partition 结构体的指针 part，用于存储分区信息
    union video_req req;                   // 定义一个视频请求结构体，用于获取拍摄的图像

    /*
     * 获取拍摄的IMAGE
     */
    if (!storage_device_ready()) {         // 如果存储设备未准备好
        if (!dev_online(SDX_DEV)) {        // 如果 SD 卡设备未在线
            video_photo_post_msg("noCard");// 发送无卡消息
        } else {
            video_photo_post_msg("fsErr"); // 否则发送文件系统错误消息
        }
        return -ENODEV;                    // 返回错误代码，表示设备不存在
    } else {
        part = fget_partition(CONFIG_ROOT_PATH); // 获取当前的文件系统分区
        if (part->clust_size < 32 || (part->fs_attr & F_ATTR_RO)) { // 如果簇大小小于 32 或文件系统为只读
            video_photo_post_msg("fsErr"); // 发送文件系统错误消息
            return -ENODEV;                // 返回错误代码，表示设备不可用
        }
    }

    err = server_request(camera->server, VIDEO_REQ_GET_IMAGE, &req); // 向服务器请求获取拍摄的图像
    if (!err) {                           // 如果请求成功
        FILE *fp = NULL;
        fp = fopen(camera->id == 0 ? CAMERA0_CAP_PATH"img_***.jpg" : CAMERA1_CAP_PATH"img_***.jpg", "w+"); // 打开文件进行写入

        if (fp) {                          // 如果文件打开成功
            fwrite(fp, req.image.buf, req.image.size); // 将图像数据写入文件
            fclose(fp);                    // 关闭文件
        } else {
            log_e("open file err\n");      // 如果文件打开失败，记录错误日志
        }
    }
    gpio_direction_output(IO_PORTA_05, 1);

    return err;                            // 返回函数执行的结果，0 表示成功，非 0 表示错误
}



static int video_take_photo(struct photo_camera *camera)
{
    int err = 0;
    union video_req req = {0};
    struct icap_auxiliary_mem aux_mem;
    struct jpg_thumbnail thumbnails;
    static struct video_source_crop crop = {0};
    char video_name[16];
    int i = 1;

#ifndef CONFIG_FAST_CAPTURE
    if (!storage_device_available()) {
        return -EINVAL;
    }
#else
    void ispt_set_block_frame_count(u32 c);
    ispt_set_block_frame_count(6);
#endif

    if (!camera || (camera->state != CAMERA_ONLINE)) {
        return -EINVAL;
    }

    /*
     *打开相机对应的video服务
     */
    /* #ifdef CONFIG_VIDEO1_ENABLE */
    /* sprintf(video_name, "video%d", __this->camera_id); */
    /* #elif (defined CONFIG_VIDEO2_ENABLE) */
    /* sprintf(video_name, "video%d", (__this->camera_id == 1) ? 3 : 0); */
    /* #endif */
#ifdef CONFIG_VIDEO4_ENABLE
    sprintf(video_name, "video4.%d.0", __this->camera_id);
#else
    sprintf(video_name, "video%d.0", __this->camera_id);
#endif

    if (!camera->server) {
        camera->server = server_open("video_server", video_name);
    } else {
        if ((camera->prev_width != camera->width) || (camera->prev_height != camera->height)) {
            server_close(camera->server);
            /******一般尺寸拍照去掉2个buffer,节省动态空间*****/
            if (camera->width < 1920) {
                /* if (__this->aux_buf) { */
                /* free(__this->aux_buf);	 */
                /* __this->aux_buf = NULL; */
                /* } */

                /*
                if (__this->thumbnails_buf) {
                    free(__this->thumbnails_buf);
                    __this->thumbnails_buf = NULL;
                }
                */
            }
            camera->server = server_open("video_server", video_name);
        }
    }
    camera->prev_width = camera->width;
    camera->prev_height = camera->height;

    if (!camera->server) {
        return -EFAULT;
    }

    /*
     *设置拍照所需要的buffer
     */
    if (!__this->cap_buf) {
        __this->cap_buf = (u8 *)malloc(CAMERA_CAP_BUF_SIZE);
    }

    if (camera->width > 1920 && !__this->aux_buf) {
        /*
         *设置尺寸较大时缩放所需要的buffer
         */
        __this->aux_buf = (u8 *)malloc(IMAGE_AUX_BUF_SIZE);
        /*__this->thumbnails_buf = (u8 *)malloc(IMAGE_THUMB_BUF_SIZE);*/
        if (!__this->aux_buf/* || !__this->thumbnails_buf*/) {
            err = -ENOMEM;
            goto __err;
        }
    }

    if (!__this->cap_buf) {
        err = -ENOMEM;
        goto __err;
    }

    aux_mem.addr = __this->aux_buf;
    aux_mem.size = IMAGE_AUX_BUF_SIZE;

    /*
     *配置拍照服务参数
     *尺寸、buffer、质量、日期标签
     */

    req.icap.exif = NULL;
    req.icap.exif_size = 0;
    req.icap.width = camera->width;
    req.icap.height = camera->height;
    req.icap.quality = photo_db_select("qua");

#ifdef CONFIG_FAST_CAPTURE
    req.icap.path = NULL;
#else
    req.icap.path = photo_path[camera->id];
#endif
    /* if (camera->id == 0) { */
    /* req.icap.path = CAMERA0_CAP_PATH"img_***.jpg"; */
    /* } else if (camera->id == 1) { */
    /* req.icap.path = CAMERA1_CAP_PATH"img_***.jpg"; */
    /* } else { */
    /* req.icap.path = CAMERA2_CAP_PATH"img_***.jpg"; */
    /* } */

    /*req.icap.path = camera->id == 0 ? CAMERA0_CAP_PATH"img_***.jpg" : CAMERA1_CAP_PATH"img_***.jpg";*/
    req.icap.buf = __this->cap_buf;
    req.icap.buf_size = CAMERA_CAP_BUF_SIZE;
    req.icap.aux_mem = &aux_mem;
    req.icap.camera_type = VIDEO_CAMERA_NORMAL;
#ifdef JPG_THUMBNAILS_ENABLE
    if (camera->width > 1920) {
        thumbnails.enable = 1;
        thumbnails.quality = 10;
        thumbnails.width = 480;
        thumbnails.height = 320;
        /**将缩略图buffer和拍照缓存复用**/
        thumbnails.buf = __this->cap_buf + CAMERA_CAP_BUF_SIZE - IMAGE_THUMB_BUF_SIZE;//thumbnails_buf;
        thumbnails.len = IMAGE_THUMB_BUF_SIZE;
        req.icap.thumbnails = &thumbnails;
    }
#endif

    if (camera->id == 3) {
        req.icap.camera_type = VIDEO_CAMERA_UVC;
        req.icap.uvc_id = __this->uvc_id;
    }

    /* if(camera->id == 1){ */
    /* req.icap.mirror = 1;	 */
    /* }else{ */
    req.icap.mirror = 0;
    req.icap.jaggy = 0;
    req.icap.linear_scale = 0;	// 线性插值选项
    /* } */

    req.icap.text_label = photo_db_select("pdat") ? &__this->label : NULL;
    req.icap.src_w = camera->src_w;
    req.icap.src_h = camera->src_h;

    set_label_config(__this->camera_id, req.icap.width, req.icap.height, 0xe20095, req.icap.text_label);

#ifdef CONFIG_FAST_CAPTURE
    err = server_request(camera->server, VIDEO_REQ_IMAGE_CAPTURE, &req);
    if (err != 0) {
        puts("take photo err.\n");
        goto __err;
    }
    gpio_direction_output(IO_PORTA_05, 0);
    wait_completion(storage_device_ready, photo_quick_save, camera);
#else

    if (photo_db_select("cyt")) {
        i = 3;
    }

    sys_key_event_disable();
    sys_touch_event_disable();
    video_photo_post_msg("tphin");


    /* crop.x_offset = 320; */
    /* crop.y_offset = 180; */
    /* crop.width = 1920 - 640; */
    /* crop.height = 1080 - 360; */
    /* req.icap.crop = &crop; */

    while (i--) {
        /*
         *发送拍照请求
         */
        err = server_request(camera->server, VIDEO_REQ_IMAGE_CAPTURE, &req);
        if (err != 0) {
            puts("take photo err.\n");
            goto __err;
        }
        key_voice_start(1);
        video_photo_post_msg("tphend");
    }

    extern void malloc_stats(void);
    malloc_stats();

    video_photo_post_msg("tphout");
    photo_quick_scan(camera);

    //__this->free_file_num--;

    log_d("video take photo end\n");
    if (!photo_db_select("sca")) {
        sys_key_event_enable();
        sys_touch_event_enable();
    }
    take_photo_num_calc(NULL);
#endif

    return 0;
__err:

    log_w("video take photo err : %d\n", err);
    camera_close(camera);

    return err;

}



static void take_photo_delay(void *p)
{
    if (__this->delay_ms) {
        video_take_photo(&__this->camera[__this->camera_id]);
        __this->delay_ms = 0;
    }
}


static int video_delay_take_photo(struct photo_camera *camera)
{
    int delay_sec;
    struct vfs_partition *part;

    if (!storage_device_available()) {
        return -EINVAL;
    }

    if (__this->delay_ms > 0) {
        return 0;
    }

    delay_sec = photo_db_select("phm");
    __this->delay_ms = delay_sec * 1000;


    if (__this->timeout) {
        sys_timeout_del(__this->timeout);
        __this->timeout = 0;
    }

    video_photo_post_msg("dlyTPH:ms=%4", __this->delay_ms);
    /*
     *设置超时函数，超过delay_ms拍照
     */
    __this->timeout = sys_timeout_add((void *)camera, take_photo_delay, __this->delay_ms);
    if (!__this->timeout) {
        return -EFAULT;
    }
    sys_key_event_disable();
    sys_touch_event_disable();

    return 0;
}

static int show_main_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (!__this->ui) {
        return -EINVAL;
    }
//    req.show.id = ID_WINDOW_VIDEO_TPH;
    server_request_async(__this->ui, UI_REQ_SHOW, &req);
#endif
    return 0;
}


static void hide_main_ui()
{
#ifdef CONFIG_UI_ENABLE
    union uireq req;

    if (!__this->ui) {
        return;
    }


//    req.hide.id = ID_WINDOW_VIDEO_TPH;
    server_request(__this->ui, UI_REQ_HIDE, &req);
#endif
}

static int photo_camera_display(void)
{
    union video_req req = {0};
    char video_name[16];

#ifdef CONFIG_DISPLAY_ENABLE
    if (__this->display) {
        return 0;
    }

    if (__this->camera[__this->camera_id].state == CAMERA_ONLINE) {

        /*
         *打开对应摄像头的服务
         */
#ifdef CONFIG_VIDEO4_ENABLE
        sprintf(video_name, "video4.%d.0", __this->camera_id);
#else
        sprintf(video_name, "video%d.0", __this->camera_id);
#endif

        __this->display = server_open("video_server", video_name);

        if (!__this->display) {
            puts("server_open:faild\n");
            return -EFAULT;
        }

        /*
         *设置显示参数
         */
        memset(&req, 0x0, sizeof(req));
        req.display.fb 		= "fb1";
        req.display.left  	= 0;//dc->rect.left;
        req.display.top 	= 0;//dc->rect.top;
        req.display.state 	= VIDEO_STATE_START;
        req.display.pctl = video_disp_get_pctl();
#ifdef CONFIG_UI_STYLE_JL02_ENABLE
        /*
         * 切割尺寸
         * */
        req.display.width 	= 1280;
        // 拍照APP启动导致复位
        req.display.height 	= 0;//720;// height;
        /* req.display.border_left  = 0; */
        /* req.display.border_top   = (req.display.height - 480) / 2 / 16 * 16; // 0; */
        /* req.display.border_right = 0; */
        /* req.display.border_bottom = (req.display.height - 480) / 2 / 16 * 16; // 0; */
#elif (defined CONFIG_UI_STYLE_LY_LONGSCREEN_ENABLE)
        req.display.width 	= LCD_DEV_WIDTH;//dc->rect.width;
        req.display.height 	= LCD_DEV_HIGHT;//dc->rect.height;
        req.display.border_left = 0;
        req.display.border_top  = 0;
        req.display.border_right = 0;
        req.display.border_bottom = 0;
#else
        req.display.width 	= LCD_DEV_WIDTH;//dc->rect.width;
        req.display.height 	= LCD_DEV_HIGHT;//dc->rect.height;
        /* req.display.width 	= 0;//dc->rect.width; */
        /* req.display.height 	= 0;//dc->rect.height; */
#endif

        req.display.camera_type = VIDEO_CAMERA_NORMAL;
#ifdef CONFIG_VIDEO3_ENABLE
        if (__this->camera_id == 3) {
            req.display.camera_type = VIDEO_CAMERA_UVC;
            req.display.uvc_id = __this->uvc_id;
        }
#endif
        req.display.src_w = __this->camera[__this->camera_id].src_w;
        req.display.src_h = __this->camera[__this->camera_id].src_h;


        /* if(__this->camera_id == 1){ */
        /* req.display.mirror = 1;	 */
        /* }else{ */
        req.display.mirror = 0;
        req.display.jaggy = 0;
        /* } */

        server_request(__this->display, VIDEO_REQ_DISPLAY, &req);
        if (__this->camera_id == 0) {
#ifndef CONFIG_VIDEO4_ENABLE
            int color = photo_db_select("col");
            if (color != PHOTO_COLOR_NORMAL) {
                set_isp_special_effect(__this->display, color);
            } else {
                start_update_isp_scenes(__this->display);
            }
#endif
        }
        /*
         *设置当前模式下摄像头的参数 : 曝光补偿/白平衡/锐化
         */
        if (__this->camera_id == 0) {
#ifndef CONFIG_VIDEO4_ENABLE
            set_camera_config(&__this->camera[__this->camera_id]);
#endif
        }
    }
#endif
    return 0;
}

static int photo_camera_stop_display(void)
{
    union video_req req = {0};

    if (__this->display) {
        /*
        req.camera.mode = ISP_MODE_REC;
        req.camera.cmd = SET_CAMERA_MODE;
        server_request(__this->display, VIDEO_REQ_CAMERA_EFFECT, &req);
        */
        if (__this->camera_id == 0) {
            stop_update_isp_scenes();
        }

        req.display.state = VIDEO_STATE_STOP;

        server_request(__this->display, VIDEO_REQ_DISPLAY, &req);
        server_close(__this->display);
        __this->display = NULL;

    }

    return 0;
}


static int video_photo_start(void)
{
    int err;
    int res = db_select("pres");
    bool online;
    /*
     *相机初始化，后拉默认VGA分辨率
     */
#ifdef CONFIG_VIDEO0_ENABLE
    __this->camera[0].id = 0;
    __this->camera[0].state = CAMERA_ONLINE;
    __this->camera[0].width = camera0_reso[res].width;
    __this->camera[0].height = camera0_reso[res].height;
#endif

#ifdef CONFIG_VIDEO1_ENABLE
    online = dev_online("video1.*");
    __this->camera[1].id = 1;
    __this->camera[1].state = online ? CAMERA_ONLINE : CAMERA_OFFLINE;
#endif
#ifdef CONFIG_VIDEO3_ENABLE
    online = dev_online("uvc");
    __this->camera[3].id = 3;
    __this->camera[3].state = online ? CAMERA_ONLINE : CAMERA_OFFLINE;
#endif

#ifdef CONFIG_VIDEO1_ENABLE
    __this->camera[1].width = 640;
    __this->camera[1].height = 480;
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    __this->camera[3].width = 1280;//640;
    __this->camera[3].height = 720;//480;
#endif


#ifdef CONFIG_VIDEO4_ENABLE
    for (int i = 0; i < 4; i++) {
        __this->camera[i].id = i;
        __this->camera[i].state = CAMERA_ONLINE;
        __this->camera[i].width = camera0_reso[res].width;
        __this->camera[i].height = camera0_reso[res].height;
    }
#endif


    __this->camera_id = 0;
    puts("video_photo start.\n");
    err = photo_camera_display();
    if (err) {
        return err;
    }

    show_main_ui();

    __this->sd_wait = wait_completion(storage_device_ready, take_photo_num_calc, NULL);
    __this->state = PHOTO_MODE_ACTIVE;

#ifdef CONFIG_FAST_CAPTURE
    gpio_direction_output(IO_PORTA_05, 1);
    video_take_photo(&__this->camera[__this->camera_id]);
#endif

    return 0;
}

static int video_photo_stop(void)
{
    __this->state |= PHOTO_MODE_STOP;

    /*
     *停止快速预览
     */
    stop_quick_scan(NULL);
    /*
     *关闭相机0，相机1，关闭显示
     */
    camera_close(&__this->camera[0]);
    camera_close(&__this->camera[1]);
    camera_close(&__this->camera[2]);
    camera_close(&__this->camera[3]);

    photo_camera_stop_display();

    if (__this->video_dec) {
        server_close(__this->video_dec);
        __this->video_dec = NULL;
    }

    if (__this->timeout) {
        __this->delay_ms = 0;
        sys_timeout_del(__this->timeout);
        __this->timeout = 0;
        //防止拍照计时插USB，按键和touch失效
    }

    hide_main_ui();

    return 0;
}



static void photo_mode_init(void)
{
    if (__this->state == PHOTO_MODE_UNINIT) {
        memset(__this, 0, sizeof(*__this));
    }

    if (!__this->cap_buf) {
        __this->cap_buf = (u8 *)malloc(CAMERA_CAP_BUF_SIZE);
    }
    if (!__this->aux_buf) {
        __this->aux_buf = (u8 *)malloc(IMAGE_AUX_BUF_SIZE);
    }

    __this->state = PHOTO_MODE_ACTIVE;

    void image_set_max_width(u16 max_width);
    image_set_max_width(4608);
}

/*
 * 摄像头切换
 */
static int photo_switch_camera(void)
{
    int err;
    u8 switch_id;

    switch_id = __this->camera_id;
    do {
        if (++switch_id >= PHOTO_CAMERA_NUM) {
            switch_id = 0;
        }

        if (switch_id == __this->camera_id) {
            return 0;
        }

        if (__this->camera[switch_id].state == CAMERA_ONLINE) {
            break;
        }

    } while (switch_id != __this->camera_id);

    stop_quick_scan(NULL);
    camera_close(&__this->camera[__this->camera_id]);
    video_photo_cfg_reset();
    photo_camera_stop_display();

    __this->camera_id = switch_id;

    take_photo_num_calc(NULL);

    err = photo_camera_display();
    if (err) {
        return err;
    }
    printf("camera_id = %d\n",  __this->camera_id);
    video_photo_post_msg("swCAM:id=%1", __this->camera_id);

    return 0;
}

static int video_photo_change_status(struct intent *it)
{
    int err;
    u32 free_space = 0;
    int file_num;
    int coef;
#ifdef CONFIG_UI_ENABLE
    struct key_event org = { KEY_EVENT_CLICK, KEY_OK };
    struct key_event new = { KEY_EVENT_CLICK, KEY_PHOTO };

    if (!strcmp(it->data, "opMENU:")) { /* ui要求打开rec菜单 */
        puts("ui ask me to opMENU:.\n");
        if (1) { /* 允许ui打开菜单 */
            it->data = "opMENU:en";
            sys_key_event_unmap(&org, &new);
        } else { /* 禁止ui打开菜单 */

            it->data = "opMENU:dis";
        }

    } else if (!strcmp(it->data, "exitMENU")) { /* ui已经关闭rec菜单 */
        sys_key_event_map(&org, &new);
        puts("ui tell me exitMENU.\n");
        video_photo_restore();
        take_photo_num_calc(NULL);
    } else if (!strcmp(it->data, "idCAM:")) { /* 获取摄像头id */
        if (__this->camera_id == 0) {
            it->data = "idCAM:0";
        } else {
            it->data = "idCAM:1";
        }
    } else {
        puts("unknow status ask by ui.\n");
    }
#endif
    return 0;
}

static int video_photo_change_source_reso(int dev_id, u16 width, u16 height)
{
    __this->camera[dev_id].src_w = width;
    __this->camera[dev_id].src_h = height;
    if (__this->camera[__this->camera_id].state == CAMERA_ONLINE) {
        log_d("video%d.* change source reso to %d x %d\n", dev_id, width, height);
        if (__this->camera_id == dev_id) {
            if ((__this->state & PHOTO_MODE_ACTIVE) &&
                !(__this->state & PHOTO_MODE_STOP) &&
                !(__this->state & PHOTO_MODE_QSCAN)) {
                photo_camera_stop_display();
                if (__this->timeout) {
                    sys_timeout_del(__this->timeout);
                }
                camera_close(&__this->camera[__this->camera_id]);
                //video2.* photo taking and display must be closed before source reso changing
                photo_camera_display();
                if (__this->timeout) {
                    __this->timeout = sys_timeout_add((void *)&__this->camera[__this->camera_id], take_photo_delay, __this->delay_ms);
                }
            }
        }
    }
    return 0;
}

static int state_machine(struct application *app, enum app_state state, struct intent *it)
{
    int len;
    struct intent mit;
    struct key_event org = { KEY_EVENT_CLICK, KEY_OK };
    struct key_event new = { KEY_EVENT_CLICK, KEY_PHOTO };

    switch (state) {
    case APP_STA_CREATE:
        photo_mode_init();
        server_load(video_server);
#ifdef CONFIG_UI_ENABLE
        __this->ui = server_open("ui_server", NULL);
        if (!__this->ui) {
            return -EINVAL;
        }
#endif
        sys_key_event_map(&org, &new);
        break;
    case APP_STA_START:
        if (!it) {
            break;
        }
        switch (it->action) {
        case ACTION_PHOTO_TAKE_MAIN:
            video_photo_start();
            break;
        case ACTION_PHOTO_TAKE_SET_CONFIG:
            if (!__this->ui) {
                return -EINVAL;
            }
            video_photo_set_config(it);
            db_flush();
            if (it->data && !strcmp(it->data, "pres")) {
                take_photo_num_calc(NULL);
            }
            break;
        case ACTION_PHOTO_TAKE_CHANGE_STATUS:
            video_photo_change_status(it);
            break;
#ifdef CONFIG_UI_STYLE_JL02_ENABLE
        case ACTION_PHOTO_TAKE_CONTROL:
            puts("---------to take photo----------.\n");
            if (__this->state & PHOTO_MODE_QSCAN) {
                break;
            }

            if (photo_db_select("phm") == 0) {
                /*
                 *正常拍照
                 */
                video_take_photo(&__this->camera[__this->camera_id]);
            } else {
                /*
                 *延时拍照
                 */
                video_delay_take_photo(&__this->camera[__this->camera_id]);
            }
            break;
        case ACTION_PHOTO_TAKE_SWITCH_WIN:
            photo_switch_camera();
            break;
#endif
        }
        break;
    case APP_STA_PAUSE:
        break;
    case APP_STA_RESUME:
        break;
    case APP_STA_STOP:
        video_photo_cfg_reset();
        video_photo_stop();
        if (__this->sd_wait) {
            wait_completion_del(__this->sd_wait);
            __this->sd_wait = 0;
        }
        break;
    case APP_STA_DESTROY:
        if (__this->ui) {
            server_close(__this->ui);
            __this->ui = NULL;
        }
        sys_key_event_unmap(&org, &new);
        f_free_cache(CONFIG_ROOT_PATH);
        malloc_stats();
        log_d("<<<<<< video_photo: destroy\n");
        break;
    }

    return 0;

}

static int video_photo_key_event_handler(struct key_event *key)
{
    struct intent it;

    switch (key->event) {
    case KEY_EVENT_CLICK:
        switch (key->value) {
        case KEY_PHOTO:
            if (__this->state & PHOTO_MODE_QSCAN) {
                break;
            }

            if (photo_db_select("phm") == 0) {
                /*
                 *正常拍照
                 */
                video_take_photo(&__this->camera[__this->camera_id]);
            } else {
                /*
                 *延时拍照
                 */
                video_delay_take_photo(&__this->camera[__this->camera_id]);
            }
            break;
        case KEY_MODE:
            break;
        case KEY_MENU:
            break;
        case KEY_UP:
            photo_switch_camera();
            break;
        case KEY_DOWN:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return false;
}

static int video_photo_device_event_handler(struct sys_event *e)
{
    int err;

    if (!strcmp(e->arg, "video1")) {
        switch (e->u.dev.event) {
        case DEVICE_EVENT_IN:
            if (__this->camera_id == 1 && __this->camera[1].state == CAMERA_OFFLINE) {
                err = photo_camera_display();
                if (err) {
                    return err;
                }
            }
            __this->camera[1].state = CAMERA_ONLINE;
            break;
        case DEVICE_EVENT_OUT:
            if (__this->camera_id == 1) {
                photo_switch_camera();
            }
            __this->camera[1].state = CAMERA_OFFLINE;
            break;
        default:
            break;
        }
    }

    if (!strncmp((char *)e->arg, "uvc", 3)) {
        switch (e->u.dev.event) {
        case DEVICE_EVENT_IN:
            __this->uvc_id = ((char *)e->arg)[3] - '0';
            if (__this->camera_id == 3 && __this->camera[3].state == CAMERA_OFFLINE) {
                err = photo_camera_display();
                if (err) {
                    return err;
                }
            }
            __this->camera[3].state = CAMERA_ONLINE;
            break;
        case DEVICE_EVENT_OUT:
            if (__this->camera_id == 3) {
                photo_switch_camera();
            }
            __this->camera[3].state = CAMERA_OFFLINE;
            break;

        }
    }

    if (!strcmp((char *)e->arg, "camera0_err")) {
        photo_camera_stop_display();
        photo_camera_display();
    }

    if (!ASCII_StrCmp(e->arg, "sd*", 4)) {
        take_photo_num_calc(NULL);
    }

    return false;
}

static int event_handler(struct application *app, struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        return video_photo_key_event_handler(&event->u.key);
    case SYS_DEVICE_EVENT:
        return video_photo_device_event_handler(event);
    default:
        return false;
    }
    return false;
}

static const struct application_operation video_photo_ops = {
    .state_machine  = state_machine,
    .event_handler  = event_handler,
};

REGISTER_APPLICATION(app_video_photo) = {
    .name 	= "video_photo",
    .action	= ACTION_PHOTO_TAKE_MAIN,
    .ops 	= &video_photo_ops,
    .state  = APP_STA_DESTROY,
};

