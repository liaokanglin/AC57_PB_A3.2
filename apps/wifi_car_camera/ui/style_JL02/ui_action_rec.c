#include "ui/includes.h"
#include "server/ui_server.h"
#include "style.h"
#include "action.h"
#include "app_config.h"
#include "system/includes.h"
/* #include "menu_parm_api.h" */
#include "app_database.h"
#include "ui/ui_slider.h"
#include "asm/lcd_config.h"
#include "get_image_data.h"

#ifdef CONFIG_UI_STYLE_JL02_ENABLE

#define STYLE_NAME  JL02

#define REC_RUNNING_TO_HOME     1  //录像时返回主界面
#define ENC_MENU_HIDE_ENABLE    1  //选定菜单后子菜单收起

struct rec_menu_info {
    char resolution;
    char double_route;
    char mic;
    char gravity;
    char motdet;
    char park_guard;
    char wdr;
    char cycle_rec;
    char car_num;
    char dat_label;
    char exposure;
    char gap_rec;
    char backlight_val;

    u8 lock_file_flag; /* 是否当前文件被锁 */
    u8 sd_write_err;

    u8 page_exit;  /*页面退出方式  1切模式退出  2返回HOME退出 */
    u8 menu_status;/*0 menu off, 1 menu on*/
    s8 enc_menu_status;
    u8 battery_val;
    u8 battery_char;

    u8 onkey_mod;
    s8 onkey_sel;
    u8 key_disable;
    u8 hlight_show_status;  /* 前照灯显示状态 */

    int car_pos_x;
    int car_pos_y;
    int car_pos_w;
    int car_pos_c;

    int remain_time;
};


int rec_cnt = 0;
volatile char if_in_rec; /* 是否正在录像 */
static struct rec_menu_info handler = {0};
#define __this 	(&handler)
#define sizeof_this     (sizeof(struct rec_menu_info))


extern u8 table_light_lcd[6];

static void screen_light_set(int sel_item)
{
    struct intent it;  // 定义一个意图结构体变量
    init_intent(&it);  // 初始化意图结构体
    it.name = "video_rec";  // 设置意图的名称为"video_rec"
    it.action = ACTION_VIDEO_REC_SET_CONFIG;  // 设置意图的动作为"设置视频录制配置"
    it.data = "bkl";  // 设置意图的数据为"bkl"（可能表示背光）
    it.exdata = table_light_lcd[sel_item];  // 将选中的亮度值传递给意图的附加数据字段
    start_app(&it);  // 启动应用程序并传递意图
    ui_pic_show_image_by_id(LIGHT_ADJ_PIC, sel_item);  // 根据选中的亮度项显示相应的图片
}

// static void screen_light_show(int sel_item)
// {
//     ui_pic_show_image_by_id(LIGHT_ADJ_PIC, sel_item);
// }





const static int onkey_sel_item[3] = {
    ENC_PIC_SETTING,
    ENC_BTN_VIDEO,
    ENC_BTN_HOME,
};

// const static int onkey_sel_setting[12] = {
//     ENC_SET_PIC_1_1,
//     ENC_SET_PIC_1_2,
//     ENC_SET_PIC_1_3,
//     ENC_SET_PIC_1_4,
//     ENC_SET_PIC_2_1,
//     ENC_SET_PIC_2_2,
//     ENC_SET_PIC_2_3,
//     ENC_SET_PIC_2_4,
//     ENC_SET_PIC_3_1,
//     ENC_SET_PIC_3_2,
//     ENC_SET_PIC_3_3,
//     ENC_SET_PIC_3_4,
// };
// const static int onkey_sel_setting1[12] = {
//     ENC_SET_TXT_1_1,
//     ENC_SET_TXT_1_2,
//     ENC_SET_TXT_1_3,
//     ENC_SET_TXT_1_4,
//     ENC_SET_TXT_2_1,
//     ENC_SET_TXT_2_2,
//     ENC_SET_TXT_2_3,
//     ENC_SET_TXT_2_4,
//     ENC_SET_TXT_3_1,
//     ENC_SET_TXT_3_2,
//     ENC_SET_TXT_3_3,
//     ENC_SET_TXT_3_4,
// };
// const static int _ENC_SET_PIC_C2[] = {
//     ENC_SET_PIC_C2_1,
//     ENC_SET_PIC_C2_2,
// };
// const static int _ENC_SET_PIC_C3[] = {
//     ENC_SET_PIC_C3_1,
//     ENC_SET_PIC_C3_2,
//     ENC_SET_PIC_C3_3,
// };
// const static int _ENC_SET_PIC_C4[] = {
//     ENC_SET_PIC_C4_1,
//     ENC_SET_PIC_C4_2,
//     ENC_SET_PIC_C4_3,
//     ENC_SET_PIC_C4_4,
// };
// const static int _ENC_SET_PIC_EXP[] = {
//     ENC_SET_PIC_EXP_S01,
//     ENC_SET_PIC_EXP_S02,
//     ENC_SET_PIC_EXP_S03,
//     ENC_SET_PIC_EXP_S04,
//     ENC_SET_PIC_EXP_S05,
//     ENC_SET_PIC_EXP_S06,
//     ENC_SET_PIC_EXP_S07,
// };
// const static int _ENC_SET_PIC_C20[] = {
//     ENC_SET_PIC_C20_1,
//     ENC_SET_PIC_C20_2,
// };

extern void set_page_main_flag(u8 flag);
extern int storage_device_ready();
int sys_cur_mod;
u8 av_in_statu = 0;

/************************************************************
				    	录像模式设置
************************************************************/
/*
 * rec分辨率设置
 */
static const u8 table_video_resolution[] = {
    VIDEO_RES_1080P,
    VIDEO_RES_720P,
    VIDEO_RES_VGA,
};
/*
 * rec循环录像设置
 */
static const u8 table_video_cycle[] = {
    0,
    3,
    5,
    10,
};


/*
 * rec曝光补偿设置
 */
static const u8 table_video_exposure[] = {
    3,
    2,
    1,
    0,
    (u8) - 1,
    (u8) - 2,
    (u8) - 3,
};


/*
 * rec重力感应设置
 */
static const u8 table_video_gravity[] = {
    GRA_SEN_OFF,
    GRA_SEN_LO,
    GRA_SEN_MD,
    GRA_SEN_HI,
};



/*
 * rec间隔录影设置, ms
 */
static const u16 table_video_gap[] = {
    0,
    100,
    200,
    500,
};

static const u16 province_gb2312[] = {
    0xA9BE, 0xFEC4, 0xA8B4, 0xA6BB, 0xF2BD, //京，宁，川，沪，津
    0xE3D5, 0xE5D3, 0xE6CF, 0xC1D4, 0xA5D4, //浙，渝，湘，粤，豫
    0xF3B9, 0xD3B8, 0xC9C1, 0xB3C2, 0xDABA, //贵，赣，辽，鲁，黑
    0xC2D0, 0xD5CB, 0xD8B2, 0xF6C3, 0xFABD, //新，苏，藏，闽，晋
    0xEDC7, 0xBDBC, 0xAABC, 0xF0B9, 0xCAB8, //琼，冀，吉，桂，甘，
    0xEECD, 0xC9C3, 0xF5B6, 0xC2C9, 0xE0C7, //皖，蒙，鄂，陕，青，
    0xC6D4                                  //云
};

static const u8 num_table[] = {
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',
    'Z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8',
    '9'
};


struct car_num {
    const char *mark;
    u32 text_id;
    u32 text_index;
};

struct car_num_str {
    u8 province;
    u8 town;
    u8 a;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
};


enum {
    PAGE_SHOW = 0,
    MODE_SW_EXIT,
    HOME_SW_EXIT,
};

struct car_num text_car_num_table[] = {
    {"province", ENC_PIC_CID_0, 0},// [> 京 <]
    {"town",     ENC_PIC_CID_1, 0},// [> A <]
    {"a",        ENC_PIC_CID_2, 0},// [> 1 <]
    {"b",        ENC_PIC_CID_3, 0},// [> 2 <]
    {"c",        ENC_PIC_CID_4, 0},// [> 3 <]
    {"d",        ENC_PIC_CID_5, 0},// [> 4 <]
    {"e",        ENC_PIC_CID_6, 0},// [> 5 <]
};



enum ENC_MENU {
    ENC_MENU_NULL = 0,
    ENC_MENU_RESOLUTION,
    ENC_MENU_DUALVIDEO,
    ENC_MENU_CYCLE,
    ENC_MENU_GAP,
    ENC_MENU_HDR,
    ENC_MENU_EXPOSURE,
    ENC_MENU_MOTION,
    ENC_MENU_LABEL,
    ENC_MENU_GSEN,
    ENC_MENU_SOUND,
    ENC_MENU_GUARD,
    ENC_MENU_CID,
    ENC_MENU_HIDE = 100,
};


// void enc_menu_show(enum ENC_MENU item)
// {
//     if (__this->enc_menu_status == ENC_MENU_HIDE) {
//         __this->enc_menu_status = ENC_MENU_NULL;
//         return;
//     }
//     switch (item) {
//     case ENC_MENU_NULL:
//         __this->enc_menu_status = ENC_MENU_NULL;
//         break;
//     case ENC_MENU_RESOLUTION:
//         if (__this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_GUARD &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C3);
//         ui_show(ENC_SET_PIC_SEL_1_1);
//         ui_highlight_element_by_id(ENC_SET_PIC_1_1);
//         ui_highlight_element_by_id(ENC_SET_TXT_1_1);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C3[__this->resolution]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_DUALVIDEO:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_CID &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_GUARD &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C2);
//         ui_show(ENC_SET_PIC_SEL_1_2);
//         ui_highlight_element_by_id(ENC_SET_PIC_1_2);
//         ui_highlight_element_by_id(ENC_SET_TXT_1_2);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C2[__this->double_route]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_CYCLE:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_GUARD &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C4);
//         ui_show(ENC_SET_PIC_SEL_1_3);
//         ui_highlight_element_by_id(ENC_SET_PIC_1_3);
//         ui_highlight_element_by_id(ENC_SET_TXT_1_3);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_2, 0);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_3, 0);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_4, 0);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C4[__this->cycle_rec]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_GAP:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_GUARD &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C4);
//         ui_show(ENC_SET_PIC_SEL_1_4);
//         ui_highlight_element_by_id(ENC_SET_PIC_1_4);
//         ui_highlight_element_by_id(ENC_SET_TXT_1_4);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_2, 1);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_3, 1);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_4, 1);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C4[__this->gap_rec]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_HDR:
//         if (__this->enc_menu_status != ENC_MENU_EXPOSURE &&
//             __this->enc_menu_status != ENC_MENU_MOTION &&
//             __this->enc_menu_status != ENC_MENU_LABEL) {
//             ui_hide(ENC_SET_LAY_3);
//             ui_show(ENC_SET_LAY_SET_2);
//         }
//         ui_show(ENC_SET_LAY_C20);
//         ui_show(ENC_SET_PIC_SEL_2_1);
//         ui_highlight_element_by_id(ENC_SET_PIC_2_1);
//         ui_highlight_element_by_id(ENC_SET_TXT_2_1);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C20[__this->wdr]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_EXPOSURE:
//         if (__this->enc_menu_status != ENC_MENU_HDR &&
//             __this->enc_menu_status != ENC_MENU_MOTION &&
//             __this->enc_menu_status != ENC_MENU_LABEL) {
//             ui_hide(ENC_SET_LAY_3);
//             ui_show(ENC_SET_LAY_SET_2);
//         }
//         ui_show(ENC_SET_LAY_EXP);
//         ui_show(ENC_SET_PIC_SEL_2_2);
//         ui_highlight_element_by_id(ENC_SET_PIC_2_2);
//         ui_highlight_element_by_id(ENC_SET_TXT_2_2);
//         ui_show(_ENC_SET_PIC_EXP[(6 - __this->exposure)]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_MOTION:
//         if (__this->enc_menu_status != ENC_MENU_HDR &&
//             __this->enc_menu_status != ENC_MENU_EXPOSURE &&
//             __this->enc_menu_status != ENC_MENU_LABEL) {
//             ui_hide(ENC_SET_LAY_3);
//             ui_show(ENC_SET_LAY_SET_2);
//         }
//         ui_show(ENC_SET_LAY_C20);
//         ui_show(ENC_SET_PIC_SEL_2_3);
//         ui_highlight_element_by_id(ENC_SET_PIC_2_3);
//         ui_highlight_element_by_id(ENC_SET_TXT_2_3);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C20[__this->motdet]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_LABEL:
//         if (__this->enc_menu_status != ENC_MENU_HDR &&
//             __this->enc_menu_status != ENC_MENU_EXPOSURE &&
//             __this->enc_menu_status != ENC_MENU_MOTION) {
//             ui_hide(ENC_SET_LAY_3);
//             ui_show(ENC_SET_LAY_SET_2);
//         }
//         ui_show(ENC_SET_LAY_C20);
//         ui_show(ENC_SET_PIC_SEL_2_4);
//         ui_highlight_element_by_id(ENC_SET_PIC_2_4);
//         ui_highlight_element_by_id(ENC_SET_TXT_2_4);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C20[__this->dat_label]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_GSEN:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_GUARD &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C4);
//         ui_show(ENC_SET_PIC_SEL_3_1);
//         ui_highlight_element_by_id(ENC_SET_PIC_3_1);
//         ui_highlight_element_by_id(ENC_SET_TXT_3_1);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_2, 2);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_3, 2);
//         ui_text_show_index_by_id(ENC_SET_TXT_C4_4, 2);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C4[__this->gravity]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_SOUND:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_GUARD &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C2);
//         ui_show(ENC_SET_PIC_SEL_3_2);
//         ui_highlight_element_by_id(ENC_SET_PIC_3_2);
//         ui_highlight_element_by_id(ENC_SET_TXT_3_2);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C2[__this->mic]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_GUARD:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C2);
//         ui_show(ENC_SET_PIC_SEL_3_3);
//         ui_highlight_element_by_id(ENC_SET_PIC_3_3);
//         ui_highlight_element_by_id(ENC_SET_TXT_3_3);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C2[__this->park_guard]);
//         __this->enc_menu_status = item;
//         break;
//     case ENC_MENU_CID:
//         if (__this->enc_menu_status != ENC_MENU_RESOLUTION &&
//             __this->enc_menu_status != ENC_MENU_DUALVIDEO &&
//             __this->enc_menu_status != ENC_MENU_CYCLE &&
//             __this->enc_menu_status != ENC_MENU_GAP &&
//             __this->enc_menu_status != ENC_MENU_GSEN &&
//             __this->enc_menu_status != ENC_MENU_SOUND &&
//             __this->enc_menu_status != ENC_MENU_GUARD) {
//             ui_hide(ENC_SET_LAY_2);
//             ui_show(ENC_SET_LAY_SET_1);
//         }
//         ui_show(ENC_SET_LAY_C2);
//         ui_show(ENC_SET_PIC_SEL_3_4);
//         ui_highlight_element_by_id(ENC_SET_PIC_3_4);
//         ui_highlight_element_by_id(ENC_SET_TXT_3_4);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C2[__this->car_num]);
//         __this->enc_menu_status = item;
//         break;
//     default:
//         break;
//     }
// }


/*****************************布局上部回调 ************************************/
static int video_layout_up_onchange(void *ctr, enum element_change_event e, void *arg)
{
    int index;

    switch (e) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        /*
         * 在此获取默认隐藏的图标的状态并显示
         */

        int x,y;
        x = db_select("bkl");
        y = round((x - 20) * 5 / 60);
        ui_pic_show_image_by_id(LIGHT_ADJ_PIC, y);

        // 处理 db_select("mic")
        switch (db_select("mic")) {
            case 1:
                ui_highlight_element_by_id(ENC_BTN_MU);
                ui_no_highlight_element_by_id(ENC_BTN_MU_2);
                break;
            default:
                ui_no_highlight_element_by_id(ENC_BTN_MU);
                ui_highlight_element_by_id(ENC_BTN_MU_2);
                break;
        }

        // 处理 db_select("wfo")
        switch (db_select("wfo")) {
            case 1:
                ui_highlight_element_by_id(ENC_BTN_WIFI);
                ui_no_highlight_element_by_id(ENC_BTN_WIFI1);
                ui_pic_show_image_by_id(PIC_REC_WIFI, 3);
                break;
            default:
                ui_no_highlight_element_by_id(ENC_BTN_WIFI);
                ui_highlight_element_by_id(ENC_BTN_WIFI1);
                ui_pic_show_image_by_id(PIC_REC_WIFI, 0);
                break;
        }

        // 处理 db_select("dat")
        switch (db_select("dat")) {
            case 1:
                ui_highlight_element_by_id(ENC_BTN_SY);
                ui_no_highlight_element_by_id(ENC_BTN_SY1);
                break;
            default:
                ui_no_highlight_element_by_id(ENC_BTN_SY);
                ui_highlight_element_by_id(ENC_BTN_SY1);
                break;
        }

        // 处理 db_select("sxt")
        switch (db_select("sxt")) {
            case 1:
                ui_highlight_element_by_id(ENC_BTN_CHANNEL_1);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_2);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_3);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_4);
                break;
            case 2:
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_1);
                ui_highlight_element_by_id(ENC_BTN_CHANNEL_2);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_3);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_4);
                break;
            case 3:
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_1);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_2);
                ui_highlight_element_by_id(ENC_BTN_CHANNEL_3);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_4);
                break;
            case 4:
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_1);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_2);
                ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_3);
                ui_highlight_element_by_id(ENC_BTN_CHANNEL_4);
                break;
            default:
                // 可根据需要在这里处理其他情况
                break;
        }

        break;
    default:
    return 0;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(ENC_SET_WIN)
.onchange = video_layout_up_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
/*****************************布局下部回调 ************************************/
static int video_layout_down_onchange(void *ctr, enum element_change_event e, void *arg)
{
    u32 a, b;

    switch (e) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        /*
         * 在此获取默认隐藏的图标的状态并显示
         */
            //云台开关设置
            if (db_select("yta") == 1){
                ui_highlight_element_by_id(GIMBAL_BTN_TOGGLE);
                ui_no_highlight_element_by_id(GIMBAL_BTN_TOGGLE1);
            } else {
                ui_no_highlight_element_by_id(GIMBAL_BTN_TOGGLE);
                ui_highlight_element_by_id(GIMBAL_BTN_TOGGLE1);
            }

            //自动关机设置
            switch (db_select("aff"))
            {
            case 0:
                    ui_highlight_element_by_id(ZD_BTN_TOGGLE3); // 设置图标为开启按钮
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE1);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE2);
                break;
            case 10:
                    ui_highlight_element_by_id(ZD_BTN_TOGGLE); // 设置图标为开启按钮
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE1);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE2);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE3);
                break;
            case 20:
                    ui_highlight_element_by_id(ZD_BTN_TOGGLE1); // 设置图标为开启按钮
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE2);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE3);
                break;
            case 30:
                    ui_highlight_element_by_id(ZD_BTN_TOGGLE2); // 设置图标为开启按钮
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE1);
                    ui_no_highlight_element_by_id(ZD_BTN_TOGGLE3);
                break;
            default:
                break;
            }

            //图像大小
            switch (db_select("qua"))
            {
            case 0:
                    ui_highlight_element_by_id(TPH_0); // 设置图标为开启按钮
                    ui_no_highlight_element_by_id(TPH_1);
                    ui_no_highlight_element_by_id(TPH_2);
                break;
            case 1:
                    ui_no_highlight_element_by_id(TPH_0); // 设置图标为开启按钮
                    ui_highlight_element_by_id(TPH_1);
                    ui_no_highlight_element_by_id(TPH_0);
                break;
            case 2:
                    ui_no_highlight_element_by_id(TPH_0); // 设置图标为开启按钮
                    ui_no_highlight_element_by_id(TPH_1);
                    ui_highlight_element_by_id(TPH_2);
                break;
            default:
                break;
            }
        break;
    default:
    return 0;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(ENC_MENU_SET_2)
.onchange = video_layout_down_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

/******************************三级菜单回调**************************************/
static int video_YY_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        /*
         * 在此获取默认隐藏的图标的状态并显示
         */
            int lag = db_select("lag");
                switch (lag)
            {
            case 1:
                    ui_highlight_element_by_id(SYS_YY_C1_1); // 设置图标为开启按钮
                break;
            case 5:
                    ui_highlight_element_by_id(SYS_YY_C1_2); // 设置图标为开启按钮
                break;
            case 7:
                    ui_highlight_element_by_id(SYS_YY_C1_3); // 设置图标为开启按钮
                break;
            case 6:
                    ui_highlight_element_by_id(SYS_YY_C1_4); // 设置图标为开启按钮
                break;
            case 10:
                    ui_highlight_element_by_id(SYS_YY_C2_1); // 设置图标为开启按钮
                break;
            case 11:
                    ui_highlight_element_by_id(SYS_YY_C2_2); // 设置图标为开启按钮
                break;
            case 8:
                    ui_highlight_element_by_id(SYS_YY_C2_3); // 设置图标为开启按钮
                break;
            case 16:
                    ui_highlight_element_by_id(SYS_YY_C2_4); // 设置图标为开启按钮
                break;
            case 4:
                    ui_highlight_element_by_id(SYS_YY_C3_1); // 设置图标为开启按钮
                break;
            case 3:
                    ui_highlight_element_by_id(SYS_YY_C3_2); // 设置图标为开启按钮
                break;
            case 15:
                    ui_highlight_element_by_id(SYS_YY_C3_3); // 设置图标为开启按钮
                break;
            default:
                break;
            }
    return 0;
    }
}
REGISTER_UI_EVENT_HANDLER(SYS_YY)
.onchange = video_YY_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

//图像参数回调
static int video_set_pot_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;  // 将传入的控件指针转换为布局类型指针
    switch (e) {
    case ON_CHANGE_INIT:
        // 初始化操作
        break;

    case ON_CHANGE_RELEASE:
        ui_ontouch_unlock(layout);  // 解锁触控，允许用户继续进行触控操作
        break;

    case ON_CHANGE_FIRST_SHOW:
        ui_ontouch_lock(layout);  // 锁定触控，防止在显示期间有其他触控操作
        /*
         * 在此获取默认隐藏的图标的状态并显示
         */
        int con = db_select("con");
        UI_ONTOUCH_DEBUG("Current contrast value (con): %d\n", con);  // 打印 con 值

        switch (con) {
        case 90:
            ui_pic_show_image_by_id(LIGHT_4, 0);
            break;
        case 100:
            ui_pic_show_image_by_id(LIGHT_4, 1);
            break;
        case 110:
            ui_pic_show_image_by_id(LIGHT_4, 2);
            break;
        case 120:
            ui_pic_show_image_by_id(LIGHT_4, 3);
            break;
        case 130:
            ui_pic_show_image_by_id(LIGHT_4, 4);
            break;
        default:
            break;
        }

        int sat = db_select("sat");
        switch (sat) {
            case 80:
                ui_pic_show_image_by_id(LIGHT_6, 0);
                break;
            case 90:
                ui_pic_show_image_by_id(LIGHT_6, 1);
                break;
            case 100:
                ui_pic_show_image_by_id(LIGHT_6, 2);
                break;
            case 110:
                ui_pic_show_image_by_id(LIGHT_6, 3);
                break;
            case 120:
                ui_pic_show_image_by_id(LIGHT_6, 4);
                break;
            default:
                break;
            }

            break;
        default:
            break;
        }
        return 0;
}
REGISTER_UI_EVENT_HANDLER(BTN_SET_POT)
.onchange = video_set_pot_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


// static void get_sys_time(struct sys_time *time)
// {
//     void *fd = dev_open("rtc", NULL);
//     if (!fd) {
//         memset(time, 0, sizeof(*time));
//         return;
//     }
//     dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
//     dev_close(fd);
// }


