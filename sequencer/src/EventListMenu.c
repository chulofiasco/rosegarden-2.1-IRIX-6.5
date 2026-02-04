/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           EventListMenu.c
 *
 *    Description:    Handling for menu options on event list sub-windows.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     21/02/94        AJG             File Created.
 *
 *
 */

#include <MidiXInclude.h>
#include <MidiFile.h>
#include <MidiErrorHandler.h>

#include "Main.h"
#include "Globals.h"
#include "EventListWindow.h"
#include "Menu.h"
#include "TrackList.h"
#include "Clipboard.h"
#include "EventDlgs.h"
#include "PianoRoll.h"
#include "Dispatch.h"
#include "Undo.h"

#include <Debug.h>

#include <toolbar/piano_roll.xbm>
#include <toolbar/quantize.xbm>
#include <toolbar/cut.xbm>
#include <toolbar/copy.xbm>
#include <toolbar/paste.xbm>
#include <toolbar/ninja_cross.xbm>
#include <toolbar/undo.xbm>



extern ELWindowList	MIDIEventListWindows;

void Midi_ELInsertGenericEvt(ELWindowList ParentWindow,
                             EventList InsertionPoint,
                             EventList NewEvent)
{
BEGIN("Midi_ELInsertGenericEvt");
    if (NewEvent)
    {
        if (InsertionPoint &&InsertionPoint->Event.DeltaTime == NewEvent->Event.DeltaTime)
        {
            Insert(NewEvent, InsertionPoint);
        }
        else
        {
            InsertionPoint = MIDITracks[ParentWindow->TrackNum];

            while(InsertionPoint)
            {
                if (InsertionPoint->Event.DeltaTime >= NewEvent->Event.DeltaTime)
                {
                    Insert(NewEvent, InsertionPoint);
                    break;
                }
                else InsertionPoint = (EventList)Next(InsertionPoint);
            }
        }

        if (InsertionPoint == MIDITracks[ParentWindow->TrackNum])
        {
            MIDITracks[ParentWindow->TrackNum] = (EventList)First(MIDITracks[ParentWindow->TrackNum]);
        }

	Midi_SetFileModified(True);
        Midi_TrackListSetup();
        Midi_SetupEventList(ParentWindow);
        Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);
    }

END;
}

ELWindowList
Midi_ELGetWindowFromTrack(int TrackNum)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELGetWindowFromTrack");

    if (TrackNum < 0 )
         RETURN_PTR(NULL);

    ParentWindow = (ELWindowList)First(MIDIEventListWindows);

    while((TrackNum) && (ParentWindow))
    {
        ParentWindow = (ELWindowList)Next(ParentWindow);
        TrackNum--;
    }

RETURN_PTR(ParentWindow);
}

ELWindowList Midi_ELGetWindowFromWidget(Widget w)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELGetWindowFromWidget");

		
	while(XtParent(w)) w = XtParent(w);

	ParentWindow = (ELWindowList)First(MIDIEventListWindows);

	while(ParentWindow)
	{
		if (ParentWindow->Shell == w)
		{
			RETURN_PTR(ParentWindow);
		}
		ParentWindow = (ELWindowList)Next(ParentWindow);
	}

RETURN_PTR(NULL);
}



void Midi_ELPianoRollCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELPianoRollCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);

	Midi_PianoRollWindowCreate(ParentWindow->TrackNum);

END;
}


void Midi_ELTrackRenameCB(Widget w, XtPointer a, XtPointer b)
{
int Temp;
ELWindowList ParentWindow;

BEGIN("Midi_ELTrackRenameCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Temp = MIDISelectedTrack;
	MIDISelectedTrack = ParentWindow->TrackNum;

	Midi_TrackRenameCB(w, a, b);

	MIDISelectedTrack = Temp;

END;
}


void Midi_ELTrackInfoCB(Widget w, XtPointer a, XtPointer b)
{
int Temp;
ELWindowList ParentWindow;

BEGIN("Midi_ELTrackRenameCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Temp = MIDISelectedTrack;
	MIDISelectedTrack = ParentWindow->TrackNum;

	Midi_TrackInfoCB(w, a, b);

	MIDISelectedTrack = Temp;

END;
}

	
void Midi_ELTransposeCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELTransposeCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_TrackTransposeDlg(ParentWindow->TrackNum);

END;
}




void Midi_ELQuantizeCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELQuantizeCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_TrackQuantizeDlg(ParentWindow->TrackNum);

END;
}


void Midi_ELChangeChannelCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELChangeChannelCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_TrackChangeChannelDlg(ParentWindow->TrackNum);

END;
}


