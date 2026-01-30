#include "../include/minikv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 1024

// 打印帮助信息
void print_help() {
    printf("Commands:\n");
    printf("  get <key>          - Get value by key\n");
    printf("  put <key> <value>  - Set key-value pair\n");
    printf("  del <key>          - Delete key\n");
    printf("  save <file>        - Save MiniKV data to file\n");
    printf("  load <file>        - Load MiniKV data from file\n");
    printf("  list               - List all keys\n");
    printf("  help               - Show this help\n");
    printf("  quit / exit        - Exit program\n");
}

int main() {
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
        } else if (strcmp(cmd, "help") == 0) {//输入help显示帮助信息
            print_help();
        } else if (strcmp(cmd, "put") == 0) {//如果是put命令 设置键值对
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, ""); // 分割出key和value
            
            if (key && value) {//如果key和value都不为空
                 // 去除 value 前导空格
                while(*value == ' ') value++;
               
                
                if (mk_put(mk, key, value) == 0) {
                    printf("OK\n");
                }
            } else {
                printf("Usage: put <key> <value>\n");
            }
        } else if (strcmp(cmd, "get") == 0) {//如果是get命令 获取键对应的值
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
        } else if (strcmp(cmd, "del") == 0) {//如果是del命令 删除键值对
            char *key = strtok(NULL, " ");
            if (key) {
                if (mk_del(mk, key) == 0) {
                    //删除成功，mk_del内部会输出信息
                }
            } else {
                printf("Usage: del <key>\n");
            }
        } else if (strcmp(cmd, "save") == 0) {//如果是save命令 保存MiniKV数据到文件
            char *file = strtok(NULL, " ");
            if (file) {
                if (mk_save(mk, file) == 0) {
                    printf("成功将信息保存到%s中\n", file);
                }
            } else {
                printf("Usage: save <file>\n");
            }
        } else if (strcmp(cmd, "load") == 0) {//如果是load命令 从文件加载MiniKV数据
            char *file = strtok(NULL, " ");
            if (file) {
                if (mk_load(mk, file) == 0) {
                    printf("已从%s中加载信息\n", file);
                }
            } else {
                printf("Usage: load <file>\n");
            }
        } else if (strcmp(cmd, "list") == 0) {//如果是list命令 列出所有键值对
            mk_print(mk);
        } else {
            printf("未知的命令: %s\n", cmd);
        }
    }

    mk_destroy(mk);
    printf("Bye.\n");
    return 0;
}
