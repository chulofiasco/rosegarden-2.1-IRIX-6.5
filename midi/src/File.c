/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	File.c

Description:	Functions to read from and write to MIDI files. The functions provide low-
		level facilities (such as reading and writing byte-streams directly) and
		high-level facilities (dealing with chunks, events and files).

Functions:  Midi_FileOpen:			Open a MIDI file for reading/writing.

	    Midi_FileClose:			Close a MIDI file after manipulation.

	    Midi_FileSkipToNextChunk:		Move on to the next chunk which matches the
						given header code.

	    Midi_FileReadByte:			Read a single byte from the MIDI file.

	    Midi_FileReadBytes:			Read N bytes from the MIDI file.

	    Midi_FileRewindNBytes:		Move back N bytes in the MIDI file.

	    Midi_FileReadVarNum:		Read a variable length value from the MIDI
						file and return it as a long integer.

	    Midi_FileReadNextEvent:		Read in the next event in the MIDI file stream
						and return it in a general event structure.

	    Midi_FileReadTrack:			Read in the next MIDI track, returning an event list.
						No filtering is done and note events are returned in
						their raw two-point format.

Macros:	    ByteStreamTo32BitInt:		Reads 4 bytes from the supplied buffer and
						returns a correctly formed 32-bit integer.

	    ByteStreamTo16BitInt:		As above but for two byte values.

	    Midi_FileBytesLeftInCurrentChunk:	Returns the number of bytes remaining in the
						current chunk of the MIDI file.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	24/01/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/


#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <MidiVarLenNums.h>

#ifdef YAWN_AVAILABLE

#include <MidiXInclude.h>

#endif

#include <Debug.h>


Boolean FirstTrack;
long	TimeCount;

void EnlargeDataBuffer(MIDIFileHandle FileHandle, unsigned long newSize)
{
  char ErrMsgBuffer[30];
  long offset;

  BEGIN("EnlargeDataBuffer");

#ifdef DEBUG
  fprintf(stderr, "Current size : %u, new size : %u\n",
	  FileHandle->BufferSize, newSize);
#endif

  if(newSize < FileHandle->BufferSize) {
#ifdef YAWN_AVAILABLE
      Error(NON_FATAL_REPORT_TO_MSGBOX, "Attempt to shrink DataBuffer");
#else
      Error(NON_FATAL_REPORT_TO_STDERR, "Attempt to shrink DataBuffer");
#endif
      return;
  }

  if(newSize == FileHandle->BufferSize) return;

  offset = (long)FileHandle->BufferIdx - FileHandle->BufferSize;

  if(! (FileHandle->Buffer = 
     realloc(FileHandle->Buffer, FileHandle->BufferSize = newSize))) {
    sprintf(ErrMsgBuffer, "Couldn't enlarge Buffer to size %u\n", newSize);
    Error(FATAL, ErrMsgBuffer);
  }

  /* Set BufferIdx to the same relative value */
  FileHandle->BufferIdx = FileHandle->Buffer + offset;
  END;
}

void Output32BitInt(MIDIFileHandle FileHandle, long num)
{
  byte *Buffer;

  if(BufferOffset(FileHandle) + 4 > FileHandle->BufferSize)
    EnlargeDataBuffer(FileHandle, FileHandle->BufferSize * 2);

  Buffer = FileHandle->BufferIdx;

  Buffer[0] = (byte)((num&0xff000000)>>24);
  Buffer[1] = (byte)((num&0x00ff0000)>>16);
  Buffer[2] = (byte)((num&0x0000ff00)>>8);
  Buffer[3] = (byte)((num&0xff));
 
  FileHandle->BufferIdx += 4;
}

void Output16BitInt(MIDIFileHandle FileHandle, short num)
{
  byte *Buffer = FileHandle->BufferIdx;

  if(BufferOffset(FileHandle) + 2 > FileHandle->BufferSize)
    EnlargeDataBuffer(FileHandle, FileHandle->BufferSize * 2);
  
  Buffer[0] = (byte)((num&0xff00)>>8);
  Buffer[1] = (byte)(num&0xff);

  FileHandle->BufferIdx += 2;
}


