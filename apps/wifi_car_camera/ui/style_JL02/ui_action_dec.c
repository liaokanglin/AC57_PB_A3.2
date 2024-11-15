#include "ui/includes.h"
#include "ui/ui_battery.h"
#include "ui/ui_slider.h"
#include "system/includes.h"
#include "server/ui_server.h"
#include "style.h"
/* #include "menu_parm_api.h" */
#include "action.h"
#include "app_config.h"
#include "stdlib.h"
#include "video_rec.h"

#ifdef CONFIG_UI_STYLE_JL02_ENABLE

#define STYLE_NAME  JL02

struct replay_info {
    u8 onkey_mod;			/*按键选中组*/
    s8 onkey_sel;			/*按键选中项*/

    u8 type;				/*当前过滤类型*/
    u8 edit;				/*编辑模式*/
    u8 no_file;             /*没有文件 */

    u8 dec_player;			/*播放器显示状态*/
    u8 is_lock;				/*加锁文件*/
    u8 err_file;			/*损坏文件*/
    u8 file_res;    		/*文件分辨率*/
    u8 if_in_rep;           /*正在播放*/
    u8 file_type;           /*文件类型*/
    int dec_player_timer;	/*播放器隐藏计时器*/

    u8 file_dialog;			/*预览对话框内容*/
    u8 file_msg;			/*预览弹窗提示内容*/
    u8 file_timerout_msg;	/*自动隐藏的预览弹窗内容*/
    int file_msg_timer;		/*预览弹窗隐藏计时器*/

    u8 dec_msg;				/*播放弹窗提示内容*/
    u8 dec_timerout_msg;	/*自动隐藏的播放弹窗内容*/
    int dec_msg_timer;		/*播放弹窗隐藏计时器*/

    u8 dec_show_status;     /*界面状态 0:文件列表 1:解码界面  */
    u8 page_exit;           /* 退出页面方式 */

    u8 battery_val;			/*电池电量*/
    u8 battery_charging;	/*电池充电状态*/

    int file_num;			/*当前页文件数*/
    char page_cur[5];		/*当前页码值*/
    char page_tol[5];		/*总页码值*/

    char cur_path[128];		/*当前目录路径*/

    u8 edit_sel[FILE_SHOW_NUM];		/*编辑选中*/

    struct utime sum_time;  /*当前播放的视频的总时间 */
    struct utime cur_time;  /*当前播放的视频的当前时间 */

    struct ui_browser *browser;		/*文件列表控件指针*/
};

static struct replay_info handler;

#define __this 	(&handler)
#define sizeof_this     (sizeof(struct replay_info))
extern int sys_cur_mod;  /* 1:rec, 2:tph, 3:dec */

extern int storage_device_ready();

const static char *cPATH[] = {
    CONFIG_DEC_PATH_1,
    CONFIG_DEC_PATH_2,
};
const static char *cTYPE[] = {
    "-tMOVAVI -sn -d",
    "-tJPG -sn -d",
    "-tMOVJPGAVI -sn -ar -d",//只读文件
};
static int file_tool[] = {
    FILE_PIC_EDIT,
    FILE_BTN_PHOTO,
    FILE_BTN_HOME,
};
const static int file_tool_type[] = {
    FILE_BTN_PHOTO,
    FILE_BTN_LOCK,
    FILE_BTN_VIDEO,
};
const static int file_tool_dir[] = {
    FILE_PIC_BACK,
    FILE_BTN_PREV,
    FILE_BTN_NEXT,
};
static int file_edit_tool[] = {
    FILE_PIC_EDIT,
    FILE_BTN_UNLOCK,
    FILE_BTN_DELETE,
};
const static int dec_tool[] = {
    DEC_PIC_LOCK,
    DEC_BTN_DELETE,
    DEC_BTN_RETURN,
};
enum {
    PAGE_SHOW = 0,  // 页面显示，赋值为 0
    MODE_SW_EXIT,   // 模式切换退出，自动赋值为 1
    HOME_SW_EXIT,   // 主页切换退出，自动赋值为 2
};

enum eFILE_DIALOG {
    NONE = 0,
    DEL_ALL,
    DEL_CUR,
    DEL_PRO_DIR,
    DEL_DIR,
    UNLOCK_ALL,
};
enum eONKEY_MOD {
    ONKEY_MOD_NORMAL = 0,          // 普通模式：默认的操作模式，通常用于基本操作或导航。
    ONKEY_MOD_NORMAL_TOOL,        // 普通工具模式：可能用于普通模式下的工具操作，比如调出工具菜单等。
    ONKEY_MOD_NORMAL_DIR,         // 普通目录模式：表示在普通模式下操作目录（比如文件浏览、文件夹浏览等）。
    ONKEY_MOD_EDIT,               // 编辑模式：用于文本或数据的编辑操作，例如进入编辑状态进行修改。
    ONKEY_MOD_EDIT_TOOL,          // 编辑工具模式：在编辑模式下，使用工具类功能，例如工具栏、快捷操作等。
    ONKEY_MOD_EDIT_DIALOG,        // 编辑对话框模式：表示在编辑过程中，弹出了一个对话框用于输入或修改。
    ONKEY_MOD_PLAY,               // 播放模式：用于播放音频、视频或其他多媒体内容。
    ONKEY_MOD_PLAY_TOOL,          // 播放工具模式：在播放模式下，使用工具或控制播放的功能，比如暂停、快进、音量调整等。
    ONKEY_MOD_PLAY_DIALOG,        // 播放对话框模式：播放过程中弹出的对话框，可能用于设置或选项调整。
};

enum eFILE_MSG {
    // 数字越小，优先级越高，高优先级的消息能够替换低优先级消息的提示
    // 这需要 UI 布局工具修改对应的消息显示顺序
    FILE_MSG_NONE = 0,           // 无消息
    FILE_MSG_POWER_OFF,         // 电源关闭
    FILE_MSG_NO_POWER,          // 电量不足
    FILE_MSG_NO_FILE,           // 没有文件
    FILE_MSG_DEL_FILE,          // 文件删除
    FILE_MSG_UNLOCK_FILE,       // 解锁文件
    FILE_MSG_LOCK_FILE,         // 锁定文件
    FILE_MSG_LOCKED_FILE,       // 文件已锁定
};

enum eDEC_MSG {
    //数字越小，优先级越高，高优先级的消息能替换低优先级消息的提示
    DEC_MSG_NONE = 0,
    DEC_MSG_POWER_OFF,
    DEC_MSG_NO_POWER,
    DEC_MSG_ERR_FILE,
    DEC_MSG_LOCKED_FILE,
    DEC_MSG_DEL_FILE,
};
enum eDIR {
    eDIR_FRONT = 0,
    eDIR_BACK,
};
enum eTYPE {
    eTYPE_VIDEO = 0,
    eTYPE_PHOTO,
    eTYPE_LOCK,
};





static void file_msg_show(int msg)
{
    if (__this->file_msg > msg) {
        __this->file_msg = msg;
        ui_text_show_index_by_id(FILE_TXT_MESSAGEBOX, __this->file_msg - 1);
    } else if (__this->file_msg == FILE_MSG_NONE) {
        __this->file_msg = msg;
        ui_show(FILE_LAY_MESSAGEBOX);
    }
}
static void file_msg_hide(int msg)
{
    if (__this->file_msg == msg) {
        __this->file_msg = FILE_MSG_NONE;
        ui_hide(FILE_LAY_MESSAGEBOX);
    }
}
static void file_msg_timeout()
{
    file_msg_hide(__this->file_timerout_msg);
    __this->file_msg_timer = 0;
    __this->file_timerout_msg = FILE_MSG_NONE;
}
static void file_msg_timeout_start(int msg, int ms)
{
    if (!__this->file_timerout_msg) {
        file_msg_show(msg);
        __this->file_timerout_msg = msg;
        __this->file_msg_timer = sys_timeout_add(NULL, file_msg_timeout, ms);
    } else if (__this->file_timerout_msg == msg) {
        sys_timeout_del(__this->file_msg_timer);
        __this->file_msg_timer = sys_timeout_add(NULL, file_msg_timeout, ms);
    }
}
static void dec_msg_show(int msg)
{
    if (__this->dec_msg > msg) {
        __this->dec_msg = msg;
        ui_text_show_index_by_id(DEC_TXT_MESSAGEBOX, __this->dec_msg - 1);
    } else if (__this->dec_msg == DEC_MSG_NONE) {
        __this->dec_msg = msg;
        ui_show(DEC_LAY_MESSAGEBOX);
    }
}
static void dec_msg_hide(int msg)
{
    if (__this->dec_msg == msg) {
        __this->dec_msg = DEC_MSG_NONE;
        ui_hide(DEC_LAY_MESSAGEBOX);
    }
}
static void dec_msg_timeout()
{
    dec_msg_hide(__this->dec_timerout_msg);
    __this->dec_msg_timer = 0;
    __this->dec_timerout_msg = DEC_MSG_NONE;
}
static void dec_msg_timeout_start(int msg, int ms)
{
    if (!__this->dec_timerout_msg) {
        dec_msg_show(msg);
        __this->dec_timerout_msg = msg;
        __this->dec_msg_timer = sys_timeout_add(NULL, dec_msg_timeout, ms);
    } else if (__this->dec_timerout_msg == msg) {
        sys_timeout_del(__this->dec_msg_timer);
        __this->dec_msg_timer = sys_timeout_add(NULL, dec_msg_timeout, ms);
    }
}
static int show_file_dialog(const char *cmd)
{
    __this->onkey_mod = ONKEY_MOD_EDIT_DIALOG;
    __this->onkey_sel = 0;
    printf("cmd===%s\n", cmd);
    if (!strcmp(cmd, "unlock_all")) {
        ui_text_show_index_by_id(FILE_TXT_DELETE, 2);
        ui_text_show_index_by_id(FILE_TXT_DELETE_DELETE, 1);
    } else if (!strcmp(cmd, "del_all")) {
        ui_text_show_index_by_id(FILE_TXT_DELETE, 1);
        ui_text_show_index_by_id(FILE_TXT_DELETE_DELETE, 0);
    } else if (!strcmp(cmd, "del_cur")) {
        ui_text_show_index_by_id(FILE_TXT_DELETE, 0);
        ui_text_show_index_by_id(FILE_TXT_DELETE_DELETE, 0);
    } else if (!strcmp(cmd, "del_pro_dir")) {
        ui_text_show_index_by_id(FILE_TXT_DELETE, 3);
        ui_text_show_index_by_id(FILE_TXT_DELETE_DELETE, 0);
    } else if (!strcmp(cmd, "del_dir")) {
        ui_text_show_index_by_id(FILE_TXT_DELETE, 4);
        ui_text_show_index_by_id(FILE_TXT_DELETE_DELETE, 0);
    }
    ui_highlight_element_by_id(FILE_BTN_DELETE_CANCEL);
    return 0;
}
static int hide_file_dialog(void)
{
    puts("hide file dialog\n");
    ui_hide(FILE_LAY_DELETE);
    __this->file_dialog  = 0;
    __this->onkey_mod = ONKEY_MOD_EDIT_TOOL;
    __this->onkey_sel = 1;
    return 0;
}
static int browser_set_dir(int p)
{
    // 设置文件浏览器的当前目录路径
    ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);
    return 0;
}

