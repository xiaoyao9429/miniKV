#include "../include/minikv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// 校验key合法性
int mk_is_valid_key(const char *key) {
    // 1. 边界条件：key为NULL指针，直接返回非法(-1)
    if (key == NULL) {
        return -1;
    }

    // 2. 空字符串，判定为非法(-1)
    if (*key == '\0') {
        return -1;
    }

    // 3. 遍历核心字符（非首部空格的部分）
    while (*key != '\0') {
        char c = *key;

        // 3.1 中间出现空格，直接返回非法(-1)
        if (c == ' ') {
            return -1;
        }

        // 3.2 判断是否为允许的合法字符
        int is_valid_char = 
            (c >= '0' && c <= '9') ||   // 数字 0-9
            (c >= 'a' && c <= 'z') ||   // 小写字母 a-z
            (c >= 'A' && c <= 'Z') ||   // 大写字母 A-Z
            (c == '.') || (c == '_') || (c == '-');  // 特殊允许字符
        
        // 3.3 遇到非法字符，返回-1
        if (!is_valid_char) {
            return -1;
        }

        // 移动到下一个字符
        key++;
    }

    // 4. 所有校验通过，返回合法(0)
    return 0;
}



// 去除字符串首尾空白
char* mk_trim(const char *str) {
    if (str == NULL) {
        return NULL;
    }

    const char *start = str;

    // 跳过首部空白
    while (isspace((unsigned char)*start) && *start != '\0') {
        start++;
    }

    // 全空格
    if (*start == '\0') {
        return NULL;
    }

    // 找到最后一个非空白字符
    const char *end = start;
    const char *p = start;
    while (*p != '\0') {
        if (!isspace((unsigned char)*p)) {
            end = p;
        }
        p++;
    }

    // 计算新字符串长度，分配内存
    size_t len = end - start + 1;
    char *result = (char *)malloc(len + 1);  // +1 存结束符
    if (result == NULL) {
        return NULL;  // 内存分配失败
    }

    // 复制有效字符到新内存
    strncpy(result, start, len);
    result[len] = '\0';  // 手动添加结束符

    return result;
}


int mk_parse_line(const char *line, char **key, char **value) {
    // 参数检查
    if (line == NULL || key == NULL || value == NULL) {
        perror("mk_parse_line 参数无效");
        return -1;
    }
    
    // 确保key和value指针初始化为NULL
    *key = NULL;
    *value = NULL;
    
    // 第一步：trim整行
    char *trimmed_line = mk_trim(line);
    if (trimmed_line == NULL) {
        return -1;
    }

    //注释行
    if ( *trimmed_line == '#' || *trimmed_line == ';') {
        free(trimmed_line);
        *key = NULL;
        *value = NULL;
        return -1;
    }

    // 第二步：找第一个=作为分隔符（处理多=号场景）
    char *eq_pos = strchr(trimmed_line, '=');
    if (eq_pos == NULL) { // 无=号，解析失败
        free(trimmed_line);
        // 确保key和value指针被设置为NULL
        *key = NULL;
        *value = NULL;
        return -1;
    }

    // 拆分key和value
    *eq_pos = '\0'; // 截断为key部分
    char *trimmed_key = mk_trim(trimmed_line);
    char *trimmed_value = mk_trim(eq_pos + 1);

    free(trimmed_line);

    // 处理key为全空格的情况
    if (trimmed_key == NULL) {
        // 全空格的key视为无效
        // 只有当trimmed_value不为NULL时才释放
        if (trimmed_value != NULL) {
            free(trimmed_value);
        }
        *key = NULL;
        *value = NULL;
        return -1;
    }

    // 处理value为全空格的情况，视为空字符串
    if (trimmed_value == NULL) {
        trimmed_value = (char *)malloc(1);
        if (trimmed_value == NULL) {
            free(trimmed_key);
            *key = NULL;
            *value = NULL;
            return -1;
        }
        trimmed_value[0] = '\0';
    }

    // 校验key合法性
    if (mk_is_valid_key(trimmed_key)!=0) {
        // 释放已分配的内存
        free(trimmed_key);
        free(trimmed_value);
        *key = NULL;
        *value = NULL;
        return -1;
    }

    *key = trimmed_key;
    *value = trimmed_value;
    return 0;
}