Boolean Midi_FileCheckHeaderChunk(MIDIFileHandle FileHandle, 
				  MIDIHeaderChunk *HeaderBuffer)
{
  byte *DataBuffer;
  char ErrBuff[256];
  long TestLength;

  /****************************************************************/
  /* Read in the header chunk - first fourteen bytes of the file. */
  /****************************************************************/
  BEGIN("Midi_FileCheckHeaderChunk");

  DataBuffer = FileHandle->Buffer;

  if (memcmp(DataBuffer, MIDI_FILE_HEADER, 4))
    {
      sprintf(ErrBuff, "File %s is not a standard midi format file.", 
	      FileHandle->FileName);
#ifdef YAWN_AVAILABLE
      Error(NON_FATAL_REPORT_TO_MSGBOX, ErrBuff);
#else
      Error(NON_FATAL_REPORT_TO_STDERR, ErrBuff);
#endif
      RETURN_BOOL(False);
    }

  TestLength = ByteStreamTo32BitInt(DataBuffer + 4);

  if (TestLength != 6L)
    {
      sprintf(ErrBuff, "File %s is not a standard midi format file.",
	      FileHandle->FileName);
#ifdef YAWN_AVAILABLE
      Error(NON_FATAL_REPORT_TO_MSGBOX, ErrBuff);
#else
      Error(NON_FATAL_REPORT_TO_STDERR, ErrBuff);
#endif
      RETURN_BOOL(False);
    }
	
  HeaderBuffer->Format = (MIDIFileFormatType)ByteStreamTo16BitInt(DataBuffer + 8);
  HeaderBuffer->NumTracks 	= ByteStreamTo16BitInt(DataBuffer + 10);
  HeaderBuffer->Timing.Division   = ByteStreamTo16BitInt(DataBuffer + 12);

  if (HeaderBuffer->Timing.Division < 0)
    {
      sprintf(ErrBuff, "File %s uses SMPTE timing formatted.\nThis is not \
supported at present.", FileHandle->FileName);

#ifdef YAWN_AVAILABLE
      Error(NON_FATAL_REPORT_TO_MSGBOX, ErrBuff);
#else
      Error(NON_FATAL_REPORT_TO_STDERR, ErrBuff);
#endif			
      RETURN_BOOL(False);
    }

  FileHandle->BufferIdx = DataBuffer + 14;
  RETURN_BOOL(True);
}

void Midi_FileWriteHeaderChunk(MIDIFileHandle FileHandle,
			       MIDIHeaderChunk *HeaderBuffer)
{
  BEGIN("Midi_FileWriteHeaderChunk");

  memcpy(FileHandle->BufferIdx, MIDI_FILE_HEADER, 4);
  FileHandle->BufferIdx += 4;
  Output32BitInt(FileHandle, 6L);
  FileHandle->BufferIdx[0] = '\0';
  FileHandle->BufferIdx[1] = (char)HeaderBuffer->Format;
  FileHandle->BufferIdx += 2;
  Output16BitInt(FileHandle, HeaderBuffer->NumTracks);
  Output16BitInt(FileHandle, HeaderBuffer->Timing.Division);

  END;
}

/***********************************************************************************/
/* Midi_FileOpen: Opens a MIDI file for input and/or output. If opening in Read or */
/* Read/Write modes then the file is checked for an initial MIDI header chunk mark */
/* and the header information is returned in the buffer provided. The file is not  */
/* wound on to the next chunk, however.						   */
/*										   */
/* If the file is opened in Write mode then the contents of the header buffer      */
/* provided to the function are written out to form the header chunk of the new    */
/* file. The file pointer then sits at the point directly after the header chunk.  */
/***********************************************************************************/

