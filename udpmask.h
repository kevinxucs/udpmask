#ifndef _incl_UDPMASK_H
#define _incl_UDPMASK_H

#include <time.h>
#include <netinet/in.h>

#define UM_SERVER_PORT  51194
#define UM_CLIENT_PORT  61194
#define UM_MAX_CLIENT   16
#define UM_BUFFER       65507
#define UM_TIMEOUT      300     // socket clean up timeout
#define UM_HOST_TIMEOUT 60      // dns lookup cache timeout

#define TIME_INVALID    (time_t) -1

#define ARRAY_SIZE(a)   (int) (sizeof(a) / sizeof(a[0]))
#define NEW_SOCK()      socket(AF_INET, SOCK_DGRAM, 0)

enum um_mode {
    UM_MODE_NONE = -1,
    UM_MODE_SERVER,
    UM_MODE_CLIENT,
    UM_MODE_PASSTHROU
};

struct um_sockmap {
    int                 in_use;
    int                 sock;
    time_t              last_use;
    struct sockaddr_in  from;
};

#endif /* _incl_UDPMASK_H */
