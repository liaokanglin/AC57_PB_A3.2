#ifndef __CVE_H__
#define __CVE_H__

#include "generic/typedef.h"

//<AVOUT制式>
#define CVE_NTSC_720      	0x01
#define CVE_PAL_720       	0x02
#define CVE_NTSC_960        0x04
#define CVE_PAL_960         0x08
#define CVE_PAL_1280        0x10
#define CVE_PAL_1920        0x20
#define CVE_COLOR_BAR       0x40
#define CVE_OSC_12M			0x80

void vcm_init(void);
void avo_init_ex(u8 mode);
void avohd_init_ex(int mode);

#endif
