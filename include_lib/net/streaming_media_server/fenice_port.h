#ifndef __FENICE_PORT_H__
#define __FENICE_PORT_H__

#include "typedef.h"
#include "os/os_compat.h"
#include "system/init.h"
struct frame {
    u8 *buf;
    u32 len;
    u32 offset;
};

struct fenice_port {
    OS_SEM video_sem;
    OS_SEM video_read_sem;
    OS_SEM audio_sem;
    OS_SEM audio_read_sem;
    OS_SEM stop_sem;
    struct frame vframe;
    struct frame aframe;
};

int fenice_set_vframe(u8 *buf, u32 len, u32 offset);
struct frame *fenice_get_vframe(void);
int fenice_set_aframe(u8 *buf, u32 len, u32 offset);
struct frame *fenice_get_aframe(void);
int fenice_query_video_sem(void);
int fenice_query_audio_sem(void);
int fenice_post_video_read_sem(void);
int fenice_post_audio_read_sem(void);
int fenice_query_stop_sem(void);
int fenice_set_stop_sem(int value);
int fenice_pend_stop_sem(void);
int fenice_post_stop_sem(void);




#endif
