#include <stdlib.h>
#include <string.h>

#include "transform.h"

unsigned char *mask = NULL;
int mask_loaded = 0;

int load_mask(const char *smask)
{
    int smask_len = strlen(smask);

    if (smask_len < 1) {
        return -1;
    }

    mask = (unsigned char *) malloc(MASK_LEN);

    for (size_t i = 0; i < MASK_LEN / MASK_BASE_LEN; i++) {
        for (size_t j = 0; j < MASK_BASE_LEN; j++) {
            mask[i * MASK_BASE_LEN + j] = (unsigned char) smask[j % smask_len];
        }
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

int transform(unsigned char *buf, size_t buflen, int tlimit)
{
    size_t bufplen;

    if (tlimit < 0) {
        bufplen = buflen;
    } else {
        bufplen = (size_t) tlimit < buflen ? (size_t) tlimit : buflen;
    }

    // Mask

    size_t bufplen_mask_chunk = bufplen / MASK_LEN;
    for (size_t i = 0; i < bufplen_mask_chunk; i++) {
        ((MASK_UNIT *) buf)[i] = XOR_FUNC(((MASK_UNIT *) buf)[i], *((MASK_UNIT *) mask));
    }

    for (size_t i = bufplen_mask_chunk * MASK_LEN; i < bufplen; i++) {
        buf[i] ^= mask[i % MASK_LEN];
    }

    return 0;
}
