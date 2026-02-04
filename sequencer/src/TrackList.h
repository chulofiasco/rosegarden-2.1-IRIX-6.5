/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           TrackList.h
 *
 *    Description:    Function prototypes for track list manipulation.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     18/02/94        AJG             File Created.
 *
 */

#ifndef _TRACK_LIST_H_
#define _TRACK_LIST_H_

void Midi_TrackListSetup(void);
void Midi_TrackListSetupDefaults(void);
void Midi_TrackListAddTrack(EventList NewBoy);

void Midi_TrackListCutCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackListCopyCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackListPasteCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackListDeleteCB(Widget w, XtPointer a, XtPointer b);

void Midi_TrackListCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackInfoCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackListSelectionCB(Widget w, XtPointer Closure, XEvent *event, Boolean *cont);
void Midi_TrackRenameCB(Widget w, XtPointer a, XtPointer b);

void Midi_TrackEventListCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackPianoRollCB(Widget w, XtPointer a, XtPointer b);

void Midi_TrackTransposeCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackFilterByChannelCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackFilterByEventCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackFilterByPitchCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackSplitByChannelCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackSplitByPitchCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackCloneCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackMergeCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackQuantizeCB(Widget w, XtPointer a, XtPointer b);
void Midi_TrackChangeChannelCB(Widget w, XtPointer a, XtPointer b);

/* Added by cc 95, no idea if they should be here but ajg certainly */
/* calls them elsewhere and I can't be arsed to work it out now     */

void Midi_TrackTransposeDlg(int);
void Midi_TrackQuantizeDlg(int);
void Midi_TrackChangeChannelDlg(int);
void Midi_TrackFilterByChannelDlg(int);
void Midi_TrackFilterByEventDlg(int);
void Midi_TrackFilterByPitchDlg(int);

#endif
