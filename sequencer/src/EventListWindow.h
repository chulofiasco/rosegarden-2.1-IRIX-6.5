/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	EventListWindow.h

Description:	Type definitions and function prototypes for event-list sub-window functions.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	18/02/94	AJG		File Created.
	23/06/96	cc		added quit action

--------------------------------------------------------------------------------------------
*/

#ifndef _EVENTLISTWINDOW_H_
#define _EVENTLISTWINDOW_H_

#include "MidiFile.h"
#include "MidiTrack.h"
#include <X11/Intrinsic.h>

/******************************************/
/* Event List window selection structure. */
/******************************************/

typedef struct
{
	long		StartChar;
	long		EndChar;
	EventList	StartEvt;
	EventList	EndEvt;
}
ELWindowSelection;


/****************************************************/
/* Event list window private information structure. */
/****************************************************/

typedef struct
{
	ListElement		Base;
	int			TrackNum;
	Widget			Shell;
	Widget			Pane;
	Widget			TopBox;
	Widget			MenuBar;
	Widget			HelpBox;
	Widget			PlayBackBox;
        Widget                  Toolbar;
	Widget			Form;
	Widget			ViewPort;
	Widget			ListBox;
	Widget			TrackMenuButton;
	Widget			EditMenuButton;
	Widget			EventMenuButton;
	Widget			TrackDeviceLabel;
        Widget                  TrackPlaybackStatus;
	Widget			TrackPlaybackLabel;
	int			SelectedEvent;
	char	               *EventList;
	YMenuElement           *TrackMenu;
	YMenuId			TrackMenuId;
	YMenuElement           *EditMenu;
	YMenuId			EditMenuId;
	YMenuElement   	       *EventMenu;
	YMenuId			EventMenuId;
	ELWindowSelection	Selection;
        String                 *DeviceMenuLabels;
        Widget                  DeviceMenu;
}
ELWindowListElt, *ELWindowList;



void Midi_EventListWindowCreate(int TrackNum);
void Midi_UpdateEventListWindow(int TrackNum);
void Midi_EventListDeleteAllWindows(void);
void Midi_EventListCalculateSelection(ELWindowList CurrWindow);
void Midi_RemoveEventListWindow(int TrackNum);
void Midi_EventListQuitAction(Widget, XEvent *, String *, Cardinal *);
Widget Midi_EventListLocateWindowShell(int TrackNum);
void Midi_SetupEventList(ELWindowList);

#define MIDI_TRACK_PB_TEXT "Active"
#define MIDI_TRACK_MUTE_TEXT "Mute"
#define MIDI_TRACK_DIS_TEXT "Disabled"
#endif