// void enc_menu_hide(enum ENC_MENU item)
// {
//     switch (__this->enc_menu_status) {
//     case ENC_MENU_NULL:
//         break;
//     case ENC_MENU_RESOLUTION:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_1_1);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_1_1);
//         if (item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_GUARD &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_1_1);
//             ui_hide(ENC_SET_LAY_C3);
//         }
//         break;
//     case ENC_MENU_DUALVIDEO:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_1_2);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_1_2);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_GUARD &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_1_2);
//             ui_hide(ENC_SET_LAY_C2);
//         }
//         break;
//     case ENC_MENU_CYCLE:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_1_3);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_1_3);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_GUARD &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_1_3);
//             ui_hide(ENC_SET_LAY_C4);
//         }
//         break;
//     case ENC_MENU_GAP:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_1_4);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_1_4);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_GUARD &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_1_4);
//             ui_hide(ENC_SET_LAY_C4);
//         }
//         break;
//     case ENC_MENU_HDR:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_2_1);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_2_1);
//         if (item != ENC_MENU_EXPOSURE &&
//             item != ENC_MENU_MOTION &&
//             item != ENC_MENU_LABEL) {
//             ui_hide(ENC_SET_LAY_SET_2);
//             ui_show(ENC_SET_LAY_3);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_2_1);
//             ui_hide(ENC_SET_LAY_C20);
//         }
//         break;
//     case ENC_MENU_EXPOSURE:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_2_2);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_2_2);
//         if (item != ENC_MENU_HDR &&
//             item != ENC_MENU_MOTION &&
//             item != ENC_MENU_LABEL) {
//             ui_hide(ENC_SET_LAY_SET_2);
//             ui_show(ENC_SET_LAY_3);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_2_2);
//             ui_hide(ENC_SET_LAY_EXP);
//         }
//         break;
//     case ENC_MENU_MOTION:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_2_3);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_2_3);
//         if (item != ENC_MENU_HDR &&
//             item != ENC_MENU_EXPOSURE &&
//             item != ENC_MENU_LABEL) {
//             ui_hide(ENC_SET_LAY_SET_2);
//             ui_show(ENC_SET_LAY_3);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_2_3);
//             ui_hide(ENC_SET_LAY_C20);
//         }
//         break;
//     case ENC_MENU_LABEL:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_2_4);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_2_4);
//         if (item != ENC_MENU_HDR &&
//             item != ENC_MENU_EXPOSURE &&
//             item != ENC_MENU_MOTION) {
//             ui_hide(ENC_SET_LAY_SET_2);
//             ui_show(ENC_SET_LAY_3);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_2_4);
//             ui_hide(ENC_SET_LAY_C20);
//         }
//         break;
//     case ENC_MENU_GSEN:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_3_1);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_3_1);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_GUARD &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_3_1);
//             ui_hide(ENC_SET_LAY_C4);
//         }
//         break;
//     case ENC_MENU_SOUND:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_3_2);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_3_2);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_GUARD &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_3_2);
//             ui_hide(ENC_SET_LAY_C2);
//         }
//         break;
//     case ENC_MENU_GUARD:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_3_3);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_3_3);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_CID) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_3_3);
//             ui_hide(ENC_SET_LAY_C2);
//         }
//         break;
//     case ENC_MENU_CID:
//         ui_no_highlight_element_by_id(ENC_SET_PIC_3_4);
//         ui_no_highlight_element_by_id(ENC_SET_TXT_3_4);
//         if (item != ENC_MENU_RESOLUTION &&
//             item != ENC_MENU_DUALVIDEO &&
//             item != ENC_MENU_CYCLE &&
//             item != ENC_MENU_GAP &&
//             item != ENC_MENU_GSEN &&
//             item != ENC_MENU_SOUND &&
//             item != ENC_MENU_GUARD) {
//             ui_hide(ENC_SET_LAY_SET_1);
//             ui_show(ENC_SET_LAY_2);
//         } else {
//             ui_hide(ENC_SET_PIC_SEL_3_4);
//             ui_hide(ENC_SET_LAY_C2);
//         }
//         break;
//     default:
//         __this->enc_menu_status = ENC_MENU_NULL;
//         break;
//     }

//     if (item == __this->enc_menu_status) {
//         __this->onkey_mod = 2;
//         __this->enc_menu_status = ENC_MENU_HIDE;
//     }
// }



//int enc_menu(int item)
//{
//    enc_menu_hide(item);
//    enc_menu_show(item);
//    return 0;
//}
/*
 * (begin)提示框显示接口 ********************************************
 */
enum box_msg {
    BOX_MSG_POWER_OFF = 1,
    BOX_MSG_NO_POWER,
    BOX_MSG_MEM_ERR,
    BOX_MSG_NO_MEM,
    BOX_MSG_NEED_FORMAT,
    BOX_MSG_INSERT_SD,
    BOX_MSG_SD_WRITE_ERR,
    BOX_MSG_EXIT_APP,

    /* BOX_MSG_DEFAULT_SET, */
    /* BOX_MSG_FORMATTING, */
    /* BOX_MSG_10S_SHUTDOWN, */
    /* BOX_MSG_SD_ERR, */
};
static u8 msg_show_f = 0;
static enum box_msg msg_show_id = 0;
static void __rec_msg_hide(enum box_msg id)
{
    if (msg_show_id == id) {
        if (msg_show_f) {
            msg_show_f = 0;
            ui_hide(ENC_LAY_MESSAGEBOX);
        }
    } else if (id == 0) { /* 没有指定ID，强制隐藏 */
        if (msg_show_f) {
            msg_show_f = 0;
            ui_hide(ENC_LAY_MESSAGEBOX);
        }
    }
}
static void __rec_msg_timeout_func(void *priv)
{
    __rec_msg_hide((enum box_msg)priv);
}
static void __rec_msg_show(enum box_msg msg, u32 timeout_msec)
{
    static int t_id = 0;
    if (msg == 0) {
        ASSERT(0, "__rec_msg_show msg 0!\n");
        return;
    }

    if (msg == msg_show_id) {
        if (msg_show_f == 0) {
            msg_show_f = 1;
            ui_show(ENC_LAY_MESSAGEBOX);
            ui_text_show_index_by_id(ENC_TXT_MESSAGEBOX, msg - 1);
            /* ui_show(ENC_TXT_MESSAGEBOX_1); */
            if (t_id) {
                sys_timeout_del(t_id);
                t_id = 0;
            }
            if (timeout_msec > 0) {
                t_id = sys_timeout_add((void *)msg, __rec_msg_timeout_func, timeout_msec);
            }
        }
    } else {
        msg_show_id = msg;
        if (msg_show_f == 0) {
            msg_show_f = 1;
            ui_show(ENC_LAY_MESSAGEBOX);
        }
        ui_text_show_index_by_id(ENC_TXT_MESSAGEBOX, msg - 1);
        //        ui_show(ENC_TXT_MESSAGEBOX_1);//显示提示
        if (t_id) {
            sys_timeout_del(t_id);
            t_id = 0;
        }
        if (timeout_msec > 0) {
            t_id = sys_timeout_add((void *)msg, __rec_msg_timeout_func, timeout_msec);
        }
    }
}
/*
 * (end)
 */
static int rec_ask_app_open_menu(void)
{
    int err;
    struct intent it;

    init_intent(&it);
    it.name	= "video_rec";
    it.action = ACTION_VIDEO_REC_CHANGE_STATUS;
    it.data = "opMENU:";
    err = start_app(&it);
    if (err) {
        ASSERT(err == 0, ":rec opMENU fail! %d\n", err);
    }
    if (!strcmp(it.data, "opMENU:dis")) {
        puts("rec do not allow ui to open menu.\n");
        return -1;
    } else if (!strcmp(it.data, "opMENU:en")) {
        puts("rec allow ui to open menu.\n");
    } else {
        ASSERT(0, "opMENU err\n");
    }
    return 0; /* 返回0则打开菜单 */
}
/*
 * (begin)UI状态变更主动请求APP函数 ***********************************
 */
static void rec_tell_app_exit_menu(void)
{
    int err;
    struct intent it;
    init_intent(&it);
    it.name	= "video_rec";
    it.action = ACTION_VIDEO_REC_CHANGE_STATUS;
    it.data = "exitMENU";
    err = start_app(&it);
    if (err) {
        ASSERT(err == 0, ":rec exitMENU\n");
    }
}
static void menu_rec_set_backlight(int duty)
{
    struct intent it;
    __this->backlight_val = duty;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "bkl";
    it.exdata = duty;
    start_app_async(&it, NULL, NULL);

}
static void menu_rec_set_res(int sel_item)
{
    struct intent it;
    __this->resolution = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "res";
    it.exdata = table_video_resolution[sel_item];
    start_app(&it);
    ui_pic_show_image_by_id(ENC_PIC_RESOLUTION, sel_item);
}
static void menu_rec_set_mic(int sel_item)
{
    struct intent it; // 定义一个意图结构体变量
    __this->mic = sel_item; // 将选择的麦克风选项保存到当前对象的 mic 成员
    init_intent(&it); // 初始化意图
    it.name = "video_rec"; // 设置意图的目标应用名称为 "video_rec"
    it.action = ACTION_VIDEO_REC_SET_CONFIG; // 设置意图的动作为设置视频录制配置
    it.data = "mic"; // 设置意图的数据为 "mic"，表示当前正在配置麦克风
    it.exdata = sel_item; // 将选择的麦克风选项值传递给意图的额外数据字段
    start_app(&it); // 启动目标应用并传递意图进行配置
    ui_pic_show_image_by_id(ENC_PIC_SOUND, sel_item); // 根据选择的麦克风选项显示对应的图像
}

static void menu_rec_set_mot(int sel_item)
{
    struct intent it;
    __this->motdet = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "mot";
    it.exdata = sel_item;
    start_app(&it);
    if (sel_item) {
        ui_show(ENC_PIC_MOVE);
    } else {
        ui_hide(ENC_PIC_MOVE);
    }

    rec_tell_app_exit_menu();//生效移动侦测选项
}
static void menu_rec_set_par(int sel_item)
{
    struct intent it;
    __this->park_guard = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "par";
    it.exdata = sel_item;
    start_app(&it);
    if (sel_item) {
        ui_show(ENC_PIC_GUARD);
    } else {
        ui_hide(ENC_PIC_GUARD);
    }
}
static void menu_rec_set_wdr(int sel_item)
{
    struct intent it;
    __this->wdr = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "wdr";
    it.exdata = sel_item;
    start_app(&it);
    if (sel_item) {
        ui_show(ENC_PIC_HDR);
    } else {
        ui_hide(ENC_PIC_HDR);
    }
}
static void menu_rec_set_num(int sel_item)
{
    struct intent it;
    __this->car_num = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "num";
    it.exdata = sel_item;
    start_app(&it);
    if (sel_item) {
        ui_show(ENC_LAY_CID);
    } else {
        ui_hide(ENC_LAY_CID);
    }
}
static void menu_rec_set_dat(int sel_item)
{
    struct intent it;
    __this->dat_label = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "dat";
    it.exdata = sel_item;
    start_app(&it);
}
static void menu_rec_set_double(int sel_item)  // 设置双摄像头的函数，参数为选择的项
{
    struct intent it;  // 创建一个意图结构体
    __this->double_route = sel_item;  // 更新当前的双摄像头设置为用户选择的项
    init_intent(&it);  // 初始化意图结构体
    it.name = "video_rec";  // 设置意图名称为视频录制
    it.action = ACTION_VIDEO_REC_SET_CONFIG;  // 设置意图的动作为配置视频录制
    it.data = "two";  // 设置附加数据为"two"，表示双摄像头
    it.exdata = sel_item;  // 将选择的项作为额外数据附加到意图中
    start_app(&it);  // 启动处理意图的应用
}

static void menu_rec_set_gravity(int sel_item)
{
    struct intent it;
    __this->gravity = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "gra";
    it.exdata = table_video_gravity[sel_item];
    start_app(&it);
    if (sel_item == 0) {
        ui_hide(ENC_PIC_GSEN);
    } else {
        ui_pic_show_image_by_id(ENC_PIC_GSEN, sel_item - 1);
    }
}
static void menu_rec_set_cycle(int sel_item)
{
    struct intent it;
    __this->cycle_rec = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "cyc";
    it.exdata = table_video_cycle[sel_item];
    start_app(&it);
    ui_pic_show_image_by_id(ENC_PIC_CYCLE, sel_item);
}
static void menu_rec_set_exposure(int sel_item)
{
    struct intent it;
    __this->exposure = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "exp";
    it.exdata = table_video_exposure[sel_item];
    start_app(&it);
}
static void menu_rec_set_gap(int sel_item)
{
    struct intent it;
    __this->gap_rec = sel_item;
    init_intent(&it);
    it.name = "video_rec";
    it.action = ACTION_VIDEO_REC_SET_CONFIG;
    it.data = "gap";
    it.exdata = table_video_gap[sel_item];
    start_app(&it);
    ui_pic_show_image_by_id(ENC_PIC_DELAY, sel_item);
}

static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    //  printf("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec);
    dev_close(fd);
}

static int disp_RecSetting_lay(u8 menu_status)
{
    if (menu_status) {
        /* 如果正在录制，无法打开菜单 */
        /* puts("It is in rec, can't open menu.\n"); */
        /* return -1; */
        /* } */
        /* if (rec_ask_app_open_menu() == (int) - 1) { */
        /* return -1; */
        /* } */
        __this->menu_status = 1;
        __this->enc_menu_status = ENC_MENU_NULL;
        ui_hide(ENC_LAY_REC);  // 隐藏录制界面
        ui_show(ENC_SET_WIN);  // 显示设置窗口
        ui_highlight_element_by_id(ENC_PIC_SETTING);  // 高亮设置图标
    } else {
        ui_hide(ENC_SET_WIN);  // 隐藏设置窗口
        ui_show(ENC_LAY_REC);  // 显示录制界面
        if (av_in_statu) {
            // ui_show(ENC_BTN_SWITCH);  // 显示切换按钮
        }
        if (if_in_rec == TRUE) {
            // ui_show(ENC_ANI_REC_HL);  // 显示录制动画
            ui_highlight_element_by_id(ENC_PIC_REC);
        }
        if (__this->onkey_mod == 0) {
            ui_no_highlight_element_by_id(ENC_PIC_SETTING);  // 取消高亮设置图标
        } else {
            ui_highlight_element_by_id(ENC_PIC_SETTING);  // 高亮设置图标
        }
        __this->menu_status = 0;
        __this->enc_menu_status = ENC_MENU_NULL;
    }
    return 0;
}
static void avin_event_handler(struct sys_event *event, void *priv)
{
    // 判断事件是否与 "video1" 或 "uvc" 设备相关
    if (!strncmp(event->arg, "video1", 6) || !strncmp((char *)event->arg, "uvc", 3)) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:  // 设备插入事件
        case DEVICE_EVENT_ONLINE:  // 设备上线事件
            av_in_statu = 1;  // 设置 av_in_statu 状态为 1，表示设备在线或已插入
            // ui_show(ENC_BTN_SWITCH);  // 显示切换按钮
            break;
        case DEVICE_EVENT_OUT:  // 设备拔出事件
#ifdef THREE_WAY_ENABLE  // 如果启用了三路视频功能
            // 检查 "video1" 和 "uvc" 设备是否都不在线
            if (!dev_online("video1") && !dev_online("uvc")) {
                av_in_statu = 0;  // 设置 av_in_statu 状态为 0，表示设备不在线
                // ui_hide(ENC_BTN_SWITCH);  // 隐藏切换按钮
            }
#else  // 如果没有启用三路视频功能
            av_in_statu = 0;  // 设置 av_in_statu 状态为 0，表示设备不在线
            // ui_hide(ENC_BTN_SWITCH);  // 隐藏切换按钮
#endif
            break;
        }
    }
}


/*
 * 录像计时的定时器,1s
 */
static void rec_cnt_handler(int rec_cnt_sec)
{
    struct sys_time sys_time;
    struct utime time_r;

    get_sys_time(&sys_time);

    if (if_in_rec == true) {
        rec_cnt = sys_time.hour * 3600 + sys_time.min * 60 + sys_time.sec - rec_cnt_sec;
    }
    time_r.hour = rec_cnt_sec / 60 / 60;
    time_r.min = rec_cnt_sec / 60 % 60;
    time_r.sec = rec_cnt_sec % 60;
    ui_time_update_by_id(ENC_TIM_REC, &time_r);
}

/*
 * sd卡事件处理函数
 */
static void sd_event_handler(struct sys_event *event, void *priv)
{
    // 处理与SD卡相关的设备事件
    if (!strcmp(event->arg, "sd0") || !strcmp(event->arg, "sd1") || !strcmp(event->arg, "sd2")) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:
            // SD卡插入事件处理
            __rec_msg_hide(BOX_MSG_INSERT_SD);                      // 隐藏插入SD卡的提示框
            ui_pic_show_image_by_id(ENC_PIC_SD, 1);               // 隐藏SD卡图标
            /* 如果没有正在录像，则更新剩余时间显示 */
            /* if (!if_in_rec) { */
            /*     ui_hide(ENC_TIM_REMAIN); */
            /*     ui_show(ENC_TIM_REMAIN); */
            /* } */
            break;
        case DEVICE_EVENT_OUT:
            // SD卡拔出事件处理
            __rec_msg_hide(BOX_MSG_NEED_FORMAT); // 隐藏需要格式化的提示框
            ui_pic_show_image_by_id(ENC_PIC_SD, 0);                   // 不在线SD卡图标
            if (if_in_rec) {
                // 如果正在录像，隐藏录制时间图标
                ui_hide(ENC_TIM_REC);
            }
            if (__this->sd_write_err) {
                // 如果有SD卡写入错误，隐藏相关提示并重置错误标志
                __rec_msg_hide(BOX_MSG_SD_WRITE_ERR);
                __this->sd_write_err = 0;
            }
            // 更新剩余时间的显示，重置剩余时间为0
            ui_hide(ENC_TIM_REMAIN);
            ui_show(ENC_TIM_REMAIN);
            __this->remain_time = 0;
            break;
        default:
            break;
        }
    } else if (!strcmp(event->arg, "video_rec_time")) {
        // 处理录像时间的变化事件
        switch (event->u.dev.event) {
        case DEVICE_EVENT_CHANGE:
            rec_cnt_handler(event->u.dev.value); // 调用录像计数处理函数
            break;
        default:
            break;
        }
    } else if (!strncmp((char *)event->arg, "rec", 3)) {
        // 处理录像设备的离线事件
        switch (event->u.dev.event) {
        case DEVICE_EVENT_OFFLINE:
            log_e("video rec write error"); // 记录录像写入错误日志
            if (!__this->sd_write_err) {
                // 如果未标记写入错误，则显示错误提示框并设置错误标志
                __rec_msg_show(BOX_MSG_SD_WRITE_ERR, 0);
                __this->sd_write_err = 1;
            }
            break;
        }
    }
}



/*
 * (begin)APP状态变更，UI响应回调 ***********************************
 */
static int rec_on_handler(const char *type, u32 arg)
{
    puts("\n***rec_on_handler.***\n");
    struct sys_time sys_time;
    get_sys_time(&sys_time);
    rec_cnt = sys_time.hour * 3600 + sys_time.min * 60 + sys_time.sec;
    if_in_rec = TRUE;
    ui_hide(ENC_TIM_REMAIN);
    ui_show(ENC_TIM_REC);
    // ui_show(ENC_ANI_REC_HL);
    ui_highlight_element_by_id(ENC_PIC_REC);
    return 0;
}
static int rec_off_handler(const char *type, u32 arg)
{
    puts("rec_off_handler.\n");

    rec_cnt = 0;

    if_in_rec = FALSE;
    if (__this->lock_file_flag) {
        __this->lock_file_flag = 0;
        ui_hide(ENC_PIC_LOCK);
    }
    ui_hide(ENC_TIM_REC);
    ui_show(ENC_TIM_REMAIN);
    // ui_hide(ENC_ANI_REC_HL);
    ui_no_highlight_element_by_id(ENC_PIC_REC);

    return 0;
}
static int rec_save_handler(const char *type, u32 arg)
{
    struct utime time_r;
    // 初始化录像时间，将小时、分钟和秒数设置为0
    time_r.hour = 0;
    time_r.min = 0;
    time_r.sec = 0;

    // 更新录像时间的UI显示
    ui_time_update_by_id(ENC_TIM_REC, &time_r);

    // 如果锁定文件标志被设置，则重置该标志并隐藏锁定图标
    if (__this->lock_file_flag) {
        __this->lock_file_flag = 0; // 重置锁定文件标志
        ui_hide(ENC_PIC_LOCK);      // 隐藏锁定文件的图标
    }

    return 0; // 函数成功执行后返回0
}

static int rec_no_card_handler(const char *type, u32 arg)
{
    puts("rec_no_card_handler.\n");
    __rec_msg_show(BOX_MSG_INSERT_SD, 3000);
    return 0;
}
static int rec_fs_err_handler(const char *type, u32 arg)
{
    if (fget_err_code(CONFIG_ROOT_PATH) == -EIO) {
        /* __rec_msg_show(BOX_MSG_SD_ERR, 3000); */
        __rec_msg_show(BOX_MSG_MEM_ERR, 3000);
    } else {
        __rec_msg_show(BOX_MSG_NEED_FORMAT, 3000);
    }
    return 0;
}
static int rec_av_in_handler(const char *type, u32 arg)
{
    av_in_statu = 1;  // 设置 av_in_statu 状态为 1，表示视频输入设备已连接
    // ui_show(ENC_BTN_SWITCH);  // 显示切换按钮
    return 0;  // 返回 0，表示操作成功
}

static int rec_av_off_handler(const char *type, u32 arg)
{
    av_in_statu = 0;
    // ui_hide(ENC_BTN_SWITCH);
    return 0;
}

static int rec_lock_handler(const char *type, u32 arg)
{
    puts("rec lock handler\n");
    __this->lock_file_flag = 1;
    ui_show(ENC_PIC_LOCK);
    return 0;
}



extern void play_voice_file(const char *file_name);
static int rec_headlight_on_handler(const char *type, u32 arg)
{
    puts("rec_headlight_on_handler\n");
    if (__this->hlight_show_status == 0) {
        __this->hlight_show_status = 1;
        ui_show(ENC_LAY_FLIG);//show head light
        play_voice_file("mnt/spiflash/audlogo/olight.adp");
    }
    return 0;
}
static int rec_headlight_off_handler(const char *type, u32 arg)
{
    puts("rec_headlight_off_handler\n");
    ui_hide(ENC_LAY_FLIG);//hide head light
    __this->hlight_show_status = 0;
    return 0;
}
static int rec_car_pos_handler(const char *type, u32 arg)
{
    /* video_rec_post_msg("carpos:p=%4",((u32)(x)|(y << 16)));//x:x y:y */
    /* video_rec_post_msg("carpos:w=%4",((u32)(w)|(c << 16)));//w:width c:color,0:green,1:yellow,2:red */
    /* video_rec_post_msg("carpos:s=%4",value);//0:hide , 1:show */
    puts("rec_car_pos_handler\n");
    u8 color;
    u32 tmp;
    if (*type == 'p') {
        tmp = arg;
        //注意宽高比例转换
        __this->car_pos_x = ((tmp & 0x0000ffff) * ((double)LCD_DEV_WIDTH / 640));
        __this->car_pos_y = (((tmp & 0xffff0000) >> 16) * ((double)720 / 360)) - (720 - LCD_DEV_HIGHT) / 2;
        //printf("car pos x=%d y=%d", __this->car_pos_x, __this->car_pos_y);
    } else if (*type == 'w') {
        tmp = arg;
        //注意宽高比例转换
        __this->car_pos_w = ((tmp & 0x0000ffff) * ((double)LCD_DEV_WIDTH / 640));
        __this->car_pos_c = (tmp & 0xffff0000) >> 16;
        //printf("width = %d\n",__this -> car_pos_w);
        //printf("car pos w=%d c=%d", __this->car_pos_w, __this->car_pos_c);
    } else if (*type == 's') {
        ui_hide(ENC_PIC_POS);
        ui_pic_show_image_by_id(ENC_PIC_POS, __this->car_pos_c);
    } else {
        return 0;
    }
    return 0;
}
static int rec_remain_handler(const char *type, u32 arg)
{
    printf("remain= %s %d\n", type, arg);
    if (type[0] == 's') {
        if (__this->remain_time != arg) {
            __this->remain_time = arg;
            if (if_in_rec == 0) {
                ui_hide(ENC_TIM_REMAIN);
                ui_show(ENC_TIM_REMAIN);
            }
        }
    }
    return 0;
}
// static int wifi_onoff_set_handler(const char *type, u32 arg)
// {
//     if(arg == 0){  // 如果参数arg为0
//         // ui_hide(PIC_REC_WIFI);  // 隐藏WiFi图标
//         ui_pic_show_image_by_id(PIC_REC_WIFI, 0);
//         wifi_off();  // 关闭WiFi
//         play_voice_file("mnt/spiflash/audlogo/wifi_off.adp");  // 播放WiFi关闭的语音文件
//     }else if(arg == 1){  // 如果参数arg为1
//         // ui_show(PIC_REC_WIFI);  // 显示WiFi图标、
//         ui_pic_show_image_by_id(PIC_REC_WIFI, 3);
//         wifi_on();  // 打开WiFi
//         play_voice_file("mnt/spiflash/audlogo/wifi_on.adp");  // 播放WiFi打开的语音文件
//     }
//     return 0;  // 返回0表示函数执行成功
// }
// static int app_link_msg_handler(const char *type, u32 arg)
// {
//     printf("==========app_link:%s,%d\n",type,arg);  // 打印出传入的type和arg值，方便调试
//     if (type[0] == 'a') {  // 如果type的第一个字符是'a'
//         if(arg == 0){
//            ui_pic_show_image_by_id(PIC_REC_WIFI, 0);  // 显示WiFi图片状态0
//         }else if(arg == 1){
//             ui_pic_show_image_by_id(PIC_REC_WIFI, 3);  // 显示WiFi图片状态1
//         }
//     }else if(type[0] == 'w'){  // 如果type的第一个字符是'w'
//         if(arg == 0){
//            __rec_msg_show(BOX_MSG_EXIT_APP, 3000);  // 显示退出应用的消息，持续3000毫秒
//         }else if(arg == 1){
//             __rec_msg_show(BOX_MSG_EXIT_APP, 3000);  // 显示退出应用的消息，持续3000毫秒
//         }
//     }