static void back_to_normal_mode(void)
{
    int i;
    ui_no_highlight_element_by_id(FILE_PIC_EDIT); // 取消高亮 FILE_PIC_EDIT 元素
    ui_hide(file_edit_tool[1]); // 隐藏编辑工具栏中的第二个工具
    ui_show(file_tool_type[__this->type]); // 显示当前类型对应的工具
    ui_hide(FILE_BTN_DELETE); // 隐藏删除按钮
    ui_show(FILE_BTN_HOME); // 显示主页按钮
    __this->edit = 0; // 退出编辑模式
    __this->onkey_sel = 0; // 取消按键选择状态
    __this->onkey_mod = ONKEY_MOD_NORMAL; // 将按键模式设置为正常模式

    if (__this->browser) { // 如果浏览器存在
        for (int i = 0; i < __this->file_num; i++) {
            ui_core_hide((struct element *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_PIC_SEL));
            // 隐藏文件浏览器中每个文件的选择框
        }
    }
}

static void return_prev_path(char *path)
{
    int i, len;
    if (!path) {
        return;
    }
    if (!strcmp(path, CONFIG_DEC_ROOT_PATH)) {
        printf("it's root dir!\n");
        return;
    }
    len = strlen(path) - 1;
    do {
        len--;
    } while (path[len] != '/');
    for (i = len + 1; i < strlen(path) - 1; i++) {
        path[i] = 0;
    }

}
static void goto_next_path(char *path, char *name)
{
    int len;
    if (!path || !name) {
        return;
    }
    strcat(path, name);
    len = strlen(path);
    path[len] = '/';
    path[len + 1] = '\0';

}

int open_file(int p)
{
    struct intent it;
    FILE *fp = ui_file_browser_open_file(__this->browser, p);
    if (fp) {
        printf("sel=%d\n", __this->onkey_sel - 1);
        ui_hide(FILE_WIN);  // 隐藏文件窗口
        init_intent(&it);  // 初始化意图结构体
        it.name = "video_dec";  // 设置意图的名称为 "video_dec"
        it.action = ACTION_VIDEO_DEC_OPEN_FILE;  // 设置意图的动作为打开视频文件
        it.data = (const char *)fp;  // 设置意图的数据指向文件指针
        it.exdata = (int)__this->cur_path;  // 设置意图的扩展数据为当前路径
        start_app_async(&it, NULL, NULL);  // 异步启动应用程序
        __this->onkey_mod = ONKEY_MOD_PLAY;  // 设置按键模式为播放模式
        ui_show(DEC_WIN);  // 显示解码窗口
    }
    return 0;
}

static u8 is_dir_protect(u8 *dir_path)
{
    u8 cur_path[128];
    u8 fname[MAX_FILE_NAME_LEN];
    struct vfscan *fs;
    int attr;
    FILE *file;
    printf("dir_path=%s\n", dir_path);
    fs = fscan((char *)dir_path, "-tMOVJPG -sn -ar -d");

    if (!fs) {
        puts("folder open failed!\n");
        return 0;
    }

    file = fselect(fs, FSEL_FIRST_FILE, 0);
    while (file) {
        fget_attr(file, &attr);
        if (attr & F_ATTR_RO) {
            fclose(file);
            file = NULL;
            fscan_release(fs);
            return 1;
        } else if (attr & F_ATTR_DIR) {
            strcpy((char *)cur_path, (char *)dir_path);
            fget_name(file, fname, MAX_FILE_NAME_LEN);
            goto_next_path((char *)cur_path, (char *)fname);
            if (is_dir_protect(cur_path) == 1) {
                fclose(file);
                file = NULL;
                fscan_release(fs);
                return 1;
            }
        } else {
            fclose(file);
            file = NULL;
        }

        file = fselect(fs, FSEL_NEXT_FILE, 0);

    }
    fscan_release(fs);
    return 0;

}
static int del_sel_file(int del_dir)
{
    puts("del cur file \n");
    struct ui_file_attrs attrs;
    struct ui_text *fname_text;
    struct intent it;
    u8 cur_path[128];
    char itdata[128];
    u8 del_unlocked_file = 0;
    u8 del_cnt = 0;
    back_to_normal_mode();

    for (int i = __this->file_num - 1; i >= 0; i--) {
        if (__this->edit_sel[i]) {
            ui_file_browser_get_file_attrs(__this->browser, i, &attrs);
            if (!(attrs.attr.attr & F_ATTR_RO)) {
                del_unlocked_file = 1;
                file_msg_show(FILE_MSG_DEL_FILE);
                break;
            }
        }
    }
    if (del_unlocked_file == 0) {
        //删除单个加锁文件显示文件已加锁提示
        file_msg_show(FILE_MSG_LOCKED_FILE);
    }

    for (int i = __this->file_num - 1; i >= 0; i--) {
        if (__this->edit_sel[i]) {
            //删除文件
            ui_file_browser_get_file_attrs(__this->browser, i, &attrs);
            if (attrs.ftype == UI_FTYPE_DIR) {
                strcpy((char *)cur_path, __this->cur_path);
                fname_text = (struct ui_text *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_TXT_NAME);
                goto_next_path((char *)cur_path, (char *)fname_text->str);
                if (is_dir_protect(cur_path) == 1 && del_dir == 1) {
                    //提示
                    file_msg_hide(FILE_MSG_DEL_FILE);
                    ui_show(FILE_LAY_DELETE);
                    show_file_dialog("del_pro_dir");
                    __this->file_dialog = DEL_PRO_DIR;
                    sys_touch_event_enable();
                    return 0;
                }
                puts("del dir\n");
                del_cnt++;
                ui_file_browser_del_file(__this->browser, i);
                ui_file_browser_set_all_item_visible(__this->browser);
                ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);//删除文件夹后，手动刷新列表
                continue;
            }
            del_cnt++;
            ui_file_browser_del_file(__this->browser, i);
        }
    }
    if (del_unlocked_file) {
        file_msg_hide(FILE_MSG_DEL_FILE);
    } else {
        os_time_dly(50);
        file_msg_hide(FILE_MSG_LOCKED_FILE);
    }

    if (del_cnt == __this->file_num) {
        //当前页删到无文件,手动刷新列表
        ui_file_browser_set_all_item_visible(__this->browser);
        ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);
        /* ui_file_browser_set_page(__this->browser,ui_file_browser_cur_page(__this->browser, 0)); */
    }

    //刷新页码
    if (ui_file_browser_page_num(__this->browser)) {
        sprintf(__this->page_cur, "%d", ui_file_browser_cur_page(__this->browser, &__this->file_num) + 1);
        sprintf(__this->page_tol, "%d", ui_file_browser_page_num(__this->browser));
        ui_text_set_str_by_id(FILE_TXT_PAGE_CUR, "ascii", __this->page_cur);
        ui_text_set_str_by_id(FILE_TXT_PAGE_TOL, "ascii", __this->page_tol);
        __this->no_file = 0;
    } else {
        __this->no_file = 1;
        __this->file_num = 0;
        strcpy(__this->page_cur, "0");
        strcpy(__this->page_tol, "0");
        ui_hide(FILE_FORM_BRO);
        file_msg_show(FILE_MSG_NO_FILE);
    }
    sys_touch_event_enable();
    return 0;
}
static void del_all_file_ok(void *p, int err)
{
    int i;
    
    // 如果删除操作成功，打印删除成功信息
    if (err == 0) {
        puts("---del_all_file_ok\n");
    } else {
        // 如果删除失败，打印失败信息和错误码
        printf("---del_file_faild: %d\n", err);
    }

    // 恢复到正常模式
    back_to_normal_mode();

    // 隐藏删除文件的提示消息
    file_msg_hide(FILE_MSG_DEL_FILE);

    // 显示文件浏览器中的所有文件项
    ui_file_browser_set_all_item_visible(__this->browser);

    // 重新设置当前目录路径
    ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);

    // 启用触摸事件
    sys_touch_event_enable();
}

