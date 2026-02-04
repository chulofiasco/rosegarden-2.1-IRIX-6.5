
/* MenuText.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Functions to handle actions from Words menu options */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "ItemList.h"
#include "Yawn.h"

#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Paned.h>

/* }}} */

/* {{{ Add Text */

static void TextMenuText(TextPosnTag tag)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  String         text;
  Item          *item;
  ItemList       newList;

  Begin("TextMenuTextAbove");

  if (!stave || !mstave->sweep.swept || start != end) {
    XBell(display, 70);
    End;
  }

  if (!(text = YGetUserInput
	(topLevel, "Enter text:",
	 NULL, YOrientHorizontal, tag == TextAboveBarLine ?
	 "Bar - Label" : "Text - Add Text options"))) End;

  item = (Item *)NewText(NULL, XtNewString(text), tag);
  newList = NewItemList(item);

  UndoAssertPreviousContents("Text", stave, staveNo, start, start);

  if (start)
    if (Next(start)) Insert(newList, Next(start));
    else Nconc(start, newList);
  else mstave->music[staveNo] =
    (ItemList)First(Nconc(newList, mstave->music[staveNo]));

  UndoAssertNewContents("Text", stave, staveNo, start, newList);

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}



void TextMenuTextAbove(Widget w, XtPointer a, XtPointer b)
{
  Begin("TextMenuTextAbove");

  TextMenuText(TextAboveStave);

  End;
}


void TextMenuTextAboveBig(Widget w, XtPointer a, XtPointer b)
{
  Begin("TextMenuTextAboveBig");

  TextMenuText(TextAboveStaveLarge);

  End;
}


void TextMenuTextBelow(Widget w, XtPointer a, XtPointer b)
{
  Begin("TextMenuTextBelow");

  TextMenuText(TextBelowStave);

  End;
}


void TextMenuTextBelowI(Widget w, XtPointer a, XtPointer b)
{
  Begin("TextMenuTextBelowI");

  TextMenuText(TextBelowStaveItalic);

  End;
}

/* }}} */
/* {{{ Add Dynamic Text, and Convert from I/O Texts */

String textDynamics[] = {
  "pppp", "ppp", "pp", "p", "mp", "mf", "f", "ff", "fff", "ffff"
};

int textDynamicCount = XtNumber(textDynamics);

void TextDynamicOK(Widget w, XtPointer a, XtPointer b)
{
  Begin("TextDynamicOK");

  *((int *)a) = 1;

  End;
}

void TextDynamicCancel(Widget w, XtPointer a, XtPointer b)
{
  Begin("TextDynamicCancel");

  *((int *)a) = 0;

  End;
}

void TextMenuDynamic(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  String         text;
  Item          *item;
  ItemList       newList;
  Widget         dlg, pane, topBox, label, option, bottomBox, ok, cancel, help;
  int            result = -1;
  int            i;
  static int     prevSelection = 5;
  Begin("TextMenuDynamic");

  dlg = XtCreatePopupShell("Dynamic", transientShellWidgetClass,
			   topLevel, NULL, 0);

  pane = YCreateWidget("Dynamic Pane", panedWidgetClass, dlg);

  topBox = YCreateShadedWidget("Dynamic Option Box", formWidgetClass,
			       pane, LightShade);

  label = YCreateLabel("Dynamic indication:", topBox);

  option = YCreateOptionMenu(topBox, textDynamics, textDynamicCount,
			     prevSelection, NULL, NULL);

  bottomBox = YCreateShadedWidget("Dynamic Bottom Box", boxWidgetClass,
				   pane, MediumShade);

  ok = YCreateCommand("OK", bottomBox);
  XtAddCallback(ok, XtNcallback, TextDynamicOK, (XtPointer)&result);
  cancel = YCreateCommand("Cancel", bottomBox);
  XtAddCallback(cancel, XtNcallback, TextDynamicCancel, (XtPointer)&result);

  YSetValue(XtParent(option), XtNfromHoriz, XtParent(label));

  if (appData.interlockWindow) {
    help = YCreateCommand("Help", bottomBox);
    XtAddCallback(help, XtNcallback, yHelpCallbackCallback,
		  (XtPointer)"Text - Dynamic");
  } else {
    help = NULL;
  }

  YPushPointerPosition();
  YPlacePopupAndWarp(dlg, XtGrabNonexclusive, ok, NULL);
  YAssertDialogueActions(dlg, ok, cancel, help);
  YFixOptionMenuLabel(option);

  while (result == -1 || XtAppPending(XtWidgetToApplicationContext(ok))) {
    XtAppProcessEvent(XtWidgetToApplicationContext(ok), XtIMAll);
  }

  i = YGetCurrentOption(option);

  YPopdown(dlg);
  YDestroyOptionMenu(option);
  XtDestroyWidget(dlg);
  YPopPointerPosition();

  if (result != 1) End;
  prevSelection = i;
  
  item = (Item *)NewText(NULL, XtNewString(textDynamics[i]), TextDynamic);
  newList = NewItemList(item);

  UndoAssertPreviousContents("Text", stave, staveNo, start, start);

  if (start)
    if (Next(start)) Insert(newList, Next(start));
    else Nconc(start, newList);
  else mstave->music[staveNo] =
    (ItemList)First(Nconc(newList, mstave->music[staveNo]));

  UndoAssertNewContents("Text", stave, staveNo, start, newList);

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);

  End;
}


