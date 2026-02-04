
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* MajorStave constructors, methods &c.             */

/* {{{ Includes */

#include <stdio.h>

#include "General.h"
#include "Tags.h"
#include "Notes.h"
#include "Classes.h"
#include "GC.h"
#include "IO.h"
#include "Palette.h"
#include "Visuals.h"
#include "Widgets.h"
#include "Stave.h"
#include "Yawn.h"
#include "Format.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "ItemList.h"
#include "Marks.h"
#include "Menu.h"
#include "Spline.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Scrollbar.h>

/* }}} */
/* {{{ Variables */

Boolean       staveMoved     = False;
Boolean       staveChanged   = False;
Widget        staveLabel     = NULL;
static Widget staveScrollbar = NULL;

MajorStave   stave = NULL;
static Key * initialKey = NULL;

/* }}} */
/* {{{ Prototypes */

static void StaveScrollbarSet(Widget);
static void StaveScrollbarSetTop(Widget);

static void StaveTrackingEventCallback(Widget, XtPointer, XEvent *, Boolean *);

/* }}} */

/* {{{ Constructors and destructors */

StaveEltList AddBarArray(StaveEltList, int);


void StaveInitialise(Widget parent)
{
  Begin("StaveInitialise");

  staveLabel = YCreateWidget("Music", labelWidgetClass, parent);

  YSetValue(staveLabel, XtNinternalHeight, 0);
  YSetValue(staveLabel, XtNinternalWidth,  0);
  YSetValue(staveLabel, XtNborderWidth,    0);
  YSetValue(staveLabel, XtNlabel,       "  ");

  StaveEditInitialise(staveLabel);

  /*  XtAddRawEventHandler(staveLabel, NoEventMask, True,
		       StaveTrackingEventCallback, NULL);
		       */

  XtInsertEventHandler(staveLabel, NoEventMask, True,
		       StaveTrackingEventCallback, NULL, XtListHead);

  End;
}


/* Call before exiting, but after all staves */
/* have been unmapped and destroyed.  Can be */
/* safely called before StaveInitialise...   */

void StaveCleanUp(void)
{
  Pixmap bitmap = 0;

  Begin("StaveCleanUp");

  if (!display) End;

  if (staveLabel) {
    YGetValue(staveLabel, XtNbitmap, &bitmap);
    XtDestroyWidget(staveLabel);
  }

  if (bitmap) XFreePixmap(display, bitmap);

  End;
}


/* This function SHARES the `music' argument you give it, and   */
/* sets all list pointers in the music array back to the starts */
/* of their lists.  You'd be wise to create a new array for     */
/* each new stave, and allow the stave to get on with it.  (Not */
/* forgetting to free the array when you destroy the stave.)    */

MajorStave NewStave(int staves, ItemList *music)
{
  MajorStaveRec *mstave;
  int            i;

  Begin("NewStave");

  if (staves < 1) Error("NewStave called for empty stave list");

  mstave = (MajorStaveRec *)XtMalloc(sizeof(MajorStaveRec));

  mstave->staves  = staves;
  mstave->formats =
    (StaveFormattingRec *)XtMalloc(staves * sizeof(StaveFormattingRec));

  mstave->display_start  = 0;
  mstave->display_number = 0;
  mstave->total_length   = zeroTime;
  mstave->bars           = 1;
  mstave->bar_list       = AddBarArray(NULL, staves);
  mstave->bar_tags       = (StaveBarTag *)XtCalloc(staves, sizeof(StaveBarTag));
  mstave->sweep          = NewSweep();
  mstave->names          = (String *)XtCalloc(staves, sizeof(String));
  mstave->name_lengths   = (int *)XtMalloc(staves * sizeof(int));
  mstave->connected_down = (Boolean *)XtMalloc(staves * sizeof(Boolean));
  mstave->midi_patches   = (int *)XtCalloc(staves, sizeof(int));
  mstave->visible        = False;
  mstave->music          = music;

  mstave->undo = UndoCreateUndoStack();

  for (i = 0; i < staves; ++i) {

    mstave->music[i]         = (ItemList)First(mstave->music[i]);
    mstave->formats[i].items = mstave->music[i];
    mstave->formats[i].tied  = False;
    mstave->formats[i].key   = NULL;
    mstave->formats[i].clef  = NULL;
    mstave->formats[i].next  = 0;
    mstave->name_lengths[i]  = -1;

    mstave->names[i]         = (String)XtMalloc(10);
    sprintf(mstave->names[i], "Staff %d", i+1);

    mstave->connected_down[i] = False;

    mstave->bar_tags[i].precedes = DoubleBar;
    mstave->bar_tags[i].follows  = DoubleBar;

    StaveFormatBars((MajorStave)mstave, i, -1);
  }

  Return((MajorStave)mstave);
}


/* if freeP is True, the music fields will be razed and the        */
/* stave structure's own memory set free.  We have the technology. */

void StaveDestroy(MajorStave sp, Boolean freeP)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  StaveEltList   list;
  int            i;

  Begin("StaveDestroy");

  for (list = (StaveEltList)First(mstave->bar_list); list;
       list = (StaveEltList)Next(list)) {

    for (i = 0; i < mstave->staves; ++i)
      if (list->bars[i]) DestroyBar((MusicObject)(list->bars[i]));

    XtFree((void *)(list->widths));
    XtFree((void *)(list->bars));
  }

  UndoDestroyUndoStack(mstave->undo);
  DestroyList(mstave->bar_list);

  for (i = 0; i < mstave->staves; ++i)
    if (mstave->names[i]) XtFree(mstave->names[i]);

  XtFree((void *)(mstave->bar_tags));

  XtFree((void *)(mstave->formats));
  if (freeP) {
    for (i = 0; i < mstave->staves; ++i) DestroyItemList(mstave->music[i]);
    XtFree((void *)(mstave));
  }

  End;
}


StaveEltList NewStaveEltList(Bar **bars, BarTag *starts, Dimension *widths)
{
  StaveEltList list;

  Begin("NewStaveEltList");

  list = (StaveEltList)NewList(sizeof(StaveEltListElement));
  list->bars   = bars;
  list->widths = widths;

  Return(list);
}



StaveSweep NewSweep(void)
{
  StaveSweep sweep;

  Begin("NewSweep");
 
  sweep.swept      = False;
  sweep.stave      = 0;

  MakeNullRec(sweep.from);
  MakeNullRec(sweep.to);

  Return(sweep);
}