static void unlock_all_file_ok(void *p, int err)
{
    int i, file_num;
    struct ui_file_attrs attrs;
    struct intent it;
    u8 tol_page;
    u8 cur_page;
    if (err == 0) {
        puts("---unlock_all_file_ok\n");
        back_to_normal_mode();
        if (__this->type == eTYPE_LOCK) {
            //刷新加锁文件
            browser_set_dir(0);
            file_msg_hide(FILE_MSG_UNLOCK_FILE);
            /* ui_file_browser_set_dir_by_id(FILE_FORM_BRO, cPATH[__this->dir], cTYPE[__this->type]); */
            return;
        }
        //刷新第一页文件图标
        for (i = 0; i < __this->file_num; i++) {
            ui_file_browser_get_file_attrs(__this->browser, i, &attrs);
            attrs.attr.attr &= ~F_ATTR_RO;
            ui_file_browser_set_file_attrs(__this->browser, i, &attrs);
        }
    } else {
        printf("---unlock_file_faild: %d\n", err);
    }
    file_msg_hide(FILE_MSG_UNLOCK_FILE);
    sys_touch_event_enable();
}
static int rep_current_time_handler(const char *type, u32 args)
{
    int sec;
    u32 sum_sec;
    struct utime t;

    if (*type == 's') {
        sec = args;
    } else {
        return 0;
    }

    t.sec = sec % 60;
    t.min = sec / 60 % 60;
    t.hour = sec / 60 / 60;

    __this->cur_time.sec = t.sec;
    __this->cur_time.min = t.min;
    __this->cur_time.hour = t.hour;
    ui_time_update_by_id(DEC_TIM_CUR, &t);
    sum_sec = __this->sum_time.sec + __this->sum_time.min * 60 + __this->sum_time.hour * 60 * 60;
    ui_slider_set_persent_by_id(DEC_SLI, sec * 100 / sum_sec);

    return 0;
}
static int rep_film_length_handler(const char *type, u32 args)
{
    int sec;
    struct utime t;

    if (*type == 's') {
        sec = args;
    } else {
        return 0;
    }

    if (__this->file_type == 1) {
        if (sec) {
            t.sec = sec % 60;
            t.min = sec / 60 % 60;
            t.hour = sec / 60 / 60;
            __this->sum_time.sec = t.sec;
            __this->sum_time.min = t.min;
            __this->sum_time.hour = t.hour;
            ui_time_update_by_id(DEC_TIM_TOL, &t);
        } else {
            t.sec = 1;
            t.min = 0;
            t.hour = 0;
            __this->sum_time.sec = 1;
            __this->sum_time.min = 0;
            __this->sum_time.hour = 0;
            ui_time_update_by_id(DEC_TIM_TOL, &t);
        }
    }
    printf("rep_film_length_handler: %d.\n", args);

    return 0;
}
static void dec_slider_timeout_func(void *priv)
{
    if (__this->if_in_rep) {
        ui_hide(DEC_BTN_PAUSE);
        ui_hide(DEC_LAY_PLAYER);
        __this->dec_player_timer = 0;
        __this->dec_player = 0;
    }
}
static int rep_play_handler(const char *type, u32 args)
{
    puts("rep_play!\n");
    __this->if_in_rep = 1;
    ui_show(DEC_BTN_PAUSE);
    ui_hide(DEC_BTN_PLAY);
    ui_pic_show_image_by_id(DEC_PIC_STATUS, 1);
    if (__this->dec_player_timer) {
        sys_timeout_del(__this->dec_player_timer);
        __this->dec_player_timer = 0;
    }
    __this->dec_player_timer = sys_timeout_add(NULL, dec_slider_timeout_func, 4000);

    return 0;
}
static int rep_pause_handler(const char *type, u32 args)
{
    puts("rep_pause!\n");
    struct utime t;
    struct utime t_tol;

    __this->if_in_rep = 0;
    ui_show(DEC_BTN_PLAY);
    ui_show(DEC_LAY_PLAYER);

    t.sec = __this->cur_time.sec;
    t.min = __this->cur_time.min;
    t.hour = __this->cur_time.hour;
    ui_time_update_by_id(DEC_TIM_CUR, &t);
    t_tol.sec = __this->sum_time.sec;
    t_tol.min = __this->sum_time.min;
    t_tol.hour = __this->sum_time.hour;
    ui_time_update_by_id(DEC_TIM_TOL, &t_tol);
    ui_show(DEC_BTN_PLAY);
    ui_hide(DEC_BTN_PAUSE);
    ui_pic_show_image_by_id(DEC_PIC_STATUS, 0);

    return 0;
}
static int rep_end_handler(const char *type, u32 args)
{
    puts("rep_end!\n");
    __this->if_in_rep = 0;
    ui_show(DEC_BTN_PLAY);
    ui_hide(DEC_BTN_PAUSE);
    ui_pic_show_image_by_id(DEC_PIC_STATUS, 0);

    return 0;
}
static int rep_file_name_handler(const char *type, u32 args)
{
    const char *fname = (const char *)args;
    const char *str_p;

    puts("rep_file_name_handler\n");

    if (!strcmp(type, "ascii")) {
        str_p = (const char *)(args + strlen((char *)args) - 3);
        if (!strcmp(str_p, "JPG") || !strcmp(str_p, "jpg")) {
            __this->file_type = 0;
            ui_hide(DEC_LAY_PLAYER);
            ui_hide(DEC_BTN_PLAY);
        } else {
            __this->file_type = 1;
            ui_show(DEC_LAY_PLAYER);
            ui_show(DEC_BTN_PLAY);

        }
        __this->dec_player = 1;
        __this->if_in_rep = 0;
        __this->no_file = 0;
        __this->err_file = 0;
        ui_text_set_str_by_id(DEC_TXT_FILENAME, "ascii", (const char *)args);
    } else if (!strcmp(type, "utf16")) {

    }
    return 0;
}
static int rep_file_attribute_handler(const char *type, u32 read_only)
{
    puts("rep_file_attribute_handler \n");
    if (read_only) {
        __this->is_lock = 1;
        ui_pic_show_image_by_id(DEC_PIC_LOCK, 1);
    } else {
        __this->is_lock = 0;
        ui_pic_show_image_by_id(DEC_PIC_LOCK, 0);
    }

    return 0;
}
static int rep_file_res_handler(const char *type, u32 args)
{
    if (*type == 'w') {
        printf("rep_file_res_handler w: %d.\n", args);
        if (__this->file_type == 1) {
            if (args >= 1920) {
                /* 1080p */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "1080P 1920x1080");
                __this->file_res = 0;
            } else if (args >= 1080) {
                /* 720p */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "720P 1280x720");
                __this->file_res = 1;
            } else {
                /* vga */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "VGA 640x480");
                __this->file_res = 2;
            }
        } else if (__this->file_type == 0) {
            if (args >= 4032) {
                /* 12M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "12M 4032x3024");
                __this->file_res = 10;
            } else if (args >= 3648) {
                /* 10M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "10M 3648x2736");
                __this->file_res = 9;
            } else if (args >= 3264) {
                /* 8M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "8M 3264x2448");
                __this->file_res = 8;
            } else if (args >= 2592) {
                /* 5M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "5M 2599x1944");
                __this->file_res = 7;
            } else if (args >= 2048) {
                /* 3M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "3M 2048x1536");
                __this->file_res = 6;
            } else if (args >= 1920) {
                /* 2M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "2M 1920x1080");
                __this->file_res = 5;
            } else if (args >= 1280) {
                /* 1.3M */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "1M 1280x960");
                __this->file_res = 4;
            } else {
                /* vga */
                ui_text_set_str_by_id(DEC_TXT_RESOLUTION, "ascii", "VGA 640x480");
                __this->file_res = 3;
            }
        }
    }

    return 0;
}
static int rep_no_file_handler(const char *type, u32 args)
{
    struct intent it;
    int cur_page;
    printf("rep_no_file_handler.\n"); // 打印处理器调用的调试信息
    __this->no_file = 1; // 设置 no_file 标志为 1，表示没有文件
    ui_hide(DEC_WIN); // 隐藏解码窗口
    ui_show(FILE_WIN); // 显示文件窗口

    return 0;
}

static int rep_file_err_handler(const char *type, u32 args)
{
    puts("rep_file_err_handler \n");
    if (__this->dec_player) {
        ui_hide(DEC_LAY_PLAYER);
        ui_hide(DEC_BTN_PLAY);
        if (__this->dec_player_timer) {
            sys_timeout_del(__this->dec_player_timer);
            __this->dec_player_timer = 0;
        }
        __this->dec_player = 0;
    }
    dec_msg_show(DEC_MSG_ERR_FILE);
    __this->err_file = 1;
    __this->if_in_rep = 0;
    return 0;
}
static void sd_event_handler(struct sys_event *event, void *priv)
{
    puts("sd_event_handler\n");  // 输出调试信息，标识事件处理函数被调用

    // 判断事件涉及的设备是否为 sd0、sd1 或 sd2
    if (!strcmp(event->arg, "sd0") || !strcmp(event->arg, "sd1") || !strcmp(event->arg, "sd2")) {
        switch (event->u.dev.event) {  // 根据事件类型执行不同的操作
        case DEVICE_EVENT_IN:  // 设备插入事件
            puts("dev event online\n");  // 输出调试信息，设备插入
            file_msg_hide(FILE_MSG_NO_FILE);  // 隐藏“没有文件”信息
            strcpy(__this->cur_path, CONFIG_DEC_ROOT_PATH);  // 设置当前路径为根目录
            if (__this->onkey_mod == ONKEY_MOD_NORMAL) {  // 判断是否为普通模式
                __this->onkey_sel = 0;  // 设置选中的文件为0
            }
            ui_show(FILE_FORM_LAY);  // 显示文件操作界面
            break;

        case DEVICE_EVENT_OUT:  // 设备拔出事件
            puts("dev event offline\n");  // 输出调试信息，设备拔出
            ui_hide(FILE_FORM_LAY);  // 隐藏文件操作界面
            if (__this->file_dialog) {  // 如果文件对话框存在
                hide_file_dialog();  // 隐藏文件对话框
            }
            file_msg_show(FILE_MSG_NO_FILE);  // 显示“没有文件”信息
            // 页数清0
            strcpy(__this->page_cur, "0");  // 设置当前页为0
            strcpy(__this->page_tol, "0");  // 设置总页数为0
            ui_text_set_str_by_id(FILE_TXT_PAGE_CUR, "ascii", __this->page_cur);  // 更新当前页数显示
            ui_text_set_str_by_id(FILE_TXT_PAGE_TOL, "ascii", __this->page_tol);  // 更新总页数显示
            if (__this->edit) {  // 如果当前处于编辑模式
                back_to_normal_mode();  // 切换回正常模式
            }
            break;

        default:  // 如果事件类型不匹配
            break;
        }
    }
}

static void del_file_callback(void *priv, int err)
{
    /* printf("del_file_callback: err=%d\n", err); */
    if (err == 0) { // 如果删除文件操作成功
        puts("---del_file_ok\n"); // 打印删除成功的提示信息
        ui_hide(DEC_WIN); // 隐藏解码窗口
        ui_show(FILE_WIN); // 显示文件窗口
    } else { // 如果删除文件操作失败
        printf("---del_file_faild: %d\n", err); // 打印失败的错误码
    }
}

static const struct uimsg_handl rep_msg_handler[] = {
    { "fname",     rep_file_name_handler     },
    /* { "fnum",      rep_file_number_handler   }, */
    { "res",       rep_file_res_handler      },
    { "filmLen",   rep_film_length_handler   },
    { "plyTime",   rep_current_time_handler  },
    { "fattr",     rep_file_attribute_handler},
    { "noFile",    rep_no_file_handler       },
    { "fileErr",   rep_file_err_handler      },
    { "play",      rep_play_handler          },
    { "pause",     rep_pause_handler         },
    { "end",       rep_end_handler           },
    /* { "ff",        rep_ff_handler            }, */
    /* { "fr",        rep_fr_handler            }, */
    { NULL, NULL},
};







static void cfun_table_normal(int p)
{
    struct ui_file_attrs attrs;

    // 获取指定索引 p 对应的文件或目录的属性
    ui_file_browser_get_file_attrs(__this->browser, p, &attrs); 

    // 如果该项是一个目录
    if (attrs.ftype == UI_FTYPE_DIR) {
        // 更新到下一路径，路径名为当前路径加上该目录名
        goto_next_path(__this->cur_path, attrs.fname);
        // 设置文件浏览器的目录为更新后的路径
        ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);
        // 将选择的项重置为第一个项目
        __this->onkey_sel = 1;
        // 取消高亮当前被选中的元素
        ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, p, FILE_FORM_VID));
        // 高亮新的第一个元素
        ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, 0, FILE_FORM_VID));
        return; // 结束函数执行
    }

    // 如果该项是一个普通文件
    __this->onkey_mod = ONKEY_MOD_PLAY; // 设置按键模式为播放模式
    __this->onkey_sel = p + 1; // 将选中的项设置为当前项的索引加一

    // 判断文件是否为只读属性
    if (attrs.attr.attr & F_ATTR_RO) {
        __this->is_lock = 1; // 如果是只读文件，锁定状态为 1
    } else {
        __this->is_lock = 0; // 否则锁定状态为 0
    }

    // 设置回调函数，调用 open_file 函数并传入当前索引 p
    ui_set_call(open_file, p);
}

