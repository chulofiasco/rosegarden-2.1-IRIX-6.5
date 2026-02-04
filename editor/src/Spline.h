
/* Musical Notation Editor for X, Chris Cannam 1994      */

/* Header for spline code I'm using for drawing ties &c. */
/* Yes, this is probably overkill; yes, it was entirely  */
/* free overkill because I already had the code and it's */
/* easier to use this than to fight against XDrawArc.    */

#ifndef _MUSIC_SPLINE_
#define _MUSIC_SPLINE_

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

/* args: display, drawable, gc, control points, start, end, number of
   controls, desire fast draft mode option */

extern void DrawSpline(Display *, Drawable, GC,
		       XPoint *, XPoint, XPoint, int, Boolean);


#endif /* _MUSIC_SPLINE_ */


