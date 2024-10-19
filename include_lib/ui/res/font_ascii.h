#ifndef FONT_ASCII_H
#define FONT_ASCII_H

#include "typedef.h"
#include "fs/fs.h"

struct ascii_file {
    char format[8];
    char name[32];
    FILE *file;
    u8 font_size;
};

struct ascii_file *font_ascii_find_format(const char *format);
void font_ascii_set_format(struct ascii_file *ascii, const char *name);
struct ascii_file *font_ascii_add_format(const char *format, const char *name);
int font_ascii_init(struct ascii_file *ascii);
int font_ascii_get_pix(struct ascii_file *ascii, char code, u8 *pixbuf, int *height, int *width);
int font_ascii_width_check(struct ascii_file *ascii, const char *str);
int font_ascii_uninit(struct ascii_file *ascii);

#endif

