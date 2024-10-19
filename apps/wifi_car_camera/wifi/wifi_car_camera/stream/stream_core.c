#include "stream_core.h"

struct stream_core_info {
    struct net_video_stream_sub *sub;

};

//struct stream_core_info info;

//#define __this (&info)

#if (defined CONFIG_NET_PKG_H264) || (defined CONFIG_NET_PKG_JPEG)

static struct net_video_stream_sub *protocol_probe(const char *path)
{
    struct net_video_stream_sub *p = NULL;
    for (p = net_video_stream_sub_begin; p < net_video_stream_sub_end; p++) {
        if (strstr(path, p->name)) {
            return p;
        }
    }
    return NULL;
}


static int check_func(struct stream_core_info *info)
{
    if ((!info->sub->open) || (!info->sub->write) || (!info->sub->close)) {
        return -1;
    }
    return 0;
}

void *stream_open(const char *path, const char *mode)
{

    struct stream_core_info *info = NULL;

    info = calloc(1, sizeof(struct stream_core_info));

    if (!info) {
        return NULL;
    }

    //find hdl
    info->sub = protocol_probe(path);
    if (!info->sub) {
        printf("\n [ %s not find net_video_stream_sub fail, %s, %s]\n", __func__, path, mode);
        goto exit;
    }
    // check
    if (check_func(info)) {
        printf("\n [%s  check func is null fail ] \n", __func__);
        goto exit;
    }
    info->sub->private_data = info->sub->open(path, mode);
    if (!info->sub->private_data) {
        printf("\n[ %s open fail]\n", __func__);
        goto exit;
    }

    return info;
exit:
    free(info);
    return NULL;
}

int stream_write(void *file, void *buf, u32 len, u32 type)
{
    struct stream_core_info *core = (struct stream_core_info *)file;
    if (core) {
        struct net_video_stream_sub *sub = (struct net_video_stream_sub *)core->sub;
        return sub->write(sub->private_data, buf, len, type);
    }
    return 0;

}

int stream_ctrl(void *file, u32 cmd, u32 arg)
{
    return 0;
}

void stream_close(void *file)
{
    struct stream_core_info *core = (struct stream_core_info *)file;
    if (core) {
        struct net_video_stream_sub *sub = (struct net_video_stream_sub *)core->sub;
        sub->close(sub->private_data);
        free(file);
    }
}
#endif