void Midi_ELQuitCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList DoomedWindow;

BEGIN("Midi_ELQuitCB");

	DoomedWindow = Midi_ELGetWindowFromWidget(w);

	XtFree(DoomedWindow->EventList);
	XtFree(DoomedWindow->TrackMenu);
	XtFree(DoomedWindow->EditMenu);
	XtFree(DoomedWindow->EventMenu);
	YDestroyToolbar(DoomedWindow->Toolbar);
	XtDestroyWidget(DoomedWindow->Shell);
	MIDIEventListWindows = (ELWindowList)First(Remove(DoomedWindow));

END;
}



void Midi_ELDeleteCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList 	ParentWindow;
EventList	DoomedEvent, ClonedEvent, CloneStart = 0, Clone, OuterLimit, DieDieDie;
Boolean		FirstEvtDeleted;

BEGIN("Midi_ELDeleteCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	DoomedEvent = ParentWindow->Selection.StartEvt;
	Clone = NULL;

	FirstEvtDeleted = (ParentWindow->Selection.StartEvt == MIDITracks[ParentWindow->TrackNum]);

	OuterLimit = ParentWindow->Selection.EndEvt;

	if (ParentWindow->Selection.StartEvt == ParentWindow->Selection.EndEvt)
	{
		Midi_UndoRecordOperation(Midi_EventCreateList(&ParentWindow->Selection.StartEvt->Event, True),
				    (EventList)Next(ParentWindow->Selection.StartEvt),
				    ParentWindow->TrackNum);

		if (FirstEvtDeleted) 
		{
			MIDITracks[ParentWindow->TrackNum] = 
					(EventList)Remove(ParentWindow->Selection.StartEvt);
		}
		else Remove(ParentWindow->Selection.StartEvt);
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
	Midi_SetupEventList(ParentWindow);
	Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);
END;
}



void Midi_ELCutCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELCutCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	Midi_UndoRecordOperation(Midi_TrackClone(MIDITracks[ParentWindow->TrackNum]),
				 NULL,
				 ParentWindow->TrackNum);

	MIDITracks[ParentWindow->TrackNum] = Midi_ClipboardCut(ParentWindow->Selection.StartEvt, 
							       ParentWindow->Selection.EndEvt);

	Midi_TrackListSetup();
	Midi_SetupEventList(ParentWindow);
	Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);

END;
}

void Midi_ELCopyCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELCopyCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);
	Midi_ClipboardCopy(ParentWindow->Selection.StartEvt, ParentWindow->Selection.EndEvt);

END;
}


void Midi_ELPasteCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELPasteCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);
	Midi_ClipboardPaste(ParentWindow->Selection.StartEvt);

	if (ParentWindow->Selection.StartEvt == MIDITracks[ParentWindow->TrackNum])
	{
		MIDITracks[ParentWindow->TrackNum] = (EventList)First(MIDITracks[ParentWindow->TrackNum]);
	}

	Midi_TrackListSetup();
	Midi_SetupEventList(ParentWindow);
	Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);
END;
}

	

void Midi_ELFilterByChannelCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELFilterByChannelCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_TrackFilterByChannelDlg(ParentWindow->TrackNum);

END;
}




void Midi_ELFilterByEventCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELFilterByEventCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_TrackFilterByEventDlg(ParentWindow->TrackNum);

END;
}






void Midi_ELFilterByPitchCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;

BEGIN("Midi_ELFilterByPitchCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_TrackFilterByPitchDlg(ParentWindow->TrackNum);

END;
}




void Midi_ELModifyEventCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
EventList    CurrentEvt, ModifiedEvt, EndEvt;
Boolean      Update = False;

