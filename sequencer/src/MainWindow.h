/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	MainWindow.h

Description:	Functions that build up the sequencer main window.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <ILClient.h>

void Midi_CreateMainWindow(void);
void Midi_SetBusy(Boolean State);
void Midi_AcknowledgeHelp(IL_ReturnCode);

#endif
