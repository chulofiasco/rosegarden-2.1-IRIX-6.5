#ifndef _MAPPER_GENERIC_H_
#define _MAPPER_GENERIC_H_

typedef struct _MidiInfo
{
    char name[30];  /* no actually as arbitrary as it looks -
                       consistent with the OSS (Hmm, 2.5/3.0?)
                       definition of name length */
} MidiInfo;

typedef struct _DeviceData
{
    MidiInfo Midi;
} DeviceData;

/* some stuff */
typedef struct _DeviceInformation
{
    DeviceType Type;  /* need this one */
    DeviceData Data;
} DeviceInformation;

#endif 
