/*
 * new! Sequence.c uses Mapper interface
 */


#undef POSIX_PLEASE
#undef _POSIX_SOURCE

#include <Mapper.h>
#include "Globals.h"
#include "Consts.h"
#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <MidiBHeap.h>
#include <MidiTrack.h>

#include "Tracking.h"          /* for play-tracking */
#include "Sequence.h"
#include "Menu.h"
#include "Record.h"
#include "MidiSetupDlgs.h"
#include <Debug.h>

#define MIDI_EVENT_HEAP_SIZE 1024
#define MidiPlaybackEventsMask MidiSoundEventsMask | MidiSetTempoEventMask

extern Widget topLevel;
extern Widget TimeDisplay;
extern Widget EndofTrackLabel;
extern DeviceMetaInfoElement Devices;
extern TrackMetaInfoElement   Tracks;
extern int TracksOnDevice[Mapper_Devices_Supported][Mapper_Max_Tracks];
extern Boolean Recording;

byte InitialPatches[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

BinaryHeap  EventHeap[Mapper_Devices_Supported];
BinaryHeap  NoteOffHeap[Mapper_Devices_Supported];
MIDIEvent   NoteOffRecords[Mapper_Devices_Supported];
Boolean     Playing;

unsigned int LastTime = 0; /* used in Record positioning */
static unsigned int StartLastTime = 0;
static float PlayTime = 0.0;
static float TimeInc = 0;
static float StartPlayTime = 0.0;

float EndTrackTime = 0.0;

/* SGI and ZILOG synchronisation */


Boolean MidiPortSync        = True;

#ifndef SYSTEM_OSS
Boolean MidiMaintainTempo   = True;
#else
Boolean MidiMaintainTempo   = False;
#endif



void
Midi_SetTimeField(int Value)
{
char TimeField[20];
BEGIN("Midi_SetTimeField");

    sprintf(TimeField," %8.8d", Value);
    YSetValue(TimeDisplay, XtNlabel, TimeField);

    if (Playing == False)  /* don't process this while not playing */
         while(XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);
END;
}


void
Midi_SeqAllNotesOff()
{
    int i;
    MIDIEventStruct NextEvent;
    Boolean savedSync;

BEGIN("Midi_SeqAllNotesOff");

    /* Temporarily disable MidiPortSync to send note-offs immediately */
    savedSync = MidiPortSync;
    MidiPortSync = False;

    for ( i = 0; i < Devices.ActiveDevices; i++ )
    {
        while (HeapSize(NoteOffHeap[i]))
        {
            NextEvent = *(MIDIEvent)Value(NoteOffHeap[i], Root);
	    NextEvent.DeltaTime = 0;  /* Send immediately, not at scheduled time */
            Mapper_WriteEvent(&NextEvent, i);
            ExtractMin(NoteOffHeap[i]);
        }
    }

    /* Restore MidiPortSync setting */
    MidiPortSync = savedSync;

    Mapper_FlushQueue(PlayTime - StartPlayTime);

END;
}



/*
 * If the MIDI Device isn't in multi timbral mode then the
 * last loaded program change is going to be used for all
 * the channels.
 *
 * currently sends just the one set to the whole set of
 * devices
 *
 */
void
Midi_SendInitialProgramChange()
{
    int i, j;
    MIDIEventStruct IPCEvent;

BEGIN("Midi_SendInitialProgramChange");

    for ( j = 0; j < Devices.ActiveDevices; j++ )
    {
        for(i = 0; i < 16; i++)
        {
            IPCEvent.DeltaTime = 0;
            IPCEvent.EventCode = (MIDI_PROG_CHANGE+(byte)i);
            IPCEvent.EventData.NoteOn.Note = (byte)InitialPatches[i];
            IPCEvent.EventData.NoteOn.Velocity = 127;
            Mapper_WriteEvent(&IPCEvent, j);
        }
    }
END;
}



/*
 * Midi_NextDevice
 *
 * returns a null (-1) device if there's no events left on
 * any heap, else returns the device number of the next stack
 * that should play.
 *
 */
int
Midi_NextDevice(byte Type)
{
int i,
    NextDevice = -1,
    MinTime = -1,
    CurTime = -1;

BEGIN("Midi_NextDevice");


    for (i = 0; i < Devices.ActiveDevices; i++ )
    {
        if (Type == MIDI_NOTE_OFF)
        {
            if (HeapSize(NoteOffHeap[i]))
                CurTime=((MIDIEvent)Value(NoteOffHeap[i], Root))->DeltaTime;
        }
        else if (Type == MIDI_NOTE_ON)
        {
            if (HeapSize(EventHeap[i]))
                CurTime=(((EventList)Value(EventHeap[i], Root))
                                                        ->Event.DeltaTime);
        }
        else
        {
            fprintf(stderr,"EventList type unrecognised\n");
        }

        if (CurTime >= 0)
        {
            if ((CurTime < MinTime) || (MinTime == -1))
                NextDevice = i;
                MinTime = CurTime;
        }
    }

    RETURN_INT(NextDevice);
}


/*
 * Midi_NextNoteOff
 */
MIDIEvent
Midi_NextNoteOff(int *ReturnDevice)
{
MIDIEvent NextNoteOffEvent = NULL;
BEGIN("Midi_NextNoteOff");

    if ((((*ReturnDevice) = Midi_NextDevice(MIDI_NOTE_OFF)) != -1) &&
           (HeapSize(NoteOffHeap[(*ReturnDevice)])))
    {
        NextNoteOffEvent = (MIDIEvent)Value(NoteOffHeap[(*ReturnDevice)], Root);
    }

RETURN_PTR(NextNoteOffEvent);
}


/*
 * Midi_NextNoteOn is ANY event on the EventHeap not just note on
 */
EventList
Midi_NextNoteOn(int *ReturnDevice)
{
EventList NextNoteOnEvent = NULL;
BEGIN("Midi_NextNoteOn");

    if ((((*ReturnDevice) = Midi_NextDevice(MIDI_NOTE_ON)) != -1) &&
           (HeapSize(EventHeap[(*ReturnDevice)])))
    {
        NextNoteOnEvent = (EventList)Value(EventHeap[(*ReturnDevice)], Root);
    }

RETURN_PTR(NextNoteOnEvent);
}


/*
 * Midi_NextEvent
 *
 *
 * hides the MIDIEvent or EventList entries in a glorious
 * smudge of EventList.  Note Off returning ptr trouble,
 * but the Event itself is buried in the NoteOff stack.
 *
 */
int
Midi_NextEvent(EventList *ReturnEvent, int *ReturnDevice)
{
int NoteOnDevice,
    NoteOffDevice;

EventList NextNoteOnEvent,
          NextNoteOffEvent = (EventList)XtMalloc(sizeof(EventListElement));

MIDIEvent MidiNoteOff;

BEGIN("Midi_NextEvent");

    /* ++ get event and device next to play ++ */
    if ( ( NextNoteOnEvent = Midi_NextNoteOn(&NoteOnDevice) )
           == NULL )
    {
        RETURN_INT(False);
    }

    /* got something to play, check for next note off if
       we need one */
    if ( ( MidiNoteOff = Midi_NextNoteOff(&NoteOffDevice) ) == NULL )
    {
        /* no note off events pending - use the note on event only */
        (*ReturnEvent) = NextNoteOnEvent;
        (*ReturnDevice) = NoteOnDevice;

        ExtractMin(EventHeap[NoteOnDevice]);

        if (Next(NextNoteOnEvent))
            BHeapInsert(EventHeap[NoteOnDevice], Next(NextNoteOnEvent));
    }
    else
    {
        (NextNoteOffEvent->Event) = *MidiNoteOff;

        if ( (NextNoteOffEvent->Event.DeltaTime) >
             (NextNoteOnEvent->Event.DeltaTime) )
        {
            (*ReturnEvent) = NextNoteOnEvent;
            (*ReturnDevice) = NoteOnDevice;

            ExtractMin(EventHeap[NoteOnDevice]);

            if (Next(NextNoteOnEvent))
                BHeapInsert(EventHeap[NoteOnDevice], Next(NextNoteOnEvent));

        }
        else
        {
            (*ReturnEvent) = NextNoteOffEvent;
            (*ReturnDevice) = NoteOffDevice;

            /* only juggle the heap if we're going to use the thing */
            ExtractMin(NoteOffHeap[NoteOffDevice]);
        }
    }

/*  catches hanging end of tracks - this should be down to the midi file
   if ( ( (*ReturnEvent)->Event.EventCode == MIDI_FILE_META_EVENT ) &&
        ( (*ReturnEvent)->Event.EventData.MetaEvent.MetaEventCode ==
             MIDI_END_OF_TRACK ) )
       RETURN_INT(False);
*/

RETURN_INT(True);
}

void
Midi_ProcessNoteOn(EventList NextEvent, int Device)
{
int NoteOffIndex;
MIDIEvent NewEvent;

BEGIN("Midi_ProcessNoteOn");

    if ( MessageType(NextEvent->Event.EventCode) == MIDI_NOTE_ON)
    {
        NoteOffIndex = ChannelNum(NextEvent->Event.EventCode) * 128 +
                   (NextEvent->Event.EventData.Note.Note);

        /* reference to the note off event */
        NewEvent = (MIDIEvent)&(NoteOffRecords[Device][NoteOffIndex]);

        NewEvent->EventCode = CreateMessageByte(MIDI_NOTE_OFF,
                           ChannelNum(NextEvent->Event.EventCode));

        NewEvent->DeltaTime = NextEvent->Event.DeltaTime +
                           NextEvent->Event.EventData.Note.Duration;

        NewEvent->EventData.NoteOff.Note = NextEvent->Event.EventData.Note.Note;
        NewEvent->EventData.NoteOff.Velocity = 127;
        BHeapInsert(NoteOffHeap[Device], NewEvent);
    }

END;
}


/*
 * Midi_DispatchEvent - we're dispatching an ACTIVE Device
 */

void
Midi_DispatchEvent(EventList NextEvent, int Device)
{
BEGIN("Midi_DispatchEvent");

    Mapper_WriteEvent(&(NextEvent->Event), Device);
    Midi_ProcessNoteOn(NextEvent, Device);

END;
}



void
Midi_MetaEvent(EventList NextEvent)
{
unsigned int TempoValue;
BEGIN("Midi_MetaEvent");

    switch(NextEvent->Event.EventData.MetaEvent.MetaEventCode)
    {
        case MIDI_SET_TEMPO:
            TempoValue =
                (NextEvent-> Event.EventData.MetaEvent.Bytes << 16) |
                (*(&NextEvent->Event.EventData.MetaEvent.Bytes + 1) << 8) |
                *(&NextEvent->Event.EventData.MetaEvent.Bytes + 2);

            TimeInc = TempoValue / ( 10000.0 * MIDIHeaderBuffer.
                                                        Timing.Division );
            
            /* Send tempo meta event to MIDI devices so hardware knows about tempo changes */
            {
                MIDIEventStruct tempoEvent;
                int j;
                tempoEvent.DeltaTime = NextEvent->Event.DeltaTime;
                tempoEvent.EventCode = MIDI_SYSTEM_EXCLUSIVE;
                tempoEvent.EventData.MetaEvent.MetaEventCode = MIDI_SET_TEMPO;
                tempoEvent.EventData.MetaEvent.Bytes = NextEvent->Event.EventData.MetaEvent.Bytes;
                for (j = 0; j < Devices.ActiveDevices; j++) {
                    Mapper_WriteEvent(&tempoEvent, j);
                }
            }
            break;

        case MIDI_SYSTEM_EXCLUSIVE:
            break;

        default:
#ifdef SEQ_DEBUG
            fprintf(stderr,"Unsupported Meta Event in Sequence Loop\n");
#endif
            break;
    }

END;
}

/*
 * Scan the playback heaps for Program and Tempo change
 * events and send them to the required devices
 */
void
Midi_SendInitialPlaybackState(int StartTime)
{
    int i, j, TotalTracks, DeviceTrack;
    EventList Temp;
    DeviceList DevicePtr;

BEGIN("Midi_SendInitialPlaybackState");

    /* Switch to MD_NOSTAMP mode for immediate program change delivery */
    Mapper_SetStampModeAllDevices(0);  /* 0 = MD_NOSTAMP */

    for ( j = 0; j < Devices.ActiveDevices; j++ )
    {
        DevicePtr = Mapper_GetActiveDevice(j);
        TotalTracks = DevicePtr->TotalTracksOnDevice;

        for (i = 0; i < TotalTracks; i++)
        {
            DeviceTrack = TracksOnDevice[DevicePtr->Number][i];

            if ( DeviceTrack == -1 )
                 fprintf(stderr,"track/device allocation problem\n");

            Temp = (EventList)First(MIDITracks[DeviceTrack]);

            do
            {
                /* scan for meta events */

                if (Temp->Event.EventCode == MIDI_FILE_META_EVENT)
                    Midi_MetaEvent(Temp);

                else  /* scan for other important events */

                switch((int)((Temp->Event.EventCode) & 0xF0))
                {
                    case MIDI_PROG_CHANGE:
                        Mapper_WriteEvent(&(Temp->Event), j);
                        break;

                    default:
                        break;
                }

                if ((Temp = (EventList)Next(Temp)) == NULL)
                    break;
            }
            while (((Temp->Event.DeltaTime) <= (int)StartTime));
        }
    }
    
    /* Give synthesizer time to process program changes before playback starts.
     * 100ms should be enough for hardware to switch all instruments. */
    usleep(100000);
    
    /* Switch back to MD_RELATIVETICKS mode for timestamped playback */
    Mapper_SetStampModeAllDevices(1);  /* 1 = MD_RELATIVETICKS */
    
END;
}

void
Midi_FreeResources()
{
int i;
BEGIN("Midi_FreeResources");

    for ( i = 0; i < Devices.ActiveDevices; i++ )
    {
        if ( EventHeap[i] )
            XtFree(EventHeap[i]);

        if ( NoteOffHeap[i] )
            XtFree(NoteOffHeap[i]);

        if ( NoteOffRecords[i] )
            XtFree(NoteOffRecords[i]);
    }

END;
}

/*
 *
 * Midi_SeqStopPlaying
 *
 * called from the specific close CB
 * 
 */
void
Midi_SeqStopPlaying()
{
BEGIN("Midi_SeqStopPlaying");
    if (Playing)
    {
        Mapper_StopTimer();
        Midi_SeqAllNotesOff();
        /* Don't close devices - they stay open for next playback */
        /* Mapper_CloseActiveDevices(); */
        Midi_FreeResources();
        Playing = False;
    }
    
    if (Recording == False)
        Midi_PlayTrackingClose();
END;
}



/*
 * Midi_SeqStopPlayingCB
 */
void Midi_SeqStopPlayingCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SeqStopPlayingCB");

    Midi_SeqStopPlaying();

    XUndefineCursor(display, XtWindow(topLevel));
    XSync(display, False);
    
    if (Recording == True)
        Midi_SeqStopRecordingCB(w, a, b);

    
    Midi_ServiceEventListGUIs(True);
    Midi_LeaveMenuMode(PlaybackMode);
    Midi_EnterMenuMode(NotPlayingMode);

END;
}



/*
 * Function:     Midi_SeqPlayNotes
 *
 * Description:  Play the events.  Sibling to Midi_SeqIntCB.
 *
 */
void
Midi_SeqPlayNotes()
{
EventList    NextEvent = NULL;
int Device;
int TrackTime;

    BEGIN("Midi_SeqPlayNotes");

    Mapper_StartTimer();
    Mapper_OpenActiveDevices();
    
    /* Reset MIDI timeline for new playback session */
    Mapper_ReinitializeForPlayback();

    StartPlayTime = PlayTime;
    StartLastTime = LastTime;
    while ((Playing == True) || (Recording == True))
    {
        if ( Playing == True )
        {
            if ( Midi_NextEvent(&NextEvent, &Device) == False )
            {
                if (Recording == False)
                {
                    Midi_PlayTrackingClose();
                    Midi_RewindTimerCB(NULL, NULL, NULL);
                }
                Playing = False;
                continue;
            }

            if (Recording == False)
                Midi_PlayTrackingJump(LastTime);

            if (((NextEvent->Event.DeltaTime) > LastTime ) && TimeInc )
            {
                int Passer = (int) LastTime;
                if ((TrackTime = Mapper_QueueEvent(NextEvent, &Passer,
                                 &PlayTime, StartLastTime, StartPlayTime,
                                 TimeInc)))
                {
                    Midi_SetTimeField(TrackTime);
                }
                LastTime = (unsigned int)Passer;
            }

            if (NextEvent->Event.EventCode == MIDI_FILE_META_EVENT)
                Midi_MetaEvent(NextEvent);
            else
                Midi_DispatchEvent(NextEvent, Device);
        }

        if (Recording)
             Midi_SeqRecordEvent();

        while (XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);
    }

    while (XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);
END;
}


     /* compute the end of track time only
        needs to be done once per load */
void
Midi_ComputeTrackEndTime()
{
    int i;
    EventList Temp;
    char TimeField[20];

    EndTrackTime = 0.0;

    for (i = 0; i < MIDIHeaderBuffer.NumTracks; i++)
    {
        Temp = (EventList)First(MIDITracks[i]);

        while (Temp)
        {
            if (Temp->Event.DeltaTime >= EndTrackTime)
                EndTrackTime = (Temp->Event.DeltaTime);

            Temp = (EventList)Next(Temp);
        }
    }

    sprintf(TimeField," ( %8.8d )", (int)EndTrackTime);
    YSetValue(EndofTrackLabel, XtNlabel, TimeField);
}


void
Midi_ResetTimingInformation()
{
BEGIN("Midi_ResetTimingInformation");

    LastTime = 0;
    PlayTime = 0.0;
    Midi_ComputeTrackEndTime();

END;
}


/*
 * Midi_InitializePlaybackHeaps
 * 
 * get devices that are being used and filter tracks by the output device.
 * then create heaps for each active device.
 *
 */

void
Midi_InitializePlaybackHeaps(int StartTime)
{
int i,
    j,
    ActiveDevice,
    TotalTracks,
    DeviceTrack;

DeviceList DevicePtr;
EventList Temp;

BEGIN("Midi_InitializePlaybackHeaps");

    for ( j = 0; j < Devices.ActiveDevices; j++ )
    {
        EventHeap[j] = CreateBHeap(MIDI_EVENT_HEAP_SIZE,
                               Midi_EventListTimeLessp);

        NoteOffHeap[j] = CreateBHeap(128*16, Midi_EventTimeLessp);

        NoteOffRecords[j] = (MIDIEvent)XtMalloc(128*16*sizeof(MIDIEventStruct));

        DevicePtr = Mapper_GetActiveDevice(j);
        ActiveDevice = DevicePtr->Number;
        TotalTracks = DevicePtr->TotalTracksOnDevice;

        for (i = 0; i < TotalTracks; i++)
        {
            DeviceTrack = TracksOnDevice[ActiveDevice][i];

            if (DeviceTrack == -1)
                 fprintf(stderr,"track/device allocation problem\n");

            Temp = (EventList)First(MIDITracks[DeviceTrack]);

            if (StartTime >= 0)
            {
                while (Temp)
                {
                    if (Temp->Event.DeltaTime > StartTime)
                        break;

                    Temp = (EventList)Next(Temp);
                }
            }

            if (Temp) BHeapInsert(EventHeap[j], Temp);
        }
    }
END;
}


/*
 * Function:     Midi_SeqPlayFile
 *
 * Description:  Once a tree.  Now sawdust.
 *
 */
Boolean
Midi_SeqPlayFile()
{
int DeviceOpenOK;
char message[1000];
int ManageMask = 0;
BEGIN("Midi_SeqPlayFile");

    if (Playing == True)
        RETURN_BOOL(True);
    else
        Playing = True;

    if (MidiPortSync==True)
        ManageMask|=Device_Port_Sync;

    if (MidiMaintainTempo==True)
        ManageMask|=Device_Tempo_Maintain;

    if (Recording == False)
        DeviceOpenOK = Mapper_ManageDevices(Device_Active_WO|ManageMask);
    else
        DeviceOpenOK = Mapper_ManageDevices(Device_Active_WR|ManageMask);

    if (DeviceOpenOK == False)
    {
        /* pop up a dialog that says so */

        sprintf(message, "\n  Device : %s\n\
\n\
  The sequencer device shown above is not responding.  \n\
  Because of this %s cannot take place.  \
\n\n",
            Devices.FileDescriptor,
            Recording == True ? "Recording" : "Playback");

        if (Playing == True)
            Midi_SeqStopPlayingCB(NULL, NULL, NULL);

        if (Recording == True)
            Midi_SeqStopRecordingCB(NULL, NULL, NULL);

        Playing = Recording = False;

        (void)YQuery(topLevel, message, 1, 0, 0,
                                      "Continue without device support");

        while(XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);

        RETURN_BOOL(False);
    }

    Midi_EnterMenuMode(PlaybackMode);
    Midi_LeaveMenuMode(NotPlayingMode);
    while(XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);

    /* ensure the tracks are arranged properly
       per device - this uses a temporary array
       for the playback/record loop */

    Mapper_FilterTracksbyDevice();
    Midi_InitializePlaybackHeaps(LastTime);

    /* initialize and mark up the devices with the
       correct programs etc. */
    Mapper_Initialize();
    /* Midi_SendInitialProgramChange(); */  /* disabled - rely on file's program changes */
    Midi_SendInitialPlaybackState(LastTime);
    Midi_ServiceEventListGUIs(False);

    /* register with editor */
    if (Recording == False)
        Midi_PlayTrackingOpen();

    /* process any pending X events */
    while(XtAppPending(appContext))
    {
        XtAppProcessEvent(appContext, XtIMAll);
    }

    Midi_SeqPlayNotes();

    if ( Playing == True )
    {
        if (Recording == True)
        {
            Midi_SeqStopRecordingCB(NULL, NULL, NULL);
            Recording = False;
        }
        else
        {
            Midi_PlayTrackingClose();
        }
        Midi_RewindTimerCB(NULL, NULL, NULL);  /* sets Playing to False */
    }

    RETURN_BOOL(True);
}



    /* for the rwd'ing and ffwd'ing (REMEMBER):
       got to clear the bheaps - rewind or ffwd the 
       event stacks to the required timing point and
       then set the timers */


void
Midi_RwdTimerCB(Widget w, XtPointer a, XtPointer b)
{
    const float RwdTime = 600.0;

BEGIN("Midi_RwdTimer");


    LastTime -= (LastTime > (int)RwdTime) ? (int)RwdTime : LastTime;
    StartLastTime -= (StartLastTime > (int)RwdTime) ? (int)RwdTime :
                                                               StartLastTime;
    PlayTime = 0.0;
    StartPlayTime = 0.0;

    Midi_SetTimeField(LastTime);

    if (Playing == True)
    {
         Mapper_StopTimer();
         Midi_SeqAllNotesOff();
         Midi_FreeResources();
         Midi_InitializePlaybackHeaps(LastTime);
         Mapper_InitVoiceAllocations();
         Mapper_StartTimer();
    }

END;
}


void
Midi_FfwdTimerCB(Widget w, XtPointer a, XtPointer b)
{
    const float FfwdTime = 600.0;

BEGIN("Midi_FfwdTimer");

    if ((EndTrackTime - LastTime) > FfwdTime)
    {
         LastTime += (int)FfwdTime;
         StartLastTime += (int)FfwdTime;
         PlayTime += FfwdTime;
         StartPlayTime += FfwdTime;
    }
    else
    {
        LastTime = (unsigned int)EndTrackTime;
        StartLastTime = (unsigned int)EndTrackTime;
    }

    /* increment the mapper timer in step with Sequencer*/
    Mapper_ModifyTimer(FfwdTime);

    Midi_SetTimeField(LastTime);

    if (Playing == True)
    {
        Midi_SeqAllNotesOff();
        Midi_FreeResources();
        Midi_InitializePlaybackHeaps(LastTime);
        Mapper_InitVoiceAllocations();
    }

END;
}


/*
 * rewind all the way
 */
void
Midi_RewindTimerCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_RewindTimer");

    if (Playing == True)
    {
        Midi_SeqStopPlayingCB(w, a, b);
    }

    Midi_ResetTimingInformation();
    Midi_SetTimeField(0);
END;
}

void
Midi_SkiptoEndCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SkiptoEndCB");
    if (Playing == True)
    {
        Midi_SeqStopPlayingCB(w, a, b);
    }

    LastTime = (int)EndTrackTime;
    StartLastTime = (int)EndTrackTime;
    PlayTime = EndTrackTime;
    StartPlayTime = EndTrackTime;

    Midi_SetTimeField(LastTime);

END;
}


void
Midi_AllNotesOffCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_AllNotesOffCB");

    Mapper_AllNotesOff();  /* Light reset - just silence notes */
    fprintf(stderr, "Reset: All Notes Off sent\n");

END;
}

void
Midi_ResetCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_ResetCB");

    Mapper_Reset();  /* straight through to the mapper */
    fprintf(stderr, "System Reset: GM System Reset complete\n");

END;
}
