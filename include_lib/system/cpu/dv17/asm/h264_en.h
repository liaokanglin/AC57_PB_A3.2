#ifndef __H264_H__
#define __H264_H__

#include "asm/cpu.h"
#include "typedef.h"
#include "os/os_api.h"
#include "system/includes.h"

#define SLICE_TYPE_I    2
#define SLICE_TYPE_P    0
#define AVC_NUM 2
/*#define MAX_WIDTH 352
#define MAX_HEIGHT 288*/

#define AVC_BS_LEN  100*1024
// #define ROUND_BUFFER
#define REF_DOUBLE_BUF
#define Print_stream 0
#define Print_encdata 0
#define Cover_addr 0

#define X264_MIN(a,b) ((a) > (b) ? (b) : (a))
#define CACHE2UNCACHE(addr) {u32 t=addr; t = t & (~0x4000000);   t = t | 0x8000000; addr=t;}
#define UNCACHE2CACHE(addr) {u32 t=addr; t = t & (~0x8000000);   t = t | 0x4000000; addr=t;}

#define AVC_FTYPE_NORMAL		1
#define AVC_FTYPE_PSKIP			2

enum {
    AVC_STA_OPEN = 1,
    AVC_STA_START,
    AVC_STA_STOPING,
    AVC_STA_STOP,
    AVC_STA_PAUSE,
};

#define IFRAME_CHECK_EN					0
#define H264_ENC_STATE_FAIL_TH           2
#define H264_ENC_STATE_HISTORY_MAX      64

enum h264_irq_code {
    H264_IRQ_REF_FRAME_ERR,
    H264_IRQ_BUFFER_FULL,
    H264_IRQ_SPEED_INTEN,
    H264_IRQ_NO_BUFF,
    H264_IRQ_MCU_PEND,
    H264_IRQ_NEXT_PS,
};

enum h264_frame_type {
    H264_P_FRAME = 0,
    H264_N_FRAME = 1,
    H264_I_FRAME = 2,
};


struct h264_user_ops {
    void *(*malloc)(void *priv, u32 size);
    void *(*realloc)(void *priv, void *buf, u32 size);
    void (*free)(void *priv, void *buf);
    int (*size)(void *priv) ;
    void (*output_frame_end)(void *priv, void *buf, enum h264_frame_type);
};

typedef struct avc_sfr {
    volatile u32 BUF_NUM        ;
    volatile u32 CON1           ;
    volatile u32 DB_OFFSET      ;
    volatile u32 MV_RANGE       ;
    volatile u32 CON2           ;
    volatile u32 YENC_BASE_ADR0 ;
    volatile u32 UENC_BASE_ADR0 ;
    volatile u32 VENC_BASE_ADR0 ;
    volatile u32 YENC_BASE_ADR1 ;
    volatile u32 UENC_BASE_ADR1 ;
    volatile u32 VENC_BASE_ADR1 ;
    volatile u32 LDSZA_BASE_ADR;
    volatile u32 LDSZB_BASE_ADR;
    volatile u32 LDCPA_BASE_ADR;
    volatile u32 LDCPB_BASE_ADR;
    volatile u32 WBSZA_BASE_ADR;
    volatile u32 WBSZB_BASE_ADR;
    volatile u32 WBCPA_BASE_ADR;
    volatile u32 WBCPB_BASE_ADR;

    volatile u32  CPA_START_ADR ;
    volatile u32  CPA_END_ADR   ;
    volatile u32  CPB_START_ADR ;
    volatile u32  CPB_END_ADR   ;

    volatile u32 BS_ADDR        ;
    volatile u32 QP_CON0        ;
    volatile u32 QP_CON1        ;
    volatile u32 ROI0_XY        ;
    volatile u32 ROI1_XY        ;
    volatile u32 ROI2_XY        ;
    volatile u32 ROI3_XY        ;
    volatile u32 ROIX_QP        ;
    volatile u32 VAR_MUL16      ;
    volatile u32 DECIMATE       ;
    volatile u32 BCNT           ;
    volatile u32 HEADER_CON     ;
    volatile u32 TOTAL_MB_COST  ;
    volatile u32 INTER_WEIGHT   ;
    volatile u32 DEADZONE       ;
    volatile u32 HAS_BUF        ;
    volatile u32 FS_RANGE       ;
    volatile u32 DCT_DENOISE    ;
    volatile u32 PRESKIP_DECIMATE;
    volatile u32 BS_SUN ;
    volatile u32 CON3         ;
    volatile u32 DBDAT_CHECKSUM;
    volatile u32 LOAD_ENC_CNT;
    volatile u32 LOAD_FREF_ENC;
    volatile u32 PSKIP_TH;
    volatile u32 CON4           ;
} avc_sfr_t ;


