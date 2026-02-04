/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Event.c

Description:	Functions to manipulate MIDI events.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/02/94	AJG		File Created.

002     14/02/95	AJG		Added Midi_EventTimeLessOrEqp predicate to
					fix one-to-two-point conversion problem
					(see Track.c for explanation).
--------------------------------------------------------------------------------------------
*/

#include <MidiEvent.h>
#include <Debug.h>
#include <MidiErrorHandler.h>


/**************************************************************************/
/* Midi_EventCreateList: Create a new event list from a single MIDIEvent. */
/**************************************************************************/

EventList Midi_EventCreateList(MIDIEvent NewEvent, Boolean RetainEventStruct)
{
EventList NewEventList;

BEGIN("Midi_EventCreateList");

	if (NewEvent->EventCode == MIDI_FILE_META_EVENT)
	{
		NewEventList = (EventList)NewList(sizeof(EventListElement) +
						  NewEvent->EventData.MetaEvent.NBytes);

		memcpy(&NewEventList->Event, NewEvent, 
		       sizeof(MIDIEventStruct) + NewEvent->EventData.MetaEvent.NBytes);
	}
	else 
	{
		NewEventList = (EventList)NewList(sizeof(EventListElement));
		NewEventList->Event = *NewEvent;
	}

	if (!RetainEventStruct) free(NewEvent);

RETURN_PTR(NewEventList);
}



/***************************************************/
/* Midi_EventConvertTempoToBPM: Converts the value */
/* held in a MIDI Set Tempo event into a Beats Per */
/* Minute representation.			   */
/***************************************************/

long Midi_EventConvertTempoToBPM(MIDIEvent TempoEvent)
{
long TempoValue;

BEGIN("Midi_EventConvertTempoToBPM");

	if (TempoEvent->EventCode != MIDI_FILE_META_EVENT ||
	    TempoEvent->EventData.MetaEvent.MetaEventCode != MIDI_SET_TEMPO)
	{
		RETURN_LONG(0);
	}

	TempoValue = (((TempoEvent->EventData.MetaEvent.Bytes << 8) +
 		     *(&TempoEvent->EventData.MetaEvent.Bytes + 1)) << 8) +
		     *(&TempoEvent->EventData.MetaEvent.Bytes + 2);

RETURN_LONG(60000000/TempoValue);
}




MIDIEvent Midi_EventCreateTextEvt(byte EventCode, long DeltaTime, char *Text)
{
MIDIEvent NewEvent;
int 	  Len;

BEGIN("Midi_EventCreateTextEvt");

	Len = strlen(Text);
	NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct) + Len + 1);

	if (!NewEvent)
	{
		Error(FATAL, "Unable to allocate MIDI event.");
	}

	NewEvent->EventCode = MIDI_FILE_META_EVENT;
	NewEvent->DeltaTime = DeltaTime;

	NewEvent->EventData.MetaEvent.MetaEventCode = EventCode;
	NewEvent->EventData.MetaEvent.NBytes = strlen(Text);
	strcpy((char *)&NewEvent->EventData.MetaEvent.Bytes, Text);

RETURN_PTR(NewEvent);
}


MIDIEvent Midi_EventCreateSoundEvt(byte EventCode, long DeltaTime, 
				   byte Channel, byte Param1, byte Param2)
{
MIDIEvent NewEvent;

BEGIN("Midi_EventCreateSoundEvt");

	NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct));

	NewEvent->EventCode = CreateMessageByte(EventCode, Channel);
	NewEvent->DeltaTime = DeltaTime;
	NewEvent->EventData.Note.Note = Param1;
	NewEvent->EventData.Note.Velocity = Param2;

RETURN_PTR(NewEvent);
}

MIDIEvent Midi_EventCreateNote(long DeltaTime, byte Channel, byte Pitch, 
			       byte Velocity, long Duration)
{
MIDIEvent NewEvent;

BEGIN("Midi_EventCreateNote");

	NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct));

	NewEvent->EventCode = CreateMessageByte(MIDI_NOTE_ON, Channel);
	NewEvent->DeltaTime = DeltaTime;
	NewEvent->EventData.Note.Note     = Pitch;
	NewEvent->EventData.Note.Velocity = Velocity;
	NewEvent->EventData.Note.Duration = Duration;

