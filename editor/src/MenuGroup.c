
/* MenuGroup.c */

/* Musical Notation Editor for X, Chris Cannam 1994- */
/* Actions available from the Group menu             */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "Yawn.h"
#include "Undo.h"
#include "ItemList.h"
#include "Marks.h"

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

/* }}} */

/* {{{ Tie */

void GroupMenuTie(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       templist;
  int            i;

  Begin("GroupMenuTie");

  /* bleah */
  while (iNext(start ? start : mstave->music[staveNo]) &&
	 iNext(start ? start : mstave->music[staveNo])->item->object_class ==
	 TextClass) {
    start = iNext(start ? start : mstave->music[staveNo]);
  }

  if (!stave || !mstave->sweep.swept || !end || start == end ||
      (start && (iNext(start) == end)) || !iPrev(end)) {

    XBell(display, 70);
    End;
  }

  for (templist = iNext(start), i = 0; templist && templist != end;
       templist = iNext(templist)) {

    if (templist->item->object_class == ChordClass) ++i;

    if ((templist->item->object_class != ChordClass &&
	 templist->item->object_class != TextClass) || i > 1) {
      
      IssueMenuComplaint("You can only tie two exactly adjacent chords.");
      End;
    }
  }

  start = start ? iNext(start) : mstave->music[staveNo];

  if (TIED_BACKWARD(start->item)) start = iPrev(start);
  if (TIED_BACKWARD(end->item)) end = iPrev(end);

  if (start->item->object_class != ChordClass ||
      end->item->object_class != ChordClass) {

    IssueMenuComplaint("You can only tie chords. "
		       "(I'm not at all sure about this error message.)");
    End;
  }

  for (i = 0; i < ((Chord *)start->item)->chord.voice_count; ++i) {
    if (i >= ((Chord *)end->item)->chord.voice_count ||
	((Chord *)start->item)->chord.voices[i].pitch !=
	((Chord *)end->item)->chord.voices[i].pitch) {
      IssueMenuComplaint("You can only tie chords with identical pitches.");
      End;
    }
  }

  MarkItems(Tie, start, end);

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);

  End;
}

/* }}} */
/* {{{ Slur */

void GroupMenuSlur(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;

  Begin("GroupMenuSlur");

  if (!stave || !mstave->sweep.swept || !end || start == end ||
      (start && (iNext(start) == end))) {

    XBell(display, 70);

  } else {

    start = start ? iNext(start) : mstave->music[staveNo];

    if (TIED_BACKWARD(start->item)) start = iPrev(start);
    if (TIED_BACKWARD(end->item)) end = iPrev(end);

    MarkItems(Slur, start, end);

    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Crescendo */

void GroupMenuCrescendo(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;

  Begin("GroupMenuCrescendo");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    start = start ? iNext(start) : mstave->music[staveNo];

    if (TIED_BACKWARD(start->item)) start = iPrev(start);
    if (TIED_BACKWARD(end->item)) end = iPrev(end);

    MarkItems(Crescendo, start, end);

    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Decrescendo */

void GroupMenuDecrescendo(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;

  Begin("GroupMenuDecrescendo");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    start = start ? iNext(start) : mstave->music[staveNo];

    if (TIED_BACKWARD(start->item)) start = iPrev(start);
    if (TIED_BACKWARD(end->item)) end = iPrev(end);

    MarkItems(Decrescendo, start, end);

    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Remove Marks */

void GroupMenuRemove(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  Boolean        found = False;
  ItemList       list, sublist;
  MarkList       marklist, submarklist, tempmarklist;

  Begin("GroupMenuRemove");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  for (list = start ? iNext(start) : mstave->music[staveNo]; list;
       list = (list == end ? NULL : iNext(list))) {

    if (list->item->item.marks) {

      marklist = (MarkList)First(list->item->item.marks);

      while (marklist) {

	if (marklist->mark->start) {

	  for (sublist = list; sublist;
	       sublist = (sublist == end ? NULL : iNext(sublist))) {

	    if ((submarklist = 
		 FindPairMark(marklist->mark, sublist->item->item.marks))) {

	      found = True;
	      sublist->item->item.marks = (MarkList)Remove(submarklist);
	      tempmarklist= list->item->item.marks = (MarkList)Remove(marklist);
	      break;
	    }
	  }

	  if (sublist) marklist = tempmarklist;
	  else marklist = (MarkList)Next(marklist);

	} else marklist = (MarkList)Next(marklist);
      }
    }
  }

  if (!found) {
    IssueMenuComplaint
      ("There are no indications completely within the selected area.");
    End;
  }

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}     

/* }}} */

/* {{{ Beam */

void GroupMenuBeam(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;

  Begin("GroupMenuBeam");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    UndoAssertPreviousContents("Beam Group", stave, staveNo,
			       start ? iPrev(start) : start, end);

    ItemListEnGroup
      (GroupBeamed, start ? iNext(start) : mstave->music[staveNo], end);

    UndoAssertNewContents("Beam Group", stave, staveNo,
			  start ? iPrev(start) : start, end);

    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Tuplet */

typedef struct _TupletRec
{
  char     description[40];
  NoteTag  noteTag;
  Boolean  dotted;
  int      count;
} TupletRec;


void TupletChoiceOK(Widget w, XtPointer a, XtPointer b)
{
  int *result = (int *)a;
  Begin("TupletChoiceOK");
  *result = 1;
  End;
}


void TupletChoiceCancel(Widget w, XtPointer a, XtPointer b)
{
  int *result = (int *)a;
  Begin("TupletChoiceCancel");
  *result = 0;
  End;
}


int GetTupletChoice(TupletRec *choices, int nch, int deft, int *newCount)
{
  Widget tuDlg, tuPane, tuTopBox, tuLabel, tuForm, tuBottomBox, tuOk, tuCancel,
    tuHelp, tuPlayLabel, tuNowMenuButton, tuDescLabel, tuThenInputForm;
  String *options;
  int result = -1;
  char deftext[10];
  int i;

  Begin("GetTupletChoice");

  options = (String *)XtMalloc(nch * sizeof(String));
  for (i = 0; i < nch; ++i) {
    options[i] = choices[i].description;
  }

  tuDlg = XtCreatePopupShell("Tuplet", transientShellWidgetClass,
			     topLevel, NULL, 0);

  tuPane = YCreateWidget("Tuplet Pane", panedWidgetClass, tuDlg);
  tuTopBox = YCreateShadedWidget("Tuplet Title Box", boxWidgetClass,
				 tuPane, MediumShade);
  tuLabel = YCreateLabel("New timing for this group:", tuTopBox);
  tuForm = YCreateShadedWidget("Tuplet Form", formWidgetClass,
			       tuPane, LightShade);
  tuPlayLabel = YCreateLabel("Play", tuForm);
  tuNowMenuButton = YCreateOptionMenu(tuForm, options, nch, deft, NULL, NULL);
  tuDescLabel = YCreateLabel("in the time of", tuForm);
  tuThenInputForm = YCreateSurroundedWidget
    ("Input Text", asciiTextWidgetClass, tuForm, LightShade, NoShade);

  sprintf(deftext, "%d", (choices[deft].count < 3 ?
			  choices[deft].count + 1 : choices[deft].count - 1));
  YSetValue(tuThenInputForm, XtNstring, deftext);
  YSetValue(tuThenInputForm, XtNinsertPosition, strlen(deftext));
  YSetValue(tuThenInputForm, XtNeditType, XawtextEdit);

  tuBottomBox = YCreateShadedWidget("Tuplet Bottom Box", boxWidgetClass,
				    tuPane, MediumShade);

  tuOk = YCreateCommand("OK", tuBottomBox);
  XtAddCallback(tuOk, XtNcallback, TupletChoiceOK, (XtPointer)&result);
  tuCancel = YCreateCommand("Cancel", tuBottomBox);
  XtAddCallback(tuCancel, XtNcallback, TupletChoiceCancel, (XtPointer)&result);

  if (appData.interlockWindow) {
    tuHelp = YCreateCommand("Help", tuBottomBox);
    XtAddCallback(tuHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Group - Tuplet");
  } else {
    tuHelp = NULL;
  }

  YSetValue(XtParent(tuNowMenuButton), XtNfromHoriz, XtParent(tuPlayLabel));
  YSetValue(XtParent(tuDescLabel), XtNfromVert, XtParent(tuPlayLabel));
  YSetValue(XtParent(tuThenInputForm), XtNfromVert, XtParent(tuPlayLabel));
  YSetValue(XtParent(tuThenInputForm), XtNfromHoriz, XtParent(tuDescLabel));

  YPushPointerPosition();
  YPlacePopupAndWarp(tuDlg, XtGrabNonexclusive, tuOk, NULL);
  XtSetKeyboardFocus(tuDlg, tuThenInputForm);
  YAssertDialogueActions(tuDlg, tuOk, NULL, NULL);
  YFixOptionMenuLabel(tuNowMenuButton);

  while (result == -1 || XtAppPending(XtWidgetToApplicationContext(tuOk))) {
    XtAppProcessEvent(XtWidgetToApplicationContext(tuOk), XtIMAll);
  }

  YPopdown(tuDlg);

  if (result == 1) {
    char *text;
    result = YGetCurrentOption(tuNowMenuButton);
    YGetValue(tuThenInputForm, XtNstring, &text);
    *newCount = atoi(text);
    if (*newCount < 1) result = -1;
  } else {
    result = -1;
  }

  YDestroyOptionMenu(tuNowMenuButton);
  XtDestroyWidget(tuDlg);
  XtFree(options);

  Return(result);
}


/* if w is non-NULL, we want a dialogue box; if it's NULL we try to do
   the "obvious" thing */

void GroupMenuTuplet(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  TupletRec     *choices = NULL; /* possibility records */
  int            nch     = 0;    /* number of choices */
  int            deft    = 0;	 /* "natural" choice */
  int            length  = 0;
  Boolean        dotted;
  int            n, newCount;
  ItemList       i;

  Begin("GroupMenuTuplet");

  for (i = start ? iNext(start) : mstave->music[staveNo];
       i && end && (i != iNext(end)); i = iNext(i)) {

    length += MTimeToNumber(i->item->methods->get_length(i->item));
  }

  if (!stave || !mstave->sweep.swept || !end || start == end || length == 0) {
    XBell(display, 70);
    End;
  }

  /* collect all notes which divide the length of the group exactly */

  for (n = ShortestNote, dotted = False; (n <= LongestNote) || !dotted;
       (dotted || n == ShortestNote) ?
	 (++n, dotted = False) : (dotted = True)) {

    if ((length % TagToNumber(n, dotted) == 0) &&
	(length / TagToNumber(n, dotted) > 1)) {

      int m;

      choices = (TupletRec *)
	XtRealloc((char *)choices, (nch + 1) * sizeof(TupletRec));

      choices[nch].noteTag = n;
      choices[nch].dotted = dotted;
      choices[nch].count = length / TagToNumber(n, dotted);

      sprintf
	(choices[nch].description, "%d %ss", choices[nch].count,
	 dotted ? noteVisuals[n].dotted.name : noteVisuals[n].undotted.name);

      for (m = 0; choices[nch].description[m]; ++m) {
	if (choices[nch].description[m] == ' ') {
	  if (isupper(choices[nch].description[m+1])) {
	    choices[nch].description[m+1] =
	      tolower(choices[nch].description[m+1]);
	  }
	}
      }

      if (!dotted) deft = nch;
      ++nch;
    }
  }

  if (nch == 0) { XBell(display, 70); End; }
  if (deft == 0) deft = nch - 1;

  if (w) {
    n = GetTupletChoice(choices, nch, deft, &newCount);
  } else {
    n = deft;
    newCount = (choices[deft].count < 3 ?
		choices[deft].count + 1 : choices[deft].count - 1);
  }

  if (n < 0) { 
    XtFree((void *)choices);
    End;
  }

  {
    /* !!! new, unsure */

    short tupledCount = choices[n].count;
    short tupledLength =
      newCount * TagToNumber(choices[n].noteTag, choices[n].dotted);

    UndoAssertPreviousContents("Tuplet Group", stave, staveNo, start, end);

    ItemListEnTuplet(start ? iNext(start) : mstave->music[staveNo], end,
		     length, tupledLength, tupledCount);

    UndoAssertNewContents("Tuplet Group", stave, staveNo, start, end);
  }

  XtFree((void *)choices);

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);

  End;
}


void GroupMenuSimpleTuplet(Widget w, XtPointer a, XtPointer b)
{
  Begin("GroupMenuSimpleTuplet");

  GroupMenuTuplet(0, 0, 0);
  UndoChangeLabelName("Simple Tuplet Group", stave);

  End;
}

/* }}} */
/* {{{ Grace */

void GroupMenuGrace(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;

  Begin("GroupMenuGrace");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    UndoAssertPreviousContents("Grace Group", stave, staveNo, start, end);

    ItemListEnGroup
      (GroupDeGrace, start ? iNext(start) : mstave->music[staveNo], end);

    UndoAssertNewContents("Grace Group", stave, staveNo, start, end);

    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Break Group */

void GroupMenuBreakGroup(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;

  Begin("GroupMenuBreakGroup");

  if (!stave || !mstave->sweep.swept || !end || start == end) {

    XBell(display, 70);

  } else {

    UndoAssertPreviousContents("Break Group", stave, staveNo, start, end);
    
    for (list = start ? iNext(start) : mstave->music[staveNo];
	 list; list = (list == end) ? NULL : iNext(list)) {
      list->item->item.grouping.none.type = GroupNone;
    }
    
    UndoAssertNewContents("Break Group", stave, staveNo, start, end);

    FileMenuMarkChanged(stave, True);
    StaveResetFormatting(stave, staveNo);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */

/* {{{ Transpose */

static int transposeValueMaxWidth = 100;


static void TransposeUp(Widget w, XtPointer p, XtPointer b)
{
  int          n;
  String       temp;
  static char  buffer[24];
  Widget       value = (Widget)p;

  Begin("TransposeUp");

  YGetValue(value, XtNlabel, &temp);

  n = atoi(temp) + 1;
  if (n > 99) n = 99;

  sprintf(buffer, " %d Semitone%c ", n, (n == 1 || n == -1) ? '\0' : 's');

  XtUnmanageChild(value);

  YSetValue(value, XtNlabel, buffer);
  YSetValue(value, XtNwidth, transposeValueMaxWidth);

  XtManageChild(value);

  End;
}


static void TransposeDown(Widget w, XtPointer p, XtPointer b)
{
  int          n;
  String       temp;
  static char  buffer[24];
  Widget       value = (Widget)p;

  Begin("TransposeDown");

  YGetValue(value, XtNlabel, &temp);

  n = atoi(temp) - 1;
  if (n < -99) n = -99;

  sprintf(buffer, " %d Semitone%c ", n, (n == 1 || n == -1) ? '\0' : 's');

  XtUnmanageChild(value);

  YSetValue(value, XtNlabel, buffer);
  YSetValue(value, XtNwidth, transposeValueMaxWidth);

  XtManageChild(value);

  End;
}


void TransposeOK(Widget w, XtPointer a, XtPointer b)
{
  Begin("TransposeOK");

  *((int *)a) = 1;

  End;
}


void TransposeCancel(Widget w, XtPointer a, XtPointer b)
{
  Begin("TransposeOK");

  *((int *)a) = 0;

  End;
}


void GroupMenuTranspose(Widget w, XtPointer a, XtPointer b)
{
  Widget         tShell;
  Widget         tPane;
  Widget         tTopBox;
  Widget         tLabel;
  Widget         tValue;
  Widget         tForm;
  Widget         tUp;
  Widget         tDown;
  Widget         tBottomBox;
  Widget         tOK;
  Widget         tCancel;
  Widget         tHelp = NULL;
  XtAppContext   context;

  Dimension      w1;
  XPoint         op;
  String         temp;
  char           title[32];
  int            i, result = -1;

  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  int            scope;
  ItemList       last;

  Begin("GroupMenuTranspose");
 
  if (!mstave->sweep.swept || !start) scope = 3;  /* whole piece   */
  else if (start == end)              scope = 2;  /* rest of staff */
  else                                scope = 1;  /* swept area    */

  tShell  = XtCreatePopupShell("Transpose", transientShellWidgetClass, 
			      topLevel, NULL, 0);

  tPane   = YCreateWidget("Transpose Pane", panedWidgetClass, tShell);

  tTopBox = YCreateShadedWidget("Transpose Title Box", boxWidgetClass,
				tPane, MediumShade);

  switch(scope) {
  case 1:  sprintf(title, "Transpose Area");          break;
  case 2:  sprintf(title, "Transpose Rest of Staff"); break;
  default: sprintf(title, "Transpose Whole Piece");   break;
  }

  tLabel     = YCreateLabel(title, tTopBox);

  tForm      = YCreateShadedWidget
    ("Transpose Form", formWidgetClass, tPane, LightShade);

  tUp        = YCreateArrowButton("Up",       tForm, YArrowUp);
  tDown      = YCreateArrowButton("Down",     tForm, YArrowDown);
  tValue     = YCreateLabel(" XXX Semitones ", tForm);

  tBottomBox = YCreateShadedWidget
    ("Transpose Button Box", formWidgetClass, tPane, MediumShade);

  tOK	     = YCreateCommand("OK",     tBottomBox);
  tCancel    = YCreateCommand("Cancel", tBottomBox);

  YSetValue(XtParent(tDown) ,  XtNfromVert,     XtParent(tUp));
  YSetValue(XtParent(tValue),  XtNfromHoriz,    XtParent(tDown));
  YSetValue(XtParent(tCancel), XtNfromHoriz,    XtParent(tOK));
  YSetValue(XtParent(tValue),  XtNvertDistance, 16);

  if (appData.interlockWindow) {

    tHelp = YCreateCommand("Help", tBottomBox);
    YSetValue(XtParent(tHelp), XtNfromHoriz, XtParent(tCancel));

    XtAddCallback(tHelp, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Group - Transpose");
  }

  XtAddCallback(tOK,     XtNcallback, TransposeOK,     (XtPointer)&result);
  XtAddCallback(tCancel, XtNcallback, TransposeCancel, (XtPointer)&result);
  XtAddCallback(tUp,     XtNcallback, TransposeUp,     (XtPointer)tValue);
  XtAddCallback(tDown,   XtNcallback, TransposeDown,   (XtPointer)tValue);

  XtSetMappedWhenManaged(tShell, False);
  XtRealizeWidget(tShell);

  YGetValue(tTopBox, XtNwidth, &w1);
  YGetValue(tValue,  XtNwidth, &transposeValueMaxWidth);

  XtUnrealizeWidget(tShell);

  YSetValue(tValue, XtNlabel,  "0 Semitones");
  YSetValue(tValue, XtNwidth,   transposeValueMaxWidth);

  XtSetMappedWhenManaged(tShell, True);
  op = YPlacePopupAndWarp(tShell, XtGrabExclusive, tOK, tCancel);
  YAssertDialogueActions(tShell, tOK, tCancel, tHelp);

  context = XtWidgetToApplicationContext(tShell);
  while (result < 0 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (op.x || op.y) (void) YPopPointerPosition();

  if (result) {
    YGetValue(tValue, XtNlabel, &temp);
    result = atoi(temp);
  }

  YPopdown(tShell);
  YRetractDialogueActions(tShell);
  XtDestroyWidget(tShell);

  if (!result) End;

  switch(scope) {

  case 1:

    UndoAssertPreviousContents("Transpose", stave, staveNo, start, end);
    ItemListTranspose(iNext(start), end, result);
    UndoAssertNewContents("Transpose", stave, staveNo, start, end);
    StaveResetFormatting(stave, staveNo);
    break;

  case 2:

    last = (ItemList)Last(start);
    UndoAssertPreviousContents("Transpose", stave, staveNo, start, last);
    ItemListTranspose(start, NULL, result);
    UndoAssertNewContents("Transpose", stave, staveNo, start, last);
    StaveResetFormatting(stave, staveNo);
    break;

  default:

    if (!UndoInvalidate("Transposing the whole piece", stave,
			"Group - Transpose")) End;

    for (i = 0; i < mstave->staves; ++i) {

      ItemListTranspose(mstave->music[i], NULL, result);
      StaveResetFormatting(stave, i);
    }

    break;
  }

  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Invert */

/* Invert and Retrograde functions added JPff 94 */
      
static void Invert(ItemList from, ItemList to, int centre)
{
  int         mp;		/* MIDI pitch, used as intermediate repn */
  ItemList    i, j;
  short       v;
  Chord      *c;
  Key        *k  = NULL;
  static Key *k0 = NULL;
  NoteVoice   voice;
  Boolean     first = True;

  Begin("Invert");

  for (j = from; j; j = iPrev(j))
    if (j->item->object_class == KeyClass) k = (Key *)(j->item);

  if (!k)
    if (!k0) k = k0 = NewKey(NULL, KeyC);
    else k = k0;

  for (i = from; i; i = i ? iNext(i) : i) {

    if (i->item->object_class == ChordClass)
      for (v = 0, c = (Chord *)(i->item); v < c->chord.voice_count; ++v) {

	mp = VoiceToMidiPitch(&c->chord.voices[v], TrebleClef);
	if (first) centre = centre + mp, first = False;
	voice = MidiPitchToVoice(centre + centre - mp, k->key.visual->sharps);
	c->chord.voices[v] = voice;
      }

    if (i == to) i = NULL;
  }

  End;
}

static void InvertUp(Widget w, XtPointer p, XtPointer b)
{
  int          n;
  String       temp;
  static char  buffer[24];
  Widget       value = (Widget)p;

  Begin("InvertUp");

  YGetValue(value, XtNlabel, &temp);

  n = atoi(temp) + 1;
  if (n > 99) n = 99;

  sprintf(buffer, " %d Semitone%c ", n, (n == 1 || n == -1) ? '\0' : 's');

  XtUnmanageChild(value);

  YSetValue(value, XtNlabel, buffer);
  YSetValue(value, XtNwidth, transposeValueMaxWidth);

  XtManageChild(value);

  End;
}


static void InvertDown(Widget w, XtPointer p, XtPointer b)
{
  int          n;
  String       temp;
  static char  buffer[24];
  Widget       value = (Widget)p;

  Begin("InvertUp");

  YGetValue(value, XtNlabel, &temp);

  n = atoi(temp) - 1;
  if (n < -99) n = -99;

  sprintf(buffer, " %d Semitone%c ", n, (n == 1 || n == -1) ? '\0' : 's');

  XtUnmanageChild(value);

  YSetValue(value, XtNlabel, buffer);
  YSetValue(value, XtNwidth, transposeValueMaxWidth);

  XtManageChild(value);

  End;
}


void InvertOK(Widget w, XtPointer a, XtPointer b)
{
  Begin("InvertOK");

  *((int *)a) = 1;

  End;
}


void InvertCancel(Widget w, XtPointer a, XtPointer b)
{
  Begin("InvertOK");

  *((int *)a) = 0;

  End;
}


void GroupMenuInvert(Widget w, XtPointer a, XtPointer b)
{
  Widget         tShell;
  Widget         tPane;
  Widget         tTopBox;
  Widget         tLabel;
  Widget         tValue;
  Widget         tForm;
  Widget         tUp;
  Widget         tDown;
  Widget         tBottomBox;
  Widget         tOK;
  Widget         tCancel;
  Widget         tHelp = NULL;
  XtAppContext   context;

  Dimension      w1;
  XPoint         op;
  String         temp;
  char           title[32];
  int            i, result = -1;

  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  int            scope;
  ItemList       last;

  Begin("GroupMenuInvert");
 
  if (!mstave->sweep.swept || !start) scope = 3;  /* whole piece   */
  else if (start == end)              scope = 2;  /* rest of staff */
  else                                scope = 1;  /* swept area    */

  tShell  = XtCreatePopupShell("Inversion", transientShellWidgetClass, 
			      topLevel, NULL, 0);

  tPane   = YCreateWidget("Inversion Pane", panedWidgetClass, tShell);

  tTopBox = YCreateShadedWidget("Inversion Title Box", boxWidgetClass,
				tPane, MediumShade);

  switch(scope) {
  case 1:  sprintf(title, "Invert Area around pitch");          break;
  case 2:  sprintf(title, "Invert Rest of Staff around pitch"); break;
  default: sprintf(title, "Invert Whole Piece around pitch");   break;
  }

  tLabel     = YCreateLabel(title, tTopBox);

  tForm      = YCreateShadedWidget
    ("Invert Form", formWidgetClass, tPane, LightShade);

  tUp        = YCreateArrowButton("Up",       tForm, YArrowUp);
  tDown      = YCreateArrowButton("Down",     tForm, YArrowDown);
  tValue     = YCreateLabel(" XXX Semitones ", tForm);

  tBottomBox = YCreateShadedWidget
    ("Invert Button Box", formWidgetClass, tPane, MediumShade);

  tOK	     = YCreateCommand("OK",     tBottomBox);
  tCancel    = YCreateCommand("Cancel", tBottomBox);

  YSetValue(XtParent(tDown) ,  XtNfromVert,     XtParent(tUp));
  YSetValue(XtParent(tValue),  XtNfromHoriz,    XtParent(tDown));
  YSetValue(XtParent(tCancel), XtNfromHoriz,    XtParent(tOK));
  YSetValue(XtParent(tValue),  XtNvertDistance, 16);

  if (appData.interlockWindow) {

    tHelp = YCreateCommand("Help", tBottomBox);
    YSetValue(XtParent(tHelp), XtNfromHoriz, XtParent(tCancel));

    XtAddCallback
      (tHelp, XtNcallback, yHelpCallbackCallback,
       (XtPointer)"Group - Invert");
  }

  XtAddCallback(tOK,     XtNcallback, InvertOK,     (XtPointer)&result);
  XtAddCallback(tCancel, XtNcallback, InvertCancel, (XtPointer)&result);
  XtAddCallback(tUp,     XtNcallback, InvertUp,     (XtPointer)tValue);
  XtAddCallback(tDown,   XtNcallback, InvertDown,   (XtPointer)tValue);

  XtSetMappedWhenManaged(tShell, False);
  XtRealizeWidget(tShell);

  YGetValue(tTopBox, XtNwidth, &w1);
  YGetValue(tValue,  XtNwidth, &transposeValueMaxWidth);

  XtUnrealizeWidget(tShell);

  YSetValue(tValue, XtNlabel,  "0 Semitones");
  YSetValue(tValue, XtNwidth,   transposeValueMaxWidth);

  XtSetMappedWhenManaged(tShell, True);
  op = YPlacePopupAndWarp(tShell, XtGrabExclusive, tOK, tCancel);
  YAssertDialogueActions(tShell, tOK, tCancel, tHelp);

  context = XtWidgetToApplicationContext(tShell);
  while (result < 0 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (op.x || op.y) (void) YPopPointerPosition();

  if (result) {
    YGetValue(tValue, XtNlabel, &temp);
    i = atoi(temp);
  }

  YPopdown(tShell);
  YRetractDialogueActions(tShell);
  XtDestroyWidget(tShell);

  if (!result) End;

  result = i;

  switch(scope) {

  case 1:

    UndoAssertPreviousContents("Invert", stave, staveNo, start, end);
    Invert(iNext(start), end, result);
    UndoAssertNewContents("Invert", stave, staveNo, start, end);
    StaveResetFormatting(stave, staveNo);
    break;

  case 2:

    last = (ItemList)Last(start);
    UndoAssertPreviousContents("Invert", stave, staveNo, start, last);
    Invert(start, NULL, result);
    UndoAssertNewContents("Invert", stave, staveNo, start, last);
    StaveResetFormatting(stave, staveNo);
    break;

  default:

    if (!UndoInvalidate("Inverting the whole piece", stave,
			"Group - Invert")) End;

    for (i = 0; i < mstave->staves; ++i) {

      Invert(mstave->music[i], NULL, result);
      StaveResetFormatting(stave, i);
    }

    break;
  }
  /*
  switch(scope) {

  case 1:

    Invert(iNext(start), end, result);
    StaveResetFormatting(stave, staveNo);
    break;

  case 2:

    Invert(iNext(start), NULL, result);
    StaveResetFormatting(stave, staveNo);
    break;

  default:

    for (i = 0; i < mstave->staves; ++i) {

      Invert(mstave->music[i], NULL, result);
      StaveResetFormatting(stave, i);
    }
    
    break;
  }
  */
  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Retrograde */

static ItemList tempboard = NULL;

void GroupMenuRetrograde(Widget w, XtPointer a, XtPointer b)
{
  ItemList       list;
  Item          *newItem;
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  ItemList       tempEnd;
  int            staveNo = mstave->sweep.stave;

  Begin("GroupMenuRetrograde");

  if (!stave || !mstave->sweep.swept || !start || start==end) {
    XBell(display, 70);
    End;
  }

  tempboard = NULL;

  if (start) list = iNext(start);
  else       list = mstave->music[mstave->sweep.stave];

  while (list) {

    newItem = (Item *)list->item->methods->clone(list->item);
    tempboard = (ItemList)Nconc(NewItemList(newItem), tempboard);

    if (list == end) break;
    list = iNext(list);
  }

  UndoAssertPreviousContents("Retrograde", stave, staveNo, start, end);

  mstave->music[staveNo] = ItemListRemoveItems(start, end);
  tempEnd = (ItemList)Last(tempboard);

  /*
  EditMenuDelete(w, a, b);
  */

  if (start) {
    if (Next(start)) Insert(tempboard, Next(start));
    else Nconc(start, tempboard);
  }
  else mstave->music[staveNo] = (ItemList)Nconc(tempboard, start);

  UndoAssertNewContents("Retrograde", stave, staveNo, start, tempEnd);
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */

/* {{{ Auto-beam */

void GroupMenuAutoBeam(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  ItemList       beamS;
  int            staveNo = mstave->sweep.stave;
  int            scope;
  ItemList       last;
  TimeSignature *time;

  Begin("GroupMenuAutoBeam");


  time = &defaultTimeSignature;  /* hack? */


  if (!mstave->sweep.swept || !start) scope = 3;  /* whole piece   */
  else if (start == end)              scope = 2;  /* rest of staff */
  else                                scope = 1;  /* swept area    */

  if (start) beamS = iNext(start);
  else beamS = mstave->music[staveNo];
  last = (ItemList)Last(beamS);
     
  switch(scope) {

  case 1:
    time = StaveItemToTimeSignature(stave, staveNo, start);

    if (!beamS) break;

    UndoAssertPreviousContents("Auto-beam", stave, staveNo, start, end);
    ItemListAutoBeam(time, beamS, end);
    UndoAssertNewContents("Auto-beam", stave, staveNo, start, end);
    StaveResetFormatting(stave, staveNo);
    break;

  case 2:

    if (YQuery(topLevel, "Auto-beam the rest of the staff?", 2, 1, 1,
	       "Continue", "Cancel", "Group - Auto Beam")) break;

    time = StaveItemToTimeSignature(stave, staveNo, start);

    if (!beamS) break;

    UndoAssertPreviousContents("Auto-beam", stave, staveNo, start, last);
    ItemListAutoBeam(time, beamS, last);
    UndoAssertNewContents("Auto-beam", stave, staveNo, start, last);
    StaveResetFormatting(stave, staveNo);
    break;

  case 3:
    /*
    if (YQuery(topLevel, "Auto-beam the whole piece?", 2, 1, 1,
	       "Continue", "Cancel", "Group - Auto Beam")) break;
	       */

    if (!UndoInvalidate("Auto-beaming the whole piece", stave,
			"Group - Auto Beam")) break;

    for (staveNo = 0; staveNo < mstave->staves; ++staveNo) {

      mstave->music[staveNo] = (ItemList)First(mstave->music[staveNo]);
      time = StaveItemToTimeSignature(stave, staveNo, start);

      ItemListAutoBeam(time, mstave->music[staveNo], last);
      StaveResetFormatting(stave, staveNo);
    }

    break;
  }

  FileMenuMarkChanged(stave, True);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */

