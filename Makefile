CC	= gcc
AR	= ar

CFLAGS	= -std=gnu99 -Os -Wall
ifeq ($(OS),Windows_NT)
	LDFLAGS	= -lws2_32
endif

OBJS	= udpmask.o log.o transform.o
EXEC	= udpmask

LIBCOMPAT	= compat/libcompat.a
COMPAT_OBJS	= compat/inet_pton.o compat/socket.o

TESTS	= tests/test_transform

all: $(EXEC)

$(EXEC): $(OBJS) $(LIBCOMPAT)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIBCOMPAT): $(COMPAT_OBJS)
	$(AR) rcs $@ $^

tests/test_%: tests/test_%.c %.o
	$(CC) $(CFLAGS) -I. -o $@ $^

test: $(TESTS)
	$(foreach test_cmd,$(TESTS),$(test_cmd))

clean:
	rm -f $(EXEC) $(OBJS) $(LIBCOMPAT) $(COMPAT_OBJS) $(TESTS) 

.PHONY: all clean test
