
/* MenuStave.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Functions to handle actions from Stave menu options */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "ItemList.h"
#include "Yawn.h"
#include <X11/Xaw/Label.h>

/* }}} */

/* {{{ New Staff */

/* NewStave works by destroying the old stave record and constructing
   a new one; DeleteStave works by modifying the stave record in place
   (which is probably the better approach). */

void StaveAddANewStave(MajorStave stave, int number)
{
  int            i;
  int            off;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;
  MajorStave     oldStave, newStave;
  ItemList      *newMusic;
  String        *newNames;
  ItemList       list;

  Begin("StaveAddANewStave");

  newMusic = (ItemList *)XtMalloc((mstave->staves + 1) * sizeof(ItemList));
  newNames = (String   *)XtMalloc((mstave->staves + 1) * sizeof(String));

  off = (number == 0 ? 1 : 0);

  for (i = 0; i < mstave->staves; ++i) {

    if (off == 0 && i >= number) off = 1;

    newMusic[i + off] = mstave->music[i];
    newNames[i + off] = XtNewString(mstave->names[i]);
  }

  list = NewItemList((Item *)NewClef(NULL, TrebleClef));
  
  newMusic[number] = list;
  newNames[number] = XtNewString("unnamed");
  newStave = NewStave(mstave->staves + 1, newMusic);

#define NEW_STAFF_NO(i,n) ((n)==0 ? (i)+1 : \
			   ((i) < (n) ? (i) : (i)+1))

  for (i = 0; i < mstave->staves; ++i) {
    StaveSetEndBarTags(newStave, NEW_STAFF_NO(i, number),
		       mstave->bar_tags[i].precedes,
		       mstave->bar_tags[i].follows);
    ((MajorStaveRec *)newStave)->midi_patches[NEW_STAFF_NO(i, number)] =
      mstave->midi_patches[i];
  }
  
  oldStave = stave;		/* make sure "stave" is set okay in case */
  stave = newStave;		/* any stupid functions try to access it */

  for (i = 0; i < mstave->staves; ++i) {
    StaveResetFormatting(stave, NEW_STAFF_NO(i, number));
    StaveFormatBars(stave, NEW_STAFF_NO(i, number), -1);
  }

  /* copy time sigs across */

  {
    StaveEltList ol= (StaveEltList)First(mstave->bar_list);
    StaveEltList nl= (StaveEltList)First(((MajorStaveRec *)newStave)->bar_list);

    while (ol && nl) {

      for (i = 0; i < mstave->staves; ++i) {

	int j = NEW_STAFF_NO(i, number);

	if (ol->bars[i] && nl->bars[j]) {
	  (void)NewTimeSignature
	    (&nl->bars[j]->bar.time, ol->bars[i]->bar.time.numerator,
	     ol->bars[i]->bar.time.denominator);
	}
      }

      ol = (StaveEltList)Next(ol);
      nl = (StaveEltList)Next(nl);
    }
  }

#undef NEW_STAFF_NO
  
  XtFree((void *)mstave->music);
  StaveDestroy(oldStave, False);

  /* mstave still points to the old stave rec, and it's this pointer
     that the File menu tracks, so we must maintain it */

  memcpy((void *)mstave, (void *)newStave, sizeof(MajorStaveRec));
  stave = (MajorStave)mstave;	/* restore global stave pointer */

  for (i = 0; i < mstave->staves; ++i) {
    StaveRenameStave(stave, i, newNames[i]);
    XtFree(newNames[i]);
  }

  XtFree((void *)newNames);

  End;
}


void StaveMenuNewStave(Widget w, XtPointer a, XtPointer b)
{
  int            i;
  int            posn;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveMenuNewStave");

  if (mstave->sweep.swept) {

    posn = YQuery(topLevel, "Where should the new staff go?", 4, 3, 3,
		  "First", "Last", "After current staff", "Cancel",
		  "Stave - Add Staff");

    if (posn == 3) End;

  } else {

    posn = YQuery(topLevel, "Where should the new staff go?", 3, 2, 2,
		  "First", "Last", "Cancel", "Stave - Add Staff");

    if (posn == 2) End;
  }

  if (!UndoInvalidate("Adding a staff", stave, "Stave - Add Staff")) End;

  if (posn == 0) i = 0;
  else if (posn == 1) i = mstave->staves;
  else i = mstave->sweep.stave + 1;

  StaveAddANewStave(stave, i);

  for (i = 0; i < mstave->staves; ++i) {
    StaveResetFormatting(stave, i);
    StaveFormatBars(stave, i, -1);
  }

  staveMoved = True;
  StaveRefresh(stave, -1);
  FileMenuMarkChanged(stave, True);

  End;
}

/* }}} */
/* {{{ Empty Staff */

