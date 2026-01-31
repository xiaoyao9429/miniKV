#!/bin/bash

# 定义变量
SRC_DIR="src"
INCLUDE_DIR="include"
LIB_DIR="lib"
STATIC_LIB="libminikv.a"
DYNAMIC_LIB="libminikv.so"
SOURCE_FILES="$SRC_DIR/minikv.c $SRC_DIR/parser.c"

# 创建库目录
mkdir -p $LIB_DIR

# 编译生成目标文件（使用-fPIC选项创建动态库）
echo "正在编译源代码..."
gcc -I$INCLUDE_DIR -fPIC -c $SOURCE_FILES

# 创建静态库
echo "正在创建静态库 $STATIC_LIB..."
ar rcs $LIB_DIR/$STATIC_LIB *.o

# 创建动态库
echo "正在创建动态库 $DYNAMIC_LIB..."
gcc -shared -o $LIB_DIR/$DYNAMIC_LIB *.o

# 清理目标文件
echo "正在清理目标文件..."
rm -f *.o

echo "库文件创建完成！"
echo "静态库：$LIB_DIR/$STATIC_LIB"
echo "动态库：$LIB_DIR/$DYNAMIC_LIB"
