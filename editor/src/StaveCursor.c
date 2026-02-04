
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Stave cursor handling code */

/* {{{ Includes */

#include "General.h"
#include "Classes.h"
#include "GC.h"
#include "Stave.h"
#include "StavePrivate.h"
#include "StaveCursor.h"
#include "StaveEdit.h"
#include "ItemList.h"
#include "Menu.h"

#include <X11/StringDefs.h>
#include <Yawn.h>

/* }}} */
/* {{{ Useful macros */

#define LeftOfItem(i)  ((i)->item.x - 2)
#define RightOfItem(i) ((i)->item.x + (i)->methods->get_min_width(i) + 3)

#define FirstVisibleBar(s) ((s)->display_start + ((s)->display_start == -1))
#define LastVisibleBar(s)  (FirstVisibleBar(s) +  (s)->display_number - 1)

/* }}} */

/* {{{ Getting the pointed item, bar, stave and pitch */

static Position PointerRecToXBound(PointerRec *r)
{
  Position b1, b2;
  Begin("PointerRecToXBound");

  if (r->left) {
    if (iNext(r->left)) {

      b1 = LeftOfItem(iNext(r->left)->item) - 2;
      b2 = RightOfItem(r->left->item);

      if (b2 > b1) Return(b2 - 1);
      else Return(b1);

    } else {

      b1 = RightOfItem(r->left->item);
      b2 = r->bar->item.x + r->bar->bar.width;

      if (b2 - 4 > b1) Return(b2 - 6);
      else Return(b1);
    }
  } else {
    Return(LeftOfItem(((ItemList)First(r->bar->group.start))->item));
  }
}


/* As a matter of convention, we will agree that a pointer-rec will      */
/* contain two 0 fields iff the pointer-location code couldn't find any  */
/* candidate items; if the pointer is off the left end of the stave, and */
/* hence has no `left' item, the `left' field will be NULL (a great loss */
/* to the world's artistic subculture) and the `bar' will be non-NULL.   */


/* To find the itemlist to the left of the cursor, given the bar in the  */
/* cursor:  scan from the right-hand end of the bar, taking advantage of */
/* the fact that list back-pointers ignore groups.  Stop and return the  */
/* first item whose (x + width) is less than `off'.  If there is no such */
/* item, return the item before the bar item but set `bound' to the left */
/* hand edge of the leftmost (failed) item.                              */


static PointerRec GetBarPointed(StaveEltList barList,
				int staveNo, Position off)
{
  int        x;
  PointerRec rtn;
  ItemList   list;
  Dimension  wd;

  Begin("GetBarPointed");

  MakeNullRec(rtn);

  list = barList->bars[staveNo]->group.end;
  if (!list) Return(rtn);

  while (list && list != iPrev(barList->bars[staveNo]->group.start)) {

    wd = list->item->methods->get_min_width(list->item);

    if (wd == 0) {
      list = iPrev(list);
      continue;
    }

    x = list->item->item.x + wd/2 - 2;

    if (x < off) {

      rtn.left = list;
      rtn.bar  = barList->bars[staveNo];
      rtn.time =
	AddMTime(StaveItemToTime(NULL, staveNo, rtn.bar, list),
		 list ? list->item->methods->get_length(list->item) : zeroTime);

      /* never allow the pointer at the end of a bar unless we have to
         -- we'd rather be at the start of the next bar */

      if (rtn.left == barList->bars[staveNo]->group.end && iNext(rtn.left)) {
	MakeNullRec(rtn);
      }

      Return(rtn);
    }

    list = iPrev(list);
  }

  rtn.left = iPrev(barList->bars[staveNo]->group.start);
  rtn.time = barList->bars[staveNo]->bar.start_time;
  rtn.bar  = Prev(barList) ? ((StaveEltList)Prev(barList))->bars[staveNo] :
    barList->bars[staveNo];

  Return(rtn);
}



PointerRec StaveGetPointedItem(MajorStave sp, XPoint p, int staveNo, List elist)
{
  PointerRec     rtn;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;

  Begin("StaveGetPointedItem");

  MakeNullRec(rtn);

  if (!mstave || !staveLabel || staveNo < 0 ||
      !elist || !BarValid(((StaveEltList)elist)->bars[staveNo])) Return(rtn);

  while (TestNullRec(rtn) && elist &&
	 BarValid(((StaveEltList)elist)->bars[staveNo])) {

    rtn = GetBarPointed((StaveEltList)elist, staveNo, p.x);
    elist = Next(elist);
  }

  Return(rtn);
}



