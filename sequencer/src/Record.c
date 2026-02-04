/*
 * Record.c
 */

#undef POSIX_PLEASE
#undef _POSIX_SOURCE

#include "Globals.h"
#include "Consts.h"
#include "Main.h"
#include "Menu.h"
#include "Mapper.h"
#include "Record.h"
#include "Sequence.h"


#include <MidiXInclude.h>
#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <Debug.h>
#include <MidiBHeap.h>
#include <MidiTrack.h>

#include "TrackList.h"

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/signal.h>

long RecordTempo = 500000;
int MidiEventBufferSize = 10000;

extern Boolean Playing;
extern TrackMetaInfoElement   Tracks;
extern unsigned int LastTime;

MIDIRawEventBuffer IncomingEventBuf;
MIDIRawEventBuffer NextEventPtr;
Boolean            Recording;
int                MidiEventBufferTimer = 0;

void Midi_SeqStopRecordingCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SeqStopRecordingCB");

    Midi_SetFileModified(True);
    Recording = False;

END;
}


/* adds default timing information if either no tracks
   are already available or any of them don't already
   have any timing information */

EventList
Midi_SeqRecordTimingDefaults(EventList *RunningPtr)
{
    int AddTimingInfo = True;
    int i;
    char TmpString[40];

    BEGIN("Midi_SeqRecordTimingDefaults");

    for (i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
    {
        if(Midi_TrackFilterByEvent(MIDITracks[i], MidiSetTempoEventMask))
            AddTimingInfo = False;
    }

    if (AddTimingInfo || (!MIDIHeaderBuffer.NumTracks))
    {
        Midi_TrackListSetupDefaults(); /* MIDI_NO_FILE_LOADED won't 
                                          catch record + deleted tracks */

        (*RunningPtr) = (EventList)Insert(Midi_EventCreateList
           (Midi_EventCreateTempoEvt(0, 120), False), *RunningPtr);

    }

    (*RunningPtr) = (EventList)Insert(Midi_EventCreateList
      (Midi_EventCreateTextEvt (MIDI_TEXT_MARKER, 0,
       "Created by the Rosegarden sequencer"), False), *RunningPtr);

    sprintf(TmpString, "Recorded Track %d", i%16);

    (*RunningPtr) = (EventList)Insert(Midi_EventCreateList
       (Midi_EventCreateTextEvt(MIDI_TRACK_NAME, 0,
             TmpString), False), *RunningPtr);

    RETURN_PTR(*RunningPtr);
}

void
Midi_SeqRecordEvent()
{
BEGIN("Midi_SeqRecordEvent");

    if (Mapper_ReadEvent(NextEventPtr) == True )
    {
#ifdef RECORD_DEBUG
        fprintf(stdout, " ++ Received event in\n");
        fprintf(stderr, " ++ MIDI EVENT : %ld %x %x %x\n",
                            NextEventPtr->Time,
                            (int)NextEventPtr->Bytes[0],
                            (int)NextEventPtr->Bytes[1],
                            (int)NextEventPtr->Bytes[2]);
#endif

        NextEventPtr++;
    }

END;
}


EventList
Midi_SeqConvertRawDataToEventList(unsigned int StartTime)
{
    EventList          NewTrack, RunningPtr, TempPtr;
    MIDIEventStruct    EventBuffer;
    MIDIRawEventBuffer EventPtr;

    BEGIN("Midi_SeqConvertRawDataToEventList");

    NewTrack   = NULL;
    RunningPtr = NULL;
    TempPtr = NULL;

    EventPtr = IncomingEventBuf;

    if (MIDIHeaderBuffer.Format == MIDI_NO_FILE_LOADED) 
    {
        Midi_TrackListSetupDefaults();
    }

    while (EventPtr != NextEventPtr)
    {
        EventBuffer.DeltaTime = (((float)(EventPtr->Time * 10000L) /
                  RecordTempo)) * MIDIHeaderBuffer.Timing.Division + StartTime;

        EventBuffer.EventCode = EventPtr->Bytes[0];

        EventBuffer.EventData.NoteOn.Note     = EventPtr->Bytes[1];
        EventBuffer.EventData.NoteOn.Velocity = EventPtr->Bytes[2];

        if (!NewTrack)
        {
            NewTrack = Midi_EventCreateList(&EventBuffer, True);
            RunningPtr = NewTrack;
        }
        else
        {
            RunningPtr = (EventList)Nconc(RunningPtr, Midi_EventCreateList
                                                        (&EventBuffer, True));
        }
        ++EventPtr;
    }

    EventBuffer.EventCode = MIDI_FILE_META_EVENT;
    EventBuffer.EventData.MetaEvent.MetaEventCode = MIDI_END_OF_TRACK;
    EventBuffer.EventData.MetaEvent.NBytes = 0;

    if (!NewTrack)
    {
        NewTrack = Midi_EventCreateList(&EventBuffer, True);
        RunningPtr = NewTrack;
    }
    else
    {
        RunningPtr = (EventList)Nconc(RunningPtr, Midi_EventCreateList
                                                        (&EventBuffer, True));
    }

    TempPtr = Midi_SeqRecordTimingDefaults(&RunningPtr);
    Midi_TrackConvertToOnePointRepresentation(TempPtr);
    Mapper_NewTrackMetaInfo();

    RETURN_PTR(TempPtr);
}


Boolean
Midi_SeqReadTrack()
{
char message[400];
unsigned int StartTime;
BEGIN("Midi_SeqReadTrack");

    IncomingEventBuf = (MIDIRawEventBuffer)malloc(MidiEventBufferSize
                                                      * sizeof(MIDIRawEvent));

    if (!IncomingEventBuf)
    {
         Error(NON_FATAL_REPORT_TO_MSGBOX, "Unable to allocate event buffer.");
         RETURN_BOOL(False);
    }

    if (Tracks.RecordDevice == -1)
    {
         sprintf(message, "\n  There is no Record device specified. \n\
   Ensure a Record device is available on your system. \n\
   Rosegarden will use the first Record device available.  \n");
         (void)YQuery(topLevel, message, 1, 0, 0, "Continue");
         Recording = False;
         RETURN_BOOL(False);
    }

    Midi_EnterMenuMode(RecordMode);
    while(XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);

    NextEventPtr = IncomingEventBuf;
    StartTime = LastTime; /* get current sequencer position */
    Recording = True;

    Midi_SeqPlayFile();
    Midi_TrackListAddTrack(Midi_SeqConvertRawDataToEventList(StartTime));
    Midi_ComputeTrackEndTime();

    Recording = False;
    /* need to scan off all pending recorded events */
    /* Mapper_GetPendingEvents(NextEventPtr) */

    Midi_LeaveMenuMode(RecordMode);

RETURN_BOOL(True);
}
