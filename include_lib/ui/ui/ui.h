#ifndef UI_CORE_H
#define UI_CORE_H

#include "window.h"
#include "ui_button.h"
#include "ui_grid.h"
#include "ui_time.h"
#include "ui_camera.h"
#include "ui_pic.h"
#include "ui_text.h"
#include "ui_battery.h"
#include "ui_browser.h"

#include <stdarg.h>


struct uimsg_handl {
    const char *msg;
    int (*handler)(const char *type, u32 args);
};

int ui_framework_init(); // 初始化UI框架

int ui_set_style_file(struct ui_style *style); // 设置UI样式文件

int ui_style_file_version_compare(int version); // 比较UI样式文件的版本

int ui_show(int id); // 显示指定ID的UI元素

int ui_hide(int id); // 隐藏指定ID的UI元素

int ui_set_call(int (*func)(int), int param); // 设置UI回调函数及其参数

int ui_event_onkey(struct element_key_event *e); // 处理键盘事件

int ui_event_ontouch(struct element_touch_event *e); // 处理触摸事件

struct element *ui_get_highlight_child_by_id(int id); // 获取指定ID的高亮子元素

int ui_no_highlight_element(struct element *elm); // 取消元素的高亮显示
int ui_no_highlight_element_by_id(int id); // 根据ID取消元素的高亮显示
int ui_highlight_element(struct element *elm); // 设置元素为高亮显示
int ui_highlight_element_by_id(int id); // 根据ID设置元素为高亮显示

int ui_get_current_window_id(); // 获取当前窗口ID

int ui_register_msg_handler(int id, const struct uimsg_handl *handl); // 注册消息处理程序

int ui_message_handler(int id, const char *msg, va_list); // 处理指定ID的UI消息

const char *str_substr_iter(const char *str, char delim, int *iter); // 字符串子串迭代器，根据分隔符提取子串

/*
 * 锁定元素elm之外的区域，所有的触摸消息都发给elm
 */
void ui_ontouch_lock(void *elm); // 锁定触摸消息到指定元素
void ui_ontouch_unlock(void *elm); // 解锁触摸消息，使其不再锁定到指定元素

/*
 * 锁定控件的父图层，先不推向imb显示
 */
int ui_lock_layer(int id); // 锁定控件的父图层，不推送到显示
int ui_unlock_layer(int id); // 解锁控件的父图层，推送到显示


#endif

