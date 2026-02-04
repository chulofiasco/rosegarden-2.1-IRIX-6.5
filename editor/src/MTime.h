
#ifndef MTIME_H
#define MTIME_H

/* MTimes.  The MTime is an opaque time-measurement type.  Its internals */
/* are only referred to in here and MTime.c.  As it happens, it's just a */
/* long; but you don't know that.                                        */

#include "Tags.h"
#include <limits.h>

/* not unsigned long -- FormatBar relies on -ve MTime in some cases */
typedef long MTime;

#define zeroTime        (0L)
#define longestNoteTime (3*(1L<<(LongestNote-1)))

/* Degenerate case for finding shortest of a set of MTimes -- */
/* LONG_MAX should be defined by any ANSI-compliant limits.h  */
#define longestMTime    ((LONG_MAX)-1)


extern MTime *NewMTime      (MTime *, NoteTag, unsigned long);
extern int    CompareMTime  (MTime, MTime);

#define AddMTime(a,b)      ((a)+(b))
#define SubtractMTime(a,b) ((a)-(b))
#define MultiplyMTime(a,b) ((a)*(b))
#define DivideMTime(a,b)   ((a)/(b))
#define MTimeGreater(a,b)  ((a)>(b))
#define MTimeLesser(a,b)   ((a)<(b))
#define MTimeEqual(a,b)    ((a)==(b))

/* Handy conversion functions.  Here `number' is some numeric representation */
/* where all the normal arithmetic and relational operators do exactly what  */
/* you expect in terms of relative note lengths.  It shouldn't be examined,  */
/* because it might change, but it can be manipulated and converted freely   */
/* to and from the MTime notation without loss.  Note that MTimeToTag and    */
/* NumberToTag are potentially lossy operations.                             */

extern MTime         TagToMTime    (NoteTag, Boolean);   /*      tag, dotted */
extern unsigned long TagToNumber   (NoteTag, Boolean);   /*      tag, dotted */
extern NoteTag       MTimeToTag    (MTime, Boolean *);   /* time, dotted-rtn */
extern NoteTag       NumberToTag   (unsigned long, Boolean *);
				                         /*  num, dotted-rtn */

#define MTimeToNumber(a) (a)
#define NumberToMTime(a) (a)

#endif
