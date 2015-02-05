#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "transform.h"
#include "udpmask.h"

#define MASK_UNIT   8   // 64 bits

unsigned char *mask = NULL;
size_t mask_len = 0;
int mask_loaded = 0;

int load_mask(const char *smask)
{
    int smask_len = strlen(smask);

    if (smask_len < 1) {
        return -1;
    }

    int smask_len_mul = smask_len / MASK_UNIT;

    if (smask_len_mul * MASK_UNIT < smask_len) {
        smask_len_mul++;
    }

    mask_len = smask_len_mul * MASK_UNIT;

    mask = (unsigned char *) malloc(mask_len);

    for (size_t i = 0; i < mask_len; i++) {
        mask[i] = (unsigned char) smask[i % smask_len];
    }

    mask_loaded = 1;

    return 0;
}

void unload_mask(void)
{
    free(mask);
    mask = NULL;
    mask_loaded = 0;
}

int transform(__attribute__((unused)) enum um_mode mode,
              const unsigned char *buf, size_t buflen,
              unsigned char *outbuf, size_t *outbuflen)
{
    if (buflen < 8) {
        for (size_t i = 0; i < buflen; i++) {
            outbuf[i] = buf[i] ^ mask[i % mask_len];
        }
    } else {
        size_t buflen_mul = buflen / MASK_UNIT;
        size_t mask_len_mul = mask_len / 8;

        for (size_t i = 0; i < buflen_mul; i++) {
            ((uint64_t *) outbuf)[i] = ((uint64_t *) buf)[i] ^
                                       ((uint64_t *) mask)[i % mask_len_mul];
        }

        for (size_t i = buflen_mul * 8; i < buflen; i++) {
            outbuf[i] = buf[i] ^ mask[i % mask_len];
        }
    }

    *outbuflen = buflen;

    return 0;
}
