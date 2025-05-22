CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = channels
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean 