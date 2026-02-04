/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Dispatch.c
 *
 *    Description:    Dispatch command invocations to correct callback for
 *                    window (provides support for keyboard accelerators
 *                    across multiple windows). 
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     15/06/94        AJG             File Created.
 *
 */

#include <MidiXInclude.h>
#include <MidiFile.h>

#include "TrackList.h"
#include "Menu.h"
#include "EventListWindow.h"
#include "EventListMenu.h"
#include "PianoRoll.h"
#include "PianoRollMenu.h"
#include "Globals.h"

#include <Debug.h>


/*******/
/* Cut */
/*******/

void Midi_DispatchCutCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_DispatchCutCB");

	if (b) w = (Widget)b;

	w = Midi_GetShellWidget(w);

	if (w == topLevel) 
	{
		Midi_TrackListCutCB(w, a, b);
	}
	else if (Midi_PRGetWindowFromWidget(w))
	{
		Midi_PRCutCB(w, a, b);
	}
	else if (Midi_ELGetWindowFromWidget(w))
	{
		Midi_ELCutCB(w, a, b);
	}

END;
}


/*******/
/* Copy */
/*******/

void Midi_DispatchCopyCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_DispatchCopyCB");

	w = Midi_GetShellWidget(w);

	if (w == topLevel) 
	{
		Midi_TrackListCopyCB(w, a, b);
	}
	else if (Midi_PRGetWindowFromWidget(w))
	{
		Midi_PRCopyCB(w, a, b);
	}
	else if (Midi_ELGetWindowFromWidget(w))
	{
		Midi_ELCopyCB(w, a, b);
	}

END;
}



/*******/
/* Paste */
/*******/

void Midi_DispatchPasteCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_DispatchPasteCB");

	w = Midi_GetShellWidget(w);

	if (w == topLevel) 
	{
		Midi_TrackListPasteCB(w, a, b);
	}
	else if (Midi_PRGetWindowFromWidget(w))
	{
		Midi_PRPasteCB(w, a, b);
	}
	else if (Midi_ELGetWindowFromWidget(w))
	{
		Midi_ELPasteCB(w, a, b);
	}

END;
}





/*******/
/* Delete */
/*******/

void Midi_DispatchDeleteCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_DispatchDeleteCB");

	w = Midi_GetShellWidget(w);

	if (w == topLevel) 
	{
		Midi_TrackListDeleteCB(w, a, b);
	}
	else if (Midi_PRGetWindowFromWidget(w))
	{
		Midi_PRDeleteCB(w, a, b);
	}
	else if (Midi_ELGetWindowFromWidget(w))
	{
		Midi_ELDeleteCB(w, a, b);
	}

END;
}