typedef struct {
    int frame_num;
    int frame_tag;
    int key_frame;
    int idr_pic_id;

    int i_deblocking_filter_alphac0;
    int i_deblocking_filter_beta;
    int i_disable_deblocking_filter_idc;
    int i_alpha_c0_offset;
    int i_beta_offset;
    int b_deblocking_filter_control;
    int b_deblocking_filter;

    int transform_8x8_flag;
    int intra8x8_en;
    int roi_en;
    int roi0_qp_adj;
    int roi1_qp_adj;
    int roi2_qp_adj;
    int roi3_qp_adj;
    int pre_pskip_en;
    int pre_pskip_limit_flag;
    int common_pskip_limit_flag;
    int pre_pskip_th;
    int common_pskip_th;

    int dct_coeff;
    int inter_cost_offset;

    int decimate_chroma;
    int decimate_luma_16x16;
    int decimate_luma_8x8;
    int decimate_prepskip_chroma;
    int decimate_prepskip_luma_16x16;
    int decimate_prepskip_luma_8x8;

    int deadzone1;
    int deadzone0;
    int i_luma_deadzone0;
    int i_luma_deadzone1;

    int ime_x_range;
    int ime_y_range;

    int buf_max_size;
    int qp_min;
    int qp_max;
    int i_pic_init_qp;
    int i_pic_init_qs;
    int i_global_qp;
    int qpadj_en;
    int qpadj_mb_en;
    int rc_aq_autovariance;
    int rc_aq_strength;
    int i_chroma_qp_index_offset;

    int qp_clip_max;
    int qp_clip_min;
    int dsw_limit_pos;
    u8 dsw_size;

    int enc_no_ref_frame_en;
    int no_ref_interval_num;
    int ref_flag;
    int ref_id;
    int b_gaps_in_frame_num_value_allowed;
    int i_num_ref_idx_l0_active;
    int i_last_ref_frame_num;
    int no_ref_num_stat;
    int no_ref_num_tmp;
    int no_ref_flag;
    int pre_no_ref_flag;
    int i_num_ref_frames;
    int i_ref0;
    int i_poc;
    int frames_reference;
    int pskip_frame_flag;
    int pre_pskip_frame_flag;
    int irq_speed_flag;
    u32 ref_len;


    int yuv_pic_w;
    int yuv_pic_h;
    int i_mb_width;
    int i_mb_height;
    int i_log2_max_frame_num;

    int i_fps_num;
    int i_fps_den;

    int bs_buf_len;
    int sps_addr, pps_addr, sheader_addr;
    int sps_len, pps_len ;

    // int enc_data_addr;
    // u16  enc_buf_num;
    // int enc_data_addr_hw;
    int stream_addr;
    // int stream_addr_hw;
    // int ref_ini_addr;
    // int ref_ini_addr_hw;

    // u32 ratio;
    // u32 ratio1;
    // u32 ratio2;
    // u32 ratio3;
    u32 write_cnt;
    u32 header_cnt;
    u8  reset_abr;
    //u8  frame_type;
    float f_aq_strength;
    u8  b_dct_decimate;
    u8  b_dct_decimate_prepskip;
    u8  i_cabac_init_idc;
    u8  i_level_idc;
    u8  i_profile_idc;
    u8 ch;
    u8 owe ;
    // u8 need_updata_ref_frame;
    volatile u8 status;
    bool auto_idr ;
    void *priv;
    const struct h264_user_ops *ops;
    avc_sfr_t *sfr;
    u32 max_bs_len ;
    OS_SEM sem_stop ;
    // void *abr ;
    u32 i_bitrate;
    u8 static_min;
    u8 static_max;
    u8 dynamic_min;
    u8 dynamic_max;
    u8 enable_change_qp;
    void *abr_handle ;
    u32 i_last_idr ;
    u32 i_keyint_min;
    u32 i_keyint_max ;
    u32 asc_mb_cost_threshold ;
    s64 total_bytes;
    s64 frame_bytes;
    // u8 *ref_cp_a;
    // u8 *ref_cp_b;
    // u8 *ref_sz_a;
    // u8 *ref_sz_b;
    u8 *ref_addr ;

    // int drop_frame;

    u8 b_crop;
    int crop_left;
    int crop_right;
    int crop_top;
    int crop_bottom;

    int skip_stream_addr;
    int skip_stream_len;
    u8 manual_skip;
    u8 special_addr;
    u8 force_skip;

    // u8 bsta;
    // u8 fill_forbidden;
    // u32 fill_msecs;
    // u32 fill_one_sec_msecs;
    // int frm_fill;
    // int frm_one_sec;
    // u32 fill_interval;
    // u32 fill_continue;
    // u32 fill_fbase;
    // u32 fill_one_sec_fbase;
    // u32 fill_fnum;
    // u32 fill_cnt_fnum;
    // u32 fill_secs;
    // volatile u8 fill_enable;
    // volatile u8 fill_cri;
#if IFRAME_CHECK_EN
    u8 state_history_win[H264_ENC_STATE_HISTORY_MAX];
    s32 state_fail_cnt;
    u32 state_history_win_idx;
#endif

    // u32 cycle_time;
    // u32 cfnum;
    // u32 bk_cycle_time;
    // u32 cyc_ctime;
    // u8 cyc_flag;

    u8 last_frame_type;
    u8 std_header;
    u8 next_iframe;
    u8 cap_iframe;
    u8 change_bitrate;
    u8 time_stamp;
    u8 real_fps;
    enum h264_frame_type frame_type;
    struct roi_cfg roi;
    struct h264_user_data user_data;
    void *irq_priv;
    int (*irq_handler)(void *, enum h264_irq_code, enum h264_frame_type);
    int manu_enc;
    int manu_enc_err;
    OS_SEM manu_sem;
    u32 precpa;
    u32 precpb;
    u8 resume;
    struct video_cap_buffer input_b;
    struct app_enc_info app_avc_cfg;
} H264_EN_VAR;

