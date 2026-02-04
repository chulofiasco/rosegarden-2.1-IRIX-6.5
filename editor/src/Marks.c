
/* {{{ Includes */

#include "Notes.h"
#include "Marks.h"
#include "ItemList.h"
#include "StavePrivate.h"
#include "Draw.h"
#include "Equation.h"
#include "Spline.h"

/* }}} */

/* {{{ Constructors, destructors */

MarkList NewMarkList(Mark *m)
{
  MarkList list;
  Begin("NewMarkList");

  list = (MarkList)NewList(sizeof(MarkListElement));
  list->mark = m;

  Return(list);
}


void DestroyMarkList(MarkList list)
{
  MarkList first = (MarkList)First(list);
  Begin("DestroyMarkList");

  list = first;
  while (list) {

    XtFree((void *)list->mark);
    list = (MarkList)Next(list);
  }

  DestroyList(first);
  End;
}


Mark *NewMark(Mark *space, MarkTag tag, Boolean start, Mark *otherEnd)
{
  Begin("NewMark");
  if (!space) space = (Mark *)XtMalloc(sizeof(Mark));

  space->type = tag;
  space->start = start;
  space->other_end = otherEnd;

  Return(space);
}

/* }}} */
/* {{{ Collation (ie. making a list of aliased pointers to marks in an area) */

static void CollateMarksForItem(ItemList ilist, MarkList *mlist)
{
  MarkList msublist;
  
  Begin("CollateMarksForItem");

  for (msublist = (MarkList)First(ilist->item->item.marks); msublist;
       msublist = (MarkList)Next(msublist)) {

    if (!msublist->mark->other_end) {
      fprintf(stderr, "Warning: Found mark (type %d) with invalid other-end "
	      "field; ignoring\n", msublist->mark->type);
    } else {
      msublist->mark->ilist = ilist;
      *mlist = (MarkList)Nconc(*mlist, NewMarkList(msublist->mark));
    }
  }

  End;
}


void CollateMarksInBar(Bar *bar, MarkList *mlist)
{
  ItemList ilist;
  Begin("CollateMarksInBar");

  for (ilist = bar->group.start; ilist;
       ilist = (ilist == bar->group.end ? NULL : iNext(ilist))) {

    if (ilist->item->item.marks) {
      CollateMarksForItem(ilist, mlist);
    }
  }

  End;
}

/* }}} */

/* {{{ Drawing Slurs */

static void GetSlurCoord(ItemList ilist, Position y, XPoint *pr,
			 Boolean above, Boolean force)
{
  Item *i;
  Pitch pitch;
  Begin("GetSlurCoords");

  pr->x = ilist->item->item.x + NoteWidth/3;

  while (iNext(ilist) && ilist->item->object_class != ChordClass)
    ilist = iNext(ilist);

  i = ilist->item;

  if (above) pitch = i->methods->get_highest(i)->pitch;
  else       pitch = i->methods->get_lowest(i)->pitch;

  if (force) {			/* make sure it's off the staff */

    if (above) pr->y = y + STAVE_Y_COORD(pitch > 8 ? pitch+3 : 10);
    else       pr->y = y + STAVE_Y_COORD(pitch < 0 ? pitch-3 : -3);

  } else {

    if (above) pr->y = y + STAVE_Y_COORD(pitch+3);
    else       pr->y = y + STAVE_Y_COORD(pitch-3);
  }

  End;
}


static void DrawHalfSlur(Drawable drawable, Position y, Dimension wd,
			 Mark *m, Boolean left)
{
  XPoint points[3];
  Begin("DrawHalfSlur");

  if (left) {
    GetSlurCoord((ItemList)m->ilist, y, &points[0], True, True);
    points[1].x = wd + 3;
    points[1].y = points[0].y - 15;
  } else {
    GetSlurCoord((ItemList)m->ilist, y, &points[1], True, True);
    points[0].x = 0;
    points[0].y = points[1].y - 15;
  }

  if (left) {
    points[2].x = points[0].x + 20; points[2].y = points[1].y;
  } else {
    points[2].x = points[1].x - 20; points[2].y = points[0].y;
  }

  DrawSpline(display, drawable, drawingGC, &points[2],
	     points[0], points[1], 1, False);

  points[2].y--;
  if (left) points[1].y--; else points[0].y--;

  DrawSpline(display, drawable, drawingGC, &points[2],
	     points[0], points[1], 1, False);

  End;
}


  /* assume haveOtherEnd true for the moment, and we'll deal with the
     other case separately */

static void DrawSlur(Drawable drawable, Position y, Dimension wd, Mark *m,
		     Boolean left, Boolean haveOtherEnd)
{
#define SLUR_POINT_MAX 30

  static XPoint points[SLUR_POINT_MAX];
  int mx1, my1, mx2, my2;
  int x1, y1, x2, y2;
  ItemList ii;
  int pcount;
  int high = 0, low = 0;
  Boolean above;
  
  Begin("DrawSlur");

  if (!(left && haveOtherEnd)) {
    DrawHalfSlur(drawable, y, wd, m, left);
    End;
  }

  for (ii = (ItemList)m->ilist; ii && ii != m->other_end->ilist;
       ii = iNext(ii)) {
    if (ii->item->object_class == ChordClass) {
      Chord *c = (Chord *)ii->item;
      if (c->methods->get_highest(c)->pitch - 4 <
	  4 - c->methods->get_lowest(c)->pitch) ++low;
      else ++high;
    }
  }

  above = !(low > high);

  GetSlurCoord((ItemList)m->ilist, y, &points[0], above, False);
  GetSlurCoord((ItemList)m->other_end->ilist, y, &points[1], above, False);
  
  if      (points[1].x - points[0].x > 150) points[0].x += 3;
  else if (points[1].x - points[0].x >  75) points[0].x += 1;
  
  for (ii = iNext(m->ilist), pcount = 2;
       ii && ii != m->other_end->ilist && pcount < SLUR_POINT_MAX-1;
       ii = iNext(ii)) {

    GetSlurCoord(ii, y, &points[pcount], above, False);
    ++pcount;
  }

  x1 = points[0].x; y1 = points[0].y; x2 = points[1].x; y2 = points[1].y;
    
  mx1 = x1 + (x2 - x1)/5;
  mx2 = x2 - (x2 - x1)/5;

  if (above) {			/* cut 'n paste coding! */

    /* this reorders the list in-place */
    SolveForYByMaxYPoints(points, pcount, mx1, &my1);
    SolveForYByMaxYPoints(points, pcount, mx2, &my2);

    if (x2 - x1 < 100) {
      my1 -= (float)(NoteHeight * (x2 - x1)) / 50.0;
      my2 -= (float)(NoteHeight * (x2 - x1)) / 50.0;
    } else {
      my1 -= 10; my2 -= 10;
    }

    if (y2 - y1 > 40) my2 -= NoteHeight;
    if (y1 - y2 > 40) my1 -= NoteHeight;

    if      (my1 > my2 + 10) my1 = my2 + 10;
    else if (my2 > my1 + 10) my2 = my1 + 10;

    if (my1 < y - StaveUpperGap) my1 = y - StaveUpperGap + 1;
    if (my2 < y - StaveUpperGap) my2 = y - StaveUpperGap + 1;

    if      (y1  > my1 + 15) y1 = my1 + 15;
    else if (y1  < my1 -  5) my1 = y1 + 5;

    if      (y2  > my2 + 15) y2 = my2 + 15;
    else if (y2  < my2 -  5) my2 = y2 + 5;

  } else {

    /* this reorders the list in-place */
    SolveForYByMinYPoints(points, pcount, mx1, &my1);
    SolveForYByMinYPoints(points, pcount, mx2, &my2);

    if (x2 - x1 < 100) {
      my1 += (float)(NoteHeight * (x2 - x1)) / 50.0;
      my2 += (float)(NoteHeight * (x2 - x1)) / 50.0;
    } else {
      my1 += 10; my2 += 10;
    }

    if (y1 - y2 > 40) my2 += NoteHeight;
    if (y2 - y1 > 40) my1 += NoteHeight;

    if      (my1 < my2 - 10) my1 = my2 - 10;
    else if (my2 < my1 - 10) my2 = my1 - 10;

    if (my1 > y + StaveHeight + StaveLowerGap + 10)
      my1 = y + StaveHeight + StaveLowerGap + 10 - 1;
    if (my2 > y + StaveHeight + StaveLowerGap + 10)
      my2 = y + StaveHeight + StaveLowerGap + 10 - 1;

    if      (y1  < my1 - 15) y1 = my1 - 15;
    else if (y1  > my1 +  5) my1 = y1 - 5;

    if      (y2  < my2 - 15) y2 = my2 - 15;
    else if (y2  > my2 +  5) my2 = y2 - 5;
  }

  points[0].x = x1;  points[0].y = y1;  points[1].x = x2;  points[1].y = y2;
  points[2].x = mx1; points[2].y = my1; points[3].x = mx2; points[3].y = my2;

  DrawSpline(display, drawable, drawingGC, &points[2],
	     points[0], points[1], 2, False);

  points[2].y--; points[3].y--;

  DrawSpline(display, drawable, drawingGC, &points[2],
	     points[0], points[1], 2, False);

  End;
}

