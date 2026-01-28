#include "../include/minikv.h"
#include <stdio.h>
#include <string.h>

// 阶段性测试主函数
int main() {
    printf("===== MiniKV 阶段性测试开始 =====\n\n");
    //=================== 创建实例 ====================
    printf("【创建Hash表测试】\n");
    mk_t *mk = mk_create();
    if (mk == NULL) {
        fprintf(stderr, "错误：Hash表创建失败\n");
        return 1;
    }
    printf("✅ 创建Hash表创建成功\n");
    printf("\n");

    // ====================  基础put测试  ====================
    printf("【基础put测试】\n");
   
    // 测试合法key的put
              mk_put(mk, "   test_key1    ", "test_value1");
              mk_put(mk, "test_key2    ", "test_value2");
              mk_put(mk, "test_key3", "test_value3");
              mk_put(mk, "test_key4", "   test_value4   ");
              mk_put(mk, "test_key5", "  test_va lue5");
    int ret = mk_put(mk, "test_key6", "test_va lue6");
    if (ret == 0) {
        printf("✅ mk_put 成功\n");
        printf("\n");
    } else {
        fprintf(stderr, "mk_put失败❌\n");
    }

    // // 测试get
    printf("【基础get测试】\n");
    const char *val1 = mk_get(mk, "test_key1");
    const char *val2 = mk_get(mk, "  test_key2  ");
    const char *val3 = mk_get(mk, "  test_key3");
    const char *val4 = mk_get(mk, "  test_key4");
    const char *val5 = mk_get(mk, "  test_key5");
    const char *val6 = mk_get(mk, "  test_key6  ");
    
    printf("%s ✅\n", val1);
    printf("%s ✅\n", val2);
    printf("%s ✅\n", val3);
    printf("%s ✅\n", val4);
    printf("%s ✅\n", val5);
    printf("%s ✅\n", val6);
    printf("\n");
   

     // 测试put覆盖
    printf("【put覆盖测试】\n");
    ret = mk_put(mk, "test_key1", "new_value1111");
    ret = mk_put(mk, "test_key2", "new_value2222");
    ret = mk_put(mk, "test_key3", "new_value3333");

    printf("【遍历输出所有键值对】\n");
    ret = mk_print(mk);
    if (ret != 0) {
        fprintf(stderr, "遍历输出失败❌\n");
    }
    printf("\n");
   

    // // 测试非法key
    printf("【非法key测试】\n");
    ret = mk_put(mk, "invalid*key", "123");
    ret = mk_put(mk, "invalid   key", "123");
    ret = mk_put(mk, "invalid (()))key", "123");
    ret = mk_put(mk, "中文", "123");
    ret = mk_put(mk, "      ", "123");
    ret = mk_put(mk, "", "123");
    if (ret == -1) {
       
    } else {
        fprintf(stderr, "非法key检验失败\n") ;
    }
    printf("\n");

 // ==================== 遍历输出测试 ====================
   
    printf("【遍历输出所有键值对】\n");
    ret = mk_print(mk);
    if (ret != 0) {
        fprintf(stderr, "遍历输出失败❌\n");
    }
    printf("\n");

    // ====================  删除测试  ====================
    // 删除存在的key
    printf("【删除测试】 \n");
    ret = mk_del(mk, "   test_key1   ");
    ret = mk_del(mk, "   test_key2   ");
    ret = mk_del(mk, "test_key3   ");

    printf("\n");


     printf("【遍历输出所有键值对】\n");
    ret = mk_print(mk);
    if (ret != 0) {
        fprintf(stderr, "遍历输出失败❌\n");
    }
    printf("\n");
    

    // 删除不存在的key
    ret = mk_del(mk, "nonexist_key1");
    ret = mk_del(mk, "nonexist_key2");
    ret = mk_del(mk, "nonexist_key3");
    ret = mk_del(mk, "");
    ret = mk_del(mk, "fsdf*****");
    ret = mk_del(mk, "    中文 ");
    printf("\n");


    

    // ==================== 保存到文件测试 ====================
    printf("【文件加载测试】\n");
    ret = mk_save(mk, "../tests/minikv01.txt");
    if (ret == 0) {
        printf("保存成功 ✅\n");
    } else {
        fprintf(stderr, "文件保存失败 ❌\n");
    }

    printf("\n");

    // ==================== 从文件中加载测试 ====================
    printf("【从文件加载测试】\n");
    ret = mk_load(mk, "../tests/test01.txt");
    if (ret == 0) {
        printf("加载成功 ✅\n");
    } else {
        fprintf(stderr, "文件加载失败 ❌\n");
    }
    printf("\n");

    // ==================== 遍历输出测试 ====================
   
    printf("【遍历输出所有键值对】\n");
    ret = mk_print(mk);
    if (ret != 0) {
        fprintf(stderr, "遍历输出失败❌\n");
    }
    printf("\n");


   
   
   
    // ==================== 销毁Hash表 ====================
    printf("【阶段6】Hash表销毁\n");
    int destroy_ret = mk_destroy(mk);
    if(destroy_ret==0){
        printf("✅ Hash表销毁成功\n");
    }else{
        fprintf(stderr, "❌ Hash表销毁失败，错误码：%d\n", destroy_ret);
    }
    printf("\n===== MiniKV 阶段性测试结束 =====\n");
    return 0;
}