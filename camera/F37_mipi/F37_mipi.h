#ifndef __F37_MIPI_H__
#define __F37_MIPI_H__

#include "typedef.h"

#define F37_MIPI   1

#define F37_MIPI_OUTPUT_W    1920
#define F37_MIPI_OUTPUT_H    1080

#define F37_HDR_EN    0

s32 F37_mipi_set_output_size(u16 *width, u16 *height, u8 *freq);
s32 F37_mipi_power_ctl(u8 isp_dev, u8 is_work);

s32 F37_mipi_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq);


void F37_mipi_sleep();
void F37_mipi_wakeup();
void F37_mipi_W_Reg(u16 addr, u16 val);
u16 F37_mipi_R_Reg(u16 addr);


#endif



