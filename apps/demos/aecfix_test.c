
#include "video.h"
#include "printf.h"
#include "system/includes.h"
#include "app_config.h"
#include "gpio.h"
#include "audio_dev.h"
#include "app_config.h"
#include "asm/mpu.h"
#include "drc/limiter_api.h"

#if (defined CONFIG_AEC_DNS_ENABLE)
#define AUDIO_TEST
#define AEC_TEST
#define DNS_TEST
//#define DRC_TEST
#elif (defined CONFIG_ONLY_DNS_ENABLE)

#define AUDIO_TEST
//#define AEC_TEST
#define DNS_TEST
//#define DRC_TEST
#elif (defined CONFIG_ONLY_AEC_ENABLE)

#define AUDIO_TEST
#define AEC_TEST
//#define DNS_TEST
//#define DRC_TEST

#endif

//#define FILE_TEST

#define SAMPLERATE 16000
#define SR_POINTS 256
#define AEC_DNS_TEST_FILE_LEN ( 1 *1024 * 1024)


#ifdef AUDIO_TEST

static u8 file_head[48] = {
    0x52, 0x49, 0x46, 0x46, 0x20, 0x3A, 0x0B, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
    0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x00, 0x7D, 0x00, 0x00,
    0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x80, 0xFC, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00
};
static u8 *adc_file;
static u8 *dac_file;
static u8 *out_file;
static u32 file_cnt;






#ifdef AEC_TEST
#include "AECfix/AptFiltLib.h"
#include "AECfix/EchoSuppressLib.h"
#define Use_NLP 1
static const int AECReset_Thr = 16;
static const int M = 128;
static const int B = M / 64; //2
static const int bz = M / B;  //64
static const int Batch = 2;
static const float RefEngThr = -70.f;
static const float DT_AggressiveFactor = 4.f;
static const int frame_size = Batch * bz;
#if Use_NLP
static const float AggressFactor = -8.f;
static const float MiniSuppress = 4.f;
static const int Batchnlp = 1;
static const float OverSuppressThr = 0.05f;
static const float OverSuppress = 4.f;
#endif
static u8 aecfixrunbuf[4456];
static u8 aecfixtempbuf[1300];
#if Use_NLP
static u8 nlpfixrunbuf[5216];
static u8 nlpfixtempbuf[4120];
#endif

void aec_init()
{
    int AECReset_Cnt = 0;

    /* int aecfixrunbuf_size = AptFilt_QueryBufSize(M, B); */
    /* int aecfixtempbuf_size = AptFilt_QueryTempBufSize(M, B); */

    /* printf("get_dMS_runbuflen: %d \n", aecfixrunbuf_size); */
    /* printf("get_dMS_tmpbuflen: %d \n", aecfixtempbuf_size); */
    /* printf("get_D: %d \n", frame_size); */

    /* aecfixrunbuf = malloc(sizeof(int) * (aecfixrunbuf_size / sizeof(int))); */
    /* aecfixtempbuf = malloc(sizeof(int) * (aecfixtempbuf_size / sizeof(int))); */
    /* memset(aecfixrunbuf, 0, aecfixrunbuf_size); */
    /* memset(aecfixtempbuf, 0, aecfixtempbuf_size); */

    memset(aecfixrunbuf, 0, sizeof(aecfixrunbuf));
    memset(aecfixtempbuf, 0, sizeof(aecfixtempbuf));
    /* printf(" **************AptFilt_Init \n"); */
    AptFilt_Init(aecfixrunbuf, M, B, 30.f, RefEngThr, DT_AggressiveFactor);
    /* printf(" **************AptFilt_Init  end \n "); */


#if Use_NLP

    int D = 128 * Batchnlp;

    /* int nlpfixrunbuf_size = EchoSuppress_QueryBufSize(); */
    /* int nlpfixtempbuf_size = EchoSuppress_QueryTempBufSize();  */

    /* printf("process size:%d\n", D); */
    /* printf("echosup bufsize:%d\n",	nlpfixrunbuf_size); */
    /* printf("echosup tmpbufsize:%d\n",nlpfixtempbuf_size); */

    /* nlpfixrunbuf = malloc(sizeof(int) * (nlpfixrunbuf_size / sizeof(int))); */
    /* nlpfixtempbuf = malloc(sizeof(int) * (nlpfixtempbuf_size / sizeof(int))); */
    /* memset(nlpfixrunbuf, 0, nlpfixrunbuf_size); */
    /* memset(nlpfixtempbuf, 0, nlpfixtempbuf_size); */

    memset(nlpfixrunbuf, 0, sizeof(nlpfixrunbuf));
    memset(nlpfixtempbuf, 0, sizeof(nlpfixtempbuf));
    /* printf("*********** nlp_Init ************\n"); */
    EchoSuppress_Init(nlpfixrunbuf,
                      AggressFactor,
                      MiniSuppress,
                      OverSuppressThr,
                      OverSuppress
                     );
    /* printf("*********** nlp_Init end  ************\n"); */
#endif

}