BEGIN("Midi_ELInstertTextEvtCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	CurrentEvt = ParentWindow->Selection.StartEvt;

	if (CurrentEvt == ParentWindow->Selection.EndEvt) {
	  EndEvt = (EventList)Next(ParentWindow->Selection.EndEvt);
	} else {
	  EndEvt = ParentWindow->Selection.EndEvt;
	} 


	while(CurrentEvt != EndEvt)
	{

		ModifiedEvt = NULL;
		switch(MessageType(CurrentEvt->Event.EventCode))
		{
		case MIDI_NOTE_ON:

			ModifiedEvt = Midi_NoteEventDlg(CurrentEvt->Event.DeltaTime,
							  ChannelNum(CurrentEvt->Event.EventCode),
							  CurrentEvt->Event.EventData.Note.Note,
							  CurrentEvt->Event.EventData.Note.Velocity,
							  CurrentEvt->Event.EventData.Note.Duration,
							  False);

			break;

		case MIDI_CTRL_CHANGE:
			ModifiedEvt = Midi_CtrlChngEventDlg(CurrentEvt->Event.DeltaTime,
							ChannelNum(CurrentEvt->Event.EventCode),
							CurrentEvt->Event.EventData.ControlChange.Controller,
							CurrentEvt->Event.EventData.ControlChange.Value,
							False);

			break;

		case MIDI_PROG_CHANGE:
			ModifiedEvt = Midi_ProgramChangeEventDlg
                                        (CurrentEvt->Event.DeltaTime,
                                         ChannelNum(CurrentEvt->Event.EventCode),
                                         CurrentEvt->Event.EventData.
                                             ProgramChange.Program,
                                         False);
                        break;

		case MIDI_POLY_AFTERTOUCH:
                        ModifiedEvt = Midi_AftertouchEventDlg
                                        (CurrentEvt->Event.DeltaTime,
                                         ChannelNum(CurrentEvt->Event.EventCode),
                                         CurrentEvt->Event.EventData.PolyAftertouch.Velocity,
					 CurrentEvt->Event.EventData.PolyAftertouch.Note,
                                         0,
                                         False);
                        break;
  
		case MIDI_CHNL_AFTERTOUCH:
                        ModifiedEvt = Midi_AftertouchEventDlg
                                        (CurrentEvt->Event.DeltaTime,
                                         ChannelNum(CurrentEvt->Event.EventCode),
                                         CurrentEvt->Event.EventData.PolyAftertouch.Velocity,
					 CurrentEvt->Event.EventData.PolyAftertouch.Note,
                                         1,
                                         False);
                        break;
  
		case MIDI_PITCH_BEND:
			ModifiedEvt = Midi_PitchBendEventDlg
					(CurrentEvt->Event.DeltaTime,
					 ChannelNum(CurrentEvt->Event.EventCode),
 					 CurrentEvt->Event.EventData.PitchWheel.LSB,
					 CurrentEvt->Event.EventData.PitchWheel.MSB,
					 False);
			break;
                 
		case MIDI_SYSTEM_MSG:
			switch(CurrentEvt->Event.EventData.MetaEvent.MetaEventCode)
			{
                		case MIDI_TEXT_EVENT:
                                case MIDI_COPYRIGHT_NOTICE:
				case MIDI_TRACK_NAME:
				case MIDI_INSTRUMENT_NAME:
				case MIDI_LYRIC:
				case MIDI_TEXT_MARKER:
				case MIDI_CUE_POINT:
                        		ModifiedEvt = Midi_TextEventDlg
                                        (ChannelNum(CurrentEvt->Event.EventCode),
                                         CurrentEvt->Event.DeltaTime,
					 &CurrentEvt->Event.EventData.MetaEvent.Bytes,
                                         CurrentEvt->Event.EventData.MetaEvent.MetaEventCode,
                                         False);

					break;

				case MIDI_SET_TEMPO:
					ModifiedEvt = Midi_TempoEventDlg
					(CurrentEvt->Event.DeltaTime,
					(long) (CurrentEvt->Event.EventData.MetaEvent.Bytes << 16 |
					 *(&CurrentEvt->Event.EventData.MetaEvent.Bytes + 1) << 8 |
					 *(&CurrentEvt->Event.EventData.MetaEvent.Bytes + 2) ),
					False);

					break;
				default:
				  break;
			}
			break;

		default:

			break;
		}

		if (ModifiedEvt)
		{
			Insert(ModifiedEvt, CurrentEvt);

			if (CurrentEvt == MIDITracks[ParentWindow->TrackNum])
			{
				MIDITracks[ParentWindow->TrackNum] = ModifiedEvt;
			}

			CurrentEvt = (EventList)Next(CurrentEvt);
			Remove(Prev(CurrentEvt));
                        Update = True;
		} else {
		  CurrentEvt = (EventList)Next(CurrentEvt);
		}
	}

	if (Update)
	{
		Midi_SetFileModified(True);
		Midi_TrackListSetup();
		Midi_SetupEventList(ParentWindow);
		Midi_UpdatePianoRollWindow(ParentWindow->TrackNum);
        }


END;
}

void Midi_ELInsertTextEvtCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long	     time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELInsertTextEvtCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	InsertionPoint = ParentWindow->Selection.StartEvt;

	if (InsertionPoint)
	{
		time = InsertionPoint->Event.DeltaTime;
	}
	else time = 0;

	NewEvent = Midi_TextEventDlg(MIDI_TEXT_EVENT, time, NULL, MIDI_TEXT_EVENT, True);

        Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}

