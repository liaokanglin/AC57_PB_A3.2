#include "stdlib.h"
#include "common/common.h"
#include "system/spinlock.h"
#include "os/os_api.h"
#include "server/rt_stream_pkg.h"
#include "app_config.h"
#include "lwip.h"
#include "lwip/sockets.h"
#include "sock_api/sock_api.h"
#include "os/os_compat.h"
#include "stream_core.h"



/*
 UDP端口:2224
 UDP 每片数据包 格式:
|1byte 类型 |  1byte 保留 | 2byte  payload长度 | 4byte 序号 |  4byte  帧大小 |  4byte 偏移  | 4byte 时间戳  |  payload  |
*/
#define UDP_SEND_BUF_SIZE  (44*1472)
#define MAX_PAYLOAD (1472-sizeof(struct frm_head))
#define JL_000dc ntohl(0x30306463)
#define JPEG_HEAD 0xE0FFD8FF

static u8 send_buf[UDP_SEND_BUF_SIZE];
static u32 video_timestramp  = 0;
static u32 audio_timestramp  = 0;
static u32 seq = 1;
static u32 aux_frame_cnt = 0;
static u32 old_times = 0;
static u32 new_times = 0;
static struct rt_stream_info *rt_info = NULL;
extern int net_video_rec_get_list_vframe(void);
extern int net_video_buff_set_frame_cnt(void);




int UDP_client_socket_unreg(int addr)
{
    if (rt_info) {
        printf("\n[ UDP_client_socket_quit , addr 0x%x , 0x%x]\n", rt_info->addr.sin_addr.s_addr, addr);
        if (rt_info->addr.sin_addr.s_addr == addr) {
            video_timestramp = 0;
            audio_timestramp = 0;
            aux_frame_cnt = 0;
            sock_set_quit(rt_info->fd);
            /* sock_unreg(rt_info->fd); */
            /* rt_info->fd = NULL; */
            /* rt_info = NULL; */
            return 0;
        }
    }
    return -1;
}

static int net_rt_pkg_udp_callback(enum sock_api_msg_type type, void *p)
{
    //缓存大于2帧丢帧
    struct rt_stream_info *info = (struct rt_stream_info *)p;
    int vcnt = net_video_rec_get_list_vframe();
    int set_cnt = net_video_buff_set_frame_cnt();
    if (vcnt > set_cnt && set_cnt > 0) {
        if (info) {
            info->cb_flag = 1;
        }
        return -1;
    }
    return 0;
}

static int  path_analyze(struct rt_stream_info *info, const *path)
{
    char *tmp = NULL;
    char *tmp2 = NULL;
    char ip[15] = {0};
    u16 port = 0;
    tmp = strstr(path, "udp://");
    if (!tmp) {
        goto exit;
    }
    tmp += strlen("udp://");
    tmp2 = strchr(tmp, ':');
    /* printf("tmp=%s  len=%d\n", tmp, tmp2 - tmp); */
    strncpy(ip, tmp, tmp2 - tmp);
    port = atoi(tmp2 + 1);
    printf("\n [remote ip:%s  port:%d] \n", ip, port);
    info->addr.sin_family = AF_INET;
    info->addr.sin_addr.s_addr = inet_addr(ip);
    info->addr.sin_port = htons(port);

    return 0;
exit:
    return -1;
}



static void *get_sock_handler(int (*cb_func)(enum sock_api_msg_type type, void *priv), void *priv)
{
    void *fd = NULL;
    u32 millsec = 100;
    fd = sock_reg(AF_INET, SOCK_DGRAM, 0, cb_func, priv);
    if (fd == NULL) {
        printf("\n[%s %d->Error in socket()]\n", __func__, __LINE__);
        return NULL;
    }
    sock_setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const void *)&millsec, sizeof(millsec));
    return fd;
}

static int net_send(struct rt_stream_info *info, char *buffer, size_t len, struct sockaddr *addr, int addrlen)
{
    return sock_sendto(info->fd, buffer, len, 0, addr, addrlen);
}

struct rt_stream_info *net_rt_open(const char *path, const char *mode)
{

    int flags = 0;
    int err;
    struct rt_stream_info *info = (struct rt_stream_info *)calloc(1, sizeof(struct rt_stream_info));
    if (info == NULL) {
        printf("\n[%s %d->Error in malloc()]\n", __func__, __LINE__);
        goto exit;
    }
    if (path_analyze(info, path)) {
        printf("\n[%s %d->Error in path_analyze]\n", __func__, __LINE__);
        goto exit;
    }
    info->fd = get_sock_handler(net_rt_pkg_udp_callback, info);
    if (!info->fd) {
        printf("\n[%s %d->Error get_sock_handler]\n", __func__, __LINE__);
        goto exit;;
    }
    if (os_mutex_create(&info->mutex)) {
        goto exit;
    }
    rt_info = info;
    return info;
exit:
    if (info) {
        free(info);
    }
    return NULL;
}

