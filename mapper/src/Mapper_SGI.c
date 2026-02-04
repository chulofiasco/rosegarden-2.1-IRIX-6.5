
/* Mapper class for SGI IRIX MIDI API, Chris Cannam October 1997 */

#undef POSIX_PLEASE
#include <SysDeps.h>
#undef _POSIX_SOURCE
#include <Debug.h>

#include <stdio.h>
#include <sys/bsd_types.h>
#include <sys/types.h>
#include <dmedia/midi.h>
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
  mdSetDivision(info->MidiPort, MIDIHeaderBuffer.Timing.Division); /*!!!*/
  mdSetTempo(info->MidiPort, 500000);
  mdSetOrigin(info->MidiPort, 0);

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


static Boolean Mapper_UnprepareDevice(DeviceList device)
{
  DeviceInformation *info = &device->Device;
  BEGIN("Mapper_UnprepareDevice");

  if (info->Type != Mapper_Midi_Device) RETURN_BOOL(True);

  if ((info->MidiPort == 0) || (info->MidiPortStatus == SGIMidiPortClosed))
    RETURN_BOOL(True);		/* already unready */

  if (info->MidiPortStatus != SGIMidiPortFailed) {

    if (info->MidiPortStatus == SGIMidiPortOpenOut) { /* shut it up */
      /*      Mapper_SilenceOutDevice(device);*/
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
      ev.msg[0] = status;
      ev.msglen = 0;

      if (!Mapper_PrepareOutDevice(device)) continue; /* device is broken */

      mdSetTempo(device->Device.MidiPort, Tempo);

      Mapper_UpdateTimerOffset(NextEvent->DeltaTime);

      if (MessageType(status) == MIDI_CTRL_CHANGE ||
	  MessageType(status) == MIDI_PROG_CHANGE) {

	if (delta < Mapper_GetTimerOffset()) ev.stamp = 0;
	else ev.stamp = delta - Mapper_GetTimerOffset();

	ev.msg[1] = evData->ProgramChange.Program;
	
      } else {
	
	if (delta < Mapper_GetTimerOffset()) { /* as will happen with rewind */
	  Mapper_SetTimerOffset(delta);
	  mdSetOrigin(device->Device.MidiPort, 0);
	}

	ev.stamp = delta - Mapper_GetTimerOffset();

	fprintf(stderr, "Adjusted stamp: %lld\n", ev.stamp);

	if (MidiPortSync) {

	  long long tell;
	  long uSleep;

	  if ((tell = mdTellNow(device->Device.MidiPort)) < ev.stamp &&
	      (uSleep = mdTicksToNanos
	       (device->Device.MidiPort, ev.stamp - tell) / 1000) > 600000) {
	    usleep(uSleep - 500000);
	  }
	}

	ev.msg[1] = evData->NoteOn.Note;
	ev.msg[2] = evData->NoteOn.Velocity;
      }

      if (mdSend(device->Device.MidiPort, &ev, 1) <= 0) {
	perror("mdSend");
	if (errno != EWOULDBLOCK) {
	  (void)Mapper_UnprepareDevice(device);
	  device->Device.MidiPortStatus = SGIMidiPortFailed;
	}
	continue;
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

    ReturnEvent->Time = inEv.stamp;
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

  /* I take it this means it's already open? */
  if (Devices.MetaDeviceStatus == Sense) RETURN_BOOL(True);

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

  /* We should probably close all MIDI devices here?  let's try that */

  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    if (dList->Device.Type == Mapper_Midi_Device) {
      if (Mapper_PrepareOutDevice(dList)) Mapper_SilenceOutDevice(dList);
    }
    (void)Mapper_UnprepareDevice(dList); /* can't do owt about failure */
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
  int DeltaTime;
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

  Mapper_Reset();

  END;
}


void Mapper_InitVoiceAllocations()
{
  BEGIN("Mapper_InitVoiceAllocations");

  /* don't need this for SGI */

  END;
}

void Mapper_Reset()
{
  DeviceList dList;
  BEGIN("Mapper_Reset");

  /* and why not? */

  for (dList = Devices.Device; dList; dList = (DeviceList)Next(dList)) {
    (void)Mapper_UnprepareDevice(dList); /* can't do owt about failure */
  }

  END;
}

