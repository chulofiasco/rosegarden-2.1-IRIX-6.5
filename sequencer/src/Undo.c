/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Undo.c

Description:	Functions to maintain a record any data destroyed by the last operation, and
		provide the facility to put it all back again, right as rain.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	18/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#include <MidiErrorHandler.h>
#include <MidiFile.h>

#include "MidiConsts.h"
#include "Globals.h"
#include "Undo.h"
#include "EventListWindow.h"
#include "PianoRoll.h"
#include "Main.h"
#include "Menu.h"
#include "TrackList.h"
#include "EventListMenu.h"
#include "PianoRollMenu.h"

#include <Debug.h>


Midi_UndoBuffer	UndoBuffer = {  0, NULL, NULL };

void Midi_UndoRecordOperation(EventList DeadData, EventList InsertionPoint, int AffectedTrack)
{
BEGIN("Midi_UndoRecordOperation");

	if (UndoBuffer.DestroyedData)
	{
		Midi_TrackDelete(UndoBuffer.DestroyedData);
	}

	UndoBuffer.AffectedTrack  = AffectedTrack;
	UndoBuffer.DestroyedData  = DeadData;
	UndoBuffer.InsertionPoint = InsertionPoint;

	Midi_LeaveMenuMode(NothingDoneMode);
	Midi_ELAllWindowsLeaveMenuMode(NothingDoneMode);
	Midi_PRAllWindowsLeaveMenuMode(NothingDoneMode);

	Midi_SetFileModified(True);

END;
}


void Midi_UndoLastOperation()
{
BEGIN("Midi_UndoLastOperation");

	if (!UndoBuffer.DestroyedData) END;

	if (UndoBuffer.InsertionPoint)
	{
		MIDITracks[UndoBuffer.AffectedTrack] = 
					(EventList)First(Insert(UndoBuffer.DestroyedData, 
							        UndoBuffer.InsertionPoint));
	}

	else
	{
		Midi_TrackDelete(MIDITracks[UndoBuffer.AffectedTrack]);
		MIDITracks[UndoBuffer.AffectedTrack] = (EventList)First(UndoBuffer.DestroyedData);
	}

	Midi_EnterMenuMode(NothingDoneMode);

	Midi_TrackListSetup();
	Midi_UpdateEventListWindow(UndoBuffer.AffectedTrack);
	Midi_UpdatePianoRollWindow(UndoBuffer.AffectedTrack);

	UndoBuffer.AffectedTrack  = 0;
	UndoBuffer.DestroyedData  = NULL;
	UndoBuffer.InsertionPoint = NULL;

END;
}


void Midi_UndoClearBuffer()
{
BEGIN("Midi_UndoClearBuffer");

	if (UndoBuffer.DestroyedData)
	{
		Midi_TrackDelete(UndoBuffer.DestroyedData);
	}

	UndoBuffer.DestroyedData  = NULL;
	UndoBuffer.InsertionPoint = NULL;
	UndoBuffer.AffectedTrack  = 0;

	Midi_EnterMenuMode(NothingDoneMode);
	Midi_ELAllWindowsEnterMenuMode(NothingDoneMode);
	Midi_PRAllWindowsEnterMenuMode(NothingDoneMode);
END;
}


Boolean Midi_UndoIsBufferEmpty()
{
Boolean YesOrNo;

BEGIN("Midi_UndoIsBufferEmpty");

	YesOrNo = (UndoBuffer.DestroyedData == NULL) ? True : False;

RETURN_BOOL(YesOrNo);
}

