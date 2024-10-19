#ifndef SERVER_H
#define SERVER_H

#include "generic/typedef.h"
#include "system/task.h"
#include "spinlock.h"
#include "list.h"


#define REQ_COMPLETE_CALLBACK 			0x01000000
#define REQ_WAIT_COMPLETE 				0x02000000
#define REQ_HI_PRIORITY 				0x04000000


#define REQ_TYPE_MASK 					0x00ffffff


struct server_req {
    int type;
    int err;
    void *server_priv;
    struct list_head entry;
    struct server *server;
    void *user;
    const char *owner;
    OS_SEM sem;
    union {
        int state;
        void (*func)(void *, void *, int);
    } complete;
    u32 arg[0];
};


struct server_info {
    const char *name;
    int reqlen;
    void *(*open)(void *, void *);
    void (*close)(void *);
};

#define REQ_BUF_LEN  	512

struct server {
    bool avaliable;                     // 服务器是否可用
    void *server;                       // 服务器指针，指向具体的服务器实例
    OS_SEM sem;                         // 信号量，用于同步
    OS_MUTEX mutex;                     // 互斥锁，确保对共享资源的互斥访问
    spinlock_t lock;                    // 自旋锁，适用于高频率的锁定和解锁
    struct list_head *req_buf;          // 请求缓冲区的链表头指针
    struct list_head free;              // 空闲请求链表，存放可重用的请求
    struct list_head pending;           // 待处理请求链表，存放等待处理的请求
    const struct server_info *info;     // 服务器信息结构体指针，包含服务器的配置信息
    const char *owner;                  // 服务器拥有者的标识
    void *handler_priv;                 // 事件处理程序的私有数据
    void (*event_handler)(void *, int argc, int *argv); // 事件处理函数指针
};




#define SERVER_REGISTER(info) \
	const struct server_info info sec(.server_info)


#define server_load(server) \
	load_module(server)

struct server *server_open(const char *name, void *arg);

void server_register_event_handler(struct server *server, void *priv,
                                   void (*handler)(void *, int argc, int *argv));

void server_close(struct server *server);

int server_request(struct server *server, int req_type, void *arg);

int server_request_async(struct server *server, int req_type, void *arg, ...);

int server_req_complete(struct server_req *req);

int server_event_handler(void *_server, int argc, int *argv);

#endif

