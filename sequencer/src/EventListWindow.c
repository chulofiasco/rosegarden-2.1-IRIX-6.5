/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           EventListWindow.c
 *
 *    Description:    Function to manage the event list sub-windows that
 *                    displayevent information for tracks and allow editing
 *                    and allow editing and low-level event manipulation.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     18/02/94        AJG             File Created.
 *    002     03/11/96        rwb             double click on event list
 *                                            brings up modify event dialog.
 * 
 *
 */

#include "Mapper.h"
#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include "Globals.h"
#include "Consts.h"
#include "Main.h"
#include "EventListWindow.h"
#include "EventListMenu.h"
#include "Parse.h"
#include "Clipboard.h"
#include "Undo.h"
#include "TrackList.h"

#include <Debug.h>

ELWindowList	MIDIEventListWindows = NULL;

extern char **TrackListEntries;
extern void Midi_ELModifyEventCB();

extern DeviceMetaInfoElement    Devices;
extern TrackMetaInfoElement     Tracks;

XawTextSelectType Midi_EventListSelect[] = 
{
	XawselectLine,
	XawselectLine,
	XawselectLine,
	XawselectLine,
	XawselectLine,
	XawselectNull,
};


/********************************/
/* Private Function Prototypes. */
/********************************/

void Midi_SetupEventList(ELWindowList NewWindow);

/*****************************************************************************/
/* Midi_EventListWindowClearSelection: Clear the internal record of the      */
/*  current selection for an event list window, and insensitise the          */
/*  appropriate menu items.                                                  */
/*****************************************************************************/

void Midi_EventListWindowClearSelection(ELWindowList WindowToClear)
{
BEGIN("Midi_EventListWindowClearSelection");

	WindowToClear->Selection.StartChar = 0L;
	WindowToClear->Selection.EndChar   = 0L;
	WindowToClear->Selection.StartEvt  = NULL;
	WindowToClear->Selection.EndEvt    = NULL;

	Midi_ELEnterMenuMode(WindowToClear, NothingSelectedMode);

END;
}

/***************************************************************************/
/* Midi_EventListGetSelectionCB: Callback called when a button is released */
/* within the list box for an event list window. Removes any selections    */
/* that are current for any other visible event list windows and then      */
/* retrieves the selection extent values from the list box text widget.    */
/***************************************************************************/

static int WaitingForClick = False;

static void Midi_ClickTimeoutCB(XtPointer a, XtIntervalId *b)
{
  WaitingForClick = False;
}

void Midi_EventListGetSelectionCB(Widget w, XtPointer a, XEvent *b,
                                                             Boolean *cont)
{
ELWindowList ParentWindow, Windows;

BEGIN("Midi_EventListGetSelectionCB");

	if (cont) *cont = True;

	/********************************/
	/* Clear all window selections. */
	/********************************/

	Windows = MIDIEventListWindows;

	while(Windows)
	{
		Midi_EventListWindowClearSelection(Windows);
		Windows = (ELWindowList)Next(Windows);
	}

	ParentWindow = Midi_ELGetWindowFromWidget(w);

	/***********************************/
	/* Get the text selection extents. */
	/***********************************/

	XawTextGetSelectionPos(w, &ParentWindow->Selection.StartChar,
				&ParentWindow->Selection.EndChar);

	/*****************************************/
	/* Sensitise the appropriate menu items. */
	/*****************************************/

	Midi_ELLeaveMenuMode(ParentWindow, NothingSelectedMode);

        WaitingForClick = True;
        XtAppAddTimeOut(XtWidgetToApplicationContext(w),
                            XtGetMultiClickTime(XtDisplay(w)),
                            Midi_ClickTimeoutCB, NULL);

END;
}



