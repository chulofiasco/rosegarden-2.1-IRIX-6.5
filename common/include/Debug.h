

/* Debugging macros, for optional trace information.    */
/* Should be included *after* any X stuff               */
/*                                                      */
/* Define DEBUG for function entry/exit tracing and     */
/* malloc/free reports.                                 */
/*                                                      */
/* Define REPORT_LINE_AND_FILE as well for more detail. */
/*                                                      */
/* Chris Cannam and Andy Green, 1993-94                 */



#ifndef _DEBUG_HEADER_
#define _DEBUG_HEADER_

#include "SysDeps.h"

#ifdef DEBUG_PLUS_PLUS
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef NULL
#undef NULL
#endif

#define NULL 0			/* one of the Sun headers we have,   */
				/* rather ludicrously, defines this  */
				/* as (char *)0 - which is (a) wrong */
				/* and (b) offensive to C++          */

#include <stdio.h>

/* Ensure that including the X headers after this will cause a  */
/* fatal error (otherwise it may just cause more subtle errors) */

#ifndef X_H
#define XID ERROR!  Debug.h SHOULD BE INCLUDED *AFTER* X INCLUDES
#endif



/* Variable IO type identifiers to help make code more readable. */

#define INPUT
#define OUTPUT
#define IO



#ifdef USE_GARBAGE_COLLECTION
#undef DEBUG
#undef DEBUG_PLUS_PLUS
#define malloc(x)      GC_malloc(x)
#define realloc(x,y)   GC_realloc(x,y)
#define free(x)
#define XtMalloc(x)    GC_malloc(x)
#define XtRealloc(x,y) GC_realloc(x,y)
#define XtFree(x)
#endif


#ifdef DEBUG

#ifdef REPORT_LINE_AND_FILE



#include <unistd.h>		/* for getpid(), below */
#include <sys/types.h>

#define XtMalloc(c) (fprintf(stderr, \
	       " %25s [+] [%6d: %14s %-4d] XtMalloc(%d)\n", \
	       _DebugFn, _DebugPId, __FILE__, __LINE__, (c)), malloc((c)) )

#define XtCalloc(c,e) (fprintf(stderr, \
	       " %25s [+] [%6d: %14s %-4d] XtCalloc(%d)\n", \
	       _DebugFn, _DebugPId, __FILE__, __LINE__, (c), (e)), \
               calloc((c),(e)) )

#define XtFree(c) (fprintf(stderr, \
	       " %25s [-] [%6d: %14s %-4d]   XtFree(%p)\n", \
	       _DebugFn, _DebugPId, __FILE__, __LINE__, (c)), \
               free((char *)(c)) )

#define malloc(c) (fprintf(stderr, \
	       " %25s [+] [%6d: %14s %-4d]   Malloc(%d)\n", \
	       _DebugFn, _DebugPId, __FILE__, __LINE__, (c)), malloc((c)) )

#define calloc(c,e) (fprintf(stderr, \
	       " %25s [+] [%6d: %14s %-4d]   Calloc(%d x %d)\n", \
	       _DebugFn, _DebugPId, __FILE__, __LINE__, (c),(e)), \
               calloc((c),(e)) )

#define free(c) (fprintf(stderr, \
	       " %25s [-] [%6d: %14s %-4d]     Free(%p)\n", \
	       _DebugFn, _DebugPId, __FILE__, __LINE__, (c)), \
               free((char *)(c)) )


#define Begin(x)    char *_DebugFn=(x); \
                    if (_DebugPId==0) _DebugPId=getpid(); \
                    fprintf(stderr," %25s <-= [%6d: %14s %-4d]\n", \
                    _DebugFn, _DebugPId, __FILE__, __LINE__)

#define End         do{fprintf(stderr," %25s =-> [%6d: %14s %-4d]\n", \
		    _DebugFn,_DebugPId, __FILE__, __LINE__);return;}while(0)

#define Return(x)   do{fprintf(stderr," %25s =-> [%6d: %14s %-4d]\n", \
		    _DebugFn,_DebugPId, __FILE__, __LINE__);return(x);}while(0)

#define Assert(x,y) do{fprintf(stderr," %25s [*] [%6d: %14s %-4d] ", \
		    _DebugFn, _DebugPId, __FILE__, __LINE__); fprintf(stderr, \
                    (x), (y));fputc((int)'\n', stderr);}while(0)


static pid_t _DebugPId = 0;



/* Andy's code likes its macros in capitals, and uses a few more: */

#define BEGIN(x)     Begin(x)
#define END          End
#define ASSERT(x,y)  Assert(x,y)

#define RETURN_INT(X) \
  do{fprintf(stderr, " %25s =-> [%6d: %14s %-4d] Int: %d\n", \
	     _DebugFn, _DebugPId, __FILE__, __LINE__, X);return(X);}while(0)

