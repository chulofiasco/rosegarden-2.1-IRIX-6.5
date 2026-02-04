/*
 *    Rosegarden MIDI Sequencer
 */

#include <ILClient.h>
/*#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>*/
#include "Globals.h"

#include "Types.h"
#include "Menu.h"
#include "Main.h"
#include "MainWindow.h"
#include "TrackList.h"
#include "EventListWindow.h"
#include "PianoRollMenu.h"
#include "PianoRoll.h"
#include "Message.h"
#include "Undo.h"
#include "Clipboard.h"
#include "Consts.h"
#include "Sequence.h"
#include "Record.h"
#include "Dispatch.h"
#include "EventListMenu.h"
#include "Csound.h"
#include "MidiSetupDlgs.h"

#include <string.h>
#include <unistd.h>

#include <Yawn.h>
#include <Debug.h>

#include <MidiErrorHandler.h>

extern unsigned int InitialTempo;  /* Initial tempo from loaded MIDI file */

#include <toolbar/beam.xbm>
#include <toolbar/undo.xbm>
#include <toolbar/ninja_cross.xbm>
#include <toolbar/copy.xbm>
#include <toolbar/cut.xbm>
#include <toolbar/paste.xbm>
#include <toolbar/open.xbm>
#include <toolbar/save.xbm>
#include <toolbar/quantize.xbm>
#include <toolbar/event_list.xbm>
#include <toolbar/piano_roll.xbm>
#include <toolbar/sequence.xbm>
#include <toolbar/stop.xbm>
#include <toolbar/record.xbm>
#include <toolbar/ffwd.xbm>
#include <toolbar/rwd.xbm>
#include <toolbar/rrwd.xbm>
#include <toolbar/skp.xbm>

MIDIHeaderChunk MIDIHeaderBuffer;
MIDIFileHandle	MIDIFile;
char	       *MIDIFileName;
EventList      *MIDITracks;
char	       *MIDItempFileName = NULL;
char           *MIDIexternalPlayFileName = NULL;
pid_t           MIDIexternalPlayerPID;


void Unimplemented(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Unimplemented");

  (void)YQuery(topLevel, "Sorry, that function is not yet implemented.",
		1, 0, 0, "Continue", NULL);
END;
}

void Midi_ExternalPlayerExitCB()
{
    int status;
    
    waitpid(MIDIexternalPlayerPID, &status, 0);
    
    /* Don't unlink - soundplayer daemonizes and the file is still needed by the grandchild */
    /* unlink(MIDIexternalPlayFileName); */
    free(MIDIexternalPlayFileName);
    MIDIexternalPlayFileName = NULL;
    MIDIexternalPlayerPID = 0;
    signal(SIGCHLD, SIG_DFL);
}

Widget Midi_GetWidgetFromPointerPos(void)
{
Window       Root;
Window       Child;
int          RootX, RootY, WinX, WinY;
unsigned int mask;
Widget	     w;

BEGIN("Midi_GetWidgetFromPointerPos");

	XQueryPointer(display, RootWindowOfScreen(XtScreen(topLevel)), &Root, &Child,
		      &RootX, &RootY, &WinX, &WinY, &mask);

	w = XtWindowToWidget(display, Child);


RETURN_WIDGET(w);
}

Widget Midi_GetShellWidget(Widget w)
{
BEGIN("Midi_GetShellWidget");

	if (w == RoseLabel) w = Midi_GetWidgetFromPointerPos();

	while(XtParent(w)) w = XtParent(w);

RETURN_WIDGET(w);
}
	

void Midi_RecordCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_RecordCB");

    w = Midi_GetShellWidget(w);

    if (w != topLevel) END;

    if (Midi_SeqReadTrack()==True) END;
    else
    {
        XtAppContext appContext;

        XFlush(display);
        appContext = XtWidgetToApplicationContext(topLevel);
        while(XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);
    }

    Midi_LeaveMenuMode(RecordMode);

END;
}


