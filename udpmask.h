#include <netinet/in.h>

#define UM_SERVER_PORT  51194
#define UM_CLIENT_PORT  61194
#define UM_MAX_CLIENT   32
#define UM_BUFFER       65536

enum um_mode {
    UM_MODE_NONE = -1,
    UM_MODE_SERVER,  
    UM_MODE_CLIENT 
};
