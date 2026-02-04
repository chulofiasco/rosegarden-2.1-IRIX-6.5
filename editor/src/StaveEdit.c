
/* Musical Notation Editor for X, Chris Cannam 1994   */
/* Functions to catch events, call dragging functions */
/* and so on.  Interacts with StaveCursor.c.  Also    */
/* handles insertion of the current palette entry.    */

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif

/* {{{ Includes */

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Simple.h>
#ifndef XtNcursor
#define XtNcursor "cursor"
#endif

#include "General.h"
#include "Visuals.h"
#include "Classes.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "StaveCursor.h"
#include "GC.h"
#include "Yawn.h"
#include "ItemList.h"
#include "Palette.h"
#include "Menu.h"
#include "Marks.h"

#include "hourglass.xbm"
#include "hourglass_mask.xbm"

#include "drag.xbm"
#include "drag_mask.xbm"

#include "hour0.xbm"
#include "hour1.xbm"
#include "hour2.xbm"
#include "hour3.xbm"
#include "hour4.xbm"
#include "hour5.xbm"
#include "hour6.xbm"
#include "hour7.xbm"

#include <X11/keysym.h>

/* }}} */
/* {{{ Declarations */

#define HOUR_FRAMES 8

static Pixmap dragPixmap      = 0;
static Pixmap dragMask        = 0;
static Pixmap hourglassPixmap = 0;
static Pixmap hourglassMask   = 0;

static Cursor hourglassCursor = 0;
static Cursor    insertCursor = 0;
static Cursor      dragCursor = 0;

static Pixmap      hourPixmap[] = { 0, 0, 0, 0, 0, 0, 0, 0, };
static Cursor      hourCursor[] = { 0, 0, 0, 0, 0, 0, 0, 0, };

static XPoint      pPrev, lPrev;
static Boolean     xDrawn = False;
static Boolean     buttonPressed = False;
static Boolean     staveInsertMode;
static MusicObject staveVisual;
static int         staveVisualIndex;
static PaletteTag  staveVisualType;
                           /* PaletteNotes, PaletteRests, PaletteClefs */

/* }}} */

/* {{{ Creating and inserting insertions */

/* Makes a generic object to be inserted.  Doesn't currently bother about */
/* getting the pitch (note voices) in chords, or getting modifiers or any */
/* such right.  These should be dealt with by StaveEditInsertInsertion.   */

MusicObject StaveEditCreateInsertion(void)
{
  Begin("StaveEditCreateInsertion");

  switch(staveVisualType) {

  case PaletteNotes:

    Return
      ((MusicObject)NewChord
       (NULL, NULL, 0, ModNone,
	((NoteVisualCompound *)staveVisual)[staveVisualIndex].dotted.type,
	PaletteModDottedQuery()));

  case PaletteRests:

    Return
      ((MusicObject)NewRest
       (NULL,
	((RestVisualCompound *)staveVisual)[staveVisualIndex].dotted.type,
	PaletteModDottedQuery()));

  case PaletteClefs:

    Return((MusicObject)NewClef
	   (NULL, ((ClefVisualRec *)staveVisual)[staveVisualIndex].type));

  default:
    Return(NULL);	   /* this would probably crash it, but never mind */
  }
}


ItemList StaveEditCreateInsertionList(void)
{
  MusicObject insertion;

  Begin("StaveEditCreateInsertionList");

  if ((insertion = StaveEditCreateInsertion()) == NULL) Return(NULL);
  else Return(NewItemList((Item *)insertion));
}


/* Insert the item found in "i" (a one-item list) after item "left" in
   staff "staveNo" */

