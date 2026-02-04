
#ifndef _MUSIC_UNDO_H_
#define _MUSIC_UNDO_H_

#include "Stave.h"

/* Undo only available for operations that affect contiguous items on
   one staff only (for now).

   To define an undo operation: before you change whatever-it-is, call
   UndoAssertPreviousContents giving operation name, stave, staff
   number and start and end ItemLists for any stuff you're about to
   remove.  Then after you change it, call UndoAssertNewContents with
   the ItemLists for the stuff you've changed this bit to.  Note that
   the "start" ItemList SHOULD be the same as for the PreviousContents
   call; it's the left bound for the Undo operation.  If you've called
   AssertPrevious and the operation fails, call UndoCancel; if you're
   doing something that can't be undone and that will break any
   previous Undos, call UndoInvalidate.  (Even things like changing
   which buffer is visible probably should count in this case;
   operations that affect multiple staffs definitely should, as there
   is no mechanism for allowing you to undo them.)

   You must always call both Previous and New assert functions; if you
   haven't deleted or added anything, improvise appropriately.

   Note the start/end ItemLists should be defined in the same way as
   for stave sweep records.
*/

void UndoAssertPreviousContents(String, MajorStave, int, ItemList, ItemList);
void UndoAssertNewContents(String, MajorStave, int, ItemList, ItemList);
void UndoChangeLabelName(String, MajorStave); /* for opns that "use" others */
void UndoCancel(void);
Boolean UndoInvalidate(String, MajorStave, String);
                                 /* -- if it returns False, don't do anything */
void UndoInstallLabelsForStave(MajorStave); /* stave may be NULL for this one */

/* The Undo operation itself is EditMenuUndo, declared in Menu.h */

typedef void *UndoStack;	/* only Undo.c actually knows what this is */

/* administrative functions: */

void UndoSetUndoMenuButtons(Widget, Widget);
UndoStack UndoCreateUndoStack(void);
void UndoDestroyUndoStack(UndoStack);

#endif

