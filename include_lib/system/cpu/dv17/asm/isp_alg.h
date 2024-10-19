

#ifndef __ISP_ALG_H__
#define __ISP_ALG_H__

#include "typedef.h"

enum {
    ISP_ISO_100 = 0,
    ISP_ISO_200,
    ISP_ISO_400,
    ISP_ISO_800,
    ISP_ISO_1600,
    ISP_ISO_3200,
    ISP_ISO_6400,
    ISP_ISO_AUTO = 0xff,
};
#define ISP_ISO_MAX   (ISP_ISO_6400+1)


//AE
#define AE_CURVE_END                ((unsigned int)(-1))
#define AE_CURVE_INFO_MAX                 4


enum {
    AE_INTERP_EXP = 1,
    AE_INTERP_GAIN = 2,
};

enum {
    AE_CURVE_50HZ = 0,
    AE_CURVE_60HZ,
};

typedef enum {
    AE_WIN_WEIGHT_AVG = 0,  //  平均测光
    AE_WIN_WEIGHT_CENTRE,   //  中央区域测光
    AE_WIN_WEIGHT_SPOT,     //  点测光
    AE_WIN_WEIGHT_MULTI,    //  多区域测光
    AE_WIN_WEIGHT_CARDV,    //  行车记录仪专用
    AE_WIN_WEIGHT_CUST,
} isp_ae_ww_type_t;

#define AE_WIN_WEIGHT_MAX  AE_WIN_WEIGHT_CUST


typedef struct {
    u32 ae_exp_line;
    u32 ae_exp_clk;
    u32 ae_gain;

    u32 ae_exp_line_short;
    u32 ae_exp_clk_short;
    u32 ae_gain_short;

    u32 isp_d_gain;
} isp_ae_shutter_t;

typedef struct {
    u32(*calc_shutter)(isp_ae_shutter_t *shutter, u32 exp_time, u32 gain);
    u32(*set_shutter)(isp_ae_shutter_t *shutter);
    void   *(*get_ae_curve)(u32 type,  u32 fps, int *min_ev, int *max_ev);
} isp_ae_ops_t;

typedef struct ae_params {

    u32 ae_max_ev;
    u32 ae_min_ev;
    u32 ae_init_ev;

    u32 ae_curve_type;

    u32 ae_target;

    u32 ae_phase_comp;

    u32 ae_luma_smooth_num;

    u32 ae_conver_h;
    u32 ae_conver_l;
    u32 ae_diver_h;
    u32 ae_diver_l;

    u32 ae_ratio_max_h;
    u32 ae_ratio_max_l;

    u32 ae_ratio_max2_h;
    u32 ae_ratio_max2_l;

    u32 ae_ratio_slope;

    u32 ae_expline_th;

    u32 *ae_user_ev_value_table;
    u32 ae_user_ev_value_table_size;
    s32 ae_user_ev_value; // centered to zero

    u8 ae_init_weight_type;
    u8 *ae_win_cust_weights;

    isp_ae_ops_t ae_ops;

    u32 ae_step;

    u32 ae_hl_comp_en;
    u32 ae_hl_count_th0;
    u32 ae_hl_count_th1;
    u32 ae_hl_lv_th;
    u32 ae_hl_comp_max;

    u32 reserved[11];

} isp_ae_params_t;


//AWB
typedef enum {
    AWB_DAYLIGHT,  // 晴天5600K
    AWB_CLOUNDY,       //  多云/阴天 D65 6500K
    AWB_TUNGSTEN,      // 钨丝灯/白炽灯 TL83/U30 3000K
    AWB_FLUORESCENT1,    // 荧光灯1 D65 6500K
    AWB_FLUORESCENT2,    // 荧光灯2 TL84 4000K
    AWB_AUTO,
} isp_wb_type_t;

#define AWB_SCENE_MAX   AWB_AUTO

enum {
    AWB_ALG_GW1 = 0,
    AWB_ALG_HYBRID,
};

typedef enum {
    AWB_WIN_WEIGHT_AVG = 0,  //  平均测光
    AWB_WIN_WEIGHT_CENTRE,   //  中央区域测光
    AWB_WIN_WEIGHT_SPOT,     //  点测光
    AWB_WIN_WEIGHT_MULTI,    //  多区域测光
    AWB_WIN_WEIGHT_CARDV,    //  行车记录仪专用
    AWB_WIN_WEIGHT_CUST,
} isp_awb_ww_type_t;