/* }}} */
/* {{{ Drawing Ties */

void DrawHalfTie(Drawable drawable, Position sx, Position sy,
		 Position fx, Boolean left, Boolean above)
{
  XPoint control;
  XPoint start, end;

  Begin("DrawHalfTie");

  if (left) sx += 3;

  start.x = sx;
  start.y = sy;
  
  control.x = sx;
  control.y = sy + (above ? -4 : 4);

  end.x = sx + (left ? 10 : -10);
  end.y = control.y;

  if (left) { if (end.x > fx) end.x = fx-1; }
  else      { if (end.x < fx) end.x = fx+1; }

  DrawSpline(display, drawable, drawingGC, &control, end, start, 1, True);
  XDrawLine(display, drawable, drawingGC, end.x, end.y, fx, end.y);

  --end.y; --control.y;
  
  DrawSpline(display, drawable, drawingGC, &control, end, start, 1, True);
  XDrawLine(display, drawable, drawingGC, end.x, end.y, fx, end.y);

  End;
}


static void DrawTie(Drawable drawable, Position y, Dimension wd, Mark *m,
		    Boolean left, Boolean haveOtherEnd)
{
  XPoint p1, p2;
  Boolean above = False;
  Item *i1 = ((ItemList)m->ilist)->item, *i2;
  Begin("DrawTie");

  if (haveOtherEnd) {
    i2 = ((ItemList)m->other_end->ilist)->item;
    
    if (i1->methods->get_highest(i1)->pitch >= 4 ||
	i2->methods->get_highest(i2)->pitch >= 4) above = True;

  } else {
    if (i1->methods->get_highest(i1)->pitch >= 4) above = True;
  }

  GetSlurCoord((ItemList)m->ilist, y, &p1, above, True);

  if (left) {
    if (haveOtherEnd) {
      GetSlurCoord((ItemList)m->other_end->ilist, y, &p2, above, True);
      DrawHalfTie(drawable, p1.x, p1.y, (p2.x - p1.x) /2 + p1.x, True, above);
      DrawHalfTie(drawable, p2.x, p1.y, (p2.x - p1.x) /2 + p1.x, False, above);
    } else {
      DrawHalfTie(drawable, p1.x, p1.y, wd, True, above);
    }
  } else {
    DrawHalfTie(drawable, p1.x, p1.y, 0, False, above);
  }

  End;
}

/* }}} */
/* {{{ Drawing Hairpins */

