/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           EventDlgs.c
 *
 *    Description:    Functions for event creation and modification
 *                    dialogue boxes.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     21/02/94        AJG             File Created.
 *    002     3/11/96         rwb             all event dialogs completed.
 *
 *
 */

#include <ctype.h>
#include <MidiXInclude.h>
#include <MidiFile.h>
#include <MidiErrorHandler.h>

#include "Globals.h"
#include "EventDlgs.h"

#include <Debug.h>

char *Midi_TextEvtTypes[] = 
{
	"Text Event",
	"Copyright Notice",
	"Track Name",
	"Instrument Name",
	"Lyric",
	"Text Marker",
	"Cue Point"
};

byte   TextEventType;	
Widget TextEventDlg;
Widget TextEventTypeButton;
Widget AfterTypeField;
Widget AfterEventNoteLabel;
Widget AfterEventNoteField;

static int aftertouchtype = 0;

Widget NoteEventDlg;
Widget ProgEventDlg;
Widget AfterEventDlg;

Midi_EventDlgState Done;

void Midi_ClearCurrentXEvents(void)
{
BEGIN("Midi_ClearCurrentXEvents");

	while(XtAppPending(appContext))
	{
		XtAppProcessEvent(appContext, XtIMAll);
	}

END;
}

void Midi_TextEvtSetMenuButtonName(byte EventType)
{
BEGIN("Midi_TextEvtSetMenuButtonName");

	XtUnmanageChild(TextEventTypeButton);
	XtUnmanageChild(XtParent(TextEventTypeButton));

	YSetValue(TextEventTypeButton, XtNlabel, Midi_TextEvtTypes[EventType - MIDI_TEXT_EVENT]);
	TextEventType = EventType;

	XtManageChild(TextEventTypeButton);
	XtManageChild(XtParent(TextEventTypeButton));

END;
}

void Midi_TextEvtMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TextEvtMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_TEXT_EVENT);

END;
}


void Midi_CopyrightMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_CopyrightMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_COPYRIGHT_NOTICE);

END;
}


void Midi_TrackNameMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackNameMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_TRACK_NAME);

END;
}


void Midi_InstrNameMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_InstrNameMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_INSTRUMENT_NAME);

END;
}


void Midi_LyricMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_LyricMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_LYRIC);

END;
}

void Midi_TextMarkerMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TextMarkerMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_TEXT_MARKER);

END;
}

void Midi_CuePointMenuCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_CuePointMenuCB");

	Midi_TextEvtSetMenuButtonName(MIDI_CUE_POINT);

END;
}


YMenuElement TextEvtDlgMenu[] =
{
	{ "Text Event",		NullMode,	Midi_TextEvtMenuCB,	NULL, },
	{ "Copyright Notice",	NullMode,	Midi_CopyrightMenuCB,	NULL, },
	{ "Track Name",		NullMode,	Midi_TrackNameMenuCB,	NULL, },
	{ "Instrument Name",	NullMode,	Midi_InstrNameMenuCB,	NULL, },
	{ "Lyric",		NullMode,	Midi_LyricMenuCB,	NULL, },
	{ "Text Marker",		NullMode,	Midi_TextMarkerMenuCB,	NULL, },
	{ "Cue Point",		NullMode,	Midi_CuePointMenuCB,	NULL, },
};

YMenuId TextEvtDlgMenuId;



void Midi_TextEvtCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TextEvtCancelCB");

	Done = CANCELLED;

END;
}

void Midi_TextEvtOKCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TextEvtCancelCB");

	Done = COMPLETED;

END;
}

EventList Midi_TextEventDlg(byte EventCode, long time, char *Text,
                            byte TextType, Boolean New)
{
Widget TextEventPane;
Widget TextEventTopBox;
Widget TextEventLabel;

Widget TextEventForm;
Widget TextEventTimeLabel;
Widget TextEventTimeField;
Widget TextEventContentLabel;
Widget TextEventContentField;

Widget TextEventBottomBox;
Widget TextEventOK;
Widget TextEventCancel;
XPoint op;
char TimeStr[16];

EventList 	ReturnEvent;
MIDIEvent	EventBuffer;
char	       *OutText, *OutTimeStr;
float		FloatTime;
long		OutTime;
long		Length;

BEGIN("Midi_TextEventDlg");

	TextEventDlg = XtCreatePopupShell("Text Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	TextEventPane = YCreateWidget("Text Event Pane", panedWidgetClass, 
				      TextEventDlg);

	TextEventTopBox = YCreateShadedWidget("Text Event Title Box", boxWidgetClass,
					      TextEventPane, MediumShade);


	if (New)
	{
		TextEventLabel = YCreateLabel("Create New Text Event", TextEventTopBox);
	}
	else
		TextEventLabel = YCreateLabel("Modify Text Event", TextEventTopBox);


	TextEventForm = YCreateShadedWidget("Text Event Form", formWidgetClass,
					    TextEventPane, LightShade);

	TextEventTypeButton = YCreateMenuButton("Text Event", TextEventForm);

	TextEventTimeLabel  = YCreateLabel("Event Time:", TextEventForm);

	TextEventTimeField  = YCreateSurroundedWidget("Event Time", asciiTextWidgetClass, 
						     TextEventForm, NoShade, NoShade);

	TextEventContentLabel  = YCreateLabel("Event Text:", TextEventForm);

	TextEventContentField  = YCreateSurroundedWidget("Event Text", asciiTextWidgetClass, 
						        TextEventForm, NoShade, NoShade);
 


	TextEventBottomBox = YCreateShadedWidget("Text Event Button Box", boxWidgetClass,
						 TextEventPane, MediumShade);

	TextEventOK     = YCreateCommand("OK", TextEventBottomBox);
	TextEventCancel = YCreateCommand("Cancel", TextEventBottomBox);

	TextEvtDlgMenuId = YCreateMenu(TextEventTypeButton, 
					 "Type Menu",
					 XtNumber(TextEvtDlgMenu),
					 TextEvtDlgMenu);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(TextEventOK, XtNcallback, Midi_TextEvtOKCB, NULL);
	XtAddCallback(TextEventCancel, XtNcallback, Midi_TextEvtCancelCB, NULL);

	/************************************/
	/* Format the form widget contents. */
	/************************************/

	YSetValue(XtParent(TextEventTimeLabel), XtNfromVert, XtParent(TextEventTypeButton));
	YSetValue(XtParent(TextEventTimeField), XtNfromVert, XtParent(TextEventTypeButton));
	YSetValue(XtParent(TextEventTimeField), XtNfromHoriz, XtParent(TextEventContentLabel));

	YSetValue(XtParent(TextEventContentLabel), XtNfromVert, XtParent(TextEventTimeLabel));
	YSetValue(XtParent(TextEventContentField), XtNfromVert, XtParent(TextEventTimeLabel));
	YSetValue(XtParent(TextEventContentField), XtNfromHoriz, XtParent(TextEventContentLabel));

	YSetValue(TextEventContentField, XtNwidth, 320);
	YSetValue(XtParent(TextEventContentField), XtNwidth, 328);

	YSetValue(TextEventContentField, XtNeditType, XawtextEdit);

	if (New)
	{
		YSetValue(TextEventTimeField, XtNeditType, XawtextEdit);
	}

        /* imported from calling functions - from the meta event data */
	TextEventType = TextType;

	/***********************************************/
	/* Set up the fields with the required values. */
	/***********************************************/

	Midi_TextEvtSetMenuButtonName(TextEventType);

	if (Text)
	{
		YSetValue(TextEventContentField, XtNstring, Text);
	}

	sprintf(TimeStr, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(TextEventTimeField, XtNstring, TimeStr);

	op = YPlacePopupAndWarp(TextEventDlg, XtGrabNonexclusive,
				TextEventOK, TextEventCancel);

	YAssertDialogueActions(TextEventDlg, TextEventOK,
			       TextEventCancel, NULL);

	Done = RUNNING;

  	while (!Done || XtAppPending(appContext)) 
	{
		XtAppProcessEvent(appContext, XtIMAll);
	}

	if (Done == CANCELLED)
	{
	        YPopdown(TextEventDlg);
		YDestroyMenu(TextEvtDlgMenuId);
		XtDestroyWidget(TextEventDlg);
		Midi_ClearCurrentXEvents();
		RETURN_PTR(NULL);
	}

	YGetValue(TextEventTimeField, XtNstring, &OutTimeStr);
	FloatTime = atof(OutTimeStr);
	OutTime   = Midi_BeatToTime(FloatTime);

	YGetValue(TextEventContentField, XtNstring, &OutText);
	Length = strlen(OutText);

	YPopdown(TextEventDlg);
	YDestroyMenu(TextEvtDlgMenuId);
	XtDestroyWidget(TextEventDlg);
	Midi_ClearCurrentXEvents();

	EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct) + (Length + 1) * sizeof(char));

	if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

	EventBuffer->EventCode = MIDI_FILE_META_EVENT;
	EventBuffer->DeltaTime = OutTime;
	EventBuffer->EventData.MetaEvent.MetaEventCode = TextEventType;
	EventBuffer->EventData.MetaEvent.NBytes = Length + 1;
	strcpy((char *)&EventBuffer->EventData.MetaEvent.Bytes, OutText);

	ReturnEvent = Midi_EventCreateList(EventBuffer, False);
	/*XtFree(EventBuffer);*/

