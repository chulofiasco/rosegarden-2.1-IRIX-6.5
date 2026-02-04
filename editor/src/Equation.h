
#ifndef _EQUATION_H_
#define _EQUATION_H_

extern void SolveForY(int *, float, int, int); /* y-rtn, m, x, c */
extern void SolveForM(int, float *, int, int); /* y, m-rtn, x, c */
extern void SolveForX(int, float, int *, int); /* y, m, x-rtn, c */
extern void SolveForC(int, float, int, int *); /* y, m, x, c-rtn */

extern void SolveForYByEndPoints(int *, XPoint, XPoint, int);
				/* y-rtn, start, end, x to solve at */


/* These functions reorder the points array in-place, so beware */

/* calculates minimum y at x such that the line passes through or
   above (ie. with larger y than at equal x) all points in list;
   args: point list, point count, x, y-rtn */
extern void SolveForYByMinYPoints(XPoint *, int, int, int *);

/* calculates maximum y at x such that the line passes through or
   below (ie. with smaller y than at equal x) all points in list;
   args: point list, point count, x, y-rtn */
extern void SolveForYByMaxYPoints(XPoint *, int, int, int *);

#endif

