
/* MenuBar.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Functions to handle actions from Bar menu options */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "ItemList.h"
#include "GC.h"
#include "Yawn.h"

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Repeater.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

/* }}} */
/* {{{ Time Signature */

static void TsNumUp(Widget w, XtPointer client, XtPointer call)
{
  int           i;
  String        s1;
  static String s = 0;
  Widget        ww = (Widget)client;
  
  Begin("TsNumUp");

  if (!s) s = XtNewString("12345");
  YGetValue(ww, XtNlabel, &s1);

  i = atoi(s1);
  if (i < 99) ++i;
  sprintf(s, "%3d ", i);

  YSetValue(ww, XtNlabel, s);
  End;
}
 

static void TsNumDown(Widget w, XtPointer client, XtPointer call)
{
  int           i;
  String        s1;
  static String s = 0;
  Widget        ww = (Widget)client;
  
  Begin("TsNumDown");

  if (!s) s = XtNewString("12345");
  YGetValue(ww, XtNlabel, &s1);

  i = atoi(s1);
  if (i > 1) --i;
  sprintf(s, "%3d ", i);

  YSetValue(ww, XtNlabel, s);
  End;
}

 
static void TsDenUp(Widget w, XtPointer client, XtPointer call)
{
  int           i;
  String        s1;
  static String s = 0;
  Widget        ww = (Widget)client;
  
  Begin("TsDenUp");

  if (!s) s = XtNewString("12345");
  YGetValue(ww, XtNlabel, &s1);

  i = atoi(s1);
  if (i < 64) i *= 2;
  sprintf(s, "%3d ", i);

  YSetValue(ww, XtNlabel, s);
  End;
}

 
static void TsDenDown(Widget w, XtPointer client, XtPointer call)
{
  int           i;
  String        s1;
  static String s = 0;
  Widget        ww = (Widget)client;
  
  Begin("TsDenDown");

  if (!s) s = XtNewString("12345");
  YGetValue(ww, XtNlabel, &s1);

  i = atoi(s1);
  if (i > 1) i /= 2;
  sprintf(s, "%3d ", i);

  YSetValue(ww, XtNlabel, s);
  End;
}


static void TsCallback(Widget w, XtPointer client, XtPointer call)
{
  Begin("TsCallback");

  *((Widget *)client) = w;

  End;
}
 


