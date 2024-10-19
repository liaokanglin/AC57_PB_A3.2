#include "system/includes.h"
#include "server/audio_server.h"
#include "server/server_core.h"

#include "app_config.h"

#include "net_server.h"
#include "net_config.h"



enum {
    SD_UNMOUNT,
    SD_MOUNT_SUSS,
    SD_MOUNT_FAILD,
};

static char *const sd_list[] = {
    "sd0",
    "sd1",
    "sd2",
};

static u32 class;
static u8 fs_mount = SD_UNMOUNT;
static OS_MUTEX sd_mutex;

static int __sd_mutex_init()
{
    return os_mutex_create(&sd_mutex);
}
early_initcall(__sd_mutex_init);


int mount_sd_to_fs(const char *name)
{
    int err = 0;
    struct imount *mt;
    int id = ((char *)name)[2] - '0';
    const char *dev  = sd_list[id];

    err = os_mutex_pend(&sd_mutex, 0);
    if (err) {
        return -EFAULT;
    }


    if (fs_mount == SD_MOUNT_SUSS) {
        goto __exit;
    }
    if (fs_mount == SD_MOUNT_FAILD) {
        err = -EFAULT;
        goto __exit;
    }
    if (!dev_online(dev)) {
        err = -EFAULT;
        goto __exit;
    }

    void *fd = dev_open(dev, 0);
    if (!fd) {
        err = -EFAULT;
        goto __err;
    }
    dev_ioctl(fd, SD_IOCTL_GET_CLASS, (u32)&class);
    if (class == SD_CLASS_10) {
        puts("sd card class: 10\n");
    } else {
        log_w("sd card class: %d\n", class * 2);
    }
    dev_close(fd);

    mt = mount(dev, CONFIG_STORAGE_PATH, "fat", FAT_CACHE_NUM, NULL);
    if (!mt) {
        puts("mount fail\n");
        err = -EFAULT;
    } else {
        puts("mount sd suss\n");
    }

__err:
    fs_mount = err ? SD_MOUNT_FAILD : SD_MOUNT_SUSS;
__exit:
    os_mutex_post(&sd_mutex);
//if (fs_mount == SD_MOUNT_SUSS){
    // extern void sd_check()
    //sd_check();
//}
    return err;
}

void unmount_sd_to_fs(const char *path)
{
    os_mutex_pend(&sd_mutex, 0);

    unmount(path);
    fs_mount = SD_UNMOUNT;

    os_mutex_post(&sd_mutex);
}

int storage_device_ready()
{
    // 检查设备是否在线
    if (!dev_online(SDX_DEV)) {
        return false;  // 如果设备不在线，返回false
    }

    // 检查文件系统是否未挂载
    if (fs_mount == SD_UNMOUNT) {
        // 如果未挂载，则挂载SD卡到文件系统
        mount_sd_to_fs(SDX_DEV);
    }

    // 检查存储路径是否存在，并返回结果
    return fdir_exist(CONFIG_STORAGE_PATH);
}


int storage_device_format()
{
    int err;

    unmount_sd_to_fs(CONFIG_STORAGE_PATH);

    err = f_format(SDX_DEV, "fat", 32 * 1024);
    if (err == 0) {
        mount_sd_to_fs(SDX_DEV);
    }

    return err;
}


/*
 * sd卡插拔事件处理
 */

static void sd_event_handler(struct sys_event *event)
{
    // 从event->arg中获取SD卡ID
    int id = ((char *)event->arg)[2] - '0';
    // 获取对应的SD卡设备名称
    const char *dev  = sd_list[id];

    // 根据设备事件类型执行不同的操作
    switch (event->u.dev.event) {
    case DEVICE_EVENT_IN:  // SD卡插入事件
        // 将SD卡挂载到文件系统
        mount_sd_to_fs(event->arg);
#if defined CONFIG_ENABLE_VLIST
        // 如果启用了VLIST功能，则将文件列表加载到内存中
        FILE_LIST_IN_MEM(1);
#endif

        break;
    case DEVICE_EVENT_OUT:  // SD卡移除事件
        // 输出设备名称及"out"提示
        printf("%s: out\n", dev);

#if defined CONFIG_ENABLE_VLIST
        extern u8 file_list_in_flag;
        // 如果文件列表标志被设置，则退出文件列表
        if (file_list_in_flag) {
            FILE_LIST_EXIT();
        }
#endif
        // 解除SD卡挂载
        unmount_sd_to_fs(CONFIG_STORAGE_PATH);
        break;
    }
}

