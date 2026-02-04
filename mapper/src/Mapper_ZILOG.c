
/* Mapper class for SGI ZILOG output controller, Chris Cannam October 1997  */
/* This is pretty poor, low-resolution I/O (if it works at all).  Don't use */
/* it if your system will support Mapper_SGI instead.                       */

#undef POSIX_PLEASE
#include <SysDeps.h>
#undef _POSIX_SOURCE
#include <Debug.h>

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/signal.h>

#include <sys/termio.h>
#include <sys/z8530.h>
#include <sys/stropts.h>

#include <Mapper.h>
#include <MidiErrorHandler.h>
#include <MidiEvent.h>

#include "../../sequencer/src/Globals.h" /* bleah! */

TrackMetaInfoElement  Tracks;
DeviceMetaInfoElement Devices;
int TracksOnDevice[Mapper_Devices_Supported][Mapper_Max_Tracks];

extern Boolean MidiMaintainTempo;
extern Boolean MidiPortSync;

static char *globalMidiPortName = 0;
static clock_t recordStartTime = 0;


Boolean
Mapper_SetupDevices(char *midiPortName)
{
  BEGIN("Mapper_SetupDevices");

  Devices.Device = NULL;
  Devices.FileDescriptor = XtNewString(midiPortName);
  Devices.MetaDeviceStatus = Device_Unitialised;
  Tracks.EditDevice = -1;
  Tracks.RecordDevice = -1;
  Tracks.Track = NULL;

  Devices.MaxDevices = 1;
  globalMidiPortName = XtNewString(midiPortName);

  if (Mapper_DeviceQuery() == NULL) RETURN_BOOL(False);

  Tracks.RecordDevice = 0;

  Mapper_SetTrackInfo();
  Mapper_LoadPatches();		/* not that we need to */

  RETURN_BOOL(True);
}

DeviceList
Mapper_DeviceQuery(void)
{
  DeviceInformation tempDevice;
  BEGIN("Mapper_DeviceQuery");

  Devices.ActiveDevices = 0;
  if (Devices.Device || Devices.MaxDevices == 0) RETURN_PTR(Devices.Device);
  tempDevice.Data.Midi.name = XtNewString(globalMidiPortName);
  tempDevice.Type = Mapper_Midi_Device;
  tempDevice.fd = -1;

  Devices.Device = (DeviceList)First
    (Nconc(Devices.Device, Mapper_NewDeviceList(tempDevice, 0)));
  Devices.MaxDevices = 1;

  RETURN_PTR(Devices.Device);
}

Boolean
Mapper_OpenDevice(int Sense, char *mainDeviceName)
{
  char              ErrBuff[128];
  struct termio     t;
  struct strioctl   str;
  int               arg;
  int               OutputFlag;
  int               fd;
  struct tms        garbage;
  BEGIN("Mapper_OpenDevice");

  /* we have no idea whether we're about to record, but it doesn't
     matter if we're wrong */

  recordStartTime = times(&garbage);

  if (Devices.MetaDeviceStatus == Sense) RETURN_BOOL(True);

  if (Devices.MaxDevices == 0 || !Devices.Device) {
    Error(NON_FATAL_REPORT_TO_MSGBOX,
	  "Unable to initialise midi: No devices available\n");
    RETURN_BOOL(False);
  }

  if (Devices.Device->Device.fd >= 0) RETURN_BOOL(True);

  if (Devices.Device->Device.Data.Midi.name) {
    XtFree(Devices.Device->Device.Data.Midi.name);
    Devices.Device->Device.Data.Midi.name = XtNewString(mainDeviceName);
  }

  OutputFlag = O_RDWR | O_NDELAY;
  if (MidiPortSync) OutputFlag |= O_SYNC;

  fd = Devices.Device->Device.fd = open(Devices.Device->Device.Data.Midi.name,
					OutputFlag, 0666);
  if (fd < 0)
    {
      sprintf(ErrBuff, "Unable to open Midi Port %s\n",
	      Devices.Device->Device.Data.Midi.name);
      Error(NON_FATAL_REPORT_TO_MSGBOX, ErrBuff);
      RETURN_BOOL(False);
    }

  t.c_iflag = IGNBRK;
  t.c_oflag = 0;
  t.c_cflag = B9600 | CS8 | CREAD | CLOCAL | HUPCL;
  t.c_lflag = 0;
  t.c_line = 1;
  t.c_cc[VINTR] = 0;
  t.c_cc[VQUIT] = 0;
  t.c_cc[VERASE] = 0;
  t.c_cc[VKILL] = 0;
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;
  ioctl(fd, TCSETAF, &t);

  if (ioctl(fd, I_POP, 0) < 0)
    {
      Error(NON_FATAL_REPORT_TO_MSGBOX, "Unable to configure MIDI port");
      RETURN_BOOL(False);
    }

  str.ic_cmd = SIOC_RS422;
  str.ic_timout = 0;
  str.ic_len = 4;
  arg = RS422_ON;
  str.ic_dp = (char *)&arg;

  if (ioctl(fd, I_STR, &str) < 0) 
    {
      Error(NON_FATAL_REPORT_TO_MSGBOX, "Can't ioctl RS422");
      RETURN_BOOL(False);
    }

  str.ic_cmd = SIOC_EXTCLK;
  str.ic_timout = 0;
  str.ic_len = 4;
  arg = EXTCLK_32X;
  str.ic_dp = (char *)&arg;

  if (ioctl(fd, I_STR, &str) < 0) 
    {
      Error(NON_FATAL_REPORT_TO_MSGBOX, "Can't ioctl EXTCLK");
      RETURN_BOOL(False);
    }

  str.ic_cmd = SIOC_ITIMER;
  str.ic_timout = 0;
  str.ic_len = 4;
  arg = 0;
  str.ic_dp = (char *)&arg;

  if (ioctl(fd, I_STR, &str) < 0) 
    {
      Error(NON_FATAL_REPORT_TO_MSGBOX, "Can't ioctl ITIMER");
      RETURN_BOOL(False);
    }

  RETURN_BOOL(True);
}

void
Mapper_CloseDevice()
{
  BEGIN("Mapper_CloseDevice");
  
  if (Devices.Device) {
    if (Devices.Device->Device.fd >= 0) {
      close(Devices.Device->Device.fd);
      Devices.Device->Device.fd = -1;
    }
  }

  END;
}

clock_t MapperPrevClock = 0;

void
Mapper_OpenActiveDevices()
{
  struct tms        garbage;
  BEGIN("Mapper_OpenActiveDevices");

  recordStartTime = times(&garbage);
  MapperPrevClock = 0;

  END;
}

void
Mapper_CloseActiveDevices()
{
  BEGIN("Mapper_CloseActiveDevices");
  END;
}

void
Mapper_WriteEvent(MIDIEvent NextEvent, int dno)
{
  int i = 0;
  DeviceList device;
  Boolean playAll = False;
  unsigned char status;
  static unsigned char lastStatus = 0;
  BEGIN("Mapper_WriteEvent");

  device = Mapper_GetActiveDevice(dno);
  if (device->Device.Type == Mapper_All_Device) playAll = True;

  do {

    if (playAll) device = Mapper_GetDevice(i++);

    if (device->Device.Type == Mapper_Midi_Device) {

      if (device->Device.fd < 0) continue;	/* device not open! */

      status = NextEvent->EventCode;

      if (status != lastStatus) {
	lastStatus = status;
	write(device->Device.fd, &status, 1);
      }

      if (MessageType(status) == MIDI_CTRL_CHANGE ||
	  MessageType(status) == MIDI_PROG_CHANGE) { /* one-parameter events */
	/*
	printf("Writing one-parameter event\n");
	*/
	write(device->Device.fd, &NextEvent->EventData.ProgramChange.Program,1);

      } else {		                             /* two-parameter events */
	/*
	printf("Writing two-parameter event\n");
	*/
	write(device->Device.fd, &NextEvent->EventData.NoteOn.Note, 2);
      }
    }
  } while (playAll && (i < Devices.MaxDevices-1));

  fflush(stdout);

  END;
}

