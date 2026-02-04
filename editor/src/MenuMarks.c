
/* MenuMarks.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Functions dealing with the actions available from the Mark menu */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "ItemList.h"

#include "Yawn.h"

/* }}} */

/* {{{ Mark */

void MarkMenuMark(Widget w, XtPointer client, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;
  Boolean        haveChord = False;
  ChordMods      mods;
  int            i;
  YMenuElement  *element = (YMenuElement *)client;

  Begin("MarkMenuMark");

  if (!stave || !mstave->sweep.swept || start == end) {
    XBell(display, 70);
    End;
  }

  for (i = 0; i < chordModVisualCount; ++i)
    if (!strncasecmp(element->label,
		     chordModVisuals[i].name, strlen(element->label))) break;

  if (i >= chordModVisualCount) {
    IssueMenuComplaint("Internal problem -- no such modifier. Sorry...");
    End;
  }

  mods = chordModVisuals[i].type;

  if (start) list = iNext(start);
  else       list = mstave->music[staveNo];

  UndoAssertPreviousContents("Mark", stave, staveNo, start, end);

  while (list) {

    if (list->item->object_class == ChordClass) {
      haveChord = True;
      ((Chord *)list->item)->chord.modifiers |= mods;
    }

    if (list == end) break;
    list = iNext(list);
  }

  if (!haveChord) {
    IssueMenuComplaint("You can only add marks to chords.");
    UndoCancel();
    End;
  }

  UndoAssertNewContents("Mark", stave, staveNo, start, end);
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */
/* {{{ Unmark */

/*
  -- I think you're just suffering from post-university depression.

  -- But that makes PUD.  I can't be suffering from that.

  -- Post-university blues, then?  Look, I don't really care what you
     did.  I'm a bit worried about why you think it's so... serious.
     I'm just concerned about your state of mind.  When you phoned me
     up like that, I mean... it was scary.  I was just sitting around
     watching a video with everyone and you were telling me all this
     stuff.
*/

  
void MarkMenuUnmark(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       end     = mstave->sweep.to.left;
  int            staveNo = mstave->sweep.stave;
  ItemList       list;
  Boolean        haveChord = False;

  Begin("MarkMenuMark");

  if (!stave || !mstave->sweep.swept || start == end) {
    XBell(display, 70);
    End;
  }

  if (start) list = iNext(start);
  else       list = mstave->music[staveNo];

  UndoAssertPreviousContents("Unmark", stave, staveNo, start, end);

  while(list) {

    if (list->item->object_class == ChordClass) {
      haveChord = True;
      ((Chord *)list->item)->chord.modifiers = ModNone;
    }

    if (list == end) break;
    list = iNext(list);
  }

  if (!haveChord) {
    IssueMenuComplaint("You can only remove marks from chords.");
    UndoCancel();
    End;
  }

  UndoAssertNewContents("Unmark", stave, staveNo, start, end);
  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, staveNo);
  StaveRefreshAsDisplayed(stave);
  End;
}

/* }}} */