//     return 0;  // 返回0表示函数执行成功
// }
/*
 * 录像模式的APP状态响应回调
 */
static const struct uimsg_handl rec_msg_handler[] = {
    { "lockREC",        rec_lock_handler     }, /* 锁文件 */
    { "onREC",          rec_on_handler       }, /* 开始录像 */
    { "offREC",         rec_off_handler      }, /* 停止录像 */
    { "saveREC",        rec_save_handler     }, /* 保存录像 */
    { "noCard",         rec_no_card_handler  }, /* 没有SD卡 */
    { "fsErr",          rec_fs_err_handler   },
    { "avin",           rec_av_in_handler    },
    { "avoff",          rec_av_off_handler   },
    { "HlightOn",    rec_headlight_on_handler},
    { "HlightOff",   rec_headlight_off_handler},
    { "carpos",         rec_car_pos_handler  },
    // { "onMIC",          rec_on_mic_handler   },
    // { "offMIC",         rec_off_mic_handler  },

    // { "Remain",         rec_remain_handler  },
    // { "wifi",       wifi_onoff_set_handler},
    // { "app",       app_link_msg_handler},
    { NULL, NULL},      /* 必须以此结尾！ */
};
/*
 * (end)
 */
/*****************************录像模式页面回调 ************************************/
static int video_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    int err, item, id;
    const char *str = NULL;
    struct intent it;
    int ret;
    switch (e) {
    case ON_CHANGE_INIT:
        puts("\n***rec mode onchange init***\n");
        ui_register_msg_handler(ID_WINDOW_VIDEO_REC, rec_msg_handler); /* 注册APP消息响应 */
        sys_cur_mod = 1;  /* 1:rec, 2:tph, 3:dec */
        memset(__this, 0, sizeof_this);
        __this->backlight_val  = db_select("bkl");
        set_page_main_flag(1);
        break;
    case ON_CHANGE_RELEASE:
        if (__this->menu_status) {
            ui_hide(ENC_SET_WIN);
        }
        if (__this->page_exit == HOME_SW_EXIT) {
            ui_show(ID_WINDOW_MAIN_PAGE);
        }
        __rec_msg_hide(0);//强制隐藏消息框

        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ID_WINDOW_VIDEO_REC)
.onchange = video_mode_onchange,
 .ontouch = NULL,
};
static int parking_page_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        ui_register_msg_handler(ID_WINDOW_PARKING, rec_msg_handler); /* 注册APP消息响应 */
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ID_WINDOW_PARKING)
.onchange = parking_page_onchange,
 .ontouch = NULL,
};
static int rec_cid_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct intent it;
    int err;
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        ui_pic_show_image_by_id(ENC_PIC_CID_0, index_of_table16(db_select("cna") >> 16, TABLE(province_gb2312)));
        ui_pic_show_image_by_id(ENC_PIC_CID_1, index_of_table8((db_select("cna") >> 8) & 0xff, TABLE(num_table)));
        ui_pic_show_image_by_id(ENC_PIC_CID_2, index_of_table8((db_select("cna") >> 0) & 0xff, TABLE(num_table)));
        ui_pic_show_image_by_id(ENC_PIC_CID_3, index_of_table8((db_select("cnb") >> 24) & 0xff, TABLE(num_table)));
        ui_pic_show_image_by_id(ENC_PIC_CID_4, index_of_table8((db_select("cnb") >> 16) & 0xff, TABLE(num_table)));
        ui_pic_show_image_by_id(ENC_PIC_CID_5, index_of_table8((db_select("cnb") >> 8) & 0xff, TABLE(num_table)));
        ui_pic_show_image_by_id(ENC_PIC_CID_6, index_of_table8((db_select("cnb") >> 0) & 0xff, TABLE(num_table)));

        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_LAY_CID)
.onchange = rec_cid_onchange,
};

/*****************************图标布局回调 ************************************/
static void rec_layout_up_onchange_ok(void *p, int err)
{
    struct intent *it = p;
    if (it->exdata == 1) {
        //已加锁
        ui_show(ENC_PIC_LOCK);
        __this->lock_file_flag = 1;
    }

    if(usb_is_charging()){ // 如果设备正在充电
        ui_pic_show_image_by_id(LIGHT_ADJ_PIC, index_of_table8(db_select("bkl"), TABLE(table_light_lcd)));
        // 显示亮度调整图片，并根据当前亮度设置相应的图像
    }else{
        ui_pic_show_image_by_id(LIGHT_ADJ_PIC, 0);
        // 如果设备未在充电，则隐藏亮度调整图片
    }
}

static int rec_layout_up_onchange(void *ctr, enum element_change_event e, void *arg)
{
    int item, id;
    const char *str = NULL;
    struct intent it;
    int ret;
    int index;
    int err;
    static int lock_event_flag = 0;  // 静态变量，用于判断是否是第一次调用

    switch (e) {
    case ON_CHANGE_INIT:
        // 在初始化时注册系统事件处理程序
        // lock_event_id = register_sys_event_handler(SYS_DEVICE_EVENT, 0, 0, lock_event_handler);
        break;

    case ON_CHANGE_RELEASE:
        // 在控件释放时注销系统事件处理程序
        // unregister_sys_event_handler(lock_event_id);
        break;

    case ON_CHANGE_FIRST_SHOW:  /* 处理控件第一次显示的逻辑 */
#ifdef CONFIG_VIDEO4_ENABLE
        // // 如果启用了 CONFIG_VIDEO4，设置 av_in_statu，并显示 ENC_BTN_SWITCH 按钮
        // av_in_statu = 1;
        // ui_show(ENC_BTN_SWITCH);
#else
        // 如果 "uvc" 设备或 "video1.*" 设备在线，显示 ENC_BTN_SWITCH 按钮
        // if (dev_online("uvc") || dev_online("video1.*")) {
        //     av_in_statu = 1;
        //     ui_show(ENC_BTN_SWITCH);
        // }
#endif

        // 如果存储设备已准备好，显示 SD 卡图标
        if (storage_device_ready() == 0) {
            ui_pic_show_image_by_id(ENC_PIC_SD, 1);                   // 在线SD卡图标
        }

        // 获取录像加锁状态，并异步启动应用
        // if (lock_event_flag && if_in_rec) {  // 第一次不获取状态，防止 start_app 超时
        //     init_intent(&it);
        //     it.name = "video_rec";
        //     it.action = ACTION_VIDEO_REC_LOCK_FILE;
        //     it.data = "get_lock_statu";
        //     start_app_async(&it, rec_layout_up_onchange_ok, (void *)&it);
        // }
        // lock_event_flag = 1;

#ifndef CONFIG_VIDEO4_ENABLE
        // 如果运动检测打开，显示运动图标
        // if (db_select("mot")) {
        //     ui_show(ENC_PIC_MOVE);
        // }
#endif

        // 根据传感器状态显示图标
        // index = db_select("gra");
        // if (index != 0) {
        //     ui_pic_show_image_by_id(ENC_PIC_GSEN, index - 1);
        // } else {
        //     ui_hide(ENC_PIC_GSEN);
        // }

        // 如果防盗模式打开，显示防盗图标
        // if (db_select("par")) {
        //     ui_show(ENC_PIC_GUARD);
        // }

#ifndef CONFIG_VIDEO4_ENABLE
        // 如果开启了宽动态范围 (WDR)，显示对应图标
        // if (db_select("wdr")) {
        //     ui_show(ENC_PIC_HDR);
        // }
#endif

        // 显示视频循环录制时间
        index = index_of_table8(db_select("cyc"), TABLE(table_video_cycle));
        if (index != 0) {
            ui_pic_show_image_by_id(ENC_PIC_CYCLE, index);
        }

        // 显示视频间隔拍摄时间
        index = index_of_table16(db_select("gap"), TABLE(table_video_gap));
        if (index) {
            ui_pic_show_image_by_id(ENC_PIC_DELAY, index);
        }

#ifndef CONFIG_VIDEO4_ENABLE
        // 显示视频分辨率图标
        // index = index_of_table8(db_select("res"), TABLE(table_video_resolution));
        // if (index != 0) {
        //     ui_pic_show_image_by_id(ENC_PIC_RESOLUTION, index);
        // }
#else
        // 如果启用了 CONFIG_VIDEO4，直接显示 720P 的分辨率图标
        // ui_pic_show_image_by_id(ENC_PIC_RESOLUTION, VIDEO_RES_720P);
#endif

        // 如果开启了车牌号显示功能，显示对应图层
        // if (db_select("num")) {
        //     ui_show(ENC_LAY_CID);
        // }

        // 如果录像正在进行，更新 UI 显示
        if (if_in_rec == TRUE) {
            ui_hide(ENC_TIM_REMAIN);  // 隐藏剩余时间
            ui_show(ENC_TIM_REC);     // 显示正在录像的时间
            // ui_show(ENC_ANI_REC_HL);  // 显示录像中的动画效果
            ui_highlight_element_by_id(ENC_PIC_REC);
        }
        //  if(db_select("wfo")){  // 如果数据库查询“wfo”返回为真（即查询成功或有记录）
        //     //ui_pic_show_image_by_id(PIC_REC_WIFI, wifi_app_flag);  // 根据wifi_app_flag显示WiFi图标对应的状态
        //      ui_pic_show_image_by_id(PIC_REC_WIFI, 3);  // 根据wifi_app_flag显示WiFi图标对应的状态
        // }else{
        //     ui_pic_show_image_by_id(PIC_REC_WIFI, 0);
        // }

        break;

    default:
        return false;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(SYS_LAY) // 注册 UI 事件处理函数
.onchange = rec_layout_up_onchange,
};

static int rec_layout_button_ontouch(void *ctr, struct element_touch_event *e)
{
#define GAP_VAL  8  //
#define BACLIGHT_MAX  100
#define BACLIGHT_MIN  20

    UI_ONTOUCH_DEBUG("**rec layout button ontouch**");
    struct intent it;
    struct application *app;
    static u16 down_y = 0;
    s16 y_ch = 0;
    s16 tmp = 0;
    static s16 backlight_val = 0;
    static u8 is_move = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        down_y = e->pos.y;
        backlight_val = __this->backlight_val;
        is_move = 0;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        y_ch = down_y - e->pos.y;
        tmp = backlight_val;
        if (y_ch < GAP_VAL && y_ch > -GAP_VAL) {
            return false;
        }
        tmp = backlight_val + y_ch / GAP_VAL;
        if (tmp > BACLIGHT_MAX) {
            tmp = BACLIGHT_MAX;
            down_y = e->pos.y;
            backlight_val = tmp;
        } else if (tmp < BACLIGHT_MIN) {
            tmp = BACLIGHT_MIN;
            down_y = e->pos.y;
            backlight_val = tmp;
        }
        /* printf("\n tmp_backlight = %d \n", tmp); */
        if (backlight_val == tmp) {
            return false;
        }
        is_move = 1;
        menu_rec_set_backlight(tmp);
        backlight_val = tmp;

        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if (is_move) {
            break;
        }
        if (__this->menu_status) {
            if (if_in_rec) {
                __this->onkey_mod = 0;
                __this->onkey_sel = 0;
                disp_RecSetting_lay(0);
                break;
            }
            disp_RecSetting_lay(0);
            __this->onkey_mod = 1;
            __this->onkey_sel = 1;
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_BTN_BASE)
.ontouch = rec_layout_button_ontouch,
};
static int ani_headlight_onchange(void *_ani, enum element_change_event e, void *arg)
{
    UI_ONTOUCH_DEBUG("ani_headlight_onchange: %d\n", e);
    switch (e) {
    case ON_CHANGE_SHOW_PROBE:
        if (msg_show_f && __this->hlight_show_status) {
            //有提示框时隐藏前照灯
            ui_hide(ENC_LAY_FLIG);//hide head light
        }
        break;
    default:
        break;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(ENC_ANI_FLIG)
.onchange = ani_headlight_onchange,
};
/***************************** 录像按钮显示 ************************************/
static int enc_rec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        // if (__this->onkey_mod == 0) {
        //     ui_highlight_element_by_id(ENC_PIC_REC);
        // } else {
        //     ui_no_highlight_element_by_id(ENC_PIC_REC);
        // }
        if (if_in_rec == 1) {
            ui_highlight_element_by_id(ENC_PIC_REC);
        } else {
            ui_no_highlight_element_by_id(ENC_PIC_REC);
        }
        return TRUE;
    default:
        return FALSE;
    }
    return FALSE;
}

REGISTER_UI_EVENT_HANDLER(ENC_LT_SIX)
.onchange = enc_rec_onchange,
};
/***************************** 录像设置显示 ************************************/
// static int enc_set_onchange(void *ctr, enum element_change_event e, void *arg)
// {
//     switch (e) {
//     case ON_CHANGE_FIRST_SHOW:
//         if (__this->onkey_sel) {
//             ui_highlight_element_by_id(onkey_sel_setting[0]);
//             ui_highlight_element_by_id(onkey_sel_setting1[0]);
//         }
//         return TRUE;
//     default:
//         return FALSE;
//     }
//     return FALSE;
// }

// REGISTER_UI_EVENT_HANDLER(ENC_SET_LAY)
// .onchange = enc_set_onchange,
// };
/***************************** MIC 图标动作 ************************************/
static int pic_mic_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    switch (e) {
    case ON_CHANGE_INIT:
        if (db_select("mic")) {
            ui_pic_set_image_index(pic, 1);    /* 禁止录音 */
        } else {
            ui_pic_set_image_index(pic, 0);
        }
        return TRUE;
    default:
        return FALSE;
    }
    return FALSE;
}

REGISTER_UI_EVENT_HANDLER(ENC_PIC_SOUND)
.onchange = pic_mic_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
/***************************** 系统时间控件动作 ************************************/
static int timer_sys_rec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_time *time = (struct ui_time *)ctr;
    struct sys_time sys_time;

    switch (e) {
    case ON_CHANGE_INIT:
    case ON_CHANGE_SHOW:
        get_sys_time(&sys_time);
        time->year = sys_time.year;
        time->month = sys_time.month;
        time->day = sys_time.day;
        time->hour = sys_time.hour;
        time->min = sys_time.min;
        time->sec = sys_time.sec;
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_TIM_TIME)
.onchange = timer_sys_rec_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

/***************************** 录像时间控件动作 ************************************/
static int timer_rec_red_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_time *time = (struct ui_time *)ctr;

    switch (e) {
    case ON_CHANGE_INIT:
        if (rec_cnt) {
            struct sys_time sys_time;
            get_sys_time(&sys_time);
            int rtime = sys_time.hour * 3600 + sys_time.min * 60 + sys_time.sec - rec_cnt;
            time->hour = rtime / 60 / 60;
            time->min = rtime / 60 % 60;
            time->sec = rtime % 60;
        } else {
            time->hour = 0;
            time->min = 0;
            time->sec = 0;
        }
        break;
    case ON_CHANGE_HIDE:
        time->hour = 0;
        time->min = 0;
        time->sec = 0;
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_PROBE:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_TIM_REC)
.onchange = timer_rec_red_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#if 0
static int retime_buf_transform(char *retime_buf, struct ui_time *time)
{
    u32 cur_space;
    u32 one_pic_size;
    int err = 0;
    int hour, min, sec;
    int i, s;
    err = fget_free_space(CONFIG_ROOT_PATH, &cur_space);
    if (err) {
        hour = 0;
        min = 0;
        sec = 0;
    } else {
        u32 res = db_select("res");
        if (res == VIDEO_RES_1080P) {
            one_pic_size = (0x21000 + 0xa000) / 1024;
        } else if (res == VIDEO_RES_720P) {
            one_pic_size = (0x13000 + 0xa000) / 1024;
        } else {
            one_pic_size = (0xa000 + 0xa000) / 1024;
        }
        hour = (cur_space / one_pic_size) / 30 / 60 / 60;
        min = (cur_space / one_pic_size) / 30 / 60 % 60;
        sec = (cur_space / one_pic_size) / 30 % 60;
    }
    sprintf(retime_buf, "%2d.%2d.%2d", hour, min, sec);
    printf("retime_buf: %s\n", retime_buf);
    i = 0;
    s = 10;
    time->hour = 0;
    while (retime_buf[i] != '.' && retime_buf[i] != '\0') {
        if (retime_buf[i] >= '0' && retime_buf[i] <= '9') {
            time->hour += ((retime_buf[i] - '0') * s);
        }
        i++;
        s = s / 10;
    }
    i++;
    s = 10;
    time->min = 0;
    while (retime_buf[i] != '.' && retime_buf[i] != '\0') {
        if (retime_buf[i] >= '0' && retime_buf[i] <= '9') {
            time->min += ((retime_buf[i] - '0') * s);
        }
        i++;
        s = s / 10;
    }
    i++;
    s = 10;
    time->sec = 0;
    while (retime_buf[i] != '.' && retime_buf[i] != '\0') {
        if (retime_buf[i] >= '0' && retime_buf[i] <= '9') {
            time->sec += ((retime_buf[i] - '0') * s);
        }
        i++;
        s = s / 10;
    }

    return err;
}
#endif
/***************************** 录像剩余时间控件动作 ************************************/
static int timer_rec_remain_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_time *time = (struct ui_time *)ctr;
    struct intent it;
    int err;
    static char retime_buf[30];
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        if (storage_device_ready() == 0) {
            /*
             * 第一次显示需要判断一下SD卡是否在线
             */
            time->hour = 0;
            time->min = 0;
            time->sec = 0;
            break;
        }
        if (__this->lock_file_flag == 1) {
            ui_hide(ENC_TIM_REMAIN);
            break;
        }
        break;
    case ON_CHANGE_SHOW_PROBE:
        if (storage_device_ready() == 0) {
            __this->remain_time = 0;
        }

        time->hour = __this->remain_time / 3600;
        time->min = __this->remain_time % 3600 / 60;
        time->sec = __this->remain_time % 60;
        printf("reTIME hour:%02d, min:%02d, sec:%02d\n", time->hour, time->min, time->sec);
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_TIM_REMAIN)
.onchange = timer_rec_remain_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static void no_power_msg_box_timer(void *priv)
{
    static u32 cnt = 0;  // 定义并初始化静态计数器，用于控制提示信息的闪烁

    // 检查电池电量值和充电状态
    if (__this->battery_val <= 20 && __this->battery_char == 0) {
        cnt++;  // 每次定时器触发时，计数器自增

        // 每隔两次定时器触发闪烁一次提示框
        if ((cnt % 2) == 0) {
            puts("no power show.\n");  // 输出提示信息 "no power show"
            __rec_msg_show(BOX_MSG_NO_POWER, 0);  // 显示低电量提示框
        } else {
            puts("no power hide.\n");  // 输出提示信息 "no power hide"
            __rec_msg_hide(BOX_MSG_NO_POWER);  // 隐藏低电量提示框
        }
    } else {
        // 如果电池电量正常或者设备正在充电，隐藏低电量提示框并重置计数器
        __rec_msg_hide(BOX_MSG_NO_POWER);  // 隐藏提示框
        cnt = 0;  // 计数器重置为0
    }
}

/*
 * battery事件处理函数
 */
static void battery_event_handler(struct sys_event *event, void *priv)
{
    static u8 ten_sec_off = 0;
    // 静态变量，用于记录是否已经显示10秒的关机提示，如果值为1表示提示显示中

    if (ten_sec_off) {
        if (event->type == SYS_KEY_EVENT || event->type == SYS_TOUCH_EVENT) {
            // 如果收到按键事件或触摸事件，并且ten_sec_off为1，则隐藏关机提示
            ten_sec_off = 0;
            __rec_msg_hide(0); // 隐藏关机提示信息
            return;
        }
    }

    if (__this->sd_write_err) {
        if (event->type == SYS_KEY_EVENT || event->type == SYS_TOUCH_EVENT) {
            // 如果当前有SD卡写入错误，并且收到按键或触摸事件，隐藏SD卡写入错误提示
            __rec_msg_hide(BOX_MSG_SD_WRITE_ERR); // 隐藏SD卡写入错误提示信息
            __this->sd_write_err = 0; // 重置SD卡写入错误标志
        }
    }

    if (event->type == SYS_DEVICE_EVENT) {
        // 处理设备事件
        if (!ASCII_StrCmp(event->arg, "sys_power", 9)) {
            // 检查事件参数是否与“sys_power”匹配
            if (event->u.dev.event == DEVICE_EVENT_POWER_PERCENT) {
                // 如果是电池电量百分比变化事件
                __this->battery_val = event->u.dev.value; // 获取当前电池电量值
                if (__this->battery_val > 100) {
                    __this->battery_val = 100; // 如果电量值超过100，限制为100
                }
                if (__this->battery_char == 0) {
                    // 如果没有在充电状态，更新电池显示为当前电量
                    ui_battery_level_change(__this->battery_val, 0);
                }
            } else if (event->u.dev.event == DEVICE_EVENT_POWER_CHARGER_IN) {
                // 如果检测到充电器插入事件
                ui_battery_level_change(100, 1); // 将电池显示更新为满电状态，充电中
                __this->battery_char = 1; // 设置充电状态为1（正在充电）
                // if (ten_sec_off) {
                //     ten_sec_off = 0; // 取消10秒关机提示
                //     __rec_msg_hide(0); // 隐藏关机提示信息
                // }
            } else if (event->u.dev.event == DEVICE_EVENT_POWER_CHARGER_OUT) {
                // 如果检测到充电器拔出事件
                ui_battery_level_change(__this->battery_val, 0); // 更新电池电量为实际电量值，非充电状态
                __this->battery_char = 0; // 设置充电状态为0（未充电）
                // __rec_msg_show(BOX_MSG_POWER_OFF, 0); // 显示关机提示信息
                // ten_sec_off = 1; // 开始10秒倒计时，标志关机提示显示中
            }
        }
    }
}

/*****************************主界面电池控件动作 ************************************/
static int battery_rec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_battery *battery = (struct ui_battery *)ctr;
    static u16 id = 0; // 静态变量，存储系统事件处理器的ID
    static u32 timer_handle = 0; // 静态变量，存储定时器句柄

    switch (e) {
    case ON_CHANGE_INIT:
        // 当UI控件初始化时，注册电池相关的系统事件处理器，处理按键、触摸及设备事件
        id = register_sys_event_handler(SYS_DEVICE_EVENT | SYS_KEY_EVENT | SYS_TOUCH_EVENT, 200, 0, battery_event_handler);
        break;

    case ON_CHANGE_FIRST_SHOW:
        // 当UI控件第一次显示时，获取当前的电池电量，并更新电池显示状态
        __this->battery_val = sys_power_get_battery_persent(); // 获取当前电池电量百分比

        /* u32 power_level = 0; */
        /* dev_ioctl(fd, POWER_DET_GET_LEVEL, (u32)&power_level); */
        /* __this->battery_val = power_level * 20; */
        // 注释部分为可选代码，用于从设备中获取电池电量的替代实现

        if (__this->battery_val > 100) {
            __this->battery_val = 100; // 如果电量超过100%，则限制为100%
        }

        // 检查设备是否在充电状态，保存充电状态到__this结构体中
        __this->battery_char = (usb_is_charging() ? 1 : 0); // 判断是否正在充电，设置充电状态
        __this->battery_char = 0; // 判断是否正在充电，设置充电状态
        ui_battery_level_change(__this->battery_val, __this->battery_char); // 更新UI中的电池显示，电量值和充电状态
        timer_handle = sys_timer_add(NULL, no_power_msg_box_timer, 1000); // 添加一个定时器，每秒触发一次，用于定期检查
        break;

    case ON_CHANGE_RELEASE:
        // 当UI控件被释放时，取消注册的事件处理器，并删除定时器
        unregister_sys_event_handler(id); // 取消之前注册的系统事件处理器
        if (timer_handle) {
            sys_timer_del(timer_handle); // 如果定时器存在，删除定时器
            timer_handle = 0; // 重置定时器句柄为0
        }
        break;

    default:
        return false; // 如果事件不属于上述类型，则返回false，表示没有处理该事件
    }

    return false; // 默认返回false
}

