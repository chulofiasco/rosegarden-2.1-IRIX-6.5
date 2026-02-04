/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	EventListMenu.h

Description:	Function Prototyping for event list window menu options.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	21/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#ifndef _EVENT_LIST_MENU_H_
#define _EVENT_LIST_MENU_H_

#include <X11/Intrinsic.h>
#include "EventListWindow.h"

void Midi_ELCopyCB(Widget, XtPointer, XtPointer);
void Midi_ELInstallMenus(ELWindowList NewWindow);
void Midi_ELEnterMenuMode(ELWindowList Sproing, unsigned long MenuMode);
void Midi_ELLeaveMenuMode(ELWindowList Sproing, unsigned long MenuMode);
void Midi_ELAllWindowsLeaveMenuMode(unsigned long MenuMode);
void Midi_ELAllWindowsEnterMenuMode(unsigned long MenuMode);
ELWindowList Midi_ELGetWindowFromWidget(Widget w);
ELWindowList Midi_ELGetWindowFromTrack(int TrackNum);

void Midi_ELPasteCB(Widget, XtPointer, XtPointer); /* decls added cc 2/95 */
void Midi_ELCutCB(Widget, XtPointer, XtPointer); 
void Midi_ELDeleteCB(Widget, XtPointer, XtPointer); 

#endif

