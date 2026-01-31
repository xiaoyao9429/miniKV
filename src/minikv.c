#include "../include/minikv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void mk_destroy_chain(mk_node_t *node);//递归销毁一条Hash链

// 简易哈希函数（DJB2）
size_t mk_hash(const char *key) {
    size_t hash = 5381;// 初始化哈希值
    int c;
    while ((c = *key)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
        key++;
    }


    return hash % MK_HASH_SIZE;
}

// 创建Hash表
mk_t* mk_create(void) {
    mk_t *mk = calloc(1, sizeof(mk_t)); // 自动初始化为0
    if (mk == NULL) return NULL;
    return mk;
}

// 销毁Hash表
int mk_destroy(mk_t *mk) {
    if (mk == NULL) {
        fprintf(stderr,"mk_destroy 无效的参数\n");
        return -1;
    }
    // 遍历哈希桶，释放链表
    for (int i = 0; i < MK_HASH_SIZE; i++) {
        mk_destroy_chain(mk->buckets[i]);//逐个销毁Hash桶的链
    }
    free(mk);
    return 0;

}

//递归销毁一条链
void mk_destroy_chain(mk_node_t *node) {
    if (node == NULL) return;
    mk_destroy_chain(node->next);
    free(node->key);
    free(node->value);
    free(node);
}



// 查找key对应的节点（内部函数）
mk_node_t* mk_find_node(const mk_t *mk, const char *key) {
    if (mk == NULL || key == NULL) return NULL;
    //去除key两边的空格
   char * validKey=mk_trim(key);
    if (validKey == NULL) {
        perror("mk_find_node 内存分配失败");
        return NULL;//内存分配失败
    }
    // 校验key合法性
    size_t idx = mk_hash(validKey);
    mk_node_t *node = mk->buckets[idx];
    while (node != NULL) {
        if (strcmp(node->key, validKey) == 0) {
            free(validKey);
            return node;//找到节点，返回节点指针
        }
        node = node->next;
    }
    free(validKey);
    return NULL;//未找到节点
}

// 设置/覆盖key的value
int mk_put(mk_t *mk, const char *key, const char *value) {//todo
    // 参数校验
    if (mk == NULL || key == NULL || *key == '\0') {
        fprintf(stderr, "mk_put 函数参数错误 ❌\n");
        return -1;
    }

  //去除key两边的空格
   char * validKey=mk_trim(key);//如果key两边有空格也判定为合法
   if (validKey == NULL) {
        perror("mk_put 内存分配失败");
        return -1;
   }

    // 校验key合法性
   int check =mk_is_valid_key(validKey);//判断key的合法性
   if(check!=0){//key不合法
        fprintf(stderr, "mk_put 非法的key! ❌\n");
        free(validKey);
        return check;
   }


 

    // 处理value（允许空字符串）
    const char *val = (value == NULL) ? "" : value;

    // 查找是否已存在该key
    mk_node_t *node = mk_find_node(mk, validKey);
    if (node != NULL) {
        // 覆盖value：先释放旧值，再分配新值
        char *new_val = strdup(val);
        if (new_val == NULL) {
            perror("mk_put 内存分配失败");
            free(validKey);
            return -1;
        }
        free(node->value);
        node->value = new_val;
        free(validKey);
        return 0;
    }

    // 不存在则新建节点
    node = malloc(sizeof(mk_node_t));
    if (node == NULL) {
        perror("mk_put 内存分配失败");
        free(validKey);
        return -1;
    }
    node->next = NULL;
    // 分配key和value内存
    node->key = strdup(validKey);
    node->value = strdup(val);

    if (node->key == NULL || node->value == NULL) {
        free(node->key);
        free(node->value);
        free(node);
        free(validKey);
        perror("mk_put 内存分配失败");
        return -1;
    }

    // 插入哈希桶（头插法）
    size_t idx = mk_hash(validKey);
    node->next = mk->buckets[idx];
    mk->buckets[idx] = node;
    mk->count++;
    free(validKey);
    return 0;
}

