CC	= cc
CFLAGS	= -std=gnu99 -Os -Wall
OBJS	= udpmask.o log.o transform_xor.o
TESTS	= tests/test_transform_xor

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
	rm -f udpmask $(TESTS) $(OBJS)

.PHONY: all clean test
