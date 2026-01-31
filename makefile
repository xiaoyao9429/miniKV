
CC = gcc
CFLAGS =  -Wall -Wextra -Werror -g -Iinclude -fpic

# SRC_DIR: 源代码文件所在目录
SRC_DIR = src
# OBJ_DIR: 编译生成的中间目标文件(.o)存放目录
OBJ_DIR = obj
# LIB_DIR: 生成的库文件存放目录
LIB_DIR = lib
# BIN: 最终生成的可执行文件名称
BIN = minikv
# 库名称
LIB_NAME = minikv
# SRCS: 手动列出需要编译的源文件列表
SRCS = $(SRC_DIR)/minikv.c $(SRC_DIR)/parser.c $(SRC_DIR)/main.c
# LIB_SRCS: 用于生成库的源文件列表（不包括main.c）
LIB_SRCS = $(SRC_DIR)/minikv.c $(SRC_DIR)/parser.c

#  将 SRCS 中所有的 src/%.c 替换为 obj/%.o
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
#  将 LIB_SRCS 中所有的 src/%.c 替换为 obj/%.o
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(LIB_SRCS))
# 静态库名称
STATIC_LIB = $(LIB_DIR)/lib$(LIB_NAME).a
# 动态库名称
DYNAMIC_LIB = $(LIB_DIR)/lib$(LIB_NAME).so


# .PHONY 声明伪目标
.PHONY: all clean



# 默认命令，生成miniKV、静态库和动态库
all: $(BIN) $(STATIC_LIB) $(DYNAMIC_LIB)


# $@ 代表目标文件 (minikv)
# $^ 代表所有依赖文件 (所有的 .o 文件)
$(BIN): $(OBJS)
	$(CC) $^ $(CFLAGS) -o $@ 

# 静态库构建规则
$(STATIC_LIB): $(LIB_OBJS)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# 动态库构建规则
$(DYNAMIC_LIB): $(LIB_OBJS)
	@mkdir -p $(LIB_DIR)
	$(CC) -shared  -o $@ $^ $(CFLAGS)

# 编译规则：将 .c 文件编译成 .o 文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# 用于删除所有编译生成的文件
clean:
	rm -rf $(OBJ_DIR) $(BIN) $(LIB_DIR)

