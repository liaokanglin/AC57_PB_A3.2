
#include "strm_video_rec.h"
#include "vrec_osd.h"
#include "user_isp_cfg.h"
#include "vrec_icon_osd.h"
#include "streaming_media_server/fenice_port.h"

static struct strm_video_hdl fv_rec_handler;
#define sizeof_this     (sizeof(struct video_rec_hdl))
#define __this_strm 	(&fv_rec_handler)

static const u16 strm_rec_pix_w[] = {1280, 640};
static const u16 strm_rec_pix_h[] = {720,  480};
#define AUDIO_VOLUME	64

static char video_net_osd_buf[512] ALIGNE(64);

struct fenice_source_info s_info = {
    .type = STRM_SOURCE_VIDEO0,//摄像头类型
    .width = 640,//分辨率
    .height = 480,
    .fps = STRM_VIDEO_REC_FPS0,//帧率
    .sample_rate = 0,//采样率，默认配置为0
    .channel_num = 1,//通道数
};

static OS_MUTEX strm_mutex;

extern void *strm_open(const char *path, const char *mode);
extern int strm_write(void *file, void *buf, u32 len, u32 type);
extern void strm_close(void *file);

extern int net_video_rec_get_uvc_id(void);
extern int net_video_rec_state(void);
extern void *get_video_rec_handler(void);
static int fv_video_rec_open(void);
static int fv_video_rec_close(void);


/*主要用于录像启停RTSP*/
int strm_video_rec_open(void)
{
    int ret = 0;
    if (!__this_strm->is_open) {
        os_mutex_post(&strm_mutex);
        return -1;
    }
    if (__this_strm->state == VIDREC_STA_STOP) {
        ret = fv_video_rec_open();
        if (ret) {
            printf("\n @@@@@@ strm_video_rec_open err\n");
        }
    }
    os_mutex_post(&strm_mutex);
    return ret;
}
int strm_video_rec_close(void)
{
    int ret = 0;
    os_mutex_pend(&strm_mutex, 0);
    if (!__this_strm->is_open) {
        return -1;
    }

    if (__this_strm->state == VIDREC_STA_START) {
        ret = fv_video_rec_close();
        if (ret) {
            printf("\n @@@@@@ strm_video_rec_stop err\n");
        }
    }
    return ret;
}

int strm_video_rec_close2(void)
{
    int ret = 0;
    if (!__this_strm->is_open) {
        return -1;
    }
    if (__this_strm->state == VIDREC_STA_START) {
        ret = fv_video_rec_close();
        if (ret) {
            printf("\n @@@@@@ strm_video_rec_stop err\n");
        }
    }
    return ret;
}


/*码率控制，根据具体分辨率设置*/
static int strm_video_rec_get_abr(u32 width)
{
    if (width <= 384) {
        return 1000;
    } else if (width <= 640) {
        return 1500;
    } else if (width <= 1280) {
        return 2000;
    } else if (width <= 1920) {
        return 2500;
    } else {
        return 3000;
    }
}
int strm_video_rec_get_fps(void)
{
    return 25;
}

void *get_strm_video_rec_handler(void)
{
    return (void *)&fv_rec_handler;
}

static int fv_video_rec_doing_isp_scenes(void *p)
{
    if (__this_strm->isp_scenes_status) {
        return 0;
    }

    stop_update_isp_scenes();
    if ((int)p == 1) {
        if (__this_strm->video_rec0 && __this_strm->state == VIDREC_STA_START) {
            __this_strm->isp_scenes_status = 1;
            printf("__this_strm->video_rec0 : 0x%x \n", __this_strm->video_rec0);
            return start_update_isp_scenes(__this_strm->video_rec0);
        }
        if (__this_strm->video_rec1 && __this_strm->state == VIDREC_STA_START) {
            __this_strm->isp_scenes_status = 2;
            printf("__this_strm->video_rec1 : 0x%x \n", __this_strm->video_rec1);
            return start_update_isp_scenes(__this_strm->video_rec1);
        }
    }
    __this_strm->isp_scenes_status = 0;

    return 1;
}


