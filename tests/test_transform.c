#include <stdio.h>
#include <string.h>

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
    struct um_transform tran;
    memset(&tran, 0, sizeof(tran));

    printf("MASK_LEN: %d\n", MASK_LEN);

    check_gen_mask(&tran);

    print_mask(&tran);

    unsigned char buf[1024] = "ABCDABCDABCDABCDCD";
    size_t buflen = 18;

    printf("buf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    buflen = maskbuf(&tran, buf, buflen);

    printf("maskbuf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    buflen = unmaskbuf(&tran, buf, buflen);

    printf("buf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    return 0;
}