void aec_process(u8 *src_buf, u8 *adc_buf, u8 *out_buf, u8 *nlp_buf)
{
#if 1
    AptFilt_Process(aecfixrunbuf,
                    aecfixtempbuf,
                    src_buf,
                    adc_buf,
#if Use_NLP
                    out_buf,
#else
                    nlp_buf,
#endif
                    frame_size);
#if Use_NLP
    EchoSuppress_Process(nlpfixrunbuf,
                         nlpfixtempbuf,
                         src_buf,
                         adc_buf,
                         out_buf,
                         nlp_buf,
                         NULL,
                         NULL,
                         frame_size);
#endif
#else
#if Use_NLP
    EchoSuppress_Process(nlpfixrunbuf,
                         nlpfixtempbuf,
                         src_buf,
                         adc_buf,
                         adc_buf,
                         nlp_buf,
                         NULL,
                         NULL,
                         frame_size);
#endif
#endif
}

#endif











#ifdef DNS_TEST
#include "DNS/jlsp_ns.h"
#define MIC_ARRAY 0
static const float gain_floor = 0.1f;
static const float over_drive = 1.0f;
static const float loudness = 1.0f;
static char private_heap[33368] __attribute__((aligned(32)));
static char share_heap[6484] __attribute__((aligned(32)));

void *audio_dns_init()
{
    void *ns = NULL;
    /* int private_heap_size, share_heap_size; */
    /* JLSP_ns_get_heap_size(&private_heap_size, &share_heap_size, SAMPLERATE, MIC_ARRAY); */
    /* printf("pri=%d  share=%d\n",private_heap_size, share_heap_size); */

    /* private_heap = (char *)malloc(private_heap_size); */
    /* share_heap = (char *)malloc(share_heap_size); */

    ns = (void *)JLSP_ns_init(private_heap, share_heap, gain_floor, over_drive, loudness, SAMPLERATE, MIC_ARRAY);
    JLSP_ns_reset(ns);
    return ns;
}

int dns_process(void *ns, u8 *inbuf, u8 *outbuf)
{
    int outsize;
    JLSP_ns_process(ns, inbuf, outbuf, &outsize);
    return outsize;
}

#endif








#ifdef DRC_TEST
u8 *drc_buf = NULL;
u32 drc_channel = 2;
u8 drc_inited = 0;
int audio_drc_init()
{
    u32 drc_buf_size = need_limiter_buf();
    drc_buf = malloc(drc_buf_size);

    int attackTime_low[2], releaseTime_low[2], threshold_low[2];
    attackTime_low[0] = 100;
    releaseTime_low[0] = 300;
    threshold_low[0] = -23;
    attackTime_low[1] = 100;
    releaseTime_low[1] = 300;
    threshold_low[1] = -23;


    for (int i = 0; i < drc_channel; i++) {
        threshold_low[i] = round(pow(10.0, threshold_low[i] / 20.0) * 32767);
    }


    if (!drc_buf) {
        log_e("drc buf malloc fail\n");
        return -1;
    }

    limiter_init(drc_buf, attackTime_low, releaseTime_low, threshold_low, SAMPLERATE, drc_channel);

    drc_inited = 1;

    return 0;
}

