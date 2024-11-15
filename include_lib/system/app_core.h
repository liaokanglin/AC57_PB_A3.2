#ifndef SYS_APPLICATION_H
#define SYS_APPLICATION_H

#include "typedef.h"
#include "generic/list.h"
#include "system/event.h"


#define ACTION_BACK               0x0a1b2c00  // 定义"返回"动作的值为 0x0a1b2c00
#define ACTION_STOP               0x0a1b2c01  // 定义"停止"动作的值为 0x0a1b2c01
#define ACTION_CLASS_MASK         0xfffff000  // 定义一个掩码（mask）为 0xfffff000




enum app_state {
    APP_STA_CREATE,  // 创建状态
    APP_STA_START,   // 启动状态
    APP_STA_PAUSE,   // 暂停状态
    APP_STA_RESUME,  // 恢复状态
    APP_STA_STOP,    // 停止状态
    APP_STA_DESTROY, // 销毁状态
};


struct application;


struct intent {
    const char *name;
    int action;
    const char *data;
    u32 exdata;
};


struct application_operation {
    int (*state_machine)(struct application *, enum app_state, struct intent *);
    int (*event_handler)(struct application *, struct sys_event *);
};


struct application {
    u8 	state;
    int action;
    char *data;
    const char *name;
    struct list_head entry;
    void *private_data;
    const struct application_operation *ops;
};



#define REGISTER_APPLICATION(at) \
	static struct application at sec(.app)


#define init_intent(it) \
	do { \
		(it)->name = NULL; \
		(it)->action= 0; \
		(it)->data = NULL; \
	}while (0)



void register_app_event_handler(int (*handler)(struct sys_event *));

struct application *get_current_app();


int start_app(struct intent *it);  // 同步启动指定应用程序，传入意图结构体参数，返回启动状态

int start_app_async(struct intent *it, void (*callback)(void *p, int err), void *p);  // 异步启动指定应用程序，传入意图结构体、回调函数和用户数据指针，启动结束后通过回调通知






#endif