void StaveMenuEmptyStave(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveMenuEmptyStave");

  if (!mstave->sweep.swept) {
    XBell(display, 70);
    End;
  }

  if (!mstave->music[mstave->sweep.stave]) {
    IssueMenuComplaint("This stave is empty anyway.");
    End;
  }

  if (!Next(mstave->music[mstave->sweep.stave])) {
    IssueMenuComplaint("You can't remove the only item in a staff.");
    End;
  }

  if (YQuery(topLevel, "Are you sure you want to empty this whole staff?",
	     2, 1, 1, "Continue", "Cancel", "Stave - Empty Staff") != 0) End;

  /*
  if (!UndoInvalidate("Emptying a staff", stave, "Stave - Empty Staff")) End;
  */
  mstave->sweep.from.left = mstave->music[mstave->sweep.stave];
  mstave->sweep.to.left   = (ItemList)Last(mstave->music[mstave->sweep.stave]);
  mstave->sweep.swept     = True;

  EditMenuDelete(w, a, b);
  UndoChangeLabelName("Empty Staff", stave);

  staveMoved = True;
  StaveResetFormatting(stave, mstave->sweep.stave);
  StaveRefresh(stave, -1);
  FileMenuMarkChanged(stave, True);

  End;
}

/* }}} */
/* {{{ Delete staff */

void StaveMenuDeleteStave(Widget w, XtPointer a, XtPointer b)
{
  int            i;
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveMenuDeleteStave");

  if (!mstave->sweep.swept) {
    XBell(display, 70);
    End;
  }

  if (mstave->staves <= 1) {
    IssueMenuComplaint("You can't delete the only remaining staff.");
    End;
  }
  /*
  if (YQuery(topLevel, "Are you sure you want to delete this staff?", 2, 1, 1,
	     "Continue", "Cancel", "Stave - Delete Staff") != 0) End;
	     */

  if (!UndoInvalidate("Deleting a staff", stave, "Stave - Delete Staff")) End;

  /* we just shuffle the old stave recs down a place in the staff
     array, without freeing them -- a memory leak, but a bounded one */

  if (mstave->sweep.stave < mstave->staves-1) {

    int rs = mstave->sweep.stave;
    int copyNo = mstave->staves - rs - 1;
    StaveEltList elist = (StaveEltList)First(mstave->bar_list);
    
    memcpy((void *)&mstave->music[rs], (void *)&mstave->music[rs+1],
	   copyNo * sizeof(ItemList));

    memcpy((void *)&mstave->names[rs], (void *)&mstave->names[rs+1],
	   copyNo * sizeof(String));

    memcpy((void *)&mstave->name_lengths[rs],
	   (void *)&mstave->name_lengths[rs+1], copyNo * sizeof(int));

    while(elist) {

      if (elist->bars[rs]) DestroyBar(elist->bars[rs]);

      memcpy((void *)&elist->bars[rs],
	     (void *)&elist->bars[rs+1], copyNo * sizeof(Bar *));

      elist = (StaveEltList)Next(elist);
    }
  }

  --mstave->staves;

  for (i = 0; i < mstave->staves; ++i) {
    StaveResetFormatting(stave, i);
    StaveFormatBars(stave, i, -1);
  }

  staveMoved = True;
  StaveRefresh(stave, -1);
  FileMenuMarkChanged(stave, True);

  End;
}

/* }}} */
/* {{{ Rename */

void StaveMenuRenameStave(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec  *mstave  = (MajorStaveRec *)stave;
  int             staveNo = mstave->sweep.stave;
  String          name;

  Begin("StaveMenuRenameStave");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  if (!(name = YGetUserInput
	(topLevel, "Name:", NULL, YOrientHorizontal, "Stave - Rename Staff")))
    End;

  FileMenuMarkChanged(stave, True);
  StaveRenameStave(stave, staveNo, name);
  End;
}

/* }}} */
/* {{{ MIDI Patch stuff */

void StaveMenuAssignPatch(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec  *mstave  = (MajorStaveRec *)stave;
  int             staveNo = mstave->sweep.stave;
  String          name;
  char            deft[10];

  Begin("StaveMenuAssignPatch");

  if (!stave || !mstave->sweep.swept ||
      mstave->sweep.from.left != mstave->sweep.to.left) {
    XBell(display, 70);
    End;
  }

  sprintf(deft, "%d\n", mstave->midi_patches[staveNo]);

  if (!(name = YGetUserInput
	(topLevel, "Patch number:", deft, YOrientHorizontal,
	 "Stave - Assign MIDI Patch")))
    End;

  FileMenuMarkChanged(stave, True);
  StaveSetMIDIPatch(stave, staveNo, atoi(name));
  StaveRefreshAsDisplayed(stave);

  End;
}


extern YMenuElement staveMenu[];
extern int staveMenuShowPatchesIndex;
Boolean ShowingMIDIPatches = True;

void StaveMenuShowPatches(Widget ww, XtPointer a, XtPointer b)
{
  Widget w;
  Begin("StaveMenuShowPatches");

  ShowingMIDIPatches = !ShowingMIDIPatches;
  w = staveMenu[staveMenuShowPatchesIndex].widget;
  if (w) YSetValue(w, XtNleftBitmap,
		   ShowingMIDIPatches ? yToggleOnMap : yToggleOffMap);
  StaveRefreshAsDisplayed(stave);

  End;
}  

/* }}} */
/* {{{ Fill To End */

/* Could possibly learn from MidiMakeRestList in MidiIn.c */

static int FillRestLength(TimeSignature *time, NoteTag *tag, Boolean *dotted)
{
  int i;
  Begin("FillRestLength");

  if (time->numerator == 6 && time->denominator == 8) {
    *dotted = True; *tag = Crotchet; Return(2);
  } else {
    *dotted = False;

    for (i = LongestNote; i >= 0; --i) {
      if (MTimeToNumber(time->bar_length) % TagToNumber(i, False) == 0) {
	*tag = i;
	Return(MTimeToNumber(time->bar_length) / TagToNumber(i, False));
      }
    }
  }
  
  *tag = Hemidemisemiquaver; *dotted = False;
  Return(MTimeToNumber(time->bar_length));	/* !! */
}


void FillStaffsToEnd(MajorStave sp, int first, int last)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  StaveEltList elist;
  StaveEltList subList;
  TimeSignature *time;
  Boolean something, changed;
  NoteTag tag;
  Boolean dotted;
  int i, n;
  Begin("FillStaffsToEnd");

  if (last < first || mstave->staves < 2) End;

  StaveReformatEverything(sp);
  changed = False;

  for (elist = (StaveEltList)First(mstave->bar_list); elist;
       elist = (StaveEltList)Next(elist)) {

    something = False;

    for (i = 0; i < mstave->staves; ++i) {
      if (BarValid(elist->bars[i])) { something = True; break; }
    }

    if (!something) break;

    for (i = first; i <= last; ++i) {

      if (BarValid(elist->bars[i])) continue;

      if (elist->bars[i]) {
	time = &elist->bars[i]->bar.time;
      } else {
	for (subList = elist, time = 0; subList;
	     subList = (StaveEltList)Prev(subList)) {
	  if (subList->bars[i]) {
	    time = &subList->bars[i]->bar.time;
	    break;
	  }
	}

	if (!time) time = &defaultTimeSignature;
      }

      n = FillRestLength(time, &tag, &dotted);

      while (n > 0) {
	mstave->music[i] =
	  (ItemList)Nconc(mstave->music[i],
			  NewItemList((Item *)NewRest(NULL, tag, dotted)));
	changed = True;
	--n;
      }
    }
  }

  if (changed) {
    for (i = 0; i < mstave->staves; ++i) {
      mstave->music[i] = (ItemList)First(mstave->music[i]);
    }
  }

  StaveReformatEverything(sp);
  End;
}


void StaveMenuFillToEnd(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;
  ItemList leftBound;
  int first, last;
  int i;

  Begin("StaveMenuFillToEnd");

  if (mstave->staves < 2) {
    YQuery(topLevel,
	   "Fill to End doesn't make sense when there's only one staff",
	   1, 0, 0, "OK", "Stave - Fill to End");
    End;
  }
  
  if (!mstave->sweep.swept) {
    if (YQuery(topLevel, "Fill up to the end of every staff with rests?",
	       2, 1, 1, "OK", "Cancel", "Stave - Fill to End") != 0) End;
    first = 0;
    last = mstave->staves - 1;
  } else {
    i = YQuery(topLevel, NULL, 3, 2, 2, "Fill this staff only",
	       "Fill all staffs", "Cancel", "Stave - Fill to End");
    if (i == 2) End;
    else if (i == 0) first = last = mstave->sweep.stave;
    else {
      first = 0;
      last = mstave->staves - 1;
    }
  }

  if (first != last) {
    if (!UndoInvalidate("Filling all staffs with rests", stave,
			"Stave - Fill to End")) End;
  } else {
    leftBound = (ItemList)Last(mstave->music[first]);
    UndoAssertPreviousContents("Fill Staff", stave, first,
			       leftBound, leftBound);
  }

  FillStaffsToEnd(stave, first, last);

  if (first == last) {
    UndoAssertNewContents("Fill Staff", stave, first, leftBound,
			  (ItemList)Last(mstave->music[first]));
  }

  staveMoved = True;
  StaveRefresh(stave, -1);
  FileMenuMarkChanged(stave, True);

  End;
}

/* }}} */
/* {{{ Connect and Disconnect */

void StaveMenuConnectBelow(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveMenuConnectBelow");

  if (!mstave->sweep.swept || mstave->sweep.stave >= mstave->staves-1) {
    XBell(display, 70);
    End;
  }

  mstave->connected_down[mstave->sweep.stave] = True;

  staveMoved = True;
  StaveRefresh(stave, -1);
  FileMenuMarkChanged(stave, True);

  End;
}


void StaveMenuDisconnect(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave = (MajorStaveRec *)stave;

  Begin("StaveMenuDisconnect");

  if (!mstave->sweep.swept) {
    XBell(display, 70);
    End;
  }

  mstave->connected_down[mstave->sweep.stave] = False;
  if (mstave->sweep.stave > 0)
    mstave->connected_down[mstave->sweep.stave-1] = False;

  staveMoved = True;
  StaveRefresh(stave, -1);
  FileMenuMarkChanged(stave, True);

  End;
}

/* }}} */

