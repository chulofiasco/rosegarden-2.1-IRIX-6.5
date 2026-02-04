
/* Mapper class for SGI IRIX MIDI API, Chris Cannam October 1997 */

#undef POSIX_PLEASE
#include <SysDeps.h>
#undef _POSIX_SOURCE
#include <Debug.h>

#include <stdio.h>
#include <sys/bsd_types.h>
#include <sys/types.h>
#include <dmedia/midi.h>
#include <dmedia/dmedia.h>
#include <dmedia/audio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#include <Mapper.h>
#include <MidiErrorHandler.h>
#include <MidiEvent.h>

#include "../../sequencer/src/Globals.h" /* bleah! */


TrackMetaInfoElement  Tracks;
DeviceMetaInfoElement Devices;
int TracksOnDevice[Mapper_Devices_Supported][Mapper_Max_Tracks];

extern Boolean MidiMaintainTempo;
extern Boolean MidiPortSync;

extern long RecordTempo;
unsigned int InitialTempo = 500000;  /* Initial tempo from loaded MIDI file */


Boolean Mapper_SetupDevices(char *midiPortName)
{
  int i;
  DeviceList dList;
  BEGIN("Mapper_SetupDevices");

  /* midiPortName has no relevance for the system MIDI API */

  Devices.Device = NULL;
  Devices.FileDescriptor = XtNewString("System MIDI");
  Devices.MetaDeviceStatus = Device_Unitialised;
  Tracks.EditDevice = -1;
  Tracks.RecordDevice = -1;
  Tracks.Track = NULL;

  Devices.MaxDevices = mdInit();

  if (Devices.MaxDevices == 0) {

    fprintf(stderr, "Mapper: No devices found for %s interface!\n",
	    Devices.FileDescriptor);

    XtFree(Devices.FileDescriptor);
    Devices.FileDescriptor = XtNewString("No Devices");
    Mapper_SetTrackInfo();
    RETURN_BOOL(False);

  } else {

    (void)Mapper_DeviceQuery();

    /* choose a Record device -- first one apparently not internal */

    for (dList = Devices.Device, i = 0, Tracks.RecordDevice = 0; dList;
	 dList = (DeviceList)Next(dList), ++i) {
      if (dList->Device.Type == Mapper_Midi_Device &&
	  strcmp(dList->Device.Data.Midi.name, "internal") &&
	  strcmp(dList->Device.Data.Midi.name, "Software Synth")) {
	Tracks.RecordDevice = i; break;
      }
    }
  }

  Mapper_SetTrackInfo();
  Mapper_LoadPatches();		/* not that we need to */

  RETURN_BOOL(True);
}


void Mapper_CreateAllDevice()
{
    DeviceInformation tempDevice;
    DeviceList current = NULL;

    BEGIN("Mapper_CreateAllDevice");

    if (Devices.MaxDevices <= 1) END;

    tempDevice.Type = Mapper_All_Device;
    tempDevice.Data.Midi.name = XtNewString(Mapper_All_Device_Label);

    Devices.Device = (DeviceList)First
      (Nconc(Devices.Device, Mapper_NewDeviceList(tempDevice,
						  Devices.MaxDevices)));

    Devices.MaxDevices++;

    END;
}


DeviceList Mapper_DeviceQuery(void)
{
    int i;
    char *name;
    DeviceInformation tempDevice;

    BEGIN("Mapper_DeviceQuery");

    /* ei du not understend */
/*    if (Mapper_ManageDevices(Device_Active_WO) != True) RETURN_PTR(NULL);  */

    /* Devices.MaxDevices has already been set in SetupDevices().  We   */
    /* can't change the number of devices part-way on the SGI, or if we */
    /* can I don't know how (can one call mdInit() more than once?)     */

    Devices.ActiveDevices = 0;
    if (Devices.Device || Devices.MaxDevices == 0) RETURN_PTR(Devices.Device);

    for (i = 0; i < Devices.MaxDevices; ++i) {

      name = mdGetName(i);
      tempDevice.Data.Midi.name = name ? XtNewString(name) : NULL;
      tempDevice.MidiPort = NULL;
      tempDevice.MidiPortStatus = SGIMidiPortClosed;
      tempDevice.Type = Mapper_Midi_Device;

      Devices.Device = (DeviceList)First
	(Nconc(Devices.Device, Mapper_NewDeviceList(tempDevice, i)));
    }

    Mapper_CloseDevice();
    Mapper_CreateAllDevice(); /* create the "All" Device */

    RETURN_PTR(Devices.Device);
}