RETURN_PTR(ReturnEvent);
}





/*
===============================================

		Note Event Dialog

===============================================
*/

#define Midi_NoteEvtCancelCB 	Midi_TextEvtCancelCB
#define Midi_NoteEvtOKCB	Midi_TextEvtOKCB

EventList Midi_NoteEventDlg(long time,     byte    Channel,
			    byte Pitch,    byte    Velocity, 
			    long Duration, Boolean New)
{
Widget NoteEventPane;
Widget NoteEventTopBox;
Widget NoteEventLabel;

Widget NoteEventForm;

Widget NoteEventTimeLabel;
Widget NoteEventTimeField;

Widget NoteEventChannelLabel;
Widget NoteEventChannelField;

Widget NoteEventPitchLabel;
Widget NoteEventPitchField;

Widget NoteEventOctaveLabel;
Widget NoteEventOctaveField;

Widget NoteEventVelocityLabel;
Widget NoteEventVelocityField;

Widget NoteEventDurationLabel;
Widget NoteEventDurationField;

Widget NoteEventBottomBox;
Widget NoteEventOK;
Widget NoteEventCancel;

XPoint op;
char FormatBuffer[16];

EventList 	ReturnEvent;
MIDIEvent	EventBuffer;
char	       *OutBufferStr;
float		FloatTime;
long		OutTime;
float		FloatDuration;
long		OutDuration;
byte		OutPitch;
byte		OutVelocity;
byte		OutChannel;

BEGIN("Midi_NoteEventDlg");

	NoteEventDlg = XtCreatePopupShell("Note Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	NoteEventPane = YCreateWidget("Note Event Pane", panedWidgetClass, 
				      NoteEventDlg);

	NoteEventTopBox = YCreateShadedWidget("Note Event Title Box", boxWidgetClass,
					      NoteEventPane, MediumShade);


	if (New)
	{
		NoteEventLabel = YCreateLabel("Create New Note Event", NoteEventTopBox);
	}
	else NoteEventLabel = YCreateLabel("Modify Note Event", NoteEventTopBox);

	NoteEventForm = YCreateShadedWidget("Note Event Form", formWidgetClass,
					    NoteEventPane, LightShade);

	
	NoteEventTimeLabel     = YCreateLabel("Time:",     NoteEventForm);
	NoteEventChannelLabel  = YCreateLabel("Channel:",  NoteEventForm);
	NoteEventPitchLabel    = YCreateLabel("Pitch:",    NoteEventForm);
	NoteEventOctaveLabel   = YCreateLabel("Octave:",   NoteEventForm);
	NoteEventVelocityLabel = YCreateLabel("Velocity:", NoteEventForm);
	NoteEventDurationLabel = YCreateLabel("Duration:", NoteEventForm);

	NoteEventTimeField = YCreateSurroundedWidget("Note Time", asciiTextWidgetClass,
						      NoteEventForm, NoShade, NoShade);

	NoteEventChannelField = YCreateSurroundedWidget("Note Channel", asciiTextWidgetClass,
						        NoteEventForm, NoShade, NoShade);

	NoteEventPitchField = YCreateSurroundedWidget("Note Pitch", asciiTextWidgetClass,
						      NoteEventForm, NoShade, NoShade);

	NoteEventOctaveField = YCreateSurroundedWidget("Note Octave", asciiTextWidgetClass,
						       NoteEventForm, NoShade, NoShade);

	NoteEventVelocityField = YCreateSurroundedWidget("Note Velocity", asciiTextWidgetClass,
						         NoteEventForm, NoShade, NoShade);

	NoteEventDurationField = YCreateSurroundedWidget("Note Duration", asciiTextWidgetClass,
						         NoteEventForm, NoShade, NoShade);

	NoteEventBottomBox = YCreateShadedWidget("Note Event Button Box", boxWidgetClass,
						 NoteEventPane, MediumShade);

	NoteEventOK     = YCreateCommand("OK", NoteEventBottomBox);
	NoteEventCancel = YCreateCommand("Cancel", NoteEventBottomBox);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(NoteEventOK, XtNcallback, Midi_NoteEvtOKCB, NULL);
	XtAddCallback(NoteEventCancel, XtNcallback, Midi_NoteEvtCancelCB, NULL);

	
	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(NoteEventTimeField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Channel);
	YSetValue(NoteEventChannelField, XtNstring, FormatBuffer);

	YSetValue(NoteEventPitchField, XtNstring, Notes[Pitch % 12]);

	sprintf(FormatBuffer, "%d", Pitch / 12);
	YSetValue(NoteEventOctaveField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Velocity);
	YSetValue(NoteEventVelocityField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(Duration));
	YSetValue(NoteEventDurationField, XtNstring, FormatBuffer);


	/************************************/
	/* Format the form widget contents. */
	/************************************/

	YSetValue(XtParent(NoteEventTimeField), XtNfromHoriz, XtParent(NoteEventDurationLabel));

	YSetValue(XtParent(NoteEventChannelLabel), XtNfromVert, XtParent(NoteEventTimeLabel));
	YSetValue(XtParent(NoteEventChannelField), XtNfromVert, XtParent(NoteEventTimeLabel));
	YSetValue(XtParent(NoteEventChannelField), XtNfromHoriz, XtParent(NoteEventDurationLabel));

	YSetValue(XtParent(NoteEventPitchLabel), XtNfromVert, XtParent(NoteEventChannelLabel));
	YSetValue(XtParent(NoteEventPitchField), XtNfromVert, XtParent(NoteEventChannelLabel));
	YSetValue(XtParent(NoteEventPitchField), XtNfromHoriz, XtParent(NoteEventDurationLabel));

	YSetValue(XtParent(NoteEventOctaveLabel), XtNfromVert, XtParent(NoteEventPitchLabel));
	YSetValue(XtParent(NoteEventOctaveField), XtNfromVert, XtParent(NoteEventPitchLabel));
	YSetValue(XtParent(NoteEventOctaveField), XtNfromHoriz, XtParent(NoteEventDurationLabel));

	YSetValue(XtParent(NoteEventVelocityLabel), XtNfromVert, XtParent(NoteEventOctaveLabel));
	YSetValue(XtParent(NoteEventVelocityField), XtNfromVert, XtParent(NoteEventOctaveLabel));
	YSetValue(XtParent(NoteEventVelocityField), XtNfromHoriz, XtParent(NoteEventDurationLabel));

	YSetValue(XtParent(NoteEventDurationLabel), XtNfromVert, XtParent(NoteEventVelocityLabel));
	YSetValue(XtParent(NoteEventDurationField), XtNfromVert, XtParent(NoteEventVelocityLabel));
	YSetValue(XtParent(NoteEventDurationField), XtNfromHoriz, XtParent(NoteEventDurationLabel));

	if (New)
	{
		YSetValue(NoteEventTimeField,     XtNeditType, XawtextEdit);
	}
	else YSetValue(NoteEventTimeField, XtNdisplayCaret, False);

	YSetValue(NoteEventChannelField,  XtNeditType, XawtextEdit);
	YSetValue(NoteEventPitchField,    XtNeditType, XawtextEdit);
	YSetValue(NoteEventOctaveField,   XtNeditType, XawtextEdit);
	YSetValue(NoteEventVelocityField, XtNeditType, XawtextEdit);
	YSetValue(NoteEventDurationField, XtNeditType, XawtextEdit);

	op = YPlacePopupAndWarp(NoteEventDlg, XtGrabNonexclusive,
				NoteEventOK, NoteEventCancel);

	YAssertDialogueActions(NoteEventDlg, NoteEventOK,
			       NoteEventCancel, NULL);

	Done = RUNNING;

	while(True)
	{  
		while (!Done || XtAppPending(appContext)) 
		{
			XtAppProcessEvent(appContext, XtIMAll);
		}

		if (Done == CANCELLED)
		{
			YPopdown(NoteEventDlg);
			YPopPointerPosition();
			XtDestroyWidget(NoteEventDlg);
			Midi_ClearCurrentXEvents();
			RETURN_PTR(NULL);
		}

		YGetValue(NoteEventTimeField, XtNstring, &OutBufferStr);
		FloatTime = atof(OutBufferStr);	
		OutTime = Midi_BeatToTime(FloatTime);

		YGetValue(NoteEventDurationField, XtNstring, &OutBufferStr);
		FloatDuration = atof(OutBufferStr);
		OutDuration = Midi_BeatToTime(FloatDuration);

		YGetValue(NoteEventPitchField, XtNstring, &OutBufferStr);

		for(OutPitch = 0; OutPitch < 13; ++OutPitch)
		{
			if (!strcasecmp(OutBufferStr, Notes[OutPitch])) break;
		}

		if (OutPitch == 13)
		{
			YQuery(NoteEventDlg, "Invalid Pitch, Please re-enter.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else
		{
			YGetValue(NoteEventOctaveField, XtNstring, &OutBufferStr);
			OutPitch += 12 * atoi(OutBufferStr);

			YGetValue(NoteEventVelocityField, XtNstring, &OutBufferStr);
			OutVelocity = (byte)atoi(OutBufferStr);

			YGetValue(NoteEventChannelField, XtNstring, &OutBufferStr);
			OutChannel = (byte)atoi(OutBufferStr);

			if ( OutVelocity > 127 )
			{
				YQuery(NoteEventDlg, "Invalid Velocity.", 1, 0, 0, "Continue", NULL);
				Done = RUNNING;
                        }
			else if ( OutChannel > 15 )
			{
				YQuery(NoteEventDlg, "Invalid Channel.", 1, 0, 0, "Continue", NULL);
				Done = RUNNING;
                        }
			else break;
		}
	}

	EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));

	if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

	EventBuffer->EventCode = CreateMessageByte(MIDI_NOTE_ON, OutChannel);
	EventBuffer->DeltaTime = OutTime;
	EventBuffer->EventData.Note.Note = OutPitch;
	EventBuffer->EventData.Note.Velocity = OutVelocity;
	EventBuffer->EventData.Note.Duration = OutDuration;

	ReturnEvent = Midi_EventCreateList(EventBuffer, False);
	/*XtFree(EventBuffer);*/

	YPopdown(NoteEventDlg);
	YPopPointerPosition();
	XtDestroyWidget(NoteEventDlg);
	Midi_ClearCurrentXEvents();

RETURN_PTR(ReturnEvent);
}



