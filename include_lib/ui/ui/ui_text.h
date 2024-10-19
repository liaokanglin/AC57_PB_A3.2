#ifndef UI_TEXT_H
#define UI_TEXT_H

#include "ui/ui_core.h"
#include "ui/control.h"
#include "font/font_all.h"


struct ui_text {
    struct element elm;
    const char *str;
    const char *format;
    int strlen;
    int color;
    char encode;
    char endian;
    u32 flags;
    struct ui_text_attrs attrs;
    const struct ui_text_info *info;
    const struct element_event_handler *handler;
};



void *new_ui_text(const void *_info, struct element *parent); // 创建一个新的用户界面文本元素

int ui_text_show_index_by_id(int id, int index); // 根据 ID 和索引显示特定文本元素

int ui_text_set_index(struct ui_text *text, int index); // 设置指定 ui_text 元素的索引

int ui_text_set_str(struct ui_text *text, const char *format, const char *str, int strlen, u32 flags); // 设置 ui_text 的字符串，支持格式化

int ui_text_set_str_by_id(int id, const char *format, const char *str); // 根据 ID 设置特定文本元素的字符串，支持格式化

int ui_text_set_text_by_id(int id, const char *str, int strlen, u32 flags); // 根据 ID 设置特定文本元素的文本内容

int ui_text_set_textw_by_id(int id, const char *str, int strlen, int endian, u32 flags); // 根据 ID 设置宽字符文本，支持字节序

int ui_text_set_textu_by_id(int id, const char *str, int strlen, u32 flags); // 根据 ID 设置 Unicode 文本内容

void ui_text_set_text_attrs(struct ui_text *text, const char *str, int strlen, u8 encode, u8 endian, u32 flags); // 设置文本的属性，包括编码、字节序等

int ui_text_rotate_by_id(int id, int angle); // 旋转指定 ID 的文本元素到特定角度

void text_release(struct ui_text *text); // 释放 ui_text 元素占用的资源










#endif
