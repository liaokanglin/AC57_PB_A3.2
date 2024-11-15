#include "product_main.h"
#include "asm/lcd_config.h"

#ifdef PRODUCT_TEST_ENABLE


static OS_SEM lcd_sem;
static OS_SEM pir_sem;
static OS_SEM gsensor_sem;
static OS_SEM touchpanel_sem;
static u8 touchpanel_mflag = 0;

#define USER_VM_SIZE			0x80000	//4K对齐

#define USER_VM_UUID_ADDR 				0x0000
#define USER_VM_SN_ADDR 				0x0010
#define USER_VM_MAC1_ADDR 				0x0020
#define USER_VM_MAC2_ADDR 				0x0030
#define USER_VM_MAC3_ADDR 				0x0040
#define USER_VM_BOOT_SCREEN_INFO_ADDR	0x0050
#define USER_VM_LICENSE_ADDR			0x0100
#define USER_VM_OPTION_ADDR				0x0300
#define USER_VM_OPTION_STRING_ADDR		0x0500
#define USER_VM_BOOT_SCREEN_ADDR		0x1000	//4K对齐


u32 get_user_vm_size()
{
    return USER_VM_SIZE;
}

u8 product_uuid_wr(u8 *uuid, u8 is_write)
{
    u8 ret = 0;
    u8 tmp[PRODUCT_UUID_SIZE];

    if (!uuid || (is_write && strlen(uuid) > PRODUCT_UUID_SIZE)) {
        return ERR_PARAMS;
    }


    if (is_write) {
        user_vm_read(tmp, USER_VM_UUID_ADDR, PRODUCT_UUID_SIZE);
        /* put_buf(tmp, PRODUCT_UUID_SIZE); */
        u8 exist = 0;
        for (int i = 0; i < PRODUCT_UUID_SIZE; i++) {
            if (tmp[i] != 0xFF) {
                exist = 1;
                break;
            }
        }
        if (!strcmp(uuid, "NULL")) {
            exist = 2;
        }
        if (strncmp(tmp, uuid, PRODUCT_UUID_SIZE)) {
            if (exist == 0) {
                ret = ((user_vm_write(uuid, USER_VM_UUID_ADDR, PRODUCT_UUID_SIZE) == PRODUCT_UUID_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
            } else if (exist == 2) {
                memset(tmp, 0xFF, PRODUCT_UUID_SIZE);
                ret = ((user_vm_write(tmp, USER_VM_UUID_ADDR, PRODUCT_UUID_SIZE) == PRODUCT_UUID_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
            } else {
                ret = ERR_ALREADY_EXIST;
            }
        } else {
            ret = ERR_SAME_DATA;
        }
    } else {
        ret = ((user_vm_read(uuid, USER_VM_UUID_ADDR, PRODUCT_UUID_SIZE) == PRODUCT_UUID_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
        for (int i = 0; i < PRODUCT_UUID_SIZE; i++) {
            if (uuid[i] != 0xFF) {
                return ret;
            }
        }
        sprintf(uuid, "NULL");
    }
    /* user_vm_read(uuid, USER_VM_UUID_ADDR, PRODUCT_UUID_SIZE); */
    /* printf("uuid = %s\n", uuid); */
    /* put_buf(uuid, PRODUCT_UUID_SIZE); */
    return ret;
}


u8 product_sn_wr(u8 *sn, u8 is_write)
{
    u8 ret = 0;
    u8 tmp[PRODUCT_SN_SIZE];

    if (!sn || (is_write && strlen(sn) > PRODUCT_SN_SIZE)) {
        return ERR_PARAMS;
    }


    if (is_write) {
        user_vm_read(tmp, USER_VM_SN_ADDR, PRODUCT_SN_SIZE);
        u8 exist = 0;
        for (int i = 0; i < PRODUCT_SN_SIZE; i++) {
            if (tmp[i] != 0xFF) {
                exist = 1;
                break;
            }
        }
        if (!strncmp(sn, "NULL", 4)) {
            exist = 2;
        }
        if (strncmp(tmp, sn, PRODUCT_SN_SIZE)) {
            if (exist == 0) {
                ret = ((user_vm_write(sn, USER_VM_SN_ADDR, PRODUCT_SN_SIZE) == PRODUCT_SN_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
            } else if (exist == 2) {
                memset(tmp, 0xFF, PRODUCT_UUID_SIZE);
                ret = ((user_vm_write(tmp, USER_VM_SN_ADDR, PRODUCT_SN_SIZE) == PRODUCT_SN_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
            } else {
                ret = ERR_ALREADY_EXIST;
            }
        } else {
            ret = ERR_SAME_DATA;
        }
    } else {
        ret = ((user_vm_read(sn, USER_VM_SN_ADDR, PRODUCT_SN_SIZE) == PRODUCT_SN_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
        for (int i = 0; i < PRODUCT_SN_SIZE; i++) {
            if (sn[i] != 0xFF) {
                return ret;
            }
        }
        sprintf(sn, "NULL");
    }
    /* user_vm_read(sn, USER_VM_SN_ADDR, PRODUCT_SN_SIZE); */
    /* printf("sn = %s\n", sn); */
    /* put_buf(sn, 16); */
    return ret;
}


u8 product_rf_mac_wr(u8 *type, u8 *mac, u8 is_write)
{
    u8 ret, idx, *str[] = {"EDR", "BLE", "WIFI"};
    u8 tmp[PRODUCT_MAC_SIZE];
    u32 mac_addr[] = {USER_VM_MAC1_ADDR, USER_VM_MAC2_ADDR, USER_VM_MAC3_ADDR};

    for (idx = 0; idx < ARRAY_SIZE(str); idx++) {
        if (!strcmp(type, str[idx])) {
            break;
        }
    }

    if (idx < 3) {
        if (is_write) {
            user_vm_read(tmp, mac_addr[idx], PRODUCT_MAC_SIZE);
            u8 exist = 0;
            int i;
            for (i = 0; i < PRODUCT_MAC_SIZE; i++) {
                if (tmp[i] != 0xFF) {
                    exist = 1;
                    break;
                }
            }
            for (i = 0; i < PRODUCT_MAC_SIZE; i++) {
                if (mac[i] != 0xFF) {
                    break;
                }
            }
            if (i == PRODUCT_MAC_SIZE) {
                exist = 2;
            }
            if (strncmp(tmp, mac, PRODUCT_MAC_SIZE)) {
                if (exist == 0 || exist == 2) {
                    ret = ((user_vm_write(mac, mac_addr[idx], PRODUCT_MAC_SIZE) == PRODUCT_MAC_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
                } else {
                    ret = ERR_ALREADY_EXIST;
                }
            } else {
                ret = ERR_SAME_DATA;
            }
        } else {
            ret = ((user_vm_read(mac, mac_addr[idx], PRODUCT_MAC_SIZE) == PRODUCT_MAC_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
        }
    } else {
        ret = ERR_NO_SUPPORT_DEV_CMD;
    }

    return ret;
}


u8 product_license_flag_wr(u8 *flag, u8 is_write)
{
    //flag标志是否已写入license(掉电保存)
    u8 rscorr = ERR_NULL;
    int len;

    if (is_write) {
    } else {
        user_vm_read((u8 *)&len, USER_VM_LICENSE_ADDR, 4);
        if (len == 0 || len == -1) {
            *flag = 0;
        } else {
            *flag = 1;
        }
    }
    return rscorr;
}


u8 product_write_license(u8 idx, u8 *buf, u32 len, u32 file_size)
{
    //完成license数据写入后 要把product_license_flag_wr()函数中用的标志位写1
    int ret = 0;
    switch (idx) {
    case 0:
        if (file_size > PRODUCT_LICENSE_INFO_SIZE - 4) {
            ret = ERR_FILE_WR_NOSPACE;
        } else {
            u8 exist;
            product_license_flag_wr(&exist, 0);
            if (exist == 0) {
                ret = ((user_vm_write((u8 *)&len, USER_VM_LICENSE_ADDR, 4) == 4) ? ERR_NULL : ERR_DEV_FAULT);
                if (ret == ERR_NULL) {
                    ret = ((user_vm_write(buf, USER_VM_LICENSE_ADDR + 4, len) == len) ? ERR_NULL : ERR_DEV_FAULT);
                }
            } else {
                ret = ERR_ALREADY_EXIST;
            }
        }
        break;
    default:
        ret = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return ret;
}


u8 product_read_license(u8 *idx, u8 *buf, u32 *len)
{
    //读取licence数据
    u8 ret;
    *idx = 0;
    user_vm_read((u8 *)len, USER_VM_LICENSE_ADDR, 4);
    if (*len == 0 || *len == -1) {
        printf("no license\n");
        *len = 0;
        return ERR_FILE_WR;
    } else {
        ret = ((user_vm_read(buf, USER_VM_LICENSE_ADDR + 4, *len) == *len) ? ERR_NULL : ERR_FILE_WR);
    }

    return ret;
}


u8 product_erase_license(void)
{
    u8 ret;
    u8 *buf = malloc(PRODUCT_LICENSE_INFO_SIZE);
    memset(buf, 0xff, PRODUCT_LICENSE_INFO_SIZE);
    ret = ((user_vm_write(buf, USER_VM_LICENSE_ADDR, PRODUCT_LICENSE_INFO_SIZE) == PRODUCT_LICENSE_INFO_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
    free(buf);
    return ret;
}


u8 product_write_bootscreens_file(u8 type, u8 idx, u8 *buf, u32 len, u32 file_size)
{
#define BS_PACKERSIZE 2048
    u8 ret;
    static u8 last_idx = 0;
    u32 baseaddr = 0;
    u8 sinfo[PRODUCT_BOOT_SCREEN_INFO_SIZE];
    u32 *info = (u32 *)sinfo;

    memset(sinfo, 0xFF, PRODUCT_BOOT_SCREEN_INFO_SIZE);

    if (type == 0) {
        baseaddr = 0;
    } else {
        product_read_bootscreens_info(info);
        if (info[1] == 0 || info[1] == 0xFFFFFFFF) {
            log_err("please write startup logo first\n");
            return ERR_DEV_FAULT;
        }
        baseaddr = (info[1] + 4095) / 4096 * 4096;
    }

    if (baseaddr + file_size > PRODUCT_BOOT_SCREEN_SIZE) {
        log_err("boot screen file oversize %d\n", file_size);
        return ERR_FILE_WR_NOSPACE;
    }

    if (last_idx == 0) {
        if (type == 0) {
            log_d("erase boot screens");
            ret = product_erase_bootscreens();
            if (ret != ERR_NULL) {
                log_err("erase boot screen info err\n");
                return ret;
            }
        }
        u32 wlen = (file_size + 4095) / 4096 * 4096;
        user_vm_erase(USER_VM_BOOT_SCREEN_ADDR + baseaddr, wlen);
        last_idx = idx;
    }


    log_info("write boot screen data %d\n", idx);
    ret = ((user_vm_write(buf, USER_VM_BOOT_SCREEN_ADDR + baseaddr + BS_PACKERSIZE * (last_idx - idx), len) == len) ? ERR_NULL : ERR_DEV_FAULT);
    if (ret == ERR_NULL) {
        if (idx == 0) {
            info[type * 2] = baseaddr;
            info[type * 2 + 1] = file_size;
            log_info("write boot screen info %x %x %x %x\n", info[0], info[1], info[2], info[3]);
            u32 product_write_bootscreens_info(u32 * info);
            ret = product_write_bootscreens_info(info);
            if (ret != ERR_NULL) {
                log_err("write boot screen info err\n");
                return ret;
            }
            last_idx = 0;
        }
    }

    return ret;
}


u8 product_read_bootscreens_info(u32 *info)
{
    return ((user_vm_read(info, USER_VM_BOOT_SCREEN_INFO_ADDR, PRODUCT_BOOT_SCREEN_INFO_SIZE) == PRODUCT_BOOT_SCREEN_INFO_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
}

u32 product_write_bootscreens_info(u32 *info)
{
    return ((user_vm_write(info, USER_VM_BOOT_SCREEN_INFO_ADDR, PRODUCT_BOOT_SCREEN_INFO_SIZE) == PRODUCT_BOOT_SCREEN_INFO_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
}

u8 product_read_bootscreens(u8 type, u8 *buf, u32 len)
{
    u32 info[4];
    product_read_bootscreens_info(info);
    u32 baseaddr = info[type * 2];
    return ((user_vm_read(buf, USER_VM_BOOT_SCREEN_ADDR + baseaddr, len) == len) ? ERR_NULL : ERR_DEV_FAULT);
}


u8 product_erase_bootscreens(void)
{
    u8 ret;
    u8 info[PRODUCT_BOOT_SCREEN_INFO_SIZE];
    memset(info, 0xFF, PRODUCT_BOOT_SCREEN_INFO_SIZE);
    ret = ((user_vm_write(info, USER_VM_BOOT_SCREEN_INFO_ADDR, PRODUCT_BOOT_SCREEN_INFO_SIZE) == PRODUCT_BOOT_SCREEN_INFO_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
    return ret;
}

u8 product_write_startup_screens(u8 idx, u8 *buf, u32 len, u32 file_size)
{
    return product_write_bootscreens_file(0, idx, buf, len, file_size);
}

u8 product_write_shutdown_screens(u8 idx, u8 *buf, u32 len, u32 file_size)
{
    return product_write_bootscreens_file(1, idx, buf, len, file_size);
}



// u8 product_rtc_default_wr(struct product_rtc_time *time, u8 is_write)
// {
//     static struct sys_time t;
//     void *fd = dev_open("rtc", NULL);
//     if (!fd) {
//         return ERR_DEV_FAULT;
//     }

//     if (is_write) {
//         t.year = time->year;
//         t.month = time->month;
//         t.day = time->day;
//         t.hour = time->hour;
//         t.min = time->min;
//         t.sec = time->sec;
//         dev_ioctl(fd, IOCTL_SET_SYS_TIME, (u32)&t);
//     } else {
//         dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)&t);
//         time->year = t.year;
//         time->month = t.month;
//         time->day  = t.day;
//         time->hour = t.hour;
//         time->min  = t.min;
//         time->sec  = t.sec;
//     }

//     dev_close(fd);
//     return ERR_NULL;
// }


#define OPTION_KEY_LEN (8)
#define OPTION_STRING_LEN (56)
typedef struct {
    char key[OPTION_KEY_LEN];
    int value;
} option_value;
typedef struct {
    char key[OPTION_KEY_LEN];
    char string[OPTION_STRING_LEN];
} option_string;

u8 product_write_options(json_object *options_obj)
{
    u32 len;
    u8 ret;

    len = json_object_array_length(options_obj);

    u8 *buf = malloc(PRODUCT_OPTION_SIZE);
    memset(buf, 0xff, PRODUCT_OPTION_SIZE);
    option_value *opt_v = (option_value *)buf;
    u8 *buf1 = malloc(PRODUCT_OPTION_STRING_SIZE);
    memset(buf1, 0xff, PRODUCT_OPTION_STRING_SIZE);
    option_string *opt_s = (option_string *)buf1;
    u32 cnt_v = 0;
    u32 cnt_s = 0;
    for (int i = 0; i < len; i++) {
        if (!strcmp(json_object_get_string(json_object_object_get(json_object_array_get_idx(options_obj, i), "type")), "text")) {
            strcpy(opt_s[cnt_s].key, json_object_get_string(json_object_object_get(json_object_array_get_idx(options_obj, i), "key")));
            strcpy(opt_s[cnt_s].string, json_object_get_string(json_object_object_get(json_object_array_get_idx(options_obj, i), "value")));
            log_info("option%d %s : %s\n", i, opt_s[cnt_s].key, opt_s[cnt_s].string);
            if (++cnt_s > PRODUCT_OPTION_STRING_SIZE / sizeof(option_string)) {
                log_err("options string oversize\n");
                free(buf);
                free(buf1);
                return ERR_FILE_WR_NOSPACE;
            }
        } else {
            strcpy(opt_v[cnt_v].key, json_object_get_string(json_object_object_get(json_object_array_get_idx(options_obj, i), "key")));
            opt_v[cnt_v].value = json_object_get_int(json_object_object_get(json_object_array_get_idx(options_obj, i), "value"));
            log_info("option%d %s : %d\n", i, opt_v[cnt_v].key, opt_v[cnt_v].value);
            if (++cnt_v > PRODUCT_OPTION_SIZE / sizeof(option_value)) {
                log_err("options oversize\n");
                free(buf);
                free(buf1);
                return ERR_FILE_WR_NOSPACE;
            }
        }
    }

    ret = ((user_vm_write(buf, USER_VM_OPTION_ADDR, PRODUCT_OPTION_SIZE) == PRODUCT_OPTION_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
    if (ret == ERR_NULL) {
        ret = ((user_vm_write(buf1, USER_VM_OPTION_STRING_ADDR, PRODUCT_OPTION_STRING_SIZE) == PRODUCT_OPTION_STRING_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
        db_reset();
    }

    free(buf);
    free(buf1);

    return ret;
}

u8 product_erase_options(void)
{
    u8 ret;
    u8 *buf = malloc(PRODUCT_OPTION_SIZE);
    memset(buf, 0xff, PRODUCT_OPTION_SIZE);
    ret = ((user_vm_write(buf, USER_VM_OPTION_ADDR, PRODUCT_OPTION_SIZE) == PRODUCT_OPTION_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
    free(buf);
    buf = malloc(PRODUCT_OPTION_STRING_SIZE);
    memset(buf, 0xff, PRODUCT_OPTION_STRING_SIZE);
    ret = ((user_vm_write(buf, USER_VM_OPTION_STRING_ADDR, PRODUCT_OPTION_STRING_SIZE) == PRODUCT_OPTION_STRING_SIZE) ? ERR_NULL : ERR_DEV_FAULT);
    free(buf);
    return ret;
}

void db_reset_from_vm()
{
    u8 *buf = malloc(PRODUCT_OPTION_SIZE);
    option_value *opt = (option_value *)buf;
    if (user_vm_read(buf, USER_VM_OPTION_ADDR, PRODUCT_OPTION_SIZE) == PRODUCT_OPTION_SIZE) {
        for (int i = 0; i < PRODUCT_OPTION_SIZE / sizeof(option_value); i++) {
            if (opt[i].key[0] == 0xFF || opt[i].key[0] == 0) {
                break;
            }
            db_update(opt[i].key, opt[i].value);
        }
    }
    free(buf);
}

int product_read_options_string(char *key, char *string)
{
    u8 *buf = malloc(PRODUCT_OPTION_STRING_SIZE);
    option_string *opt = (option_string *)buf;
    if (user_vm_read(buf, USER_VM_OPTION_STRING_ADDR, PRODUCT_OPTION_STRING_SIZE) == PRODUCT_OPTION_STRING_SIZE) {
        for (int i = 0; i < PRODUCT_OPTION_SIZE / sizeof(option_string); i++) {
            if (opt[i].key[0] == 0xFF || opt[i].key[0] == 0) {
                free(buf);
                return -EINVAL;
            }
            if (!strcmp(opt[i].key, key)) {
                strcpy(string, opt[i].string);
                break;
            }
        }
    }
    free(buf);
    return 0;
}




u8 product_sd_get_info(u32 *status, u32 *cap_size, u32 *block_size)
{
    void *hdl = NULL;

    if (!hdl) {
        if (!(hdl = dev_open(SDX_DEV, NULL))) {
            return ERR_DEV_FAULT;
        }
    }

    dev_ioctl(hdl, IOCTL_GET_STATUS, status);
    if (*status) {
        dev_ioctl(hdl, IOCTL_GET_CAPACITY, cap_size);
        dev_ioctl(hdl, IOCTL_GET_BLOCK_SIZE, block_size);
    } else {
        *cap_size = 0;
        *block_size = 0;
    }

    dev_close(hdl);
    return ERR_NULL;
}


u8 product_sd_testfile_wr_check(void)
{
    FILE *fd;
    u8 rscorr;
    u8 r_data[64] = {0};
    u8 w_data[] = "product tool sd testfile write/read test";

    if (!storage_device_ready()) {
        return ERR_NO_SUPPORT_DEV;
    }

    if (!(fd = fopen(CONFIG_ROOT_PATH"testfile.txt", "w+"))) {
        return ERR_FILE_WR;
    }

    if (fwrite(fd, w_data, sizeof(w_data)) != sizeof(w_data)) {
        rscorr = ERR_FILE_WR;
        goto _sd_wr_check_exit_;
    }

    fclose(fd);
    fd = NULL;

    if (!(fd = fopen(CONFIG_ROOT_PATH"testfile.txt", "r"))) {
        return ERR_FILE_WR;
    }

    if (fread(fd, r_data, sizeof(w_data)) != sizeof(w_data)) {
        rscorr = ERR_FILE_WR;
        goto _sd_wr_check_exit_;
    }

    log_info("read_data = %s\n", r_data);
    rscorr = strcmp(r_data, w_data) ? ERR_FILE_CHECK : ERR_NULL;

_sd_wr_check_exit_:
    fclose(fd);
    fd = NULL;

    return rscorr;
}


u8 product_battery_get_info(u8 *power_percent)
{
    // 声明并定义函数 sys_power_get_battery_persent，用于获取当前电池电量百分比
    int sys_power_get_battery_persent(void);

    *power_percent = sys_power_get_battery_persent(); 
    // 将从 sys_power_get_battery_persent 函数返回的电池电量值赋给传入的指针 power_percent

    return ERR_NULL; 
    // 返回错误码 ERR_NULL，表示没有错误（ERR_NULL 通常表示操作成功）
}



#ifdef CONFIG_UI_ENABLE

u8 product_lcd_get_info(u32 *width, u32 *height)
{
    *width = LCD_DEV_WIDTH;
    *height = LCD_DEV_HIGHT;
    return ERR_NULL;
}


static void *fb = NULL;
static void product_lcd_color_switch(void *p)
{
    static u32 test_color[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000};
    static u32 test_cnt = 0;
    struct fb_map_user map;

    if (p) {
        test_cnt = 0;
    }

    if (!fb) {
        return;
    }

    printf("prodect_lcd_color_switch %06x\n", test_color[test_cnt]);

    user_scr_draw_getmap(fb, &map);
    if (map.baddr) {
        for (int y = 0; y < LCD_DEV_HIGHT; y++) {
            for (int x = 0; x < LCD_DEV_WIDTH; x++) {
                map.baddr[y * LCD_DEV_WIDTH * 3 + x * 3 + 2] = (test_color[test_cnt] & 0x00FF0000) >> 16;
                map.baddr[y * LCD_DEV_WIDTH * 3 + x * 3 + 1] = (test_color[test_cnt] & 0x0000FF00) >> 8;
                map.baddr[y * LCD_DEV_WIDTH * 3 + x * 3 + 0] = (test_color[test_cnt] & 0x000000FF);
            }
        }
        user_scr_draw_putmap(fb, &map);
    }
    test_cnt ++;
    if (test_cnt >= sizeof(test_color) / sizeof(test_color[0])) {
        test_cnt = 0;
    }
}

u8 product_lcd_color_test(u8 on)
{
    if (on) {
        if (!fb) {
            fb = user_scr_draw_open_fb(FB_COLOR_FORMAT_RGB888, LCD_DEV_WIDTH, LCD_DEV_HIGHT, 0, 0);
            product_lcd_color_switch(1);
        }
    } else {
        user_scr_draw_close_fb(fb);
        fb = NULL;
    }
    return ERR_NULL;
}


u8 product_lcd_init(void)
{
    static int timer = 0;
    if (!timer) {
        timer = sys_timer_add(0, product_lcd_color_switch, 5000);
    }
    return ERR_NULL;
}


#endif


u8 product_camera_get_info(struct procudt_camera_info *info)
{
    void *video_dev;
    pic_size_t pic_size;
    u8 fps;

    isp0_ioctrl(&pic_size, ISP_CMD_GET_SEN_SIZE);

    info->width = pic_size.w;
    info->height = pic_size.h;

    isp0_ioctrl(&fps, ISP_CMD_GET_FREQ);
    info->fps = fps;

    isp0_ioctrl(info->name, ISP_CMD_GET_SEN_NAME);

    return ERR_NULL;
}


u8 product_camera_reg_wr(u32 addr, u32 *value, u8 is_write, u8 off)
{
    if (off) {
        return ERR_NULL;
    }

    if (is_write) {
        ispt_sensor_write_reg(addr, *value);
    } else {
        ispt_sensor_read_reg(addr, value);
    }

    return ERR_NULL;
}


u8 product_camera_ntv_ctl(u8 on)
{
    return ERR_NULL;
}


u8 product_camera_light_ctl(u8 on)
{
    return ERR_NULL;
}


u8 product_pir_init(void)
{
    os_sem_create(&pir_sem, 0);

    //添加PIR初始化
    //
    //
    return ERR_NULL;
}


static u8 product_pir_status_get(void)
{
    //PIR 触发返回DEV_STATUS_ON, 否则返回DEV_STATUS_OFF
    //for test
    static u8 status = 0;
    status ^= 1;
    return status;
}


static void pir_monitor_task(void *priv)
{
    u8 *pirstr;
    static u8 status = 0, last_status = 0;

    for (;;) {
        last_status = status;
        status = product_pir_status_get();
        if (status != last_status) {
            asprintf(&pirstr, "{\"opcode\":\"%d\",\"rscorr\":\"%d\",\"params\":{\"id\":\"%d\",\"type\":\"%d\",\"cmd\":\"%d\",\"args\":{\"pir_status\":\"%d\"}}}", \
                     OPC_DEV_CTL, ERR_NULL, 0, DEV_TYPE_PIR, CTL_DEV_MONITOR_RSP, status);

            log_info("--->resp pir_status\n\n%s\n\n", pirstr);
            data_respond(0, DATA_TYPE_OPCODE, pirstr, strlen(pirstr));

            if (pirstr) {
                free(pirstr);
            }
        }

        if (!os_sem_accept(&pir_sem)) {
            return;
        }

        os_time_dly(DEV_MONITOR_PERIOD);
    }
}


u8 product_pir_monitor_ctl(u8 on)
{
    static int pid = 0;
    if (on && !pid) {
        os_sem_set(&pir_sem, 0);
        thread_fork("pir_monitor_task", 6, 512, 0, &pid, pir_monitor_task, NULL);
    } else {
        os_sem_post(&pir_sem);
        pid = 0;
    }
    return ERR_NULL;
}


u8 product_motor_init(void)
{
    return ERR_NULL;
}


u8 product_motor_ctl(u8 cmd)
{
    u8 rscorr = ERR_NULL;
    log_info("%s, cmd = %d\n", __FUNCTION__, cmd);

    switch (cmd) {
    case CTL_MOTOR_LEFT:
        break;

    case CTL_MOTOR_RIGHT:
        break;

    case CTL_MOTOR_STOP:
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


u8 product_gsensor_init(void)
{
    os_sem_create(&gsensor_sem, 0);

    set_gse_sensity(1);
    return ERR_NULL;
}


static u8 product_gsensor_status_get(void)
{
    //达到gsensor设定阈值触发返回DEV_STATUS_ON, 否则返回DEV_STATUS_OFF
#ifdef CONFIG_GSENSOR_ENABLE
    if (get_gsen_active_state() == 0) {
        return DEV_STATUS_OFF;
    } else {
        return DEV_STATUS_ON;
    }
#else
    return DEV_STATUS_OFF;
#endif
}


static void gsensor_monitor_task(void *priv)
{
    u8 *str;
    static u8 status = 0, last_status = 0;

    for (;;) {
        last_status = status;
        status = product_gsensor_status_get();
        if (status != last_status) {
            asprintf(&str, "{\"opcode\":\"%d\",\"rscorr\":\"%d\",\"params\":{\"id\":\"%d\",\"type\":\"%d\",\"cmd\":\"%d\",\"args\":{\"gsensor_status\":\"%d\"}}}", \
                     OPC_DEV_CTL, ERR_NULL, 0, DEV_TYPE_GSENSOR, CTL_DEV_MONITOR_RSP, status);

            log_info("--->resp gsensor_status\n\n%s\n\n", str);
            data_respond(0, DATA_TYPE_OPCODE, str, strlen(str));

            if (str) {
                free(str);
            }
        }

        if (!os_sem_accept(&gsensor_sem)) {
            return;
        }

        os_time_dly(DEV_MONITOR_PERIOD);
    }
}


u8 product_gesnsor_monitor_ctl(u8 on)
{
    static int pid = 0;
    if (on && !pid) {
        os_sem_set(&gsensor_sem, 0);
        thread_fork("gsensor_monitor_task", 6, 512, 0, &pid, gsensor_monitor_task, NULL);
    } else {
        os_sem_post(&gsensor_sem);
        pid = 0;
    }
    return ERR_NULL;
}


//for test
static void product_touchpanel_test(void *priv)
{
    static int x = 0, y = 0;
    x++;
    y--;
    product_touchpanel_coord_post(x, y);
}


u8 product_touchpanel_init(void)
{
    os_sem_create(&touchpanel_sem, 0);
    //添加Gsensor初始化
    //
    //for test, delete
    sys_timer_add(NULL, product_touchpanel_test, 20);
    return ERR_NULL;
}


void product_touchpanel_coord_post(int x, int y)
{
    if (!touchpanel_mflag) {
        return;
    }

    int msg[2];
    msg[0] = x;
    msg[1] = y;
    os_taskq_post_type("touchpanel_monitor_task", 0, ARRAY_SIZE(msg), msg);
}


static void touchpanel_monitor_task(void *priv)
{
    u8 *str;
    int msg[3];

    for (;;) {
        if (os_taskq_pend(NULL, msg, ARRAY_SIZE(msg)) != OS_TASKQ) {
            continue;
        }

        asprintf(&str, "{\"opcode\":\"%d\",\"rscorr\":\"%d\",\"params\":{\"id\":\"%d\",\"type\":\"%d\",\"cmd\":\"%d\",\"args\":{\"x\":\"%d\",\"y\":\"%d\"}}}", \
                 OPC_DEV_CTL, ERR_NULL, 0, DEV_TYPE_TOUCHPANEL, CTL_DEV_MONITOR_RSP, msg[1], msg[2]);

        log_info("--->resp touchpannel_status\n\n%s\n\n", str);
        data_respond(0, DATA_TYPE_OPCODE, str, strlen(str));

        if (str) {
            free(str);
        }

        if (!os_sem_accept(&touchpanel_sem)) {
            return;
        }

        os_time_dly(DEV_MONITOR_PERIOD);
    }
}


u8 product_touchpanel_monitor_ctl(u8 on)
{
    static int pid = 0;
    if (on && !pid) {
        os_sem_set(&touchpanel_sem, 0);
        thread_fork("touchpanel_monitor_task", 6, 512, 512, &pid, touchpanel_monitor_task, NULL);
        touchpanel_mflag = 1;
    } else {
        touchpanel_mflag = 0;
        os_sem_post(&touchpanel_sem);
        pid = 0;
    }
    return ERR_NULL;
}

#endif