StaveEltList AddBarArray(StaveEltList list, int staves)
{
  int          i;
  StaveEltList newlist;

  Begin("AddBarArray");

  newlist = (StaveEltList)
    Last(Nconc(list,
	       NewStaveEltList
	       ((Bar **)XtMalloc(staves * sizeof(Bar *)),
		(BarTag *)XtMalloc(staves * sizeof(BarTag)),
		(Dimension *)XtMalloc(staves * sizeof(Dimension)))));

  for (i = 0; i < staves; ++i) {
    newlist->widths[i] = 0; newlist->bars[i] = NULL; 
  }

  Return(newlist);
}

/* }}} */
/* {{{ Setting values in opaque Stave */

void StaveSetEndBarTags(MajorStave sp, int staveNo, BarTag p, BarTag f)
{
  Begin("StaveSetEndBarTags");

  ((MajorStaveRec *)sp)->bar_tags[staveNo].precedes = p;
  ((MajorStaveRec *)sp)->bar_tags[staveNo].follows  = f;

  End;
}


void StaveSetConnection(MajorStave sp, int staveNo, Boolean connected_down)
{
  Begin("StaveSetConnection");

  ((MajorStaveRec *)sp)->connected_down[staveNo] = connected_down;

  End;
}


void StaveSetMIDIPatch(MajorStave sp, int staveNo, int program)
{
  Begin("StaveSetConnection");

  ((MajorStaveRec *)sp)->midi_patches[staveNo] = program;

  End;
}


static StaveEltList StaveItemToEltList(MajorStave, int, ItemList);

Bar *StaveSetTimeSignatures(MajorStave sp, int staveNo, ItemList i,
			    TimeSignature *time)
{
  Bar *rtn = 0;
  StaveEltList elist;
  Begin("StaveSetTimeSignatures");

  elist = StaveItemToEltList(sp, staveNo, i);
  if (elist) rtn = elist->bars[staveNo];

  while (elist && elist->bars[staveNo]) {
    (void)NewTimeSignature(&elist->bars[staveNo]->bar.time, time->numerator,
			   time->denominator);
    elist = (StaveEltList)Next(elist);
  }

  Return(rtn);
}


void StaveRenameStave(MajorStave sp, int staveNo, String name)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  int i, j;
  
  Begin("StaveRenameStave");

  for (i = staveNo; i > 0 && mstave->connected_down[i-1]; --i);
  for (j = staveNo; j < mstave->staves-1 && mstave->connected_down[j]; ++j);

  for (staveNo = i; staveNo <= j; ++staveNo) {

    if (mstave->names[staveNo]) XtFree(mstave->names[staveNo]);

    mstave->names[staveNo] = XtNewString(name);
    mstave->name_lengths[staveNo] = -1;
  }

  if (mstave->visible) StaveRefresh(sp, -1);
  
  End;
}

/* }}} */
/* {{{ Type converters and locators for items in stave */

static StaveEltList StaveItemToEltList(MajorStave sp, int staveNo, ItemList i)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  StaveEltList   elist;
  ItemList       itr;

  Begin("StaveItemToEltList");

  elist = (StaveEltList)First(mstave->bar_list);

  if (!i) Return(elist);
  if (!elist || !elist->bars[staveNo] || !Next(elist)) Return(elist);

  for (itr = (ItemList)First(mstave->music[staveNo]);
       elist && Next(elist) && ((StaveEltList)Next(elist))->bars[staveNo] &&
	 itr && (itr != i); ) {

    itr = iNext(itr);

    if (itr == ((StaveEltList)Next(elist))->bars[staveNo]->group.start) {
      elist = (StaveEltList)Next(elist);
    }
  }

  Return(elist);
}  


/* Find the bar containing a given item (inefficiently) */

Bar *StaveItemToBar(MajorStave sp, int staveNo, ItemList i)
{
  StaveEltList elist;
  Begin("StaveItemToBar");

  elist = StaveItemToEltList(sp, staveNo, i);
  if (elist) Return(elist->bars[staveNo]);
  else Return(0);
}


static StaveEltList StaveTimeToEltList(MajorStave sp, int staveNo, MTime time,
				       StaveEltList startList)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  StaveEltList elist;
  Begin("StaveTimeToBar");

  if (startList) elist = startList;
  else elist = (StaveEltList)First(mstave->bar_list);

  if (!elist) Return(0);

  while (elist->bars[staveNo] && Next(elist) &&
	 ((StaveEltList)Next(elist))->bars[staveNo] &&
	 MTimeLesser(((StaveEltList)Next(elist))->bars[staveNo]->bar.start_time,
		     time)) {
    elist = ((StaveEltList)Next(elist));
  }

  Return(elist);
}

  
Bar *StaveTimeToBar(MajorStave sp, int staveNo, MTime time)
{
  StaveEltList elist;
  Begin("StaveTimeToBar");

  elist = StaveTimeToEltList(sp, staveNo, time, 0);
  if (elist) Return(elist->bars[staveNo]);
  
  Return(NULL);
}

ItemList StaveTimeToItem(MajorStave sp, int staveNo, Bar *bar, MTime time)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  MTime delta;
  ItemList il;
  Begin("StaveTimeToItem");

  if (!bar || !BarValid(bar)) {

    StaveEltList bl = mstave->bar_list;

    while (bl && BarValid(bl->bars[staveNo])) {
      bar = bl->bars[staveNo];
      delta = bar->bar.start_time;
      if (MTimeGreater(AddMTime(delta, bar->bar.time.bar_length), time)) break;
      bl = (StaveEltList)Next(bl);
    }

    if (!bl || !BarValid(bar)) {
/*      fprintf(stderr, "Warning: StaveTimeToItem: time %ld after the end of "
	      "staff %d\n", MTimeToNumber(time), staveNo);*/
      Return((ItemList)Last(mstave->music[staveNo]));
    }
  } else {

    delta = bar->bar.start_time;
  }

  for (ItemList_ITERATE(il, bar->group.start)) {
    
    delta = AddMTime(delta, il->item->methods->get_length(il->item));
    if (MTimeGreater(delta, time)) Return(il);
  }
  /*
  fprintf(stderr, "Warning: StaveTimeToItem: time %ld after the end of "
	  "staff %d\n", MTimeToNumber(time), staveNo);*/
  Return((ItemList)Last(bar->group.start));
}

/* Find location of item, optionally given a bar (the item doesn't
   have to be in the given bar, but it must be in or after it; if you
   do supply the bar, you don't need to give sp or staveNo) */

MTime StaveItemToTime(MajorStave sp, int staveNo, Bar *bar, ItemList i)
{
  MTime delta;
  ItemList il;
  Begin("StaveItemToTime");

  if (!i) return zeroTime;

  if (!bar) bar = StaveItemToBar(sp, staveNo, i);
  delta = bar->bar.start_time;
  
  for (ItemList_ITERATE(il, bar->group.start)) {
    if (i == il) Return(delta);
    delta = AddMTime(delta, il->item->methods->get_length(il->item));
  }

  fprintf(stderr, "Warning: StaveItemToTime: itemlist %p not found in staff "
	  "%d\n", i, staveNo);

  Return(((MajorStaveRec *)sp)->total_length);
}


