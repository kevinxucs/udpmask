#ifndef _incl_TRANSFORM_H
#define _incl_TRANSFORM_H

#include <stdint.h>
#include <stdlib.h>

#define MASK_UNIT       uint32_t
#define MASK_LEN        ((int) sizeof(MASK_UNIT))
#define MASK_MAXCT      ((unsigned int) 100000)

struct um_transform {
    unsigned char   mask[MASK_LEN];
    unsigned int    mask_ct;
};

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


#define genmask(mask, n)                                        \
    do {                                                        \
        for (size_t i = 0; i < n; i++) {                        \
            mask[i] = (unsigned char) (rand() % 256);           \
        }                                                       \
    } while (0)                                                 \

void check_gen_mask(struct um_transform *);
size_t maskbuf(struct um_transform *, unsigned char *, size_t);
size_t unmaskbuf(struct um_transform *, unsigned char *, size_t);
size_t masknoop(struct um_transform *, unsigned char *, size_t);

typedef size_t (*buf_func)(struct um_transform *, unsigned char *, size_t);

#endif /* _incl_TRANSFORM_H */