// 注册UI事件处理程序
REGISTER_UI_EVENT_HANDLER(ENC_BAT)
.onchange = battery_rec_onchange, // 设置onchange事件处理器
.ontouch = NULL, // 未处理触摸事件
};



// static int rec_set_lock_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**rec set lock ontouch**");  // 调试输出：录制设置锁触摸事件
//     struct intent it;  // 创建意图结构体
//     switch (e->event) {  // 根据触摸事件类型进行处理
//     case ELM_EVENT_TOUCH_UP:  // 触摸抬起事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");  // 调试输出：触摸抬起事件
//         if (if_in_rec == TRUE) {  // 如果当前在录制状态
//             init_intent(&it);  // 初始化意图结构体
//             __this->lock_file_flag = !__this->lock_file_flag;  // 切换锁定文件标志
//             it.name = "video_rec";  // 设置意图的名称为 "video_rec"
//             it.action = ACTION_VIDEO_REC_LOCK_FILE;  // 设置意图的动作为锁定文件
//             it.data = "set_lock_statu";  // 设置意图的数据为 "set_lock_statu"
//             it.exdata = __this->lock_file_flag;  // 设置意图的附加数据为当前锁定状态
//             start_app(&it);  // 启动应用程序并传递意图
//             if (__this->lock_file_flag == 1) {  // 如果锁定文件标志为 1
//                 puts("show lock\n");  // 输出 "show lock" 表示显示锁定图标
//                 ui_show(ENC_BTN_LOCK);  // 显示锁定图标
//             } else {
//                 puts("hide lock\n");  // 输出 "hide lock" 表示隐藏锁定图标
//                 ui_hide(ENC_BTN_LOCK);  // 隐藏锁定图标
//             }
//         }
//         break;
//     }
//     return false;  // 返回 false 表示事件未被完全处理
// }

// REGISTER_UI_EVENT_HANDLER(ENC_BTN_LOCK)  // 注册 UI 事件处理程序
// .ontouch = rec_set_lock_ontouch,  // 将触摸事件处理函数与按钮关联
// };


extern int storage_device_ready();
/***************************** SD 卡图标动作 ************************************/
// pic_sd_rec_onchange函数：处理SD卡图标的变化事件
static int pic_sd_rec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    // 获取图片UI元素的指针
    struct ui_pic *pic = (struct ui_pic *)ctr;

    // 定义静态变量保存事件处理器ID
    static u16 id = 0;

    // 根据事件类型进行处理
    switch (e) {
    case ON_CHANGE_INIT:
        id = register_sys_event_handler(SYS_DEVICE_EVENT, 250, 0, sd_event_handler);
        break;
    case ON_CHANGE_RELEASE:
        // 释放资源时，取消注册系统事件处理器
        unregister_sys_event_handler(id);
        break;
     case ON_CHANGE_FIRST_SHOW:
        if (storage_device_ready() == 0) {
            ui_pic_set_image_index(pic, 0);
        } else {
            ui_pic_set_image_index(pic, 1);
        }

        break;
    default:
        return false; // 对于其他事件类型，不做处理
    }
    return false; // 事件处理完成，返回false表示没有进一步操作
}

// 注册UI事件处理程序，将pic_sd_rec_onchange函数与ENC_PIC_SD元素的变化事件关联
REGISTER_UI_EVENT_HANDLER(ENC_PIC_SD)
.onchange = pic_sd_rec_onchange,
.onkey = NULL,   // 不处理按键事件
.ontouch = NULL, // 不处理触摸事件
};




/***************************** RP设置按钮动作 ************************************/
static int is_setting_shown = 0; // 静态变量，用于跟踪当前按钮的显示状态
static int rec_set_ontouch(void *ctr, struct element_touch_event *e)
{

    struct intent it; // 定义意图结构
    struct application *app; // 定义应用程序结构
    UI_ONTOUCH_DEBUG("**rec set ontouch**"); // 输出调试信息，表示进入了触摸处理函数

    switch (e->event) { // 根据触摸事件类型进行处理
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n"); // 输出调试信息，表示触摸按下
        // screen_light_set();

        break;

    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n"); // 输出调试信息，表示触摸保持
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n"); // 输出调试信息，表示触摸移动
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 输出调试信息，表示触摸抬起
        // if (if_in_rec == TRUE) { // 如果当前处于录制状态
        //     ui_set_call(show_tips, 0); // 调用显示提示函数
        //     puts("It is in rec, can't switch mode.\n"); // 输出提示信息，表示无法切换模式
        //     break;
        // }
        if (is_setting_shown) {
            // ui_hide(ENC_SET_WIN); // 隐藏二级菜单
            // ui_hide(ENC_MENU_SET_2);    // 隐藏二级菜单2
            ui_hide(ENC_SET_WIN0);
            // ui_hide(ENC_BTN_NN);    // 隐藏二级菜单切换按钮
            //ui_show(ENC_BTN_SPIN);   // 显示方向键
            is_setting_shown = 0; // 更新状态为关闭
            ui_no_highlight_element_by_id(ENC_SET_WIN_2);
        } else {
            // ui_show(ENC_BTN_NN);// 显示二级菜单切换按钮
            ui_show(ENC_SET_WIN0);
            // ui_show(ENC_SET_WIN); // 显示二级菜单1
           //ui_hide(ENC_BTN_SPIN);   // 隐藏方向键
            is_setting_shown = 1; // 更新状态为打开
            ui_highlight_element_by_id(ENC_SET_WIN_2); // 高亮二级菜单切换按钮

        }
        #ifdef VOICE_CONTROL_ENABLE
        // rec_layout_speak_ctr_timer(0, 6 * 1000); // 如果启用了语音控制，设置语音控制计时器
        #endif
        // init_intent(&it); // 初始化意图结构
        // app = get_current_app(); // 获取当前应用程序
        // if (app) { // 如果当前应用程序存在
        //     __this->page_exit = MODE_SW_EXIT; // 设置页面退出模式
        //     it.name = "video_rec"; // 设置意图名称为“video_rec”
        //     it.action = ACTION_BACK; // 设置意图动作为“返回”
        //     start_app_async(&it, NULL, NULL); // 异步启动应用程序，不等待直接启动

        //     init_intent(&it); // 重新初始化意图结构
        //     it.name = "video_system"; // 设置意图名称为“video_system”
        //     it.action = ACTION_SYSTEM_MAIN; // 设置意图动作为“系统主界面”
        //     start_app_async(&it, NULL, NULL); // 异步启动应用程序，不等待直接启动
        //}
        break;
    }
    return 1; // 返回false表示未处理完事件，事件继续传递
}

REGISTER_UI_EVENT_HANDLER(ENC_SET_WIN_2) // 注册UI事件处理器，处理ENC_BTN_SETTING按钮的触摸事件
.ontouch = rec_set_ontouch, // 设置触摸处理函数为rec_set_ontouch
};

/***************************** 按下隐藏二级菜单 ************************************/
static int rec_seten_ontouch(void *ctr, struct element_touch_event *e)
{

    struct intent it; // 定义意图结构
    struct application *app; // 定义应用程序结构
    UI_ONTOUCH_DEBUG("**rec set ontouch**"); // 输出调试信息，表示进入了触摸处理函数

    switch (e->event) { // 根据触摸事件类型进行处理
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n"); // 输出调试信息，表示触摸按下
        // screen_light_set();

        break;

    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n"); // 输出调试信息，表示触摸保持
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n"); // 输出调试信息，表示触摸移动
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 输出调试信息，表示触摸抬起
        ui_hide(ENC_SET_WIN0);
        is_setting_shown = 0; // 更新状态为关闭
        ui_no_highlight_element_by_id(ENC_SET_WIN_2);
        break;
    }
    return 1; // 返回false表示未处理完事件，事件继续传递
}

REGISTER_UI_EVENT_HANDLER(ENC_SET_EN) // 注册UI事件处理器，处理ENC_BTN_SETTING按钮的触摸事件
.ontouch = rec_seten_ontouch, // 设置触摸处理函数为rec_set_ontouch
};
REGISTER_UI_EVENT_HANDLER(ENC_SET_EN1) // 注册UI事件处理器，处理ENC_BTN_SETTING按钮的触摸事件
.ontouch = rec_seten_ontouch, // 设置触摸处理函数为rec_set_ontouch
};

/***************************** 按下隐藏两边菜单 ************************************/
static int is_six_shown = 0; // 静态变量，用于跟踪当前按钮的显示状态
static int rec_seten2_ontouch(void *ctr, struct element_touch_event *e)
{

    struct intent it; // 定义意图结构
    struct application *app; // 定义应用程序结构
    UI_ONTOUCH_DEBUG("**rec set ontouch**"); // 输出调试信息，表示进入了触摸处理函数

    switch (e->event) { // 根据触摸事件类型进行处理
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n"); // 输出调试信息，表示触摸按下
        // screen_light_set();

        break;

    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n"); // 输出调试信息，表示触摸保持
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n"); // 输出调试信息，表示触摸移动
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 输出调试信息，表示触摸抬起
    if(  is_setting_shown == 0){
        if (is_six_shown) {
            ui_hide(ENC_LT_TC);
            // ui_hide(ENC_LT_SIX2);
            is_six_shown = 0; // 更新状态为关闭
            ui_no_highlight_element_by_id(ENC_SET_WIN_2);
        } else {
            ui_show(ENC_LT_TC);
            // ui_show(ENC_LT_SIX2);
            is_six_shown = 1; // 更新状态为打开
        }
    }else{
        return false;
        }
        break;
    }
    return 1; // 返回false表示未处理完事件，事件继续传递
}

REGISTER_UI_EVENT_HANDLER(ENC_SET_EN2) // 注册UI事件处理器，处理ENC_BTN_SETTING按钮的触摸事件
.ontouch = rec_seten2_ontouch, // 设置触摸处理函数为rec_set_ontouch
};


// /***************************** 亮度调节按钮动作 ************************************/
// #define SLID_GAP  68  // 每一项的间隔，表示滑动调节的间隔长度 (滑块长度/项目数)
// #define SLID_ITEM 6  // 项目数，表示亮度调节共有6个档位
// static int screen_light_adjustment_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**screen_light_down_ontouch**"); // 调试信息，标识触摸事件触发
//     struct intent it; // 定义意图结构体，用于后续操作
//     struct application *app; // 定义应用程序结构体，用于后续操作
//     static int i, tmp; // 静态变量i用于记录当前亮度档位，tmp用于临时存储调节后的亮度档位
//     static s16 x_pos_down = 0; // 静态变量x_pos_down用于记录触摸按下时的X坐标
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
//          x_pos_down = e->pos.x; // 记录触摸按下时的X坐标
//          i = index_of_table8(db_select("bkl"), TABLE(table_light_lcd)); // 从数据库中获取当前亮度档位
//          UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n"); // 打印调试信息
//         return true; // 返回true表示事件已处理
//         break;
//     case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n"); // 打印调试信息
//         break;
//     case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n"); // 打印调试信息
//          if (!usb_is_charging()) { // 如果当前不是充电状态，跳出
//             break;
//         }
//         s16 x_pos_now = e->pos.x; // 获取当前触摸点的X坐标
//         s16 x_pos_ch = x_pos_now - x_pos_down; // 计算触摸滑动的位移量
//         if (x_pos_ch < SLID_GAP && x_pos_ch > -SLID_GAP) { // 如果滑动距离小于间隔值，不作处理
//            return false; // 返回false，事件未处理
//        }
//         tmp = i + x_pos_ch / SLID_GAP; // 根据滑动距离计算新的亮度档位
//         if (tmp > SLID_ITEM - 1) { // 如果新档位大于最大值，限制为最大值
//             tmp = SLID_ITEM - 1;
//             x_pos_down = x_pos_now; // 更新触摸起点
//         } else if (tmp < 0) { // 如果新档位小于最小值，限制为最小值
//             tmp = 0;
//             x_pos_down = x_pos_now; // 更新触摸起点
//         }
//         if (tmp == i) { // 如果新档位和当前档位相同，不作处理
//             return false;
//         }
//         screen_light_set(tmp); // 设置新的亮度档位
//         i = tmp; // 更新当前档位
//         x_pos_down = x_pos_now; // 更新触摸起点
//         break;
//     case ELM_EVENT_TOUCH_UP: // 触摸抬起事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 打印调试信息
//         break;
//     }
//     return false; // 返回false，事件未处理
// }

// REGISTER_UI_EVENT_HANDLER(LIGHT_ADJ_PIC) // 注册亮度调节图片的触摸事件处理程序
// .ontouch = screen_light_adjustment_ontouch, // 将亮度调节触摸处理函数与控件绑定
// };

/***************************** 亮度调节按钮动作（加/减按钮调节） ************************************/
#define MAX_BRIGHTNESS_LEVEL 5 // 最大亮度级别，共6个档位（从0到5）
#define MIN_BRIGHTNESS_LEVEL 0 // 最小亮度级别

static int current_brightness_level = 0; // 用于保存当前的亮度档位

// 获取系统亮度的函数
static void get_system_brightness()
{
    // 从系统中读取当前的亮度值
    int brightness = index_of_table8(db_select("bkl"), TABLE(table_light_lcd));

    // 如果成功获取亮度，更新当前亮度档位
    if (brightness >= MIN_BRIGHTNESS_LEVEL && brightness <= MAX_BRIGHTNESS_LEVEL) {
        current_brightness_level = brightness;
        UI_ONTOUCH_DEBUG("System Brightness Level: %d\n", current_brightness_level);
    } else {
        // 如果未能成功获取亮度，设置为默认值
        current_brightness_level = 3; // 默认亮度级别
        UI_ONTOUCH_DEBUG("Failed to get system brightness, setting to default: %d\n", current_brightness_level);
    }
}

// 增加亮度按钮的触摸事件处理
static int screen_light_increase_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_light_increase_ontouch**\n"); // 调试信息
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP: // 触摸松开事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - Increase Brightness\n");
        get_system_brightness(); // 获取系统当前亮度
        if (current_brightness_level < MAX_BRIGHTNESS_LEVEL) { // 如果当前亮度未达到最大值
            current_brightness_level++; // 增加亮度
            screen_light_set(current_brightness_level); // 设置新的亮度
            UI_ONTOUCH_DEBUG("Brightness Level: %d\n", current_brightness_level);
        } else {
            UI_ONTOUCH_DEBUG("Brightness is already at maximum level\n");
        }
        break;
    default:
        break;
    }
    return false; // 返回false，事件未处理
}

// 减少亮度按钮的触摸事件处理
static int screen_light_decrease_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_light_decrease_ontouch**\n"); // 调试信息
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP: // 触摸松开事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - Decrease Brightness\n");
        get_system_brightness(); // 获取系统当前亮度
        if (current_brightness_level > MIN_BRIGHTNESS_LEVEL) { // 如果当前亮度未达到最小值
            current_brightness_level--; // 减少亮度
            screen_light_set(current_brightness_level); // 设置新的亮度
            UI_ONTOUCH_DEBUG("Brightness Level: %d\n", current_brightness_level);
        } else {
            UI_ONTOUCH_DEBUG("Brightness is already at minimum level\n");
        }
        break;
    default:
        break;
    }
    return false; // 返回false，事件未处理
}

// 程序启动时或需要时调用获取系统亮度
 // 在程序启动时，调用该函数以获取系统的当前亮度

// 注册增加亮度按钮的触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(LIGHT_INCREASE_BTN) // 这里的LIGHT_INCREASE_BTN是增加亮度按钮的控件ID
.ontouch = screen_light_increase_ontouch, // 将增加亮度触摸处理函数与控件绑定
};

// 注册减少亮度按钮的触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(LIGHT_DECREASE_BTN) // 这里的LIGHT_DECREASE_BTN是减少亮度按钮的控件ID
.ontouch = screen_light_decrease_ontouch, // 将减少亮度触摸处理函数与控件绑定
};





/***************************** 录像开始结束动作 ************************************/
static void rec_control_ok(void *p, int err)
{
    if (err == 0) {
        puts("---rec control ok\n");  // 录像控制成功
    } else {
        printf("---rec control failed: %d\n", err);  // 录像控制失败，输出错误码
    }
    sys_touch_event_enable();  // 启用触摸事件
    __this->key_disable = 0;  // 重新启用按键
}

static int rec_control_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**rec control ontouch**");  // 调试信息，触摸事件开始
    struct intent it;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");  // 触摸按下事件
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");  // 触摸保持事件
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");  // 触摸移动事件
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");  // 触摸抬起事件

         if (__this->onkey_mod == 1) {
            // ui_no_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]);  // 取消高亮选中的元素
            // ui_highlight_element_by_id(ENC_PIC_REC);  // 高亮录像图标
            __this->onkey_mod = 0;  // 重置按键模式
            __this->onkey_sel = 0;  // 重置选择
         }
         if(if_in_rec == TRUE)
         {
            ui_highlight_element_by_id(ENC_PIC_REC);  // 高亮录像图标
         }else{
            ui_no_highlight_element_by_id(ENC_PIC_REC);  // 取消高亮录像图标
         }

        __this->key_disable = 1;  // 禁用按键以防止重复触发
        sys_touch_event_disable();  // 禁用触摸事件
        it.name = "video_rec";  // 设置意图名称
        it.action = ACTION_VIDEO_REC_CONTROL;  // 设置录像控制动作
        start_app_async(&it, rec_control_ok, NULL);  // 异步启动录像应用

        break;
    }

    return false;  // 返回 false 以指示未处理事件
}

REGISTER_UI_EVENT_HANDLER(ENC_BTN_REC)
.ontouch = rec_control_ontouch,  // 注册触摸事件处理程序
};

/***************************** 切换镜头按钮动作 ************************************/
static int rec_switch_onchange(void *ctr, enum element_change_event e, void *arg)
{
    static u16 id = 0;  // 定义静态变量 id 用于存储事件处理程序的标识符

    switch (e) {
    case ON_CHANGE_INIT:  // 初始化事件
        // 注册系统事件处理程序 avin_event_handler，并将标识符存储在 id 中
        id = register_sys_event_handler(SYS_DEVICE_EVENT, 0, 0, avin_event_handler);
        break;
    case ON_CHANGE_RELEASE:  // 释放事件
        // 注销系统事件处理程序，使用之前存储的标识符 id
        unregister_sys_event_handler(id);
        break;
    default:
        return false;  // 其他情况返回 false，表示未处理事件
    }
    return false;  // 返回 false，表示事件未被完全处理
}

// static int rec_switch_win_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**rec switch win ontouch**");  // 调试信息输出
//     struct intent it;  // 定义意图结构体

//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:  // 触摸按下事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");  // 调试信息输出
//         break;
//     case ELM_EVENT_TOUCH_HOLD:  // 触摸保持事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");  // 调试信息输出
//         break;
//     case ELM_EVENT_TOUCH_MOVE:  // 触摸移动事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");  // 调试信息输出
//         break;
//     case ELM_EVENT_TOUCH_UP:  // 触摸抬起事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");  // 调试信息输出
//         init_intent(&it);  // 初始化意图结构体
//         it.name = "video_rec";  // 设置意图的名称为 "video_rec"
//         it.action = ACTION_VIDEO_REC_SWITCH_WIN;  // 设置意图的动作为切换窗口
//         start_app(&it);  // 启动应用程序并执行指定的意图
//         break;
//     }
//     return false;  // 返回 false，表示事件未被完全处理
// }

// // 注册 UI 事件处理程序，将触摸和变化事件分别绑定到 rec_switch_win_ontouch 和 rec_switch_onchange 函数
// REGISTER_UI_EVENT_HANDLER(ENC_BTN_SWITCH)
// .ontouch = rec_switch_win_ontouch,
// .onchange = rec_switch_onchange,
// };


/***************************** 返回HOME按钮动作 ************************************/
static int rec_backhome_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**rec back to home ontouch**");
    struct intent it;
    struct application *app;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        __this->page_exit = HOME_SW_EXIT;
        if (if_in_rec) {
            //正在录像不退出rec app
#if REC_RUNNING_TO_HOME
            ui_hide(ui_get_current_window_id());
            set_page_main_flag(0);
#endif
            break;
        }

        __this->page_exit = HOME_SW_EXIT;
        init_intent(&it);
        app = get_current_app();
        if (app) {
            it.name = "video_rec";
            it.action = ACTION_BACK;
            start_app_async(&it, NULL, NULL); //不等待直接启动app
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_BTN_HOME)
.ontouch = rec_backhome_ontouch,
};
/***************************** 拍照动作 ************************************/
extern int shot_flag;
static void cap_take_photo_callback(void *p, int err)
{
    printf(">>>>>>%s err=%d\n",__func__,err);
}
static int rec_in_tph_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**rec_in_tph_ontouch**");
    struct intent it;
    struct application *app;
    init_intent(&it);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:

        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
         if (if_in_rec == TRUE) {
              //  printf("\nahd_view_show_status================== %d\n",ahd_view_show_status);
//            sys_key_event_disable();
//           video_rec_take_photo();
//           sys_touch_event_enable();
            it.name = "video_rec";
            it.action = ACTION_REC_CAP_TAKE_PHOTO;
            start_app_async(&it, NULL, NULL); //不等待直接启动app
             shot_flag=1;
             ui_show(ENC_PO_PT);
            break;
        }
        else {
            it.name = "video_rec";
            it.action = ACTION_REC_TAKE_PHOTO;
            start_app_async(&it, NULL, NULL); //不等待直接启动app
            ui_show(ENC_PO_PT);
        }
         shot_flag=1;
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if(shot_flag==0){
            if (if_in_rec == TRUE) {
              //  printf("\nahd_view_show_status================== %d\n",ahd_view_show_status);
//            sys_key_event_disable();
//           video_rec_take_photo();
//           sys_touch_event_enable();
            it.name = "video_rec";
            it.action = ACTION_REC_CAP_TAKE_PHOTO;
            start_app_async(&it, cap_take_photo_callback, NULL); //不等待直接启动app
            ui_show(ENC_PO_PT);
            break;
        }else {
            it.name = "video_rec";
            it.action = ACTION_REC_TAKE_PHOTO;
            start_app_async(&it, NULL, NULL); //不等待直接启动app
            ui_show(ENC_PO_PT);
        }
     }
        shot_flag=0;
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_BTN_VIDEO)
.ontouch = rec_in_tph_ontouch,
};

static int file_browse_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**file_browse  ontouch**");
    struct intent it;
    struct application *app;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if (if_in_rec == TRUE) {
           //ui_set_call(show_tips, 0);
            puts("It is in rec,can't switch mode.\n");
            break;
        }
        init_intent(&it);
        app = get_current_app();
        if (app) {
            __this->page_exit = MODE_SW_EXIT;
            it.name = "video_rec";
            it.action = ACTION_BACK;
            start_app_async(&it, NULL, NULL); //不等待直接启动app

            it.name = "video_dec";
            it.action = ACTION_VIDEO_DEC_MAIN;
            start_app_async(&it, NULL, NULL);
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(HOME_BTN_FILE)
.ontouch = file_browse_ontouch,
};


extern  get_ahd_yuv_init(void (*cb)(u8 *data));
extern  get_ahd_yuv_uninit(void);
extern void get_ahd_yuv_tp_init(void (*cb)(u8 *data));
extern void get_ahd_yuv_tp_uninit(void);
extern void user_scr_draw_close_fb(void *fb);

/******************************** 负片效果*****************************************/
static int is_setting = 0; // 静态变量，用于跟踪当前按钮的显示状态
static int rec_fp_ontouch(void *ctr, struct element_touch_event *e)
{

    struct intent it; // 定义意图结构
    struct application *app; // 定义应用程序结构
    UI_ONTOUCH_DEBUG("**rec set ontouch**"); // 输出调试信息，表示进入了触摸处理函数

    switch (e->event) { // 根据触摸事件类型进行处理
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n"); // 输出调试信息，表示触摸按下
        // screen_light_set();

        break;

    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n"); // 输出调试信息，表示触摸保持
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n"); // 输出调试信息，表示触摸移动
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 输出调试信息，表示触摸抬起

        if (is_setting) {//判断是否为1
            get_ahd_yuv_uninit();
            // user_scr_draw_close_fb(NULL);
            get_ahd_yuv_tp_uninit();
            is_setting = 0; // 更新状态为关闭
            ui_no_highlight_element_by_id(ENC_FP_NN);
        } else {
            get_ahd_yuv_init(NULL);
            get_ahd_yuv_tp_init(NULL);
            is_setting = 1; // 更新状态为打开
            ui_highlight_element_by_id(ENC_FP_NN); // 高亮二级菜单切换按钮

        }
        #ifdef VOICE_CONTROL_ENABLE
        rec_layout_speak_ctr_timer(0, 6 * 1000); // 如果启用了语音控制，设置语音控制计时器
        #endif
        break;
    }
    return 1; // 返回false表示未处理完事件，事件继续传递
}