/* Functions to find the clef, time and key sigs in effect at a given
   point in the staff */

TimeSignature *StaveItemToTimeSignature(MajorStave sp, int staveNo, ItemList i)
{
  Bar *bar;

  Begin("StaveItemToTimeSignature");
  
  bar = StaveItemToBar(sp, staveNo, i);
  if (bar) Return(&bar->bar.time);
  else Return(&defaultTimeSignature);
}


/* ARGSUSED */
Key *StaveItemToKey(MajorStave sp, int staveNo, ItemList i)
{
  Begin("StaveItemToKeySignature");

  while (i) {
    if (i->item->object_class == KeyClass) {
      Return((Key *)i->item);
    }
    i = iPrev(i);
  }
  
  Return(&defaultKey);
}


/* ARGSUSED */
Clef *StaveItemToClef(MajorStave sp, int staveNo, ItemList i)
{
  Begin("StaveItemToClef");

  while (i) {
    if (i->item->object_class == ClefClass) Return((Clef *)i->item);
    i = iPrev(i);
  }
  
  Return(&defaultClef);
}

/* }}} */

/* {{{ Reformatting */

/* This is a cop-out.  It ensures that the stave is reformatted   */
/* after a change by setting the markers for "next bar to format" */
/* to the very first bar.  This is pretty inefficient, & doesn't  */
/* exactly make much use of the capacity for incremental format   */
/* that's provided in Stave.c.                                    */

void StaveResetFormatting(MajorStave sp, int staveNo)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;

  Begin("StaveResetFormatting");

  mstave->formats[staveNo].next  = 0;
  mstave->formats[staveNo].key   = NULL;
  mstave->formats[staveNo].clef  = NULL;
  mstave->formats[staveNo].items = mstave->music[staveNo];

  End;
}



/* a bit unpleasant */

void StaveReformatEverything(MajorStave sp)
{
  int i;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveReformatEverything");

  for (i = 0; i < mstave->staves; ++i) {
    StaveResetFormatting(sp, i);
    StaveFormatBars(sp, i, -1);
  }

  End;
}



/* Go through a stave, sorting its items into bars, starting at bar       */
/* number `barNo'.  First we throw away all bars from `barNo' onwards,    */
/* and then we start at the point on the item-list where `barNo' pointed, */
/* in each staff, and use the Bar's Format method to track down from      */
/* there, creating new StaveEltList bar structures pointing to the items  */
/* down the list.  If we run out of list in one staff before the others,  */
/* we should fill that staff up with NULL bars.                           */


void StaveFormatBars(MajorStave sp, int staveNo, int endBar)
				/* -1 in endBar for "all of it" */
{
  int                 i;
  int                 bars, barNo;     /* should be longs, really */
  ItemList            items;
  StaveEltList        list;
  MajorStaveRec      *mstave = (MajorStaveRec *)sp;
  StaveFormattingRec *frec = &(mstave->formats[staveNo]);
  TimeSignature      *time = &defaultTimeSignature;
  Clef               *clef = NULL;
  Key                *key  = NULL;
  MTime               delta = zeroTime;

  Begin("StaveFormatBars");

  if (!initialKey) initialKey = NewKey(NULL, KeyC);

  barNo = frec->next;
  if (barNo >= (bars = mstave->bars)) End;

  for (list = (StaveEltList)First(mstave->bar_list), i = 0;
       i < barNo && list && BarValid(list->bars[staveNo]);
       i ++, list = (StaveEltList)Next(list)) {

    time  = &list->bars[staveNo]->bar.time;
    clef  =  list->bars[staveNo]->bar.clef;
    key   =  list->bars[staveNo]->bar.key;

    delta = AddMTime(list->bars[staveNo]->bar.start_time,
		     time->bar_length);
  }

  barNo = i;
  ResetStaffKey(key);

  if (!frec->clef) frec->clef = clef;
  if (!frec->key)  frec->key  = key ? key : initialKey;

  for (items = mstave->music[staveNo]; items && (!(frec->clef));
       items = iNext(items)) {

    if (!frec->clef && items->item->object_class==ClefClass)
      frec->clef = (Clef *)(items->item);
  }

  if (!frec->clef) frec->clef = &defaultClef;

  items = frec->items;
  while(items && (endBar == -1 || barNo <= endBar)) {

    if (!(list->bars[staveNo])) {
      list->bars[staveNo] = NewBar(NULL, (unsigned long)barNo,
				   time->numerator, time->denominator);

    } else {
      time = &list->bars[staveNo]->bar.time;
      (void)NewBar(list->bars[staveNo], (unsigned long)barNo,
		   time->numerator, time->denominator);
    }
		   
    items = FormatBar(list->bars[staveNo],
		      frec->tied, &(frec->tied), items,
		      frec->clef, &(frec->clef),
		      frec->key,  &(frec->key), &delta);

    if (MTimeGreater(delta, mstave->total_length)) {
      mstave->total_length = delta;
    }

    /* notify relevant bar in every staff that its width (and hence
       layout) may have changed -- cc 11/95: */

    barNo ++;

#ifdef DEBUG
    fprintf(stderr,"\n\nFormatted bar no %d, items are\n\n",
	    list->bars[staveNo]->bar.number);

    PrintItemList(list->bars[staveNo]->group.start);
#endif
    
    if (!Next(list) || barNo >= bars) {
      list = AddBarArray(list, mstave->staves);
      bars ++; mstave->bars ++;
    } else list = (StaveEltList)Next(list);
  }

  frec->next  = barNo;
  frec->items = items;

  while(list) {

    if (BarValid(list->bars[staveNo])) {
      list->bars[staveNo]->bar.number = -1; /* invalidate */
    }

    list = (StaveEltList)Next(list);
  }

  End;
}

/* }}} */
/* {{{ Drawing */

void StaveForceUpdate(void)
{
  /*
     I used to send this event to force an expose on the label:

  static XExposeEvent event =
    { Expose, 0L, True, NULL, NULL, 0, 0, 1, 1000, 0 };

     This works, assuming that the stave label has no border.  Since
     we set it up to have no border that's fine.  Unfortunately with
     Xaw3d there's not just a border but also a shadowWidth, and if
     that was non-zero for the label, the expose would only affect the
     shadow and never reach the label.

     Now I could explicitly set shadowWidth zero on the stave label
     widget, except that (a) it looks quite nice with a shadow and (b)
     you never know what other infernal device someone might invent to
     stop things working right.  So from now on, I'm damned well going
     to zap the whole bloody thing and there's absolutely nothing you
     kids can do to stop me.
  */

  static XExposeEvent event =
    { Expose, 0L, True, NULL, NULL, 0, 0, 2000, 1500, 0 }; /* BWAHAHAHAHAHA!! */

  Begin("StaveForceUpdate");

  if (XtIsRealized(staveLabel) && XtIsManaged(staveLabel)) {

    event.display = XtDisplay(staveLabel);
    event.window  =  XtWindow(staveLabel);

    XSendEvent(display, XtWindow(staveLabel),
	       False, ExposureMask, (XEvent *)&event);
  }

  End;
}


