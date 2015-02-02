#ifndef _incl_LOG_H
#define _incl_LOG_H

#include <syslog.h>

extern int use_syslog;

void startlog(const char *ident);
void mylog(int priority, const char *message, ...);
void endlog(void);

#define log_err(msg...)     mylog(LOG_ERR, msg)
#define log_warn(msg...)    mylog(LOG_WARNING, msg)
#define log_info(msg...)    mylog(LOG_INFO, msg)
#define log_debug(msg...)   mylog(LOG_DEBUG, msg)

#endif /* _incl_LOG_H */
