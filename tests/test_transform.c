#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "transform.h"
#include "udpmask.h"

void print_mask(struct um_transform *ctx) {
    printf("mask:");
    for (int i = 0; i < MASK_LEN; i++) {
        printf(" 0x%02X", ctx->mask[i]);
    }
    printf("\n");
}

int main(void)
{
    printf("MASK_LEN: %d\n", MASK_LEN);

    struct um_transform tran;
    memset(&tran, 0, sizeof(tran));

    int iter = 10000;
    struct timeval t_start, t_end;
    double t_diff;

    gettimeofday(&t_start, NULL);
    for (int i = 0; i < iter; i++) {
        check_gen_mask(&tran);
        tran.mask_updated = 0;
    }
    gettimeofday(&t_end, NULL);
    t_diff = (double) ((t_end.tv_sec - t_start.tv_sec) * 1e6 + (t_end.tv_usec - t_start.tv_usec));
    printf("Time for check_gen_mask, %d iterations: %f us\n", iter, t_diff);
    printf("Time for check_gen_mask, 1 iterations: %f us\n", t_diff / iter);

    check_gen_mask(&tran);
    print_mask(&tran);

    unsigned char buf[1024] = "ABCDABCDABCDABCDCD";
    size_t buflen = 18;

    printf("buf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    gettimeofday(&t_start, NULL);
    for (int i = 0; i < iter; i++) {
        maskbuf(&tran, buf, buflen);
    }
    gettimeofday(&t_end, NULL);
    t_diff = (double) ((t_end.tv_sec - t_start.tv_sec) * 1e6 + (t_end.tv_usec - t_start.tv_usec));
    printf("Time for maskbuf, %d iterations: %f us\n", iter, t_diff);
    printf("Time for maskbuf, 1 iterations: %f us\n", t_diff / iter);

    buflen = maskbuf(&tran, buf, buflen);

    printf("maskbuf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    gettimeofday(&t_start, NULL);
    for (int i = 0; i < iter; i++) {
        unmaskbuf(&tran, buf, buflen);
    }
    gettimeofday(&t_end, NULL);
    t_diff = (double) ((t_end.tv_sec - t_start.tv_sec) * 1e6 + (t_end.tv_usec - t_start.tv_usec));
    printf("Time for unmaskbuf, %d iterations: %f us\n", iter, t_diff);
    printf("Time for unmaskbuf, 1 iterations: %f us\n", t_diff / iter);

    buflen = unmaskbuf(&tran, buf, buflen);

    printf("buf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    return 0;
}
