#include "ui/includes.h"
#include "ui/ui_battery.h"
#include "system/includes.h"
#include "server/ui_server.h"
#include "style.h"
#include "action.h"
/* #include "menu_parm_api.h" */
#include "app_config.h"

#ifdef CONFIG_UI_STYLE_JL02_ENABLE
#define STYLE_NAME  JL02

static s8 onkey_sel = 0;

static int onkey_sel_item[3] = {
    USB_BTN_MASS,
    USB_BTN_CAM,
    USB_BTN_REC,
};
static int onkey_sel_item1[3] = {
    USB_TXT_MASS,
    USB_TXT_CAM,
    USB_TXT_REC,
};
/*
 * USB菜单
 */
static const char *table_usb_menu[] = {
    "usb:msd",
    "usb:uvc",
    "usb:rec",
    "\0"
};

/*****************************USB页面回调 ************************************/
static int usb_page_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;  // 获取当前的窗口对象
    int err, item, id;  // 定义变量用于后续使用
    struct intent it;  // 用于启动其他应用的意图结构体
    int ret;  // 返回值变量

    // 根据窗口的变化事件类型来执行相应操作
    switch (e) {
    case ON_CHANGE_INIT:  // 窗口初始化时
        break;

    case ON_CHANGE_RELEASE:  // 窗口释放（关闭）时
//        ui_hide(ID_WINDOW_MAIN_PAGE);  // 隐藏主页面
        break;

    default:
        return false;  // 返回false表示未处理的事件
    }
    return false;
}

// 注册窗口的变化事件处理程序
REGISTER_UI_EVENT_HANDLER(ID_WINDOW_USB_SLAVE)
.onchange = usb_page_onchange,  // 将变化事件处理函数usb_page_onchange与窗口关联
};


/***************************** USB 存储器模式按钮动作 ************************************/
static int usb_mass_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**usb mass ontouch**");  // 打印触摸调试信息，表示触发了USB存储器模式的触摸事件
    struct intent it;  // 定义意图结构体，用于启动应用

    // 根据触摸事件的类型执行操作
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:  // 触摸按下事件
        if (onkey_sel) {  // 如果有按键选项被选中，取消高亮显示
            ui_no_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
            ui_no_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        }
        ui_highlight_element_by_id(USB_TXT_MASS);  // 高亮显示USB存储器文本
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;

    case ELM_EVENT_TOUCH_HOLD:  // 触摸保持事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;

    case ELM_EVENT_TOUCH_MOVE:  // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;

    case ELM_EVENT_TOUCH_UP:  // 触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 切换到U盘模式
        ui_hide(USB_LAY_B);  // 隐藏USB的主界面布局
        ui_pic_show_image_by_id(USB_PIC_MODE, 0);  // 显示USB存储器模式的图片

        init_intent(&it);  // 初始化意图
        it.name = "usb_app";  // 设置应用名称为"usb_app"
        it.action = ACTION_USB_SLAVE_SET_CONFIG;  // 设置意图的动作为配置USB从设备
        it.data = table_usb_menu[0];  // 设置U盘模式的数据
        start_app_async(&it, NULL, NULL);  // 异步启动USB应用
        break;
    }
    return false;  // 返回false表示没有捕获其他事件
}

// 注册USB存储器按钮的触摸事件处理程序
REGISTER_UI_EVENT_HANDLER(USB_BTN_MASS)
.ontouch = usb_mass_ontouch,  // 将触摸事件处理函数usb_mass_ontouch与按钮关联
};

/***************************** USB_PC模式按钮动作 ************************************/
static int usb_cam_ontouch(void *ctr, struct element_touch_event *e)
{
    UI_ONTOUCH_DEBUG("**usb cameraPc ontouch**");
    struct intent it;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        if (onkey_sel) {
            ui_no_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
            ui_no_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        }
        ui_highlight_element_by_id(USB_TXT_CAM);
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
        /*
         * USB摄像头模式
         */
#ifdef CONFIG_VIDEO4_ENABLE
        ui_hide(ID_WINDOW_USB_SLAVE);
#else
        ui_hide(USB_LAY_B);
        // ui_show(USB_LAY_MODE);
        ui_pic_show_image_by_id(USB_PIC_MODE, 1);
#endif

        init_intent(&it);
        it.name	= "usb_app";
        it.action = ACTION_USB_SLAVE_SET_CONFIG;
        it.data = table_usb_menu[1];
        start_app_async(&it, NULL, NULL);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(USB_BTN_CAM)
.ontouch = usb_cam_ontouch,
};
/***************************** USB 录影模式按钮动作 ************************************/
static int usb_rec_ontouch(void *ctr, struct element_touch_event *e)
{
    // 打印触摸调试信息，表明触发了USB录制界面的触摸事件
    UI_ONTOUCH_DEBUG("**usb to rec ontouch**");

    // 定义一个意图结构体，用于后续操作
    struct intent it;

    // 根据触摸事件的类型执行相应的操作
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:  // 触摸按下事件
        // 如果有选中的按键选项，取消其高亮显示
        if (onkey_sel) {
            ui_no_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
            ui_no_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        }
        // 高亮显示USB录制按钮文本
        ui_highlight_element_by_id(USB_TXT_REC);
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_DOWN\n");
        break;

    case ELM_EVENT_TOUCH_HOLD:  // 触摸保持事件（按住不放）
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_HOLD\n");
        break;

    case ELM_EVENT_TOUCH_MOVE:  // 触摸移动事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_MOVE\n");
        break;

    case ELM_EVENT_TOUCH_UP:  // 触摸抬起事件
        UI_ONTOUCH_DEBUG("ELM_EVENT_TOUCH_UP\n");

        // 初始化意图，用于启动USB应用程序
        init_intent(&it);
        it.name = "usb_app";  // 设置意图名称为"usb_app"
        it.action = ACTION_USB_SLAVE_SET_CONFIG;  // 设置意图动作为USB从设备配置
        it.data = table_usb_menu[2];  // 设置意图的数据为录像模式的配置
        start_app_async(&it, NULL, NULL);  // 异步启动应用程序
        break;
    }

    return false;  // 返回false表示未捕获其他事件
}

