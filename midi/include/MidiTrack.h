/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Track.h

Description:	Track manipulation functions.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#ifndef _MIDI_TRACK_H_
#define _MIDI_TRACK_H_

typedef struct
{
	byte		EventType;
	Midi_EventMask	EventMask;
}
Midi_EventMaskAssoc;

#define	MIDI_SEMIBREVE  	 0
#define MIDI_MINIM		 1
#define MIDI_CROTCHET		 2
#define MIDI_QUAVER		 3
#define MIDI_SEMIQUAVER 	 4
#define MIDI_DEMISEMIQUAVER 	 5
#define MIDI_HEMIDEMISEMIQUAVER	 6

void 	  Midi_TrackConvertToOnePointRepresentation(EventList Track);
EventList Midi_TrackConvertToTwoPointRepresentation(EventList Track);
void 	  Midi_TrackAggregateDeltas(EventList Track);
void 	  Midi_TrackDelete(EventList Track);
EventList Midi_TrackClone(EventList Track);

void 	  Midi_TrackTranspose(EventList Track, int Delta);
EventList Midi_TrackFilterByChannel(EventList Track, byte Channel);
EventList Midi_TrackFilterByChannels(EventList Track, byte Channel, Boolean RetainAbove);
EventList Midi_TrackFilterByEvent(EventList Track, Midi_EventMask);
EventList Midi_TrackFilterByPitch(EventList Track, byte pitch, Boolean RetainAbove);

EventList Midi_TrackQuantize(EventList Track,     MIDIHeaderChunk *Header, 
			     Boolean QuantizePos, int QuantizePosRes,
			     Boolean QuantizeDur, int QuantizeDurRes);

EventList Midi_TrackChangeChannel(EventList Track, byte ChangeFrom, byte ChangeTo);

EventList Midi_TrackMerge(EventList Track1, EventList Track2);

#endif
