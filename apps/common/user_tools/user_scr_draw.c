#include "system/includes.h"
#include "get_image_data.h"

static get_yuv_tp_cfg  __info_tp_ahd;
/*注意user_scr_draw_open_fb函数中void *fb = dev_open("fb1", (void *)(format | (z_order << 16)));决定推屏显示的图
层，公版为”fb0”为UI层数据推在这层帧率稍低，”fb1”为video层，在此图层显示帧率有明显提高。（如下图）*/
void *user_scr_draw_open_fb(u32 format, u32 width, u32 height, u32 x, u32 y)
{
    int err = 0;
    int z_order = 0;

    void *fb = dev_open("fb1", (void *)(format | (z_order << 16)));
    if (!fb) {
        log_e("fb open failed\n\n");
        return 0;
    }

    struct fb_var_screeninfo info = {0};
    info.s_xoffset = x;     //显示区域x坐标
    info.s_yoffset = y;     //显示区域y坐标
    info.s_xres    = width;  //显示区域宽度
    info.s_yres    = height;   //显示区域高度
    info.v_xoffset = 0;     //屏幕的虚拟x坐标
    info.v_yoffset = 0;     //屏幕的虚拟y坐标
    info.v_xres    = width;  //屏幕的虚拟宽度
    info.v_yres    = height;   //屏幕的虚拟高度
    err = dev_ioctl(fb, FBIOSET_VSCREENINFO, (u32)&info);
    if (err) {
        log_e("fb0 open fail:%d\n", err);
        return 0;
    }


    err = dev_ioctl(fb, FBIOSET_FBUFFER_NUM, 1);
    if (err) {
        printf("fb malloc buf fail:%d\n", err);
        return 0;
    }

    err = dev_ioctl(fb, FBIOSET_TRANSP, 0);
    if (err) {
        printf("set transter fail:%d\n", err);
        return 0;
    }

    err = dev_ioctl(fb, FBIOSET_ENABLE, true);
    if (err) {
        printf("set enable fail:%d\n", err);
        return 0;
    }


    return fb;
}


int user_scr_draw_set_pallet(void *fb, u32 format, u32 *color_tab)
{
    int err = 0;

    if ((format == FB_COLOR_FORMAT_256) || (format == FB_COLOR_FORMAT_OSD8A)) {
        if (color_tab) {
            err = dev_ioctl(fb, FBIOSET_PALLET_USE_BUF, (u32)color_tab);
            if (err) {
                printf("set palette fail:%d\n", err);
                return err;
            }
        } else {
            u32 osd_tab_size = 256 * sizeof(u32);
            u32 *osd_tab = (u32 *)malloc(osd_tab_size);

#if 0
            for (int r = 0; r < 6; r++) {
                for (int g = 0; g < 6; g++) {
                    for (int b = 0; b < 6; b++) {
                        osd_tab[r * 36 + g * 6 + b]	= ((r * 0x33) << 16) | ((g * 0x33) << 8) | ((b * 0x33) << 0);
                    }
                }
            }
#else
            u8 i = 1, r = 36, g = 0, b = 0;
            osd_tab[0] = 0;
            for (; i < 36 * 1 + 1; i++) {
                osd_tab[i] = (r * 6 + 39) << 16 | (g++ * 6 + 39) << 8 | (b * 6 + 39);
            }
            for (; i < 36 * 2 + 1; i++) {
                osd_tab[i] = (r-- * 6 + 39) << 16 | (g * 6 + 39) << 8 | (b * 6 + 39);
            }
            for (; i < 36 * 3 + 1; i++) {
                osd_tab[i] = (r * 6 + 39) << 16 | (g * 6 + 39) << 8 | (b++ * 6 + 39);
            }
            for (; i < 36 * 4 + 1; i++) {
                osd_tab[i] = (r * 6 + 39) << 16 | (g-- * 6 + 39) << 8 | (b * 6 + 39);
            }
            for (; i < 36 * 5 + 1; i++) {
                osd_tab[i] = (r++ * 6 + 39) << 16 | (g * 6 + 39) << 8 | (b * 6 + 39);
            }
            for (; i < 36 * 6 + 1; i++) {
                osd_tab[i] = (r * 6 + 39) << 16 | (g * 6 + 39) << 8 | (b-- * 6 + 39);
            }
#endif

            for (int i = 0; i < 39; i++) {
                osd_tab[216 + i] = 0x00060606 * i;
            }
            osd_tab[255] = 0x00FFFFFF;

            err = dev_ioctl(fb, FBIOSET_PALLET_USE_BUF, (u32)osd_tab);
            if (err) {
                printf("set palette fail:%d\n", err);
                return err;
            }
            if (osd_tab) {
                free((void *)osd_tab);
            }
        }
    }
    return 0;
}

