#include "product_main.h"

#ifdef PRODUCT_TEST_ENABLE

#define WIFI_SCAN_RES_CNT	(32)

static u8 *args_str;
static u8 conn_status;
static OS_SEM conn_sem;
static void *wifi_dev = NULL;
static u32 ping_on = FALSE, ping_total;


__attribute__((weak)) u8 product_uuid_wr(u8 *uuid, u8 is_write)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_sn_wr(u8 *sn, u8 is_write)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_rf_mac_wr(u8 *type, u8 *mac, u8 is_write)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_write_license(u8 idx, u8 *buf, u32 len, u32 file_size)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_read_license(u8 *idx, u8 *buf, u32 *len)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_erase_license(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_erase_screens(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_write_startup_screens(u8 idx, u8 *buf, u32 len, u32 file_size)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_write_shutdown_screens(u8 idx, u8 *buf, u32 len, u32 file_size)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_write_options(json_object *options_obj)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_sd_get_info(u32 *status, u32 *cap_size, u32 *block_size)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_sd_testfile_wr_check(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_battery_get_info(u8 *power_percent)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_lcd_init(void)
{
    return ERR_NULL;
}


__attribute__((weak)) u8 product_lcd_color_test(u8 on)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_lcd_get_info(u32 *width, u32 *height)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_camera_reg_wr(u32 addr, u32 *value, u8 is_write, u8 off)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_camera_ntv_ctl(u8 on)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_camera_light_ctl(u8 on)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_camera_get_info(struct procudt_camera_info *info)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_pir_init(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_pir_monitor_ctl(u8 on)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


__attribute__((weak)) u8 product_license_flag_wr(u8 *flag, u8 is_write)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_motor_init(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_motor_ctl(u8 cmd)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_gsensor_init(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_gesnsor_monitor_ctl(u8 on)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_touchpanel_init(void)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}

__attribute__((weak)) u8 product_touchpanel_monitor_ctl(u8 on)
{
    return ERR_NO_SUPPORT_DEV_CMD;
}


u8 *get_args_str(void)
{
    return args_str;
}


static u8 *json_itoa(u32 value, u8 base)
{
    static u8 str[16];

    switch (base) {
    case 10:
        sprintf(str, "%d", value);
        break;

    case 16:
        sprintf(str, "0x%x", value);
        break;

    default:
        sprintf(str, "%d", 0);
        break;
    }
    return str;
}


static u8 sd_control_handler(u8 id, cmd, json_object *args_obj)
{
    u8 rscorr = ERR_NULL;
    u32 status, cap_size, block_size;
    struct json_object *sub_obj;

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        if (rscorr = product_sd_get_info(&status, &cap_size, &block_size)) {
            break;
        }
        sub_obj = json_object_new_object();
        json_object_object_add(sub_obj, "online", json_object_new_string(json_itoa(status, 10)));
        json_object_object_add(sub_obj, "cap_size", json_object_new_string(json_itoa(cap_size, 10)));
        json_object_object_add(sub_obj, "block_size", json_object_new_string(json_itoa(block_size, 10)));
        asprintf(&args_str, "%s", json_object_to_json_string(sub_obj));
        json_object_put(sub_obj);
        break;

    case CTL_SD_FILE_WR:
        rscorr = product_sd_testfile_wr_check();
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


static u8 battery_control_handler(u8 id, cmd, json_object *args_obj)
{
    u8 percent; // 用于存储电池电量百分比
    u8 rscorr = ERR_NULL; // 返回值，初始设为ERR_NULL（表示没有错误）
    struct json_object *sub_obj; // JSON对象，用于构造返回数据

    args_str = NULL; // 初始化参数字符串为空

    switch (cmd) {
    case CTL_GET_DEV_INFO: // 处理获取设备信息的命令
        if (rscorr = product_battery_get_info(&percent)) { 
            // 调用 product_battery_get_info 函数获取电池电量百分比，如果有错误则跳出
            break;
        }
        sub_obj = json_object_new_object(); 
        // 创建一个新的JSON对象，存放电池电量信息
        json_object_object_add(sub_obj, "power", json_object_new_string(json_itoa(percent, 10))); 
        // 将电池百分比转换为字符串后，添加到JSON对象中，键为"power"
        asprintf(&args_str, "%s", json_object_to_json_string(sub_obj)); 
        // 将JSON对象转换为字符串，并分配给args_str，供后续使用
        json_object_put(sub_obj); 
        // 释放创建的JSON对象，避免内存泄漏
        break;

    default: 
        rscorr = ERR_NO_SUPPORT_DEV_CMD; 
        // 如果命令不被支持，返回ERR_NO_SUPPORT_DEV_CMD表示不支持的命令
        break;
    }

    return rscorr; 
    // 返回rscorr，指示操作是否成功或有错误
}



static u8 lcd_control_handler(u8 id, cmd, json_object *args_obj)
{
    json_object *sub_obj;
    u32 width, height;
    u8 rscorr = ERR_NULL, *str;

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        if (rscorr = product_lcd_get_info(&width, &height)) {
            break;
        }
        sub_obj = json_object_new_object();
        json_object_object_add(sub_obj, "width", json_object_new_string(json_itoa(width, 10)));
        json_object_object_add(sub_obj, "height", json_object_new_string(json_itoa(height, 10)));
        asprintf(&args_str, "%s", json_object_to_json_string(sub_obj));
        json_object_put(sub_obj);
        break;

    case CTL_LCD_COLOR_SWITCH_ON:
        rscorr = product_lcd_color_test(1);
        break;

    case CTL_LCD_COLOR_SWITCH_OFF:
        rscorr = product_lcd_color_test(0);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}





static u8 pir_control_handler(u8 id, cmd, json_object *args_obj)
{
    u8 rscorr = ERR_NULL;

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        break;

    case CTL_DEV_MONITOR_START:
        product_pir_monitor_ctl(1);
        break;

    case CTL_DEV_MONITOR_STOP:
        product_pir_monitor_ctl(0);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


static u8 motor_control_handler(u8 id, cmd, json_object *args_obj)
{
    u8 rscorr = ERR_NULL;
    json_object *action_obj;

    args_str = NULL;

    switch (cmd) {
    case CTL_MOTOR_LEFT:
    case CTL_MOTOR_RIGHT:
    case CTL_MOTOR_STOP:
        rscorr = product_motor_ctl(cmd);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


static u8 gsensor_control_handler(u8 id, cmd, json_object *args_obj)
{
    u8 rscorr = ERR_NULL;

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        break;

    case CTL_DEV_MONITOR_START:
        product_gesnsor_monitor_ctl(1);
        break;

    case CTL_DEV_MONITOR_STOP:
        product_gesnsor_monitor_ctl(0);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


static u8 touchpanel_control_handler(u8 id, cmd, json_object *args_obj)
{
    u8 rscorr = ERR_NULL;

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        break;

    case CTL_DEV_MONITOR_START:
        product_touchpanel_monitor_ctl(1);
        break;

    case CTL_DEV_MONITOR_STOP:
        product_touchpanel_monitor_ctl(0);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


static u8 camera_control_handler(u8 id, cmd, json_object *args_obj)
{
    u32 addr, value;
    json_object *sub_obj;
    struct procudt_camera_info info;
    u8 rscorr = ERR_NULL, *str, array_cnt;

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        rscorr = product_camera_get_info(&info);
        sub_obj = json_object_new_object();
        json_object_object_add(sub_obj, "name", json_object_new_string(info.name));
        json_object_object_add(sub_obj, "fps", json_object_new_string(json_itoa(info.fps, 10)));
        json_object_object_add(sub_obj, "width", json_object_new_string(json_itoa(info.width, 10)));
        json_object_object_add(sub_obj, "height", json_object_new_string(json_itoa(info.height, 10)));
        asprintf(&args_str, "%s", json_object_to_json_string(sub_obj));
        json_object_put(sub_obj);
        break;

    case CTL_READ_REG:
        if (!(array_cnt = json_object_array_length(args_obj))) {
            rscorr = ERR_PARAMS;
            break;
        }
        for (u8 i = 0; i < array_cnt; i++) {
            sub_obj = json_object_array_get_idx(args_obj, i);
            addr = strtoul(json_object_get_string(json_object_object_get(sub_obj, "addr")) + 2, NULL, 16);
            value = strtoul(json_object_get_string(json_object_object_get(sub_obj, "value")) + 2, NULL, 16);
            if (rscorr = product_camera_reg_wr(addr, &value, 0, 0)) {
                break;
            }
            json_object_string_set(json_object_object_get(sub_obj, "value"), json_itoa(value, 16), strlen(json_itoa(value, 16)));
        }
        asprintf(&args_str, "%s", json_object_get_string(args_obj));
        product_camera_reg_wr(0, 0, 0, 1);
        break;

    case CTL_WRITE_REG:
        if (!(array_cnt = json_object_array_length(args_obj))) {
            rscorr = ERR_PARAMS;
            break;
        }
        for (u8 i = 0; i < array_cnt; i++) {
            sub_obj = json_object_array_get_idx(args_obj, i);
            addr = strtoul(json_object_get_string(json_object_object_get(sub_obj, "addr")) + 2, NULL, 16);
            value = strtoul(json_object_get_string(json_object_object_get(sub_obj, "value")) + 2, NULL, 16);
            if (rscorr = product_camera_reg_wr(addr, &value, 1, 0)) {
                break;
            }
        }
        product_camera_reg_wr(0, 0, 0, 1);
        break;

    case CTL_CAMERA_NVG_ON:
        rscorr = product_camera_ntv_ctl(1);
        break;

    case CTL_CAMERA_NVG_OFF:
        rscorr = product_camera_ntv_ctl(0);
        break;

    case CTL_CAMERA_LIGHT_ON:
        rscorr = product_camera_light_ctl(1);
        break;

    case CTL_CAMERA_LIGHT_OFF:
        rscorr = product_camera_light_ctl(0);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


#ifdef CONFIG_WIFI_ENABLE
static int network_user_callback(void *network_ctx, enum NETWORK_EVENT state, void *priv)
{

    struct cfg_info info;
    int ret = 0;

    switch (state) {

    case WIFI_EVENT_MODULE_INIT:
//wifi module port seting
        info.port_status = 0;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER, (u32)&info);
        msleep(10);
        info.port_status = 1;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER, (u32)&info);

        info.port_status = 1;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_CS, (u32)&info);

        info.port_status = 1;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_WKUP, (u32)&info);

        msleep(100);

        info.ssid = "111222";
        info.pwd = "";
        info.mode = SMP_CFG_MODE;
        info.force_default_mode = 1;
        dev_ioctl(wifi_dev, DEV_SET_DEFAULT_MODE, (u32)&info);
        break;

    case WIFI_EVENT_MODULE_START:
        puts("|network_user_callback->WIFI_EVENT_MODULE_START\n");
        break;

    case WIFI_EVENT_MODULE_STOP:
        puts("|network_user_callback->WIFI_EVENT_MODULE_STOP\n");
        break;

    case WIFI_EVENT_AP_START:
        puts("|network_user_callback->WIFI_EVENT_AP_START\n");
        break;

    case WIFI_EVENT_AP_STOP:
        puts("|network_user_callback->WIFI_EVENT_AP_STOP\n");

#ifdef USE_MARVEL8801
        //8801要if 1，8189不能开，开了切换不了模式
        info.port_status = 0;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER, (u32)&info);
        msleep(10);
        info.port_status = 1;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER, (u32)&info);
#endif
        break;

    case WIFI_EVENT_STA_START:
        puts("|network_user_callback->WIFI_EVENT_STA_START\n");
        info.timeout = 20;//20s timeout
        dev_ioctl(wifi_dev, DEV_SET_STA_MODE_TIMEOUT_SEC, (u32)&info);
        break;

    case WIFI_EVENT_MODULE_START_ERR:
        puts("|network_user_callback->WIFI_EVENT_MODULE_START_ERR\n");
        break;

    case WIFI_EVENT_STA_STOP:
        puts("|network_user_callback->WIFI_EVENT_STA_STOP\n");
#ifdef USE_MARVEL8801
        info.port_status = 0;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER, (u32)&info);
        msleep(10);
        info.port_status = 1;
        dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER, (u32)&info);
#endif
        break;

    case WIFI_EVENT_STA_DISCONNECT:
        puts("|network_user_callback->WIFI_STA_DISCONNECT\n");
        break;

    case WIFI_EVENT_STA_SCAN_COMPLETED:
        puts("|network_user_callback->WIFI_STA_SCAN_COMPLETED\n");
        break;

    case WIFI_EVENT_STA_CONNECT_SUCC:
#ifdef CONFIG_STATIC_IPADDR_ENABLE
        if (0 == compare_dhcp_ipaddr()) {
            wifi_set_sta_ip_info();
            ret = 1;
        }
#endif
        break;

    case WIFI_EVENT_MP_TEST_START:
        puts("|network_user_callback->WIFI_EVENT_MP_TEST_START\n");
        break;

    case WIFI_EVENT_MP_TEST_STOP:
        puts("|network_user_callback->WIFI_EVENT_MP_TEST_STOP\n");
        break;

    case WIFI_EVENT_STA_CONNECT_TIMEOUT_NOT_FOUND_SSID:
        puts("|network_user_callback->WIFI_STA_CONNECT_TIMEOUT_NOT_FOUND_SSID\n");
        os_sem_post(&conn_sem);
        conn_status = 0;
#ifdef CONFIG_STA_AUTO_MODE	//打开STA模式扫描连接扫描不到wifi名称，自动切回AP模式，防止死循环扫描无法回AP模式
        static u8 cnt = 0;
        cnt++;

        if (cnt >= 3) {
            wifi_sta_to_ap_mode_change();
        }

#endif
        break;

    case WIFI_EVENT_STA_CONNECT_TIMEOUT_ASSOCIAT_FAIL:
        puts("|network_user_callback->WIFI_STA_CONNECT_TIMEOUT_ASSOCIAT_FAIL .....\n");
#ifdef CONFIG_STA_AUTO_MODE	//打开STA模式连接路由器密码错误，自动切回AP模式，防止死循环连接路由器
        wifi_sta_to_ap_mode_change();
#endif
        break;

    case WIFI_EVENT_STA_NETWORK_STACK_DHCP_SUCC:
        puts("|network_user_callback->WIFI_EVENT_STA_NETWPRK_STACK_DHCP_SUCC\n");
        os_sem_post(&conn_sem);
        conn_status = 1;
#ifdef CONFIG_STATIC_IPADDR_ENABLE
        store_dhcp_ipaddr();
#endif//CONFIG_STATIC_IPADDR_ENABLE
        break;

    case WIFI_EVENT_STA_NETWORK_STACK_DHCP_TIMEOUT:
        puts("|network_user_callback->WIFI_EVENT_STA_NETWPRK_STACK_DHCP_TIMEOUT\n");
        break;

    case WIFI_EVENT_P2P_START:
        puts("|network_user_callback->WIFI_EVENT_P2P_START\n");
        break;

    case WIFI_EVENT_P2P_STOP:
        puts("|network_user_callback->WIFI_EVENT_P2P_STOP\n");
        break;

    case WIFI_EVENT_P2P_GC_DISCONNECTED:
        puts("|network_user_callback->WIFI_EVENT_P2P_GC_DISCONNECTED\n");
        break;

    case WIFI_EVENT_P2P_GC_NETWORK_STACK_DHCP_SUCC:
        puts("|network_user_callback->WIFI_EVENT_P2P_GC_NETWORK_STACK_DHCP_SUCC\n");
        break;

    case WIFI_EVENT_P2P_GC_NETWORK_STACK_DHCP_TIMEOUT:
        puts("|network_user_callback->WIFI_EVENT_P2P_GC_NETWORK_STACK_DHCP_TIMEOUT\n");
        break;

    case WIFI_EVENT_SMP_CFG_START:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_START\n");
        break;

    case WIFI_EVENT_SMP_CFG_STOP:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_STOP\n");
        break;

    case WIFI_EVENT_SMP_CFG_TIMEOUT:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_TIMEOUT\n");
        break;

    case WIFI_EVENT_SMP_CFG_COMPLETED:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_COMPLETED\n");
        break;

    case WIFI_EVENT_PM_SUSPEND:
        puts("|network_user_callback->WIFI_EVENT_PM_SUSPEND\n");
        break;

    case WIFI_EVENT_PM_RESUME:
        puts("|network_user_callback->WIFI_EVENT_PM_RESUME\n");
        break;

    case WIFI_EVENT_AP_ON_ASSOC:
        break;

    case WIFI_EVENT_AP_ON_DISCONNECTED:
        struct eth_addr *hwaddr = (struct eth_addr *)network_ctx;
#if __SDRAM_SIZE__ >= (8 * 1024 * 1024)
        /* 强制断开所有RTSP链接,实际上应该只断开响应客户端 */
        void stream_media_server_dhwaddr_close(struct eth_addr * dhwaddr);
        stream_media_server_dhwaddr_close(hwaddr);
#endif
        ctp_keep_alive_find_dhwaddr_disconnect((struct eth_addr *)hwaddr->addr);
        cdp_keep_alive_find_dhwaddr_disconnect((struct eth_addr *)hwaddr->addr);
        break;

    default:
        break;
    }

    return ret;
}

static u8 wifi_switch_mode(u8 mode, u8 *ssid, u8 *pwd)
{

    u8 rscorr = ERR_NULL;

    if (!ssid || !pwd || mode < STA_MODE || mode > P2P_MODE) {
        return ERR_PARAMS;
    }
    log_info("mode = %d, ssid = %s, pwd = %s\n", mode, ssid, pwd);

    switch (mode) {
    case 1://STA_MODE
        wifi_network_sta_connect(ssid, pwd);
        conn_status = 0;
        os_sem_set(&conn_sem, 0);
        os_sem_pend(&conn_sem, 0);
        rscorr = conn_status ? ERR_NULL : ERR_DEV_FAULT;
        break;

    case 2://AP_MODE
        //rscorr = wifi_enter_ap_mode(ssid, pwd) ? ERR_DEV_FAULT : ERR_NULL;
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }
    return rscorr;
}


static void ping_cb(void *priv, u32 cnt, u32 time)
{
    u8 *pingstr;
    s32 interval;
    static u32 last_cnt = 0;

    if (!cnt) {
        last_cnt = cnt;
        return;
    }

    interval = (cnt != last_cnt) ? time : -999;

    asprintf(&pingstr, "{\"opcode\":\"%d\",\"rscorr\":\"%d\",\"params\":{\"id\":\"%d\",\"type\":\"%d\",\"cmd\":\"%d\",\"args\":{\"idx\":\"%d\",\"interval\":\"%d\"}}}", \
             OPC_DEV_CTL, ERR_NULL, 0, DEV_TYPE_WIFI, CTL_NETWORK_PING_RSP, cnt, interval);

    log_info("--->resp ping\n\n%s\n\n", pingstr);
    data_respond(0, DATA_TYPE_OPCODE, pingstr, strlen(pingstr));

    if (pingstr) {
        free(pingstr);
    }
    last_cnt = cnt;
    ping_on = (ping_on && cnt >= ping_total) ? FALSE : TRUE;
}


static u8 wifi_control_handler(u8 id, cmd, json_object *args_obj)
{
    char *rssi;
    u8 *evm, *mac, mac_str[24], strength[8];
    u32 num = 0, entry_num = 0;
    struct netdev_ssid_info *info;
    u8 rscorr = ERR_NULL, mode, *ssid, *pwd, *array_str;
    json_object *mode_obj, *ssid_obj, *pwd_obj, *scan_obj, *array_obj, \
    *sub_obj, *entry_obj, *ip_obj, *interval_obj, *count_obj;

    //while (!(wifi_is_on())) {
    //    os_time_dly(1);
    //}

    args_str = NULL;

    switch (cmd) {
    case CTL_GET_DEV_INFO:
        break;

    case CTL_WIFI_CHANGE_MODE:
        mode_obj = json_object_object_get(args_obj, "mode");
        ssid_obj = json_object_object_get(args_obj, "ssid");
        pwd_obj  = json_object_object_get(args_obj, "pwd");
        if (!mode_obj || !ssid_obj || !pwd_obj) {
            rscorr = ERR_PARAMS;
            break;
        }

        mode = json_object_get_int(mode_obj);
        ssid = json_object_get_string(ssid_obj);
        pwd  = json_object_get_string(pwd_obj);
        rscorr = wifi_switch_mode(mode, ssid, pwd);
        break;

    case CTL_WIFI_LAUNCH_SCAN:
        rscorr = wifi_network_req_scan() ? ERR_DEV_FAULT : ERR_NULL;
        break;

    case CTL_WIFI_GET_SCAN_RES:
        if (!(info = malloc(WIFI_SCAN_RES_CNT * sizeof(struct netdev_ssid_info)))) {
            rscorr = ERR_MALLOC_FAIL;
            break;
        }
        if (!(num = wifi_network_get_scan_ssid(info, WIFI_SCAN_RES_CNT))) {
            rscorr = ERR_NULL;
            break;
        }

        array_obj = json_object_new_array();
        log_info("wifi_sta_scan ssid_num =%d \r\n", num);
        for (int i = 0; i < num; i++) {
            sub_obj = json_object_new_object();
            json_object_object_add(sub_obj, "ssid", json_object_new_string(info[i].ssid));
            json_object_object_add(sub_obj, "rssi", json_object_new_string(json_itoa(info[i].SignalStrength, 10)));
            json_object_object_add(sub_obj, "snr", json_object_new_string(json_itoa(info[i].SignalQuality, 10)));
            json_object_array_add(array_obj, sub_obj);
        }

        asprintf(&args_str, "%s", json_object_to_json_string(array_obj));
        free(info);
        break;

    case CTL_WIFI_GET_STA_ENTRY_INFO:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;

    case CTL_NETWORK_PING_START:
#ifdef CONFIG_WIFI_ENABLE
#if LWIP_RAW
        if (!conn_status) {
            rscorr = ERR_NETWORK_DISCONNECT;
            break;
        }
        if (ping_on) {
            rscorr = ERR_NULL;
            break;
        }
        ip_obj = json_object_object_get(args_obj, "ip");
        interval_obj = json_object_object_get(args_obj, "interval");
        count_obj  = json_object_object_get(args_obj, "count");
        if (!ip_obj || !interval_obj || !count_obj) {
            rscorr = ERR_PARAMS;
            break;
        }

        rscorr = ping_init(json_object_get_string(ip_obj), \
                           json_object_get_int(interval_obj), \
                           json_object_get_int(count_obj), \
                           ping_cb, NULL) ? ERR_DEV_FAULT : ERR_NULL;
        ping_on = rscorr == ERR_NULL ? TRUE : FALSE;
        ping_total = json_object_get_int(count_obj);
#else
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
#endif
#endif
        break;

    case CTL_WIFI_GET_STA_CONN_INFO:
        if (!conn_status) {
            rscorr = ERR_NETWORK_DISCONNECT;
            break;
        }

        memset(strength, 0, sizeof(strength));
        sprintf(strength, "%d%%", get_rx_signal());

        sub_obj = json_object_new_object();
        json_object_object_add(sub_obj, "ssid", json_object_new_string(wifi_module_get_sta_ssid()));
        json_object_object_add(sub_obj, "rssi", json_object_new_string(strength));
        asprintf(&args_str, "%s", json_object_to_json_string(sub_obj));
        json_object_put(sub_obj);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV_CMD;
        break;
    }

    return rscorr;
}
#endif


static u8 devices_list_check(u8 type)
{
    char *str;
    u8 array_cnt;
    json_object *new_obj, *list_obj, *dev_obj;

    asprintf(&str, "{%s}", DEVICES_LIST);
    new_obj = json_tokener_parse(str);
    ASSERT(new_obj != NULL);

    list_obj = json_object_object_get(new_obj, "devs_list");
    ASSERT(list_obj != NULL);

    array_cnt = json_object_array_length(list_obj);
    ASSERT(array_cnt != 0);

    for (u8 i = 0; i < array_cnt; i++) {
        dev_obj = json_object_array_get_idx(list_obj, i);
        if (type == json_object_get_int(json_object_object_get(dev_obj, "type"))) {
            free(str);
            json_object_put(new_obj);
            return TRUE;
        }
    }

    free(str);
    json_object_put(new_obj);
    return FALSE;
}


u8 devices_control_handler(json_object *params_obj)
{
    u8 id, type, cmd, rscorr;
    json_object *id_obj, *type_obj, *cmd_obj, *args_obj;

    id_obj   = json_object_object_get(params_obj, "id");
    type_obj = json_object_object_get(params_obj, "type");
    cmd_obj  = json_object_object_get(params_obj, "cmd");
    args_obj = json_object_object_get(params_obj, "args");

    if (!id_obj || !type_obj || !cmd_obj || !args_obj) {
        return ERR_PARAMS;
    }

    id   = json_object_get_int(id_obj);
    type = json_object_get_int(type_obj);
    cmd  = json_object_get_int(cmd_obj);

    log_info("%s, id = %d, type = %d, cmd = %d\n", __func__, id, type, cmd);

    if (!devices_list_check(type)) {
        return ERR_NO_SUPPORT_DEV;
    }

    switch (type) {
    case DEV_TYPE_SD:
        rscorr = sd_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_LCD:
        rscorr = lcd_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_MIC:
        break;

    case DEV_TYPE_SPEAKER:
        break;

    case DEV_TYPE_CAMERA:
        rscorr = camera_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_KEYPAD:
        break;

    case DEV_TYPE_BATTERY:
        rscorr = battery_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_WIFI:
#ifdef CONFIG_WIFI_ENABLE
        rscorr = wifi_control_handler(id, cmd, args_obj);
#else
        rscorr = ERR_NO_SUPPORT_DEV;
#endif
        break;

    case DEV_TYPE_PIR:
        rscorr = pir_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_MOTOR:
        rscorr = motor_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_GSENSOR:
        rscorr = gsensor_control_handler(id, cmd, args_obj);
        break;

    case DEV_TYPE_TOUCHPANEL:
        rscorr = touchpanel_control_handler(id, cmd, args_obj);
        break;

    default:
        rscorr = ERR_NO_SUPPORT_DEV;
        break;
    }

    return rscorr;
}


u8 devices_module_init(void)
{
    u8 rscorr;

#ifdef CONFIG_WIFI_ENABLE
    struct cfg_info info = {0};

    os_sem_create(&conn_sem, 0);
    wifi_dev = dev_open("wifi", NULL);
    if (wifi_dev) {
        info.cb = network_user_callback;
        info.net_priv = NULL;
        dev_ioctl(wifi_dev, DEV_SET_CB_FUNC, (u32)&info);

        info.tx_pwr_lmt_enable = 0;//  解除WIFI发送功率限制
        dev_ioctl(wifi_dev, DEV_SET_WIFI_TX_PWR_LMT_ENABLE, (u32)&info);
        info.tx_pwr_by_rate = 1;// 设置WIFI根据不同datarate打不同power
        dev_ioctl(wifi_dev, DEV_SET_WIFI_TX_PWR_BY_RATE, (u32)&info);
        dev_ioctl(wifi_dev, DEV_NETWORK_START, 0);
    }

#endif
    rscorr = product_lcd_init();
    rscorr = product_pir_init();
    rscorr = product_motor_init();
    rscorr = product_gsensor_init();
    rscorr = product_touchpanel_init();
    ASSERT(rscorr == ERR_NULL);
    return rscorr;
}


#endif