void Midi_FileInfoCB(Widget w, XtPointer a, XtPointer b)
{
char   *FileFormat = 0;
char 	InfoBuffer[1024];
int	i, NumEvents;

BEGIN("Midi_FileInfoCB");

	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	switch(MIDIHeaderBuffer.Format)
	{
	case MIDI_SINGLE_TRACK_FILE:

		FileFormat = "1: Single Track File.";
		break;

	case MIDI_SIMULTANEOUS_TRACK_FILE:

		FileFormat = "2: Simultaneous Tracks.";
		break;

	case MIDI_SEQUENTIAL_TRACK_FILE:

		FileFormat = "3: Sequential Tracks.";
		break;

        case MIDI_NO_FILE_LOADED:

		YQuery(topLevel, "There is no file loaded.", 1, 0, 0, "OK",
		       "Sequencer File - Info");
		END;
	}

	NumEvents = 0;

	for (i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
	{
		NumEvents += Length(MIDITracks[i]);
	}

	sprintf(InfoBuffer, 
		"%s\n\n	File Format %s\n\nNumber Of Tracks: %d\n\nTimebase: %d\n\nTotal No. of Events: %d\n",
		MIDIFileName, FileFormat, MIDIHeaderBuffer.NumTracks, 
		MIDIHeaderBuffer.Timing.Division, NumEvents);

	YQuery(topLevel, InfoBuffer, 1, 0, 0, "OK", "Sequencer File - Info");

END;
} 



void Midi_ShowClipboardCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_ShowClipboardCB");

	Midi_ClipboardShowContents();

END;
}




void Midi_NotateAckCB(IL_ReturnCode Rtn)
{
BEGIN("Midi_NotateAckCB");

	if (MIDItempFileName)
	{
	  unlink(MIDItempFileName);
	  XtFree(MIDItempFileName);
	}

	MIDItempFileName = NULL;

	switch(Rtn)
	{
	case IL_NO_SUCH_SERVICE:

		YQuery(topLevel, "Notation service unavailable.", 1, 0, 0, "Continue", NULL);
		break;

	case IL_SERVICE_BUSY:

		YQuery(topLevel, "Notation service is currently busy.\nPlease try again later.", 
		       1, 0, 0, "Continue", NULL);
		break;

	case IL_SERVICE_FAILED:

		YQuery(topLevel, "Notation editor was unable to parse MIDI file.",
		       1, 0, 0, "Continue", NULL);
		break;

	case IL_SERVICE_OK:
	default:

		break;
	}
	
	Midi_SetBusy(False);
END;
}

	
void Midi_NotateCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_NotateCB");

	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	if (MIDItempFileName) 
	{
		YQuery(topLevel, "Notation Editor has yet to\nacknowledge previous request.",
		       1, 0, 0, "Continue", NULL);
		END;
	}

	if (!(MIDItempFileName = XtNewString(tmpnam(NULL))))
	{
		YQuery(topLevel, "Sorry, I couldn't get a temporary file name.",
		       1, 0, 0, "Continue", NULL);
		END;
	}
	
	Midi_SaveFile(MIDItempFileName);

	IL_RequestService(ILS_EDIT_SERVICE, Midi_NotateAckCB, MIDItempFileName, strlen(MIDItempFileName) + 1);

	Midi_SetBusy(True);
END;
}


Boolean Midi_CloseFile(void)
{
  int i;
  int resp;
  char MsgBuffer[256];
  char *SaveFileName;

  BEGIN("Midi_CloseFile");
	
  if (MIDIfileModified) {

    if (MIDIneverSaved) {

      resp = YQuery(topLevel, "Save changes to current MIDI file?",
		    3, 0, 2, "Yes", "No", "Cancel", NULL);

      if (resp == 2) RETURN_BOOL(False);
      else if (resp == 0) {

	SaveFileName = YFileGetWriteFilename
	  (topLevel, "Sequencer File - Save", ".mid", "MIDI");
	    
	if (SaveFileName) Midi_SaveFile(SaveFileName);
	else if (MIDIinServitude) {
	  IL_AcknowledgeRequest(ILS_SEQUENCE_SERVICE, IL_SERVICE_BUSY);
	  RETURN_BOOL(True);
	}
       }
    }
    else {

      sprintf(MsgBuffer, "Save changes to MIDI file `%s'?", MIDIFileName);
	  
      resp = YQuery(topLevel, MsgBuffer,
		    3, 0, 2, "Yes", "No", "Cancel", NULL);
	  
      if (resp == 2) RETURN_BOOL(False);
      else if (resp == 0) {
	Midi_SaveFile(MIDIFileName);
      }
    }
  }

  if (MIDIHeaderBuffer.Format != MIDI_NO_FILE_LOADED)
  {
    for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
    {
	Midi_TrackDelete(MIDITracks[i]);
	MIDITracks[i] = NULL;
    }
	
      if (MIDIFile)  /* yuerch.  but hey */
          Midi_FileClose(MIDIFile);	
  }

  Midi_EventListDeleteAllWindows();
  Midi_PianoRollDeleteAllWindows();
  
  /* Reset MIDI state - silence all notes and controllers */
  Mapper_Reset();
  
  MIDIHeaderBuffer.Format    = MIDI_NO_FILE_LOADED;
  MIDIHeaderBuffer.NumTracks = 0;
  
  Midi_SetTimeField(0);
  Midi_ResetTimingInformation();
  Midi_TrackListSetup();
  Midi_EnterMenuMode(NoFileLoadedMode);
  
  RETURN_BOOL(True);
}


