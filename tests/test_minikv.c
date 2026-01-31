#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minikv.h"

// 测试结构体
static mk_t *mk = NULL;

// 测试初始化
int init_suite(void) {
    mk = mk_create();
    if (mk == NULL) {
        return CU_get_error();
    }
    return 0;
}

// 测试清理
int clean_suite(void) {
    if (mk != NULL) {
        mk_destroy(mk);
    }
    return 0;
}

// 测试 mk_create 接口
void test_mk_create(void) {
    CU_ASSERT_PTR_NOT_NULL(mk);
    CU_ASSERT_EQUAL(mk_count(mk), 0);
}

// 测试 mk_put 和 mk_get 接口
void test_mk_put_get(void) {
    // 测试基本的键值对插入和获取
    CU_ASSERT_EQUAL(mk_put(mk, "key1", "value1"), 0);
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key1"), "value1");
    
    // 测试更新已存在的键
    CU_ASSERT_EQUAL(mk_put(mk, "key1", "value1_updated"), 0);
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key1"), "value1_updated");
    
    // 测试多个键值对
    CU_ASSERT_EQUAL(mk_put(mk, "key2", "value2"), 0);
    CU_ASSERT_EQUAL(mk_put(mk, "key3", "value3"), 0);
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key2"), "value2");
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key3"), "value3");
}

// 测试 mk_count 接口
void test_mk_count(void) {
    // 先清空
    mk_destroy(mk);
    mk = mk_create();
    
    CU_ASSERT_EQUAL(mk_count(mk), 0);
    
    mk_put(mk, "key1", "value1");
    CU_ASSERT_EQUAL(mk_count(mk), 1);
    
    mk_put(mk, "key2", "value2");
    CU_ASSERT_EQUAL(mk_count(mk), 2);
    
    mk_put(mk, "key3", "value3");
    CU_ASSERT_EQUAL(mk_count(mk), 3);
    
    mk_del(mk, "key2");
    CU_ASSERT_EQUAL(mk_count(mk), 2);
}

// 测试 mk_del 接口
void test_mk_del(void) {
    // 测试删除不存在的键
    CU_ASSERT_EQUAL(mk_del(mk, "non_existent_key"), -1);
    
    // 保存当前计数
    size_t original_count = mk_count(mk);
    
    // 测试删除存在的键
    mk_put(mk, "key_to_delete", "value_to_delete");
    CU_ASSERT_EQUAL(mk_del(mk, "key_to_delete"), 0);
    CU_ASSERT_PTR_NULL(mk_get(mk, "key_to_delete"));
    
    // 测试删除后计数正确（恢复到原始计数）
    CU_ASSERT_EQUAL(mk_count(mk), original_count);
}

// 测试 mk_trim 接口
void test_mk_trim(void) {
    char *trimmed = mk_trim("   hello   ");
    CU_ASSERT_STRING_EQUAL(trimmed, "hello");
    free(trimmed);
    
    trimmed = mk_trim("hello");
    CU_ASSERT_STRING_EQUAL(trimmed, "hello");
    free(trimmed);
    
    // 全空格输入，mk_trim返回NULL
    trimmed = mk_trim("   ");
    CU_ASSERT_PTR_NULL(trimmed);
    
    trimmed = mk_trim("  hello world  ");
    CU_ASSERT_STRING_EQUAL(trimmed, "hello world");
    free(trimmed);
}

// 测试 mk_is_valid_key 接口
void test_mk_is_valid_key(void) {
    CU_ASSERT_EQUAL(mk_is_valid_key("valid_key"), 0);
    CU_ASSERT_EQUAL(mk_is_valid_key("key_with_numbers123"), 0);
    CU_ASSERT_EQUAL(mk_is_valid_key("key-with-dashes"), 0);
    CU_ASSERT_EQUAL(mk_is_valid_key("key.with.dots"), 0);
    CU_ASSERT_EQUAL(mk_is_valid_key(""), -1);
    CU_ASSERT_EQUAL(mk_is_valid_key("key with spaces"), -1);
    CU_ASSERT_EQUAL(mk_is_valid_key(NULL), -1);
}

// 测试 mk_parse_line 接口
void test_mk_parse_line(void) {
    char *key = NULL;
    char *value = NULL;
    
    // 测试正常的键值对
    CU_ASSERT_EQUAL(mk_parse_line("key=value", &key, &value), 0);
    CU_ASSERT_STRING_EQUAL(key, "key");
    CU_ASSERT_STRING_EQUAL(value, "value");
    free(key);
    free(value);
    
    // 测试有空格的键值对
    CU_ASSERT_EQUAL(mk_parse_line("  key2  =  value2  ", &key, &value), 0);
    CU_ASSERT_STRING_EQUAL(key, "key2");
    CU_ASSERT_STRING_EQUAL(value, "value2");
    free(key);
    free(value);
    
    // 测试没有值的情况
    CU_ASSERT_EQUAL(mk_parse_line("key3=", &key, &value), 0);
    CU_ASSERT_STRING_EQUAL(key, "key3");
    CU_ASSERT_STRING_EQUAL(value, "");
    free(key);
    free(value);
    
    // 测试没有等号的情况
    CU_ASSERT_EQUAL(mk_parse_line("key4", &key, &value), -1);
}

