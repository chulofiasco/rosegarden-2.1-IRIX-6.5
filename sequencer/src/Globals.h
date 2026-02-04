/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Globals.h
 *
 *    Description:    General constants for MIDI sequencer.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update    Date            Programmer      Comments
 *    ======    ====            ==========      ========
 *    001       16/02/94        AJG             File Created.
 *
 *
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <SysDeps.h>

#include "Types.h"
#include "MidiFile.h"
#include "MidiTrack.h"
#include <X11/Intrinsic.h>

#include <ServiceNames.h>

extern Display	       *display;
extern Widget	 	topLevel;
extern XtAppContext	appContext;
extern Window		InterlockServer;
extern AppData		appData;
extern MIDIHeaderChunk  MIDIHeaderBuffer;
extern MIDIFileHandle	MIDIFile;
extern char	       *MIDIFileName;
extern EventList       *MIDITracks;
extern Widget 		TrackListBox;
extern Widget 		TrackListInfoBox;
extern Widget		RoseLabel;
extern Pixmap		HourglassPixmap;
extern Cursor		HourglassCursor;
extern Widget		MsgLabel;
extern Dimension	MsgLabelWidth;
extern Pixmap  		RoseMap; 
extern Pixmap		RoseMask;
extern Pixmap		UpMap;
extern Pixmap		DownMap;
extern Pixmap		TrebleClef;
extern Pixmap		BassClef;
extern Pixmap		Rewind;
extern Pixmap		Back;
extern Pixmap		Forward;
extern Pixmap		Ffwd;
extern Pixmap		ZoomIn;
extern Pixmap		ZoomOut;
extern Pixmap		Sharp;
extern Pixmap		LightGrey;
extern Pixmap		Grey;

extern Boolean		IsMono;

extern Cursor		HourglassAnimCur[];

extern char 	       *Notes[];
extern Boolean		MIDIinServitude;
extern Boolean		MIDIfileModified;
extern Boolean		MIDIneverSaved;
extern int		MIDISelectedTrack;

extern char	       *MidiPortName;

/********************/
/* Interface Modes. */
/********************/

#define NullMode		0L
#define NoFileLoadedMode	1L
#define NothingDoneMode		(1L << 1)
#define NothingSelectedMode	(1L << 2)
#define NothingCutMode		(1L << 3)
#define PlaybackMode            (1L << 4)
#define NotPlayingMode          (1L << 5)
#define RecordMode              (1L << 6)
#define EveryMode               (0xffffL)

/**********************************************************************/
/* Time/Beat conversion macros. (Put here as they contain a reference */
/* to MIDIHeaderBuffer and can therefore not be used except with the  */
/* inclusion of this file.					      */
/**********************************************************************/

#define Midi_TimeToBeat(TIME) ((float)(TIME) / (MIDIHeaderBuffer.Timing.Division))
#define Midi_BeatToTime(BEAT)  (long)((BEAT) * (MIDIHeaderBuffer.Timing.Division))

#include "Systems.h"

#endif
