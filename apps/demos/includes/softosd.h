#ifndef __SOFTOSD_H__
#define __SOFTOSD_H__


struct softosd_cfg {
    u32 x;
    u32 y;
    u32 font_w;
    u32 font_h;
    u32 font_color; //YUV color
    u8 *osd_dirt;	//字典索引，必须以0结尾
    u8 *osd_matrix; //字典（点阵偏移 = 索引*宽*高/8）
};



u8 softosd_done(u8 *yuv_buf, u32 width, u32 height, struct softosd_cfg *cfg, char *osd);



#endif
