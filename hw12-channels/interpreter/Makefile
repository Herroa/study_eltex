CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = interpreter

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean 