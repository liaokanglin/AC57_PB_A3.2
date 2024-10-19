#include "system/includes.h"
#include "stream_core.h"
#include "server/ui_server.h"
#include "server/video_server.h"
#include "server/video_engine_server.h"
#include "net_video_rec.h"
#include "video_system.h"
#include "gSensor_manage.h"
#include "user_isp_cfg.h"
#include "action.h"
#include "style.h"
#include "app_config.h"
#include "asm/debug.h"
#include "vrec_osd.h"
#include "vrec_icon_osd.h"
#include "app_database.h"
#include "storage_device.h"
#include "server/ctp_server.h"
#include "os/os_compat.h"
#include "net_config.h"
#include "server/net_server.h"
#include "strm_video_rec.h"


static int net_rt_video1_open(struct intent *it);
static int net_rt_video0_open(struct intent *it);
static int net_video_rec_start(u8 mark);
static int net_video_rec_stop(u8 close);
static int net_video_rec1_stop(u8 close);
static int net_video_rec_take_photo(void (*callback)(char *buffer, int len));

#define AUDIO_VOLUME	64
#define NET_VIDEO_REC_SERVER_NAME	"net_video_server"
#define TAKE_PHOTO_MEM_LEN			512 * 1024

#define	NET_VIDEO_ERR(x)	{if(x) printf("######## %s %d err !!!! \n\n\n",__func__,__LINE__);}

static int net_rt_video0_stop(struct intent *it);
static int net_rt_video1_stop(struct intent *it);
static int net_video_rec_start_isp_scenes();
static int net_video_rec_stop_isp_scenes();
static int net_video_rec_control(void *_run_cmd);


static char file_str[64];
static int net_vd_msg[2];

#ifdef __CPU_AC521x__
static const u16 net_rec_pix_w[] = {1280, 640};
static const u16 net_rec_pix_h[] = {720,  480};
#else
static const u16 net_rec_pix_w[] = {1920, 1280, 640};
static const u16 net_rec_pix_h[] = {1088, 720,  480};
#endif

static const u16 net_pic_pix_w[] = {320, 640, 1280};
static const u16 net_pic_pix_h[] = {240, 480, 720 };

static struct net_video_hdl net_rec_handler = {0};
static struct strm_video_hdl *fv_rec_handler = NULL;
static struct video_rec_hdl *rec_handler = NULL;

#define __this_net  (&net_rec_handler)
#define __this_strm	(fv_rec_handler)
#define __this 	(rec_handler)

static OS_MUTEX net_vdrec_mutex;


#define CONFIG_OSD_LOGO

/*
 *start user net video rec , 必须在net_config.h配置宏CONFIG_NET_USR_ENABLE 和NET_USR_PATH
 */
int user_net_video_rec_open(char forward)
{
    int ret;
    u8 open = 2;//char type : 0 audio , 1 video , 2 audio and video
    struct intent it;
    struct rt_stream_app_info info;

    info.width = 1280;
    info.height = 720;

    //帧率 net_config.h : NET_VIDEO_REC_FPS0/NET_VIDEO_REC_FPS1

    it.data = &open;
    it.exdata = (u32)&info;

    if (forward) {
        ret = net_rt_video0_open(&it);
    } else {
        ret = net_rt_video1_open(&it);
    }
    return ret;
}

int user_net_video_rec_close(char forward)
{
    int ret;
    struct intent it;
    u8 close = 2;//char type : 0 audio , 1 video , 2 audio and video

    it.data = &close;
    if (forward) {
        ret = net_rt_video0_stop(&it);
    } else {
        ret = net_rt_video1_stop(&it);
    }
    return ret;
}

void user_net_video_rec_take_photo_cb(char *buf, int len)//必须打开user_net_video_rec_open()
{
    if (buf && len) {
        printf("take photo success \n");
        put_buf(buf, 32);
        /*
        //目录1写卡例子
        FILE *fd = fopen(CAMERA0_CAP_PATH"IMG_***.jpg","w+");
        if (fd) {
        	fwrite(fd,buf,len);
        	fclose(fd);
        }
        */
    }
}
//qua : 0 240p, 1 480p, 2 720p
int user_net_video_rec_take_photo(int qua, void (*callback)(char *buf, int len))//必须打开user_net_video_rec_open()
{
    db_update("qua", qua);
    net_video_rec_take_photo(callback);
    return 0;
}
//example 720P: user_net_video_rec_take_photo(2, user_net_video_rec_take_photo_cb);
int user_net_video_rec_take_photo_test(void)
{
    user_net_video_rec_take_photo(2, user_net_video_rec_take_photo_cb);
    return 0;
}
/*
 *end of user net video rec
 */
int net_video_rec_get_list_vframe(void)
{
    return __this_net->fbuf_fcnt;
}
void net_video_rec_pkg_get_video_in_frame(char *fbuf, u32 frame_size)
{
    __this_net->fbuf_fcnt++;
    __this_net->fbuf_ffil += frame_size;
}
void net_video_rec_pkg_get_video_out_frame(char *fbuf, u32 frame_size)
{
    if (__this_net->fbuf_fcnt) {
        __this_net->fbuf_fcnt--;
    }
    if (__this_net->fbuf_ffil) {
        __this_net->fbuf_ffil -= frame_size;
    }
}
static void net_video_handler_init(void)
{
    fv_rec_handler = (struct strm_video_hdl *)get_strm_video_rec_handler();
    rec_handler  = (struct video_rec_hdl *)get_video_rec_handler();
}

void *get_net_video_rec_handler(void)
{
    return (void *)&net_rec_handler;
}

void *get_video_rec0_handler(void)
{
    return (void *)__this->video_rec0;
}

u8 get_net_video_rec_state(void)
{
    return __this_net->is_open;
}

void *get_video_rec1_handler(void)
{
#ifdef CONFIG_VIDEO1_ENABLE
    return (void *)__this->video_rec1;
#endif
#ifdef CONFIG_VIDEO3_ENABLE
    return (void *)__this->video_rec2;
#endif
    return NULL;
}

int net_video_rec_state(void)//获取录像状态，返回值大于0（1280/640）成功，失败 0
{
    u8 res = db_select("res");
    if (__this && __this->state == VIDREC_STA_START) {
        return net_rec_pix_w[res];
    }
    return 0;
}
int net_video_rec_uvc_online(void)
{
    return dev_online("uvc");
}
int net_pkg_get_video_size(int *width, int *height)
{
    u8 id = __this_net->video_id ? 1 : 0;
    *width = __this_net->net_videoreq[id].rec.width;
    *height = __this_net->net_videoreq[id].rec.height;
    return 0;
}
int net_video_buff_set_frame_cnt(void)
{
#ifdef NET_VIDEO_BUFF_FRAME_CNT
    return NET_VIDEO_BUFF_FRAME_CNT;
#else
    return 0;
#endif
}
u8 net_video_rec_get_drop_fp(void)
{
#ifdef NET_VIDEO_REC_DROP_REAl_FP
    return NET_VIDEO_REC_DROP_REAl_FP;
#else
    return 0;
#endif
}
u8 net_video_rec_get_lose_fram(void)
{
#ifdef NET_VIDEO_REC_LOSE_FRAME_CNT
    return NET_VIDEO_REC_LOSE_FRAME_CNT;
#else
    return 0;
#endif
}
u8 net_video_rec_get_send_fram(void)
{
#ifdef NET_VIDEO_REC_SEND_FRAME_CNT
    return  NET_VIDEO_REC_SEND_FRAME_CNT;
#else
    return 0;
#endif
}
int net_video_rec_get_fps(void)
{
    return 25;
}

void net_video_rec_post_msg(const char *msg, ...)
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

void net_video_rec_fmt_notify(void)
{
    char buf[32];
#if defined CONFIG_ENABLE_VLIST
    FILE_DELETE(NULL, 0);
    sprintf(buf, "frm:1");
    CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "FORMAT", "NOTIFY", buf);
#endif
}

char *video_rec_finish_get_name(FILE *fd, int index, u8 is_emf)  //index ：video0前视0，video1则1，video2则2 , is_emf 紧急文件
{
    static char path[128] ALIGNE(32) = {0};
    u8 name[64];
    u8 *dir;
    int err;

#ifdef CONFIG_ENABLE_VLIST
    memset(path, 0, sizeof(path));
    err = fget_name(fd, name, 64);
    if (err <= 0) {
        return NULL;
    }
    if (index < 0) {
        strcpy(path, name);
        return path;
    }
    switch (index) {
    case 0:
        dir = CONFIG_REC_PATH_0;
        break;
    case 1:
        dir = CONFIG_REC_PATH_1;
        break;
    case 2:
        dir = CONFIG_REC_PATH_2;
        break;
    case 3:
        dir = CONFIG_REC_PATH_2;
        break;

    default:
        return NULL;
    }
#ifdef CONFIG_EMR_DIR_ENABLE
    if (is_emf) {
        sprintf(path, "%s%s%s", dir, CONFIG_EMR_REC_DIR, name);
    } else
#endif
    {
        sprintf(path, "%s%s", dir, name);
    }

    return  path;
#else
    return NULL;
#endif
}

int video_rec_finish_notify(char *path)
{
    int err = 0;

    if (__this_net->video_rec_err) {
        __this_net->video_rec_err = FALSE;
        return 0;
    }
    os_mutex_pend(&net_vdrec_mutex, 0);
    FILE_LIST_ADD(0, (const char *)path, 0);
    os_mutex_post(&net_vdrec_mutex);
    return err;
}
int video_rec_delect_notify(FILE *fd, int id)
{
    int err = 0;
    if (__this_net->video_rec_err) {
        __this_net->video_rec_err = FALSE;
        return 0;
    }
#ifdef CONFIG_ENABLE_VLIST
    char *delect_path;
    os_mutex_pend(&net_vdrec_mutex, 0);
    char *path = video_rec_finish_get_name(fd, id, 0);
    if (path == NULL) {
        os_mutex_post(&net_vdrec_mutex);
        return -1;
    }

    FILE_DELETE((const char *)path, 0);
    os_mutex_post(&net_vdrec_mutex);
#endif

    return err;
}

int video_rec_err_notify(const char *method)
{
    int err = 0;
    char *err_path;
    os_mutex_pend(&net_vdrec_mutex, 0);
    if (method && !strcmp((const char *)method, "VIDEO_REC_ERR")) {
        __this_net->video_rec_err = TRUE;
    }
    os_mutex_post(&net_vdrec_mutex);
    return err;
}

int video_rec_state_notify(void)
{
    int err = 0;
    net_vd_msg[0] = NET_VIDREC_STATE_NOTIFY;
    err = os_taskq_post_msg(NET_VIDEO_REC_SERVER_NAME, 1, (int)net_vd_msg);
    return err;
}

int video_rec_start_notify(void)
{
    return net_video_rec_control(0);//启动录像之前需要关闭实时流
}
int video_rec_all_stop_notify(void)
{
    int err = 0;
    net_vd_msg[0] = NET_VIDREC_STA_STOP;
    err = os_taskq_post_msg(NET_VIDEO_REC_SERVER_NAME, 1, (int)net_vd_msg);
    return err;
}
void net_video_rec_status_notify(void)
{
    char buf[128];
    u8 sta = 0;
    if (__this->state == VIDREC_STA_START) {
        sta = 1;
    }
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "status:%d", sta);
    CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "VIDEO_CTRL", "NOTIFY", buf);
}
int net_video_rec_event_notify(void)
{
    char buf[128];
    u32 res = db_select("res2");
#ifdef CONFIG_VIDEO1_ENABLE
    if (dev_online("video1.*"))
#else
#ifdef CONFIG_VIDEO3_ENABLE
    if (dev_online("uvc"))
#else
    if (0)
#endif
#endif
    {
        switch (res) {
        case VIDEO_RES_1080P:
            sprintf(buf, "status:1,h:%d,w:%d,fps:%d,rate:%d,format:1", 720, 1280, net_video_rec_get_fps(), net_video_rec_get_audio_rate());
            break;
        case VIDEO_RES_720P:
            sprintf(buf, "status:1,h:%d,w:%d,fps:%d,rate:%d,format:1", 480, 640, net_video_rec_get_fps(), net_video_rec_get_audio_rate());
            break;
        case VIDEO_RES_VGA:
            sprintf(buf, "status:1,h:%d,w:%d,fps:%d,rate:%d,format:1", 480, 640, net_video_rec_get_fps(), net_video_rec_get_audio_rate());
            break;
        default:
            sprintf(buf, "status:1,h:%d,w:%d,fps:%d,rate:%d,format:1", 480, 640, net_video_rec_get_fps(), net_video_rec_get_audio_rate());
            break;
        }
        CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "PULL_VIDEO_STATUS", "NOTIFY", buf);
    } else {
        strcpy(buf, "status:0");
        CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "PULL_VIDEO_STATUS", "NOTIFY", buf);
    }
    return 0;
}
int net_video_rec_event_stop(void)
{
    strm_video_rec_close();
    net_video_rec_stop(0);
    return 0;
}
int net_video_rec_event_start(void)
{
    strm_video_rec_open();
    net_video_rec_start(1);
    return 0;
}
int video_rec_sd_event_ctp_notify(char state)
{
    char buf[128];
    printf("~~~ : %s , %d\n\n", __func__, state);

    /* #if defined CONFIG_ENABLE_VLIST */
    /* if (!state) { */
    /* FILE_REMOVE_ALL(); */
    /* } */
    /* #endif */
    if (state) {
        u32 space = 0;
        struct vfs_partition *part = NULL;
        if (storage_device_ready() == 0) {
            msleep(200);
            if (storage_device_ready()) {
                goto sd_scan;
            }
            printf("---%s , storage_device_not_ready !!!!\n\n", __func__);
            CTP_CMD_COMBINED(NULL, CTP_SD_OFFLINE, "TF_CAP", "NOTIFY", CTP_SD_OFFLINE_MSG);
        } else {
sd_scan:
            part = fget_partition(CONFIG_ROOT_PATH);
            fget_free_space(CONFIG_ROOT_PATH, &space);
            sprintf(buf, "left:%d,total:%d", space / 1024, part->total_size / 1024);
            CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "TF_CAP", "NOTIFY", buf);
        }
    } else {
        sprintf(buf, "status:%d", 0);
        CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "VIDEO_CTRL", "NOTIFY", buf);
    }
    sprintf(buf, "online:%d", state);
    CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "SD_STATUS", "NOTIFY", buf);
    return 0;
}

//NET USE API
static void video_rec_get_app_status(struct intent *it)
{
    it->data = (const char *)__this;
    it->exdata = (u32)__this_net;
}
static void video_rec_get_path(struct intent *it)
{

    u8  buf[32];
#ifdef CONFIG_VIDEO0_ENABLE

    if (!strcmp(it->data, "video_rec0") && __this->file[0] != NULL) {
        fget_name((FILE *)__this->file[0], buf, sizeof(buf));
        sprintf(file_str, CONFIG_REC_PATH_0"%s", buf);
        it->data = file_str;
    }

#endif

#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
    if (!strcmp(it->data, "video_rec1") && __this->file[1] != NULL) {
        fget_name((FILE *)__this->file[1], buf, sizeof(buf));
        sprintf(file_str, CONFIG_REC_PATH_1"%s", buf);
        it->data = file_str;
    } else if (!strcmp(it->data, "video_rec2") && __this->file[2] != NULL) {
        fget_name((FILE *)__this->file[2], buf, sizeof(buf));
        sprintf(file_str, CONFIG_REC_PATH_1"%s", buf);
        it->data = file_str;
    } else
#endif
    {
        puts("get file name fail\n");
        it->data = NULL;
    }
    printf("file %s \n", it->data);
}
static void ctp_cmd_notity(const char *path)
{
#if defined CONFIG_ENABLE_VLIST
    FILE_LIST_ADD(0, path, 0);
#endif
}

int net_video_rec_get_audio_rate()
{
    return VIDEO_REC_AUDIO_SAMPLE_RATE;
}
static int video_cyc_file(u32 sel)
{
    /*
        struct server *server = NULL;
        union video_req req = {0};
        if (sel == 0) {
            server = __this->video_rec0;

            req.rec.channel = 0;
        } else if (sel == 1) {
            server = __this->video_rec1;
            req.rec.channel = 1;
        } else {
            server = __this->video_rec2;
            req.rec.channel = 2;
        }
        if ((__this->state != VIDREC_STA_START) || (server == NULL)) {
            return -EINVAL;
        }

        req.rec.state   = VIDEO_STATE_CYC_FILE;
        req.rec.cyc_file = 1;
        int err = server_request(server, VIDEO_REQ_REC, &req);
        if (err != 0) {
            return -EINVAL;
        }
    */
    return 0;
}


/*后拉专用*/
/*
 *  *场景切换使能函数，如果显示打开就用显示句柄控制，否则再尝试用录像句柄控制
 *   */
static int net_video_rec_doing_isp_scenes(void *p)
{
    if (__this_net->isp_scenes_status) {
        return 0;
    }

    stop_update_isp_scenes();
    if ((int)p) {
        if (__this_net->net_video_rec && ((__this_net->net_state == VIDREC_STA_START) ||
                                          (__this_net->net_state == VIDREC_STA_STARTING))) {
            __this_net->isp_scenes_status = 1;
            return start_update_isp_scenes(__this_net->net_video_rec);
        } else if (__this_net->net_video_rec1 && ((__this_net->net_state1 == VIDREC_STA_START) ||
                   (__this_net->net_state1 == VIDREC_STA_STARTING))) {
            __this_net->isp_scenes_status = 2;
            return start_update_isp_scenes(__this_net->net_video_rec1);
        }
    }

    __this_net->isp_scenes_status = 0;

    return 1;
}
static int net_video_rec_start_isp_scenes()
{
    return wait_completion(isp_scenes_switch_timer_done, net_video_rec_doing_isp_scenes, (void *)1);
}
static int net_video_rec_stop_isp_scenes()
{
    if (!__this_net->isp_scenes_status) {
        return 0;
    }
    __this_net->isp_scenes_status = 0;
    stop_update_isp_scenes();
    return 0;
}
static int set_label_config(u16 image_width, u16 image_height, u32 font_color,
                            struct video_text_osd *label)
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

    label->x = (image_width - strlen(label_format) * label->font_w) / 64 * 64;
    label->y = (image_height - label->font_h - 16) / 16 * 16;


    return 0;
}

int video_rec_cap_photo(char *buf, int len)
{
    u32 *flen;
    u8 *cap_img;
    if (__this_net->cap_image) {
        cap_img = &__this_net->cap_image;
    } else {
        cap_img = &__this_strm->cap_image;
    }
    if (*cap_img && __this->cap_buf) {
        *cap_img = FALSE;
        flen = __this->cap_buf;
        memcpy(__this->cap_buf + 4, buf, len);
        *flen = len;
    }
    return 0;
}
static int net_video_rec_take_photo(void (*callback)(char *buffer, int len))
{
    struct server *server = NULL;
    void *cap_buf = NULL;
    union video_req req = {0};
    int err;
    char *path;
    u32 *image_len;
    char buf[128] = {0};
    char video_dev_name[20] = {0};
    char name_buf[20];
    struct video_text_osd text_osd;
    u8 server_open_flag = 0;
    /*不开实时流不能拍照*/
    if (!(__this_strm->state == VIDREC_STA_START)    &&
        !(__this_net->net_state == VIDREC_STA_START) &&
        !(__this_net->net_state1 == VIDREC_STA_START)) {
        goto exit;
    }

    __this_net->cap_image = FALSE;
    __this_strm->cap_image = FALSE;

    if ((__this_net->net_state == VIDREC_STA_START || __this_net->net_state1 == VIDREC_STA_START)) {
        printf("%s   %d\n", __func__, __LINE__);

        server = !__this_net->video_id ?  __this_net->net_video_rec :  __this_net->net_video_rec1;
        req.icap.path = !__this_net->video_id ? CONFIG_REC_PATH_0"IMG_****.jpg" : CONFIG_REC_PATH_1"IMG_****.jpg";
        path = !__this_net->video_id ? CONFIG_REC_PATH_0 : CONFIG_REC_PATH_1;
        req.icap.width =  __this_net->net_videoreq[__this_net->video_id].rec.width;
        req.icap.height = __this_net->net_videoreq[__this_net->video_id].rec.height;
        printf("%s   %d\n", __func__, __LINE__);
    } else if (__this_strm->state == VIDREC_STA_START) {
        printf("%s   %d\n", __func__, __LINE__);
        req.icap.path = !__this_strm->video_id ? CONFIG_REC_PATH_0"IMG_****.jpg" : CONFIG_REC_PATH_1"IMG_****.jpg";
        path = !__this_strm->video_id ? CONFIG_REC_PATH_0 : CONFIG_REC_PATH_1;
        req.icap.width = __this_strm->width;
        req.icap.height = __this_strm->height;
        server = !__this_strm->video_id ?  __this_strm->video_rec0 :  __this_strm->video_rec1;
        printf("%s   %d\n", __func__, __LINE__);
    } else {
        /*目前不使用*/
        printf("%s   %d\n", __func__, __LINE__);
        req.icap.path = __this->video_rec0 ? CONFIG_REC_PATH_0"IMG_****.jpg" : CONFIG_REC_PATH_1"IMG_****.jpg";
        path = __this->video_rec0 ? CONFIG_REC_PATH_0 : CONFIG_REC_PATH_1;
        req.icap.width = 1280;
        req.icap.height = 720;
        sprintf(video_dev_name, "video%d.3", __this_strm->video_id);
        server = server_open("video_server", video_dev_name);
        server_open_flag = server ? 1 : 0;
        printf("%s   %d\n", __func__, __LINE__);
    }


    printf("req.icap.width:%d\n", req.icap.width);
    if (!server) {
        printf("\nserver open err !!!\n");
        goto exit;
    }
    cap_buf = (u8 *)malloc(TAKE_PHOTO_MEM_LEN);
    if (!cap_buf) {
        printf("\n %s malloc fail\n", __func__);
        goto exit;
    }
    req.icap.buf_size = TAKE_PHOTO_MEM_LEN;
    req.icap.quality = VIDEO_MID_Q;
    req.icap.buf = cap_buf;
    req.icap.file_name = name_buf;


    req.rec.text_osd = NULL;
    req.rec.graph_osd = NULL;
    req.icap.text_label = NULL;
    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
    /* if (db_select("dat")) { */
    /* req.icap.text_label = &text_osd; */
    /* set_label_config(req.icap.width, req.icap.height, 0xe20095, req.icap.text_label); */
    /* } */


    if (callback) {
        image_len = cap_buf;
        req.icap.save_cap_buf = TRUE;//保存到cap_buff写TRUE，数据格式，前4字节为数据长度，4字节后为有效数据
    }

    err = server_request(server, VIDEO_REQ_IMAGE_CAPTURE, &req);
    if (err != 0) {
        printf("\ntake photo err\n");
        goto exit;
    }
    printf("%s   %d\n", __func__, __LINE__);

    if (callback) {
        if (image_len && cap_buf) {
            callback(cap_buf + 4, *image_len);
        } else {
            callback(NULL, 0);
        }
    } else {
#if defined CONFIG_ENABLE_VLIST
        sprintf(buf, "%s%s", path, req.icap.file_name);
        FILE_LIST_ADD(0, buf, 0);
#endif
    }
    printf("%s   %d\n", __func__, __LINE__);
    if (server_open_flag) {
        server_close(server);
    }
    printf("%s   %d\n", __func__, __LINE__);
    if (cap_buf) {
        free(cap_buf);
    }
    printf("%s   %d\n", __func__, __LINE__);
    return 0;
exit:
#if defined CONFIG_ENABLE_VLIST
    CTP_CMD_COMBINED(NULL, CTP_REQUEST, "PHOTO_CTRL", "NOTIFY", CTP_REQUEST_MSG);
#endif
    if (callback) {
        callback(NULL, 0);
    }
    if (server_open_flag) {
        server_close(server);
    }
    if (cap_buf) {
        free(cap_buf);
    }
    return -EINVAL;
}



extern int atoi(const char *);
static void rt_stream_cmd_analysis(u8 chl,  u32 add)
{
    char *key;
    char *value;
    struct rt_stream_app_info *info = (struct rt_stream_app_info *)add;

    __this_net->net_videoreq[chl].rec.width = info->width;
    __this_net->net_videoreq[chl].rec.height = info->height;
    __this_net->net_videoreq[chl].rec.format = info->type;
#ifdef __CPU_AC521x__
    __this_net->net_videoreq[chl].rec.format = 0;
#endif
    __this_net->net_videoreq[chl].rec.fps = net_video_rec_get_fps();
    __this_net->net_videoreq[chl].rec.real_fps = net_video_rec_get_fps();
    __this_net->priv = info->priv;
    printf(">>>>>>>> ch %d , info->type : %d ,w:%d , h:%d\n\n", chl, info->type, info->width, info->height);

}

static void net_video_rec_set_bitrate(unsigned int bits_rate)
{
    union video_req req = {0};

    req.rec.channel = __this_net->channel;

    req.rec.state = VIDEO_STATE_RESET_BITS_RATE;
    req.rec.abr_kbps = bits_rate;

    server_request(__this_net->net_video_rec, VIDEO_REQ_REC, &req);
}

/*码率控制，根据具体分辨率设置*/
static int net_video_rec_get_abr(u32 width)
{
    if (width <= 720) {
        return 3000;
    } else if (width <= 1280) {
        return 3000;
    } else {
        return 3000;
    }
}

static void net_rec_dev_server_event_handler(void *priv, int argc, int *argv)
{
    int mark = 2;
    struct intent it;
    /*
     *该回调函数会在录像过程中，写卡出错被当前录像APP调用，例如录像过程中突然拔卡
     */
    switch (argv[0]) {
    case VIDEO_SERVER_UVM_ERR:
        log_e("APP_UVM_DEAL_ERR\n");
        break;
    case VIDEO_SERVER_PKG_ERR:
        log_e("video_server_pkg_err\n");
        if (__this->state == VIDREC_STA_START) {
            net_video_rec_control(0);
        }
        init_intent(&it);
        it.data = &mark;
        net_video_rec_stop(1);
        break;
    case VIDEO_SERVER_PKG_END:
        if (db_select("cyc")) {
            /*video_rec_savefile((int)priv);*/
        } else {
            net_video_rec_control(0);
        }
        break;
    case VIDEO_SERVER_NET_ERR:
        log_e("\nVIDEO_SERVER_NET_ERR\n");
        init_intent(&it);
        it.data = &mark;
        net_video_rec_stop(1);
        __this_net->is_open = FALSE;
        break;
    default :
        /*log_e("unknow rec server cmd %x , %x!\n", argv[0], (int)priv);*/
        break;
    }
}

static int net_video_rec0_start()
{
    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;
    char buf[128];
    u8 res = db_select("res");
#if NET_REC_FORMAT
    if (__this->video_rec0) {
        __this_net->net_video_rec = __this->video_rec0;
    }
#endif


    if (!__this_net->net_video_rec) {
#ifdef CONFIG_PIP_ENABLE
        __this_net->net_video_rec = server_open("video_server", "video5.0");
#else
        __this_net->net_video_rec = server_open("video_server", "video1.1");
#endif
        printf(">>>>>>%s %d\n",__func__,__LINE__);
        if (!__this_net->net_video_rec) {
            printf("[DEBUG INFO] SERVER OPEN FAILD\n");
            return VREC_ERR_V0_SERVER_OPEN;
        }
        server_register_event_handler(__this_net->net_video_rec, (void *)0, net_rec_dev_server_event_handler);
    }
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    /*
     *通道号，分辨率，封装格式，写卡的路径
     */

    req.rec.channel     = 1;//用于区分任务
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;
    req.rec.width       = 1280;//__this_net->net_videoreq[0].rec.width;
    req.rec.height      = 720;//__this_net->net_videoreq[0].rec.height;
    req.rec.format      = 0;//NET_REC_FORMAT;
    printf(">>>>>>width=%d    height=%d\n\n\n\n", __this_net->net_videoreq[0].rec.width, __this_net->net_videoreq[0].rec.height);
    req.rec.state       = VIDEO_STATE_START;
    //req.rec.file        = __this->file[0];  //实时流无需写卡

    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_MID_Q;
    req.rec.fps         = 0;
    req.rec.real_fps    = net_video_rec_get_fps();

    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
    req.rec.audio.sample_rate = VIDEO_REC_AUDIO_SAMPLE_RATE;
    req.rec.audio.channel   = 1;
    req.rec.audio.volume    = 0;//__this_net->net_video0_art_on ? AUDIO_VOLUME : 0;
    req.rec.audio.buf = __this_net->audio_buf;
    req.rec.audio.buf_len = NET_AUDIO_BUF_SIZE;
    req.rec.pkg_mute.aud_mute = 1;//!db_select("mic");

    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标,写0表示1个宏块有效
     *roio_ratio : 区域比例系数
     */
    req.rec.abr_kbps = net_video_rec_get_abr(req.rec.width);

#if   defined __CPU_AC5401__
    {
        req.rec.IP_interval = 0;
    }
#elif defined __CPU_AC5601__
    if (req.rec.height > 720) {
        req.rec.IP_interval = 1;
    } else {
        req.rec.IP_interval = 0;
    }
#else
    req.rec.IP_interval = 64;//128;
#endif

    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
     *感兴趣区域qp 为其他区域的 70% ，可以调整
     */
#if 0
    req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16;
    req.rec.roi.roi1_xy = (req.rec.height * 11 / 12 / 16) << 24 | (req.rec.height * 4 / 12 / 16) << 16 | (req.rec.width * 11 / 12 / 16) << 8 | (req.rec.width) * 1 / 12 / 16;
    req.rec.roi.roi2_xy = 0;
    req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0;
    req.rec.roi.roio_ratio = 256 * 70 / 100 ;
    req.rec.roi.roio_ratio1 = 256 * 90 / 100;
    req.rec.roi.roio_ratio2 = 0;
    req.rec.roi.roio_ratio3 = 256 * 80 / 100;
#endif

    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

    text_osd.font_w = 16;
    text_osd.font_h = 32;
    max_one_line_strnum = strlen(video_rec_osd_buf);//21;
    osd_line_num = 1;
    /*   if (db_select("num")) { */
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
    /*     text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);

#if (defined __CPU_DV15__ || defined __CPU_DV17__ )

    text_osd.direction = 1;
    graph_osd.bit_mode = 16;//2bit的osd需要配置3个col|=    graph_osd.x = 0;
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = icon_16bit_data;//icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#else
    text_osd.direction = 0;
#endif

    /* if (db_select("dat")) { */
    req.rec.text_osd = &text_osd;
#if (defined __CPU_DV15__ || defined __CPU_DV17__ )
#ifdef CONFIG_OSD_LOGO
    req.rec.graph_osd = &graph_osd;
#endif
#endif
    /* } */
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    /*实时流不用*/
#if 0
    /*
     *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     */
    req.rec.slow_motion = 0;
    req.rec.tlp_time = db_select("gap");
    if (req.rec.tlp_time) {
        req.rec.real_fps = 1000 / req.rec.tlp_time;
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel   = 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }

#endif
    req.rec.buf = __this_net->net_v0_fbuf;
    req.rec.buf_len = NET_VREC0_FBUF_SIZE;

    /* req.rec.cycle_time = 3 * 60; */
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;




    printf(">>>>>>%s %d\n",__func__,__LINE__);
    struct sockaddr_in *addr = ctp_srv_get_cli_addr(__this_net->priv);
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    if (!addr) {
        addr = cdp_srv_get_cli_addr(__this_net->priv);
    }
#if (defined CONFIG_NET_UDP_ENABLE)
    sprintf(req.rec.net_par.netpath, "udp://%s:%d"
            , inet_ntoa(addr->sin_addr.s_addr)
            , _FORWARD_PORT);
#elif (defined CONFIG_NET_TCP_ENABLE)
    sprintf(req.rec.net_par.netpath, "tcp://%s:%d"
            , inet_ntoa(addr->sin_addr.s_addr)
            , _FORWARD_PORT);
#elif (defined CONFIG_NET_USR_ENABLE)
    sprintf(req.rec.net_par.netpath, "usr://%s", NET_USR_PATH);
#endif
    printf("\n @@@@@@ path = %s\n", req.rec.net_par.netpath);
    //数据外引，用于网络
    req.rec.target = VIDEO_TO_OUT;
    req.rec.out.path = req.rec.net_par.netpath;
    req.rec.out.arg  = NULL ;
    req.rec.out.open = stream_open;
    req.rec.out.send = stream_write;
    req.rec.out.close = stream_close;
    printf("__this_net->net_videoreq[0].rec.width;%d\n",__this_net->net_videoreq[0].rec.width);
    err = server_request(__this_net->net_video_rec, VIDEO_REQ_REC, &req);
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    if (err != 0) {
        puts("\n\n\nstart rec err\n\n\n");
        return VREC_ERR_V0_REQ_START;
    }
    return 0;
}



static int net_video_rec0_stop(u8 close)
{
    union video_req req = {0};
    int err;
    __this_net->net_state = VIDREC_STA_STOPING;
    if (__this_net->net_video_rec) {
        puts("\nnet video rec0 stop\n");
        net_video_rec_stop_isp_scenes();
        req.rec.channel = 1;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this_net->net_video_rec, VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nstop rec err 0x%x\n", err);
            return VREC_ERR_V0_REQ_STOP;
        }
        if (close) {
            server_close(__this_net->net_video_rec);
            __this_net->net_video_rec = NULL;
        }

    }

    __this_net->net_state = VIDREC_STA_STOP;
    net_video_rec_start_isp_scenes();

    puts("\nnet video rec0 stop end\n");

    return 0;
}

static int net_video_rec1_start()
{
    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;

    req.rec.width 	= __this_net->net_videoreq[1].rec.width;
    req.rec.height 	= __this_net->net_videoreq[1].rec.height;

#ifdef CONFIG_VIDEO1_ENABLE
    puts("start_video_rec1 \n");

#if NET_REC_FORMAT
    if (__this->video_rec1) {
        __this_net->net_video_rec1 = __this->video_rec1;
    }
#endif



    if (!__this_net->net_video_rec1) {
        __this_net->net_video_rec1 = server_open("video_server", "video1.1");
        if (!__this_net->net_video_rec1) {
            return VREC_ERR_V1_SERVER_OPEN;
        }
        server_register_event_handler(__this_net->net_video_rec1, (void *)1, net_rec_dev_server_event_handler);
    }
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;
    req.rec.file    = __this->file[1];
    req.rec.IP_interval = 0;
    __this_net->video_id = 1;
#endif

#ifdef CONFIG_VIDEO3_ENABLE
    char name[12];
    void *uvc_fd;
#if NET_REC_FORMAT
    if (__this->video_rec3) {
        __this_net->net_video_rec1 = __this->video_rec3;
    }
#endif

    struct uvc_capability uvc_cap;

    puts("start_video_rec3 \n");
    if (!__this_net->net_video_rec1) {
        sprintf(name, "video3.%d", __this->uvc_id);
        __this_net->net_video_rec1 = server_open("video_server", name);
        if (!__this_net->net_video_rec1) {
            return -EINVAL;
        }
        server_register_event_handler(__this_net->net_video_rec1, (void *)1, net_rec_dev_server_event_handler);
    }
    req.rec.camera_type = VIDEO_CAMERA_UVC;
    req.rec.three_way_type = 0;
    req.rec.IP_interval = 0;
    /* req.rec.width 	= UVC_ENC_WIDTH; */
    /* req.rec.height 	= UVC_ENC_HEIGH; */
    __this->uvc_width = req.rec.width;
    __this->uvc_height = req.rec.height;
    req.rec.uvc_id = __this->uvc_id;
    req.rec.file    = __this->file[2];
    __this_net->video_id = 1;
#endif
    /*
     *通道号，分辨率，封装格式，写卡的路径
     */
    req.rec.channel = __this_net->channel = 1;
    req.rec.format  = NET_REC_FORMAT;
    req.rec.state 	= VIDEO_STATE_START;

    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_MID_Q;
    req.rec.fps         = 0;
    req.rec.real_fps    = net_video_rec_get_fps();

    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
    req.rec.audio.sample_rate = VIDEO_REC_AUDIO_SAMPLE_RATE;
    req.rec.audio.channel   = 1;
    req.rec.audio.volume    = __this_net->net_video1_art_on ? AUDIO_VOLUME : 0;
    req.rec.audio.buf = __this_net->audio_buf;
    req.rec.audio.buf_len = NET_AUDIO_BUF_SIZE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");

    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标,写0表示1个宏块有效
     *roio_ratio : 区域比例系数
     */
    req.rec.abr_kbps = net_video_rec_get_abr(req.rec.width);

#if   defined __CPU_AC5401__
    {
        req.rec.IP_interval = 0;
    }
#elif defined __CPU_AC5601__
    if (req.rec.height > 720) {
        req.rec.IP_interval = 1;
    } else {
        req.rec.IP_interval = 0;
    }
#else
    req.rec.IP_interval = 64;//128;
#endif

    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
     *感兴趣区域qp 为其他区域的 70% ，可以调整
     */
#if 0
    req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16;
    req.rec.roi.roi1_xy = (req.rec.height * 11 / 12 / 16) << 24 | (req.rec.height * 4 / 12 / 16) << 16 | (req.rec.width * 11 / 12 / 16) << 8 | (req.rec.width) * 1 / 12 / 16;
    req.rec.roi.roi2_xy = 0;
    req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0;
    req.rec.roi.roio_ratio = 256 * 70 / 100 ;
    req.rec.roi.roio_ratio1 = 256 * 90 / 100;
    req.rec.roi.roio_ratio2 = 0;
    req.rec.roi.roio_ratio3 = 256 * 80 / 100;
#endif


    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

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
    /*     text_osd.color[0] = 0xe20095; */
    /* text_osd.bit_mode = 1; */
    text_osd.color[0] = 0x057d88;
    text_osd.color[1] = 0xe20095;
    text_osd.color[2] = 0xe20095;
    text_osd.bit_mode = 2;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
    /*     text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);

#if (defined __CPU_DV15__ || defined __CPU_DV17__ )

    text_osd.direction = 1;
    graph_osd.bit_mode = 16;//2bit的osd需要配置3个col|=    graph_osd.x = 0;
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = icon_16bit_data;//icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#else
    text_osd.direction = 0;
#endif

    if (db_select("dat")) {
        req.rec.text_osd = &text_osd;
#if (defined __CPU_DV15__ || defined __CPU_DV17__ )
#ifdef CONFIG_OSD_LOGO
        req.rec.graph_osd = &graph_osd;
#endif
#endif
    }

    /*实时流不用*/
#if 0
    /*
     *      *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     *           */
    req.rec.slow_motion = 0;
    req.rec.tlp_time = db_select("gap");
    if (req.rec.tlp_time) {
        req.rec.real_fps = 1000 / req.rec.tlp_time;
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel   = 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }

#endif
    req.rec.buf = __this_net->net_v0_fbuf;
    req.rec.buf_len = NET_VREC0_FBUF_SIZE;

    /* req.rec.cycle_time = 3 * 60; */
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;



    struct sockaddr_in *addr = ctp_srv_get_cli_addr(__this_net->priv);
    if (!addr) {
        addr = cdp_srv_get_cli_addr(__this_net->priv);
    }
#if (defined CONFIG_NET_UDP_ENABLE)
    sprintf(req.rec.net_par.netpath, "udp://%s:%d"
            , inet_ntoa(addr->sin_addr.s_addr)
            , _BEHIND_PORT);
#elif (defined CONFIG_NET_TCP_ENABLE)
    sprintf(req.rec.net_par.netpath, "tcp://%s:%d"
            , inet_ntoa(addr->sin_addr.s_addr)
            , _BEHIND_PORT);
#elif (defined CONFIG_NET_USR_ENABLE)
    sprintf(req.rec.net_par.netpath, "usr://%s", NET_USR_PATH);
#endif
    printf("\n @@@@@@ path = %s\n", req.rec.net_par.netpath);
    //数据外引，用于网络
    req.rec.target = VIDEO_TO_OUT;
    req.rec.out.path = req.rec.net_par.netpath;
    req.rec.out.arg  = NULL ;
    req.rec.out.open = stream_open;
    req.rec.out.send = stream_write;
    req.rec.out.close = stream_close;

    err = server_request(__this_net->net_video_rec1, VIDEO_REQ_REC, &req);
    if (err != 0) {
        puts("\n\n\nstart rec2 err\n\n\n");
        return VREC_ERR_V1_SERVER_OPEN;
    }

    return 0;
}



