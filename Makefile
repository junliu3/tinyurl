# TinyURL 短地址服务 Makefile
# 数据结构：单向链表（尾插法）

CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11 -g -O2
TARGET   = tinyurl
OBJS     = main.o short_url.o linked_list.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c linked_list.h short_url.h
	$(CC) $(CFLAGS) -c main.c

short_url.o: short_url.c short_url.h
	$(CC) $(CFLAGS) -c short_url.c

linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -c linked_list.c

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
