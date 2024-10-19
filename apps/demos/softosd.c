#if 0
#include "app_config.h"
#include "system/includes.h"
#include "generic/typedef.h"
#include "softosd.h"




u8 softosd_done(u8 *yuv_buf, u32 width, u32 height, struct softosd_cfg *cfg, char *osd)
{
    if (!yuv_buf || !cfg || !osd) {
        log_e("soft osd input err\n");
        return -1;
    }

    u8 *y = yuv_buf;
    u8 *u = y + width * height;
    u8 *v = u + width * height / 4;
    u32 osd_len = strlen(osd);//osd_len = 有多少个字符

    if (!cfg->osd_dirt || !cfg->osd_matrix) {
        log_e("soft osd cfg err\n");
        return -1;
    }

    if ((cfg->x + cfg->font_w * osd_len > width) || (cfg->y + cfg->font_h > height)) {
        //超出范围
        log_e("soft osd override");
        return -1;
    }

    for (int i = 0; i < osd_len; i++) { // i = 第几个字符
        u32 index = 0;
        for (index = 0; index < strlen(cfg->osd_dirt); index++) { //index = 字符在字典的第几个
            if (osd[i] == cfg->osd_dirt[index]) {
                break;
            }
        }

        if (index >= strlen(cfg->osd_dirt)) {
            log_e("unfound osd font %c\n", osd[i]);
            continue;
        }

        u8 *font = cfg->osd_matrix + cfg->font_w * cfg->font_h * index / 8;	//font = 字符在字典的位置
        for (int m = 0; m < cfg->font_h; m++) { //m = 字符内的第几行
            for (int n = 0; n < cfg->font_w; n++) { //n = 字符内的第几列
                if (font[cfg->font_w / 8 * m + n / 8] & BIT(7 - (n % 8))) { //注意采样顺序
                    y[width * (cfg->y + m) + cfg->x + cfg->font_w * i + n] = (u8)((cfg->font_color & 0x00FF0000) >> 16);
                    u[width / 2 * ((cfg->y + m) / 2) + (cfg->x + cfg->font_w * i + n) / 2] = (u8)((cfg->font_color & 0x0000FF00) >> 8);
                    v[width / 2 * ((cfg->y + m) / 2) + (cfg->x + cfg->font_w * i + n) / 2] = (u8)((cfg->font_color & 0x000000FF) >> 0);
                }
            }
        }
    }

    for (int m = 0; m < cfg->font_h; m++) { //m = 字符内的第几行
        flush_dcache(y + width * (cfg->y + m) + cfg->x, cfg->font_w * osd_len);
        flush_dcache(u + width / 2 * (cfg->y + m) / 2 + cfg->x / 2, cfg->font_w / 2 * osd_len);
        flush_dcache(v + width / 2 * (cfg->y + m) / 2 + cfg->x / 2, cfg->font_w / 2 * osd_len);
    }
}




void softosd_test()
{
    u8 *yuv_buf;
    /* video_rec_take_photo(); */
    /* if(get_image_data(&yuv_buf)){ */
    struct softosd_cfg cfg = {
        .x = 320,
        .y = 240,
        .font_w = 16,
        .font_h = 32,
        .font_color = 0xe20095,
        .osd_dirt = osd_str_total,
        .osd_matrix = osd_mimc_str_matrix,
    };
    softosd_done(yuv_buf, 1280, 720, &cfg, "A2022-03-22 15:41:00");
    /* manaul_encode_jpg(yuv_buf, 1280, 720, CONFIG_REC_PATH_0"jpg****.jpg"); */
    /* } */
}


#endif