void drc_process(u8 *inbuf, u32 len)
{
    if (drc_inited) {
        limiter_run_16(drc_buf, inbuf, inbuf, len / 2 / drc_channel);
    }
}

#endif






static u8 tmp_buf[SR_POINTS * 2];
static u8 out_buf[SR_POINTS * 2];
static u8 nlp_buf[SR_POINTS * 2];
static u8 dns_outbuf[SR_POINTS * 2];


static u8 dac_buf[SR_POINTS * 2 * 4];
static u32 dac_buf_waddr = 0;
static u32 dac_buf_raddr = -1;
static u32 dac_timestamp = 0;



void audio_dac_output(u8 *buf, u32 len)
{
    u32 timestamp = jiffies_to_msecs(jiffies);
//	printf("d=%d\n", len);
    if (len > SR_POINTS * 2 * 2) { // dac 双声道数据
        log_w("audio dac buf oversize %d\n", len);
        return;
    }
#ifdef DRC_TEST
    drc_process(buf, len);
#endif

    if (time_after(timestamp, dac_timestamp + 200)) {
        dac_timestamp = timestamp;
        dac_buf_waddr = 0;
        dac_buf_raddr = -1;
    }
    if (dac_buf_waddr >= sizeof(dac_buf)) {
        dac_timestamp = timestamp;
        dac_buf_waddr = 0;
    }

    s16 *src_buf = (s16 *)(dac_buf + dac_buf_waddr);
    s16 *tmp = (s16 *)buf;
    for (int i = 0; i < len / 4; i++) {
        src_buf[i] = tmp[i * 2];
    }
    dac_buf_waddr += SR_POINTS * 2;
#ifdef CONFIG_AEC_DNS_TEST_ENABLE
    {
        u8 *dac1_buffer = calloc(1, len / 2);
        s16 *tmp_dac1_buffer = dac1_buffer;
        s16 *tmp = (s16 *)buf;
        for (int i = 0; i < len / 4; i++) {
            tmp_dac1_buffer[i] = tmp[i * 2];
        }
        post_msg_doorbell_task("aec_dns_test_task", 3, 0, dac1_buffer, len / 2);
    }
#endif

}

