#ifndef _incl_TRANSFORM_H
#define _incl_TRANSFORM_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define MASK_UNIT       uint32_t
#define MASK_LEN        ((int) sizeof(MASK_UNIT))
#define MASK_TIMEOUT    ((time_t) 60)

#define transform(buf, buflen, m)                               \
    do {                                                        \
        size_t c = buflen / MASK_LEN;                           \
        for (size_t i = 0; i < c; i++) {                        \
            ((MASK_UNIT *) buf)[i] ^= *((MASK_UNIT *) m);       \
        }                                                       \
        for (size_t i = c * MASK_LEN; i < buflen; i++) {        \
            buf[i] ^= m[i % MASK_LEN];                          \
        }                                                       \
    } while (0)                                                 \

void check_gen_mask();
size_t maskbuf(unsigned char *, size_t);
size_t unmaskbuf(unsigned char *, size_t);

typedef size_t (*buf_func)(unsigned char *, size_t);

#endif /* _incl_TRANSFORM_H */