void Midi_ELInsertNoteEvtCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long	     time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELInsertNoteEvtCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	InsertionPoint = ParentWindow->Selection.StartEvt;

	if (InsertionPoint)
	{
		time = InsertionPoint->Event.DeltaTime;
	}
	else time = 0;

	NewEvent = Midi_NoteEventDlg(time, 0, 0, 0, 0L, True);

        Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}

void Midi_ELInsertCtrlChngEvtCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long	     time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELInsertNoteEvtCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	InsertionPoint = ParentWindow->Selection.StartEvt;

	if (InsertionPoint)
	{
		time = InsertionPoint->Event.DeltaTime;
	}
	else time = 0;

	NewEvent = Midi_CtrlChngEventDlg(time, 0, 0, 0, True);

        Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}

void Midi_ELInsertAftrTchEvtCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long	     time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELInsertAftrTchEvtCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	InsertionPoint = ParentWindow->Selection.StartEvt;

	if (InsertionPoint)
	{
		time = InsertionPoint->Event.DeltaTime;
	}
	else time = 0;

	NewEvent = Midi_AftertouchEventDlg(time, 0, 0, 0, 0, True);

        Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}

void Midi_ELInsertTempoEvtCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long         time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELPBEvtDlg");

    ParentWindow = Midi_ELGetWindowFromWidget(w);
    Midi_EventListCalculateSelection(ParentWindow);

    InsertionPoint = ParentWindow->Selection.StartEvt;

    if (InsertionPoint)
    {
            time = InsertionPoint->Event.DeltaTime;
    }
    else time = 0;

    NewEvent = Midi_TempoEventDlg(time, (long)0, True);

    Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}


void Midi_ELPBEvtDlg(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long         time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELPBEvtDlg");

    ParentWindow = Midi_ELGetWindowFromWidget(w);
    Midi_EventListCalculateSelection(ParentWindow);

    InsertionPoint = ParentWindow->Selection.StartEvt;

    if (InsertionPoint)
    {
            time = InsertionPoint->Event.DeltaTime;
    }
    else time = 0;

    NewEvent = Midi_PitchBendEventDlg(time, 0, 0, 0, True);

    Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}


void Midi_ELInsertProgChngEvtCB(Widget w, XtPointer a, XtPointer b)
{
ELWindowList ParentWindow;
long	     time;
EventList    InsertionPoint, NewEvent;

BEGIN("Midi_ELInsertNoteEvtCB");

	ParentWindow = Midi_ELGetWindowFromWidget(w);
	Midi_EventListCalculateSelection(ParentWindow);

	InsertionPoint = ParentWindow->Selection.StartEvt;

	if (InsertionPoint)
	{
		time = InsertionPoint->Event.DeltaTime;
	}
	else time = 0;

	NewEvent = Midi_ProgramChangeEventDlg(time, 0, 0, True);

        Midi_ELInsertGenericEvt(ParentWindow, InsertionPoint, NewEvent);

END;
}

YMenuElement ELTrackMenu[] = 
{
	{ "Show Piano Roll",	PlaybackMode,	Midi_ELPianoRollCB,		piano_roll_bits, NULL, },
	YMenuDivider,
	{ "Rename . . .",		PlaybackMode,	Midi_ELTrackRenameCB,		NULL, },
	{ "Track Info",		PlaybackMode,	Midi_ELTrackInfoCB,		NULL, },
	YMenuDivider,
	{ "Filter By Channel . . .",	PlaybackMode,	Midi_ELFilterByChannelCB,	NULL, },
	{ "Filter By Event . . .",	PlaybackMode,	Midi_ELFilterByEventCB,		NULL, },
	{ "Filter By Pitch . . .",   	PlaybackMode, 	Midi_ELFilterByPitchCB,		NULL, },
	YMenuDivider,
	{ "Split By Channel . . .",  	PlaybackMode,	Unimplemented,			NULL, },
	{ "Split By Pitch . . .",    	PlaybackMode, 	Unimplemented,			NULL, },
	{ "Change Channel . . .",	PlaybackMode,	Midi_ELChangeChannelCB,		NULL, },
	YMenuDivider,
	{ "Quantize . . .",		PlaybackMode,	Midi_ELQuantizeCB,		quantize_bits, NULL, },
	{ "Transpose . . .",		PlaybackMode,	Midi_ELTransposeCB,		NULL, },
	YMenuDivider,
	{ "Close",			PlaybackMode,	Midi_ELQuitCB,			NULL },
};


