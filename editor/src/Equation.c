
#include <stdlib.h>
#include <X11/Xlib.h>		/* just for XPoint */
#include "Equation.h"


void SolveForY(int *yr, float m, int x, int c)
{
  *yr = (int)(m*x) + c;
}

void SolveForM(int y, float *mr, int x, int c)
{
  *mr = (float)(y - c) / (float)x;
}

void SolveForX(int y, float m, int *xr, int c)
{
  *xr = (int)((float)(y - c) / m);
}

void SolveForC(int y, float m, int x, int *cr)
{
  *cr = y - (int)(m*x);
}


void SolveForYByEndPoints(int *yr, XPoint a, XPoint b, int x)
{
  float m;
  int c;

  m = (float)(b.y - a.y) / (float)(b.x - a.x);

  SolveForC(a.y, m, a.x, &c);
  SolveForY(yr, m, x, c);
}


static int pointXComparator(const void *a, const void *b)
{
  XPoint *p1 = (XPoint *)a;
  XPoint *p2 = (XPoint *)b;

  return (p1->x - p2->x);
}


void SolveForYByMinYPoints(XPoint *points, int pcount, int x, int *yr)
{
  int i;
  int raiseBy = 0;
  int y;

  qsort((void *)points, pcount, sizeof(XPoint), pointXComparator);
  
  for (i = 1; i < pcount-1; ++i) {
    SolveForYByEndPoints(&y, points[0], points[pcount-1], points[i].x);

    if (points[i].y > y && (points[i].y - y) > raiseBy)
      raiseBy = points[i].y - y;
  }

  SolveForYByEndPoints(yr, points[0], points[pcount-1], x);
  *yr += raiseBy;
}


void SolveForYByMaxYPoints(XPoint *points, int pcount, int x, int *yr)
{
  int i;
  int lowerBy = 0;
  int y;

  qsort((void *)points, pcount, sizeof(XPoint), pointXComparator);
  
  for (i = 1; i < pcount-1; ++i) {
    SolveForYByEndPoints(&y, points[0], points[pcount-1], points[i].x);

    if (points[i].y < y && (y - points[i].y) > lowerBy)
      lowerBy = y - points[i].y;
  }

  SolveForYByEndPoints(yr, points[0], points[pcount-1], x);
  *yr -= lowerBy;
}

