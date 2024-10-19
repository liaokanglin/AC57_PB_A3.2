

#ifndef _ISP_DEV_H
#define _ISP_DEV_H

#include "cpu.h"
#include "ioctl.h"
#include "typedef.h"
#include "device_drive.h"
#include "errno-base.h"

#define ISP_ABNORMAL_EVENT  (10000)

typedef struct {
    void *buffer0;
    void *buffer1;
    u32  ishdr;
    u32  size_in_bytes;
} isp_rawdata_t;

/*isp_ioctrl command*/
#define ISP_DEV_MAGIC  			        'i'
#define ISP_CMD_SET_INPUT_SIZE	        _IOW(ISP_DEV_MAGIC, 5, pic_size_t)
#define ISP_CMD_SET_OUTPUT_SIZE	        _IOW(ISP_DEV_MAGIC, 6, pic_size_t)
#define ISP_CMD_GET_SEN_SIZE		    _IOR(ISP_DEV_MAGIC, 7, pic_size_t)
#define ISP_CMD_GET_ISP_SIZE		    _IOR(ISP_DEV_MAGIC, 8, pic_size_t)
#define ISP_CMD_GET_FREQ			    _IOR(ISP_DEV_MAGIC, 9, u8)
#define ISP_CMD_GET_SEN_STATUS	        _IOR(ISP_DEV_MAGIC, 10, u8)
#define ISP_CMD_GET_VSIGNAL	            _IOR(ISP_DEV_MAGIC, 11, u8)

#define ISP_CMD_SET_CAPTURE_MODE		_IOR(ISP_DEV_MAGIC, 12, u32)
#define ISP_CMD_SET_RECORD_MODE		    _IOR(ISP_DEV_MAGIC, 13, u32)

#define ISP_CMD_GET_SEN_CAP_SIZE		_IOR(ISP_DEV_MAGIC, 14, pic_size_t)
#define ISP_CMD_GET_ISP_CAP_SIZE		_IOR(ISP_DEV_MAGIC, 15, pic_size_t)
#define ISP_CMD_GET_CAP_FREQ			_IOR(ISP_DEV_MAGIC, 16, u8)

#define ISP_CMD_GET_SEN_REC_SIZE		_IOR(ISP_DEV_MAGIC, 17, pic_size_t)
#define ISP_CMD_GET_ISP_REC_SIZE		_IOR(ISP_DEV_MAGIC, 18, pic_size_t)
#define ISP_CMD_GET_REC_FREQ			_IOR(ISP_DEV_MAGIC, 19, u8)
#define ISP_CMD_IS_NEED_REMOUNT			_IOR(ISP_DEV_MAGIC, 20, u8)
#define ISP_CMD_SET_CROP_SIZE           _IOR(ISP_DEV_MAGIC, 21, u32)
#define ISP_CMD_SET_CROP_SIZE_NORESET   _IOR(ISP_DEV_MAGIC, 22, u32)
#define ISP_CMD_GET_CROP_SIZE           _IOR(ISP_DEV_MAGIC, 23, u32)

#define ISP_CMD_SET_FPS                 _IOW(ISP_DEV_MAGIC, 24, u32)

#define ISP_CMD_SET_TEST_FRAME_KICK     _IOW(ISP_DEV_MAGIC, 25, u32)
#define ISP_CMD_GET_TEST_FRAME_DONE     _IOR(ISP_DEV_MAGIC, 26, u32)
#define ISP_CMD_GET_RAWDATA             _IOR(ISP_DEV_MAGIC, 27, isp_rawdata_t)
#define ISP_CMD_GET_REAL_FREQ           _IOR(ISP_DEV_MAGIC, 28, u32)

#define ISP_CMD_GET_SEN_NAME            _IOR(ISP_DEV_MAGIC, 29, u8*)


typedef enum  {
    ISP_RECORD_MODE,
    ISP_CAPTURE_MODE,
} isp_cap_rec_mode_t;

typedef enum  {
    SEN_INTERFACE0,
    SEN_INTERFACE1,
    SEN_INTERFACE_CSI2,
} isp_sen_if_t;

/*sensor in format*/
typedef enum {
    SEN_IN_FORMAT_RGGB = 0,
    SEN_IN_FORMAT_GRBG,
    SEN_IN_FORMAT_GBRG,
    SEN_IN_FORMAT_BGGR,
    SEN_IN_FORMAT_YUYV,
    SEN_IN_FORMAT_YVYU,
    SEN_IN_FORMAT_UYVY,
    SEN_IN_FORMAT_VYUY,
    SEN_IN_FORMAT_RGB,
    SEN_IN_FORMAT_DSI_YUV16,
    SEN_IN_FORMAT_DSI_RGB,
} sen_in_format_t;

/*S_ISP_SEN : out_format*/
/*isp out format*/
typedef enum {
    ISP_OUT_FORMAT_YUV = 0,
    ISP_OUT_FORMAT_RAW,
} isp_out_format_t;

