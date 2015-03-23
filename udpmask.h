#ifndef _incl_UDPMASK_H
#define _incl_UDPMASK_H

#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef WIN32
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>

#define SIGHUP          1
#endif

#define UM_SERVER_PORT  51194
#define UM_CLIENT_PORT  61194
#define UM_MAX_CLIENT   16
#define UM_BUFFER       65536
#define UM_TIMEOUT      300     // 5 minutes

#define TIME_INVALID    (time_t) -1

#define ARRAY_SIZE(a)   (int) (sizeof(a) / sizeof(a[0]))
#define NEW_SOCK()      socket(AF_INET, SOCK_DGRAM, 0)

enum um_mode {
    UM_MODE_NONE = -1,
    UM_MODE_SERVER,
    UM_MODE_CLIENT
};

struct um_sockmap {
    int                 in_use;
    int                 sock;
    time_t              last_use;
    struct sockaddr_in  from;
};

#endif /* _incl_UDPMASK_H */