// 注册USB录制按钮的触摸事件处理程序
REGISTER_UI_EVENT_HANDLER(USB_BTN_REC)
.ontouch = usb_rec_ontouch,  // 关联触摸事件处理函数
};




static int usb_onchange(void *ctr, enum element_change_event e, void *arg)
{
    // 根据界面变化的事件类型进行相应处理
    switch (e) {
    case ON_CHANGE_FIRST_SHOW:  // 当界面首次显示时触发
        printf("usb_onchange\n");

        // 接管系统按键事件，并禁用它们
        sys_key_event_takeover(true, false);

        // 初始化按键选择为0
        onkey_sel = 0;
        break;

    default:
        return false;  // 返回false表示没有处理其他变化事件
    }

    return false;  // 返回false表示未捕获其他事件
}

static int usb_onkey(void *ctr, struct element_key_event *e)
{
    // 检查按键事件是否为点击事件，如果不是点击事件，则不处理
    if (e->event != KEY_EVENT_CLICK) {
        return false;
    }

    // 根据按键的值执行不同的操作
    switch (e->value) {
    case KEY_UP:  // 向上选择
        if (onkey_sel) {  // 如果已经有选中的项目，取消高亮显示
            ui_no_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
            ui_no_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        }
        onkey_sel--;  // 选择上一个选项
        if (onkey_sel < 1) {  // 如果当前选中项小于1，循环回到第3个选项
            onkey_sel = 3;
        }
        // 高亮显示当前选中的项目
        ui_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
        ui_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        break;

    case KEY_DOWN:  // 向下选择
        if (onkey_sel) {  // 如果已经有选中的项目，取消高亮显示
            ui_no_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
            ui_no_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        }
        onkey_sel++;  // 选择下一个选项
        if (onkey_sel > 3) {  // 如果当前选中项大于3，循环回到第1个选项
            onkey_sel = 1;
        }
        // 高亮显示当前选中的项目
        ui_highlight_element_by_id(onkey_sel_item[onkey_sel - 1]);
        ui_highlight_element_by_id(onkey_sel_item1[onkey_sel - 1]);
        break;

    case KEY_OK:  // 确认键
//         if (onkey_sel) {  // 如果有选中的项目
//             struct intent it;  // 定义意图结构体
//             switch (onkey_sel) {
//             case 1:
//                 /*
//                  * 进入U盘模式
//                  */
//                 ui_hide(USB_LAY_B);  // 隐藏USB层界面
//                 ui_pic_show_image_by_id(USB_PIC_MODE, 0);  // 显示U盘模式图片

//                 // 初始化意图并设置为U盘模式
//                 init_intent(&it);
//                 it.name = "usb_app";  // 设置意图的应用名称为usb_app
//                 it.action = ACTION_USB_SLAVE_SET_CONFIG;  // 设置意图动作
//                 it.data = table_usb_menu[0];  // 设置数据为U盘模式配置
//                 start_app_async(&it, NULL, NULL);  // 异步启动应用
//                 break;

//             case 2:
//                 /*
//                  * 进入USB摄像头模式
//                  */
// #ifdef CONFIG_VIDEO4_ENABLE
//                 ui_hide(ID_WINDOW_USB_SLAVE);  // 如果启用了视频功能，隐藏USB从窗口
// #else
//                 ui_hide(USB_LAY_B);  // 否则，隐藏USB层界面
//                 ui_pic_show_image_by_id(USB_PIC_MODE, 1);  // 显示USB摄像头模式图片
// #endif

//                 // 初始化意图并设置为USB摄像头模式
//                 init_intent(&it);
//                 it.name = "usb_app";  // 设置意图的应用名称为usb_app
//                 it.action = ACTION_USB_SLAVE_SET_CONFIG;  // 设置意图动作
//                 it.data = table_usb_menu[1];  // 设置数据为USB摄像头模式配置
//                 start_app_async(&it, NULL, NULL);  // 异步启动应用
//                 break;

//             case 3:
//                 /*
//                  * 进入录像模式
//                  */
//                 // 初始化意图并设置为录像模式
//                 init_intent(&it);
//                 it.name = "usb_app";  // 设置意图的应用名称为usb_app
//                 it.action = ACTION_USB_SLAVE_SET_CONFIG;  // 设置意图动作
//                 it.data = table_usb_menu[2];  // 设置数据为录像模式配置
//                 start_app_async(&it, NULL, NULL);  // 异步启动应用
//                 break;
//             }
//             return true;
//         }
//         break;

    case KEY_MODE:  // 模式切换键
        return true;  // 返回true表示已处理
        break;

    default:
        return false;  // 返回false表示未处理
    }

    return true;  // 返回true表示已处理完按键事件
}

// 注册UI事件处理程序
REGISTER_UI_EVENT_HANDLER(USB_WIN)
.onchange = usb_onchange,  // UI界面变化时的处理函数
.onkey = usb_onkey,  // 按键事件的处理函数
};



#endif