static void cfun_table_edit(int p)
{
    int i;
    struct ui_file_attrs attrs;

    // 高亮当前选择的文件项
    ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, p, FILE_FORM_VID));
    __this->onkey_mod = ONKEY_MOD_EDIT; // 切换到编辑模式
    __this->onkey_sel = p + 1; // 设置当前选择项为 p + 1

    // 切换文件选择状态
    if (__this->edit_sel[p]) {
        __this->edit_sel[p] = 0; // 取消选择
        ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, p, FILE_FORM_PIC_SEL));
    } else {
        __this->edit_sel[p] = 1; // 选择当前文件
        ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, p, FILE_FORM_PIC_SEL));
    }

    // 检查是否有文件可锁定/解锁，更新锁定按钮的显示
    if (file_edit_tool[1] != FILE_BTN_LOCK) {
        // 如果按钮不是锁定按钮，遍历所有文件
        for (i = 0; i < __this->file_num; i++) {
            if (__this->edit_sel[i]) {  // 如果当前文件被选中
                ui_file_browser_get_file_attrs(__this->browser, i, &attrs); // 获取文件属性
                if ((attrs.attr.attr & F_ATTR_RO) == 0) {  // 如果文件不是只读的
                    ui_hide(FILE_BTN_UNLOCK);  // 隐藏解锁按钮
                    ui_show(FILE_BTN_LOCK);    // 显示锁定按钮
                    file_edit_tool[1] = FILE_BTN_LOCK;  // 设置为锁定按钮
                    break;
                }
            }
        }
    } else {
        // 如果当前按钮是锁定按钮，遍历所有文件
        for (i = 0; i < __this->file_num; i++) {
            if (__this->edit_sel[i]) {  // 如果当前文件被选中
                ui_file_browser_get_file_attrs(__this->browser, i, &attrs);  // 获取文件属性
                if ((attrs.attr.attr & F_ATTR_RO) == 0) {  // 如果文件不是只读的
                    break;
                }
            }
        }
        if (i >= __this->file_num) {  // 如果没有文件可以解锁
            ui_hide(FILE_BTN_LOCK);  // 隐藏锁定按钮
            ui_show(FILE_BTN_UNLOCK);  // 显示解锁按钮
            file_edit_tool[1] = FILE_BTN_UNLOCK;  // 设置为解锁按钮
        }
    }
}

static void cfun_file_home()
{
    struct intent it;
    struct application *app;
    init_intent(&it);
    app = get_current_app();
    if (app) {
        it.name = app->name;
        // it.name = "video_dec";
        it.action = ACTION_BACK;
        start_app_async(&it, NULL, NULL); //不等待直接启动app
    }
        init_intent(&it);
        it.name = "video_rec";
        it.action = ACTION_VIDEO_REC_MAIN;
        start_app_async(&it, NULL, NULL);

//    __this->page_exit = HOME_SW_EXIT;
    // file_type_flag=0;
}

static void cfun_file_back()
{
    // 判断当前路径是否为根路径（CONFIG_DEC_ROOT_PATH），如果是，则返回到主目录
    if (!strcmp(__this->cur_path, CONFIG_DEC_ROOT_PATH)) {
        cfun_file_home();  // 调用函数返回到主目录
        return;  // 返回，结束函数执行
    }

    // 如果当前没有文件，则隐藏没有文件的消息，显示浏览器界面，并重置没有文件的标志
    if (__this->no_file) {
        file_msg_hide(FILE_MSG_NO_FILE);  // 隐藏没有文件的提示消息
        ui_show(FILE_FORM_BRO);  // 显示文件浏览器界面
        __this->no_file = 0;  // 重置“没有文件”标志为0，表示当前有文件
    }

    // 设置浏览器的所有项目可见
    ui_file_browser_set_all_item_visible(__this->browser);

    // 返回上一级目录路径
    return_prev_path(__this->cur_path);

    // 根据当前路径（cur_path）和类型（type）设置文件浏览器的显示内容
    ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);
}


static void cfun_file_next()
{
    if (__this->edit) {
        return;
    }
    if (strcmp(__this->page_cur, __this->page_tol)) {
        /* ui_set_call(ui_file_browser_next_page_by_id,FILE_FORM_BRO); */
        ui_file_browser_set_all_item_visible(__this->browser);
        ui_file_browser_next_page_by_id(FILE_FORM_BRO);
        ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, 2, FILE_FORM_VID));
        if (__this->onkey_mod == ONKEY_MOD_NORMAL) {
            __this->onkey_sel = 0;
        }
    }
}
static void cfun_file_prev()
{
    if (__this->edit) {
        return;
    }
    if (strcmp(__this->page_cur, "1") && strcmp(__this->page_cur, "0")) {
        /* ui_set_call(ui_file_browser_prev_page_by_id,FILE_FORM_BRO); */
        ui_file_browser_set_all_item_visible(__this->browser);
        ui_file_browser_prev_page_by_id(FILE_FORM_BRO);
        ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, 1, FILE_FORM_VID));
        if (__this->onkey_mod == ONKEY_MOD_NORMAL) {
            __this->onkey_sel = 0;
        }
    }
}
// 文件编辑模式切换函数
static void cfun_file_edit()
{
    // 检查是否存在没有文件的情况，如果没有文件则直接返回，不进行后续操作
    if (__this->no_file) {
        return;
    }

    // 如果当前已经处于编辑模式
    if (__this->edit) {
        // 将按键模式设置为正常模式
        __this->onkey_mod = ONKEY_MOD_NORMAL;

        // 如果有选中的文件（onkey_sel），将其高亮显示
        if (__this->onkey_sel) {
            ui_highlight_element(
			(struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID)
			);
        }
        back_to_normal_mode();
    } else {
        // 如果当前未处于编辑模式，则切换到编辑模式
        __this->onkey_mod = ONKEY_MOD_EDIT;

        // 同样的，如果有选中的文件（onkey_sel），将其高亮显示
        if (__this->onkey_sel) {
            ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
        }

        // 设置编辑模式标志位为 1，表示现在处于编辑模式
        __this->edit = 1;

        // 高亮显示编辑模式的图标
        ui_highlight_element_by_id(FILE_PIC_EDIT);

        // 隐藏与文件类型相关的工具栏
        ui_hide(file_tool_type[__this->type]);

        // 隐藏“主页”按钮
        ui_hide(FILE_BTN_HOME);

        // 显示“解锁”按钮
        ui_show(FILE_BTN_UNLOCK);

        // 显示“删除”按钮
        ui_show(FILE_BTN_DELETE);

        // 将解锁按钮保存到编辑工具数组的第二个位置
        file_edit_tool[1] = FILE_BTN_UNLOCK;

        // 遍历所有文件，将其取消高亮并隐藏选择标记
        for (int i = 0; i < __this->file_num; i++) {
            ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_PIC_SEL));
            __this->edit_sel[i] = 0;

            // 输出调试信息，表示显示选择图标
            puts("show sel\n");
            ui_core_show((struct element *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_PIC_SEL), false);
        }
    }
}

static void cfun_file_type()
{
    // 将file_tool[1]所对应的元素取消高亮显示
    ui_no_highlight_element_by_id(file_tool[1]);

    // 隐藏当前选择的文件类型
    ui_hide(file_tool_type[__this->type]);

    // 更新文件类型，如果当前类型大于等于2，则循环回到0，否则增加1
    __this->type = __this->type >= 2 ? 0 : __this->type + 1;

    // 将新的文件类型赋值给file_tool[1]
    file_tool[1] = file_tool_type[__this->type];

    // 显示新的文件类型
    ui_show(file_tool_type[__this->type]);

    // 判断存储设备是否准备就绪
    if (storage_device_ready()) {
        // 如果没有文件，显示文件浏览器
        if (__this->no_file) {
            ui_show(FILE_FORM_BRO);
        }

        // 设置文件浏览器的目录（注释掉了原来的ui_set_call函数）
        /* ui_set_call(browser_set_dir,0); */

        // 通过文件类型设置文件浏览器的目录
        ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);

        // 如果当前模式是正常按键模式，重置选择索引
        if (__this->onkey_mod == ONKEY_MOD_NORMAL) {
            __this->onkey_sel = 0;
        }
    }
}