static Drawable StaveGetLabelBitmap(MajorStave sp,
				    Dimension *wd, Dimension *ht)
{
  Pixmap           bitmap;
  Dimension        width;
  static Dimension prevHeight = 0;
  static Dimension totalWidth = 0;
  MajorStaveRec  * mstave = (MajorStaveRec *)sp;

  Begin("StaveGetLabelBitmap");

  YGetValue(staveLabel, XtNwidth,  &width);
  YGetValue(staveLabel, XtNbitmap, &bitmap);

  if (!bitmap || width != totalWidth ||
      StaveTotalHeight(mstave) != prevHeight) {

    totalWidth = width;
    prevHeight = StaveTotalHeight(mstave);
    if (bitmap) XFreePixmap(display, bitmap);

    bitmap =
      XCreatePixmap(XtDisplay(staveLabel),
		    RootWindowOfScreen(XtScreen(staveLabel)),
		    totalWidth, prevHeight,
		    DefaultDepthOfScreen(XtScreen(staveLabel)));

    YSetValue(staveLabel, XtNbitmap, bitmap);
  }

  XFillRectangle(display, bitmap, clearGC, 0, 0, totalWidth, prevHeight);

  if (wd) *wd = totalWidth;
  if (ht) *ht = prevHeight;

  Return(bitmap);
}


Dimension StaveDrawNames(MajorStave sp, Drawable drawable)
{
  int              i;
  MajorStaveRec  * mstave = (MajorStaveRec *)sp;
  Dimension        maxWidth = 0;
  int              asc, dsc, dir;
  XCharStruct      info;
  XGCValues        values;
  Boolean          haveValues = False;

  Begin("StaveDrawNames");

  for (i = 0; i < mstave->staves; ++i) {

    if (mstave->name_lengths[i] == -1) {

      if (!haveValues) {
	XGetGCValues(display, bigTextGC, GCFont, &values);
	haveValues = True;
      }

      if (mstave->names[i][0]) {

	XQueryTextExtents(display, values.font, mstave->names[i],
			  strlen(mstave->names[i]), &dir, &asc, &dsc, &info);

	mstave->name_lengths[i] = info.width;

      } else mstave->name_lengths[i] = 0;
    }

    if (mstave->name_lengths[i] > maxWidth)
      maxWidth = mstave->name_lengths[i];
  }

  for (i = 0; i < mstave->staves; ++i) {

    XDrawLine(display, drawable, drawingGC,
	      maxWidth + 19,
	      StaveUpperGap + i*(StaveUpperGap+ StaveHeight + StaveLowerGap),
	      maxWidth + 19, StaveHeight - 1 +
	      StaveUpperGap + i*(StaveUpperGap+ StaveHeight + StaveLowerGap));

    if (i > 0 && mstave->connected_down[i-1]) continue;

    XDrawString(display, drawable, bigTextGC,
		(mstave->connected_down[i] ?
		  6 + (maxWidth - mstave->name_lengths[i]) :
		 10 + (maxWidth - mstave->name_lengths[i])),
		StaveUpperGap + StaveHeight/2 + 4 +
		i * (StaveUpperGap + StaveHeight + StaveLowerGap) +
		(mstave->connected_down[i] ?
		 (StaveUpperGap + StaveLowerGap + StaveHeight)/2 : 0),
		mstave->names[i], strlen(mstave->names[i]));
  }

  Return(maxWidth + 20);
}


void StaveDrawSmallNames(MajorStave sp, Drawable drawable)
{
  int             i;
  MajorStaveRec * mstave = (MajorStaveRec *)sp;

  Begin("StaveDrawSmallNames");

  for (i = 0; i < mstave->staves; ++i) {

    XDrawString(display, drawable, tinyTextGC, 8,
		StaveUpperGap + StaveHeight + StaveLowerGap + 
		i * (StaveUpperGap + StaveHeight + StaveLowerGap),
		mstave->names[i], strlen(mstave->names[i]));
  }
  
  End;
}


void StaveDrawMidiPatches(MajorStave sp, Drawable drawable, int totalWidth)
{
  int             i;
  MajorStaveRec * mstave = (MajorStaveRec *)sp;
  char            text[20];
  static int      textWidth = -1;

  Begin("StaveDrawMidiPatches");

  if (!ShowingMIDIPatches) End;

  if (textWidth == -1) {
    XGCValues values;
    int dir, asc, dsc;
    XCharStruct info;
    XGetGCValues(display, tinyTextGC, GCFont, &values);
    XQueryTextExtents
      (display, values.font, "patch 888", 9, &dir, &asc, &dsc, &info);
    textWidth = info.width;
  }

  for (i = 0; i < mstave->staves; ++i) {

    sprintf(text, "patch %d\n", mstave->midi_patches[i]);

    XDrawString(display, drawable, tinyTextGC,
		totalWidth - textWidth - 8,
		StaveUpperGap + StaveHeight + 20 + 
		i * (StaveUpperGap + StaveHeight + StaveLowerGap),
		text, strlen(text));
  }
  
  End;
}


void StaveDrawBrace(Drawable drawable, GC gc, int x, int y0, int y1)
{
  int i;
  XPoint c[4];
  Begin("StaveDrawBrace");

  y0 += 5; y1 -= 5; 

  c[0].x = x -  3; c[0].y = y0;
  c[1].x = x - 12; c[1].y = y0 + ((y1 - y0) * 2) / 9;
  c[2].x = x;      c[2].y = y0 + ((y1 - y0) * 4) / 9;
  c[3].x = x - 10; c[3].y = y0 + ((y1 - y0) / 2);

  DrawSpline(display, drawable, gc, c + 1, c[0], c[3], 2, False);
  c[1].x ++; c[2].x ++;
  DrawSpline(display, drawable, gc, c + 1, c[0], c[3], 2, False);

  for (i = 0; i < 4; ++i) c[i].y = y1 - (c[i].y - y0);
  
  DrawSpline(display, drawable, gc, c + 1, c[0], c[3], 2, False);
  c[1].x --; c[2].x --;
  DrawSpline(display, drawable, gc, c + 1, c[0], c[3], 2, False);

  End;
}


