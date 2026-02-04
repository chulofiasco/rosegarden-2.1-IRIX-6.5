/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           PianoRollMenu.c
 *
 *    Description:    Piano-Roll subwindow menu callbacks 'n' stuff.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     16/03/94        AJG             File Created.
 *
 *
 */

#include <MidiXInclude.h>
#include <MidiFile.h>
#include <MidiErrorHandler.h>

#include "Globals.h"
#include "PianoRoll.h"
#include "Menu.h"
#include "TrackList.h"
#include "Clipboard.h"
#include "EventDlgs.h"
#include "Dispatch.h"
#include "Undo.h"
#include "EventListWindow.h"

#include <Debug.h>

#include <toolbar/event_list.xbm>
#include <toolbar/quantize.xbm>
#include <toolbar/cut.xbm>
#include <toolbar/copy.xbm>
#include <toolbar/paste.xbm>
#include <toolbar/ninja_cross.xbm>
#include <toolbar/undo.xbm>


extern PRWindowList MIDIPianoRollWindows;
extern float Zoom;


PRWindowList Midi_PRGetWindowFromWidget(Widget w)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRGetWindowFromWidget");

	while(XtParent(w)) w = XtParent(w);

	ParentWindow = (PRWindowList)First(MIDIPianoRollWindows);

	while(ParentWindow)
	{
		if (ParentWindow->Shell == w)
		{
			RETURN_PTR(ParentWindow);
		}
		ParentWindow = (PRWindowList)Next(ParentWindow);
	}

RETURN_PTR(NULL);
}



void Midi_PianoRollCalculateSelection(PRWindowList ParentWindow)
{
long	  StartSelTime;
long	  EndSelTime;
int	  Temp;
float     Zoom;
float	  Resolution;
EventList CurrEvtPtr;

BEGIN("Midi_PianoRollCalculateSelection");

	if (!ParentWindow->DragStartCoord && !ParentWindow->DragEndCoord) END;


	if (ParentWindow->DragStartCoord > ParentWindow->DragEndCoord)
	{
		Temp		             = ParentWindow->DragStartCoord;
		ParentWindow->DragStartCoord = ParentWindow->DragEndCoord;
		ParentWindow->DragEndCoord   = Temp;
	}

        Zoom = ParentWindow->Zoom;

	Resolution = ((4 * MIDIHeaderBuffer.Timing.Division / (float)PIANO_ROLL_DEFAULT_BAR_WIDTH));

	StartSelTime = ParentWindow->BarNumber * 4 * MIDIHeaderBuffer.Timing.Division +
		       Resolution * ParentWindow->DragStartCoord;

	EndSelTime   = ParentWindow->BarNumber * 4 * MIDIHeaderBuffer.Timing.Division +
		       Resolution * ParentWindow->DragEndCoord;


	if (ParentWindow->BarNumber == 0)
	{
		StartSelTime  -= Resolution * PIANO_ROLL_CLEF_WIDTH;
		EndSelTime    -= Resolution * PIANO_ROLL_CLEF_WIDTH;
	}

	CurrEvtPtr = MIDITracks[ParentWindow->TrackNum];

	while(CurrEvtPtr)
	{
		if (CurrEvtPtr->Event.DeltaTime >= StartSelTime && CurrEvtPtr->Event.DeltaTime <= EndSelTime)
		{
			ParentWindow->SelectStartEvt = CurrEvtPtr;

			while(CurrEvtPtr)
			{
				CurrEvtPtr = (EventList)Next(CurrEvtPtr);
				ParentWindow->SelectEndEvt = CurrEvtPtr;
				
				if (CurrEvtPtr->Event.DeltaTime > EndSelTime) END;
			}
			END;
		}
		else CurrEvtPtr = (EventList)Next(CurrEvtPtr);
	}

END;
}



void Midi_PREventListCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PREventListCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);

	Midi_EventListWindowCreate(ParentWindow->TrackNum);

END;
}



