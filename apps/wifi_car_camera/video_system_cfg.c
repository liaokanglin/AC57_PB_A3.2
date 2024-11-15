#include "system/includes.h"
#include "server/ui_server.h"
#include "video_system.h"
#include "ui/res/resfile.h"
#include "res.h"

#include "action.h"
#include "style.h"
#include "app_config.h"
#include "storage_device.h"
#include "key_voice.h"
#include "app_database.h"
#include "device/lcd_driver.h"


extern struct video_system_hdl sys_handler;
extern int sys_cur_mod;  /* 1:rec, 2:tph, 3:dec, 4:audio, 5:music */
#define __this 	(&sys_handler)



// 全局变量来存储原始的 Y、U、V 增益
struct color_correct original_cca;
bool is_original_saved = false; // 标志位，判断是否保存了原始值

// 定义通用的宏来更新标志位
#define UPDATE_FLAG(flag, bit) \
    do { \
        *(flag) |= BIT(bit); \
    } while (0) 

void imd_contrast_effects_cfg(int parm)
{
    // 获取颜色调整结构体
    struct color_correct *cca = imd_dmm_get_color_adj();

    // 如果还没有保存过原始增益值，进行保存
    if (!is_original_saved) {
        original_cca = *cca;  // 保存当前的 Y、U、V 增益值
        is_original_saved = true;  // 标志位设为 true，表示已保存
        // printf("保存原始增益 -> Y: %d, U: %d, V: %d\n", original_cca.y_gain, original_cca.u_gain, original_cca.v_gain);
    }
    
    // 设置新的 Y、U、V 增益（这里假设 parm 是统一的增益值）
    cca->y_gain = parm;
    // cca->u_gain = parm;  // 如果需要单独调整 U、V 增益，可在此修改逻辑
    // cca->v_gain = parm;

    // 输出调试日志
    // printf("设置增益 -> Y: %d, U: %d, V: %d\n", cca->y_gain, cca->u_gain, cca->v_gain);

    // 获取更新标志
    u32 *flag = imd_dmm_get_update_flags();

    // 更新标志位
    UPDATE_FLAG(flag, SET_Y_GAIN);
    // UPDATE_FLAG(flag, SET_U_GAIN);
    // UPDATE_FLAG(flag, SET_V_GAIN);
}


// 全局变量存储原始 R、G、B 增益
struct color_correct original_saturation;
bool is_saturation_saved = false;  // 标志位，判断是否保存了原始增益

// 饱和度调整函数
void imd_saturation_effects_cfg(int parm)
{
    // 获取颜色调整结构体
    struct color_correct *cca = imd_dmm_get_color_adj();

    // 如果还没有保存过原始增益值，进行保存
    if (!is_saturation_saved) {
        original_saturation = *cca;  // 保存当前的 R、G、B 增益
        is_saturation_saved = true;  // 标志位设为 true，表示已保存
        // printf("保存原始饱和度增益 -> R: %d, G: %d, B: %d\n", original_saturation.r_gain, original_saturation.g_gain, original_saturation.b_gain);
    }

    // 设置新的 R、G、B 增益（假设 parm 是统一的饱和度增益值）
    if ((cca->r_gain = parm) == 100)
    {
        cca->r_gain = 180;
        cca->g_gain = 165;
        cca->b_gain = 147;
    }
    else if ((cca->r_gain = parm) == 90)
    {
        cca->r_gain = 180;
        cca->g_gain = 160;
        cca->b_gain = 140;
    }
    else if ((cca->r_gain = parm) == 110)
    {
        (cca->r_gain = (parm + 35));
        (cca->g_gain = (parm + 35));
        (cca->b_gain = (parm + 35));
    }
    else if ((cca->r_gain = parm) == 120)
    {
        cca->r_gain = 180;
        cca->g_gain = 170;
        cca->b_gain = 155;
    }
    else if ((cca->r_gain = parm) == 130)
    {
        cca->r_gain = 180;
        cca->g_gain = 175;
        cca->b_gain = 162;
    }
    
    printf("+++++++++++++++++++++R:%d--G:%d--B:%d   \n",cca->r_gain,cca->g_gain,cca->b_gain);

    u32 *flag = imd_dmm_get_update_flags();
    *flag |= BIT(SET_R_GAIN);
    *flag |= BIT(SET_G_GAIN);
    *flag |= BIT(SET_B_GAIN);
}