static void cfun_file_dialog()
{
    u8 dir;
    struct intent it;
    __this->onkey_mod = ONKEY_MOD_EDIT_TOOL;
    if (__this->file_dialog == UNLOCK_ALL) {
        hide_file_dialog();
        file_msg_show(FILE_MSG_UNLOCK_FILE);
        init_intent(&it);
        it.name	= "video_dec";
        it.action = ACTION_VIDEO_DEC_SET_CONFIG;
        it.data = "unlock:all";
        /* if (!strcmp(__this->cur_path, CONFIG_DEC_PATH_1)) { */
        /* dir = 0; */
        /* } else { */
        /* dir = 1; */
        /* } */
        /* it.exdata = dir * 10 + __this->type; */
        it.exdata = (int)__this->cur_path;

        /* start_app_async(&it, unlock_all_file_ok, NULL); */
        start_app(&it);
        unlock_all_file_ok(NULL, 0);
    } else if (__this->file_dialog == DEL_CUR) {
        sys_touch_event_disable();
        hide_file_dialog();
        ui_set_call(del_sel_file, 0);
    } else if (__this->file_dialog == DEL_DIR) {
        sys_touch_event_disable();
        hide_file_dialog();
        ui_set_call(del_sel_file, 1);
    } else if (__this->file_dialog == DEL_PRO_DIR) {
        sys_touch_event_disable();
        hide_file_dialog();
        ui_set_call(del_sel_file, -1);
    } else if (__this->file_dialog == DEL_ALL) {
        hide_file_dialog();
        file_msg_show(FILE_MSG_DEL_FILE);
        init_intent(&it);
        it.name	= "video_dec";
        it.action = ACTION_VIDEO_DEC_SET_CONFIG;
        /* static char itdata[128]; */
        /* sprintf(itdata, "delall:%s", __this->cur_path); */
        /* it.data = itdata; */
        /* it.exdata = __this->type; */
        it.data = "del:all";
        it.exdata = (int)__this->cur_path;

        sys_touch_event_disable();
        start_app_async(&it, del_all_file_ok, NULL);
    }
}
static void cfun_file_lock()
{
    u8 cur_path[128];
    u8 fname[MAX_FILE_NAME_LEN];
    struct intent it;
    struct ui_file_attrs attrs;

    for (int i = 0; i < __this->file_num; i++) {
        if (__this->edit_sel[i]) {
            __this->edit_sel[i] = 0;
            ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_PIC_SEL));
            ui_file_browser_get_file_attrs(__this->browser, i, &attrs);
            if (attrs.ftype == UI_FTYPE_DIR) {
                strcpy((char *)cur_path, (char *)__this->cur_path);
                goto_next_path((char *)cur_path, (char *)attrs.fname);
                log_d("lock dir %s", cur_path);
                init_intent(&it);
                it.data = "lock:all";
                it.exdata = (int)cur_path;
                it.name	= "video_dec";
                it.action = ACTION_VIDEO_DEC_SET_CONFIG;
                start_app(&it);
                continue;
            }
            if ((attrs.attr.attr & F_ATTR_RO) == 0) {
                //加锁文件
                attrs.attr.attr |= F_ATTR_RO;
                ui_file_browser_set_file_attrs(__this->browser, i, &attrs);
            }
        }
    }
    ui_no_highlight_element_by_id(file_edit_tool[1]);
    ui_hide(file_edit_tool[1]);
    file_edit_tool[1] = FILE_BTN_UNLOCK;
    ui_show(file_edit_tool[1]);
    if (__this->onkey_mod == ONKEY_MOD_EDIT_TOOL) {
        ui_highlight_element_by_id(file_edit_tool[1]);
    }
}
static void cfun_file_unlock()
{
    u8 dir_only = 1;
    struct ui_file_attrs attrs;
    u8 unlock_all_file_mark = 1;
    if (__this->edit) {
        for (int i = 0; i < __this->file_num; i++) {
            ui_file_browser_get_file_attrs(__this->browser, i, &attrs);
            if (attrs.ftype != UI_FTYPE_DIR) {
                dir_only = 0;
            }
            if (__this->edit_sel[i]) {
                if (attrs.ftype == UI_FTYPE_DIR) {
                    __this->edit_sel[i] = 0;
                    ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_PIC_SEL));
                    continue;
                }
                unlock_all_file_mark = 0;
                if (attrs.attr.attr & F_ATTR_RO) {
                    //解锁文件
                    __this->edit_sel[i] = 0;
                    ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, i, FILE_FORM_PIC_SEL));
                    attrs.attr.attr &= ~F_ATTR_RO;
                    ui_file_browser_set_file_attrs(__this->browser, i, &attrs);
                }
            }
        }
        if (unlock_all_file_mark == 0) {
            if (__this->type == eTYPE_LOCK) {
                //加锁文件刷新
                back_to_normal_mode();
                /* ui_set_call(browser_set_dir,0); */
                ui_file_browser_set_dir_by_id(FILE_FORM_BRO, __this->cur_path, cTYPE[__this->type]);
            }
        }
        if (dir_only) {
            log_d("dir only");
            return;
        }
        if (unlock_all_file_mark) {
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
                __this->onkey_sel = 0;
            }
            puts("\n unlock_all_file \n");
            ui_no_highlight_element_by_id(FILE_BTN_UNLOCK);
            ui_show(FILE_LAY_DELETE);
            ui_set_call((int (*)(int))show_file_dialog, (int)"unlock_all");
            __this->file_dialog = UNLOCK_ALL;
        }
    }
}
static void cfun_file_delete()
{
    u8 dir_only = 1;
    struct ui_file_attrs attrs;
    u8 no_edit_sel = 1;

    ui_no_highlight_element_by_id(FILE_BTN_DELETE);
    if (__this->onkey_sel) {
        ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
        __this->onkey_sel = 0;
    }
    if (__this->edit) {
        if (__this->type == eTYPE_LOCK) {
            //加锁文件类型不删除
            //弹窗
            return;
        }
        for (int i = 0; i < __this->file_num; i++) {
            ui_file_browser_get_file_attrs(__this->browser, i, &attrs);
            if (attrs.ftype != UI_FTYPE_DIR) {
                dir_only = 0;
            }
            if (__this->edit_sel[i]) {
                no_edit_sel = 0;
                if (attrs.ftype == UI_FTYPE_DIR) {
                    continue;
                }
                ui_show(FILE_LAY_DELETE);
                ui_set_call((int (*)(int))show_file_dialog, (int)"del_cur");
                __this->file_dialog = DEL_CUR;
                return;
            }
        }

        if (dir_only) {
            log_d("dir only");
            if (no_edit_sel == 0) {
                //选中了文件夹
                ui_show(FILE_LAY_DELETE);
                ui_set_call((int (*)(int))show_file_dialog, (int)"del_dir");
                __this->file_dialog = DEL_DIR;
            }
            return;
        }

        puts("\n del all file  \n");
        ui_show(FILE_LAY_DELETE);
        ui_set_call((int (*)(int))show_file_dialog, (int)"del_all");
        __this->file_dialog = DEL_ALL;
    }
}


static void cfun_dec_ok()
{
    struct intent it;
    init_intent(&it);
    it.name = "video_dec";
    it.action = ACTION_VIDEO_DEC_CONTROL;
    start_app(&it);
    if (__this->dec_player) {
        if (__this->dec_player_timer) {
            sys_timeout_del(__this->dec_player_timer);
            __this->dec_player_timer = sys_timeout_add(NULL, dec_slider_timeout_func, 4000);
        }
    }
}
static void cfun_dec_ff_fr(u8 ff_fr)
{
    struct intent it;
    init_intent(&it);
    it.name = "video_dec";
    if (ff_fr) {
        it.data = "ff";
    } else {
        it.data = "fr";
    }
    it.action = ACTION_VIDEO_DEC_CONTROL;
    start_app_async(&it, NULL, NULL);
}
static void cfun_dec_lock()
{
    struct intent it;
    if (__this->if_in_rep == 0) {
        if (__this->is_lock) {
            init_intent(&it);
            it.name	= "video_dec";
            it.data = "unlock:cur";
            it.action = ACTION_VIDEO_DEC_SET_CONFIG;
            start_app(&it);
        } else {
            init_intent(&it);
            it.name	= "video_dec";
            it.data = "lock:cur";
            it.action = ACTION_VIDEO_DEC_SET_CONFIG;
            start_app(&it);
        }
    }
}
static void cfun_dec_delete()
{
    if (__this->if_in_rep == 0) {
        if (__this->is_lock) {
            //是加锁文件
            puts("is lock file\n");
            dec_msg_timeout_start(DEC_MSG_LOCKED_FILE, 1000);
            return;
        }
        //弹窗
        __this->onkey_mod = ONKEY_MOD_PLAY_DIALOG;
        __this->onkey_sel = 0;
        ui_show(DEC_LAY_DELETE);
        ui_highlight_element_by_id(DEC_BTN_DELETE_CANCEL);
    }
}
static void cfun_dec_return()
{
    struct intent it;
    if (__this->if_in_rep == 0) { // 判断是否处于特殊状态（例如重复或其他模式），如果是 0 表示不是
        ui_hide(DEC_WIN); // 隐藏解码窗口
        init_intent(&it); // 初始化意图结构体
        it.name = "video_dec"; // 设置意图名称为 "video_dec"
        it.action = ACTION_VIDEO_DEC_CUR_PAGE; // 设置意图的动作为获取当前页面
        start_app(&it); // 启动指定的应用或操作
        ui_show(FILE_WIN); // 显示文件窗口
    }
}

static void cfun_dec_dialog()
{
    struct intent it;
    ui_hide(DEC_LAY_DELETE);
    it.name	= "video_dec";
    it.action = ACTION_VIDEO_DEC_SET_CONFIG;
    it.data = "del:cur";
    start_app_async(&it, del_file_callback, NULL);
}



/*
 * battery事件处理函数
 */
static void battery_event_handler(struct sys_event *event, void *priv)
{

    static u8 ten_sec_off = 0;
    if (ten_sec_off) {
        if (event->type == SYS_KEY_EVENT || event->type == SYS_TOUCH_EVENT) {
            ten_sec_off = 0;
            if (__this->dec_show_status == 0) {
                file_msg_hide(FILE_MSG_POWER_OFF);
            } else {
                dec_msg_hide(DEC_MSG_POWER_OFF);
            }
            return;
        }
    }

    if (event->type == SYS_DEVICE_EVENT) {
        if (!ASCII_StrCmp(event->arg, "sys_power", 9)) {
            if (event->u.dev.event == DEVICE_EVENT_POWER_PERCENT) {
                __this->battery_val = event->u.dev.value;
                if (__this->battery_val > 100) {
                    __this->battery_val = 100;
                }
                if (__this->battery_charging == 0) {
                    /* ui_battery_level_change(__this->battery_val, 0); */
                }
            } else if (event->u.dev.event == DEVICE_EVENT_POWER_CHARGER_IN) {
                /* ui_battery_level_change(100, 1); */
                __this->battery_charging = 1;
                if (ten_sec_off) {
                    ten_sec_off = 0;
                    if (__this->dec_show_status == 0) {
                        file_msg_hide(FILE_MSG_POWER_OFF);
                    } else {
                        dec_msg_hide(DEC_MSG_POWER_OFF);
                    }
                }
            } else if (event->u.dev.event == DEVICE_EVENT_POWER_CHARGER_OUT) {
                /* ui_battery_level_change(__this->battery_val, 0); */
                __this->battery_charging = 0;
                if (__this->dec_show_status == 0) {
                    file_msg_show(FILE_MSG_POWER_OFF);
                } else {
                    dec_msg_show(DEC_MSG_POWER_OFF);
                }
                ten_sec_off = 1;
            }
        }
    }
}
static void dec_no_power_msg_box_timer(void *priv)
{
    static u32 cnt = 0;
    if (__this->battery_val <= 20  && __this->battery_charging == 0) {
        cnt++;
        if ((cnt % 2) == 0) {
            puts("no power show.\n");
            if (__this->dec_show_status == 0) {
                file_msg_show(FILE_MSG_NO_POWER);
            } else {
                dec_msg_show(DEC_MSG_NO_POWER);
            }
        } else {
            puts("no power hide.\n");
            if (__this->dec_show_status == 0) {
                file_msg_hide(FILE_MSG_NO_POWER);
            } else {
                dec_msg_hide(DEC_MSG_NO_POWER);
            }
        }
    } else {
        if (__this->dec_show_status == 0) {
            file_msg_hide(FILE_MSG_NO_POWER);
        } else {
            dec_msg_hide(DEC_MSG_NO_POWER);
        }
        cnt = 0;
    }
}