MIDIFileHandle Midi_FileOpen2(char            *FileName,
			      FILE 	      *fp,
			      MIDIHeaderChunk *HeaderBuffer,
			      MIDIFileMode     Mode)
{
char 	       *FileModeStr = 0;
byte  		Buffer[14];
long		LengthTest, ByteCount = 0;
MIDIFileHandle 	NewFile;

BEGIN("Midi_FileOpen");

	/*********************************/
	/* Choose the correct file mode. */
	/*********************************/

	switch(Mode)
	{
	case MIDI_READ:

		FileModeStr = "rb";
		break;

	case MIDI_WRITE:

		FileModeStr = "wb";
		break;

	case MIDI_READ_WRITE:

		FileModeStr = "rwb+";
		break;

	default:
		Error(FATAL, "Invalid file mode.");
      	}

	/***************************************************/
	/* Attempt to open the file and report on failure. */
	/***************************************************/

	if(FileName && ((fp = fopen(FileName, FileModeStr)) == NULL))
	   {
	     char ErrBuff[256];
	     sprintf(ErrBuff, "Unable to open file: %s", FileName);

#ifdef YAWN_AVAILABLE

	     Error(NON_FATAL_REPORT_TO_MSGBOX, ErrBuff);

#else

	     Error(NON_FATAL_REPORT_TO_STDERR, ErrBuff);

#endif

	     RETURN_PTR(NULL);
	   }


	/*********************************************************/
	/* Allocate a new MIDI file handle structure on the heap */
	/* and return a pointer to this structure.		 */
	/*********************************************************/

	NewFile = (MIDIFileHandle)malloc(sizeof(MIDIFileStruct));
	
	if (NewFile == NULL)
	{
		Error(FATAL, "Unable to allocate MIDI file handle.");
	}


	NewFile->File 	  = fp;
	NewFile->Count 	  = 0;
	NewFile->MaxCount = 0;
	NewFile->Mode	  = Mode;
	NewFile->Buffer   = NULL;
	NewFile->BufferIdx = NULL;
	NewFile->BufferSize = 0;
	if(FileName) {
	  unsigned int t = strlen(FileName);
	  strcpy(NewFile->FileName,
		 (t < HANDLE_FILE_NAME_SIZE) ? FileName :
		 FileName + t - HANDLE_FILE_NAME_SIZE);
	}

	if(Mode == MIDI_READ) {

	  if(!FileName) {
	    /* We read from a pre-opened file pointer,
	       so we enlarge the buffer as needed */
	    int ReadCount = BUFFER_INITIAL_SIZE;

	    do {
	      if(ReadCount)
		EnlargeDataBuffer(NewFile, NewFile->BufferSize + ReadCount);
	      ReadCount = fread(NewFile->BufferIdx, sizeof(byte), ReadCount, fp);
	      NewFile->BufferIdx += ReadCount;
	    } while(!feof(fp));

	    NewFile->BufferSize = NewFile->BufferIdx - NewFile->Buffer;

	  } else { /* We read from a standard file, 
		      therefore we can find its length */

	    fseek(fp, 0L, SEEK_END); 
	    EnlargeDataBuffer(NewFile, ftell(fp));
	    rewind(fp);
	    fread(NewFile->Buffer, 
		  sizeof(byte), (size_t)NewFile->BufferSize, fp);
	    NewFile->BufferIdx = NewFile->Buffer;
	  }
	} else { /* I have a bit of a problem here. What to do with readwrite? */
	  EnlargeDataBuffer(NewFile, BUFFER_INITIAL_SIZE);
	}

	/****************************************************************************/
	/* If we are not in write-only mode then we must check the first four bytes */
	/* of the file to see if they match the marker for the start of a header    */
	/* chunk. If not then we report an error, otherwise we read in the header   */
	/* information and place it in the buffer provided.			    */
	/****************************************************************************/

	if (Mode != MIDI_WRITE)
	  {
	    if(Midi_FileCheckHeaderChunk(NewFile, HeaderBuffer) == False)
	      RETURN_PTR(NULL);
	  }
	else
	{
	  Midi_FileWriteHeaderChunk(NewFile, HeaderBuffer);
	  FirstTrack = True;
	}


RETURN_PTR(NewFile);
}

/******************************************************************************/
/* Midi_FileClose: This function closes a MIDI file and frees the File Handle */
/* from memory. Attempts to use a file handle after a call to this function   */
/* will therefore result in a Segmentation Violation. So don't do it.	      */
/******************************************************************************/

