/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Event.h

Description:	Definitions of MIDI events. MIDI events are held in a generalised event
		structure. Fixed length events (such as Note On, Note Off, etc.) are held
		within the structure directly. Variable length events (SysEx, File Meta-
		events) are held with their static fields store directly and the variable
		length data appended to the end of the structure. This data can then be
		accessed by using the final 'Bytes' field of the structure to gain the
		address.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	03/02/94	AJG		File Created.
002	14/02/95	CC		Changed "Length" to "NBytes" as structure elt,
					because Length is a List macro name
003     14/02/95	AJG		Added Midi_EventTimeLessOrEqp predicate to
					fix one-to-two-point conversion problem.
--------------------------------------------------------------------------------------------
*/

#ifndef _MIDI_EVENT_H_
#define _MIDI_EVENT_H_

#include <X11/Intrinsic.h>
#include <MidiConsts.h>
#include <Lists.h>

/*
------------------------------------------------
	MIDI event data field structures.
------------------------------------------------
*/


/****************************************************************/
/* Note data fields for two-point representation (i.e. separate */
/* Note On/Note Off messages). This structure is used for       */
/* representing notes externally (i.e. in a MIDI stream).	*/
/* The structure is also used for polyphonic aftertouch.	*/
/****************************************************************/

typedef struct
{
	byte Note;
	byte Velocity;
}
MIDINoteEventStruct;



/****************************************************************/
/* Note data fields for one-point representation. This is used  */
/* within the sequencer for ease of manipulation. Notes must be */
/* converted back to two-point representation before being      */
/* written out to a MIDI stream.				*/
/****************************************************************/

typedef struct
{
	byte Note;
	byte Velocity;
	long Duration;
}
MIDIOnePointNoteEventStruct;


/*******************************************/
/* Controller change event data structure. */
/*******************************************/

typedef struct
{
	byte Controller;
	byte Value;
}
MIDIControlEventStruct;


/****************************************/
/* Program change event data structure. */
/****************************************/

typedef struct
{
	byte Program;
}
MIDIProgramEventStruct;



/***********************************************/
/* Monophonic aftertouch event data structure. */
/***********************************************/

typedef struct
{
	byte Channel;
}
MIDIAftertouchEventStruct;



/*******************************************/
/* Pitchwheel change event data structure. */
/*******************************************/

typedef struct
{
	byte LSB;
	byte MSB;
}
MIDIPitchWheelEventStruct;



/***********************************************************/
/* File Meta-event data-structure. Variable data is placed */
/* after the defined structure, starting at the address    */
/* containing the field 'Bytes'.			   */
/***********************************************************/

typedef struct
{
	byte	MetaEventCode;
	long	NBytes;
	byte	Bytes;
}
MIDIFileMetaEventStruct;



/***************************************************************/
/* Event union - describes the different possible MIDI events. */
/***************************************************************/

typedef union
{
	MIDINoteEventStruct 		NoteOn;
	MIDINoteEventStruct 		NoteOff;
	MIDIOnePointNoteEventStruct	Note;
	MIDINoteEventStruct 		PolyAftertouch;
	MIDIControlEventStruct		ControlChange;
	MIDIProgramEventStruct		ProgramChange;
	MIDIAftertouchEventStruct	MonoAftertouch;
	MIDIPitchWheelEventStruct	PitchWheel;
	MIDIFileMetaEventStruct		MetaEvent;
}
MIDIEventUnion;




/*******************************************************************/
/* MIDIEvent structure. Events have a delta-time to describe where */
/* in the music they occur, an event code (the status byte of the  */
/* event on the MIDI stream) and the data fields for that event    */
/* which are accessed through the event union.			   */
/*******************************************************************/

typedef struct
{
	long 		DeltaTime;
	byte 		EventCode;
	MIDIEventUnion	EventData;	
}
MIDIEventStruct, *MIDIEvent;



/****************************************************************************/
/* Event List - MIDI Events that form a sequence are held in an event list. */
/****************************************************************************/

typedef struct
{
	ListElement	Base;
	MIDIEventStruct	Event;
}
EventListElement, *EventList;

EventList Midi_EventCreateList(MIDIEvent /*NewEvent*/, Boolean /* RetainEventStruct*/);
long	  Midi_EventConvertTempoToBPM(MIDIEvent /*TempoEvent*/);

MIDIEvent Midi_EventCreateTextEvt(byte /*EventCode*/, long /*DeltaTime*/, char * /*Text*/);

MIDIEvent Midi_EventCreateSoundEvt(byte /*EventCode*/, long /*DeltaTime*/, 
				   byte /*Channel*/, byte /*Param1*/, byte /*Param2*/);

MIDIEvent Midi_EventCreateNote(long /*DeltaTime*/, byte /*Channel*/, byte /*Pitch*/, 
			       byte /*Velocity*/, long /*Duration*/);

MIDIEvent Midi_EventCreateTempoEvt(long /*DeltaTime*/, long /*BPM*/);

MIDIEvent Midi_EventCreateTimeSigEvt(long /*DeltaTime*/, byte /*Numerator*/, byte /*Denominator*/);

MIDIEvent Midi_EventCreateKeySigEvt(long /*DeltaTime*/, byte /*Sf*/, byte /*MaMi*/);

int	  Midi_EventTimeLessp(void * /*Ev1*/, void * /*Ev2*/);
int	  Midi_EventTimeLessOrEqp(void * /*Ev1*/, void * /*Ev2*/);
int	  Midi_EventListTimeLessp(void * /*Ev1*/, void * /*Ev2*/);


#endif

