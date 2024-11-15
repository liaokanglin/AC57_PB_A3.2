
#include "server/wifi_connect.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "os/os_compat.h"
#include "wifi_ext.h"
#include "system/init.h"
#include "lwip.h"
#include "server/ctp_server.h"
#include "server/net_server.h"
#include "device/device.h"
#include "system/app_core.h"
#include "server/server_core.h"
#include "action.h"
#include "system/timer.h"
#include "http/http_server.h"
#include "asm/debug.h"
#include "net_config.h"
#include "app_config.h"
#include "ftpserver/stupid-ftpd.h"
#include "device/wifi_dev.h"
#include "server/network_mssdp.h"
#include "streaming_media_server/fenice_config.h"
#include "system/timer.h"
#include "server/video_rt_tcp.h"
#include "server/net2video.h"
#include "wifi_ext.h"
#include "eg_http_server.h"
#include "dev_desc.h"
#include "database.h"
#include "wifi-tools/voiceprint_cfg.h"


extern unsigned int time_lapse(unsigned int *handle, unsigned int time_out);
extern int http_virfile_reg(const char *path, const char *contents, unsigned long len);

struct fenice_config conf;
extern int user_cmd_cb(int cmd, char *buffer, int len, void *priv);
static struct server *ctp = NULL;
static u8 _net_dhcp_ready ;
static u8 mac_addr[6];
static char ssid[64];
static void *wifi_dev = NULL;
static void *cli_hdl = NULL;
//CTP CDP 端口号

#define WIFI_APP_TASK_NAME "wifi_app_task"

extern u8 airkiss_calcrc_bytes(u8 *p, unsigned int num_of_bytes);

enum WIFI_APP_MSG_CODE {
    WIFI_MSG_TICK_1_SEC,                // 每秒定时器消息
    WIFI_MSG_SMP_CFG_START,             // WiFi配置开始
    WIFI_MSG_SMP_CFG_STOP,              // WiFi配置停止
    WIFI_MSG_SMP_CFG_COMPLETED,         // WiFi配置完成
    WIFI_MSG_SMP_CFG_TIMEOUT,           // WiFi配置超时
    WIFI_MSG_STA_SCAN_COMPLETED,        // WiFi站点扫描完成
    WIFI_MSG_STA_NETWORK_STACK_DHCP_SUCC, // DHCP获取IP成功
    WIFI_MSG_STA_DISCONNECTED,          // WiFi站点模式断开
    WIFI_MSG_AP_DISCONNECTED,           // WiFi AP模式断开
};



static struct ctp_server_info server_info = {
    .ctp_vaild = true,                 // 表示 CTP 是否有效（注意拼写应该是 'valid'）
    .ctp_port = CTP_CTRL_PORT,         // CTP 控制端口的值
    .cdp_vaild = true,                 // 表示 CDP 是否有效（注意拼写应该是 'valid'）
    .cdp_port = CDP_CTRL_PORT,         // CDP 控制端口的值
    .k_alive_type = CTP_ALIVE,         // 保活类型设置为 CTP_ALIVE
    /*.k_alive_type = CDP_ALIVE,*/     // 备用设置的保活类型为 CDP_ALIVE，当前被注释掉
};



static struct airkiss_result {
    struct smp_cfg_result result;  // 嵌套结构体，存储配网结果
    char scan_ssid_found;          // 一个字符变量，表示是否找到扫描到的SSID
} airkiss_result;


static struct voiceprint_result {
    char rand_str[8];  // 一个字符数组，用于存储一个8个字符的随机字符串
} voiceprint_result;



#ifdef CONFIG_STATIC_IPADDR_ENABLE
#define VM_STA_IPADDR_INDEX  15

static u8 use_static_ipaddr_flag;  // 静态 IP 地址标志，控制是否使用静态 IP


struct sta_ip_info {
    u8 ssid[33];      // 存储 SSID（最多 32 字符 + 1 字符 '\0'，即字符串长度为 33）
    u32 ip;           // 存储静态 IP 地址（32 位，通常是一个 IPv4 地址）
    u32 gw;           // 存储网关地址（32 位 IPv4 地址）
    u32 netmask;      // 存储子网掩码地址（32 位 IPv4 地址）
    u32 dns;          // 存储 DNS 地址（32 位 IPv4 地址）
    u8 gw_mac[6];     // 存储网关的 MAC 地址（6 字节）
    u8 local_mac[6];  // 存储本地设备的 MAC 地址（6 字节）
    u8 chanel;        // 存储无线网络频道号（一个字节，通常为整数值）
};


static void wifi_set_sta_ip_info(void)
{
    // 定义一个用于存储 STA IP 信息的结构体
    struct sta_ip_info  sta_ip_info;
    // 从虚拟内存中读取 STA IP 地址信息并存储到 sta_ip_info 结构体中
    db_select_buffer(VM_STA_IPADDR_INDEX, (char *) &sta_ip_info, sizeof(struct sta_ip_info));

    // 初始化一个 lan_setting 结构体并设置无线网络的 IP 地址、子网掩码和网关
    struct lan_setting lan_setting_info = {
        // 设置无线网络的 IP 地址
        .WIRELESS_IP_ADDR0  = (u8)(sta_ip_info.ip >> 0),  // 获取 IP 地址的最低字节
        .WIRELESS_IP_ADDR1  = (u8)(sta_ip_info.ip >> 8),  // 获取 IP 地址的次低字节
        .WIRELESS_IP_ADDR2  = (u8)(sta_ip_info.ip >> 16), // 获取 IP 地址的次高字节
        .WIRELESS_IP_ADDR3  = (u8)(sta_ip_info.ip >> 24), // 获取 IP 地址的最高字节

        // 设置无线网络的子网掩码
        .WIRELESS_NETMASK0  = (u8)(sta_ip_info.netmask >> 0),  // 获取子网掩码的最低字节
        .WIRELESS_NETMASK1  = (u8)(sta_ip_info.netmask >> 8),  // 获取子网掩码的次低字节
        .WIRELESS_NETMASK2  = (u8)(sta_ip_info.netmask >> 16), // 获取子网掩码的次高字节
        .WIRELESS_NETMASK3  = (u8)(sta_ip_info.netmask >> 24), // 获取子网掩码的最高字节

        // 设置无线网络的网关地址
        .WIRELESS_GATEWAY0   = (u8)(sta_ip_info.gw >> 0),  // 获取网关地址的最低字节
        .WIRELESS_GATEWAY1   = (u8)(sta_ip_info.gw >> 8),  // 获取网关地址的次低字节
        .WIRELESS_GATEWAY2   = (u8)(sta_ip_info.gw >> 16), // 获取网关地址的次高字节
        .WIRELESS_GATEWAY3   = (u8)(sta_ip_info.gw >> 24), // 获取网关地址的最高字节
    };

    // 调用函数设置局域网信息
    net_set_lan_info(&lan_setting_info);
}


