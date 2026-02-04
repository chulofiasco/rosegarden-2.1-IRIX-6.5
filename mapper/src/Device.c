/*
 * Mapper Device routines
 */

#include "Mapper.h"
#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <MidiEvent.h>
#include <MidiEvent.h>
#include <MidiBHeap.h>
#include <MidiTrack.h>
#include <Debug.h>
#include <../../sequencer/src/Globals.h> /* ugh */

extern MIDIHeaderChunk  MIDIHeaderBuffer;
extern TrackMetaInfoElement   Tracks;
extern DeviceMetaInfoElement  Devices;
extern int TracksOnDevice[Mapper_Devices_Supported][Mapper_Max_Tracks];

TrackInfo
Mapper_GetTrack(int TrackNumber)
{
    TrackInfo TempTrack;
    int i = 0;
BEGIN("Mapper_GetTrack");

    TempTrack = (TrackInfo) First (Tracks.Track);

    while ( (TempTrack ) && (i < TrackNumber) )
    {
        TempTrack = (TrackInfo) Next (TempTrack);
        i++;
    }

RETURN_PTR(TempTrack);
}

/*
 * Function:     Mapper_NewDeviceList
 *
 * Description:  Dbly Linked Lists constructor for the
 *               returning of synth and midi device info.
 *
 */
DeviceList
Mapper_NewDeviceList( DeviceInformation NewDevice, int Number)
{
    DeviceList newlist;

    BEGIN("MidiNewDeviceList");

    newlist = (DeviceList) NewList (sizeof(DeviceListElement));
    newlist->Device = NewDevice;
    newlist->Number = Number;
    newlist->IO_Status = Device_Unitialised;

    RETURN_PTR(newlist);
}


Boolean
Mapper_TrackOnDeviceQuery(int DeviceNumber, int TrackNumber)
{
    TrackInfo TrackTemp = NULL;

BEGIN("Mapper_GetTrackonDevice");
    TrackTemp = (TrackInfo)Mapper_GetTrack(TrackNumber);

    if ( TrackTemp->Device == DeviceNumber )
    {
        RETURN_BOOL(True);
    }
    else
    {
        RETURN_BOOL(False);
    }
}

/*
 * Mapper_ManageDevices - rwb 12/96
 * 
 * this needs to be able to know the states of the various (possible)
 * devices hanging off the sequencer.  pass in a parameter to set
 * the device list up to - all mute devices are ignored, any record
 * optionality opens the device(s) as WR, otherwise it's RO.
 */
int
Mapper_ManageDevices(int ManageState)
{
BEGIN("Mapper_ManageDevices");

    /* if we've got no recordable devices
       we can return an error */

    if ( (ManageState&Device_Active_WO) )
    {
        RETURN_BOOL(Mapper_OpenDevice(ManageState,
                        Devices.FileDescriptor));
    }
    else
    {
        if (Tracks.RecordDevice == -1 )
        {
            RETURN_BOOL(False);
        }
        RETURN_BOOL(Mapper_OpenDevice(ManageState,
                        Devices.FileDescriptor));
    }
}


/*
 * Mapper_SetTrackInfo
 *
 */

void
Mapper_SetTrackInfo(void)
{
int i;
TrackInfo TempPtr = NULL;
BEGIN("Mapper_SetTrackInfo");

    for ( i = 0; i < MIDIHeaderBuffer.NumTracks; i++ )
    {
        TempPtr = Mapper_GetTrack(i);
        TempPtr->Device = 0;
        if (Devices.MaxDevices > 0)
            TempPtr->PlaybackStatus = Playback_Enabled;
        else
            TempPtr->PlaybackStatus = Playback_Disabled;
    }

END;
}


/*
 *
 * Mapper_NewTrackList
 *
 *
 */
TrackInfo
Mapper_NewTrackList(void)
{
    TrackInfo  newlist;

BEGIN("Mapper_NewTrackList");

    newlist = (TrackInfo) NewList (sizeof(TrackInfoElement));
    newlist->Device = 0;
    if (Devices.MaxDevices > 0)
        newlist->PlaybackStatus = Playback_Enabled;
    else
        newlist->PlaybackStatus = Playback_Disabled;

RETURN_PTR(newlist);
}

void
Mapper_NewTrackMetaInfo()
{
    TrackInfo TempTrack;

    TempTrack = (TrackInfo) Mapper_NewTrackList();
       (Tracks.Track) = (TrackInfo)Nconc(Tracks.Track, TempTrack);
}


DeviceList
Mapper_GetDevice(int DeviceNumber)
{
    DeviceList TempDev;
    int i = 0;
BEGIN("Mapper_GetDevice");

    TempDev = (DeviceList) First (Devices.Device);

    while ( (TempDev ) && ( i < DeviceNumber ) )
    {
        TempDev = (DeviceList) Next (TempDev);
        i++;
    }
RETURN_PTR(TempDev);
}

DeviceList
Mapper_GetActiveDevice(int DeviceNumber)
{
    DeviceList TempDev;
    int i = 0;
BEGIN("Mapper_GetActiveDevice");

    TempDev = (DeviceList) First (Devices.Device);

    while (TempDev)
    {
        if (TempDev->TotalTracksOnDevice > 0)
        {
            i++;

            if (i>DeviceNumber)
                break;
        }
        TempDev = (DeviceList)Next(TempDev);
    }

RETURN_PTR(TempDev);
}


/*
 * Mapper_FilterTrackbyDevice
 *
 *
 */

void
Mapper_FilterTracksbyDevice()
{
    DeviceList TempDev = NULL;
    TrackInfo  TempTracks = NULL;

    int DeviceNumber = 0;
    int TrackNumber;
    int current, i;

BEGIN("Mapper_FilterTracksbyDevice");

    TempDev = (DeviceList) First (Devices.Device);

    Devices.ActiveDevices = 0;
    while (TempDev)
    {

        TempTracks = (TrackInfo) First (Tracks.Track);
        TrackNumber = 0;
        current = 0;

        /* initialise the device numbers */
        TempDev->TotalTracksOnDevice = 0;

        for ( i = 0; i < MIDIHeaderBuffer.NumTracks; i++ )
            TracksOnDevice[DeviceNumber][i] = -1;

        while (TempTracks)
        {
            if ((Mapper_GetDevice(TempTracks->Device)->Number == DeviceNumber)
                    && ((TempTracks->PlaybackStatus&Playback_Enabled)))
            {
                (TempDev->TotalTracksOnDevice)++;

                TracksOnDevice[DeviceNumber][current++] = TrackNumber;
            }

            TempTracks = (TrackInfo) Next (TempTracks);
            TrackNumber++;
        }

        if ( TempDev->TotalTracksOnDevice > 0 )
            Devices.ActiveDevices++;

        TempDev = (DeviceList) Next (TempDev);
        DeviceNumber++;
    }

END;
}
