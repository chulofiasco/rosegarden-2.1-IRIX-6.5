
/* MenuEdit.c */

/* Musical Notation Editor for X, Chris Cannam 1994-97 */
/* Actions available from the Edit menu                */


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

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* }}} */
/* {{{ Declarations */

/* clipboard is assumed to contain one outside-accessible item, which
   is a Bar pointer */

static ItemList clipboard = NULL;

void EditMenuDelete (Widget, XtPointer, XtPointer);
void EditMenuCut    (Widget, XtPointer, XtPointer);
void EditMenuCopy   (Widget, XtPointer, XtPointer);
void EditMenuPaste  (Widget, XtPointer, XtPointer);

/* }}} */

/* {{{ Delete */

void EditMenuDelete(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  Clef          *clef;

  Begin("EditMenuDelete");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  if (BarValid(mstave->bar_list->bars[staveNo]) &&
      mstave->bar_list->bars[staveNo]->bar.clef) {

    clef = mstave->bar_list->bars[staveNo]->bar.clef;
    clef = (Clef *)clef->methods->clone((MusicObject)clef);

  } else {

    clef = NewClef(NULL, TrebleClef);
  }

  UndoAssertPreviousContents("Delete", stave, staveNo, start, end);

  /* ideally we should shift marks left or right so as not to be
     within this group, but it's a lot easier just to remove them
     entirely -- maybe one day... */

  ClearMarks(mstave->music[staveNo], start, end);

  if (start || end) mstave->music[staveNo] = ItemListRemoveItems(start, end);

  if (!mstave->music[staveNo]) {
    mstave->music[staveNo] = NewItemList((Item *)clef);
  }

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  UndoAssertNewContents("Delete", stave, staveNo, start, start);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Copy */

void EditMenuCopy(Widget w, XtPointer a, XtPointer b)
{
  ItemList       list;
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  Item          *newItem;
  MarkList mlist;
  Mark *leftMark, *rightMark;

  Begin("EditMenuCopy");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  if (clipboard) DestroyItemList(clipboard);
  clipboard = NULL;

  if (start) list = iNext(start);
  else       list = mstave->music[mstave->sweep.stave];

  /*
  while (list) {

    newItem = (Item *)list->item->methods->clone(list->item);
    clipboard = (ItemList)Nconc(clipboard, NewItemList(newItem));
    if (list == end) break;
    list = iNext(list);
  }
  */

  clipboard = ItemListDuplicateSublist(list, end);

  clipboard = (ItemList)First
    (Nconc
     (NewItemList((Item *)NewBar(NULL, 0L, 1600, 2)), clipboard)); /* ugh */

  ((Bar *)clipboard->item)->group.start = iNext(First(clipboard));
  ((Bar *)clipboard->item)->group.end   = (ItemList)Last(clipboard);

  if (w) {
    StaveResetFormatting(stave, mstave->sweep.stave);
    StaveRefreshAsDisplayed(stave);
  }

  End;
}

/* }}} */
/* {{{ Cut */

/* for the time being lets cop out and implement Cut as a Copy and a Delete */

void EditMenuCut(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;

  Begin("EditMenuCut");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  EditMenuCopy   (NULL, a, b);
  EditMenuDelete (w,    a, b);

  UndoChangeLabelName("Cut", stave);

  End;
}

/* }}} */
/* {{{ Paste */

void EditMenuPaste(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  Item          *newItem;
  ItemList       newList = NULL;
  ItemList       endList;
  ItemList       list;

  Begin("EditMenuPaste");

  if (!stave || !mstave->sweep.swept || start != end) {
    XBell(display, 70);
    End;
  }

  if (!clipboard) {
    IssueMenuComplaint("The clipboard is empty.");
    End;
  }
#ifdef NOT_DEFINED
  for (list = iNext(First(clipboard));	/* miss out grouping item */
       list; list = iNext(list)) {

    newItem = (Item *)list->item->methods->clone(list->item);
    newList = (ItemList)Nconc(newList, NewItemList(newItem));
  }
#endif

  newList = ItemListDuplicateSublist
    (iNext(First(clipboard)), (ItemList)Last(clipboard));

  endList = (ItemList)Last(newList);
  UndoAssertPreviousContents("Paste", stave, staveNo, start, end);

  if (start)
    if (Next(start)) Insert(newList, Next(start));
    else Nconc(start, newList);
  else mstave->music[staveNo] = (ItemList)Nconc(newList, start);

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  UndoAssertNewContents("Paste", stave, staveNo, start, endList);
  StaveRefreshAsDisplayed(stave);

  XSync(display, False);
  StaveCursorExpose(stave);
  StaveCursorSelectSublist(stave, staveNo, endList, endList);

  End;
}

/* }}} */
/* {{{ Show Clipboard */

static void ClipboardCallback(Widget w, XtPointer client, XtPointer call)
{
  Begin("ClipboardCallback");

  *((Boolean *)client) = True;

  End;
}


void EditMenuShowClipboard(Widget w, XtPointer a, XtPointer b)
{
  Widget       clipShell;
  Widget       clipPane;
  Widget       clipMapForm;
  Widget       clipMapViewport;
  Widget       clipMapLabel;
  Widget       clipBottomBox;
  Widget       clipButton;
  Widget       scrollbar;
  Pixmap       clipMap = NULL;
  XPoint       op;
  Dimension    h;
  Dimension    width;
  int          sy;
  Boolean      done = False;
  XtAppContext context;
  MarkList     markList = NULL;

  Begin("EditMenuShowClipboard");

  clipShell     = XtCreatePopupShell
    ("Editor Clipboard", transientShellWidgetClass, topLevel, NULL, 0);

  clipPane      = YCreateWidget("Clipboard Pane", panedWidgetClass, clipShell);

  clipMapForm   = YCreateShadedWidget
    ("Clipboard Label Form", formWidgetClass, clipPane, LightShade);
  clipBottomBox = YCreateShadedWidget
    ("Clipboard Button Box",  boxWidgetClass, clipPane, MediumShade);

  YSetValue(clipMapForm,   XtNshowGrip, False);
  YSetValue(clipBottomBox, XtNshowGrip, False);

  clipMapViewport = YCreateWidget
    ("Clipboard Viewport", viewportWidgetClass, clipMapForm);
  clipMapLabel    = YCreateWidget
    ("Clipboard Contents", labelWidgetClass, clipMapViewport);
  clipButton      = YCreateCommand("Dismiss", clipBottomBox);

  if (clipboard) {

    width = GetBarWidth((Bar *)clipboard->item, (Bar *)clipboard->item);

    clipMap = XCreatePixmap
      (display, RootWindowOfScreen(XtScreen(topLevel)), width + 10,
       StaveHeight + 2 * StaveUpperGap,
       DefaultDepthOfScreen(XtScreen(topLevel)));

    XFillRectangle(display, clipMap, clearGC, 0, 0, width + 10,
		   StaveHeight + StaveUpperGap * 2);

    (void)DrawBar((Bar *)clipboard->item, (Bar *)clipboard->item,
		  0, clipMap, 5, StaveUpperGap, 0, width);
    CollateMarksInBar((Bar *)clipboard->item, &markList);
    DrawMarks(clipMap, StaveUpperGap, width, 0, &markList);
    DestroyList(markList);	/* not DestroyMarkList, the marks are shared */

    for (sy = 0 ; sy < StaveHeight; sy += NoteHeight + 1)
      XDrawLine(display, clipMap, drawingGC,
		4, StaveUpperGap + sy, width + 4, StaveUpperGap + sy);

    XDrawLine(display, clipMap, drawingGC,
	      4, StaveUpperGap, 4, StaveUpperGap + StaveHeight - 1);

    YSetValue(clipMapLabel, XtNbitmap, clipMap);

  } else {

    YSetValue(clipMapLabel, XtNlabel, "The clipboard is empty.");
  }

  XtAddCallback(clipButton, XtNcallback, ClipboardCallback, &done);

  XtRealizeWidget(clipShell);
  YGetValue(clipButton, XtNheight, &h);
  XtUnrealizeWidget(clipShell);

  YSetValue(clipBottomBox, XtNmax, h + 15);
  YSetValue(clipBottomBox, XtNmin, h + 15);

  XtRealizeWidget(clipShell);

  scrollbar = XtNameToWidget(clipMapViewport, "horizontal");
  if (scrollbar) YSetValue(scrollbar, XtNthumb, lightGreyMap);

  op = YPlacePopupAndWarp(clipShell, XtGrabExclusive, clipButton, clipButton);
  YAssertDialogueActions(clipShell, clipButton, clipButton, NULL);

  context = XtWidgetToApplicationContext(clipShell);
  while (!done || XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);

  if (op.x || op.y) (void) YPopPointerPosition();

  YPopdown(clipShell);
  YRetractDialogueActions(clipShell);
  XtDestroyWidget(clipShell);

  End;
}

/* }}} */
/* {{{ Select Bar, Select Staff */

void EditMenuSelectBar(Widget w, XtPointer a, XtPointer b)
{
  static XPoint p = { 0, 0 };
  Begin("EditMenuSelectBar");
  StaveCursorSelectBar(stave, p, False);
  End;
}


void EditMenuSelectStaff(Widget w, XtPointer a, XtPointer b)
{
  static XPoint p = { 0, 0 };
  Begin("EditMenuSelectStaff");
  StaveCursorSelectStaff(stave, p, False);
  End;
}

/* }}} */

