#ifndef MINIKV_H
#define MINIKV_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 哈希桶大小（简易哈希表，链表法解决冲突）
#define MK_HASH_SIZE 1024

// 键值对节点（哈希表桶的链表节点）
typedef struct mk_node {
    char *key;                  // 键（动态分配）
    char *value;                // 值（动态分配）
    struct mk_node *next;       // 下一个节点（冲突链）
} mk_node_t;


// 存储数据的Hash表
typedef struct {
    mk_node_t *buckets[MK_HASH_SIZE];  // 哈希桶数组
    size_t count;                      // 总键值对数量
} mk_t;


mk_t* mk_create(void);//创建Hash表
int mk_destroy(mk_t *mk);//销毁Hash表
int mk_load(mk_t *mk, const char *filepath);//从文件中读取Key,Value键值对
int mk_save(mk_t *mk, const char *filepath);//保存Key,Value键值对到文件
const char* mk_get(const mk_t *mk, const char *key);//根据key获取value
int mk_put(mk_t *mk, const char *key, const char *value);//新增一个key,value键值对
int mk_del(mk_t *mk, const char *key);//删除key对应的键值对
size_t mk_count(const mk_t *mk);//获取Hash表中元素的数量
char* mk_trim(const char *str);//去除字符串首尾空白字符，返回新分配的字符串
int mk_is_valid_key(const char *key);//检查key是否合法，合法返回0，非法返回-1
int mk_parse_line(const char *line, char **key, char **value);//将读到的一行拆分为键值对
int mk_print(const mk_t *mk);//打印Hash表中的所有键值对

#endif