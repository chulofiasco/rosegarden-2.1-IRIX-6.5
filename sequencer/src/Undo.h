/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Undo.h

Description:	Functions to maintain a record any data destroyed by the last operation, and
		provide the facility to put it all back again, right as rain.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	18/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

typedef struct
{
	int		AffectedTrack;
	EventList	DestroyedData;
	EventList	InsertionPoint;
}
Midi_UndoBuffer;


void    Midi_UndoRecordOperation(EventList DeadData, EventList InsertionPoint, int AffectedTrack);
void    Midi_UndoLastOperation(void);
void    Midi_UndoClearBuffer(void);
Boolean Midi_UndoIsBufferEmpty(void);


