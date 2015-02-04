#include <stdlib.h>

#include "transform.h"
#include "udpmask.h"

const unsigned char *mask;
size_t mask_len = 0;

int transform(__attribute__((unused)) enum um_mode mode,
              const unsigned char *buf, size_t buflen,
              unsigned char *outbuf, size_t *outbuflen)
{
    for (size_t i = 0; i < buflen; i++) {
        outbuf[i] = buf[i] ^ mask[i % mask_len];
    }

    *outbuflen = buflen;

    return 0;
}
