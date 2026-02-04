
#ifndef _TRACKING_H_
#define _TRACKING_H_

#include "Globals.h"

extern void Midi_PlayTrackingOpen(void);
extern void Midi_PlayTrackingClose(void);
extern void Midi_PlayTrackingJump(long); /* absolute event time */
extern Widget Midi_PlayTrackingMenuButton(Widget);

extern void Midi_FollowReadyService(String); /* IL callback */

#endif