static void store_dhcp_ipaddr(void)
{
    // 定义一个用于存储 STA IP 信息的结构体，并初始化为 0
    struct sta_ip_info  sta_ip_info = {0};
    u8 sta_channel; // 定义用于存储当前 STA 渠道的变量
    u8 local_mac[6]; // 用于存储本地 MAC 地址的数组
    u8 gw_mac[6]; // 用于存储网关 MAC 地址的数组

    // 如果使用的是静态 IP 地址，则不需要保存 DHCP 获取的 IP 地址
    if (use_static_ipaddr_flag) { // 记忆 IP 匹配成功，不需要重新保存
        return;
    }

    // 获取网络接口信息并存储到 netif_info 结构体中
    struct netif_info netif_info;
    lwip_get_netif_info(1, &netif_info);

    // 获取当前 WiFi 配置信息和当前信道
    struct cfg_info info = {0};
    info.mode = STA_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&info); // 获取当前 WiFi 信息
    dev_ioctl(wifi_dev, DEV_GET_WIFI_CHANNEL, (u32)&info);  // 获取 WiFi 信道

    // 存储当前信道信息
    sta_channel = info.sta_channel;
    // 获取本地和网关的 MAC 地址
    wifi_get_mac(local_mac);
    wifi_get_bssid(gw_mac);

    // 复制 SSID 和 MAC 地址信息
    strcpy(sta_ip_info.ssid, info.ssid); // 复制 SSID
    memcpy(sta_ip_info.gw_mac, gw_mac, 6); // 复制网关 MAC 地址
    memcpy(sta_ip_info.local_mac, local_mac, 6); // 复制本地 MAC 地址
    // 赋值 IP 地址、子网掩码、网关和 DNS 服务器信息
    sta_ip_info.ip =  netif_info.ip;
    sta_ip_info.netmask =  netif_info.netmask;
    sta_ip_info.gw =  netif_info.gw;
    sta_ip_info.chanel = sta_channel;
    sta_ip_info.dns = *(u32 *)dns_getserver(0); // 获取 DNS 服务器地址

    // 更新保存 STA IP 信息到虚拟内存
    db_update_buffer(VM_STA_IPADDR_INDEX, (char *) &sta_ip_info, sizeof(struct sta_ip_info));
    // 打印保存操作完成的提示
    puts("store_dhcp_ipaddr\r\n");
}


static int compare_dhcp_ipaddr(void)
{
    // 初始化标志，表示不使用静态 IP 地址
    use_static_ipaddr_flag = 0;

    int ret; // 用于存储函数调用返回值
    u8 local_mac[6]; // 定义用于存储本地 MAC 地址的数组
    u8 gw_mac[6]; // 定义用于存储网关 MAC 地址的数组
    u8 sta_channel; // 存储当前信道
    struct sta_ip_info  sta_ip_info; // 定义 STA IP 信息的结构体
    struct netif_info netif_info; // 定义网络接口信息的结构体

    // 从虚拟内存中读取存储的 STA IP 信息
    ret = db_select_buffer(VM_STA_IPADDR_INDEX, (char *) &sta_ip_info, sizeof(struct sta_ip_info));
    if (ret < 0) {
        // 如果读取失败，打印提示信息并返回 -1
        puts("compare_dhcp_ipaddr NO VM_STA_IPADDR_INDEX\r\n");
        return -1;
    }

    // 获取网络接口信息
    lwip_get_netif_info(1, &netif_info);

    // 获取当前 WiFi 配置信息和当前信道
    struct cfg_info info = {0};
    info.mode = STA_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&info); // 获取当前 WiFi 信息
    dev_ioctl(wifi_dev, DEV_GET_WIFI_CHANNEL, (u32)&info);  // 获取 WiFi 信道

    // 存储当前信道
    sta_channel = info.sta_channel;

    // 获取本地和网关的 MAC 地址
    wifi_get_bssid(gw_mac);
    wifi_get_mac(local_mac);

    // 比较 SSID、本地 MAC 地址、网关 MAC 地址是否与存储的信息匹配
    if (!strcmp(info.ssid, sta_ip_info.ssid)
        && !memcmp(local_mac, sta_ip_info.local_mac, 6)
        && !memcmp(gw_mac, sta_ip_info.gw_mac, 6)
        /*&& sta_ip_info.gw==sta_ip_info.dns//如果路由器没接网线/没联网,每次连接都去重新获取DHCP*/
       ) {
        // 如果匹配成功，设置标志并打印提示信息
        use_static_ipaddr_flag = 1;
        puts("compare_dhcp_ipaddr Match\r\n");
        return 0;
    }

    // 如果不匹配，打印详细的对比信息并返回 -1
    printf("compare_dhcp_ipaddr not Match!!! [%s][%s],[0x%x,0x%x][0x%x,0x%x],[0x%x] \r\n", 
           info.ssid, sta_ip_info.ssid, local_mac[0], local_mac[5], 
           sta_ip_info.local_mac[0], sta_ip_info.local_mac[5], 
           sta_ip_info.dns);

    return -1;
}


void dns_set_server(u32 *dnsserver)
{
    // 定义一个用于存储 STA IP 信息的结构体
    struct sta_ip_info  sta_ip_info;
    // 从虚拟内存中读取存储的 STA IP 信息
    if (db_select_buffer(VM_STA_IPADDR_INDEX, (char *) &sta_ip_info, sizeof(struct sta_ip_info)) < 0) {
        // 如果读取失败，将 dnsserver 指针指向的值设置为 0
        *dnsserver = 0;
    } else {
        // 如果读取成功，将 STA IP 信息中的 DNS 服务器地址赋值给 dnsserver 指针
        *dnsserver = sta_ip_info.dns;
    }
}