void Midi_FileClose(MIDIFileHandle DoomedFile)
{
BEGIN("Midi_FileClose");

	if (DoomedFile->Mode == MIDI_WRITE && 
	    DoomedFile->MaxCount > 0 &&
	    DoomedFile->Count == 0)
	{
	  byte *tmp = DoomedFile->BufferIdx;
		DoomedFile->BufferIdx -= (DoomedFile->MaxCount + 4);
		Output32BitInt(DoomedFile, DoomedFile->MaxCount);
		DoomedFile->BufferIdx = tmp;
	}
	
	if(DoomedFile->Mode != MIDI_READ)
	  fwrite(DoomedFile->Buffer, sizeof(byte), BufferOffset(DoomedFile),
		 DoomedFile->File);

	fclose(DoomedFile->File);

	free(DoomedFile->Buffer);
	free(DoomedFile);

END;
}


/*******************************************************************************/
/* Midi_FileSkipToNextChunk: This function provides navigation facilities over */
/* the chunks in the MIDI file. The file position is skipped forward until the */
/* next chunk that matches the identifier passed in by the caller.	       */
/*******************************************************************************/

int Midi_FileSkipToNextChunk(MIDIFileHandle MidiFile,
			     MIDIChunkType  ChunkIdentifier)
{
byte Buffer[8]; /* Corrected bug here, size was 6, we fread 8 later on. (GL)*/

BEGIN("Midi_FileSkipToNextChunk");

	/************************************************/
	/* If we are in mid-chunk then skip to the end. */
	/************************************************/

	if (MidiFile->Count)
	{
	  MidiFile->BufferIdx += MidiFile->Count;
	}

	/**************************************************************/
	/* If there are no more chunks then complain (fairly) loudly. */
	/**************************************************************/

	if (MidiFile->BufferIdx > (MidiFile->Buffer + MidiFile->BufferSize))
	{
#ifdef YAWN_AVAILABLE
		Error(NON_FATAL_REPORT_TO_MSGBOX, "Unexpected EOF in MIDI file.");
#else
		Error(NON_FATAL_REPORT_TO_STDERR, "Unexpected EOF in MIDI file.");
#endif
		RETURN_INT(EOF);
	}

	/************************************************************************/
	/* Scan through the file for the next chunk with a matching identifier. */
	/************************************************************************/

	while(MidiFile->BufferIdx < (MidiFile->Buffer + MidiFile->BufferSize))
	{
	  /* MidiFile->BufferIdx += 8; */

		if (!memcmp(MidiFile->BufferIdx, ChunkIdentifier, 4))
		{
			MidiFile->Count    = ByteStreamTo32BitInt(MidiFile->BufferIdx + 4);
			MidiFile->MaxCount = MidiFile->Count;
			MidiFile->BufferIdx += 8;
			RETURN_INT(0);
		}
		else MidiFile->BufferIdx += (ByteStreamTo32BitInt(Buffer + 4) + 8);
	}

	/**************************************************************/
	/* If we haven't managed to find a matching chunk by the time */
	/* we reach the end of the file then report an error and      */
	/* return EOF as the error code.			      */
	/**************************************************************/

#ifdef YAWN_AVAILABLE
	Error(NON_FATAL_REPORT_TO_MSGBOX, "Unexpected EOF in MIDI file.");
#else
	Error(NON_FATAL_REPORT_TO_STDERR, "Unexpected EOF in MIDI file.");
#endif

RETURN_INT(EOF);
}



/*************************************************************************/
/* Midi_FileReadByte: Reads in a byte from a MIDI file, checking against */
/* over-running chunk boundaries, and updates the read count in the MIDI */
/* file handle structure.						 */
/*************************************************************************/

int Midi_FileReadByte(MIDIFileHandle MidiFile)
{
int	ReturnValue;
BEGIN("Midi_FileReadByte");

	if (MidiFile->Count == 0)
	{
		RETURN_INT(MIDI_FILE_ERR_END_OF_CHUNK);
	}

	MidiFile->Count--;

	ReturnValue = MidiFile->BufferIdx[0];
	MidiFile->BufferIdx++;

RETURN_INT(ReturnValue);
}