Boolean
Mapper_ReadEvent(MIDIRawEventBuffer ReturnEvent)
{
  fd_set readfds;
  struct timeval timeout;
  struct tms garbage;
  unsigned char lastStatus = 0x90;
  unsigned char numOperands = 0;
  unsigned char operandsLeft = 0;
  unsigned char inByte;
  unsigned char *bytePtr;
  DeviceList device;
  int rtn;
  BEGIN("Mapper_ReadEvent");

  if (Tracks.RecordDevice < 0) return False;
  device = Mapper_GetDevice(Tracks.RecordDevice);
  if (device->Device.fd < 0) return False;

  FD_ZERO(&readfds);
  FD_SET(device->Device.fd, &readfds);
  timeout.tv_sec = timeout.tv_usec = 0;

  if ((rtn = select(device->Device.fd + 1, &readfds,NULL,NULL, &timeout)) > 0) {

    while (!read(device->Device.fd, &inByte, 1));

    if (IsStatusByte(inByte)) {

      if (MessageType(inByte) == MIDI_SYSTEM_MSG) {
	numOperands = 0;
      } else if (MessageType(inByte) == MIDI_CTRL_CHANGE ||
		 MessageType(inByte) == MIDI_PROG_CHANGE) {
	numOperands = 1;
      } else {
	numOperands = 2;
      }
      lastStatus = inByte;
    }

    if (numOperands) {

      ReturnEvent->Time = times(&garbage) - recordStartTime;
      ReturnEvent->Bytes[0] = lastStatus;
      operandsLeft = numOperands;
      bytePtr = &ReturnEvent->Bytes[1];

      while (operandsLeft) {
	while (!read(device->Device.fd, bytePtr, 1));
	if (!IsStatusByte(*bytePtr)) {
	  ++bytePtr;
	  --operandsLeft;
	}
      }
    }

    RETURN_BOOL(True);
  }

  RETURN_BOOL(False);
}

void
Mapper_LoadPatches(void)
{
  BEGIN("Mapper_LoadPatches");
  END;
}


int
Mapper_QueueEvent(EventList NextEvent, int *LastTime, float *PlayTime,
                  unsigned int StartLastTime, float StartPlayTime,
                  float TimeInc)
{
  int DeltaTime;
  clock_t totalWaitTime;
  int remainingWaitTime, timeLost;
  clock_t thisClock;
  struct tms garbage;
  BEGIN("Mapper_QueueEvent");

  DeltaTime = ((NextEvent->Event.DeltaTime) - (*LastTime)) * TimeInc;
  (*PlayTime) += DeltaTime;  

  /* TimeInc is number of 10ms intervals per tick.  We'll do all our
     calculations in 10ms units, because that's the resolution of the
     system timer (i.e. rather poor) */

  totalWaitTime = TimeInc * (NextEvent->Event.DeltaTime - *LastTime);
  thisClock = times(&garbage);

  timeLost = MapperPrevClock ? (thisClock - MapperPrevClock) : 0;
  remainingWaitTime = totalWaitTime - timeLost;

  /*
  printf("totalWaitTime %d, thisClock %d, prevClock %d, timeLost %d\n",
	 (int)totalWaitTime, (int)thisClock,
	 (int)MapperPrevClock, (int)timeLost);
  */

  MapperPrevClock = thisClock;

  if (remainingWaitTime < 0) {	/* oops! we're late */
    if (MidiMaintainTempo) {

      /*
      printf("Late!\n");
      */

      /* reduce time available for the next iteration */
      if (MapperPrevClock > -remainingWaitTime)
	MapperPrevClock += remainingWaitTime;
    }
  } else if (remainingWaitTime > 0) {
    /*
    printf("Sleeping %ld usec\n", (long)remainingWaitTime * 10000);
    */
    usleep((long)remainingWaitTime * 10000);
    MapperPrevClock = times(&garbage);
  }

  (*LastTime) = NextEvent->Event.DeltaTime;
  RETURN_INT((int)NextEvent->Event.DeltaTime);
}

void
Mapper_Initialize(void)
{
  BEGIN("Mapper_Initialize");
  END;
}

void
Mapper_FlushQueue(float FinishTime)
{
  BEGIN("Mapper_FlushQueue");
  END;
}

void
Mapper_InitVoiceAllocations()
{
  BEGIN("Mapper_InitVoiceAllocations");
  END;
}

void Mapper_Reset()
{
  BEGIN("Mapper_Reset");
  fprintf(stderr, "Warning: Mapper_Reset absent for ZILOG\n");
  END;
}

