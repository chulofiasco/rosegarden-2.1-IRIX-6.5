/*
 *
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Record.h
 *
 *    Description:    MIDI recording function prototypes.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     28/04/94        AJG             File Created.
 *    002     06/95           cc              Archer Sully's SGI MIDI lib stuff
 *
 */

#ifndef _MIDI_RECORD_H_
#define _MIDI_RECORD_H_

#include "Systems.h"

Boolean Midi_SeqReadTrack();
void Midi_SeqRecordEvent();
void Midi_SeqStopRecordingCB(Widget w, XtPointer a, XtPointer b);

#endif
