#include "stdlib.h"
#include "common/common.h"
#include "server/rt_stream_pkg.h"
#include "lwip.h"
#include "stream_core.h"
#include "sock_api/sock_api.h"


static u32 aux_frame_cnt = 0;
static u32 seq = 0;

struct cli_info {
    struct list_head entry;
    void *fd;
    struct sockaddr_in addr;
};

struct video_rt_tcp_server_info {
    struct list_head cli_head;
    struct sockaddr_in local_addr;
    void *fd;
    OS_MUTEX mutex;
    OS_SEM sem;
    u32 flag;
    u8 cb_flag;
};

static struct video_rt_tcp_server_info server_info;

static    u8 is_start_I_frame = 0;



static struct cli_info *get_tcp_net_info();
extern int net_video_rec_get_list_vframe(void);
extern int net_video_buff_set_frame_cnt(void);

static int  path_analyze(struct rt_stream_info *info, const *path)
{
    char *tmp = NULL;
    char *tmp2 = NULL;
    char ip[15] = {0};
    u16 port = 0;
    tmp = strstr(path, "tcp://");
    if (!tmp) {
        goto exit;
    }
    tmp += strlen("tcp://");
    tmp2 = strchr(tmp, ':');
    printf("\n[ %s tmp=%s  len=%d]\n", __func__, tmp, tmp2 - tmp);
    strncpy(ip, tmp, tmp2 - tmp);
    port = atoi(tmp2 + 1);
    printf("\n[%s remote ip:%s  port:%d ]\n", __func__, ip, port);

    info->addr.sin_family = AF_INET;
    info->addr.sin_addr.s_addr = inet_addr(ip);
    info->addr.sin_port = htons(port);

    return 0;
exit:
    return -1;

}
static int video_rt_tcp_callback(enum sock_api_msg_type type, void *p)
{
    //缓存大于2帧丢帧
    struct video_rt_tcp_server_info *server_info = (struct video_rt_tcp_server_info *)p;
#if 0//暂时关闭,TCP发不完整一包会引起APP花屏
    int vcnt = net_video_rec_get_list_vframe();
    int set_cnt = net_video_buff_set_frame_cnt();
    if (vcnt > set_cnt && set_cnt > 0) {
        if (server_info) {
            server_info->cb_flag = 1;//server_info.cb_flag = 1;
            return -1;
        }
    }
#endif
    return 0;
}

