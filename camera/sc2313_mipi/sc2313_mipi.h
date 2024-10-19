#ifndef __sc2313_MIPI_H__
#define __sc2313_MIPI_H__

#include "typedef.h"

#define sc2313_MIPI   1

#define SC2313_HDR_EN   0

#if SC2313_HDR_EN
#define sc2313_MIPI_OUTPUT_W    1920
#define sc2313_MIPI_OUTPUT_H    1080
#else
#define sc2313_MIPI_OUTPUT_W    1928
#define sc2313_MIPI_OUTPUT_H    1088
#endif


s32 sc2313_mipi_set_output_size(u16 *width, u16 *height, u8 *freq);
s32 sc2313_mipi_power_ctl(u8 isp_dev, u8 is_work);

s32 sc2313_mipi_init(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq);


void sc2313_mipi_sleep();
void sc2313_mipi_wakeup();
void sc2313_mipi_W_Reg(u16 addr, u16 val);
u16 sc2313_mipi_R_Reg(u16 addr);


#endif