// 通用恢复函数
void restore_original_settings(restore_type type)
{
    // 获取颜色调整结构体
    struct color_correct *cca = imd_dmm_get_color_adj();
    
    // 获取更新标志
    u32 *flag = imd_dmm_get_update_flags();

    // 根据传入的类型恢复增益值
    if (type == RESTORE_SATURATION && is_saturation_saved ==1) {
        // 恢复 R、G、B 增益到原始值
        cca->r_gain = original_saturation.r_gain;
        cca->g_gain = original_saturation.g_gain;
        cca->b_gain = original_saturation.b_gain;

        // 打印调试日志
        // printf("恢复到原始饱和度增益 -> R: %d, G: %d, B: %d\n", cca->r_gain, cca->g_gain, cca->b_gain);

        // 更新 R、G、B 增益标志
        *flag |= BIT(SET_R_GAIN);
        *flag |= BIT(SET_G_GAIN);
        *flag |= BIT(SET_B_GAIN);

    } 
     if (type == RESTORE_CONTRAST && is_original_saved == 1) {
        // 恢复 Y、U、V 增益到原始值
        cca->y_gain = original_cca.y_gain;
        cca->u_gain = original_cca.u_gain;
        cca->v_gain = original_cca.v_gain;

        // 打印调试日志
        // printf("恢复到原始对比度增益 -> Y: %d, U: %d, V: %d\n", cca->y_gain, cca->u_gain, cca->v_gain);

        // 更新 Y、U、V 增益标志
        *flag |= BIT(SET_Y_GAIN);
        *flag |= BIT(SET_U_GAIN);
        *flag |= BIT(SET_V_GAIN);
    }

    // 打印更新后的标志
    // printf("恢复增益后标志: 0x%X\n", *flag);
}


int kvo_set_function(u32 parm)
{
    return 0;
}
int hlight_set_function(u32 parm)
{
    return 0;
}

int fre_set_function(u32 parm)
{
    return 0;
}

int aff_set_function(u32 parm)
{
    sys_power_auto_shutdown_stop();        // 停止当前正在运行的自动关机计时器
    sys_power_auto_shutdown_start(parm * 60);  // 以 parm（分钟）为基础，重新启动自动关机计时器
    return 0;                              // 函数执行成功，返回 0
}


extern void ui_lcd_light_time_set(int sec);

int pro_set_function(u32 parm)
{
#ifdef CONFIG_DISPLAY_ENABLE
    ui_lcd_light_time_set(parm);
#endif
    return 0;
}

int lag_set_function(u32 parm)
{
    ui_language_set(parm);  // 设置用户界面的语言，parm 表示语言参数

    return 0;  // 返回 0，表示函数执行成功
}


int tvm_set_function(u32 parm)
{
    return 0;
}

int frm_set_function(u32 parm)
{
    int err;

    sys_key_event_disable();
    sys_touch_event_disable();

    err = storage_device_format();

    sys_key_event_enable();
    sys_touch_event_enable();

    return err;
}

u8 get_default_setting_st()
{
    return	(__this->default_set);
}

void clear_default_setting_st()
{
    __this->default_set = 0;
}

static int def_set_function(u32 parm)
{
    puts("def_set_function\n");
    __this->default_set = 1;
    db_reset();
    /* os_time_dly(200); */
    /* cpu_reset(); */
    sys_fun_restore();

    return 0;
}

static int lane_det_set_function(u32 parm)
{


    return 0;
}

static int backlight_set_function(u32 parm)
{
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    if (parm < 20 || parm > 100) {
        parm = 100;
    }
    pwm_ch0_backlight_set_duty(parm);
#endif
    return 0;
}
/*
 * 在此处添加所需配置即可
 */
static struct app_cfg cfg_table[] = {
    {"kvo", kvo_set_function},
    {"fre", fre_set_function},
    {"aff", aff_set_function},
    {"pro", pro_set_function},
    {"lag", lag_set_function},
    {"tvm", tvm_set_function},
    {"frm", frm_set_function},
    {"def", def_set_function},
    {"hlw", hlight_set_function},
    {"lan", lane_det_set_function },
    {"blk", backlight_set_function },
};

void sys_fun_restore()
{
    aff_set_function(db_select("aff"));
    pro_set_function(db_select("pro"));
    lag_set_function(db_select("lag"));
    backlight_set_function(db_select("bkl"));
}



//修改时间
// void set_rtc_default_time(struct sys_time *t)
// {
//     t->year=2024;
//     t->month=8;
//     t->day=8;
//     t->hour=0;
//     t->min=0;
//     t->sec=0;
// }



/*
 * 被请求设置参数
 */
int video_sys_set_config(struct intent *it)
{

    ASSERT(it != NULL);
    ASSERT(it->data != NULL);

    return app_set_config(it, cfg_table, ARRAY_SIZE(cfg_table));
}

