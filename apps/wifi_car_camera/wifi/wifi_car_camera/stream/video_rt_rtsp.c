#include "streaming_media_server/fenice_port.h"
#include "server/rt_stream_pkg.h"


extern u32 timer_get_ms(void);


void *strm_open(const char *path, const char *mode)
{
    puts("\n strm_open \n");
    return (void *)!0;
}

#define JL_000dc ntohl(0x30306463)
#define JPEG_HEAD 0xE0FFD8FF

int strm_write(void *file, void *buf, u32 len, u32 type)
{
    char *data = (char *)buf;
    if (fenice_query_stop_sem()) {
        return len;//rtsp不允许回复负数
    }
    if (type == H264_TYPE_VIDEO  || type == JPEG_TYPE_VIDEO) {

        if (type == JPEG_TYPE_VIDEO) {
            if (len <= 8) {
                /* printf("\n @@@@@@ frame len = 8\n"); */
                return len;
            }
            u32 *len_ptr;
            len_ptr = (u32 *)data;
            if ((*len_ptr == JL_000dc && *(len_ptr + 2) == JPEG_HEAD) || *(len_ptr + 2) == JPEG_HEAD) {
                data += 8;
                len -= 8;
            } else if (*len_ptr != JPEG_HEAD) {
                /* printf("\n @@@@@@ frame err\n"); */
                return len;
            }
        }
        u32 t0 = timer_get_ms();
        fenice_set_vframe(data, len, 0); //阻塞函数
        u32 diff = timer_get_ms() - t0;
        if (diff > 30) {
            /* printf("\n @@@@@@ %s diff = %d\n", __func__, diff); */
        }
    } else {
        fenice_set_aframe(buf, len, 0); //阻塞函数
    }
    return len;
}

void strm_close(void *file)
{
    puts("\n strm_close \n");
}