void Midi_CloseCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_CloseCB");


	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	Midi_CloseFile();

END;
}

void Midi_LoadFile2(char *FileName, FILE *fp, Boolean DispMsgs)
{
MIDIHeaderChunk LoadingBuffer;
MIDIFileHandle	NewFile;
short		i;
char		TrackBuff[24];
Cursor		SandsOfTime;

BEGIN("Midi_LoadFile2");

	Midi_SetBusy(True);

	if ((NewFile = Midi_FileOpen2(FileName, fp, &LoadingBuffer, MIDI_READ)) == NULL)
	{
		Midi_SetBusy(False);
		END;
	}

	MIDIHeaderBuffer = LoadingBuffer;

	MIDIFileName = FileName;

	MIDIFile = NewFile;

	MIDITracks = (EventList *)XtMalloc(MIDIHeaderBuffer.NumTracks * sizeof(EventList));

	for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
	{
		Midi_FileSkipToNextChunk(MIDIFile, MIDI_TRACK_HEADER);

		if (DispMsgs)
		{
			sprintf(TrackBuff, "Reading Track %d of %d", i, MIDIHeaderBuffer.NumTracks - 1);
			Midi_DisplayPermanentMessage(TrackBuff);

			while(XtAppPending(appContext))
			{
				XtAppProcessEvent(appContext, XtIMAll);
			}
		}

		SandsOfTime = HourglassAnimCur[(int)((float)i/MIDIHeaderBuffer.NumTracks * HOUR_FRAMES)];

		XDefineCursor(display, XtWindow(TrackListBox), SandsOfTime);

		MIDITracks[i] = Midi_FileReadTrack(MIDIFile);
		Midi_TrackAggregateDeltas(MIDITracks[i]);
		Midi_TrackConvertToOnePointRepresentation(MIDITracks[i]);
	}

	Midi_TrackListSetup();
	Midi_LeaveMenuMode(NoFileLoadedMode);

	/* Extract initial tempo from track 0 for hardware initialization */
	if (MIDITracks && MIDIHeaderBuffer.NumTracks > 0) {
		EventList ev;
		InitialTempo = 500000;  /* default 120 BPM */
		for (ev = (EventList)First(MIDITracks[0]); ev; ev = (EventList)Next(ev)) {
			if (ev->Event.EventCode == MIDI_FILE_META_EVENT &&
			    ev->Event.EventData.MetaEvent.MetaEventCode == MIDI_SET_TEMPO) {
				unsigned char *bytes = &(ev->Event.EventData.MetaEvent.Bytes);
				InitialTempo = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
				break;  /* Use first tempo event */
			}
		}
	}

	if (DispMsgs)
  {
        Midi_DisplayPermanentMessage(" ");
  }

	Midi_SetBusy(False);
	Midi_SetFileModified(False);
END;
}

void Midi_LoadCB(Widget w, XtPointer a, XtPointer b)
{
String 		FileName;

BEGIN("Midi_LoadCB");


	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	FileName = YFileGetReadFilename(topLevel, "Sequencer File - Open",".mid","MIDI");

	if (!FileName) END;

	if (MIDIHeaderBuffer.Format != MIDI_NO_FILE_LOADED)
	  if (!Midi_CloseFile()) END;

	Midi_LoadFile(FileName, True);
        Midi_ResetTimingInformation();
        Midi_SetTimeField(0);
	MIDIneverSaved = False;

	Midi_SetTitleBar();
END;
}


