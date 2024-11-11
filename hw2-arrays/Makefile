CC = gcc
CFLAGS = -Wall -Wextra -Werror

TARGET = main
OBJS = main.o 
SRC = main.c

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJS): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OBJS) $(TARGET)