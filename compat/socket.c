#include <unistd.h>

#include "socket.h"

#ifdef WIN32
#include <winsock2.h>

void socket_close(SOCKET sock)
{
    closesocket(sock);
}
#else
void socket_close(int sock)
{
    close(sock);
}
#endif