#endif //CONFIG_STATIC_IPADDR_ENABL


int net_dhcp_ready()
{
    // 返回 DHCP 网络是否准备就绪的状态
    return (_net_dhcp_ready);
}

static void wifi_app_timer_func(void *p)
{
    // 向 WiFi 应用任务队列发送一个 "1 秒滴答" 消息
    os_taskq_post(WIFI_APP_TASK_NAME, 1, WIFI_MSG_TICK_1_SEC);
}

static void wifi_taskq_post(int msg)
{
    int ret = 0; // 存储函数调用返回值
    u8 retry = 0; // 用于记录重试次数

    do {
        // 向 WiFi 应用任务队列发送消息
        ret = os_taskq_post(WIFI_APP_TASK_NAME, 1, msg);

        if (ret == OS_NO_ERR) { // 如果发送成功，退出循环
            break;
        }

        // 任务队列发送失败，休眠 50 毫秒后重试
        msleep(50);
        retry++;
    } while (retry < 5); // 最多重试 5 次

    // 如果消息发送仍然失败，打印错误提示
    if (ret != OS_NO_ERR) {
        printf("post msg %d to wifi_app_task fail !!! \n", msg);
    }
}


void wifi_smp_connect(char *ssid, char *pwd, void *rand_str)
{
    // 定义 WiFi 配置信息结构体并初始化为 0
    struct cfg_info info = {0};

    // 如果提供了 SSID
    if (ssid) {
        // 复制随机字符串到全局变量
        strcpy(voiceprint_result.rand_str, rand_str);
        // 设置 WiFi 连接模式为 STA（客户端）模式
        info.mode = STA_MODE;
        // 设置 SSID 和密码
        info.ssid = ssid;
        info.pwd = pwd;
        // 调用设备控制接口设置为 STA 模式
        dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
        // 改变保存模式
        dev_ioctl(wifi_dev, DEV_CHANGE_SAVING_MODE, (u32)&info);
    } else {
        // 获取 WiFi SMP 配置结果
        dev_ioctl(wifi_dev, DEV_GET_WIFI_SMP_RESULT, (u32)&info);

        // 如果有有效的 SMP 配置信息
        if (info.smp_cfg.type) {
            // 打印 SMP 配置信息
            printf("\r\n AIRKISS INFO, SSID = %s, PWD = %s, ssid_crc = 0x%x, ran_val = 0x%x \r\n", 
                   info.smp_cfg.ssid, info.smp_cfg.passphrase, info.smp_cfg.ssid_crc, info.smp_cfg.random_val);
            // 保存 AirKiss 结果
            airkiss_result.result.type = 1;
            airkiss_result.result.ssid_crc = info.smp_cfg.ssid_crc;
            airkiss_result.result.random_val = info.smp_cfg.random_val;
            strcpy(airkiss_result.result.ssid, info.smp_cfg.ssid);
            strcpy(airkiss_result.result.passphrase, info.smp_cfg.passphrase);

            // 如果配置类型不匹配或者 SSID 校验和匹配
            if (info.smp_cfg.type != AIRKISS_SMP_CFG
                || airkiss_result.result.ssid_crc == airkiss_calcrc_bytes((u8 *)airkiss_result.result.ssid, strlen(airkiss_result.result.ssid))) {
                airkiss_result.scan_ssid_found = 1;
                info.mode = STA_MODE;
                info.ssid = info.smp_cfg.ssid;
                info.pwd = info.smp_cfg.passphrase;
                dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
                dev_ioctl(wifi_dev, DEV_CHANGE_SAVING_MODE, (u32)&info);
            } else {
                // 否则进行网络扫描
                dev_ioctl(wifi_dev, DEV_NET_SCANF, 0);
            }
        } else {
            // 无有效配置时设置默认的 STA 模式
            info.mode = STA_MODE;
            info.ssid = info.smp_cfg.ssid;
            info.pwd = info.smp_cfg.passphrase;
            dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
            dev_ioctl(wifi_dev, DEV_CHANGE_SAVING_MODE, (u32)&info);
        }
    }
}


static void airkiss_ssid_check(void)
{
    u32 i;
    struct cfg_info info = {0};

    // 如果 AirKiss 结果无效或者已找到匹配的 SSID，直接返回
    if (airkiss_result.result.type == 0 || airkiss_result.scan_ssid_found) {
        return;
    }

    // 获取当前 STA 模式下的 SSID 信息
    dev_ioctl(wifi_dev, DEV_GET_STA_SSID_INFO, (u32)&info);

    // 遍历所有存储的 SSID
    for (i = 0; i < info.sta_ssid_num; i++) {
        // 检查当前 SSID 是否与 AirKiss 结果的 SSID 部分匹配
        if (!strncmp(airkiss_result.result.ssid, info.sta_ssid_info[i].ssid, strlen(airkiss_result.result.ssid))) {
CHECK_AIRKISS_SSID_CRC:
            // 如果 CRC 校验匹配，则表明找到了对应的 AirKiss SSID
            if (airkiss_result.result.ssid_crc == airkiss_calcrc_bytes((u8 *)info.sta_ssid_info[i].ssid, strlen(info.sta_ssid_info[i].ssid))) {
                printf("find airkiss ssid = [%s]\r\n", info.sta_ssid_info[i].ssid);
                // 将匹配到的 SSID 复制到 AirKiss 结果中
                strcpy(airkiss_result.result.ssid, info.sta_ssid_info[i].ssid);
                // 标记为找到匹配的 SSID
                airkiss_result.scan_ssid_found = 1;

                // 设置 WiFi 配置为 STA 模式，并连接到找到的 SSID
                info.mode = STA_MODE;
                info.ssid = airkiss_result.result.ssid;
                info.pwd = airkiss_result.result.passphrase;
                dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
                // 改变保存模式
                dev_ioctl(wifi_dev, DEV_CHANGE_SAVING_MODE, (u32)&info);

                return;
            }
        } else {
            // 未找到匹配的 SSID 注释部分
            /*goto CHECK_AIRKISS_SSID_CRC;*/
        }
    }

    // 未找到匹配的 SSID，打印错误信息
    printf("cannot found airkiss ssid[%s] !!! \n\n", airkiss_result.result.ssid);
}



