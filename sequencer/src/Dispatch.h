/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Dispatch.h
 *
 *    Description:    Header for dispatch function prototypes.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     15/06/94        AJG             File Created.
 *
 */

#ifndef _MIDI_DISPATCH_H_
#define _MIDI_DISPATCH_H_

void Midi_DispatchCutCB(Widget w, XtPointer a, XtPointer b);
void Midi_DispatchCopyCB(Widget w, XtPointer a, XtPointer b);
void Midi_DispatchPasteCB(Widget w, XtPointer a, XtPointer b);
void Midi_DispatchDeleteCB(Widget w, XtPointer a, XtPointer b);

#endif

