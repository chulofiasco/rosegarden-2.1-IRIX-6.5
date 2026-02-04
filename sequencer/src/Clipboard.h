/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Clipboard.h

Description:	This file provides general clipboard functionality for the MIDI sequencer, 
		allowing the transfer of events between tracks via a 'Cut and Paste' method.


Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	29/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

void      Midi_ClipboardSet(EventList SomeEvents);
EventList Midi_ClipboardGet(void);

EventList Midi_ClipboardCut(EventList StartCutting, EventList StopCutting);
void 	  Midi_ClipboardCopy(EventList StartCopying, EventList StopCopying);
EventList Midi_ClipboardPaste(EventList Target);
EventList Midi_ClipboardPasteAtTime(EventList Target, long InsertionTime);
void 	  Midi_ClipboardClear(void);
void 	  Midi_ClipboardShowContents(void);
Boolean   Midi_ClipboardIsEmpty(void);

#endif

