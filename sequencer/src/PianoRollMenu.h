/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	PianoRollMenu.h

Description:	Function prototypes for Piano-Roll subwindow menu callbacks 'n' stuff.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/03/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/


#ifndef _PIANO_ROLL_MENU_H
#define _PIANO_ROLL_MENU_H

#include <X11/Intrinsic.h>
#include "PianoRoll.h"

void Midi_PRCopyCB(Widget, XtPointer, XtPointer);
void Midi_PRInstallMenus(PRWindowList NewWindow);
void Midi_PREnterMenuMode(PRWindowList Sproing, unsigned long MenuMode);
void Midi_PRLeaveMenuMode(PRWindowList Sproing, unsigned long MenuMode);
void Midi_PRAllWindowsLeaveMenuMode(unsigned long MenuMode);
void Midi_PRAllWindowsEnterMenuMode(unsigned long MenuMode);
PRWindowList Midi_PRGetWindowFromWidget(Widget w);

void Midi_PRPasteCB(Widget, XtPointer, XtPointer); /* decls added cc 2/95 */
void Midi_PRCutCB(Widget, XtPointer, XtPointer); 
void Midi_PRDeleteCB(Widget, XtPointer, XtPointer); 

#endif