void BarMenuTimeSignature(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            staveNo = mstave->sweep.stave;
  int            firstStave;
  int            lastStave;
  int            barNo;
  StaveEltList   barList;
  int            i;
  String         temp;
  String         numl;
  String         denl;
  static int     num = 4;
  static int     den = 4;

  Widget         tsShell;
  Widget         tsPane;
  Widget         tsTopPane;
  Widget         tsTopBox;
  Widget         tsBox[3];
  Widget         tsNumUp;
  Widget         tsNumDown;
  Widget         tsNumDisplay;
  Widget         tsDenUp;
  Widget         tsDenDown;
  Widget         tsDenDisplay;
  Widget         tsApply;
  Widget         tsCancel;
  Widget         tsHelp;

  XPoint         op;
  XtAppContext   context;
  Widget         value = NULL;

  Begin("BarMenuTimeSignature");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  tsShell = XtCreatePopupShell("Time Signature", transientShellWidgetClass,
			       topLevel, NULL, 0);

  tsPane = YCreateWidget("Time Signature Pane", panedWidgetClass, tsShell);
  
  tsTopBox = XtCreateManagedWidget
    ("Time Signature Top Box", boxWidgetClass, tsPane, NULL, 0);
  YSetValue(tsTopBox, XtNhSpace, 30);

  tsTopPane = XtCreateManagedWidget
    ("Time Signature Top Pane", panedWidgetClass, tsTopBox, NULL, 0);
  
  for (i = 0; i < 3; ++i)
    tsBox[i] = YCreateShadedWidget
      ("Time Signature Box", formWidgetClass, i == 2 ? tsPane : tsTopPane,
       i == 2 ? MediumShade : LightShade);

  tsNumDown = YCreateArrowButton("Numerator Down", tsBox[0], YArrowLeft);
  tsNumDisplay = YCreateLabel("   0", tsBox[0]);
  tsNumUp = YCreateArrowButton("Numerator Up", tsBox[0], YArrowRight);

  YSetValue(XtParent(tsNumDisplay), XtNfromHoriz, XtParent(tsNumDown));
  YSetValue(XtParent(tsNumUp),      XtNfromHoriz, XtParent(tsNumDisplay));

  tsDenDown = YCreateArrowButton("Denominator Down", tsBox[1], YArrowLeft);
  tsDenDisplay = YCreateLabel("   1", tsBox[1]);
  tsDenUp = YCreateArrowButton("Denominator Up", tsBox[1], YArrowRight);

  YSetValue(XtParent(tsDenDisplay), XtNfromHoriz, XtParent(tsDenDown));
  YSetValue(XtParent(tsDenUp),      XtNfromHoriz, XtParent(tsDenDisplay));

  tsApply  = YCreateCommand("OK",     tsBox[2]);
  tsCancel = YCreateCommand("Cancel", tsBox[2]);

  if (appData.interlockWindow) {
    tsHelp = YCreateCommand("Help", tsBox[2]);
    YSetValue(XtParent(tsHelp), XtNfromHoriz, XtParent(tsCancel));
    XtAddCallback(tsHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Bar - Time Signature");
  } else {
    tsHelp = NULL;
  }

  YSetValue(XtParent(tsCancel), XtNfromHoriz, XtParent(tsApply));
  YSetValue(XtParent(tsCancel), XtNfromHoriz, XtParent(tsApply));
  YSetValue(XtParent(tsCancel), XtNfromHoriz, XtParent(tsApply));

  XtAddCallback(tsNumUp,     XtNcallback, TsNumUp,    (XtPointer)tsNumDisplay);
  XtAddCallback(tsNumDown,   XtNcallback, TsNumDown,  (XtPointer)tsNumDisplay);
  XtAddCallback(tsDenUp,     XtNcallback, TsDenUp,    (XtPointer)tsDenDisplay);
  XtAddCallback(tsDenDown,   XtNcallback, TsDenDown,  (XtPointer)tsDenDisplay);
  XtAddCallback(tsApply,     XtNcallback, TsCallback, (XtPointer)&value);
  XtAddCallback(tsCancel,    XtNcallback, TsCallback, (XtPointer)&value);

  op = YPlacePopupAndWarp(tsShell, XtGrabExclusive, tsCancel, tsCancel);
  YAssertDialogueActions(tsShell, tsCancel, tsCancel, NULL);

  numl = (String)XtMalloc(5);
  sprintf(numl, "%3d ", num);
  YSetValue(tsNumDisplay, XtNlabel, numl);

  denl = (String)XtMalloc(5);
  sprintf(denl, "%3d ", den);
  YSetValue(tsDenDisplay, XtNlabel, denl);

  context = XtWidgetToApplicationContext(tsShell);
  while (!value || XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);

  YGetValue(tsNumDisplay, XtNlabel, &temp);
  num = atoi(temp);

  YGetValue(tsDenDisplay, XtNlabel, &temp);
  den = atoi(temp);

  XtFree(numl);
  XtFree(denl);

  if (value != tsApply) {
    
    YPopPointerPosition();
    YPopdown(tsShell);
    XtDestroyWidget(tsShell);
    End;
  }

  firstStave = lastStave = staveNo;

  if (mstave->staves > 1 &&
      YQuery(topLevel, NULL, 2, 1, -1, "Change this staff only",
	     "Change whole stave", "Bar - Time Signature") == 1) {

    firstStave = 0;
    lastStave = mstave->staves - 1;
  }

  YPopPointerPosition();
  YPopdown(tsShell);
  YRetractDialogueActions(tsShell);
  XtDestroyWidget(tsShell);

  for (staveNo = firstStave; staveNo <= lastStave; ++staveNo) {

    StaveEltList subList = barList;
    TimeSignature iSig;

    /* note check bar existence, not bar validity */

    if (!barList->bars[staveNo]) continue;

    (void)NewTimeSignature(&iSig, barList->bars[staveNo]->bar.time.numerator,
			   barList->bars[staveNo]->bar.time.denominator);

    while (subList && subList->bars[staveNo] &&
	   subList->bars[staveNo]->bar.time.numerator == iSig.numerator &&
	   subList->bars[staveNo]->bar.time.denominator == iSig.denominator) {

      (void)NewTimeSignature(&subList->bars[staveNo]->bar.time, num, den);
      subList = (StaveEltList)Next(subList);
    }
    
    StaveResetFormatting(stave, staveNo);
  }

  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Metronome and Remove Metronome */

static Widget    metNoteDisplay;
static Dimension metNoteMapWidth;

static void MetNoteChange(Chord *c, Boolean up)
{
  Pixmap  map;
  NoteTag tag;
  Boolean dotted;
  static  XExposeEvent event =
    { Expose, 0L, True, 0, 0,
	0, 0, NoteWidth*5, StaveHeight + NoteHeight, 0 };

  Begin("MetNoteChange");

  tag    = c->chord.visual->type;
  dotted = c->chord.visual->dotted;

  if (up)
    if (dotted) {
      if (tag < LongestNote) { ++tag; dotted = False; }
    } else dotted = True;
  else
    if (dotted) dotted = False;
    else if (tag > ShortestNote) { --tag; dotted = True; }

  (void)NewChord(c, c->chord.voices, c->chord.voice_count,
		 c->chord.modifiers, tag, dotted);

  YGetValue(metNoteDisplay, XtNbitmap, &map);

  XFillRectangle(display, map, clearGC, 0, 0,
		 metNoteMapWidth, StaveHeight + NoteHeight);

  c->methods->draw((MusicObject)c, map,
			metNoteMapWidth/2 - NoteWidth, 0, 0, 0, NULL);

  event.display = display;
  event.window  = XtWindow(metNoteDisplay);

  XSendEvent(display, XtWindow(metNoteDisplay),
	     False, ExposureMask, (XEvent *)&event);

  End;
}


static void MetNoteUp(Widget w, XtPointer client, XtPointer call)
{
  Begin("MetNoteUp");

  MetNoteChange((Chord *)client, True);

  End;
}


static void MetNoteDown(Widget w, XtPointer client, XtPointer call)
{
  Begin("MetNoteDown");

  MetNoteChange((Chord *)client, False);

  End;
}

 
static void MetBeatUp(Widget w, XtPointer client, XtPointer call)
{
  int           i;
  String        s1;
  static String s = 0;
  Widget        ww = (Widget)client;
  
  Begin("MetBeatUp");

  if (!s) s = XtNewString("123456");
  YGetValue(ww, XtNlabel, &s1);

  i = atoi(s1);
  if (i < 999) ++i;
  sprintf(s, "%3d  ", i);

  YSetValue(ww, XtNlabel, s);
  End;
}

 
static void MetBeatDown(Widget w, XtPointer client, XtPointer call)
{
  int           i;
  String        s1;
  static String s = 0;
  Widget        ww = (Widget)client;
  
  Begin("MetBeatDown");

  if (!s) s = XtNewString("123456");
  YGetValue(ww, XtNlabel, &s1);

  i = atoi(s1);
  if (i > 1) --i;
  sprintf(s, "%3d  ", i);

  YSetValue(ww, XtNlabel, s);
  End;
}


static void MetCallback(Widget w, XtPointer client, XtPointer call)
{
  Begin("MetCallback");

  *((Widget *)client) = w;

  End;
}
 


void BarMenuMetronome(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec  * mstave  = (MajorStaveRec *)stave;
  int              barNo;
  ItemList         list;
  ItemList         newList;
  StaveEltList     barList;
  static NoteVoice voice;
  static Chord     chord;
  static Boolean   haveChord = False;
  String           temp;
  String           beatl;
  static int       beat = 95;
  int              i;
  Pixmap           noteMap;

  Widget           metShell;
  Widget           metPane;
  Widget           metTopPane;
  Widget           metTopBox;
  Widget           metBox[3];
  Widget           metNoteUp;
  Widget           metNoteDown;
  Widget           metBeatUp;
  Widget           metBeatDisplay;
  Widget           metBeatDown;
  Widget           metApply;
  Widget           metCancel;
  Widget           metHelp;

  XPoint           op;
  XtAppContext     context;
  Widget           value = NULL;

  Begin("BarMenuMetronome");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  if (!haveChord) {

    (void)NewNoteVoice(&voice, 0, ModNone);
    (void)NewChord(&chord, &voice, 1, ModNone, Crotchet, False);
    haveChord = True;
  }

  metShell = XtCreatePopupShell("Metronome", transientShellWidgetClass,
				topLevel, NULL, 0);

  metPane = YCreateWidget("Metronome Pane", panedWidgetClass, metShell);
  
  metTopBox = XtCreateManagedWidget
    ("Metronome Top Box", boxWidgetClass, metPane, NULL, 0);
  YSetValue(metTopBox, XtNhSpace, 30);

  metTopPane = XtCreateManagedWidget
    ("Metronome Top Pane", panedWidgetClass, metTopBox, NULL, 0);

  for (i = 0; i < 3; ++i)
    metBox[i] = YCreateShadedWidget
      ("Metronome Box", formWidgetClass, i == 2 ? metPane : metTopPane,
       i == 2 ? MediumShade : LightShade);

  metNoteDown = YCreateArrowButton("Note Down", metBox[0], YArrowLeft);
  metNoteDisplay = YCreateLabel(" 00 ", metBox[0]);
  metNoteUp   = YCreateArrowButton("Note Up",   metBox[0], YArrowRight);

  YSetValue(XtParent(metNoteDisplay), XtNfromHoriz, XtParent(metNoteDown));
  YSetValue(XtParent(metNoteUp),      XtNfromHoriz, XtParent(metNoteDisplay));

  metBeatDown = YCreateArrowButton("Beat Down", metBox[1], YArrowLeft);
  metBeatDisplay = YCreateLabel(" 00 ", metBox[1]);
  metBeatUp   = YCreateArrowButton("Beat Up",   metBox[1], YArrowRight);

  YSetValue(XtParent(metBeatDisplay), XtNfromHoriz, XtParent(metBeatDown));
  YSetValue(XtParent(metBeatUp),      XtNfromHoriz, XtParent(metBeatDisplay));

  metApply  = YCreateCommand("OK",     metBox[2]);
  metCancel = YCreateCommand("Cancel", metBox[2]);

  if (appData.interlockWindow) {
    metHelp = YCreateCommand("Help", metBox[2]);
    YSetValue(XtParent(metHelp), XtNfromHoriz, XtParent(metCancel));
    XtAddCallback(metHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Bar - Metronome");
  } else {
    metHelp = NULL;
  }

  YSetValue(XtParent(metCancel), XtNfromHoriz, XtParent(metApply));
  /*
  Did I really write this _three times_?  What drugs was I on?

  YSetValue(XtParent(metCancel), XtNfromHoriz, XtParent(metApply));
  YSetValue(XtParent(metCancel), XtNfromHoriz, XtParent(metApply));
  */

  XtAddCallback(metBeatUp,    XtNcallback, MetBeatUp,
		(XtPointer)metBeatDisplay);
  XtAddCallback(metBeatDown,  XtNcallback, MetBeatDown,
		(XtPointer)metBeatDisplay);

  XtAddCallback(metNoteUp,    XtNcallback, MetNoteUp,   (XtPointer)&chord);
  XtAddCallback(metNoteDown,  XtNcallback, MetNoteDown, (XtPointer)&chord);
  XtAddCallback(metApply,     XtNcallback, MetCallback, (XtPointer)&value);
  XtAddCallback(metCancel,    XtNcallback, MetCallback, (XtPointer)&value);

  YSetValue(metNoteDisplay, XtNheight, StaveHeight + NoteHeight + 10);
  XtRealizeWidget(metShell);
  YGetValue(metBeatDisplay, XtNwidth, &metNoteMapWidth);
  XtUnrealizeWidget(metShell);

  noteMap = XCreatePixmap(display, RootWindowOfScreen(XtScreen(topLevel)),
			  metNoteMapWidth, StaveHeight + NoteHeight,
			  DefaultDepthOfScreen(XtScreen(topLevel)));

  XFillRectangle(display, noteMap, clearGC, 0, 0,
		 metNoteMapWidth, StaveHeight + NoteHeight);

  chord.methods->draw((MusicObject)&chord, noteMap,
			   metNoteMapWidth/2 - NoteWidth, 0, 0, 0, NULL);

  YSetValue(metNoteDisplay, XtNbitmap,                       noteMap);
  YSetValue(metNoteDisplay, XtNwidth,                metNoteMapWidth);

  op = YPlacePopupAndWarp(metShell, XtGrabExclusive, metCancel, metCancel);
  YAssertDialogueActions(metShell, metCancel, metCancel, NULL);

  beatl = (String)XtMalloc(7);
  sprintf(beatl, "%3d  ", beat);
  YSetValue(metBeatDisplay, XtNlabel, beatl);

  context = XtWidgetToApplicationContext(metShell);
  while (!value || XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);

  YPopPointerPosition();
  YPopdown(metShell);
  YRetractDialogueActions(metShell);
  XtDestroyWidget(metShell);
  XFreePixmap(display, noteMap);

  YGetValue(metBeatDisplay, XtNlabel, &temp);
  beat = atoi(temp);
  XtFree(beatl);

  if (value != metApply) End;

  if (!barList || !BarValid(barList->bars[0]) ||
      !barList->bars[0]->group.start) {
    IssueMenuComplaint
      ("There is no bar in the top staff to place the metronome mark above.");
    End;
  }

  newList = NewItemList
    ((Item *)NewMetronome
     (NULL, chord.chord.visual->type, chord.chord.visual->dotted, beat));
  list = barList->bars[0]->group.start;

  /*  while (list && list->item->object_class == TimeSignatureClass)
    list = iNext(list);
    */

  if (!list) { XBell(display, 70); End; } /* can't cope */

  UndoAssertPreviousContents("Metronome", stave, 0, iPrev(list),
			     barList->bars[0]->group.end);
  Insert(newList, list);
  
  while (list && list != iNext(barList->bars[0]->group.end))
    if (list && list->item->object_class == MetronomeClass)
      list = (ItemList)Remove(list);
    else list = iNext(list);

  UndoAssertNewContents("Metronome", stave, 0, iPrev(newList),
			barList->bars[0]->group.end);

  if (barList->bars[0]->group.start == mstave->music[0])
    mstave->music[0] = barList->bars[0]->group.start = newList;
  else barList->bars[0]->group.start = newList;

  StaveResetFormatting(stave, 0);
  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}


void BarMenuRemoveMet(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            barNo;
  ItemList       list;
  StaveEltList   barList;
  Boolean        foundMet = False;
  ItemList       leftBound;

  Begin("BarMenuRemoveMet");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  if (!barList || !BarValid(barList->bars[0]) ||
      !barList->bars[0]->group.start) {
    IssueMenuComplaint
      ("There is no bar in the top staff in which a metronome mark could be.");
    End;
  }

  list = barList->bars[0]->group.start;
  leftBound = iPrev(list);
  UndoAssertPreviousContents("Remove Metronome", stave, 0, leftBound,
			     barList->bars[0]->group.end);
  
  while (list &&
	 (list->item->object_class == ClefClass ||
	  list->item->object_class == KeyClass))
    list = iNext(list);

  while (list && list != iNext(barList->bars[0]->group.end))
    if (list && list->item->object_class == MetronomeClass) {

      if (list == mstave->music[0]) {
	mstave->music[0] = iNext(list);
      }

      if (list == barList->bars[0]->group.start) {
	barList->bars[0]->group.start = iNext(list);
      }

      list = (ItemList)Remove(list);
      foundMet = True;
    } else list = iNext(list);

  UndoAssertNewContents("Remove Metronome", stave, 0, leftBound,
			barList->bars[0]->group.end);
  StaveResetFormatting(stave, 0);

  if (!foundMet) IssueMenuComplaint
    ("There doesn't seem to be a metronome mark there to remove.");

  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Bar Styles */

static void MixInBarStyle(BarTag *oldTag, BarTag tag, Boolean start)
{
  Begin("MixInBarStyle");

  switch(*oldTag) {

  case RepeatBothBar:
    switch(tag) {
    case NoFixedBar: case NoBarAtAll:
      *oldTag = start ? RepeatRightBar : RepeatLeftBar; break;
    case RepeatLeftBar: case RepeatRightBar: break;
    default: *oldTag = tag; break;
    }
    break;

  case RepeatLeftBar:
    switch(tag) {
    case NoFixedBar: case NoBarAtAll:
      if (start) *oldTag = tag; break;
    case RepeatRightBar: *oldTag = RepeatBothBar; break;
    default: *oldTag = tag; break;
    }
    break;

  case RepeatRightBar:
    switch(tag) {
    case NoFixedBar: case NoBarAtAll:
      if (!start) *oldTag = tag; break;
    case RepeatLeftBar: *oldTag = RepeatBothBar; break;
    default: *oldTag = tag; break;
    }
    break;

  default:
    *oldTag = tag;
    break;
  }

  End;
}


static void ChangeBarStyle(Boolean start, BarTag tag)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  StaveEltList   barList;
  int            staveNo;
  int            barNo;

  Begin("ChangeBarStyle");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  /* if at end of bar, use next bar instead if possible (more intuitive) */
  if (BarValid(barList->bars[mstave->sweep.stave]) &&
      mstave->sweep.from.left == barList->bars[mstave->sweep.stave]->group.end
      && BarValid(((StaveEltList)Next(barList))->bars[mstave->sweep.stave])) {
    barList = (StaveEltList)Next(barList);
    ++barNo;
  }

  if (!barList || !BarValid(barList->bars[mstave->sweep.stave])) {
    XBell(display, 70);
    End;
  }

  if (tag != NoFixedBar)
    for (staveNo = 0; staveNo < mstave->staves; ++staveNo)
      if (!barList->bars[staveNo]) {
	IssueMenuComplaint("There must be a bar here in every staff.");
	End;
      }

  if (barNo == 0 && start && tag == NoFixedBar) tag = DoubleBar;

  for (staveNo = 0; staveNo < mstave->staves; ++staveNo) {

    Bar *bar = barList->bars[staveNo];

    if (start) {
      if (iPrev(bar->group.start)) {
	MixInBarStyle(&(iPrev(bar->group.start)->item->item.bar_tag),tag,start);
      } else {
	MixInBarStyle(&(mstave->bar_tags[staveNo].precedes), tag, start);
      }
    } else {
      if (bar->group.end) {
	MixInBarStyle(&(bar->group.end->item->item.bar_tag), tag, start);
      } else {
	MixInBarStyle(&(mstave->bar_tags[staveNo].follows), tag, start);
      }
    }

    StaveResetFormatting(stave, staveNo);
  }

  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}


void BarMenuPlain(Widget w, XtPointer a, XtPointer b)
{
  Begin("BarMenuPlain");

  ChangeBarStyle(True,  NoFixedBar);
  ChangeBarStyle(False, NoFixedBar);

  End;
}


void BarMenuDoubleBar(Widget w, XtPointer a, XtPointer b)
{
  Begin("BarMenuDoubleBar");

  ChangeBarStyle(False, DoubleBar);

  End;
}


void BarMenuRepeatAtStart(Widget w, XtPointer a, XtPointer b)
{
  Begin("BarMenuRepeatAtStart");

  ChangeBarStyle(True, RepeatLeftBar);

  End;
}


void BarMenuRepeatAtEnd(Widget w, XtPointer a, XtPointer b)
{
  Begin("BarMenuRepeatAtEnd");

  ChangeBarStyle(False, RepeatRightBar);

  End;
}


void BarMenuEndHere(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList ilist;
  Begin("BarMenuEndHere");

  if (!stave || !mstave->sweep.swept || !mstave->sweep.from.left ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  ilist = mstave->sweep.from.left;

  if (ilist->item->item.bar_tag != NoFixedBar &&
      ilist->item->item.bar_tag != NoBarAtAll) {

    YQuery(topLevel, "There already is a fixed barline here!", 1, 0, 0,
	   "OK", "Bar - End Bar Here");
    End;
  }

  ilist->item->item.bar_tag = PlainBar;

  StaveResetFormatting(stave, mstave->sweep.stave);
  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);

  End;
}


static void RemoveBarLines(ItemList start, ItemList end, /* inclusive */
			   Boolean askAboutFixed)
{
  ItemList i;
  Boolean nonFixedOnly = False;
  Begin("RemoveBarLines");

  for (i = start; i; i = (i == end) ? NULL : iNext(i)) {

    if (i->item->item.bar_tag != NoFixedBar &&
	i->item->item.bar_tag != NoBarAtAll) {
      if (askAboutFixed) {
	nonFixedOnly =
	  YQuery(topLevel, "Remove fixed and/or double barlines too?",
		 2, 0, 1, "Yes", "No", "Bar - Remove Barlines");
	askAboutFixed = False;
      }
      if (nonFixedOnly) continue;
    }

    i->item->item.bar_tag = NoBarAtAll;
  }

  End;
}


static void RefreshBarLines(ItemList start, ItemList end) /* both "left"s */
{
  ItemList i;
  Begin("RefreshBarLines");

  for (i = iNext(start); i; i = (i == end) ? NULL : iNext(i)) {
    if (i->item->item.bar_tag == NoBarAtAll) i->item->item.bar_tag = NoFixedBar;
  }

  End;
}


void BarMenuRemoveBarlines(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList start, end;
  Boolean whole;
  int i, staveNo;
  Begin("BarMenuRemoveBarlines");

  if (!stave) {
    XBell(display, 70);
    End;
  }

  staveNo = mstave->sweep.swept;

  if (!mstave->sweep.swept) {

    if (YQuery(topLevel, "Remove all barlines from the whole stave?",
	       2, 0, 1, "OK", "Cancel", "Bar - Remove Barlines") == 1) End;
    whole = True;

  } else {

    whole = False;
    start = mstave->sweep.from.left;
    end = mstave->sweep.to.left;

    if (start == end) {
      if (YQuery(topLevel, "Remove all barlines from here to end of staff?",
		 2, 0, 1, "OK", "Cancel", "Bar - Remove Barlines") == 1) End;
      end = NULL;
    } else {
      if (YQuery(topLevel, "Remove all barlines in the selected area?",
		 2, 0, 1, "OK", "Cancel", "Bar - Remove Barlines") == 1) End;
    }
  }    

  for (i = (whole? 0 : staveNo); i < (whole? mstave->staves : staveNo+1); ++i) {

    start = mstave->sweep.from.left;
    end = whole ? NULL : mstave->sweep.to.left;

    if (whole || !start) {
      start = (ItemList)First(mstave->music[i]);
    } else {
      start = (ItemList)Next(start);
    }

    RemoveBarLines(start, end, !whole || i > 0); /* not quite right! */
  }

  StaveResetFormatting(stave, mstave->sweep.stave);
  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);

  End;
}


void BarMenuRefreshBarlines(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList start, end;
  Boolean whole;
  int i, staveNo;
  Begin("BarMenuRemoveBarlines");

  if (!stave || !mstave->sweep.swept) {
    XBell(display, 70);
    End;
  }

  staveNo = mstave->sweep.stave;

  if (!mstave->sweep.swept) {

    if (YQuery(topLevel, "Refresh barlines throughout the whole stave?",
	       2, 0, 1, "OK", "Cancel", "Bar - Refresh Barlines") == 1) End;
    whole = True;

  } else {

    whole = False;
    start = mstave->sweep.from.left;
    end = mstave->sweep.to.left;

    if (start == end) {
      if (YQuery(topLevel, "Refresh barlines from here to end of staff?",
		 2, 0, 1, "OK", "Cancel", "Bar - Refresh Barlines") == 1) End;
      end = NULL;
    } else {
      if (YQuery(topLevel, "Refresh barlines in the selected area?",
		 2, 0, 1, "OK", "Cancel", "Bar - Refresh Barlines") == 1) End;
    }
  }

  for (i = (whole? 0 : staveNo); i < (whole? mstave->staves : staveNo+1); ++i) {

    start = mstave->sweep.from.left;
    end = whole ? NULL : mstave->sweep.to.left;

    if (whole || !start) {
      start = (ItemList)First(mstave->music[i]);
    } else {
      start = (ItemList)Next(start);
    }

    RefreshBarLines(start, end);
  }

  StaveResetFormatting(stave, mstave->sweep.stave);
  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);

  End;
}



/* }}} */
/* {{{ Key Signature */

static Widget    keyDisplay;
static Dimension keyMapWidth;
static Widget    keyMenuButton;
static Clef      keyClef;

NoteMods KeyModsForKey(Key *key, Pitch p)
{
  Pitch pitch;
  Boolean sharps;
  int number, n;

  Begin("KeyModsForKey");

  sharps = key->key.visual->sharps;
  number = key->key.visual->number;

  pitch = sharps ? 8 : 4;

  for (n = 0; n < number; ++n) {

    if ((pitch + 70) % 7 == (p + 70) % 7) {
      Return(sharps ? ModSharp : ModFlat);
    }

    if (sharps) { pitch -= 3; if (pitch < 3) pitch += 7; }
    else        { pitch += 3; if (pitch > 7) pitch -= 7; }
  }

  Return(ModNone);
}


void KeyRaisePitch(Chord *c, int n)
{
  NoteMods *m;
  Begin("KeyRaisePitch");

  m = &c->chord.voices[n].modifiers;

  if (*m == ModFlat) *m = ModNone;
  else if (*m == ModNone || *m == ModNatural) *m = ModSharp;
  else {
    *m = ModNone; c->chord.voices[n].pitch ++;
  }

  End;
}


void KeyLowerPitch(Chord *c, int n)
{
  NoteMods *m;
  Begin("KeyRaisePitch");

  m = &c->chord.voices[n].modifiers;

  if (*m == ModSharp) *m = ModNone;
  else if (*m == ModNone || *m == ModNatural) *m = ModFlat;
  else {
    *m = ModNone; c->chord.voices[n].pitch --;
  }

  End;
}


void KeyTransposeItemIntoNewKey(Item *item, Key *oldKey, Key *newKey)
{
  int n, p;
  Chord *c;
  NoteMods oldMods;
  NoteMods newMods;
  NoteMods *m;

  Begin("KeyTransposeItemIntoNewKey");
    
  if (item->object_class != ChordClass) End;
  c = (Chord *)item;

  for (n = 0; n < c->chord.voice_count; ++n) {
      
    p = c->chord.voices[n].pitch;

    oldMods = KeyModsForKey(oldKey, p);
    newMods = KeyModsForKey(newKey, p);
    m = &c->chord.voices[n].modifiers;

    switch (oldMods) {
    case ModSharp: KeyLowerPitch(c, n); break;
    case ModFlat:  KeyRaisePitch(c, n); break;
    default: break;
    }

    switch (newMods) {
    case ModFlat:  KeyLowerPitch(c, n); break;
    case ModSharp: KeyRaisePitch(c, n); break;
    default: break;
    }
  }

  End;
}


void KeyTransposeIntoNewKey(ItemList music, ItemList start,
			    Key *oldKey, Key *newKey)
{
  ItemList i;

  Begin("KeyTransposeIntoNewKey");

  if (start) {
    i = iNext(start);
    if (i->item->object_class == KeyClass) i = iNext(i);
  } else {
    i = music;
    if (i->item->object_class == KeyClass) i = iNext(i);
  }

  for ( ; i && i->item->object_class != KeyClass; i = iNext(i)) {
    KeyTransposeItemIntoNewKey(i->item, oldKey, newKey);
  }

  End;
}


static void KeyChange(Key *k, int newTag, Boolean up)
{
  Pixmap  map;
  int i, tag, sharps, y;
  static  XExposeEvent event =
    { Expose, 0L, True, 0, 0, 0, 0, 300, StaveHeight + 30, 0 };

  Begin("KeyChange");

  if (newTag >= 0) tag = newTag;
  else {

    sharps = k->key.visual->number;

    if (!k->key.visual->sharps) sharps = -sharps;
    if (!up) {			                      /* huh? */
      if (sharps <  7) ++sharps;
    } else {
      if (sharps > -7) --sharps;
    }
    
    for (i = 0; i < keyVisualCount; ++i) {
      if (sharps < 0) {
	if (!keyVisuals[i].sharps && keyVisuals[i].number == -sharps) {
	  tag = i; break;
	}
      } else {
	if ( keyVisuals[i].sharps && keyVisuals[i].number ==  sharps) {
	  tag = i; break;
	}
      }
    }
  }

  (void)NewKey(k, (KeyTag)tag);

  YSetCurrentOption(keyMenuButton, tag);

  YGetValue(keyDisplay, XtNbitmap, &map);

  XFillRectangle(display, map, clearGC, 0, 0, keyMapWidth, StaveHeight + 30);

  k->methods->draw(k, map, keyMapWidth/2 - 25, 15, 0, 0, NULL);
  keyClef.methods->draw(&keyClef, map, 20, 15, 0, 0, NULL);
  for (y = 0; y < StaveHeight; y += NoteHeight + 1)
    XDrawLine(display, map, drawingGC, 8, y + 15, keyMapWidth - 16, y + 15);
  
  event.display = display;
  event.window  = XtWindow(keyDisplay);

  XSendEvent(display, XtWindow(keyDisplay),
	     False, ExposureMask, (XEvent *)&event);

  End;
}


static void KeyUp(Widget w, XtPointer client, XtPointer call)
{
  Begin("KeyUp");

  KeyChange((Key *)client, -1, True);

  End;
}


static void KeyDown(Widget w, XtPointer client, XtPointer call)
{
  Begin("KeyDown");

  KeyChange((Key *)client, -1, False);

  End;
}


static void KeyCallback(Widget w, XtPointer client, XtPointer call)
{
  Begin("KeyCallback");

  *((Widget *)client) = w;

  End;
}


static void KeyMenuCallback(Widget w, XtPointer client, XtPointer call)
{
  int tag = (int)call;
  Begin("KeyMenuCallback");

  KeyChange((Key *)client, tag, False);

  End;
}


static Boolean keyTransposeP;

void KeyTransposeButtonCB(Widget w, XtPointer a, XtPointer b)
{
  keyTransposeP = !keyTransposeP;
}


void BarMenuKeySignature(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec  * mstave  = (MajorStaveRec *)stave;
  ItemList         start   = mstave->sweep.from.left;
  ItemList         end     = mstave->sweep.to.left;
  ItemList         newList;
  int              staveNo = mstave->sweep.stave;

  Key            * oldKey;
  Item           * item;
  static Key       key;
  int              i, j, k, y;
  Pixmap           keyMap;

  Widget           keyShell;
  Widget           keyPane;
  Widget           keyTopPane;
  Widget           keyTopBox;
  Widget           keyBox[4];
  Widget           keyUp;
  Widget           keyDown;
  Widget           keyApply;
  Widget           keyCancel;
  Widget           keyHelp;
  Widget           keyTranspose;

  String          *keyNames;

  XtAppContext     context;
  Widget           value = NULL;

  Begin("BarMenuKeySignature");

  if (!stave || !mstave->sweep.swept || start != end) {
    XBell(display, 70);
    End;
  }

  (void)NewKey(&key, KeyC);
  (void)NewClef(&keyClef, TrebleClef);

  keyShell = XtCreatePopupShell("Key Signature", transientShellWidgetClass,
				topLevel, NULL, 0);

  keyPane = YCreateWidget("Key Pane", panedWidgetClass, keyShell);
  
  keyTopBox = XtCreateManagedWidget
    ("Key Top Box", boxWidgetClass, keyPane, NULL, 0);
  YSetValue(keyTopBox, XtNhSpace, 10);

  keyTopPane = XtCreateManagedWidget
    ("Key Top Pane", panedWidgetClass, keyTopBox, NULL, 0);

  for (i = 0; i < 4; ++i)
    keyBox[i] = YCreateShadedWidget
      ("Key Box", formWidgetClass, i >= 2 ? keyPane : keyTopPane,
       i >= 2 ? MediumShade : LightShade);

  keyDown = YCreateArrowButton("Key Down", keyBox[0], YArrowUp);
  keyDisplay = YCreateLabel(" 00 ", keyBox[0]);
  keyUp = YCreateArrowButton("Key Up", keyBox[0], YArrowDown);

  YSetValue(XtParent(keyDisplay), XtNfromVert, XtParent(keyDown));
  YSetValue(XtParent(keyUp),      XtNfromVert, XtParent(keyDisplay));

  keyNames = (String *)XtMalloc(keyVisualCount * sizeof(String));

  for (i = 0; i < keyVisualCount; ++i) {

    String s = keyVisuals[i].name;

    keyNames[i] = (String)XtMalloc(strlen(s) + 6);
    for (j = k = 0; j < strlen(s); ++j) {

      /* exquisitely painful code */
      if (s[j] == ' ' && j > 0 && s[j-1] == ' ') {
	--k;
      } else if (s[j] == '/') {
	keyNames[i][j+k] = 'o'; keyNames[i][j+k+1] = 'r'; ++k;
      } else if (s[j] == 'j' || s[j] == 'n') {
	keyNames[i][j+k] = s[j]; keyNames[i][j+k+1] = 'o';
	keyNames[i][j+k+2] = 'r'; ++k; ++k;
      } else {
	keyNames[i][j+k] = s[j];
      }
    }

    keyNames[i][j+k] = '\0';
  }

  keyMenuButton =
    YCreateOptionMenu(keyBox[1], keyNames, keyVisualCount, (int)KeyC,
		      KeyMenuCallback, (XtPointer)&key);

  keyTransposeP = False;
  keyTranspose = YCreateToggle("Transpose rest of staff", keyBox[2],
			       KeyTransposeButtonCB);

  keyApply  = YCreateCommand("OK",     keyBox[3]);
  keyCancel = YCreateCommand("Cancel", keyBox[3]);

  if (appData.interlockWindow) {
    keyHelp = YCreateCommand("Help", keyBox[3]);
    YSetValue(XtParent(keyHelp), XtNfromHoriz, XtParent(keyCancel));
    XtAddCallback(keyHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Bar - Key Signature");
  } else {
    keyHelp = NULL;
  }

  YSetValue(XtParent(keyCancel), XtNfromHoriz, XtParent(keyApply));
  YSetValue(keyDisplay, XtNheight, StaveHeight + 40);

  XtAddCallback(keyUp,     XtNcallback, KeyUp,   (XtPointer)&key);
  XtAddCallback(keyDown,   XtNcallback, KeyDown, (XtPointer)&key);
  XtAddCallback(keyApply,  XtNcallback, KeyCallback, (XtPointer)&value);
  XtAddCallback(keyCancel, XtNcallback, KeyCallback, (XtPointer)&value);

  XtRealizeWidget(keyShell);
  YGetValue(keyMenuButton, XtNwidth, &keyMapWidth);
  XtUnrealizeWidget(keyShell);

  keyMap = XCreatePixmap(display, RootWindowOfScreen(XtScreen(topLevel)),
			 keyMapWidth, StaveHeight + 30,
			 DefaultDepthOfScreen(XtScreen(topLevel)));

  XFillRectangle(display, keyMap, clearGC, 0, 0, keyMapWidth, StaveHeight+30);

  key.methods->draw(&key, keyMap, keyMapWidth/2 - 25, 15, 0, 0, NULL);
  keyClef.methods->draw(&keyClef, keyMap, 20, 15, 0, 0, NULL);

  for (y = 0; y < StaveHeight; y += NoteHeight + 1)
    XDrawLine(display, keyMap, drawingGC, 8, y + 15, keyMapWidth - 16, y + 15);
  
  YSetValue(keyDisplay, XtNbitmap, keyMap);
  YSetValue(keyUp, XtNwidth, keyMapWidth);
  YSetValue(keyDisplay, XtNwidth, keyMapWidth);
  YSetValue(keyDown, XtNwidth, keyMapWidth);
  YSetValue(keyDisplay, XtNheight, StaveHeight + 40);

  YPlacePopupAndWarp(keyShell, XtGrabExclusive, keyCancel, keyCancel);
  YFixOptionMenuLabel(keyMenuButton);
  YAssertDialogueActions(keyShell, keyCancel, keyCancel, NULL);

  context = XtWidgetToApplicationContext(keyShell);
  while (!value || XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);

  YPopPointerPosition();
  YPopdown(keyShell);

  item = (Item *)NewKey(NULL, (KeyTag)YGetCurrentOption(keyMenuButton));

  YRetractDialogueActions(keyShell);
  YDestroyOptionMenu(keyMenuButton);
  XtDestroyWidget(keyShell);
  XFreePixmap(display, keyMap);

  for (i = 0; i < keyVisualCount; ++i) {
    XtFree((char *)keyNames[i]);
  }

  XtFree((char *)keyNames);
  if (value != keyApply) End;

  oldKey = StaveItemToKey(mstave, staveNo, start);

  if (keyTransposeP) {
    UndoAssertPreviousContents
      ("Key Signature", stave, staveNo, start,
       (ItemList)Last(start ? start : mstave->music[staveNo]));
  } else {
    UndoAssertPreviousContents("Key Signature", stave, staveNo, start, start);
  }

  newList = NewItemList(item);

  if (start)
    if (Next(start)) Insert(newList, Next(start));
    else Nconc(start, newList);
  else mstave->music[staveNo] =
    (ItemList)First(Nconc(newList, mstave->music[staveNo]));

  if (keyTransposeP) {
    KeyTransposeIntoNewKey(mstave->music[staveNo], start, oldKey, (Key *)item);
    UndoAssertNewContents
      ("Key Signature", stave, staveNo, start,
       (ItemList)Last(start ? start : mstave->music[staveNo]));
  } else {
    UndoAssertNewContents("Key Signature", stave, staveNo, start, newList);
  }

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);

  StaveRefreshAsDisplayed(stave);

  End;
}

/* }}} */