List StaveGetPointedBar(MajorStave sp, XPoint p, int staveNo)
{
  int            i;
  int            accWidth;
  Dimension      wWidth;
  StaveEltList   elist;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;

  Begin("StaveGetPointedBar");

  if (!mstave || !staveLabel || staveNo < 0) Return(NULL);

  YGetValue(staveLabel, XtNwidth, &wWidth);
  if (p.x >= wWidth) p.x = wWidth - 1;

  for (elist = mstave->bar_list, i = mstave->display_start;
       elist && Next(elist) && i > 0; elist = (StaveEltList)Next(elist), --i);

  accWidth = 0;
  if (mstave->display_start == -1) {
    for (i = 0; i < mstave->staves; ++i)
      if (mstave->name_lengths[i] + 20 > accWidth)
	accWidth = mstave->name_lengths[i] + 20;
  }

  if (p.x >= accWidth) {

    while(accWidth < wWidth && elist && BarValid(elist->bars[staveNo]) &&
	  Next(elist) && BarValid(((StaveEltList)Next(elist))->bars[staveNo]) &&
	  (p.x < accWidth || p.x >= accWidth + elist->widths[staveNo])) {
    
      accWidth += elist->widths[staveNo];
      elist = (StaveEltList)Next(elist);
    }
  }

  if (!elist || !BarValid(elist->bars[staveNo])) Return(NULL);
  else Return((List)elist);
}



int StaveGetPointedStave(MajorStave sp, XPoint p)
{
  int            staveNo;
  Dimension      iHeight;
  Dimension      wHeight;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  
  Begin("StaveGetPointedStave");
  if (!mstave || !staveLabel) Return(-1);
  
  YGetValue(staveLabel, XtNinternalHeight, &iHeight);
  YGetValue(staveLabel, XtNheight,         &wHeight);

  p.y = p.y - iHeight - (wHeight - StaveTotalHeight(mstave))/2;

  staveNo = p.y / (StaveTopHeight(1) - StaveUpperGap);

  if (StaveTopHeight(staveNo) - p.y > StaveUpperGap - 20) Return(-1);

  if (staveNo < 0) staveNo = 0;
  if (staveNo >= mstave->staves) staveNo = -1;

  Return(staveNo);
}


Pitch StaveGetPointedPitch(MajorStave sp, XPoint p, int staveNo, ClefTag clef)
{
  Pitch          rtn;
  int            y = p.y;
  Dimension      iHeight;
  Dimension      wHeight;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;

  Begin("StaveGetPointedPitch");
  if (!mstave || !staveLabel) Return(0);

  YGetValue(staveLabel, XtNinternalHeight, &iHeight);
  YGetValue(staveLabel, XtNheight,         &wHeight);

  y = y - iHeight - (wHeight - StaveTotalHeight(mstave)) / 2;
  y = y - StaveTopHeight(staveNo);

  if (y > StaveHeight + NoteHeight/4) {

    if (y > StaveHeight + StaveLowerGap - NoteHeight/2)
      y   = StaveHeight + StaveLowerGap - NoteHeight/2;

    y = y - StaveHeight - NoteHeight/2;

    rtn = y / (NoteHeight + 1);
    rtn = 2*rtn + ((rtn == (y + NoteHeight/2) / (NoteHeight + 1)) ? 0 : 1);
    rtn = -1 - rtn;

  } else {

    if (y < -(StaveUpperGap-NoteHeight/2)) y = -(StaveUpperGap-NoteHeight/2);
    y = StaveHeight + NoteHeight/2 - y - NoteHeight/2 + 1;
    
    rtn = y / (NoteHeight + 1);
    rtn = 2*rtn + ((rtn == (y + NoteHeight/2) / (NoteHeight + 1)) ? 0 : 1);
  }

  rtn -= ClefPitchOffset(clef);

  Return(rtn);
}

/* }}} */
/* {{{ Drawing the scrollbar rectangle */

static float RecToSBProportion(PointerRec *rec, int bars)
{
  Bar *bar = rec->bar;
  float propn;
  float propnOfBar;
  Begin("RecToSBProportion");

  if (!rec->left) propnOfBar = 0.0;
  else if (!iNext(rec->left)) propnOfBar = 1.0;
  else {
    propnOfBar = 
      (float)(MTimeToNumber(StaveItemToTime(NULL, 0, bar, iNext(rec->left))) -
	      MTimeToNumber(bar->bar.start_time)) /
      (float)(MTimeToNumber(bar->bar.time.bar_length));
  }

  propn = ((float)bar->bar.number + 1.0 + propnOfBar) / ((float)bars + 1.0);
  
  Return(propn);
}