RETURN_PTR(NewEvent);
}


MIDIEvent Midi_EventCreateKeySigEvt(long DeltaTime, byte Sf, byte MaMi)
{
MIDIEvent NewEvent;

BEGIN("Midi_EventCreateKeySigEvt");

	NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct) + 2);

	NewEvent->DeltaTime = DeltaTime;
	NewEvent->EventCode = MIDI_FILE_META_EVENT;
	NewEvent->EventData.MetaEvent.MetaEventCode = MIDI_KEY_SIGNATURE;
	NewEvent->EventData.MetaEvent.NBytes        = 2;

	NewEvent->EventData.MetaEvent.Bytes         = Sf;
	*(&NewEvent->EventData.MetaEvent.Bytes + 1) = MaMi;

RETURN_PTR(NewEvent);
}

MIDIEvent Midi_EventCreateTempoEvt(long DeltaTime, long BPM)
{
MIDIEvent NewEvent;
long	  TempoValue;

BEGIN("Midi_EventCreateTempoEvt");

	NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct) + 3);

	TempoValue = 60000000 / BPM;

	NewEvent->DeltaTime = DeltaTime;
	NewEvent->EventCode = MIDI_FILE_META_EVENT;
	NewEvent->EventData.MetaEvent.MetaEventCode = MIDI_SET_TEMPO;
	NewEvent->EventData.MetaEvent.NBytes        = 3;
	NewEvent->EventData.MetaEvent.Bytes         = (TempoValue >> 16) & 0xff;
	*(&NewEvent->EventData.MetaEvent.Bytes + 1) = (TempoValue >> 8) & 0xff;
	*(&NewEvent->EventData.MetaEvent.Bytes + 2) = TempoValue & 0xff;
	
RETURN_PTR(NewEvent);
}



MIDIEvent Midi_EventCreateTimeSigEvt(long DeltaTime, byte Numerator, byte Denominator)
{
MIDIEvent NewEvent;
byte      DenPowOf2;

BEGIN("Midi_EventCreateTimeSigEvt");

	DenPowOf2 = 0;

	while (Denominator >>= 1)
	{
		++DenPowOf2;
	}

	NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct) + 4);

	NewEvent->DeltaTime = DeltaTime;
	NewEvent->EventCode = MIDI_FILE_META_EVENT;
	NewEvent->EventData.MetaEvent.MetaEventCode = MIDI_TIME_SIGNATURE;
	NewEvent->EventData.MetaEvent.NBytes        = 4;
	NewEvent->EventData.MetaEvent.Bytes	    = Numerator;
	*(&NewEvent->EventData.MetaEvent.Bytes + 1) = DenPowOf2;
	*(&NewEvent->EventData.MetaEvent.Bytes + 2) = 0;
	*(&NewEvent->EventData.MetaEvent.Bytes + 3) = 0;

RETURN_PTR(NewEvent);
}

int Midi_EventTimeLessp(void *Ev1, void *Ev2)
{
BEGIN("Midi_EventTimeLessp");

RETURN_INT((((MIDIEvent)Ev1)->DeltaTime < ((MIDIEvent)Ev2)->DeltaTime));
}

int Midi_EventTimeLessOrEqp(void *Ev1, void *Ev2)
{
BEGIN("Midi_EventTimeLessp");

RETURN_INT((((MIDIEvent)Ev1)->DeltaTime <= ((MIDIEvent)Ev2)->DeltaTime));
}

int Midi_EventListTimeLessp(void *Ev1, void *Ev2)
{
BEGIN("Midi_EventListTimeLessp");

RETURN_INT((((EventList)Ev1)->Event.DeltaTime < ((EventList)Ev2)->Event.DeltaTime));
}


