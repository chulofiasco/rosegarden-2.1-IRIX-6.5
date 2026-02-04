/*
 * STUB Midi Mapper library routines
 *
 * subtitute your system specific gubbins in
 * the spaces provided and then try not to hold
 * your breath
 *
 */

#undef POSIX_PLEASE
#undef _POSIX_SOURCE

#include <Debug.h>

TrackMetaInfoElement  Tracks;
DeviceMetaInfoElement Devices;
int TracksOnDevice[Mapper_Devices_Supported][Mapper_Max_Tracks];

extern Boolean MidiMaintainTempo;
extern Boolean MidiPortSync;


Boolean
Mapper_SetupDevices(char *midiPortName)
{
  BEGIN("Mapper_SetupDevices");
  RETURN_BOOL(False);
}

DeviceList
Mapper_DeviceQuery(void)
{
  BEGIN("Mapper_DeviceQuery");
  RETURN_PTR(NULL);
}

Boolean
Mapper_OpenDevice(int Sense, char *Device)
{
  BEGIN("Mapper_OpenDevice");
  RETURN_BOOL(False);
}

void
Mapper_CloseDevice()
{
  BEGIN("Mapper_CloseDevice");
  END;
}

void
Mapper_OpenActiveDevices()
{
  BEGIN("Mapper_OpenActiveDevices");
  END;
}

void
Mapper_CloseActiveDevices()
{
  BEGIN("Mapper_CloseActiveDevices");
  END;
}

void
Mapper_WriteEvent(MIDIEvent NextEvent, int device)
{
  BEGIN("Mapper_WriteEvent");
  END;
}

Boolean
Mapper_ReadEvent(MIDIRawEventBuffer ReturnEvent)
{
  BEGIN("Mapper_ReadEvent");
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
  BEGIN("Mapper_QueueEvent");

  DeltaTime = ((NextEvent->Event.DeltaTime) - (*LastTime)) * TimeInc;
  (*PlayTime) += DeltaTime;  
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

void
Mapper_Reset()
{
  BEGIN("Mapper_Reset");
  END;
}