static Boolean Mapper_UnprepareDevice(DeviceList);

static Boolean Mapper_PrepareOutDevice(DeviceList device)
{
  DeviceInformation *info = &device->Device;
  BEGIN("Mapper_PrepareOutDevice");

  if (info->Type != Mapper_Midi_Device) RETURN_BOOL(True);

  if ((info->MidiPort != 0) && (info->MidiPortStatus == SGIMidiPortOpenOut))
    RETURN_BOOL(True);		/* already ready */

  if (info->MidiPortStatus == SGIMidiPortFailed ||
      info->MidiPortStatus == SGIMidiPortOpenOut) RETURN_BOOL(False);

  if (info->MidiPortStatus == SGIMidiPortOpenIn) {
    (void)Mapper_UnprepareDevice(device);
  }
  
  info->MidiPort = mdOpenOutPort(info->Data.Midi.name);

  if (info->MidiPort == NULL) {
    Error(NON_FATAL_REPORT_TO_MSGBOX, "Unable to open midi port!\n");
    info->MidiPortStatus = SGIMidiPortFailed;
    RETURN_BOOL(False);
  }

  mdSetStampMode(info->MidiPort, MD_RELATIVETICKS);
  /* Division, tempo, and start point will be set when file loads */
  
  info->MidiPortStatus = SGIMidiPortOpenOut;

  RETURN_BOOL(True);
}


static Boolean Mapper_PrepareInDevice(DeviceList device)
{
  DeviceInformation *info = &device->Device;
  BEGIN("Mapper_PrepareInDevice");
  
  if (info->Type != Mapper_Midi_Device) RETURN_BOOL(True);

  if ((info->MidiPort != 0) && (info->MidiPortStatus == SGIMidiPortOpenIn))
    RETURN_BOOL(True);		/* already ready */

  if (info->MidiPortStatus == SGIMidiPortFailed ||
      info->MidiPortStatus == SGIMidiPortOpenIn) RETURN_BOOL(False);

  if (info->MidiPortStatus == SGIMidiPortOpenOut) {
    (void)Mapper_UnprepareDevice(device);
  }
  
  info->MidiPort = mdOpenInPort(info->Data.Midi.name);

  if (info->MidiPort == NULL) {
    Error(NON_FATAL_REPORT_TO_MSGBOX, "Unable to open midi in port!\n");
    info->MidiPortStatus = SGIMidiPortFailed;
    RETURN_BOOL(False);
  }

  mdSetStampMode(info->MidiPort, MD_RELATIVETICKS);
  mdSetDivision(info->MidiPort, 480); /*!!!*/
  mdSetTempo(info->MidiPort, RecordTempo);
  mdSetOrigin(info->MidiPort, 0);

  info->MidiPortStatus = SGIMidiPortOpenIn;

  RETURN_BOOL(True);
}


/* use before closing the application only: */