static int fv_video_rec_start_isp_scenes()
{
    return wait_completion(isp_scenes_switch_timer_done, fv_video_rec_doing_isp_scenes, (void *)1);
}
static int fv_video_rec_stop_isp_scenes()
{
    if (!__this_strm->isp_scenes_status) {
        return 0;
    }
    __this_strm->isp_scenes_status = 0;
    stop_update_isp_scenes();
    return 0;
}

static void strm_video_rec_server_event_handler(void *priv, int argc, int *argv)
{
    /*
     *该回调函数会在录像过程中，写卡出错被当前录像APP调用，例如录像过程中突然拔卡
     */
    switch (argv[0]) {
    case VIDEO_SERVER_NET_ERR:
        log_e("\nVIDEO_SERVER_NET_ERR\n");
        break;
    default :
        break;
    }
}

static int fv_video_rec_start(void)
{
    int err;
    char name[32] = {0};
    union video_req req = {0};
    struct video_text_osd text_osd;
    u32 vd_buflen;
    u16 max_one_line_strnum;
    u16 osd_line_num;
    u8 id;
    u8 ch;

    __this_strm->fbuf_fcnt = 0;
    __this_strm->fbuf_ffil = 0;
    fenice_set_stop_sem(0);

    req.rec.width 	= s_info.width;
    req.rec.height 	= s_info.height;
    req.rec.IP_interval = 0;
    req.rec.three_way_type = 0;
    req.rec.camera_type = VIDEO_CAMERA_NORMAL;
    ch = 1;


    if (s_info.type == STRM_SOURCE_VIDEO0) {
        printf("[STRM]ACTION_VIDEO0_OPEN_RT_STREAM-----\n\n");
        vd_buflen = NET_VREC0_FBUF_SIZE;
        sprintf(name, "video0.%d", 2);
        if (!__this_strm->video_rec0) {
            __this_strm->video_rec0 = server_open("video_server", name);
        }
        if (!__this_strm->video_rec0) {
            return VREC_ERR_V0_SERVER_OPEN;
        }
        __this_strm->video_id = 0;
        /* server_register_event_handler(__this_strm->video_rec0, NULL, strm_video_rec_server_event_handler); */
    } else if (s_info.type == STRM_SOURCE_VIDEO1) {
        printf("[STRM]ACTION_VIDEO1_OPEN_RT_STREAM-----\n\n");
        vd_buflen = NET_VREC0_FBUF_SIZE;
#ifdef CONFIG_VIDEO1_ENABLE
        sprintf(name, "video1.%d", 2);
        if (!__this_strm->video_rec1) {
            __this_strm->video_rec1 = server_open("video_server", name);
        }
        if (!__this_strm->video_rec1) {
            return VREC_ERR_V1_SERVER_OPEN;
        }
        __this_strm->video_id = 1;
        puts("start_video_rec1 \n");
        /* server_register_event_handler(__this_strm->video_rec1, NULL, strm_video_rec_server_event_handler); */
#endif
#ifdef CONFIG_VIDEO3_ENABLE
    // 定义一个指向UVC设备文件的指针
    void *uvc_fd;

    // 定义一个UVC设备的能力结构体
    struct uvc_capability uvc_cap;

    // 检查名为"uvc"的设备是否在线，如果不在线则返回错误
    if (!dev_online("uvc")) {
        return -1;
    }

    // 格式化并生成一个名称 "video3.0"
    sprintf(name, "video3.%d", 0);

    // 打开一个视频服务器，并返回一个句柄
    __this_strm->video_rec1 = server_open("video_server", name);

    // 如果视频服务器无法打开，返回一个错误代码
    if (!__this_strm->video_rec1) {
        return VREC_ERR_V1_SERVER_OPEN;
    }

    // 打印启动视频录制的信息
    puts("start_video_rec2 \n");

    // 注册事件处理程序，用于处理视频录制过程中发生的事件
    server_register_event_handler(__this_strm->video_rec1, NULL, strm_video_rec_server_event_handler);

    // 设置摄像头类型为UVC（USB视频设备类）
    req.rec.camera_type = VIDEO_CAMERA_UVC;

    // 如果启用了三路视频录制功能，则配置相应参数
    // #if THREE_WAY_ENABLE
    //     req.rec.three_way_type = VIDEO_THREE_WAY_JPEG;
    //     req.rec.IP_interval = 99; // 设置I帧间隔
    // #else
    //     req.rec.three_way_type = 0; // 不使用三路录制
    //     req.rec.IP_interval = 0; // 设置I帧间隔为0
    // #endif

    // 设置视频的宽度和高度，使用UVC编码器的分辨率
    req.rec.width   = UVC_ENC_WIDTH;
    req.rec.height  = UVC_ENC_HEIGH;

    // 设置UVC ID为0，指明使用第一个UVC设备
    req.rec.uvc_id = 0; // 或者使用其他已定义的UVC ID

    // 设置视频ID为2，用于标识当前的录制通道
    __this_strm->video_id = 2;
#endif
    }
    __this_strm->width = req.rec.width;
    __this_strm->height =  req.rec.height;
    /*
     *通道号，分辨率，封装格式，写卡的路径
     */
    req.rec.channel = 1;
    req.rec.state 	= VIDEO_STATE_START;
    req.rec.format  = NET_REC_FORMAT;
    /*
     *帧率为0表示使用摄像头的帧率
     */
    req.rec.quality = VIDEO_MID_Q;
    req.rec.fps 	= 0;
    req.rec.real_fps    = strm_video_rec_get_fps();
    /*
     *采样率，通道数，录像音量，音频使用的循环BUF,录不录声音
     */
    req.rec.audio.fmt_format = AUDIO_FMT_PCM;
    req.rec.audio.sample_rate = s_info.sample_rate;
    req.rec.audio.channel 	= 1;
    req.rec.audio.volume    = AUDIO_VOLUME;
    req.rec.audio.buf = __this_strm->audio_buf;
    req.rec.audio.buf_len = NET_AUDIO_BUF_SIZE;
    req.rec.pkg_mute.aud_mute = !db_select("mic");
    /*
     *码率，I帧和P帧比例，必须是偶数（当录MOV的时候才有效）,
     *roio_xy :值表示宏块坐标， [6:0]左边x坐标 ，[14:8]右边x坐标，[22:16]上边y坐标，[30:24]下边y坐标
     *写0表示1个宏块有效
     *roio_ratio : 区域比例系数
     */

    req.rec.abr_kbps = strm_video_rec_get_abr(req.rec.width);
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
    req.rec.roi.roio_ratio = 256 * 70 / 100 ;
    req.rec.roi.roi1_xy = 0;
    req.rec.roi.roi2_xy = 0;
    req.rec.roi.roi3_xy = (1 << 24) | (0 << 16) | ((req.rec.width / 16) << 8) | 0;
    req.rec.roi.roio_ratio1 = 0;
    req.rec.roi.roio_ratio2 = 0;
    req.rec.roi.roio_ratio3 = 256 * 80 / 100;
#endif

    /*
     * osd 相关的参数，注意坐标位置，x要64对齐，y要16对齐,底下例子是根据图像大小偏移到右下
     */

    u16 osd_max_heigh;
    struct video_graph_osd graph_osd;
    text_osd.font_w = 16;
    text_osd.font_h = 32;
    max_one_line_strnum = strlen(video_rec_osd_buf);//21;
    osd_line_num = 1;
    /*     if (db_select("num")) { */
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

    req.rec.text_osd = 0;
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


    req.rec.buf = __this_strm->video_buf;
    req.rec.buf_len = vd_buflen;
    req.rec.cycle_time = 3 * 60;
    req.rec.target = VIDEO_TO_OUT;
    req.rec.out.path = NULL;
    req.rec.out.arg  = NULL ;
    req.rec.out.open = strm_open;
    req.rec.out.send = strm_write;
    req.rec.out.close = strm_close;

    if (s_info.type == STRM_SOURCE_VIDEO0) {
        err = server_request(__this_strm->video_rec0, VIDEO_REQ_REC, &req);
    } else if (s_info.type == STRM_SOURCE_VIDEO1) {
        err = server_request(__this_strm->video_rec1, VIDEO_REQ_REC, &req);
    }
    if (err != 0) {
        puts("\n\n\n strm start rec err\n\n\n");
        return -EINVAL;
    }
    __this_strm->state = VIDREC_STA_START;
    fv_video_rec_start_isp_scenes();
    return 0;
}