/***************************************************************************/
/* Midi_FileReadBytes:	Read in N bytes from the MIDI file stream checking */
/* against over-running chunk boundaries, etc. Bytes are placed in the     */
/* buffer provided, which must obviously be large enough to contain them.  */
/***************************************************************************/

int Midi_FileReadBytes(MIDIFileHandle MidiFile, byte *Buffer, int NumBytes)
{
BEGIN("Midi_FileReadBytes");

	if (NumBytes <= MidiFile->Count)
	{
    	        memcpy(Buffer, MidiFile->BufferIdx, NumBytes);
		MidiFile->Count -= NumBytes; 
		MidiFile->BufferIdx += NumBytes;
		RETURN_INT(0);
	}
	else 
	{
	        memcpy(Buffer, MidiFile->BufferIdx, MidiFile->Count);
		MidiFile->Count = 0;
		MidiFile->BufferIdx += MidiFile->Count;
		Error(NON_FATAL_REPORT_TO_STDERR, "Chunk too short.");
		RETURN_INT(MIDI_FILE_ERR_END_OF_CHUNK);
	}
}




/***********************************************************************************/
/* Midi_FileRewindNBytes: Function to move the pointer backwards in the current    */
/* chunk of a MIDI file. Checks are made against over-running the chunk boundary.  */
/* If this happens then an error is reported and the file pointer remains unmoved. */
/***********************************************************************************/

int Midi_FileRewindNBytes(MIDIFileHandle MidiFile, int Distance)
{
BEGIN("Midi_FileRewindNBytes");

	if (MidiFile->Count + Distance > MidiFile->MaxCount)
	{
		Error(NON_FATAL_REPORT_TO_STDERR, "Attempt to rewind beyond chunk boundary.");
		RETURN_INT(MIDI_FILE_ERR_END_OF_CHUNK);
	}

	MidiFile->BufferIdx -= Distance;
	MidiFile->Count += Distance;

RETURN_INT(0);
}
		
int Midi_FileFfwdNBytes(MIDIFileHandle MidiFile, int Distance)
{
BEGIN("Midi_FileFfwdNBytes");

	MidiFile->BufferIdx += Distance;
	MidiFile->Count -= Distance;

RETURN_INT(0);
}

/*******************************************************************************/
/* Midi_FileReadVarNum: Function to read a variable length value from the MIDI */
/* input file stream and return the numerical value as a long integer.	       */
/*******************************************************************************/

int Midi_FileReadVarNum(MIDIFileHandle MidiFile, long *ReturnValue)
{
/*
int  	ByteIn, 
	Count;

VarLengthNumBuffer Buffer;
*/

        long value;
	int ByteIn;

BEGIN("Midi_FileReadVarNum");

	ByteIn = Midi_FileReadByte(MidiFile);
	if (ByteIn == EOF)
	    RETURN_INT(MIDI_FILE_ERR_END_OF_CHUNK);
	value = ByteIn;
	if (ByteIn&0x80)
	{
	    value &= 0x7f;
	    do {
		ByteIn = Midi_FileReadByte(MidiFile);
		if (ByteIn == EOF)
		    RETURN_INT(MIDI_FILE_ERR_END_OF_CHUNK);
		value = (value<<7)+(ByteIn&0x7f);
	    } while(ByteIn&0x80);
	}
	*ReturnValue=value;
	RETURN_INT(0);

/* This code is architecture-dependent */
/* 	Count = 3; */
/* 	ByteIn = 0; */

/* 	while ((ByteIn = Midi_FileReadByte(MidiFile)) != EOF) */
/* 	{ */
/* 		Buffer.ByteField[Count--] = (byte)ByteIn; */

/* 		if (!(ByteIn & 0x80)) */
/* 		{ */
/* 			*ReturnValue = Midi_ConvVariableToFixed(Buffer.Number); */
/* 			RETURN_INT(0); */
/* 		} */
/* 	} */

RETURN_INT(MIDI_FILE_ERR_END_OF_CHUNK);	 /* yes, it is unreachable */
}