#define RETURN_LONG(X) \
  do{fprintf(stderr," %25s =-> [%6d: %14s %-4d] Long: %ld\n", \
	     _DebugFn, _DebugPId, __FILE__, __LINE__, X);return(X);}while(0)

#define RETURN_CHAR(X) \
  do{fprintf(stderr," %25s =-> [%6d: %14s %-4d] Char: %c\n", \
	     _DebugFn, _DebugPId, __FILE__, __LINE__, X);return(X);}while(0)

#define RETURN_PTR(X) \
  do{fprintf(stderr," %25s =-> [%6d: %14s %-4d] Ptr: %p\n", \
	     _DebugFn, _DebugPId, __FILE__, __LINE__, X);return(X);}while(0)

#define RETURN_BOOL(X) \
  do{fprintf(stderr," %25s =-> [%6d: %14s %-4d] Bool: %c\n", \
	     _DebugFn, _DebugPId, __FILE__, __LINE__, X?'T':'F'); \
  return(X);}while(0)

#define RETURN_WIDGET(X) \
  do{fprintf(stderr," %25s =-> [%6d: %14s %-4d] Widget: %p\n", \
	     _DebugFn, _DebugPId, __FILE__, __LINE__, X);return(X);}while(0)



#else  /* ifndef REPORT_LINE_AND_FILE */



#define XtMalloc(c) (fprintf(stderr, \
	       " [+] XtMalloc (%d) in %s\n", (c), _DebugFn), malloc((c)) )

#define XtCalloc(c,e) (fprintf(stderr, \
	       " [+] XtCalloc (%dx%d) in %s\n", (c), (e), _DebugFn), \
	       calloc((c),(e)) )

#define XtFree(c) (fprintf(stderr, \
               " [-] XtFree (%p) in %s\n", (c), _DebugFn), free((char *)(c)) )

#define malloc(c) (fprintf(stderr, \
	       " [+] Malloc (%d) in %s\n", (c), _DebugFn), malloc((c)) )

#define calloc(c,e) (fprintf(stderr, \
	       " [+] Calloc (%d x %d) in %s\n", (c), (e), _DebugFn), \
	       calloc((c),(e)) )

#define free(c) (fprintf(stderr, \
               " [-] Free (%p) in %s\n", (c), _DebugFn), free((char *)(c)) )


#define Begin(x)    char *_DebugFn=(x);fputs(" =-> ", stderr);\
                    fputs(_DebugFn, stderr);putc('\n',stderr);

#define End         do{fputs(" <-= ", stderr);fputs(_DebugFn,stderr);\
		    putc('\n',stderr);return;}while(0)

#define Return(x)   do{fputs(" <-= ", stderr);fputs(_DebugFn,stderr);\
		    putc('\n',stderr);return(x);}while(0)

#define Assert(x,y) do{fprintf(stderr," [*] %s/%d  ",_DebugFn, __LINE__); \
		    fprintf(stderr,(x),(y));fputc((int)'\n', stderr);}while(0)



/* Andy's code likes its macros in capitals, and uses a few more: */

#define BEGIN(x)     Begin(x)
#define END          End
#define ASSERT(x,y)  Assert(x,y)

#define RETURN_INT(x) \
   do{fprintf(stderr," <-= %s Int: %d\n",_DebugFn, x);return(x);}while(0)

#define RETURN_LONG(x) \
   do{fprintf(stderr," <-= %s Long: %ld\n",_DebugFn, x);return(x);}while(0)

#define RETURN_CHAR(x) \
   do{fprintf(stderr," <-= %s Char: %c\n",_DebugFn, x);return(x);}while(0)

#define RETURN_PTR(x) \
   do{fprintf(stderr," <-= %s Ptr: %p\n",_DebugFn, x);return(x);}while(0)

#define RETURN_BOOL(x) \
   do{fprintf(stderr," <-= %s Bool: %c\n",_DebugFn, x?'T':'F'); \
   return(x);}while(0)

#define RETURN_WIDGET(x) \
   do{fprintf(stderr," <-= %s Widget: %p\n",_DebugFn, x);return(x);}while(0)



#endif /* REPORT_LINE_AND_FILE */

#else  /* ifndef DEBUG */


#ifndef USE_GARBAGE_COLLECTION
#define XtFree(c) XtFree((char *)c)
#define   free(c)   free((char *)c)
#endif


#define Begin(x)
#define End    return
#define Return return
#define Assert(x,y)

#define BEGIN(x)
#define END                     return
#define RETURN_INT(X) 		return X
#define RETURN_LONG(X)		return X
#define RETURN_CHAR(X) 		return X
#define RETURN_PTR(X)  		return X
#define RETURN_BOOL(X) 		return X
#define RETURN_WIDGET(X)	return X
#define ASSERT(X)



#endif /* DEBUG */

#endif /* _DEBUG_HEADER_ */