void StaveRefresh(MajorStave sp, int barNo)    /* -1 is "start", with names */
{
  int              i;
  MajorStaveRec  * mstave = (MajorStaveRec *)sp;
  StaveEltList     barList = (StaveEltList)First(mstave->bar_list);
  Dimension        minWidth;
  Dimension        barWidth;
  Dimension        tempWidth;
  Dimension        incWidth = 0;
  Boolean          countedThisBar;
  Drawable         drawable;
  Bar            * bar;
  int              staveNo;
  int              bars = mstave->bars;
  Dimension        totalWidth = 0;
  MarkList       * markLists;
  Boolean          lastBar, barHere;
  MTime            finalDelta = zeroTime;
  
  Begin("StaveRefresh");

  if (bars == 0) Error("Cannot refresh zero-bar stave");

  mstave->display_start  = barNo;
  mstave->display_number = 0;
  staveMoved = True;

  drawable = (Drawable)StaveGetLabelBitmap(sp, &totalWidth, NULL);
  if (barNo == -1) { incWidth += StaveDrawNames(sp, drawable); barNo = 0; }
  else StaveDrawSmallNames(sp, drawable);
  StaveDrawMidiPatches(sp, drawable, totalWidth);

  for (i = 0; barList && Next(barList) && (i < barNo); ++ i)
    barList = (StaveEltList)Next(barList);

  markLists = (MarkList *)XtMalloc(mstave->staves * sizeof(MarkList));
  for (i = 0; i < mstave->staves; ++i) markLists[i] = 0;

  while (incWidth < totalWidth) {

    barWidth = 0;
    minWidth = 10000;

    if (barList) {

      for (staveNo = 0; staveNo < mstave->staves; ++staveNo) {

	if (mstave->formats[staveNo].next <= barNo)
	  StaveFormatBars(sp, staveNo, barNo);

	if (BarValid(barList->bars[staveNo])) {

	  bar = barList->bars[staveNo];
	  tempWidth =
	    GetBarWidth(bar, (Prev(barList) ?
			      ((StaveEltList)Prev(barList))->bars[staveNo] :0));

	  if (tempWidth < minWidth) minWidth = tempWidth;
	  if (tempWidth > barWidth) barWidth = tempWidth;
	}
      }
    }

    /* I reckon that a bar width of max * 0.6 * (1 + 1 / ((max/min)^3))
       should give reasonable results. */

 /* no - it makes things too squishy

    if (barWidth > minWidth) {
    
      double tempd;

      tempd = ((double)barWidth) / ((double)minWidth);
      tempWidth = barWidth;

      barWidth = (Dimension)
	((double)barWidth * 0.6 * (1.0 + 1.0 / (tempd * tempd * tempd)));
      
      if (barWidth > tempWidth) barWidth = tempWidth;
    }
 */
    /* No bar should be wider than the window.  In fact we force  */
    /* them to be a bit narrower, just so the user knows that the */
    /* whole bar is visible and none has disappeared off the end. */

    if (totalWidth > 90 && barWidth > totalWidth-80) barWidth = totalWidth-80;
    if (barWidth <= 0) barWidth = ClefWidth + NoteWidth;

    countedThisBar = False;
    lastBar = True;
    barHere = False;

    for (staveNo = 0; staveNo < mstave->staves; ++staveNo) {

      if (staveNo == mstave->staves-1) mstave->connected_down[staveNo] = False;

      if (barList && BarValid(barList->bars[staveNo])) {

	bar = barList->bars[staveNo];

	(void)DrawBar
	  (bar,
	   (Prev(barList) ? ((StaveEltList)Prev(barList))->bars[staveNo] : 0),
	   mstave->bar_tags, drawable, incWidth, StaveTopHeight(staveNo),
	   ClefPitchOffset(bar->bar.clef->clef.clef), barWidth);

	CollateMarksInBar(bar, &markLists[staveNo]);

	/* ensure first off-screen item *knows* it's off-screen */
	if (incWidth + barWidth >= totalWidth) {
	  if (iNext(bar->group.end))
	    iNext(bar->group.end)->item->item.x = totalWidth + 10;
	}

	barHere = True;
	if (Next(bar->group.end)) lastBar = False;

	/* tie together section-ending bars in all staffs */

	if (staveNo < mstave->staves-1) {

	  BarTag start, end;
	  Bar *bar = barList->bars[staveNo];
	  GC gc = mstave->connected_down[staveNo] ? drawingGC : dashedGC;
	  Boolean drawn = False;

	  start = BAR_START_TYPE(bar);
	  end = BAR_END_TYPE(bar);

	  if (barNo > 0) {

	    if (start != NoFixedBar && start != PlainBar &&
		start != RepeatRightBar && start != NoBarAtAll) {
	      XDrawLine(display, drawable, gc, incWidth,
			StaveTopHeight(staveNo) + StaveHeight + 2, incWidth,
			StaveTopHeight(staveNo + 1));
	      drawn = True;
	    }

	    if (end != NoFixedBar && end != PlainBar && end != RepeatLeftBar &&
		end != NoBarAtAll) {
	      XDrawLine(display, drawable, gc, incWidth + barWidth - 1,
			StaveTopHeight(staveNo) + StaveHeight,
			incWidth + barWidth - 1, StaveTopHeight(staveNo + 1));
	      drawn = True;
	    }

	    if (!drawn && mstave->connected_down[staveNo] &&
		start != NoBarAtAll) {
	      XDrawLine(display, drawable, dashedGC, incWidth,
			StaveTopHeight(staveNo) + StaveHeight + 2, incWidth,
			StaveTopHeight(staveNo + 1));
	    }

	  } else {

	    XDrawLine(display, drawable, gc, incWidth - 1,
		      StaveTopHeight(staveNo) + StaveHeight, incWidth - 1,
		      StaveTopHeight(staveNo + 1));

	    if (mstave->connected_down[staveNo]) {
	      StaveDrawBrace(drawable, drawingGC, incWidth - 1,
			     StaveTopHeight(staveNo),
			     StaveTopHeight(staveNo + 1) + StaveHeight);
	    }
	  }
	}

	bar->bar.width = barWidth;

	if (!countedThisBar) {
	  finalDelta = AddMTime(bar->bar.start_time, bar->bar.time.bar_length);
	  mstave->display_number ++;
	  countedThisBar = True;
	}
      }

      if (barList) barList->widths[staveNo] = barWidth;
    }

    if (barHere && lastBar) {
      mstave->total_length = finalDelta;
      mstave->bars = barNo+2; /* hack: should be +1, but it's wrong elsewhere */
    }

    if (barList) barList = (StaveEltList)Next(barList);
    incWidth += barWidth;
    ++ barNo;
  }

  for (i = 0; i < mstave->staves; ++i) {
    DrawMarks(drawable, StaveTopHeight(i), totalWidth, i, &markLists[i]);
    /* not DestroyMarkList, cos the marks are shared: */
    DestroyList((List)markLists[i]);
  }

  XtFree(markLists);

  mstave->visible = True;
  StaveScrollbarSet(staveScrollbar);
  StaveForceUpdate();

  End;
}
 