void StaveEditInsertItem(ItemList i, int staveNo, ItemList left)
{
  Bar *bar;
  NoteVoice *voice;
  ItemList leftBound;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveEditInsertItem");

  bar = StaveItemToBar(stave, staveNo, left);
  if (!left) leftBound = NULL;
  else leftBound = bar->group.start;

  UndoAssertPreviousContents("Insertion", stave, staveNo, leftBound, left);

  if (left) {
    ClearMarkType(mstave->music[staveNo], iPrev(left), left, Tie);

    if (left->item->item.bar_tag == NoBarAtAll) {
      i->item->item.bar_tag = NoBarAtAll;
    }
  }

  /* put in the new item, after left: */

  if (left == NULL)
    mstave->music[staveNo] =
      (ItemList)First(Insert(i, mstave->music[staveNo]));
  else if (Next(left)) Insert(i, iNext(left));
  else Nconc(left, i);

  StaveResetFormatting(stave, staveNo);

  /* experimental auto-beaming stuff: */

  if (i->item->object_class == ChordClass &&
      left && Next(left) && !Next(Next(left))) {

    ItemList       first, temp;
    TimeSignature *time;

    time = &bar->bar.time;
    first = bar->group.start;

    /* check that we're not beaming existing groups with tags other
       than Beamed: */

    for (temp = first; temp; temp = iNext(temp)) {

      if (GROUPING_TYPE(temp->item) != GroupNone &&
	  GROUPING_TYPE(temp->item) != GroupBeamed) {

	for (first = temp;
	     (first && iNext(first) &&
	      GROUPING_TYPE(first->item) == GROUPING_TYPE(temp->item));
	     first = iNext(first));

	break;
      }
    }

    if (first && first != i) {
      ItemListAutoBeam(time, first, i);
    }
  }
  
  UndoAssertNewContents("Insertion", stave, staveNo, leftBound, i);
  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);

  End;  
}


/*
   Insert a new item from the selection in the Palette after item
   "left" in staff "staveNo".  If "end" is True, ignore "left" and
   insert at the end...

   If the selection is a Chord, then:

   -- if "usePitch" is True, use pitch "Pitch", otherwise
      calculate a pitch -- in this case, the XPoint must be
      non-null;

   -- if "useMods" is true, also use "mods" (sharp, flat &c).  */

void StaveEditInsertPaletteItem(int staveNo, ItemList left, Boolean end,
				Boolean usePitch, Pitch pitch,
				Boolean useMods, NoteMods mods,
				XPoint *p)
{
  ItemList i;
  NoteVoice *voice;
  Begin("StaveEditInsertPaletteItem");

  if (end) {
    left = (ItemList)Last(((MajorStaveRec *)stave)->music[staveNo]);
  }

  i = StaveEditCreateInsertionList();

  /* if it's a chord, find out a suitable pitch: */

  if (i->item->object_class == ChordClass) {

    if (!usePitch) {
      pitch = StaveGetPointedPitch
	(stave, *p, staveNo, StaveItemToClef(stave, staveNo, left)->clef.clef);
    }

    if (PaletteFollowKey()) {
      voice = NewNoteVoice(NULL, pitch, ModNone);
    } else {
      voice = NewNoteVoice(NULL, pitch, useMods ? mods : PaletteGetNoteMods());
    }

    (void)NewChord((Chord *)i->item, voice, 1, ModNone,
		   ((Chord *)i->item)->chord.visual->type,
		   ((Chord *)i->item)->chord.visual->dotted);

    if (PaletteFollowKey()) {

      if (!useMods) mods = PaletteGetNoteMods();

      KeyTransposeItemIntoNewKey
	(i->item, &defaultKey, StaveItemToKey(stave, staveNo, left));

      if (mods != ModNone) {
	((Chord *)i->item)->chord.voices[0].modifiers = mods;
      }
    } 
  }

  StaveEditInsertItem(i, staveNo, left);
}


void StaveEditInsertInsertion(XPoint p)
{
  PointerRec     rec;
  int            barNo;
  int            staveNo;
  ItemList       i, leftBound;
  StaveEltList   barList;
  NoteVoice     *voice;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveEditInsertInsertion");
  
  /* find the point at which to insert an item: */

  if ((staveNo = StaveGetPointedStave(stave, p)) < 0 ||
      (barList = (StaveEltList)StaveGetPointedBar(stave, p, staveNo))
      == NULL) {
    XBell(display, 70);
    End;
  }

  rec = StaveGetPointedItem(stave, p, staveNo, (List)barList);
  if (TestNullRec(rec)) End;

  StaveEditInsertPaletteItem(staveNo, rec.left, False, False, 0, False, 0, &p);

  End;
}