// 白平衡
typedef struct {
    u8 wb_type;     // 白平衡类型
    u8 wb_matrix;   // 权重矩阵类型
    u8 wc[64];    // 自定义权重矩阵   0 ~ 255
} isp_wb_t;

// 训练类型
typedef enum {
    AWB_NO_TRAIN = 0,       // 无训练
    AWB_TRAIN_DAYLIGHT,     // 晴天
    AWB_TRAIN_CLOUNDY,      // 多云/阴天
    AWB_TRAIN_TUNGSTEN,     // 钨丝灯/白炽灯
    AWB_TRAIN_FLUORESCENT1, // 荧光灯1
    AWB_TRAIN_FLUORESCENT2, // 荧光灯2
} isp_wb_train_type_t;


// 预设白平衡训练
typedef struct {
    u16 train;      // 训练类型
    u16 gain[5][3]; // rgb gain 0 ~ 4096
} isp_wb_train_t;



#define AWB_WIN_WEIGHT_MAX  AWB_WIN_WEIGHT_CUST

#define ISP_AWB_ONE      (1<<10)
typedef struct {
    u16 r_gain;
    u16 g_gain;
    u16 b_gain;
} isp_wb_gain_t;

typedef struct {
    s16 wp_th;
    u8 r_th;
    u8 g_th;
    u8 b_th;

    u16 wp_ratio_th_numerator;
    u16 wp_ratio_th_denominator;

    u16 rgain_min;
    u16 rgain_max;
    u16 bgain_min;
    u16 bgain_max;
    u32 ev_th;

    u32 ultralow_th;
    u16 ultralow_rgain;
    u16 ultralow_bgain;
} isp_awb_gw1_params_t;


typedef struct {
    u8 awb_alg_type;
    u8 awb_scene_type;
    u8 awb_win_type;

    isp_wb_gain_t  awb_init_gain;
    isp_wb_gain_t  awb_scene_gains[AWB_SCENE_MAX];

    u8 *awb_win_cust_weights;

    isp_awb_gw1_params_t *awb_gw1_params;

    char *awb_bin_path;

} isp_awb_params_t;

typedef struct {
    s16     r;
    s16     gr;
    s16     gb;
    s16     b;
} isp_blc_t;

typedef struct {
    u16     dth;
    u16     sth;
    u16     vth;
} isp_dpc_t;

typedef struct {
    u16     cx;
    u16     cy;
    u16     *lr;
    u16     *lg;
    u16     *lb;
    u16     dp0;
    u16     dp1;
    u16     dth;
} isp_lsc_t;

typedef struct {
    u8      en;
    u8      ss;
    u8      st;
    u16     sth;
    u16     tth;
    u32     eb[8];
    u32     np[8];
} isp_bnr_t;

typedef struct {
    u8      en3d;
    u8      level;
    u8      wth;
    u8      sth;
    u16     mw;
    u8      eth;
    u8      mode;
    u16      esen0;
    u16      esen1;
} isp_tnr_t;

typedef s16(isp_ccm_t)[12];

typedef u8  *isp_gamma_t;

typedef struct {
    u32 enable;
    u8  *r_gamma;
    u8  *g_gamma;
    u8  *b_gamma;
} isp_three_gamma_t;

typedef struct {
    u8      en;
    u8      cen;
    u8      level;
    u8      wth;
    u8      sth;
    u16     mw;
    u32     cx;
    u32     cy;
    u32     rcoff;
    u32     rclvl;
    u32     rcmax;
} isp_nr_t;

typedef struct {
    u8      en;
    u8      lth;
    u8      mftype;
    u8      mfstr;


    u16     ha0;
    u16     ha1;
    u16     ma0;
    u16     ma1;
    u16     hth0;
    u16     hth1;
    u16     mth0;
    u16     mth1;
    u16     h_smooth;
    u16     m_smooth;
    u16     smooth_level;



    u8     agg_mode;
    u8     agg_factor;



    u8      luma_scale[8];

    s16     hcoeffs[9];
    s16     mcoeffs[9];

    u8     hshift;
    u8     mshift;


    u8      ctype;
    u8      cmin;
    u8      cmax;

    u8     pad[3];
} isp_shp_t;

typedef struct {
    u8      men;
    u8      th;
    u8      s/*mul 10*/;
} isp_cnr_t;


typedef struct {
    u8      mode;
    // u8      pad1[1];
    u8     *curve;  /* 256个 */
    // u8      pad1[2];
} isp_wdr_t;

typedef struct {
    u32    T;
    u32    K;
    u32    th1;
    u32    th2;
    u16 weight_scene1l;
    u16 weight_scene1h;
    u16 weight_scene2l;
    u16 weight_scene2h;
    u16 weight_scene3l;
    u16 weight_scene3h;
    u16 weight_scene4l;
    u16 weight_scene4h;
} isp_drc_hist_t;

typedef struct {
    u8      en;
    u8      ysel;
    u16     level;
} isp_ltmo_t;

typedef struct {
    u32     g;
} isp_dgain_t;

typedef struct {
    u16     gc0;
    u16     gc1;
    u16     gc2;
    s16     oc0;
    s16     oc1;
    s16     oc2;
    s16     cos;
    s16     sin;
    u8      th0;
    u8      th1;
    u8      th2;
    u8      th3;
    u8      uv_th;
    u8      en;
} isp_adj_t;

typedef struct {
#ifdef  ISP_TEST_ENABLE
    u32     sg;
    u32     lg;
    u32     bypass;
#endif
    u32     mc;
    u32     lth;
    u32     hth;
    s16     ob;
    u16      paddings;
    u8      *w;
} isp_hdr_t;

typedef struct {
    u8      en;
    // u8      pad1[3];
    u32     sw0;
    u32     sw1;
    u32     sth0;
    u32     sth1;
    u8      hdw;
    // u8      pad2[1];
    s16     *lut; // 24*2个
    // u8      pad3[2];
} isp_hsv_t;

typedef struct {
    u16 mode;
    u16 cx;
    u16 cy;
    s16 ob;
    u16 *ir;
    u16 *ig;
    u16 *ib;
    u16 gain;
    u16 overexp_th;


} isp_rgbir_t;

typedef struct {
    s16 wp_th;
    u8 r_min;
    u8 r_max;
    u8 g_min;
    u8 g_max;
    u8 b_min;
    u8 b_max;
    u8 y_min;
    u8 y_max;
    u8 paddings[2];
} isp_awb_stat_param_t;

typedef struct {
    u16 lv[8];
} isp_ae_stat_param_t;


// 自动曝光
typedef struct {
    u8 ww_type;     // 测光矩阵类型   0 ~ 6
    s8 ev;          // 曝光补偿值   -3 ~ 3
    u16 luma;       // 期望均值     0 ~ 4095
    u8 vw[64];    // 自定义测光矩阵 0 ~ 255
} isp_autoexp_t;



typedef struct {

#ifdef  ISP_TEST_ENABLE

    isp_ae_stat_param_t     ae_stat;
    isp_awb_stat_param_t    awb_stat;
#endif

    isp_rgbir_t         rgbir;
    isp_blc_t           blc;
    isp_lsc_t           lsc;
    isp_wdr_t           wdr;
    isp_drc_hist_t      drc_hist;
    isp_ltmo_t          ltmo;

    isp_adj_t           adj;
    isp_gamma_t         gamma;
    isp_gamma_t         y_gamma;

    isp_ccm_t           ccm;

    isp_dpc_t           dpc;
    isp_bnr_t           bnr;
    isp_tnr_t           tnr;
    isp_nr_t            nr;
    isp_shp_t           shp;
    isp_cnr_t           cnr;
    isp_hsv_t           hsv;
    isp_hdr_t           hdr;

    isp_dgain_t         lgain;
    isp_dgain_t         sgain;
    isp_wb_gain_t       prewb;
    isp_wb_gain_t       postwb;

    u32                 md_wms[5];
    u32                 md_level;
    isp_three_gamma_t       three_gamma;
} isp_iq_params_t;


void  isp_ae_curve_interp(u32(*ae_curve)[AE_CURVE_INFO_MAX], u32 ev, u32 *time, u32 *gain/*q10*/);

#endif