static void *net_rt_open(const char *path, const char *mode)
{
    int ret = 0;
    puts("\n[ net_rt_open ]\n");
    struct rt_stream_info *r_info = calloc(1, sizeof(struct rt_stream_info));
    if (!r_info) {
        printf("\n[ %s malloc fail ]\n", __FILE__);
        goto exit;
    }
    path_analyze(r_info, path);

    struct cli_info *info = get_tcp_net_info();
    if (!info) {
        printf("\n[ %s get_tcp_net_info fail]\n", __FILE__);
        goto exit;
    }
    sock_clr_quit(info->fd);
    memcpy((void *)&r_info->addr, (void *)&info->addr, sizeof(struct sockaddr));
    r_info->fd = info->fd;
    is_start_I_frame = 1;
    if (os_mutex_create(&r_info->mutex)) {
        goto exit;
    }
    return (void *)r_info;
exit:
    if (r_info) {
        free(r_info);
    }
    return NULL;
}
#define JL_000dc ntohl(0x30306463)
#define JPEG_HEAD 0xE0FFD8FF
static u32 net_rt_write(void *hdr, u8 *data, u32 len, u8 type)
{
    int ret;
    int vcnt, set_cnt;
    struct rt_stream_info *r_info = (struct rt_stream_info *)hdr;
    struct frm_head frame_head = {0};
    ret = os_mutex_pend(&r_info->mutex, 400);
    if (ret) {
        ret = -2;
        printf("\n @@@@@@ os_mutex_pend err\n");
        goto exit;
    }

    if (type == JPEG_TYPE_VIDEO) {
        if (len <= 8) {
            ret = len;
            /* puts("\n @@@@@@ frame_len = 8\n"); */
            goto exit;
        }
        u32 *len_ptr;
        len_ptr = (u32 *)data;
        if ((*len_ptr == JL_000dc && *(len_ptr + 2) == JPEG_HEAD) || *(len_ptr + 2) == JPEG_HEAD) {
            data += 8;
            len -= 8;
        } else if (*len_ptr != JPEG_HEAD) {
            ret = len;
            /* puts("\n @@@@@@ frame_err\n"); */
            goto exit;
        }
    }

    if (type == H264_TYPE_VIDEO) {

        if (is_start_I_frame) {
            if (*(char *)(data + 4) != 0x67) {
                puts("skip P frame\n");
                ret = len;
                goto exit;
            } else {
                is_start_I_frame = 0;
            }
        }

        if (*(char *)(data + 4) == 0x18) {
            ret = len;
            goto exit;

        }

    }



    server_info.cb_flag = 0;
    memset(&frame_head, 0, sizeof(struct frm_head));
    frame_head.type = type;
    frame_head.seq = (seq++);
    frame_head.timestamp = 0;
    frame_head.frm_sz = len;
    ret = sock_send(r_info->fd, (char *)&frame_head, sizeof(struct frm_head), 0);
    if (server_info.cb_flag) {
        ret = len;
        goto exit;
    }
    if (ret <= 0) {
        ret = -1;
        printf("\n sock_send err\n");
        goto exit;
    }

    char mem[64];
    u32 tmp2;
    u32 tmp;
    u32 start_code = 0x01000000;



    if (type == H264_TYPE_VIDEO) {
        memcpy(mem, data, 64);
        if (*((char *)(mem + 4)) == 0x67) {
            memcpy(&tmp, mem, 4);
            tmp = htonl(tmp);
            memcpy(mem, &start_code, 4);
            memcpy(&tmp2, mem + tmp + 4, 4);
            tmp2 = htonl(tmp2);
            memcpy(mem + tmp + 4, &start_code, 4);
            memcpy(mem + tmp + tmp2 + 8, &start_code, 4);

        } else {
            memcpy(mem, &start_code, 4);
        }

        ret = sock_send(r_info->fd, mem, 64, 0);
        if (ret <= 0) {
            printf("%s   %d\n", __func__, __LINE__);
            ret = -1;
            goto exit;
        }

        ret = sock_send(r_info->fd, (char *) data + 64, len - 64, 0);
        if (ret <= 0) {
            printf("%s   %d\n", __func__, __LINE__);
            ret = -1;
            goto exit;
        }

        ret = len;

    } else {
        ret = sock_send(r_info->fd, (char *) data, len, 0);
        if (server_info.cb_flag) {
            ret = len;
            goto exit;
        }
        if (ret <= 0) {
            ret = -1;
            printf("\n sock_send err\n");
            goto exit;
        }
    }

exit:
    if (ret == -1 || ret == len) {
        os_mutex_post(&r_info->mutex);
    }
    return ret;//len -1 -2
}

static void net_rt_close(void *hdr)
{
    puts("\n[Msg ]net_rt_close\n");
    struct rt_stream_info *r_info = (struct rt_stream_info *)hdr;
    sock_set_quit(r_info->fd);
    aux_frame_cnt = 0;
    is_start_I_frame = 0;
    os_mutex_del(&r_info->mutex, 1);
    if (r_info) {
        free(r_info);
    }
}

REGISTER_NET_VIDEO_STREAM_SUDDEV(tcp_video_stream_sub) = {
    .name = "tcp",
    .open =  net_rt_open,
    .write = net_rt_write,
    .close = net_rt_close,
};


int TCP_client_socket_quit(int addr)
{
    struct cli_info *cli = NULL;
    struct list_head *pos = NULL;
    struct list_head *n = NULL;
    struct sockaddr_in *dest_addr = NULL;
    puts("\n[ TCP_client_socket_quit ]\n");
    os_mutex_pend(&server_info.mutex, 0);
    list_for_each_safe(pos, n, &server_info.cli_head) {
        cli = list_entry(pos, struct cli_info, entry);
        if (cli) {
            dest_addr = &cli->addr;
            if (dest_addr->sin_addr.s_addr == addr) {
                list_del(&cli->entry);
                printf("[Msg] %s sock_set_quit", __func__);
                sock_set_quit(cli->fd);
                sock_unreg(cli->fd);
                free(cli);
            }
        }
    }
    os_mutex_post(&server_info.mutex);
    os_sem_set(&server_info.sem, 0);
    return 0;
}


