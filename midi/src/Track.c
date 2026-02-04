/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Track.c

Description:	Track manipulation functions.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/02/94	AJG		File Created.

002	25/01/95	AJG		Corrected TrackTime to long from int for
					compatibility with Borland C++

003	14/02/95	AJG		Fixed problem in the conversion function
					Midi_ConvertToTwoPointRepresentation. Note Off
					events are now inserted into the expanded track
					when they have times <= the time of the next
					event (instead of just <). This corrects a bug
					whereby when a note ended at the same time as
					the next note began, the Note Off event was
					added after the next note, effectively blanking
					it if it was on the same channel.
--------------------------------------------------------------------------------------------
*/

#include <MidiConsts.h>
#include <MidiErrorHandler.h>
#include <MidiFile.h>
#include <MidiTrack.h>
#include <MidiBHeap.h>

#include <Debug.h>

#define NOTE_OFF_HEAP_MAX_SIZE	96

/**************************************************************************/
/* MIDI event mask association tables for use by Midi_TrackFilterByEvent. */
/**************************************************************************/

Midi_EventMaskAssoc EventMaskTable[] =
{
	{MIDI_NOTE_OFF,			MidiNoteOffEventMask},
	{MIDI_NOTE_ON,			MidiNoteOnEventMask},
	{MIDI_POLY_AFTERTOUCH,		MidiPolyAftertouchEventMask},
	{MIDI_CTRL_CHANGE,		MidiCtrlChangeEventMask},
	{MIDI_PROG_CHANGE,		MidiProgChangeEventMask},
	{MIDI_CHNL_AFTERTOUCH,		MidiChnlAftertouchEventMask},
	{MIDI_PITCH_BEND,		MidiPitchBendEventMask},
};

Midi_EventMaskAssoc  SystemMaskTable[] = 
{
	{MIDI_SYSTEM_EXCLUSIVE,		MidiSystemExEventMask},
	{MIDI_SONG_POSITION_PTR,	MidiSongPosPtrEventMask},
	{MIDI_TUNE_REQUEST,		MidiTuneRequestEventMask},
};

Midi_EventMaskAssoc MetaMaskTable[] =
{
	{MIDI_TEXT_EVENT,		MidiTextEventMask},
	{MIDI_SET_TEMPO,		MidiSetTempoEventMask},
	{MIDI_SMPTE_OFFSET,		MidiSmpteOffsetEventMask},
	{MIDI_TIME_SIGNATURE,		MidiTimeSignatureEventMask},
	{MIDI_KEY_SIGNATURE,		MidiKeySignatureEventMask},
	{MIDI_SEQUENCER_SPECIFIC,	MidiSequencerSpecificEventMask},
	{MIDI_COPYRIGHT_NOTICE,		MidiCopyrightNoticeEventMask},
	{MIDI_TRACK_NAME,		MidiTrackNameEventMask},
	{MIDI_INSTRUMENT_NAME,		MidiInstrumentNameEventMask},
	{MIDI_LYRIC,			MidiLyricEventMask},
	{MIDI_TEXT_MARKER,		MidiTextMarkerEventMask},
	{MIDI_CUE_POINT,		MidiCuePointEventMask},
	{MIDI_SEQUENCE_NUMBER,		MidiSequenceNumberEventMask}
};





/**********************************************************************************************/
/* Midi_TrackConvertToOnePointRepresentation: This function converts a track from a MIDI file */
/* from the two-point Note On/Note Off representation to the more malleable one-point Note On */
/* with duration form. 									      */
/**********************************************************************************************/

void Midi_TrackConvertToOnePointRepresentation(EventList Track)
{
EventList Pilgrim;
byte	  Cmd;
bool	  Transformed;

BEGIN("Midi_TrackConvertToOnePointRepresentation");

	Track = (EventList)First(Track);

	while(Track)
	{
		Cmd = Track->Event.EventCode;

		if (MessageType(Cmd) == MIDI_NOTE_ON)
		{
			Pilgrim     = (EventList)Next(Track);
			Transformed = FALSE;

			while(Pilgrim)
			{
				if (ChannelNum(Cmd) == ChannelNum(Pilgrim->Event.EventCode) &&
				    (MessageType(Pilgrim->Event.EventCode) == MIDI_NOTE_OFF ||
				     (MessageType(Pilgrim->Event.EventCode) == MIDI_NOTE_ON &&
				      Pilgrim->Event.EventData.NoteOn.Velocity == 0 )) 
 					&& Track->Event.EventData.Note.Note == Pilgrim->Event.EventData.Note.Note)
				{
					Track->Event.EventData.Note.Duration = Pilgrim->Event.DeltaTime -
					  				       Track->Event.DeltaTime;
					Remove(Pilgrim);
					Transformed = TRUE;
					break;
				}

				Pilgrim = (EventList)Next(Pilgrim);
			}

			if (!Transformed)
			{
				/*Error(NON_FATAL_REPORT_TO_STDERR, "Missing Note Off event.");*/
				Track->Event.EventData.Note.Duration = ((EventList)Last(Track))->Event.DeltaTime -
									Track->Event.DeltaTime;
			}
		}

		Track = (EventList)Next(Track);
	}

END;
}






/***************************************************************************************/
/* Midi_TrackConvertToTwoPointRepresentation: Convert a one-point representation track */
/* back to the two point representation. 					       */
/***************************************************************************************/

EventList Midi_TrackConvertToTwoPointRepresentation(EventList Track)
{
EventList  NewTrack, RunningPtr, NoteOffEvent, CloneEvent;
BinaryHeap NoteOffHeap;
MIDIEvent  NewEvent;

BEGIN("Midi_TrackConvertToTwoPointRepresentation");

	NewTrack   = NULL;
	RunningPtr = NULL;
	NoteOffHeap = CreateBHeap(NOTE_OFF_HEAP_MAX_SIZE, Midi_EventTimeLessp);

	while(Track)
	{
		while (HeapSize(NoteOffHeap) && 
			(Midi_EventTimeLessOrEqp(Value(NoteOffHeap, Root), &Track->Event)))
		{
			NoteOffEvent = Midi_EventCreateList((MIDIEvent)ExtractMin(NoteOffHeap), False);

			if (NewTrack)
			{
				RunningPtr = (EventList)Nconc(RunningPtr, NoteOffEvent);
			}
			else
			{
				NewTrack = NoteOffEvent;
				RunningPtr = NewTrack;
			}
		}

		if (MessageType(Track->Event.EventCode) == MIDI_NOTE_ON)
		{
			NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct));

			NewEvent->EventCode = CreateMessageByte(MIDI_NOTE_OFF, 
								ChannelNum(Track->Event.EventCode));

			NewEvent->DeltaTime = Track->Event.DeltaTime + 
					      Track->Event.EventData.Note.Duration;

			NewEvent->EventData.NoteOff.Note     = Track->Event.EventData.Note.Note;
			NewEvent->EventData.NoteOff.Velocity = 127;

			BHeapInsert(NoteOffHeap, NewEvent);
		}

		CloneEvent = Midi_EventCreateList(&Track->Event, True);

		if (NewTrack)
		{
			RunningPtr = (EventList)Nconc(RunningPtr, CloneEvent);
		}
		else
		{
			NewTrack = CloneEvent;
			RunningPtr = NewTrack;
		}

		Track = (EventList)Next(Track);
	}

	while (HeapSize(NoteOffHeap))
	{
		NoteOffEvent = Midi_EventCreateList((MIDIEvent)ExtractMin(NoteOffHeap), False);
		RunningPtr = (EventList)Nconc(RunningPtr, NoteOffEvent);
	}
	
	DestroyBHeap(NoteOffHeap);

RETURN_PTR(NewTrack);
}
	

/*************************************************************************/
/* Midi_TrackAggregateDeltas: Convert the MIDI events in a track to have */
/* delta times corresponding to offsets from the start of the track,     */
/* rather than from the previous event. This makes them easier to work   */
/* with, and improves their flavour and temperament, not to mention      */
/* the wonders it works for the programmer's karma and social standing.	 */
/*************************************************************************/

void Midi_TrackAggregateDeltas(EventList Track)
{
long TrackTime = 0;

BEGIN("Midi_TrackAggregateDeltas");

	while(Track)
	{
		TrackTime += Track->Event.DeltaTime;
		Track->Event.DeltaTime = TrackTime;
		Track = (EventList)Next(Track);
	}
END;
}

/********************************************************************/
/* Midi_TrackDelete: Delete a track freeing all space allocated for */
/* events in that track.					    */
/********************************************************************/

void Midi_TrackDelete(EventList Track)
{
BEGIN("Midi_TrackDelete");

	Track = (EventList)First(Track);

	while(Track)
	{
		Track = (EventList)Remove(Track);
	}

END;
}


