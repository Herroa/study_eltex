CC := gcc
CFLAGS := -Wall -Wextra -Werror -Iinclude

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include
BIN := app

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

all: $(BIN)
