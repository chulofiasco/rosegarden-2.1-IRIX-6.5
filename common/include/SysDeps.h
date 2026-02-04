
/* System dependencies    */
/* Chris Cannam, Feb 1995 */

#ifndef _SYS_DEPS_H_
#define _SYS_DEPS_H_


#include "../../config.h"


#ifdef POSIX_PLEASE
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE	      /* Undef this in any files that aren't... */
#endif
#endif


#ifdef USE_GARBAGE_COLLECTION
#include <gc.h>
#endif


#include <signal.h>


/* handle varying signal() types */

#ifndef SIGNAL_CALLBACK_TYPE
#define SIGNAL_CALLBACK_TYPE (void (*)())    /* This is POSIX */
#endif

#ifndef SIGCHLD
#define SIGCHLD SIGCLD
#endif

#ifdef LINUX
#ifndef SIGBUS
#define SIGBUS SIGSEGV
#endif
#endif


/* non-ansi or non-posix things */

#ifdef POSIX_PLEASE

#define NO_SYS_ERRLIST
#define NO_STRCASECMP
#define NO_GETHOSTNAME

#ifndef SIGBUS
#define SIGBUS SIGSEGV        /* POSIX has no bus error signal. In theory. */
#endif

/* This is rather an exciting way to implement signal() ... */

#define signal(x,y)     \
  do { \
    struct sigaction sAct; \
    (void)sigemptyset(&sAct.sa_mask); \
    sAct.sa_flags = 0; \
    sAct.sa_handler = (SIGNAL_CALLBACK_TYPE(y)); \
    (void)sigaction((x), &sAct, NULL); \
  } while (0)

#else

#define signal(x,y) signal(x, SIGNAL_CALLBACK_TYPE(y))

#endif /* ! POSIX_PLEASE */


#ifdef NO_STRCASECMP

#include <ctype.h>
#include <string.h>

#define strcasecmp(a,b)    StrCaseCmp(a,b)
#define strncasecmp(a,b,n) StrNCaseCmp(a,b,n)

static int StrNCaseCmp(const char *, const char *, size_t);


/* No str[n]casecmp?  Let's write our own, cos we're masochistic that way */

static int StrCaseCmp(const char *a, const char *b)
{
    int i, aa, bb;

    for (i = 0; a[i] || b[i]; ++i) {
	
	aa = a[i];
	bb = b[i];

	if (!aa) return -1;
	if (!bb) return  1;

	if (islower(aa)) aa = toupper(aa);
	if (islower(bb)) bb = toupper(bb);

	if (aa != bb) return aa-bb;
    }

    /* this case can never be reached, and is here only to
       prevent warnings about StrNCaseCmp being unused... */

    if (a[i]) return StrNCaseCmp(a, b, 0);

    return 0;
}

static int StrNCaseCmp(const char *a, const char *b, size_t n)
{
    unsigned int i;
    int aa, bb;

    for (i = 0; i < n && (a[i] || b[i]); ++i) {

	aa = a[i];
	bb = b[i];
	
	if (!aa) return -1;
	if (!bb) return  1;

	if (islower(aa)) aa = toupper(aa);
	if (islower(bb)) bb = toupper(bb);

	if (aa != bb) return aa-bb;
    }

    /* this case can never be reached, and is here only to
       prevent warnings about StrCaseCmp being unused... */

    if (i > n) return StrCaseCmp(a, b);

    return 0;
}


#endif /* NO_STRCASECMP */



#endif /* _SYS_DEPS_H_ */


