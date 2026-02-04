/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Sequence.h
 *
 *    Description:    Prototypes for sequencer playback functions.
 *
 *
 *    Author:         AJG
 *
 *
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     29/03/94        AJG             File Created.
 *    002     09/08/94        JPff            Midi_SeqOutInit given Boolean
 *                                               value
 *    003     17/02/95        cc              Added #ifdefs for HAVE_Z8530
 *
 */

#ifndef _MIDI_SEQUENCE_H_
#define _MIDI_SEQUENCE_H_

#include "Systems.h"
#include <X11/Intrinsic.h>

Boolean Midi_SeqPlayFile(void);

void Midi_RewindTimerCB(Widget w, XtPointer a, XtPointer b);
void Midi_FfwdTimerCB(Widget w, XtPointer a, XtPointer b);
void Midi_RwdTimerCB(Widget w, XtPointer a, XtPointer b);
void Midi_SeqStopPlayingCB(Widget w, XtPointer a, XtPointer b);
void Midi_SkiptoEndCB(Widget w, XtPointer a, XtPointer b);
void Midi_ResetCB(Widget w, XtPointer a, XtPointer b);
void Midi_ResetTimingInformation();
void Midi_ComputeTrackEndTime();
void Midi_SetTimeField();
int Midi_SeqPlayFileCB();

#endif /* _MIDI_SEQUENCE_H_ */
