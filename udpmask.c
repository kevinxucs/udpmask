#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "log.h"
#include "udpmask.h"

static int bind_sock = -1;
static const unsigned char *mask;
static size_t mask_len = 0;
static struct sockaddr_in conn_addr;
static int timeout = UM_TIMEOUT;
static struct um_sockmap map[UM_MAX_CLIENT];

static volatile sig_atomic_t signal_alrm = 0;

static int usage(void)
{
    const char ubuf[] =
    "Usage: udpmask -m mode -s mask\n"
    "               -c remote -o remote port\n"
    "               [-l listen] [-p listen port]\n"
    "               [-t timeout] [-h]\n";
    fprintf(stderr, ubuf);
    return 1;
}

static inline int max_sock_fd(void)
{
    int max_sock = bind_sock;
    for (int i = 0; i < ARRAY_SIZE(map); i++) {
        if (map[i].in_use && map[i].sock > max_sock) {
            max_sock = map[i].sock;
        }
    }

    return max_sock + 1;
}

static inline int um_sockmap_ins(int sock, struct sockaddr_in *addr)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZE(map); i++) {
        if (!map[i].in_use) {
            map[i].in_use = 1;
            map[i].sock = sock;
            map[i].last_use = TIME_INVALID;
            map[i].from = *addr;
            break;
        }
    }

    if (i >= ARRAY_SIZE(map)) {
        return -1;
    }

    return i;
}

static inline int sockaddr_in_cmp(struct sockaddr_in *a,
                                  struct sockaddr_in *b)
{
    int af_cmp = a->sin_family - b->sin_family;
    if (af_cmp != 0) {
        return af_cmp;
    }

    int port_cmp = a->sin_port - b->sin_port;
    if (port_cmp != 0) {
        return port_cmp;
    }

    long in_addr_cmp = a->sin_addr.s_addr - b->sin_addr.s_addr;
    if (in_addr_cmp != 0) {
        return (int) in_addr_cmp;
    }

    return 0;
}

static void sighanlder(int signum)
{
    if (signum == SIGALRM) {
        signal_alrm = 1;
    }
}

int transform(__attribute__((unused)) enum um_mode mode,
              const unsigned char *buf, size_t buflen,
              unsigned char *outbuf, size_t *outbuflen)
{
    for (size_t i = 0; i < buflen; i++) {
        outbuf[i] = buf[i] ^ mask[i % mask_len];
    }

    *outbuflen = buflen;

    return 0;
}