static void GetDynamicCoords(Mark *m, Position y,
			     int *xr, int *yr, Boolean left)
{
  Pitch pitch;
  Item *item = ((ItemList)m->ilist)->item;
  int x;
  Begin("GetDynamicCoords");

  *xr = x = item->item.x + NoteWidth/3;

  if (!left) {

    ItemList next = iNext((ItemList)m->ilist);

    if (next && (next->item->item.x > x)) {

      x = next->item->item.x - 15;
      if (x > *xr + 200) x = *xr + 200;

      if (x > *xr) *xr = x;
      else *xr += item->methods->get_min_width(item);

    } else{
      *xr += item->methods->get_min_width(item);
    }
  }

  pitch = item->methods->get_lowest(item)->pitch;
  *yr = y + STAVE_Y_COORD(pitch < 0 ? pitch-5 : -7);

  End;
}


static void DrawDynamic(Drawable drawable, Position y, Dimension wd, Mark *m,
			Boolean left, Boolean haveOtherEnd)
{
  int ax, ay, bx, by;
  Begin("DrawDynamic");

  if (left) {
    GetDynamicCoords(m, y, &ax, &ay, True);

    if (haveOtherEnd) {
      GetDynamicCoords(m->other_end, y, &bx, &by, False);
    } else {
      bx = wd;
      by = y + STAVE_Y_COORD(-7);
    }
  } else {
    ax = 0;
    ay = y + STAVE_Y_COORD(-7);
    GetDynamicCoords(m, y, &bx, &by, False);
  }

  ay = (ay > by ? ay : by) + 6;	/* top y-coord */
  by = ay + (bx-ax)/50 + 6;	/* bottom y-coord */

  if (m->type == Crescendo) {

    XDrawLine(display, drawable, drawingGC, ax, (ay+by)/2, bx, ay);
    XDrawLine(display, drawable, drawingGC, ax, (ay+by)/2, bx, by);

  } else {

    XDrawLine(display, drawable, drawingGC, ax, ay, bx, (ay+by)/2);
    XDrawLine(display, drawable, drawingGC, ax, by, bx, (ay+by)/2);
  }

  End;
}

/* }}} */
/* {{{ Generic Draw function */

void DrawMarks(Drawable drawable, Position y, Dimension wd,
	       int staveNo, MarkList *mlist) /* why "staveNo"? it's not used */
{
  Begin("DrawMarks");

  if (!*mlist) End;

  for (*mlist = (MarkList)First(*mlist); *mlist;
       *mlist = (MarkList)Next(*mlist)) {

    Mark *m = (*mlist)->mark;

    if (m->start) {		/* search for end */

      MarkList tempList = *mlist;
      for (tempList = (MarkList)Next(tempList); tempList;
	   tempList = (MarkList)Next(tempList)) {

	if (tempList->mark == m->other_end) {

	  switch (m->type) {

	  case Tie:
	    DrawTie(drawable, y, wd, m, True, True); break;

	  case Slur:
	    DrawSlur(drawable, y, wd, m, True, True); break;

	  case Crescendo: case Decrescendo:
	    DrawDynamic(drawable, y, wd, m, True, True); break;
	  }

	  (void)Remove(tempList);
	  break;
	}
      }

      if (!tempList) {		/* didn't find right-hand end */

	switch (m->type) {

	case Tie: case Slur:
	  DrawSlur(drawable, y, wd, m, True, False); break;

	case Crescendo: case Decrescendo:
	  DrawDynamic(drawable, y, wd, m, True, False); break;
	}
      }
    } else {			/* right-hand end, so we haven't seen start */

      switch (m->type) {

      case Tie: case Slur:
	DrawSlur(drawable, y, wd, m, False, False); break;

      case Crescendo: case Decrescendo:
	DrawDynamic(drawable, y, wd, m, False, False); break;
      }
    }
  }

  End;
}

/* }}} */

/* {{{ Utility functions */