void audio_adc_output(u8 *buf, u32 len)
{



    if (dac_buf_raddr == -1) {
        u32 timestamp = jiffies_to_msecs(jiffies) - len / 2 * 1000 / SAMPLERATE * 2; //当前adc数据对应的时间戳
        u32 tmin = dac_timestamp - ((sizeof(dac_buf) - dac_buf_waddr) / 2 * 1000 / SAMPLERATE);
        u32 tmax = dac_timestamp + (dac_buf_waddr / 2 * 1000 / SAMPLERATE);
        if (timestamp < tmin || timestamp > tmax) {
//            log_e("aec timestamp err");
#ifdef CONFIG_MPU_ENABLE
            memset(mpu_inbuf, 0, len);
            memcpy(mpu_inbuf + SR_POINTS * 2, buf, len);
            if (cpu_post_msg(0, MPU_MSG_INBUF, 0) == 0) {
//                putchar('d');
            } else {
//                putchar('s');
            }
            goto __exit;
#endif
        }

        /*这里是为了确保取到的数据是t时刻的*/
        if (timestamp < dac_timestamp) {
            dac_buf_raddr = sizeof(dac_buf) - (((dac_timestamp - timestamp) * SAMPLERATE / 1000) * 2);
        } else {
            dac_buf_raddr = ((timestamp - dac_timestamp) * SAMPLERATE / 1000) * 2;
        }
        dac_buf_raddr = dac_buf_raddr / 16 * 16;
    } else {
        dac_buf_raddr += len;
        if (dac_buf_raddr >= sizeof(dac_buf)) {
            dac_buf_raddr -= sizeof(dac_buf);
        }
    }

    u8 *src_buf;
    if (dac_buf_raddr + len <= sizeof(dac_buf)) {
        src_buf = dac_buf + dac_buf_raddr;
    } else {

        u32 left = sizeof(dac_buf) - dac_buf_raddr;
        memcpy(tmp_buf, dac_buf + dac_buf_raddr, left);
        memcpy(tmp_buf + left, dac_buf, len - left);
        src_buf = tmp_buf;
    }

#ifdef CONFIG_MPU_ENABLE
    memcpy(mpu_inbuf, src_buf, len);
    memcpy(mpu_inbuf + SR_POINTS * 2, buf, len);
    if (cpu_post_msg(0, MPU_MSG_INBUF, 0) == 0) {
//		putchar('i');
    } else {
//		putchar('e');
    }
#else

#ifdef AEC_TEST
    //分包处理
    aec_process(src_buf, buf, out_buf, nlp_buf);
    aec_process(src_buf + 256, buf + 256, out_buf + 256, nlp_buf + 256);
#else
    memcpy(nlp_buf, buf, len);
#endif

#ifdef DNS_TEST
//	dns_process(nlp_buf, dns_outbuf);
#else
    memcpy(dns_outbuf, nlp_buf, len);
#endif
    /* memcpy(buf, nlp_buf, len); */
    /* audio_adc_putbuf(nlp_buf, len); */

#endif

__exit:

#ifdef CONFIG_AEC_DNS_TEST_ENABLE

    u8 *dac2_buffer = calloc(1, len);
    memcpy(dac2_buffer, src_buf, len);
    post_msg_doorbell_task("aec_dns_test_task", 3, 1, dac2_buffer, len);
    u8 *adc_buffer = calloc(1, len);
    memcpy(adc_buffer, buf, len);
    post_msg_doorbell_task("aec_dns_test_task", 3, 2, adc_buffer, len);
#endif
    return;
}




void audio_test(void *priv)
{

#ifdef CONFIG_MPU_ENABLE

    extern void mpu_test_init();
    mpu_test_init();

__again:
    if (cpu_post_msg(0, MPU_MSG_INIT, 1000) != 0) {
        msleep(100);
        goto __again;
    }

#else

#ifdef AEC_TEST
    aec_init();
#endif

#ifdef DNS_TEST
    dns_audio_init();
#endif

#endif

#ifdef DRC_TEST
    audio_drc_init();
#endif

#ifdef FILE_TEST

    while (1) {
        extern void rec_wav_start(char *file_name, u8 audio_type, char *sorce);
        extern void rec_wav_stop();
//        extern int  doorbell_play_voice_file(const char *file_name,u32 volume,u32 sample_rate );
//        doorbell_play_voice_file(CONFIG_ROOT_PATH"BGM.wav",30,16000);
        rec_wav_start(CONFIG_ROOT_PATH"****.wav", AUDIO_TYPE_ENC, "mic");

        os_time_dly(3000);
        rec_wav_stop();
//        extern void stop_play_voice_file();
//        stop_play_voice_file();
    }


    //aec_dns_test_file_save();
#endif


}

#ifdef CONFIG_AEC_DNS_TEST_ENABLE

void aec_dns_test_file_save()
{
    post_msg_doorbell_task("aec_dns_test_task", 1, 4);
}
void aec_dns_test_file_write(u8 *buf, u32 len)
{
    u8 *out_buffer = calloc(1, len);
    memcpy(out_buffer, buf, len);
    post_msg_doorbell_task("aec_dns_test_task", 3, 3, out_buffer, len);
}

