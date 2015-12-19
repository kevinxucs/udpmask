#include <stdio.h>

#include "transform.h"
#include "udpmask.h"

extern unsigned char *mask;

int main(void)
{
    printf("MASK_BASE_LEN: %d\n", MASK_BASE_LEN);
    printf("MASK_LEN: %d\n", MASK_LEN);

    unsigned char buf[1024] = "ABCDABCDABCDABCDCD";
    size_t buflen = 18;

    load_mask("ABCD");

    for (size_t i = 0; i < MASK_LEN; i++) {
        printf("%c ", mask[i]);
    }
    printf("\n");

    transform(buf, buflen, -1);
    unload_mask();

    for (size_t i = 0; i < buflen; i++) {
        printf("0x%02X ", buf[i]);
    }
    printf("\n");

    return 0;
}