void Midi_EventListSelectionCB(Widget w, XtPointer a, XEvent *event, Boolean *cont)
{
static int posX;
static int posY;
XButtonEvent *ButtonEvent;

    BEGIN("Midi_EventListSelectionCB");

    ButtonEvent = (XButtonEvent *)event;

    if ( !WaitingForClick )
    {
        WaitingForClick = True;
        posX = ButtonEvent->x;
        posY = ButtonEvent->y;
    }
    else
    {
        /* modify the event - frig past the scrollbar for 2 button mice*/
        if ( posX == ButtonEvent->x && posY == ButtonEvent->y && posX > 20)
        Midi_ELModifyEventCB(w,a,a);
        WaitingForClick = False;
    }

    END;
}


/******************************************************************************/
/* Midi_EventListCalculateSelection: Calculate the start and finish events    */
/* in the selection for an event list window from the selection text extents. */
/******************************************************************************/

void Midi_EventListCalculateSelection(ELWindowList CurrWindow)
{
int i;

BEGIN("Midi_EventListCalculateSelection");

	CurrWindow->Selection.StartEvt = MIDITracks[CurrWindow->TrackNum];

	for(i = 0; i < CurrWindow->Selection.StartChar; ++i)
	{
		if (CurrWindow->EventList[i] == '\n')
		{
			CurrWindow->Selection.StartEvt = 
						(EventList)Next(CurrWindow->Selection.StartEvt);
		}
	}

	CurrWindow->Selection.EndEvt = CurrWindow->Selection.StartEvt;

	for(i = CurrWindow->Selection.StartChar; i < CurrWindow->Selection.EndChar; ++i)
	{
		if (CurrWindow->EventList[i] == '\n')
		{
			CurrWindow->Selection.EndEvt = (EventList)Next(CurrWindow->Selection.EndEvt);
		}
	}
END;
}


void Midi_EventListQuitAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
  ELWindowList windows;
  Begin("Midi_EventListQuitAction");

  windows = MIDIEventListWindows;

  while (windows) {
    if (windows->Shell == w) break;
    windows = (ELWindowList)Next(windows);
  }

  if (!windows) End;

  Midi_RemoveEventListWindow(windows->TrackNum);
  End;
}


Widget Midi_EventListLocateWindowShell(int TrackNum)
{
ELWindowList Windows;
BEGIN("Midi_EventListLocateWindowShell");

    Windows = MIDIEventListWindows;

    while(Windows)
      {
	if (TrackNum == Windows->TrackNum)
	  {
	    RETURN_PTR(Windows->Shell);
	  }
	
	Windows = (ELWindowList)Next(Windows);
      }

RETURN_PTR(NULL);
} 


void
Midi_TrackMuteToggleCB(Widget w, XtPointer a, XtPointer b)
{
    TrackInfo tempptr = Mapper_GetTrack(((ELWindowList)a)->TrackNum);
    Dimension ButtonWidth;

BEGIN("Midi_TrackMuteToggleCB");

    YGetValue(w, XtNwidth, &ButtonWidth);

    switch(tempptr->PlaybackStatus)
    {
        case Playback_Enabled:
            tempptr->PlaybackStatus = Playback_Muted;
            YSetValue(w, XtNlabel, MIDI_TRACK_MUTE_TEXT);
            break;

        case Playback_Muted:
            tempptr->PlaybackStatus = Playback_Enabled;
            YSetValue(w, XtNlabel, MIDI_TRACK_PB_TEXT);
            break;

        case Playback_Disabled:
            break; /* do nothing */

        default:
            break;
    }

    YSetValue(w, XtNwidth, ButtonWidth);

    Midi_TrackListSetup();

END;
}

void
Midi_DeviceMenuCB(Widget w, XtPointer a, XtPointer b)
{
  ELWindowList ThisWindow = (ELWindowList)a;
  int i = (int)b;

  BEGIN("Midi_DeviceMenuCB");

  Mapper_GetTrack(ThisWindow->TrackNum)->Device = i;
  Midi_TrackListSetup();	/* ugh.  You're foul. */
                                /* thanks */

  END;
}

