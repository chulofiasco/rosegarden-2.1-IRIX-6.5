/*
 *
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Menu.h
 *
 *    Description: 
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     16/02/94        AJG             File Created.
 *
 */

#ifndef __MIDI_MENU__
#define __MIDI_MENU__

#include <X11/Intrinsic.h>

Widget Midi_GetShellWidget(Widget w);

void Midi_InstallFileMenu(Widget File);
void Midi_InstallEditMenu(Widget Edit);
void Midi_InstallTrackMenu(Widget Track);
void Midi_InstallMidiMenu(Widget Midi);
void Midi_InstallFilterMenu(Widget Filter);

void Midi_UndoCB(Widget w, XtPointer a, XtPointer b);
void Midi_ShowClipboardCB(Widget w, XtPointer a, XtPointer b);
void Unimplemented(Widget w, XtPointer a, XtPointer b);
void Midi_LoadFile2(char *FileName, FILE *fp, Boolean DispMsgs);
void Midi_SaveFile2(char *SaveFileName, FILE *fp);

#define Midi_LoadFile(filename, dispmsgs) Midi_LoadFile2(filename, NULL, dispmsgs)
#define Midi_LoadFileFromFP(fp, dispmsgs) Midi_LoadFile2(NULL, fp, dispmsgs)

#define Midi_SaveFile(filename) Midi_SaveFile2(filename, NULL)
#define Midi_SaveFileToFP(fp) Midi_SaveFile2(NULL, fp)

void Midi_EnterMenuMode(long unsigned MenuMode);
void Midi_LeaveMenuMode(long unsigned MenuMode);
Boolean Midi_CloseFile(void);

#endif