/* called from IO.c */
void MaybeConvertDynamics(MajorStave sp, int dc)
{
  int i, j;
  Text *text;
  ItemList il;
  MajorStaveRec *mstave  = (MajorStaveRec *)sp;
  char message[1000];
  Begin("MaybeConvertDynamics");

  sprintf(message,
"\n  This is a Rosegarden 2.0 file.  Version 2.0 did not  \n\
  support the text-style `dynamic', but there %s %d textual  \n\
  marking%s in this file that look%s as though %s could be  \n\
  safely made into style `dynamic'.  Should I convert %s?  \n\n",
	  dc == 1 ? "is" : "are", dc, dc == 1 ? "" : "s", dc != 1 ? "" : "s",
	  dc == 1 ? "it" : "they", dc == 1 ? "it" : "them");

  if (YQuery(topLevel, message, 2, 1, 1,
	     "Yes", "No", "Converting Rosegarden 2.0 files") != 0) End;

  for (j = 0; j < mstave->staves; ++j) {

    for (il = (ItemList)First(mstave->music[j]); il; il = iNext(il)) {

      if (il->item->object_class == TextClass) {

	text = (Text *)il->item;
	if (text->text.position == TextBelowStave ||
	    text->text.position == TextBelowStaveItalic) {

	  for (i = 0; i < textDynamicCount; ++i) {
	    if (!strcmp(text->text.text, textDynamics[i])) break;
	  }

	  if (i < textDynamicCount - 1) {
	    text->text.position = TextDynamic;
	  }
	}
      }
    }
  }

  End;
}

/* }}} */
/* {{{ Remove Text */

void TextMenuUnlabelGroup(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;
  Boolean        haveText = False;

  Begin("TextMenuUnlabelGroup");

  if (!stave || !mstave->sweep.swept || !end || start == end) {
    XBell(display, 70);
    End;
  }

  if (start) list = iNext(start);
  else       list = mstave->music[staveNo];

  /* !!! a bit experimental */
  while (end && iNext(end) && IS_TEXT(end->item)) end = iNext(end);

  UndoAssertPreviousContents("Remove Text", stave, staveNo, start, end);

  while (list) {

    if (list->item->object_class == TextClass) {

      haveText = True;
      list = (ItemList)Remove(list);

    } else list = iNext(list);

    if (list && Prev(list) && end == iPrev(list)) break;
  }

  if (!haveText) {
    IssueMenuComplaint("There is no text within that group.");
    UndoCancel();
    End;
  }

  /* Wrong in the case where "end" was a Text object and was the last
     item in the staff */
  UndoAssertNewContents("Remove Text", stave, staveNo, start, end);

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}