static void StavePlotScrollbarRectangle(MajorStaveRec *mstave)
{
  float start, end;
  Begin("StavePlotScrollbarRectangle");

  if (TestNullRec(mstave->sweep.from) || TestNullRec(mstave->sweep.to)) End;

  start  = RecToSBProportion(&mstave->sweep.from, mstave->bars);
  end    = RecToSBProportion(&mstave->sweep.to,   mstave->bars);

  if (end < start) {
    float temp = start; start = end; end = temp;
  }

  if (end - start < 0.005) end = start + 0.005;

  StaveSetScrollbarMarks(True, start, end);
  End;
}


static Boolean staveRectDrawn = False;

/* This has to be a bit weird, because the xorGC will normally only */
/* work on mono displays.  In colour, we're using (essentially) the */
/* copyGC, so we want to avoid as far as possible drawing on top of */
/* other things on the screen (which would be rubbed out when we    */
/* removed the cursor).  So we have completely different cursor     */
/* designs in mono and colour.  Nasty, but it'll have to do for the */
/* moment.                                                          */

static void StavePlotRectangle(MajorStaveRec *ms)
{
  Position ay, by;
  Position ax = -2;
  Position bx = -2;
  Dimension w, ih, wh;
  PointerRec *from, *to;
  Begin("StavePlotRectangle");

  if (!staveLabel) End;

  from = &ms->sweep.from;
  if (TestNullRec(ms->sweep.to)) to = from;
  else to = &ms->sweep.to;

  YGetValue(staveLabel, XtNwidth, &w);

  if (from->bar->bar.number < FirstVisibleBar(ms)) ax = -1;
  else if (from->bar->bar.number > LastVisibleBar(ms)) {
    ax = w + 2;
  }

  if (to->bar->bar.number < FirstVisibleBar(ms)) bx = -1;
  else if (to->bar->bar.number > LastVisibleBar(ms)) {
    bx = w + 2;
  }

  if ((ax == -1 && bx == -1) || (ax > 0 && bx > 0)) End;

  if (ax == -2) ax = PointerRecToXBound(from);
  if (bx == -2) bx = PointerRecToXBound(to);

  if (ax > bx) {
    Position temp = bx;
    bx = ax;
    ax = temp;
  }

  YGetValue(staveLabel, XtNinternalHeight, &ih);
  YGetValue(staveLabel, XtNheight,         &wh);

  ay = by =
    ih + (wh - StaveTotalHeight(ms))/2 - (StaveUpperGap + StaveLowerGap)/2;

  ay += StaveTopHeight(ms->sweep.stave)  + 15;
  by += StaveTopHeight(ms->sweep.stave+1) - 5;

  if (oneD) {

    if (ax == bx) {

      XDrawLine(display, XtWindow(staveLabel), xorGC,
		ax - NoteWidth/2, ay + 20, ax + NoteWidth/2, ay + 20);

      XDrawLine(display, XtWindow(staveLabel), xorGC,
		ax, ay + 20, bx, by - 23);

      XDrawLine(display, XtWindow(staveLabel), xorGC,
		bx - NoteWidth/2, by - 23, bx + NoteWidth/2, by - 23);

      StaveSetScrollbarMarks(False, 0.0, 0.0);
      staveRectDrawn = False;

    } else {
    
      /* multiple XDrawLine calls seem to interact better */
      /* when XORing than a single call to XDrawRectangle */

      XDrawLine(display, XtWindow(staveLabel), xorGC, ax, ay, bx, ay);
      XDrawLine(display, XtWindow(staveLabel), xorGC, bx, ay, bx, by);
      XDrawLine(display, XtWindow(staveLabel), xorGC, bx, by, ax, by);
      XDrawLine(display, XtWindow(staveLabel), xorGC, ax, by, ax, ay);

      StavePlotScrollbarRectangle(ms);
      staveRectDrawn = True;
    }
  } else {

    if (ax == bx) {

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		ax - NoteWidth/2, ay + 20 - NoteWidth/2, ax, ay + 20);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		ax, ay + 20, ax + NoteWidth/2, ay + 20 - NoteWidth/2);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		bx - NoteWidth/2, by - 23 + NoteWidth/2, bx, by - 23);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		bx, by - 23, bx + NoteWidth/2, by - 23 + NoteWidth/2);

      StaveSetScrollbarMarks(False, 0.0, 0.0);
      staveRectDrawn = False;

    } else {
      
      XDrawLine(display, XtWindow(staveLabel), copyGC,
		ax, ay + NoteWidth/2, ax, ay);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		ax, ay, bx, ay);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		bx, ay + NoteWidth/2, bx, ay);
      
      XDrawLine(display, XtWindow(staveLabel), copyGC,
		ax, by - NoteWidth/2, ax, by);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		ax, by, bx, by);

      XDrawLine(display, XtWindow(staveLabel), copyGC,
		bx, by - NoteWidth/2, bx, by);

      StavePlotScrollbarRectangle(ms);
      staveRectDrawn = True;
    }
  }

  End;
}


