#ifndef __PKG_RECOVER__H__
#define  __PKG_RECOVER__H__


#include "typedef.h"
#include "list.h"
#include "server/vpkg_server.h"
#include "system/spinlock.h"
#include "system/task.h"







void *pkg_rcv_open(char *path, void *format);
int pkg_rcv(void *__pkg);
int pkg_rcv_close(void *__pkg);



















#endif