static int replay_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    static u32 timer_handle = 0;
    static u16 id = 0;
    static u16 id_bat = 0;
    switch (e) {
    case ON_CHANGE_INIT:
        memset(__this, 0, sizeof_this);
        strcpy(__this->cur_path, CONFIG_DEC_ROOT_PATH);
        file_tool[1] = file_tool_type[0];

        ui_register_msg_handler(ID_WINDOW_VIDEO_REP, rep_msg_handler);
        if (id == 0) {
            id = register_sys_event_handler(SYS_DEVICE_EVENT, 0, 0, sd_event_handler);
        }
        id_bat = register_sys_event_handler(SYS_DEVICE_EVENT | SYS_KEY_EVENT | SYS_TOUCH_EVENT, 200, 0, battery_event_handler);
        sys_cur_mod = 3;
        break;
    case ON_CHANGE_FIRST_SHOW:
        __this->battery_val = sys_power_get_battery_persent();
        /* u32 power_level = 0; */
        /* dev_ioctl(fd, POWER_DET_GET_LEVEL, (u32)&power_level); */
        /* __this->battery_val = power_level * 20; */
        if (__this->battery_val > 100) {
            __this->battery_val = 100;
        }
        __this->battery_charging = (usb_is_charging() ? 1 : 0);

        timer_handle = sys_timer_add(NULL, dec_no_power_msg_box_timer, 1000);
        break;
    case ON_CHANGE_RELEASE:
        puts("replay page release\n");
        if (id || id_bat) {
            unregister_sys_event_handler(id);
            unregister_sys_event_handler(id_bat);
            id = 0;
            id_bat = 0;
        }
        if (timer_handle) {
            sys_timer_del(timer_handle);
            timer_handle = 0;
        }
        if (get_parking_status()) {
            break;
        }

        if (__this->page_exit == HOME_SW_EXIT) {
            ui_show(ID_WINDOW_VIDEO_REC);
        }

        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(ID_WINDOW_VIDEO_REP)
.onchange = replay_mode_onchange,
};


static int file_msg_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        if (__this->file_msg) {
            ui_text_show_index_by_id(FILE_TXT_MESSAGEBOX, __this->file_msg - 1);
        }
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_LAY_MESSAGEBOX)
.onchange = file_msg_onchange,
};
static int dec_msg_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        if (__this->dec_msg) {
            ui_text_show_index_by_id(DEC_TXT_MESSAGEBOX, __this->dec_msg - 1);
        }
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_LAY_MESSAGEBOX)
.onchange = dec_msg_onchange,
};
static int dec_right_tool_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:  // 如果是第一次显示
        if (__this->type == eTYPE_PHOTO) { // 如果当前类型是照片
            ui_hide(FILE_BTN_PHOTO); // 隐藏照片按钮
            ui_show(FILE_BTN_LOCK);  // 显示锁定按钮
        } else if (__this->type == eTYPE_LOCK) { // 如果当前类型是锁定
            ui_hide(FILE_BTN_PHOTO); // 隐藏照片按钮
            ui_show(FILE_BTN_VIDEO); // 显示视频按钮
        }
        break;
    default:
        return false;  // 其他情况下返回 false
    }
    return false;  // 返回 false，表示没有其他事件需要处理
}

REGISTER_UI_EVENT_HANDLER(FILE_LAY_TOOL)
.onchange = dec_right_tool_onchange,  // 注册 onchange 事件处理函数
};



static int file_browse_table_onchange(void *ctr, enum element_change_event e, void *arg)
{
    u32 cur_page;
    switch (e) {
    case ON_CHANGE_INIT:
        __this->browser = (struct ui_browser *)ctr;
        __this->browser->order = 1;
        __this->browser->path = __this->cur_path;
        /* __this->browser->path = cPATH[__this->dir]; */
        __this->browser->ftype = cTYPE[__this->type];
        __this->browser->show_mode = 1;
        /* __this->browser->mirror = 1; */
        ASCII_StrToInt(__this->page_cur, &cur_page, strlen(__this->page_cur));
        if (cur_page) {
            /* ui_file_browser_set_page(__this->browser, cur_page - 1); */
            __this->browser->cur_number = (cur_page - 1) * 12;
        }

        break;
    case ON_CHANGE_SHOW:
        __this->browser->grid->auto_hi = false;
        break;
    case ON_CHANGE_SHOW_COMPLETED:
        if (ui_file_browser_page_num(__this->browser)) {
            sprintf(__this->page_cur, "%d", ui_file_browser_cur_page(__this->browser, NULL) + 1);
            sprintf(__this->page_tol, "%d", ui_file_browser_page_num(__this->browser));
            __this->no_file = 0;
            file_msg_hide(FILE_MSG_NO_FILE);
        } else {
            __this->no_file = 1;
            ui_hide(FILE_FORM_BRO);
            strcpy(__this->page_cur, "0");
            strcpy(__this->page_tol, "0");
        }
        ui_text_set_str_by_id(FILE_TXT_PAGE_CUR, "ascii", __this->page_cur);
        ui_text_set_str_by_id(FILE_TXT_PAGE_TOL, "ascii", __this->page_tol);

        if (__this->no_file) {
            __this->file_num = 0;
            file_msg_show(FILE_MSG_NO_FILE);
        } else {
            ui_file_browser_cur_page(__this->browser, &__this->file_num);
            if (__this->onkey_sel && __this->onkey_sel < __this->file_num) {
                ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
        }
        printf("file_num =%d\n", __this->file_num);
        break;
    case ON_CHANGE_RELEASE:
        __this->browser = NULL;
        break;
    default:
        return false;
    }
    return false;
}
static int file_browse_table_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**file_browse table ontouch**");
    u8 select_grid = ui_grid_cur_item(__this->browser->grid); // 获取当前选中的网格项
    int i;
    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        return true; // 触摸移动事件处理
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return false; // 触摸按下事件，未作处理，直接返回 false
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 触摸抬起事件
        if (__this->no_file || select_grid >= __this->file_num) {
            break; // 如果没有文件或者选中的网格超出文件数，则退出
        }
        if (__this->edit) { // 编辑模式
            if (__this->onkey_mod == ONKEY_MOD_EDIT) { // 如果按键模式为编辑
                if (__this->onkey_sel) {
                    ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID)); // 取消高亮显示
                }
            } else if (__this->onkey_mod == ONKEY_MOD_EDIT_TOOL) { // 如果按键模式为编辑工具
                if (__this->onkey_sel > 1) {
                    ui_no_highlight_element_by_id(file_tool[__this->onkey_sel - 1]); // 取消工具栏的高亮显示
                }
            }
            __this->onkey_sel = 0; // 重置选择
            cfun_table_edit(select_grid); // 调用编辑模式函数
        } else { // 普通模式
            if (__this->onkey_mod == ONKEY_MOD_NORMAL) { // 普通模式
                if (__this->onkey_sel) {
                    ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID)); // 取消高亮显示
                }
            } else if (__this->onkey_mod == ONKEY_MOD_NORMAL_TOOL) { // 工具模式
                if (__this->onkey_sel > 1) {
                    ui_no_highlight_element_by_id(file_tool[__this->onkey_sel - 1]); // 取消工具的高亮显示
                }
            } else if (__this->onkey_mod == ONKEY_MOD_NORMAL_DIR) { // 目录模式
                if (__this->onkey_sel) {
                    ui_no_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]); // 取消目录工具的高亮显示
                }
            }
            __this->onkey_sel = 0; // 重置选择
            cfun_table_normal(select_grid); // 调用普通模式函数
        }
        break;
    }
    return true; // 触摸事件处理结束
}

REGISTER_UI_EVENT_HANDLER(FILE_FORM_BRO)
.onchange = file_browse_table_onchange, // 注册 onChange 事件处理
.ontouch = file_browse_table_ontouch,   // 注册 onTouch 事件处理
};


static int file_browse_down_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**file_browse down ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_next();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_NEXT)
.ontouch = file_browse_down_ontouch,
};

static int file_browse_prev_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**file_browse up ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_prev();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_PREV)
.ontouch = file_browse_prev_ontouch,
};



static int dec_dir_return_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_dir_return**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if(!storage_device_ready())
        {
            cfun_file_home();
        }else   if (__this->edit == 0) {
            cfun_file_back();
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_PIC_BACK)
.ontouch = dec_dir_return_ontouch,
};

static int dec_edit_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_edit_ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        return true;  // 触摸按下事件，直接返回 true，表示该事件被处理
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        
        if (!dev_online(SDX_DEV)) {  // 如果设备未连接，跳过后续处理
            break;
        }

        // 根据当前按键模式（`onkey_mod`）处理不同的操作
        if (__this->onkey_mod == ONKEY_MOD_NORMAL || __this->onkey_mod == ONKEY_MOD_EDIT) {
            // 如果当前处于普通模式或编辑模式，取消当前选择项的高亮显示
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
        } else if (__this->onkey_mod == ONKEY_MOD_NORMAL_TOOL) {
            // 如果当前处于工具模式，取消当前工具项的高亮显示
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            }
        } else if (__this->onkey_mod == ONKEY_MOD_NORMAL_DIR) {
            // 如果当前处于目录模式，取消当前目录工具项的高亮显示
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            }
        } else if (__this->onkey_mod == ONKEY_MOD_EDIT_TOOL) {
            // 如果当前处于编辑工具模式，取消当前编辑工具项的高亮显示
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            }
        }
        
        __this->onkey_sel = 0;  // 重置当前选择项
        cfun_file_edit();  // 调用文件编辑函数，执行实际的编辑操作
        break;
    }
    return false;  // 默认返回 false，表示未处理其他事件
}

REGISTER_UI_EVENT_HANDLER(FILE_BTN_EDIT)
.ontouch = dec_edit_ontouch,  // 注册触摸事件处理函数
};

// static int dec_home_ontouch(void *ctr, struct element_touch_event *e)
// {
//     struct intent it; // 定义 intent 结构体，用于表示启动应用的意图
//     struct application *app; // 定义一个指向当前应用的指针
//     UI_ONTOUCH_DEBUG("**dec_home_ontouch**"); // 调试信息：触摸事件函数 dec_home_ontouch 被调用
//     switch (e->event) {
//     case ELM_EVENT_TOUCH_UP: // 处理触摸抬起事件
//         UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n"); // 调试信息：触摸抬起事件发生
//         init_intent(&it);
//     app = get_current_app();
//     if (app) {
//         it.name = "video_dec";
//         it.action = ACTION_BACK;
//         start_app_async(&it, NULL, NULL); //不等待直接启动app
//     }
//         init_intent(&it);
//         it.name = "video_rec";
//         it.action = ACTION_VIDEO_REC_MAIN;
//         start_app_async(&it, NULL, NULL);

//    // __this->page_exit = HOME_SW_EXIT;
//     // file_type_flag=0;
//     // }
//         break;
//     }
//     return false; // 返回 false，表示事件未被完全处理，可能会传递给其他处理器
// }
// REGISTER_UI_EVENT_HANDLER(FILE_BTN_HOME) // 注册触摸事件处理程序到 FILE_BTN_HOME 按钮
// .ontouch = dec_home_ontouch, // 绑定 dec_home_ontouch 函数为触摸事件处理程序
// };

static int dec_home_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_home_ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_home();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_HOME)//返回录像界面
.ontouch = dec_home_ontouch,
};


static int dec_photo_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_photo_ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_type();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_PHOTO)//进入照片按钮
.ontouch = dec_photo_ontouch,
};