static struct cli_info *get_tcp_net_info()
{
    struct list_head *pos = NULL;
    struct cli_info *cli = NULL;
    struct cli_info *old_cli = NULL;
    int count = 0;

    extern u8 get_net_video_rec_state(void);
    if (get_net_video_rec_state()) {
        os_sem_pend(&server_info.sem, 100);
    }
    os_mutex_pend(&server_info.mutex, 0);
    list_for_each(pos, &server_info.cli_head) {
        cli = list_entry(pos, struct cli_info, entry);
        printf("\n [ %s ip:%s - port:%d]\n", __func__, inet_ntoa(cli->addr.sin_addr.s_addr), htons(cli->addr.sin_port));
        count++;
    }
    printf("\n [ %s count = %d]\n", __func__, count);

    while (count > 1) {
        old_cli = list_first_entry(&server_info.cli_head, struct cli_info, entry);
        list_del(&old_cli->entry);
        sock_set_quit(old_cli->fd);
        sock_unreg(old_cli->fd);
        old_cli->fd = NULL;
        printf("\n [Msg] %s free old_cli\n", __func__);
        free(old_cli);
        count--;
    }
    os_mutex_post(&server_info.mutex);
    return cli;
}

static void __do_sock_accpet(void *arg)
{
    socklen_t len = sizeof(server_info.local_addr);
    while (1) {

        struct cli_info *__cli = calloc(1, sizeof(sizeof(struct cli_info)));
        if (__cli == NULL) {
            printf("\n[ %s malloc fail ]\n", __func__);
            while (1);
        }
        __cli->fd  = sock_accept(server_info.fd, (struct sockaddr *)&__cli->addr, &len, video_rt_tcp_callback, &server_info);
        if (server_info.flag) {
            break;
        }
        if (__cli->fd == NULL) {
            printf("\n[ %s sock_accept err ]\n", __func__);
            continue;
        }
        printf("\n[ %s add client ]\n", __func__);
        os_mutex_pend(&server_info.mutex, 0);
        sock_set_send_timeout(__cli->fd, 4000);
        list_add_tail(&__cli->entry, &server_info.cli_head);
        os_mutex_post(&server_info.mutex);
        os_sem_post(&server_info.sem);
    }
}


int video_rt_tcp_server_init(int port)
{
    int ret = 0;
    puts("\n[ video_rt_tcp_server_init ]\n");
    memset(&server_info, 0x0, sizeof(server_info));

    os_sem_create(&server_info.sem, 0);
    os_mutex_create(&server_info.mutex);
    server_info.local_addr.sin_family = AF_INET;
    server_info.local_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
    server_info.local_addr.sin_port = htons(port);
    server_info.fd = sock_reg(AF_INET, SOCK_STREAM, 0, NULL, NULL);
    if (server_info.fd == NULL) {
        goto exit;
    }

    u32 opt = 1;
    if (sock_setsockopt(server_info.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("\n[ %s sock_bind fail ]\n", __FILE__);
        goto exit;
    }


    if (sock_bind(server_info.fd, (struct sockaddr *)&server_info.local_addr, sizeof(struct sockaddr))) {
        printf("\n[ %s sock_bind fail ]\n", __FILE__);
        goto exit;
    }
    sock_listen(server_info.fd, 0x5);

    INIT_LIST_HEAD(&server_info.cli_head);
    ret = thread_fork("__do_sock_accpet", 25, 0x400, 0, 0, __do_sock_accpet, NULL);
    if (ret != OS_NO_ERR) {
        printf("\n[%s thread fork fail ]\n", __FILE__);
        goto exit;
    }

    return 0;
exit:
    return -1;
}
void video_rt_tcp_server_uninit()
{
    puts("\n[ video_rt_tcp_server_uninit ]\n");
    server_info.flag = 1;
    sock_set_quit(server_info.fd);
    sock_unreg(server_info.fd);
    os_sem_del(&server_info.sem, OS_DEL_ALWAYS);
    os_mutex_del(&server_info.mutex, OS_DEL_ALWAYS);
    server_info.flag = 0;

}