REGISTER_UI_EVENT_HANDLER(ENC_FP_NN) // 注册UI事件处理器，处理ENC_BTN_SETTING按钮的触摸事件
.ontouch = rec_fp_ontouch, // 设置触摸处理函数为rec_set_ontouch
};
/*********************************摄像头亮度曝光***************************************************/


#define MAX_NEGATIVE_LEVEL 3  // 最大负片级别为3（共7档，从-3到3）
#define NEGATIVE_STEP 1       // 每次增加1档

static int current_negative_level = 0;  // 记录当前负片级别

static void update_negative_effect() {
    // 根据负片级别调用设置函数或处理函数
    switch (current_negative_level) {
    case -3:
        ui_pic_show_image_by_id(ENC_NEG_1, 0);
        break;
    case -2:
        ui_pic_show_image_by_id(ENC_NEG_1, 1);
        break;
    case -1:
        ui_pic_show_image_by_id(ENC_NEG_1, 2);
        break;
    case 0:
        ui_pic_show_image_by_id(ENC_NEG_1, 3);
        break;
    case 1:
        ui_pic_show_image_by_id(ENC_NEG_1, 4);
        break;
    case 2:
        ui_pic_show_image_by_id(ENC_NEG_1, 5);
        break;
    case 3:
        ui_pic_show_image_by_id(ENC_NEG_1, 6);
        break;
    default:
        break;
    }
    // 调用处理负片效果的函数
    // convert_yuv_to_grayscale(input_yuv_data, output_yuv_data, width, height);

}

static int screens_negative_adjustment_up(void *ctr, struct element_touch_event *e) {
    UI_ONTOUCH_DEBUG("**screen_negative_adjustment_up**");
    if (e->event == ELM_EVENT_TOUCH_DOWN) { // 处理按下事件
        if (current_negative_level < MAX_NEGATIVE_LEVEL) {
            current_negative_level += NEGATIVE_STEP; // 增加负片级别
            update_negative_effect(); // 更新负片效果
        }
    }
    return false; // 返回 false 表示事件未处理完，继续传递
}

static int screens_negative_adjustment_down(void *ctr, struct element_touch_event *e) {
    UI_ONTOUCH_DEBUG("**screen_negative_adjustment_down**");
    if (e->event == ELM_EVENT_TOUCH_DOWN) { // 处理按下事件
        if (current_negative_level > -MAX_NEGATIVE_LEVEL) {
            current_negative_level -= NEGATIVE_STEP; // 减少负片级别
            update_negative_effect(); // 更新负片效果
        }
    }
    return false; // 返回 false 表示事件未处理完，继续传递
}

// 注册触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(ENC_FP_6)
.ontouch = screens_negative_adjustment_up, // 增加负片效果
};
REGISTER_UI_EVENT_HANDLER(ENC_FP_7)
.ontouch = screens_negative_adjustment_down, // 减少负片效果
};








/***************************** 冻屏按钮动作 ************************************/
/**
 * @brief 处理冻屏按钮的触摸事件，按下按钮时执行冻屏操作，再次按下按钮时解除冻屏。
 *        图标在冻屏时保持为图1，解除冻屏后切换为图2。
 *
 * @param ctr UI控件指针
 * @param e   触摸事件结构体
 * @return 返回false表示事件处理完成
 */

static int freeze_screen_ontouch(void *ctr, struct element_touch_event *e)
{
    static bool is_frozen = false; // 记录当前是否处于冻屏状态
    struct ui_pic *pic = (struct ui_pic *)ctr; // 获取图标控件的指针
    // change_camera_config(4);
    UI_ONTOUCH_DEBUG("**freeze screen ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        if (is_frozen) {
            // 如果当前处于冻屏状态，则解除冻屏，并将图标切换为图2
            imc_ch5_com_con |= BIT(0);

            ui_pic_show_image_by_id(ENC_BTN_FREEZE_1, 0); // 设置图标为图2
        } else {
            // 如果当前没有冻屏，则执行冻屏操作，并将图标保持为图1
            imc_ch5_com_con &= ~BIT(0);
            ui_pic_show_image_by_id(ENC_BTN_FREEZE_1, 1);  // 设置图标为图1
        }

        is_frozen = !is_frozen; // 切换冻屏状态
        break;
    }
    return false;
}

/**
 * @brief 注册冻屏按钮的触摸事件处理函数
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_FREEZE)
.ontouch = freeze_screen_ontouch,
};

/***************************** 打开云台 ************************************/
static int toggle_gimbal_ontouch(void *ctr, struct element_touch_event *e)
{
    // static bool gimbal_visible = false; // 记录当前云台是否显示
    struct ui_pic *pic = (struct ui_pic *)ctr; // 获取图标控件的指针

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // if (gimbal_visible) {
        //     // 如果云台当前是显示状态，则隐藏云台，并将图标切换为关闭按钮图标
        //     ui_hide(GIMBAL_UI_ID); // 隐藏云台界面
        //     ui_pic_show_image_by_id(GIMBAL_BTN_TOGGLE, 0); // 设置图标为关闭按钮
        // } else {
        //     // 如果云台当前是隐藏状态，则显示云台，并将图标切换为开启按钮图标
            ui_show(ENC_BTN_SPIN); // 显示云台界面
            ui_highlight_element_by_id(GIMBAL_BTN_TOGGLE); // 设置图标为开启按钮
            ui_no_highlight_element_by_id(GIMBAL_BTN_TOGGLE1);
            db_update("yta", 1); // 打开云台
        // }

        // gimbal_visible = !gimbal_visible; // 切换云台显示状态
        // break;
    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief 注册云台开启/关闭按钮的触摸事件处理函数
 */
REGISTER_UI_EVENT_HANDLER(GIMBAL_BTN_TOGGLE)
.ontouch = toggle_gimbal_ontouch,
};

/***************************** 关闭云台按钮 ************************************/
static int toggle_gimbal1_ontouch(void *ctr, struct element_touch_event *e)
{
    // static bool gimbal_visible = false; // 记录当前云台是否显示
    struct ui_pic *pic = (struct ui_pic *)ctr; // 获取图标控件的指针

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // if (gimbal_visible) {
        //     // 如果云台当前是显示状态，则隐藏云台，并将图标切换为关闭按钮图标
        //     ui_hide(GIMBAL_UI_ID); // 隐藏云台界面
        //     ui_pic_show_image_by_id(GIMBAL_BTN_TOGGLE, 0); // 设置图标为关闭按钮
        // } else {
        //     // 如果云台当前是隐藏状态，则显示云台，并将图标切换为开启按钮图标
            ui_hide(ENC_BTN_SPIN); // 显示云台界面
            ui_highlight_element_by_id(GIMBAL_BTN_TOGGLE1); // 设置图标为开启按钮
            ui_no_highlight_element_by_id(GIMBAL_BTN_TOGGLE);
            db_update("yta", 0); // 打开云台
        // }

        // gimbal_visible = !gimbal_visible; // 切换云台显示状态
        // break;
    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief 注册云台开启/关闭按钮的触摸事件处理函数
 */
REGISTER_UI_EVENT_HANDLER(GIMBAL_BTN_TOGGLE1)
.ontouch = toggle_gimbal1_ontouch,
};


/***************************** 打开录音 ************************************/
static int toggle_mic_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
            // ui_show(ENC_BTN_SPIN);
            ui_highlight_element_by_id(ENC_BTN_MU);
            ui_no_highlight_element_by_id(ENC_BTN_MU_2);
            db_update("mic", 1);

    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_MU)
.ontouch =  toggle_mic_ontouch,
};
/***************************** 关闭录音 ************************************/
static int toggle_mic1_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
            // ui_show(ENC_BTN_SPIN);
            ui_highlight_element_by_id(ENC_BTN_MU_2);
            ui_no_highlight_element_by_id(ENC_BTN_MU);
            db_update("mic", 0);

    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_MU_2)
.ontouch = toggle_mic1_ontouch,
};


/***************************** 打开WIFI ************************************/
static int toggle_WIFI_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
            // ui_show(ENC_BTN_SPIN);
            ui_highlight_element_by_id(ENC_BTN_WIFI);
            ui_no_highlight_element_by_id(ENC_BTN_WIFI1);
            db_update("wfo", 1);
            db_flush(); // 立即将数据写入数据库
            wifi_on();
            ui_pic_show_image_by_id(PIC_REC_WIFI, 3);

    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_WIFI)
.ontouch =  toggle_WIFI_ontouch,
};
/***************************** 关闭WIFI ************************************/
static int toggle_WIFI1_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
            // ui_show(ENC_BTN_SPIN);
            ui_highlight_element_by_id(ENC_BTN_WIFI1);
            ui_no_highlight_element_by_id(ENC_BTN_WIFI);
            db_update("wfo", 0);
            db_flush(); // 立即将数据写入数据库
            wifi_off();
            ui_pic_show_image_by_id(PIC_REC_WIFI, 0);

    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_WIFI1)
.ontouch = toggle_WIFI1_ontouch,
};



/***************************** 打开水印 ************************************/
static int toggle_SY_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
            // ui_show(ENC_BTN_SPIN);
            ui_highlight_element_by_id(ENC_BTN_SY);
            ui_no_highlight_element_by_id(ENC_BTN_SY1);
            db_update("dat", 1);

    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_SY)
.ontouch =  toggle_SY_ontouch,
};
/***************************** 关闭水印 ************************************/
static int toggle_SY1_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;

    UI_ONTOUCH_DEBUG("**toggle gimbal ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
            // ui_show(ENC_BTN_SPIN);
            ui_highlight_element_by_id(ENC_BTN_SY1);
            ui_no_highlight_element_by_id(ENC_BTN_SY);
            db_update("dat", 0);

    }
    return 1; // 返回1表示事件处理完成
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_BTN_SY1)
.ontouch = toggle_SY1_ontouch,
};



/***************************** 信号通道选择按钮动作 ************************************/
extern void change_camera_config();

// 通道1触摸事件处理函数
static int channel_1_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**channel 1 ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 更新UI显示
        ui_highlight_element_by_id(ENC_BTN_CHANNEL_1); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_2); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_3); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_4); // 选中通道1的按钮
        // 更新摄像头配置
        change_camera_config(1);
        db_update("sxt", 1);
        db_flush(); // 立即将数据写入数据库
        break;
    }
    return 1;
}
// 注册通道1触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(ENC_BTN_CHANNEL_1)
.ontouch = channel_1_ontouch,
};

// 通道2触摸事件处理函数
static int channel_2_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**channel 2 ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 更新UI显示
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_1); // 选中通道1的按钮
        ui_highlight_element_by_id(ENC_BTN_CHANNEL_2); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_3); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_4); // 选中通道1的按钮
        change_camera_config(2);
        db_update("sxt", 2);
        db_flush(); // 立即将数据写入数据库
        break;
    }
    return 1;
}
// 注册通道2触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(ENC_BTN_CHANNEL_2)
.ontouch = channel_2_ontouch,
};
// 通道3触摸事件处理函数
static int channel_3_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**channel 3 ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 更新UI显示
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_1); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_2); // 选中通道1的按钮
        ui_highlight_element_by_id(ENC_BTN_CHANNEL_3); // 选中通道1的按钮
         ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_4); // 选中通道1的按钮

        change_camera_config(3);
        db_update("sxt", 3);
        db_flush(); // 立即将数据写入数据库
        break;
    }
    return 1;
}
// 注册通道3触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(ENC_BTN_CHANNEL_3)
.ontouch = channel_3_ontouch,
};
// 通道4触摸事件处理函数
static int channel_4_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**channel 4 ontouch**");

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 更新UI显示
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_1); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_2); // 选中通道1的按钮
        ui_no_highlight_element_by_id(ENC_BTN_CHANNEL_3); // 选中通道1的按钮
        ui_highlight_element_by_id(ENC_BTN_CHANNEL_4); // 选中通道1的按钮
        change_camera_config(4);
        db_update("sxt", 4);
        db_flush(); // 立即将数据写入数据库
        break;
    }
    return 1;
}
// 注册通道3触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(ENC_BTN_CHANNEL_4)
.ontouch = channel_4_ontouch,
};


/*********************************数字变焦***************************************************/
// static void screens_light_set(int sel_item) {
//     struct intent it; // 用于设置意图和启动应用

//     init_intent(&it); // 初始化意图结构体
//     it.name = "video_rec"; // 设置意图名称为录像应用
//     it.action = ACTION_VIDEO_REC_SET_CONFIG; // 设置意图动作为设置配置
//     it.data = "bkl"; // 设置意图数据，表示请求设置背光
//     it.exdata = table_light_lcd[sel_item]; // 设置额外数据，表示选择的背光值

//     start_app(&it); // 启动应用并传递意图

//     ui_pic_show_image_by_id(ENC_BL_1, sel_item); // 根据选择的背光值显示相应的图像
// }

// 在头文件中声明
extern void video_set_crop(int x, int y, int width, int height);

#define MAX_ZOOM0_LEVEL 6  // 最大变焦级别为6（共7档，从0到6）
#define ZOOM_STEP 1       // 每次增加1档
static int current_zoom0_level = 0; // 记录当前变焦级别
static int zoom0_factor = 0;

// 变焦调节触摸事件处理函数
static int camera_zoom_adjustment_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**camera_zoom_adjustment_ontouch**"); // 调试信息，显示触摸事件

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");

        // 计算新的变焦级别
        current_zoom0_level++;
        if (current_zoom0_level > MAX_ZOOM0_LEVEL) {
            current_zoom0_level = 0; // 循环重置到初始状态
        }

        zoom0_factor = current_zoom0_level * ZOOM_STEP; // 根据点击次数计算变焦因子

        // 调用变焦处理逻辑
        switch (zoom0_factor) {
            case 0:
                video_set_crop(0, 0, 1280, 720);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 0);
                break;
            case 1:
                video_set_crop(32, 18, 1216, 684);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 1);
                break;
            case 2:
                video_set_crop(64, 36, 1152, 648);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 2);
                break;
            case 3:
                video_set_crop(96, 54, 1088, 612);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 3);
                break;
            case 4:
                video_set_crop(128, 72, 1024, 576);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 4);
                break;
            case 5:
                video_set_crop(160, 90, 960, 540);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 5);
                break;
            case 6:
                video_set_crop(192, 108, 896, 504);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 6);
                break;
            default:
                break;
        }
        break;

    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸松开事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        break;
    }

    return false; // 返回 false 表示事件未处理完，继续传递
}

REGISTER_UI_EVENT_HANDLER(PIC_REC_FOUCS_1)
.ontouch = camera_zoom_adjustment_ontouch, // 注册触摸事件处理函数
};

/****************************************镜像******************************************/
#define MAX_ZOOM_LEVEL 1  // 最大变焦级别为6（共7档，从0到6）
#define ZOOM_STEP 1       // 每次增加1档
static int current_zoom_level = 0; // 记录当前变焦级别
extern video_display_mirror();
static int zoom_xz_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**zoom_xz_ontouch**"); // 调试信息，显示触摸事件


    switch (e->event) { // 根据触摸事件类型进行处理
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");


    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸松开事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        // 计算新的变焦级别
        current_zoom_level++;
        if (current_zoom_level > MAX_ZOOM_LEVEL) {
            current_zoom_level = 0; // 循环重置到初始状态
        }

        int zoom_factor = current_zoom_level * ZOOM_STEP; // 根据点击次数计算变焦因子

        switch (zoom_factor) {
            case 0:
                video_display_mirror();
                ui_pic_show_image_by_id(PIC_REC_XZ, 0);
                break;
            case 1:
                video_display_mirror();
                ui_pic_show_image_by_id(PIC_REC_XZ, 5);
                break;
            default:
                break;
        }
        break;
    }
    return 0;
}

REGISTER_UI_EVENT_HANDLER(ENC_XZ_WIN)
.ontouch = zoom_xz_ontouch, // 注册触摸事件处理函数
};

/****************************************标尺切换和隐藏******************************************/

static int toggle_pic_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**toggle_pic_ontouch**"); // 调试信息，显示触摸事件
    static int current_state = 0; // 记录当前状态，0：显示第一张，1：显示第二张，2：隐藏图片

    switch (e->event) { // 根据触摸事件类型进行处理
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;

    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸松开事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 根据 current_state 来切换图片或隐藏图片
        switch (current_state) {
            case 0:
                // 显示第一张图片
                ui_pic_show_image_by_id(ENC_PT_RULER, 0);
                // ui_pic_hide_image_by_id(ENC_PT_RULER,1); // 隐藏第二张图
                current_state = 1; // 切换到显示第二张图的状态
                break;
            case 1:
                // 显示第二张图片
                ui_pic_show_image_by_id(ENC_PT_RULER, 1);
                // ui_pic_hide_image_by_id(ENC_PT_RULER,0); // 隐藏第一张图
                current_state = 2; // 切换到隐藏图片的状态
                break;
            case 2:
                // 隐藏所有图片
                // ui_pic_hide_image_by_id(ENC_PT_RULER,0); // 隐藏第一张图
                // ui_pic_hide_image_by_id(ENC_PT_RULER,1); // 隐藏第二张图
                ui_hide(ENC_PT_RULER); // 隐藏图片
                current_state = 0; // 切换回显示第一张图的状态
                break;
            default:
                break;
        }
        break;
    }
    return 0;
}

REGISTER_UI_EVENT_HANDLER(ENC_BTN_RULER)
.ontouch = toggle_pic_ontouch, // 注册触摸事件处理函数
};


/*********************************照明灯亮度调节***************************************************/
static bool led1_on = false; // LED1状态，false表示熄灭，true表示亮起
static bool led2_on = false; // LED2状态
extern sys_pwm_ctrl(u8 ch, u8 duty_val);
#define MAX_BRIGHTNESS_LEVEL 6  // 最大亮度级别为6（共7档，从0到6）
#define BRIGHTNESS_STEP 1       // 每次增加1档
#define MAX_CLICKS (MAX_BRIGHTNESS_LEVEL + 1) // 最大点击次数为7次（7档亮度）
static bool is_led1_selected = true;   // true表示控制LED1，false表示控制LED2
static int current_clicks_led1 = 0;    // LED1的当前点击次数
static int brightness_led1 = 0;        // LED1的当前亮度
static int current_clicks_led2 = 0;    // LED2的当前点击次数
static int brightness_led2 = 0;        // LED2的当前亮度
// 函数控制LED灯的状态
void control_led1(bool state) {
    if (state) {
        sys_pwm_ctrl(6, 50); // LED1全亮，假设100为亮度值
    } else {
        sys_pwm_ctrl(6, 0);   // LED1熄灭
    }
}

void control_led2(bool state) {
    if (state) {
        sys_pwm_ctrl(7, 50); // LED2全亮，假设100为亮度值
    } else {
        sys_pwm_ctrl(7, 0);   // LED2熄灭
    }
}

// extern pwm_duty_cycle();
static int screens_light_adjustment_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_light_adjustment_ontouch**"); // 调试信息，显示触摸事件


     switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN: // 触摸按下事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");

        if (is_led1_selected) {//turn on led1
            // LED1亮度调节逻辑
            current_clicks_led1++;
            if (current_clicks_led1 > MAX_BRIGHTNESS_LEVEL) {
                current_clicks_led1 = 0; // 循环重置到初始状态
            }
            brightness_led1 = current_clicks_led1 * BRIGHTNESS_STEP; // 根据点击次数计算亮度
            switch (brightness_led1) {
            case 0:
                ui_pic_show_image_by_id(ENC_BL_1, 0);
                sys_pwm_ctrl(7, 0);  // LED1亮度0
                break;
            case 1:
                ui_pic_show_image_by_id(ENC_BL_1, 1);
                sys_pwm_ctrl(7, 50); // LED1亮度1
                break;
            case 2:
                ui_pic_show_image_by_id(ENC_BL_1, 2);
                sys_pwm_ctrl(7, 60); // LED1亮度2
                break;
            case 3:
                ui_pic_show_image_by_id(ENC_BL_1, 3);
                sys_pwm_ctrl(7, 70); // LED1亮度3
                break;
            case 4:
                ui_pic_show_image_by_id(ENC_BL_1, 4);
                sys_pwm_ctrl(7, 75); // LED1亮度4
                break;
            case 5:
                ui_pic_show_image_by_id(ENC_BL_1, 5);
                sys_pwm_ctrl(7, 80); // LED1亮度5
                break;
            case 6:
                ui_pic_show_image_by_id(ENC_BL_1, 6);
                sys_pwm_ctrl(7, 85); // LED1亮度6
                break;
            default:
                break;
            }
        } else {
            // LED2亮度调节逻辑
            current_clicks_led2++;
            if (current_clicks_led2 > MAX_BRIGHTNESS_LEVEL) {
                current_clicks_led2 = 0; // 循环重置到初始状态
            }
            brightness_led2 = current_clicks_led2 * BRIGHTNESS_STEP; // 根据点击次数计算亮度
            switch (brightness_led2) {
            case 0:
                ui_pic_show_image_by_id(ENC_BL_1, 0);
                sys_pwm_ctrl(6, 0);  // LED2亮度0
                break;
            case 1:
                ui_pic_show_image_by_id(ENC_BL_1, 1);
                sys_pwm_ctrl(6, 50); // LED2亮度1
                break;
            case 2:
                ui_pic_show_image_by_id(ENC_BL_1, 2);
                sys_pwm_ctrl(6, 60); // LED2亮度2
                break;
            case 3:
                ui_pic_show_image_by_id(ENC_BL_1, 3);
                sys_pwm_ctrl(6, 70); // LED2亮度3
                break;
            case 4:
                ui_pic_show_image_by_id(ENC_BL_1, 4);
                sys_pwm_ctrl(6, 75); // LED2亮度4
                break;
            case 5:
                ui_pic_show_image_by_id(ENC_BL_1, 5);
                sys_pwm_ctrl(6, 80); // LED2亮度5
                break;
            case 6:
                ui_pic_show_image_by_id(ENC_BL_1, 6);
                sys_pwm_ctrl(6, 85); // LED2亮度6
                break;
            default:
                break;
            }
        }
        break;



    case ELM_EVENT_TOUCH_HOLD: // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;

    case ELM_EVENT_TOUCH_MOVE: // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;

    case ELM_EVENT_TOUCH_UP: // 触摸松开事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        break;
    }
    return false; // 返回 false 表示事件未处理完，继续传递
}

REGISTER_UI_EVENT_HANDLER(ENC_LIGHT)
.ontouch = screens_light_adjustment_ontouch, // 注册触摸事件处理函数
};


/*********************************************************************************
 *  		     				菜单动作
 *********************************************************************************/
// static int menu_res_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**res menu ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
// #ifndef CONFIG_VIDEO4_ENABLE
//         __this->resolution = index_of_table8(db_select("res"), TABLE(table_video_resolution));
//         if (__this->enc_menu_status == ENC_MENU_RESOLUTION) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 1;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_RESOLUTION);
// #endif
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_1_1)
// .ontouch = menu_res_ontouch,
// };
// static int menu_cyc_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**cyc video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         __this->cycle_rec = index_of_table8(db_select("cyc"), TABLE(table_video_cycle));
//         if (__this->enc_menu_status == ENC_MENU_CYCLE) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 2;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_CYCLE);
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_1_3)
// .ontouch = menu_cyc_rec_ontouch,
// };
// static int menu_double_route_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**double video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
// #ifndef CONFIG_VIDEO4_ENABLE
//         __this->double_route = db_select("two");
//         if (__this->enc_menu_status == ENC_MENU_DUALVIDEO) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 3;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_DUALVIDEO);
// #endif
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_1_2)
// .ontouch = menu_double_route_ontouch,
// };
// static int menu_gap_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**gap video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         /* #ifndef CONFIG_VIDEO4_ENABLE */
//         __this->gap_rec = index_of_table16(db_select("gap"), TABLE(table_video_gap));
//         if (__this->enc_menu_status == ENC_MENU_GAP) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 4;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_GAP);
//         /* #endif */
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_1_4)
// .ontouch = menu_gap_rec_ontouch,
// };
// static int menu_hdr_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**hdr video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
// #ifndef CONFIG_VIDEO4_ENABLE
//         __this->wdr = db_select("wdr");
//         if (__this->enc_menu_status == ENC_MENU_HDR) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 5;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_HDR);
// #endif
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_2_1)
// .ontouch = menu_hdr_rec_ontouch,
// };
// static int menu_exposure_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**exp video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
// #ifndef CONFIG_VIDEO4_ENABLE
//         __this->exposure = index_of_table8(db_select("exp"), TABLE(table_video_exposure));
//         if (__this->enc_menu_status == ENC_MENU_EXPOSURE) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 6;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_EXPOSURE);
// #endif
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_2_2)
// .ontouch = menu_exposure_rec_ontouch,
// };
// static int menu_movdet_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**movdet video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
// #ifndef CONFIG_VIDEO4_ENABLE
//         __this->motdet = db_select("mot");
//         if (__this->enc_menu_status == ENC_MENU_MOTION) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 7;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_MOTION);
// #endif
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_2_3)
// .ontouch = menu_movdet_rec_ontouch,
// };
// static int menu_date_label_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**date lable video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         __this->dat_label = db_select("dat");
//         if (__this->enc_menu_status == ENC_MENU_LABEL) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 8;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_LABEL);
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_2_4)
// .ontouch = menu_date_label_rec_ontouch,
// };
// static int menu_gravity_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**gravity video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         __this->gravity = index_of_table8(db_select("gra"), TABLE(table_video_gravity));
//         if (__this->enc_menu_status == ENC_MENU_GSEN) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 9;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_GSEN);
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_3_1)
// .ontouch = menu_gravity_rec_ontouch,
// };
/*
 * 菜单麦克风录音的触摸事件处理函数
 */
