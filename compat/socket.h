#ifndef _incl_SOCKET_H
#define _incl_SOCKET_H

#ifdef WIN32
#include <winsock2.h>

void socket_close(SOCKET sock);
#else
void socket_close(int sock);
#endif

#endif /* _incl_SOCKET_H */