void TextMenuUnlabelBar(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            staveNo = mstave->sweep.stave;
  int            barNo;
  StaveEltList   barList;
  ItemList       list, leftBound, right;
  Boolean        haveText = False;

  Begin("TextMenuUnlabelBar");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  if (!barList || !BarValid(barList->bars[staveNo])) {
    XBell(display, 70);
    End;
  }

  list = barList->bars[staveNo]->group.start;

  leftBound = iPrev(list);
  right = iNext(barList->bars[staveNo]->group.end);
  if (!right) right = (ItemList)Last(leftBound);
  UndoAssertPreviousContents("Remove Text", stave, staveNo, leftBound, right);

  while (list) {

    if (list->item->object_class == TextClass) {

      haveText = True;
      list = (ItemList)Remove(list);

    } else list = iNext(list);

    if (list && Prev(list) &&
	barList->bars[staveNo]->group.end == iPrev(list)) break;
  }

  if (!haveText) {
    IssueMenuComplaint("There is no text in that bar.");
    UndoCancel();
    End;
  }

  UndoAssertNewContents("Remove Text", stave, staveNo, leftBound, right);
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}


void TextMenuUnlabelStaff(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;
  Boolean        haveText = False;

  Begin("TextMenuUnlabelStaff");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  if (YQuery(topLevel,
	     "Are you sure you want to clear all the text in the staff?",
	     2, 1, 1, "Yes", "No", "Text - Clear Staff") != 0) End;

  list = mstave->music[staveNo];
  UndoAssertPreviousContents("Remove Text", stave, staveNo, NULL,
			     (ItemList)Last(mstave->music[staveNo]));

  while (list) {

    if (list->item->object_class == TextClass) {

      haveText = True;
      list = (ItemList)Remove(list);

    } else list = iNext(list);
  }

  if (!haveText) {
    IssueMenuComplaint("There is no text on that staff.");
    UndoCancel();
    End;
  }

  UndoAssertNewContents("Remove Text", stave, staveNo, NULL,
			(ItemList)Last(mstave->music[staveNo]));
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */

/* {{{ Label Bar */

void BarMenuLabel(Widget w, XtPointer a, XtPointer b)
{
  int            barNo;
  StaveEltList   barList;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("BarMenuLabel");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.to.left != mstave->sweep.from.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  if (!barList || !BarValid(barList->bars[mstave->sweep.stave])) {
    XBell(display, 70);
    End;
  }

  if (Prev(barList->bars[mstave->sweep.stave]->group.start))
    mstave->sweep.from.left = mstave->sweep.to.left =
      iPrev(barList->bars[mstave->sweep.stave]->group.start);
  else mstave->sweep.from.left = mstave->sweep.to.left = NULL;

  TextMenuText(TextAboveBarLine);
  UndoChangeLabelName("Label Bar", stave);

  End;
}

/* }}} */
/* {{{ Unlabel Bar */

void BarMenuUnlabel(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            staveNo = mstave->sweep.stave;
  int            barNo;
  StaveEltList   barList;
  ItemList       list, leftBound, right;
  Boolean        haveText = False;

  Begin("BarMenuUnlabel");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  for (barNo = 0, barList = mstave->bar_list;
       barList && barNo < mstave->sweep.from.bar->bar.number;
       barList = (StaveEltList)Next(barList), ++barNo);

  if (!barList || !BarValid(barList->bars[staveNo])) {
    XBell(display, 70);
    End;
  }

  list = barList->bars[staveNo]->group.start;

  leftBound = iPrev(list);
  right = iNext(barList->bars[staveNo]->group.end);
  if (!right) right = (ItemList)Last(leftBound);
  UndoAssertPreviousContents("Remove Label", stave, staveNo, leftBound, right);

  while (list) {

    if (list->item->object_class == TextClass &&
	((Text *)list->item)->text.position == TextAboveBarLine) {

      haveText = True;

      if (mstave->music[staveNo] == list)
	mstave->music[staveNo] = iNext(list);
      list = (ItemList)Remove(list);

    } else list = iNext(list);

    if (list && Prev(list) &&
	barList->bars[staveNo]->group.end == iPrev(list)) break;
  }

  if (!haveText) {
    IssueMenuComplaint("That bar isn't labelled.");
    UndoCancel();
    End;
  }

  UndoAssertNewContents("Remove Label", stave, staveNo, leftBound, right);
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */

