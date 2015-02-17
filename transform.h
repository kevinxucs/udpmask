#ifndef _incl_TRANSFORM_H
#define _incl_TRANSFORM_H

#include <stdlib.h>

#include "udpmask.h"

extern int mask_loaded;

int load_mask(const char *smask);

void unload_mask(void);

int transform(__attribute__((unused)) enum um_mode mode,
              unsigned char *buf, size_t buflen, int tlimit);

#endif /* _incl_TRANSFORM_H */
