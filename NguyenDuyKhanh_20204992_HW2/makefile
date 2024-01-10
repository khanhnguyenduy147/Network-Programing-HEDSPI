CC = gcc
CFLAGS = -Wall
SRC_DIR = src
INC_DIR = inc
BIN_DIR = bin

# Danh sách các file .c trong thư mục src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Đối tượng tương ứng với các file .c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRCS))

# Chương trình thực thi
TARGET = $(BIN_DIR)/main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)
	
run: $(TARGET)
	./$(TARGET)