static int net_video_rec1_stop(u8 close)
{
    union video_req req = {0};
    int err;

#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
    __this_net->net_state1 = VIDREC_STA_STOPING;
    net_video_rec_stop_isp_scenes();
    if (__this_net->net_video_rec1) {
        req.rec.channel = 1;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(__this_net->net_video_rec1, VIDEO_REQ_REC, &req);

        if (err != 0) {
            printf("\nstop rec2 err 0x%x\n", err);
            return VREC_ERR_V1_REQ_STOP;
        }
        if (close) {
            server_close(__this_net->net_video_rec1);
            __this_net->net_video_rec1 = NULL;
        }
    }

    puts("net_video_rec1_stop\n");
    net_video_rec_start_isp_scenes();
    __this_net->net_state1 = VIDREC_STA_STOP;
    __this_net->video_id = 0;
#endif

    return 0;
}

static void net_video_rec_free_buf(void)
{
    if (__this_net->net_v0_fbuf) {
        free(__this_net->net_v0_fbuf);
        __this_net->net_v0_fbuf = NULL;
    }
    if (__this_net->audio_buf) {
        free(__this_net->audio_buf);
        __this_net->audio_buf = NULL;
    }
}

static int net_video0_rec_start(struct intent *it)
{
    int err = 0;
#if 1//def CONFIG_VIDEO0_ENABLE
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    __this_net->net_state = VIDREC_STA_STARTING;
    if (!__this_net->net_v0_fbuf) {
        __this_net->net_v0_fbuf = malloc(NET_VREC0_FBUF_SIZE);
        if (!__this_net->net_v0_fbuf) {
            puts("malloc v0_buf err\n\n");
            return -1;
        }
    }
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    if (!__this_net->audio_buf) {
        __this_net->audio_buf = malloc(NET_AUDIO_BUF_SIZE);
        if (!__this_net->audio_buf) {
            free(__this_net->net_v0_fbuf);
            return -1;
        }
    }
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    err = net_video_rec0_start();
    if (err) {
        goto __start_err0;
    }
    printf(">>>>>>%s %d\n",__func__,__LINE__);
    if (__this->state != VIDREC_STA_START) {
        __this_net->videoram_mark = 1;
    } else {
        __this_net->videoram_mark = 0;
    }
    __this_net->net_state = VIDREC_STA_START;
//    net_video_rec_start_isp_scenes();

    return 0;

__start_err0:
    puts("\nstart net_video_rec0 err\n");
    err = net_video_rec0_stop(0);
    if (err) {
        printf("\nstop net_video_rec0 wrong0 %x\n", err);
    }

#endif
    return -1;
}

static int net_video0_rec_stop(u8 close)
{
    int err;
#if 1//def CONFIG_VIDEO0_ENABLE
    __this_net->net_state = VIDREC_STA_STOPING;
    err = net_video_rec0_stop(close);
    if (err) {
        puts("\n net stop0 err\n");
    }
    __this_net->net_state = VIDREC_STA_STOP;
#endif

    return err;
}

static int net_video1_rec_start(struct intent *it)
{
    int err = 0;

#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)


    if (!__this_net->net_v0_fbuf) {
        __this_net->net_v0_fbuf = malloc(NET_VREC0_FBUF_SIZE);

        if (!__this_net->net_v0_fbuf) {
            puts("malloc v1_buf err\n\n");
            return -1;
        }
    }
    if (!__this_net->audio_buf) {
        __this_net->audio_buf = malloc(NET_AUDIO_BUF_SIZE);

        if (!__this_net->audio_buf) {
            free(__this_net->net_v0_fbuf);
            return -1;
        }
    }
    if (__this->video_online[1] || __this->video_online[3]) {
        __this_net->net_state1 = VIDREC_STA_STARTING;
        err = net_video_rec1_start();
        if (err) {
            goto __start_err1;
        }

        __this_net->net_state1 = VIDREC_STA_START;
        net_video_rec_start_isp_scenes();
    }

    return 0;

__start_err1:
    puts("\nstart net_video_rec1 err\n");
    err = net_video_rec1_stop(0);
    if (err) {
        printf("\nstop net_video_rec1 wrong1 %x\n", err);
    }
#endif

    return -1;
}

static int net_video_rec_start(u8 mark)
{
    int err;
    if (!__this_net->is_open) {
        return 0;
    }
    puts("start net rec\n");
    if (!__this_net->net_v0_fbuf) {
        __this_net->net_v0_fbuf = malloc(NET_VREC0_FBUF_SIZE);

        if (!__this_net->net_v0_fbuf) {
            puts("malloc v0_buf err\n\n");
            return -1;
        }

    }
    if (!__this_net->audio_buf) {
        __this_net->audio_buf = malloc(NET_AUDIO_BUF_SIZE);

        if (!__this_net->audio_buf) {
            free(__this_net->net_v0_fbuf);
            return -ENOMEM;
        }
    }

#if 1//def CONFIG_VIDEO0_ENABLE
    printf("\n art %d, vrt %d\n", __this_net->net_video0_art_on, __this_net->net_video0_vrt_on);
    if ((__this_net->net_video0_art_on || __this_net->net_video0_vrt_on)
        && (__this_net->net_state != VIDREC_STA_START)) {
        puts("\nnet video rec0 start \n");
        err = net_video_rec0_start();
        if (err) {
            goto __start_err0;
        }
        __this_net->net_state = VIDREC_STA_START;
    }

#endif

#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)

    if ((__this_net->net_video1_art_on || __this_net->net_video1_vrt_on)
        && (__this_net->net_state1 != VIDREC_STA_START))	{
        if (__this->video_online[1] || __this->video_online[3]) {
            puts("\nnet video rec1 start \n");
            err = net_video_rec1_start();
            if (err) {
                goto __start_err1;
            }
            __this_net->net_state1 = VIDREC_STA_START;
        }
    }

#endif

    net_video_rec_start_isp_scenes();
    return 0;
#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
__start_err1:
    puts("\nstart1 err\n");
    err = net_video_rec1_stop(0);

    if (err) {
        printf("\nstart wrong1 %x\n", err);
    }

#endif
#if 1//def CONFIG_VIDEO0_ENABLE
__start_err0:
    puts("\nstart0 err\n");
    err = net_video_rec0_stop(0);

    if (err) {
        printf("\nstart wrong0 %x\n", err);
    }

#endif
    return -EFAULT;
}




static int net_video_rec_stop(u8 close)
{
    int err = 0;
    if (!__this_net->is_open) {
        return 0;
    }
#if 1//def CONFIG_VIDEO0_ENABLE
    puts("\n net_video_rec_stop. 0.. \n");
#if !(defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
    if (__this_net->videoram_mark != 1 && close == 0) {
        puts("\n video ram mark\n");
        return 0;
    }
#endif
    if (__this_net->net_state == VIDREC_STA_START) {
        __this_net->net_state = VIDREC_STA_STOPING;
        err = net_video_rec0_stop(close);
        if (err) {
            puts("\n net stop0 err\n");
        }
    }
#endif

#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
    if (__this_net->net_state1 == VIDREC_STA_START) {
        __this_net->net_state1 = VIDREC_STA_STOPING;
        err = net_video_rec1_stop(close);
        if (err) {
            puts("\n net stop1 err\n");
        }
    }
#endif

    return err;
}


static int  net_rt_video0_open(struct intent *it)
{
    int ret = 0;
    if (__this_net->is_open) {
        return 0;
    }
    __this_net->is_open = TRUE;
#if 1//def CONFIG_VIDEO0_ENABLE
    if (it) {
        u8 mark = *((u8 *)it->data);
        __this_net->net_video0_art_on = (mark | (mark >> 1)) & 0x01;
        __this_net->net_video0_vrt_on = (mark >> 1) & 0x01 ;
        rt_stream_cmd_analysis(0, it->exdata);
    } else {
        if (__this_net->net_video0_art_on == 0 && __this_net->net_video0_vrt_on == 0) {
            goto exit;
        }
    }
    if (__this_net->net_state == VIDREC_STA_STOP || __this_net->net_state == VIDREC_STA_IDLE) {
        puts("\nnet rt video0 open \n");
        ret = net_video0_rec_start(it);
        if (ret) {
            __this_net->is_open = FALSE;//启动失败允许重新打开
        }
    }
#endif
exit:
    return ret;
}


static int  net_rt_video0_stop(struct intent *it)
{
    int ret = 0;
    if (!__this_net->is_open) {
        return 0;
    }
    __this_net->is_open = FALSE;
#if 1//def CONFIG_VIDEO0_ENABLE
    u8 mark = *((u8 *)it->data);
    __this_net->net_video0_art_on = 0;
    __this_net->net_video0_vrt_on = 0;
    if (__this_net->net_state == VIDREC_STA_START) {
        ret = net_video0_rec_stop(0);
        if (ret) {
            __this_net->is_open = TRUE;
            printf("net_video_rec0_stop fail \n\n");
        } else {
            printf("net_video_rec0_stop suc \n\n");
        }
    }
#endif
    return ret;
}

static int net_rt_video1_open(struct intent *it)
{

    int ret = 0;
    if (__this_net->is_open) {
        return 0;
    }
    __this_net->is_open = TRUE;
#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
    if (it) {
        u8 mark = *((u8 *)it->data);
        __this_net->net_video1_art_on = (mark | (mark >> 1)) & 0x01;
        __this_net->net_video1_vrt_on = (mark >> 1) & 0x01 ;
        rt_stream_cmd_analysis(1, it->exdata);
    } else {
        if (__this_net->net_video1_art_on == 0 && __this_net->net_video1_vrt_on == 0) {
            puts("\nvideo1 rt not open \n");
            goto exit;
        }
    }
    if (__this_net->net_state1 == VIDREC_STA_STOP || __this_net->net_state1 == VIDREC_STA_IDLE) {
        puts("\n net rt video1 open\n");
        ret = net_video1_rec_start(it);
        if (ret) {
            __this_net->is_open = FALSE;//启动失败允许重新打开
        }
    }
#endif
exit:
    return ret;
}

static int  net_rt_video1_stop(struct intent *it)
{
    int ret = 0;
    if (!__this_net->is_open) {
        return 0;
    }
    __this_net->is_open = FALSE;
#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
    u8 mark = *((u8 *)it->data);
    __this_net->net_video1_art_on = 0;
    __this_net->net_video1_vrt_on = 0 ;
    if (__this_net->net_state1 == VIDREC_STA_START) {
        ret = net_video_rec1_stop(0);
        if (ret) {
            __this_net->is_open = TRUE;
            printf("net_video_rec1_stop fail \n\n");
        } else {
            printf("net_video_rec1_stop suc \n\n");
        }
    }
#endif
    return  ret;
}


/*
 * 录像app的录像控制入口, 根据当前状态调用相应的函数
 */
static int net_video_rec_control(void *_run_cmd)
{
    int err = 0;
    u32 clust;
    int run_cmd = (int)_run_cmd;
    struct vfs_partition *part;
    if (storage_device_ready() == 0) {
        if (!dev_online(SDX_DEV)) {
            net_video_rec_post_msg("noCard");
        } else {
            net_video_rec_post_msg("fsErr");
        }
        CTP_CMD_COMBINED(NULL, CTP_SDCARD, "VIDEO_CTRL", "NOTIFY", CTP_SDCARD_MSG);
        return 0;
    } else {
        part = fget_partition(CONFIG_ROOT_PATH);

        __this->total_size = part->total_size;

        if (part->clust_size < 32 || (part->fs_attr & F_ATTR_RO)) {
            net_video_rec_post_msg("fsErr");
            CTP_CMD_COMBINED(NULL, CTP_SDCARD, "VIDEO_CTRL", "NOTIFY", CTP_SDCARD_MSG);
            return 0;
        }
    }
    switch (__this->state) {
    case VIDREC_STA_IDLE:
    case VIDREC_STA_STOP:
        if (run_cmd) {
            break;
        }
        __this_net->video_rec_err = FALSE;//用在录像IMC打不开情况下
        printf("--NET_VIDEO_STOP\n");
#if 1//defined CONFIG_VIDEO0_ENABLE && (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
#if NET_REC_FORMAT
        NET_VIDEO_ERR(strm_video_rec_close());
        NET_VIDEO_ERR(net_video_rec_stop(0));
        __this_net->fbuf_fcnt = 0;
        __this_net->fbuf_ffil = 0;
#endif
#endif
        err = video_rec_control_start();
        if (err == 0) {
            if (__this->gsen_lock == 1) {
                net_video_rec_post_msg("lockREC");
            }
        }
        NET_VIDEO_ERR(err);
#if 1//defined CONFIG_VIDEO0_ENABLE && (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
#if NET_REC_FORMAT
        NET_VIDEO_ERR(strm_video_rec_open());
        NET_VIDEO_ERR(net_video_rec_start(1));
#endif
#endif
        net_video_rec_status_notify();
        printf("--NET_VIDEO_OPEN OK\n");

        break;
    case VIDREC_STA_START:
        if (run_cmd == 0) {
            printf("--NET_VIDEO_STOP\n");
#if 1//defined CONFIG_VIDEO0_ENABLE && (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
#if NET_REC_FORMAT
            NET_VIDEO_ERR(strm_video_rec_close());
            NET_VIDEO_ERR(net_video_rec_stop(0));
            __this_net->fbuf_fcnt = 0;
            __this_net->fbuf_ffil = 0;
#endif
#endif
            err = video_rec_control_doing();
            NET_VIDEO_ERR(err);
#if defined CONFIG_VIDEO0_ENABLE && (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)
#if NET_REC_FORMAT
            NET_VIDEO_ERR(strm_video_rec_open());
            NET_VIDEO_ERR(net_video_rec_start(1));
#endif
#endif
            printf("--NET_VIDEO_OPEN OK\n");
        }
        net_video_rec_status_notify();
        break;
    default:
        puts("\nvrec forbid\n");
        err = 1;
        break;
    }

    return err;
}