void StaveRefreshAsDisplayed(MajorStave sp)
{
  Begin("StaveRefreshAsDisplayed");

  StaveRefresh(sp, ((MajorStaveRec *)sp)->display_start);

  End;
}


static void StaveRefreshProportion(float distance) /* 0.0 left -> 1.0 right */
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;
  int            barNo  = (int)((float)(mstave->bars) * distance);

  Begin("StaveRefreshProportion");

  if (barNo >= mstave->bars) barNo = mstave->bars - 1;
  if (mstave->display_start != barNo - 1) StaveRefresh(stave, barNo - 1);

  End;
}



void StaveUnmap(MajorStave sp)
{
  Pixmap bitmap;
  Begin("StaveUnmap");

  YGetValue(staveLabel, XtNbitmap, &bitmap);
  if (bitmap) XFreePixmap(display, bitmap);
  YSetValue(staveLabel, XtNbitmap, NULL);
  StaveForceUpdate();

  End;
}

/* }}} */
/* {{{ Scrollbars and positioning */

static void StaveScrollbarSetTop(Widget scrollbar)
{
  Arg            arg;
  float          top;
  XtArgVal      *ld;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveScrollbarSetTop");

  top = ((float)(mstave->display_start + 1))/((float)(mstave->bars));

  if (sizeof(float) > sizeof(XtArgVal)) XtSetArg(arg, XtNtopOfThumb, &top);
  else {
    ld = (XtArgVal *)&top;
    XtSetArg(arg, XtNtopOfThumb, *ld);
  }

  XtSetValues(scrollbar, &arg, 1);

  End;
}


static void StaveScrollbarSet(Widget scrollbar)
{
  Arg            arg;
  static String  label = 0;
  static Arg     barg = { XtNlabel, (XtArgVal)0 };
  float          shown;
  XtArgVal      *ld;
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;

  Begin("StaveScrollbarSet");

  if (!label) {
    label = XtNewString("Bar 0000");
    barg.value = (XtArgVal)label;
  }

  shown = ((float)(mstave->display_number))/((float)(mstave->bars));

  if (sizeof(float) > sizeof(XtArgVal)) XtSetArg(arg, XtNshown, &shown);
  else {
    ld = (XtArgVal *)&shown;
    XtSetArg(arg, XtNshown, *ld);
  }

  sprintf(label + 4, "%d", mstave->display_start + 1);

  XtSetValues(scrollbar,   &arg, 1);
  XtSetValues(pageButton, &barg, 1);

  StaveScrollbarExpose(scrollbar, NULL, NULL, NULL);

  XtSetSensitive(pageButton, True);

  End;
}


/* w unused; client non-zero for "move whole page"; call unused */
void StaveLeftCallback(Widget w, XtPointer client, XtPointer call)
{
  int            barNo;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveLeftCallback");

  if (!stave) End;

  barNo = mstave->display_start - (client ? mstave->display_number : 1);
  if (barNo == mstave->display_start) barNo = mstave->display_start - 1;
  if (barNo < -1) barNo = -1;

  StaveRefresh(stave, barNo);
  StaveScrollbarSetTop(staveScrollbar);
  StaveScrollbarSet(staveScrollbar);

  StaveScrollbarExpose(staveScrollbar, NULL, NULL, NULL);

  End;
}


/* w unused; client non-zero for "move whole page"; call unused */
void StaveRightCallback(Widget w, XtPointer client, XtPointer call)
{
  int            barNo;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveRightCallback");

  if (!stave) End;

  barNo = mstave->display_start + (client ? mstave->display_number : 1);
  if (barNo == mstave->display_start) barNo = mstave->display_start + 1;
  if (barNo >= mstave->bars - 1) barNo = mstave->bars - 2;

  StaveRefresh(stave, barNo);
  StaveScrollbarSetTop(staveScrollbar);
  StaveScrollbarSet(staveScrollbar);

  StaveScrollbarExpose(staveScrollbar, NULL, NULL, NULL);

  End;
}


void StavePageCallback(Widget w, XtPointer client, XtPointer call)
{
  int    bar;
  String label;
  String reply;

  Begin("StavePageCallback");

  if (!stave) End;

  YGetValue(w, XtNlabel, &label);

  if ((reply = YGetUserInput
       (XtParent(w), "Jump to bar number", label + 4,
	YOrientHorizontal, "Moving around")) == NULL)
    End;

  bar = atoi(reply) - 1;

  if (bar > ((MajorStaveRec *)stave)->bars - 2)
      bar = ((MajorStaveRec *)stave)->bars - 2;
  if (bar <= 0) bar = -1;

  StaveRefresh(stave, bar);
  StaveScrollbarSetTop(staveScrollbar);
  StaveScrollbarSet(staveScrollbar);

  StaveScrollbarExpose(staveScrollbar, NULL, NULL, NULL);

  End;
}


void StaveScrollbarCallback(Widget scrollbar, XtPointer client, XtPointer call)
{
  int            barNo;
  int            move;
  Dimension      length;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveScrollbarCallback");

  if (!stave) End;

  YGetValue(scrollbar, XtNlength, &length);

  move  = mstave->display_number * ((int)call) / ((int)length);
  barNo = mstave->display_start + move;

  if (barNo == mstave->display_start)
    barNo = mstave->display_start + ((int)call > 0 ? -1 : 1);

  if (barNo >= mstave->bars - 1) barNo = mstave->bars - 2;
  if (barNo < -1) barNo = -1;

  StaveRefresh(stave, barNo);
  StaveScrollbarSetTop(staveScrollbar);
  StaveScrollbarSet(staveScrollbar);

  StaveScrollbarExpose(scrollbar, NULL, NULL, NULL);

  End;
}


void StaveJumpCallback(Widget scrollbar, XtPointer client, XtPointer call)
{
  Begin("StaveJumpCallback");

  StaveScrollbarExpose(scrollbar, NULL, NULL, NULL);
  if (stave) StaveRefreshProportion(*((float *)call));

  End;
}


void StaveInitialiseScrollbar(Widget scrollbar)
{
  Arg       arg;
  XtArgVal *ld;
  float     shown = 1.0;

  Begin("StaveInitialiseScrollbar");

  if (sizeof(float) > sizeof(XtArgVal)) XtSetArg(arg, XtNshown, &shown);
  else {
    ld = (XtArgVal *)&shown;
    XtSetArg(arg, XtNshown, *ld);
  }

  staveScrollbar = scrollbar;
  XtSetValues(scrollbar, &arg, 1);
  StaveScrollbarExpose(scrollbar, NULL, NULL, NULL);

  End;
}