static void airkiss_broadcast(void)
{
    int i, ret;
    int onOff = 1;
    int sock;
    struct sockaddr_in dest_addr;

    // 如果 AirKiss 结果类型无效，直接返回
    if (airkiss_result.result.type == 0) {
        return;
    }

    puts("airkiss_broadcast random_val \n");

    // 创建 UDP 套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        printf("%s %d->Error in socket()\n", __FUNCTION__, __LINE__);
        goto EXIT;
    }

    // 初始化目的地址信息
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_port = 0;

    // 绑定套接字
    ret = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (ret == -1) {
        printf("%s %d->Error in bind()\n", __FUNCTION__, __LINE__);
        goto EXIT;
    }

    // 设置套接字选项为允许广播
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&onOff, sizeof(onOff));

    if (ret == -1) {
        printf("%s %d->Error in setsockopt() SO_BROADCAST\n", __FUNCTION__, __LINE__);
        goto EXIT;
    }

    // 设置广播地址
    inet_pton(AF_INET, "255.255.255.255", &dest_addr.sin_addr.s_addr);
    dest_addr.sin_port = htons(10000);

    // 发送广播消息 8 次
    for (i = 0; i < 8; i++) {
        ret = sendto(sock, (unsigned char *)&airkiss_result.result.random_val, 1, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
        if (ret == -1) {
            printf("%s %d->Error in sendto\n", __FUNCTION__, __LINE__);
        }

        // 每次发送后休眠 20 毫秒
        msleep(20);
    }

    // 清空 AirKiss 结果结构体
    memset(&airkiss_result, 0, sizeof(airkiss_result));

EXIT:
    // 关闭套接字
    if (sock != -1) {
        closesocket(sock);
    }
}



static unsigned int best_ch, least_cnt = -1;

static void get_best_ch_fn(unsigned int ch, unsigned int cnt)
{
    // 检查信道编号是否在限制范围内（禁止使用 13 信道及以上）
    if (ch >= 12) {
        return;
    }

    // 如果当前信道的计数值比当前记录的最小计数值小
    // 表示当前信道的干扰较小，将其作为最佳信道
    if (cnt < least_cnt) {
        least_cnt = cnt;
        best_ch = ch;
    }
}


#if defined CONFIG_NET_CLIENT

// 外部声明用于断开所有视频客户端连接的函数
extern void video_disconnect_all_cli();

// Wi-Fi IPC 状态回调函数
static int wifi_ipc_state_cb(void *priv, int on)
{
    if (on) {
        // 当连接状态为 "on" 时执行的操作
        // 目前为空，没有实现特定的操作
    } else {
        // 当连接状态为 "off" 时，断开所有视频客户端连接
        video_disconnect_all_cli();
    }

    return 0;
}

#endif

#if defined CONFIG_NET_SERVER

// 外部声明用于连接和断开 IPC 的函数
extern void ipc_connect();
extern void ipc_disconnect();

// Wi-Fi IPC 状态回调函数
static int wifi_ipc_state_cb(void *priv, int on)
{
    if (on) {
        // 当连接状态为 "on" 时，执行 IPC 连接
        ipc_connect();
    } else {
        // 当连接状态为 "off" 时，执行 IPC 断开
        ipc_disconnect();
    }

    return 0;
}

#endif


void wifi_enter_smp_cfg_mode(void)
{
    struct cfg_info info = {0};
    info.timeout = 100;  // 设置配置超时时间为 100 秒

    /* 注释掉的代码，可能是取消平台初始化的部分 */
    /* extern tutk_platform_uninit(); */
    /* tutk_platform_uninit(); */

    /*dev_ioctl(wifi_dev, DEV_SET_SMP_AIRKISS_AES_ON_OFF, 1);*/ // 注释掉的代码，可能是开启 AirKiss AES 加密

    // 进入 SMP 配置模式
    dev_ioctl(wifi_dev, DEV_SMP_MODE, 0);

    // 设置 SMP 配置的超时设置
    dev_ioctl(wifi_dev, DEV_SET_SMP_CONFIG_TIMEOUT_SEC, (u32)&info);
}

int get_wifi_is_smp_mode(void)
{
    struct cfg_info info = {0};
    info.mode = NONE_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&info);
    return info.mode == SMP_CFG_MODE;
}

void wifi_return_sta_mode(void)
{
    struct cfg_info info = {0};
    info.mode = STA_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&info);
    dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
}


static void wifi_smp_connect_task(void *p)
{
    struct cfg_info info;
    dev_ioctl(wifi_dev, DEV_GET_WIFI_SMP_RESULT, (u32)&info);

    info.mode = STA_MODE;
    dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
}

u8 *wifi_get_module_mac_addr(void)
{
    return (u8 *)&mac_addr;
}

static void wifi_sta_to_ap_mode_change(void)//用在STA模式密码不对或者找不到SSID，自动切换AP模式
{
    struct cfg_info info;
    info.mode = AP_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&info);

    if (!strcmp("", info.ssid)) {
        dev_ioctl(wifi_dev, DEV_GET_MAC, (u32)&mac_addr);
        sprintf(ssid, WIFI_CAM_PREFIX"%02x%02x%02x%02x%02x%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        info.ssid = ssid;
        info.pwd = "";
    }

    info.mode = AP_MODE;
    info.force_default_mode = 1;
    dev_ioctl(wifi_dev, DEV_SAVE_DEFAULT_MODE, (u32)&info);
    puts("\n\nwifi_sta_to_ap_mode_change->STA to AP , system reset...\n\n");
    cpu_reset();
}
static void wifi_first_setup(void)
{
    struct cfg_info info;
    info.mode = AP_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&info);
    dev_ioctl(wifi_dev, DEV_GET_MAC, (u32)&mac_addr);

    if (!strstr(info.ssid, WIFI_CAM_NAME_MATCH)) {
        sprintf(ssid, WIFI_CAM_PREFIX"%02x%02x%02x%02x%02x%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        info.ssid = ssid;
        info.pwd = WIFI_CAM_WIFI_PWD; 
        info.mode = AP_MODE;
        info.force_default_mode = 1;
        dev_ioctl(wifi_dev, DEV_SAVE_DEFAULT_MODE, (u32)&info);
    } else {
        info.mode = AP_MODE;
        info.force_default_mode = 0;
        dev_ioctl(wifi_dev, DEV_SAVE_DEFAULT_MODE, (u32)&info);
    }
}


