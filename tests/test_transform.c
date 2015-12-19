#include <stdio.h>

#include "transform.h"
#include "udpmask.h"

int main(void)
{
    unsigned char buf[1024] = "ABCDEFGHB";
    size_t buflen = 9;

    load_mask("ABCDEFGH");
    transform(buf, buflen, -1);
    unload_mask();

    for (size_t i = 0; i < buflen; i++) {
        printf("0x%02X ", buf[i]);
    }

    printf("\n");

    return 0;
}
