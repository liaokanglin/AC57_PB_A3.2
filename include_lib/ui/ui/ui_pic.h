#ifndef UI_PIC_H
#define UI_PIC_H

#include "ui/ui_core.h"




struct ui_pic {
    struct element elm;
    char index;
    const struct ui_pic_info *info;
    const struct element_event_handler *handler;
};






void *new_ui_pic(const void *_info, struct element *parent); 
// 创建一个新的UI图片对象，并将其添加到指定的父元素中

int ui_pic_show_image_by_id(int id, int index); 
// 根据ID显示图片对象的指定索引的图像

int ui_pic_set_image_index(struct ui_pic *pic, int index); 
// 设置图片对象的图像索引

int ui_pic_rotate_by_id(int id, int index, int angle); 
// 根据ID旋转图片对象中指定索引的图像到指定角度









#endif