static void StaveUnplotRectangle(MajorStaveRec *mstave)
{
  GC saveGC;
  Begin("StaveUnplotRectangle");

  if (oneD) StavePlotRectangle(mstave);
  else {

    saveGC = copyGC;
    copyGC = clearGC;

    StavePlotRectangle(mstave);

    copyGC = saveGC;
  }

  StaveSetScrollbarMarks(False, 0.0, 0.0);

  End;
}


static void StaveCursorDrawRectangle(MajorStaveRec *mstave)
{
  Begin("StaveCursorDrawRectangle");

  if (!mstave || !staveLabel) End;
  if (TestNullRec(mstave->sweep.from)) End;
  StavePlotRectangle(mstave);

  End;
}


static void StaveRemoveRectangle(MajorStaveRec *mstave)
{
  Begin("StaveRemoveRectangle");

  if (!mstave || !staveLabel) End;
  if (TestNullRec(mstave->sweep.from)) End;
  StaveUnplotRectangle(mstave);

  End;
}

/* }}} */
/* {{{ Placing, marking, extending cursor marks */

void StaveCursorDrawX(XPoint p, XPoint l)
{
  Begin("StaveCursorDrawX");

  XDrawLine(display, XtWindow(staveLabel), xorGC,
	    p.x - 8, p.y, p.x - 8, p.y + NoteHeight-1);

  XDrawLine(display, XtWindow(staveLabel), xorGC,
	    p.x + 9, p.y, p.x + 9, p.y + NoteHeight-1);

  XDrawLine(display, XtWindow(staveLabel), xorGC,
	    l.x, l.y - 2, l.x, l.y - StaveHeight + 1);

  End;
}


static void StaveCursorReject(MajorStaveRec *mstave)
{
  Begin("StaveCursorReject");

  MakeNullRec(mstave->sweep.from);
  MakeNullRec(mstave->sweep.to);
  mstave->sweep.swept = False;

  LeaveMenuMode(CursorPlacedMode | AreaSweptMode | SingleItemSweptMode |
		MultipleItemsSweptMode);
  EnterMenuMode(CursorNotPlacedMode | NoAreaSweptMode);

  StaveSetScrollbarMarks(False, 0.0, 0.0);

  End;
}


void StaveCursorMark(MajorStave sp, XPoint p)
{
  int staveNo;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorMark");

  if (!TestNullRec(mstave->sweep.from)) StaveRemoveRectangle(mstave);
  staveNo = StaveGetPointedStave(sp, p);

  mstave->sweep.swept = False;
  mstave->sweep.stave = staveNo;
  mstave->sweep.from  = StaveGetPointedItem(sp, p, staveNo,
					    StaveGetPointedBar(sp, p, staveNo));
  MakeNullRec(mstave->sweep.to);

  if (TestNullRec(mstave->sweep.from)) {
    StaveCursorReject(mstave); End;
  }

  StaveCursorDrawRectangle(mstave);
  
  End;
}


static void SwapStavePointerRecs(MajorStaveRec *mstave)
{
  PointerRec temp;
  Begin("SwapStavePointerRecs");

  temp = mstave->sweep.from;
  mstave->sweep.from = mstave->sweep.to;
  mstave->sweep.to = temp;

  End;
}


/* This will happily leave the "from" and "to" fields the wrong way
   around, with "from" after "to" -- StaveCursorFinish deals with this */

