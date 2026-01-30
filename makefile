
CC = gcc
CFLAGS =  -Wall -Wextra -Werror -g -Iinclude 

# SRC_DIR: 源代码文件所在目录
SRC_DIR = src
# OBJ_DIR: 编译生成的中间目标文件(.o)存放目录
OBJ_DIR = obj
# BIN: 最终生成的可执行文件名称
BIN = minikv
# SRCS: 手动列出需要编译的源文件列表
SRCS = $(SRC_DIR)/minikv.c $(SRC_DIR)/parser.c $(SRC_DIR)/main.c


#  将 SRCS 中所有的 src/%.c 替换为 obj/%.o
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# 默认命令，生成miniKV
all: $(BIN)


# $@ 代表目标文件 (minikv)
# $^ 代表所有依赖文件 (所有的 .o 文件)
$(BIN): $(OBJS)
	$(CC) $^ $(CFLAGS) -o $@ 

# 编译规则：将 .c 文件编译成 .o 文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# 用于删除所有编译生成的文件
clean:
	rm -rf $(OBJ_DIR) $(BIN)

# .PHONY 声明伪目标
# 告诉 Make：all 和 clean 不是真正的文件名，而是操作指令
.PHONY: all clean