// static int menu_mic_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     // 输出调试信息，表示进入麦克风视频触摸事件处理函数
//     UI_ONTOUCH_DEBUG("**mic video  ontouch**");

//     struct intent it;             // 定义一个意图 (intent) 结构体，用于处理事件
//     struct application *app;      // 定义一个应用 (application) 结构体指针，可能会在后续处理中用到

//     // 根据不同的触摸事件类型进行处理
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:    // 触摸按下事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");  // 输出调试信息，表示触摸按下事件
//         return true;              // 返回 true，表示触摸按下事件被处理
//         break;                    // 注意：此处的 break 实际上没有必要，因为 return 后代码不会继续执行

//     case ELM_EVENT_TOUCH_HOLD:    // 触摸保持事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");  // 输出调试信息，表示触摸保持事件
//         break;                    // 处理结束，跳出 switch

//     case ELM_EVENT_TOUCH_MOVE:    // 触摸移动事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");  // 输出调试信息，表示触摸移动事件
//         break;                    // 处理结束，跳出 switch

//     case ELM_EVENT_TOUCH_UP:      // 触摸抬起事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");    // 输出调试信息，表示触摸抬起事件

//         // 从数据库中选择麦克风的相关设置，并存储到 __this->mic
//         __this->mic = db_select("mic");

//         // 判断当前菜单状态是否为录音设置菜单 (ENC_MENU_SOUND)
//         if (__this->enc_menu_status == ENC_MENU_SOUND) {
//             __this->onkey_mod = 2;      // 设置 onkey_mod 为 2，可能表示录音模式的某个状态
//             __this->onkey_sel = 0;      // 重置 onkey_sel 选择项为 0
//         } else {                        // 如果当前菜单状态不是录音设置菜单
//             if (__this->onkey_sel) {    // 如果存在已选择的 onkey 选项
//                 // 取消之前高亮的 onkey 选择项
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 10; // 设置 onkey_mod 为 12，表示不同的模式
//             __this->onkey_sel = 0;      // 重置 onkey_sel 选择项为 0
//         }

//         // 切换到录音菜单 (ENC_MENU_SOUND)
//         ui_set_call(enc_menu, ENC_MENU_SOUND);
//         break;                    // 处理结束，跳出 switch
//     }
//     return false;                 // 返回 false，表示未处理其他触摸事件
// }

// /*
//  * 注册 UI 事件处理程序，将此函数绑定到 ENC_SET_PIC_3_2 控件的触摸事件处理
//  */
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_3_2)
// .ontouch = menu_mic_rec_ontouch,  // 关联控件的 ontouch 事件到 menu_mic_rec_ontouch 函数
// };

// static int menu_guard_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**guard video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         __this->park_guard = db_select("par");
//         if (__this->enc_menu_status == ENC_MENU_GUARD) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 11;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_GUARD);
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_3_3)
// .ontouch = menu_guard_rec_ontouch,
// };
// static int menu_carnum_rec_ontouch(void *ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**carnum video  ontouch**");
//     struct intent it;
//     struct application *app;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
// #ifndef CONFIG_VIDEO4_ENABLE
//         __this->car_num = db_select("num");
//         if (__this->enc_menu_status == ENC_MENU_CID) {
//             __this->onkey_mod = 2;
//             __this->onkey_sel = 0;
//         } else {
//             if (__this->onkey_sel) {
//                 ui_no_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]);
//                 ui_no_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]);
//             }
//             __this->onkey_mod = 2 + 12;
//             __this->onkey_sel = 0;
//         }
//         ui_set_call(enc_menu, ENC_MENU_CID);
// #endif
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_3_4)
// .ontouch = menu_carnum_rec_ontouch,
// };
/*********************************************************************************
 *  		     				子菜单动作
 *********************************************************************************/
// 处理 ENC_SET_PIC_C2 触摸事件的回调函数
// static int menu_enc_c2_ontouch(void *_ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**c2 ontouch**");  // 输出调试信息，显示 c2 触摸事件
//     struct intent it;  // 声明一个意图结构
//     int sel_item = 0;  // 用于存储当前选中的项目
//     struct ui_pic *ctr = (struct ui_pic *)_ctr;  // 将传入的控件转换为图片控件结构体

//     switch (e->event) {  // 根据触摸事件的类型进行分支处理
//     case ELM_EVENT_TOUCH_DOWN:  // 触摸按下事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");  // 输出调试信息，显示按下事件
//         return true;  // 返回 true，表示事件已处理
//         break;

//     case ELM_EVENT_TOUCH_HOLD:  // 长按事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");  // 输出调试信息，显示长按事件
//         break;

//     case ELM_EVENT_TOUCH_MOVE:  // 触摸移动事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");  // 输出调试信息，显示触摸移动事件
//         break;

//     case ELM_EVENT_TOUCH_UP:  // 触摸松开事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");  // 输出调试信息，显示松开事件

//         // 查找当前控件是否在 _ENC_SET_PIC_C2 数组中
//         for (int i = 0; i < 2; i++) {
//             if (ctr->elm.id == _ENC_SET_PIC_C2[i]) {  // 如果控件 ID 匹配
//                 sel_item = i;  // 设置当前选中项
//                 break;
//             }
//         }

// #if ENC_MENU_HIDE_ENABLE  // 如果启用了 ENC_MENU_HIDE_ENABLE 宏
//         ui_set_call(enc_menu, __this->enc_menu_status);  // 更新菜单状态
// #endif

//         // 根据当前菜单状态执行不同操作
//         switch (__this->enc_menu_status) {
//         case ENC_MENU_DUALVIDEO:  // 双视频模式
//             if (__this->double_route == sel_item) {  // 如果选择的项目和当前项目一致
//                 return false;  // 不做任何处理，直接返回
//             }
//             menu_rec_set_double(sel_item);  // 设置双视频模式
//             break;

//         case ENC_MENU_SOUND:  // 声音模式
//             if (__this->mic == sel_item) {  // 如果选择的项目和当前麦克风状态一致
//                 return false;  // 不做任何处理，直接返回
//             }
//             menu_rec_set_mic(sel_item);  // 设置麦克风状态
//             break;

//         case ENC_MENU_GUARD:  // 停车守卫模式
//             if (__this->park_guard == sel_item) {  // 如果选择的项目和当前守卫状态一致
//                 return false;  // 不做任何处理，直接返回
//             }
//             menu_rec_set_par(sel_item);  // 设置停车守卫状态
//             break;

//         case ENC_MENU_CID:  // 车牌号模式
//             if (__this->car_num == sel_item) {  // 如果选择的车牌号和当前车牌号一致
//                 return false;  // 不做任何处理，直接返回
//             }
//             menu_rec_set_num(sel_item);  // 设置车牌号
//             break;

//         default:  // 其他未处理的菜单状态
//             return false;  // 不做任何处理，直接返回
//             break;
//         }

//         // 更新 UI 界面，取消高亮另一个未选中的项目，并高亮选中的项目
//         ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[!sel_item]);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C2[sel_item]);
//         break;
//     }

//     return false;  // 返回 false，表示不处理其他事件
// }

// // 注册 UI 事件处理程序，绑定触摸事件到 ENC_SET_PIC_C2_1 控件
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C2_1)
// .ontouch = menu_enc_c2_ontouch,  // 绑定触摸事件处理函数
// };

// // 注册 UI 事件处理程序，绑定触摸事件到 ENC_SET_PIC_C2_2 控件
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C2_2)
// .ontouch = menu_enc_c2_ontouch,  // 绑定触摸事件处理函数
// };


// static int menu_enc_c3_ontouch(void *_ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**c3 ontouch**");
//     struct intent it;
//     int sel_item = 0;
//     struct ui_pic *ctr = (struct ui_pic *)_ctr;

//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         for (int i = 0; i < 3; i++) {
//             if (ctr->elm.id == _ENC_SET_PIC_C3[i]) {
//                 sel_item = i;
//                 break;
//             }
//         }
// #if ENC_MENU_HIDE_ENABLE
//         ui_set_call(enc_menu, __this->enc_menu_status);
// #endif
//         switch (__this->enc_menu_status) {
//         case ENC_MENU_RESOLUTION:
//             if (__this->resolution == sel_item) {
//                 break;
//             }
//             ui_no_highlight_element_by_id(_ENC_SET_PIC_C3[__this->resolution]);
//             ui_highlight_element_by_id(_ENC_SET_PIC_C3[sel_item]);
//             menu_rec_set_res(sel_item);
//             break;
//         default:
//             break;
//         }
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C3_1)
// .ontouch = menu_enc_c3_ontouch,
// };
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C3_2)
// .ontouch = menu_enc_c3_ontouch,
// };
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C3_3)
// .ontouch = menu_enc_c3_ontouch,
// };

// static int menu_enc_c4_ontouch(void *_ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**c4 ontouch**");
//     const char *data;
//     int sel_item = 0;
//     struct ui_pic *ctr = (struct ui_pic *)_ctr;

//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         for (int i = 0; i < 4; i++) {
//             if (ctr->elm.id == _ENC_SET_PIC_C4[i]) {
//                 sel_item = i;
//                 break;
//             }
//         }
// #if ENC_MENU_HIDE_ENABLE
//         ui_set_call(enc_menu, __this->enc_menu_status);
// #endif
//         switch (__this->enc_menu_status) {
//         case ENC_MENU_GAP:
//             if (__this->gap_rec == sel_item) {
//                 return false;
//             } else {
//                 ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[__this->gap_rec]);
//             }
//             menu_rec_set_gap(sel_item);
//             break;
//         case ENC_MENU_CYCLE:
//             if (__this->cycle_rec == sel_item) {
//                 return false;
//             } else {
//                 ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[__this->cycle_rec]);
//             }
//             menu_rec_set_cycle(sel_item);
//             break;
//         case ENC_MENU_GSEN:
//             if (__this->gravity == sel_item) {
//                 return false;
//             } else {
//                 ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[__this->gravity]);
//             }
//             menu_rec_set_gravity(sel_item);
//             break;
//         default:
//             return false;
//             break;
//         }
//         ui_highlight_element_by_id(_ENC_SET_PIC_C4[sel_item]);
//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C4_1)
// .ontouch = menu_enc_c4_ontouch,
// };
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C4_2)
// .ontouch = menu_enc_c4_ontouch,
// };
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C4_3)
// .ontouch = menu_enc_c4_ontouch,
// };
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C4_4)
// .ontouch = menu_enc_c4_ontouch,
// };

// static int menu_enc_c20_ontouch(void *_ctr, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("**c20 ontouch**");
//     const char *data;
//     int sel_item = 0;
//     struct ui_pic *ctr = (struct ui_pic *)_ctr;

//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         for (int i = 0; i < 2; i++) {
//             if (ctr->elm.id == _ENC_SET_PIC_C20[i]) {
//                 sel_item = i;
//                 break;
//             }
//         }
// #if ENC_MENU_HIDE_ENABLE
//         ui_set_call(enc_menu, __this->enc_menu_status);
// #endif
//         switch (__this->enc_menu_status) {
//         case ENC_MENU_HDR:
//             if (__this->wdr == sel_item) {
//                 return false;
//             }
//             menu_rec_set_wdr(sel_item);
//             break;
//         case ENC_MENU_MOTION:
//             if (__this->motdet == sel_item) {
//                 return false;
//             }
//             menu_rec_set_mot(sel_item);
//             break;
//         case ENC_MENU_LABEL:
//             if (__this->dat_label == sel_item) {
//                 return false;
//             }
//             menu_rec_set_dat(sel_item);
//             break;
//         default:
//             return false;
//             break;
//         }
//         ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[!sel_item]);
//         ui_highlight_element_by_id(_ENC_SET_PIC_C20[sel_item]);

//         break;
//     }
//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C20_1)
// .ontouch = menu_enc_c20_ontouch,
// };
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_C20_2)
// .ontouch = menu_enc_c20_ontouch,
// };
/*************************************************************************************/
/**************************************曝光补偿控件动作***************************************/
#define SLID_X    820 //滑块x起始绝对坐标
#define SLID_GAP  42  //每一项的间隔(滑块长度/项目数)
#define SLID_ITEM 7  //项目数
// static int menu_exp_ontouch(void *arg1, struct element_touch_event *e)
// {
//     UI_ONTOUCH_DEBUG("** exp video ontouch  ");
//     static s16 x_pos_down = 0;
//     static s16 old_exp = 0;
//     s16 tmp;
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         x_pos_down = e->pos.x;
//         old_exp = __this->exposure;
//         return true;
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         return true;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         s16 x_pos_now = e->pos.x;
//         s16 x_pos_ch = x_pos_down - x_pos_now;
//         tmp = __this->exposure;
//         if (x_pos_ch < SLID_GAP && x_pos_ch > -SLID_GAP) {
//             return false;
//         }
//         tmp = old_exp + x_pos_ch / SLID_GAP;
//         if (tmp > SLID_ITEM - 1) {
//             tmp = SLID_ITEM - 1;
//             x_pos_down = x_pos_now;
//             old_exp = SLID_ITEM - 1;
//         } else if (tmp < 0) {
//             tmp = 0;
//             x_pos_down = x_pos_now;
//             old_exp = 0;
//         }
//         printf("\n tmp_exp = %d \n", tmp);
//         if (__this->exposure == tmp) {
//             return false;
//         }
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         /* printf("x_pos=%d y_pos=%d",e->pos.x,e->pos.y); */
// #if ENC_MENU_HIDE_ENABLE
//         ui_set_call(enc_menu, __this->enc_menu_status);
// #endif
//         int i;
//         tmp = __this->exposure;
//         for (i = 1; i <= SLID_ITEM; i++) {
//             if (e->pos.x - SLID_X < SLID_GAP * i && e->pos.x > SLID_X + SLID_GAP * (i - 1)) {
//                 tmp = SLID_ITEM - i;
//             }
//         }
//         if (__this->exposure == tmp) {
//             return false;
//         }
//         break;
//     }
//     ui_hide(_ENC_SET_PIC_EXP[6 - __this->exposure]);
//     ui_show(_ENC_SET_PIC_EXP[6 - tmp]);
//     menu_rec_set_exposure(tmp);

//     return false;
// }
// REGISTER_UI_EVENT_HANDLER(ENC_SET_PIC_EXP)
// .ontouch = menu_exp_ontouch,
// };








static int enc_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    // 当控件第一次显示时触发
    case ON_CHANGE_FIRST_SHOW:
        // 打印调试信息
        printf("enc_onchange\n");

        // 接管系统按键事件，将按键事件转发至该控件
        sys_key_event_takeover(true, false);

        // 初始化按键模式，设置为0，表示没有启用按键模式
        __this->onkey_mod = 0;

        // 初始化当前选择项为0
        __this->onkey_sel = 0;

        // 突出显示控件 ID 为 ENC_PIC_REC 的元素
        // ui_highlight_element_by_id(ENC_PIC_REC);
        break;

    default:
        return false; // 对于未处理的事件，返回false
    }

    return false; // 如果事件已处理完毕，返回false
}
extern switch_camera_disp();
extern void key_voice_start(int id);
static int enc_onkey(void *ctr, struct element_key_event *e)//PB3.2按键版事件
{
    struct intent it;
    struct application *app;
    printf("e->value=%d\n",e->value);
    if (e->event == KEY_EVENT_LONG && e->value == KEY_POWER) { // 长按电源键事件处理
        ui_hide(ui_get_current_window_id()); // 隐藏当前窗口
        sys_key_event_takeover(false, true); // 系统键接管
        return true;
    }
    if (e->event == KEY_EVENT_LONG && e->value == KEY_6) { // 长按LED键事件处理
        // 切换LED状态
        if (is_led1_selected) {
            sys_pwm_ctrl(7, 0);
            sys_pwm_ctrl(6, 50);
            ui_pic_show_image_by_id(ENC_BL_1, 0);
            is_led1_selected = false;
        } else {
            sys_pwm_ctrl(7, 50);
            sys_pwm_ctrl(6, 0);
            ui_pic_show_image_by_id(ENC_BL_1, 0);
            is_led1_selected = true;
        }
        return true;
    }


    if (e->event != KEY_EVENT_CLICK || __this->key_disable) { // 点击事件且键未禁用时处理
        return true;
    }
    if (__this->onkey_mod == 0) { // onkey模式为0时处理
        switch (e->value) {
        case KEY_DOWN: // 按下键处理
            // if (if_in_rec == TRUE) { // 如果在录像中
            //     init_intent(&it);
            //     __this->lock_file_flag = !__this->lock_file_flag; // 切换锁文件标志
            //     it.name = "video_rec";
            //     it.action = ACTION_VIDEO_REC_LOCK_FILE; // 锁文件动作
            //     it.data = "set_lock_statu";
            //     it.exdata = __this->lock_file_flag;
            //     start_app(&it); // 启动应用
            //     if (__this->lock_file_flag == 1) { // 如果文件锁定
            //         puts("show lock\n");
            //         ui_show(ENC_PIC_LOCK); // 显示锁图标
            //     } else {
            //         puts("hide lock\n");
            //         ui_hide(ENC_PIC_LOCK); // 隐藏锁图标
            //     }
            // }
            break;

        case KEY_UP: // 按上键处理
            init_intent(&it);
            it.name = "video_rec";
            it.action = ACTION_VIDEO_REC_CONTROL; // 用于验证切换功能的动作
            start_app(&it); // 启动应用
            break;
        case KEY_OK: // OK键处理
            __this->key_disable = 1; // 禁用按键
            sys_touch_event_disable(); // 禁用触摸事件
            it.name = "video_rec";
            it.action = ACTION_VIDEO_REC_CONTROL; // 录像控制动作
            start_app_async(&it, rec_control_ok, NULL); // 异步启动应用
            break;
        case KEY_1: // 拍照键处理
            // __this->key_disable = 1; // 禁用按键
            // sys_touch_event_disable(); // 禁用触摸事件

            it.name = "video_rec";
            it.action = ACTION_REC_TAKE_PHOTO;
            start_app_async(&it, NULL, NULL); //不等待直接启动app
            ui_show(ENC_PO_PT);
            // shot_flag=1;
            // shot_flag=0;
            break;
        case KEY_3: // 右键处理
            key_voice_start(1);
        break;
        case KEY_6: // LED键处理
        if (is_led1_selected) {//turn on led1
            // LED1亮度调节逻辑
            current_clicks_led1++;
            if (current_clicks_led1 > MAX_BRIGHTNESS_LEVEL) {
                current_clicks_led1 = 0; // 循环重置到初始状态
            }
            brightness_led1 = current_clicks_led1 * BRIGHTNESS_STEP; // 根据点击次数计算亮度
            switch (brightness_led1) {
            case 0:
                ui_pic_show_image_by_id(ENC_BL_1, 0);
                sys_pwm_ctrl(7, 0);  // LED1亮度0
                break;
            case 1:
                ui_pic_show_image_by_id(ENC_BL_1, 1);
                sys_pwm_ctrl(7, 50); // LED1亮度1
                break;
            case 2:
                ui_pic_show_image_by_id(ENC_BL_1, 2);
                sys_pwm_ctrl(7, 60); // LED1亮度2
                break;
            case 3:
                ui_pic_show_image_by_id(ENC_BL_1, 3);
                sys_pwm_ctrl(7, 70); // LED1亮度3
                break;
            case 4:
                ui_pic_show_image_by_id(ENC_BL_1, 4);
                sys_pwm_ctrl(7, 75); // LED1亮度4
                break;
            case 5:
                ui_pic_show_image_by_id(ENC_BL_1, 5);
                sys_pwm_ctrl(7, 80); // LED1亮度5
                break;
            case 6:
                ui_pic_show_image_by_id(ENC_BL_1, 6);
                sys_pwm_ctrl(7, 85); // LED1亮度6
                break;
            default:
                break;
            }
        } else {
            // LED2亮度调节逻辑
            current_clicks_led2++;
            if (current_clicks_led2 > MAX_BRIGHTNESS_LEVEL) {
                current_clicks_led2 = 0; // 循环重置到初始状态
            }
            brightness_led2 = current_clicks_led2 * BRIGHTNESS_STEP; // 根据点击次数计算亮度
            switch (brightness_led2) {
            case 0:
                ui_pic_show_image_by_id(ENC_BL_1, 0);
                sys_pwm_ctrl(6, 0);  // LED2亮度0
                break;
            case 1:
                ui_pic_show_image_by_id(ENC_BL_1, 1);
                sys_pwm_ctrl(6, 50); // LED2亮度1
                break;
            case 2:
                ui_pic_show_image_by_id(ENC_BL_1, 2);
                sys_pwm_ctrl(6, 60); // LED2亮度2
                break;
            case 3:
                ui_pic_show_image_by_id(ENC_BL_1, 3);
                sys_pwm_ctrl(6, 70); // LED2亮度3
                break;
            case 4:
                ui_pic_show_image_by_id(ENC_BL_1, 4);
                sys_pwm_ctrl(6, 75); // LED2亮度4
                break;
            case 5:
                ui_pic_show_image_by_id(ENC_BL_1, 5);
                sys_pwm_ctrl(6, 80); // LED2亮度5
                break;
            case 6:
                ui_pic_show_image_by_id(ENC_BL_1, 6);
                sys_pwm_ctrl(6, 85); // LED2亮度6
                break;
            default:
                break;
            }
        }
            break;
        case KEY_7: // 菜单按键
            // __this->key_disable = 1; // 禁用按键
            // sys_touch_event_disable(); // 禁用触摸事件
        if (is_setting_shown) {
            // ui_hide(ENC_SET_WIN); // 隐藏二级菜单
            // ui_hide(ENC_MENU_SET_2);    // 隐藏二级菜单2
            ui_hide(ENC_SET_WIN0);
            // ui_hide(ENC_BTN_NN);    // 隐藏二级菜单切换按钮
            //ui_show(ENC_BTN_SPIN);   // 显示方向键
            is_setting_shown = 0; // 更新状态为关闭
            ui_no_highlight_element_by_id(ENC_SET_WIN_2);
        } else {
            // ui_show(ENC_BTN_NN);// 显示二级菜单切换按钮
            ui_show(ENC_SET_WIN0);
            // ui_show(ENC_SET_WIN); // 显示二级菜单1
           //ui_hide(ENC_BTN_SPIN);   // 隐藏方向键
            is_setting_shown = 1; // 更新状态为打开
            ui_highlight_element_by_id(ENC_SET_WIN_2); // 高亮二级菜单切换按钮

        }

            break;
        case KEY_8:  // 窗口切换
            // __this->key_disable = 1;
            // sys_touch_event_disable();
            // it.name = "video_rec";
            // it.action = ACTION_VIDEO_REC_CONTROL;
            // start_app_async(&it, rec_control_ok, NULL);
            switch_camera_disp();//窗口切换函数
            break;

        break;
        case KEY_9:  // 镜像按键
        current_zoom_level++;
        if (current_zoom_level > MAX_ZOOM_LEVEL) {
            current_zoom_level = 0; // 循环重置到初始状态
        }
        int zoom_factor = current_zoom_level * ZOOM_STEP; // 根据点击次数计算变焦因子

        switch (zoom_factor) {
            case 0:
                video_display_mirror();
                ui_pic_show_image_by_id(PIC_REC_XZ, 0);
                break;
            case 1:
                video_display_mirror();
                ui_pic_show_image_by_id(PIC_REC_XZ, 5);
                break;
            default:
                break;
        }
        break;
        case KEY_10:  // 变焦按键缩放按键
        // 计算新的变焦级别
        current_zoom0_level++;
        if (current_zoom0_level > MAX_ZOOM0_LEVEL) {
            current_zoom0_level = 0; // 循环重置到初始状态
        }

        zoom0_factor = current_zoom0_level * ZOOM_STEP; // 根据点击次数计算变焦因子

        // 调用变焦处理逻辑
        switch (zoom0_factor) {
            case 0:
                video_set_crop(0, 0, 1280, 720);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 0);
                break;
            case 1:
                video_set_crop(32, 18, 1216, 684);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 1);
                break;
            case 2:
                video_set_crop(64, 36, 1152, 648);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 2);
                break;
            case 3:
                video_set_crop(96, 54, 1088, 612);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 3);
                break;
            case 4:
                video_set_crop(128, 72, 1024, 576);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 4);
                break;
            case 5:
                video_set_crop(160, 90, 960, 540);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 5);
                break;
            case 6:
                video_set_crop(192, 108, 896, 504);
                ui_pic_show_image_by_id(PIC_REC_FOUCS, 6);
            break;
        default:
            break;
        }
            break;
        case KEY_MODE: // 模式键处理
            __this->onkey_mod = 1;
            __this->onkey_sel = 1;
            // ui_no_highlight_element_by_id(ENC_PIC_REC); // 取消高亮录像图标
            ui_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]); // 高亮选择项
            break;
        default:
            return false;
        }
    } else if (__this->onkey_mod == 1) { // onkey模式为1时处理
        switch (e->value) {
        case KEY_UP: // 按上键处理
            ui_no_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]); // 取消当前项高亮
            __this->onkey_sel --;
            if (__this->onkey_sel < 1) {
                __this->onkey_sel = 3; // 如果选择项少于1，设置为3
            }
            ui_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]); // 高亮新的选择项
            break;
        case KEY_DOWN: // 按下键处理
            ui_no_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]); // 取消当前项高亮
            __this->onkey_sel ++;
            if (__this->onkey_sel > 3) {
                __this->onkey_sel = 1; // 如果选择项大于3，设置为1
            }
            ui_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]); // 高亮新的选择项
            break;
        case KEY_OK: // OK键处理
