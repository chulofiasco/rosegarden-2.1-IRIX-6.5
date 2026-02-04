/* This file should probably be using ItemListDuplicateSublist in
   various places, so as to retain some of the Marks */

/* {{{ Includes */

#include "General.h"
#include "Stave.h"
#include "StavePrivate.h"
#include "Undo.h"
#include "ItemList.h"
#include "Marks.h"
#include "Menu.h"

#include <X11/Xaw/Label.h>

/* }}} */

/* {{{ Structures and declarations */

static Widget undoMenuButton = 0;
static Widget redoMenuButton = 0;

typedef struct _UndoSection {
  long start;			/* counters */
  long end;
  ItemList list;		/* whole snipped list */
} UndoSection;

typedef struct _UndoRec {
  Boolean        valid;
  MajorStaveRec *stave;
  int            staff;
  UndoSection    before;
  UndoSection    after;
  ItemList       startWas;	/* just to compare for "prev" & "new" calls */
  String         name;
} UndoRec, *Undo;

typedef struct _UndoListElement {
  ListElement   typeless;
  Undo          undo;
} UndoListElement, *UndoList;

typedef struct _UndoStackRec {
  UndoList firstUndo;
  UndoList nextUndo;
  UndoList nextRedo;
  int      undoCount;
} UndoStackRec;			/* UndoStack is opaque in StavePrivate.h */

static Undo undo = 0;		/* in construction, not yet in list */

static unsigned long undoCountMax = 20;

/* }}} */
/* {{{ Basic methods */

static UndoList NewUndoList(Undo u)
{
  UndoList list;
  Begin("NewUndoList");
  list = (UndoList)NewList(sizeof(UndoListElement));
  list->undo = u;
  Return(list);
}

void DestroyUndo(Undo u)
{
  if (!u) return;
  if (u->name) XtFree(u->name);
  if (u->before.list) DestroyItemList(u->before.list);
  if (u->after.list)  DestroyItemList(u->after.list);
  u->valid = False;
  XtFree(u);
}


UndoStack UndoCreateUndoStack()
{
  UndoStackRec *u;
  Begin("UndoCreateUndoStack");

  u = (UndoStackRec *)XtMalloc(sizeof(UndoStackRec));
  u->firstUndo = NULL;
  u->nextUndo  = NULL;
  u->nextRedo  = NULL;
  u->undoCount = 0;

  Return(u);
}

void UndoDestroyUndoStack(UndoStack uS)
{
  UndoStackRec *u = (UndoStackRec *)uS;
  Begin("UndoDestroyUndoStack");

  while (u->firstUndo) {
    DestroyUndo(u->firstUndo->undo);
    u->firstUndo = (UndoList)Remove(u->firstUndo);
  }

  XtFree(uS);

  End;
}

/* }}} */
/* {{{ Menu labels, &c */

void UndoChangeLabelNames(String uname, String rname)
{
  char s[100];
  Begin("UndoChangeLabelNames");

  if (uname) {
    sprintf(s, "Undo %s", uname);
    YSetValue(undoMenuButton, XtNlabel, s);
    LeaveMenuMode(UndoUnavailableMode);
  } else {
    YSetValue(undoMenuButton, XtNlabel, "Undo");
    EnterMenuMode(UndoUnavailableMode);
  }

  if (rname) {
    sprintf(s, "Redo %s", rname);
    YSetValue(redoMenuButton, XtNlabel, s);
    LeaveMenuMode(RedoUnavailableMode);
  } else {
    YSetValue(redoMenuButton, XtNlabel, "Redo");
    EnterMenuMode(RedoUnavailableMode);
  }

  End;
}


void UndoSetUndoMenuButtons(Widget u, Widget r)
{
  Begin("UndoSetUndoMenuButton");
  undoMenuButton = u;
  redoMenuButton = r;
  End;
}


void UndoChangeLabelName(String name, MajorStave stave) /* for app to call */
{
  char s[100];
  UndoStackRec *uS = (UndoStackRec *)((MajorStaveRec *)stave)->undo;
  Begin("UndoChangeLabelName");

  sprintf(s, "Undo %s", name);
  YSetValue(undoMenuButton, XtNlabel, s);

  if (uS->nextUndo) {
    if (uS->nextUndo->undo->name) XtFree(uS->nextUndo->undo->name);
    uS->nextUndo->undo->name = XtNewString(name);
  }

  End;
}


