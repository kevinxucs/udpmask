#include <netdb.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "udpmask.h"

int bind_sock = -1;
int conn_sock = -1;
unsigned char *mask = 0;
size_t mask_len = 0;

static int usage(void)
{
    const char ubuf[] =
    "Usage: udpmask -m mode -s mask\n"
    "               -c remote -o remote port\n"
    "               [-l listen] [-p listen port]\n"
    "               [-h]\n";
    fprintf(stderr, ubuf);
    return 1;
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
    int pr;

    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len = sizeof(recv_addr);
    memset((void *) &recv_addr, 0, recv_addr_len);

    struct pollfd fds[2] = {
        {
            .fd     = bind_sock,
            .events = POLLIN
        },
        {
            .fd     = conn_sock,
            .events = POLLIN
        }
    };

    unsigned char buf[UM_BUFFER];
    unsigned char outbuf[UM_BUFFER];
    size_t bl;
    size_t obl;

    do {
        fds[0].revents = 0;
        fds[1].revents = 0;

        pr = poll(fds, 2, -1);

        if (pr < 0) {
            perror("poll");
        }
        if (pr <= 0) {
            continue;
        }

        if (fds[0].revents & POLLIN) {
            r = recvfrom(fds[0].fd, (void *) buf, UM_BUFFER, 0,
                         (struct sockaddr *) &recv_addr, &recv_addr_len);

            if (r > 0) {
                bl = (size_t) r;
                transform(mode, buf, bl, outbuf, &obl);

                send(fds[1].fd, (void *) outbuf, obl, 0);
            }
        }

        if (fds[1].revents & POLLIN) {
            r = recv(fds[1].fd, (void *) buf, UM_BUFFER, 0);

            if (r > 0){
                bl = (size_t) r;
                transform(mode, buf, bl, outbuf, &obl);

                sendto(fds[0].fd, (void *) outbuf, obl, 0,
                       (struct sockaddr *) &recv_addr, recv_addr_len);
            }
        }
    } while (1);

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    enum um_mode mode = UM_MODE_NONE;
    struct in_addr addr = {
        .s_addr = INADDR_ANY
    };
    uint16_t port = 0;
    struct in_addr addr_conn = {
        .s_addr = 0
    };
    uint16_t port_conn = 0;

    struct sockaddr_in bind_addr;
    struct sockaddr_in conn_addr;
    memset((void *) &bind_addr, 0, sizeof(bind_addr));
    memset((void *) &conn_addr, 0, sizeof(conn_addr));

    struct hostent * rh;
    int show_usage = 0;
    int c;
    int r;

    while ((c = getopt(argc, argv, "m:p:l:s:c:o:h")) != -1) {
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
            mask = (unsigned char *) malloc(mask_len);
            memcpy((void *) mask, (void *) optarg, mask_len);
            break;

        case 'l':
            r = inet_pton(AF_INET, optarg, (void *) &addr);
            if (r != 1) {
                show_usage = 1;
            }
            if (r < 0) {
                perror("inet_pton");
            }
            break;

        case 'p':
            port = (uint16_t) atoi(optarg);
            break;

        case 'c':
            rh = gethostbyname2(optarg, AF_INET);
            if (!rh) {
                show_usage = 1;
                herror("gethostbyname2");
            }
            memcpy(&addr_conn, rh->h_addr_list[0], rh->h_length);

            break;

        case 'o':
            port_conn = (uint16_t) atoi(optarg);
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

    bind_sock = socket(AF_INET, SOCK_DGRAM, 0);
    conn_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind_sock == -1 || conn_sock == -1) {
        perror("socket");
        ret = 1;
        goto exit;
    }

    switch (mode) {
    case UM_MODE_SERVER:
        if (port == 0) {
            port = UM_SERVER_PORT;
        }
        break;

    case UM_MODE_CLIENT:
        if (port == 0) {
            port = UM_CLIENT_PORT;
        }
        break;

    default:
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

    r = bind(bind_sock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));
    if (r != 0) {
        perror("bind");
        ret = 1;
        goto exit;
    }

    conn_addr.sin_family = AF_INET;
    conn_addr.sin_addr = addr_conn;
    conn_addr.sin_port = htons(port_conn);

    r = connect(conn_sock, (struct sockaddr *) &conn_addr, sizeof(conn_addr));
    if (r != 0) {
        perror("connect");
        ret = 1;
        goto exit;
    }

    ret = start(mode);

exit:
    if (mask) {
        free(mask);
    }
    return ret;
}