void StaveScrollUpOrDownABit(Boolean up)
{
  static Widget scrollbar = NULL;
  int distance = up ? -30 : 30;

  Begin("StaveScrollUpOrDownABit");

  if (!scrollbar) scrollbar = XtNameToWidget(musicViewport, "vertical");
  if (!scrollbar) End;

  XtCallCallbacks(scrollbar, XtNscrollProc, (XtPointer)distance);
  StaveScrollbarExpose(scrollbar, NULL, NULL, NULL);

  End;
}


static float staveScrollbarMarkStart = 0.0;
static float staveScrollbarMarkEnd = 0.0;
static Boolean staveScrollbarMarked = False;

void StaveSetScrollbarMarks(Boolean marked, float start, float end)
{
  Dimension width;
  Dimension height;
  XExposeEvent event;
  Begin("StaveSetScrollbarMarks");

  if (!staveScrollbar) End;

  staveScrollbarMarked = marked;
  staveScrollbarMarkStart = start;
  staveScrollbarMarkEnd = end;

  StaveScrollbarExpose(staveScrollbar, (XtPointer)1, NULL, NULL);

  event.type = Expose;
  event.display = XtDisplay(staveScrollbar);
  event.window = XtWindow(staveScrollbar);

  XtVaGetValues(staveScrollbar, XtNwidth, &width, XtNheight, &height, NULL);
  event.x = 0; event.y = 0; event.width = width; event.height = height;
  event.count = 0;

  XSendEvent(XtDisplay(staveScrollbar), XtWindow(staveScrollbar),
	     False, ExposureMask, (XEvent *)&event);

  End;
}
  

/* p non-NULL for "explicitly clear previous" */

void StaveScrollbarExpose(Widget scrollbar, XtPointer p, XEvent *e, Boolean *b)
{
  Dimension width;
  Dimension height;
  Position x1, x2;
  static Position ox1 = -1, ox2 = -1;
  Begin("StaveScrollbarExpose");

  if (b) *b = True;
  if (!scrollbar) End;

  XtVaGetValues(scrollbar, XtNwidth, &width, XtNheight, &height, NULL);
  if (width < 10) End;

  x1 = staveScrollbarMarkStart * width;
  x2 = staveScrollbarMarkEnd * width;
  if (x2 <= x1) x2 = x1;

  if (p) {
    if (x1 <= ox1 && x2 >= ox2) {
      XDrawLine(XtDisplay(scrollbar), XtWindow(scrollbar),
		clearGC, ox1, 2, ox1, height - 5);
      XDrawLine(XtDisplay(scrollbar), XtWindow(scrollbar),
		clearGC, ox2, 2, ox2, height - 5);
    } else if (x1 >= 0 && x2 >= 0) {
      XDrawRectangle(XtDisplay(scrollbar), XtWindow(scrollbar),
		     clearGC, ox1, 1, ox2 - ox1, height - 3);
    }
  }

  if (!staveScrollbarMarked) End;

  XDrawRectangle(XtDisplay(scrollbar), XtWindow(scrollbar),
		 drawingGC, x1, 1, x2 - x1, height - 3);

  ox1 = x1;
  ox2 = x2;

  End;
}


Window StaveTrackingTargetWindow(void)
{
  if (staveLabel) return XtWindow(staveLabel);
  else return 0;
}


void StaveLeapToBar(MajorStave sp, int bar)
{
  /*  XtAppContext appContext;*/
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Begin("StaveLeapToBar");

  if (bar > mstave->bars - 2) bar = mstave->bars - 2;
  if (bar <= 0) bar = -1;

  StaveRefresh(sp, bar);
  StaveScrollbarSetTop(staveScrollbar);
  StaveScrollbarSet(staveScrollbar);
  StaveScrollbarExpose(staveScrollbar, NULL, NULL, NULL);

  StaveForceUpdate();

  /* ugh.  must be a better way */
  /*
  XSync(display, False);
  appContext = XtWidgetToApplicationContext(topLevel);
  while (XtAppPending(appContext)) {
    XtAppProcessEvent(appContext, XtIMAll);
  }
  */
  End;
}


/* Used for playback tracking */

static int prevLeapX = 0;

void StaveLeapToTime(MajorStave sp, MTime time, Boolean barOnly)
{
  int i;
  Bar *bar;
  static StaveEltList barList;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  Position x, maxX;
  Dimension w, h;
  ItemList il;
  
  Begin("StaveLeapToTime");

  YGetValue(staveLabel, XtNwidth, &w);
  YGetValue(staveLabel, XtNheight, &h);

  if (time == zeroTime) {
    prevLeapX = 0;
    barList = 0;
    XFillRectangle(display, XtWindow(staveLabel), clearGC, 0, 0, w, h);
    StaveLeapToBar(sp, -1);
    End;
  }

  barList = StaveTimeToEltList(sp, 0, time, barList);
  if (!barList) End;

  maxX = 0;

  for (i = 0; i < mstave->staves; ++i) {

    bar = barList->bars[i];
    if (!bar || !BarValid(bar)) continue;

    if (bar->bar.number < mstave->display_start ||
	(bar->bar.number > mstave->display_start &&
	 bar->bar.number > mstave->display_start + mstave->display_number -2)) {

      XFillRectangle(display, XtWindow(staveLabel), clearGC, 0, 0, w, h);
      StaveLeapToBar(sp, bar->bar.number);
      prevLeapX = 0;
    }

    if (barOnly) {

      if (!bar || !BarValid(bar) || !bar->group.start) continue;
      x = bar->item.x;

    } else {

      il = StaveTimeToItem(sp, i, bar, time);
      if (!il) continue;

      x = il->item->item.x;
    }

    if (x > w) {

      XFillRectangle(display, XtWindow(staveLabel), clearGC, 0, 0, w, h);
      StaveLeapToBar(sp, bar->bar.number);
      maxX = 0; prevLeapX = 0; break;

    } else if (x > maxX) maxX = x;
  }

  x = maxX;

  if (x > prevLeapX) {
    XFillRectangle(display, XtWindow(staveLabel), lightFillGC,
		   prevLeapX, 0, x - prevLeapX, h);
  } else if (x < prevLeapX) {
    XFillRectangle(display, XtWindow(staveLabel), lightFillGC, 0, 0, x, h);
  }

  prevLeapX = x;

  End;
}