static int file_dialog_cancal_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**file dialog cancal ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        hide_file_dialog();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_DELETE_CANCEL)
.ontouch = file_dialog_cancal_ontouch,
};
static int file_dialog_confirm_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**file dialog confirm ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_dialog();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_DELETE_DELETE)
.ontouch = file_dialog_confirm_ontouch,
};

static int dec_lock_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_lock_ontouch**");
    u8 lock_all_file_mark = 1;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if (__this->edit) {
            cfun_file_lock();
        } else {
            cfun_file_type();
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_LOCK)
.ontouch = dec_lock_ontouch,
};

static int dec_unlock_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_unlock_ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_unlock();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_UNLOCK)
.ontouch = dec_unlock_ontouch,
};

static int dec_delete_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_delete_ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_delete();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_DELETE)//删除文件按钮
.ontouch = dec_delete_ontouch,
};

static int file_dialog_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;

    switch (e) {
    case ON_CHANGE_SHOW:
        ui_ontouch_lock(layout); /* 对话框聚焦 */
        break;
    case ON_CHANGE_RELEASE:
        ui_ontouch_unlock(layout);
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_LAY_DELETE)
.onchange = file_dialog_onchange,
};
static int dec_video_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec_video_ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_file_type();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(FILE_BTN_VIDEO)//进入录像界面按钮
.ontouch = dec_video_ontouch,
};

/************************ 解码界面控件动作************************************/
static int dec_dialog_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;

    switch (e) {
    case ON_CHANGE_SHOW:
        ui_ontouch_lock(layout);
        break;
    case ON_CHANGE_RELEASE:
        ui_ontouch_unlock(layout);
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_LAY_DELETE)
.onchange = dec_dialog_onchange,
};

static int dec_lay_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_FIRST_SHOW: // 处理首次显示事件
        if (__this->is_lock) { // 判断当前是否处于锁定状态
            ui_pic_show_image_by_id(DEC_PIC_LOCK, 1); // 如果锁定，显示锁定图标
        } else {
            ui_pic_show_image_by_id(DEC_PIC_LOCK, 0); // 如果未锁定，隐藏锁定图标
        }
        break;
    default:
        return false; // 如果事件未被处理，返回 false
    }
    return false; // 返回 false，表示事件处理完毕或未被处理
}
REGISTER_UI_EVENT_HANDLER(DEC_LAY_TOOL) // 注册 UI 事件处理程序到 DEC_LAY_TOOL 元素
.onchange = dec_lay_onchange, // 绑定 dec_lay_onchange 函数为 onchange 事件处理程序
};

static int dec_play_button_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec play button ontouch**");
    struct intent it;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if (__this->file_type) {
            if (__this->if_in_rep == 0) {
                cfun_dec_ok();
            }
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_PLAY)
.ontouch = dec_play_button_ontouch,
};
static int dec_pause_button_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec pause button ontouch**");
    struct intent it;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        if (__this->file_type) {
            if (__this->if_in_rep == 1) {
                cfun_dec_ok();
            }
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_PAUSE)
.ontouch = dec_pause_button_ontouch,
};
static void show_cur_page_file(void *p, int err)
{
    puts("show cur page file.\n");
    ui_show(FILE_WIN);
}
static int dec_return_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec return ontouch**");
    struct intent it;
    struct application *app;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_dec_return();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_RETURN)
.ontouch = dec_return_ontouch,
};

static int dec_player_layout_onchange(void *ctr, enum element_change_event e, void *arg)
{
    int item, id;
    const char *str = NULL;
    struct intent it;
    struct utime t;
    struct utime t_tol;
    int ret;
    int err;
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        if (__this->if_in_rep) {
            ui_pic_show_image_by_id(DEC_PIC_STATUS, 1);
        } else {
            ui_pic_show_image_by_id(DEC_PIC_STATUS, 0);
        }
        t.sec = __this->cur_time.sec;
        t.min = __this->cur_time.min;
        t.hour = __this->cur_time.hour;
        ui_time_update_by_id(DEC_TIM_CUR, &t);
        t_tol.sec = __this->sum_time.sec;
        t_tol.min = __this->sum_time.min;
        t_tol.hour = __this->sum_time.hour;
        ui_time_update_by_id(DEC_TIM_TOL, &t_tol);
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_LAY_PLAYER)
.onchange = dec_player_layout_onchange,
};
static int dec_layout_button_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec layout button ontouch**");  // 打印调试信息，表明触摸事件处理函数被调用

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:  // 处理触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");  // 打印调试信息，表明触摸事件类型

        if (__this->file_type) {  // 检查文件类型是否存在
            if (__this->if_in_rep == 0) {  // 如果不在重复模式下
                if (__this->dec_player) {  // 如果播放器已经激活
                    ui_hide(DEC_LAY_PLAYER);  // 隐藏播放器界面
                    ui_hide(DEC_BTN_PLAY);  // 隐藏播放按钮
                    if (__this->dec_player_timer) {  // 如果定时器存在
                        sys_timeout_del(__this->dec_player_timer);  // 删除定时器
                        __this->dec_player_timer = 0;  // 重置定时器变量
                    }
                    __this->dec_player = 0;  // 设置播放器状态为关闭
                } else {  // 如果播放器未激活
                    ui_show(DEC_LAY_PLAYER);  // 显示播放器界面
                    ui_show(DEC_BTN_PLAY);  // 显示播放按钮
                    __this->dec_player = 1;  // 设置播放器状态为激活
                }
            } else {  // 如果在重复模式下
                if (__this->dec_player) {  // 如果播放器已经激活
                    ui_hide(DEC_LAY_PLAYER);  // 隐藏播放器界面
                    ui_hide(DEC_BTN_PAUSE);  // 隐藏暂停按钮
                    if (__this->dec_player_timer) {  // 如果定时器存在
                        sys_timeout_del(__this->dec_player_timer);  // 删除定时器
                        __this->dec_player_timer = 0;  // 重置定时器变量
                    }
                    __this->dec_player = 0;  // 设置播放器状态为关闭
                } else {  // 如果播放器未激活
                    ui_show(DEC_LAY_PLAYER);  // 显示播放器界面
                    ui_show(DEC_BTN_PAUSE);  // 显示暂停按钮
                    if (__this->dec_player_timer) {  // 如果定时器存在
                        sys_timeout_del(__this->dec_player_timer);  // 删除定时器
                        __this->dec_player_timer = 0;  // 重置定时器变量
                    }
                    // 设置新的定时器，4秒后调用 dec_slider_timeout_func 函数
                    __this->dec_player_timer = sys_timeout_add(NULL, dec_slider_timeout_func, 4000);
                    __this->dec_player = 1;  // 设置播放器状态为激活
                }
            }
        }
        break;
    }

    return false;  // 返回 false 表示事件未被完全处理
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_SHOW)
.ontouch = dec_layout_button_ontouch,
};

static int dec_lock_unlockfile_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec lock or unlock file ontouch**");
    struct intent it;
    struct application *app;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_dec_lock();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_LOCK)
.ontouch = dec_lock_unlockfile_ontouch,
};
static int dec_delfile_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec del file ontouch**");
    struct intent it;
    struct application *app;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_dec_delete();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_DELETE)
.ontouch = dec_delfile_ontouch,
};
static int dec_del_cancal_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**dec del cancal ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        ui_hide(DEC_LAY_DELETE);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_DELETE_CANCEL)
.ontouch = dec_del_cancal_ontouch,
};

static int dec_del_confirm_ontouch(void *ctr, struct element_touch_event *e)
{
    struct intent it;
    UI_ONTOUCH_DEBUG("**dec del confirm ontouch**");
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");
        cfun_dec_dialog();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_BTN_DELETE_DELETE)
.ontouch = dec_del_confirm_ontouch,
};












static int file_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {  // 根据不同的事件类型执行不同的操作
    case ON_CHANGE_FIRST_SHOW:  // 窗口首次显示时触发的事件
        sys_key_event_takeover(true, false);  // 接管按键事件，开始处理按键输入
        __this->onkey_mod = ONKEY_MOD_NORMAL;  // 设置当前按键模式为普通模式
        __this->file_msg = FILE_MSG_NONE;  // 重置文件消息
        __this->dec_show_status = 0;  // 重置解码显示状态
        break;

    case ON_CHANGE_SHOW_COMPLETED:  // 窗口显示完成时触发的事件
        ui_core_element_on_focus(ui_core_get_element_by_id(FILE_WIN), true);  // 获取文件窗口元素并设置为聚焦状态
        if (!storage_device_ready()) {  // 判断存储设备是否准备好（是否有 SD 卡）
            puts("no sd card!\n");  // 如果没有 SD 卡，输出提示信息
            ui_hide(FILE_FORM_LAY);  // 隐藏文件表单界面
        }
        break;

    case ON_CHANGE_RELEASE:  // 窗口释放时触发的事件
    case ON_CHANGE_HIDE:  // 窗口隐藏时触发的事件
        if (__this->file_msg) {  // 如果有文件消息
            if (__this->file_msg_timer) {  // 如果定时器存在
                sys_timeout_del(__this->file_msg_timer);  // 删除定时器
                __this->file_msg_timer = 0;  // 重置定时器
                __this->file_timerout_msg = FILE_MSG_NONE;  // 重置超时消息
            }
            file_msg_hide(__this->file_msg);  // 隐藏当前的文件消息
        }
        ui_core_element_on_focus(ui_core_get_element_by_id(FILE_WIN), false);  // 取消文件窗口元素的聚焦状态
        break;

    default:  // 其他事件类型不处理
        return false;
    }

    return false;
}