/* }}} */
/* {{{ Dragging, pointer motion, button & key presses */

static Boolean shiftPressed = False;
static Boolean  ctrlPressed = False;
static Boolean rCtrlPressed = False;
static Boolean   altPressed = False;

static Boolean startedDragging = False;	/* some redundancy with buttonPressed */


void StavePointerMovedCallback(Widget w, XtPointer client,
			       XEvent *e, Boolean *cont)
{
  XMotionEvent  *event = (XMotionEvent *)e;
  XPoint         p, l;
  Pitch          pitch;
  int            staveNo;
  StaveEltList   barList;
  Dimension      height;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StavePointerMovedCallback");
  if (event->type != MotionNotify || !stave ||
      (pPrev.x == event->x && pPrev.y == event->y)) End;

  p.x = event->x;
  p.y = event->y;

  if (xDrawn) {
    StaveCursorDrawX(pPrev, lPrev);
    xDrawn = False;
  }

  if (staveInsertMode) {

    PointerRec rec;

    if ((staveNo = StaveGetPointedStave(stave, p)) < 0 ||
	(barList = (StaveEltList)StaveGetPointedBar(stave, p, staveNo))
	== NULL)
      End;
      
    pitch = StaveGetPointedPitch
      (stave, p, staveNo, barList->bars[staveNo]->bar.clef->clef.clef);
    
    YGetValue(staveLabel, XtNheight, &height);
    
    p.y = (height - StaveTotalHeight(mstave))/2 + StaveTopHeight(staveNo) +
      STAVE_Y_COORD
      (pitch +
       ClefPitchOffset(barList->bars[staveNo]->bar.clef->clef.clef));
    
    /* line to show where to insert stuff -- very grimy: */
    
    rec = StaveGetPointedItem(stave, p, staveNo, (List)barList);
    if (TestNullRec(rec) || !rec.left) l.x = event->x;
    else {
      l.x = rec.left->item->item.x +
	rec.left->item->methods->get_min_width(rec.left->item) + 4;
      if (iNext(rec.left)) {
	if (l.x > iNext(rec.left)->item->item.x) {
	  l.x = iNext(rec.left)->item->item.x - 1;
	}
      } else {
	/* if we're at the end of the staff, don't show a line at all */
	l.x = -2;
      }
    }

    l.y = (height - StaveTotalHeight(mstave))/2 + StaveTopHeight(staveNo) +
      STAVE_Y_COORD(-1);	/* Why -1, and not 0?  No idea */
    
    pPrev = p;
    lPrev = l;

    StaveCursorDrawX(pPrev, lPrev);
    xDrawn = True;

  } else {

    pPrev = p;
    if (buttonPressed && startedDragging) StaveCursorExtend(stave, p);
  }

  End;
}


static int staveClickCount = 0;
static XtIntervalId staveTimeoutId = 0;

static void StaveClearMultiClickCallback(XtPointer p, XtIntervalId *id)
{
  staveClickCount = 0;
  staveTimeoutId = 0;
}

void StaveButtonPressCallback(Widget w, XtPointer client,
			      XEvent *e, Boolean *cont)
{
  XButtonEvent  *event = (XButtonEvent *)e;
  XPoint         p;

  Begin("StaveButtonPressCallback");
  if (event->type != ButtonPress || !stave) End;
  if (event->button != Button1) End;
  if (slaveMode) End;

  p.x = event->x;
  p.y = event->y;
  pPrev = p;

  buttonPressed = True;

  if (staveInsertMode) {

    StaveEditInsertInsertion(p);

  } else {
    if (shiftPressed) {
      startedDragging = False;
      StaveCursorExplicitExtend(stave, p);
    } else {

      ++staveClickCount;

      switch(staveClickCount) {

      case 1:
	startedDragging = True;
	StaveCursorMark(stave, p);
	break;

      case 2:
	startedDragging = False;
	StaveCursorSelectBar(stave, p, True);
	break;

      case 3:
	startedDragging = False;
	StaveCursorSelectStaff(stave, p, True);
	break;

      default:
	break;
      }

      if (staveTimeoutId) XtRemoveTimeOut(staveTimeoutId);
      staveTimeoutId = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
				       XtGetMultiClickTime(XtDisplay(w)),
				       StaveClearMultiClickCallback, 0);
    }
  }

  End;
}