static void Mapper_SilenceOutDevice(DeviceList device)
{
  MDevent ev;
  unsigned long long stamp = 0;
  int channel, note, byte;
  char data[] = { MD_RESETALLCONTROLLERS, MD_ALLNOTESOFF, MD_ALLSOUNDOFF };
  DeviceInformation *info = &device->Device;
  BEGIN("Mapper_SilenceOutDevice");
  
  if (info->Type != Mapper_Midi_Device || info->MidiPort == 0 ||
      info->MidiPortStatus != SGIMidiPortOpenOut) END;

  (void)mdPause(info->MidiPort);
  
  mdSetDivision(info->MidiPort, 1);
  mdSetTempo(info->MidiPort, 1000);
  mdSetOrigin(info->MidiPort, 0);

  for (channel = 0; channel < 16; ++channel) {
    for (note = 0; note < 128; ++note) {
      ev.stamp = ++stamp;
      ev.msg[0] = MD_NOTEOFF;
      ev.msg[1] = note;
      ev.msg[2] = 0;
      mdSend(info->MidiPort, &ev, 1);
     }
  }

  for (channel = 0; channel < 16; ++channel) {
    for (byte = 0; byte < 3; ++byte) {
      ev.stamp ++;
      ev.msg[0] = MD_CONTROLCHANGE | channel;
      ev.msg[1] = data[byte];
      ev.msg[2] = 0;
      mdSend(info->MidiPort, &ev, 1);
    }
  }

  while (mdTellNow(info->MidiPort) <= stamp);

  END;
}


/* Set stamp mode for all active output devices */
void Mapper_SetStampModeAllDevices(int mode)
{
  int i;
  DeviceList device;
  DeviceInformation *info;
  int stamp_mode = (mode == 0) ? MD_NOSTAMP : MD_RELATIVETICKS;
  
  BEGIN("Mapper_SetStampModeAllDevices");
  
  for (i = 0; i < Devices.ActiveDevices; i++) {
    device = Mapper_GetActiveDevice(i);
    if (device == NULL) continue;
    
    info = &device->Device;
    if (info->Type != Mapper_Midi_Device || info->MidiPort == 0 ||
        info->MidiPortStatus != SGIMidiPortOpenOut) continue;
    
    mdSetStampMode(info->MidiPort, stamp_mode);
  }
  
  END;
}


static Boolean Mapper_UnprepareDevice(DeviceList device)
{
  DeviceInformation *info = &device->Device;
  BEGIN("Mapper_UnprepareDevice");

  if (info->Type != Mapper_Midi_Device) RETURN_BOOL(True);

  if ((info->MidiPort == 0) || (info->MidiPortStatus == SGIMidiPortClosed))
    RETURN_BOOL(True);		/* already unready */

  if (info->MidiPortStatus != SGIMidiPortFailed) {

    if (info->MidiPortStatus == SGIMidiPortOpenOut) {
      /* Wait for buffer to drain before closing to avoid blocking */
      long long tell, last_tell = -1;
      int stall_count = 0;
      
      /* Poll until buffer is empty or stalled for 1 second */
      while (stall_count < 100) {  /* 100 * 10ms = 1 second max */
        tell = mdTellNow(info->MidiPort);
        if (tell < 0) break;  /* Error getting position */
        
        if (tell == last_tell) {
          stall_count++;
          usleep(10000);  /* Wait 10ms */
        } else {
          stall_count = 0;
          last_tell = tell;
          usleep(10000);  /* Wait 10ms */
        }
      }
    }

    if (mdClosePort(info->MidiPort)) {
      perror("Can't close MIDI port");
      info->MidiPortStatus = SGIMidiPortFailed;
      info->MidiPort = 0;
      RETURN_BOOL(False);
    }
  }

  info->MidiPortStatus = SGIMidiPortClosed;
  info->MidiPort = 0;
  RETURN_BOOL(True);
}


void Mapper_OpenActiveDevices()
{
  DeviceList device;
  BEGIN("Mapper_OpenActiveDevices");

  for (device = Devices.Device; device; device = (DeviceList)Next(device)) {
    if (device->TotalTracksOnDevice > 0) Mapper_PrepareOutDevice(device);
  }

  END;
}


void Mapper_CloseActiveDevices()
{
  DeviceList device;
  BEGIN("Mapper_OpenActiveDevices");

  for (device = Devices.Device; device; device = (DeviceList)Next(device)) {
    Mapper_UnprepareDevice(device);
  }

  END;
}  


extern void Mapper_RewindTimer();
extern void Mapper_UpdateTimerOffset(int);
extern int  Mapper_GetTimerOffset();
static long Tempo = 500000;

void Mapper_WriteEvent(MIDIEvent NextEvent, int dno)
{
  int i = 0;
  DeviceList device;
  Boolean playAll = False;
  BEGIN("Mapper_WriteEvent");

  device = Mapper_GetActiveDevice(dno);
  if (device->Device.Type == Mapper_All_Device) playAll = True;

  do {

    if (playAll) device = Mapper_GetDevice(i++);

    if (device->Device.Type == Mapper_Midi_Device) {

      MDevent ev;		/* let's timidly send only one at a time */
      MIDIEventUnion *evData = &NextEvent->EventData;
      unsigned char status = NextEvent->EventCode;
      int delta = NextEvent->DeltaTime;
      unsigned char channel = status & 0x0F;
      ev.msg[0] = status;  /* Will be overridden for specific message types */
      ev.msglen = 0;

      if (!Mapper_PrepareOutDevice(device)) continue; /* device is broken */

      /* Don't call mdSetTempo on every event - it was set at port open */
      /* mdSetTempo(device->Device.MidiPort, Tempo); */

      Mapper_UpdateTimerOffset(NextEvent->DeltaTime);

      if (status == MIDI_SYSTEM_EXCLUSIVE && 
          evData->MetaEvent.MetaEventCode == MIDI_SET_TEMPO) {
        /* Handle tempo meta events - send as MD_META to hardware */
        static unsigned char sysex[6];
        sysex[0] = 0xFF;  /* MD_META */
        sysex[1] = 0x51;  /* Set Tempo */
        sysex[2] = 0x03;  /* Length */
        sysex[3] = evData->MetaEvent.Bytes;
        sysex[4] = *(&evData->MetaEvent.Bytes + 1);
        sysex[5] = *(&evData->MetaEvent.Bytes + 2);
        
        ev.msg[0] = 0xFF;  /* MD_META */
        ev.sysexmsg = sysex;
        ev.stamp = (delta < Mapper_GetTimerOffset()) ? 0 : delta - Mapper_GetTimerOffset();
        ev.msglen = 6;
        
        mdSend(device->Device.MidiPort, &ev, 1);
        continue;  /* Done with this event */
      }

      if (MessageType(status) == MIDI_CTRL_CHANGE ||
	  MessageType(status) == MIDI_PROG_CHANGE) {

	/* During initialization, send immediately; during playback, timestamp appropriately */
	if (delta == 0 || delta < Mapper_GetTimerOffset()) {
	  ev.stamp = 0;  /* Send immediately */
	} else {
	  ev.stamp = delta - Mapper_GetTimerOffset();
	}

	/* Set message data based on type */
	/* Use MD_PROGRAMCHANGE/MD_CONTROLCHANGE constants like SGI sample code */
	if (MessageType(status) == MIDI_CTRL_CHANGE) {
	  ev.msg[0] = MD_CONTROLCHANGE | channel;  /* Channel ORed into msg[0] */
	  ev.msg[1] = evData->ControlChange.Controller;
	  ev.msg[2] = evData->ControlChange.Value;
	} else {  /* MIDI_PROG_CHANGE */
	  ev.msg[0] = MD_PROGRAMCHANGE | channel;  /* Channel ORed into msg[0] */
	  ev.msg[1] = evData->ProgramChange.Program;
	}
	/* msglen stays 0 for standard MIDI messages */
	
      } else {
	
	if (delta < Mapper_GetTimerOffset()) { /* as will happen with rewind */
	  Mapper_SetTimerOffset(delta);
	  /* Don't reset origin during playback - breaks timing */
	  /* mdSetOrigin(device->Device.MidiPort, 0); */
	}

	ev.stamp = delta - Mapper_GetTimerOffset();

	/* Use MD_NOTEON/MD_NOTEOFF constants like SGI sample code */
	if (MessageType(status) == MIDI_NOTE_ON) {
	  /* NOTE_ON with velocity 0 is actually a NOTE_OFF */
	  if (evData->NoteOn.Velocity == 0) {
	    ev.msg[0] = MD_NOTEOFF | channel;
	  } else {
	    ev.msg[0] = MD_NOTEON | channel;
	  }
	} else if (MessageType(status) == MIDI_NOTE_OFF) {
	  ev.msg[0] = MD_NOTEOFF | channel;
	}
	
	ev.msg[1] = evData->NoteOn.Note;
	ev.msg[2] = evData->NoteOn.Velocity;
      }

      /* MidiPortSync: Always check buffer status before sending to prevent blocking */
      if (MidiPortSync) {
	long long tell = mdTellNow(device->Device.MidiPort);
	
	/* Only proceed if we can get current playback position */
	if (tell >= 0) {
	  /* For timestamped events, check if we're too far ahead */
	  if (ev.stamp > 0 && tell < ev.stamp) {
	    long long gap_us = mdTicksToNanos(device->Device.MidiPort, 
	                                       ev.stamp - tell) / 1000;
	    
	    /* Sleep in small increments to stay responsive */
	    while (gap_us > 1500000) {
	      /* More than 1.5s ahead - sleep for 20ms and recheck */
	      usleep(20000);
	      tell = mdTellNow(device->Device.MidiPort);
	      if (tell < 0 || tell >= ev.stamp) break;
	      gap_us = mdTicksToNanos(device->Device.MidiPort, 
	                               ev.stamp - tell) / 1000;
	    }
	    
	    if (gap_us > 500000) {
	      /* 500ms-1.5s ahead - brief yield */
	      sginap(1);
	    }
	    /* Less than 500ms - send immediately */
	  }
	}
      }

      {
        int result = mdSend(device->Device.MidiPort, &ev, 1);
        int retry_count = 0;
        while (result <= 0) {
          if (errno == EWOULDBLOCK) {
            if (++retry_count > 100) {
              /* If still blocking after 100 retries (~1s), give up to avoid freeze */
              fprintf(stderr, "mdSend: EWOULDBLOCK timeout after 100 retries\n");
              break;
            }
            sginap(1);  /* yield CPU and retry */
            result = mdSend(device->Device.MidiPort, &ev, 1);
            continue;
          }
          perror("mdSend");
          (void)Mapper_UnprepareDevice(device);
          device->Device.MidiPortStatus = SGIMidiPortFailed;
          break;
        }
      }

      if (ev.msglen > 0) mdFree(ev.sysexmsg);
    }

  } while (playAll && (i < Devices.MaxDevices-1));

  END;
}


Boolean Mapper_ReadEvent(MIDIRawEventBuffer ReturnEvent)
{
  fd_set readfds;
  DeviceList device;
  DeviceInformation *info;
  struct timeval timeout;
  int i, rtn, fd, opsLeft;
  MDevent inEv;
  unsigned char *bytePtr;
  static int complainedDevice = -1;
  BEGIN("Mapper_ReadEvent");

  if (Tracks.RecordDevice < 0) return False;
  device = Mapper_GetDevice(Tracks.RecordDevice);

  if (device->Device.Type != Mapper_Midi_Device) {
    fprintf(stderr, "Mapper: Record device is not a MIDI device!\n");
    Tracks.RecordDevice = -1;
    return False;
  }

  if (device->Device.MidiPortStatus == SGIMidiPortOpenOut) {
    if (complainedDevice != Tracks.RecordDevice) {
      fprintf(stderr,
	      "Mapper: Can't read from port %s while I'm writing to it\n",
	      device->Device.Data.Midi.name);
    }
    complainedDevice = Tracks.RecordDevice;
    return False;
  }

  if (!Mapper_PrepareInDevice(device)) {
    fprintf(stderr, "Mapper: Record device failed!\n");
    Tracks.RecordDevice = -1;
    return False;
  }

  info = &device->Device;
  fd = mdGetFd(info->MidiPort);

  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  timeout.tv_sec = timeout.tv_usec = 0;

  if ((rtn = select(fd + 1, &readfds, NULL, NULL, &timeout)) > 0) {
    
    if (mdReceive(info->MidiPort, &inEv, 1) <= 0) RETURN_BOOL(False);

    /* we aren't handling sysex messages correctly -- in fact we
       aren't going to handle them at all, for now */

    if ((inEv.msg[0] & 0xf0) == 0xf0) {	/* see? */
      mdFree(inEv.sysexmsg);
      RETURN_BOOL(False);
    }

    /* Truncate 64-bit timestamp to 32-bit - limits to ~4.2 billion ticks */
    ReturnEvent->Time = (unsigned long)inEv.stamp;
    ReturnEvent->Bytes[0] = inEv.msg[0];
    opsLeft = mdMsgLen(inEv.msg[0]) - 1;
    bytePtr = &ReturnEvent->Bytes[1];

    i = 0;
    while (opsLeft) {
      *bytePtr++ = inEv.msg[i++];
      --opsLeft;
    }

    if (((ReturnEvent->Bytes[0] & 0xf0) == MD_NOTEON) &&
	(ReturnEvent->Bytes[2] == 0)) {	/* cast Note On, vel. 0, to Note Off */
      ReturnEvent->Bytes[0] = (ReturnEvent->Bytes[0] & 0x0f) | MD_NOTEOFF;
    }

    RETURN_BOOL(True);
  }

  RETURN_BOOL(False);
}


Boolean Mapper_OpenDevice(int Sense, char *Device)
{
  BEGIN("Mapper_OpenDevice");

  /* Don't skip if opening for write - we need to reinitialize */
  if (!(Sense & Device_Active_WO)) {
    /* I take it this means it's already open? */
    if (Devices.MetaDeviceStatus == Sense) RETURN_BOOL(True);
  }

  if (Devices.MaxDevices == 0) {
    Error(NON_FATAL_REPORT_TO_MSGBOX,
	  "Unable to initialise midi: No devices available\n");
    RETURN_BOOL(False);
  }

  /* There is nothing we can do here that makes any sense -- the
     "device" argument refers to /dev/sequencer in OSS terms, and
     that's already been opened by mdInit() and will remain open */

  RETURN_BOOL(True);
}


void Mapper_CloseDevice()
{
  DeviceList dList;
  BEGIN("Mapper_CloseDevice");

  /* Silence devices but don't close ports - mdClosePort() blocks waiting for buffer drain */

  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    if (dList->Device.Type == Mapper_Midi_Device) {
      if (Mapper_PrepareOutDevice(dList)) Mapper_SilenceOutDevice(dList);
    }
    /* Don't call Mapper_UnprepareDevice - keeps ports open */
  }

  END;
}


void Mapper_LoadPatches(void)
{
  BEGIN("Mapper_LoadPatches");

  /* do nothing */

  END;
}


int Mapper_QueueEvent(EventList NextEvent, int *LastTime, float *PlayTime,
		      unsigned int StartLastTime, float StartPlayTime,
		      float TimeInc)
{
  float DeltaTime;  /* Changed from int to preserve fractional centiseconds
                     * With low-res files (div=96), TimeInc=0.52, so 1-tick
                     * gaps truncated to 0, breaking PlayTime counter */
  BEGIN("Mapper_QueueEvent");

  DeltaTime = ((NextEvent->Event.DeltaTime) - (*LastTime)) * TimeInc;
  (*PlayTime) += DeltaTime;  
  (*LastTime) = NextEvent->Event.DeltaTime;

  /* The port tempo (usec/beat) is the division * the tick length (usec).
     Our TimeInc argument is the tick length in 1/100s units, so we need
     to multiply by 10000 and by the division to get the tempo */

  Tempo = (long)((TimeInc * 10000.0) * MIDIHeaderBuffer.Timing.Division);
  
  RETURN_INT((int)NextEvent->Event.DeltaTime);
}


void Mapper_FlushQueue(float FinishTime)
{
  /*  long timeInc, finishTick; */
  BEGIN("Mapper_FlushQueue");

  usleep(500000); /* we're just waiting for some note-off events, all
                     timestamped "now", to be cleared out */

  END;
}


