#ifndef _incl_TRANSFORM_H
#define _incl_TRANSFORM_H

#include <stdlib.h>

#include "udpmask.h"

int load_mask(const char *smask);

void unload_mask(void);

int transform(__attribute__((unused)) enum um_mode mode,
              const unsigned char *buf, size_t buflen,
              unsigned char *outbuf, size_t *outbuflen);

#endif /* _incl_TRANSFORM_H */