EventList Midi_TrackClone(EventList Track)
{
EventList Clone = NULL,
	  CloneStart = NULL,
	  ClonedEvent;

BEGIN("Midi_TrackClone");

	while (Track)
	{
		ClonedEvent = (EventList)Midi_EventCreateList( &Track->Event, True);	

		if (Clone)
		{
			Clone = (EventList)Nconc(Clone, ClonedEvent);
		}
		else
		{
			Clone = ClonedEvent;
			CloneStart = Clone;
		}

		Track = (EventList)Next(Track);
	}

RETURN_PTR(CloneStart);
}

/*************************************************************************/
/* Midi_TrackTranspose:	Transpose all the note events in a track by      */
/* in integer delta (measured in semitones). Only Note On, Note Off      */
/* and Polyphonic Aftertouch events are affected by this transformation. */
/*************************************************************************/

void Midi_TrackTranspose(EventList Track, int Delta)
{
byte NewNote, Cmd;

BEGIN("Midi_TrackTranspose");

	while(Track)
	{
		Cmd = MessageType(Track->Event.EventCode);

		if (Cmd == MIDI_NOTE_ON ||
		    Cmd == MIDI_NOTE_OFF ||
		    Cmd == MIDI_POLY_AFTERTOUCH)
		{
			NewNote = Track->Event.EventData.NoteOn.Note + Delta;

			if (NewNote > 127)
			{
			  Error(NON_FATAL_REPORT_TO_MSGBOX,
				"Transposed note outside valid MIDI note range.\nLeaving unchanged.");
			}
			else Track->Event.EventData.NoteOn.Note = NewNote;
		}

		Track = (EventList)Next(Track);
	}
END;
}


/*************************************************************************/
/* Midi_TrackFilterByChannel:	This function returns a new track that   */
/* contains only events related to the specified channel. The original   */
/* track is left unchanged - it is up to the caller to delete it if      */
/* necessary. File Meta events are not duplicated in the filtered track. */
/*************************************************************************/

EventList Midi_TrackFilterByChannel(EventList Track, byte Channel)
{
EventList FilteredTrack = NULL, 
	  NewTrackElt, 
	  FilteredTrackStart = NULL;
MIDIEvent Evt;
byte	  Cmd;

BEGIN("Midi_TrackFilterByChannel");

	while(Track)
	{
		Evt = &Track->Event;
		Cmd = Evt->EventCode;

		if (MessageType(Cmd) != MIDI_FILE_META_EVENT)
		{
			if (ChannelNum(Cmd) == Channel)
			{
				NewTrackElt = Midi_EventCreateList(Evt, True);
				if (FilteredTrack)
				{
					FilteredTrack = (EventList)Nconc(FilteredTrack, NewTrackElt);
				}
				else
				{
					FilteredTrack      = NewTrackElt;
					FilteredTrackStart = FilteredTrack;
				}
			}
		}
		Track = (EventList)Next(Track);
	}
RETURN_PTR(FilteredTrackStart);
}


/* filters up to a channel - rwb/97 */

EventList Midi_TrackFilterByChannels(EventList Track, byte Channel,
                                   Boolean RetainUpwards)
{
EventList FilteredTrack = NULL, 
	  NewTrackElt, 
	  FilteredTrackStart = NULL;
MIDIEvent Evt;
byte	  Cmd;

BEGIN("Midi_TrackFilterUpToChannel");

	while(Track)
	{
		Evt = &Track->Event;
		Cmd = Evt->EventCode;

		if (MessageType(Cmd) != MIDI_FILE_META_EVENT)
		{
			if ((ChannelNum(Cmd) <= Channel &&
                                    (RetainUpwards == False)) ||
                            ((ChannelNum(Cmd) > Channel) &&
                                      (RetainUpwards == True)))
			{
				NewTrackElt = Midi_EventCreateList(Evt, True);
				if (FilteredTrack)
				{
					FilteredTrack = (EventList)Nconc(FilteredTrack, NewTrackElt);
				}
				else
				{
					FilteredTrack      = NewTrackElt;
					FilteredTrackStart = FilteredTrack;
				}
			}
		}
		Track = (EventList)Next(Track);
	}
RETURN_PTR(FilteredTrackStart);
}



/****************************************************************************/
/* Midi_TrackFilterByEvent: This functions takes a track and an event mask  */
/* and returns a track that contains only those MIDI events that conform to */
/* the event mask. This operation is not destructive - the input track is   */
/* unaffected and must be deleted by the calling application as necessary.  */
/****************************************************************************/

