CC	= gcc
CFLAGS	:= $(CFLAGS) -std=gnu99 -O2 -Wall
OBJS	= udpmask.o log.o transform.o
TESTS	= tests/test_transform
EXEC	= udpmask

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

tests/test_%: tests/test_%.c %.o
	$(CC) $(CFLAGS) -I. -o $@ $^

test: $(TESTS)
	$(foreach test_cmd,$(TESTS),$(test_cmd))

clean:
	rm -f $(EXEC) $(TESTS) *.o

.PHONY: all clean test
