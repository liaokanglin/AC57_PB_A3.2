

#include "asm/isp_alg.h"
#include "F37_mipi_iq.h"

static u8 gamma_curve[256] = {
    0,   1,   2,   3,   4,   6,   9,  12,  15,  19,  24,  28,  34,  39,  44,  50,
    55,  60,  64,  68,  72,  76,  79,  82,  85,  88,  91,  93,  95,  98, 100, 102,
    104, 106, 108, 109, 111, 113, 114, 116, 118, 119, 121, 122, 124, 125, 127, 128,
    129, 131, 132, 134, 135, 136, 138, 139, 140, 141, 143, 144, 145, 146, 148, 149,
    150, 151, 152, 154, 155, 156, 157, 158, 159, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 176, 177, 178, 179, 180, 180, 181, 182, 183,
    184, 185, 186, 187, 188, 189, 190, 191, 191, 192, 193, 194, 195, 196, 196, 197,
    198, 199, 199, 200, 201, 202, 202, 203, 204, 204, 205, 206, 206, 207, 207, 208,
    209, 209, 210, 210, 211, 211, 212, 212, 213, 213, 214, 214, 215, 215, 216, 216,
    217, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 222, 223, 223, 224, 224,
    225, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 232,
    233, 233, 233, 234, 234, 235, 235, 236, 236, 236, 237, 237, 238, 238, 239, 239,
    239, 240, 240, 240, 241, 241, 242, 242, 242, 243, 243, 243, 244, 244, 244, 245,
    245, 245, 246, 246, 246, 246, 247, 247, 247, 248, 248, 248, 248, 249, 249, 249,
    249, 250, 250, 250, 250, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 253,
    253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255,

};


static s16 hsv_lut[] = {

    0, 512,  //0
    0, 512,  //15
    0, 512,  //30
    0, 512,  //45
    0, 512,  //60
    0, 512,  //75
    0, 512,  //90
    0, 512,  //105
    0, 512,  //120
    0, 512,  //135
    0, 512,  //150
    0, 512,  //165
    0, 512,  //180
    0, 512,  //195
    0, 512,  //210
    0, 512,  //225
    0, 512,  //240
    0, 512,  //255
    0, 512,  //270
    0, 512,  //285
    0, 512,  //300
    0, 512,  //315
    0, 512,  //330
    0, 512   //345

};

static isp_iq_params_t F37_mipi_iq_params = {
    .blc = {
        0, 0, 0, 0,
    },

    .lsc = {
        0, 0, 0, 0, 0, 0, 0, 0,
    },

    .wdr = {0, 0},
    .ltmo = {0, 0, 250},

    .adj = {
        0x100, 0x100, 0x100, 0, 0, 0, 512, 0,
        15, 30, 200, 230, 50, 0 // chroma supress
    },

    .gamma = gamma_curve,


    .ccm = {
        0x100,   0x00,   0x00,   0x00,
        0x00,   0x100,   0x00,   0x00,
        0x00,    0x00,   0x100,   0x00,
    },

    .hsv = {
        0, 0, 256, 5, 25, 1, hsv_lut
    },


    .bnr =   {
        1, 15, 0, 12, 20,
        {384, 384, 256, 127, 127, 127, 127},
        {64, 96, 127, 127, 127, 127, 127}
    },
    .dpc =   {(3 << 4), (4 << 4), (5 << 4)},
    .tnr =   {1, 20, 48, 5, 0, 25, 2, 1023, 1023},
    .nr =    {
        1, 1, 23, 48, 1, 0,
        960, 540, 39, 15, 15 //radial compesation
    },
    .shp =   {
        1, 15, 1, 127, 50, 50, 50, 50, 8, 32, 8, 32, 8, 8, 20, 1, 10,
        {30, 50, 80, 127, 127, 127, 127, 127},
        //{-1, -1, -1, -1, -2, -2, -1, -2, 32},
        //{0, -1, -2, -1, 0, 2, -2, 2, 8},
        {0, 0, 0, 0, 0, 0, -2, 1, 2},
        {0, 0, -2, 0, 0, 1, 0, 0, 2},
        0, 0, 1, 15, 15
    },
    .cnr =   {1, 255, 78},

    .md_wms = {100, 130, 180, 250, 400},
    .md_level = 3,
};

void *F37_mipi_get_iq_params()
{
    return (void *)&F37_mipi_iq_params;
}

