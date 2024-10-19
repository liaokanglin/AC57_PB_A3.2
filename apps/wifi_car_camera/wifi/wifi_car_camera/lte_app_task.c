
#include "os/os_compat.h"
#include "system/init.h"
#include "device/device.h"
#include "system/app_core.h"
#include "server/server_core.h"
#include "system/timer.h"
#include "app_config.h"
#include "wifi_dev.h"
#include "database.h"
#include "action.h"
#include "asm/gpio.h"
#include "gpio.h"
#include "lwip.h"

#if defined CONFIG_LTE_ENABLE

static void *dev = NULL ;
static OS_SEM sem;
static int adb_pid = 0, lte_pid = 0;

/*
 * LTE事件处理
 */


void adb_to_lte(void);
void lte_to_adb(void);
u8 adb_to_lte_check(void);
u8 lte_to_adb_check(void);
static void lte_app_init(void);
static void lte_app_uninit(void);

static void adb_to_lte_task(void *priv)
{
    for (;;) {
        if (!os_sem_accept(&sem)) {
            adb_pid = 0;
            break;
        }

        if (!adb_to_lte_check()) {
            adb_to_lte();
            break;
        }
        os_time_dly(1);
    }
}


static void lte_to_adb_task(void *priv)
{
    for (;;) {
        if (!os_sem_accept(&sem)) {
            lte_pid = 0;
            break;
        }

        if (!lte_to_adb_check()) {
            lte_to_adb();
            break;
        }
        os_time_dly(1);
    }
}


static void adb_to_lte_cue_start(void)
{
    if (adb_pid) {
        return;
    }
    thread_fork("adb_to_lte_task", 6, 512, 0, &adb_pid, adb_to_lte_task, NULL);
}


static void adb_to_lte_cue_stop(void)
{
    if (adb_pid) {
        return;
    }
    os_sem_post(&sem);
}


static void lte_to_adb_cue_start(void)
{
    if (lte_pid) {
        return;
    }
    thread_fork("lte_to_adb_task", 6, 512, 0, &lte_pid, lte_to_adb_task, NULL);
}


static void lte_to_adb_cue_stop(void)
{
    if (lte_pid) {
        return;
    }
    os_sem_post(&sem);
}


static void lte_event_handler(struct sys_event *event)
{
    extern u32 ASCII_StrCmp(const char *src, const char *dst, u32 len);
    printf("%s, dev = %s, event = %d\n", __FUNCTION__, event->arg, event->u.dev.event);
    if (!ASCII_StrCmp(event->arg, "lte*", 5)) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN: {
            dev = dev_open("lte0", 0);
            ASSERT(dev);
            void rndis_operatrions(void);
            rndis_operatrions();
            if (dev)  {
                //重新获取DHCP
                lwip_netif_set_up(LTE_NETIF);
                lwip_set_default_netif(LTE_NETIF);
                lwip_renew(LTE_NETIF, 1);
                lte_app_init();
            }
            lte_to_adb_cue_start();
        }
        break;
        case DEVICE_EVENT_OUT: {
            if (dev) {
                lte_app_uninit();
                lwip_dhcp_release_and_stop(LTE_NETIF);
                dev_close(dev);
                dev = NULL;
            }
            lte_to_adb_cue_stop();
        }

        }
    } else if (!ASCII_StrCmp(event->arg, "adb*", 3)) {
        switch (event->u.dev.event) {
        case DEVICE_EVENT_IN:
            adb_to_lte_cue_start();
            break;

        case DEVICE_EVENT_OUT:
            adb_to_lte_cue_stop();
            break;

        default:
            break;
        }
    }
}
/*
 * 静态注册设备事件回调函数，优先级为0
 */
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, lte_event_handler, 0);


static void ping_cb(void *priv, u32 cnt, u32 time)
{
    printf("cnt = %d, time = %d\n", cnt, time);
}


static void lte_app_init_task(void *priv)
{
    //for test
    //ping_init("120.24.247.138", 1000, 10, ping_cb, NULL);
    void net_app_init(void);
    //net_app_init();
}


static void lte_app_init(void)
{
    thread_fork("lte_app_init_task", 20, 0x1000, 0, 0, lte_app_init_task, NULL);
}


static void lte_app_uninit_task(void *priv)
{
    void net_app_uninit(void);
    //net_app_uninit();
}


static void lte_app_uninit(void)
{
    thread_fork("lte_app_uninit_task", 20, 0x1000, 0, 0, lte_app_uninit_task, NULL);
}


static int lte_net_init(void)//主要是有线网卡线程的
{
    puts("lte early init \n\n\n\n\n\n");
    os_sem_create(&sem, 0);
    Init_LwIP(LTE_NETIF);
    return 0;
}
late_initcall(lte_net_init);
#endif