void Midi_SaveFile2(char *SaveFileName, FILE *fp)
{
short          i;
MIDIFileHandle SaveFile;
EventList      ExpandedTrack;

BEGIN("Midi_SaveFile");

	Midi_SetBusy(True);

	if ((SaveFile = Midi_FileOpen2(SaveFileName, fp, &MIDIHeaderBuffer, MIDI_WRITE)) == NULL)
	{
		Midi_SetBusy(False);
		YQuery(topLevel, "Unable to open output file.", 1, 0, 0, "Continue", NULL);
		END;
	}

	for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
	{
		ExpandedTrack = Midi_TrackConvertToTwoPointRepresentation(MIDITracks[i]);
		Midi_FileWriteTrack(SaveFile, ExpandedTrack);
		Midi_TrackDelete(ExpandedTrack);
	}

	Midi_FileClose(SaveFile);
	Midi_SetBusy(False);

END;
}

void Midi_SaveAsCB(Widget w, XtPointer a, XtPointer b)
{
String SaveFileName;

BEGIN("Midi_SaveAsCB");


	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	SaveFileName = YFileGetWriteFilename(topLevel, "Sequencer File - Save As",".mid","MIDI");

	if (!SaveFileName) END;
	Midi_SaveFile(SaveFileName);
	XtFree(MIDIFileName);
	MIDIFileName = SaveFileName;
	MIDIneverSaved = False;
	Midi_SetTitleBar();
END;
}

void Midi_SaveCB(Widget w, XtPointer a, XtPointer b)
{
String message;
BEGIN("Midi_SaveCB");

        w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	if (MIDIFileName)
	{
	        message = (String)XtMalloc(strlen(MIDIFileName) + 17);
	        sprintf(message, "Save file `%s' ?", MIDIFileName);
	        if (YQuery(topLevel, message, 2, 0, 1,
			   "Yes", "No", "Sequencer File - Save") == 1)
		  {
		    XtFree(message); END;
		  } else XtFree(message);

		Midi_SaveFile(MIDIFileName);
		MIDIneverSaved = False;
	}
	else Midi_SaveAsCB(w, a, b);

END;
}

void Midi_SetTimebaseCB(Widget w, XtPointer a, XtPointer b)
{
char 	OldTimebase[12];
char   *NewTimebaseStr;
short	NewTimebase;

BEGIN("Midi_SetTimebaseCB");


	w = Midi_GetShellWidget(w);

	if (w != topLevel) END;

	sprintf(OldTimebase, "%hd", MIDIHeaderBuffer.Timing.Division);

	NewTimebaseStr = YGetUserInput(topLevel, "Set Timebase for file to:", 
				       OldTimebase, YOrientHorizontal, "Midi - Set Timebase");

	if (NewTimebaseStr == NULL) END;

	NewTimebase = (short)atoi(NewTimebaseStr);

	MIDIHeaderBuffer.Timing.Division = NewTimebase;

END;
}

void Midi_PlayCB(Widget w, XtPointer a, XtPointer b)
{
  BEGIN("Midi_PlayCB");

  Midi_LeaveMenuMode(PlaybackMode);
  Midi_EnterMenuMode(NotPlayingMode);

  w = Midi_GetShellWidget(w);

  if (w != topLevel) END;

  /* Midi_SeqPlayFile() should return 0 for "success, and it's playing
     now" (ie. in multithreaded code) or 1 for "failure" or "too late,
     I've already done it" (in single-threaded code). */

  /* rwb 10/97 - forget the threading as we're not using it now.
      Midi_SeqPlayFile has the control - we can simplify this.  */

  if (Midi_SeqPlayFile() == True) END;
  else {
    XtAppContext appContext;

    XFlush(display);
    appContext = XtWidgetToApplicationContext(topLevel);
    while(XtAppPending(appContext)) {
      XtAppProcessEvent(appContext, XtIMAll);
    }
  }

  END;
}

void Midi_InitialPatchesDlgCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_InitialPatchesDlgCB");

    Midi_InitialPatchesDlg();

END;
}