static int net_rt_write(struct rt_stream_info *info, char *buffer, size_t len, u8 type)
{
    int vcnt, set_cnt;
    u16 payload_len = 0;
    u32 total_udp_send = 0;
    struct frm_head frame_head = {0};
    int ret = 0;
    if (os_mutex_pend(&info->mutex, 200)) {
        ret = -2;
        goto exit;
    }


    if (type == JPEG_TYPE_VIDEO) {
        if (len <= 8) {
            ret = len;
            goto exit;
        }
        u32 *len_ptr;
        len_ptr = (u32 *)buffer;
        if ((*len_ptr == JL_000dc && *(len_ptr + 2) == JPEG_HEAD) || *(len_ptr + 2) == JPEG_HEAD) {
            buffer += 8;
            len -= 8;
        } else if (*len_ptr != JPEG_HEAD) {
            ret = len;
            goto exit;
        }
    }


    ret = len;
    u32 remain_len = len;


    //缓存大于2帧丢帧
    vcnt = net_video_rec_get_list_vframe();
    set_cnt = net_video_buff_set_frame_cnt();
    if (vcnt > set_cnt && set_cnt > 0) {
        ret = -1;
        goto exit;
    }

    info->cb_flag = 0;
    frame_head.offset = 0;
    frame_head.frm_sz = len;
    frame_head.type &= ~LAST_FREG_MAKER;
    frame_head.type |=  type;



    if (frame_head.type == H264_TYPE_VIDEO || frame_head.type == JPEG_TYPE_VIDEO) {
        //video_timestramp += 90000 / VIDEO_FRQ_30HZ;
        new_times = timer_get_ms();
        if (aux_frame_cnt < 60) {
            video_timestramp = 0;
        } else {
            video_timestramp += 0;//(new_times - old_times) * 100;
        }
        old_times = new_times;
        frame_head.timestamp = video_timestramp;




    } else {
        audio_timestramp = 501;
        frame_head.timestamp = audio_timestramp;
    }
    frame_head.timestamp = 0;
    frame_head.seq = seq++;

    while (remain_len) {
        if (remain_len < MAX_PAYLOAD) {
            payload_len = remain_len;
            frame_head.type |= LAST_FREG_MAKER;
        } else {
            payload_len = MAX_PAYLOAD;
        }
        frame_head.payload_size = payload_len;
        memcpy(send_buf + total_udp_send, &frame_head, sizeof(struct frm_head));
        total_udp_send += sizeof(struct frm_head);
        memcpy(send_buf + total_udp_send, buffer + frame_head.offset, payload_len);

        u32 change_data = 0x01000000;
        if (frame_head.offset  == 0 && frame_head.type == H264_TYPE_VIDEO) {
            if (*((char *)(send_buf + total_udp_send + 4)) == 0x67) {
                /* 处理PPS帧和SPS帧 I帧 */
                u32 tmp, tmp2, tmp3;

                memcpy(&tmp, send_buf + total_udp_send, 4);
                tmp = htonl(tmp);
                memcpy(send_buf + total_udp_send, &change_data, 4);
                memcpy(&tmp2, send_buf + total_udp_send + tmp + 4, 4);
                tmp2 = htonl(tmp2);
                memcpy(send_buf + total_udp_send + tmp + 4, &change_data, 4);
                memcpy(send_buf + total_udp_send + tmp + tmp2 + 8, &change_data, 4);

            } else {
                //	   处理P帧
                memcpy(send_buf + total_udp_send, &change_data, 4);
            }
        }

        total_udp_send += payload_len;
        if ((total_udp_send == UDP_SEND_BUF_SIZE) || (payload_len < MAX_PAYLOAD)) {
            if ((net_send(info, send_buf, total_udp_send, (struct sockaddr *)&info->addr, sizeof(struct sockaddr_in))) != total_udp_send) {
                if (info->cb_flag) {
                    ret = len;
                }
                puts("\n[ rt_stream_sent error! ]\n");
                ret = -1;
                goto exit;
            }
            total_udp_send = 0;
        }
        remain_len -= payload_len;
        frame_head.offset += payload_len;
    }

exit:
    if (ret == -1 || ret == len) {
        os_mutex_post(&info->mutex);
    }
    return ret;
}

static int net_rt_close(struct rt_stream_info *info)
{
    video_timestramp = 0;
    audio_timestramp = 0;
    aux_frame_cnt = 0;
    if (info->fd) {
        sock_set_quit(info->fd);
        sock_unreg(info->fd);
    }
    os_mutex_pend(&info->mutex, 0);
    os_mutex_del(&info->mutex, 1);
    free(info);
    rt_info = NULL;
    return 0;
}

REGISTER_NET_VIDEO_STREAM_SUDDEV(udp_video_stream_sub) = {
    .name = "udp",
    .open = net_rt_open,
    .write = net_rt_write,
    .close = net_rt_close,
};


