/* **********************************************************
 *
 * 名    称：test_audio.c
 * 说    明：用户获取音频数据接口测试文件
 * 备    注：
 * 		1、此方法在库中中断用弱定义方式定义函数:
 * 			int user_get_pcm_data(void *buf, u32 len)
 * 			当用户在应用层强定义时会自动调用用户定义的函数；
 * 		2、提供 user_audio_enc_open 和 user_audio_enc_close 函数
 * 			用于打开和关闭pcm编码器；
 *		3、启动录像时，一定要调用 user_audio_enc_close 关闭用户的
 *			pcm 编码器，使用录像app启动的编码器，否则会导致录像没
 *			声音；
 *
 * 使用说明：
 * 		1、未开启录像时，用 user_audio_enc_open 启动pcm编码，中断
 * 		即会调用 user_get_pcm_data，将audio数据传入
 * 		2、启动录像时，如果之前调用过open，则需在启动录像前调用
 * 		user_audio_enc_close 关闭客户启动的pcm编码
 * 		3、关闭录像时再调用 user_audio_enc_open 启动pcm编码
 * 		录像中用录像APP启动的pcm编码即可，只要进中断就会将audio数
 * 		据传到 user_get_pcm_data
 *
 * **********************************************************/


#include "server/audio_server.h"
#include "app_config.h"
#include "printf.h"
#include "cpu.h"

#include "event.h"
#include "system/includes.h"
#include "generic/gpio.h"
#include "generic/typedef.h"
#include "os/os_compat.h"

/* #define USER_GET_PCM_DATA	// 宏开关 */

#ifdef USER_GET_PCM_DATA

#define DATA_LEN	500 * 1024
struct _audio_config {
    void *audio_dev;
    u8 audio_enc_status;

    u32 data_len;
    char audio_buf[DATA_LEN];
    u8 buf_status;
};
static struct _audio_config dev_cfg;
#define __this  (&dev_cfg)


static u8 channel_bindex = 0xff;


/* ******************************************************************
 * 用户获取audio数据接口，在内部中断中调用；
 * 参数: buf --> 音频数据buf
 * 		 len --> 音频数据长度
 * ******************************************************************/
int user_get_pcm_data(void *buf, u32 len)
{
    __this->data_len += len;
    if (__this->data_len <= DATA_LEN) {
        memcpy(&__this->audio_buf[__this->data_len - len], buf, len);
    } else {
        __this->buf_status = 1;
    }
    return 0;

}


// 8K 采样率, 单声道
static void audio_enc_init(void)
{
    int err;
    struct audio_format f;
    struct video_reqbufs breq;

    __this->audio_dev = dev_open("audio", (void *)AUDIO_TYPE_ENC);
    if (!__this->audio_dev) {
        puts("\n open audio enc err \n");
    }
    f.type          = AUDIO_FMT_PCM;
    f.channel       = 1;
    f.sample_rate   = 8000;
    f.volume        = -1;
    f.sample_source = "mic";
    f.frame_len     = 8192 * 2;	// 16k
    f.priority      = 0;

    err = dev_ioctl(__this->audio_dev, AUDIOC_SET_FMT, (unsigned int)&f);
    if (err) {
        puts("\n\n AUDIOC_SET_FMT err\n\n");
        goto __err;
    }

    breq.buf = NULL;
    breq.size = 80 * 1024;
    err = dev_ioctl(__this->audio_dev, AUDIOC_REQBUFS, (unsigned int)&breq);
    if (err) {
        puts("audio req buf err \n");
        goto __err;
    }

    return;
__err:
    puts("\n stream \n");
    dev_close(__this->audio_dev);
    __this->audio_dev = NULL;
}


void audio_enc_start(void)
{
    int err = 0;
    if (__this->audio_dev) {
        err = dev_ioctl(__this->audio_dev, AUDIOC_STREAM_ON, (u32)&channel_bindex);
        printf("channel_bindex =%d\n", channel_bindex);
        __this->audio_enc_status = 1;

        if (err) {
            puts("audio stream on err \n");
            dev_close(__this->audio_dev);
            __this->audio_dev = NULL;
        }

    }
}


void audio_enc_stop(void)
{
    int err = 0;
    if (__this->audio_dev) {
        err = dev_ioctl(__this->audio_dev, AUDIOC_STREAM_OFF, (u32)channel_bindex);
        channel_bindex = 0xff;
        __this->audio_enc_status = 0;
    }
}


void user_audio_enc_close(void)
{
    if (__this->audio_dev) {
        if (channel_bindex != 0xff) {
            dev_ioctl(__this->audio_dev, AUDIOC_STREAM_OFF, (u32)channel_bindex);
            __this->audio_enc_status = 0;
            channel_bindex = 0xff;
        }
        puts("audio_enc_close \n");
        dev_close(__this->audio_dev);
        __this->audio_dev = NULL;
    }
}


void user_audio_enc_open(void)
{
    audio_enc_init();
    audio_enc_start();
}


// audio线程，把audio数据保存到SDK例子
static void local_audio_task(void *p)
{
    struct _audio_config *rf_para = p;
    struct video_buffer b;
    int err;

    user_audio_enc_open();

    printf("\n--func=%s\n", __FUNCTION__);
    while (1) {
        /* printf("get audio data: %d\n", __this->data_len); */
        if (__this->buf_status) {
            FILE *f = fopen(CONFIG_ROOT_PATH"audio***.pcm", "w+");
            fwrite(f, __this->audio_buf, __this->data_len - 256);
            fclose(f);
            __this->buf_status = 0;
            __this->data_len = 0;
        }
        os_time_dly(1);
    }
}


void tes_audio_init(void)
{
    int err;
    memset(__this, 0x0, sizeof(*__this));
    __this->audio_enc_status = 0;

    puts("\n tes_audio_init init \n\n\n\n\n\n");

    err = task_create(local_audio_task, __this, "audio_task");
}
late_initcall(tes_audio_init);

#endif