YMenuElement ELEditMenu[] = 
{
	{ "Undo",			NoFileLoadedMode | PlaybackMode | NothingDoneMode,	Midi_UndoCB,		undo_bits, NULL, },
	{ "Delete",		NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchDeleteCB,	ninja_cross_bits, NULL, },
	YMenuDivider,
	{ "Cut",			NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchCutCB,	cut_bits, NULL, },
	{ "Copy",			NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchCopyCB,	copy_bits, NULL, },
	{ "Paste",		NoFileLoadedMode | PlaybackMode | NothingCutMode,	Midi_DispatchPasteCB,	paste_bits, NULL, },
	{ "Show Clipboard",	PlaybackMode,				Midi_ShowClipboardCB,	NULL },
};


YMenuElement ELEventMenu[] = 
{
	{ "Modify Event . . .",			NothingSelectedMode | PlaybackMode, Midi_ELModifyEventCB, NULL, },
	YMenuDivider,
	{ "Insert Text Event . . .",		PlaybackMode,		Midi_ELInsertTextEvtCB,     NULL, },
	{ "Insert Tempo Event . . .",		PlaybackMode,		Midi_ELInsertTempoEvtCB,    NULL, },
	{ "Insert Note . . .",			PlaybackMode,		Midi_ELInsertNoteEvtCB,     NULL, },
	{ "Insert Controller Change . . .",	PlaybackMode,		Midi_ELInsertCtrlChngEvtCB, NULL, },
	{ "Insert Aftertouch . . .",		PlaybackMode,		Midi_ELInsertAftrTchEvtCB,  NULL, },
	{ "Insert Program Change . . .",	PlaybackMode,		Midi_ELInsertProgChngEvtCB, NULL, },
	{ "Insert Pitch Bend . . .",		PlaybackMode,		Midi_ELPBEvtDlg, 	    NULL, },
};


void Midi_ELInstallMenus(ELWindowList NewWindow)
{
BEGIN("Midi_ELInstallMenus");

	NewWindow->TrackMenu = (YMenuElement *)XtMalloc(sizeof(ELTrackMenu));
	memcpy(NewWindow->TrackMenu, ELTrackMenu, sizeof(ELTrackMenu));

	NewWindow->EditMenu = (YMenuElement *)XtMalloc(sizeof(ELEditMenu));
	memcpy(NewWindow->EditMenu, ELEditMenu, sizeof(ELEditMenu));

	NewWindow->EventMenu = (YMenuElement *)XtMalloc(sizeof(ELEventMenu));
	memcpy(NewWindow->EventMenu, ELEventMenu, sizeof(ELEventMenu));

	NewWindow->TrackMenuId = YCreateMenu(NewWindow->TrackMenuButton,
					     "Track Menu",
					     XtNumber(ELTrackMenu),
					     NewWindow->TrackMenu);

	NewWindow->EditMenuId = YCreateMenu(NewWindow->EditMenuButton,
					    "Edit Menu",
					    XtNumber(ELEditMenu),
					    NewWindow->EditMenu);

	NewWindow->EventMenuId = YCreateMenu(NewWindow->EventMenuButton,
					     "Event Menu",
					     XtNumber(ELEventMenu),
					     NewWindow->EventMenu);

END;
}


void Midi_ELEnterMenuMode(ELWindowList Sproing, unsigned long MenuMode)
{
BEGIN("Midi_ELEnterMenuMode");

	YEnterMenuMode(Sproing->TrackMenuId, MenuMode);
	YEnterMenuMode(Sproing->EditMenuId,  MenuMode);
	YEnterMenuMode(Sproing->EventMenuId, MenuMode);

END;
}

void Midi_ELLeaveMenuMode(ELWindowList Sproing, unsigned long MenuMode)
{
BEGIN("Midi_ELLeaveMenuMode");

	YLeaveMenuMode(Sproing->TrackMenuId, MenuMode);
	YLeaveMenuMode(Sproing->EditMenuId,  MenuMode);
	YLeaveMenuMode(Sproing->EventMenuId, MenuMode);

END;
}

void Midi_ELAllWindowsEnterMenuMode(unsigned long MenuMode)
{
ELWindowList	Windows;

BEGIN("Midi_ELAllWindowsEnterMenuMode");

	Windows = MIDIEventListWindows;

	while(Windows)
	{
		Midi_ELEnterMenuMode(Windows, MenuMode);
		Windows = (ELWindowList)Next(Windows);
	}

END;
}



void Midi_ELAllWindowsLeaveMenuMode(unsigned long MenuMode)
{
ELWindowList	Windows;

BEGIN("Midi_ELAllWindowsLeaveMenuMode");

	Windows = MIDIEventListWindows;

	while(Windows)
	{
		Midi_ELLeaveMenuMode(Windows, MenuMode);
		Windows = (ELWindowList)Next(Windows);
	}

END;
}