void StaveButtonReleaseCallback(Widget w, XtPointer client,
				XEvent *e, Boolean *cont)
{
  XButtonEvent *event = (XButtonEvent *)e;
  XPoint        p;

  Begin("StaveButtonReleaseCallback");

  if (event->type != ButtonRelease || !stave) End;
  if (slaveMode) End;

  if (event->button == Button3) {
    PaletteChangeMode(!staveInsertMode);
    End;
  } else if (event->button != Button1) End;

  buttonPressed = False;

  if (staveInsertMode || !startedDragging) End;
  startedDragging = False;

  p.x = event->x;
  p.y = event->y;

  StaveCursorExtend(stave, p);
  StaveCursorFinish(stave);

  End;
}


static void UpDownCallback(Boolean up)
{
  Begin("UpDownCallback");

  if (staveInsertMode) {

    if (up) PaletteMoveUp();
    else    PaletteMoveDown();

  } else {

    MajorStaveRec *mstave = (MajorStaveRec *)stave;
    ItemList a = mstave->sweep.from.left;
    ItemList b = mstave->sweep.to.left;
    int staveNo = mstave->sweep.stave;

    if (mstave->sweep.swept && (a != b)) {

      UndoAssertPreviousContents(up ? "Raise Notes" : "Lower Notes", stave,
				 staveNo, a, b);
      ItemListTranspose
	(a ? iNext(a) : mstave->music[staveNo], b, up ? 1 : -1);

      UndoAssertNewContents(up ? "Raise Notes" : "Lower Notes", stave,
			    staveNo, a, b);

      FileMenuMarkChanged(stave, True);
      StaveRefreshAsDisplayed(stave);
      StaveCursorSelectSublist(stave, staveNo, a, b);

      staveMoved = True;
      staveChanged = False;	/* gross */

    } else StaveScrollUpOrDownABit(up);
  }

  End;
}


static Boolean waitForRelease = False;

/* this function is a right mess. "client" is 1 if this is a key
   release rather than a key press */

void StaveKeyPressCallback(Widget w, XtPointer client,
			   XEvent *e, Boolean *cont)
{
  XKeyEvent *event = (XKeyEvent *)e;
  KeySym key;

  Begin("StaveKeyPressCallback");

  if (slaveMode) End;
  key = XKeycodeToKeysym(XtDisplay(w), event->keycode, 0);

  switch(key) {

#ifdef NOT_DEFINED
  case XK_a:
    fprintf(stderr, "A pressed\n");
    if (waitForRelease && client) waitForRelease = False;
    else if (stave && !waitForRelease) {
      PaletteSelectNote(Semiquaver);
      StaveEditInsertPaletteItem(/* staveNo */ 0,
				 /* left */ NULL, /* end */ True,
				 /* usePitch */ True, /* pitch */ 4,
				 /* useMods */ True, /* mods */ ModFlat,
				 /* p */ NULL);
      waitForRelease = True;
    }
    break;
#endif

  case XK_Left:
    if (!client && !buttonPressed) StaveLeftCallback(NULL, NULL, NULL);
    break;

  case XK_Right:
    if (!client && !buttonPressed) StaveRightCallback(NULL, NULL, NULL);
    break;
    
  case XK_Up:
    if (client) break;
    UpDownCallback(True);
    break;

  case XK_Down:
    if (client) break;
    UpDownCallback(False);
    break;

  case XK_Shift_L: case XK_Shift_R:
    if (!staveInsertMode) { shiftPressed = !client; break; }
    if (client && shiftPressed) PalettePopDot();
    else if (!client && !shiftPressed) PalettePushDot(True);
    shiftPressed = !client;
    break;

  case XK_Control_L:
    if (!staveInsertMode) { ctrlPressed = !client; break; }
    if (client && ctrlPressed) PalettePopSharp();
    else if (!ctrlPressed) PalettePushSharp(True);
    ctrlPressed = !client;
    break;

  case XK_Alt_R:
    if (!staveInsertMode) { rCtrlPressed = !client; break; }
    if (client && rCtrlPressed) PalettePopNatural();
    else if (!rCtrlPressed) PalettePushNatural(True);
    rCtrlPressed = !client;
    break;

  case XK_Alt_L:
    if (!staveInsertMode) { altPressed = !client; break; }
    if (client && altPressed) PalettePopFlat();
    else if (!altPressed) PalettePushFlat(True);
    altPressed = !client;
    break;

  default: break;
  }

  End;
}

