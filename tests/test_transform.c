#include <stdio.h>

#include "transform.h"
#include "udpmask.h"

extern unsigned char mask[MASK_LEN];

void print_mask() {
    printf("mask:");
    for (int i = 0; i < MASK_LEN; i++) {
        printf(" 0x%02X", mask[i]);
    }
    printf("\n");
}

int main(void)
{
    printf("MASK_LEN: %d\n", MASK_LEN);

    check_gen_mask();

    print_mask();

    unsigned char buf[1024] = "ABCDABCDABCDABCDCD";
    size_t buflen = 18;

    printf("buf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    buflen = maskbuf(buf, buflen);

    printf("maskbuf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    buflen = unmaskbuf(buf, buflen);

    printf("buf:");
    for (size_t i = 0; i < buflen; i++) {
        printf(" 0x%02X", buf[i]);
    }
    printf("\n");

    return 0;
}
