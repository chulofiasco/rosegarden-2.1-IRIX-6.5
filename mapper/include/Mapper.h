/*
 * Mapper.h
 */

#ifndef _MAPPER_H_
#define _MAPPER_H_

#undef POSIX_PLEASE
#include <SysDeps.h>
#undef _POSIX_SOURCE

#include "Systems.h"

/* Midi Support */
#include "Lists.h"
#include <MidiXInclude.h>
#include <MidiEvent.h>

typedef enum
{
    Mapper_No_Device = 0,
    Mapper_All_Device,
    Mapper_Midi_Device,
    Mapper_Synth_Device
} DeviceType;

#ifdef SYSTEM_OSS
#include "Mapper_OSS.h"
#else
#ifdef SYSTEM_SGI
#include "Mapper_SGI.h"
#else
#ifdef SYSTEM_ZILOG
#include "Mapper_ZILOG.h"
#else
#include "Mapper_GENERIC.h"
#endif
#endif
#endif

#define PERC_CHN 9

/* used for recognising echo events from Rosegarden */
#define Rosegarden_Echo_Key 0xae

/* Track configuration */
#define Track_Mute            0x000001

/* Device support */
#define Mapper_Devices_Supported 8
#define Mapper_Max_Tracks        40
#define Mapper_All_Device_Label  "All Devices"

typedef int DeviceStatus;

/* Device configuration */
enum
{
    Playback_Muted,
    Playback_Enabled,
    Playback_Disabled
};

#define Patchloaded           0x000001
#define Device_Unitialised    0x000010
#define Device_Active_WO      0x000100
#define Device_Active_WR      0x001000
#define Device_Port_Sync      0x010000
#define Device_Tempo_Maintain 0x100000

typedef struct _TrackInfo
{
    ListElement Scooter;
    int         Device;
    Boolean     PlaybackStatus;
} TrackInfoElement, *TrackInfo;

typedef struct _TrackMetaInfo
{
    /* others to be defined later */
    TrackInfo  Track;
    int        EditDevice;
    int        RecordDevice;
} TrackMetaInfoElement, *TrackMetaInfo;

typedef struct _DeviceListElement
{
    ListElement       Scooter;
    int               Number;
    DeviceInformation Device;
    DeviceStatus      IO_Status;
    int               TotalTracksOnDevice;
    unsigned char     buffering[20];

} DeviceListElement, *DeviceList;


typedef struct _DeviceMetaInfo
{
    int              MaxDevices;
    int              ActiveDevices;
    DeviceList       Device;
    char             *FileDescriptor;  /* holds the actual device file des */
    int              MetaDeviceStatus;

    unsigned char     buffering[20];

} DeviceMetaInfoElement, *DeviceMetaInfo;

typedef struct
{
    unsigned long int Time;
    byte Bytes[3];
} MIDIRawEvent, *MIDIRawEventBuffer;


/* Device functions */
void Mapper_SetTrackInfo();
void Mapper_NewTrackMetaInfo();
void Mapper_FilterTracksbyDevice();
Boolean Mapper_TrackOnDeviceQuery();
int Mapper_ManageDevices();
DeviceList Mapper_NewDeviceList();
DeviceList Mapper_GetDevice();
DeviceList Mapper_GetActiveDevice();
DeviceList Mapper_GetActiveDevice();
TrackInfo Mapper_GetTrack();
TrackInfo Mapper_NewTrackList();

/* Midi functions */
void Mapper_InitVoiceAllocations();
void Mapper_Initialize();
void Mapper_ReinitializeForPlayback();
DeviceList Mapper_DeviceQuery();
Boolean Mapper_SetupDevices(char *midiPortName);
Boolean Mapper_OpenDevice(int Sense, char *Device);
void Mapper_AllNotesOff();
void Mapper_Reset();
void Mapper_CloseDevice();
void Mapper_LoadPatches(void);
void Mapper_FlushQueue(float FinishTime);
void Mapper_WriteEvent(MIDIEvent NextEvent, int DeviceNumber);
Boolean Mapper_ReadEvent(MIDIRawEventBuffer ReturnEvent);
int Mapper_QueueEvent(EventList NextEvent, int *LastTime,
                      float *PlayTime, unsigned int StartLastTime,
                      float StartPlayTime, float TimeInc);
void Mapper_OpenActiveDevices();
void Mapper_CloseActiveDevices();
void Mapper_ModifyTimer(float);

/* Timer functions */
void Mapper_StopTimer();
void Mapper_ContinueTimer();
void Mapper_StartTimer();

#endif /* _MAPPER_H_ */