// 查询key对应的value
const char* mk_get(const mk_t *mk, const char *key) {
    mk_node_t *node = mk_find_node(mk, key);
    return (node != NULL) ? node->value : NULL;
}

// 删除指定key
int mk_del(mk_t *mk, const char *key) {
    if (mk == NULL || key == NULL || *key == '\0') {


        fprintf(stderr,"mk_del 无效的参数\n");
        return -1;
    }

    // 去除key两边的空格
    char * validKey=mk_trim(key);
    if (validKey == NULL) {
        perror("mk_del 内存分配失败");
        return -1;
    }
    size_t idx = mk_hash(validKey);
    mk_node_t *prev = NULL;
    mk_node_t *curr = mk->buckets[idx];

    // 遍历链表找key
    while (curr != NULL) {
        if (strcmp(curr->key, validKey) == 0) {
            // 从链表中移除节点
            if (prev == NULL) {
                mk->buckets[idx] = curr->next;
            } else {
                prev->next = curr->next;
            }
            // 释放节点内存
            free(curr->key);
            free(curr->value);
            free(curr);
            mk->count--;
            printf("%s 删除成功 ✅\n",validKey);
            free(validKey);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    // key不存在
    fprintf(stderr, "键 %s 不存在 ❌\n",validKey);
    free(validKey);
    return -1;
}

// 获取键值对数量
size_t mk_count(const mk_t *mk) {
    return (mk == NULL) ? 0 : mk->count;
}

// 从文件读取数据
int mk_load(mk_t *mk, const char *filepath) {
    if (mk == NULL || filepath == NULL) {
        fprintf(stderr, "mk_load 无效的参数 ❌\n");
        return -1;
    }

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        fprintf(stderr, "mk_load 文件打开失败 ❌\n");
        return -1;
    }

    //mk中所有数据清空
    for(int i=0;i<MK_HASH_SIZE;i++){
        mk_destroy_chain(mk->buckets[i]);
        mk->buckets[i]=NULL;
    }
    mk->count=0;

    char line[4096];
    int line_num = 0;
    int ret = 0;

    // 逐行解析
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        char *key = "";//key和value是传出参数
        char *value = "";

        // 解析行
        int parse_ret = mk_parse_line(line, &key, &value);
        switch (parse_ret) {
            case 0: // 有效行
                //清空value最后的\n
                value[strcspn(value, "\n")] = '\0';
                if (mk_put(mk, key, value) != 0) {
                    ret = -1;
                    free(key);
                    free(value);
                    char tempMessage[256];
                    snprintf(tempMessage, sizeof(tempMessage), "mk_load 键值对存放失败");
                    fprintf(stderr, "%s ❌\n", tempMessage);
                    fclose(fp);
                    return ret;
                    
                }
                free(key);
                free(value);//mk_parse_line中让局部变量指向了堆地址
                break;
            case -1: // 空行/注释，跳过
                
                continue;
        }
    }


    fclose(fp);
    return ret;
}

// 保存配置到文件
int mk_save(mk_t *mk, const char *filepath) {
    if (mk == NULL || filepath == NULL) {
        perror("mk_save 无效的参数");
        return -1;
    }

    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        perror("mk_save 文件打开失败");
        return -1;
    }

    // 遍历所有哈希桶，写入键值对
    for (int i = 0; i < MK_HASH_SIZE; i++) {
        mk_node_t *node = mk->buckets[i];
        while (node != NULL) {
            fprintf(fp, "%s=%s\n", node->key, node->value);
            node = node->next;
        }
    }

    fclose(fp);
    return 0;
}