static void StaveTrackingEventCallback(Widget w, XtPointer p, XEvent *e,
				       Boolean *b)
{
  MTime time;
  float beat;
  XEvent newE, goodE;
  Boolean barOnly = False;
  XtAppContext appContext;
  Begin("StaveTrackingEventCallback");

  if (b) *b = True;
  if (e->type != ClientMessage) End;

  if (!isdigit(e->xclient.data.b[0]) && e->xclient.data.b[0] != '-') {
    FileMenuFollowILCallback(e->xclient.data.b);
    End;
  }

  if (b) *b = False;		/* keep this to ourselves */
  if (!stave || !slaveMode) End;

  /* discard any more stuff */

  appContext = XtWidgetToApplicationContext(topLevel);

  goodE = *e;
  e = &goodE;

  while (XtAppPending(appContext)) {

    XtAppNextEvent(appContext, &newE);

    if (newE.type == Expose) {
      /*      if (newE.xexpose.window == StaveTrackingTargetWindow())*/
      prevLeapX = 0;
      (void)XtDispatchEvent(&newE);
    }
    
    if (newE.type == ClientMessage) {
	
      if (newE.xclient.window == StaveTrackingTargetWindow()) {
	goodE = newE;
      } else {
	/* we don't want to throw away potential Interlock messages */
	(void)XtDispatchEvent(&newE);
      }
    }
  }
  
  beat = atof(e->xclient.data.b);
  if (beat < 0.0) { barOnly = True; beat = -beat; }

  time = NumberToMTime
    ((long)(beat * (float)TagToNumber(Crotchet, False) + 0.01));

  if (time == zeroTime) time = NumberToMTime(1);
  
  StaveLeapToTime(stave, time, barOnly);

  End;
}

/* }}} */

/* {{{ I/O, and functions to clean up loaded staves */

void StaveAddBarTime(MajorStave sp,
		     int staveNo, unsigned long barNo, int num, int den)
{
  unsigned long barCount;
  StaveEltList slist, subList;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;

  Begin("StaveAddBarTime");

  for (slist = (StaveEltList)First(mstave->bar_list), barCount = 0;
       slist && barCount < barNo; slist = (StaveEltList)Next(slist),++barCount);

  if (slist) {
    if (slist->bars[staveNo]) {

      for (subList = (StaveEltList)Next(slist);
	   subList && subList->bars[staveNo] &&
	     BarTimesEqual(slist->bars[staveNo], subList->bars[staveNo]);
	   subList = (StaveEltList)Next(subList)) {
	
	(void)NewTimeSignature(&subList->bars[staveNo]->bar.time, num, den);
      }

      (void)NewTimeSignature(&slist->bars[staveNo]->bar.time, num, den);
      StaveResetFormatting(sp, staveNo);
    } else {
      /*      fprintf(stderr,"no bar %d in stave %d\n",(int)barNo,(int)staveNo);*/
    }
  } else {
    /*    fprintf(stderr,"no bar %d at all\n",(int)barNo);*/
  }
  
  End;
}

  

/* This is deprecated and only used by "old" I/O -- it doesn't work
   properly */

void StaveAddAbsoluteMark(MajorStave sp, int staveNo, int type,
			  unsigned long start, unsigned long end)
{
  unsigned long iCount;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  ItemList i, beginList, endList;
  Mark *leftMark, *rightMark;

  Begin("StaveAddAbsoluteMark");

  for (iCount = 0L, i = (ItemList)First(mstave->music[staveNo]);
       (iCount < start) && i; ) {

    i = iNext(i);

    if (!(i && TIED_BACKWARD(i->item))) {
      iCount ++;
    }
  }

  if (!i) return;
  beginList = i;

  while ((iCount < end) && i) {

    i = iNext(i);

    if (!(i && TIED_BACKWARD(i->item))) {
      iCount ++;
    }
  }

  if (!i) return;
  endList = i;

  leftMark = NewMark(NULL, (MarkTag)type, True, NULL);
  rightMark = NewMark(NULL, (MarkTag)type, False, leftMark);
  leftMark->other_end = rightMark;

  beginList->item->item.marks = (MarkList)Nconc
    (beginList->item->item.marks, NewMarkList(leftMark));

  endList->item->item.marks = (MarkList)Nconc
    (endList->item->item.marks, NewMarkList(rightMark));

  End;
}


void StaveWriteToFile(MajorStave sp, FILE *file)
{
  int            n;
  Group         *g;
  ItemList       i, j;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  StaveEltList   slist;
  unsigned long  iCount;

  Begin("StaveWriteToFile");

  StaveBusyStartCount(mstave->staves*2 + 1);
  fprintf(file, "\nStaves %d\n\n", mstave->staves);

  markIOIndex = 0;

  /* I. Items */

  for (n = 0; n < mstave->staves; ++n) {

    StaveBusyMakeCount(n*2);
    UnformatItemList((ItemList *)&mstave->music[n], NULL);
    StaveResetFormatting(sp, n);
    StaveBusyMakeCount(n*2 + 1);

    fprintf(file, "\nName %s\n",
	    mstave->names[n] ? mstave->names[n] : "<none>");

    for (ItemList_ITERATE(i, mstave->music[n])) {

      if (GROUPING_TYPE(i->item) != GroupNone &&
	  (i->item->item.grouping.beamed.start)) {

	for (j = i; j && !j->item->item.grouping.beamed.end; j = iNext(j));
	g = NewFakeGroup(i, j ? j : (ItemList)Last(i));
	g->methods->write((MusicObject)g, file, 0);
	i = g->group.end; continue;
      }

      i->item->methods->write((MusicObject)(i->item), file, 0);
      WriteMarks(i, file, 0); WriteFixedBars(i, file, 0);
    }

    fprintf(file, "End\n");
  }

  StaveBusyMakeCount(mstave->staves * 2);

  /* III. Stave and bar information */

  for (slist = (StaveEltList)First(mstave->bar_list), iCount = 0; slist;
       slist = (StaveEltList)Next(slist), ++iCount) {

    for (n = 0; n < mstave->staves; ++n) {

      if (slist->bars[n] &&
	  (!Prev(slist) ||
	   !BarTimesEqual(slist->bars[n],
			  ((StaveEltList)Prev(slist))->bars[n]))) {

	fprintf(file, "\nBar %d %lx time %d %d", n, iCount,
		(int)slist->bars[n]->bar.time.numerator,
		(int)slist->bars[n]->bar.time.denominator);
      }
    }
  }

  for (n = 0; n < mstave->staves; ++n) {
    fprintf(file, "\nStave %d tags %d %d connected %d program %d", n,
	    (int)mstave->bar_tags[n].precedes,
	    (int)mstave->bar_tags[n].follows,
	    (int)mstave->connected_down[n],
	    (int)mstave->midi_patches[n]);
  }

  StaveBusyFinishCount();

  End;
}

/* }}} */

