CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iuser
BUILD_DIR = build
TARGET = $(BUILD_DIR)/main.exe

SRC = kernel/main.c user/shell.c user/commands.c
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all run clean rebuild

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all
