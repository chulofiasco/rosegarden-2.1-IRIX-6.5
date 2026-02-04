/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           EventDlgs.h
 *
 *    Description:    Prototypes for event creation and modification dialogue boxes.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     18/02/94        AJG             File Created.
 *    002     03/11/96        rwb             new event prototypes
 *
 */

#ifndef _EVENT_DLGS_H_
#define _EVENT_DLGS_H_

typedef enum
{
	RUNNING   = 0,
	COMPLETED = 1,
	CANCELLED = 2
}
Midi_EventDlgState;

EventList Midi_TextEventDlg(byte EventCode, long time, char *Text,
                            byte TextType, Boolean New);

EventList Midi_NoteEventDlg(long time, byte Channel, byte Pitch, byte Velocity, 
			    long Duration, Boolean New);


EventList Midi_CtrlChngEventDlg(long time, byte Channel, byte Controller,
                           byte Value, Boolean New);

EventList Midi_ProgramChangeEventDlg(long time, byte Channel,
                           byte Program, Boolean New);

EventList Midi_AftertouchEventDlg(long time, byte Channel, byte Aftertouch,
                                   byte Note, Boolean Type, Boolean New);

EventList Midi_PitchBendEventDlg(long time, byte Channel, byte LSB,
                           byte MSB, Boolean New);

EventList Midi_TempoEventDlg(long time, long TempoValue, Boolean New);

#endif

