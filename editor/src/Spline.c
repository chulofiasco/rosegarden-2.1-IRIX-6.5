
/* Spline.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Vaguely portable; requires a GC called clearGC   */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>	/* purely for Boolean -- what a waste */
#include <stdio.h>

#include "GC.h"

#define SCALEF (2L<<17)		/* for integer spline code       */

/*
  Today I'm the scruffy get with the spots, greasy hair and food-
  stained T-shirt who couldn't be bothered to have a bath this morning
  and is just hoping his deodorant's prominent enough.  I quite like the
  feeling, I've got an all-grey hangover, Idaho on the headphones
  (mediocre but excessively glum -- ideal) and a picture of my own good
  self in the background (reclining in the Sun-3 lab in Bath).
*/


/* Draw a single-control-point Bezier spline, given the number   */
/* of plot points to include in the spline.  Arguments are no.   */
/* of points, start, finish, control point, pixmap and GC.       */
/* Floating-point version, slower but more accurate than int-    */
/* only one.                                                     */

static void DrawSplineNSub(Display *display, int n, XPoint s,
			   XPoint f, XPoint c, Drawable map, GC gc)
{
  int            m;
  double         ax, ay;
  double         bx, by;
  XPoint         p;
  static int     np = -1;
  static XPoint *pt;

  ax = (double)(f.x + s.x - 2 * c.x) / (double)n;
  ay = (double)(f.y + s.y - 2 * c.y) / (double)n;
  bx = 2.0 * (double)(c.x - s.x);
  by = 2.0 * (double)(c.y - s.y);

  if (n > np) {
    if (np >= 0) XtFree((char *)pt);
    pt = (XPoint *)XtMalloc((n+1) * sizeof(XPoint));
    np = n;
  }

  for (m = 0; m <= n; ++m) {

    p.x = (short)((m * ((double)m * ax + bx)) / n);
    p.y = (short)((m * ((double)m * ay + by)) / n);

    pt[m].x = (short)(s.x + p.x);
    pt[m].y = (short)(s.y + p.y);
  }

  /*  XDrawLines(display, map, clearGC, pt, n + 1, CoordModeOrigin);*/
  XDrawLines(display, map, gc,      pt, n + 1, CoordModeOrigin);
} 




/* Integer-only version, faster but less accurate.  Use for rubber-banding. */
/* Here if the start point is negative in either x or y, the line will be   */
/* drawn starting at the point at which the previous line ended             */

static void DraftDrawSplineNSub(Display *display, int n, XPoint s, XPoint f,
				XPoint c, Drawable map, GC gc)
{
  int              m;
  long             ax;
  long             ay;
  XPoint           b;
  XPoint           p;
  static int       np = -1;
  static XPoint   *pt;
  static XPoint    l = { -1, -1 };

  if   (s.x < 0 || s.y < 0)
    if (l.x < 0 || l.y < 0) { s.x = 0;   s.y = 0;   }
    else                    { s.x = l.x; s.y = l.y; }

   ax = (f.x + s.x - 2 * c.x) * SCALEF / n;
   ay = (f.y + s.y - 2 * c.y) * SCALEF / n;
  b.x = 2 * (c.x - s.x);
  b.y = 2 * (c.y - s.y);

  if (n > np) {
    if (np >= 0) XtFree((char *)pt);
    pt = (XPoint *)XtMalloc((n+1) * sizeof(XPoint));
    np = n;
  }

  for (m = 0; m <= n; ++m) {

    p.x = (short)((m * (((long)m * ax)/SCALEF + (long)b.x)) / n);
    p.y = (short)((m * (((long)m * ay)/SCALEF + (long)b.y)) / n);

    pt[m].x = (short)(s.x + p.x);
    pt[m].y = (short)(s.y + p.y);
  }

  l.x = s.x + p.x;
  l.y = s.y + p.y;

  XDrawLines(display, map, gc, pt, n+1, CoordModeOrigin);
} 




/* DrawSplineSub:                                         */
/*                                                        */
/* Draw a single-control-point Bezier spline.  Arguments: */
/*                                                        */
/*         s -  start point                               */
/*         f -  end point                                 */
/*         c -  control point                             */
/*   connect -  should the start of this spline be        */
/*              connected to the end of the last one?     */
/*              this is useful in draft mode, when the    */
/*              code isn't necessarily accurate enough    */
/*              to make the points match up otherwise;    */
/*              if not in draft mode, it's ignored        */
/*       map -  Drawable on which to draw the spline      */
/*        gc -  GC to use for drawing                     */
/*     draft -  should we use the integer-only faster     */
/*              algorithm, for less accurate results?     */

void DrawSplineOneControl(Display *display, Drawable map, GC gc, XPoint s,
			  XPoint f, XPoint c, Boolean connect, Boolean draft)
{
  static XPoint cp = { -1, -1 };
  XPoint        p;
  int              n;

  p.x = c.x - s.x;
  p.y = c.y - s.y;

  if (p.x <   0) p.x = -p.x;
  if (p.y <   0) p.y = -p.y;
  if (p.x > p.y) n   =  p.x;
  else           n   =  p.y;

  p.x = f.x - c.x;
  p.y = f.y - c.y;

  if (p.x <   0) p.x = -p.x;
  if (p.y <   0) p.y = -p.y;
  if (p.x > p.y) n  +=  p.x;
  else           n  +=  p.y;

  /* debug -- plot X over each control point
  XDrawLine(display, map, gc, c.x-4, c.y-4, c.x+4, c.y+4);
  XDrawLine(display, map, gc, c.x-4, c.y+4, c.x+4, c.y-4);
  */

  if (draft)
    if (connect) DraftDrawSplineNSub(display, n / 2, cp, f, c, map, gc);
    else         DraftDrawSplineNSub(display, n / 2, s,  f, c, map, gc);
  else                DrawSplineNSub(display, n,     s,  f, c, map, gc);
}




/* Draw a multi-control-point Bezier spline.  Control points are */
/* held in `controls'; the other arguments are start and finish  */
/* points and number of control points.  Also refreshes pixmap.  */

void DrawSpline(Display *display, Drawable map, GC gc, XPoint *controls,
		XPoint s, XPoint f, int np, Boolean draft)
{
  int       i;
  XPoint    c;
  XPoint    p;
  XPoint    n;
  Boolean   connect = False;

  p.x = s.x;
  p.y = s.y;

  for (i = 1; i < np; ++i) {

    c.x = controls[i-1].x;
    c.y = controls[i-1].y;

    n.x = (c.x + controls[i].x)/2;
    n.y = (c.y + controls[i].y)/2;

    DrawSplineOneControl(display, map, gc, p, n, c, connect, draft);
    connect = True;

    p.x = n.x;
    p.y = n.y;
  }

  DrawSplineOneControl(display, map, gc, p, f, controls[i-1], connect, draft);
}