static int fv_video_rec_stop(void)
{
    union video_req req = {0};
    int err;
    void *video_rec = NULL;
    printf("\n fv_video_rec_stop \n");
    if (s_info.type == STRM_SOURCE_VIDEO0 && __this_strm->video_rec0) {
        video_rec = __this_strm->video_rec0;
    } else if (s_info.type == STRM_SOURCE_VIDEO1 && __this_strm->video_rec1) {
        video_rec = __this_strm->video_rec1;
    }
    fenice_set_stop_sem(1);
    req.rec.channel = 1;  /* video0的sd卡录像为:channel0,所以这里不能在占用channel0 */
    req.rec.state = VIDEO_STATE_STOP;
    err = server_request(video_rec, VIDEO_REQ_REC, &req);
    if (err != 0) {
        printf("ERR:stop video rec1 err 0x%x\n", err);
        return VREC_ERR_V0_REQ_STOP;
    }
    __this_strm->fbuf_fcnt = 0;
    __this_strm->fbuf_ffil = 0;
    printf("fv_video_rec_stop ok \n");
    return 0;
}

static int fv_video_rec_open(void)
{
    int ret;
    int vd_buflen;
    printf("\n [%s enter]\n", __func__);
    if (__this_strm->state == VIDREC_STA_START) {
        printf("\n[Msg] repeat open\n");
        return -1;
    }
    __this_strm->state = VIDREC_STA_STARTING;
    vd_buflen = NET_VREC0_FBUF_SIZE;
    if (!__this_strm->video_buf) {
        __this_strm->video_buf = malloc(vd_buflen);
        if (!__this_strm->video_buf) {
            puts("malloc fv_v0_buf err\n");
            goto exit;
        }
    }

    if (!__this_strm->audio_buf) {
        __this_strm->audio_buf = malloc(NET_AUDIO_BUF_SIZE);
        if (!__this_strm->audio_buf) {
            puts("malloc fv_audio_buf err\n");
            free(__this_strm->video_buf);
            goto exit;
        }
    }
    return fv_video_rec_start();
exit:
    return -1;
}