static char *const udisk_list[] = {
    "udisk0",
    "udisk1",
};

// 检查USB设备是否在线
static int usb_disk_online(const char *dev_name)
{
    void *dev = NULL;
    u32 sta = 0;
    // 打开设备
    dev = dev_open(dev_name, 0);
    if (dev) {
        // 获取设备状态
        dev_ioctl(dev, IOCTL_GET_STATUS, (u32)&sta);
        // 关闭设备
        dev_close(dev);
        return sta;
    }
    return 0;
}

// 测试USB大容量存储设备的文件系统
static int usb_mass_storage_fs_test(const char *disk)
{
    struct imount *mt = NULL;
    char path[64];
    int ret = 0;
    u8 id = disk[5] - '0';  // 从设备名获取ID（例如"udisk0" -> 0）
    char *const udisk_root_path[] = {
        "storage/udisk0",
        "storage/udisk1",
    };
    // 挂载USB磁盘
    mt = mount(disk, udisk_root_path[id], "fat", FAT_CACHE_NUM, 0);
    if (!mt) {
        log_w("mount %s fail", disk);  // 挂载失败日志
        return -ENODEV;
    }
    // 生成文件路径
    sprintf(path, "%s/C/", udisk_root_path[id]);
    ret = strlen(path);
    if (path[ret - 1] != '/') {
        path[ret] = '/';
        path[ret + 1] = 0;
    }
    log_d("%d resolute path of file: %s", __LINE__, path);  // 打印文件路径

#if 1
    /// 测试代码
    char path1[64];
    // 构建源文件路径
    strcpy(path1, path);
    strcat(path1, "source.txt");
    log_d("resolute path of file: %s", path1);  // 打印源文件路径
    // 打开源文件
    FILE *f0 = fopen(path1, "r");
    if (!f0) {
        log_e("fail to open file %s", path1);  // 文件打开失败日志
        return -1;
    }
    log_d("succeed to open source file");  // 成功打开源文件日志
    // 构建目标文件路径
    strcpy(path1, path);
    strcat(path1, "dest.txt");
    log_d("resolute dst path of file: %s", path1);  // 打印目标文件路径
    // 打开目标文件
    FILE *f1 = fopen(path1, "w+");
    if (!f1) {
        log_e("fail to open file %s", path1);  // 文件打开失败日志
        return -1;
    }
    log_d("succeed to open dest file");  // 成功打开目标文件日志

    // 分配读写缓冲区
    u8 *wbuf = (u8 *)zalloc(512);
    u8 *rbuf = (u8 *)zalloc(512);
    if (!wbuf || !rbuf) {
        log_e("malloc wbuf or rbuf fail");  // 内存分配失败日志
        return -ENOMEM;
    }
    // 将源文件内容拷贝到目标文件
    int rlen = fread(f0, rbuf, 512);
    while (rlen > 0) {
        int wlen = fwrite(f1, rbuf, rlen);
        rlen = fread(f0, rbuf, 512);
    }

    // 关闭文件
    fclose(f0);
    fclose(f1);
    // 释放缓冲区
    if (wbuf) {
        free(wbuf);
    }
    if (rbuf) {
        free(rbuf);
    }
#endif

    return 0;
}

/*
 * U盘插拔事件处理
 */
static void udisk_event_handler(struct sys_event *event)
{
    int id = ((char *)event->arg)[5] - '0';
    const char *dev  = udisk_list[id];
    char root_path[32];

    switch (event->u.dev.event) {
    case DEVICE_EVENT_IN:
        printf("\n %s: in\n", dev);
        if (usb_disk_online(dev)) {
            /* usb_mass_storage_fs_test(dev); */
            int udisk_upgrade_detect(void);
            os_time_dly(20); //防止app阻塞
            udisk_upgrade_detect();
        }
        break;
    case DEVICE_EVENT_OUT:
        sprintf(root_path, "storage/%s", dev);
        unmount(root_path);
        printf("\n %s: out\n", dev);
        break;
    }
}

static void device_event_handler(struct sys_event *event)
{
    if (!ASCII_StrCmp(event->arg, "sd*", 4)) {
        sd_event_handler(event);
    } else if (!ASCII_StrCmp(event->arg, "usb", 4)) {
    } else if (!ASCII_StrCmp(event->arg, "udisk*", 7)) {
        udisk_event_handler(event);
    }
}
/*
 * 静态注册设备事件回调函数，优先级为0
 */
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, device_event_handler, 0);





