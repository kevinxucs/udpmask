#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "log.h"
#include "transform.h"

void check_gen_mask(struct um_transform *ctx)
{
    time_t time_now = time(NULL);
    if (time_now == -1) {
        log_err("Failed to get current time");
        return;
    }

    if (time_now - ctx->mask_updated <= MASK_TIMEOUT) {
        return;
    }

    log_debug("mask updated more than %s seconds ago", MASK_TIMEOUT);

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        log_err("open(): %s", strerror(errno));
        return;
    }

    ssize_t ret = read(fd, ctx->mask, MASK_LEN);
    if (ret < 0) {
        log_err("read(): %s", strerror(errno));
        goto close;
    }

    ctx->mask_updated = time_now;

close:
    close(fd);
    return;
}

size_t maskbuf(struct um_transform *ctx, unsigned char *buf, size_t buflen) {
    check_gen_mask(ctx);

    transform(buf, buflen, ctx->mask);
    memcpy(buf + buflen, ctx->mask, MASK_LEN);

    return buflen + MASK_LEN;
}

size_t unmaskbuf(struct um_transform *ctx, unsigned char *buf, size_t buflen) {
    unsigned char rcv_mask[MASK_LEN];
    size_t len = buflen - MASK_LEN;

    memcpy(rcv_mask, buf + len, MASK_LEN);
    transform(buf, len, rcv_mask);

    return len;
}
