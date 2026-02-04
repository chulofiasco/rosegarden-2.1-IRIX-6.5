#ifndef _MAPPER_SGI_H_
#define _MAPPER_SGI_H_

#include <stdio.h>
#include <sys/bsd_types.h>
#include <sys/types.h>
#include <dmedia/midi.h>

#define SGIMidiPortClosed  0
#define SGIMidiPortOpenIn  1
#define SGIMidiPortOpenOut 2
#define SGIMidiPortFailed -1

/* All these wrapping structs for the device name are
   needed so as to provide a uniform appearance for code
   elsewhere (e.g. EventListWindow in the sequencer) that
   needs to get at the name field of a DeviceInformation
   object without knowing what sort it is */

typedef struct _DeviceMidiInfo {
    char *name;			/* as returned from mdGetName() */
} DeviceMidiInfo;

typedef struct _DeviceData {
    DeviceMidiInfo Midi;
} DeviceData;

typedef struct _DeviceInformation {

    DeviceType Type;
    DeviceData Data;
    MDport MidiPort;
    int MidiPortStatus;

} DeviceInformation;

#endif 
