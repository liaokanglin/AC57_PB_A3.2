#include "app_config.h"
#include "asm/mpu.h"
#include "timer.h"

#ifdef CONFIG_MPU_ENABLE

extern void audio_adc_putbuf(u8 *buf, u32 len);


#if 0
//sram 可使用空间约96k
u8 inbuf[1024] sec(.sram) = {0};
u8 outbuf[1024] sec(.sram) = {0};

static u8 mpu_buf[1024] = {0};
void mpu_handler(struct _mpu_msg msg, struct _mpu_msg *new_msg)
{
    switch (msg.msg) {
    case MPU_MSG_TEST:
        memcpy(mpu_buf, inbuf, 1024);
        mpu_buf[0xAA] = mpu_buf[12];
        mpu_buf[0xBB] = mpu_buf[34];
        mpu_buf[0xCC] = mpu_buf[56];
        mpu_buf[0xDD] = mpu_buf[78];
        memcpy(outbuf, mpu_buf, 1024);
        new_msg->msg = MPU_MSG_TEST;
        break;
    case MPU_MSG_OUTBUF:
        break;
    }
}

static u8 cpu_buf[1024] = {0};
void cpu_handler(struct _mpu_msg msg)
{
    u32 tmp = 0;
    u32 tmp1 = 0;
    switch (msg.msg) {
    case MPU_MSG_ACK:
        break;
    case MPU_MSG_TEST:
        memcpy(cpu_buf, outbuf, 1024);
        tmp = (u32)cpu_buf[12] | (u32)cpu_buf[34] << 8 | (u32)cpu_buf[56] << 16 | (u32)cpu_buf[78] << 24;
        tmp1 = (u32)cpu_buf[0xAA] | (u32)cpu_buf[0xBB] << 8 | (u32)cpu_buf[0xCC] << 16 | (u32)cpu_buf[0xDD] << 24;
        break;
    }
}

static u8 tmpbuf[1024] = {0};
void mpu_puts(void *p)
{
    extern u32 mpu_tick;
    extern u32 mpu_proc;
    u32 tmp = mpu_tick;
    u32 err = 0;



    tmpbuf[12] = (tmp & 0x000000FF) >> 0;
    tmpbuf[34] = (tmp & 0x0000FF00) >> 8;
    tmpbuf[56] = (tmp & 0x00FF0000) >> 16;
    tmpbuf[78] = (tmp & 0xFF000000) >> 24;
    memcpy(inbuf, tmpbuf, 1024);

    if (err = cpu_post_msg(0, MPU_MSG_TEST, 500)) {
        log_e("post msg err %d %d\n", err, mpu_is_busy());
    }
}

void mpu_test_init()
{
    if (setup_mpu(0) == 0) {
        sys_timer_add(NULL, mpu_puts, 1000);
    }
}

#endif




#if 1
void mpu_puts(void *p)
{
//	printf("mpu_tick=%d mpu_proc=%d mpu_rets=%x mpu_reti=%x\n", mpu_tick, mpu_proc, mpu_rets, mpu_reti);
}


void mpu_test_init()
{
    int ret;
    ret = setup_mpu(0);
    printf("\n >>>>>>>>>>>>>ret = %d\n", ret);
    sys_timer_add(NULL, mpu_puts, 1000);
}

void amplify(short *in, short *out, int npoint, float gain)
{
    for (int i = 0; i < npoint; i++) {
        float tmp = (float)in[i] * gain;
        if (tmp > 32767) {
            tmp = 32767;
        }
        if (tmp < -32768) {
            tmp = -32768;
        }
        out[i] = (short)tmp;
    }
}

void mpu_handler(struct _mpu_msg msg, struct _mpu_msg *new_msg)
{
    switch (msg.msg) {
    case MPU_MSG_INIT:
        extern void mpu_algorithm_init(void);
        mpu_algorithm_init();
        break;
    case MPU_MSG_INBUF:
        extern void mpu_algorithm_process(struct _mpu_msg * new_msg);
        mpu_algorithm_process(new_msg);
        break;
    }
}

void cpu_handler(struct _mpu_msg msg)
{
    switch (msg.msg) {
    case MPU_MSG_ACK:
        break;
    case MPU_MSG_OUTBUF:
        extern void mpu_algorithm_output(char *buf, int len);
        mpu_algorithm_output(mpu_outbuf, 512);
        break;
    }
}





#endif


#endif


