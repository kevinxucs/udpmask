#ifndef _incl_TRANSFORM_H
#define _incl_TRANSFORM_H

#include <stdlib.h>

#include "udpmask.h"

extern const unsigned char *mask;
extern size_t mask_len;

int transform(__attribute__((unused)) enum um_mode mode,
              const unsigned char *buf, size_t buflen,
              unsigned char *outbuf, size_t *outbuflen);

#endif /* _incl_TRANSFORM_H */