void Midi_PRTrackRenameCB(Widget w, XtPointer a, XtPointer b)
{
int Temp;
PRWindowList ParentWindow;

BEGIN("Midi_PRTrackRenameCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Temp = MIDISelectedTrack;
	MIDISelectedTrack = ParentWindow->TrackNum;

	Midi_TrackRenameCB(w, a, b);

	MIDISelectedTrack = Temp;

END;
}


void Midi_PRTrackInfoCB(Widget w, XtPointer a, XtPointer b)
{
int Temp;
PRWindowList ParentWindow;

BEGIN("Midi_PRTrackRenameCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Temp = MIDISelectedTrack;
	MIDISelectedTrack = ParentWindow->TrackNum;

	Midi_TrackInfoCB(w, a, b);

	MIDISelectedTrack = Temp;

END;
}

	

void Midi_PRFilterByChannelCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRFilterByChannelCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_TrackFilterByChannelDlg(ParentWindow->TrackNum);

	ParentWindow->Dragging 	     = False;
	ParentWindow->DragStartCoord = 0;
	ParentWindow->DragEndCoord   = 0;

END;
}




void Midi_PRFilterByEventCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRFilterByEventCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_TrackFilterByEventDlg(ParentWindow->TrackNum);

	ParentWindow->Dragging 	     = False;
	ParentWindow->DragStartCoord = 0;
	ParentWindow->DragEndCoord   = 0;

END;
}




void Midi_PRFilterByPitchCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRFilterByPitchCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_TrackFilterByPitchDlg(ParentWindow->TrackNum);

	ParentWindow->Dragging 	     = False;
	ParentWindow->DragStartCoord = 0;
	ParentWindow->DragEndCoord   = 0;


END;
}





void Midi_PRTransposeCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRTransposeCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_TrackTransposeDlg(ParentWindow->TrackNum);

	ParentWindow->Dragging 	     = False;
	ParentWindow->DragStartCoord = 0;
	ParentWindow->DragEndCoord   = 0;


END;
}



void Midi_PRQuantizeCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRQuantizeCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_TrackQuantizeDlg(ParentWindow->TrackNum);

END;
}



void Midi_PRChangeChannelCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRChangeChannelCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_TrackChangeChannelDlg(ParentWindow->TrackNum);

	ParentWindow->Dragging 	     = False;
	ParentWindow->DragStartCoord = 0;
	ParentWindow->DragEndCoord   = 0;


END;
}

void Midi_PRQuitCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList DoomedWindow;

BEGIN("Midi_PRQuitCB");

	DoomedWindow = Midi_PRGetWindowFromWidget(w);

	XFreePixmap(XtDisplay(topLevel), DoomedWindow->LabelPixmap);

	XtFree(DoomedWindow->TrackMenu);
	XtFree(DoomedWindow->EditMenu);
	XtFree(DoomedWindow->EventMenu);
	YDestroyToolbar(DoomedWindow->Toolbar);
	XtDestroyWidget(DoomedWindow->Shell);
	MIDIPianoRollWindows = (PRWindowList)First(Remove(DoomedWindow));

END;
}




void Midi_PRDeleteCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList 	ParentWindow;
EventList	DoomedEvent, ClonedEvent, CloneStart = 0, Clone, OuterLimit, DieDieDie;
Boolean		FirstEvtDeleted;

BEGIN("Midi_PRDeleteCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_PianoRollCalculateSelection(ParentWindow);

	DoomedEvent = ParentWindow->SelectStartEvt;
	Clone = NULL;

	FirstEvtDeleted = (ParentWindow->SelectStartEvt == MIDITracks[ParentWindow->TrackNum]);

	OuterLimit = ParentWindow->SelectEndEvt;

	if (ParentWindow->SelectStartEvt == ParentWindow->SelectEndEvt)
	{
		Midi_UndoRecordOperation(Midi_EventCreateList(&ParentWindow->SelectStartEvt->Event, True),
				    (EventList)Next(ParentWindow->SelectStartEvt),
				    ParentWindow->TrackNum);

		if (FirstEvtDeleted) 
		{
			MIDITracks[ParentWindow->TrackNum] = 
					(EventList)Remove(ParentWindow->SelectStartEvt);
		}
		else Remove(ParentWindow->SelectStartEvt);
	}
	else
	{
		do
		{
			ClonedEvent = Midi_EventCreateList(&DoomedEvent->Event, True);
	
			if (Clone)
			{
				Clone = (EventList)Nconc(Clone, ClonedEvent);
			}
			else
			{
				Clone = ClonedEvent;
				CloneStart = Clone;
			}

			DieDieDie   = DoomedEvent;
			DoomedEvent = (EventList)Next(DoomedEvent);
			Remove(DieDieDie);
		}
		while(DoomedEvent && DoomedEvent != OuterLimit);

		if (FirstEvtDeleted) MIDITracks[ParentWindow->TrackNum] = DoomedEvent;
		Midi_UndoRecordOperation(CloneStart, DoomedEvent, ParentWindow->TrackNum);
	}

	Midi_TrackListSetup();
	Midi_UpdateEventListWindow(ParentWindow->TrackNum);
	Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);