/*
 *录像的状态机,进入录像app后就是跑这里
 */
static int net_video_rec_state_machine(struct application *app, enum app_state state, struct intent *it)
{
    int err = 0;
    int len;
    char buf[128];

    switch (state) {
    case APP_STA_START:
        if (!it) {
            break;
        }
        switch (it->action) {
        case ACTION_VIDEO_DEC_MAIN:
            break;
        case ACTION_VIDEO_TAKE_PHOTO:
            printf("----ACTION_VIDEO_TAKE_PHOTO----\n\n");
            net_video_rec_take_photo(NULL);
            break;
        case ACTION_VIDEO_REC_CONCTRL:
            printf("----ACTION_VIDEO_REC_CONCTRL-----\n\n");
            err = net_video_rec_control(0);
            break;

        case ACTION_VIDEO_REC_GET_APP_STATUS:
            printf("----ACTION_VIDEO_REC_GET_APP_STATUS-----\n\n");
            video_rec_get_app_status(it);
            break;

        case ACTION_VIDEO_REC_GET_PATH:
            /*printf("----ACTION_VIDEO_REC_GET_PATHL-----\n\n");*/
            video_rec_get_path(it);
        case ACTION_VIDEO0_OPEN_RT_STREAM:

            printf("----ACTION_VIDEO0_OPEN_RT_STREAM-----\n\n");
            __this_net->fbuf_fcnt = 0;
            __this_net->fbuf_ffil = 0;



            err = net_rt_video0_open(it);
            sprintf(buf, "format:%d,w:%d,h:%d,fps:%d,rate:%d"
                    , __this_net->net_videoreq[0].rec.format
                    , __this_net->net_videoreq[0].rec.width
                    , __this_net->net_videoreq[0].rec.height
                    , __this_net->net_videoreq[0].rec.real_fps
                    , VIDEO_REC_AUDIO_SAMPLE_RATE);
            printf("<<<<<<< : %s\n\n\n\n\n", buf);
            if (err) {
                printf("ACTION_VIDEO0_OPEN_RT_STREAM err!!!\n\n");
                CTP_CMD_COMBINED(NULL, CTP_RT_OPEN_FAIL, "OPEN_RT_STREAM", "NOTIFY", CTP_RT_OPEN_FAIL_MSG);
            } else {
                if (CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "OPEN_RT_STREAM", "NOTIFY", buf)) {
                    CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "OPEN_RT_STREAM", "NOTIFY", buf);
                }
                printf("CTP NOTIFY VIDEO0 OK\n\n");
            }
            break;

        case ACTION_VIDEO1_OPEN_RT_STREAM:
            printf("----ACTION_VIDEO1_OPEN_RT_STREAM----\n\n");
            __this_net->fbuf_fcnt = 0;
            __this_net->fbuf_ffil = 0;
            err = net_rt_video1_open(it);
            sprintf(buf, "format:%d,w:%d,h:%d,fps:%d,rate:%d"
                    , __this_net->net_videoreq[1].rec.format
                    , __this_net->net_videoreq[1].rec.width
                    , __this_net->net_videoreq[1].rec.height
                    , __this_net->net_videoreq[1].rec.real_fps
                    , VIDEO_REC_AUDIO_SAMPLE_RATE);
            printf("<<<<<<< : %s\n\n\n\n\n", buf);
            if (err) {
                printf("ACTION_VIDEO1_OPEN_RT_STREAM err!!!\n\n");
                CTP_CMD_COMBINED(NULL, CTP_RT_OPEN_FAIL, "OPEN_PULL_RT_STREAM", "NOTIFY", CTP_RT_OPEN_FAIL_MSG);
            } else {
                CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "OPEN_PULL_RT_STREAM", "NOTIFY", buf);
                printf("CTP NOTIFY VIDEO1 OK\n\n");
            }
            break;

        case ACTION_VIDEO0_CLOSE_RT_STREAM:
            printf("---ACTION_VIDEO0_CLOSE_RT_STREAM---\n\n");
            err = net_rt_video0_stop(it);
            if (err) {
                printf("ACTION_VIDEO_CLOE_RT_STREAM err!!!\n\n");
                strcpy(buf, "status:0");
            } else {
                strcpy(buf, "status:1");
            }
            CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "CLOSE_RT_STREAM", "NOTIFY", buf);
            printf("CTP NOTIFY VIDEO0 OK\n\n");
            __this_net->fbuf_fcnt = 0;
            __this_net->fbuf_ffil = 0;
            break;

        case ACTION_VIDEO1_CLOSE_RT_STREAM:
            printf("---ACTION_VIDEO1_CLOSE_RT_STREAM---\n\n");
            err =  net_rt_video1_stop(it);
            if (err) {
                printf("ACTION_VIDE1_CLOE_RT_STREAM err!!!\n\n");
                strcpy(buf, "status:0");
            } else {
                strcpy(buf, "status:1");
            }

            CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "CLOSE_PULL_RT_STREAM", "NOTIFY", buf);
            printf("CTP NOTIFY VIDEO1 OK\n\n");
            __this_net->fbuf_fcnt = 0;
            __this_net->fbuf_ffil = 0;
            break;
        case ACTION_VIDEO_CYC_SAVEFILE:
#if 0
            video_cyc_file(0);
#if defined CONFIG_VIDEO1_ENABLE
            video_cyc_file(1);
#endif
#if defined CONFIG_VIDEO3_ENABLE
            video_cyc_file(2);
#endif
#endif
            strcpy(buf, "status:1");
            CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "VIDEO_CYC_SAVEFILE", "NOTIFY", buf);
            break;

#ifdef CONFIG_NET_SCR
        case ACTION_NET_SCR_REC_OPEN:
            printf("%s ACTION_NET_SCR_REC_OPEN\n", __func__);
            extern int net_video_disp_stop(int id);
            extern int net_hide_main_ui(void);
            net_video_disp_stop(0);
            net_hide_main_ui();
            break;

        case ACTION_NET_SCR_REC_CLOSE:
            printf("%s ACTION_NET_SCR_REC_CLOSE\n", __func__);
            extern int net_video_disp_start(int id);
            extern int net_show_main_ui(void);
            net_video_disp_start(0);
            net_show_main_ui();
            break;
#endif
        }

        break;
    case APP_STA_STOP:
        puts("\n[MSG] net_video_rec APP_STA_STOP\n");
        break;
    case APP_STA_DESTROY:
        puts("\n[MSG] net_video_rec APP_STA_DESTROY\n");
        break;

    }

    return err;
}

static void net_video_rec_ioctl(u32 argv)
{
    char buf[128];
    u32 *pargv = (u32 *)argv;
    u32 type = (u32)pargv[0];
    char *path = (char *)pargv[1];

    /*printf("%s type : %d , %s \n\n", __func__, type, path);*/
    switch (type) {
    case NET_VIDREC_STA_STOP:
        puts("\n NET_VIDREC_STA_STOP\n");
        if (__this_net->net_state == VIDREC_STA_START || __this_net->net_state1 == VIDREC_STA_START) {
            net_video_rec_stop(1);
        }
        if (__this_strm->state == VIDREC_STA_START) {
            printf("\n strm_video_rec_close 1\n");
            extern int strm_video_rec_close2(void);
            strm_video_rec_close2();
            printf("\n strm_video_rec_close 2\n");
        }
        net_video_rec_free_buf();
        extern void strm_video_rec_free_buf(void);
        strm_video_rec_free_buf();
        __this_net->is_open = FALSE;
        __this_strm->is_open = FALSE;
        break;
    case NET_VIDREC_STATE_NOTIFY:
        sprintf(buf, "status:%d", ((__this->state == VIDREC_STA_START) ? 1 : 0));
        CTP_CMD_COMBINED(NULL, CTP_NO_ERR, "VIDEO_CTRL", "NOTIFY", buf);
        break;
    default :
        return ;
    }
}

static int net_video_rec_device_event_handler(struct sys_event *event)
{
    int err;
    struct intent it;

    if (!ASCII_StrCmp(event->arg, "sd*", 4)) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:
            video_rec_sd_in_notify();
            break;
        case DEVICE_EVENT_OUT:
            if (!fdir_exist(CONFIG_STORAGE_PATH)) {
                video_rec_sd_out_notify();
            }
            break;
        }
    } else if (!ASCII_StrCmp(event->arg, "sys_power", 7)) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_POWER_CHARGER_IN:
            puts("---charger in\n\n");// 输出充电器插入事件日志
        // 如果当前状态是空闲或停止，启动视频录制
            if ((__this->state == VIDREC_STA_IDLE) ||
                (__this->state == VIDREC_STA_STOP)) {
                video_rec_control_start();// 启动视频录制
            }
            break;
        case DEVICE_EVENT_POWER_CHARGER_OUT:
            puts("---charger out\n");// 输出充电器拔出事件日志
        // 可以在此处添加处理充电器拔出事件的逻辑
#if 0
            if (__this->state == VIDREC_STA_START) {
                video_rec_control_doing();
            }
#endif
            break;
        }
    }
    return 0;
}
static void net_video_server_task(void *p)
{
    int res;
    int msg[16];

    if (os_mutex_create(&net_vdrec_mutex) != OS_NO_ERR) {
        printf("net_video_server_task , os_mutex_create err !!!\n\n");
        return;
    }
    net_video_handler_init();
    printf("net_video_server_task running\n\n");

    while (1) {

        res = os_task_pend("taskq", msg, ARRAY_SIZE(msg));
        if (os_task_del_req(OS_TASK_SELF) == OS_TASK_DEL_REQ) {
            os_task_del_res(OS_TASK_SELF);
        }

        switch (res) {
        case OS_TASKQ:
            switch (msg[0]) {
            case Q_EVENT:
                break;
            case Q_MSG:
                net_video_rec_ioctl((u32)msg[1]);
                break;
            default:
                break;
            }
            break;
        case OS_TIMER:
            break;
        case OS_TIMEOUT:
            break;
        }
    }
    os_mutex_del(&net_vdrec_mutex, OS_DEL_ALWAYS);
}

void net_video_server_init(void)
{
    task_create(net_video_server_task, 0, "net_video_server");
}
__initcall(net_video_server_init);


/*录像app的事件总入口*/
static int net_video_rec_event_handler(struct application *app, struct sys_event *event)
{
    switch (event->type) {
    case SYS_DEVICE_EVENT:
        return video_rec_device_event_action(event);//设备事件和 vidoe_rec公用一个handler，
    default:
        return false;
    }
}

static const struct application_operation net_video_rec_ops = {
    .state_machine  = net_video_rec_state_machine,
    .event_handler 	= net_video_rec_event_handler,
};

REGISTER_APPLICATION(app_video_rec) = {
    .name 	= "net_video_rec",
    .action	= ACTION_VIDEO_REC_MAIN,
    .ops 	= &net_video_rec_ops,
    .state  = APP_STA_DESTROY,
};



extern int get_videox_rec_state(u8 uvc_id);
extern void *get_videox_rec_server(u8 uvc_id);
extern void *get_usb_uvc_hdl();
extern int ispt_params_flush();
extern int ispt_params_set_smooth_step(int step);
extern void *get_uvc_camera_device(void);

#define UVC0_REC_FORMAT    1 //0 表示AVI 1表示H264
#define UVC1_REC_FORMAT    1 //0 表示AVI 1表示H264
#define UVC2_REC_FORMAT    1 //0 表示AVI 1表示H264
#define UVC3_REC_FORMAT    1 //0 表示AVI 1表示H264


static void *video0_server  = NULL;
static void *video0_file = NULL;
static int net_video0_state;

static void *uvc_video_server[4];
static void *uvc_video_file[4];
static u8 uvc_video_format[4] = {UVC0_REC_FORMAT, UVC1_REC_FORMAT, UVC2_REC_FORMAT, UVC3_REC_FORMAT};
static int uvc_video_state[4];


int user_net_video3_recx_start(u8 uvc_id, u8 is_stream);


int video3_rec_get_iframe(u8 uvc_id)
{
    //通过UVC扩展单元请求发送强制I针指令
}

int video0_rec_get_iframe(void)
{
    union video_req req = {0};
    void *server  = get_videox_rec_server(-1);
    if (!server) {
        return -1;
    }
    req.rec.channel = 1;
    req.rec.buf = NULL; //如果想要I帧数据，malloc buf
    req.rec.buf_len = 0;
    req.rec.state   = VIDEO_STATE_CAP_IFRAME;
    req.rec.format = VIDEO_FMT_MOV;
    return server_request(server, VIDEO_REQ_REC, &req);
}

u8 get_video_rec_state(void)
{
    rec_handler  = (struct video_rec_hdl *)get_video_rec_handler();
    return __this->state == VIDREC_STA_START;
}

int ircut_get_isp_scenes_flag(void)
{
    if (__this->video_rec0 && __this->state == VIDREC_STA_START && isp_scenes_switch_timer_done()) {

        return 0;
    }
    if (get_videox_rec_server(-1) && get_videox_rec_state(0) == VIDREC_STA_START && isp_scenes_switch_timer_done()) {

        return 0;
    }

    if (__this_strm->video_rec0 && __this_strm->state == VIDREC_STA_START && isp_scenes_switch_timer_done()) {

        return 0;
    }

    if (get_uvc_camera_device() && isp_scenes_switch_timer_done()) {

        return 0;
    }


    return -1;
}