/**********************************************************************************/
/* Midi_FileReadNextEvent: High-level access function. Reads in the next complete */
/* MIDI event from the file stream and returns it in a MIDIEvent structure.       */
/**********************************************************************************/

MIDIEvent Midi_FileReadNextEvent(MIDIFileHandle MidiFile)
{
static MIDIEvent 	NewEvent = NULL;
static int		EventBufferSize = 0;
long 			DeltaTime;
static int		EventCode;
int			TempCode;
long			MessageLength;

BEGIN("Midi_FileReadNextEvent");

	if (NewEvent == NULL)
	{
		NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct));
		EventBufferSize = sizeof(MIDIEventStruct);
	}

	/******************************/
	/* First read the delta time. */
	/******************************/

	if (Midi_FileReadVarNum(MidiFile, &DeltaTime))
	{
		Error(FATAL, "File error.");
	}

	TempCode = Midi_FileReadByte(MidiFile);

	/***********************************/
	/* Deal with running status bytes. */
	/***********************************/

	if (!IsStatusByte(TempCode))
	{
		Midi_FileRewindNBytes(MidiFile, 1);
	}
	else EventCode = TempCode;

	/********************************************************************/
	/* Handle File Meta-events. These are of variable length defined by */
	/* their first field, held itself as a variable length value.       */
	/********************************************************************/

	if (EventCode == MIDI_FILE_META_EVENT)
	{
		EventCode = Midi_FileReadByte(MidiFile);
		Midi_FileReadVarNum(MidiFile, &MessageLength);

		if (EventBufferSize < (sizeof(MIDIEventStruct) + MessageLength + 1))
		{
		    if (NewEvent) { /* cc 95, SunOS realloc fails passed 0 */
			NewEvent = (MIDIEvent)realloc(NewEvent, 
						      (sizeof(MIDIEventStruct) + MessageLength + 1));
		    } else {
			NewEvent = (MIDIEvent)malloc((sizeof(MIDIEventStruct) + MessageLength + 1));
		    }
 		}

		if (NewEvent == NULL)
		{
			Error(FATAL, "Unable to allocate new event.");
		}

		NewEvent->EventCode = MIDI_FILE_META_EVENT;
		NewEvent->EventData.MetaEvent.MetaEventCode = EventCode;
		NewEvent->DeltaTime = DeltaTime;
		NewEvent->EventData.MetaEvent.NBytes = MessageLength;

		Midi_FileReadBytes(MidiFile, 
				   (byte *)&NewEvent->EventData.MetaEvent.Bytes, 
				   MessageLength);

		*(&NewEvent->EventData.MetaEvent.Bytes + MessageLength) = '\0';
	}
	else

	/******************************************************/
	/* Handle nice easy standard events of fixed lengths. */
	/******************************************************/

	switch(MessageType(EventCode))
	{
	case MIDI_NOTE_OFF:
	case MIDI_NOTE_ON:
	case MIDI_POLY_AFTERTOUCH:
	case MIDI_CTRL_CHANGE:
	case MIDI_PITCH_BEND:

	        if(NewEvent)
		  free(NewEvent);
		NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct));

		if (NewEvent == NULL)
		{
			Error(FATAL, "Unable to allocate new event.");
		}

		NewEvent->DeltaTime = DeltaTime;
		NewEvent->EventCode = (byte)EventCode;

		NewEvent->EventData.NoteOn.Note     = Midi_FileReadByte(MidiFile);
		NewEvent->EventData.NoteOn.Velocity = Midi_FileReadByte(MidiFile);

		break;

	case MIDI_PROG_CHANGE:
	case MIDI_CHNL_AFTERTOUCH:

	        if(NewEvent)
		  free(NewEvent);
		NewEvent = (MIDIEvent)malloc(sizeof(MIDIEventStruct));

		if (NewEvent == NULL)
		{
			Error(FATAL, "Unable to allocate new event.");
		}

		NewEvent->DeltaTime = DeltaTime;
		NewEvent->EventCode = (byte)EventCode;

		NewEvent->EventData.NoteOn.Note = Midi_FileReadByte(MidiFile);
		break;

	default:

#ifdef YAWN_AVAILABLE
		Error(NON_FATAL_REPORT_TO_MSGBOX, "Unsupported event.");
#else
		Error(NON_FATAL_REPORT_TO_STDERR, "Unsupported event.");
#endif
		break;
	}

RETURN_PTR(NewEvent);
}


/**********************************************************************************/
/* Midi_FileReadTrack: Function to read in an entire track chunk from a MIDI file */
/* and return a pointer to a list containing the events in the correct order.     */
/**********************************************************************************/

EventList Midi_FileReadTrack(MIDIFileHandle MidiFile)
{
EventList Track, NextEventList, TrackStart = NULL;
MIDIEvent NextEvent;

BEGIN("Midi_FileReadTrack");

	Track = NULL;

	while(Midi_FileBytesLeftInCurrentChunk(MidiFile))
	{
		NextEvent = Midi_FileReadNextEvent(MidiFile);

		NextEventList = (EventList)Midi_EventCreateList(NextEvent, True);

		if (Track)
		{
			Track = (EventList)Nconc(Track, NextEventList);
		}
		else 
		{
			Track = NextEventList;
			TrackStart = Track;
		}
	}

RETURN_PTR(TrackStart);
}



/*
====================================================

		File Output Functions

====================================================
*/


/******************************************************************************/
/* Midi_FileWriteByte: Write a single byte to the specified Midi file stream. */
/******************************************************************************/

void Midi_FileWriteByte(MIDIFileHandle MidiFile, byte Byte)
{
BEGIN("Midi_FileWriteByte");

	if (MidiFile->Count != 0)
	{
		--MidiFile->Count;
	}
	else
	{
		++MidiFile->MaxCount;
	}
	MidiFile->BufferIdx[0] = Byte;
	MidiFile->BufferIdx++;

	if(BufferOffset(MidiFile) > MidiFile->BufferSize)
	  EnlargeDataBuffer(MidiFile, MidiFile->BufferSize * 2);
END;
}



/*****************************************************************************/
/* Midi_FileWriteBytes: Write the specified number of bytes to the MIDI file */
/* stream from the buffer supplied.					     */
/*****************************************************************************/

void Midi_FileWriteBytes(MIDIFileHandle MidiFile, byte *Buffer, int NumBytes)
{
        int i;
BEGIN("Midi_FileWriteBytes");

	if((BufferOffset(MidiFile) + NumBytes) >= MidiFile->BufferSize)
	  EnlargeDataBuffer(MidiFile, NumBytes + (MidiFile->BufferSize * 2));

	memcpy(MidiFile->BufferIdx, Buffer, NumBytes);

	if (MidiFile->Count != 0)
	{
		MidiFile->Count    -= NumBytes;
	}
	else
	{
		MidiFile->MaxCount += NumBytes;
	}
	MidiFile->BufferIdx += NumBytes;

END;
}



void Midi_FileWriteVarNum(MIDIFileHandle MidiFile, long VarNum)
{
/*
int  	ByteOut, 
	Count;

VarLengthNumBuffer OutBuffer;
*/

        long buffer;

	BEGIN("Midi_FileWriteVarNum");

	buffer = VarNum & 0x7f;
	while ((VarNum>>=7)>0)
        {
	    buffer <<= 8;
	    buffer |= 0x80;
	    buffer += (VarNum & 0x7f);
	}
	while(1)
	{
	    Midi_FileWriteByte(MidiFile, (char)(buffer & 0xff));
	    if (buffer&0x80)
		buffer>>=8;
	    else
		break;
	}

/* This code is architecture-dependent */

/* 	OutBuffer.Number = Midi_ConvFixedToVariable(VarNum); */

/* 	ByteOut = 0; */
/* 	Count = 3; */
/* 	do */
/* 	{ */
/* 		ByteOut = OutBuffer.ByteField[Count]; */
/* 		Midi_FileWriteByte(MidiFile, ByteOut); */
/* 		--Count; */
/* 	} */
/* 	while(ByteOut & 0x80 && Count > 0); */

END;
}