END;
}


void Midi_PRCutCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRCutCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_PianoRollCalculateSelection(ParentWindow);

	Midi_UndoRecordOperation(Midi_TrackClone(MIDITracks[ParentWindow->TrackNum]),
				 NULL,
				 ParentWindow->TrackNum);

	MIDITracks[ParentWindow->TrackNum] = Midi_ClipboardCut(ParentWindow->SelectStartEvt, 
							       ParentWindow->SelectEndEvt);

	Midi_TrackListSetup();
	Midi_UpdateEventListWindow(ParentWindow->TrackNum);
	Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);

	ParentWindow->Dragging 	     = False;
	ParentWindow->DragStartCoord = 0;
	ParentWindow->DragEndCoord   = 0;

END;
}


void Midi_PRCopyCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PRCopyCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);
	Midi_PianoRollCalculateSelection(ParentWindow);
	Midi_ClipboardCopy(ParentWindow->SelectStartEvt, ParentWindow->SelectEndEvt);

END;
}



void Midi_PRPasteCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;
EventList    InsertionPoint;
long	     Resolution, InsertionTime;
float        Zoom;

BEGIN("Midi_PRPasteCB");

	ParentWindow = Midi_PRGetWindowFromWidget(w);

        Zoom = ParentWindow->Zoom;

	InsertionPoint = MIDITracks[ParentWindow->TrackNum];

	Resolution = ((4 * MIDIHeaderBuffer.Timing.Division / (float)PIANO_ROLL_DEFAULT_BAR_WIDTH));

	InsertionTime = ParentWindow->BarNumber * 4 * MIDIHeaderBuffer.Timing.Division +
		        Resolution * ParentWindow->DragStartCoord;

	if (ParentWindow->BarNumber == 0) InsertionTime -= Resolution * PIANO_ROLL_CLEF_WIDTH;

	while(Next(InsertionPoint) && InsertionPoint->Event.DeltaTime <= InsertionTime)
	{
		InsertionPoint = (EventList)Next(InsertionPoint);
	}

	if (InsertionPoint)
	{
		Midi_ClipboardPasteAtTime(InsertionPoint, InsertionTime); 
		if (InsertionPoint == MIDITracks[ParentWindow->TrackNum])
		{
			MIDITracks[ParentWindow->TrackNum] = (EventList)First(MIDITracks[ParentWindow->TrackNum]);
		}
	}
	else MIDITracks[ParentWindow->TrackNum] = Midi_ClipboardPaste(NULL);

	Midi_TrackListSetup();	
	Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);
	Midi_UpdateEventListWindow(ParentWindow->TrackNum);

END;
}


YMenuElement PRTrackMenu[] = 
{
	{ "Show Event List",	PlaybackMode,	Midi_PREventListCB,		event_list_bits, NULL, },
	YMenuDivider,
	{ "Rename . . .",		PlaybackMode,	Midi_PRTrackRenameCB,		NULL, },
	{ "Track Info",		PlaybackMode,	Midi_PRTrackInfoCB,		NULL, },
	YMenuDivider,
	{ "Filter By Channel . . .",	PlaybackMode,	Midi_PRFilterByChannelCB,	NULL, },
	{ "Filter By Event . . .",	PlaybackMode,	Midi_PRFilterByEventCB,		NULL, },
	{ "Filter By Pitch . . .",   	PlaybackMode, 	Midi_PRFilterByPitchCB,		NULL, },
	YMenuDivider,
	{ "Split By Channel . . .",  	PlaybackMode,	Unimplemented,			NULL, },
	{ "Split By Pitch . . .",    	PlaybackMode, 	Unimplemented,			NULL, },
	{ "Change Channel . . .",	PlaybackMode,	Midi_PRChangeChannelCB,		NULL, },
	YMenuDivider,
	{ "Quantize . . .",		PlaybackMode,	Midi_PRQuantizeCB,		quantize_bits, NULL, },
	{ "Transpose . . .",		PlaybackMode,	Midi_PRTransposeCB,		NULL, },
	YMenuDivider,
	{ "Close",			PlaybackMode,	Midi_PRQuitCB,			NULL },
};


