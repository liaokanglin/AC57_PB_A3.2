#ifndef _STRM_VIDEO_REC_H_
#define _STRM_VIDEO_REC_H_

#include "system/includes.h"
#include "server/ctp_server.h"
#include "video_rec.h"
#include "os/os_compat.h"
#include "action.h"
#include "app_config.h"
#include "app_database.h"
#include "server/rt_stream_pkg.h"
#include "server/net_server.h"
#include "net_video_rec.h"
#include "video_fps_ctrl.h"
#include "video_bitrate_ctrl.h"
#include "streaming_media_server/fenice_config.h"

struct strm_video_hdl {
    enum VIDEO_REC_STA state;

    struct server *video_rec0;
    struct server *video_rec1;
    struct server *video_rec2;
    struct server *video_display_0;
    struct server *video_display_1;
    struct server *video_display_2;
    u16 width;
    u16 height;
    u8 *video_buf;
    u8 *audio_buf;
    u8 isp_scenes_status;
    u8 channel;

    int timer_handler;
    fps_ctrl_t fps_ctrl_hdl;
    bitrate_ctrl_t bitrate_ctrl_hdl;
    u32 dy_fr;
    u32 dy_fr_denom;
    u32 dy_bitrate;
    u32 fbuf_fcnt;
    u32 fbuf_ffil;
    u8 video_id;
    u8 is_open;//标记RTSP是否被打开过
    u8 vd_buf_share_state;
    u8 ad_buf_share_state;
    u8 cap_image;
};

extern void *get_strm_video_rec_handler(void);
/*主要用于录像启停RTSP*/
extern int strm_video_rec_open(void);
extern int strm_video_rec_close(void);

#endif

