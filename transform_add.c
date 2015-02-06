#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "transform.h"
#include "udpmask.h"

int mask = 0;
int mask_loaded = 0;

int load_mask(const char *smask)
{
    mask = atoi(smask);
    mask_loaded = 1;

    return 0;
}

void unload_mask(void)
{
    mask = 0;
    mask_loaded = 0;
}

int transform(__attribute__((unused)) enum um_mode mode,
              const unsigned char *buf, size_t buflen,
              unsigned char *outbuf, size_t *outbuflen,
              int tlimit)
{
    *outbuflen = buflen;

    if (mask == 0) {
        memcpy((void *) outbuf, (void *) buf, buflen);
        return 0;
    }

    size_t bufplen;

    if (tlimit < 0) {
        bufplen = buflen;
    } else {
        bufplen = (size_t) tlimit < buflen ? (size_t) tlimit : buflen;
    }

    if (mode == UM_MODE_CLIENT) {
        for (size_t i = 0; i < bufplen; i++) {
            outbuf[i] = buf[i] + mask;
        }
    } else if (mode == UM_MODE_SERVER) {
        for (size_t i = 0; i < bufplen; i++) {
            outbuf[i] = buf[i] - mask;
        }
    }

    size_t copylen = buflen - bufplen;

    if (copylen > 0) {
        memcpy((void *) (outbuf + bufplen), (void *) (buf + bufplen), copylen);
    }

    return 0;
}