void osd_display_cur_scenec(int value)
{
    video_rec_osd_buf[29] = value + '0';
}

int ircut_set_isp_scenes(u8 on, char *buf, int size)
{

    void *server = NULL;
    struct ispt_customize_cfg cfg = {0};
    union video_req req = {0};
    int err;

    if (get_uvc_camera_device()) {
        server = get_usb_uvc_hdl();
    } else if (get_videox_rec_server(-1)) {
        server = get_videox_rec_server(-1);
    } else if (__this->video_rec0) {
        server = __this->video_rec0;
    } else if (__this_strm->video_rec0) {
        server = __this_strm->video_rec0;
    }

    if (on) {
#ifdef CONFIG_AUTO_ISP_SCENES_ENABLE
        stop_update_isp_scenes();
#endif
        ispt_params_flush();
        ispt_params_set_smooth_step(1);
        cfg.mode = ISP_CUSTOMIZE_MODE_FILE;
        cfg.data = buf;
        cfg.len =  size;

        req.camera.cmd = SET_CUSTOMIZE_CFG;
        req.camera.customize_cfg = &cfg;
        err = server_request(server, VIDEO_REQ_CAMERA_EFFECT, &req);
#ifdef CONFIG_OSD_DISPLAY_CUR_SENCE
        extern void osd_display_cur_scenec(int value);
        osd_display_cur_scenec(4);
#endif
    } else {
        stop_update_isp_scenes();
        ispt_params_flush();
        ispt_params_set_smooth_step(1);
        cfg.mode = ISP_CUSTOMIZE_MODE_FILE;
        cfg.data = buf;
        cfg.len =  size;
        req.camera.cmd = SET_CUSTOMIZE_CFG;
        req.camera.customize_cfg = &cfg;
        err = server_request(server, VIDEO_REQ_CAMERA_EFFECT, &req);
        if (!err) {
#ifdef CONFIG_OSD_DISPLAY_CUR_SENCE
            extern void osd_display_cur_scenec(int value);
            osd_display_cur_scenec(2);
#endif // CONFIG_OSD_DISPLAY_CUR_SENCE
#ifdef CONFIG_AUTO_ISP_SCENES_ENABLE
            start_update_isp_scenes(server);
#endif // CONFIG_AUTO_ISP_SCENES_ENABLE
        }




    }
    return err;
}



int net_video_recx_take_photo(u8 uvc_id, u8 *buffer, u32 buffer_len)
{
    struct server *server = NULL;
    union video_req req = {0};
    int err;
    /*不开实时流不能拍照*/
    int retry = 100;

    do {
        if (get_videox_rec_state(uvc_id) == VIDREC_STA_START) {
            break;
        }
        msleep(10);
    } while (--retry);
    if (retry == 0 && get_videox_rec_state(uvc_id) != VIDREC_STA_START) {
        goto exit;
    }

    server = get_videox_rec_server(uvc_id);



    req.icap.width = 320;
    req.icap.height = 240;
    req.icap.buf_size = buffer_len;
    printf("\n req.icap.buf_size = %d\n", req.icap.buf_size);
    req.icap.quality = VIDEO_MID_Q;
    req.icap.buf = buffer;
    req.rec.text_osd = NULL;
    req.rec.graph_osd = NULL;
    req.icap.text_label = NULL;
    req.icap.save_cap_buf = TRUE;//保存到cap_buff写TRUE，数据格式，前4字节为数据长度，4字节后为有效数据
    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
    /* if (db_select("dat")) { */
    /* req.icap.text_label = &text_osd; */
    /* set_label_config(req.icap.width, req.icap.height, 0xe20095, req.icap.text_label); */
    /* } */

    err = server_request(server, VIDEO_REQ_IMAGE_CAPTURE, &req);
    if (err != 0) {
        printf("\ntake photo err\n");
        goto exit;
    }


    err = server_request(server, VIDEO_REQ_GET_IMAGE, &req);

    return req.image.size;

exit:
    return 0;
}





int video3_norec_take_photo_save_file(u8 uvc_id)
{
    static struct server *server = NULL;
    union video_req req = {0};
    int err;
    int buffer_len = 1 * 1024 * 1024;
    u8 *buffer = calloc(1, buffer_len);
    if (!buffer) {
        goto exit;
    }

    if (!uvc_video_server[uvc_id]) {
        static char dev_name[20];
        //uvc_id = 0时，对应3.1
        //uvc_id = 1时，对应3.6
        //uvc_id = 2时，对应3.8
        //uvc_id = 3时，对应3.A
        sprintf(dev_name, "video3.%X", uvc_id * 5 + 1);
        if (uvc_id >= 2) {
            sprintf(dev_name, "video3.%X", uvc_id + 7);
        }
        printf("\n dev_name = %s\n", dev_name);
        uvc_video_server[uvc_id] = server_open("video_server", dev_name);
    }

    req.icap.width = 1280;
    req.icap.height = 720;
    req.icap.buf_size = buffer_len;
    printf("\n req.icap.buf_size = %d\n", req.icap.buf_size);
    req.icap.quality = VIDEO_MID_Q;
    req.icap.buf = buffer;
    req.rec.text_osd = NULL;
    req.rec.graph_osd = NULL;
    req.icap.text_label = NULL;
    req.icap.save_cap_buf = FALSE;//保存到cap_buff写TRUE，数据格式，前4字节为数据长度，4字节后为有效数据
    char name[128] = {0};
    snprintf(name, sizeof(name), CAMERA0_CAP_PATH"uvc%d_****.jpg", uvc_id);
    req.icap.path = name;
    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
    /* if (db_select("dat")) { */
    /* req.icap.text_label = &text_osd; */
    /* set_label_config(req.icap.width, req.icap.height, 0xe20095, req.icap.text_label); */
    /* } */

    err = server_request(uvc_video_server[uvc_id], VIDEO_REQ_IMAGE_CAPTURE, &req);
    if (err != 0) {
        printf("\ntake photo err\n");
        goto exit;
    }


exit:
    if (buffer) {
        free(buffer);
    }

    if (uvc_video_server[uvc_id]) {
        server_close(uvc_video_server[uvc_id]);
        uvc_video_server[uvc_id] = NULL;
    }


    return 0;
}


int video3_recining_take_photo_save_file(int uvc_id)
{
    void *server = NULL;
    union video_req req = {0};
    int err;
    int buffer_len = 1 * 1024 * 1024;
    u8 *buffer = calloc(1, buffer_len);
    if (!buffer) {
        goto exit;
    }
    if (!uvc_video_server[uvc_id]) {
        static char dev_name[20];
        user_net_video3_recx_start(uvc_id, 1);

    }


    req.icap.width = 1280;
    req.icap.height = 720;
    req.icap.buf_size = buffer_len;
    printf("\n req.icap.buf_size = %d\n", req.icap.buf_size);
    req.icap.quality = VIDEO_MID_Q;
    req.icap.buf = buffer;
    req.rec.text_osd = NULL;
    req.rec.graph_osd = NULL;
    req.icap.text_label = NULL;
    req.icap.save_cap_buf = FALSE;//保存到cap_buff写TRUE，数据格式，前4字节为数据长度，4字节后为有效数据

    char name[128] = {0};
    snprintf(name, sizeof(name), CAMERA0_CAP_PATH"uvc%d_****.jpg", uvc_id);
    req.icap.path = name;
    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */
    /* if (db_select("dat")) { */
    /* req.icap.text_label = &text_osd; */
    /* set_label_config(req.icap.width, req.icap.height, 0xe20095, req.icap.text_label); */
    /* } */

    err = server_request(uvc_video_server[uvc_id], VIDEO_REQ_IMAGE_CAPTURE, &req);
    if (err != 0) {
        printf("\ntake photo err\n");
        goto exit;
    }


exit:
    if (buffer) {
        free(buffer);
    }


    return 0;
}



int user_net_video0_recx_start(u8 is_stream)
{


    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;
    void *server = NULL;
    void *file = NULL;
    u8 channel = 0;
    u8 format = 0;



#ifdef CONFIG_VIDEO0_INIT_ASYNC
    extern int os_query_init_video_sem(void);
    if (os_query_init_video_sem()) {
        printf("\n os_query_init_video_sem err\n");
        hi3861l_reset();
    }
#endif // CONFIG_VIDEO0_INIT_ASYNC


    puts("start_video_rec0 \n");




    if (!video0_server) {
        char name[12];
        sprintf(name, "video0.%d", 0);
        video0_server = server_open("video_server", name);
        if (!video0_server) {
            return -EINVAL;
        }
        server_register_event_handler(video0_server, (void *)0, net_rec_dev_server_event_handler);
        server = video0_server;
        format = 1;
        if (!is_stream) {
            char fname[128] = {0};

            if (format) {
                snprintf(fname, sizeof(fname), CONFIG_DEC_PATH_1"VIDEO0_****.MOV");
            } else {
                snprintf(fname, sizeof(fname), CONFIG_DEC_PATH_1"VIDEO0_****.AVI");
            }
            video0_file = fopen(fname, "w+");
            file = video0_file;
        }
    } else {
        return 0;
    }


    channel = 1;
    int qos = db_select("vqua");

    if (qos >= 16 && qos <= 20) {
        req.rec.width = 1440;
        req.rec.height = 1088;
    } else if (qos >= 6 && qos <= 10) {
        req.rec.width = 1280;
        req.rec.height = 720;
    } else if (qos >= 1 && qos <= 5) {
        req.rec.width = 640;
        req.rec.height = 480;
    }


    /*
     *通道号，分辨率，封装格式，写卡的路径
     */
    req.rec.channel = channel;
    req.rec.format  = format;
    req.rec.state 	= VIDEO_STATE_START;
    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_MID_Q;
    req.rec.fps         = 0;
    req.rec.real_fps    = net_video_rec_get_fps();

    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
#if (CONFIG_AUDIO_FORMAT_FOR_MIC == AUDIO_FORMAT_AAC)
    req.rec.audio.fmt_format = AUDIO_FMT_AAC;
#else
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
#endif
    req.rec.audio.aud_interval_size = 1024;
    req.rec.audio.sample_rate = VIDEO_REC_AUDIO_SAMPLE_RATE;
    req.rec.audio.channel   = 1;
    req.rec.audio.volume    = __this_net->net_video1_art_on ? AUDIO_VOLUME : 0;

    req.rec.audio.buf = NULL;//库里面分配
    req.rec.audio.buf_len = AUDIO_BUF_SIZE;


#ifdef CONFIG_USB_UVC_AND_UAC_ENABLE
    req.rec.audio.sample_source = "user";
#endif

    req.rec.pkg_mute.aud_mute = !db_select("mic");

    req.rec.qp_attr.static_min = 31;
    req.rec.qp_attr.static_max = 35;
    req.rec.qp_attr.dynamic_min = 32;
    req.rec.qp_attr.dynamic_max = 34;
    req.rec.qp_attr.enable_change_qp = 1;
    struct app_enc_info app_avc_cfg;
    app_avc_cfg.pre_pskip_limit_flag = 1; 				//SKIP块提前榖Ωِ制开关 */
    app_avc_cfg.pre_pskip_th = 0;						//SKIP块提剈Ŧ֭阈值 */
    app_avc_cfg.common_pskip_limit_flag = 0; 			//SKIP块常规榖Ωِ制开关 */
    app_avc_cfg.common_pskip_th = 0;					//SKIP块常览Ŧ֭阈值 */
    app_avc_cfg.dsw_size = 0;							//搜索窗口大小（5x7  1:3x7） */
    app_avc_cfg.fs_x_points = 36;						//水平秴ç¹数 */
    app_avc_cfg.fs_y_points = 32;						//垂直秴ç¹数 */
    app_avc_cfg.f_aq_strength = 0.8f;					//宏块QP臀åڔ强度 */
    app_avc_cfg.qp_offset_low = 0;						//宏块QP槧ܤ؋限 */
    app_avc_cfg.qp_offset_high = 7;					//宏块QP槧ܤ؊限 */
    app_avc_cfg.nr_reduction = 0;						//DCT降噼eų */
    app_avc_cfg.user_cfg_enable = 1;					//编码参敔ɦȷ配置使能 */
    req.rec.app_avc_cfg = &app_avc_cfg;
    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标,写0表示1个宏块有效
     *roio_ratio : 区域比例系数
     */
    req.rec.abr_kbps = net_video_rec_get_abr(req.rec.width);

#if   defined __CPU_AC5401__
    {
        req.rec.IP_interval = 0;
    }
#elif defined __CPU_AC5601__
    if (req.rec.height > 720) {
        req.rec.IP_interval = 1;
    } else {
        req.rec.IP_interval = 0;
    }
#else
    req.rec.IP_interval = 32;//64;//128;
#endif

    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
     *感兴趣区域qp 为其他区域的 70% ，可以调整
     */
#if 0
    req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16;
    req.rec.roi.roi1_xy = (req.rec.height * 11 / 12 / 16) << 24 | (req.rec.height * 4 / 12 / 16) << 16 | (req.rec.width * 11 / 12 / 16) << 8 | (req.rec.width) * 1 / 12 / 16;
    req.rec.roi.roi2_xy = 0;
    req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0;
    req.rec.roi.roio_ratio = 256 * 70 / 100 ;
    req.rec.roi.roio_ratio1 = 256 * 90 / 100;
    req.rec.roi.roio_ratio2 = 0;
    req.rec.roi.roio_ratio3 = 256 * 80 / 100;
#endif


    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

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
    /*     text_osd.color[0] = 0xe20095; */
    /* text_osd.bit_mode = 1; */
    text_osd.color[0] = 0x057d88;
    text_osd.color[1] = 0xe20095;
    text_osd.color[2] = 0xe20095;
    text_osd.bit_mode = 2;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
    /*     text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);

#if (defined __CPU_DV15__ || defined __CPU_DV17__ )

    text_osd.direction = 1;
    graph_osd.bit_mode = 16;//2bit的osd需要配置3个col|=    graph_osd.x = 0;
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = icon_16bit_data;//icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#else
    text_osd.direction = 0;
#endif

    if (db_select("dat")) {
        req.rec.text_osd = &text_osd;
#if (defined __CPU_DV15__ || defined __CPU_DV17__ )
#ifdef CONFIG_OSD_LOGO
        req.rec.graph_osd = NULL; // &graph_osd;
#endif
#endif
    }

    /*实时流不用*/
#if 0
    /*
     *      *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     *           */
    req.rec.slow_motion = 0;
    if (req.rec.camera_type != VIDEO_CAMERA_UVC) {
        req.rec.tlp_time = db_select("gap");
        if (req.rec.tlp_time) {
            req.rec.real_fps = 1000 / req.rec.tlp_time;
        }
    } else {
        req.rec.tlp_time = 0;
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel   = 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }

#endif

    req.rec.buf = NULL;
    req.rec.buf_len = 2 * 1024 * 1024;


    /* req.rec.cycle_time = 3 * 60; */
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;


    struct sockaddr_in *addr = ctp_srv_get_cli_addr(__this_net->priv);
    if (!addr) {
        addr = cdp_srv_get_cli_addr(__this_net->priv);
    }

    sprintf(req.rec.net_par.netpath, "usr://%s", NET_USR_PATH);


    printf("\n @@@@@@ path = %s\n", req.rec.net_par.netpath);

    if (!is_stream) {
        req.rec.file  = file;
    } else {
        //数据外引，用于网络
#if CONFIG_H264_STD_HEAD_ENABLE
        req.rec.target = VIDEO_TO_OUT | VIDEO_USE_STD ;
#else
        req.rec.target = VIDEO_TO_OUT;
#endif
        req.rec.out.path = req.rec.net_par.netpath;
        req.rec.out.arg  = NULL ;
        req.rec.out.open = stream_open;
        req.rec.out.send = stream_write;
        req.rec.out.close = stream_close;
    }

    // printf("\n\n %s %d\n\n",__func__,__LINE__);
    err = server_request(server, VIDEO_REQ_REC, &req);
    if (err != 0) {
        puts("\n\n\nstart rec0 err\n\n\n");
        return VREC_ERR_V1_SERVER_OPEN;
    }
    net_video0_state = VIDREC_STA_START;

    return 0;
}