// 测试 mk_load 接口
void test_mk_load(void) {
    // 使用我们创建的测试数据文件
    CU_ASSERT_EQUAL(mk_load(mk, "tests/test_data.txt"), 0);
    CU_ASSERT_EQUAL(mk_count(mk), 5);
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key1"), "value1");
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key2"), "value2");
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key3"), "value3");
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key4"), "value4");
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "key5"), "value5");
}

// 测试 mk_save 接口
void test_mk_save(void) {
    // 先插入一些数据
    mk_destroy(mk);
    mk = mk_create();
    mk_put(mk, "save_key1", "save_value1");
    mk_put(mk, "save_key2", "save_value2");
    
    // 保存到文件
    CU_ASSERT_EQUAL(mk_save(mk, "tests/test_save.txt"), 0);
    
    // 重新加载验证
    mk_destroy(mk);
    mk = mk_create();
    CU_ASSERT_EQUAL(mk_load(mk, "tests/test_save.txt"), 0);
    CU_ASSERT_EQUAL(mk_count(mk), 2);
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "save_key1"), "save_value1");
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "save_key2"), "save_value2");
}

// 测试 mk_destroy 接口
void test_mk_destroy(void) {
    // 创建一个新实例进行测试
    mk_t *test_mk = mk_create();
    CU_ASSERT_PTR_NOT_NULL(test_mk);
    
    // 插入一些数据
    mk_put(test_mk, "test_key", "test_value");
    CU_ASSERT_EQUAL(mk_count(test_mk), 1);
    
    // 销毁并验证
    CU_ASSERT_EQUAL(mk_destroy(test_mk), 0);
}

// 测试哈希冲突处理
void test_hash_collision(void) {
    // 清理mk实例，确保测试环境干净
    mk_destroy(mk);
    mk = mk_create();
    CU_ASSERT_PTR_NOT_NULL(mk);
    CU_ASSERT_EQUAL(mk_count(mk), 0);
    
    // 插入多个可能导致哈希冲突的键
    // 这里使用一些可能会哈希到相同桶的键（简单测试）
    mk_put(mk, "test1", "value1");
    mk_put(mk, "test2", "value2");
    mk_put(mk, "test3", "value3");
    mk_put(mk, "test4", "value4");
    mk_put(mk, "test5", "value5");
    mk_put(mk, "test6", "value6");
    mk_put(mk, "test7", "value7");
    mk_put(mk, "test8", "value8");
    mk_put(mk, "test9", "value9");
    mk_put(mk, "test10", "value10");
    
    // 验证所有键都能正确获取
    
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test1"), "value1");
   
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test2"), "value2");
  
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test3"), "value3");
    
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test4"), "value4");
   
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test5"), "value5");
    
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test6"), "value6");
    
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test7"), "value7");
   
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test8"), "value8");
 
    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test9"), "value9");

    CU_ASSERT_STRING_EQUAL(mk_get(mk, "test10"), "value10");
   
    CU_ASSERT_EQUAL(mk_count(mk), 10);
}

// 主函数
int main() {
    // 初始化CUnit测试注册表
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    
    // 创建测试套件
    CU_pSuite pSuite = CU_add_suite("minikv_test_suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // 添加测试用例
    if (NULL == CU_add_test(pSuite, "test_mk_create", test_mk_create) ||
        NULL == CU_add_test(pSuite, "test_mk_put_get", test_mk_put_get) ||
        NULL == CU_add_test(pSuite, "test_mk_count", test_mk_count) ||
        NULL == CU_add_test(pSuite, "test_mk_del", test_mk_del) ||
        NULL == CU_add_test(pSuite, "test_mk_trim", test_mk_trim) ||
        NULL == CU_add_test(pSuite, "test_mk_is_valid_key", test_mk_is_valid_key) ||
        NULL == CU_add_test(pSuite, "test_mk_parse_line", test_mk_parse_line) ||
        NULL == CU_add_test(pSuite, "test_mk_load", test_mk_load) ||
        NULL == CU_add_test(pSuite, "test_mk_save", test_mk_save) ||
        NULL == CU_add_test(pSuite, "test_mk_destroy", test_mk_destroy) ||
        NULL == CU_add_test(pSuite, "test_hash_collision", test_hash_collision)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // 设置测试运行模式
    CU_basic_set_mode(CU_BRM_VERBOSE);
    
    // 运行所有测试
    CU_basic_run_tests();
    
    // 清理并返回结果
    unsigned int ret = CU_get_number_of_failures();
    CU_cleanup_registry();
    return ret;
}
