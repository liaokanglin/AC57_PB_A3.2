#include "asm/includes.h"
#include "asm/sysinfo.h"
#include "asm/ldo.h"
#include "asm/adc.h"

//如需使用avdd18 avdd28 drcvdd 作为ldo请和硬件同事沟通

static int lod_trim = 0;


void avdd18_ctrl(AVDD18_LEV lev, u8 avdd18en)
{
    avdd18en = (avdd18en ? 1 : 0);
    if (lev > 7) {
        lev = 7;
    }
    SFR(LDO_CON, 4, 1, avdd18en);
    SFR(LDO_CON, 5, 3, lev);
}

void avdd28_ctrl(AVDD28_LEV lev, u8 avdd28en)
{
    avdd28en = (avdd28en ? 1 : 0);
    if (lev > 7) {
        lev = 7;
    }
    SFR(LDO_CON, 8, 1, avdd28en);
    SFR(LDO_CON, 9, 3, lev);
}

void ddrvdd_ctrl(DDRVDD_LEV lev, u8 sddr1, u8 ddrvdden)
{
    ddrvdden = (ddrvdden ? 1 : 0);
    sddr1 = (sddr1 ? 1 : 0);
    if (lev > 7) {
        lev = 7;
    }
    SFR(LDO_CON, 15, 1, ddrvdden);
    SFR(LDO_CON, 19, 1, sddr1);
    SFR(LDO_CON, 12, 3, lev);
}

void dvdd_ctrl(DVDD_LEV lev)
{
    if (lev > 7) {
        lev = 7;
    }
    SFR(LDO_CON, 0, 3, lev);
}


#define ldo_get_disable 0
#define ldo_get_dvdd    3
#define ldo_get_avdd15  5
#define ldo_get_avdd28  7

static void inline LDO_TO_ADC(u8 sel)
{
    LDO_CON &= ~(0x7 << 13);
    LDO_CON |= (sel << 13);
}

int ldo_trim(u8 ch)
{
    int val = 0;
    u16 tmp = 0;

    LDO_TO_ADC(ch);

    for (int i = 0; i < 100; i++) {
        tmp = adc_scan_manual(AD_CH15_LDO_VBG);
        val += tmp;
        /* delay(100); */
    }
    val /= 100;

    LDO_TO_ADC(ldo_get_disable);

    return val;
}