int user_net_video0_recx_stop(u8 close)
{
    union video_req req = {0};
    int err;
    void *server = NULL;
    void *file = NULL;
    u8 channel = 0;

    server = video0_server;
    file = video0_file;
    video0_server = NULL;
    video0_file = NULL;
    channel =  1;

#if (defined CONFIG_VIDEO0_ENABLE)

    if (server) {
        req.rec.channel = channel;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(server, VIDEO_REQ_REC, &req);

        if (err != 0) {
            printf("\nstop rec0 err 0x%x\n", err);
            return VREC_ERR_V1_REQ_STOP;
        }
        if (close) {
            server_close(server);
            server = NULL;
        }
        if (file) {
            fclose(file);
        }
    }
    net_video0_state = VIDREC_STA_STOP;
    puts("\nuser_net_video_rec0_stop\n");
#endif

    return 0;
}




int user_net_video3_recx_start(u8 uvc_id, u8 is_stream)
{
    int err;
    union video_req req = {0};
    struct video_text_osd text_osd;
    struct video_graph_osd graph_osd;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u16 osd_max_heigh;


    u8 channel = 0;
    u8 format = 0;



    char name[20];
    void *uvc_fd;


    struct uvc_capability uvc_cap;

    puts("start_video_rec3 \n");

    if (!uvc_video_server[uvc_id]) {
        static char dev_name[20];
        //uvc_id = 0时，对应3.1
        //uvc_id = 1时，对应3.6
        //uvc_id = 2时，对应3.8
        //uvc_id = 3时，对应3.A
        sprintf(dev_name, "video3.%X", uvc_id * 5 + 1);
        if (uvc_id >= 2) {
            sprintf(dev_name, "video3.%X", uvc_id + 7);
        }
        printf("\n dev_name = %s\n", dev_name);
        uvc_video_server[uvc_id] = server_open("video_server", dev_name);
        if (!uvc_video_server[uvc_id]) {
            return -EINVAL;
        }
        server_register_event_handler(uvc_video_server[uvc_id], (void *)3, net_rec_dev_server_event_handler);
    }

    format = uvc_video_format[uvc_id];
    channel = uvc_id;
    if (!is_stream) {
        char fname[128] = {0};

        if (format) {
            snprintf(fname, sizeof(fname), CONFIG_DEC_PATH_1"uvc%d_****.MOV", uvc_id);
        } else {
            snprintf(fname, sizeof(fname), CONFIG_DEC_PATH_1"uvc%d_****.AVI", uvc_id);
        }
        uvc_video_file[uvc_id] = fopen(fname, "w+");
    }

    req.rec.camera_type = VIDEO_CAMERA_UVC;
    req.rec.three_way_type = 0;
    req.rec.IP_interval = 0;
    req.rec.width 	= UVC_ENC_WIDTH;
    req.rec.height 	= UVC_ENC_HEIGH;

    req.rec.uvc_id = __this->uvc_id;



    /*
     *通道号，分辨率，封装格式，写卡的路径
     */
    req.rec.channel = channel;
    req.rec.format  = format;
    req.rec.state 	= VIDEO_STATE_START;

    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality     = VIDEO_MID_Q;
    req.rec.fps         = 0;
    req.rec.real_fps    = net_video_rec_get_fps();


    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
//#if (CONFIG_AUDIO_FORMAT_FOR_MIC == AUDIO_FORMAT_AAC)
//    req.rec.audio.fmt_format = AUDIO_FMT_AAC;
//#else
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
//#endif
    req.rec.audio.aud_interval_size = 1024;
    req.rec.audio.sample_rate = VIDEO_REC_AUDIO_SAMPLE_RATE;
    req.rec.audio.channel   = 1;
    req.rec.audio.volume    = __this_net->net_video1_art_on ? AUDIO_VOLUME : 0;
//#ifndef CONFIG_SHARED_REC_HDL_ENABLE    //定义共用录像句柄
//    req.rec.audio.buf = __this_net->audio_buf;
//    req.rec.audio.buf_len = NET_AUDIO_BUF_SIZE;
//#else
    req.rec.audio.buf = __this->audio_buf[3];
    req.rec.audio.buf_len = AUDIO_BUF_SIZE;
//#endif

//#ifdef CONFIG_USB_UVC_AND_UAC_ENABLE
    req.rec.audio.sample_source = "user";
//#endif
//    printf("\n\n %s %d\n\n",__func__,__LINE__);
    req.rec.pkg_mute.aud_mute = !db_select("mic");
    //  printf("\n\n %s %d\n\n",__func__,__LINE__);
    req.rec.qp_attr.static_min = 31;
    req.rec.qp_attr.static_max = 35;
    req.rec.qp_attr.dynamic_min = 32;
    req.rec.qp_attr.dynamic_max = 34;
    req.rec.qp_attr.enable_change_qp = 1;
    struct app_enc_info app_avc_cfg;
    app_avc_cfg.pre_pskip_limit_flag = 1; 				//SKIP块提前榖Ωِ制开关 */
    app_avc_cfg.pre_pskip_th = 0;						//SKIP块提剈Ŧ֭阈值 */
    app_avc_cfg.common_pskip_limit_flag = 0; 			//SKIP块常规榖Ωِ制开关 */
    app_avc_cfg.common_pskip_th = 0;					//SKIP块常览Ŧ֭阈值 */
    app_avc_cfg.dsw_size = 0;							//搜索窗口大小（5x7  1:3x7） */
    app_avc_cfg.fs_x_points = 36;						//水平秴ç¹数 */
    app_avc_cfg.fs_y_points = 32;						//垂直秴ç¹数 */
    app_avc_cfg.f_aq_strength = 0.8f;					//宏块QP臀åڔ强度 */
    app_avc_cfg.qp_offset_low = 0;						//宏块QP槧ܤ؋限 */
    app_avc_cfg.qp_offset_high = 7;					//宏块QP槧ܤ؊限 */
    app_avc_cfg.nr_reduction = 0;						//DCT降噼eų */
    app_avc_cfg.user_cfg_enable = 1;					//编码参敔ɦȷ配置使能 */
    req.rec.app_avc_cfg = &app_avc_cfg;
    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标,写0表示1个宏块有效
     *roio_ratio : 区域比例系数
     */
    req.rec.abr_kbps = net_video_rec_get_abr(req.rec.width);
    //  printf("\n\n %s %d\n\n",__func__,__LINE__);
#if   defined __CPU_AC5401__
    {
        req.rec.IP_interval = 0;
    }
#elif defined __CPU_AC5601__
    if (req.rec.height > 720) {
        req.rec.IP_interval = 1;
    } else {
        req.rec.IP_interval = 0;
    }
#else
    req.rec.IP_interval = 32;//64;//128;
#endif
    // printf("\n\n %s %d\n\n",__func__,__LINE__);
    /*感兴趣区域为下方 中间 2/6 * 4/6 区域，可以调整
     *感兴趣区域qp 为其他区域的 70% ，可以调整
     */
#if 0
    req.rec.roi.roio_xy = (req.rec.height * 5 / 6 / 16) << 24 | (req.rec.height * 3 / 6 / 16) << 16 | (req.rec.width * 5 / 6 / 16) << 8 | (req.rec.width) * 1 / 6 / 16;
    req.rec.roi.roi1_xy = (req.rec.height * 11 / 12 / 16) << 24 | (req.rec.height * 4 / 12 / 16) << 16 | (req.rec.width * 11 / 12 / 16) << 8 | (req.rec.width) * 1 / 12 / 16;
    req.rec.roi.roi2_xy = 0;
    req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0;
    req.rec.roi.roio_ratio = 256 * 70 / 100 ;
    req.rec.roi.roio_ratio1 = 256 * 90 / 100;
    req.rec.roi.roio_ratio2 = 0;
    req.rec.roi.roio_ratio3 = 256 * 80 / 100;
#endif
    // printf("\n\n %s %d\n\n",__func__,__LINE__);

    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

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
    /*     text_osd.color[0] = 0xe20095; */
    /* text_osd.bit_mode = 1; */
    text_osd.color[0] = 0x057d88;
    text_osd.color[1] = 0xe20095;
    text_osd.color[2] = 0xe20095;
    text_osd.bit_mode = 2;
    text_osd.text_format = video_rec_osd_buf;
    text_osd.font_matrix_table = osd_str_total;
    /*     text_osd.font_matrix_base = osd_str_matrix; */
    /* text_osd.font_matrix_len = sizeof(osd_str_matrix); */
    text_osd.font_matrix_base = osd2_str_matrix;
    text_osd.font_matrix_len = sizeof(osd2_str_matrix);

#if (defined __CPU_DV15__ || defined __CPU_DV17__ )

    text_osd.direction = 1;
    graph_osd.bit_mode = 16;//2bit的osd需要配置3个col|=    graph_osd.x = 0;
    graph_osd.x = 0;
    graph_osd.y = 0;
    graph_osd.width = 256;
    graph_osd.height = 256;
    graph_osd.icon = icon_16bit_data;//icon_osd_buf;
    graph_osd.icon_size = sizeof(icon_16bit_data);//sizeof(icon_osd_buf);
#else
    text_osd.direction = 0;
#endif

    if (db_select("dat")) {
        req.rec.text_osd = &text_osd;
#if (defined __CPU_DV15__ || defined __CPU_DV17__ )
#ifdef CONFIG_OSD_LOGO
        req.rec.graph_osd = NULL; // &graph_osd;
#endif
#endif
    }

    /*实时流不用*/
#if 0
    /*
     *      *慢动作倍数(与延时摄影互斥,无音频); 延时录像的间隔ms(与慢动作互斥,无音频)
     *           */
    req.rec.slow_motion = 0;
    if (req.rec.camera_type != VIDEO_CAMERA_UVC) {
        req.rec.tlp_time = db_select("gap");
        if (req.rec.tlp_time) {
            req.rec.real_fps = 1000 / req.rec.tlp_time;
        }
    } else {
        req.rec.tlp_time = 0;
    }

    if (req.rec.slow_motion || req.rec.tlp_time) {
        req.rec.audio.sample_rate = 0;
        req.rec.audio.channel   = 0;
        req.rec.audio.volume    = 0;
        req.rec.audio.buf = 0;
        req.rec.audio.buf_len = 0;
    }

#endif

    req.rec.buf_len = 2 * 1024 * 1024;

//#ifndef CONFIG_SHARED_REC_HDL_ENABLE    //定义共用录像句柄
////    req.rec.buf = __this_net->net_v0_fbuf;
//    req.rec.buf_len = NET_VREC0_FBUF_SIZE;
//#else
//
//#if (defined  CONFIG_VIDEO1_ENABLE)
//    req.rec.buf = __this->video_buf[1];
//    req.rec.buf_len = VREC1_FBUF_SIZE;
//#elif (defined CONFIG_VIDEO3_ENABLE)
//    req.rec.buf = __this->video_buf[3];
//    req.rec.buf_len = VREC3_FBUF_SIZE;
//#else
//#error "\n  Please define a video device \n"
//#endif
//
//#endif

    /* req.rec.cycle_time = 3 * 60; */
    req.rec.cycle_time = db_select("cyc");
    if (req.rec.cycle_time == 0) {
        req.rec.cycle_time = 5;
    }
    req.rec.cycle_time = req.rec.cycle_time * 60;


    struct sockaddr_in *addr = ctp_srv_get_cli_addr(__this_net->priv);
    if (!addr) {
        addr = cdp_srv_get_cli_addr(__this_net->priv);
    }

    sprintf(req.rec.net_par.netpath, "uvc%d://%s", uvc_id, NET_USR_PATH);


    printf("\n @@@@@@ path = %s\n", req.rec.net_par.netpath);

    if (!is_stream) {
        req.rec.file    = uvc_video_file[uvc_id];
    } else {
        //数据外引，用于网络
#if CONFIG_H264_STD_HEAD_ENABLE
        req.rec.target = VIDEO_TO_OUT | VIDEO_USE_STD ;
#else
        req.rec.target = VIDEO_TO_OUT;
#endif
        req.rec.out.path = req.rec.net_par.netpath;
        req.rec.out.arg  = NULL ;
        req.rec.out.open = stream_open;
        req.rec.out.send = stream_write;
        req.rec.out.close = stream_close;
    }

    // printf("\n\n %s %d\n\n",__func__,__LINE__);
    err = server_request(uvc_video_server[uvc_id], VIDEO_REQ_REC, &req);
    if (err != 0) {
        puts("\n\n\nstart rec2 err\n\n\n");
        return VREC_ERR_V1_SERVER_OPEN;
    }
    uvc_video_state[uvc_id] = VIDREC_STA_START;
    // printf("\n\n %s %d\n\n",__func__,__LINE__);
    return 0;
}