void Midi_StopCB(Widget w, XtPointer a, XtPointer b)
{
  BEGIN("Midi_StopCB");

  Midi_SeqStopPlayingCB(w, a, b);
  Midi_LeaveMenuMode(PlaybackMode);
  Midi_EnterMenuMode(NotPlayingMode);

  END;
}

void Midi_MenuExitCB(Widget w, XtPointer a, XtPointer b)
{
  BEGIN("Midi_MenuExitCB");

  Midi_StopCB(w, a, b);

  if (MIDIfileModified)
  {
      if (Midi_CloseFile() == True)
          Midi_ExitCleanly();
  }
  else
  {
      Midi_QuitCB(w, a, b);
  }

  END;
}

void Midi_ExternalPlayCB(Widget w, XtPointer a, XtPointer b)
{
    char 	player[128];
    char   *playerRtn;
    char   *argList[16];
    int     count;
    
    BEGIN("Midi_ExternalPlayCB");
    
    strcpy(player, appData.externalPlayer);

    playerRtn = YGetUserInput(topLevel, "External Player command string:",
                              player, YOrientHorizontal,
			      "Sequencer File - Play through Slave");

    if (!playerRtn) END;

    /***************************/
    /* Create a temporary file */
    /***************************/
    
	if (MIDIexternalPlayFileName) 
	{
		YQuery(topLevel, "External player currently running.",
		       1, 0, 0, "Continue", NULL);
		END;
	}

	{
		char *tmpName;
		char tmpBuf[256];
		
		tmpName = tempnam("/tmp", "rose");
		if (!tmpName)
		{
			YQuery(topLevel, "Sorry, I couldn't get a temporary file name.",
			       1, 0, 0, "Continue", NULL);
			END;
		}
		
		sprintf(tmpBuf, "%s.mid", tmpName);
		free(tmpName);
		MIDIexternalPlayFileName = XtNewString(tmpBuf);
	}
    
	Midi_SaveFile(MIDIexternalPlayFileName);
	if (access(MIDIexternalPlayFileName, F_OK) != 0) {
		fprintf(stderr, "ERROR: Temp file %s was not created\n", MIDIexternalPlayFileName);
		YQuery(topLevel, "Unable to create temporary MIDI file for external player.", 
		       1, 0, 0, "Continue", NULL);
		END;
	}

    
    argList[0] = strtok(playerRtn, " \t\n");

    count = 1;
    
    while((argList[count] = strtok(NULL, " \t\n")))
    {
        ++count;
    }
    
    
    argList[count] = MIDIexternalPlayFileName;
    argList[count + 1] = NULL;

    MIDIexternalPlayerPID = fork();

    if (MIDIexternalPlayerPID == 0)
    {
        execvp(argList[0], argList);
        perror("execvp");
        _exit(1);
    }
    
    signal(SIGCHLD, Midi_ExternalPlayerExitCB);

    END;
}

void Midi_UndoCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_UndoCB");

	Midi_UndoLastOperation();

END;
}


YMenuElement FileMenu[] =
{
  { "Record",
#ifdef SYSTEM_SILENT
    EveryMode,
#else
    NullMode | PlaybackMode,
#endif
    Midi_RecordCB, record_bits, NULL, },
  { "Open . . .",    PlaybackMode, Midi_LoadCB,   open_bits,   NULL, },
  YMenuDivider,
  { "Close",         NoFileLoadedMode | PlaybackMode, Midi_CloseCB, NULL, },
  { "Save",          NoFileLoadedMode | PlaybackMode, Midi_SaveCB,save_bits, NULL, },
  { "Save As . . .", NoFileLoadedMode | PlaybackMode, Midi_SaveAsCB,	NULL, },
  YMenuDivider,
  { "Return to Start", NoFileLoadedMode | RecordMode, Midi_RewindTimerCB,
      rrwd_bits, NULL, },
  { "Rewind", NoFileLoadedMode | RecordMode, Midi_RwdTimerCB,  rwd_bits,
      NULL, },
  { "Play",
#ifdef SYSTEM_SILENT
    EveryMode,
#else
    NoFileLoadedMode | PlaybackMode,
#endif
    Midi_PlayCB, sequence_bits, NULL, },
  { "Stop",  NotPlayingMode, Midi_StopCB,  stop_bits, NULL, },
  { "Fast Forward",  NoFileLoadedMode | RecordMode, Midi_FfwdTimerCB,
     ffwd_bits, NULL, },
  { "Skip to End", NoFileLoadedMode | RecordMode, Midi_SkiptoEndCB, skp_bits,
     NULL, },
  YMenuDivider,
  { "Export CSound . . .",	NoFileLoadedMode | PlaybackMode, 	Midi_2CsoundCB,	NULL, },
  { "Play through Slave . . .",  NoFileLoadedMode | PlaybackMode,  Midi_ExternalPlayCB, NULL, },
  YMenuDivider,
  { "Notate!",		NoFileLoadedMode | PlaybackMode,	Midi_NotateCB,	beam_bits, NULL, },
  { "Exit",          NullMode,     Midi_MenuExitCB,	  NULL },
};