void Mapper_Initialize(void)
{
  BEGIN("Mapper_Initialize");

  /* do you know, I don't think I really want to initialise
     anything at the moment.  It might make sense for this to
     call PrepareDevice on all Active Devices, though. */

  END;
}

void Mapper_ReinitializeForPlayback(void)
{
  DeviceList dList;
  unsigned long long now;
  
  BEGIN("Mapper_ReinitializeForPlayback");

  /* Set start point FAR in the future - 5 seconds */
  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    if (dList->Device.Type == Mapper_Midi_Device && 
        dList->Device.MidiPort != NULL) {
      
      dmGetUST(&now);
      mdSetStartPoint(dList->Device.MidiPort, (long long)now + 5000000, 0);
      mdSetDivision(dList->Device.MidiPort, MIDIHeaderBuffer.Timing.Division);
      mdSetTempo(dList->Device.MidiPort, InitialTempo);
      mdSetTemposcale(dList->Device.MidiPort, 1.0);
      
      dList->Device.MidiPortStatus = SGIMidiPortOpenOut;
    }
  }

  END;
}

void Mapper_UpdatePortSettings(void)
{
  DeviceList dList;
  BEGIN("Mapper_UpdatePortSettings");

  /* Reset port timing parameters aggressively */
  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    if (dList->Device.MidiPort != NULL) {
      mdSetOrigin(dList->Device.MidiPort, 0);
      mdSetDivision(dList->Device.MidiPort, MIDIHeaderBuffer.Timing.Division);
      mdSetTempo(dList->Device.MidiPort, 500000);
    }
  }

  END;
}

void Mapper_ReinitializeDevices(void)
{
  BEGIN("Mapper_ReinitializeDevices");

  /* Just send MIDI reset without closing/reopening ports */
  /* Closing ports via mdClosePort() blocks waiting for buffer drain */
  Mapper_Reset();

  END;
}




void Mapper_InitVoiceAllocations()
{
  BEGIN("Mapper_InitVoiceAllocations");

  /* don't need this for SGI */

  END;
}

void Mapper_AllNotesOff()
{
  DeviceList dList;
  int chan;
  MDevent ev;
  
  BEGIN("Mapper_AllNotesOff");

  /* Send All Notes Off on all channels to all devices */
  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    if (dList->Device.MidiPort != NULL) {
      for (chan = 0; chan < 16; chan++) {
        /* All Notes Off (CC 123) */
        ev.msg[0] = MIDI_CTRL_CHANGE | chan;
        ev.msg[1] = 123;
        ev.msg[2] = 0;
        ev.msglen = 0;
        ev.stamp = 0;
        mdSend(dList->Device.MidiPort, &ev, 1);
      }
    }
  }

  END;
}

void Mapper_Reset()
{
  DeviceList dList;
  int chan;
  MDevent ev;
  unsigned char gmReset[] = { 0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7 }; /* GM System On */
  
  BEGIN("Mapper_Reset");

  /* Send GM System Reset to all devices */
  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    if (dList->Device.MidiPort != NULL) {
      ev.msg[0] = 0xF0;  /* SysEx start */
      ev.msglen = 6;
      ev.sysexmsg = gmReset;
      ev.stamp = 0;
      mdSend(dList->Device.MidiPort, &ev, 1);
      usleep(50000);  /* wait 50ms for reset to complete */
      
      /* Send All Notes Off and Reset All Controllers on all channels */
      for (chan = 0; chan < 16; chan++) {
        /* All Notes Off (CC 123) */
        ev.msg[0] = MIDI_CTRL_CHANGE | chan;
        ev.msg[1] = 123;
        ev.msg[2] = 0;
        ev.msglen = 0;
        ev.stamp = 0;
        mdSend(dList->Device.MidiPort, &ev, 1);
        
        /* Reset All Controllers (CC 121) */
        ev.msg[1] = 121;
        mdSend(dList->Device.MidiPort, &ev, 1);
      }
    }
  }

  /* Don't close devices - they stay open for next playback session */
  /* Closing ports via mdClosePort() blocks waiting for buffer drain */

  END;
}

