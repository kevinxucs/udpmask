#include <stdio.h>

#include "transform.h"
#include "udpmask.h"

int main(void)
{
    unsigned char outbuf[1024];
    size_t outbuflen;

    load_mask("ABCDEFGH");
    transform(UM_MODE_SERVER, (unsigned char *) "ABCDEFGHB", 9, outbuf, &outbuflen, 0);
    unload_mask();

    printf("outbuflen: %lu\n", outbuflen);

    for (size_t i = 0; i < outbuflen; i++) {
        printf("0x%02X ", outbuf[i]);
    }

    printf("\n");

    return 0;
}