void Midi_EventListWindowCreate(int TrackNum)
{
ELWindowList	NewWindow;
Dimension 	h1;
char		TitleBuffer[256];
Widget		WindowShell, HelpButton;
Atom 		WmProtocols;
TrackInfo       TrackTemp;

int i;

BEGIN("Midi_EventListWindowCreate");


	WindowShell = Midi_EventListLocateWindowShell(TrackNum);
	if (WindowShell) {
	  XMapRaised(XtDisplay(WindowShell), XtWindow(WindowShell));
	  END;
	}

	NewWindow = (ELWindowList)NewList(sizeof(ELWindowListElt));

	if (NewWindow == NULL)
	{
		Error(FATAL, "Unable to allocate space for new window.");
	}

	NewWindow->TrackNum = TrackNum;

        NewWindow->DeviceMenuLabels = (String *)XtMalloc
	  (Devices.MaxDevices * sizeof(String));

	/* create a device list */
	for ( i = 0; i < Devices.MaxDevices; i++ )
	{
	  NewWindow->DeviceMenuLabels[i] = XtNewString
	    (Mapper_GetDevice(i)->Device.Data.Midi.name);
	}

	NewWindow->Shell    = XtAppCreateShell("Rosegarden Sequencer Event List", ProgramName,
					       applicationShellWidgetClass, display, NULL, 0);

	NewWindow->Pane     = YCreateWidget("Event List", 
					    panedWidgetClass, NewWindow->Shell);

	NewWindow->TopBox   = YCreateShadedWidget("Top Box", formWidgetClass,
						  NewWindow->Pane, MediumShade);

	NewWindow->MenuBar  = YCreateShadedWidget("Menu Bar", boxWidgetClass, 
					    	   NewWindow->TopBox, MediumShade);

	NewWindow->HelpBox = YCreateShadedWidget("Help Box", boxWidgetClass,
						 NewWindow->TopBox, MediumShade);

	NewWindow->Toolbar = YCreateToolbar(NewWindow->Pane);
	
	NewWindow->Form     = YCreateShadedWidget("Form", formWidgetClass, 
					   	   NewWindow->Pane, LightShade);

	XtVaSetValues(NewWindow->TopBox, XtNdefaultDistance, 0, NULL);

	XtVaSetValues(NewWindow->MenuBar,
		      XtNleft,   XawChainLeft,   XtNright,  XawChainRight,
		      XtNtop,    XawChainTop,    XtNbottom, XawChainTop,
		      XtNhorizDistance, 0,       XtNvertDistance, 0,
		      XtNborderWidth, 0, NULL);

	XtVaSetValues(NewWindow->HelpBox,
		      XtNfromHoriz, NewWindow->MenuBar, XtNleft, XawChainRight,
		      XtNright,  XawChainRight,  XtNtop, XawChainTop,
		      XtNbottom, XawChainTop,    XtNhorizDistance, 0,
		      XtNvertDistance, 0,        XtNborderWidth, 0, NULL);

	NewWindow->ListBox  = YCreateWidget("List Box", asciiTextWidgetClass,
					     NewWindow->Form);

	NewWindow->PlayBackBox = YCreateShadedWidget("Playback Box", boxWidgetClass,
						NewWindow->Pane, MediumShade);

	sprintf(TitleBuffer, "%s Sequencer Event List - %s", ProgramName, TrackListEntries[TrackNum]);
	YSetValue(NewWindow->Shell, XtNtitle, 	   TitleBuffer);
	YSetValue(NewWindow->Shell, XtNiconName,   TitleBuffer);
	YSetValue(NewWindow->Shell, XtNiconPixmap, RoseMap);
	YSetValue(NewWindow->Shell, XtNiconMask,   RoseMask);

	YSetValue(NewWindow->ListBox, XtNselectTypes,  Midi_EventListSelect);
	YSetValue(NewWindow->ListBox, XtNdisplayCaret, False);

	NewWindow->TrackMenuButton = YCreateMenuButton("Track", NewWindow->MenuBar);
	NewWindow->EditMenuButton  = YCreateMenuButton("Edit", NewWindow->MenuBar);
	NewWindow->EventMenuButton = YCreateMenuButton("Event", NewWindow->MenuBar);
	HelpButton = YCreateCommand("Help", NewWindow->HelpBox);

	XtVaSetValues(NewWindow->MenuBar, XtNleft, XawChainLeft, XtNright,
		      XawChainRight, XtNtop, XawChainTop, XtNbottom,
		      XawChainTop, NULL);

	NewWindow->TrackDeviceLabel = YCreateLabel("Device Label", NewWindow->PlayBackBox);

	YSetValue(NewWindow->TrackDeviceLabel, XtNlabel,
					"Device : ");
  	YGetValue(NewWindow->TrackDeviceLabel, XtNheight, &h1);
	
 	YSetValue(NewWindow->PlayBackBox, XtNmin, h1 + 15);
	YSetValue(NewWindow->PlayBackBox, XtNmax, h1 + 15);

	if (Devices.MaxDevices == 0) { /* cc 4/97 */
	  NewWindow->DeviceMenu =
	    YCreateLabel("(no devices)", NewWindow->PlayBackBox);
	} else {
	  NewWindow->DeviceMenu =
	    YCreateOptionMenu(NewWindow->PlayBackBox,
			      NewWindow->DeviceMenuLabels,
			      Devices.MaxDevices,
			      Mapper_GetTrack(TrackNum)->Device,
			      Midi_DeviceMenuCB,
			      (XtPointer)NewWindow); 
	}

	NewWindow->TrackPlaybackLabel = YCreateLabel("Playback Label",
					NewWindow->PlayBackBox);

        YSetValue(NewWindow->TrackPlaybackLabel, XtNlabel,
				"Playback Mode : ");

        NewWindow->TrackPlaybackStatus = YCreateCommand("Playback Status",
                                               NewWindow->PlayBackBox);

	XtAddCallback(NewWindow->TrackPlaybackStatus, XtNcallback,
		Midi_TrackMuteToggleCB, NewWindow);

        TrackTemp = Mapper_GetTrack(NewWindow->TrackNum);

        switch(TrackTemp->PlaybackStatus)
        {
            case Playback_Muted:
                YSetValue(NewWindow->TrackPlaybackStatus, XtNlabel,
                                              MIDI_TRACK_MUTE_TEXT);
                break;

            case Playback_Enabled:
                YSetValue(NewWindow->TrackPlaybackStatus, XtNlabel,
                                              MIDI_TRACK_PB_TEXT);
                break;

            case Playback_Disabled:
                YSetValue(NewWindow->TrackPlaybackStatus, XtNlabel,
                                              MIDI_TRACK_DIS_TEXT);
                break;

            default:
                break;
        }

	if (!appData.interlockWindow) YSetValue(HelpButton, XtNsensitive, False);

	XtAddCallback(HelpButton, XtNcallback, Midi_HelpCallback, "Track - Show Event List");

/* Press or Release */

	XtAddEventHandler(NewWindow->ListBox, ButtonReleaseMask, True, 
			  Midi_EventListSelectionCB, NULL);

	XtAddEventHandler(NewWindow->ListBox, ButtonReleaseMask, True, 
			  Midi_EventListGetSelectionCB, NULL);

	Midi_ELInstallMenus(NewWindow);

	XtSetMappedWhenManaged(NewWindow->Shell, False);
  
  	XtRealizeWidget(NewWindow->Shell);
  
  	YGetValue(NewWindow->TrackMenuButton, XtNheight, &h1);

	XtUnrealizeWidget(NewWindow->Shell);

	XtSetMappedWhenManaged(NewWindow->Shell, True);

 	YSetValue(NewWindow->TopBox, XtNmin, h1 + 15);
	YSetValue(NewWindow->TopBox, XtNmax, h1 + 15);

	XtRealizeWidget(NewWindow->Shell);

	if (Devices.MaxDevices > 0) {
	  YFixOptionMenuLabel(NewWindow->DeviceMenu);
	}

	WmProtocols = XInternAtom(XtDisplay(NewWindow->Shell),
				  "WM_DELETE_WINDOW", False);
 	XtOverrideTranslations(NewWindow->Shell,
     			       XtParseTranslationTable("<Message>WM_PROTOCOLS: eventlist-wm-quit()"));
	XSetWMProtocols(XtDisplay(NewWindow->Shell),
			XtWindow(NewWindow->Shell), &WmProtocols, 1);

	if (MIDIEventListWindows)
	{
		Nconc(MIDIEventListWindows, NewWindow);
	}
	else MIDIEventListWindows = NewWindow;

	Midi_SetupEventList(NewWindow);
	XawTextEnableRedisplay(NewWindow->ListBox);

	Midi_EventListWindowClearSelection(NewWindow);

	Midi_ELEnterMenuMode(NewWindow, NothingSelectedMode);

	if (Midi_ClipboardIsEmpty())
	{
		Midi_ELEnterMenuMode(NewWindow, NothingCutMode);
	}

	if (Midi_UndoIsBufferEmpty())
	{
		Midi_ELEnterMenuMode(NewWindow, NothingDoneMode);
	}

	XtInstallAccelerators(NewWindow->ListBox, NewWindow->Shell);
END;
}