int start(enum um_mode mode)
{
    ssize_t r;
    int sr;
    time_t t;
    int si;
    int tmp_sock = -1;

    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len = sizeof(recv_addr);

    unsigned char buf[UM_BUFFER];
    size_t bl;
    size_t obl;

    fd_set active_fd_set, read_fd_set;

    FD_ZERO(&active_fd_set);
    FD_SET(bind_sock, &active_fd_set);

    if (timeout > 0) {
        log_info("Connection timeout %d", timeout);
        signal(SIGALRM, sighanlder);
        alarm(UM_CHK_INTERV);
    }

    while (1) {
        read_fd_set = active_fd_set;
        si = -1;

        if (signal_alrm) {
            log_debug("SIGALRM");
            t = time(NULL);

            for (int i = 0; i < ARRAY_SIZE(map); i++) {
                if (map[i].in_use &&
                    (map[i].last_use == TIME_INVALID ||
                     t - map[i].last_use >= timeout)) {
                    map[i].in_use = 0;
                    close(map[i].sock);

                    log_info("Purged connection from [%s:%hu]",
                             inet_ntoa(map[i].from.sin_addr),
                             ntohs(map[i].from.sin_port));
                }
            }

            signal_alrm = 0;
            alarm(UM_CHK_INTERV);
        }

        sr = select(max_sock_fd(), &read_fd_set, NULL, NULL, NULL);
        if (sr <= 0) {
            log_debug("select() returns %d", sr);
            continue;
        }

        if (FD_ISSET(bind_sock, &read_fd_set)) {
            r = recvfrom(bind_sock, (void *) buf, UM_BUFFER, 0,
                         (struct sockaddr *) &recv_addr, &recv_addr_len);

            if (r > 0) {
                bl = (size_t) r;

                for (int i = 0; i < ARRAY_SIZE(map); i++) {
                    if (map[i].in_use &&
                        sockaddr_in_cmp(&recv_addr, &(map[i].from)) == 0) {
                        si = i;
                        break;
                    }
                }

                if (si < 0) {
                    log_info("New connection from [%s:%hu]",
                             inet_ntoa(recv_addr.sin_addr),
                             ntohs(recv_addr.sin_port));

                    tmp_sock = NEW_SOCK();
                    if (tmp_sock < 0) {
                        log_err("socket(): %s", strerror(errno));
                    } else if ((si = um_sockmap_ins(tmp_sock, &recv_addr)) >= 0) {
                        FD_SET(tmp_sock, &active_fd_set);
                        connect(tmp_sock, (struct sockaddr *) &conn_addr,
                                sizeof(conn_addr));
                    } else {
                        log_warn("Max clients reached. "
                                 "Dropping new connection [%s:%hu]",
                                 inet_ntoa(recv_addr.sin_addr),
                                 ntohs(recv_addr.sin_port));
                    }
                }

                if (si >= 0) {
                    transform(mode, buf, bl, buf, &obl);
                    send(map[si].sock, (void *) buf, obl, 0);

                    t = time(NULL);
                    if (t != TIME_INVALID) {
                        map[si].last_use = t;
                    }
                }
            }
        }

        for (int i = 0; i < ARRAY_SIZE(map); i++) {
            if (map[i].in_use && FD_ISSET(map[i].sock, &read_fd_set)) {
                r = recv(map[i].sock, (void *) buf, UM_BUFFER, 0);

                t = time(NULL);
                if (t != TIME_INVALID) {
                    map[i].last_use = t;
                }

                if (r > 0) {
                    bl = (size_t) r;

                    transform(mode, buf, bl, buf, &obl);
                    sendto(bind_sock, (void *) buf, obl, 0,
                           (struct sockaddr *) &map[i].from,
                           sizeof(map[i].from));
                }
            }
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    startlog("udpmask");

    memset((void *) &conn_addr, 0, sizeof(conn_addr));
    memset((void *) map, 0, sizeof(map));

    enum um_mode mode = UM_MODE_NONE;
    struct in_addr addr = { .s_addr = INADDR_ANY };
    uint16_t port = 0;
    struct in_addr addr_conn = { .s_addr = 0 };
    uint16_t port_conn = 0;

    struct sockaddr_in bind_addr;
    memset((void *) &bind_addr, 0, sizeof(bind_addr));

    struct hostent * rh;
    int show_usage = 0;
    int c;
    int r;

    while ((c = getopt(argc, argv, "m:p:l:s:c:o:t:h")) != -1) {
        switch (c) {
        case 'm':
            if (strcmp(optarg, "server") == 0) {
                mode = UM_MODE_SERVER;
            } else if (strcmp(optarg, "client") == 0) {
                mode = UM_MODE_CLIENT;
            } else {
                show_usage = 1;
            }
            break;

        case 's':
            mask_len = strlen(optarg);
            mask = (unsigned char *) optarg;
            break;

        case 'l':
            r = inet_pton(AF_INET, optarg, (void *) &addr);
            if (r != 1) {
                show_usage = 1;
            }
            if (r < 0) {
                log_err("inet_pton(): %s", strerror(errno));
            }
            break;

        case 'p':
            port = (uint16_t) atoi(optarg);
            break;

        case 'c':
            rh = gethostbyname2(optarg, AF_INET);
            if (!rh) {
                show_usage = 1;
                log_err("gethostbyname2(): %s", hstrerror(h_errno));
            }
            memcpy(&addr_conn, rh->h_addr_list[0], rh->h_length);

            break;

        case 'o':
            port_conn = (uint16_t) atoi(optarg);
            break;

        case 't':
            r = atoi(optarg);
            if (r >= 0) {
                timeout = r;
            }
            break;

        case 'h':
        case '?':
        default:
            show_usage = 1;
            break;
        }
    }

    if (mask_len == 0 || port_conn == 0 || addr_conn.s_addr == 0) {
        show_usage = 1;
    }

    bind_sock = NEW_SOCK();
    if (bind_sock < 0) {
        log_err("socket(): %s", strerror(errno));
        ret = 1;
        goto exit;
    }

    switch (mode) {
    case UM_MODE_SERVER:
        log_info("Server mode");
        if (port == 0) {
            port = UM_SERVER_PORT;
        }
        break;

    case UM_MODE_CLIENT:
        log_info("Client mode");
        if (port == 0) {
            port = UM_CLIENT_PORT;
        }
        break;

    default:
        log_info("Unknown mode");
        show_usage = 1;
        break;
    }

    if (show_usage) {
        ret = usage();
        goto exit;
    }

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr = addr;
    bind_addr.sin_port = htons(port);

    log_info("Bind to [%s:%hu]", inet_ntoa(addr), port);

    r = bind(bind_sock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));
    if (r != 0) {
        log_err("bind(): %s", strerror(errno));
        ret = 1;
        goto exit;
    }

    conn_addr.sin_family = AF_INET;
    conn_addr.sin_addr = addr_conn;
    conn_addr.sin_port = htons(port_conn);

    log_info("Remote address [%s:%hu]", inet_ntoa(addr_conn), port_conn);

    ret = start(mode);

exit:
    endlog();
    return ret;
}