void aec_dns_test_task(void *priv)
{
    int res;
    int msg[32];
    FILE *dac_fp1;
    FILE *dac_fp2;
    FILE *adc_fp;
    FILE *out_fp;
    static int dac_fp1_len ;
    static int dac_fp2_len ;
    static int adc_fp_len ;
    static int out_fp_len ;
    dac_fp1 = fopen(CONFIG_ROOT_PATH"dac1.pcm", "r");
    dac_fp2 = fopen(CONFIG_ROOT_PATH"dac2.pcm", "r");
    adc_fp = fopen(CONFIG_ROOT_PATH"adc.pcm", "r");
    out_fp = fopen(CONFIG_ROOT_PATH"out.pcm", "r");
    if (dac_fp1) {
        fdelete(dac_fp1);
        fdelete(dac_fp2);
        fdelete(adc_fp);
        fdelete(out_fp);
    }
    dac_fp1 = fopen(CONFIG_ROOT_PATH"dac1.pcm", "w+");
    dac_fp2 = fopen(CONFIG_ROOT_PATH"dac2.pcm", "w+");
    adc_fp = fopen(CONFIG_ROOT_PATH"adc.pcm", "w+");
    out_fp = fopen(CONFIG_ROOT_PATH"out.pcm", "w+");
    while (1) {
        res = os_task_pend("taskq", msg, ARRAY_SIZE(msg));
        switch (res) {
        case OS_TASKQ:
            switch (msg[0]) {
            case Q_USER:
                switch (msg[1]) {
                case 0:
                    fwrite(dac_fp1, msg[2], msg[3]);
                    free((void *)msg[2]);
                    dac_fp1_len += msg[3];
                    break;
                case 1:
                    fwrite(dac_fp2, msg[2], msg[3]);
                    free((void *)msg[2]);
                    dac_fp2_len += msg[3];
                    break;
                case 2:
                    fwrite(adc_fp, msg[2], msg[3]);
                    free((void *)msg[2]);
                    adc_fp_len += msg[3];
                    break;
                case 3:
                    fwrite(out_fp, msg[2], msg[3]);
                    free((void *)msg[2]);
                    out_fp_len += msg[3];
                    break;
                case 4:
                    goto __exit;
                    break;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
__exit:
    fclose(dac_fp1);
    fclose(dac_fp2);
    fclose(adc_fp);
    fclose(out_fp);
}
#endif
void audio_test_init()
{
#ifdef CONFIG_AEC_DNS_TEST_ENABLE
    thread_fork("aec_dns_test_task", 25, 0x2E00, 4096 * 3, 0, aec_dns_test_task, NULL);
#endif
    thread_fork("user_audio", 10, 1000, 0, 0, audio_test, NULL);
}

#ifdef CONFIG_MPU_ENABLE
#include "asm/mpu.h"
u8 mpu_inbuf[1024] sec(.sram) = {0};
u8 mpu_outbuf[512] sec(.sram) = {0};
void *mpu_ns sec(.sram) = NULL;
static u8 mpu_tmpbuf[512];
static u8 mpu_nlpbuf[512];
u8 *src_buf = &mpu_inbuf[0];
u8 *adc_buf = &mpu_inbuf[512];

void mpu_algorithm_init(void)
{
#if (defined CONFIG_AEC_DNS_ENABLE)
#ifdef AEC_TEST
    extern void aec_init();
    aec_init();
#endif // AEC_TEST

#ifdef DNS_TEST
    extern void *audio_dns_init();
    mpu_ns = audio_dns_init();
#endif // DNS_TEST

#elif (defined CONFIG_ONLY_DNS_ENABLE)

#ifdef DNS_TEST
    extern void *audio_dns_init();
    mpu_ns = audio_dns_init();
#endif // DNS_TEST

#elif (defined CONFIG_ONLY_AEC_ENABLE)

#ifdef AEC_TEST
    extern void aec_init();
    aec_init();
#endif // AEC_TEST

    mpu_ns = !NULL; //置标志位允许算法运行

#endif

}

void mpu_algorithm_process(struct _mpu_msg *new_msg)
{
#if (defined CONFIG_AEC_DNS_ENABLE)
    if (mpu_ns) {
        extern void aec_process(u8 * src_buf, u8 * adc_buf, u8 * out_buf, u8 * nlp_buf);
        aec_process(src_buf, adc_buf, mpu_tmpbuf, mpu_nlpbuf);
        aec_process(src_buf + 256, adc_buf + 256, mpu_tmpbuf + 256, mpu_nlpbuf + 256);
        /* memcpy(mpu_outbuf, mpu_nlpbuf, 512); */
        /* new_msg->cpu_id = 0; */
        /* new_msg->msg = MPU_MSG_OUTBUF; */

        extern int dns_process(void *ns, u8 * inbuf, u8 * outbuf);
        if (dns_process(mpu_ns, mpu_nlpbuf, mpu_tmpbuf)) {
            memcpy(mpu_outbuf, mpu_tmpbuf, 512);
            new_msg->cpu_id = 0;
            new_msg->msg = MPU_MSG_OUTBUF;
        }

    }

#elif (defined CONFIG_ONLY_DNS_ENABLE)
    if (mpu_ns) {
        if (dns_process(mpu_ns, adc_buf, mpu_tmpbuf)) {
            memcpy(mpu_outbuf, mpu_tmpbuf, 512);
            new_msg->cpu_id = 0;
            new_msg->msg = MPU_MSG_OUTBUF;
        }
    }
#elif (defined CONFIG_ONLY_AEC_ENABLE)
    if (mpu_ns) {
        extern void aec_process(u8 * src_buf, u8 * adc_buf, u8 * out_buf, u8 * nlp_buf);
        aec_process(src_buf, adc_buf, mpu_tmpbuf, mpu_nlpbuf);
        aec_process(src_buf + 256, adc_buf + 256, mpu_tmpbuf + 256, mpu_nlpbuf + 256);
        memcpy(mpu_outbuf, mpu_nlpbuf, 512);
        new_msg->cpu_id = 0;
        new_msg->msg = MPU_MSG_OUTBUF;
    }
#endif

}

void mpu_algorithm_output(char *buf, int len)
{
    //putchar('o');
#ifdef CONFIG_AEC_DNS_TEST_ENABLE
    extern void aec_dns_test_file_write(u8 * buf, u32 len);
    aec_dns_test_file_write(mpu_outbuf, 512);
#endif

#ifndef CONFIG_USB_UVC_AND_UAC_ENABLE

    audio_adc_putbuf(mpu_outbuf, 512);
#else

    user_enc_input_data(buf, len);
//        putchar('P');
#endif
}

#endif // CONFIG_MPU_ENABLE





#else

void audio_test()
{

    int msg[32];

    int retry;

    while (1) {
        if (storage_device_ready()) {
            break;
        }
        msleep(1000);
    }
    while (1) {

        extern int  doorbell_play_voice_file(const char *file_name, u32 volume, u32 sample_rate);
        doorbell_play_voice_file(CONFIG_ROOT_PATH"BGoffsetM.wav", 26, 16000);

        while (1) {

            os_taskq_accept(ARRAY_SIZE(msg), msg);
            msleep(1000);
            retry++;
            if (retry >= 65) {
                break;
            }
        }
        retry = 0;


    }
}
void audio_test_init()
{
    //thread_fork("user_audio", 10, 0x2E00, 4096, 0, audio_test, NULL);
}
#endif

void usb_mic_data_input(u8 *buf, u32 len)
{
#ifdef CONFIG_MPU_ENABLE
    memset(mpu_inbuf, 0, len);
    memcpy(mpu_inbuf + SR_POINTS * 2, buf, len);
    if (cpu_post_msg(0, MPU_MSG_INBUF, 0) == 0) {
//		putchar('i');
    } else {
//		putchar('e');
    }
#else
    user_enc_input_data(buf, len);
#endif // CONFIG_MPU_ENABLE
}





int aec_dns_init(void)
{
#if (defined CONFIG_AEC_DNS_ENABLE || defined CONFIG_ONLY_DNS_ENABLE)

    extern void audio_test_init();
    audio_test_init();
#endif
    return 0;
}



late_initcall(aec_dns_init);