int user_scr_draw_getmap(void *fb, struct fb_map_user *map)
{
    int err = 0;
    err = dev_ioctl(fb, FBIOGETMAP, (u32)map);
    return err;
}


int user_scr_draw_putmap(void *fb, struct fb_map_user *map)
{
    int err = 0;
    u32 len = 0;

    switch (map->format) {
    case FB_COLOR_FORMAT_YUV420:
        len = map->width * map->height * 3 / 2;
        break;
    case FB_COLOR_FORMAT_YUV422:
        len = map->width * map->height * 4 / 2;
        break;
    case FB_COLOR_FORMAT_RGB888:
        len = map->width * map->height * 3;
        break;
    case FB_COLOR_FORMAT_RGB565:
        len = map->width * map->height * 2;
        break;
    case FB_COLOR_FORMAT_OSD8A:
        len = map->width * map->height * 2;
        break;
    case FB_COLOR_FORMAT_16K:
        len = map->width * map->height * 2;
        break;
    case FB_COLOR_FORMAT_256:
        len = map->width * map->height;
        break;
    case FB_COLOR_FORMAT_2BIT:
        len = map->width * map->height / 4;
        break;
    case FB_COLOR_FORMAT_1BIT:
        len = map->width * map->height / 8;
        break;
    default:
        log_e("unknow fb format %d", map->format);
        return -1;
    }

    /* flush_dcache(map->baddr, map->width * map->height); */
    dev_ioctl(fb, FBIOPUTMAP, (u32)map);
    return err;
}

void user_scr_draw_close_fb(void *fb)
{
    dev_ioctl(fb, FBIOSET_FBUFFER_NUM, 0);
    dev_close(fb);
}








//推屏部分公版有相对应的接口，所有接口均在user_scr_draw.c；选择推屏的数据模式，#define DISP_MODE 1(公版为2)。（如下图）


#if 1

//demo

#define DISP_MODE 1   //0:OSD8  1:YUV420  2:user

#if (DISP_MODE == 0)
u8 tmp_buf[1280 * 27];
#elif (DISP_MODE == 1)
u8 tmp_buf[1280 * 32 * 3];
#elif (DISP_MODE == 2)
#endif
u8 display_state=0;
void user_scr_draw_point(u8 *dmap, u32 x, u32 y, u32 color)
{
    dmap[y * 256 * 3 + x * 3] = (u8)(color >> 16);
    dmap[y * 256 * 3 + x * 3 + 1] = (u8)(color >> 8);
    dmap[y * 256 * 3 + x * 3 + 2] = (u8)(color >> 0);
}
extern u8 yuv_deal[1280*720*3/2];

void user_scr_draw_test(void *p)
{
    void *fb;
    struct fb_map_user map;

#if (DISP_MODE == 0)
    /***OSD8模式***/
    //准备显示用的数据
    for (int i = 0; i < 27; i++) {
        u8 c = i * 8;
        for (int x = 0; x < 1280; x++) {
            tmp_buf[1280 * i + x] = c;
            if (++c >= 216) {
                c = 1;
            }
        }
        /* for(int x = 1000; x < 1280; x++){ */
        /* tmp_buf[1280 * i +x] = c; */
        /* if(++c <= 216){ */
        /* c = 216;	 */
        /* } */
        /* } */
    }

    //打开显示图层
    fb = user_scr_draw_open_fb(FB_COLOR_FORMAT_256, 1280, 480, 0, 0);
    if (fb) {
        //设置图层颜色表（图层共用，非必须设置）
        user_scr_draw_set_pallet(fb, FB_COLOR_FORMAT_256, 0);
        u8 b = 0;
        while (1) {
            //获取图层显存
            user_scr_draw_getmap(fb, &map);

            if (map.baddr) {
                //填充显存
                if (++b >= 27) {
                    b = 0;
                }

                printf("a = %x, b=%d\n", map.baddr, b);
                for (int y = 0; y < 480; y++) {
                    memcpy(&map.baddr[y * 1280], &tmp_buf[b * 1280], 1280);
                }

                //提交并显示
                user_scr_draw_putmap(fb, &map);
            } else {
                printf("b=x\n");
            }
        }
    }
#elif (DISP_MODE == 1)
    /***YUV420模式***/
    //准备显示用的数据
//    u32 u = 0, v = 0;
//    for (int x = 0; x < 1280 * 2; x++) {
//        tmp_buf[x] = 0x80;
//    }
//    for (int x = 0; x < 1280 / 2 / 4 * 1; x++) {
//        u += 1280 / 4;
//        tmp_buf[1280 * 2 + x] = u / 256;
//        tmp_buf[1280 * 2 + 1280 / 2 + x] = v / 256;
//    }
//    for (int x = 1280 / 2 / 4 * 1; x < 1280 / 2 / 4 * 2; x++) {
//        v += 1280 / 4;
//        tmp_buf[1280 * 2 + x] = u / 256;
//        tmp_buf[1280 * 2 + 1280 / 2 + x] = v / 256;
//    }
//    for (int x = 1280 / 2 / 4 * 2; x < 1280 / 2 / 4 * 3; x++) {
//        u -= 1280 / 4;
//        tmp_buf[1280 * 2 + x] = u / 256;
//        tmp_buf[1280 * 2 + 1280 / 2 + x] = v / 256;
//    }
//    for (int x = 1280 / 2 / 4 * 3; x < 1280 / 2 / 4 * 4; x++) {
//        v -= 1280 / 4;
//        tmp_buf[1280 * 2 + x] = u / 256;
//        tmp_buf[1280 * 2 + 1280 / 2 + x] = v / 256;
//    }
//    u32 b = 0;
//    for (int i = 1; i < 32; i ++) {
//        b += 1280 / 32;
//        //Y
//        memcpy(&tmp_buf[1280 * 3 * i + 0], &tmp_buf[b], 1280 - b);
//        memcpy(&tmp_buf[1280 * 3 * i + 1280 - b], &tmp_buf[0], b);
//        memcpy(&tmp_buf[1280 * 3 * i + 1280], &tmp_buf[1280 + b], 1280 - b);
//        memcpy(&tmp_buf[1280 * 3 * i + 1280 + 1280 - b], &tmp_buf[1280], b);
//        //U
//        memcpy(&tmp_buf[1280 * 3 * i + 1280 * 2], &tmp_buf[1280 * 2 + b / 2], 1280 / 2 - b / 2);
//        memcpy(&tmp_buf[1280 * 3 * i + 1280 * 2 + 1280 / 2 - b / 2], &tmp_buf[1280 * 2], b / 2);
//        //V
//        memcpy(&tmp_buf[1280 * 3 * i + 1280 * 2 + 1280 / 2], &tmp_buf[1280 * 2 + 1280 / 2 + b / 2], 1280 / 2 - b / 2);
//        memcpy(&tmp_buf[1280 * 3 * i + 1280 * 2 + 1280 / 2 + 1280 / 2 - b / 2], &tmp_buf[1280 * 2 + 1280 / 2], b / 2);
//    }

    /* put_buf(tmp_buf, 1280*3); */

    //创建数据推屏的任务，任务执行函数为void user_scr_draw_test(void *p)；
    //此处打开图层显示，参数1需与实际的数据对应，参数2和参数3为显示的宽和高，参数4和参数5为显示区域的起始坐标（如下图）

    /*最后把数据推屏，首先把YUV数据的三个分量拷贝到对应的地方
    具体YUV数据如何储存可以查阅资料https://blog.csdn.net/wkd_007/article/details/141218195
    注意数据处理和数据推屏因为此案例中只用了一个数组实现交互，所以需要等待数据推屏完成后，才能对数组进行重写新数据的操作。所以在代码中有display_state标志位进行重写数据的限制*/
    //打开显示图层
    fb = user_scr_draw_open_fb(FB_COLOR_FORMAT_YUV420, 1280, 720, 0, 40);
    if (fb) {
        u8 b = 0;
        while (1) {
                    if (__info_tp_ahd.exit_state) {
                    __info_tp_ahd.exit_state = 0; // 重置退出状态
        break; // 退出循环
    }
            //获取图层显存
            user_scr_draw_getmap(fb, &map);

            if (map.baddr&&display_state==1) {

//                //填充显存
//                if (++b >= 32) {
//                    b = 0;
//                }
//
//                /* printf("a = %x, b=%d\n",map.baddr,b); */
//                for (int y = 0; y < 480 / 2; y++) {
//                    /* memcpy(&map.yaddr[y * 1280 * 2], &tmp_buf[b * 1280 * 3], 1280 * 2); */
//                    memset(&map.yaddr[y * 1280 * 2], y * 256 * 2 / 480, 1280 * 2);
//                    memcpy(&map.uaddr[y * 1280 / 2], &tmp_buf[b * 1280 * 3 + 1280 * 2], 1280 / 2);
//                    memcpy(&map.vaddr[y * 1280 / 2], &tmp_buf[b * 1280 * 3 + 1280 * 2 + 1280 / 2], 1280 / 2);
//                }

                memcpy(map.yaddr,yuv_deal,1280*720);
                memcpy(map.uaddr,yuv_deal+1280*720,1280*720/4);
                memcpy(map.vaddr,yuv_deal+1280*720*5/4,1280*720/4);
//                memcpy(&map.uaddr,&yuv_deal[1280*480],1280*480/4);
//                memcpy(&map.vaddr,&yuv_deal[1280*480*5/4],1280*480/4);

                //提交并显示
                user_scr_draw_putmap(fb, &map);
                display_state=0;
            } else {
                msleep(1);
                /* printf("b=x\n"); */
            }
        }
    } else {
        log_e("open fb failed\n");
    }
    user_scr_draw_close_fb(fb); // 关闭显示图层
    // while (1) {
    //     os_time_dly(10);
    // }
#elif (DISP_MODE == 2)
    //打开显示图层
    fb = user_scr_draw_open_fb(FB_COLOR_FORMAT_RGB888, 256, 480, 0, 0);
    if (fb) {
        //获取图层显存
        user_scr_draw_getmap(fb, &map);

        if (map.baddr) {
            for (int y = 0; y < 480; y++) {
                for (int x = 0; x < 256; x++) {
                    map.baddr[y * 256 * 3 + x * 3] = 0xff;
                    map.baddr[y * 256 * 3 + x * 3 + 1] = 0xff;
                    map.baddr[y * 256 * 3 + x * 3 + 2] = 0xff;
                }
            }

            //提交并显示
            user_scr_draw_putmap(fb, &map);
        } else {

        }


    } else {
        log_e("open fb failed\n");
    }

    int dx = 100;
    int dy = 100;
    int state = 0;
    while (1) {
        os_time_dly(3);
        //获取图层显存
        user_scr_draw_getmap(fb, &map);

        if (map.baddr) {

            user_scr_draw_point(map.baddr, dx, dy, 0);
            switch (state) {
            case 0:
                if (dx > 150) {
                    state = 1;
                } else {
                    dx ++;
                }
                break;
            case 1:
                if (dy > 150) {
                    state = 2;
                } else {
                    dy ++;
                }
                break;
            case 2:
                if (dx < 100) {
                    state = 3;
                } else {
                    dx --;
                }
                break;
            case 3:
                if (dy < 100) {
                    state = 4;
                } else {
                    dy --;
                }
                break;
            case 4:
                for (dy = 100; dy < 150; dy++) {
                    for (dx = 100; dx < 150; dx++) {
                        user_scr_draw_point(map.baddr, dx, dy, 0x00808000);
                    }
                }
                state = -1;
                break;
            default:
                user_scr_draw_putmap(fb, &map);
                while (1);
            }
            //提交并显示
            user_scr_draw_putmap(fb, &map);
        } else {

        }
    }

#endif
}
#endif

static u8 get_ahd_yuv_tp_init_flag=0;
//首先创建任务获取对应摄像头的YUV数据。（如下图）
//然后在需要使用YUV数据的地方调用get_ahd_yuv_init()函数，传入回调函数，即可获取到对应摄像头的YUV数据。
void get_ahd_yuv_tp_init(void (*cb)(u8 *data)) // 获取对应摄像头的数据
{
    if (get_ahd_yuv_tp_init_flag == 1) {
        return; // 如果已经初始化过，则直接返回
    }
    get_ahd_yuv_tp_init_flag = 1; // 标记为已初始化
    __info_tp_ahd.cb = cb; // 设置回调函数
    __info_tp_ahd.exit_state = 0; // 初始化退出状态
    thread_fork("GET_YUV_TP_TASK_AHD", 10, 0x1000, 0, &__info_tp_ahd.pid, user_scr_draw_test, NULL); // 创建新的线程以获取YUV数据
}

void get_ahd_yuv_tp_uninit(void)
{
    if (get_ahd_yuv_tp_init_flag == 0) { // 如果初始化标志为0，表示未初始化
        return; // 直接返回
    }
    get_ahd_yuv_tp_init_flag = 0; // 将初始化标志设为0，表示已取消初始化
    __info_tp_ahd.exit_state = 1; // 设置退出状态，指示线程应终止
    thread_kill(&__info_tp_ahd.pid, KILL_WAIT); // 杀死获取YUV数据的线程，等待其退出
}