/* }}} */
/* {{{ Exposure */

void StaveExposedCallback(Widget w, XtPointer client,
			  XEvent *e, Boolean *cont)
{
  static Dimension staveLabelWidth = 0;
  Dimension        thisWidth;

  Begin("StaveExposedCallback");

  xDrawn = False;

  if (stave && staveLabel) {

    YGetValue(staveLabel, XtNwidth, &thisWidth);

    if (thisWidth != staveLabelWidth) StaveRefreshAsDisplayed(stave);
    else StaveCursorExpose(stave);

    staveLabelWidth = thisWidth;
  }

  End;
}

/* }}} */
/* {{{ Modes, visuals, busy-ness */

void StaveEditAssertInsertVisual(PaletteTag type, MusicObject visual, int n)
{
  Begin("StaveEditAssertInsertVisual");

  staveVisualType  = type;
  staveVisual      = visual;
  staveVisualIndex = n;

  End;
}


void StaveEditEnterInsertMode(void)
{
  Begin("StaveEditEnterInsertMode");

  if (staveLabel) XDefineCursor(display, XtWindow(staveLabel), insertCursor);
  staveInsertMode = True;

  End;
}


void StaveEditEnterEditMode(void)
{
  Begin("StaveEditEnterEditMode");

  if (staveLabel) XDefineCursor(display, XtWindow(staveLabel), dragCursor);
  staveInsertMode = False;

  End;
}


void StaveBusy(Boolean busy)
{
  Begin("StaveBusy");

  if (!staveLabel) End;

  if (busy) {

    XDefineCursor(display, XtWindow(staveLabel), hourglassCursor);
    XSync(display, False);

  } else {

    XDefineCursor(display, XtWindow(staveLabel),
		  staveInsertMode ? insertCursor : dragCursor);
  }
}


static void StaveBusyDoCount(int count, Boolean total)
{
  static int totalCount = 7;

  Begin("StaveBusyDoCount");

  if (total) totalCount = count;
  else {

    if (count > totalCount) count = totalCount;

    XDefineCursor(display, XtWindow(staveLabel),
		  hourCursor[7 * count / totalCount]);

    XSync(display, False);
  }

  End;
}


/* Start the animated cursor.  Pass `count,' the total */
/* number of frames.  Must be at least 1.  Does not    */
/* install a cursor; you must call StaveBusyMakeCount  */
/* as well.                                            */

void StaveBusyStartCount(int count)
{
  Begin("StaveBusyStartCount");
  StaveBusyDoCount(count - 1, True);
  End;
}


/* Install the `count'th image in the animation sequence.  */
/* The total number of images is the same as the number    */
/* passed to StaveBusyStartCount originally.  (If that     */
/* number was greater than 8, multiple virtual images will */
/* mapped to each actual screen cursor image.)             */

void StaveBusyMakeCount(int count)
{
  Begin("StaveBusyMakeCount");
  StaveBusyDoCount(count, False);
  End;
}


/* Finish the animation and restore the mode cursor. */

