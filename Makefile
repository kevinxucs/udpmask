CC		= gcc
CFLAGS	= -std=gnu99 -Os -Wall
OBJS	= udpmask.o

all: udpmask

udpmask: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f udpmask $(OBJS)

.PHONY: all