void UndoInstallLabelsForStave(MajorStave stave)
{
  UndoStackRec *uS;
  Begin("UndoInstallLabelsForStave");

  if (!stave) UndoChangeLabelNames(NULL, NULL);
  else {

    uS = (UndoStackRec *)((MajorStaveRec *)stave)->undo;
    
    UndoChangeLabelNames(uS->nextUndo ? uS->nextUndo->undo->name : NULL,
			 uS->nextRedo ? uS->nextRedo->undo->name : NULL);
  }

  End;
}

/* }}} */
/* {{{ Setting up for an undo */

Boolean UndoSetUndoSection(UndoSection *s,
			   ItemList first, ItemList start, ItemList end)
{
  long i, j;
  Begin("UndoSetUndoSection");

  if (start == end) {
    s->list = NULL;

  } else {

    s->list = ItemListDuplicateSublist(start ? iNext(start) : first, end);

  }

  for (i = 0; start; start = iPrev(start)) ++i;
  for (j = 0;   end;   end = iPrev(end))   ++j;

  if (i > j) {
    fprintf(stderr,"Warning: start later than end in UndoSetUndoSection\n");
    Return(False);
  }

  s->start = i;
  s->end   = j;

  Return(True);
}


void UndoAssertPreviousContents(String name, MajorStave stave, int staffNo,
				ItemList start, ItemList end)
{
  Begin("UndoAssertPreviousContents");

  if (undo) DestroyUndo(undo);
  undo = (UndoRec *)XtMalloc(sizeof(UndoRec));

  undo->stave       = (MajorStaveRec *)stave;
  undo->staff       = staffNo;
  undo->name        = XtNewString(name);
  undo->valid       = False;
  undo->startWas    = start;
  undo->before.list = NULL;
  undo->after.list  = NULL;

  undo->before.start = undo->before.end = -1;
  undo->after.start  = undo->after.end = -1;

  if (!end) {
    if (start) fprintf(stderr, "Warning: NULL end (but not start) in "
		       "UndoAssertPreviousContents\n");
    End;
  }

  if (!UndoSetUndoSection(&undo->before,
			  undo->stave->music[undo->staff], start, end)) {
    fprintf(stderr, "Warning: "
	    "UndoSetUndoSection failed in UndoAssertPreviousContents\n");
    End;
  }

  End;
}
  

void UndoAssertNewContents(String name, MajorStave stave, int staffNo,
			   ItemList start, ItemList end)
{
  UndoList temp;
  UndoStackRec *uS = (UndoStackRec *)((MajorStaveRec *)stave)->undo;
  Begin("UndoAssertNewContents");

  if (!undo || stave != undo->stave || staffNo != undo->staff || !undo->name) {
    fprintf(stderr, "Warning: UndoAssertNewContents called before "
	    "UndoAssertPreviousContents\n");
    End;
  }

  if (strcmp(name, undo->name)) {
    fprintf(stderr, "Warning: UndoAssertPreviousContents called with operation"
	    " name \"%s\", but\nUndoAssertNewContents called with \"%s\"\n",
	    undo->name, name);
    End;
  }

  if (start != undo->startWas) {
    fprintf(stderr,
	    "Warning: UndoAssertNewContents called with invalid start\n");
    End;
  }

  if (!UndoSetUndoSection(&undo->after,
			  undo->stave->music[undo->staff], start, end)) {
    fprintf(stderr,
	    "Warning: UndoSetUndoSection failed in UndoAssertNewContents\n");
    End;
  }

  undo->valid = True;
  UndoChangeLabelNames(name, NULL);

  /* append this Undo into the list, removing any alternative Redo branch */

  if (uS->nextUndo) temp = (UndoList)Next(uS->nextUndo);
  else temp = uS->firstUndo;

  while (temp) {
    UndoList temp2 = (UndoList)Next(temp);
    DestroyUndo(temp->undo);
    Remove(temp);
    --uS->undoCount;
    temp = temp2;
  }

  uS->nextUndo = (UndoList)Last(Nconc(uS->nextUndo, NewUndoList(undo)));
  uS->firstUndo = (UndoList)First(uS->nextUndo);
  uS->nextRedo = NULL;
  undo = NULL;

  /* Try to avoid wasting too much memory -- remove an item from the
     start if we have too many */

  if (++uS->undoCount > undoCountMax) {

    if (!uS->firstUndo || uS->undoCount != Length(uS->firstUndo)) {
      fprintf(stderr, "Warning: UndoCount != Length(UndoList) in "
	      "UndoAssertNewContents [%d!=%d]\n",
	      uS->undoCount, Length(uS->firstUndo));
      uS->undoCount = Length(uS->firstUndo);
    }

    if (uS->firstUndo) {

      if (uS->nextUndo == uS->firstUndo) {
	uS->nextUndo = (UndoList)Next(uS->nextUndo); 
      }
      if (uS->nextRedo == uS->firstUndo) {
	uS->nextRedo = (UndoList)Next(uS->nextRedo); 
      }

      uS->firstUndo = (UndoList)Remove(uS->firstUndo);
      --uS->undoCount;
    }
  }

  End;
}

/* }}} */
/* {{{ Undo */

void EditMenuUndo(Widget w, XtPointer a, XtPointer b)
{
  long i;
  Undo u;
  ItemList sList, eList;
  UndoStackRec *uS = (UndoStackRec *)((MajorStaveRec *)stave)->undo;
  Begin("EditMenuUndo");

  if (!uS->nextUndo || !uS->nextUndo->undo->valid ||
      !uS->nextUndo->undo->stave) {
    XBell(display, 70);
    End;
  }

  u = uS->nextUndo->undo;
  /*  fprintf(stderr, "UNDO -- start = %ld, end = %ld\n",
	  u->after.start, u->after.end);*/

  /* first, find the left bound for the whole Undo operation */

  i = 0; sList = NULL;

  while (i < u->after.start) {	/* should be u->after.start==u->before.start */

    sList = (sList ? iNext(sList) : u->stave->music[u->staff]);
    ++i;

    if (!sList) {
      fprintf(stderr, "Undo: Error: start of insertion after end of staff\n");
      End;
    }
  }

  /* now delete the bits we'd added for the last operation */

  if (u->after.start != u->after.end) {

    eList = sList;

    while (i < u->after.end) {

      eList = (eList ? iNext(eList) : u->stave->music[u->staff]);
      ++i;

      if (!eList) {
	fprintf(stderr, "Undo: Error: end of insertion after end of staff\n");
	End;
      }
    }

    /* now "sList" is prev of start of bit we need to cut, and "eList"
       is prev of end of bit we need to cut */

    ClearMarks(u->stave->music[u->staff], sList, eList);
    u->stave->music[u->staff] = ItemListRemoveItems(sList, eList);
  }

  /* finally insert the bits we'd removed before the last operation */

  if (u->before.list) {

    if (sList) {
      if (iNext(sList)) {
	Insert(ItemListDuplicate(u->before.list), iNext(sList));
      } else {
	Nconc(sList, ItemListDuplicate(u->before.list));
      }
    } else {
      u->stave->music[u->staff] = (ItemList)First(Nconc
	(ItemListDuplicate(u->before.list), u->stave->music[u->staff]));
    }
  }

  FileMenuMarkChanged(u->stave, True);
  StaveResetFormatting(u->stave, u->staff);
  StaveRefreshAsDisplayed(u->stave);

  uS->nextUndo = (UndoList)Prev(uS->nextUndo);
  uS->nextRedo = uS->nextUndo ? (UndoList)Next(uS->nextUndo) : uS->firstUndo;

  if ((uS->nextUndo && (!uS->nextUndo->undo || !uS->nextUndo->undo->name)) ||
      (uS->nextRedo && (!uS->nextRedo->undo || !uS->nextRedo->undo->name))) {
    fprintf(stderr, "Undo: Error: now pointing to invalid Undo for "
	    "nextUndo or nextRedo\n");
    UndoChangeLabelNames(NULL, NULL);
    End;
  }
    
  UndoChangeLabelNames(uS->nextUndo ? uS->nextUndo->undo->name : NULL,
		       uS->nextRedo ? uS->nextRedo->undo->name : NULL);

  End;
}

/* }}} */
/* {{{ Redo */

/* just like undo backwards! */

void EditMenuRedo(Widget w, XtPointer a, XtPointer b)
{
  long i;
  Undo u;
  ItemList sList, eList;
  UndoStackRec *uS = (UndoStackRec *)((MajorStaveRec *)stave)->undo;
  Begin("EditMenuRedo");

  if (!uS->nextRedo || !uS->nextRedo->undo->valid ||
      !uS->nextRedo->undo->stave) {
    XBell(display, 70);
    End;
  }

  u = uS->nextRedo->undo;
  /*  fprintf(stderr, "REDO -- start = %ld, end = %ld\n",
	  u->before.start, u->before.end);*/

  /* first, find the left bound for the whole Undo operation */

  i = 0; sList = NULL;

  while (i < u->before.start) {

    sList = (sList ? iNext(sList) : u->stave->music[u->staff]);
    ++i;

    if (!sList) {
      fprintf(stderr, "Undo: Error: start of insertion after end of staff\n");
      End;
    }
  }

  /* now delete the bits we'd deleted for this operation first time around */

  if (u->before.start != u->before.end) {

    eList = sList;

    while (i < u->before.end) {

      eList = (eList ? iNext(eList) : u->stave->music[u->staff]);
      ++i;

      if (!eList) {
	fprintf(stderr, "Undo: Error: end of insertion after end of staff\n");
	End;
      }
    }

    /* now "sList" is prev of start of bit we need to cut, and "eList"
       is prev of end of bit we need to cut */

    ClearMarks(u->stave->music[u->staff], sList, eList);
    u->stave->music[u->staff] = ItemListRemoveItems(sList, eList);
  }

  /* finally insert the bits we'd added for this operation first time */

  if (u->after.list) {

    if (sList) {
      if (iNext(sList)) {
	Insert(ItemListDuplicate(u->after.list), iNext(sList));
      } else {
	Nconc(sList, ItemListDuplicate(u->after.list));
      }
    } else {
      u->stave->music[u->staff] = (ItemList)First(Nconc
	(ItemListDuplicate(u->after.list), u->stave->music[u->staff]));
    }
  }

  FileMenuMarkChanged(u->stave, True);
  StaveResetFormatting(u->stave, u->staff);
  StaveRefreshAsDisplayed(u->stave);

  uS->nextRedo = (UndoList)Next(uS->nextRedo);
  uS->nextUndo =
    uS->nextRedo ? (UndoList)Prev(uS->nextRedo) : (UndoList)Last(uS->firstUndo);

  if ((uS->nextUndo && (!uS->nextUndo->undo || !uS->nextUndo->undo->name)) ||
      (uS->nextRedo && (!uS->nextRedo->undo || !uS->nextRedo->undo->name))) {
    fprintf(stderr, "Redo: Error: now pointing to invalid Undo for "
	    "nextUndo or nextRedo\n");
    UndoChangeLabelNames(NULL, NULL);
    End;
  }

  UndoChangeLabelNames(uS->nextUndo ? uS->nextUndo->undo->name : NULL,
		       uS->nextRedo ? uS->nextRedo->undo->name : NULL);

  End;
}

/* }}} */
/* {{{ Cancel & invalidate */

void UndoCancel(void)
{
  if (undo) DestroyUndo(undo);
  undo = NULL;
}


Boolean UndoInvalidate(String name, MajorStave stave, String helpTag)
{
  String message;
  UndoStackRec *uS = (UndoStackRec *)((MajorStaveRec *)stave)->undo;
  Begin("UndoInvalidate");

  message = (String)XtMalloc(strlen(name) + 100);
  sprintf(message, "%s cannot be undone.\nProceed?", name);

  if (YQuery(topLevel, message, 2, 0, 1, "Ok", "Cancel", helpTag)) {
    XtFree(message);
    Return(False);
  }

  /* remove the entire Undo list! */

  while (uS->firstUndo) {
    DestroyUndo(uS->firstUndo->undo);
    uS->firstUndo = (UndoList)Remove(uS->firstUndo);
  }

  uS->nextUndo = NULL;
  uS->nextRedo = NULL;
  uS->undoCount = 0L;

  UndoChangeLabelNames(NULL, NULL);
  XtFree(message);
  Return(True);
}

/* }}} */
