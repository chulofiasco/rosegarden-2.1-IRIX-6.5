/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	Csound.c

Description: 	Code to display and run dialogues from the Csound menu on
		the Tools Menu, and to do the translations

Author:	        John ffitch, jpff@maths.bath.ac.uk

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	30/09/94	JPff		File Created.
002	20/02/95	cc	        streamlined includes &c

-------------------------------------------------------------------------------------------- 
*/

#include <ctype.h>
#include <MidiXInclude.h>
#include <MidiErrorHandler.h>
#include <MidiFile.h>
#include <MidiEvent.h>

#include "Globals.h"
#include "Menu.h"
#include "Main.h"
#include "Csound.h"

#include <Debug.h>

void Midi_2CsoundCB(Widget w, XtPointer a, XtPointer b)
{
String 		FileName;

BEGIN("Midi_2CsoundCB");

	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	FileName = YFileGetWriteFilename
	  (topLevel, "Sequencer File - Export Csound", NULL, NULL);

	if (!FileName) END;

        Create_ScoreFile(FileName);

END;
}

extern char **TrackListEntries;
extern char *MajorKeys[];
extern char *MinorKeys[];
extern char *Notes[];

void Create_ScoreFile(String FileName)
{
FILE       *scofile;
EventList   Track;
MIDIEvent   Evt;
int         i, data;
double	    last_time = 0.0;
char	    RootNote, KeyType;
double	    tempobeat[100];
long	    temporate[100];
int	    tempp = 0;
BEGIN("Create_ScoreFile");

        scofile = fopen(FileName, "w");

	for (i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
	{
	    Track = MIDITracks[i];
	    while (Track)
	    {
		Evt = &Track->Event;
		if (Evt->EventCode == MIDI_FILE_META_EVENT)
		{
		    switch (Evt->EventData.MetaEvent.MetaEventCode)
		    {
		    case MIDI_SEQUENCE_NUMBER:
			fprintf(scofile, ";; Sequence Number:");
			break;
		    case MIDI_TEXT_EVENT:
			fprintf(scofile, ";; Text Event:");
			break;
		    case MIDI_END_OF_TRACK:
			fprintf(scofile, ";; End of Track.");
			break;
		    case MIDI_SET_TEMPO:
			{	/* Do not know why it is this complex!! */
			    long ttt =
				(((Evt->EventData.MetaEvent.Bytes << 8) +
				  *(&Evt->EventData.MetaEvent.Bytes + 1)) << 8)+
				*(&Evt->EventData.MetaEvent.Bytes + 2);
			    tempobeat[tempp]   = last_time;
			    temporate[tempp++] = (60000000L/ttt);
			    fprintf(scofile, ";; Tempo %ld ", (60000000L/ttt));
			}
			break;
		    case MIDI_SMPTE_OFFSET:
			fprintf(scofile, ";; Set SMPTE Offset:");
			break;
		    case MIDI_TIME_SIGNATURE:
			fprintf(scofile,";; Set Time Signature. %d/%d",
				Evt->EventData.MetaEvent.Bytes,
				1 << *(&Evt->EventData.MetaEvent.Bytes + 1));
			break;
		    case MIDI_KEY_SIGNATURE:
			RootNote = Evt->EventData.MetaEvent.Bytes + 7;
			KeyType  = *(&Evt->EventData.MetaEvent.Bytes + 1);
			fprintf(scofile, ";; Set Key Signature. %s",
				(KeyType == 1) ? MinorKeys[(int)RootNote] :
				                 MajorKeys[(int)RootNote]);
			break;
		    case MIDI_SEQUENCER_SPECIFIC:
			fprintf(scofile, ";; Sequencer Specific Event:");
			break;
		    case MIDI_COPYRIGHT_NOTICE:
			fprintf(scofile, ";; Copyright Notice:");
			break;
		    case MIDI_TRACK_NAME:
			fprintf(scofile, ";; Track Name:");
			break;
		    case MIDI_INSTRUMENT_NAME:
			fprintf(scofile, ";; Instrument Name:");
			break;
		    case MIDI_LYRIC:
			fprintf(scofile, ";; Lyric:");
			break;
		    case MIDI_TEXT_MARKER:
			fprintf(scofile, ";; Text Marker:");
			break;
		    case MIDI_CUE_POINT:
			fprintf(scofile, ";; Cue Point:");
			break;
		    default:
			fprintf(scofile, ";; Unsupported Event: %f %x %lx ",
				Midi_TimeToBeat(Evt->DeltaTime),
				Evt->EventData.MetaEvent.MetaEventCode,
				Evt->EventData.MetaEvent.NBytes);
			for (data = 0; data < Evt->EventData.MetaEvent.NBytes; ++data)
			    {
				fprintf(scofile, "%x ", 
					*((byte *)&Evt->EventData.MetaEvent.Bytes + data));
			    }
			break;
		    }
		    if (Evt->EventData.MetaEvent.MetaEventCode >= MIDI_TEXT_EVENT &&
			Evt->EventData.MetaEvent.MetaEventCode <= MIDI_CUE_POINT)
		    {
			fprintf(scofile, " %s\n",
				&Evt->EventData.MetaEvent.Bytes); /*,
				Evt->EventData.MetaEvent.NBytes); */
		    }
		    else 
			fprintf(scofile, " (%7.2f)\n", 
				Midi_TimeToBeat(Evt->DeltaTime));
		}
		else switch(MessageType(Evt->EventCode))
		{
		case MIDI_NOTE_ON:
		    fprintf(scofile, "i%d\t%7f\t%7f\t%d.%#.2d\t%d\t; %s\n",
			    i,	/* Instrument */
			    last_time = Midi_TimeToBeat(Evt->DeltaTime),
			    Midi_TimeToBeat(Evt->EventData.Note.Duration),
			    3 + (Evt->EventData.Note.Note / 12), /* MidC=8 */
			    Evt->EventData.Note.Note % 12,
			    Evt->EventData.Note.Velocity,
			    Notes[Evt->EventData.Note.Note % 12]);
		}
		Track = (EventList)Next(Track);
	    }
	}
        
        if (tempp != 0)
        {
	    fprintf(scofile, ";; Tempo changes\nt ");
	    for (i=0; i<tempp-1; i++)
		fprintf(scofile, "%f %ld %f %ld ",
			tempobeat[i], temporate[i],
			tempobeat[i+1], temporate[i]);
	    fprintf(scofile, "%f %ld\n",
			tempobeat[tempp-1], temporate[tempp-1]);
	}
        fprintf(scofile, "e\n");
        fclose(scofile);

END;
}