void StaveCursorExtend(MajorStave sp, XPoint p)
{
  int staveNo;
  PointerRec r;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorExtend");

  if (TestNullRec(mstave->sweep.from)) End;

  staveNo = StaveGetPointedStave(sp, p);
  if (staveNo != mstave->sweep.stave) {
    StaveRemoveRectangle(mstave);
    XBell(display, 70); StaveCursorReject(mstave); End;
  }
    
  r = StaveGetPointedItem(sp, p, staveNo, StaveGetPointedBar(sp, p, staveNo));

  if (!TestNullRec(mstave->sweep.to) && r.left == mstave->sweep.to.left) {
    End;
  }

  StaveRemoveRectangle(mstave);
  mstave->sweep.to = r;

  if (TestNullRec(mstave->sweep.to)) {
    XBell(display, 70); StaveCursorReject(mstave); End;
  }

  StaveCursorDrawRectangle(mstave);

  End;
}


void StaveCursorExpose(MajorStave sp)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorExpose");

  if (staveMoved || staveChanged) {

    if (!staveChanged && mstave->sweep.swept && staveRectDrawn) {

      StavePlotRectangle(mstave);

    } else {

      StaveCursorReject(mstave);
      staveChanged = False;
    }
  } else {

    if (!TestNullRec(mstave->sweep.from)) StavePlotRectangle(mstave);
  }

  End;
}


void StaveCursorRemove(MajorStave sp)
{
  Begin("StaveCursorRemove");

  if (stave && staveLabel) {
    StaveRemoveRectangle((MajorStaveRec *)sp);
    StaveCursorReject((MajorStaveRec *)sp);
    staveMoved = True;
    StaveForceUpdate();
  }

  End;
}


static Boolean RecsInOrder(MajorStaveRec *s, PointerRec *a, PointerRec *b)
{
  ItemList l;
  Begin("RecsInOrder");

  /* okay.  These two are in the right order if:

     -- their bars are in the right numerical order; or
     -- both are in the same bar, and the second is "later"

     At least one of these must be true for any two pointers that are
     in the right order.
  */

  if      (b->bar->bar.number > a->bar->bar.number) Return(True);
  else if (a->bar->bar.number > b->bar->bar.number) Return(False);

  if (a->left == b->left) Return(True);	/* sort of */

  if (!a->left) Return(True);

  for (l = a->bar->group.start; l; l = iNext(l)) {
    if      (l == a->left) Return(True);
    else if (l == b->left) Return(False);
  }

  fprintf(stderr, "Warning: StaveCursor.c: RecsInOrder: something is wrong\n");
  Return(True);
}


/* This should only be called when both ends of the sweep are visible, */
/* i.e. it's only just been swept by hand, it's not an extend-select   */

void StaveCursorFinish(MajorStave sp)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorFinish");

  if (TestNullRec(mstave->sweep.from)) End;

  mstave->sweep.swept = True;

  if (!RecsInOrder(mstave, &mstave->sweep.from, &mstave->sweep.to)) {
    SwapStavePointerRecs(mstave);
  }

  if (mstave->sweep.from.left == mstave->sweep.to.left) {
    LeaveMenuMode( CursorNotPlacedMode |   AreaSweptMode );
    EnterMenuMode( CursorPlacedMode    | NoAreaSweptMode );
  } else {
    LeaveMenuMode( NoAreaSweptMode | CursorPlacedMode    );
    EnterMenuMode(   AreaSweptMode | CursorNotPlacedMode );
  }

  /* We decide there's only one item swept if:

     -- there's at least one item swept ("end" is valid), &
        -- the item before the last one in the sweep is the same one as
           that before the start of the sweep
  */

  if (mstave->sweep.to.left &&
      (iPrev(mstave->sweep.to.left) == mstave->sweep.from.left)) {

    LeaveMenuMode(MultipleItemsSweptMode);
    EnterMenuMode(SingleItemSweptMode);

  } else {

    LeaveMenuMode(SingleItemSweptMode);
    EnterMenuMode(MultipleItemsSweptMode);
  }

  staveMoved = False;

  End;
}


/* called from select-extend */

void StaveCursorExplicitExtend(MajorStave sp, XPoint p)
{
  int staveNo;
  PointerRec r;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorExplicitExtend");

  if (TestNullRec(mstave->sweep.from)) {
    XBell(display, 70);
    End;
  }

  if (TestNullRec(mstave->sweep.to)) {

    StaveCursorExtend(sp, p);
    StaveCursorFinish(sp);

  } else {
    
    staveNo = StaveGetPointedStave(sp, p);

    if (staveNo != mstave->sweep.stave) {
      StaveRemoveRectangle(mstave);
      XBell(display, 70); StaveCursorReject(mstave); End;
    }
    
    r = StaveGetPointedItem(sp, p, staveNo, StaveGetPointedBar(sp, p, staveNo));

    if (r.left == mstave->sweep.from.left || r.left == mstave->sweep.to.left) {
      End;
    }

    if (RecsInOrder(mstave, &r, &mstave->sweep.from)) {
      StaveRemoveRectangle(sp);
      mstave->sweep.from = r;
    } else if (RecsInOrder(mstave, &mstave->sweep.to, &r)) {
      StaveRemoveRectangle(sp);
      mstave->sweep.to = r;
    } else {
      XBell(display, 70);
      End;
    }
    
    StaveCursorDrawRectangle(sp);
    StaveCursorFinish(sp);
  }

  End;
}    

