/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           PianoRoll.h
 *
 *    Description:    Function prototypes and typedefs for Piano Roll
 *                    track display windows.
 *
 *    Author:         AJG
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     15/03/94        AJG             File Created.
 *    001     15/03/94        AJG             File Created.
 *            23/06/96        cc              added quit action
 *
 *
 */


#ifndef _PIANO_ROLL_H_
#define _PIANO_ROLL_H_

typedef struct
{
	ListElement	Base;
	int		TrackNum;
	Widget		Shell;
	Widget 		Pane;
	Widget		TopBox;
	Widget		MenuBar;
	Widget		HelpBox;
	Widget		Toolbar;
	Widget		Form;
	Widget		ViewPort;
	Widget		PianoRollLabel;
	Widget		Scrollbar;
	Widget		TrackMenuButton;
	Widget		EditMenuButton;
	Widget		EventMenuButton;
	Widget		CounterDisplay;
	YMenuElement   *TrackMenu;
	YMenuId		TrackMenuId;
	YMenuElement   *EditMenu;
	YMenuId		EditMenuId;
	YMenuElement   *EventMenu;
	YMenuId		EventMenuId;
	Pixmap		LabelPixmap;
	Dimension	PianoRollWidth;
	Dimension	PianoRollHeight;
	GC		Clear;
	GC		DrawForce;
	GC		DrawXor;
	GC		LightGrey;
	GC		Grey;
	int		BarNumber;
	long		LastEvtTime;
	Boolean		Dragging;
	int		DragStartCoord;
	int		DragEndCoord;
	EventList	SelectStartEvt;
	EventList	SelectEndEvt;

  /* stick these here so as not to destroy creative castings */

      
	Widget		PianoRollStrengthLabel;
/*cc	Widget		ViewPortStrength;*/
	Pixmap		StrengthLabelPixmap;
        Widget          ZoomValue;
        float           Zoom;
}
PRWindowListElt, *PRWindowList;

#define PIANO_ROLL_STAVE_SPACING      ((14 * ( Zoom / PIANO_ROLL_ZOOM_DEFAULT )))  /* magnification */
#define PIANO_ROLL_STAVE_HEIGHT	      37

#define PIANO_ROLL_DEFAULT_BAR_WIDTH  (192 * ( Zoom / PIANO_ROLL_ZOOM_DEFAULT ))

#define PIANO_ROLL_CLEF_WIDTH	      100
#define PIANO_ROLL_CLEF_HEIGHT        53
#define PIANO_ROLL_SHARP_WIDTH        6
#define PIANO_ROLL_SHARP_HEIGHT	      14

#define PIANO_ROLL_OCTAVE_YSIZE	      (3.5 * PIANO_ROLL_STAVE_SPACING)

#define PIANO_ROLL_MIDDLE_C_YPOS      ((PIANO_ROLL_TOP_STAVE_OFFSET + \
				       PIANO_ROLL_OCTAVE_YSIZE + \
				       PIANO_ROLL_STAVE_SPACING))

#define PIANO_ROLL_MAX_OCTAVES        12
#define PIANO_ROLL_TOP_STAVE_OFFSET   (4 * PIANO_ROLL_OCTAVE_YSIZE)
#define PIANO_ROLL_PIXMAP_HEIGHT      (PIANO_ROLL_MAX_OCTAVES * PIANO_ROLL_OCTAVE_YSIZE)
#define PIANO_ROLL_VELOCITY_HEIGHT    ((80) * ( Zoom / PIANO_ROLL_ZOOM_DEFAULT ))
#define PIANO_ROLL_SCROLLBAR_OFFSET   14
#define PIANO_ROLL_MAX_VELOCITY       127

#define PIANO_ROLL_TIMING_INFO(x, bar)   ( ( ( (x) - PIANO_ROLL_CLEF_WIDTH ) / ( PIANO_ROLL_DEFAULT_BAR_WIDTH \
                                           / ( 4 * (MIDIHeaderBuffer.Timing.Division) ) ) ) + \
                                           ( (bar) * (4) * ( MIDIHeaderBuffer.Timing.Division ) ) )

#define PIANO_ROLL_ZOOM_DEFAULT       100
#define PIANO_ROLL_ZOOM_MIN           40
#define PIANO_ROLL_ZOOM_MAX           300
#define PIANO_ROLL_ZOOM_STEP          20

void Midi_PianoRollWindowCreate(int TrackNum);
void Midi_UpdatePianoRollWindow(int TrackNum);
void Midi_PianoRollDeleteAllWindows(void);
void Midi_RemovePianoRollWindow(int TrackNum);
void Midi_PianoRollQuitAction(Widget, XEvent *, String *, Cardinal *);
Widget Midi_PianoRollLocateWindowShell(int TrackNum);



#endif
