
/* MenuChord.c */
/* Musical Notation Editor for X, Chris Cannam 1994-96 */
/* Actions available from the Chord menu               */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "Yawn.h"
#include "ItemList.h"
#include "GC.h"
#include "Marks.h"
#include "Undo.h"
#include "Draw.h"

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Command.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

char *Rechord(Chord);
Chord *SpellChord(char *, ClefTag, Boolean);

/* }}} */

/* {{{ Edit Chord */

static Chord     *cTempChord;
static NoteVoice *cNoteVoices;
static int        cNoteVoice;
static int        cTotalVoices;
static Clef      *cClef;
static Widget     changeHeadCount;
static Dimension  changeHeadCountWidth;


static void CDrawChord(Widget w, Dimension wd, Dimension ht)
{
  static Pixmap map = 0;
  Begin("CDrawChord");

  if (!map) {
    map = XCreatePixmap(XtDisplay(w), RootWindowOfScreen(XtScreen(w)),
			wd, ht, DefaultDepthOfScreen(XtScreen(w)));
  }

  DrawClefAndChordOnSimpleDrawable(cClef, cTempChord, map, wd, ht,
				   True, cNoteVoices[cNoteVoice].pitch, True);

  YSetValue(w, XtNbitmap, map);
  End;
}


static NoteVoice *ChangeDuplicateVoices(NoteVoice *voices, int voiceCount)
{
  int i;
  NoteVoice *rtn;
  Begin("ChangeDuplicateVoices");

  rtn = (NoteVoice *)XtMalloc(sizeof(NoteVoice) * voiceCount);
  for (i = 0; i < voiceCount; ++i) {
    (void)NewNoteVoice(rtn + i, voices[i].pitch, voices[i].modifiers);
  }

  Return(rtn);
}


static void ChangeUp(Widget w, XtPointer client, XtPointer call)
{
  Chord *tempChord;
  Begin("ChangeUp");

  if (!cNoteVoices[cNoteVoice].modifiers) {

    cNoteVoices[cNoteVoice].modifiers = ModSharp;

  } else if (cNoteVoices[cNoteVoice].modifiers == ModFlat) {

    cNoteVoices[cNoteVoice].modifiers = ModNone;

  } else {
    cNoteVoices[cNoteVoice].modifiers = ModFlat;
    cNoteVoices[cNoteVoice].pitch ++;

    if (cNoteVoices[cNoteVoice].pitch > highestNoteVoice.pitch)
	cNoteVoices[cNoteVoice].pitch = highestNoteVoice.pitch;
  }

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers,
     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}


static void ChangeDown(Widget w, XtPointer client, XtPointer call)
{
  Chord *tempChord;
  Begin("ChangeDown");

  if (!cNoteVoices[cNoteVoice].modifiers) {

    cNoteVoices[cNoteVoice].modifiers = ModFlat;

  } else if (cNoteVoices[cNoteVoice].modifiers == ModSharp){

    cNoteVoices[cNoteVoice].modifiers = ModNone;

  } else {
    cNoteVoices[cNoteVoice].modifiers = ModSharp;
    cNoteVoices[cNoteVoice].pitch --;

    if (cNoteVoices[cNoteVoice].pitch < lowestNoteVoice.pitch)
	cNoteVoices[cNoteVoice].pitch = lowestNoteVoice.pitch;
  }

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers,
     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}


static void ChangeOctDown(Widget w, XtPointer client, XtPointer call)
{
  Chord *tempChord;
  Begin("ChangeOctDown");

  cNoteVoices[cNoteVoice].pitch -= 7;

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers,
     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}

static void ChangeDelNote(Widget w, XtPointer client, XtPointer call)
{
  Chord *tempChord;
  int x;
  Begin("ChangeDelNote");

  if (cTotalVoices < 2) End;

  cTotalVoices --;
  for (x = cNoteVoice; x < cTotalVoices; x++)
    cNoteVoices[x] = cNoteVoices[x+1]; 

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers,
     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}

static void ChangeAddNote(Widget w, XtPointer client, XtPointer call)
{
  int i;
  int pitch;
  Chord *tempChord;
  Begin("ChangeAddNote");

  for (pitch = 0; pitch < 20; ++pitch) {

    for (i = 0; i < cTotalVoices; ++i) {
      if (cNoteVoices[i].pitch == pitch) break;
    }

    if (i == cTotalVoices) break;
  }

  if (pitch == 20) {
    XBell(display, 70);
    End;
  }

  cNoteVoices = (NoteVoice *)XtRealloc((void *)cNoteVoices,
				       (cTotalVoices + 1) * sizeof(NoteVoice));

  cNoteVoices[cTotalVoices].pitch = pitch;
  cNoteVoices[cTotalVoices].modifiers = 0;
  cTotalVoices ++;

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers,
     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}

static void ChangeOctUp(Widget w, XtPointer client, XtPointer call)
{
  Chord *tempChord;
  Begin("ChangeOctUp");

  cNoteVoices[cNoteVoice].pitch += 7;

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers,
     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted); 

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}


static void ChangeShorter(Widget w, XtPointer client, XtPointer call)
{
  NoteTag tag;
  Boolean dotted;
  Chord *tempChord;

  Begin("ChangeShorter");

  tag    = cTempChord->chord.visual->type;
  dotted = cTempChord->chord.visual->dotted;

  if (dotted) dotted = False;
  else if (tag > ShortestNote) { --tag; dotted = True; }

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers, tag, dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}


