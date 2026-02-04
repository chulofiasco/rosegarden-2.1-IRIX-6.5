/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Parse.c
 *
 *    Description:    Parser for MIDI file format.
 *                    Getting unsupported events.
 *                    Needs some work.
 *
 *    Author:         AJG
 *
 *    History:
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     01/02/94        AJG             File Created.
 *
 *
 */

#include <stdio.h>
#include <MidiConsts.h>
#include <MidiFile.h>
#include <MidiErrorHandler.h> 
#include <MidiVarLenNums.h>

#include "Types.h"
#include "Parse.h"
#include "Globals.h"

#include <Debug.h>


char *MajorKeys[] = 
{
	"Cb Maj", "Gb Maj", "Db Maj", "Ab Maj", "Eb Maj", "Bb Maj", "F Maj", "C Maj",
	"G Maj", "D Maj", "A Maj", "E Maj", "B Maj", "F# Maj", "C# Maj"
};

char *MinorKeys[] = 
{
	"Ab Min", "Eb Min", "Bb Min", "F Min", "C Min", "G Min", "D Min", "A Min",
	"E Min", "B Min", "F# Min", "C# Min", "G# Min", "D# Min", "A# Min"
};

char *Notes[] = 
{
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

#define MIDI_AVERAGE_EVENT_DESCRIPTION_LENGTH 40

char *FileFormats[3] = 
{
	"MIDI single track file.",
	"MIDI simultaneous multi-track file.",
	"MIDI sequential multi-track file."
};



/******************************************************************/
/* Midi_ParseTrack: Parse the events in a single track returning  */
/* a textual representation of each event in an array of strings. */
/******************************************************************/

char *Midi_ParseTrack(EventList Track)
{
int    	    i, NumberOfEvents, BufferSize, Count, StrLength;
char   	    Buffer[256];
char	    FormattingBuffer[64];
char	    MessageData[256];
char 	   *EventTextList = NULL;
char	    RootNote, KeyType;
MIDIEvent   Evt;
char	   *Text = 0;
Boolean	    UnsupportedEvent;
int	    data;

BEGIN("Midi_ParseTrack");

	if (Track == NULL) RETURN_PTR(NULL);

	Track = (EventList)First(Track);

	NumberOfEvents = Length(Track);
	
	Count = 0;
	BufferSize = MIDI_AVERAGE_EVENT_DESCRIPTION_LENGTH * NumberOfEvents;
	EventTextList = (char *)malloc(BufferSize);

	for(i = 0; i < NumberOfEvents; ++i)
	{
		Evt = &Track->Event;
		UnsupportedEvent = False;

		if (Evt->EventCode == MIDI_FILE_META_EVENT)
		{
			switch(Evt->EventData.MetaEvent.MetaEventCode)
			{
			case MIDI_SEQUENCE_NUMBER:

				Text = "Sequence Number:";
				break;

			case MIDI_TEXT_EVENT:

				Text = "Text Event:";
				break;

			case MIDI_END_OF_TRACK:

				Text = "End of Track.";
				break;

			case MIDI_SET_TEMPO:

				sprintf(FormattingBuffer, "Set Tempo. %ld bpm",
					Midi_EventConvertTempoToBPM(Evt));

				Text = FormattingBuffer;

				break;

			case MIDI_SMPTE_OFFSET:
	
				Text = "Set SMPTE Offset:";
				break;
	
			case MIDI_TIME_SIGNATURE:

				sprintf(FormattingBuffer,"Set Time Signature. %d/%d",
					Evt->EventData.MetaEvent.Bytes,
					1 << *(&Evt->EventData.MetaEvent.Bytes + 1));
				Text = FormattingBuffer;
				break;

			case MIDI_KEY_SIGNATURE:

				RootNote = Evt->EventData.MetaEvent.Bytes + 7;
				KeyType  = *(&Evt->EventData.MetaEvent.Bytes + 1);
				sprintf(FormattingBuffer, "Set Key Signature. %s",
					(KeyType == 1) ?
					MinorKeys[(int)RootNote] :
					MajorKeys[(int)RootNote]);
				Text = FormattingBuffer;
				break;

			case MIDI_SEQUENCER_SPECIFIC:

				Text = "Sequencer Specific Event:";
				break;

			case MIDI_COPYRIGHT_NOTICE:

				Text = "Copyright Notice:";
				break;

			case MIDI_TRACK_NAME:

				Text = "Track Name:";
				break;

			case MIDI_INSTRUMENT_NAME:

				Text = "Instrument Name:";
				break;

			case MIDI_LYRIC:

				Text = "Lyric:";
				break;

			case MIDI_TEXT_MARKER:

				Text = "Text Marker:";
				break;

			case MIDI_CUE_POINT:

				Text = "Cue Point:";
				break;

			default:

				sprintf(Buffer, "%7.2f: Unsupported Event: ff %x %lx ",
					Midi_TimeToBeat(Evt->DeltaTime),
					Evt->EventData.MetaEvent.MetaEventCode,
					Evt->EventData.MetaEvent.NBytes);
				MessageData[0] = '\0';
				for (data = 0; data < Evt->EventData.MetaEvent.NBytes; ++data)
				{
					sprintf(FormattingBuffer, "%x ", 
						*((byte *)&Evt->EventData.MetaEvent.Bytes + data));
					strcat(Buffer, FormattingBuffer);
				}
				strcat(Buffer, "\n");
				UnsupportedEvent = True;
				break;
			}
			
			if (Evt->EventData.MetaEvent.MetaEventCode >= MIDI_TEXT_EVENT &&
			    Evt->EventData.MetaEvent.MetaEventCode <= MIDI_CUE_POINT)
			{

				memcpy(MessageData, &Evt->EventData.MetaEvent.Bytes,
					Evt->EventData.MetaEvent.NBytes);
				MessageData[Evt->EventData.MetaEvent.NBytes] = '\0';
				
				sprintf(Buffer, "%7.2f: %s %s\n", Midi_TimeToBeat(Evt->DeltaTime), 
					Text, MessageData);
			}
			else if (!UnsupportedEvent) sprintf(Buffer, "%7.2f: %s\n", 
							    Midi_TimeToBeat(Evt->DeltaTime), Text);
		}
		else switch(MessageType(Evt->EventCode))
		{
		case MIDI_NOTE_OFF:

			sprintf(Buffer, "%7.2f: Note Off: %2d %-2s %d\t%3d\n", 
				Midi_TimeToBeat(Evt->DeltaTime),
				ChannelNum(Evt->EventCode),
				Notes[Evt->EventData.NoteOff.Note % 12],
				Evt->EventData.NoteOff.Note / 12,
				Evt->EventData.NoteOff.Velocity);
			break;

		case MIDI_NOTE_ON:

			sprintf(Buffer, "%7.2f: Note On: %2d %-2s %d\t%3d %5.2f\n",
				Midi_TimeToBeat(Evt->DeltaTime),
				ChannelNum(Evt->EventCode),
				Notes[Evt->EventData.NoteOff.Note % 12],
				Evt->EventData.NoteOff.Note / 12,
				Evt->EventData.Note.Velocity,
				Midi_TimeToBeat(Evt->EventData.Note.Duration));
			break;

		case MIDI_POLY_AFTERTOUCH:

			sprintf(Buffer, "%7.2f: Polyphonic Aftertouch: %2d %-2s %d\t%3d\n",
				Midi_TimeToBeat(Evt->DeltaTime),
				ChannelNum(Evt->EventCode),
				Notes[Evt->EventData.PolyAftertouch.Note % 12],
				Evt->EventData.PolyAftertouch.Note / 12, 
				Evt->EventData.PolyAftertouch.Velocity);
			break;

		case MIDI_CTRL_CHANGE:

			sprintf(Buffer, "%7.2f: Controller Change: %2d %d %d\n",
				Midi_TimeToBeat(Evt->DeltaTime),
				ChannelNum(Evt->EventCode),
				Evt->EventData.ControlChange.Controller, 
				Evt->EventData.ControlChange.Value);
			break;

		case MIDI_PROG_CHANGE:

			sprintf(Buffer, "%7.2f: Program Change: %2d %d\n",
				Midi_TimeToBeat(Evt->DeltaTime), 
				ChannelNum(Evt->EventCode),
				Evt->EventData.ProgramChange.Program);
			break;

		case MIDI_CHNL_AFTERTOUCH:

			sprintf(Buffer, "%7.2f: Monophonic Aftertouch: %2d %d\n",
				Midi_TimeToBeat(Evt->DeltaTime),
				ChannelNum(Evt->EventCode),
				Evt->EventData.MonoAftertouch.Channel);
			break;

		case MIDI_PITCH_BEND:

			sprintf(Buffer, "%7.2f: Pitch Bend: %2d %d\n", 
				Midi_TimeToBeat(Evt->DeltaTime),
				ChannelNum(Evt->EventCode),
				Evt->EventData.PitchWheel.LSB |
				Evt->EventData.PitchWheel.MSB << 7);
			break;

		default:

			Error(NON_FATAL_REPORT_TO_MSGBOX, "Unsupported event.\n");
			break;
		}

		StrLength = strlen(Buffer);

		if (Count + StrLength >= BufferSize)
		{
			BufferSize += StrLength + 1;

			if (EventTextList) { /* cc 95, Sun's fault */
			    EventTextList = (char *)realloc(EventTextList, BufferSize);
			} else {
			    EventTextList = (char *)malloc(BufferSize);
			}
		}

		strcpy(EventTextList + Count, Buffer);

		Count += StrLength;

		Track = (EventList)Next(Track);
	}

RETURN_PTR(EventTextList);
}