void StaveBusyFinishCount(void)
{
  Begin("StaveBusyFinishCount");
  StaveBusy(False);
  End;
}

/* }}} */
/* {{{ Initialisation and clean-up */

/* We don't really need the argument here; sLabel is actually the same
   as the global staveLabel anyway */

void StaveEditInitialise(Widget sLabel)
{
  XColor screenFg, screenBg;
  XColor exact;
  Window w;
  int    i;

  Begin("StaveEditInitialise");

  if (!display) End;

  w = RootWindowOfScreen(XtScreen(topLevel));

  /* Yeuch: */

  XLookupColor(display,
	       DefaultColormapOfScreen(XtScreen(topLevel)),
	       "black", &exact, &screenFg);

  XLookupColor(display,
	       DefaultColormapOfScreen(XtScreen(topLevel)),
	       "white", &exact, &screenBg);

  dragPixmap = XCreateBitmapFromData
    (display, w, drag_bits, drag_width, drag_height);

  dragMask  = XCreateBitmapFromData
    (display, w, drag_mask_bits, drag_mask_width, drag_mask_height);

  dragCursor = XCreatePixmapCursor
    (display, dragPixmap, dragMask, &screenFg, &screenBg,
     drag_x_hot, drag_y_hot);

  hourglassPixmap = XCreateBitmapFromData
    (display, w, hourglass_bits, hourglass_width, hourglass_height);

  hourglassMask   = XCreateBitmapFromData
    (display, w, hourglass_mask_bits, hourglass_width, hourglass_height);

  hourglassCursor = XCreatePixmapCursor
    (display, hourglassPixmap, hourglassMask, &screenFg, &screenBg,
     hourglass_x_hot, hourglass_y_hot);

  insertCursor = XCreateFontCursor(display, XC_crosshair);

  for (i = 0; i < HOUR_FRAMES; ++i) {

    hourPixmap[i] = XCreateBitmapFromData
      (display, w,
       i == 0 ? hour0_bits :
       i == 1 ? hour1_bits :
       i == 2 ? hour2_bits :
       i == 3 ? hour3_bits :
       i == 4 ? hour4_bits :
       i == 5 ? hour5_bits :
       i == 6 ? hour6_bits : hour7_bits, hourglass_width, hourglass_height);

    hourCursor[i] = XCreatePixmapCursor
      (display, hourPixmap[i], hourglassMask, &screenFg, &screenBg,
       hourglass_x_hot, hourglass_y_hot);
  }

  YSetValue(sLabel, XtNcursor, dragCursor);

  XtAddEventHandler(sLabel, ButtonPressMask, False,
		    StaveButtonPressCallback, NULL);

  XtAddEventHandler(sLabel, ButtonReleaseMask, False,
		    StaveButtonReleaseCallback, NULL);

  XtAddEventHandler(sLabel, KeyPressMask, False,
		    StaveKeyPressCallback, NULL);

  XtAddEventHandler(sLabel, KeyReleaseMask, False,
		    StaveKeyPressCallback, (XtPointer)1); /* "released" arg */

  XtAddEventHandler(sLabel, PointerMotionMask, False,
		    StavePointerMovedCallback, NULL);

  XtAddEventHandler(sLabel, ExposureMask, False,
		    StaveExposedCallback, NULL);

  End;
}


void StaveEditCleanUp(void)
{
  int i;

  Begin("StaveEditCleanUp");

  if (!display) End;

  for (i = 0; i < HOUR_FRAMES; ++i) {
    if (hourPixmap[i]) XFreePixmap(display, hourPixmap[i]);
    hourPixmap[i] = 0;
  }

  if (hourglassPixmap) XFreePixmap(display, hourglassPixmap);
  if (hourglassMask)   XFreePixmap(display, hourglassMask);
  hourglassPixmap = hourglassMask = 0;

  if (dragPixmap) XFreePixmap(display, dragPixmap);
  if (dragMask)   XFreePixmap(display, dragMask);
  dragPixmap = dragMask = 0;

  End;
}

/* }}} */

