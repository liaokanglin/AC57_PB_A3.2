#ifndef _GC1034_H
#define _GC1034_H

#include "typedef.h"

#define GC1034_DEBUG printf


#define HOST_MCLK           24
#define REG_SH_DELAY        0x18

#define GC1034_OUTPUT_W    1280
#define GC1034_OUTPUT_H     720
#define REG_WIN_WIDTH       1288
#define REG_WIN_HEIGHT      728



s32 GC1034_set_output_size(u16 *width, u16 *height, u8 *freq);
s32 GC1034_power_ctl(u8 isp_dev, u8 is_work);

//s32 GC1034_check(u8 isp_dev);
s32 GC1034_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq);


void GC1034_sleep();
void GC1034_wakeup();
void GC1034_W_Reg(u16 addr, u16 val);
u16 GC1034_R_Reg(u16 addr);


#endif