YMenuElement PREditMenu[] = 
{
	{ "Undo",			NoFileLoadedMode | PlaybackMode | NothingDoneMode,	Midi_UndoCB,		undo_bits, NULL, },
	{ "Delete",		NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchDeleteCB,	ninja_cross_bits, NULL, },
	YMenuDivider,
	{ "Cut",			NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchCutCB,	cut_bits, NULL, },
	{ "Copy",			NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchCopyCB,	copy_bits, NULL, },
	{ "Paste",		NoFileLoadedMode | PlaybackMode | NothingCutMode,	Midi_DispatchPasteCB,	paste_bits, NULL, },
	{ "Show Clipboard",	PlaybackMode,				Midi_ShowClipboardCB,	NULL },
};



YMenuElement PREventMenu[] = 
{
	{ "Modify Event . . .",			NothingSelectedMode | PlaybackMode,	Unimplemented, 		    NULL, },
	YMenuDivider,
	{ "Insert Text Event . . .",		PlaybackMode,		Unimplemented, 		    NULL, },
	{ "Insert Tempo Event . . .",		PlaybackMode,		Unimplemented,		    NULL, },
	{ "Insert Note . . .",			PlaybackMode,		Unimplemented, 		    NULL, },
	{ "Insert Controller Change . . .",	PlaybackMode,		Unimplemented,		    NULL, },
	{ "Insert Channel Aftertouch . . .",	PlaybackMode,		Unimplemented, 		    NULL, },
	{ "Insert Polyphonic Aftertouch . . .",	PlaybackMode,		Unimplemented, 		    NULL, },
	{ "Insert Program Change . . .",	PlaybackMode,		Unimplemented, 		    NULL, },
	{ "Insert Pitch Change . . .",		PlaybackMode,		Unimplemented, 		    NULL, },
};


void Midi_PRInstallMenus(PRWindowList NewWindow)
{
BEGIN("Midi_PRInstallMenus");

	NewWindow->TrackMenu = (YMenuElement *)XtMalloc(sizeof(PRTrackMenu));
	memcpy(NewWindow->TrackMenu, PRTrackMenu, sizeof(PRTrackMenu));

	NewWindow->EditMenu = (YMenuElement *)XtMalloc(sizeof(PREditMenu));
	memcpy(NewWindow->EditMenu, PREditMenu, sizeof(PREditMenu));

	NewWindow->EventMenu = (YMenuElement *)XtMalloc(sizeof(PREventMenu));
	memcpy(NewWindow->EventMenu, PREventMenu, sizeof(PREventMenu));

	NewWindow->TrackMenuId = YCreateMenu(NewWindow->TrackMenuButton,
					     "Track Menu",
					     XtNumber(PRTrackMenu),
					     NewWindow->TrackMenu);

	NewWindow->EditMenuId = YCreateMenu(NewWindow->EditMenuButton,
					    "Edit Menu",
					    XtNumber(PREditMenu),
					    NewWindow->EditMenu);

	NewWindow->EventMenuId = YCreateMenu(NewWindow->EventMenuButton,
					     "Event Menu",
					     XtNumber(PREventMenu),
					     NewWindow->EventMenu);

END;
}



void Midi_PREnterMenuMode(PRWindowList Sproing, unsigned long MenuMode)
{
BEGIN("Midi_PREnterMenuMode");

	YEnterMenuMode(Sproing->TrackMenuId, MenuMode);
	YEnterMenuMode(Sproing->EditMenuId,  MenuMode);
	YEnterMenuMode(Sproing->EventMenuId, MenuMode);

END;
}

void Midi_PRLeaveMenuMode(PRWindowList Sproing, unsigned long MenuMode)
{
BEGIN("Midi_PRLeaveMenuMode");

	YLeaveMenuMode(Sproing->TrackMenuId, MenuMode);
	YLeaveMenuMode(Sproing->EditMenuId,  MenuMode);
	YLeaveMenuMode(Sproing->EventMenuId, MenuMode);

END;
}



void Midi_PRAllWindowsEnterMenuMode(unsigned long MenuMode)
{
PRWindowList	Windows;

BEGIN("Midi_PRAllWindowsEnterMenuMode");

	Windows = MIDIPianoRollWindows;

	while(Windows)
	{
		Midi_PREnterMenuMode(Windows, MenuMode);
		Windows = (PRWindowList)Next(Windows);
	}

END;
}


void Midi_PRAllWindowsLeaveMenuMode(unsigned long MenuMode)
{
PRWindowList	Windows;

BEGIN("Midi_PRAllWindowsLeaveMenuMode");

	Windows = MIDIPianoRollWindows;

	while(Windows)
	{
		Midi_PRLeaveMenuMode(Windows, MenuMode);
		Windows = (PRWindowList)Next(Windows);
	}

END;
}