void MarkItems(MarkTag tag, ItemList left, ItemList right)
{
  MarkList mlist;
  Mark *leftMark, *rightMark;
  Begin("MarkItems");

  /* first check they aren't already marked this way */
  if ((mlist = FindMarkType(tag, left->item->item.marks))
      && mlist->mark->start) {
    if (FindPairMark(mlist->mark, right->item->item.marks)) End;
  }

  leftMark = NewMark(NULL, tag, True, NULL);
  rightMark = NewMark(NULL, tag, False, leftMark);
  leftMark->other_end = rightMark;

  left->item->item.marks =
    (MarkList)First(Nconc(left->item->item.marks, NewMarkList(leftMark)));
  right->item->item.marks =
    (MarkList)First(Nconc(right->item->item.marks, NewMarkList(rightMark)));

  End;
}


MarkList FindMarkType(MarkTag tag, MarkList list)
{
  Begin("FindMarkType");

  while (list) {
    if (list->mark->type == tag) break;
    list = (MarkList)Next(list);
  }

  Return(list);
}


MarkList FindPairMark(Mark *mark, MarkList list)
{
  Begin("FindPairMark");

  for (list = (MarkList)First(list); list; list = (MarkList)Next(list)) {
    if (list->mark->other_end == mark) break;
  }

  Return(list);
}


/* handle with care, the ilist is only a mutable cached value */

MarkList FindOtherMarkByIList(Mark *mark, MarkList list)
{
  Begin("FindPairMark");

  for (list = (MarkList)First(list); list; list = (MarkList)Next(list)) {
    if (list->mark->ilist == mark->ilist && list->mark != mark) break;
  }

  Return(list);
}


/* removes all marks which start *or* end in this area; once again,
   start is left item */

void ClearMarkType(ItemList music, ItemList start, ItemList end, MarkTag tag)
{
  ItemList list, sub;
  MarkList marklist, tempmarklist;
  Begin("ClearMarkType");

  for (list = start ? iNext(start) : music; list;
       list = (list == end ? NULL : iNext(list))) {

    if (list->item->item.marks) {

      for (marklist = (MarkList)First(list->item->item.marks);
	   marklist; ) {

	if (marklist->mark->type != tag) {
	  marklist = (MarkList)Next(marklist);
	  continue;
	}

	for (sub = (marklist->mark->start ? iNext(list) : iPrev(list)); sub;
	     sub = (marklist->mark->start ? iNext(sub)  : iPrev(sub))) {
	  
	  if ((tempmarklist = FindPairMark
	       (marklist->mark, sub->item->item.marks))) {
	    
	    sub->item->item.marks = (MarkList)Remove(tempmarklist);
	    break;
	  }
	}

	marklist = (MarkList)Remove(marklist);
	list->item->item.marks = marklist;
      }
    }
  }

  End;
}


void ClearMarks(ItemList music, ItemList start, ItemList end)
{
  Begin("ClearMarks");

  ClearMarkType(music, start, end, Tie);
  ClearMarkType(music, start, end, Slur);
  ClearMarkType(music, start, end, Crescendo);
  ClearMarkType(music, start, end, Decrescendo);

  End;
}


/* takes an itemlist as read from file, containing marks that haven't
   got correct other_end tags yet but have index numbers in the ilist
   fields, and matches up the indexes to get the other_end fields */
	
void EnsureMarkIntegrity(ItemList i)
{
  ItemList ii;
  MarkList mlist, m2;
  Begin("EnsureMarkIntegrity");

  for (i = (ItemList)Last(i); i; i = iPrev(i)) {

    for (mlist = (MarkList)Last(i->item->item.marks); mlist;
	 mlist = (MarkList)Prev(mlist)) {

      if (mlist->mark->start) {
	continue;
      }

      for (ii = i; ii; ii = iPrev(ii)) {
	if ((m2 = FindOtherMarkByIList(mlist->mark, ii->item->item.marks))) {

	  m2->mark->other_end = mlist->mark;
	  mlist->mark->other_end = m2->mark;
	  mlist->mark->type = m2->mark->type;

	  break;
	}
      }
    }
  }

  End;
}

/* }}} */