static int fv_video_rec_close(void)
{
    int err;
    printf("\n [%s enter]\n", __func__);
    if (__this_strm->state != VIDREC_STA_START) {
        printf("\n[Msg] repeat close\n");
        return -1;
    }
    __this_strm->state = VIDREC_STA_STOPING;
    fv_video_rec_stop_isp_scenes();
    err = fv_video_rec_stop();
    if (err) {
        printf("fv_video_rec_stop err !!!\n");
        return -1;
    }
    if (__this_strm->video_rec0 && s_info.type == STRM_SOURCE_VIDEO0) {
        server_close(__this_strm->video_rec0);
        __this_strm->video_rec0 = NULL;
    } else if (__this_strm->video_rec1 && s_info.type == STRM_SOURCE_VIDEO1) {
        server_close(__this_strm->video_rec1);
        __this_strm->video_rec1 = NULL;
    }
//修改为退出APP才释放内存
#if 0
    if (__this_strm->video_buf) {
        free(__this_strm->video_buf);
        __this_strm->video_buf = NULL;
    }
    if (__this_strm->audio_buf) {
        free(__this_strm->audio_buf);
        __this_strm->audio_buf = NULL;

    }
#endif
    __this_strm->video_id = 0;
    __this_strm->state = VIDREC_STA_STOP;
    fv_video_rec_start_isp_scenes();
    printf("stream_media close \n\n");
    return 0;
}

