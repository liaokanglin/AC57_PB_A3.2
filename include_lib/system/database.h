#ifndef DATABASE_H
#define DATABASE_H

#include "typedef.h"


struct db_table {
    const char *name;
    u8 value_bits;
    int value;
};




int db_select_buffer(u8 index, char *buffer, int len); 
// 根据索引选择缓冲区的数据，将其放入缓冲区buffer，len为缓冲区大小

int db_update_buffer(u8 index, char *buffer, int len); 
// 根据索引更新缓冲区的数据，从buffer中读取len长度的数据进行更新

int db_create(const char *store_dev); 
// 创建数据库，store_dev指定数据库的存储设备

int db_create_table(const struct db_table *table, int num); 
// 创建数据库表，table是表结构指针，num是要创建的表数量

u32 db_select(const char *table); 
// 从指定的表中选择数据，返回表的标识

int db_update(const char *table, u32 value); 
// 更新指定表的数据，value是要更新的值

int db_flush(); 
// 刷新数据库，将缓存中的数据写入存储

int db_reset(); 
// 重置数据库，清除所有数据


#endif