//#define NET_LOG_ENABLE

#if defined NET_LOG_ENABLE

char log_buf[100 * 1024];
u32 log_cnt = 0;




#include "sock_api/sock_api.h"



void putbyte(char c)
{
    log_buf[(log_cnt++) % (sizeof(log_buf))] = c;
}



static void net_log_task(void *arg)
{


    struct sockaddr_in dest_addr;
    int ret = 0;
    void *sock_hdl = sock_reg(AF_INET, SOCK_DGRAM, 0, NULL, NULL);

    dest_addr.sin_addr.s_addr = inet_addr("192.168.1.2");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(2345);
    while (1) {
        if (log_cnt > 0) {
            ret = sock_sendto(sock_hdl, log_buf, log_cnt, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (ret < 0) {
                printf("%s  %d\n", __func__, __LINE__);
            }
            memset(log_buf, 0, sizeof(log_buf));
            log_cnt = 0;
        }

        os_time_dly(30);

    }

}

#endif














static int network_user_callback(void *network_ctx, enum NETWORK_EVENT state, void *priv)
{

    struct cfg_info info;
    int ret = 0;

    switch (state) {

    case WIFI_EVENT_MODULE_INIT:

#if 0
        gpio_set_hd(IO_PORTG_00, 0);
        gpio_set_hd(IO_PORTG_01, 0);
        gpio_set_hd(IO_PORTG_02, 0);
        gpio_set_hd(IO_PORTG_03, 0);
        gpio_set_hd(IO_PORTG_04, 0);
        gpio_set_hd(IO_PORTG_05, 0);

#endif


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

        info.ssid = WIFI_CAM_PREFIX;
        info.pwd = "";
        info.mode = AP_MODE;
        info.force_default_mode = 0;
        dev_ioctl(wifi_dev, DEV_SET_DEFAULT_MODE, (u32)&info);
        break;

    case WIFI_EVENT_MODULE_START:
        puts("|network_user_callback->WIFI_EVENT_MODULE_START\n");

//void linked_info_dump(int padapter, u8 benable);
//void *netdev_priv(struct net_device *dev);
//void  *net_dev_find(void);
//linked_info_dump(*(int*)netdev_priv(net_dev_find()), 1);

#if 0 //是否使能自动扫描获取最佳信道
        int rtw_scan(void);W
        rtw_scan();//扫描1秒
        rtw_scan();//扫描1秒
        rtw_scan();//扫描1秒
        int rtw_get_best_ch(void (*get_best_ch_fn)(int, int));
        rtw_get_best_ch(get_best_ch_fn);
        printf("hostapd_set_wifi_channel = %d\r\n", best_ch);
        int hostapd_set_wifi_channel(int channel);
        hostapd_set_wifi_channel(best_ch);
#endif

#ifdef CONFIG_STA_MODE
    info.ssid = "CMW-AP";
    info.pwd = "12345678";//WIFI_CAM_WIFI_PWD;
    info.mode = STA_MODE;
    info.force_default_mode = 1;//1
    dev_ioctl(wifi_dev, DEV_SAVE_DEFAULT_MODE, (u32)&info);
    break;

#endif
//设置VM保存AP_MODE
#if defined (WIFI_CAM_SUFFIX)
        sprintf(ssid, WIFI_CAM_PREFIX WIFI_CAM_SUFFIX);
#else
        wifi_first_setup();

#if 1
        dev_ioctl(wifi_dev, DEV_GET_MAC, (u32)&mac_addr);
        sprintf(ssid, WIFI_CAM_PREFIX"%02x%02x%02x%02x%02x%02x"
                , mac_addr[0]
                , mac_addr[1]
                , mac_addr[2]
                , mac_addr[3]
                , mac_addr[4]
                , mac_addr[5]);
        info.ssid = ssid;
        info.pwd = WIFI_CAM_WIFI_PWD;
        info.mode = AP_MODE;
        info.force_default_mode = 1;
        dev_ioctl(wifi_dev, DEV_SAVE_DEFAULT_MODE, (u32)&info);
#endif
#endif
        break;

    case WIFI_EVENT_MODULE_STOP:
        puts("|network_user_callback->WIFI_EVENT_MODULE_STOP\n");
        break;

    case WIFI_EVENT_AP_START:
        puts("|network_user_callback->WIFI_EVENT_AP_START\n");

#if defined NET_LOG_ENABLE
        thread_fork("net_log_task", 10, 2048, 64, 0, net_log_task, NULL);
#endif

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
        wifi_taskq_post(WIFI_MSG_STA_DISCONNECTED);
        break;

    case WIFI_EVENT_STA_SCAN_COMPLETED:

        /* if(wpa_supplicant_get_state() != STA_WPA_CONNECT_COMPLETED) */
        /* { */
        puts("|network_user_callback->WIFI_STA_SCAN_COMPLETED\n");
        wifi_taskq_post(WIFI_MSG_STA_SCAN_COMPLETED);
        /* } */
        break;

    case WIFI_EVENT_STA_CONNECT_SUCC:
        dev_ioctl(wifi_dev, DEV_GET_WIFI_CHANNEL, (u32)&info);
        printf("|network_user_callback->WIFI_STA_CONNECT_SUCC,CH=%d\r\n", info.sta_channel);
#ifdef CONFIG_STATIC_IPADDR_ENABLE
        if (0 == compare_dhcp_ipaddr()) {
            wifi_set_sta_ip_info();
            ret = 1;
        }
#endif
        break;

    case WIFI_EVENT_MP_TEST_START:
        puts("|network_user_callback->WIFI_EVENT_MP_TEST_START\n");
        /*extern spec_uart_init(void);*/
        /*extern wifi_mp_uart_test(void);*/
        /*spec_uart_init();*/
        // extern int spec_uart_send(char *buf, u32 len);
        // set_putbyte_remap((void*)-1);   // spec_uart_send
        /*wifi_mp_uart_test();*/
        break;

    case WIFI_EVENT_MP_TEST_STOP:
        puts("|network_user_callback->WIFI_EVENT_MP_TEST_STOP\n");
        break;

    case WIFI_EVENT_STA_CONNECT_TIMEOUT_NOT_FOUND_SSID:
        puts("|network_user_callback->WIFI_STA_CONNECT_TIMEOUT_NOT_FOUND_SSID\n");
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
#ifdef CONFIG_STATIC_IPADDR_ENABLE
        store_dhcp_ipaddr();
#endif//CONFIG_STATIC_IPADDR_ENABLE
        wifi_taskq_post(WIFI_MSG_STA_NETWORK_STACK_DHCP_SUCC);
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
        wifi_taskq_post(WIFI_MSG_SMP_CFG_START);
        break;

    case WIFI_EVENT_SMP_CFG_STOP:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_STOP\n");
        wifi_taskq_post(WIFI_MSG_SMP_CFG_STOP);
        break;

    case WIFI_EVENT_SMP_CFG_TIMEOUT:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_TIMEOUT\n");
        wifi_taskq_post(WIFI_MSG_SMP_CFG_TIMEOUT);
        break;

    case WIFI_EVENT_SMP_CFG_COMPLETED:
        puts("|network_user_callback->WIFI_EVENT_SMP_CFG_COMPLETED\n");
        wifi_taskq_post(WIFI_MSG_SMP_CFG_COMPLETED);
        break;

    case WIFI_EVENT_PM_SUSPEND:
        puts("|network_user_callback->WIFI_EVENT_PM_SUSPEND\n");
        break;

    case WIFI_EVENT_PM_RESUME:
        puts("|network_user_callback->WIFI_EVENT_PM_RESUME\n");
        break;

    case WIFI_EVENT_AP_ON_ASSOC:
        ;
        struct eth_addr *hwaddr = (struct eth_addr *)network_ctx;
        printf("WIFI_EVENT_AP_ON_ASSOC hwaddr = %02x:%02x:%02x:%02x:%02x:%02x \r\n\r\n",
               hwaddr->addr[0], hwaddr->addr[1], hwaddr->addr[2], hwaddr->addr[3], hwaddr->addr[4], hwaddr->addr[5]);

        break;

    case WIFI_EVENT_AP_ON_DISCONNECTED:
        hwaddr = (struct eth_addr *)network_ctx;
        printf("WIFI_EVENT_AP_ON_DISCONNECTED hwaddr = %02x:%02x:%02x:%02x:%02x:%02x \r\n\r\n",
               hwaddr->addr[0], hwaddr->addr[1], hwaddr->addr[2], hwaddr->addr[3], hwaddr->addr[4], hwaddr->addr[5]);

#if __SDRAM_SIZE__ >= (8 * 1024 * 1024)
        /* 强制断开所有RTSP链接,实际上应该只断开响应客户端 */
        void stream_media_server_dhwaddr_close(struct eth_addr * dhwaddr);
        stream_media_server_dhwaddr_close(hwaddr);
#endif
        ctp_keep_alive_find_dhwaddr_disconnect((struct eth_addr *)hwaddr->addr);
        cdp_keep_alive_find_dhwaddr_disconnect((struct eth_addr *)hwaddr->addr);
        wifi_taskq_post(WIFI_MSG_AP_DISCONNECTED);
        break;

    default:
        break;
    }

    return ret;
}


static void wifi_set_lan_setting_info(void)
{
    struct lan_setting lan_setting_info = {

        .WIRELESS_IP_ADDR0  = 192,
        .WIRELESS_IP_ADDR1  = 168,
        .WIRELESS_IP_ADDR2  = 1,
        .WIRELESS_IP_ADDR3  = 1,

        .WIRELESS_NETMASK0  = 255,
        .WIRELESS_NETMASK1  = 255,
        .WIRELESS_NETMASK2  = 255,
        .WIRELESS_NETMASK3  = 0,

        .WIRELESS_GATEWAY0  = 192,
        .WIRELESS_GATEWAY1  = 168,
        .WIRELESS_GATEWAY2  = 1,
        .WIRELESS_GATEWAY3  = 1,

        .SERVER_IPADDR1  = 192,
        .SERVER_IPADDR2  = 168,
        .SERVER_IPADDR3  = 1,
        .SERVER_IPADDR4  = 1,

        .CLIENT_IPADDR1  = 192,
        .CLIENT_IPADDR2  = 168,
        .CLIENT_IPADDR3  = 1,
        .CLIENT_IPADDR4  = 2,

        .SUB_NET_MASK1   = 255,
        .SUB_NET_MASK2   = 255,
        .SUB_NET_MASK3   = 255,
        .SUB_NET_MASK4   = 0,
    };

    struct cfg_info info;
    info.__lan_setting_info = &lan_setting_info;
    dev_ioctl(wifi_dev, DEV_SET_LAN_SETTING, (u32)&info);
}


void net_app_init(void)
{
    ctp = server_open("ctp_server", (void *)&server_info);

    if (!ctp) {
        printf("ctp server fail\n");
    }

    puts("http server init\n");
    http_virfile_reg(DEV_DESC_PATH, DEV_DESC_CONTENT, strlen(DEV_DESC_CONTENT)); //注册虚拟文件描述文档,可在dev_desc.h修改
    http_get_server_init(HTTP_PORT); //8080

    printf("video preview playback init \n");
    preview_init(VIDEO_PREVIEW_PORT, NULL); //2226
    playback_init(VIDEO_PLAYBACK_PORT, NULL);
    video_rt_tcp_server_init(2229);


    printf("ftpd server init \n");
    extern void ftpd_vfs_interface_cfg(void);
    ftpd_vfs_interface_cfg();
    stupid_ftpd_init("MAXUSERS=2\nUSER=FTPX 12345678     0:/      2   A\n", NULL);

    printf("network mssdp init\n");
    network_mssdp_init();

#if 1
    //网络测试工具，使用iperf
    extern void iperf_test(void);
    iperf_test();
#endif

    /*
     *代码段功能:修改RTSP的URL
     *默认配置  :URL为rtsp://192.168.1.1/avi_pcm_rt/front.sd,//(avi_pcma_rt 传G7111音频)传JPEG实时流
     *
     */
#if 0
    char *user_custom_name = "avi_pcma_rt";
    char *user_custom_content =
        "stream\r\n \
	file_ext_name avi\r\n \
	media_source live\r\n \
	priority 1\r\n \
	payload_type 26\r\n \
	clock_rate 90000\r\n \
	encoding_name JPEG\r\n \
	coding_type frame\r\n \
	byte_per_pckt 1458\r\n \
	stream_end\r\n \
	stream\r\n \
	file_ext_name pcm\r\n \
	media_source live\r\n \
	priority 1\r\n \
	payload_type 8\r\n \
	encoding_name PCMA\r\n \
	clock_rate 8000\r\n \
	stream_end";
    extern void rtsp_modify_url(const char *user_custom_name, const char *user_custom_content);
    rtsp_modify_url(user_custom_name, user_custom_content);
#endif

    extern int stream_media_server_init(struct fenice_config * conf);
    extern int fenice_get_video_info(struct fenice_source_info * info);
    extern int fenice_get_audio_info(struct fenice_source_info * info);
    extern int fenice_set_media_info(struct fenice_source_info * info);
    extern int fenice_video_rec_setup(void);
    extern int fenice_video_rec_exit(void);
    struct fenice_config conf;

    strncpy(conf.protocol, "UDP", 3);
    conf.exit = fenice_video_rec_exit;
    conf.setup = fenice_video_rec_setup;
    conf.get_video_info = fenice_get_video_info;
    conf.get_audio_info = fenice_get_audio_info;
    conf.set_media_info = fenice_set_media_info;
    conf.port = RTSP_PORT;  // 当为0时,用默认端口554
    stream_media_server_init(&conf);


#ifdef CONFIG_EG_HTTP_SERVER
    eg_protocol_init();
#endif
}


void net_app_uninit(void)
{
    puts("ctp server init\n");
    server_close(ctp);

    puts("http server init\n");
    http_get_server_uninit(); //8080

    preview_uninit(); //2226

    playback_uninit();

    puts("ftpd server uninit\n");
    stupid_ftpd_uninit();


    video_rt_tcp_server_uninit();

#if __SDRAM_SIZE__ >= (8 * 1024 * 1024)
    extern void stream_media_server_uninit(void);
    stream_media_server_uninit();
#endif
}

void wifi_on(void)
{
    static u8 flag;
    dev_ioctl(wifi_dev, DEV_NETWORK_START, 0);
    if (!flag) {
        flag = 1;
        net_app_init();
    }
}


void wifi_off(void)
{
    //net_app_uninit();//不需要卸载网络服务
    dev_ioctl(wifi_dev, DEV_NETWORK_STOP, 0);
}



/* #define RTOS_STACK_CHECK_ENABLE */
void malloc_debug_start(void);
void malloc_debug_show(void);
void malloc_stats(void);
#ifdef RTOS_STACK_CHECK_ENABLE
static void rtos_stack_check_func(void *p)
{
    static char pWriteBuffer[4096];
    extern void vTaskList(char *pcWriteBuffer);
    vTaskList((char *)&pWriteBuffer);
    printf(" \n\ntask_name          task_state priority stack task_num\n%s\n", pWriteBuffer);
    malloc_stats();
    //extern void malloc_debug_start(void);
    //malloc_debug_start();
    //malloc_debug_show();
}
#endif

void wifi_app_task(void *priv)
{
    int res, err = 0;
    int msg[32];
    unsigned int timehdl = 0;
    struct cfg_info info = {0};

#ifdef PRODUCT_TEST_ENABLE
    u8 product_enter_check(void);
    if (product_enter_check()) {
        //进入产测模式后，将直接退出
        return;
    }
#endif

    printf(">>>>>>> wifi dev running <<<<<<<<\r\n");
#ifdef CONFIG_FORCE_RESET_VM
    extern void vm_eraser(void);
    vm_eraser();
#endif

    wifi_dev = dev_open("wifi", NULL);

    if (!wifi_dev) {
        printf(">>>>>>> wifi dev open err !!!! <<<<<<<<\r\n");
        return ;

    }

#ifdef RTOS_STACK_CHECK_ENABLE
    sys_timer_add(NULL, rtos_stack_check_func, 3 * 1000);
#endif
    info.cb = network_user_callback;
    info.net_priv = NULL;
    dev_ioctl(wifi_dev, DEV_SET_CB_FUNC, (u32)&info);

#if 0
    printf("\n >>>> DEV_SET_WIFI_POWER_SAVE<<<<   \n");
    dev_ioctl(wifi_dev, DEV_SET_WIFI_POWER_SAVE, 0);//打开就启用低功耗模式, 只有STA模式才有用
#endif

#if 1
    printf("\n >>>> DEV_SET_WIFI_TX_PWR_BY_RATE<<<   \n");

    info.tx_pwr_lmt_enable = 0;//  解除WIFI发送功率限制
    dev_ioctl(wifi_dev, DEV_SET_WIFI_TX_PWR_LMT_ENABLE, (u32)&info);
    info.tx_pwr_by_rate = 1;// 设置WIFI根据不同datarate打不同power
    dev_ioctl(wifi_dev, DEV_SET_WIFI_TX_PWR_BY_RATE, (u32)&info);
#endif

    wifi_set_lan_setting_info();

    wifi_on();

    extern char get_MassProduction(void);
    if (get_MassProduction()) {
        info.mode = STA_MODE;
        info.ssid = "xing";
        info.pwd = "12345678";
        dev_ioctl(wifi_dev, DEV_STA_MODE, (u32)&info);
    }

#if 0
    info.p2p_role = 0;
    dev_ioctl(wifi_dev, DEV_P2P_MODE, (u32)&info);
#endif

    sys_timer_add(NULL, wifi_app_timer_func, 1000);


    //os_time_dly(500);


    //extern int pthread_posix_test(void);
    //pthread_posix_test();

    /* gpio_direction_output(IO_PORTB_00,1); */
    /* gpio_direction_output(IO_PORTB_01,0); */
    /* msleep(10000); */


    /* printf("gpio G00=%d\n",gpio_read(IO_PORTG_00)); */
    /* printf("gpio G01=%d\n",gpio_read(IO_PORTG_01)); */
    /* msleep(2000); */


    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_ANTICLOCK, &timer_moto1, 550, 90, 1); */
    /* msleep(6000); */
    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_CLOCKWISE, &timer_moto1, 550, 90, 1); */
    /* msleep(6000); */
    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_ANTICLOCK, &timer_moto2, 400, 360, 2); */
    /* msleep(6000); */
    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_CLOCKWISE, &timer_moto2, 400, 360, 2); */
    /* msleep(6000); */


    /* if(!gpio_read(IO_PORTG_00)) { */
    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_CLOCKWISE,&timer_moto2,400,360 ,2); */
    /* } else if(!gpio_read(IO_PORTG_01)) { */
    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_ANTICLOCK,&timer_moto2,400,360 ,2); */
    /* } else { */
    /* step_moto_timer_ctrl(STEP_MOTO_ROTA_CLOCKWISE,&timer_moto2,400,90 ,2); */
    /* } */
    /* msleep(8000); */

    /* } */
    int ret = 0;
    while (1) {
        res = os_task_pend("taskq", msg, ARRAY_SIZE(msg));

        switch (res) {
        case OS_TASKQ:
            switch (msg[0]) {
            case Q_USER:
                switch (msg[1]) {
                case WIFI_MSG_TICK_1_SEC:
                    if (time_lapse(&timehdl, 3 * 1000)) {
                        malloc_stats();
                        if (wifi_module_is_init()) {
                            printf("WIFI U= %d KB/s, D= %d KB/s\r\n", wifi_get_upload_rate() / 1024, wifi_get_download_rate() / 1024);
#if 0
                            /*stats_display();*/

                            if (get_cur_wifi_info()->mode == STA_MODE) {
                                get_rx_signal();
                            }

#endif
                        }
                    }

                    break;

                case WIFI_MSG_SMP_CFG_START:
                    memset(&airkiss_result, 0, sizeof(airkiss_result));
                    /*voiceprint_cfg_start();*/
                    break;

                case WIFI_MSG_SMP_CFG_STOP:
                    /*voiceprint_cfg_stop();*/
                    break;

                case WIFI_MSG_SMP_CFG_COMPLETED:
                    wifi_smp_connect(NULL, NULL, NULL);
                    break;

                case WIFI_MSG_SMP_CFG_TIMEOUT: {
                    struct sys_event evt;
                    evt.arg = "net";
                    evt.type = SYS_NET_EVENT;
                    evt.u.net.event = NET_EVENT_SMP_CFG_TIMEOUT;
                    sys_event_notify(&evt);
                }
                break;

                case WIFI_MSG_STA_SCAN_COMPLETED:
                    airkiss_ssid_check();
                    break;

                case WIFI_MSG_STA_NETWORK_STACK_DHCP_SUCC:
                    airkiss_broadcast();
                    /*voiceprint_broadcast();*/
                    _net_dhcp_ready = 1;
                    {
                        struct sys_event evt;
                        evt.arg = "net";
                        evt.type = SYS_NET_EVENT;
                        evt.u.net.event = NET_EVENT_CONNECTED;
                        sys_event_notify(&evt);
                    }


#ifdef CONFIG_EG_HTTP_CLIENT
                    extern int https_client_init(void);
                    https_client_init();
#endif
                    /*                     extern int tutk_platform_init(const char *username, const char *password); */
                    /* tutk_platform_init("aaaa", "12345678"); */
                    break;

                case WIFI_MSG_STA_DISCONNECTED:
                    _net_dhcp_ready = 0;
                    {
                        struct sys_event evt;
                        evt.arg = "net";
                        evt.type = SYS_NET_EVENT;
                        evt.u.net.event = NET_EVENT_DISCONNECTED;
                        sys_event_notify(&evt);
                    }
                    break;
                case WIFI_MSG_AP_DISCONNECTED:
                    break;

                default :
                    break;
                }

                break;

            default:
                break;
            }

            break;
        }
    }
}