#define SEN_MBUS_HSYNC_ACTIVE_HIGH             (1 << 2)
#define SEN_MBUS_HSYNC_ACTIVE_LOW              (1 << 3)
#define SEN_MBUS_VSYNC_ACTIVE_HIGH             (1 << 4)
#define SEN_MBUS_VSYNC_ACTIVE_LOW              (1 << 5)
#define SEN_MBUS_PCLK_SAMPLE_RISING            (1 << 6)
#define SEN_MBUS_PCLK_SAMPLE_FALLING           (1 << 7)
#define SEN_MBUS_DATA_ACTIVE_HIGH              (1 << 8)
#define SEN_MBUS_DATA_ACTIVE_LOW               (1 << 9)
#define SEN_MBUS_DATA_WIDTH_8B                 (1 << 10)
#define SEN_MBUS_DATA_WIDTH_10B                (1 << 11)
#define SEN_MBUS_DATA_WIDTH_12B                (1 << 12)
#define SEN_MBUS_DATA_REVERSE                  (1 << 13)
/* FIELD = 0/1 - Field1 (odd)/Field2 (even) */
#define SEN_MBUS_FIELD_EVEN_HIGH               (1 << 14)
/* FIELD = 1/0 - Field1 (odd)/Field2 (even) */
#define SEN_MBUS_FIELD_EVEN_LOW                (1 << 15)

#define SEN_MBUS_FIELDS                        (SEN_MBUS_FIELD_EVEN_HIGH | SEN_MBUS_FIELD_EVEN_LOW)

/* Serial flags */
/* How many lanes the client can use */
#define SEN_MBUS_CSI2_1_LANE                   (1 << 0)
#define SEN_MBUS_CSI2_2_LANE                   (1 << 1)
#define SEN_MBUS_CSI2_3_LANE                   (1 << 2)
#define SEN_MBUS_CSI2_4_LANE                   (1 << 3)
/* On which channels it can send video data */
#define SEN_MBUS_CSI2_CHANNEL_0                (1 << 4)
#define SEN_MBUS_CSI2_CHANNEL_1                (1 << 5)
#define SEN_MBUS_CSI2_CHANNEL_2                (1 << 6)
#define SEN_MBUS_CSI2_CHANNEL_3                (1 << 7)
/* Does it support only continuous or also non-continuous clock mode */
#define SEN_MBUS_CSI2_CONTINUOUS_CLOCK         (1 << 8)
#define SEN_MBUS_CSI2_NONCONTINUOUS_CLOCK      (1 << 9)

#define SEN_MBUS_CSI2_LANES            (SEN_MBUS_CSI2_1_LANE | SEN_MBUS_CSI2_2_LANE | \
                                         SEN_MBUS_CSI2_3_LANE | SEN_MBUS_CSI2_4_LANE)
#define SEN_MBUS_CSI2_CHANNELS         (SEN_MBUS_CSI2_CHANNEL_0 | SEN_MBUS_CSI2_CHANNEL_1 | \
                                         SEN_MBUS_CSI2_CHANNEL_2 | SEN_MBUS_CSI2_CHANNEL_3)


#define SEN_MBUS_CSI2_HDR_VC_INV               (1 << 16)

/**
 * enum SEN_mbus_type - media bus type
 * @SEN_MBUS_PARALLEL: parallel interface with hsync and vsync
 * @SEN_MBUS_BT656:    parallel interface with embedded synchronisation, can
 *                      also be used for BT.1120
 * @SEN_MBUS_CSI2:     MIPI CSI-2 serial interface
 */
typedef enum sen_mbus_type {
    SEN_MBUS_PARALLEL,
    SEN_MBUS_BT656,
    SEN_MBUS_BT601,
    SEN_MBUS_BT1120,
    SEN_MBUS_CSI2,
} sen_mbus_type_t;

typedef enum  {
    ISP_DEV_0 = 0x00,
    ISP_DEV_1 = 0x01,
    ISP_DEV_2 = 0x02,
    MCV_DEV = 0x03,
    ISP_DEV_NONE = 0xff,
} isp_id_t;

#define    HDR_EN               BIT(0)
#define    HDR_VC               BIT(1)
#define    HDR_VC_SYNC          BIT(2)
#define    HDR_OV               BIT(3)

#define    ISP_HDR_DISABLE      (0)
#define    ISP_HDR_NVC_OV       (HDR_EN | HDR_OV)
#define    ISP_HDR_NVC_SS       (HDR_EN)
#define    ISP_HDR_VC_OV        (HDR_EN | HDR_VC | HDR_OV)
#define    ISP_HDR_VC_SS        (HDR_EN | HDR_VC)
#define    ISP_HDR_VC_SYNC_OV        (HDR_EN | HDR_VC_SYNC | HDR_OV)
#define    ISP_HDR_VC_SYNC_SS        (HDR_EN | HDR_VC_SYNC)

//rgbir
#define    RGBIR_PATTERN_4X4    0
#define    RGBIR_PATTERN_2X2    1


/*
 * G  B
 * R  I
 */
