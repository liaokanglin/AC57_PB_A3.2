#ifndef __ASM_MPU_H__
#define __ASM_MPU_H__

#include "typedef.h"
#include "asm/cache.h"


enum _MPU_MSG {
    MPU_MSG_NONE = 0,
    MPU_MSG_TEST,
    MPU_MSG_INIT,
    MPU_MSG_INBUF,
    MPU_MSG_OUTBUF,



    MPU_MSG_ACK = 0xFF,
};

struct _mpu_msg {
    u8  cpu_id;
    u8  msg;
};


extern u8 mpu_is_busy();
extern int setup_mpu(u8 mpu_id);
extern int cpu_post_msg(u8 mpu_id, u8 msg, u32 timeout);

//__attribute__((weak))
void mpu_handler(struct _mpu_msg msg, struct _mpu_msg *new_msg);
void cpu_handler(struct _mpu_msg msg);

extern u8 mpu_inbuf[];
extern u8 mpu_outbuf[];

extern u32 mpu_tick;
extern u32 mpu_proc;
extern u32 mpu_rets;
extern u32 mpu_reti;

#endif
