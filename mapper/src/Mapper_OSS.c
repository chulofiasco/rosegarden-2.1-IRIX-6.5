/*
 * Midi Event Mapper routines
 */


#undef POSIX_PLEASE
#undef _POSIX_SOURCE

#include "Mapper.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <MidiBHeap.h>
#include <MidiTrack.h>
#include <Debug.h>
#include <../../sequencer/src/Globals.h>

TrackMetaInfoElement  Tracks;
DeviceMetaInfoElement Devices;
int TracksOnDevice[Mapper_Devices_Supported][Mapper_Max_Tracks];
int seqfd;


SEQ_DEFINEBUF(1024);

void
Mapper_FlushQueue(float FinishTime)
{

    SEQ_WAIT_TIME((int) FinishTime);
    SEQ_DUMPBUF();
}


void
seqbuf_dump(void)
{
    BEGIN("seqbuf_dump");

    if ( _seqbufptr )
        if ( write ( seqfd, _seqbuf, _seqbufptr ) == -1 )
        {
            /* write has failed */
            perror ("Rosegarden OSS Mapper: write /dev/sequencer failed");
        }
    _seqbufptr = 0;

    /* synchonisation call */
    ioctl(seqfd, SNDCTL_SEQ_SYNC);

END;
}        

void
Mapper_CloseDevice()
{
BEGIN("Mapper_CloseDevice");

    Devices.MetaDeviceStatus = Device_Unitialised;
    close(seqfd);

END;
}

/*
 * Function:     Mapper_WriteMidiEvent
 *
 * Description:  Write the events onto the output buffer according to type
 *
 */
void
Mapper_WriteMidiEvent(MIDIEvent OutEvent, int DeviceNumber)
{
    byte        StatusByte;
    static byte LastStatusByte = 0;

    BEGIN("Mapper_WriteMidiEvent");

#ifdef SEQ_DEBUG
    fprintf(stdout,"Writing out Event Data %x\n",OutEvent->
                                      EventData.NoteOn.Note);
#endif /* SEQ_DEBUG */
    
    StatusByte = OutEvent->EventCode;

    if (StatusByte != LastStatusByte)
    {
        LastStatusByte = StatusByte;
        SEQ_MIDIOUT(DeviceNumber,StatusByte);
    }

    if (MessageType(StatusByte) == MIDI_CTRL_CHANGE ||
        MessageType(StatusByte) == MIDI_PROG_CHANGE)
    {
        SEQ_MIDIOUT(DeviceNumber,(byte)OutEvent->EventData.
                                                   ProgramChange.Program);
    }
    else
    {
        /* out with an event */
        SEQ_MIDIOUT(DeviceNumber,(byte)OutEvent->EventData.NoteOn.Note);
        SEQ_MIDIOUT(DeviceNumber,(byte)OutEvent->EventData.NoteOn.Velocity);
    }

END;
}


/*
 * Function:     Mapper_WriteSynthEvent
 *
 * Description:  Kajagoogoo
 *
 * Now incorporating a timing mechanism to map the MIDI channels
 * to available synthesiser channels.  This means that we can now
 * have some "polyphony" on playback to a single channel( MIDI).
 * The synth channels  are allocated by the NOTE_ON events and
 * then deallocated by the NOTE_OFF, although the ltter gives a 
 * weight to the free channels that means that they cannot be used
 * for a certain amount of NOTE_ON operations.  This allows some 
 * time for note decay to take place before the channel is reused
 * by another NOTE_ON.  As we have (arbitrarily) 16 channels then
 * we cannot use this many iterations for each free channel if we
 * still want polyphony.  Play around with the value on the
 * NOTE_OFF events to see the effects on playback.  There is of course a 
 * maximum bandwidth available and we're just stealiung bites out
 * of it.  Some pieces just won't play properly.
 * e.g. setting the decay value to -10, leaves us with a max of 
 * 6 note polyphony in case we need it.  This buffer is useful!
 *                
 * Another mechanism!  The above isn't (by itself) good enough.
 * 
 *
 * Stopping Notes on Channel 0 seems to give spurious playback
 * results on that channel - now given a moveable playback scope
 * (channels) to allow for edit channel(s).
 *
 */

void
Mapper_WriteSynthEvent(MIDIEvent OutEvent, int DeviceNumber)
{
    byte        StatusByte;
    static byte LastStatusByte = 0;
    byte Note;
    byte Vely;

    static long SRLastTime[Mapper_Devices_Supported];

    static byte Channel[Mapper_Devices_Supported] =
        { 0,0,0,0,0,0,0,0 };

    static int ChanneltoUse[Mapper_Devices_Supported] =
          {-1,-1,-1,-1,-1,-1,-1,-1};

    static int Mapping[Mapper_Devices_Supported][16] =
        { {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
          {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} };

    static int Patches[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    static int Started[Mapper_Devices_Supported][16] =
        { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };

    int FirstAvailable;

    static int bank[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    int DecayFactor = -4; /* twiddle this and hear the difference
                             (no value greater than -2 please )     */

    int MinSynthChannel = 1;
    int MaxSynthChannel = 16;

    int i;

#ifdef NOTE_QUEUE_DEBUG
    char map[50];
#endif /* NOTE_QUEUE_DEBUG */

    BEGIN("Mapper_WriteMidiEvent");

    /* reset all channels and all static mappings */
    for ( i = 0; i < 16; i++ )
    {

        if ( OutEvent == NULL )
        {
            Mapping[DeviceNumber][i] = -1;
            Started[DeviceNumber][i] = 0;
        }
        else
            SEQ_CONTROL(DeviceNumber, i, 127, 0); /* hmm */
    }

    if ( OutEvent == NULL )
        return;

    Note = (byte)OutEvent->EventData.NoteOn.Note;
    Vely = (byte)OutEvent->EventData.NoteOn.Velocity;

    SEQ_DUMPBUF();

    StatusByte = OutEvent->EventCode;

    if (StatusByte != LastStatusByte)
    {
        LastStatusByte = StatusByte;
    }

    /* channel for output */
    if ( (StatusByte&0xf) != Channel[DeviceNumber] )
    {
        Channel[DeviceNumber] = ((StatusByte & 0xf));
    }

    /* promote any that are marked for freeing */
    switch(MessageType(StatusByte & 0xf0))
    {
        case MIDI_KEY_PRESSURE:
            ChanneltoUse[DeviceNumber] = -1;
            for (i = MinSynthChannel; ( i < MaxSynthChannel ) &&
                        ( ChanneltoUse[DeviceNumber] == -1 ) ; i++ )
                if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                {
                    ChanneltoUse[DeviceNumber] = i;
                }

            SEQ_KEY_PRESSURE(DeviceNumber, ChanneltoUse[DeviceNumber],
                             Note, Vely);
            break;

        case MIDI_NOTEON:
            if ( Vely == 0 ) /* Note off event */
            {
                for (i = MinSynthChannel; i < MaxSynthChannel; i++ )
                    if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                    {
                        SEQ_STOP_NOTE(DeviceNumber, Mapping[DeviceNumber][i],
                                      Note, Vely);
                        /* mark for freeing */
                        Mapping[DeviceNumber][i] = DecayFactor;
                    }
            }
            else {
            SRLastTime[DeviceNumber]++;

            /* get a free channel */
            ChanneltoUse[DeviceNumber] = -1;
            FirstAvailable = 0;
            for ( i = MinSynthChannel; i < MaxSynthChannel ; i++ )
                if (Started[DeviceNumber][i] > FirstAvailable)
                    FirstAvailable = Started[DeviceNumber][i];

            for ( i = MinSynthChannel; i < MaxSynthChannel ; i++ )
            {
                if ( (Mapping[DeviceNumber][i] == -1) &&
                        ( Started[DeviceNumber][i] <= FirstAvailable ) )
                {
                    FirstAvailable = Started[DeviceNumber][i];
                    ChanneltoUse[DeviceNumber] = i;
                }
            }

            if (ChanneltoUse[DeviceNumber] >= 0)
            {

                Mapping[DeviceNumber][ChanneltoUse[DeviceNumber]]
                    = Channel[DeviceNumber];   /* occupy the voice */

                /* we're using this channel now */
                Started[DeviceNumber][ChanneltoUse[DeviceNumber]]
                    = SRLastTime[DeviceNumber];

                SEQ_CONTROL(DeviceNumber, ChanneltoUse[DeviceNumber],
                            CTL_BANK_SELECT, bank[ChanneltoUse[DeviceNumber]]);

                 /* drums */
                 if ( ( Channel[DeviceNumber] == PERC_CHN ) )
                 {
/*
                     SEQ_SET_PATCH(DeviceNumber, ChanneltoUse[DeviceNumber],
                                   Patches[ChannelNum(StatusByte)]);
*/
                     SEQ_SET_PATCH(DeviceNumber, ChanneltoUse[DeviceNumber],
                                   Note+128);
#ifdef DRUM_DEBUG
                      fprintf(stderr,"\n ++ Drum Patch = %d ++",
                                   Patches[Channel[DeviceNumber]]);

                      fprintf(stderr,"\n ++ Note       = %d ++\n\n", Note);
#endif /* DRUM_DEBUG */

                 }
                 else
                 {
                     /* set the patch and play on the synth channel used */
                     SEQ_SET_PATCH(DeviceNumber, ChanneltoUse[DeviceNumber],
                                   Patches[ChannelNum(StatusByte)]);
                 }

                SEQ_START_NOTE(DeviceNumber, ChanneltoUse[DeviceNumber],
                               Note, Vely);

#ifdef NOTE_QUEUE_DEBUG
                sprintf(map,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                                  Mapping[DeviceNumber][0],
                                  Mapping[DeviceNumber][1],
                                  Mapping[DeviceNumber][2],
                                  Mapping[DeviceNumber][3],
                                  Mapping[DeviceNumber][4],
                                  Mapping[DeviceNumber][5],
                                  Mapping[DeviceNumber][6],
                                  Mapping[DeviceNumber][7],
                                  Mapping[DeviceNumber][8],
                                  Mapping[DeviceNumber][9],
                                  Mapping[DeviceNumber][10],
                                  Mapping[DeviceNumber][11],
                                  Mapping[DeviceNumber][12],
                                  Mapping[DeviceNumber][13],
                                  Mapping[DeviceNumber][14],
                                  Mapping[DeviceNumber][15]);

                fprintf(stderr,"%s\n",map);
#endif /* NOTE_QUEUE_DEBUG */

            }

            /* else don't play anything because no free channels */

#ifdef NOTE_QUEUE_DEBUG
            else
                fprintf(stderr,
                     "SKIPPED NOTE in PLAYBACK - decrease note DECAY factor\n");
#endif /* NOTE_QUEUE_DEBUG */

            for (i = MinSynthChannel; i < MaxSynthChannel; i++ )
            {
                if (Mapping[DeviceNumber][i] < -1 )
                    Mapping[DeviceNumber][i]++;
            } }
            break;

        case MIDI_NOTEOFF:
            /* free channel as note ends */

            for (i = MinSynthChannel; i < MaxSynthChannel; i++ )
                if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                {
                    SEQ_STOP_NOTE(DeviceNumber, i, Note, Vely);

                    if ( Mapping[DeviceNumber] [i] != PERC_CHN )
                        /* mark for freeing 
                           - with a langrous delay for normal notes */
                        Mapping[DeviceNumber][i] = DecayFactor;
                    else
                        /* free a drum staright away */
                        Mapping[DeviceNumber][i] = -1;
                }

            break;

        case MIDI_CTL_CHANGE:
            ChanneltoUse[DeviceNumber] = -1;
            for (i = MinSynthChannel; ( i < MaxSynthChannel ) &&
                        ( ChanneltoUse[DeviceNumber] == -1 ) ; i++ )
                if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                {
                    ChanneltoUse[DeviceNumber] = i;
                }

             switch(Note)
             {
                 case CTL_BANK_SELECT:
                     bank[ChanneltoUse[DeviceNumber]] = Vely;
                     break;

                 default:
                     SEQ_CONTROL(DeviceNumber, ChanneltoUse[DeviceNumber],
                                 Note, Vely);
                     break;
             }
             break;

        case MIDI_CHN_PRESSURE:
            ChanneltoUse[DeviceNumber] = -1;
            for (i = MinSynthChannel; ( i < MaxSynthChannel ) &&
                        ( ChanneltoUse[DeviceNumber] == -1 ) ; i++ )
                if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                {
                    ChanneltoUse[DeviceNumber] = i;
                }

             SEQ_CHN_PRESSURE(DeviceNumber, ChanneltoUse[DeviceNumber], Note);

             break;

        case MIDI_PITCH_BEND:
            ChanneltoUse[DeviceNumber] = -1;
            for (i = MinSynthChannel; ( i < MaxSynthChannel ) &&
                        ( ChanneltoUse[DeviceNumber] == -1 ) ; i++ )
                if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                {
                    ChanneltoUse[DeviceNumber] = i;
                }

             SEQ_BENDER(DeviceNumber, ChanneltoUse[DeviceNumber], Note);

             break;

        case MIDI_PGM_CHANGE:
            ChanneltoUse[DeviceNumber] = -1;
            for (i = MinSynthChannel; ( i < MaxSynthChannel ) &&
                        ( ChanneltoUse[DeviceNumber] == -1 ) ; i++ )
                if ( Mapping[DeviceNumber][i] == Channel[DeviceNumber] )
                {
                    ChanneltoUse[DeviceNumber] = i;
                }

             /* drums */
             if ( ( Channel[DeviceNumber] == PERC_CHN ) )
             {
                 Note += 128;
             }

             Patches[ChannelNum(StatusByte)] = Note;
             break;
 
        default:
            fprintf(stderr,"Unsupported MIDI event to Synth\n");
            break;
    }

    SEQ_DUMPBUF();

END;
}



/*
 * Mapper_WriteEvent
 */
void
Mapper_WriteEvent(MIDIEvent NextEvent, int DeviceNumber)
{
    DeviceList PBDevice;
    int PlayAll = False;
    int CurrentDevice = 0;

BEGIN("Mapper_WriteEvent");

    PBDevice = Mapper_GetActiveDevice(DeviceNumber);
    if (Mapper_GetActiveDevice(DeviceNumber)->Device.Type == Mapper_All_Device)
        PlayAll = True;

    do
    {
        /* get the nth Active device if we're playing to a single
           device or step through them all in turn if this is
           an ALL device */

        if (PlayAll == True)
            PBDevice = Mapper_GetDevice(CurrentDevice++);

        if (PBDevice->Device.Type == Mapper_Midi_Device)
            Mapper_WriteMidiEvent(NextEvent,PBDevice->Device.Data.Midi.device);
        else /* a synth */
        {
            switch(PBDevice->Device.Data.Synth.synth_type)
            {
                case SYNTH_TYPE_FM:
                case SYNTH_TYPE_SAMPLE:
                    Mapper_WriteSynthEvent(NextEvent,
                                             PBDevice->Device.Data.Midi.device);
                    break;

                case SYNTH_TYPE_MIDI: /* ?? */
                    break;

                default:
                fprintf(stderr,"Synth Write to unsupported device type\n");
                break;
            }
        }
    }
    while((PlayAll == True) && ( CurrentDevice < (Devices.MaxDevices - 1)));
END;
}

Boolean
Mapper_OpenDevice(int Sense, char *Device)
{
    char  ErrBuff[128];
    int   DevSense;

BEGIN("Mapper_OpenDevice");

    if (Sense&Device_Active_WO) DevSense = O_WRONLY;
    else if (Sense&Device_Active_WR) DevSense = O_RDWR|O_NONBLOCK;
    else RETURN_BOOL(False);

    /* if the Sense is WO and we are WO then return */
    if ((Devices.MetaDeviceStatus&Sense) && (Sense&Device_Active_WO))
        RETURN_BOOL(True);

    /* if we're WR then close and re-open no matter what for timing
       purposes */
    if (Sense&Device_Active_WR)
        Mapper_CloseDevice();

    /* we're clear to carry on */
    Devices.MetaDeviceStatus=Sense;

    if ((seqfd = open(Device, DevSense, 0)) == -1)
    {

        switch (errno)
        {
            case ENOENT:
                sprintf(ErrBuff, "\"%s\" : Device File Missing\n", Device);
                break;

            case ENODEV:
                sprintf(ErrBuff, "\"%s\" : Device Driver Not Installed\n",
                                                     Device);
                break;

            case ENXIO:
                sprintf(ErrBuff, "\"%s\" : No Hardware detected\n", Device);
                break;

            case EBUSY:
                sprintf(ErrBuff, "Device \"%s\" is busy\n", Device);
                break;

            default:
                sprintf(ErrBuff, "\"%s\" : Cannot Open Device\n", Device);
                break;
        }
        Error(NON_FATAL_REPORT_TO_MSGBOX, ErrBuff);
        RETURN_BOOL(False);
    }

RETURN_BOOL(True);
}

void
Mapper_LoadFMPatchSet(int DeviceNumber)
{
    struct sbi_instrument Instrument;
    int i,j;
    int PatchFile;
    byte Buffer[60];
    int DataSize;
    int VoiceSize;

    BEGIN("Mapper_LoadFMPatchSet");

    Instrument.device = DeviceNumber;
    Instrument.key = FM_PATCH;
    VoiceSize = 52;
    DataSize = 11;


    /* manage devices to be able to write patch information to them */
    if ( Mapper_ManageDevices(Device_Active_WO) != True )
        return;

    /* open synth patch file */
    if ((PatchFile = open(appData.midiFmPatchFile, O_RDONLY, 0)) <= 0 )
    {
        fprintf(stderr,
             "Rosegarden OSS Mapper: Cannot open Synth Patch File\n");
        return;
    }

    for ( i = 0; i < 128; i++ )
    {

        /*if (read(PatchFile, Buffer, VoiceSize) != VoiceSize)*/
        if (read(PatchFile, Buffer, VoiceSize) == -1 )
        {
            fprintf(stderr,
 "Rosegarden OSS Mapper:  Short Synth Patch File - Aborting Patch Load\n");
            perror("Patch loading error");
            Mapper_CloseDevice();
            return;
        }

        Instrument.channel = i;

        Buffer[46] = (Buffer[46] & 0xcf) | 0x30;  /* provide two channel
                                                     output it seems */

        for ( j = 0; j < 32; j++ )
            Instrument.operators[j] = (j < DataSize) ? Buffer[36 + j] : 0;

        SEQ_WRPATCH(&Instrument,sizeof(Instrument));
    }
    close(PatchFile);


    if ((PatchFile = open(appData.midiFmDrumPFile, O_RDONLY, 0)) <= 0 )
    {
        fprintf(stderr, "Rosegarden OSS Mapper: Cannot open Drum Patch File\n");
        Mapper_CloseDevice();
        return;
    }

    for (i = 128; i < 175; i++)
    {
        if (read(PatchFile, Buffer, VoiceSize) != VoiceSize)
        {
            fprintf(stderr,
              "Rosegarden OSS Mapper: Short FM Drums Patch File - Aborting Patch Load\n");
            return;
        }
        Instrument.channel = i;

        Buffer[46] = (Buffer[46] & 0xcf) | 0x30;  /* provide two channel
                                                     output it seems */

        for ( j = 0; j < 32; j++ )
            Instrument.operators[j] = (j < DataSize) ? Buffer[36 + j] : 0;

        SEQ_WRPATCH(&Instrument,sizeof(Instrument));
    }
    close(PatchFile);

    Mapper_CloseDevice();
END;
}

void
Mapper_LoadSamplePatches(int DeviceNumber)
{
/*
    struct sbi_instrument Instrument;
    int i,j;
    int PatchFile;
    byte Buffer[60];
    int DataSize;
    int VoiceSize;
*/

BEGIN("Mapper_LoadSamplePatches");

    if ( Mapper_ManageDevices(Device_Active_WO) != True )
        return;

    Mapper_CloseDevice();

END;
}

/*
 * Mapper_LoadPatches
 */
void
Mapper_LoadPatches(void)
{
int i;
DeviceList TempPtr = NULL;

BEGIN("Mapper_LoadPatches");
    for ( i = 0; i < Devices.MaxDevices; i++ )
    {
        TempPtr = Mapper_GetDevice(i);

        if ( !(TempPtr->IO_Status&Patchloaded) )
        {
            switch(TempPtr->Device.Type)
            {
   
                case Mapper_Synth_Device:
                {
                    switch(TempPtr->Device.Data.Synth.synth_type)
                    {
                        case SYNTH_TYPE_FM:
                            Mapper_LoadFMPatchSet(i);
                            break;

                        case SYNTH_TYPE_SAMPLE:
                            Mapper_LoadSamplePatches(i);
                            break;

                        default:
                            break;
                    }
                }

                case Mapper_Midi_Device:
                default:
                    break;
            }
            TempPtr->IO_Status&=Patchloaded;
        }
    }
END;
}


/* a logical ALL device for all devices in
   the list - if we only have one device in
   the list then we don't need an ALL.
   We use the Number entry to signify this is
   a logical device */
void
Mapper_CreateAllDevice()
{
    DeviceInformation TempDevice;
    DeviceList CurrentDev = NULL;

BEGIN("Mapper_CreateALLDevice");

    if (  Devices.MaxDevices <= 1 )
        END;

    TempDevice.Type = Mapper_All_Device;
    strcpy(TempDevice.Data.Synth.name, Mapper_All_Device_Label);
    CurrentDev = (DeviceList)Mapper_NewDeviceList
                                          (TempDevice,Devices.MaxDevices);
    LIST_Insert ( (List) Devices.Device, (List) CurrentDev );
    Devices.MaxDevices++;

END;
}


/*
 * Function:     Mapper_DeviceQuery
 *
 * Description:  Opens device and queries for all Midi ports
 *               and available synths.  Fills out a list that
 *               is inherent to the global meta device info
 *               object.
 *
 */
DeviceList
Mapper_DeviceQuery()
{
    int midi_devs = 0;
    int synth_devs = 0;
    DeviceInformation TempDevice;
    DeviceList CurrentDev = NULL;

    int i;

BEGIN("Mapper_QueryDevice");

    Devices.MaxDevices = 0;
    Devices.ActiveDevices = 0;

    /* open device */
    if ( Mapper_ManageDevices(Device_Active_WO) != True )
        return NULL;

    /* call to see if there are any synths on-line */
    ioctl(seqfd, SNDCTL_SEQ_NRSYNTHS, &synth_devs);

    /* see if we have any MIDI devices available */
    ioctl(seqfd, SNDCTL_SEQ_NRMIDIS, &midi_devs);

    if ( (!midi_devs) && (!synth_devs) )
    {
#ifdef DEBUG
        Error(NON_FATAL_REPORT_TO_MSGBOX,"No Configurable Devices on Port\n");
#endif
        Mapper_CloseDevice();
        return NULL;
    }

    /* construct the devices return list */
    for ( i = 0; i < synth_devs; i++ )
    {
        /* initialise the structure */
        TempDevice.Data.Synth.device = i;
        ioctl(seqfd, SNDCTL_SYNTH_INFO, &(TempDevice.Data.Synth));

        TempDevice.Type = Mapper_Synth_Device;

        /* work around perceived bug in kernel with AWE32
           device not returning the correct device number -
           perhaps this method is actually correct and the
           OSS documentation is omitting something */

        TempDevice.Data.Synth.device=i;

        if ( Devices.Device == NULL )
        {
            Devices.Device = (DeviceList)Mapper_NewDeviceList(TempDevice,
                                                      Devices.MaxDevices);
        }
        else
        {
             /* append */
            CurrentDev = (DeviceList)Mapper_NewDeviceList(TempDevice,
                                                     Devices.MaxDevices);
            LIST_Insert ( (List) Devices.Device, (List) CurrentDev );
        }
        Devices.MaxDevices++;
    }

    for ( i = 0; i < midi_devs; i++ )
    {
        TempDevice.Data.Midi.device = i;
        ioctl(seqfd, SNDCTL_MIDI_INFO, &(TempDevice.Data.Midi));

        TempDevice.Type = Mapper_Midi_Device;

        if ( Devices.Device == NULL )
        {
            Devices.Device = (DeviceList)Mapper_NewDeviceList(TempDevice,
                                                      Devices.MaxDevices);
        }
        else
        {
            /* again append */
            CurrentDev = (DeviceList)Mapper_NewDeviceList(TempDevice,
                                                     Devices.MaxDevices);

            LIST_Insert ( (List) Devices.Device, (List) CurrentDev );
        }

        Devices.MaxDevices++;
    }

    Mapper_CloseDevice();
    Mapper_CreateAllDevice(); /* create the "All" Device */

    RETURN_PTR(Devices.Device);
}

Boolean
Mapper_SetupDevices(char *midiPortName)
{
int i;
BEGIN("Mapper_SetupDevices");

    /* initialise devices meta information */
    Devices.Device = NULL;
    Devices.FileDescriptor = XtNewString(midiPortName);
    Devices.MetaDeviceStatus = Device_Unitialised;
    Tracks.RecordDevice = -1;
    Tracks.EditDevice = -1;
    Tracks.Track = NULL;

    if (!(Mapper_DeviceQuery()))
    {
        /* no devices found - return devices as unavailable */
        Devices.FileDescriptor = XtNewString("/dev/null");
        Mapper_SetTrackInfo();
        RETURN_BOOL(False);
    }

    for (i = 0; (i < Devices.MaxDevices) && (Tracks.RecordDevice==-1); i++)
    {
        if (Mapper_GetDevice(i)->Device.Type == Mapper_Midi_Device)
            Tracks.RecordDevice = i;
    }

    /* the record and edit devices are global constants
       above the dynamic track info - init them here */
    Mapper_SetTrackInfo();
    Mapper_LoadPatches();  /* init the hardware */

RETURN_BOOL(True);
}

int
Mapper_QueueEvent(EventList NextEvent, int *LastTime, float *PlayTime,
              unsigned int StartLastTime, float StartPlayTime, float TimeInc)
{
float DeltaTime;
BEGIN("Mapper_QueueEvent");

    DeltaTime = ( (float)( NextEvent->Event.DeltaTime )
                                - (float)(*LastTime) ) * TimeInc;
    (*PlayTime) += DeltaTime;

    SEQ_WAIT_TIME( (int)(*PlayTime) - (int)StartPlayTime );
    SEQ_DUMPBUF();

    (*LastTime) = NextEvent->Event.DeltaTime;

RETURN_INT((int)NextEvent->Event.DeltaTime);
}

void
Mapper_InitVoiceAllocations()
{
    int i;

BEGIN("Mapper_InitVoiceAllocations");

    for ( i = 0; i < Devices.ActiveDevices; i++ )
    {
        if (Mapper_GetActiveDevice(i)->Device.Type == Mapper_Synth_Device)
            Mapper_WriteSynthEvent(NULL, Mapper_GetActiveDevice(i)->
                                              Device.Data.Synth.device);
    }
END;
}

void
Mapper_Reset()
{
int i;
BEGIN("Mapper_Reset");

    for ( i = 0; i < Devices.ActiveDevices; i++ )
        ioctl(seqfd, SNDCTL_SEQ_RESET, Mapper_GetActiveDevice(i)->
                                              Device.Data.Midi.device);
END;
}
void
Mapper_Initialize()
{
BEGIN("Mapper_Initialize");

    Mapper_Reset();
    Mapper_InitVoiceAllocations();

END;
}


/*
 * Mapper_ReadEvent
 */
Boolean
Mapper_ReadEvent(MIDIRawEventBuffer ReturnEvent)
{
    static byte LastStatus   = 0x90;
    static byte NumOperands  = 0;
    static byte OperandsLeft = 0;

    unsigned char InBytes[4];
    int out, pass;
    unsigned long int StartTime = 0;

BEGIN("Mapper_ReadEvent");

    if ( ( out = read(seqfd, &InBytes, sizeof(InBytes)) ) <= 0 )
        RETURN_BOOL(False);

    /* ensure that we read all pending "read" events */
    for ( pass = 0; pass < (out/sizeof(InBytes)); pass++)
    {
        /* check the API's returned event type */
        switch ( InBytes[0] )
        {
            case SEQ_WAIT:
               /* for the moment we'll just start recording from
                  a normalised "zero" */

                ReturnEvent->Time = ((InBytes[3]<<16)|(InBytes[2]<<8)|
                                      InBytes[1]);

                break;


            case SEQ_ECHO:
                /* no echo events yet defined */
#ifdef RECORD_DEBUG
                fprintf(stderr,"ECHO EVENT\n");
#endif
                break;

            case SEQ_MIDIPUTC:
                if (IsStatusByte(InBytes[1]))
                {
                    if (MessageType(InBytes[1]) == MIDI_SYSTEM_MSG)
                    {
                        NumOperands = 0; /* no timing info */
                        fprintf(stderr, "SYSTEM MESSAGE\n");
                    }
                    else if (MessageType(InBytes[1]) == MIDI_CTRL_CHANGE ||
                               MessageType(InBytes[1]) == MIDI_PROG_CHANGE)
                    {
                        NumOperands = 1;
                    }
                    else
                    {
                        NumOperands = 2;
                    }
                    LastStatus = InBytes[1];
                    OperandsLeft = NumOperands;
                }

                if (OperandsLeft)
                {
                    ReturnEvent->Bytes[0] = LastStatus;

                    if (!IsStatusByte(InBytes[1]))
                    {
                        ReturnEvent->Bytes[(OperandsLeft == 2) ? 1 : 2 ]
                                = InBytes[1];

                        --OperandsLeft;
                    }

                    if ( !OperandsLeft )
                    {
                        /* if no status byte then just collect another
                           event of the same type */

#ifdef RECORD_DEBUG
                            fprintf(stderr, "MIDI EVENT : %ld %x %x %x\n",
                                        ReturnEvent->Time,
                                        (int)ReturnEvent->Bytes[0],
                                        (int)ReturnEvent->Bytes[1],
                                        (int)ReturnEvent->Bytes[2]);
#endif

                        if (!IsStatusByte(InBytes[1]))
                        {
                            OperandsLeft = 2;
                        }

                        RETURN_BOOL(True);
                    }
                }
            default:
                break;
        }
    }
RETURN_BOOL(False);
}

void
Mapper_GetPendingEvents(MIDIRawEventBuffer ReturnEvent)
{
BEGIN("Mapper_GetPendingEvents");

/*
    if ( ( out = read(seqfd, &InBytes, sizeof(InBytes)) ) <= 0 )
        END;
*/

END;
}

void
Mapper_OpenActiveDevices()
{
  BEGIN("Mapper_OpenActiveDevices");
  /* do nothing */
  END;
}

void
Mapper_CloseActiveDevices()
{
  BEGIN("Mapper_CloseActiveDevices");
  /* do nothing */
  END;
}