//             switch (__this->onkey_sel) {
//             case 1:
//                 if (!__this->menu_status) {
//                     __this->onkey_mod = 2;
//                     __this->onkey_sel = 1;
//                     disp_RecSetting_lay(1); // 显示录像设置界面
//                 }
//                 break;
//             case 2:
//                 if (if_in_rec == TRUE) {
//                     break;
//                 }
//                 init_intent(&it);
//                 app = get_current_app();
//                 if (app) {
//                     __this->page_exit = MODE_SW_EXIT; // 切换模式退出
//                     it.name = "video_rec";
//                     it.action = ACTION_BACK;
//                     start_app_async(&it, NULL, NULL); // 异步启动应用

//                     it.name = "video_photo";
//                     it.action = ACTION_PHOTO_TAKE_MAIN; // 启动拍照应用
//                     start_app_async(&it, NULL, NULL); // 异步启动拍照应用
//                 }
//                 break;
//             case 3:
//                 if (if_in_rec) { // 如果正在录像
//                     __this->page_exit = HOME_SW_EXIT; // 设置主页退出标志
// #if REC_RUNNING_TO_HOME
//                     ui_hide(ui_get_current_window_id()); // 隐藏当前窗口
//                     set_page_main_flag(0); // 设置主页标志
// #else
//                     puts("It is in rec,can't back home.\n"); // 提示正在录像，无法返回主页
// #endif
//                     break;
//                 }
//                 __this->page_exit = HOME_SW_EXIT; // 主页退出
//                 init_intent(&it);
//                 app = get_current_app();
//                 if (app) {
//                     it.name = "video_rec";
//                     it.action = ACTION_BACK; // 返回动作
//                     start_app_async(&it, NULL, NULL); // 异步启动应用
//                 }
//                 break;
//             }
//             break;
        case KEY_MODE: // 模式键处理
            // ui_highlight_element_by_id(ENC_PIC_REC); // 高亮录像图标
            ui_no_highlight_element_by_id(onkey_sel_item[__this->onkey_sel - 1]); // 取消当前项高亮
            __this->onkey_mod = 0;
            __this->onkey_sel = 0;
            break;
        default:
            return false;
        }
    } else if (__this->onkey_mod == 2) { // onkey模式为2时处理
        switch (e->value) {
        case KEY_UP: // 按上键处理
            __this->onkey_sel --;
            if (__this->onkey_sel < 1) {
                __this->onkey_sel = 12; // 如果选择项少于1，设置为12
            }
            break;
        case KEY_DOWN: // 按下键处理
            __this->onkey_sel ++;
            if (__this->onkey_sel > 12) {
                __this->onkey_sel = 1; // 如果选择项大于12，设置为1
            }
            break;
//         case KEY_OK: // OK键处理
//             if (__this->onkey_sel) {
//                 switch (__this->onkey_sel) {
//                 case 1: // 分辨率设置
// #ifndef CONFIG_VIDEO4_ENABLE
//                     // __this->onkey_mod = 2 + __this->onkey_sel;
//                     // __this->resolution = index_of_table8(db_select("res"), TABLE(table_video_resolution)); // 获取分辨率索引
//                     // __this->onkey_sel = __this->resolution;
//                     // ui_set_call(enc_menu, ENC_MENU_RESOLUTION); // 设置菜单为分辨率
// #endif
//                     break;
//                 case 2: // 双视频设置
// #ifndef CONFIG_VIDEO4_ENABLE
//                     __this->onkey_mod = 2 + __this->onkey_sel;
//                     __this->double_route = db_select("two"); // 双视频设置
//                     __this->onkey_sel = __this->double_route;
//                     // ui_set_call(enc_menu, ENC_MENU_DUALVIDEO); // 设置菜单为双视频
// #endif
//                     break;
//                 case 3: // 循环录像设置
//                     __this->onkey_mod = 2 + __this->onkey_sel;
//                     __this->cycle_rec = index_of_table8(db_select("cyc"), TABLE(table_video_cycle)); // 获取循环录像设置
//                     __this->onkey_sel = __this->cycle_rec;
//                     // ui_set_call(enc_menu, ENC_MENU_CYCLE); // 设置菜单为循环录像
//                     break;
//                 case 4: // 间隔录像设置
//                     __this->onkey_mod = 2 + __this->onkey_sel;
//                     __this->gap_rec = index_of_table16(db_select("gap"), TABLE(table_video_gap)); // 获取间隔录像设置
//                     __this->onkey_sel = __this->gap_rec;
//                     // ui_set_call(enc_menu, ENC_MENU_GAP); // 设置菜单为间隔录像
//                     break;
//                 case 5: // HDR设置
// #ifndef CONFIG_VIDEO4_ENABLE
//                     __this->onkey_mod = 2 + __this->onkey_sel;
//                     __this->wdr = db_select("wdr"); // HDR设置
//                     __this->onkey_sel = __this->wdr;
//                     // ui_set_call(enc_menu, ENC_MENU_HDR); // 设置菜单为HDR
// #endif
//                     break;
//                 case 6:
// #ifndef CONFIG_VIDEO4_ENABLE
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->exposure = index_of_table8(db_select("exp"), TABLE(table_video_exposure));  // 获取曝光值的索引
//                     __this->onkey_sel = __this->exposure;  // 保存当前选择
//                     // ui_set_call(enc_menu, ENC_MENU_EXPOSURE);  // 切换到曝光菜单
// #endif
//                     break;
//                 case 7:
// #ifndef CONFIG_VIDEO4_ENABLE
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->motdet = db_select("mot");  // 获取运动检测值
//                     __this->onkey_sel = __this->motdet;  // 保存当前选择
//                     // ui_set_call(enc_menu, ENC_MENU_MOTION);  // 切换到运动检测菜单
// #endif
//                     break;
//                 case 8:
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->dat_label = db_select("dat");  // 获取日期标签值
//                     __this->onkey_sel = __this->dat_label;  // 保存当前选择
//                     ui_set_call(enc_menu, ENC_MENU_LABEL);  // 切换到日期标签菜单
//                     break;
//                 case 9:
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->gravity = index_of_table8(db_select("gra"), TABLE(table_video_gravity));  // 获取重力值的索引
//                     __this->onkey_sel = __this->gravity;  // 保存当前选择
//                     ui_set_call(enc_menu, ENC_MENU_GSEN);  // 切换到重力传感器菜单
//                     break;
//                 case 10:
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->mic = db_select("mic");  // 获取麦克风值
//                     __this->onkey_sel = __this->mic;  // 保存当前选择
//                     ui_set_call(enc_menu, ENC_MENU_SOUND);  // 切换到声音菜单
//                     break;
//                 case 11:
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->park_guard = db_select("par");  // 获取停车守卫值
//                     __this->onkey_sel = __this->park_guard;  // 保存当前选择
//                     ui_set_call(enc_menu, ENC_MENU_GUARD);  // 切换到停车守卫菜单
//                     break;
//                 case 12:
// #ifndef CONFIG_VIDEO4_ENABLE
//                     __this->onkey_mod = 2 + __this->onkey_sel;  // 设置当前选择模式
//                     __this->car_num = db_select("num");  // 获取车牌号值
//                     __this->onkey_sel = __this->car_num;  // 保存当前选择
//                     ui_set_call(enc_menu, ENC_MENU_CID);  // 切换到车牌号菜单
// #endif
//                     break;
//                 }
            // }
            break;
        case KEY_MODE:
            __this->onkey_mod = 1;  // 设置为模式选择
            __this->onkey_sel = 1;  // 默认选择第一个选项
            disp_RecSetting_lay(0);  // 显示录像设置界面
            break;
        default:
            return false;  // 其他按键不处理
        }
    } else if (__this->onkey_mod > 2 && __this->onkey_mod < 3 + 12) {  // 如果在子菜单模式
        s8 tmp;  // 临时变量
        switch (e->value) {
        case KEY_UP:  // 上键处理
            switch (__this->enc_menu_status) {
            case ENC_MENU_RESOLUTION:  // 分辨率菜单
                // tmp = __this->resolution;  // 获取当前分辨率
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C3[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 2 : tmp - 1;  // 循环选择分辨率
                // ui_highlight_element_by_id(_ENC_SET_PIC_C3[tmp]);  // 高亮新选择的分辨率
                // menu_rec_set_res(tmp);  // 设置新的分辨率
                break;
            case ENC_MENU_DUALVIDEO:  // 双摄像头菜单
                // tmp = __this->double_route;  // 获取双摄像头设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 1 : tmp - 1;  // 切换双摄像头设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择
                // menu_rec_set_double(tmp);  // 设置双摄像头
                break;
            case ENC_MENU_CYCLE:  // 循环录像菜单
                // tmp = __this->cycle_rec;  // 获取循环录像设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 3 : tmp - 1;  // 切换循环录像选项
                // ui_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 高亮新选择
                // menu_rec_set_cycle(tmp);  // 设置循环录像
                break;
            case ENC_MENU_GAP:  // 间隔录制菜单
                // tmp = __this->gap_rec;  // 获取间隔录制设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 3 : tmp - 1;  // 切换间隔录制选项
                // ui_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 高亮新选择
                // menu_rec_set_gap(tmp);  // 设置间隔录制
                break;
            case ENC_MENU_HDR:  // HDR菜单
                // tmp = __this->wdr;  // 获取HDR设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 1 : tmp - 1;  // 切换HDR设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择
                // menu_rec_set_wdr(tmp);  // 设置HDR
                break;
            case ENC_MENU_EXPOSURE:  // 曝光菜单
                // if (__this->exposure == 6) {  // 如果已经达到最大曝光值，停止调整
                //     break;
                // }
                // tmp = __this->exposure;  // 获取当前曝光值
                // tmp++;  // 增加曝光值
                // ui_hide(_ENC_SET_PIC_EXP[6 - __this->exposure]);  // 隐藏当前曝光设置
                // ui_show(_ENC_SET_PIC_EXP[6 - tmp]);  // 显示新的曝光设置
                // menu_rec_set_exposure(tmp);  // 设置新的曝光值
                break;
            case ENC_MENU_MOTION:  // 运动检测菜单
                // tmp = __this->motdet;  // 获取运动检测设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 1 : tmp - 1;  // 切换运动检测设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择
                // menu_rec_set_mot(tmp);  // 设置运动检测
                break;
            case ENC_MENU_LABEL:  // 标签菜单
                tmp = __this->dat_label;  // 获取日期标签设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消当前选项的高亮
                tmp = tmp == 0 ? 1 : tmp - 1;  // 切换日期标签设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择
                menu_rec_set_dat(tmp);  // 设置日期标签
                break;
            case ENC_MENU_GSEN:  // 重力传感器菜单
                // tmp = __this->gravity;  // 获取重力设置
                // if (tmp == 0) {
                //     ui_show(ENC_PIC_GSEN);  // 显示重力传感器图标
                // }
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 3 : tmp - 1;  // 切换重力传感器设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 高亮新选择
                // menu_rec_set_gravity(tmp);  // 设置重力传感器
                break;
            case ENC_MENU_SOUND:  // 声音菜单
                // tmp = __this->mic;  // 获取麦克风设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 1 : tmp - 1;  // 切换麦克风设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择
                // menu_rec_set_mic(tmp);  // 设置麦克风
                break;
            case ENC_MENU_GUARD:  // 停车守卫菜单
                // tmp = __this->park_guard;  // 获取停车守卫设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 1 : tmp - 1;  // 切换停车守卫设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择
                // menu_rec_set_par(tmp);  // 设置停车守卫
                break;
            case ENC_MENU_CID:  // 车牌号菜单
                // tmp = __this->car_num;  // 获取车牌号设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消当前选项的高亮
                // tmp = tmp == 0 ? 1 : tmp - 1;  // 切换车牌号设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择
                // menu_rec_set_num(tmp);  // 设置车牌号
                break;
            }
            break;
        case KEY_DOWN:  // 当按下向下键时执行的代码
            switch (__this->enc_menu_status) {  // 根据当前的菜单状态执行相应操作
            case ENC_MENU_RESOLUTION:  // 解析度菜单
                // tmp = __this->resolution;  // 获取当前解析度
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C3[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 2 ? 0 : tmp + 1;  // 选择下一个解析度，循环到0
                // ui_highlight_element_by_id(_ENC_SET_PIC_C3[tmp]);  // 高亮新选择的元素
                // menu_rec_set_res(tmp);  // 更新解析度设置
                break;
            case ENC_MENU_DUALVIDEO:  // 双摄像头菜单
                // tmp = __this->double_route;  // 获取当前双摄像头设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换双摄像头设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择的元素
                // menu_rec_set_double(tmp);  // 更新双摄像头设置
                break;
            case ENC_MENU_CYCLE:  // 循环录制菜单
                // tmp = __this->cycle_rec;  // 获取当前循环设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 3 ? 0 : tmp + 1;  // 切换到下一个循环设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 高亮新选择的元素
                // menu_rec_set_cycle(tmp);  // 更新循环设置
                break;
            case ENC_MENU_GAP:  // 间隔录制菜单
                // tmp = __this->gap_rec;  // 获取当前间隔设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 3 ? 0 : tmp + 1;  // 切换到下一个间隔设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 高亮新选择的元素
                // menu_rec_set_gap(tmp);  // 更新间隔设置
                break;
            case ENC_MENU_HDR:  // HDR菜单
                // tmp = __this->wdr;  // 获取当前HDR设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换HDR设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择的元素
                // menu_rec_set_wdr(tmp);  // 更新HDR设置
                break;
            case ENC_MENU_EXPOSURE:  // 曝光菜单
                // if (__this->exposure == 0) {  // 如果当前曝光为0，则不做操作
                //     break;
                // }
                // tmp = __this->exposure;  // 获取当前曝光值
                // tmp--;  // 减少曝光值
                // ui_hide(_ENC_SET_PIC_EXP[6 - __this->exposure]);  // 隐藏当前曝光图标
                // ui_show(_ENC_SET_PIC_EXP[6 - tmp]);  // 显示新的曝光图标
                // menu_rec_set_exposure(tmp);  // 更新曝光设置
                break;
            case ENC_MENU_MOTION:  // 运动检测菜单
                // tmp = __this->motdet;  // 获取当前运动检测设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换运动检测设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择的元素
                // menu_rec_set_mot(tmp);  // 更新运动检测设置
                break;
            case ENC_MENU_LABEL:  // 标签菜单
                // tmp = __this->dat_label;  // 获取当前标签设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换标签设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C20[tmp]);  // 高亮新选择的元素
                // menu_rec_set_dat(tmp);  // 更新标签设置
                break;
            case ENC_MENU_GSEN:  // 重力传感器菜单
                // tmp = __this->gravity;  // 获取当前重力传感器设置
                // if (tmp == 0) {  // 如果重力传感器设置为0
                //     ui_show(ENC_PIC_GSEN);  // 显示重力传感器图标
                // }
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 3 ? 0 : tmp + 1;  // 切换重力传感器设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C4[tmp]);  // 高亮新选择的元素
                // menu_rec_set_gravity(tmp);  // 更新重力传感器设置
                break;
            case ENC_MENU_SOUND:  // 音频菜单
                // tmp = __this->mic;  // 获取当前音频设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换音频设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择的元素
                // menu_rec_set_mic(tmp);  // 更新音频设置
                break;
            case ENC_MENU_GUARD:  // 停车监控菜单
                // tmp = __this->park_guard;  // 获取当前停车监控设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换停车监控设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择的元素
                // menu_rec_set_par(tmp);  // 更新停车监控设置
                break;
            case ENC_MENU_CID:  // 车辆ID菜单
                // tmp = __this->car_num;  // 获取当前车辆ID设置
                // ui_no_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 取消高亮当前元素
                // tmp = tmp == 1 ? 0 : tmp + 1;  // 切换车辆ID设置
                // ui_highlight_element_by_id(_ENC_SET_PIC_C2[tmp]);  // 高亮新选择的元素
                // menu_rec_set_num(tmp);  // 更新车辆ID设置
                break;
            }
            break;
        case KEY_OK:  // 确认键处理
        case KEY_MODE:  // 模式键处理
            // __this->onkey_sel = __this->onkey_mod - 2;  // 记录选择的菜单选项
            // __this->onkey_mod = 2;  // 返回主菜单模式
            // enc_menu(__this->enc_menu_status);  // 切换菜单状态
           // ui_highlight_element_by_id(onkey_sel_setting[__this->onkey_sel - 1]); // 高亮选中的设置
            //ui_highlight_element_by_id(onkey_sel_setting1[__this->onkey_sel - 1]); // 高亮选中的设置1
            break;
        default:
            return false;
        }
    }
    return true;
}
REGISTER_UI_EVENT_HANDLER(ENC_WIN)
.onchange = enc_onchange,
 .onkey = enc_onkey,
};



static int enc_car_pos_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct draw_context *dc = (struct draw_context *)arg;

    switch (e) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        dc->rect.top = __this->car_pos_y;
        dc->draw.top = dc->rect.top;
        dc->rect.left = __this->car_pos_x;
        dc->draw.left = dc->rect.left;
        dc->rect.width = __this->car_pos_w;
        dc->draw.width = dc->rect.width;
        break;
    default:
        return false;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(ENC_PIC_POS)
.onchange = enc_car_pos_onchange,
};

/***************************** 时间设置 ************************************/
static int set_time_ontouch(void *ctr, struct element_touch_event *e)
{


    UI_ONTOUCH_DEBUG("**time set ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        ui_show(SYS_LAY_TIME);
        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(SYS_SET_TIME_BTN)
.ontouch =  set_time_ontouch,
};

/***************************** 图像参数 ************************************/
static int set_photo_ontouch(void *ctr, struct element_touch_event *e)
{


    UI_ONTOUCH_DEBUG("**time set ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        ui_show(BTN_SET_POT);
        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_PP_POT)
.ontouch =  set_photo_ontouch,
};

/***************************** 系统时间控件动作 ************************************/
// 显示系统菜单的时间设置界面
static struct sys_time t;
static struct utime ts, tu, td;
static u8 day_max = 31;
static u8 day_set = 31;static void sys_menu_tim_show()
{
    // 打开RTC设备（实时时钟）
    void *fd = dev_open("rtc", NULL);

    // 如果无法打开 RTC 设备（fd 为空），则初始化时间为默认值
    if (!fd) {
        // 初始化时间为默认值：2000-01-01 00:00:00
        t.year = 2000;
        t.month = 1;
        t.day = 1;
        t.hour = 0;
        t.min = 0;
        t.sec = 0;
    } else {
        // 从RTC设备读取系统时间，并存储到 t 结构体中
        dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)&t);

        // 输出调试信息，打印获取的系统时间
        UI_ONTOUCH_DEBUG("get_sys_time: %d-%d-%d %d:%d:%d\n",
                         t.year,
                         t.month,
                         t.day,
                         t.hour,
                         t.min,
                         t.sec);

        // 关闭RTC设备
        dev_close(fd);
    }

    // 将读取到的系统时间保存到 ts 结构体中
    ts.year = t.year;
    ts.month = t.month;
    ts.day = t.day;
    ts.hour = t.hour;
    ts.min = t.min;
    ts.sec = t.sec;

    // 使用 ts 中的时间值更新用户界面上的时间显示
    ui_time_update_by_id(SYS_LAY_TIME_YEAR, &ts);    // 更新年份
    ui_time_update_by_id(SYS_LAY_TIME_MONTH, &ts);   // 更新月份
    ui_time_update_by_id(SYS_LAY_TIME_DAY, &ts);     // 更新日期
    ui_time_update_by_id(SYS_LAY_TIME_HOURS, &ts);   // 更新小时
    ui_time_update_by_id(SYS_LAY_TIME_MINUTE, &ts);  // 更新分钟
    ui_time_update_by_id(SYS_LAY_TIME_SEC, &ts);     // 更新秒数
}

// 定时器系统录制布局的事件处理函数
static int set_timer_sys_rec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    UI_ONTOUCH_DEBUG("**time set ontouch 1**\n");  // 调试信息输出，表示触控事件触发

    struct layout *layout = (struct layout *)ctr;  // 将传入的控件指针转换为布局类型指针

    switch (e) {
    case ON_CHANGE_FIRST_SHOW:  // 当布局第一次显示时触发的事件
        ui_ontouch_lock(layout);  // 锁定触控，防止在显示期间有其他触控操作
        sys_menu_tim_show();  // 显示系统菜单的时间设置界面
        break;

    case ON_CHANGE_RELEASE:  // 当布局释放时触发的事件
        ui_ontouch_unlock(layout);  // 解锁触控，允许用户继续进行触控操作
        break;

    default:
        return false;  // 对于未处理的事件类型，返回 false，表示未处理
    }

    return false;  // 对于所有情况，返回 false 表示未进行进一步处理
}

// 注册 UI 事件处理程序，将 `SYS_LAY_TIME` 的 `onchange` 事件绑定到 `set_timer_sys_rec_onchange` 函数
REGISTER_UI_EVENT_HANDLER(SYS_LAY_TIME)
.onchange = set_timer_sys_rec_onchange,  // 绑定 `onchange` 事件
.onkey = NULL,  // 不处理按键事件
.ontouch = NULL,  // 不处理触摸事件
};

// 定时器系统语言布局的事件处理函数
static int set_LANGUAGE_sys_rec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    UI_ONTOUCH_DEBUG("**time set ontouch 1**\n");  // 调试信息输出，表示触控事件触发

    struct layout *layout = (struct layout *)ctr;  // 将传入的控件指针转换为布局类型指针

    switch (e) {
    case ON_CHANGE_FIRST_SHOW:  // 当布局第一次显示时触发的事件
        ui_ontouch_lock(layout);  // 锁定触控，防止在显示期间有其他触控操作
        // sys_menu_tim_show();  // 显示系统菜单的时间设置界面
        break;

    case ON_CHANGE_RELEASE:  // 当布局释放时触发的事件
        ui_ontouch_unlock(layout);  // 解锁触控，允许用户继续进行触控操作
        break;

    default:
        return false;  // 对于未处理的事件类型，返回 false，表示未处理
    }

    return false;  // 对于所有情况，返回 false 表示未进行进一步处理
}


REGISTER_UI_EVENT_HANDLER(SYS_YY_0)
.onchange = set_LANGUAGE_sys_rec_onchange,  // 绑定 `onchange` 事件
.onkey = NULL,  // 不处理按键事件
.ontouch = NULL,  // 不处理触摸事件
};

// // 定时器系统图片参数布局的事件处理函数
// static int set_poto_onchange(void *ctr, enum element_change_event e, void *arg)
// {
//     UI_ONTOUCH_DEBUG("**time set ontouch 1**\n");  // 调试信息输出，表示触控事件触发

//     struct layout *layout = (struct layout *)ctr;  // 将传入的控件指针转换为布局类型指针

//     switch (e) {
//     case ON_CHANGE_FIRST_SHOW:  // 当布局第一次显示时触发的事件
//         ui_ontouch_lock(layout);  // 锁定触控，防止在显示期间有其他触控操作
//         break;

//     case ON_CHANGE_RELEASE:  // 当布局释放时触发的事件
//         ui_ontouch_unlock(layout);  // 解锁触控，允许用户继续进行触控操作
//         break;

//     default:
//         return false;  // 对于未处理的事件类型，返回 false，表示未处理
//     }

