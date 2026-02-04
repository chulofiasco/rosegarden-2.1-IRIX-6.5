#ifndef _MAPPER_OSS_H_

#ifdef MIDI_PITCH_BEND
#undef MIDI_PITCH_BEND
#endif

#ifdef SYSTEM_FREEBSD
#include <machine/soundcard.h>
#elif SYSTEM_OSS
#include <sys/soundcard.h>
#endif

typedef struct _DeviceInformation
{
    DeviceType         Type;

    union
    {
        struct midi_info   Midi;
        struct synth_info  Synth;
    } Data;

} DeviceInformation;

#endif
