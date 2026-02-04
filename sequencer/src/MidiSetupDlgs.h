/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           MidiSetupDlgs.h
 *
 *    Description:    Includes and generic sequencing/GUI structures.
 *
 *    Author:         rwb
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     23/8/96         rwb             File Created.
 *
 */

#ifndef _MIDI_SETUP_DLGS_H_
#define _MIDI_SETUP_DLGS_H_

void Midi_GetDeviceInfo(void);
void Midi_InitialPatchesDlg(void);
void Midi_SetupCB(Widget w, XtPointer a, XtPointer b);
void Midi_ChangeTracksToDeviceCB(Widget w, XtPointer a, XtPointer b);
void Midi_MuteAllTracksCB(Widget w, XtPointer a, XtPointer b);
void Midi_ActivateAllTracksCB(Widget w, XtPointer a, XtPointer b);
void Midi_ServiceEventListGUIs(int Mode);

#endif