//     return false;  // 对于所有情况，返回 false 表示未进行进一步处理
// }
// REGISTER_UI_EVENT_HANDLER(BTN_SET_POT)
// .onchange = set_poto_onchange,  // 绑定 `onchange` 事件
// .onkey = NULL,  // 不处理按键事件
// .ontouch = NULL,  // 不处理触摸事件
// };

///***************************** 时间加减按钮动作 ************************************/
// 判断是否是闰年
static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 根据年份和月份返回该月的天数
static int get_days_in_month(int year, int month) {
    // 每个月的天数
    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // 检查月份是否为2月，并且年份是否是闰年
    if (month == 2 && is_leap_year(year)) {
        return 29;  // 闰年2月有29天
    }

    // 返回相应月份的天数
    return days_in_month[month - 1];
}

enum SYS_MENU_TIME {
    SYS_TIME_YEAR_DEC,
    SYS_TIME_MONTH_DEC,
    SYS_TIME_DAY_DEC,
    SYS_TIME_HOURS_DEC,
    SYS_TIME_MIN_DEC,
    SYS_TIME_SEC_DEC,
    SYS_TIME_YEAR_ADD,
    SYS_TIME_MONTH_ADD,
    SYS_TIME_DAY_ADD,
    SYS_TIME_HOURS_ADD,
    SYS_TIME_MIN_ADD,
    SYS_TIME_SEC_ADD,
};
static void sys_parm_set_(int value)
{
    switch (value) {
        // 年份减少
        case SYS_TIME_YEAR_DEC:
            t.year--;
            ts.year = t.year;
            ui_time_update_by_id(SYS_LAY_TIME_YEAR, &ts);
            break;

        // 月份减少
        case SYS_TIME_MONTH_DEC:
            if (t.month > 1) {
                t.month--;
            } else {
                t.month = 12;
                t.year--;  // 借位年份
            }
            ts.month = t.month;
            ui_time_update_by_id(SYS_LAY_TIME_MONTH, &ts);
            break;

        // 天数减少
        case SYS_TIME_DAY_DEC:
            if (t.day > 1) {
                t.day--;
            } else {
                // 借位到前一个月，并调整天数
                if (t.month == 1) {
                    t.month = 12;
                    t.year--;
                } else {
                    t.month--;
                }
                t.day = get_days_in_month(t.year, t.month);  // 获取当前月的天数
            }
            ts.day = t.day;
            ui_time_update_by_id(SYS_LAY_TIME_DAY, &ts);
            break;

        // 小时减少
        case SYS_TIME_HOURS_DEC:
            if (t.hour > 0) {
                t.hour--;
            } else {
                t.hour = 23;  // 借位回到23小时
                t.day--;      // 日期借位减少一天
                if (t.day < 1) {
                    // 借位到前一个月，并调整天数
                    if (t.month == 1) {
                        t.month = 12;
                        t.year--;
                    } else {
                        t.month--;
                    }
                    t.day = get_days_in_month(t.year, t.month);  // 获取当前月的天数
                }
            }
            ts.hour = t.hour;
            ui_time_update_by_id(SYS_LAY_TIME_HOURS, &ts);
            break;

        // 分钟减少
        case SYS_TIME_MIN_DEC:
            if (t.min > 0) {
                t.min--;
            } else {
                t.min = 59;  // 借位回到59分钟
                t.hour--;    // 小时借位减少一小时
                if (t.hour < 0) {
                    t.hour = 23;
                    t.day--;  // 日期借位减少一天
                    if (t.day < 1) {
                        // 借位到前一个月，并调整天数
                        if (t.month == 1) {
                            t.month = 12;
                            t.year--;
                        } else {
                            t.month--;
                        }
                        t.day = get_days_in_month(t.year, t.month);  // 获取当前月的天数
                    }
                }
            }
            ts.min = t.min;
            ui_time_update_by_id(SYS_LAY_TIME_MINUTE, &ts);
            break;

        // 秒数减少
        case SYS_TIME_SEC_DEC:
            if (t.sec > 0) {
                t.sec--;
            } else {
                t.sec = 59;  // 借位回到59秒
                t.min--;     // 分钟借位减少一分钟
                if (t.min < 0) {
                    t.min = 59;
                    t.hour--;  // 小时借位减少一小时
                    if (t.hour < 0) {
                        t.hour = 23;
                        t.day--;  // 日期借位减少一天
                        if (t.day < 1) {
                            // 借位到前一个月，并调整天数
                            if (t.month == 1) {
                                t.month = 12;
                                t.year--;
                            } else {
                                t.month--;
                            }
                            t.day = get_days_in_month(t.year, t.month);  // 获取当前月的天数
                        }
                    }
                }
            }
            ts.sec = t.sec;
            ui_time_update_by_id(SYS_LAY_TIME_SEC, &ts);
            break;

        // 年份增加
        case SYS_TIME_YEAR_ADD:
            t.year++;
            ts.year = t.year;
            ui_time_update_by_id(SYS_LAY_TIME_YEAR, &ts);
            break;

        // 月份增加
        case SYS_TIME_MONTH_ADD:
            if (t.month < 12) {
                t.month++;
            } else {
                t.month = 1;
                t.year++;  // 进位年份
            }
            ts.month = t.month;
            ui_time_update_by_id(SYS_LAY_TIME_MONTH, &ts);
            break;

        // 天数增加
        case SYS_TIME_DAY_ADD:
            if (t.day < get_days_in_month(t.year, t.month)) {
                t.day++;
            } else {
                t.day = 1;  // 进位到下个月
                if (t.month < 12) {
                    t.month++;
                } else {
                    t.month = 1;
                    t.year++;
                }
            }
            ts.day = t.day;
            ui_time_update_by_id(SYS_LAY_TIME_DAY, &ts);
            break;

        // 小时增加
        case SYS_TIME_HOURS_ADD:
            if (t.hour < 23) {
                t.hour++;
            } else {
                t.hour = 0;  // 进位回到0点
                t.day++;     // 日期进位增加一天
                if (t.day > get_days_in_month(t.year, t.month)) {
                    t.day = 1;
                    t.month++;
                    if (t.month > 12) {
                        t.month = 1;
                        t.year++;
                    }
                }
            }
            ts.hour = t.hour;
            ui_time_update_by_id(SYS_LAY_TIME_HOURS, &ts);
            break;

        // 分钟增加
        case SYS_TIME_MIN_ADD:
            if (t.min < 59) {
                t.min++;
            } else {
                t.min = 0;  // 进位到0分钟
                t.hour++;   // 小时进位增加一小时
                if (t.hour > 23) {
                    t.hour = 0;
                    t.day++;  // 日期进位增加一天
                    if (t.day > get_days_in_month(t.year, t.month)) {
                        t.day = 1;
                        t.month++;
                        if (t.month > 12) {
                            t.month = 1;
                            t.year++;
                        }
                    }
                }
            }
            ts.min = t.min;
            ui_time_update_by_id(SYS_LAY_TIME_MINUTE, &ts);
            break;

        // 秒数增加
        case SYS_TIME_SEC_ADD:
            if (t.sec < 59) {
                t.sec++;
            } else {
                t.sec = 0;  // 进位到0秒
                t.min++;    // 分钟进位增加一分钟
                if (t.min > 59) {
                    t.min = 0;
                    t.hour++;  // 小时进位增加一小时
                    if (t.hour > 23) {
                        t.hour = 0;
                        t.day++;  // 日期进位增加一天
                        if (t.day > get_days_in_month(t.year, t.month)) {
                            t.day = 1;
                            t.month++;
                            if (t.month > 12) {
                                t.month = 1;
                                t.year++;
                            }
                        }
                    }
                }
            }
            ts.sec = t.sec;
            ui_time_update_by_id(SYS_LAY_TIME_SEC, &ts);
            break;
    }
}

static int sys_time_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**set time button**");

    struct intent it;
    int sel_item = 0;
    struct button *btn = (struct button *)ctr;
    const static int btn_id[] = {
        BTN_YEAR_UP,
        BTN_MONTH_UP,
        BTN_DAY_UP,
        BTN_HOURS_UP,
        BTN_MIN_UP,
        BTN_SEC_UP,
        BTN_YEAR_DOWN,
        BTN_MONTH_DOWN,
        BTN_DAY_DOWN,
        BTN_HOURS_DOWN,
        BTN_MIN_DOWN,
        BTN_SEC_DOWN,
    };
    u8 i,j;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        if(__this->onkey_sel){
            ui_no_highlight_element_by_id(btn_id[__this->onkey_sel-1]);
        }
        j=sizeof(btn_id)/sizeof(btn_id[0]);
        for (i = 0; i < j; i++) {
            if (btn->elm.id == btn_id[i]) {
                ui_highlight_element_by_id(btn_id[i]);
                sel_item = i;
                break;
            }
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
//        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
//        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        j=sizeof(btn_id)/sizeof(btn_id[0]);
        for (i = 0; i < j; i++) {
            if (btn->elm.id == btn_id[i]) {
                ui_no_highlight_element_by_id(btn_id[i]);
                sel_item = i;
                break;
            }
        }
        printf("=============button=%d\n",sel_item);
        __this->onkey_sel = 0;
        sys_parm_set_(sel_item);
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(BTN_YEAR_UP)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_MONTH_UP)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_DAY_UP)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_HOURS_UP)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_MIN_UP)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_SEC_UP)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_YEAR_DOWN)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_MONTH_DOWN)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_DAY_DOWN)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_HOURS_DOWN)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_MIN_DOWN)
.ontouch = sys_time_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BTN_SEC_DOWN)
.ontouch = sys_time_ontouch,
};
void save_set_time()
{
    // 将 ts（时间源）的各个时间属性复制到 t（目标时间结构）中
    t.year = ts.year;
    t.month = ts.month;
    t.day = ts.day;
    t.hour = ts.hour;
    t.min = ts.min;
    t.sec = ts.sec;

    // 打开 RTC（实时时钟）设备
    void *fd = dev_open("rtc", NULL);

    // 设置系统时间，将 t 结构体中的时间传递给 RTC 设备
    dev_ioctl(fd, IOCTL_SET_SYS_TIME, (u32)&t);

    // 关闭 RTC 设备
    dev_close(fd);

    // 使用指定 ID 更新 UI 上显示的时间
    ui_time_update_by_id(ENC_TIM_TIME, &ts);
}

static int set_time_cancel_ontouch(void *ctr, struct element_touch_event *e)
{


    UI_ONTOUCH_DEBUG("**time set ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        ui_hide(SYS_LAY_TIME);
        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(BTN_SET_TIME_CANCEL)
.ontouch =  set_time_cancel_ontouch,
};
static int set_time_confirm_ontouch(void *ctr, struct element_touch_event *e)
{


    UI_ONTOUCH_DEBUG("**time set ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        save_set_time();
        ui_hide(SYS_LAY_TIME);
        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(BTN_SET_TIME_CONFIRM)
.ontouch =  set_time_confirm_ontouch,
};
static int set_potot_on_ontouch(void *ctr, struct element_touch_event *e)
{


    UI_ONTOUCH_DEBUG("**time set ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        save_set_time();
        ui_hide(BTN_SET_POT);
        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief
 */
REGISTER_UI_EVENT_HANDLER(ENC_POT_ON)
.ontouch =  set_potot_on_ontouch,
};
// static int set_potot_off_ontouch(void *ctr, struct element_touch_event *e)
// {


//     UI_ONTOUCH_DEBUG("**time set ontouch**\n");

//     switch (e->event) {
//     case ELM_EVENT_TOUCH_DOWN:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
//         break;
//     case ELM_EVENT_TOUCH_HOLD:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
//         break;
//     case ELM_EVENT_TOUCH_MOVE:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
//         break;
//     case ELM_EVENT_TOUCH_UP:
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
//         save_set_time();
//         ui_hide(BTN_SET_POT);
//         break;
//     default:
//         return false;
//     }
//     return false;
// }

// /**
//  * @brief
//  */
// REGISTER_UI_EVENT_HANDLER(ENC_POT_OFF)
// .ontouch =  set_potot_off_ontouch,
// };

//对比度设置按钮动作
/***************************** 对比度调节按钮动作（加/减按钮调节） ************************************/
#define MAX_CONTRAST_INDEX 4 // 最大对比度索引，共5个档位（从0到4）
#define MIN_CONTRAST_INDEX 0 // 最小对比度索引
extern void imd_contrast_effects_cfg(int parm); // 对比度设置函数

static int current_contrast_index = 2; // 当前对比度索引，默认指向110

// 设置对比度的函数，根据索引切换到相应的对比度值
static void set_contrast_by_index(int index)
{
    int contrast_value;

    switch (index) {
        case 0:
            contrast_value = 90;  // 对比度值90
            ui_pic_show_image_by_id(LIGHT_4, 0);
            db_update("con", 90); // 更新数据库记录当前对比度索引
            db_flush();
            break;
        case 1:
            contrast_value = 100; // 对比度值100
            ui_pic_show_image_by_id(LIGHT_4, 1);
            db_update("con", 100); // 更新数据库记录当前对比度索引
            db_flush();
            break;
            break;
        case 2:
            contrast_value = 110; // 对比度值110 (默认)
            ui_pic_show_image_by_id(LIGHT_4, 2);
            db_update("con", 110); // 更新数据库记录当前对比度索引
            db_flush();
            break;
        case 3:
            contrast_value = 120; // 对比度值120
            ui_pic_show_image_by_id(LIGHT_4, 3);
            db_update("con", 120); // 更新数据库记录当前对比度索引
            db_flush();
            break;
        case 4:
            contrast_value = 130; // 对比度值130
            ui_pic_show_image_by_id(LIGHT_4, 4);
            db_update("con", 130); // 更新数据库记录当前对比度索引
            db_flush();
            break;
        default:
            break;
    }

    imd_contrast_effects_cfg(contrast_value); // 调用函数设置新的对比度
    UI_ONTOUCH_DEBUG("Set Contrast to: %d\n", contrast_value); // 调试信息，输出设置的对比度值
}

// 增加对比度按钮的触摸事件处理
static int screen_contrast_increase_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_contrast_increase_ontouch**\n"); // 调试信息
    switch (e->event) {
        case ELM_EVENT_TOUCH_UP: // 触摸松开事件
            UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - Increase Contrast\n");
            if (current_contrast_index < MAX_CONTRAST_INDEX) { // 如果当前对比度未达到最大值
                current_contrast_index++; // 增加对比度索引
                set_contrast_by_index(current_contrast_index); // 调用函数设置新的对比度
                printf("current_contrast_index: %d\n", current_contrast_index); // 调试信息，输出当前对比度索引
            } else {
                UI_ONTOUCH_DEBUG("Contrast is already at maximum level\n");
            }
            break;
        default:
            break;
    }
    return false; // 返回false，事件未处理
}

// 减少对比度按钮的触摸事件处理
static int screen_contrast_decrease_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_contrast_decrease_ontouch**\n"); // 调试信息
    switch (e->event) {
        case ELM_EVENT_TOUCH_UP: // 触摸松开事件
            UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - Decrease Contrast\n");
            if (current_contrast_index > MIN_CONTRAST_INDEX) { // 如果当前对比度未达到最小值
                current_contrast_index--; // 减少对比度索引
                set_contrast_by_index(current_contrast_index); // 调用函数设置新的对比度
                printf("current_contrast_index: %d\n", current_contrast_index); // 调试信息，输出当前对比度索引
            } else {
                UI_ONTOUCH_DEBUG("Contrast is already at minimum level\n");
            }
            break;
        default:
            break;
    }
    return false; // 返回false，事件未处理
}

// 注册增加对比度按钮的触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(CONTRAST_INCREASE_BTN) // 这里的CONTRAST_INCREASE_BTN是增加对比度按钮的控件ID
.ontouch = screen_contrast_increase_ontouch, // 将增加对比度触摸处理函数与控件绑定
};

// 注册减少对比度按钮的触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(CONTRAST_DECREASE_BTN) // 这里的CONTRAST_DECREASE_BTN是减少对比度按钮的控件ID
.ontouch = screen_contrast_decrease_ontouch, // 将减少对比度触摸处理函数与控件绑定
};




/* *******************************图像饱和度控件动作*********************************/
#define MAX_SATURATION_INDEX 4 // 最大饱和度索引，共5个档位（从0到4）
#define MIN_SATURATION_INDEX 0 // 最小饱和度索引
extern void imd_saturation_effects_cfg(int parm); // 饱和度设置函数

static int current_saturation_index = 2; // 当前饱和度索引，默认值为中间档

// 设置饱和度的函数，根据索引切换到相应的饱和度值
static void set_saturation_by_index(int index)
{
    int saturation_value;

    switch (index) {
        case 0:
            saturation_value = 80;  // 饱和度值80
            ui_pic_show_image_by_id(LIGHT_6, 0); // 更新UI显示对应饱和度档位的图片
            db_update("sat", 80); // 更新数据库记录当前饱和度索引
            db_flush();
            break;
        case 1:
            saturation_value = 90;  // 饱和度值90
            ui_pic_show_image_by_id(LIGHT_6, 1);
            db_update("sat", 90); // 更新数据库记录当前饱和度索引
            db_flush();
            break;
        case 2:
            saturation_value = 100; // 饱和度值100 (默认)
            ui_pic_show_image_by_id(LIGHT_6, 2);
            db_update("sat", 100); // 更新数据库记录当前饱和度索引
            db_flush();
            break;
        case 3:
            saturation_value = 110; // 饱和度值110
            ui_pic_show_image_by_id(LIGHT_6, 3);
            db_update("sat", 110); // 更新数据库记录当前饱和度索引
            db_flush();
            break;
        case 4:
            saturation_value = 120; // 饱和度值120
            ui_pic_show_image_by_id(LIGHT_6, 4);
            db_update("sat", 120); // 更新数据库记录当前饱和度索引
            db_flush();
            break;
        default:
            break;
    }

    imd_saturation_effects_cfg(saturation_value); // 调用函数设置新的饱和度
    UI_ONTOUCH_DEBUG("Set Saturation to: %d\n", saturation_value); // 调试信息，输出设置的饱和度值
}

// 增加饱和度按钮的触摸事件处理
static int screen_saturation_increase_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_saturation_increase_ontouch**\n"); // 调试信息
    switch (e->event) {
        case ELM_EVENT_TOUCH_UP: // 触摸松开事件
            UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - Increase Saturation\n");
            if (current_saturation_index < MAX_SATURATION_INDEX) { // 如果当前饱和度未达到最大值
                current_saturation_index++; // 增加饱和度索引
                set_saturation_by_index(current_saturation_index); // 调用函数设置新的饱和度
                printf("current_saturation_index: %d\n", current_saturation_index); // 调试信息，输出当前饱和度索引
            } else {
                UI_ONTOUCH_DEBUG("Saturation is already at maximum level\n");
            }
            break;
        default:
            break;
    }
    return false; // 返回false，事件未处理
}

// 减少饱和度按钮的触摸事件处理
static int screen_saturation_decrease_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**screen_saturation_decrease_ontouch**\n"); // 调试信息
    switch (e->event) {
        case ELM_EVENT_TOUCH_UP: // 触摸松开事件
            UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - Decrease Saturation\n");
            if (current_saturation_index > MIN_SATURATION_INDEX) { // 如果当前饱和度未达到最小值
                current_saturation_index--; // 减少饱和度索引
                set_saturation_by_index(current_saturation_index); // 调用函数设置新的饱和度
                printf("current_saturation_index: %d\n", current_saturation_index); // 调试信息，输出当前饱和度索引
            } else {
                UI_ONTOUCH_DEBUG("Saturation is already at minimum level\n");
            }
            break;
        default:
            break;
    }
    return false; // 返回false，事件未处理
}

// 注册增加饱和度按钮的触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(SATURATION_INCREASE_BTN) // 这里的SATURATION_INCREASE_BTN是增加饱和度按钮的控件ID
.ontouch = screen_saturation_increase_ontouch, // 将增加饱和度触摸处理函数与控件绑定
};

// 注册减少饱和度按钮的触摸事件处理函数
REGISTER_UI_EVENT_HANDLER(SATURATION_DECREASE_BTN) // 这里的SATURATION_DECREASE_BTN是减少饱和度按钮的控件ID
.ontouch = screen_saturation_decrease_ontouch, // 将减少饱和度触摸处理函数与控件绑定
};


// 声明恢复函数
extern void restore_original_contrast(); // 恢复对比度
extern void restore_original_saturation(); // 恢复饱和度
extern void restore_original_settings();


// 静态变量用于跟踪当前的点击状态
static int restore_state = 0; // 0 表示恢复对比度，1 表示恢复饱和度

// 触摸事件处理函数
static int set_dft_pt_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**time set ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // if (restore_state == 0) {
        //     restore_original_contrast(); // 第一次点击恢复对比度
        //     current_contrast_index = 2; // 重置对比度索引
        //     restore_state = 1; // 切换到恢复饱和度状态
        //     UI_ONTOUCH_DEBUG("Restored Contrast. Next click will restore Saturation.\n");
        // } else {
        //     restore_original_saturation(); // 第二次点击恢复饱和度
        //     current_saturation_index = 2; // 重置饱和度索引
        //     restore_state = 0; // 切换回恢复对比度状态
        //     UI_ONTOUCH_DEBUG("Restored Saturation. Next click will restore Contrast.\n");
        // }
        restore_original_settings(RESTORE_SATURATION);
        current_saturation_index = 2; // 重置饱和度索引
        restore_original_settings(RESTORE_CONTRAST);
        current_contrast_index = 2; // 重置对比度索引
        ui_pic_show_image_by_id(LIGHT_4, 2);
        db_update("con", 110); // 更新数据库记录当前对比度索引
        db_flush();
        ui_pic_show_image_by_id(LIGHT_6, 2);
        db_update("sat", 100); // 更新数据库记录当前饱和度索引
        db_flush();
        break;
    default:
        return false;
    }
    return false;
}

// 注册 UI 事件处理
REGISTER_UI_EVENT_HANDLER(BTN_DFT_PT)
.ontouch = set_dft_pt_ontouch,
};
/**********************************图像比例事件***********************************/
// extern void video_set_disp_window_with_aspect_ratio(float aspect_ratio); // 裁剪函数
extern void set_display_window(int aspect_ratio);
extern void set_display_crop(int aspect_ratio);
extern void video_disp_stop(int id);
static int set_aspect_ratio_16_9_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**set aspect ratio to 16:9 ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN - 16:9\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - 16:9\n");
        // 先停止当前显示的窗口
        video_disp_stop(1);  // 停止显示窗口 1（假设在窗口 1 上显示）
        video_disp_stop(2);
        video_disp_stop(3);
        // 设置为 16:9 比例
        // video_set_disp_window_with_aspect_ratio(16.0f / 9.0f);  // 调用裁剪函数，设置16:9
        set_display_window(0); // 设置显示窗口
        set_display_crop(0);  // 设置裁剪 16:9

        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief 注册 16:9 按钮触摸事件
 */
REGISTER_UI_EVENT_HANDLER(BTN_SET_16_9)
.ontouch =  set_aspect_ratio_16_9_ontouch,
};
static int set_aspect_ratio_4_3_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**set aspect ratio to 4:3 ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN - 4:3\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - 4:3\n");
        // 先停止当前显示的窗口
        video_disp_stop(1);  // 停止显示窗口 1（假设在窗口 1 上显示）
        video_disp_stop(2);
        video_disp_stop(3);
        // 设置为 4:3 比例
        // video_set_disp_window_with_aspect_ratio(4.0f / 3.0f);  // 调用裁剪函数，设置4:3
        set_display_window(1); // 设置显示窗口
        set_display_crop(1);  // 设置裁剪 4:3

        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief 注册 4:3 按钮触摸事件
 */
REGISTER_UI_EVENT_HANDLER(BTN_SET_4_3)
.ontouch =  set_aspect_ratio_4_3_ontouch,
};
static int set_aspect_ratio_1_1_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**set aspect ratio to 1:1 ontouch**\n");

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN - 1:1\n");
        break;
    case ELM_EVENT_TOUCH_HOLD:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP - 1:1\n");
        // 先停止当前显示的窗口
        video_disp_stop(1);  // 停止显示窗口 1（假设在窗口 1 上显示）
        video_disp_stop(2);
        video_disp_stop(3);
        // 设置为 1:1 比例
        // video_set_disp_window_with_aspect_ratio(1.0f);  // 调用裁剪函数，设置1:1
        set_display_window(2); // 设置显示窗口
        set_display_crop(2);  // 设置裁剪 1:1

        break;
    default:
        return false;
    }
    return false;
}

/**
 * @brief 注册 1:1 按钮触摸事件
 */
REGISTER_UI_EVENT_HANDLER(BTN_SET_1_1)
.ontouch =  set_aspect_ratio_1_1_ontouch,
};

#endif