void Midi_SetupEventList(ELWindowList NewWindow)
{
BEGIN("Midi_SetupEventList");

	/*if (!MIDITracks[NewWindow->TrackNum]) END;*/

	NewWindow->EventList = Midi_ParseTrack(MIDITracks[NewWindow->TrackNum]);
	YSetValue(NewWindow->ListBox, XtNstring, NewWindow->EventList);

END;
}



void Midi_UpdateEventListWindow(int TrackNum)
{
ELWindowList SubWindow;
char		TitleBuffer[256];

BEGIN("Midi_UpdateEventListWindow");

	SubWindow = MIDIEventListWindows;

	while(SubWindow)
	{
		if (SubWindow->TrackNum == TrackNum)
		{
			XtFree(SubWindow->EventList);
			Midi_SetupEventList(SubWindow);
			sprintf(TitleBuffer, "%s Sequencer Event List - %s",
				ProgramName, TrackListEntries[TrackNum]);
			YSetValue(SubWindow->Shell, XtNtitle, 	   TitleBuffer);
			YSetValue(SubWindow->Shell, XtNiconName,   TitleBuffer);
			END;
		}
		SubWindow = (ELWindowList)Next(SubWindow);
	}
END;
}

void Midi_RemoveEventListWindow(int TrackNum)
{
ELWindowList SubWindow;

BEGIN("Midi_RemoveEventListWindow");

	SubWindow = MIDIEventListWindows;

	while(SubWindow)
	{
		if (SubWindow->TrackNum == TrackNum)
		{
			XtFree(SubWindow->EventList);
			XtFree(SubWindow->TrackMenu);
			XtFree(SubWindow->EditMenu);
			XtFree(SubWindow->EventMenu);
			YDestroyToolbar(SubWindow->Toolbar);
			XtDestroyWidget(SubWindow->Shell);
			MIDIEventListWindows = (ELWindowList)First(Remove(SubWindow));
			break;
		}
		SubWindow = (ELWindowList)Next(SubWindow);
	}
END;
}

void Midi_EventListDeleteAllWindows()
{
BEGIN("Midi_EventListDeleteAllWindows");

	while(MIDIEventListWindows)
	{
		XtFree(MIDIEventListWindows->EventList);
		XtFree(MIDIEventListWindows->TrackMenu);
		XtFree(MIDIEventListWindows->EditMenu);
		XtFree(MIDIEventListWindows->EventMenu);
		YDestroyToolbar(MIDIEventListWindows->Toolbar);
		XtDestroyWidget(MIDIEventListWindows->Shell);
		MIDIEventListWindows = (ELWindowList)First(Remove(MIDIEventListWindows));
	}
END;
}