static void ChangeLonger(Widget w, XtPointer client, XtPointer call)
{
  NoteTag tag;
  Boolean dotted;
  Chord *tempChord;

  Begin("ChangeLonger");

  tag    = cTempChord->chord.visual->type;
  dotted = cTempChord->chord.visual->dotted;

  if (dotted) {
    if (tag < LongestNote) { ++tag; dotted = False; }
  } else dotted = True;

  tempChord = NewChord
    (NULL, ChangeDuplicateVoices(cNoteVoices, cTotalVoices),
     cTotalVoices, cTempChord->chord.modifiers, tag, dotted);

  DestroyChord(cTempChord);
  cTempChord = tempChord;

  *((Boolean *)client) = True;

  End;
}


static void ChangeHeadUp(Widget w, XtPointer client, XtPointer call)
{
  static String s = 0;
  Begin("ChangeHeadUp");

  if (cNoteVoice < cTotalVoices - 1) {
    cNoteVoice ++;
    *((Boolean *)client) = True;
  }

  if (!s) s = XtNewString("Head xxxx");
  sprintf(s, "Head %d", cNoteVoice + 1);

  YSetValue(changeHeadCount, XtNlabel, s);
  YSetValue(changeHeadCount, XtNwidth, changeHeadCountWidth);

  End;
}


static void ChangeHeadDown(Widget w, XtPointer client, XtPointer call)
{
  static String s = 0;
  Begin("ChangeHeadDown");

  if (cNoteVoice > 0) {
    cNoteVoice --;
    *((Boolean *)client) = True;
  }

  if (!s) s = XtNewString("Head xxxx");
  sprintf(s, "Head %d", cNoteVoice + 1);

  YSetValue(changeHeadCount, XtNlabel, s);
  YSetValue(changeHeadCount, XtNwidth, changeHeadCountWidth);

  End;
}


static void ChangeApply(Widget w, XtPointer client, XtPointer call)
{
  Begin("ChangeApply");

  *((int *)client) = 1;

  End;
}


static void ChangeCancel(Widget w, XtPointer client, XtPointer call)
{
  Begin("ChangeCancel");

  *((int *)client) = 0;

  End;
}


