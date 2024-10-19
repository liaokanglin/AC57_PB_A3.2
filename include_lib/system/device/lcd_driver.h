#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__

#include "generic/typedef.h"
#include "device/device.h"
#include "asm/cpu.h"
#include "asm/imd.h"
#include "asm/dsi.h"
#include "asm/lvds.h"

#define     LCD_ESD_CHECK_CTRL          _IOW('F', 0, sizeof(int))

extern struct dvp_port {
    enum LCD_MODE drive_mode;
    enum LCD_PORT data_width;
    enum LCD_GROUP port_sel;
} dvp_port;

struct lcd_platform_data {
    const char *lcd_name;
    struct {
        u8 backlight;
        u8 backlight_value;

        u8 lcd_standby;
        u8 lcd_reset;
        u8 lcd_cs;
        u8 lcd_rs;
        u8 lcd_spi_ck;
        u8 lcd_spi_di;
        u8 lcd_spi_do;
    } lcd_io;
    struct {
        struct dsi_lane_mapping  mipi_mapping;
        struct lvds_lane_mapping lvds_mapping;
        struct dvp_port dvp_port;
    } lcd_port;
};

#define LCD_PLATFORM_DATA_BEGIN(data) \
static const struct lcd_platform_data data[2] = { \
{

#define LCD_PLATFORM_DATA_ADD() \
}, \
{  \

#define LCD_PLATFORM_DATA_END() \
} \
};

extern const struct device_operations lcd_dev_ops;


enum {
    ESD_NOACTION,
    ESD_INTERNAL,/*此方法不允许在推屏过程中向屏发送命令*/
    ESD_EXTERNAL,
};

struct esd_deal {
    u8  method;
    /*内部处理的相关参数*/
    u8  deal_per_frames;
    /*外部处理的相关参数*/
    u8  en;
    u16 count;
    u16 interval;
    void (*esd_check_isr)(struct esd_deal *esd);
    int timer;
};

enum {
    BL_CTRL_BACKLIGHT_AND_CONTROLER,
    BL_CTRL_BACKLIGHT,
};

enum {
    INIT_FIRST,
    INIT_LAST,
};

struct base_info {
    struct imd_dmm_info info;
    struct te_mode_ctrl te_mode;
    struct pll4_info pll4;
};

#define REGISTER_IMD_DEVICE_BEGIN(info) \
    static union lcd_dev_info info sec(.lcd_device) = { \
        .imd

#define REGISTER_IMD_DEVICE_END() \
    };


#define REGISTER_LVDS_DEVICE_BEGIN(info) \
    static union lcd_dev_info info sec(.lcd_device) = { \
        .lvds

#define REGISTER_LVDS_DEVICE_END() \
    };

#define REGISTER_MIPI_DEVICE_BEGIN(info) \
    static union lcd_dev_info info sec(.lcd_device) = { \
        .mipi

#define REGISTER_MIPI_DEVICE_END() \
    };

union lcd_dev_info {
    struct base_info base;
    struct imd_dev imd;
    struct mipi_dev mipi;
    struct lvds_dev lvds;
};

struct lcd_dev_drive {
    const char *logo;
    enum LCD_IF type;
    int (*init)(void *_data);
    int (*uninit)(void *_data);
    void (*bl_ctrl)(void *_data, u8 onoff);
    u8 bl_ctrl_flags;
    void (*setxy)(int x0, int x1, int y0, int y1);
    struct esd_deal esd;
    union lcd_dev_info *dev;
    u8 bl_sta;//背光状态
    u8 te_mode_dbug;
    u8 init_order;
    struct lcd_platform_data *private_data;
};

#define REGISTER_LCD_DEVICE_DRIVE(dev) \
    static const struct lcd_dev_drive dev##_drive sec(.lcd_device_drive)

extern struct lcd_dev_drive lcd_device_drive_begin[];
extern struct lcd_dev_drive lcd_device_drive_end[];

#define list_for_each_lcd_device_drive(p) \
    for (p=lcd_device_drive_begin; p < lcd_device_drive_end; p++)

void *lcd_get_cur_hdl();
void lcd_set_cur_hdl(void *dev);
u16 lcd_get_width();
u16 lcd_get_height();
int lcd_reinit();
struct lcd_dev_drive *lcd_get_dev_hdl();
int  lcd_setxy(void);
void lcd_backlight_ctrl(u8 onoff);
int  lcd_get_type();
void lcd_esd_check_reset();
u8 lcd_get_bl_sta();
u8 lcd_get_esd_freq();
void pwm_ch0_backlight_init(u8 backlight_io);
void pwm_ch0_backlight_close(void);
void pwm_ch0_backlight_on(void);
void pwm_ch0_backlight_off(void);
void pwm_ch0_backlight_set_duty(u8 duty);

#endif