void strm_video_rec_free_buf(void)
{
    if (__this_strm->video_buf) {
        free(__this_strm->video_buf);
        __this_strm->video_buf = NULL;
    }
    if (__this_strm->audio_buf) {
        free(__this_strm->audio_buf);
        __this_strm->audio_buf = NULL;

    }
}

/* 用于开启实时流时,stream_media_server回调 */
int fenice_video_rec_setup(void)
{
    printf("\n [ %s ]\n", __func__);
    int ret;
    os_mutex_pend(&strm_mutex, 0);
    if (__this_strm->is_open == TRUE) {
        os_mutex_post(&strm_mutex);
        return 0;
    }
    fv_video_rec_open();
    __this_strm->is_open = TRUE;
    os_mutex_post(&strm_mutex);
    return 0;
}

/* 用于实时流异常退出时,stream_media_server回调 */
int fenice_video_rec_exit(void)
{
    int ret;
    printf("\n [ %s ] enter\n", __func__);
    os_mutex_pend(&strm_mutex, 0);
    if (__this_strm->is_open == FALSE) {
        os_mutex_post(&strm_mutex);
        return 0;
    }
    fv_video_rec_close();
    //RTSP恢复默认
    s_info.type 		= STRM_SOURCE_VIDEO0;//摄像头类型
    s_info.width 		= 640;//分辨率
    s_info.height 		= 480;
    s_info.fps 			= STRM_VIDEO_REC_FPS0;//帧率
    s_info.sample_rate 	= 0;//采样率，默认配置为0
    s_info.channel_num 	= 1;//通道数
    __this_strm->is_open = FALSE;
    os_mutex_post(&strm_mutex);
    printf("\n [ %s ] exit\n", __func__);
    return 0;
}


int fenice_set_media_info(struct fenice_source_info *info)
{
    s_info.type = info->type == 0 ? STRM_SOURCE_VIDEO0 : STRM_SOURCE_VIDEO1;
    if (info->width >= 1920) {
        info->width = 1280;
        info->height = 720;
    }
    s_info.width = info->width;
    s_info.height = info->height;
    printf("strm media info : %s , w : %d , h : %d \n"
           , s_info.type == STRM_SOURCE_VIDEO1 ? "video1/2" : "video0"
           , s_info.width
           , s_info.height);
    return 0;
}

int fenice_get_audio_info(struct fenice_source_info *info)
{
    s_info.sample_rate = VIDEO_REC_AUDIO_SAMPLE_RATE;
    info->width = s_info.width;
    info->height = s_info.height;
    info->fps = s_info.fps;
    info->sample_rate = s_info.sample_rate;//rtsp选择打开,将采样率设置为8000
    info->channel_num = s_info.channel_num;
    printf("strm audio info : %d , %d\n", info->sample_rate, info->channel_num);
    return 0;
}

int fenice_get_video_info(struct fenice_source_info *info)
{
    info->width = s_info.width;
    info->height = s_info.height;
    info->fps = s_info.fps;
    info->sample_rate = s_info.sample_rate;
    info->channel_num = s_info.channel_num;
    printf("strm video info : %d , %d\n", info->sample_rate, info->channel_num);
    return 0;
}

void strm_video_rec_init(void)
{
    os_mutex_create(&strm_mutex);
}

late_initcall(strm_video_rec_init);
