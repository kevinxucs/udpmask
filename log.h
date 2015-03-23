#ifndef _incl_LOG_H
#define _incl_LOG_H

#ifndef WIN32
#include <syslog.h>
#else
#define LOG_EMERG       0
#define LOG_ALERT       1
#define LOG_CRIT        2
#define LOG_ERR         3
#define LOG_WARNING     4
#define LOG_NOTICE      5
#define LOG_INFO        6
#define LOG_DEBUG       7
#endif

extern int use_syslog;

void startlog(const char *ident);
void mylog(int priority, const char *message, ...);
void endlog(void);

#define log_err(msg...)     mylog(LOG_ERR, msg)
#define log_warn(msg...)    mylog(LOG_WARNING, msg)
#define log_info(msg...)    mylog(LOG_INFO, msg)
#define log_debug(msg...)   mylog(LOG_DEBUG, msg)

#endif /* _incl_LOG_H */