EventList Midi_TrackFilterByEvent(EventList Track, Midi_EventMask mask)
{
EventList FilteredTrack = NULL, 
	  FilteredTrackStart = NULL,
	  NewTrackElt;

MIDIEvent Evt;
byte	  Cmd;
int	  i;

BEGIN("Midi_TrackFilterByEvent");

	while(Track)
	{
		Evt = &(Track->Event);
		Cmd = Track->Event.EventCode;
		NewTrackElt = NULL;

		/**********************************/
		/* First filter file meta-events. */
		/**********************************/

		if (Cmd == MIDI_FILE_META_EVENT)
		{
			Cmd = Track->Event.EventData.MetaEvent.MetaEventCode;
			for (i = 0; i < NumberElts(MetaMaskTable); ++i)
			{
				if (Cmd == MetaMaskTable[i].EventType && 
				    (mask & MetaMaskTable[i].EventMask))
				{
					NewTrackElt = (EventList)Midi_EventCreateList(Evt, True);
					break;
				}
			}
		}

		/********************************/
		/* Next filter system messages. */
		/********************************/

		else if (MessageType(Cmd) == MIDI_SYSTEM_MSG)
		{
			for (i = 0 ; i < NumberElts(SystemMaskTable); ++i)
			{
				if (Cmd == SystemMaskTable[i].EventType &&
				    (mask & SystemMaskTable[i].EventMask))
				{
					NewTrackElt = (EventList)Midi_EventCreateList(Evt, True);
					break;
				}
			}
		}

		/********************************/
		/* Finally filter sound events. */
		/********************************/

		else
		{
			for(i = 0 ; i < NumberElts(EventMaskTable); ++i)
			{
				if (MessageType(Cmd) == EventMaskTable[i].EventType &&
				    (mask & EventMaskTable[i].EventMask))
				{
					NewTrackElt = (EventList)Midi_EventCreateList(Evt, True);
					break;
				}
			}
		}

		if (NewTrackElt)
		{
			if (FilteredTrack)
			{
				FilteredTrack = (EventList)Nconc(FilteredTrack, NewTrackElt);
			}
			else
			{
				FilteredTrack = NewTrackElt;
				FilteredTrackStart = FilteredTrack;
			}
		}

		Track = (EventList)Next(Track);
	}

RETURN_PTR(FilteredTrackStart);
}



EventList Midi_TrackQuantize(EventList Track,     MIDIHeaderChunk *Header, 
			     Boolean QuantizePos, int QuantizePosRes,
			     Boolean QuantizeDur, int QuantizeDurRes)
{
EventList QuantizedTrack, RunningPtr, QuantizedEvent;
long	  NotePosResolution = 0;
long	  NoteDurResolution = 0;

BEGIN("Midi_TrackQuantize");

	QuantizedTrack = NULL;
	RunningPtr     = NULL;

	if (QuantizePos)
	{
		NotePosResolution = (Header->Timing.Division * 4000) >> QuantizePosRes;
	}

	if (QuantizeDur)
	{
		NoteDurResolution = (Header->Timing.Division * 4000) >> QuantizeDurRes;
	}

	while(Track)
	{
		QuantizedEvent = Midi_EventCreateList(&Track->Event, True);

		if (QuantizePos)
		{
			QuantizedEvent->Event.DeltaTime *= 1000;
			QuantizedEvent->Event.DeltaTime += NotePosResolution / 2;
			QuantizedEvent->Event.DeltaTime /= NotePosResolution;
			QuantizedEvent->Event.DeltaTime *= NotePosResolution;
			QuantizedEvent->Event.DeltaTime /= 1000;
		}

		if (QuantizeDur && MessageType(QuantizedEvent->Event.EventCode) == MIDI_NOTE_ON)
		{
			if ((QuantizedEvent->Event.EventData.Note.Duration * 1000) < (NoteDurResolution / 2))
			{
				QuantizedEvent->Event.EventData.Note.Duration = NoteDurResolution / 1000;
			}
			else
			{
				QuantizedEvent->Event.EventData.Note.Duration *= 1000;
				QuantizedEvent->Event.EventData.Note.Duration += NoteDurResolution / 2;
				QuantizedEvent->Event.EventData.Note.Duration /= NoteDurResolution;
				QuantizedEvent->Event.EventData.Note.Duration *= NoteDurResolution;
				QuantizedEvent->Event.EventData.Note.Duration /= 1000;
			}
		}

		if (QuantizedTrack)
		{
			RunningPtr = (EventList)Nconc(RunningPtr, QuantizedEvent);
		}
		else
		{
			QuantizedTrack = QuantizedEvent;
			RunningPtr     = QuantizedEvent;
		}

		Track = (EventList)Next(Track);

	}
			
RETURN_PTR(QuantizedTrack);
}