YMenuId	FileMenuId;

#ifdef NOT_DEFINED
YMenuElement	ToolsMenu[] =
{
  { "Notate!",		NoFileLoadedMode | PlaybackMode,	Midi_NotateCB,	beam_bits, NULL, },
  { "Export CSound . . .",	NoFileLoadedMode | PlaybackMode, 	Midi_2CsoundCB,	NULL, },
  { "External MIDI Player . . .",  NoFileLoadedMode | PlaybackMode,  Midi_ExternalPlayCB, NULL, },
  /*YMenuDivider,
  { "Preferences . . .",	PlaybackMode,		Unimplemented,	NULL },*/
};

YMenuId ToolsMenuId;
#endif


YMenuElement	EditMenu[] =
{
  { "Undo",	NoFileLoadedMode | PlaybackMode | NothingDoneMode,	Midi_UndoCB,		undo_bits, NULL, },
  { "Delete",	NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchDeleteCB,	ninja_cross_bits, NULL, },
  YMenuDivider,
  { "Cut",	NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchCutCB,	cut_bits, NULL, },
  { "Copy",	NoFileLoadedMode | PlaybackMode | NothingSelectedMode,	Midi_DispatchCopyCB,	copy_bits, NULL, },
  { "Paste",	NoFileLoadedMode | PlaybackMode | NothingCutMode,	Midi_DispatchPasteCB,	paste_bits, NULL, },
  { "Show Clipboard",	PlaybackMode,				Midi_ShowClipboardCB,	NULL },
};
  
YMenuId EditMenuId;
  
  
  
YMenuElement	TrackMenu[] =
{
  { "Show Event List",     NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackEventListCB,	  event_list_bits, NULL, },
  { "Show Piano Roll",     NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackPianoRollCB,	  piano_roll_bits, NULL, },
  YMenuDivider,
  { "Mute All Tracks", NoFileLoadedMode | PlaybackMode, Midi_MuteAllTracksCB, NULL, },
  { "Activate All Tracks", NoFileLoadedMode | PlaybackMode, Midi_ActivateAllTracksCB, NULL, },
  { "Change All to Device . . .", NoFileLoadedMode | PlaybackMode, Midi_ChangeTracksToDeviceCB, NULL, },
  YMenuDivider,
  { "Rename . . .",	         NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackRenameCB,	  NULL, },
  { "Track Info",	 NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackInfoCB,		  NULL, },
  YMenuDivider,
  { "Clone",	         NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackCloneCB,		  NULL, },
  { "Merge . . .",	         NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackMergeCB,		  NULL, },
  YMenuDivider,
  { "Filter By Channel . . .", NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackFilterByChannelCB, NULL, },
  { "Filter By Event . . .",   NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackFilterByEventCB,   NULL, },
  { "Filter By Pitch . . .",   NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackFilterByPitchCB,	  NULL, },
#ifdef NOT_DEFINED
  YMenuDivider,
  { "Split By Channel . . .",  NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Unimplemented /* Midi_TrackSplitByChannelCB - rwb 7/97 */,		  NULL, },
  { "Split By Pitch . . .",    NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Unimplemented /* Midi_TrackSplitByPitchCB - rwb 7/97 */,		  NULL, },
#endif
  { "Change Channel . . .",    NoFileLoadedMode | PlaybackMode | NothingSelectedMode, Midi_TrackChangeChannelCB,	  NULL, },
  YMenuDivider,
  { "Quantize . . .",	         NoFileLoadedMode | PlaybackMode,			     Midi_TrackQuantizeCB,	  quantize_bits, NULL, },
  { "Transpose . . .",	 NoFileLoadedMode | PlaybackMode,			     Midi_TrackTransposeCB,	  NULL, },
};

