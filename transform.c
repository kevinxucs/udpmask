#include <stdlib.h>
#include <string.h>

#include "transform.h"

#define mask_unit       unsigned int
#define MASK_UNIT_LEN   ((int) sizeof(mask_unit))

unsigned char *mask = NULL;
size_t mask_len = 0;
int mask_loaded = 0;

int load_mask(const char *smask)
{
    int smask_len = strlen(smask);

    if (smask_len < 1) {
        return -1;
    }

    mask_len = MASK_UNIT_LEN;
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

int transform(unsigned char *buf, size_t buflen, int tlimit)
{
    size_t bufplen;

    if (tlimit < 0) {
        bufplen = buflen;
    } else {
        bufplen = (size_t) tlimit < buflen ? (size_t) tlimit : buflen;
    }

    // Mask

    size_t bufplen_mask_chunk = bufplen / mask_len;
    for (size_t i = 0; i < bufplen_mask_chunk; i++) {
        ((mask_unit *) buf)[i] ^= *((mask_unit *) mask);
    }

    for (size_t i = bufplen_mask_chunk * mask_len; i < bufplen; i++) {
        buf[i] ^= mask[i % mask_len];
    }

    return 0;
}