EventList Midi_TrackChangeChannel(EventList Track, byte ChangeFrom, byte ChangeTo)
{
EventList ModifiedTrack, RunningPtr, ModifiedEvent;

BEGIN("Midi_TrackChangeChannel");

	ModifiedTrack = NULL;
	RunningPtr    = NULL;

	while(Track)
	{
		ModifiedEvent = Midi_EventCreateList(&Track->Event, True);

		if (MessageType(ModifiedEvent->Event.EventCode) != MIDI_SYSTEM_MSG)
		{
			if (ChannelNum(ModifiedEvent->Event.EventCode) == ChangeFrom)
			{

				ModifiedEvent->Event.EventCode = 
					CreateMessageByte(MessageType(ModifiedEvent->Event.EventCode),
							 ChangeTo);
			}
		}

		if (ModifiedTrack)
		{
			RunningPtr = (EventList)Nconc(RunningPtr, ModifiedEvent);
		}
		else
		{
			ModifiedTrack = ModifiedEvent;
			RunningPtr    = ModifiedEvent;
		}

		Track = (EventList)Next(Track);
	}

RETURN_PTR(ModifiedTrack);
}


EventList Midi_TrackFilterByPitch(EventList Track, byte Pitch, Boolean RetainAbove)
{
EventList FilteredTrack, CloneEvent, RunningPtr;

BEGIN("Midi_TrackFilterByPitch");

	FilteredTrack = NULL;
	RunningPtr    = NULL;

	while(Track)
	{
		if (MessageType(Track->Event.EventCode) != MIDI_NOTE_ON ||
		    (Track->Event.EventData.Note.Note >= Pitch && RetainAbove) ||
		    (Track->Event.EventData.Note.Note <= Pitch && !RetainAbove))
		{
			CloneEvent = Midi_EventCreateList(&Track->Event, True);

			if (FilteredTrack)
			{
				RunningPtr = (EventList)Nconc(RunningPtr, CloneEvent);
			}
			else
			{
				FilteredTrack = CloneEvent;
				RunningPtr    = CloneEvent;
			}
		}

		Track = (EventList)Next(Track);
	}

RETURN_PTR(FilteredTrack);
}



EventList Midi_TrackMerge(EventList Track1, EventList Track2)
{
EventList MergedTrack, RunningPtr, NextEvent;

BEGIN("Midi_TrackMerge");

	MergedTrack = NULL;
	RunningPtr  = NULL;

	if (Track1 == Track2) RETURN_PTR(Track1);

	while(Track1 || Track2)
	{
		if (Track1 && !Track2)
		{
			if (!MergedTrack)
			{
				MergedTrack = Track1;
			}
			else while(Track1)
			{
				NextEvent = Midi_EventCreateList(&Track1->Event, True);
				Nconc(RunningPtr, Midi_EventCreateList(&Track1->Event, True));
				Track1 = (EventList)Next(Track1);
			}
			break;
		}
		else if (Track2 && !Track1)
		{
			if (!MergedTrack)
			{
				MergedTrack = Track2;
			}
			else while(Track1)
			{
				NextEvent = Midi_EventCreateList(&Track2->Event, True);
				Nconc(RunningPtr, Midi_EventCreateList(&Track2->Event, True));
				Track2 = (EventList)Next(Track2);
			}
			break;
		}

		if (Track2->Event.DeltaTime <= Track1->Event.DeltaTime)
		{
			NextEvent = Midi_EventCreateList(&Track2->Event, True);
			Track2    = (EventList)Next(Track2);
		}
		else
		{
			NextEvent = Midi_EventCreateList(&Track1->Event, True);
			Track1    = (EventList)Next(Track1);
		}

		if (NextEvent->Event.EventCode != MIDI_FILE_META_EVENT ||
		    NextEvent->Event.EventData.MetaEvent.MetaEventCode != MIDI_END_OF_TRACK)
		{
			if (RunningPtr)
			{
				RunningPtr->Base.next = (List)NextEvent;
				NextEvent->Base.prev  = (List)RunningPtr;
				RunningPtr            = NextEvent;
			}
			else
			{
				RunningPtr  = NextEvent;
				MergedTrack = NextEvent;
			}
		}
		else free(NextEvent);
	}

RETURN_PTR(MergedTrack);
}

