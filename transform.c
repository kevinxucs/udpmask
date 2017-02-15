#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "log.h"
#include "transform.h"

unsigned char mask[MASK_LEN];
time_t mask_updated = 0;

void check_gen_mask()
{
    time_t time_now = time(NULL);
    if (time_now == -1) {
        log_err("Failed to get current time");
        return;
    }

    if (time_now - mask_updated <= MASK_TIMEOUT) {
        return;
    }

    log_debug("mask updated more than %s seconds ago", MASK_TIMEOUT);

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        log_err("open(): %s", strerror(errno));
        return;
    }

    ssize_t ret = read(fd, mask, MASK_LEN);
    if (ret < 0) {
        log_err("read(): %s", strerror(errno));
        goto close;
    }

    mask_updated = time_now;

close:
    close(fd);
    return;
}

size_t maskbuf(unsigned char *buf, size_t buflen) {
    check_gen_mask();

    transform(buf, buflen, mask);

    memcpy(buf + buflen, mask, MASK_LEN);

    return buflen + MASK_LEN;
}

size_t unmaskbuf(unsigned char *buf, size_t buflen) {
    unsigned char rcv_mask[MASK_LEN];

    size_t len = buflen - MASK_LEN;

    memcpy(rcv_mask, buf + len, MASK_LEN);

    transform(buf, len, rcv_mask);

    return len;
}