YMenuId TrackMenuId;



YMenuElement	MidiMenu[] =
{
  { "MIDI Setup . . .",
#ifdef SYSTEM_SILENT
    EveryMode,
#else
    PlaybackMode,
#endif
    Midi_SetupCB, NULL, },
  { "Set Initial Patches . . .",
#ifdef SYSTEM_OSS
    PlaybackMode,
#else
    EveryMode,
#endif
    Midi_InitialPatchesDlgCB, NULL, },
  { "File Info",     NoFileLoadedMode | PlaybackMode, Midi_FileInfoCB,  NULL, },
  { "Set Timebase . . .", NoFileLoadedMode | PlaybackMode,	Midi_SetTimebaseCB,	NULL, },
  YMenuDivider,
  { "Reset",
#ifndef SYSTEM_SILENT
    PlaybackMode,
#else
    EveryMode,
#endif
    Midi_AllNotesOffCB, NULL, },  /* Light reset - just All Notes Off */
  { "System Reset",
#ifndef SYSTEM_SILENT
    PlaybackMode,
#else
    EveryMode,
#endif
    Midi_ResetCB, NULL, },  /* Full GM System Reset */
  YMenuDivider,
  { "System Exclusive",	
#ifndef SYSTEM_SILENT
    PlaybackMode,
#else
    EveryMode,
#endif
    Unimplemented, NULL },
};

YMenuId MidiMenuId;

extern YMenuId filterMenuId;

void Midi_InstallFileMenu(Widget File)
{
BEGIN("Midi_InstallFileMenu");

	FileMenuId = YCreateMenu(File, "File Menu", XtNumber(FileMenu), FileMenu);

END;
}


void Midi_InstallEditMenu(Widget Edit)
{
BEGIN("Midi_InstallEditMenu");

	EditMenuId = YCreateMenu(Edit, "Edit Menu", XtNumber(EditMenu), EditMenu);

END;
}

extern void InstallFilterMenu(Widget);

void Midi_InstallFilterMenu(Widget Filter)
{
BEGIN("Midi_InstallFilterMenu");

InstallFilterMenu(Filter);	/* in Filter.c */

END;
}


	
void Midi_InstallTrackMenu(Widget Track)
{
BEGIN("Midi_InstallTrackMenu");

	TrackMenuId = YCreateMenu(Track, "Track Menu", XtNumber(TrackMenu), TrackMenu);

END;
}


void Midi_InstallMidiMenu(Widget Midi)
{
BEGIN("Midi_InstallMidiMenu");

	MidiMenuId = YCreateMenu(Midi, "Midi Menu", XtNumber(MidiMenu), MidiMenu);

END;
}

void Midi_EnterMenuMode(unsigned long MenuMode)
{
BEGIN("Midi_EnterMenuMode");

	YEnterMenuMode(FileMenuId,  MenuMode);
	YEnterMenuMode(EditMenuId,  MenuMode);
	YEnterMenuMode(TrackMenuId, MenuMode);
	YEnterMenuMode(MidiMenuId,  MenuMode);
	YEnterMenuMode(filterMenuId, MenuMode);

	Midi_ELAllWindowsEnterMenuMode(MenuMode);
	Midi_PRAllWindowsEnterMenuMode(MenuMode);

	if (MenuMode == PlaybackMode) {
	  YSetValue(TrackListBox, XtNsensitive, False);
	} else if (MenuMode == NotPlayingMode) {
	  YSetValue(TrackListBox, XtNsensitive, True);
	}
END;
}


void Midi_LeaveMenuMode(unsigned long MenuMode)
{
BEGIN("Midi_LeaveMenuMode");

	YLeaveMenuMode(FileMenuId,  MenuMode);
	YLeaveMenuMode(EditMenuId,  MenuMode);
	YLeaveMenuMode(TrackMenuId, MenuMode);
	YLeaveMenuMode(MidiMenuId,  MenuMode);
	YLeaveMenuMode(filterMenuId, MenuMode);

	Midi_ELAllWindowsLeaveMenuMode(MenuMode);
	Midi_PRAllWindowsLeaveMenuMode(MenuMode);
END;
}
