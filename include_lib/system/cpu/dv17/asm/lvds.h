#ifndef __LVDS_H__
#define __LVDS_H__

#include "typedef.h"
#include "asm/imd_common.h"
#include "asm/port_waked_up.h"


#define LVDS_TX_CLK_EN          lvds_tx_con |= BIT(0)
#define LVDS_TX_CLK_DIS         lvds_tx_con &=~BIT(0)
#define LVDS_NORMAL             lvds_tx_con |= BIT(1)
#define LVDS_RESET              lvds_tx_con &=~BIT(1)
#define LVDS_KICK_START         lvds_tx_con |= BIT(6)
#define LVDS_CLR_PND            lvds_tx_con |= BIT(7)
#define LVDS_IE_EN              lvds_tx_con |= BIT(19)
#define LVDS_IE_DIS             lvds_tx_con &=~BIT(19)
#define LVDS_TX_RESET           lvds_tx_con &=~BIT(21)
#define LVDS_TX_NORMAL          lvds_tx_con |= BIT(21)
#define LVDS_TX_PND             (lvds_tx_con & BIT(23))

#define LVDS_POLAR_DEFAULT      0x00
#define LVDS_POLAR_DEN_INV      0x01
#define LVDS_POLAR_VSYNC_INV    0x02
#define LVDS_POLAR_HSYNC_INV    0x04

#define LVDS_PLL_DIVA_1         0x01
#define LVDS_PLL_DIVA_3         0x02
#define LVDS_PLL_DIVA_5         0x04
#define LVDS_PLL_DIVA_7         0x08

#define LVDS_PLL_DIVB_1         0x10
#define LVDS_PLL_DIVB_2         0x20
#define LVDS_PLL_DIVB_4         0x40
#define LVDS_PLL_DIVB_8         0x80

enum {
    LVDS_MODE_LOOP,
    LVDS_MODE_SIGNAL,
};

enum {
    LVDS_UNIT_LINE,
    LVDS_UNIT_PIXEL,
};

enum {
    LVDS_SINGAL_6BIT,
    LVDS_SINGAL_8BIT,
};

enum {
    LVDS_DATA_JEIDA,
    LVDS_DATA_VSEA,
};

#define LVDS_LANE_DIS   0
#define LVDS_LANE_CLK   1
#define LVDS_LANE_D0    2
#define LVDS_LANE_D1    3
#define LVDS_LANE_D2    4
#define LVDS_LANE_D3    5

struct lvds_lane_mapping {
    u8 x0_lane;
    u8 x1_lane;
    u8 x2_lane;
    u8 x3_lane;
    u8 x4_lane;
    u8 swap_dp_dn;
};

struct lvds_timing {
    u16 hori_total;
    u16 hori_sync;
    u16 hori_back_porth;
    u16 hori_pixel;

    u16 vert_total;
    u16 vert_sync;
    u16 vert_back_porth;
    u16 vert_pixel;
};

/*
 * MIPI PLL时钟分频
 */
enum LVDS_PLL_DIV {
    LVDS_PLL_DIV1,
    LVDS_PLL_DIV2,
    LVDS_PLL_DIV4,
    LVDS_PLL_DIV8,
};

struct lvds_dev {
    struct imd_dmm_info info;
    struct te_mode_ctrl te_mode;
    struct pll4_info pll4;
    struct lvds_lane_mapping mapping;
    struct lvds_timing timing;
    u8 lvds_div;
    u8 singal_polar;
    u8 frame_mode;
    u8 unit;
    u8 clk_phase;
    u8 interface;
    u8 data_format;
};

void lvds_init(struct lvds_dev *dev);
void lvds_kick_start();

#endif
