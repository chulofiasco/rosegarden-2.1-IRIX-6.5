#ifndef _MAPPER_ZILOG_H_
#define _MAPPER_ZILOG_H_

#include <stdio.h>
#include <sys/bsd_types.h>
#include <sys/types.h>

/* All these wrapping structs for the device name are
   needed so as to provide a uniform appearance for code
   elsewhere (e.g. EventListWindow in the sequencer) that
   needs to get at the name field of a DeviceInformation
   object without knowing what sort it is */

typedef struct _DeviceMidiInfo {
    char *name;
} DeviceMidiInfo;

typedef struct _DeviceData {
    DeviceMidiInfo Midi;
} DeviceData;

typedef struct _DeviceInformation {
    DeviceType Type;
    DeviceData Data;
    int fd;
} DeviceInformation;

#endif 