/*
================================================

	     CONTROLLER CHANGE DIALOG

================================================
*/

EventList Midi_CtrlChngEventDlg(long time,       byte Channel,
				byte Controller, byte Value,   Boolean New)
{
Widget CtrlChngDlg;
Widget CtrlChngPane;
Widget CtrlChngTopBox;
Widget CtrlChngLabel;

Widget CtrlChngForm;

Widget CtrlChngTimeLabel;
Widget CtrlChngTimeField;

Widget CtrlChngChannelLabel;
Widget CtrlChngChannelField;

Widget CtrlChngControllerLabel;
Widget CtrlChngControllerField;

Widget CtrlChngValueLabel;
Widget CtrlChngValueField;

Widget CtrlChngBottomBox;
Widget CtrlChngOK;
Widget CtrlChngCancel;

XPoint op;
char FormatBuffer[16];

EventList 	ReturnEvent;
MIDIEvent	EventBuffer;
char	       *OutBufferStr;
float		FloatTime;
long		OutTime;
long		OutChannel;
byte 		OutController;
byte		OutValue;

BEGIN("Midi_CtrlChngEventDlg");

	CtrlChngDlg = XtCreatePopupShell("Note Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	CtrlChngPane = YCreateWidget("Note Event Pane", panedWidgetClass, 
				      CtrlChngDlg);

	CtrlChngTopBox = YCreateShadedWidget("Note Event Title Box", boxWidgetClass,
					      CtrlChngPane, MediumShade);


	if (New)
	{
		CtrlChngLabel = YCreateLabel("Create New Controller Change Event", CtrlChngTopBox);
	}
	else CtrlChngLabel = YCreateLabel("Modify Controller Change Event", CtrlChngTopBox);

	CtrlChngForm = YCreateShadedWidget("Note Event Form", formWidgetClass,
					    CtrlChngPane, LightShade);

	
	CtrlChngTimeLabel       = YCreateLabel("Time:",     CtrlChngForm);
	CtrlChngChannelLabel    = YCreateLabel("Channel:",  CtrlChngForm);
	CtrlChngControllerLabel = YCreateLabel("Controller:", CtrlChngForm);
	CtrlChngValueLabel      = YCreateLabel("Value:",   CtrlChngForm);

	CtrlChngTimeField = YCreateSurroundedWidget("Controller Change Time", asciiTextWidgetClass,
						      CtrlChngForm, NoShade, NoShade);

	CtrlChngChannelField = YCreateSurroundedWidget("Controller Change Channel", 
							asciiTextWidgetClass,
						        CtrlChngForm, NoShade, NoShade);

	CtrlChngControllerField = YCreateSurroundedWidget("Controller Change Controller", 
							asciiTextWidgetClass,
						        CtrlChngForm, NoShade, NoShade);

	CtrlChngValueField = YCreateSurroundedWidget("Controller Change Value", asciiTextWidgetClass,
						      CtrlChngForm, NoShade, NoShade);

	
	CtrlChngBottomBox = YCreateShadedWidget("Controller Change Button Box", boxWidgetClass,
						 CtrlChngPane, MediumShade);

	CtrlChngOK     = YCreateCommand("OK", CtrlChngBottomBox);
	CtrlChngCancel = YCreateCommand("Cancel", CtrlChngBottomBox);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(CtrlChngOK, XtNcallback, Midi_NoteEvtOKCB, NULL);
	XtAddCallback(CtrlChngCancel, XtNcallback, Midi_NoteEvtCancelCB, NULL);

	YSetValue(XtParent(CtrlChngTimeField), XtNfromHoriz, XtParent(CtrlChngControllerLabel));

	YSetValue(XtParent(CtrlChngChannelLabel), XtNfromVert, XtParent(CtrlChngTimeLabel));
	YSetValue(XtParent(CtrlChngChannelField), XtNfromVert, XtParent(CtrlChngTimeLabel));
	YSetValue(XtParent(CtrlChngChannelField), XtNfromHoriz, XtParent(CtrlChngControllerLabel));

	YSetValue(XtParent(CtrlChngControllerLabel), XtNfromVert, XtParent(CtrlChngChannelLabel));
	YSetValue(XtParent(CtrlChngControllerField), XtNfromVert, XtParent(CtrlChngChannelLabel));
	YSetValue(XtParent(CtrlChngControllerField), XtNfromHoriz, XtParent(CtrlChngControllerLabel));

	YSetValue(XtParent(CtrlChngValueLabel), XtNfromVert, XtParent(CtrlChngControllerLabel));
	YSetValue(XtParent(CtrlChngValueField), XtNfromVert, XtParent(CtrlChngControllerLabel));
	YSetValue(XtParent(CtrlChngValueField), XtNfromHoriz, XtParent(CtrlChngControllerLabel));

	if (New)
	{
		YSetValue(CtrlChngTimeField,        XtNeditType, XawtextEdit);
	}
	else YSetValue(CtrlChngTimeField, XtNdisplayCaret, False);

	YSetValue(CtrlChngChannelField,     XtNeditType, XawtextEdit);
	YSetValue(CtrlChngControllerField,  XtNeditType, XawtextEdit);
	YSetValue(CtrlChngValueField,       XtNeditType, XawtextEdit);

	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(CtrlChngTimeField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Channel);
	YSetValue(CtrlChngChannelField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Controller);
	YSetValue(CtrlChngControllerField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Value);
	YSetValue(CtrlChngValueField, XtNstring, FormatBuffer);

	op = YPlacePopupAndWarp(CtrlChngDlg, XtGrabNonexclusive,
				CtrlChngOK, CtrlChngCancel);

	YAssertDialogueActions(CtrlChngDlg, CtrlChngOK, CtrlChngCancel, NULL);

	Done = RUNNING;

	while(True)
	{ 
		while (!Done || XtAppPending(appContext)) 
		{
			XtAppProcessEvent(appContext, XtIMAll);
		}

		if (Done == CANCELLED)
		{
			YPopdown(CtrlChngDlg);
			YPopPointerPosition();
			XtDestroyWidget(CtrlChngDlg);
			Midi_ClearCurrentXEvents();
			RETURN_PTR(NULL);
		}

		YGetValue(CtrlChngTimeField, XtNstring, &OutBufferStr);
		FloatTime = atof(OutBufferStr);	
		OutTime   = Midi_BeatToTime(FloatTime);

		YGetValue(CtrlChngChannelField, XtNstring, &OutBufferStr);
		OutChannel = (byte)atoi(OutBufferStr);	
	
		YGetValue(CtrlChngControllerField, XtNstring, &OutBufferStr);
		OutController = (byte)atoi(OutBufferStr);

		YGetValue(CtrlChngValueField, XtNstring, &OutBufferStr);
		OutValue = (byte)atoi(OutBufferStr);

		if (OutChannel > 15)
		{
			YQuery(CtrlChngDlg, "Invalid Channel.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else if (OutController > 127)
		{
			YQuery(CtrlChngDlg, "Invalid Controller Number.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else if (OutValue > 127)
		{
			YQuery(CtrlChngDlg, "Invalid Controller Value", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else break;
	}


	EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));

	if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

	EventBuffer->EventCode = CreateMessageByte(MIDI_CTRL_CHANGE, OutChannel);
	EventBuffer->DeltaTime = OutTime;
	EventBuffer->EventData.ControlChange.Controller = OutController;
	EventBuffer->EventData.ControlChange.Value      = OutValue;

	ReturnEvent = Midi_EventCreateList(EventBuffer, False);
	/*XtFree(EventBuffer);*/

	YPopdown(CtrlChngDlg);
	YPopPointerPosition();
	XtDestroyWidget(CtrlChngDlg);
	Midi_ClearCurrentXEvents();

RETURN_PTR(ReturnEvent);
}


#define Midi_ProgEvtCancelCB 	Midi_TextEvtCancelCB
#define Midi_ProgEvtOKCB	Midi_TextEvtOKCB


EventList Midi_ProgramChangeEventDlg(long time, byte Channel, byte Program, Boolean New)
{
Widget ProgEventPane;
Widget ProgEventTopBox;

Widget ProgEventLabel;

Widget ProgEventForm;

Widget ProgEventTimeLabel;
Widget ProgEventTimeField;

Widget ProgEventChannelLabel;
Widget ProgEventChannelField;

Widget ProgEventProgramLabel;
Widget ProgEventProgramField;

Widget ProgEventBottomBox;
Widget ProgEventOK;
Widget ProgEventCancel;

EventList ReturnEvent;
MIDIEvent EventBuffer;
char   *OutBufferStr;
long  OutTime;
long  OutChannel;
byte  OutProgram;
float FloatTime;

XPoint op;
char FormatBuffer[16];


BEGIN("Midi_ProgramChangeEventDlg");


	ProgEventDlg = XtCreatePopupShell("Prog Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	ProgEventPane = YCreateWidget("Prog Event Pane", panedWidgetClass, 
				      ProgEventDlg);

	ProgEventTopBox = YCreateShadedWidget("Prog Event Title Box", boxWidgetClass,
					      ProgEventPane, MediumShade);


	if (New)
	{
		ProgEventLabel = YCreateLabel("Create New Program Change Event", ProgEventTopBox);
	}
	else ProgEventLabel = YCreateLabel("Modify Program Change Event", ProgEventTopBox);

	ProgEventForm = YCreateShadedWidget("Prog Event Form", formWidgetClass,
					    ProgEventPane, LightShade);
	
	ProgEventTimeLabel     = YCreateLabel("Time:",     ProgEventForm);
	ProgEventChannelLabel  = YCreateLabel("Channel:",  ProgEventForm);
	ProgEventProgramLabel  = YCreateLabel("Program Number:",    ProgEventForm);
	
	ProgEventTimeField = YCreateSurroundedWidget("Prog Time", asciiTextWidgetClass,
						      ProgEventForm, NoShade, NoShade);

	ProgEventChannelField = YCreateSurroundedWidget("Prog Channel", asciiTextWidgetClass,
						        ProgEventForm, NoShade, NoShade);


	ProgEventProgramField = YCreateSurroundedWidget("Prog Number", asciiTextWidgetClass,
						         ProgEventForm, NoShade, NoShade);
 


	ProgEventBottomBox = YCreateShadedWidget("Prog Event Button Box", boxWidgetClass,
						 ProgEventPane, MediumShade);

	ProgEventOK     = YCreateCommand("OK", ProgEventBottomBox);
	ProgEventCancel = YCreateCommand("Cancel", ProgEventBottomBox);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(ProgEventOK, XtNcallback, Midi_ProgEvtOKCB, NULL);
	XtAddCallback(ProgEventCancel, XtNcallback, Midi_ProgEvtCancelCB, NULL);

	
	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(ProgEventTimeField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Channel);
	YSetValue(ProgEventChannelField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Program);
	YSetValue(ProgEventProgramField, XtNstring, FormatBuffer);

	YSetValue(XtParent(ProgEventTimeField), XtNfromHoriz, XtParent(ProgEventProgramLabel));

	YSetValue(XtParent(ProgEventChannelLabel), XtNfromVert, XtParent(ProgEventTimeLabel));
	YSetValue(XtParent(ProgEventChannelField), XtNfromVert, XtParent(ProgEventTimeLabel));
	YSetValue(XtParent(ProgEventChannelField), XtNfromHoriz, XtParent(ProgEventProgramLabel));

	YSetValue(XtParent(ProgEventProgramLabel), XtNfromVert, XtParent(ProgEventChannelLabel));
	YSetValue(XtParent(ProgEventProgramField), XtNfromVert, XtParent(ProgEventChannelLabel));
	YSetValue(XtParent(ProgEventProgramField),  XtNfromHoriz, XtParent(ProgEventProgramLabel));

	if (New)
	{
		YSetValue(ProgEventTimeField, XtNeditType, XawtextEdit);
	}
	else YSetValue(ProgEventTimeField, XtNdisplayCaret, False);

	YSetValue(ProgEventChannelField, XtNeditType, XawtextEdit);
	YSetValue(ProgEventProgramField, XtNeditType, XawtextEdit);

	op = YPlacePopupAndWarp(ProgEventDlg, XtGrabNonexclusive,
				ProgEventOK, ProgEventCancel);

	YAssertDialogueActions(ProgEventDlg, ProgEventOK,
			       ProgEventCancel, NULL);

	Done = RUNNING;


	while(True)
	{  
		while (!Done || XtAppPending(appContext)) 
		{
			XtAppProcessEvent(appContext, XtIMAll);
		}

		if (Done == CANCELLED)
		{
			YPopdown(ProgEventDlg);
			YPopPointerPosition();
			XtDestroyWidget(ProgEventDlg);
			Midi_ClearCurrentXEvents();
			RETURN_PTR(NULL);
		}

		YGetValue(ProgEventTimeField, XtNstring, &OutBufferStr);
		FloatTime = atof(OutBufferStr);	
		OutTime   = Midi_BeatToTime(FloatTime);

		YGetValue(ProgEventChannelField, XtNstring, &OutBufferStr);
		OutChannel = (byte)atoi(OutBufferStr);	
	
		YGetValue(ProgEventProgramField, XtNstring, &OutBufferStr);
		OutProgram = (byte)atoi(OutBufferStr);

		if (OutChannel > 15)
		{
			YQuery(ProgEventDlg, "Invalid Channel.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else if (OutProgram > 127)
		{
			YQuery(ProgEventDlg, "Invalid Program Number.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else break;
	}

    EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));

    if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

    EventBuffer->EventCode = CreateMessageByte(MIDI_PROG_CHANGE, OutChannel);
    EventBuffer->DeltaTime = OutTime;
    EventBuffer->EventData.ProgramChange.Program = OutProgram;

    ReturnEvent = Midi_EventCreateList(EventBuffer, False);

    YPopdown(ProgEventDlg);
    YPopPointerPosition();
    XtDestroyWidget(ProgEventDlg);
    Midi_ClearCurrentXEvents();

RETURN_PTR(ReturnEvent);
}


void AftertouchSetSensitive(void)
{
    if (aftertouchtype)
    {
        YSetValue(AfterEventNoteLabel, XtNsensitive, False);
        YSetValue(AfterEventNoteField, XtNsensitive, False);
    }
    else 
    {
        YSetValue(AfterEventNoteLabel, XtNsensitive, True);
        YSetValue(AfterEventNoteField, XtNsensitive, True);
    }
}

static void AftertouchMenuCB(Widget w, XtPointer client, XtPointer call)
{
BEGIN("AftertouchMenuCB");
    aftertouchtype =  YGetCurrentOption(AfterTypeField);

    AftertouchSetSensitive();
END;
}

#define Midi_AfterEvtOKCB 	Midi_TextEvtOKCB
#define Midi_AfterEvtCancelCB   Midi_TextEvtCancelCB

EventList Midi_AftertouchEventDlg(long time, byte Channel, byte Aftertouch,
                                   byte Note, Boolean Type, Boolean New)
{
Widget AfterEventPane;
Widget AfterEventTopBox;

Widget AfterEventLabel;

Widget AfterEventForm;

Widget AfterEventTimeLabel;
Widget AfterEventTimeField;

Widget AfterEventChannelLabel;
Widget AfterEventChannelField;


Widget AfterEventAfterLabel;
Widget AfterEventAfterField;

Widget AfterEventBottomBox;
Widget AfterEventOK;
Widget AfterEventCancel;

EventList ReturnEvent;
MIDIEvent EventBuffer;
char   *OutBufferStr;
long  OutTime;
long  OutChannel;
byte  OutNote;
long  OutAfter;
float FloatTime;

XPoint op;
char FormatBuffer[16];

String AftertouchTypes[] = { "Polyphonic Aftertouch", "Channel Aftertouch" };

BEGIN("Midi_AftertouchEventDlg");


	AfterEventDlg = XtCreatePopupShell("Aftertouch Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	AfterEventPane = YCreateWidget("Aftertouch Event Pane", panedWidgetClass, 
				      AfterEventDlg);

	AfterEventTopBox = YCreateShadedWidget("Aftertouch Event Title Box", boxWidgetClass,
					      AfterEventPane, MediumShade);

	if (New)
	{
		AfterEventLabel = YCreateLabel("Create New Aftertouch Event", AfterEventTopBox);
	}
	else AfterEventLabel = YCreateLabel("Modify Aftertouch Event", AfterEventTopBox);

	AfterEventForm = YCreateShadedWidget("Aftertouch Event Form", formWidgetClass,
					    AfterEventPane, LightShade);
	
        AfterTypeField = YCreateOptionMenu(AfterEventForm, AftertouchTypes,
                                 XtNumber(AftertouchTypes), aftertouchtype, AftertouchMenuCB, 0);

	AfterEventTimeLabel     = YCreateLabel("Time:",     AfterEventForm);
	AfterEventChannelLabel  = YCreateLabel("Channel:",  AfterEventForm);
	AfterEventNoteLabel  = YCreateLabel("Note:",    AfterEventForm);
        AfterEventAfterLabel = YCreateLabel("Aftertouch:", AfterEventForm);
	
	AfterEventTimeField = YCreateSurroundedWidget("Aftertouch Time", asciiTextWidgetClass,
						      AfterEventForm, NoShade, NoShade);

	AfterEventChannelField = YCreateSurroundedWidget("Aftertouch Channel", asciiTextWidgetClass,
						        AfterEventForm, NoShade, NoShade);

        AfterEventNoteField = YCreateSurroundedWidget("Aftertouch Note", asciiTextWidgetClass,
							AfterEventForm, NoShade, NoShade);

	AfterEventAfterField = YCreateSurroundedWidget("Aftertouch value", asciiTextWidgetClass,
						         AfterEventForm, NoShade, NoShade);
 
	AfterEventBottomBox = YCreateShadedWidget("After Event Button Box", boxWidgetClass,
						 AfterEventPane, MediumShade);

	AfterEventOK     = YCreateCommand("OK", AfterEventBottomBox);
	AfterEventCancel = YCreateCommand("Cancel", AfterEventBottomBox);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(AfterEventOK, XtNcallback, Midi_AfterEvtOKCB, NULL);
	XtAddCallback(AfterEventCancel, XtNcallback, Midi_AfterEvtCancelCB, NULL);
	
	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(AfterEventTimeField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Note);
	YSetValue(AfterEventNoteField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Channel);
	YSetValue(AfterEventChannelField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Aftertouch);
	YSetValue(AfterEventAfterField, XtNstring, FormatBuffer);

	YSetValue(XtParent(AfterEventTimeField), XtNfromVert, XtParent(AfterTypeField));
	YSetValue(XtParent(AfterEventTimeLabel), XtNfromVert, XtParent(AfterTypeField));
        YSetValue(XtParent(AfterEventTimeField), XtNfromHoriz, XtParent(AfterEventTimeLabel));

        YSetValue(XtParent(AfterEventTimeField), XtNfromHoriz, XtParent(AfterEventAfterLabel));

        YSetValue(XtParent(AfterEventChannelField), XtNfromHoriz, XtParent(AfterEventAfterLabel));
        YSetValue(XtParent(AfterEventNoteField), XtNfromHoriz, XtParent(AfterEventAfterLabel));
        YSetValue(XtParent(AfterEventAfterField), XtNfromHoriz, XtParent(AfterEventAfterLabel));

	YSetValue(XtParent(AfterEventChannelField), XtNfromVert, XtParent(AfterEventTimeField));
	YSetValue(XtParent(AfterEventNoteField), XtNfromVert, XtParent(AfterEventChannelField));
	YSetValue(XtParent(AfterEventAfterField), XtNfromVert, XtParent(AfterEventNoteField));

	YSetValue(XtParent(AfterEventChannelLabel), XtNfromVert, XtParent(AfterEventTimeLabel));
	YSetValue(XtParent(AfterEventNoteLabel), XtNfromVert, XtParent(AfterEventChannelLabel));
	YSetValue(XtParent(AfterEventAfterLabel), XtNfromVert, XtParent(AfterEventNoteLabel));


	if (New)
	{
		YSetValue(AfterEventTimeField, XtNeditType, XawtextEdit);
	}
	else YSetValue(AfterEventTimeField, XtNdisplayCaret, False);

	YSetValue(AfterEventTimeField, XtNeditType, XawtextEdit);
	YSetValue(AfterEventChannelField, XtNeditType, XawtextEdit);
	YSetValue(AfterEventNoteField, XtNeditType, XawtextEdit);
	YSetValue(AfterEventAfterField, XtNeditType, XawtextEdit);

	aftertouchtype = Type;

        YSetCurrentOption(AfterTypeField, aftertouchtype);
        AftertouchSetSensitive();

	op = YPlacePopupAndWarp(AfterEventDlg, XtGrabNonexclusive,
				AfterEventOK, AfterEventCancel);

	YAssertDialogueActions(AfterEventDlg, AfterEventOK,
			       AfterEventCancel, NULL);
	YSetValue(XtParent(AfterEventAfterField), XtNfromHoriz, XtParent(AfterEventAfterLabel));

        YFixOptionMenuLabel(AfterTypeField);

	Done = RUNNING;


	while(True)
	{  
		while (!Done || XtAppPending(appContext)) 
		{
			XtAppProcessEvent(appContext, XtIMAll);
		}

		if (Done == CANCELLED)
		{
			YPopdown(AfterEventDlg);
			YPopPointerPosition();
			YDestroyOptionMenu(AfterTypeField);
			XtDestroyWidget(AfterEventDlg);
			Midi_ClearCurrentXEvents();
			RETURN_PTR(NULL);
		}

		YGetValue(AfterEventTimeField, XtNstring, &OutBufferStr);
		FloatTime = atof(OutBufferStr);	
		OutTime   = Midi_BeatToTime(FloatTime);

		YGetValue(AfterEventChannelField, XtNstring, &OutBufferStr);
		OutChannel = (byte)atoi(OutBufferStr);	
	
		YGetValue(AfterEventNoteField, XtNstring, &OutBufferStr);
		OutNote = (byte)atoi(OutBufferStr);

		YGetValue(AfterEventAfterField, XtNstring, &OutBufferStr);
                OutAfter = (long)atoi(OutBufferStr);

		if (OutChannel > 15)
		{
			YQuery(AfterEventDlg, "Invalid Channel.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else if (OutNote > 127)
		{
			YQuery(AfterEventDlg, "Invalid Note Number.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else if (OutAfter > 127 )
                {
			YQuery(AfterEventDlg, "Invalid Aftertouch Value.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
                }
		else break;
	}

    EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));

    if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

    if (!aftertouchtype)
    {
        EventBuffer->EventCode = CreateMessageByte(MIDI_POLY_AFTERTOUCH, OutChannel);
        EventBuffer->EventData.PolyAftertouch.Note = OutNote;
    }
    else
    {
        EventBuffer->EventCode = CreateMessageByte(MIDI_CHNL_AFTERTOUCH, OutChannel);
    }

    EventBuffer->DeltaTime = OutTime;
    EventBuffer->EventData.PolyAftertouch.Velocity = OutAfter;

    ReturnEvent = Midi_EventCreateList(EventBuffer, False);

    YPopdown(AfterEventDlg);
    YPopPointerPosition();
    YDestroyOptionMenu(AfterTypeField);
    XtDestroyWidget(AfterEventDlg);
    Midi_ClearCurrentXEvents();

RETURN_PTR(ReturnEvent);
}

#define Midi_PitchBendOKCB 	Midi_TextEvtOKCB
#define Midi_PitchBendCancelCB   Midi_TextEvtCancelCB

EventList Midi_PitchBendEventDlg(long time, byte Channel, byte LSB, byte MSB, Boolean New)
{
Widget PitchBendEventTopBox;

Widget PitchBendEventDlg;
Widget PitchBendEventPane;

Widget PitchBendEventForm;

Widget PitchBendEventLabel;

Widget PitchBendEventTimeLabel;
Widget PitchBendEventTimeField;

Widget PitchBendEventChannelLabel;
Widget PitchBendEventChannelField;

Widget PitchBendEventValueLabel;
Widget PitchBendEventValueField;

Widget PitchBendEventBottomBox;
Widget PitchBendEventOK;
Widget PitchBendEventCancel;

EventList ReturnEvent;
MIDIEvent EventBuffer;
char   *OutBufferStr;
long  OutTime;
long  OutChannel;
long  OutValue;
float FloatTime;
long Value = 0;

XPoint op;
char FormatBuffer[16];


BEGIN("Midi_PitchBendEventDlg");

	/* convert bytes into ptich bend value */
        Value = LSB | ( MSB << 7 );

	PitchBendEventDlg = XtCreatePopupShell("PB Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	PitchBendEventPane = YCreateWidget("PB Event Pane", panedWidgetClass, 
				      PitchBendEventDlg);

	PitchBendEventTopBox = YCreateShadedWidget("PB Event Title Box", boxWidgetClass,
					      PitchBendEventPane, MediumShade);

	if (New)
	{
		PitchBendEventLabel = YCreateLabel("Create New Pitch Bend Event", PitchBendEventTopBox);
	}
	else PitchBendEventLabel = YCreateLabel("Modify Pitch Bend Event", PitchBendEventTopBox);

	PitchBendEventForm = YCreateShadedWidget("PB Event Form", formWidgetClass,
					    PitchBendEventPane, LightShade);
	
	PitchBendEventTimeLabel     = YCreateLabel("Time:",     PitchBendEventForm);
	PitchBendEventChannelLabel  = YCreateLabel("Channel:",  PitchBendEventForm);
	PitchBendEventValueLabel  = YCreateLabel("Value:",    PitchBendEventForm);
	
	PitchBendEventTimeField = YCreateSurroundedWidget("PB Time", asciiTextWidgetClass,
						      PitchBendEventForm, NoShade, NoShade);

	PitchBendEventChannelField = YCreateSurroundedWidget("PB Channel", asciiTextWidgetClass,
						        PitchBendEventForm, NoShade, NoShade);

	PitchBendEventValueField = YCreateSurroundedWidget("PB Number", asciiTextWidgetClass,
						         PitchBendEventForm, NoShade, NoShade);

	PitchBendEventBottomBox = YCreateShadedWidget("PB Event Button Box", boxWidgetClass,
						 PitchBendEventPane, MediumShade);

	PitchBendEventOK     = YCreateCommand("OK", PitchBendEventBottomBox);
	PitchBendEventCancel = YCreateCommand("Cancel", PitchBendEventBottomBox);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(PitchBendEventOK, XtNcallback, Midi_PitchBendOKCB, NULL);
	XtAddCallback(PitchBendEventCancel, XtNcallback, Midi_PitchBendCancelCB, NULL);

	
	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(PitchBendEventTimeField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%d", Channel);
	YSetValue(PitchBendEventChannelField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%ld", Value);
	YSetValue(PitchBendEventValueField, XtNstring, FormatBuffer);

	YSetValue(XtParent(PitchBendEventTimeField), XtNfromHoriz, XtParent(PitchBendEventChannelLabel));
	YSetValue(XtParent(PitchBendEventChannelField), XtNfromHoriz, XtParent(PitchBendEventChannelLabel));
	YSetValue(XtParent(PitchBendEventValueField), XtNfromHoriz, XtParent(PitchBendEventChannelLabel));

	YSetValue(XtParent(PitchBendEventChannelLabel), XtNfromVert, XtParent(PitchBendEventTimeLabel));
	YSetValue(XtParent(PitchBendEventValueLabel), XtNfromVert, XtParent(PitchBendEventChannelLabel));

	YSetValue(XtParent(PitchBendEventChannelField), XtNfromVert, XtParent(PitchBendEventTimeField));
	YSetValue(XtParent(PitchBendEventValueField), XtNfromVert, XtParent(PitchBendEventChannelField));

	if (New)
	{
		YSetValue(PitchBendEventTimeField, XtNeditType, XawtextEdit);
	}
	else YSetValue(PitchBendEventTimeField, XtNdisplayCaret, False);

	YSetValue(PitchBendEventChannelField, XtNeditType, XawtextEdit);
	YSetValue(PitchBendEventValueField, XtNeditType, XawtextEdit);

	op = YPlacePopupAndWarp(PitchBendEventDlg, XtGrabNonexclusive,
				PitchBendEventOK, PitchBendEventCancel);

	YAssertDialogueActions(PitchBendEventDlg, PitchBendEventOK,
			       PitchBendEventCancel, NULL);

	Done = RUNNING;

	while(True)
	{  
		while (!Done || XtAppPending(appContext)) 
		{
			XtAppProcessEvent(appContext, XtIMAll);
		}

		if (Done == CANCELLED)
		{
			YPopdown(PitchBendEventDlg);
			YPopPointerPosition();
			XtDestroyWidget(PitchBendEventDlg);
			Midi_ClearCurrentXEvents();
			RETURN_PTR(NULL);
		}

		YGetValue(PitchBendEventTimeField, XtNstring, &OutBufferStr);
		FloatTime = atof(OutBufferStr);	
		OutTime   = Midi_BeatToTime(FloatTime);

		YGetValue(PitchBendEventChannelField, XtNstring, &OutBufferStr);
		OutChannel = (byte)atoi(OutBufferStr);	
	
		YGetValue(PitchBendEventValueField, XtNstring, &OutBufferStr);
		OutValue = atoi(OutBufferStr);

		if (OutChannel > 15)
		{
			YQuery(PitchBendEventDlg, "Invalid Channel.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else if (OutValue > 16383 )
		{
			YQuery(PitchBendEventDlg, "Invalid Pitch Bend Value.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else break;
	}

    EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));

    if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

    EventBuffer->EventCode = CreateMessageByte(MIDI_PITCH_BEND, OutChannel);
    EventBuffer->DeltaTime = OutTime;
    EventBuffer->EventData.PitchWheel.LSB =  (byte)( OutValue & 0x7F );
    EventBuffer->EventData.PitchWheel.MSB =  (byte)( ( OutValue >> 7 ) & 0x7F );

    ReturnEvent = Midi_EventCreateList(EventBuffer, False);

    YPopdown(PitchBendEventDlg);
    YPopPointerPosition();
    XtDestroyWidget(PitchBendEventDlg);
    Midi_ClearCurrentXEvents();

RETURN_PTR(ReturnEvent);
}

#define Midi_TempoOKCB 	Midi_TextEvtOKCB
#define Midi_TempoCancelCB   Midi_TextEvtCancelCB

EventList Midi_TempoEventDlg(long time, long TempoValueIn, Boolean New)
{
Widget TempoEventTopBox;

Widget TempoEventDlg;
Widget TempoEventPane;

Widget TempoEventForm;

Widget TempoEventLabel;

Widget TempoEventTimeLabel;
Widget TempoEventTimeField;

Widget TempoEventValueLabel;
Widget TempoEventValueField;

Widget TempoEventBottomBox;
Widget TempoEventOK;
Widget TempoEventCancel;

EventList ReturnEvent;
MIDIEvent EventBuffer;
char   *OutBufferStr;
long  TempoValue;
long  OutTime;
long  OutValue;
float FloatTime;
long Value;

XPoint op;
char FormatBuffer[16];

BEGIN("Midi_TempoEventDlg");

        Value = TempoValueIn;

	if (!Value)
		Value = 500000;

	TempoEventDlg = XtCreatePopupShell("Tempo Event", transientShellWidgetClass, 
					  topLevel, NULL, 0);

	TempoEventPane = YCreateWidget("Tempo Event Pane", panedWidgetClass, 
				      TempoEventDlg);

	TempoEventTopBox = YCreateShadedWidget("Tempo Event Title Box", boxWidgetClass,
					      TempoEventPane, MediumShade);

	if (New)
	{
		TempoEventLabel = YCreateLabel("Create New Tempo Event", TempoEventTopBox);
	}
	else TempoEventLabel = YCreateLabel("Modify Tempo Event", TempoEventTopBox);

	TempoEventForm = YCreateShadedWidget("Tempo Event Form", formWidgetClass,
					    TempoEventPane, LightShade);
	
	TempoEventTimeLabel     = YCreateLabel("Time:",     TempoEventForm);
	
	TempoEventValueLabel  = YCreateLabel("Value:",    TempoEventForm);

	TempoEventTimeField = YCreateSurroundedWidget("PB Time", asciiTextWidgetClass,
						      TempoEventForm, NoShade, NoShade);

	TempoEventValueField = YCreateSurroundedWidget("PB Number", asciiTextWidgetClass,
						         TempoEventForm, NoShade, NoShade);

	TempoEventBottomBox = YCreateShadedWidget("Tempo Event Button Box", boxWidgetClass,
						 TempoEventPane, MediumShade);

	TempoEventOK     = YCreateCommand("OK", TempoEventBottomBox);
	TempoEventCancel = YCreateCommand("Cancel", TempoEventBottomBox);

	/******************/
	/* Add callbacks. */
	/******************/

	XtAddCallback(TempoEventOK, XtNcallback, Midi_TempoOKCB, NULL);
	XtAddCallback(TempoEventCancel, XtNcallback, Midi_TempoCancelCB, NULL);

	
	sprintf(FormatBuffer, "%7.2f", Midi_TimeToBeat(time));
	YSetValue(TempoEventTimeField, XtNstring, FormatBuffer);

	sprintf(FormatBuffer, "%ld", 60000000 / Value);
	YSetValue(TempoEventValueField, XtNstring, FormatBuffer);

	YSetValue(XtParent(TempoEventValueLabel), XtNfromVert, XtParent(TempoEventTimeLabel));
	YSetValue(XtParent(TempoEventValueField), XtNfromVert, XtParent(TempoEventTimeField));

	YSetValue(XtParent(TempoEventValueField), XtNfromHoriz, XtParent(TempoEventValueLabel));
	YSetValue(XtParent(TempoEventTimeField), XtNfromHoriz, XtParent(TempoEventValueLabel));

	if (New)
	{
		YSetValue(TempoEventTimeField, XtNeditType, XawtextEdit);
	}
	else YSetValue(TempoEventTimeField, XtNdisplayCaret, False);

	YSetValue(TempoEventValueField, XtNeditType, XawtextEdit);

	op = YPlacePopupAndWarp(TempoEventDlg, XtGrabNonexclusive,
				TempoEventOK, TempoEventCancel);

	YAssertDialogueActions(TempoEventDlg, TempoEventOK,
			       TempoEventCancel, NULL);

	Done = RUNNING;

	while(True)
	{  
		while (!Done || XtAppPending(appContext)) 
		{
			XtAppProcessEvent(appContext, XtIMAll);
		}

		if (Done == CANCELLED)
		{
			YPopdown(TempoEventDlg);
			YPopPointerPosition();
			XtDestroyWidget(TempoEventDlg);
			Midi_ClearCurrentXEvents();
			RETURN_PTR(NULL);
		}

		YGetValue(TempoEventTimeField, XtNstring, &OutBufferStr);
		FloatTime = atof(OutBufferStr);	
		OutTime   = Midi_BeatToTime(FloatTime);

		YGetValue(TempoEventValueField, XtNstring, &OutBufferStr);
		OutValue = atoi(OutBufferStr);

		if (OutValue > 16383 )
		{
			YQuery(TempoEventDlg, "Invalid Tempo Value.", 1, 0, 0, "Continue", NULL);
			Done = RUNNING;
		}
		else break;
	}

    EventBuffer = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct) + 3);

    if (!EventBuffer) Error(FATAL, "Unable to allocate event buffer.");

    TempoValue = 60000000 / OutValue;

    EventBuffer->EventCode = MIDI_FILE_META_EVENT;
    EventBuffer->EventData.MetaEvent.MetaEventCode = MIDI_SET_TEMPO;
    EventBuffer->DeltaTime = OutTime;

    EventBuffer->EventData.MetaEvent.NBytes = 3;
    EventBuffer->EventData.MetaEvent.Bytes = (TempoValue >> 16) & 0xff;
    *(&EventBuffer->EventData.MetaEvent.Bytes + 1) = (TempoValue >> 8) & 0xff;
    *(&EventBuffer->EventData.MetaEvent.Bytes + 2) = TempoValue & 0xff;

    ReturnEvent = Midi_EventCreateList(EventBuffer, False);

    /* watch out for memory leaks around these parts */

    YPopdown(TempoEventDlg);
    YPopPointerPosition();
    XtDestroyWidget(TempoEventDlg);
    Midi_ClearCurrentXEvents();

RETURN_PTR(ReturnEvent);
}
