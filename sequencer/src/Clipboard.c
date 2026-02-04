/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Clipboard.c

Description:	This file provides general clipboard functionality for the MIDI sequencer, 
		allowing the transfer of events between tracks via a 'Cut and Paste' method.


Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	29/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#include <MidiFile.h>
#include <MidiErrorHandler.h>

#include "Parse.h"
#include "Globals.h"
#include "Consts.h"
#include "EventListWindow.h"
#include "MainWindow.h"
#include "Clipboard.h"
#include "Menu.h"
#include "EventListMenu.h"
#include "PianoRollMenu.h"

#include <Debug.h>

EventList Clipboard 	    = NULL;
Widget	  ClipboardWindow   = NULL;
Widget	  ClipboardText     = NULL;
char	 *ClipboardContents = NULL;


void Midi_ClipboardSet(EventList SomeEvents)
{
BEGIN("Midi_ClipboardSet");

	Midi_ClipboardClear();

	Clipboard = SomeEvents;

	if (ClipboardWindow) Midi_ClipboardShowContents();

	Midi_LeaveMenuMode(NothingCutMode);
	Midi_ELAllWindowsLeaveMenuMode(NothingCutMode);
	Midi_PRAllWindowsLeaveMenuMode(NothingCutMode);

END;
}



EventList Midi_ClipboardGet()
{
BEGIN("Midi_ClipboardGet");

RETURN_PTR(Clipboard);
}


void Midi_ClipboardCopy(EventList StartCopying, EventList StopCopying)
{
EventList ClonedEvent, RunningPtr = NULL;
long StartTime;

BEGIN("Midi_ClipboardCopy");

	Midi_ClipboardClear();

	if (!StartCopying) END;

	StartTime = StartCopying->Event.DeltaTime;

	if (StartCopying == StopCopying) 
	{
		Clipboard = Midi_EventCreateList(&StartCopying->Event, True);
		Clipboard->Event.DeltaTime -= StartTime;
	}

	while (StartCopying && StartCopying != StopCopying)
	{
		ClonedEvent = Midi_EventCreateList(&StartCopying->Event, True);
		ClonedEvent->Event.DeltaTime -= StartTime;

		if (Clipboard)
		{
			RunningPtr = (EventList)Nconc(RunningPtr, ClonedEvent);
		}
		else
		{
			Clipboard  = ClonedEvent;
			RunningPtr = Clipboard;
		}

		StartCopying = (EventList)Next(StartCopying);
	}

	if (ClipboardWindow) Midi_ClipboardShowContents();

	Midi_LeaveMenuMode(NothingCutMode);
	Midi_ELAllWindowsLeaveMenuMode(NothingCutMode);
	Midi_PRAllWindowsLeaveMenuMode(NothingCutMode);

END;
}


EventList Midi_ClipboardCut(EventList StartCutting, EventList StopCutting)
{
EventList ClonedEvent, RunningPtr = NULL, DoomedEvent;
long StartTime, CutTime;

BEGIN("Midi_ClipboardCut");

	Midi_ClipboardClear();

	if (!StartCutting) RETURN_PTR(NULL);

	StartTime = StartCutting->Event.DeltaTime;
	CutTime   = 0;

	if (StartCutting == StopCutting)
	{
		Clipboard = Midi_EventCreateList(&StartCutting->Event, True);
		CutTime   = Clipboard->Event.DeltaTime;
		Clipboard->Event.DeltaTime = 0;
		DoomedEvent = StartCutting;
		StartCutting = (EventList)Next(StartCutting);
		Remove(DoomedEvent);
	}
	else
	{
		while (StartCutting && StartCutting != StopCutting)
		{
			ClonedEvent = Midi_EventCreateList(&StartCutting->Event, True);
			CutTime = ClonedEvent->Event.DeltaTime;
			ClonedEvent->Event.DeltaTime -= StartTime;
			DoomedEvent = StartCutting;

			if (Clipboard)
			{
				RunningPtr = (EventList)Nconc(RunningPtr, ClonedEvent);
			}
			else
			{
				Clipboard = ClonedEvent;
				RunningPtr = Clipboard;
			}

			StartCutting = (EventList)Next(StartCutting);
			Remove(DoomedEvent);
		}
	}

	while(StartCutting)
	{
		StartCutting->Event.DeltaTime = StartCutting->Event.DeltaTime - CutTime + StartTime;
		RunningPtr = StartCutting;
		StartCutting = (EventList)Next(StartCutting);
	}

	if (ClipboardWindow) Midi_ClipboardShowContents();

	RunningPtr = (EventList)First(RunningPtr);

	Midi_LeaveMenuMode(NothingCutMode);
	Midi_ELAllWindowsLeaveMenuMode(NothingCutMode);
	Midi_PRAllWindowsLeaveMenuMode(NothingCutMode);

RETURN_PTR(RunningPtr);
}






/***************************************************************************/
/* Midi_ClipboardPaste: This function pastes the contents of the clipboard */
/* into the target event list at the point specified.			   */
/***************************************************************************/