typedef struct {
    int level_idc;
    int mbps;        /* max macroblock processing rate (macroblocks/sec) */
    int frame_size;  /* max frame size (macroblocks) */
    int dpb;         /* max decoded picture buffer (bytes) */
    int bitrate;     /* max bitrate (kbit/sec) */
    int cpb;         /* max vbv buffer (kbit) */
    int mv_range;    /* max vertical mv component range (pixels) */
    int mvs_per_2mb; /* max mvs per 2 consecutive mbs. */
    int slice_rate;  /* ?? */
    int bipred8x8;   /* limit bipred to >=8x8 */
    int direct8x8;   /* limit b_direct to >=8x8 */
    int frame_only;  /* forbid interlacing */
} x264_level_t;

static const x264_level_t x264_levels[] = {
    { 10,   1485,    99,   152064,     64,    175,  64, 64,  0, 0, 0, 1 },
//  {"1b",  1485,    99,   152064,    128,    350,  64, 64,  0, 0, 0, 1 },
    { 11,   3000,   396,   345600,    192,    500, 128, 64,  0, 0, 0, 1 },
    { 12,   6000,   396,   912384,    384,   1000, 128, 64,  0, 0, 0, 1 },
    { 13,  11880,   396,   912384,    768,   2000, 128, 64,  0, 0, 0, 1 },
    { 20,  11880,   396,   912384,   2000,   2000, 128, 64,  0, 0, 0, 1 },
    { 21,  19800,   792,  1824768,   4000,   4000, 256, 64,  0, 0, 0, 0 },
    { 22,  20250,  1620,  3110400,   4000,   4000, 256, 64,  0, 0, 0, 0 },
    { 30,  40500,  1620,  3110400,  10000,  10000, 256, 32, 22, 0, 1, 0 },
    { 31, 108000,  3600,  6912000,  14000,  14000, 512, 16, 60, 1, 1, 0 },
    { 32, 216000,  5120,  7864320,  20000,  20000, 512, 16, 60, 1, 1, 0 },
    { 40, 245760,  8192, 12582912,  20000,  25000, 512, 16, 60, 1, 1, 0 },
    { 41, 245760,  8192, 12582912,  50000,  62500, 512, 16, 24, 1, 1, 0 },
    { 42, 522240,  8704, 13369344,  50000,  62500, 512, 16, 24, 1, 1, 1 },
    { 50, 589824, 22080, 42393600, 135000, 135000, 512, 16, 24, 1, 1, 1 },
    { 51, 983040, 36864, 70778880, 240000, 240000, 512, 16, 24, 1, 1, 1 },
    { 0 }
};

static const u8 x264_ue_size_tab[256] = {
    1, 1, 3, 3, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
};

static u8 nal_tag[4] = {0, 0, 0, 1};

extern H264_EN_VAR x264_en_st0;
extern void x264_sps_write(H264_EN_VAR *x264);
extern void x264_pps_write(H264_EN_VAR *x264);
extern void slice_header_write(H264_EN_VAR *x264);
void x264_sps_write_line0(H264_EN_VAR *x264);
void x264_pps_write_line0(H264_EN_VAR *x264);
void slice_header_write_line0(H264_EN_VAR *x264, enum h264_frame_type, u8 pskip);
void x264_sps_write_line1(H264_EN_VAR *x264);
void x264_pps_write_line1(H264_EN_VAR *x264);
void slice_header_write_line1(H264_EN_VAR *x264);


struct h264_enc_info {
    u16 width;
    u16 height;
    struct roi_cfg roi;
    u8  fps;
    u8 real_fps;
    u8 fill_forbidden;
    u8  key_frame_num;
    u8  yuv_buf_num;
    u32 yuv_base;
    u32 i_bitrate ;
    u32 std_head;
    u32 cycle_time;
    u8 quality;
    u8 time_stamp_en;
    void *priv;
    const struct h264_user_ops *ops;
};



void *h264_enc_open(struct video_format *, int mode);
// int h264_enc_manu(void *fd, u32 yuv_base, u32 yuv_buf_num, u8 ip_flag);
int h264_enc_manu(void *fd, u32 yuv_base, u32 yuv_buf_num, u8 ip_flag, int (*pskip_cb)(void *), void *priv);
int h264_enc_set_resume_handler(void *_fh, void *priv, int (*handler)(void *));
int h264_enc_set_pause_handler(void *_fh, void *priv, int (*handler)(void *));
void h264_enc_init();

int h264_enc_get_s_attr(void *_fh, struct h264_s_attr *attr);
int h264_enc_set_s_attr(void *_fh, struct h264_s_attr *attr);

int h264_enc_get_d_attr(void *_fh, struct h264_d_attr *attr);
int h264_enc_set_d_attr(void *_fh, struct h264_d_attr *attr);

int h264_enc_set_output_fps(void *_fh, int fps);

// int h264_enc_set_input_buf(void *_fh, u8 *buf, int num);
int h264_enc_set_input_buf(void *_fh, struct video_cap_buffer *b);

void h264_enc_set_output_buf_ops(void *_fh, void *, const struct h264_user_ops *ops);

int h264_enc_set_irq_handler(void *_fh, void *priv,
                             int (*handler)(void *, enum h264_irq_code, enum h264_frame_type));

int h264_enc_start(void *_fh);

int h264_enc_kstart(void *_h264);

int h264_enc_start_I_frame(void *_x264, int reset);

int h264_enc_start_P_frame(void *_x264);

int h264_enc_start_PS_frame(void *_x264, int mode);

int h264_enc_reset_abr(void *fd);

int h264_enc_wait_stop(void *_x264);

int h264_enc_stop(void *_x264, int mode);

int h264_enc_close(void *_x264);

int h264_enc_pause(void *_h264);
// int h264_enc_resume(void *_h264, void *buf, int num);
int h264_enc_resume(void *_h264, struct video_cap_buffer *b);


void *avc_enc_open(struct h264_enc_info *info);
void avc_enc_cap_iframe(void *fd);

int avc_enc_start(void *fd);

int avc_enc_restart(void *fd);
int avc_enc_rekstart(void *fd);
void avc_enc_wait_end(void *fd);

int avc_enc_change_bitrate(void *fd, u32 bitrate);
int avc_enc_reset(void *fd);
int avc_enc_reset_abr(void *fd);
int avc_enc_set_count_down(void *fd, u32 time);
int avc_enc_set_cycle_time(void *fd, int time) ;

int avc_enc_stop(void *fd);
int avc_enc_pause(void *fd);
// int avc_enc_continue(void *fd, u8 yuv_buf_num, u32 yuv_base);
int avc_enc_continue(void *fd, struct video_cap_buffer *b);

int avc_enc_close(void *fd);
int avc_encode_frame_I(H264_EN_VAR *x264, u8 resetbuf) ;
int avc_encode_frame_P(H264_EN_VAR *x264, u8 resetbuf) ;
void build_skip_frame(H264_EN_VAR *x264) ;
void avc_slice_coding(H264_EN_VAR *x264) ;
int avc_enc_get_fnum(H264_EN_VAR *x264);
int manual_skip_frame(H264_EN_VAR *x264);
void avc_enc_set_next_iframe(u8 ch);

void avc_imc_reset_down(struct video_endpoint *ep);
void avc_imc_reset_up(struct video_endpoint *ep);
void avc_imc_restart(struct video_endpoint *ep);
void avc_reset_uvc_source(struct video_endpoint *ep, int reset);
void avc_enc_set_targe_fps(void *fd, int fps);

int h264_enc_set_error_handler(void *_fh, void *priv,
                               int (*handler)(void *, enum h264_irq_code, enum h264_frame_type));

int h264_handl2ch(void *_h264);

void h264_enc_append_user_data(void *fd, struct h264_user_data *data);
void h264_enc_append_user_avc(void *fd, struct app_enc_info *data);

int h264_force_skip_frame(void *_x264);
#endif

