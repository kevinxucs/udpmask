#ifndef _incl_TRANSFORM_H
#define _incl_TRANSFORM_H

#include <stdlib.h>

#define MASK_BASE_UNIT  unsigned int
#define MASK_BASE_LEN   ((int) sizeof(MASK_BASE_UNIT))

#ifdef __SSE2__

#include <emmintrin.h>
#define MASK_UNIT       __m128
#define XOR_FUNC(a, b)  (_mm_xor_si128((a), (b)))

#else

#define MASK_UNIT       MASK_BASE_UNIT
#define XOR_FUNC(a, b)  ((a) ^ (b))

#endif

#define MASK_LEN        ((int) sizeof(MASK_UNIT))


extern int mask_loaded;

int load_mask(const char *smask);

void unload_mask(void);

int transform(unsigned char *buf, size_t buflen, int tlimit);

#endif /* _incl_TRANSFORM_H */
