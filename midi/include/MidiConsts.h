/*
-------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
-------------------------------------------------------------------------------

File Name:      MidiConsts.h

Description:    Header file to define constants and macros for the MIDI
                protocol and file format.

Author:         AJG

History:

Update  Date            Programmer      Comments
======  ====            ==========      ========
001     24/01/94        AJG             File Created.
002     24/04/96        RWB             Added ifndef to eliminate USS Lite
                                        declarations duplication.

-------------------------------------------------------------------------------
*/

#ifndef __MIDICONSTS_H__
#define __MIDICONSTS_H__

typedef unsigned char byte;

typedef unsigned char bool;
#define FALSE 	0
#define TRUE 	1

#define NumberElts(X) (sizeof(X) / sizeof(X[0]))

/***************************/
/* Channel Voice Messages. */
/***************************/

#define MIDI_NOTE_OFF		((byte)0x80)
#define MIDI_NOTE_ON		((byte)0x90)
#define MIDI_POLY_AFTERTOUCH	((byte)0xA0)
#define	MIDI_CTRL_CHANGE	((byte)0xB0)
#define MIDI_PROG_CHANGE	((byte)0xC0)
#define MIDI_CHNL_AFTERTOUCH	((byte)0xD0)

#ifndef MIDI_PITCH_BEND
#define MIDI_PITCH_BEND		((byte)0xE0)
#endif

/**************************/
/* Channel Mode Messages. */
/**************************/

#define MIDI_SELECT_CHNL_MODE	((byte)0xB0)

/********************/
/* System Messages. */
/********************/

#define MIDI_SYSTEM_MSG		((byte)0xF0)

#define MIDI_SYSTEM_EXCLUSIVE	((byte)0xF0)
#define MIDI_SONG_POSITION_PTR	((byte)0xF2)
#define MIDI_SONG_SELECT	((byte)0xF3)
#define MIDI_TUNE_REQUEST	((byte)0xF6)
#define MIDI_EOX		((byte)0xF7)

/******************************/
/* System Real-Time Messages. */
/******************************/

#define MIDI_TIMING_CLOCK	((byte)0xF8)
#define MIDI_START		((byte)0xFA)
#define MIDI_CONTINUE		((byte)0xFB)
#define MIDI_STOP		((byte)0xFC)
#define MIDI_ACTIVE_SENSING	((byte)0xFE)
#define MIDI_SYSTEM_RESET	((byte)0xFF)

/*******************************/
/* MIDI file format constants. */
/*******************************/

#define MIDI_FILE_HEADER	"MThd"
#define MIDI_TRACK_HEADER	"MTrk"
#define MIDI_FILE_META_EVENT	((byte)0xFF)

/**************************/
/* MIDI file Meta-Events. */
/**************************/

#define MIDI_SEQUENCE_NUMBER	((byte)0x00)
#define MIDI_TEXT_EVENT		((byte)0x01)
#define MIDI_END_OF_TRACK	((byte)0x2F)
#define MIDI_SET_TEMPO		((byte)0x51)
#define MIDI_SMPTE_OFFSET	((byte)0x54)
#define MIDI_TIME_SIGNATURE	((byte)0x58)
#define MIDI_KEY_SIGNATURE	((byte)0x59)
#define MIDI_SEQUENCER_SPECIFIC	((byte)0x7F)

/******************************/
/* Specific Text Meta-Events. */
/******************************/

#define MIDI_COPYRIGHT_NOTICE	((byte)0x02)
#define MIDI_TRACK_NAME		((byte)0x03)
#define MIDI_INSTRUMENT_NAME	((byte)0x04)
#define MIDI_LYRIC		((byte)0x05)
#define MIDI_TEXT_MARKER	((byte)0x06)
#define MIDI_CUE_POINT		((byte)0x07)

/**************************************************************************/
/* Macro to build a message header byte by assigning the supplied channel */
/* number to the lower nybble of the channel message symbol supplied.	  */
/**************************************************************************/

#define CreateMessageByte(MSG, CHANNEL) (byte)((MSG) | (byte)(CHANNEL))

/*************************************************************************************/
/* Macro to extract message type from a status byte by masking out the lower nybble. */
/*************************************************************************************/

#define MessageType(MSG) 		(byte)((MSG) & ((byte)0xF0))

/****************************************************************************************/
/* Macro to test whether a byte on the MIDI stream is a status byte by examining bit 7. */
/****************************************************************************************/

#define IsStatusByte(MSG) 		(byte)((MSG) & ((byte)0x80))

/*******************************************************************/
/* Macro to extract the channel number from a voice channel event. */
/*******************************************************************/

#define ChannelNum(MSG)			(byte)((MSG) & ((byte)0x0F))


typedef unsigned long Midi_EventMask;

#define MidiNoEventMask			(0)
#define MidiNoteOffEventMask		(1)
#define	MidiNoteOnEventMask		(1 << 1)
#define MidiPolyAftertouchEventMask	(1 << 2)
#define MidiCtrlChangeEventMask		(1 << 3)
#define MidiProgChangeEventMask		(1 << 4)
#define MidiChnlAftertouchEventMask	(1 << 5)
#define MidiPitchBendEventMask		(1 << 6)
#define	MidiSystemExEventMask		(1 << 7)
#define MidiTextEventMask		(1 << 8)
#define MidiSetTempoEventMask		(1 << 9)
#define MidiSmpteOffsetEventMask	(1 << 10)
#define MidiTimeSignatureEventMask	(1 << 11)
#define MidiKeySignatureEventMask	(1 << 12)
#define MidiSequencerSpecificEventMask	(1 << 13)
#define MidiCopyrightNoticeEventMask	(1 << 14)
#define MidiTrackNameEventMask		(1 << 15)
#define MidiInstrumentNameEventMask	(1 << 16)
#define MidiLyricEventMask		(1 << 17)
#define MidiTextMarkerEventMask		(1 << 18)
#define MidiCuePointEventMask		(1 << 19)
#define MidiSongPosPtrEventMask		(1 << 20)
#define MidiTuneRequestEventMask	(1 << 21)
#define	MidiSequenceNumberEventMask	(1 << 22)

#define MidiAllEventsMask		~MidiNoEventMask

#define MidiSoundEventsMask (MidiNoteOffEventMask | MidiNoteOnEventMask   | MidiPolyAftertouchEventMask |\
			MidiCtrlChangeEventMask | MidiProgChangeEventMask | MidiChnlAftertouchEventMask |\
			 MidiPitchBendEventMask)

#define MidiTextEventsMask (MidiTextEventMask | MidiCopyrightNoticeEventMask | MidiTrackNameEventMask  |\
			    MidiInstrumentNameEventMask | MidiLyricEventMask | MidiTextMarkerEventMask |\
			    MidiCuePointEventMask)

#define MidiMetaEventsMask (MidiTextEventsMask | MidiSetTempoEventMask | MidiSmpteOffsetEventMask |\
			    MidiTimeSignatureEventMask | MidiKeySignatureEventMask |\
			    MidiSequencerSpecificEventMask)

#endif