/* }}} */
/* {{{ Selecting area, bar and staff */

void StaveCursorSelectSublist(MajorStave sp, int staveNo,
			      ItemList a, ItemList b) /* lefts */
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  /*  ItemList start = a ? iNext(a) : mstave->music[staveNo];
  ItemList afterEnd = b ? iNext(b) : mstave->music[staveNo];*/
  Begin("StaveCursorSelectSublist");

  StaveRemoveRectangle(sp);

  mstave->sweep.from.left = a;
  mstave->sweep.from.bar = StaveItemToBar(sp, staveNo, a);
  mstave->sweep.from.time = StaveItemToTime
    (sp, staveNo, mstave->sweep.from.bar, a);

  mstave->sweep.to.left = b;
  mstave->sweep.to.bar = StaveItemToBar(sp, staveNo, b);
  mstave->sweep.to.time = StaveItemToTime
    (sp, staveNo, mstave->sweep.to.bar, b);

  StaveCursorDrawRectangle(sp);
  StaveCursorFinish(sp);
  End;
}  


void StaveCursorSelectBar(MajorStave sp, XPoint p, Boolean doubleClick)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorSelectBar");

  if (!(TestNullRec(mstave->sweep.to) ||
	mstave->sweep.to.left == mstave->sweep.from.left)) {
    XBell(display, 70);
    End;
  }

  if (doubleClick) {
    PointerRec r;
    int staveNo;

    staveNo = StaveGetPointedStave(sp, p);
    if (staveNo != mstave->sweep.stave) End;
    
    r = StaveGetPointedItem(sp, p, staveNo, StaveGetPointedBar(sp, p, staveNo));
    if (r.left != mstave->sweep.from.left) End;
  }

  StaveRemoveRectangle(sp);

  mstave->sweep.from.left = iPrev(mstave->sweep.from.bar->group.start);
  mstave->sweep.from.time = mstave->sweep.from.bar->bar.start_time;

  mstave->sweep.to.left = mstave->sweep.from.bar->group.end;

  mstave->sweep.to.bar = mstave->sweep.from.bar;
  mstave->sweep.to.time =
    StaveItemToTime(0, 0, mstave->sweep.from.bar, mstave->sweep.to.left) +
    mstave->sweep.to.left->item->methods->get_length
    (mstave->sweep.to.left->item);

  StaveCursorDrawRectangle(sp);
  StaveCursorFinish(sp);

  End;
}


void StaveCursorSelectStaff(MajorStave sp, XPoint p, Boolean doubleClick)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveCursorSelectBar");

  if (TestNullRec(mstave->sweep.from)) {
    XBell(display, 70);
    End;
  }

  StaveReformatEverything(sp);	/* extremely dubious */

  if (doubleClick) {
    PointerRec r;
    int staveNo;

    staveNo = StaveGetPointedStave(sp, p);
    if (staveNo != mstave->sweep.stave) End;
    
    r = StaveGetPointedItem(sp, p, staveNo, StaveGetPointedBar(sp, p, staveNo));
    if (r.bar != mstave->sweep.from.bar) End;
  }

  StaveRemoveRectangle(sp);

  mstave->sweep.from.left = NULL;
  mstave->sweep.from.time = zeroTime;
  mstave->sweep.from.bar =
    ((StaveEltList)First(mstave->bar_list))->bars[mstave->sweep.stave];

  mstave->sweep.to.left = (ItemList)Last(mstave->music[mstave->sweep.stave]);

  mstave->sweep.to.time = mstave->total_length;
  mstave->sweep.to.bar = StaveItemToBar(sp, mstave->sweep.stave,
					 mstave->sweep.to.left);

  StaveCursorDrawRectangle(sp);
  StaveCursorFinish(sp);

  End;
}

/* }}} */
/* {{{ Clean up */

void StaveCursorCleanUp(MajorStave sp)
{
  StaveCursorReject((MajorStaveRec *)sp);
}

/* }}} */