EventList Midi_ClipboardPaste(EventList Target)
{
EventList	ClipboardDuplicate, TrackPtr, RunningPtr = NULL, ClonedEvent;
long		TimeAccumulator, InsertedTime;

BEGIN("Midi_ClipboardPaste");

	ClipboardDuplicate = NULL;

	if (Clipboard == NULL) RETURN_PTR(NULL);

	TimeAccumulator = 0L;

	if (Target)
	{
		TrackPtr = (EventList)First(Target);

		while(TrackPtr != Target)
		{
			TimeAccumulator = TrackPtr->Event.DeltaTime;
			TrackPtr = (EventList)Next(TrackPtr);
		}
	}

	TrackPtr = Clipboard;

	while(TrackPtr)
	{
		ClonedEvent = Midi_EventCreateList(&TrackPtr->Event, True);
		ClonedEvent->Event.DeltaTime += TimeAccumulator;
		if (ClipboardDuplicate)
		{
			RunningPtr = (EventList)Nconc(RunningPtr, ClonedEvent);
		}
		else
		{
			ClipboardDuplicate = ClonedEvent;
			RunningPtr = ClipboardDuplicate;
		}
		TrackPtr = (EventList)Next(TrackPtr);
	}

	Insert(ClipboardDuplicate, Target);

	InsertedTime = ((EventList)Last(Clipboard))->Event.DeltaTime;

	TrackPtr = Target;
	
	while(TrackPtr)
	{
		TrackPtr->Event.DeltaTime += InsertedTime;
		TrackPtr = (EventList)Next(TrackPtr);
	}

	if (ClipboardWindow) Midi_ClipboardShowContents();

RETURN_PTR((EventList)First(ClipboardDuplicate));
}


EventList Midi_ClipboardPasteAtTime(EventList Target, long InsertionTime)
{
EventList ClipboardDuplicate, TrackPtr, RunningPtr = NULL, ClonedEvent;
long      InsertedTime;


BEGIN("Midi_ClipboardPasteAtTime");

	ClipboardDuplicate = NULL;

	if (Clipboard == NULL) RETURN_PTR(NULL); /* cc'95, hope that's right */

	TrackPtr = Clipboard;

	while(TrackPtr)
	{
		ClonedEvent = Midi_EventCreateList(&TrackPtr->Event, True);
		ClonedEvent->Event.DeltaTime += InsertionTime;

		if (ClipboardDuplicate)
		{
			RunningPtr = (EventList)Nconc(RunningPtr, ClonedEvent);
		}
		else
		{
			ClipboardDuplicate = ClonedEvent;
			RunningPtr = ClipboardDuplicate;
		}
		TrackPtr = (EventList)Next(TrackPtr);
	}

	Insert(ClipboardDuplicate, Target);

	InsertedTime = ((EventList)Last(Clipboard))->Event.DeltaTime;

	TrackPtr = Target;
	
	while(TrackPtr)
	{
		TrackPtr->Event.DeltaTime += InsertedTime;
		TrackPtr = (EventList)Next(TrackPtr);
	}

	if (ClipboardWindow) Midi_ClipboardShowContents();

RETURN_PTR((EventList)First(ClipboardDuplicate));
}

/********************************************************************/
/* Midi_ClipboardClear: Clear the clipboard, freeing the event list */
/* that is currently held there (if the clipboard isn't empty).     */
/********************************************************************/

void Midi_ClipboardClear()
{
BEGIN("Midi_ClipboardClear");

	if (Clipboard) 
	{
		Midi_TrackDelete(Clipboard);
		Clipboard = NULL;
	}

	if (ClipboardWindow) Midi_ClipboardShowContents();

	Midi_ELAllWindowsEnterMenuMode(NothingCutMode);
	Midi_PRAllWindowsEnterMenuMode(NothingCutMode);
END;
}




void Midi_ClipboardWindowDismissCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_ClipboardWindowDismissCB");

	XtDestroyWidget(ClipboardWindow);

	ClipboardWindow = NULL;

END;
}

void Midi_ClipboardShowContents()
{
Widget 		ClipboardPane;
Widget 		ClipboardForm;
Widget 		ClipboardBox;
Widget 		ClipboardOK;
Dimension	h1;

BEGIN("Midi_ClipboardShowContents");

	if (!ClipboardWindow)
	{
		ClipboardWindow = XtAppCreateShell("Rosegarden Sequencer Clipboard", ProgramName,
						   applicationShellWidgetClass, display, NULL, 0);

		ClipboardPane = YCreateWidget("Clipboard", panedWidgetClass, ClipboardWindow);

		ClipboardForm = YCreateShadedWidget("Form", formWidgetClass, 
						    ClipboardPane, LightShade);

		ClipboardText = YCreateWidget("List Box", asciiTextWidgetClass, ClipboardForm);

		ClipboardBox  = YCreateShadedWidget("Button Box", boxWidgetClass, 
						    ClipboardPane, MediumShade);

		ClipboardOK   = YCreateCommand("Dismiss", ClipboardBox);

		XtAddCallback(ClipboardOK, XtNcallback, Midi_ClipboardWindowDismissCB, NULL);

		YSetValue(ClipboardText, XtNdisplayCaret, False);

		XtSetMappedWhenManaged(ClipboardWindow, False);
		XtRealizeWidget(ClipboardWindow);
		YGetValue(ClipboardOK, XtNheight, &h1);
		XtUnrealizeWidget(ClipboardWindow);

		YSetValue(ClipboardBox, XtNmin, h1 + 15);
		YSetValue(ClipboardBox, XtNmax, h1 + 15);

		XtSetMappedWhenManaged(ClipboardWindow, True);
		XtRealizeWidget(ClipboardWindow);
		YAssertShellDismissButton(ClipboardWindow, ClipboardOK);
	}

	ClipboardContents = Midi_ParseTrack(Clipboard);
	YSetValue(ClipboardText, XtNstring, ClipboardContents);

END;
}


Boolean	Midi_ClipboardIsEmpty()
{
Boolean State;

BEGIN("Midi_ClipboardIsEmpty");

	State = (Clipboard) ? False : True;

RETURN_BOOL(State);
}
