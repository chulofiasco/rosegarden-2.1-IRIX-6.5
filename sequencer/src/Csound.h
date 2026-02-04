/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Csound.h

Description:	Type definitions and prototypes for Csound generation

Author:		JPff

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	30/09/94        JPff		File Created.

--------------------------------------------------------------------------------------------
*/

#ifndef _MIDI_CSOUND_H_
#define _MIDI_CSOUND_H_

#include <stdio.h>
#include <MidiEvent.h>

void Midi_2CsoundCB(Widget, XtPointer, XtPointer);
void Create_ScoreFile(String);

#endif

