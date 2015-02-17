CC	= cc
CFLAGS	= -std=gnu99 -Os -Wall
OBJS	= udpmask.o log.o transform.o
TESTS	= tests/test_transform

all: udpmask

udpmask: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

tests/test_%: tests/test_%.c %.o
	$(CC) $(CFLAGS) -I. -o $@ $^

test: $(TESTS)
	$(foreach test_cmd,$(TESTS),$(test_cmd))

clean:
	rm -f udpmask $(TESTS) *.o

.PHONY: all clean test