Boolean EditChord(Chord *chord, Clef *clef, String helpText) /* True = change */
{
  Widget changeShell;
  Widget changePane;
  Widget changeTopBox;
  Widget changeTopPane;
  Widget changeForm;
  Widget changeUpperBox;
  Widget changeLowerBox;
  Widget changeOctUp;
  Widget changeOctDown;
  Widget changeAddNote;
  Widget changeDelNote;
  Widget changeUp;
  Widget changeDown;
  Widget changeNote;
  Widget changeShorter;
  Widget changeLonger;
  Widget changeHeadUp;
  Widget changeHeadDown;
  Widget changeApply;
  Widget changeCancel;
  Widget changeHelp;

  Dimension wd, ht;

  XtAppContext context;
  Boolean madeChange;
  int result;

  Begin("EditChord");

  cTempChord = chord->methods->clone(chord);
  cTotalVoices = cTempChord->chord.voice_count;
  cNoteVoice = 0;
  cNoteVoices = ChangeDuplicateVoices
    (cTempChord->chord.voices,
     cTempChord->chord.voice_count);
  cClef = clef ? (Clef *)clef->methods->clone(clef) : 0;

  changeShell = XtCreatePopupShell
    ("Edit Chord", transientShellWidgetClass, topLevel, NULL, 0);

  changePane = YCreateWidget
    ("Change Chord Pane", panedWidgetClass, changeShell);

  changeTopBox = XtCreateManagedWidget
    ("Change Chord Top Box", boxWidgetClass, changePane, NULL, 0);
  YSetValue(changeTopBox, XtNhSpace, 8);

  changeTopPane = XtCreateManagedWidget
    ("Change Chord Top Pane", panedWidgetClass, changeTopBox, NULL, 0);
  
  changeForm = YCreateShadedWidget
    ("Change Chord Form", formWidgetClass, changeTopPane, LightShade);

  changeUpperBox = YCreateShadedWidget
    ("Change Chord Upper Box", formWidgetClass, changeTopPane, MediumShade);

  changeLowerBox = YCreateShadedWidget
    ("Change Chord Lower Box", formWidgetClass, changePane, LightShade);

  changeNote    = YCreateLabel(" 00 ", changeForm);
  changeUp      = YCreateArrowButton("Raise",   changeForm, YArrowUp);
  changeDown    = YCreateArrowButton("Lower",   changeForm, YArrowDown);
  changeShorter = YCreateArrowButton("Shorter", changeForm, YArrowLeft);
  changeLonger  = YCreateArrowButton("Longer",  changeForm, YArrowRight);

  changeAddNote = YCreateCommand("Add", changeForm);
  changeDelNote = YCreateCommand("Del", changeForm);
  changeOctUp   = YCreateCommand("+8ve", changeForm);
  changeOctDown = YCreateCommand("-8ve", changeForm);
  YSetValue(changeAddNote, XtNleftBitmap, 0);
  YSetValue(changeDelNote, XtNleftBitmap, 0);
  YSetValue(changeOctUp,   XtNleftBitmap, 0);
  YSetValue(changeOctDown, XtNleftBitmap, 0);
  
  YSetValue(XtParent(changeAddNote), XtNfromVert,  XtParent(changeUp));
  YSetValue(XtParent(changeDelNote), XtNfromVert,  XtParent(changeAddNote));
  YSetValue(XtParent(changeOctUp),   XtNfromHoriz, XtParent(changeNote));
  YSetValue(XtParent(changeOctUp),   XtNfromVert,  XtParent(changeUp));
  YSetValue(XtParent(changeOctDown), XtNfromHoriz, XtParent(changeNote));
  YSetValue(XtParent(changeOctDown), XtNfromVert,  XtParent(changeOctUp));
  YSetValue(XtParent(changeUp),      XtNfromHoriz, XtParent(changeShorter));
  YSetValue(XtParent(changeNote),    XtNfromVert,  XtParent(changeUp));
  YSetValue(XtParent(changeNote),    XtNfromHoriz, XtParent(changeShorter));
  YSetValue(XtParent(changeLonger),  XtNfromHoriz, XtParent(changeNote));
  YSetValue(XtParent(changeDown),    XtNfromVert,  XtParent(changeNote));
  YSetValue(XtParent(changeDown),    XtNfromHoriz, XtParent(changeShorter));

  ht = StaveHeight + StaveUpperGap + StaveLowerGap + 12;
  YSetValue(changeNote, XtNheight, ht);

  YSetValue(XtParent(changeShorter), XtNvertDistance, ht + 14);
  YSetValue(XtParent(changeLonger),  XtNvertDistance, ht + 14);

  changeHeadDown = YCreateArrowButton("Head Down", changeUpperBox, YArrowLeft);
  changeHeadCount = YCreateLabel("Head 1", changeUpperBox);
  changeHeadUp = YCreateArrowButton("Head Up", changeUpperBox, YArrowRight);

  YSetValue(XtParent(changeHeadCount), XtNfromHoriz, XtParent(changeHeadDown));
  YSetValue(XtParent(changeHeadUp), XtNfromHoriz, XtParent(changeHeadCount));

  YGetValue(changeHeadCount, XtNwidth, &wd);
  if (wd < 105) wd = 105;

  YSetValue(changeNote,      XtNwidth, wd);
  YSetValue(changeUp,        XtNwidth, wd);
  YSetValue(changeDown,      XtNwidth, wd);
  YSetValue(changeHeadCount, XtNwidth, wd);
  changeHeadCountWidth = wd;

  YSetValue(changeNote, XtNinternalWidth,  0);
  YSetValue(changeNote, XtNinternalHeight, 0);

  changeApply = YCreateCommand("OK", changeLowerBox);
  changeCancel = YCreateCommand("Cancel", changeLowerBox);

  YSetValue(XtParent(changeCancel), XtNfromHoriz, XtParent(changeApply));

  if (appData.interlockWindow) {
    changeHelp = YCreateCommand("Help", changeLowerBox);
    YSetValue(XtParent(changeHelp), XtNfromHoriz, XtParent(changeCancel));
    XtAddCallback(changeHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)helpText);
  } else {
    changeHelp = NULL;
  }

  XtAddCallback(changeUp,       XtNcallback,
		ChangeUp,       (XtPointer)&madeChange);
  XtAddCallback(changeDown,     XtNcallback,
		ChangeDown,     (XtPointer)&madeChange);
  XtAddCallback(changeOctUp,    XtNcallback,
		ChangeOctUp,    (XtPointer)&madeChange);
  XtAddCallback(changeOctDown,  XtNcallback,
		ChangeOctDown,  (XtPointer)&madeChange);
  XtAddCallback(changeAddNote,  XtNcallback,
		ChangeAddNote,  (XtPointer)&madeChange);
  XtAddCallback(changeDelNote,  XtNcallback,
		ChangeDelNote,  (XtPointer)&madeChange);
  XtAddCallback(changeShorter,  XtNcallback,
		ChangeShorter,  (XtPointer)&madeChange);
  XtAddCallback(changeLonger,   XtNcallback,
		ChangeLonger,   (XtPointer)&madeChange);
  XtAddCallback(changeHeadUp,   XtNcallback,
		ChangeHeadUp,   (XtPointer)&madeChange);
  XtAddCallback(changeHeadDown, XtNcallback,
		ChangeHeadDown, (XtPointer)&madeChange);

  XtAddCallback(changeApply,  XtNcallback, ChangeApply, (XtPointer)&result);
  XtAddCallback(changeCancel, XtNcallback, ChangeCancel, (XtPointer)&result);

  madeChange = False;
  result = -1;

  YPushPointerPosition();
  YPlacePopupAndWarp(changeShell, XtGrabExclusive,
		     changeCancel, changeCancel);

  CDrawChord(changeNote, wd, ht);

  context = XtWidgetToApplicationContext(changeShell);
  while (result < 0 || XtAppPending(context)) {

    XtAppProcessEvent(context, XtIMAll);

    if (madeChange) {
      CDrawChord(changeNote, wd, ht);
      madeChange = False;
    }
  }  

  XtPopdown(changeShell);
  XtDestroyWidget(changeShell);
  YPopPointerPosition();

  if (cClef) DestroyClef(cClef);

  if (result == 0) {
    DestroyChord(cTempChord);
    Return(False);
  }

  /* Too much information about the internals of a Chord here */

  if (chord->chord.voices) free(chord->chord.voices);
  if (chord->chord.chord_name) XtFree(chord->chord.chord_name);
  chord->chord.chord_named = False;

  {
    MarkList marks = chord->item.marks;
    chord->item.marks = 0;
    NewChord(chord, cNoteVoices, cTotalVoices, chord->chord.modifiers,
	     cTempChord->chord.visual->type, cTempChord->chord.visual->dotted);
    chord->item.marks = marks;
  }

  DestroyChord(cTempChord);

  Return(True); 
}



void ChordMenuChangeChord(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  Clef          *clef;
  ItemList       clefI;

  Begin("ChordMenuChangeChord");

  /* don't check one-item-only here; assume stave code did it right
     (it was complicated enough there, don't want to duplicate it) */
  if (!stave || !mstave->sweep.swept || !end ||
      end->item->object_class != ChordClass) {

    XBell(display, 70);
    End;
  }

  clef = 0; clefI = start;

  while (clefI) {
    if (clefI->item->object_class == ClefClass) {
      clef = (Clef *)clefI->item; break; 
    } else {
      clefI = iPrev(clefI);
    }
  }

  UndoAssertPreviousContents("Edit Chord", stave, staveNo, start, end);

  if (!EditChord((Chord *)end->item, clef, "Chord - Edit Chord")) {
    UndoCancel();
    End;
  }

  UndoAssertNewContents("Edit Chord", stave, staveNo, start, end);
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Make Chord */

/* These used to be in MenuGroup.c */

void ChordMenuChord(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  int            voiceCount, i, j;
  NoteVoice     *voices;
  ItemList       list;
  Chord         *chord;
  ChordMods      mods = ModNone;
  NoteTag        tag;
  Boolean        dotted;
  MarkList       marklist = 0;
  ItemList       newItem;

  Begin("ChordMenuChord");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    if (!start) start = mstave->music[staveNo];	/* ouch! */

    for (voiceCount = 0, list = iNext(start);
	 list && iPrev(list) != end; list = iNext(list)) {

      if (list->item->object_class !=  ChordClass &&
	  list->item->object_class !=   TextClass) {

	IssueMenuComplaint
	  ("You can only chord notes, not other musical objects.");

	End;
      }

      if (list->item->object_class == ChordClass)
	voiceCount += ((Chord *)list->item)->chord.voice_count;
    }

    if (voiceCount == 0) {

      IssueMenuComplaint("There are no notes here to chord.");
      End;
    }

    UndoAssertPreviousContents("Create Chord", stave, staveNo, start, end);

    voices = (NoteVoice *)XtMalloc(voiceCount * sizeof(NoteVoice));
    voiceCount = 0;
    list = iNext(start);

    tag = ShortestNote;
    dotted = False;

    while (list) {

      if (list->item->object_class == ChordClass) {

	/* accumulate marks onto one list, for the new chord */
	marklist = (MarkList)Nconc(marklist, list->item->item.marks);
	list->item->item.marks = NULL;

	chord  = (Chord *)list->item;
	mods  |= chord->chord.modifiers;

	if ( chord->chord.visual->type >  tag ||
	    (chord->chord.visual->type == tag &&
	     chord->chord.visual->dotted)) {

	  tag    = chord->chord.visual->type;
	  dotted = chord->chord.visual->dotted;
	}

	for (i = 0; i < chord->chord.voice_count; ++i) {

	  for (j = 0; j < voiceCount; ++j)
	    if (voices[j].pitch     == chord->chord.voices[i].pitch &&
		voices[j].modifiers == chord->chord.voices[i].modifiers) break;

	  if (j >= voiceCount) voices[voiceCount++] = chord->chord.voices[i];
	}

	chord->methods->destroy((MusicObject)chord);

	if (list == end) { Remove(list); break; }
	else list = (ItemList)Remove(list);

      } else list = iNext(list);
    }
    
    newItem = NewItemList((Item *)NewChord(NULL, voices, voiceCount,
	 				   mods, tag, dotted)); 

    newItem->item->item.marks = marklist;
    
    if (Next(start)) Insert(newItem, Next(start));
    else Nconc(start, newItem);

    UndoAssertNewContents("Create Chord", stave, staveNo, start, iNext(start));
    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Break Chord */

void ChordMenuUnchord(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  int            voiceCount, i;
  NoteVoice     *voices;
  NoteVoice     *newVoice;
  Chord         *chord;
  ChordMods      mods = ModNone;
  NoteTag        tag;
  Boolean        dotted;
  ItemList       newItem;

  Begin("ChordMenuUnchord");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    if (start != iPrev(end)) {
      IssueMenuComplaint("You can only unchord a single chord.");
      End;
    }

    if (end->item->object_class != ChordClass) {
      IssueMenuComplaint("You can't unchord that; it's not a chord.");
      End;
    }

    chord      = (Chord *)end->item;
    voices     = chord->chord.voices;
    voiceCount = chord->chord.voice_count;
    mods       = chord->chord.modifiers;
    tag        = chord->chord.visual->type;
    dotted     = chord->chord.visual->dotted;

    UndoAssertPreviousContents("Break Chord", stave, staveNo, start, end);

    for (i = 0; i < voiceCount; ++i) {

      newVoice = (NoteVoice *)XtMalloc(sizeof(NoteVoice));
     *newVoice = voices[i];

     newItem =
       NewItemList((Item *)NewChord(NULL, newVoice, 1, mods, tag, dotted));

     if (i == 0) {
       newItem->item->item.marks = chord->item.marks;
     }

     Insert(newItem, end);
    }

    chord->methods->destroy((MusicObject)chord);
    Remove(end);

    UndoAssertNewContents("Break Chord", stave, staveNo, start, newItem);
    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }
}

/* }}} */
/* {{{ Name and Unname Chord */

void ChordMenuNameChord(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;
  String         str;
  Chord          *chord;

  Begin("ChordMenuNameChord");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  for (list = start ? iNext(start) : (ItemList)First(mstave->music[staveNo]); 
       list && iPrev(list) != end; list = iNext(list)) {

    if (!IS_CHORD(list->item)) continue;

    chord = (Chord*)list->item;
    str = Rechord(*chord);
    NameChord(chord, str, True); /* explicit */
    free(str);
  }

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);

  End;
}

void ChordMenuUnnameChord(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;
  Begin("ChordMenuUnnameChord");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  for (list = start ? iNext(start) : (ItemList)First(mstave->music[staveNo]); 
       list && iPrev(list) != end; list = iNext(list)) {

    if (!IS_CHORD(list->item)) continue;

    NameChord((Chord *)list->item, NULL, True);
  }

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);

  End;
}

/* }}} */
/* {{{ Create Chord by Name */

void ChordMenuCreateChord(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  Item          *newItem;
  ItemList       newItemList;
  String         str;
  static String  prevStr = NULL;
  char           *chordName;

  Begin("ChordMenuCreateChord");

  if (!stave || !start || start != end) {
    XBell(display, 70);
    End;
  }

  if (!prevStr) prevStr = XtNewString("C");

  if ((str = YGetUserInput(topLevel, "Chord name:", prevStr, YOrientHorizontal,
			   "Chord - Create by Name")) == NULL) End;

  XtFree(prevStr);
  prevStr = XtNewString(str);

  UndoAssertPreviousContents("Create Chord by Name", stave, staveNo,
			     start, end);

  chordName = strtok(str, ",");
  while(chordName) {

    newItem = (Item *)SpellChord
      (chordName,
       StaveItemToClef(stave, staveNo, start)->clef.clef,
       StaveItemToKey(stave, staveNo, start)->key.visual->sharps);

    if (!newItem) {
      XBell(display, 70);
      break;
    }

    newItemList = NewItemList(newItem);
    if (Next(start)) Insert(newItemList, Next(start));
    else Nconc(start, newItemList);

    chordName = strtok(NULL, ",");
  }

  UndoAssertNewContents("Create Chord by Name", stave, staveNo,
			start, iNext(start));

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ "Show Names" menu options */

extern YMenuElement chordMenu[];
extern int chordMenuFirstChoiceIndex;


static void SetChordMenuChoice(int i)
{
  int j;
  Widget w;
  Begin("SetChordMenuChoice");

  for (j = 0; j < 4; ++j) {
    w = chordMenu[chordMenuFirstChoiceIndex + j].widget;
    if (w) YSetValue(w, XtNleftBitmap, i==j ? yToggleOnMap : yToggleOffMap);
  }

  End;
}


void ChordMenuHideNames(Widget w, XtPointer a, XtPointer b)
{
  int i;
  Begin("ChordMenuHideNames");

  EnterMenuMode(ShowingNoChordNamesMode);
  LeaveMenuMode(ShowingChordNamesMode);
  LeaveMenuMode(ShowingAllChordNamesMode);
  LeaveMenuMode(ShowingAllNoteNamesMode);

  SetChordMenuChoice(0);

  for (i = 0; i < ((MajorStaveRec *)stave)->staves; ++i) {
    StaveResetFormatting(stave, i);
  }

  StaveRefreshAsDisplayed(stave);
  End;
}


void ChordMenuShowNames(Widget w, XtPointer a, XtPointer b)
{
  int i;
  Begin("ChordMenuShowNames");

  LeaveMenuMode(ShowingNoChordNamesMode);
  EnterMenuMode(ShowingChordNamesMode);
  LeaveMenuMode(ShowingAllChordNamesMode);
  LeaveMenuMode(ShowingAllNoteNamesMode);

  SetChordMenuChoice(1);

  for (i = 0; i < ((MajorStaveRec *)stave)->staves; ++i) {
    StaveResetFormatting(stave, i);
  }

  StaveRefreshAsDisplayed(stave);
  End;
}


void ChordMenuShowAllNames(Widget w, XtPointer a, XtPointer b)
{
  int i;
  Begin("ChordMenuShowAllNames");

  LeaveMenuMode(ShowingNoChordNamesMode);
  LeaveMenuMode(ShowingChordNamesMode);
  EnterMenuMode(ShowingAllChordNamesMode);
  LeaveMenuMode(ShowingAllNoteNamesMode);

  SetChordMenuChoice(2);

  for (i = 0; i < ((MajorStaveRec *)stave)->staves; ++i) {
    StaveResetFormatting(stave, i);
  }

  StaveRefreshAsDisplayed(stave);
  End;
}


void ChordMenuShowAllNoteNames(Widget w, XtPointer a, XtPointer b)
{
  int i;
  Begin("ChordMenuShowAllNoteNames");

  LeaveMenuMode(ShowingNoChordNamesMode);
  LeaveMenuMode(ShowingChordNamesMode);
  LeaveMenuMode(ShowingAllChordNamesMode);
  EnterMenuMode(ShowingAllNoteNamesMode);

  SetChordMenuChoice(3);

  for (i = 0; i < ((MajorStaveRec *)stave)->staves; ++i) {
    StaveResetFormatting(stave, i);
  }

  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Pitch Search and Replace */

static int replaceWd = 0, replaceHt = 0;
static Clef *replaceClef;


static void ReplaceDrawChord(Widget w, Chord *chord, Dimension wd, Dimension ht)
{
  Pixmap map;
  Begin("ReplaceDrawChord");

  YGetValue(w, XtNbitmap, &map);

  if (!map || map == None) {
    map = XCreatePixmap(display, RootWindowOfScreen(XtScreen(w)),
			wd, ht, DefaultDepthOfScreen(XtScreen(w)));
  }

  DrawClefAndChordOnSimpleDrawable(replaceClef, chord, map, wd, ht,
				   False, 0, True);
  YSetValue(w, XtNbitmap, map);

  End;
}


static void ReplaceFrom(Widget w, XtPointer client, XtPointer call)
{
  Chord *from = (Chord *)client;
  Begin("ReplaceFrom");

  if (EditChord(from, replaceClef, "Chord - Pitch Search and Replace")) {
    ReplaceDrawChord(w, from, replaceWd, replaceHt);
  }

  End;
}


static void ReplaceTo(Widget w, XtPointer client, XtPointer call)
{
  Chord *to = (Chord *)client;
  Begin("ReplaceFrom");

  if (EditChord(to, replaceClef, "Chord - Pitch Search and Replace")) {
    ReplaceDrawChord(w, to, replaceWd, replaceHt);
  }

  End;
}


static void ReplaceToggleCallback(Widget w, XtPointer client, XtPointer call)
{
  Boolean *value = (Boolean *)client;
  *value = !*value;
}


static void ReplaceApply(Widget w, XtPointer client, XtPointer call)
{
  Begin("ReplaceApply");

  *((int *)client) = 1;

  End;
}


static void ReplaceCancel(Widget w, XtPointer client, XtPointer call)
{
  Begin("ReplaceCancel");

  *((int *)client) = 0;

  End;
}


#define ReplaceIgnoreDurations (1<<0)
#define ReplaceMatchingSubsets (1<<1)
#define ReplaceQuery           (1<<2)

Boolean noteVoicesEqual(NoteVoice *a, NoteVoice *b, Boolean sameOctave)
{
  Boolean result;
  Begin("noteVoicesEqual");

  /* We're not interested in whether the pitches are equal, but
     whether the notes are (in a notational sense).  Thus mods &c must
     be the same, and a flat can't match the sharp of the note below */

  if (a->modifiers != b->modifiers) result = False;
  else if (sameOctave) result = (a->pitch == b->pitch);
  else result = (a->pitch % 7 == b->pitch % 7);

  Return(result);
}

static Boolean HighlightAndQuery(MajorStave sp, int staveNo, ItemList i,
				 int matched, int total, Boolean *cancel)
{
  int rtn;
  Bar *bar;
  char message[256];
  Begin("HighlightAndQuery");

  bar = StaveItemToBar(sp, staveNo, i);
  if (bar) {
    StaveLeapToBar(sp, bar->bar.number);
  }

  StaveCursorSelectSublist(sp, staveNo, iPrev(i), i);

  if (matched < total) {
    sprintf(message, "This chord contains %d matching pitch%s.  Replace %s?",
	    matched, matched == 1 ? "" : "es", matched == 1 ? "it" : "them");
  } else {
    sprintf(message, "Replace this chord?");
  }
  
  rtn = YQuery(topLevel, message, 3, 1, 2, "Yes", "No", "Cancel", NULL);

  if (rtn == 0) Return(True);
  else if (rtn == 1) Return(False);
  else {
    if (cancel) *cancel = True;
    Return(False);
  }
}


Boolean ChordReplacePitches(MajorStave sp, int staveNo, ItemList ilist,
			    Chord *from, Chord *to, Chord *c,
			    unsigned int flags, int *found, Boolean *cancel)
{
  int i, j, matching, oldTotal, toFind, newTotal;
  NoteVoice *v = c->chord.voices;
  NoteVoice *newV;
  Begin("ChordReplacePitches");

  if (!(flags & ReplaceIgnoreDurations) &&
      !MTimeEqual(c->chord.length, from->chord.length)) Return(False);

  /* We assume the note voices are in ascending order.  NewChord
     guarantees this.  That's one reason for only ever constructing
     chords with NewChord. */

  oldTotal = c->chord.voice_count;
  toFind = from->chord.voice_count;
  matching = 0;

  for (i = j = 0; i < oldTotal; ++i) {
    if (j >= toFind) break;
    if (noteVoicesEqual(&v[i], &from->chord.voices[j], True)) {
      ++j; ++matching;
    }
  }

  if (j < toFind) Return(False);
  if (matching < oldTotal && !(flags & ReplaceMatchingSubsets)) Return(False);

  /* We've established that this chord should have some notes replaced */

  if (found) ++*found;

  if ((flags & ReplaceQuery) &&
      !HighlightAndQuery(sp, staveNo, ilist, matching, oldTotal, cancel))
    Return(False); /* user objection */

  newTotal = 0;
  newV = (NoteVoice *)
    XtCalloc(oldTotal - toFind + to->chord.voice_count, sizeof(NoteVoice));
  
  for (i = j = 0; i < oldTotal; ++i) {

    if (j < toFind &&
	noteVoicesEqual(&v[i], &from->chord.voices[j], True)) {
      ++j; continue;
    }

    /* This one is *not* to be replaced, so preserve it */

    memcpy(&newV[newTotal], &v[i], sizeof(NoteVoice));
    ++newTotal;
  }

  memcpy(&newV[newTotal], to->chord.voices,
	 to->chord.voice_count * sizeof(NoteVoice));
  newTotal += to->chord.voice_count;
  
  free(v);

  /* iffy, again */
  {
    MarkList marks = c->item.marks;
    c->item.marks = 0;
    NewChord(c, newV, newTotal, c->chord.modifiers,
	     c->chord.visual->type, c->chord.visual->dotted);
    c->item.marks = marks;
  }

  Return(True);
}

void ChordSearchAndReplacePitches(Chord *from, Chord *to,
				  MajorStave sp, int staveNo,
				  ItemList start, ItemList end, /* inclusive */
				  unsigned int flags, int *foundR,
				  int *replacedR)
{
  ItemList i;
  unsigned found = 0, replaced = 0;
  Boolean cancel;
  ItemList sLeft = iPrev(start);
  Begin("ChordSearchAndReplacePitches");

  UndoAssertPreviousContents("Search and Replace", sp, staveNo, sLeft, end);
  
  for (i = start; i; i = (i == end ? NULL : iNext(i))) {
    if (i->item->object_class == ChordClass) {
      if (ChordReplacePitches(sp, staveNo, i,
			      from, to, (Chord *)i->item, flags,
			      &found, &cancel))
	++replaced;
      if (cancel) break;
    }
  }

  if (foundR) *foundR = found;
  if (replacedR) *replacedR = replaced;

  if (replaced > 0) {
    UndoAssertNewContents("Search and Replace", sp, staveNo, sLeft, end);
  } else {
    UndoCancel();
  }

  End;
}


void ChordMenuSearchReplace(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;

  Widget replaceShell;
  Widget replacePane;
  Widget replaceTopBox;
  Widget replaceTopLabel;
  Widget replaceMidBox;
  Widget replaceMidLeftLabel;
  Widget replaceFrom;
  Widget replaceMidLabel;
  Widget replaceTo;
  Widget replaceOptionBox;
  Widget replaceLowerBox;
  Widget replaceApply;
  Widget replaceCancel;
  Widget replaceHelp;

  Widget replaceMatchLength;
  Widget replaceMatchSubset;
  Widget replaceQuery;

  static Boolean matchLength = True;
  static Boolean matchSubset = False;
  static Boolean query = True;

  int found = 0, replaced = 0;
  char message[256];

  XtAppContext context;
  int result;

  ItemList clefI;
  static Chord *from, *to;

  Begin("ChordMenuSearchReplace");

  if (!stave || !mstave->sweep.swept || !end) {
    XBell(display, 70);
    End;
  }

  if ((start && !iNext(start)) || (!start && !mstave->music[staveNo])) {
    XBell(display, 70);
    End;
  }    

  replaceClef = 0;

  for (clefI = start ? start : mstave->music[staveNo]; clefI;
       clefI = iPrev(clefI)) {
    if (clefI->item->object_class == ClefClass) {
      replaceClef = (Clef *)clefI->item; break; 
    }
  }

  if (!replaceClef) {
    for (clefI = start ? start : mstave->music[staveNo]; clefI;
	 clefI = iNext(clefI)) {
      if (clefI->item->object_class == ClefClass) {
	replaceClef = (Clef *)clefI->item; break; 
      }
    }
  }

  if (!replaceClef) replaceClef = &defaultClef;

  replaceShell = XtCreatePopupShell
    ("Replace Chord", transientShellWidgetClass, topLevel, NULL, 0);

  replacePane = YCreateWidget
    ("Replace Chord Pane", panedWidgetClass, replaceShell);

  replaceTopBox = YCreateShadedWidget
    ("Replace Chord Top Box", boxWidgetClass, replacePane, LightShade);

  replaceTopLabel = YCreateShadedWidget
    ("Within selected area:", labelWidgetClass, replaceTopBox, LightShade);
  YSetValue(replaceTopLabel, XtNborderWidth, 0);

  replaceMidBox = YCreateShadedWidget
    ("Replace Chord Mid Box", formWidgetClass, replacePane, LightShade);

  replaceWd = 105;
  replaceHt = StaveHeight + StaveUpperGap + StaveLowerGap + 12;
  
  replaceMidLeftLabel = YCreateShadedWidget
    ("Replace", labelWidgetClass, replaceMidBox, LightShade);
  YSetValue(replaceMidLeftLabel, XtNborderWidth, 0);
  
  replaceFrom = YCreateCommand("from", replaceMidBox);
  YSetValue(XtParent(replaceFrom), XtNfromHoriz, replaceMidLeftLabel);
  XtVaSetValues(replaceFrom, XtNwidth, replaceWd, XtNheight, replaceHt,
		XtNleftBitmap, NULL, XtNbitmap, NULL, NULL);

  replaceMidLabel = YCreateShadedWidget
    ("with", labelWidgetClass, replaceMidBox, LightShade);
  YSetValue(replaceMidLabel, XtNfromHoriz, XtParent(replaceFrom));
  YSetValue(replaceMidLabel, XtNborderWidth, 0);

  replaceTo = YCreateCommand("to", replaceMidBox);
  YSetValue(XtParent(replaceTo), XtNfromHoriz, replaceMidLabel);
  XtVaSetValues(replaceTo, XtNwidth, replaceWd, XtNheight, replaceHt,
		XtNleftBitmap, NULL, XtNbitmap, NULL, NULL);

  replaceOptionBox = YCreateShadedWidget
    ("Replace Chord options", formWidgetClass, replacePane, MediumShade);

  replaceQuery = YCreateToggle
    ("Query before each replacement", replaceOptionBox, NULL);
  XtAddCallback(replaceQuery, XtNcallback, ReplaceToggleCallback, &query);
  YSetToggleValue(replaceQuery, query);

  replaceMatchLength = YCreateToggle
    ("Replace pitches even if different durations", replaceOptionBox, NULL);
  XtAddCallback(replaceMatchLength, XtNcallback, ReplaceToggleCallback,
		&matchLength);
  YSetToggleValue(replaceMatchLength, matchLength);
  YSetValue(XtParent(replaceMatchLength), XtNfromVert,
	    XtParent(replaceQuery));

  replaceMatchSubset = YCreateToggle
    ("Replace matching subsets of chord pitches", replaceOptionBox, NULL);
  XtAddCallback(replaceMatchSubset, XtNcallback, ReplaceToggleCallback,
		&matchSubset);
  YSetToggleValue(replaceMatchSubset, matchSubset);
  YSetValue(XtParent(replaceMatchSubset), XtNfromVert,
	    XtParent(replaceMatchLength));

  /* doesn't work -- too difficult for now; we rely on NoteVoices
     being in order to make the matching process half-way efficient,
     and of course if you have to consider different octaves you can't
     do that.  maybe one day

  replaceMatchOctaves = YCreateToggle
    ("Replace even in different octaves", replaceOptionBox, NULL);
  XtAddCallback(replaceMatchOctaves, XtNcallback, ReplaceToggleCallback,
		&matchOctaves);
  YSetToggleValue(replaceMatchOctaves, matchOctaves);
  YSetValue(XtParent(replaceMatchOctaves), XtNfromVert,
	    XtParent(replaceMatchSubset)); */
  replaceLowerBox = YCreateShadedWidget
    ("Replace Chord Lower Box", formWidgetClass, replacePane, LightShade);

  replaceApply = YCreateCommand("OK", replaceLowerBox);
  replaceCancel = YCreateCommand("Cancel", replaceLowerBox);

  YSetValue(XtParent(replaceCancel), XtNfromHoriz, XtParent(replaceApply));

  if (appData.interlockWindow) {
    replaceHelp = YCreateCommand("Help", replaceLowerBox);
    YSetValue(XtParent(replaceHelp), XtNfromHoriz, XtParent(replaceCancel));
    XtAddCallback(replaceHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Chord - Pitch Search and Replace");
  } else {
    replaceHelp = NULL;
  }

  XtAddCallback(replaceApply, XtNcallback, ReplaceApply, (XtPointer)&result);
  XtAddCallback(replaceCancel, XtNcallback, ReplaceCancel, (XtPointer)&result);

  if (!from) from = NewChord(NULL, NewNoteVoice(NULL, 3, ModNone),
			     1, ModNone, Crotchet, False);

  if (!to)     to = NewChord(NULL, NewNoteVoice(NULL, 4, ModNone),
			     1, ModNone, Crotchet, False);

  XtAddCallback(replaceFrom, XtNcallback, ReplaceFrom, (XtPointer)from);
  XtAddCallback(replaceTo, XtNcallback, ReplaceTo, (XtPointer)to);

  result = -1;

  YPushPointerPosition();
  YPlacePopupAndWarp(replaceShell, XtGrabNonexclusive,
		     replaceCancel, replaceCancel);

  ReplaceDrawChord(replaceFrom, from, replaceWd, replaceHt);
  ReplaceDrawChord(replaceTo, to, replaceWd, replaceHt);

  context = XtWidgetToApplicationContext(replaceShell);
  while (result < 0 || XtAppPending(context)) {
    XtAppProcessEvent(context, XtIMAll);
  }  

  if (result > 0) {

    unsigned flags = 0;
    if (query)        flags |= ReplaceQuery;
    if (matchLength)  flags |= ReplaceIgnoreDurations;
    if (matchSubset)  flags |= ReplaceMatchingSubsets;

    if (!start) start = (ItemList)First(mstave->music[staveNo]);
    else start = iNext(start);

    ChordSearchAndReplacePitches(from, to, stave, staveNo,
				 start, end, flags, &found, &replaced); 
  }

  XtPopdown(replaceShell);
  XtDestroyWidget(replaceShell);
  YPopPointerPosition();

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);

  if (found > 0) {
    if (replaced < found) {
      sprintf(message, "Search and replace complete.\n"
	      "%d matching chord%s found; %d change%s made.",
	      found, found == 1 ? "" : "s", replaced, replaced == 1 ? "" : "s");
    } else {
      sprintf(message, "Search and replace complete.\n"
	      "%d matching chord%s found and changed.",
	      found, found == 1 ? "" : "s");
    }

    (void)YQuery(topLevel, message, 1, 0, 0, "OK", NULL);
  }

  End;
}

/* }}} */