int mk_print(const mk_t *mk){
    if (mk == NULL) {
        perror("无效的参数");
        return -1;
    }

   
    size_t printed_count = 0; // 统计输出的键值对数量
    printf("===== MiniKV Key-Value List (total: %zu) =====\n", mk_count(mk));

    // 遍历所有哈希桶（0 ~ MK_HASH_SIZE-1）
    for (int bucket_idx = 0; bucket_idx < MK_HASH_SIZE; bucket_idx++) {
        mk_node_t *node = mk->buckets[bucket_idx];
        
        // 遍历当前桶的链表节点
        while (node != NULL) {
            // 输出格式：[桶索引] key = value
            printf("[%d]%s = %s ✅\n", bucket_idx, node->key, node->value);
            printed_count++;
            node = node->next; // 移动到下一个节点
        }
    }

    
    if (printed_count == 0) {
        printf("Hash表中无数据\n");
    }

    else{
        printf("一共输出了%ld个键值对\n",printed_count);
        printf("输出成功✅\n");
    }
    printf("=============================================\n");

    return 0;

}



// 比较函数，用于qsort升序排序
int compare_kv_asc(const void *a, const void *b) {
    const kv_pair_t *pair1 = (const kv_pair_t *)a;
    const kv_pair_t *pair2 = (const kv_pair_t *)b;
    return strcmp(pair1->key, pair2->key);
}

// 比较函数，用于qsort降序排序
int compare_kv_desc(const void *a, const void *b) {
    const kv_pair_t *pair1 = (const kv_pair_t *)a;
    const kv_pair_t *pair2 = (const kv_pair_t *)b;
    return strcmp(pair2->key, pair1->key);
}

// 按key升序打印Hash表中的所有键值对
int mk_asc_print(const mk_t *mk) {
    if (mk == NULL) {
        perror("无效的参数");
        return -1;
    }

    size_t count = mk_count(mk);
    printf("===== MiniKV Key-Value List (total: %zu) =====\n", count);

    if (count == 0) {
        printf("Hash表中无数据\n");
        printf("=============================================\n");
        return 0;
    }

    // 收集所有键值对
    kv_pair_t *pairs = (kv_pair_t *)malloc(count * sizeof(kv_pair_t));
    if (pairs == NULL) {
        perror("内存分配失败");
        return -1;
    }

    size_t idx = 0;
    // 遍历所有哈希桶，收集键值对
    for (int bucket_idx = 0; bucket_idx < MK_HASH_SIZE; bucket_idx++) {
        mk_node_t *node = mk->buckets[bucket_idx];
        while (node != NULL) {
            pairs[idx].key = node->key;
            pairs[idx].value = node->value;
            idx++;
            node = node->next;
        }
    }

    // 按key升序排序
    qsort(pairs, count, sizeof(kv_pair_t), compare_kv_asc);

    // 打印排序后的键值对
    for (size_t i = 0; i < count; i++) {
        printf("%s = %s ✅\n", pairs[i].key, pairs[i].value);
    }

    // 释放临时数组
    free(pairs);

    printf("一共输出了%zu个键值对\n", count);
    printf("输出成功✅\n");
    printf("=============================================\n");

    return 0;
}

// 按key降序打印Hash表中的所有键值对
int mk_desc_print(const mk_t *mk) {
    if (mk == NULL) {
        perror("无效的参数");
        return -1;
    }

    size_t count = mk_count(mk);
    printf("===== MiniKV Key-Value List (total: %zu) =====\n", count);

    if (count == 0) {
        printf("Hash表中无数据\n");
        printf("=============================================\n");
        return 0;
    }

    // 收集所有键值对
    kv_pair_t *pairs = (kv_pair_t *)malloc(count * sizeof(kv_pair_t));
    if (pairs == NULL) {
        perror("内存分配失败");
        return -1;
    }

    size_t idx = 0;
    // 遍历所有哈希桶，收集键值对
    for (int bucket_idx = 0; bucket_idx < MK_HASH_SIZE; bucket_idx++) {
        mk_node_t *node = mk->buckets[bucket_idx];
        while (node != NULL) {
            pairs[idx].key = node->key;
            pairs[idx].value = node->value;
            idx++;
            node = node->next;
        }
    }

    // 按key降序排序
    qsort(pairs, count, sizeof(kv_pair_t), compare_kv_desc);

    // 打印排序后的键值对
    for (size_t i = 0; i < count; i++) {
        printf("%s = %s ✅\n", pairs[i].key, pairs[i].value);
    }

    // 释放临时数组
    free(pairs);

    printf("一共输出了%zu个键值对\n", count);
    printf("输出成功✅\n");
    printf("=============================================\n");

    return 0;
}


