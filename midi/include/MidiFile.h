/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	File.h

Description:	Type definitions and prototypes for MIDI file-handling.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	24/01/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#ifndef _MIDI_FILE_H_
#define _MIDI_FILE_H_

#include <MidiEvent.h>
#include <netinet/in.h>
#include <stdio.h>

/*************************************************************/
/* Hack to provide constant definition for fseek that is not */
/* present when compiling under (for instance) SunOS.	     */
/*************************************************************/

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

/*************************************************************/
/* Format enumeration - defines how the various track chunks */
/* in the MIDI file relate to each other.		     */
/*************************************************************/

typedef enum
{
	MIDI_SINGLE_TRACK_FILE		= 0x00,
	MIDI_SIMULTANEOUS_TRACK_FILE	= 0x01,
	MIDI_SEQUENTIAL_TRACK_FILE	= 0x02,
	MIDI_NO_FILE_LOADED		= 0xFF
}
MIDIFileFormatType;

#define MIDI_FILE_ERR_END_OF_CHUNK	257

#define	MIDI_24_FPS -24
#define MIDI_25_FPS -25
#define MIDI_29_FPS -29
#define MIDI_30_FPS -30

typedef struct
{
	byte FrameRate;
	byte Resolution;
}
SMPTEGranularity;

typedef union
{
	short		 Division;
	SMPTEGranularity SMPTE;

}
MIDITimingData;

/*************************************/
/* MIDI header trunk data-structure. */
/*************************************/

typedef struct
{
	MIDIFileFormatType	Format;
	short			NumTracks;
	MIDITimingData		Timing;
}
MIDIHeaderChunk;

/*********************************************/
/* Mode to be used when opening a MIDI file. */
/*********************************************/

typedef enum
{
	MIDI_READ,
	MIDI_WRITE,
	MIDI_READ_WRITE
}
MIDIFileMode;

/*****************************************************************/
/* MIDI file handle data structure. Contains a pointer to a file */
/* and a counter to help with efficient chunk skipping.		 */
/*****************************************************************/

#define BUFFER_INITIAL_SIZE 50 * 1024 /* 50Kb should be enough for most files */
#define HANDLE_FILE_NAME_SIZE 20
typedef struct
{
  FILE         *File;
  long	        Count;
  long	        MaxCount;
  MIDIFileMode  Mode;
  char          FileName[HANDLE_FILE_NAME_SIZE]; /* for err. msgs composition */
  byte         *Buffer;
  byte         *BufferIdx;
  unsigned long BufferSize; /* Doesn't represents the actual malloc-ed
			       size in the case of files read from
			       stdin, but the actual size of MIDI data */
}
MIDIFileStruct;

#define BufferOffset(x) (x->BufferIdx - x->Buffer)

typedef MIDIFileStruct *MIDIFileHandle;
typedef char *MIDIChunkType;

#ifndef HAVE_NTOHL

#define ByteStreamTo32BitInt(PTR) 	((((long)*(PTR)) << 24) | (((long)*(PTR + 1)) << 16) | \
					(((long)*(PTR + 2)) << 8) | (long)*(PTR + 3))

#define ByteStreamTo16BitInt(PTR)	((((short)*(PTR)) << 8) | (((short)*(PTR + 1))))

#else

#define ByteStreamTo32BitInt(PTR) ntohl(*(long*)(PTR))
#define ByteStreamTo16BitInt(PTR) ntohs(*(long*)(PTR))

#endif

MIDIFileHandle Midi_FileOpen(char            *FileName, 
			     MIDIHeaderChunk *HeaderBuffer,
			     MIDIFileMode     Mode);

MIDIFileHandle Midi_FileOpen2(char            *FileName,
			      FILE *fp,
			      MIDIHeaderChunk *HeaderBuffer,
			      MIDIFileMode     Mode);

#define Midi_FileOpen(filename, headerbuffer, mode) Midi_FileOpen2(filename, NULL, headerbuffer, mode)
#define Midi_FilePtrOpen(fp, headerbuffer, mode) Midi_FileOpen2(NULL, fp, headerbuffer, mode)

Boolean Midi_FileCheckHeaderChunk(MIDIFileHandle, MIDIHeaderChunk*);
void Midi_FileWriteHeaderChunk(MIDIFileHandle, MIDIHeaderChunk*);


void Midi_FileClose(MIDIFileHandle);

int Midi_FileSkipToNextChunk(MIDIFileHandle File,
		     	     MIDIChunkType  ChunkIdentifier);
int Midi_FileReadByte(MIDIFileHandle File);

int Midi_FileReadBytes(MIDIFileHandle File, byte *Buffer, int NumBytes);

int Midi_FileRewindNBytes(MIDIFileHandle MidiFile, int Distance);
int Midi_FileFfwdNBytes(MIDIFileHandle MidiFile, int Distance);

#define Midi_FileBytesLeftInCurrentChunk(X)	X->Count

int Midi_FileReadVarNum(MIDIFileHandle MidiFile, long *ReturnValue);

MIDIEvent Midi_FileReadNextEvent(MIDIFileHandle MidiFile);

EventList Midi_FileReadTrack(MIDIFileHandle MidiFile);

void Midi_FileWriteByte(MIDIFileHandle MidiFile, byte Byte);
void Midi_FileWriteBytes(MIDIFileHandle MidiFile, byte *Buffer, int NumBytes);

void Midi_FileWriteVarNum(MIDIFileHandle MidiFile, long Value);

void Midi_FileWriteNewChunkHeader(MIDIFileHandle MidiFile, MIDIChunkType ChunkIdentifier);
void Midi_FileWriteEvent(MIDIFileHandle MidiFile, MIDIEvent Event);
void Midi_FileWriteTrack(MIDIFileHandle MidiFile, EventList Track);

#endif