#define    RGBIR_PHASE_2X2_GBRI    0
#define    RGBIR_PHASE_2X2_BGIR    1
#define    RGBIR_PHASE_2X2_RIGB    2
#define    RGBIR_PHASE_2X2_IRBG    3


/*
 * BGRG
 * GIGI
 * RGBG
 * GIGI
 * */
#define    RGBIR_PHASE_4X4_BGGI    0
#define    RGBIR_PHASE_4X4_GRIG    1
#define    RGBIR_PHASE_4X4_RGGI    2
#define    RGBIR_PHASE_4X4_GBIG    3
#define    RGBIR_PHASE_4X4_GIRG    4
#define    RGBIR_PHASE_4X4_IGGB    5
#define    RGBIR_PHASE_4X4_GIBG    6
#define    RGBIR_PHASE_4X4_IGGR    7

#define RGBIR_MODE_BYPASS           0
#define RGBIR_MODE_CORRECTED        1
#define RGBIR_MODE_INTERPOLATED     2
#define RGBIR_MODE_IR               3

/*isp sensor control function*/
typedef struct {

    u8(*avin_fps)(void *parm);
    u8(*avin_valid_signal)(void *parm);
    u8(*avin_mode_det)(void *parm);

    s32(*sensor_check)(u8 isp_dev, u32 reset_gpio, u32 pwdn_gpio);  /*sensor check online*/

    /*camera init callback function*/
    /*e.g xclk io\frequency init; sensor reset; sensor width\height\format\frame_freq set...*/
    s32(*init)(u8 isp_dev, u16 *width, u16 *height, u8 *format, u8 *frame_freq);
    s32(*set_size_fps)(u16 *width, u16 *height, u8 *freq);  /*sensor size control*/
    s32(*get_mipi_clk)(u32 *mipi_clk, u32 *tval_hstt, u32 *tval_stto, u16 width, u16 height, u8 frame_freq);
    s32(*power_ctrl)(u8 isp_dev, u8 is_work);  /*sensor power control*/

    void *(*get_ae_params)();
    void *(*get_awb_params)();
    void *(*get_iq_params)();

    void (*sleep)();
    void (*wakeup)();
    void (*write_reg)(u16 addr, u16 val);
    u16(*read_reg)(u16 addr);

} sensor_ops_t;

/*isp sensor config parameters*/
typedef struct S_ISP_SEN {
    u8 logo[8];     /*sensor logo*/ /*e.g "GC0308"*/
    isp_id_t    isp_dev;

    u8 in_format;      /*(raw input)0:R; 1:Gr; 2:Gb; 3:B;(yuv input)4:YUYV; 5:UYVY*/

    isp_out_format_t out_format;        /*0:yuv; 1:raw*/

    sen_mbus_type_t mbus_type;
    u32 mbus_config;    /*isp0 sensor config*/

    //hdr
    u32 hdr_mode;
    u32 hdr_prefetch_lines;
    u32 hdr_short_delay_lines;
    u32 hdr_short_gain;
    u32 hdr_long_gain;

    //d_gain
    u32 isp_d_gain_en;
    u32 delay_frame;

    //rgbir
    u32 rgbir_en;
    u32 rgbir_pattern;
    u32 rgbir_phase;

    u32 real_fps; //实际帧率（浮点）q16
    u8 fps;             /*frame frequency*/
    pic_size_t sen_size;    /*sensor size*/
    pic_size_t isp_size;    /*isp output size*/

    u8 cap_fps;             /*frame frequency for capture*/
    pic_size_t sen_cap_size;    /*sensor size for capture*/
    pic_size_t isp_cap_size;    /*isp output size for capture*/

    /*isp sensor control*/
    sensor_ops_t ops;

} isp_sen_t;


extern isp_sen_t camera_dev_begin[];
extern isp_sen_t camera_dev_end[];


#define REGISTER_CAMERA(camera) \
	static isp_sen_t camera sec(.camera_dev)


#define list_for_each_camera(c) \
	for (c=camera_dev_begin; c<camera_dev_end; c++)

s32 isp2_mount(isp_sen_t *parm, void *data) ;
s32 isp2_unmount();
s32 isp2_ioctrl(void *parm, u32 cmd);
s32 isp2_power(u32 mod);


s32 isp1_mount(isp_sen_t *parm, void *data) ;
s32 isp1_unmount();
s32 isp1_ioctrl(void *parm, u32 cmd);
s32 isp1_power(u32 mod);


s32 isp0_mount(isp_sen_t *parm, void *data) ;
s32 isp0_unmount();
s32 isp0_ioctrl(void *parm, u32 cmd);
s32 isp0_power(u32 mod);

s32 isp2_mount(isp_sen_t *parm, void *data) ;
s32 isp2_unmount();
s32 isp2_ioctrl(void *parm, u32 cmd);
s32 isp2_power(u32 mod);


const dev_io_t *get_isp0_ops(void *parm);
const dev_io_t *get_isp1_ops(void *parm);
const dev_io_t *get_isp2_ops(void *parm);



#endif


