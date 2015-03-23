#ifndef _incl_INET_PTON_H
#define _incl_INET_PTON_H

#ifdef WIN32

int inet_pton(int af, const char *src, void *dst);

/*
 * Those OSes may also not have AF_INET6, so declare it here if it's not
 * already declared, so that we can pass it to "inet_ntop()" and "inet_pton()".
 */
#ifndef AF_INET6
#define	AF_INET6	127	/* pick a value unlikely to duplicate an existing AF_ value */
#endif

/*
 * And if __P isn't defined, define it here, so we can use it in
 * "inet_ntop.c" and "inet_pton.c" (rather than having to change them
 * not to use it).
 */
#ifndef __P
#define __P(args)	args
#endif

#endif

#endif /* _incl_INET_PTON_H */
