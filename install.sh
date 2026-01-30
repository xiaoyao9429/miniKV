#!/bin/sh

# 获取脚本所在目录（支持从任意位置调用）
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR" || { echo "无法进入脚本目录"; exit 1; }

# 如果可执行文件不存在则触发编译
if [ ! -f "$SCRIPT_DIR/minikv" ]; then
    echo "正在编译 MiniKV..."
    make
    if [ $? -ne 0 ]; then
        echo "编译失败"
        exit 1
    fi
fi

# 安装到 /usr/local/bin（需要 sudo 权限）
echo "正在安装 MiniKV 到系统目录..."
sudo cp "$SCRIPT_DIR/minikv" /usr/local/bin/
sudo chmod +x /usr/local/bin/minikv

# 验证安装
if [ -f "/usr/local/bin/minikv" ]; then
    echo "安装成功!"
else
    echo "安装失败，请检查权限"
    exit 1
fi