int wireless_net_init(void)//主要是create wifi 线程的
{
    puts("wifi early init \n");
    thread_fork(WIFI_APP_TASK_NAME, 10, 2048, 64, 0, wifi_app_task, NULL);
    return 0;
}
#if defined CONFIG_WIFI_ENABLE
late_initcall(wireless_net_init);
#endif


unsigned short DUMP_PORT()
{
    return _DUMP_PORT;
}

unsigned short FORWARD_PORT()
{
    return _FORWARD_PORT;
}

unsigned short BEHIND_PORT()
{
    return _BEHIND_PORT;
}

const char *get_rec_path_0()
{
    return NULL;
}
const char *get_rec_path_1()
{
    return CONFIG_REC_PATH_0;
}
const char *get_rec_path_2()
{
#if defined CONFIG_VIDEO1_ENABLE
    return CONFIG_REC_PATH_1;
#elif defined CONFIG_VIDEO3_ENABLE
    return CONFIG_REC_PATH_3;
#else
    return CONFIG_REC_PATH_1;
#endif
}
const char *get_rec_path_3()
{
    return CONFIG_REC_PATH_2;
}
const char *get_rec_emr_path_1()
{
#ifdef CONFIG_EMR_REC_PATH_0
    return CONFIG_EMR_REC_PATH_0;
#else
    return NULL;
#endif
}
const char *get_rec_emr_path_2()
{
#ifdef CONFIG_EMR_REC_PATH_1
    return CONFIG_EMR_REC_PATH_1;
#else
    return NULL;
#endif
}
const char *get_rec_emr_path_3()
{
#ifdef CONFIG_EMR_REC_PATH_2
    return CONFIG_EMR_REC_PATH_2;
#else
    return NULL;
#endif
}
const char *get_root_path()
{
    return CONFIG_ROOT_PATH;
}

static struct cfg_info wifi_info;
char *get_wifi_ssid(void)
{
    wifi_info.mode = NONE_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&wifi_info);
    return wifi_info.ssid;
}


char *get_wifi_pwd(void)
{
    wifi_info.mode = NONE_MODE;
    dev_ioctl(wifi_dev, DEV_GET_CUR_WIFI_INFO, (u32)&wifi_info);
    return wifi_info.pwd;
}


#if defined CONFIG_NET_SERVER
static void dhcp_charge_device_event_handler(struct sys_event *event)
{
    if (!strncmp(event->arg, "dhcp_srv", 8)) {
        if (event->u.dev.event == DEVICE_EVENT_CHANGE) {
            wifi_ipc_state_cb(NULL, 1);

        }
    }
}
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, dhcp_charge_device_event_handler, 0);

#endif

#if 0
void sdio_recv_pkg_irq(void)
{
    static u32 thdll, count222;
    int ret22;
    ret22 = time_lapse(&thdll, 1000);

    if (ret22) {
        printf("sdio_recv_cnt = %d,  %d \r\n", ret22, count222);
        count222 = 0;
    }

    ++count222;
}
#endif