int user_net_video3_recx_stop(u8 uvc_id, u8 close)
{
    union video_req req = {0};
    int err;
    void *server = NULL;
    void *file = NULL;
    u8 channel = 0;

    channel = uvc_id;;
#if (defined CONFIG_VIDEO1_ENABLE || defined CONFIG_VIDEO3_ENABLE)

    if (uvc_video_server[uvc_id]) {
        req.rec.channel = channel;
        req.rec.state = VIDEO_STATE_STOP;
        err = server_request(uvc_video_server[uvc_id], VIDEO_REQ_REC, &req);
        if (err != 0) {
            printf("\nstop rec0 err 0x%x\n", err);
            return VREC_ERR_V1_REQ_STOP;
        }

        uvc_video_state[uvc_id] = VIDREC_STA_STOP;

        if (close) {
            server_close(uvc_video_server[uvc_id]);
            uvc_video_server[uvc_id] = NULL;
        }
        if (uvc_video_file[uvc_id]) {
            fclose(uvc_video_file[uvc_id]);
        }
    }

    puts("\nuser_net_video_rec0_stop\n");
#endif

    return 0;
}


#if 0

void uvc_jpg_frame_callback(u8 id, u8 *buf, int len)
{
    if (id == 1) {
        void *fd = fopen(CONFIG_ROOT_PATH"***.JPG", "w+");
        fwrite(fd, buf, len);
        fclose(fd);
        printf("\n >>>>>> len = %d\n", len);
    }


}
void uvc_jpg_frame_decode_end_callback(u8 id, u8 *buf, int len)
{

}

#endif

#if 0
void uvc_h264_frame_callback(u8 id, u8 *buf, int len)
{
    void *fd = fopen(CONFIG_ROOT_PATH"***.JPG", "w+");
    fwrite(fd, buf, len);
    fclose(fd);
    printf("\n >>>>>> len = %d\n", len);
}
#endif

u32 get_uvc_usb_port(u8 uvc_id)
{
    u32 port = 0;
    void *fd = dev_open("uvc", (void *)uvc_id);
    if (fd) {
        dev_ioctl(fd, USBIOC_GET_USB_PORT, (u32)(&port));
        printf("usb port=%d\n", port);
        dev_close(fd);
    } else {
        log_e("uvc %d offline!\n", uvc_id);
    }
    return port;
}




int get_videox_rec_state(u8 uvc_id)
{
    int state = 0;
#if 1//def CONFIG_VIDEO0_ENABLE
    state =  net_video0_state;
#else
    state = uvc_video_state[uvc_id];
#endif

    return state;
}

void *get_videox_rec_server(u8 uvc_id)
{
    void  *server = NULL;
#if 1//def CONFIG_VIDEO0_ENABLE
    server =  video0_server;
#else
    server = uvc_video_server[uvc_id];
#endif
    return server;
}



void doorbell_reopen_rt_stream(u8 uvc_id)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    user_net_video0_recx_stop(0);
    user_net_video0_recx_start(1);
#else
    if (__this->video_online[3]) {
        user_net_video3_recx_stop(uvc_id, 0);
        user_net_video3_recx_start(uvc_id, 1);
    }
#endif // CONFIG_VIDEO0_ENABLE
}

void doorbell_open_all_rt_stream(void)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    user_net_video0_recx_start(0);
#else
    user_net_video3_recx_start(0, 1);
    user_net_video3_recx_start(1, 1);
    user_net_video3_recx_start(2, 1);
    user_net_video3_recx_start(3, 1);
#endif // CONFIG_VIDEO0_ENABLE
}

void doorbell_close_all_rt_stream(void)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    user_net_video0_recx_stop(0);
#else
    user_net_video3_recx_stop(0, 0);
    user_net_video3_recx_stop(1, 0);
    user_net_video3_recx_stop(2, 0);
    user_net_video3_recx_stop(3, 0);
#endif // CONFIG_VIDEO0_ENABLE
}


void doorbell_open_rt_stream(u8 uvc_id)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    user_net_video0_recx_start(1);
#else
    if (__this->video_online[3]) {
        user_net_video3_recx_start(uvc_id, 1);
    }
#endif // CONFIG_VIDEO0_ENABLE
}

void doorbell_close_rt_stream(u8 uvc_id)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    user_net_video0_recx_stop(0);
#else
    user_net_video3_recx_stop(uvc_id, 0);
#endif // CONFIG_VIDEO0_ENABLE
}

void doorbell_start_rec(void)
{
    if (!get_video_rec_state()) {
        video_rec_control_start();
    }
}

void doorbell_stop_rec(void)
{
    if (get_video_rec_state()) {
        video_rec_control_stop();
    }
}

void doorbell_in_usb(void)
{
    void *fd = NULL;
    extern char get_MassProduction(void);
    if (!get_MassProduction()) {
        puts("DOORBELL_EVENT_IN_USB\n");

        doorbell_close_all_rt_stream();  //处于usb模式不允许发流

        doorbell_stop_rec();

        struct intent it;
        init_intent(&it);

        it.name = "usb_app";
        it.action = ACTION_USB_SLAVE_MAIN;
        start_app(&it);
        fd = fopen(CONFIG_ROOT_PATH"uvc.bin", "r");
        if (fd) {
            it.data = "usb:uvc";
            fclose(fd);
        } else {
            it.data = "usb:msd";

        }
        it.name	= "usb_app";
        it.action = ACTION_USB_SLAVE_SET_CONFIG;
        start_app(&it);
    }

}

void doorbell_video_rec_get_iframe(u8 uvc_id)
{
#if 1//def CONFIG_VIDEO0_ENABLE
    video0_rec_get_iframe();
#else
    video3_rec_get_iframe(uvc_id);
#endif // CONFIG_VIDEO0_ENABLE
}


int doorbell_video_rec_take_photo(u8 uvc_id, u8 *buffer, u32 buffer_len)
{
#ifdef CONFIG_VIDEO0_ENABLE

#else

#endif // CONFIG_VIDEO0_ENABLE
}

static void image_transpose_90(u8 *src, u8 *dst, int sw, int sh)
{
   u8 *sptr;   // 源指针
   u8 *sptr1;  // 用于临时存储源指针
   u8 *dptr;   // 目标指针

   // 确保图像高度必须是4的倍数
   ASSERT(sh % 4 == 0, " image_transpose_ height must be align 4");

   // 外层循环，遍历图像的每一行（以4行为单位）
   for (int y = 0; y < sh / 4; y++) {
       // 计算源指针位置
       sptr = src + sw * y * 4;
       // 计算目标指针位置
       dptr = dst + sh - 4 - y * 4;

       // 内层循环，遍历图像的每一列（以4列为单位）
       for (int x = 0; x < sw / 4; x++) {
           sptr1 = sptr; // 将源指针赋值给临时指针

           // 使用汇编代码进行高效的像素拷贝和旋转
           __asm__ volatile(
               " r7 = [%0++=%2] \n\t"   // 从源指针读取4个字节到r7
               " r6 = [%0++=%2] \n\t"   // 从源指针读取4个字节到r6
               " r5 = [%0++=%2] \n\t"   // 从源指针读取4个字节到r5
               " r4 = [%0++=%2] \n\t"   // 从源指针读取4个字节到r4
               " r7_r6 = mts0(r7, r6) \n\t" // 合并r7和r6的值
               " r5_r4 = mts0(r5, r4) \n\t" // 合并r5和r4的值
               " r8 = r6 \n\t"           // 将r6的值存入r8
               " r7_r6 = mts1(r7, r5) \n\t" // 将r5的值合并到r7_r6
               " r5_r4 = mts1(r8, r4) \n\t" // 将r4的值合并到r5_r4
               " [%1++=%3] = r4 \n\t"    // 将合并后的结果写入目标指针
               " [%1++=%3] = r5 \n\t"    // 将合并后的结果写入目标指针
               " [%1++=%3] = r6 \n\t"    // 将合并后的结果写入目标指针
               " [%1++=%3] = r7 \n\t"    // 将合并后的结果写入目标指针
               : "=&r"(sptr1), "=&r"(dptr), "=&r"(sw), "=&r"(sh) // 输出约束
               : "0"(sptr1), "1"(dptr), "2"(sw), "3"(sh)        // 输入约束
               : "r4", "r5", "r6", "r7", "r8"                   // 被修改的寄存器
           );

           // 更新源指针，移动到下一个4字节位置
           sptr += 4;
       }
   }
}

static void image_transpose_270(u8 *src, u8 *dst, int sw, int sh)
{
   u8 *sptr;
   //uint8_t *sptr1;
   u8 *dptr;
   //uint8_t *dptr1;

   ASSERT(sh % 4 == 0, " image_transpose_ height must be align 4");
   for (int y = 0; y < sh / 4; y++) {
       //sptr = src + sw*y * 4;
       //dptr = dst + sh-4 - y * 4;
       for (int x = 0; x < sw / 4; x++) {
           sptr = src + sw * y * 4 + x * 4;
           dptr = dst + (sw - 4 - x * 4) * sh + y * 4;

           //sptr1 = sptr;
           __asm__ volatile(
               " r7 = [%0++=%2] \n\t"
               " r6 = [%0++=%2] \n\t"
               " r5 = [%0++=%2] \n\t"
               " r4 = [%0++=%2] \n\t"
               " r7_r6 = mts0(r6, r7) \n\t"
               " r5_r4 = mts0(r4, r5) \n\t"
               " r8 = r6 \n\t"
               " r7_r6 = mts1(r5, r7) \n\t"
               " r5_r4 = mts1(r4, r8) \n\t"
               " [%1++=%3] = r7 \n\t"
               " [%1++=%3] = r6 \n\t"
               " [%1++=%3] = r5 \n\t"
               " [%1++=%3] = r4 \n\t"
               : "=&r"(sptr), "=&r"(dptr), "=&r"(sw), "=&r"(sh)
               : "0"(sptr), "1"(dptr), "2"(sw), "3"(sh)
               : "r4", "r5", "r6", "r7", "r8"
           );

           //sptr += 4;
       }
   }
}

static void image_transpose(u8 *src, u8 *dst, int sw, int sh, int angle)
{
   // 检查源指针和目标指针是否为空
   if (!dst || !src) {
       return; // 如果为空，直接返回
   }

   // 根据传入的角度选择不同的图像转置函数
   switch (angle) {
   case 90:
       image_transpose_90(src, dst, sw, sh); // 处理90度旋转
       break;
   case 270:
       image_transpose_270(src, dst, sw, sh); // 处理270度旋转
       break;
   default:
       // 如果角度不支持，打印错误信息
       printf("不支持的角度=%d\n", angle);
       break;
   }
}


// 拍照时，返回是否需要对YUV进行旋转
u8 is_video3_uvc_image_need_rotate(struct video_device *device,u16 src_w,u16 src_h,u16 dst_w,u16 dst_h)
{
   if (device->major == 3 && device->mijor >=5 ) {
       return 1;
   }
   return 0;
}
// 如果需要旋转，库里面默认顺时针旋转90度，不满足需求时，可以外部实现

void  video3_uvc_image_capture_rotate(u8 *src,u8 *dst,u16 src_w,u16 src_h)
{
#define ANGLE    270
   u8 *s_yaddr = src;
   u8 *s_uaddr = src + src_w * src_h;
   u8 *s_vaddr = s_uaddr + src_w * src_h / 4;

   u8 *d_yaddr = dst;
   u8 *d_uaddr = dst + src_w * src_h;
   u8 *d_vaddr = d_uaddr + src_w * src_h / 4;
   u32 t = timer_get_ms();

   /* 旋转90 */
   void image_transpose(u8 *src, u8 *dst, int sw, int sh,int angle);
   image_transpose(s_yaddr,d_yaddr, src_w, src_h, ANGLE); //Y
   image_transpose(s_uaddr,d_uaddr, src_w/2, src_h/2,ANGLE); //U
   image_transpose(s_vaddr,d_vaddr, src_w/2, src_h/2,ANGLE); //V

   printf("\n t = %d\n",timer_get_ms() - t);
   flush_dcache(dst, src_w * src_h * 3 / 2);
}



