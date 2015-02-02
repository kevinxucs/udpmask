#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "log.h"

const static char *logname;
static int loglevel;

int use_syslog = 0;

void startlog(const char *ident)
{
    loglevel = LOG_INFO;

    if (use_syslog) {
        openlog(ident, LOG_PID, LOG_USER);
        setlogmask(LOG_UPTO(loglevel));
    } else {
        logname = ident;
    }
}

void mylog(int priority, const char *message, ...)
{
    va_list ap;

    if (use_syslog) {
        va_start(ap, message);
        vsyslog(priority, message, ap);
        va_end(ap);
    } else {
        if (priority > loglevel) {
            return;
        }

        time_t t = time(NULL);
        char tmp[256];
        memset((void *) tmp, 0, sizeof(tmp));

        char *loglevel;
        switch (priority) {
        case LOG_ERR:
            loglevel = "ERROR";
            break;

        case LOG_WARNING:
            loglevel = "WARNING";
            break;

        case LOG_INFO:
            loglevel = "INFO";
            break;

        case LOG_DEBUG:
        default:
            loglevel = "DEBUG";
            break;
        }

        sprintf(tmp, "[%lu] %s[%d]: %s: ", t, logname, getpid(), loglevel);

        char out[strlen(tmp) + strlen(message) + 1];
        strcpy(out, tmp);
        strcat(out, message);
        strcat(out, "\n");

        va_start(ap, message);
        vfprintf(stderr, out, ap);
        va_end(ap);
    }
}

void endlog(void)
{
    if (use_syslog) {
        closelog();
    }
}
