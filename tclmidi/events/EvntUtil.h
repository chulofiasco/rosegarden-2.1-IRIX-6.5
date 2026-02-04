#ifndef EVENTUTIL_H
#define EVENTUTIL_H

#include "Event.h"

extern Event *ReadEventFromSMFTrack(SMFTrack &track, unsigned long &last_t,
    int use_time, const char *&errstr);
extern int WriteEventToSMFTrack(SMFTrack &track, unsigned long &last_t,
    const Event *event, int use_time, const char *&errstr);
#endif
