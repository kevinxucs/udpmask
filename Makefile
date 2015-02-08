CC	= cc
CFLAGS	= -std=gnu99 -Os -Wall
OBJS	= udpmask.o log.o
TESTS	= tests/test_transform_xor

all: udpmask_xor udpmask_add

udpmask_xor: $(OBJS) transform_xor.o
	$(CC) $(CFLAGS) -o $@ $^

udpmask_add: $(OBJS) transform_add.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

tests/test_%: tests/test_%.c %.o
	$(CC) $(CFLAGS) -I. -o $@ $^

test: $(TESTS)
	$(foreach test_cmd,$(TESTS),$(test_cmd))

clean:
	rm -f udpmask_xor udpmask_add $(TESTS) *.o

.PHONY: all clean test
