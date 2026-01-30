#!/bin/sh

# 卸载脚本：移除系统中的 minikv 可执行文件

echo "开始卸载 MiniKV..."

# 检查可执行文件是否存在于系统目录
if [ -f "/usr/local/bin/minikv" ]; then
    echo "找到已安装的 MiniKV，正在移除..."
    # 使用 sudo 权限移除
    sudo rm -f /usr/local/bin/minikv
    
    # 验证卸载
    if [ ! -f "/usr/local/bin/minikv" ]; then
        rm -rf minikv
        echo "卸载成功！MiniKV 已从系统中移除"
    else
        echo "卸载失败，请检查权限"
        exit 1
    fi
else
    echo "未找到已安装的 MiniKV（在 /usr/local/bin/minikv）"
    
    # 检查用户目录
    if [ -f "$HOME/bin/minikv" ]; then
        echo "找到用户目录中的 MiniKV，正在移除..."
        rm -f "$HOME/bin/minikv"
        if [ ! -f "$HOME/bin/minikv" ]; then
            rm -rf minikv
            echo "卸载成功！MiniKV 已从用户目录移除"
        else
            echo "卸载失败，请检查权限"
            exit 1
        fi
    else
        rm -rf minikv
        echo "MiniKV 未安装，无需卸载"
        
        exit 0
    fi
fi



# 提示用户清理环境变量（如果手动添加过）
echo "\n提示：如果您手动将 MiniKV 目录添加到了环境变量，请在 ~/.bashrc 或 ~/.zshrc 中移除对应的 export 语句"
echo "卸载完成！"