// 启动函数
int start_minikv(void) {
    mk_t *mk = mk_create();
    if (mk == NULL) {
        fprintf(stderr, "Failed to initialize MiniKV\n");
        return 1;
    }

    printf("MiniKV Interactive Shell\n");
    printf("Type 'help' for commands.\n");

    char line[MAX_CMD_LEN];
    while (1) {
        printf("minikv> ");
        //从标准输入中读取一行命令
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break; // EOF
        }

        // 去除行尾换行符
        line[strcspn(line, "\n")] = 0;

        // 输入回车继续
        if (strlen(line) == 0) continue;

        //分割出要执行的命令
        char *cmd = strtok(line, " ");

        // 忽略空命令
        if (cmd == NULL) continue;


        //输入quit或exit退出工具
        if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "help") == 0) {//help指令 用于打印帮助信息
            // 打印帮助信息
            printf("Commands:\n");
            printf("  get <key>          - Get value by key\n");
            printf("  put <key> <value>  - Set key-value pair\n");
            printf("  del <key>          - Delete key\n");
            printf("  save <file>        - Save MiniKV data to file\n");
            printf("  load <file>        - Load MiniKV data from file\n");
            printf("  list [-asc|-desc]  - List all keys\n");
            printf("  help               - Show this help\n");
            printf("  quit / exit        - Exit program\n");
        } else if (strcmp(cmd, "put") == 0) {//put指令 用于设置key和value
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, ""); // 分割出key和value
            
            if (key && value) {
                 // 去除 value 前导空格
                while(*value == ' ') value++;
                
                
                if (mk_put(mk, key, value) == 0) {
                    printf("OK\n");
                }
            } else {
                printf("Usage: put <key> <value>\n");
            }
        } else if (strcmp(cmd, "get") == 0) {//get指令 用于获取指定key对应的value
            char *key = strtok(NULL, " ");
            if (key) {
                const char *val = mk_get(mk, key);
                if (val) {
                    printf("%s\n", val);
                } else {
                    printf("\n");
                }
            } else {
                printf("Usage: get <key>\n");
            }
        } else if (strcmp(cmd, "del") == 0) {//del指令 用于删除指定key
            char *key = strtok(NULL, " ");
            if (key) {
                if (mk_del(mk, key) == 0) {
                    //删除成功，mk_del内部会输出信息
                }
            } else {
                printf("Usage: del <key>\n");
            }
        } else if (strcmp(cmd, "save") == 0) {//save指令 用于保存当前Hash表中数据到文件
            char *file = strtok(NULL, " ");
            if (file) {
                if (mk_save(mk, file) == 0) {
                    printf("成功将信息保存到%s中\n", file);
                }
            } else {
                printf("Usage: save <file>\n");
            }
        } else if (strcmp(cmd, "load") == 0) {//load指令 用于从文件中加载数据到Hash表
            char *file = strtok(NULL, " ");
            if (file) {
                if (mk_load(mk, file) == 0) {
                    printf("已从%s中加载信息\n", file);
                }
            } else {
                printf("Usage: load <file>\n");
            }
        } else if (strcmp(cmd, "list") == 0) {//list指令 用于打印Hash表中所有键值对
            char *arg = strtok(NULL, " ");
            if (arg == NULL) {
                // 没有参数，使用默认打印
                mk_print(mk);
            } else if (strcmp(arg, "-asc") == 0) {
                // 升序打印
                mk_asc_print(mk);
            } else if (strcmp(arg, "-desc") == 0) {
                // 降序打印
                mk_desc_print(mk);
            } else {
                printf("未知的list命令参数: %s\n", arg);
                printf("Usage: list [-asc|-desc]\n");
            }
        } else {
            printf("未知的命令: %s\n", cmd);
        }
    }

    mk_destroy(mk);
    printf("Bye.\n");
    return 0;
}