void Midi_FileWriteNewChunkHeader(MIDIFileHandle MidiFile, MIDIChunkType ChunkIdentifier)
{
BEGIN("Midi_FileWriteNewChunkHeader");

	if (strlen(ChunkIdentifier) != 4)
	{
		Error(FATAL, "Invalid Chunk Identifier.");
	}

	if (!FirstTrack)
	{
		MidiFile->BufferIdx -= (MidiFile->MaxCount + 4);
		Output32BitInt(MidiFile, MidiFile->MaxCount);
		MidiFile->BufferIdx += MidiFile->MaxCount;
	}

	strcpy(MidiFile->BufferIdx, ChunkIdentifier);
	MidiFile->BufferIdx += 4; /* actually strlen(ChunkIdentifier) */

	MidiFile->Count = 0;
	MidiFile->MaxCount = 0;


        Output32BitInt(MidiFile, MidiFile->MaxCount);

	FirstTrack = False;
	TimeCount  = 0;

END;
}


void Midi_FileWriteEvent(MIDIFileHandle MidiFile, MIDIEvent Event)
{
static byte EventCode = 0;

BEGIN("Midi_FileWriteEvent");

	/*******************************/
	/* First write the delta time. */
	/*******************************/

	Midi_FileWriteVarNum(MidiFile, Event->DeltaTime - TimeCount);

	if (Event->EventCode == MIDI_FILE_META_EVENT)
	{
		Midi_FileWriteByte(MidiFile, MIDI_FILE_META_EVENT);
		Midi_FileWriteByte(MidiFile, Event->EventData.MetaEvent.MetaEventCode);
		Midi_FileWriteVarNum(MidiFile, Event->EventData.MetaEvent.NBytes);
		Midi_FileWriteBytes(MidiFile, &Event->EventData.MetaEvent.Bytes, 
				    Event->EventData.MetaEvent.NBytes);
		EventCode = 0;
	}
	else
	{
		if (Event->EventCode != EventCode)
		{
			Midi_FileWriteByte(MidiFile, Event->EventCode);
			EventCode = Event->EventCode;
		}

		switch(MessageType(EventCode))
		{
		case MIDI_NOTE_ON:
		case MIDI_NOTE_OFF:
		case MIDI_POLY_AFTERTOUCH:

			Midi_FileWriteByte(MidiFile, Event->EventData.NoteOn.Note);
			Midi_FileWriteByte(MidiFile, Event->EventData.NoteOn.Velocity);
			break;

		case MIDI_CTRL_CHANGE:

			Midi_FileWriteByte(MidiFile, Event->EventData.ControlChange.Controller);
			Midi_FileWriteByte(MidiFile, Event->EventData.ControlChange.Value);
			break;

		case MIDI_PROG_CHANGE:

			Midi_FileWriteByte(MidiFile, Event->EventData.ProgramChange.Program);
			break;

		case MIDI_CHNL_AFTERTOUCH:

			Midi_FileWriteByte(MidiFile, Event->EventData.MonoAftertouch.Channel);
			break;

		case MIDI_PITCH_BEND:

			Midi_FileWriteByte(MidiFile, Event->EventData.PitchWheel.LSB);
			Midi_FileWriteByte(MidiFile, Event->EventData.PitchWheel.MSB);
			break;

		default:
			Error(NON_FATAL_REPORT_TO_MSGBOX, "Unsupported event.");
			END;
		}
	}

	TimeCount = Event->DeltaTime;
END;
}


void Midi_FileWriteTrack(MIDIFileHandle MidiFile, EventList Track)
{
BEGIN("Midi_FileWriteTrack");

	Midi_FileWriteNewChunkHeader(MidiFile, MIDI_TRACK_HEADER);

	while(Track)
	{
		Midi_FileWriteEvent(MidiFile, &Track->Event);

		Track = (EventList)Next(Track);
	}

END;
}
