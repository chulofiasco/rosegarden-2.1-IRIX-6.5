/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Main.h
 *
 *    Description:
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
 *    001     16/02/94        AJG             File Created.
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_

void Midi_QuitCB(Widget w, XtPointer a, XtPointer b);
void Midi_SetFileModified(Boolean);
void Midi_SetTitleBar(void);
void Midi_HelpCallback(Widget w, XtPointer HelpTopic, XtPointer a);
void Midi_DemandHelp(char *HelpTopic);
void Midi_ExitCleanly(void);

#include "Tracking.h"

#endif