static int file_onkey(void *ctr, struct element_key_event *e)
{
    struct intent it;
    struct application *app;
    struct ui_file_attrs attrs;
    FILE *fp;
    if (e->event == KEY_EVENT_LONG && e->value == KEY_POWER) {
        ui_hide(ui_get_current_window_id());
        sys_key_event_takeover(false, true);           
        return true;
    }

    if (e->event != KEY_EVENT_CLICK) {
        return false;
    }
    if (__this->onkey_mod == ONKEY_MOD_NORMAL) {
        //预览模式中间窗口
        switch (e->value) {
        case KEY_UP:
        case KEY_LEFT:
            if (__this->file_num == 0) {
                __this->onkey_sel = 0;
                break;
            }
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
            __this->onkey_sel = __this->onkey_sel <= 1 ? __this->file_num : __this->onkey_sel - 1;
            ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            break;
        case KEY_DOWN:
        case KEY_RIGHT:
            if (__this->file_num == 0) {
                __this->onkey_sel = 0;
                break;
            }
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
            __this->onkey_sel = __this->onkey_sel >= __this->file_num ? 1 : __this->onkey_sel + 1;
            ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            break;
        case KEY_OK:
            // if (__this->file_num == 0 || __this->onkey_sel == 0) {
            //     __this->onkey_sel = 0;
            //     break;
            // }
            // if (__this->onkey_sel == 0) {
            //     break;
            // }
            // /* ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID)); */
            // cfun_table_normal(__this->onkey_sel - 1);
            // break;
        case KEY_MODE:
            //MODE键切换到工具栏
            if (__this->onkey_sel && !__this->no_file) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
            __this->onkey_mod = ONKEY_MOD_NORMAL_TOOL;
            __this->onkey_sel = 1;
            ui_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            break;
        default:
            return false;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_NORMAL_TOOL) {
        switch (e->value) {
        case KEY_UP:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel <= 1 ? 3 : __this->onkey_sel - 1;
            ui_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            break;
        case KEY_DOWN:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel >= 3 ? 1 : __this->onkey_sel + 1;
            ui_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            break;
        case KEY_OK:
            // switch (__this->onkey_sel) {
            // case 1:
            //     __this->onkey_sel = 1;
            //     cfun_file_edit();
            //     break;
            // case 2:
            //     cfun_file_type();
            //     break;
            // case 3:
            //     cfun_file_home();
            //     break;
            // }
            // break;
        case KEY_MODE:
            //MODE键切换到左侧栏
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_mod = ONKEY_MOD_NORMAL_DIR;
            __this->onkey_sel = 1;
            ui_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            break;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_NORMAL_DIR) {
        switch (e->value) {
        case KEY_UP:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel <= 1 ? 3 : __this->onkey_sel - 1;
            ui_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            break;
        case KEY_DOWN:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel >= 3 ? 1 : __this->onkey_sel + 1;
            ui_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            break;
        case KEY_OK:
            // if (__this->edit) {
            //     break;
            // }
            // switch (__this->onkey_sel) {
            // case 1:
            //     cfun_file_back();
            //     break;
            // case 2:
            //     cfun_file_prev();
            //     break;
            // case 3:
            //     cfun_file_next();
            //     break;
            // }
            // break;
        case KEY_MODE:
            //MODE键切换到预览区
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(file_tool_dir[__this->onkey_sel - 1]);
            }
            __this->onkey_mod = ONKEY_MOD_NORMAL;
            __this->onkey_sel = 1;
            if (!__this->no_file) {
                ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            } else {
                __this->onkey_sel = 0;
            }
            break;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_EDIT) {
        switch (e->value) {
        case KEY_UP:
        case KEY_LEFT:
            if (__this->file_num == 0) {
                __this->onkey_sel = 0;
                break;
            }
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
            __this->onkey_sel = __this->onkey_sel <= 1 ? __this->file_num : __this->onkey_sel - 1;
            ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            break;
        case KEY_DOWN:
        case KEY_RIGHT:
            if (__this->file_num == 0) {
                __this->onkey_sel = 0;
                break;
            }
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
            __this->onkey_sel = __this->onkey_sel >= __this->file_num ? 1 : __this->onkey_sel + 1;
            ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            break;
        case KEY_OK:
            // cfun_table_edit(__this->onkey_sel - 1);
            break;
        case KEY_MODE:
            if (__this->onkey_sel) {
                ui_no_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            }
            __this->onkey_mod = ONKEY_MOD_EDIT_TOOL;
            __this->onkey_sel = 1;
            ui_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            break;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_EDIT_TOOL) {
        switch (e->value) {
        case KEY_UP:
            if (__this->onkey_sel > 1) {
                ui_no_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel <= 1 ? 3 : __this->onkey_sel - 1;
            ui_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            break;
        case KEY_DOWN:
            if (__this->onkey_sel > 1) {
                ui_no_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel >= 3 ? 1 : __this->onkey_sel + 1;
            ui_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            break;
        case KEY_OK:
            // switch (__this->onkey_sel) {
            // case 1:
            //     __this->onkey_sel = 1;
            //     cfun_file_edit();
            //     ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            //     break;
            // case 2:
            //     if (file_edit_tool[1] == FILE_BTN_LOCK) {
            //         cfun_file_lock();
            //     } else {
            //         cfun_file_unlock();
            //     }
            //     break;
            // case 3:
            //     cfun_file_delete();
            //     break;
            // }
            // break;
        case KEY_MODE:
            __this->onkey_mod = ONKEY_MOD_EDIT;
            if (__this->onkey_sel > 1) {
                ui_no_highlight_element_by_id(file_edit_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = 1;
            ui_highlight_element((struct element *)ui_file_browser_get_child_by_id(__this->browser, __this->onkey_sel - 1, FILE_FORM_VID));
            break;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_EDIT_DIALOG) {
        switch (e->value) {
        case KEY_UP:
        case KEY_DOWN:
            if (__this->onkey_sel) {
                ui_highlight_element_by_id(FILE_BTN_DELETE_CANCEL);
                ui_no_highlight_element_by_id(FILE_BTN_DELETE_DELETE);
                __this->onkey_sel = 0;
            } else {
                ui_highlight_element_by_id(FILE_BTN_DELETE_DELETE);
                ui_no_highlight_element_by_id(FILE_BTN_DELETE_CANCEL);
                __this->onkey_sel = 1;
            }
            break;
        case KEY_OK:
            // if (__this->onkey_sel) {
            //     cfun_file_dialog();
            // } else {
            //     hide_file_dialog();
            // }
            break;
        case KEY_MODE:
            hide_file_dialog();
            break;
        }
    }

    return true;
}
REGISTER_UI_EVENT_HANDLER(FILE_WIN)
.onchange = file_onchange,
 .onkey = file_onkey,
};


static int dec_onchange(void *ctr, enum element_change_event e, void *arg)
{
    static u8 file_num;
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:
        printf("dec_onchange\n");
        sys_key_event_takeover(true, false);
        file_num = __this->onkey_sel;
        __this->onkey_mod = ONKEY_MOD_PLAY;
        __this->onkey_sel = 0;
        __this->dec_msg = DEC_MSG_NONE;
        __this->dec_show_status = 1;
        __this->cur_time.sec = 0;
        __this->cur_time.min = 0;
        __this->cur_time.hour = 0;
        break;
    case ON_CHANGE_SHOW_COMPLETED:
        ui_core_element_on_focus(ui_core_get_element_by_id(DEC_WIN), true);
        break;
    case ON_CHANGE_HIDE:
        if (__this->dec_msg) {
            if (__this->dec_msg_timer) {
                sys_timeout_del(__this->dec_msg_timer);
                __this->dec_msg_timer = 0;
                __this->dec_timerout_msg = DEC_MSG_NONE;
            }
            dec_msg_hide(__this->dec_msg);
        }
        ui_core_element_on_focus(ui_core_get_element_by_id(DEC_WIN), false);
        __this->onkey_sel = file_num;
        __this->onkey_mod = ONKEY_MOD_NORMAL;
        break;
    default:
        return false;
    }
    return false;
}
static int dec_onkey(void *ctr, struct element_key_event *e)
{
    struct intent it;
    struct application *app;
    struct ui_file_attrs attrs;
    FILE *fp;
    if (e->event == KEY_EVENT_LONG && e->value == KEY_POWER) {
        ui_hide(ui_get_current_window_id());
        sys_key_event_takeover(false, true);
        return true;
    }
    if (e->event != KEY_EVENT_CLICK) {
        return false;
    }
    if (__this->onkey_mod == ONKEY_MOD_PLAY) {
        //预览模式中间窗口
        switch (e->value) {
        case KEY_UP:
            if (__this->if_in_rep == 1) {
                cfun_dec_ff_fr(0);
            }
            break;
        case KEY_DOWN:
            if (__this->if_in_rep == 1) {
                cfun_dec_ff_fr(1);
            }
            break;
        case KEY_RIGHT:
        case KEY_LEFT:
            break;
        case KEY_OK:
            // if (__this->file_type) {
            //     if (__this->dec_player) {
            //         ui_hide(DEC_LAY_PLAYER);
            //         ui_hide(DEC_BTN_PLAY);
            //         if (__this->dec_player_timer) {
            //             sys_timeout_del(__this->dec_player_timer);
            //             __this->dec_player_timer = 0;
            //         }
            //         __this->dec_player = 0;
            //     }
            //     cfun_dec_ok();
            // }
            // break;
        case KEY_MODE:
            if (__this->if_in_rep == 0) {
                __this->onkey_mod = ONKEY_MOD_PLAY_TOOL;
                __this->onkey_sel = 1;
                ui_highlight_element_by_id(dec_tool[__this->onkey_sel - 1]);
            }
            break;
        default:
            return false;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_PLAY_TOOL) {
        switch (e->value) {
        case KEY_UP:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(dec_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel <= 1 ? 3 : __this->onkey_sel - 1;
            ui_highlight_element_by_id(dec_tool[__this->onkey_sel - 1]);
            break;
        case KEY_DOWN:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(dec_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_sel = __this->onkey_sel >= 3 ? 1 : __this->onkey_sel + 1;
            ui_highlight_element_by_id(dec_tool[__this->onkey_sel - 1]);
            break;
        case KEY_OK:
            // switch (__this->onkey_sel) {
            // case 1:
            //     cfun_dec_lock();
            //     break;
            // case 2:
            //     cfun_dec_delete();
            //     break;
            // case 3:
            //     cfun_dec_return();
            //     break;
            // }
            // break;
        case KEY_MODE:
            if (__this->onkey_sel) {
                ui_no_highlight_element_by_id(dec_tool[__this->onkey_sel - 1]);
            }
            __this->onkey_mod = ONKEY_MOD_PLAY;
            __this->onkey_sel = 0;
            break;
        }
    } else if (__this->onkey_mod == ONKEY_MOD_PLAY_DIALOG) {
        switch (e->value) {
        case KEY_UP:
        case KEY_DOWN:
            if (__this->onkey_sel) {
                ui_highlight_element_by_id(DEC_BTN_DELETE_CANCEL);
                ui_no_highlight_element_by_id(DEC_BTN_DELETE_DELETE);
                __this->onkey_sel = 0;
            } else {
                ui_highlight_element_by_id(DEC_BTN_DELETE_DELETE);
                ui_no_highlight_element_by_id(DEC_BTN_DELETE_CANCEL);
                __this->onkey_sel = 1;
            }
            break;
        case KEY_OK:
            // if (__this->onkey_sel) {
            //     cfun_dec_dialog();
            // } else {
            //     ui_hide(DEC_LAY_DELETE);
            //     __this->onkey_mod = ONKEY_MOD_PLAY_TOOL;
            //     __this->onkey_sel = 2;
            // }
            // break;
        case KEY_MODE:
            ui_hide(DEC_LAY_DELETE);
            __this->onkey_mod = ONKEY_MOD_PLAY_TOOL;
            __this->onkey_sel = 2;
            break;
        }
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(DEC_WIN)
.onchange = dec_onchange,
 .onkey = dec_onkey,
};








#endif
