/*
 * TrackList.c
 *
 */

#include "Mapper.h"
#include "Globals.h"
#include "Consts.h"
#include "EventListWindow.h"
#include "PianoRoll.h"
#include "Clipboard.h"
#include "MainWindow.h"
#include "Main.h"
#include "Menu.h"
#include "EventListMenu.h"
#include "PianoRollMenu.h"
#include "Undo.h"

#include <MidiFile.h>
#include <MidiTrack.h>
#include <MidiErrorHandler.h>

#include <Debug.h>
#include <string.h>

/********************************/
/* Private Function Prototypes. */
/********************************/

void Midi_TrackTransposeDlgDestroy(void);
void Midi_TrackTransposeDlg(int TrackNum);
void Midi_TrackFilterByEventDlg(int TrackNum);
void Midi_TrackQuantizeDlg(int TrackNum);
void Midi_QuantizeSetValue(Widget w, int Value);
void Midi_TrackFilterByPitchDlg(int TrackNum);
void Midi_TrackListRemoveTrack(int);

extern DeviceMetaInfoElement    Devices;
extern TrackMetaInfoElement     Tracks;

/****************************************/
/* Track List "No File Loaded" Message. */
/****************************************/

char *TrackListNoFileLoadedMsg[] = { "No Tracks." };

/**********************************************************************/
/* String array containing the string entries for the Track list box. */
/**********************************************************************/

char **TrackListEntries = TrackListNoFileLoadedMsg;

/*******************************************************/
/* Number of the track currently selected by the user. */
/*******************************************************/

int MIDISelectedTrack = -1;

Widget        TransposeDlg = NULL;
Widget      TransposeValue;
int        TransposeAmount;
int        TransposeTrack;
Dimension     TransposeValMaxWidth;

Widget        FilterDlg = NULL;
int        FilterTrack;
Midi_EventMask  FilterMask;
Dimension    FilterMaxWidth;

Widget         QuantizeDlg = NULL;
Widget        QuantizeNotePosResMenuButton;
Widget        QuantizeNoteDurResMenuButton;
Widget         QuantizeNotePosToggle;
Widget         QuantizeNoteDurToggle;

YMenuElement   *QuantizeNotePosResMenu;
YMenuElement   *QuantizeNoteDurResMenu;

YMenuId        QuantizeNotePosMenuId;
YMenuId        QuantizeNoteDurMenuId;

Boolean        QuantizeByPosition;
Boolean        QuantizeByDuration;

int        QuantizePosRes;
int        QuantizeDurRes;
int        QuantizeTrack;

Widget         ChannelDlg = NULL;
Widget         ChannelFromText;
Widget        ChannelToText;
int        ChannelTrackNum;

Widget        FilterByPitchDlg = NULL;
int        FilterByPitchTrackNum;
Boolean        FilterByPitchDirection;
Widget        FilterByPitchNoteField;
Widget         FilterByPitchOctaveField;


/*
 Midi_TrackListSetup: Constructs the list of elements in the track list box by 
 scanning each track in turn for Track Name and Instrument Name events. The 
 track list entry is then constructed showing the Track Name (followed by the 
 instrument name in brackets if a name was found) and inserted into the list. 

 Finally the widget is updated to reflect the new data.
*/

void Midi_TrackListSetup(void)
{
EventList    Track;
int        i, j;
static int    OldListSize = 0;
char           *TrackName, *InstrumentName, *DeviceName = 0;
char        Buffer[128], Buffer2[128];
int            dir;
int            asc;
int            dsc;
int            mwd = 0;
XCharStruct    info;
XFontStruct   *font;
DeviceList     TmpDev;        /* gl 2/97? */

BEGIN("Midi_TrackListSetup");

    /************************/
    /* Unselect everything. */
    /************************/

    XawListUnhighlight(TrackListBox);
    MIDISelectedTrack = -1;
    Midi_EnterMenuMode(NothingSelectedMode);

    if (MIDIHeaderBuffer.NumTracks == 0)
    {
        if (TrackListEntries != TrackListNoFileLoadedMsg) XtFree(TrackListEntries);
        TrackListEntries = TrackListNoFileLoadedMsg;
        YSetValue(TrackListBox, XtNlist, TrackListNoFileLoadedMsg);
        YSetValue(TrackListBox, XtNnumberStrings, 1);
        YSetValue(TrackListBox, XtNsensitive, False);
        Tracks.Track = NULL;
        END;
    }

    else

    /*****************************************************************/
    /* If we already have some track entries then re-allocate memory */
    /* to fit the new number of tracks and free all the previous     */
    /* track list entries.                         */
    /*****************************************************************/

    if (TrackListEntries != TrackListNoFileLoadedMsg)
    {
        for(i = 0; i < OldListSize; ++i)
        {
            XtFree(TrackListEntries[i]);
        }
        if (OldListSize != MIDIHeaderBuffer.NumTracks)
        {
            if (MIDIHeaderBuffer.NumTracks > 0)
            {
                if (TrackListEntries) { /* cc 95 */
                TrackListEntries = (char **)realloc((char *)TrackListEntries, 
                            MIDIHeaderBuffer.NumTracks * sizeof(char *));
                } else {
                TrackListEntries = (char **)malloc
                  (MIDIHeaderBuffer.NumTracks * sizeof(char *));
                }
            }
            else TrackListEntries = TrackListNoFileLoadedMsg;
        }
    }

    /*******************************************************/
    /* Otherwise just allocate enough memory for everyone. */
    /*******************************************************/

    else if (MIDIHeaderBuffer.NumTracks) 
    {
        TrackListEntries = (char **)XtMalloc(MIDIHeaderBuffer.NumTracks * sizeof(char *));
    }

    /* create new track entries */
    if ( Tracks.Track == NULL)
    {
        for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
            Mapper_NewTrackMetaInfo();
    }

    /*********************************************************/
    /* Scan the tracks for track and instrument name events. */
    /*********************************************************/

    for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
    {
        Track = MIDITracks[i];

        TrackName = NULL;
        InstrumentName = NULL;

        while(Track)
        {
            if (Track->Event.EventCode == MIDI_FILE_META_EVENT)
            {
                switch(Track->Event.EventData.MetaEvent.MetaEventCode)
                {
                case MIDI_TRACK_NAME:

                    TrackName = (char *)&Track->Event.EventData.MetaEvent.Bytes;
                    break;

                case MIDI_INSTRUMENT_NAME:

                    InstrumentName = 
                        (char *)&Track->Event.EventData.MetaEvent.Bytes;
                    break;
                }

                if (TrackName && InstrumentName) break;
            }

            Track = (EventList)Next(Track);
        }

        /*******************************************************************/
        /* If we haven't found a track name then print 'Un-named' instead. */
        /*******************************************************************/

        if (TrackName == NULL)
        {
            TrackName = "Un-named";
        }

        /*******************************************************************/
        /* If we've found an instrument name then print it after the track */
        /* name in brackets, otherwise just print the track name.       */
        /*******************************************************************/

        if ( Mapper_GetTrack(i) && /* fix for no-devices case, GL */
            (TmpDev = Mapper_GetDevice(Mapper_GetTrack(i)->Device)) )
                    DeviceName = TmpDev->Device.Data.Midi.name;
                else
                    DeviceName = "No Device"; /* gl 2/97 ? */

        if (InstrumentName)
        {
            sprintf(Buffer, "Track %d:   %s (%s)", i, TrackName,
                InstrumentName);

                        if (DeviceName != "" )
            {


                /* this is simpler -- Guillaume's suggestion */

                  /* rwb 8,97 - Simpler yeah but gets confused when names
                     overrun - shifting it back to the primitives for the
                     moment as those sprintf arguments make me swoon */

                  /*
                sprintf(Buffer2, "%-48.46s%.20s", Buffer, DeviceName);
                  */

                strcpy(Buffer2,"");
  
                j = 0;

                while ( Buffer[j] != '\0' && j < 46 )
                {
                    Buffer2[j] = Buffer[j];
                    j++;
                }
  
                Buffer2[j] = '\0';

                while (strlen(Buffer2) < 48 )
                    strcat(Buffer2," ");

                strcat(Buffer2, DeviceName);

                while (strlen(Buffer2) < 68 )
                    strcat(Buffer2," ");

                switch(Mapper_GetTrack(i)->PlaybackStatus)
                                {
                    case Playback_Enabled:
                    strcat(Buffer2, MIDI_TRACK_PB_TEXT);
                    break;

                    case Playback_Muted:
                    strcat(Buffer2, MIDI_TRACK_MUTE_TEXT);
                    break;

                    case Playback_Disabled:
                    strcat(Buffer2, MIDI_TRACK_DIS_TEXT);
                    break;

                    default:
                    break;
                                }

                strcpy(Buffer, Buffer2);
            }
        }
        else
        {
            sprintf(Buffer, "Track %d:   %s", i, TrackName);

                        if (DeviceName != "" )
            {
                strcpy(Buffer2,"");

                j = 0;

                while ( Buffer[j] != '\0' && j < 46 )
                {
                    Buffer2[j] = Buffer[j];
                    j++;
                }

                Buffer2[j] = '\0';

                while (strlen(Buffer2) < 48 )
                    strcat(Buffer2," ");

                strcat(Buffer2,DeviceName);

                while (strlen(Buffer2) < 68 )
                    strcat(Buffer2," ");

                switch(Mapper_GetTrack(i)->PlaybackStatus)
                                {
                    case Playback_Enabled:
                    strcat(Buffer2, MIDI_TRACK_PB_TEXT);
                    break;

                    case Playback_Muted:
                    strcat(Buffer2, MIDI_TRACK_MUTE_TEXT);
                    break;

                    case Playback_Disabled:
                    strcat(Buffer2, MIDI_TRACK_DIS_TEXT);
                    break;

                    default:
                    break;
                                }

                strcpy(Buffer, Buffer2);
            }
        }

        TrackListEntries[i] = strcpy((char *)malloc(strlen(Buffer2) + 1), Buffer2);


    }

    YGetValue(TrackListBox, XtNfont, &font);

    for (i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
    {

        XTextExtents(font, TrackListEntries[i], strlen(TrackListEntries[i]),
                  &dir, &asc, &dsc, &info);

            if (info.width > mwd) mwd = info.width;
      }

    /**********************/
    /* Update the widget. */
    /**********************/

    XtUnmanageChild(TrackListBox);

    YSetValue(TrackListBox, XtNlist, TrackListEntries);
    YSetValue(TrackListBox, XtNnumberStrings, MIDIHeaderBuffer.NumTracks);
    YSetValue(TrackListBox, XtNlongest, mwd + 12);

/*
        YSetValue(TrackListInfoBox, XtNwidth, 200);
        YSetValue(TrackListInfoBox, XtNlist, TrackListEntries);
        YSetValue(TrackListBox, XtNwidth, 200);
*/

    XtManageChild(TrackListBox);

    /**************************************/
    /* Store the number of entries in the */
    /* track list so we know how many to  */
    /* free the next time we are called.  */
    /**************************************/

    OldListSize = MIDIHeaderBuffer.NumTracks;

END;
}

void Midi_TrackListSetupDefaults()
{
BEGIN("Midi_TrackListSetupDefaults");

    MIDIHeaderBuffer.NumTracks       = 0;
    MIDIHeaderBuffer.Format          = MIDI_SIMULTANEOUS_TRACK_FILE;
    MIDIHeaderBuffer.Timing.Division = 480;
    Midi_SetFileModified(True);
    Midi_LeaveMenuMode(NoFileLoadedMode);
    YSetValue(TrackListBox, XtNsensitive, True);
END;
}

    


void Midi_TrackListCopyCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_CopyCB");

    w = Midi_GetShellWidget(w);
    if (w != topLevel)
    {
        if (Midi_ELGetWindowFromWidget(w))
        {
            Midi_ELCopyCB(w, a, b);
        }
        else if (Midi_PRGetWindowFromWidget(w))
        {
            Midi_PRCopyCB(w, a, b);
        }
    }
    else
    {
        Midi_ClipboardCopy(MIDITracks[MIDISelectedTrack], 
                   (EventList)Last(MIDITracks[MIDISelectedTrack]));
    }

END;
}


void Midi_TrackListCutCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackListCutCB");

    Midi_UndoRecordOperation(Midi_TrackClone(MIDITracks[MIDISelectedTrack]), 
                 NULL, MIDISelectedTrack);
    Midi_ClipboardSet(MIDITracks[MIDISelectedTrack]);
    Midi_TrackListRemoveTrack(MIDISelectedTrack);
    Midi_SetFileModified(True);

END;
}


/*********************************************************************************/
/* Midi_TrackListAddTrack: Add a new track to the track list. Space is allocated */
/* for an extra element in the list of MIDI tracks and the event list pointer    */
/* passed in is assigned to the last element of this new array.          */
/*********************************************************************************/

void Midi_TrackListAddTrack(EventList NewBoy)
{
BEGIN("Midi_TrackListAddTrack");

    if (MIDITracks) {   /* cc '95, Sun's realloc fails when passed NULL */
        MIDITracks = (EventList *)realloc
          ((char *)MIDITracks,
           (++MIDIHeaderBuffer.NumTracks) * sizeof (EventList));
    } else {
        MIDITracks = (EventList *)malloc
          ((++MIDIHeaderBuffer.NumTracks) * sizeof (EventList));
    }

    MIDITracks[MIDIHeaderBuffer.NumTracks - 1] = NewBoy;
    Midi_TrackListSetup();

END;
}


void Midi_TrackListPasteCB(Widget w, XtPointer a, XtPointer b)
{
EventList Track, CloneStart, Clone, ClonedEvent;

BEGIN("Midi_TrackListPasteCB");
    
    Clone      = NULL;
    CloneStart = NULL;

    Track = Midi_ClipboardGet();

    while (Track)
    {
        ClonedEvent = (EventList)Midi_EventCreateList( &Track->Event, True );    

        if (Clone)
        {
            Clone = (EventList)Nconc(Clone, ClonedEvent);
        }
        else
        {
            Clone = ClonedEvent;
            CloneStart = Clone;
        }

        Track = (EventList)Next(Track);
    }

    Midi_TrackListAddTrack(CloneStart);

    Midi_SetFileModified(True);
END;
}


void Midi_TrackListDeleteCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackListDeleteCB");

    if (YQuery(topLevel, "Delete Track cannot be undone.\nProceed?", 2, 0, 1, "Ok", "Cancel", NULL))
    {
        END;
    }

    Midi_TrackDelete(MIDITracks[MIDISelectedTrack]);
    Midi_TrackListRemoveTrack(MIDISelectedTrack);
    Midi_SetFileModified(True);

END;
}


/******************************************************************************/
/* Midi_TrackListRemoveTrack: Remove the specified track from the track list. */
/******************************************************************************/

void Midi_TrackListRemoveTrack(int DoomedTrack)
{
int i;
EventList *NewTrackList;
TrackInfo  TempTracks;

BEGIN("Midi_TrackListRemoveTrack");

    Midi_RemovePianoRollWindow(DoomedTrack);
    Midi_RemoveEventListWindow(DoomedTrack);

    if (MIDIHeaderBuffer.NumTracks == 1)
    {
        XtFree(MIDITracks);
        MIDITracks = NULL;
        MIDIHeaderBuffer.NumTracks = 0;
        DestroyList(First(Tracks.Track));
    }
    else
    {
        NewTrackList = (EventList *)XtMalloc((MIDIHeaderBuffer.NumTracks - 1) * sizeof(EventList));

        /* update the track meta information */
        Tracks.Track = (TrackInfo)First(Tracks.Track);

        if ( DoomedTrack == 0 )
        {
            Tracks.Track = Mapper_GetTrack(1);
        }

            TempTracks = Mapper_GetTrack(DoomedTrack);
        Remove(TempTracks);

        for(i = 0; i < DoomedTrack; ++i)
        {
            NewTrackList[i] = MIDITracks[i];
        }

        for(i = DoomedTrack + 1; i < MIDIHeaderBuffer.NumTracks; ++i)
        {
            NewTrackList[i - 1] = MIDITracks[i];
        }

        --MIDIHeaderBuffer.NumTracks;

        XtFree(MIDITracks);
        MIDITracks = NewTrackList;
    }

    Midi_TrackListSetup();

END;
}



/*******************************************************/
/* Midi_TrackInfoCB:    Produce a message box showing  */
/* track information for the currently selected track. */
/*******************************************************/

void Midi_TrackInfoCB(Widget w, XtPointer a, XtPointer b)
{
int      NumEvents;
long      LastEvtTime;
byte      HighestPitch,
      LowestPitch;
char      Buffer[384];
EventList RunningPtr;

BEGIN("Midi_TrackInfoCB");

    if (MIDISelectedTrack == -1)
    {
        Error(NON_FATAL_REPORT_TO_MSGBOX, "No Track Selected.");
        END;
    }

    RunningPtr = MIDITracks[MIDISelectedTrack];

    HighestPitch = 0;
    LowestPitch  = 127;
    LastEvtTime  = 0;

    while(RunningPtr)
    {
        if (MessageType(RunningPtr->Event.EventCode) == MIDI_NOTE_ON)
        {
            if (RunningPtr->Event.EventData.Note.Note > HighestPitch)
            {
                HighestPitch = RunningPtr->Event.EventData.Note.Note;
            }

            if (RunningPtr->Event.EventData.Note.Note < LowestPitch)
            {
                LowestPitch = RunningPtr->Event.EventData.Note.Note;
            }

        }

        if (!Next(RunningPtr))
        {
            LastEvtTime = RunningPtr->Event.DeltaTime;
        }

        RunningPtr = (EventList)Next(RunningPtr);
    }
        
    NumEvents = Length(MIDITracks[MIDISelectedTrack]);

    if (HighestPitch == 0 && LowestPitch == 127)
    {
        sprintf(Buffer, "%s\n\nNo. of Events: %d\n\nNo. of Beats: %5.2f\n\nNo note events.\n", 
            TrackListEntries[MIDISelectedTrack], NumEvents, Midi_TimeToBeat(LastEvtTime));
    }
    else
    {
        sprintf(Buffer, "%s\n\nNo. of Events: %d\n\nNo. of Beats: %5.2f\n\nHighest Note: %s %d\n\nLowest Note: %s %d\n", 
            TrackListEntries[MIDISelectedTrack], NumEvents, Midi_TimeToBeat(LastEvtTime),
            Notes[HighestPitch % 12], HighestPitch / 12, Notes[LowestPitch % 12], LowestPitch / 12);
    }

    YQuery(topLevel, Buffer, 1, 0, 0, "OK", "Track - Track Info");

END;
}


static Boolean WaitingForClick = False;
static void Midi_ClickTimeoutCB(XtPointer a, XtIntervalId *b)
{
  WaitingForClick = False;
}


/*
 Midi_TrackListCB: Register when a track has been selected from the track list.
*/

void Midi_TrackListCB(Widget w, XtPointer a, XtPointer b)
{
XawListReturnStruct *ReturnStruct;

BEGIN("Midi_TrackListCB");

    ReturnStruct      = (XawListReturnStruct *)b;
    MIDISelectedTrack = ReturnStruct->list_index;

    Midi_LeaveMenuMode(NothingSelectedMode);

    WaitingForClick = True;
    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
            XtGetMultiClickTime(XtDisplay(w)),
            Midi_ClickTimeoutCB, NULL);
END;
}



void Midi_TrackListPopupTrack(int);


/*
 Midi_TrackListDetectDeselectionCB: This event-handler function traps button
 releases occurring within the track list and then determines whether the user
 has selected or deselected an element from the list. If an element was
 deselected then the menus are put back into NothingSelectedMode, and the
 selected track is set to -1.
*/

/* cc 6/96: modified to trap selection for double-click,
   and name changed accordingly.  Note TrackListCB above
   still handles single-click selections */

void Midi_TrackListSelectionCB(Widget w, XtPointer Closure, XEvent *event, Boolean *cont)
{
  int track = XawListShowCurrent(TrackListBox)->list_index;

  BEGIN("Midi_SelectionCB");

  if (cont) *cont = True;
  if (event->type != ButtonRelease) END;

  if (track == -1)
    {
      Midi_EnterMenuMode(NothingSelectedMode);
      MIDISelectedTrack = -1;
    }
  else if (track == MIDISelectedTrack && WaitingForClick)
    {
      Midi_TrackListPopupTrack(track);
    }

  END;
}


/*
===========================================================

            TRACK | RENAME

===========================================================
*/


/************************************************************************/
/* Midi_TrackRenameCB: This callback function prompts the user to enter */
/* a textual description for the selected MIDI track. This description  */
/* is then inserted in the MIDI track as a Track Name event, replacing  */
/* the current Track Name event for the track if one is present.    */
/************************************************************************/

void Midi_TrackRenameCB(Widget w, XtPointer a, XtPointer b)
{
EventList    Track, NewTrack;
MIDIEvent    NewEvent;
char            *NewName;
char           MsgBuffer[80];
char           *OldName = NULL;
int        RenamedTrack;

BEGIN("Midi_TrackRenameCB");

    /************************/
    /* Format title string. */
    /************************/

    sprintf(MsgBuffer, "Enter a new name for Track %d", MIDISelectedTrack);

    Track = MIDITracks[MIDISelectedTrack];


    /****************************************/
    /* Scan to find the current track name. */
    /****************************************/

    while(Track)
    {
        if (Track->Event.EventCode == MIDI_FILE_META_EVENT &&
            Track->Event.EventData.MetaEvent.MetaEventCode == MIDI_TRACK_NAME)
        {
            OldName = (char *)&Track->Event.EventData.MetaEvent.Bytes;
            break;
        }

        Track = (EventList)Next(Track);
    }

    /*************************************************/
    /* If the track has no name then use "Un-named". */
    /*************************************************/

    if (OldName == NULL)
    {
        OldName = "Un-named";
    }


    /****************************************************************/
    /* Prompt for a new name using the current name as the default. */
    /****************************************************************/

    NewName = YGetUserInput(topLevel, MsgBuffer, OldName, YOrientVertical, "Track - Rename");

    if (NewName == NULL) END;


    /**************************************/
    /* Build a new Track Name Meta-event. */
    /**************************************/

    NewEvent = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct) + strlen(NewName) + 1);
    NewEvent->EventCode = MIDI_FILE_META_EVENT;
    NewEvent->DeltaTime = 0;
    NewEvent->EventData.MetaEvent.MetaEventCode = MIDI_TRACK_NAME;
    NewEvent->EventData.MetaEvent.NBytes = strlen(NewName);
    strcpy((char *)&NewEvent->EventData.MetaEvent.Bytes, NewName);

    NewTrack = Midi_EventCreateList(NewEvent, True);

    MIDITracks[MIDISelectedTrack] = (EventList)First(Nconc(NewTrack, 
                             MIDITracks[MIDISelectedTrack]));

    if (Track) Remove(Track);

    XtFree(NewEvent);

    RenamedTrack = MIDISelectedTrack;
    Midi_TrackListSetup();

    Midi_UpdateEventListWindow(RenamedTrack);
    Midi_UpdatePianoRollWindow(RenamedTrack);
    Midi_SetFileModified(True);
END;
}


void Midi_TrackListPopupTrack(int track)
{
  BEGIN("Midi_TrackListPopupTrack");
  if (track < 0) END;
  
  if ( Midi_EventListLocateWindowShell(track) &&
      !Midi_PianoRollLocateWindowShell(track))
    {
      Midi_PianoRollWindowCreate(track);
    }
  else
    {
      Midi_EventListWindowCreate(track);
    }

  END;
}


void Midi_TrackEventListCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackEventListCB");

    if (MIDISelectedTrack == -1) END;

    Midi_EventListWindowCreate(MIDISelectedTrack);

END;
}

void Midi_TrackPianoRollCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackPianoRollCB");

    if (MIDISelectedTrack == -1) END;

    Midi_PianoRollWindowCreate(MIDISelectedTrack);

END;
}



void Midi_TrackMergeCB(Widget w, XtPointer a, XtPointer b)
{
int          MergeWithTrack;
char       *MergeWithTrackStr;
char       MsgBuffer[32];
EventList  Temp1, 
       Temp2;

BEGIN("Midi_TrackMergeCB");

    if (MIDISelectedTrack == -1) END;

    sprintf(MsgBuffer, "Merge track %d with track:", MIDISelectedTrack);

    for(;;)
    {

        MergeWithTrackStr = YGetUserInput(topLevel, MsgBuffer, 
                          NULL, YOrientHorizontal, 
                          "Track - Merge");

        if (MergeWithTrackStr)
        {
            MergeWithTrack = atoi(MergeWithTrackStr);
        } 
        else END;
    
        if (MergeWithTrack < 0 || MergeWithTrack > MIDIHeaderBuffer.NumTracks)
        {
            YQuery(topLevel, "Invalid Track Number.", 1, 0, 0, "Continue", NULL);
        }
        else break;
    }

    Temp1 = MIDITracks[MergeWithTrack];
    Temp2 = MIDITracks[MIDISelectedTrack];

    MIDITracks[MergeWithTrack] = Midi_TrackMerge(MIDITracks[MergeWithTrack], 
                             MIDITracks[MIDISelectedTrack]);

    Midi_TrackListRemoveTrack(MIDISelectedTrack);

    Midi_TrackDelete(Temp1);
    Midi_TrackDelete(Temp2);

    Midi_TrackListSetup();
    
END;
}

    
/*
===========================================================

              TRACK | TRANSPOSE

===========================================================
*/


void Midi_TrackTransposeCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackTransposeCB");

    Midi_TrackTransposeDlg(MIDISelectedTrack);

END;
}

void Midi_TrackTransposeOKCB(Widget w, XtPointer a, XtPointer b)
{
int i;

BEGIN("Midi_TrackTransposeOKCB");

    Midi_TrackTransposeDlgDestroy();
    if (TransposeTrack != -1)
    {
        Midi_TrackTranspose(MIDITracks[TransposeTrack], TransposeAmount);
        Midi_UpdateEventListWindow(TransposeTrack);
        Midi_UpdatePianoRollWindow(TransposeTrack);
    }
    else
    {
        for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
        {
            Midi_TrackTranspose(MIDITracks[i], TransposeAmount);
            Midi_UpdateEventListWindow(i);
            Midi_UpdatePianoRollWindow(i);
        }
    }
    Midi_SetFileModified(True);
END;
}


void Midi_TrackTransposeCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackTransposeCancelCB");

    Midi_TrackTransposeDlgDestroy();

END;
}


void Midi_TrackTransposeIncrementCB(Widget w, XtPointer a, XtPointer b)
{
char Buffer[24];

BEGIN("Midi_TrackTransposeIncrementCB");

    ++TransposeAmount;
    sprintf(Buffer, "%d Semitone%c", TransposeAmount, 
        (TransposeAmount == 1 || TransposeAmount == -1) ? '\0' : 's');

    XtUnmanageChild(TransposeValue);

    YSetValue(TransposeValue, XtNlabel, Buffer);
    YSetValue(TransposeValue, XtNwidth, TransposeValMaxWidth);

    XtManageChild(TransposeValue);
END;
}


void Midi_TrackTransposeDecrementCB(Widget w, XtPointer a, XtPointer b)
{
char Buffer[24];

BEGIN("Midi_TrackTransposeDecrementCB");

    --TransposeAmount;
    sprintf(Buffer, "%d Semitone%c", TransposeAmount, 
        (TransposeAmount == 1 || TransposeAmount == -1) ? '\0' : 's');

    XtUnmanageChild(TransposeValue);

    YSetValue(TransposeValue, XtNlabel, Buffer);
    YSetValue(TransposeValue, XtNwidth, TransposeValMaxWidth);

    XtManageChild(TransposeValue);
END;
}


void Midi_TrackTransposeDlgDestroy(void)
{
BEGIN("Midi_TrackTransposeDlgDestroy");

    if (TransposeDlg)
    {
        YPopPointerPosition();
        YPopdown(TransposeDlg);
        XtDestroyWidget(TransposeDlg);
        TransposeDlg = NULL;
    }

END;
}


void Midi_TrackTransposeDlg(int TrackNum)
{
Widget    TransposePane;

Widget    TransposeTopBox;
Widget    TransposeLabel;

Widget    TransposeForm;
Widget  TransposeUp;
Widget  TransposeDown;

Widget    TransposeBottomBox;
Widget  TransposeOK;
Widget  TransposeCancel;
Widget    TransposeHelp = NULL;

char     DlgTitle[32];

Dimension     w1;
XPoint        op;
Position    px, py;

BEGIN("Midi_TrackTransposeDlg");

    TransposeDlg = XtCreatePopupShell("Transpose", transientShellWidgetClass, 
                      topLevel, NULL, 0);

    TransposePane = YCreateWidget("Transpose Pane", panedWidgetClass, 
                        TransposeDlg);

    TransposeTopBox = YCreateShadedWidget("Transpose Title Box", boxWidgetClass,
                          TransposePane, MediumShade);

    if (TrackNum != -1)
    {
        sprintf(DlgTitle, "Transpose Track %d", TrackNum);
    }
    else sprintf(DlgTitle, "Transpose All Tracks");

    TransposeLabel = YCreateLabel(DlgTitle, TransposeTopBox);

    TransposeForm = YCreateShadedWidget("Transpose Form", formWidgetClass,
                        TransposePane, LightShade);

    TransposeUp   = YCreateSurroundedWidget("Transpose Up", repeaterWidgetClass, 
                        TransposeForm, NoShade, NoShade);
    TransposeDown = YCreateSurroundedWidget("Transpose Down", repeaterWidgetClass, 
                        TransposeForm, NoShade, NoShade);

    TransposeValue = YCreateLabel("XXX Semitones", TransposeForm);

    TransposeBottomBox = YCreateShadedWidget("Transpose Button Box", formWidgetClass,
                         TransposePane, MediumShade);

    TransposeOK    = YCreateCommand("OK", TransposeBottomBox);
    TransposeCancel = YCreateCommand("Cancel", TransposeBottomBox);

    YSetValue(TransposeUp, XtNbitmap, UpMap);
    YSetValue(TransposeDown, XtNbitmap, DownMap);

    YSetValue(XtParent(TransposeDown), XtNfromVert, XtParent(TransposeUp));
    YSetValue(XtParent(TransposeValue), XtNfromHoriz, XtParent(TransposeUp));
    YSetValue(XtParent(TransposeValue), XtNvertDistance, 16);

    YSetValue(XtParent(TransposeCancel), XtNfromHoriz, XtParent(TransposeOK));

    if (appData.interlockWindow)
    {
        TransposeHelp    = YCreateCommand("Help", TransposeBottomBox);
        YSetValue(XtParent(TransposeHelp),   XtNfromHoriz, XtParent(TransposeCancel));
        XtAddCallback(TransposeHelp,   XtNcallback, Midi_HelpCallback, "Track - Transpose");
    }

    XtAddCallback(TransposeOK,     XtNcallback, Midi_TrackTransposeOKCB,        NULL);
    XtAddCallback(TransposeCancel, XtNcallback, Midi_TrackTransposeCancelCB,    NULL);
    XtAddCallback(TransposeUp,     XtNcallback, Midi_TrackTransposeIncrementCB, NULL);
    XtAddCallback(TransposeDown,   XtNcallback, Midi_TrackTransposeDecrementCB, NULL);

    XtSetMappedWhenManaged(TransposeDlg, False);

    XtRealizeWidget(TransposeDlg);

    YGetValue(TransposeTopBox, XtNwidth, &w1);
    YGetValue(TransposeValue,  XtNwidth, &TransposeValMaxWidth);

    XtUnrealizeWidget(TransposeDlg);

    YSetValue(TransposeValue, XtNlabel, "0 Semitones");
    YSetValue(TransposeValue, XtNwidth, TransposeValMaxWidth);

    XtSetMappedWhenManaged(TransposeDlg, True);
  
    op = YPushPointerPosition();

     if (op.x || op.y) 
    {
            XtTranslateCoords(topLevel, (Position)0, (Position)0, &px, &py);

            if ((op.x - 30) > px) px = op.x - 30;
            else                  px = px + 10;

            if ((op.y - 30) > py) py = op.y - 30;
            else                  py = py + 10;

            if (px >  WidthOfScreen(XtScreen(TransposeDlg)) - w1 - 50)
            {
            px =  WidthOfScreen(XtScreen(TransposeDlg)) - w1 - 50;
         }

           if (py > HeightOfScreen(XtScreen(TransposeDlg)) - 70)
        {
                py = HeightOfScreen(XtScreen(TransposeDlg)) - 70;
        }

        if (px < 0) px = 0;
        if (py < 0) py = 0;

            YSetValue(TransposeDlg, XtNx, px);
        YSetValue(TransposeDlg, XtNy, py);
    }

    op = YPlacePopupAndWarp(TransposeDlg, XtGrabNonexclusive,
                TransposeOK, TransposeCancel);

    TransposeAmount = 0;
    TransposeTrack  = TrackNum;

    YAssertDialogueActions(TransposeDlg, TransposeOK,
                   TransposeCancel, TransposeHelp);
END;
}



void Midi_TrackFilterByChannelDlg(int TrackNum)
{
char           *UserInput;
int        ChannelNum;
EventList    OldTrack;

BEGIN("Midi_TrackFilterByChannelDlg");

    UserInput = YGetUserInput(topLevel, "Retain events for channel:", 
                  NULL, YOrientHorizontal, "Track - Filter By Channel");

    if (!UserInput) END;

    sscanf(UserInput, "%d", &ChannelNum);

    OldTrack = MIDITracks[TrackNum];

        /* dangling pointers? */
    MIDITracks[TrackNum] = Midi_TrackFilterByChannel(MIDITracks[TrackNum], ChannelNum);

    Midi_TrackListSetup();
    Midi_UpdateEventListWindow(TrackNum);
    Midi_UpdatePianoRollWindow(TrackNum);

    Midi_UndoRecordOperation(OldTrack, NULL, TrackNum);

END;
}

void Midi_TrackFilterByChannelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackFilterByChannelCB");

    Midi_TrackFilterByChannelDlg(MIDISelectedTrack);

END;
}


void Midi_TrackCloneMetaData(int SelectedTrack, int NewTrack)
{
BEGIN("Midi_TrackCloneMetaData");

    Mapper_NewTrackMetaInfo();

    Mapper_GetTrack(NewTrack)->Device =
    Mapper_GetTrack(SelectedTrack)->Device;

    Mapper_GetTrack(NewTrack)->PlaybackStatus =
        Mapper_GetTrack(SelectedTrack)->PlaybackStatus;

END;
}

void
Midi_TrackSplitByChannelDlg(int TrackNum)
{
char *UserInput;
int ChannelNum;
EventList OldTrack;
BEGIN("Midi_TrackSplitByChannelDlg");

    /* clone the track silently */
    Midi_TrackCloneMetaData(MIDISelectedTrack, MIDIHeaderBuffer.NumTracks);

    if (MIDITracks)
    {
        MIDITracks = (EventList *)realloc
        ((char *)MIDITracks,
        (++MIDIHeaderBuffer.NumTracks) * sizeof (EventList));
    }
    else
    {
        MIDITracks = (EventList *)malloc
        ((++MIDIHeaderBuffer.NumTracks) * sizeof (EventList));
    }

    MIDITracks[MIDIHeaderBuffer.NumTracks - 1] =
            Midi_TrackFilterByChannels(Midi_TrackClone(MIDITracks[TrackNum]),
                                        ChannelNum, False);

    UserInput = YGetUserInput(topLevel, "Split track on channel:",
                       NULL, YOrientHorizontal, "Track - Filter By Channel");

    if (!UserInput) END;

    sscanf(UserInput, "%d", &ChannelNum);

    OldTrack = MIDITracks[TrackNum];

    MIDITracks[TrackNum] =
            Midi_TrackFilterByChannels(MIDITracks[TrackNum], ChannelNum, True);

    Midi_TrackListSetup();
    Midi_UpdateEventListWindow(TrackNum);
    Midi_UpdatePianoRollWindow(TrackNum);

/*
    Midi_UndoRecordOperation(OldTrack, NULL, TrackNum);
*/

END;
}


void
Midi_TrackSplitByPitchDlg(int TrackNum)
{
BEGIN("Midi_TrackSplitByPitchDlg");
END;
}


void
Midi_TrackSplitByChannelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN(""Midi_TrackSplitByChannelCB);

    Midi_TrackSplitByChannelDlg(MIDISelectedTrack);

END;
}


void
Midi_TrackSplitByPitchCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackSplitByPitchCB");

    Midi_TrackSplitByPitchDlg(MIDISelectedTrack);

END;
}


void Midi_TrackFilterDlgDestroy(void)
{
BEGIN("Midi_TrackFilterDlgDestroy");

    if (FilterDlg)
    {
        YPopPointerPosition();
        YPopdown(FilterDlg);
        XtDestroyWidget(FilterDlg);
        FilterDlg = NULL;
    }

END;
}


void Midi_TrackFilterOKCB(Widget w, XtPointer a, XtPointer b)
{
int i;
EventList    OldTrack;

BEGIN("Midi_TrackFilterOKCB");

    Midi_TrackFilterDlgDestroy();
    if (FilterTrack != -1)
    {
        OldTrack = MIDITracks[FilterTrack];
        MIDITracks[FilterTrack] = Midi_TrackFilterByEvent(MIDITracks[FilterTrack], FilterMask);
        Midi_UpdateEventListWindow(FilterTrack);
        Midi_UpdatePianoRollWindow(FilterTrack);
        Midi_UndoRecordOperation(OldTrack, NULL, FilterTrack);
    }
    else
    {
        for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
        {
            OldTrack = MIDITracks[i];
            Midi_TrackFilterByEvent(MIDITracks[i], FilterMask);
            Midi_UpdateEventListWindow(i);
            Midi_UpdatePianoRollWindow(i);
            Midi_TrackDelete(OldTrack);
        }
    }

    Midi_SetFileModified(True);
    Midi_TrackListSetup();

END;
}


void Midi_TrackFilterCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackFilterCancelCB");

    Midi_TrackFilterDlgDestroy();

END;
}


#define MIDI_NUM_EVENT_MASKS 20

Widget    FilterButtons[MIDI_NUM_EVENT_MASKS];

char *Midi_FilterLabelStrings[MIDI_NUM_EVENT_MASKS] =
{
    "Note Off",
    "Note On",
    "Polyphonic Aftertouch",
    "Control Change",
    "Program Change",
    "Channel Aftertouch",
    "Pitch Change",
    "System Exclusive",
    "Text Event",
    "Set Tempo",
    "Set SMPTE Offset",
    "Time Signature",
    "Key Signature",
    "Sequencer Specific",
    "Copyright Notice",
    "Track Name",
    "Instrument Name",
    "Lyric",
    "Text Marker",
    "Cue Point"
};


    
void Midi_TrackFilterByEventCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackFilterByEventCB");

    Midi_TrackFilterByEventDlg(MIDISelectedTrack);

END;
}


void Midi_TrackFilterButtonsCB(Widget w, XtPointer a, XtPointer b)
{
unsigned long StateChange;
int i;

BEGIN("Midi_TrackFilterButtonsCB");

    YSetValue(w, XtNwidth, FilterMaxWidth);
    StateChange = 1;

    for(i = 0; i < MIDI_NUM_EVENT_MASKS; ++i)
    {
        if (w == FilterButtons[i])
        {
            FilterMask ^= StateChange;
            END;
        }
        StateChange = StateChange << 1;
    }
END;
}

void Midi_TrackFilterByEventDlg(int TrackNum)
{
char         TitleBuffer[64];
Widget        FilterPane;

Widget        FilterTopBox;
Widget        FilterLabel;

Widget        FilterForm;
int        i;
Widget        FilterBottomBox;
Widget      FilterOK;
Widget      FilterCancel;
Widget        FilterHelp = NULL;
Dimension    w1, w2;
XPoint        op;
Position    px, py;

BEGIN("Midi_TrackFilterByChannelCB");

    

    FilterDlg = XtCreatePopupShell("Filter", transientShellWidgetClass, 
                      topLevel, NULL, 0);

    FilterPane = YCreateWidget("Filter Pane", panedWidgetClass, 
                        FilterDlg);

    FilterTopBox = YCreateShadedWidget("Filter Title Box", boxWidgetClass,
                          FilterPane, MediumShade);

    if (TrackNum != -1)
    {
        sprintf(TitleBuffer, "Filter Track %d by event", TrackNum);
    }
    else sprintf(TitleBuffer, "Filter All Tracks");

    FilterLabel = YCreateLabel(TitleBuffer, FilterTopBox);

    FilterForm = YCreateShadedWidget("Filter Form", formWidgetClass,
                        FilterPane, LightShade);

    for(i = 0 ; i < MIDI_NUM_EVENT_MASKS; ++i)
    {
        FilterButtons[i] = YCreateToggle(Midi_FilterLabelStrings[i], FilterForm, 
                         Midi_TrackFilterButtonsCB);

        YSetValue(XtParent(FilterButtons[i]), XtNleft, XawChainLeft);
        YSetValue(XtParent(FilterButtons[i]), XtNright, XawChainLeft);
        YSetValue(XtParent(FilterButtons[i]), XtNtop, XawChainTop);
        YSetValue(XtParent(FilterButtons[i]), XtNbottom, XawChainTop);

        if (i > 0)
        {
            if (i % 10)
            {
                YSetValue(XtParent(FilterButtons[i]), XtNfromVert, 
                           XtParent(FilterButtons[i-1]));
            }

            if (i > 9)
            {
                YSetValue(XtParent(FilterButtons[i]), XtNfromHoriz, 
                           XtParent(FilterButtons[i-10]));
            }
        }
        YSetToggleValue(FilterButtons[i], True);
    }
    
    FilterBottomBox = YCreateShadedWidget("Filter Button Box", boxWidgetClass,
                         FilterPane, MediumShade);

    FilterOK    = YCreateCommand("OK", FilterBottomBox);
    FilterCancel = YCreateCommand("Cancel", FilterBottomBox);

    if (appData.interlockWindow)
    {
        FilterHelp    = YCreateCommand("Help", FilterBottomBox);
        XtAddCallback(FilterHelp,   XtNcallback, Midi_HelpCallback, "Track - Filter By Event");
    }

    XtAddCallback(FilterOK,     XtNcallback, Midi_TrackFilterOKCB,        NULL);
    XtAddCallback(FilterCancel, XtNcallback, Midi_TrackFilterCancelCB,    NULL);

    op = YPushPointerPosition();

    XtSetMappedWhenManaged(FilterDlg, False);

    XtRealizeWidget(FilterDlg);

    YGetValue(FilterForm, XtNwidth, &w1);

    FilterMaxWidth = 0;

    for (i = 0; i < MIDI_NUM_EVENT_MASKS; ++i)
    {
        YGetValue(FilterButtons[i], XtNwidth, &w2);
        if (w2 > FilterMaxWidth) FilterMaxWidth = w2;
    }

    XtUnrealizeWidget(FilterDlg);

    XtSetMappedWhenManaged(FilterDlg, True);

    for (i = 0; i < MIDI_NUM_EVENT_MASKS; ++i)
    {
        YSetValue(FilterButtons[i], XtNwidth, FilterMaxWidth);
    }

    

    if (op.x || op.y)     
    {
            XtTranslateCoords(topLevel, (Position)0, (Position)0, &px, &py);

            if ((op.x - 30) > px) px = op.x - 30;
            else                  px = px + 10;

            if ((op.y - 30) > py) py = op.y - 30;
            else                  py = py + 10;

            if (px >  WidthOfScreen(XtScreen(FilterDlg)) - w1 - 50)
            {
            px =  WidthOfScreen(XtScreen(FilterDlg)) - w1 - 50;
         }

           if (py > HeightOfScreen(XtScreen(FilterDlg)) - 70)
        {
                py = HeightOfScreen(XtScreen(FilterDlg)) - 70;
        }

        if (px < 0) px = 0;
        if (py < 0) py = 0;

            YSetValue(FilterDlg, XtNx, px);
        YSetValue(FilterDlg, XtNy, py);
    }

    YPlacePopupAndWarp(FilterDlg, XtGrabNonexclusive,
               FilterOK, FilterCancel);

    FilterTrack  = TrackNum;
    FilterMask   = MidiAllEventsMask;

    YSetValue(FilterForm, XtNwidth, FilterMaxWidth * 2 + 28);
    YSetValue(FilterDlg, XtNwidth, FilterMaxWidth * 2 + 28);
    YAssertDialogueActions(FilterDlg, FilterOK, FilterCancel, FilterHelp);

END;
}



void Midi_TrackCloneCB(Widget w, XtPointer a, XtPointer b)
{
EventList    Track;

BEGIN("Midi_TrackCloneCB");

    if (MIDISelectedTrack == -1) END;

    Track = MIDITracks[MIDISelectedTrack];

        Midi_TrackCloneMetaData(MIDISelectedTrack, MIDIHeaderBuffer.NumTracks);

    Midi_TrackListAddTrack(Midi_TrackClone(Track));

END;
}




char *QuantizeMenuStrings[] =
{
    "Semibreve",
    "Minim",    
    "Crotchet",    
    "Quaver",    
    "Semiquaver",    
    "DemiSemiquaver",
    "HemiDemiSemiquaver",
};



void Midi_QuantizeSemibreveCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeSemibreve");

    Midi_QuantizeSetValue(w, MIDI_SEMIBREVE);

END;
}

void Midi_QuantizeMinimCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeMinimCB");

    Midi_QuantizeSetValue(w, MIDI_MINIM);

END;
}

void Midi_QuantizeCrotchetCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeCrotchetCB");

    Midi_QuantizeSetValue(w, MIDI_CROTCHET);

END;
}

void Midi_QuantizeQuaverCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeQuaverCB");

    Midi_QuantizeSetValue(w, MIDI_QUAVER);

END;
}

void Midi_QuantizeSemiquaverCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeSemibreve");

    Midi_QuantizeSetValue(w, MIDI_SEMIQUAVER);

END;
}

void Midi_QuantizeDemiSemiquaverCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeSemiquaverCB");

    Midi_QuantizeSetValue(w, MIDI_DEMISEMIQUAVER);

END;
}

void Midi_QuantizeHemiDemiSemiquaverCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuantizeSemibreve");

    Midi_QuantizeSetValue(w, MIDI_HEMIDEMISEMIQUAVER);

END;
}


YMenuElement QuantizeMenu[] =
{
    { "Semibreve",        NullMode,    Midi_QuantizeSemibreveCB,        NULL, },
    { "Minim",        NullMode,    Midi_QuantizeMinimCB,            NULL, },
    { "Crotchet",        NullMode,    Midi_QuantizeCrotchetCB,        NULL, },
    { "Quaver",        NullMode,    Midi_QuantizeQuaverCB,            NULL, },
    { "Semiquaver",        NullMode,    Midi_QuantizeSemiquaverCB,        NULL, },
    { "DemiSemiquaver",    NullMode,    Midi_QuantizeDemiSemiquaverCB,        NULL, },
    { "HemiDemiSemiquaver",    NullMode,    Midi_QuantizeHemiDemiSemiquaverCB,    NULL  },
};



void Midi_QuantizeSetValue(Widget w, int Value)
{
Widget MenuButton;

BEGIN("Midi_QuantizeSetValue");

    MenuButton = XtParent(XtParent(w));

    if (MenuButton == XtParent(QuantizeNotePosResMenuButton))
    {
        QuantizePosRes = Value;
        MenuButton = QuantizeNotePosResMenuButton;

    }
    else if (MenuButton == XtParent(QuantizeNoteDurResMenuButton))
    {
        QuantizeDurRes = Value;
        MenuButton = QuantizeNoteDurResMenuButton;

    }
    else END;

    XtUnmanageChild(MenuButton);
    XtUnmanageChild(XtParent(MenuButton));

    YSetValue(MenuButton, XtNlabel, QuantizeMenuStrings[Value]);

    XtManageChild(MenuButton);
    XtManageChild(XtParent(MenuButton));

END;
}
    
void Midi_TrackQuantizeCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackQuantizeCB");

    Midi_TrackQuantizeDlg(MIDISelectedTrack);

END;
}

void Midi_TrackQuantizeCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackQuantizeCancelCB");

    YDestroyMenu(QuantizeNotePosMenuId);
    YDestroyMenu(QuantizeNoteDurMenuId);

    XtFree(QuantizeNotePosResMenu);
    XtFree(QuantizeNoteDurResMenu);

    YPopdown(QuantizeDlg);
    XtDestroyWidget(QuantizeDlg);
END;
}

void Midi_TrackQuantizeOKCB(Widget w, XtPointer a, XtPointer b)
{
EventList OldTrack;
int i;

BEGIN("Midi_TrackQuantizeOKCB");

    if (QuantizeTrack != -1)
    {
        OldTrack = MIDITracks[QuantizeTrack];
        MIDITracks[QuantizeTrack] = Midi_TrackQuantize(MIDITracks[QuantizeTrack],
                                   &MIDIHeaderBuffer, 
                                   QuantizeByPosition,
                                   QuantizePosRes,
                                   QuantizeByDuration,
                                   QuantizeDurRes);
        Midi_UpdateEventListWindow(QuantizeTrack);
        Midi_UpdatePianoRollWindow(QuantizeTrack);
        Midi_UndoRecordOperation(OldTrack, NULL, QuantizeTrack);
    }
    else
    {
        for(i = 0; i < MIDIHeaderBuffer.NumTracks; ++i)
        {
            OldTrack = MIDITracks[i];
            MIDITracks[i] = Midi_TrackQuantize(MIDITracks[i], 
                               &MIDIHeaderBuffer,
                               QuantizeByPosition,
                               QuantizePosRes,
                               QuantizeByDuration,
                               QuantizeDurRes);
            Midi_UpdateEventListWindow(i);
            Midi_UpdatePianoRollWindow(i);
            Midi_TrackDelete(OldTrack);
        }
    }

    Midi_TrackListSetup();
    Midi_TrackQuantizeCancelCB(w, a, b);
    Midi_SetFileModified(True);

END;
}


void Midi_TrackQuantizeButtonsCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackQuantizeButtonsCB");

    if (w == QuantizeNotePosToggle)
    {
        QuantizeByPosition = !QuantizeByPosition;
        YSetValue(QuantizeNotePosResMenuButton, XtNsensitive, QuantizeByPosition);
    }
    else if (w == QuantizeNoteDurToggle)
    {
        QuantizeByDuration = !QuantizeByDuration;
        YSetValue(QuantizeNoteDurResMenuButton, XtNsensitive, QuantizeByDuration);
    }
END;
}


void Midi_TrackQuantizeDlg(int TrackNum)
{
Widget QuantizePane;

Widget QuantizeTopBox;
Widget QuantizeLabel;

Widget QuantizeForm;

Widget QuantizeBottomBox;
Widget QuantizeOK;
Widget QuantizeCancel;
Widget QuantizeHelp = NULL;

char DlgTitle[32];

XPoint  op;

BEGIN("Midi_TrackQuantizeDlg");

    QuantizeDlg = XtCreatePopupShell("Quantize", transientShellWidgetClass,
                                     topLevel, NULL, 0);

    QuantizePane = YCreateWidget("Quantize Pane", panedWidgetClass, 
                                     QuantizeDlg);

    QuantizeTopBox = YCreateShadedWidget("Quantize Title Box", boxWidgetClass,
                                     QuantizePane, MediumShade);

    if (TrackNum != -1)
    {
        sprintf(DlgTitle, "Quantize Track %d", TrackNum);
    }
    else sprintf(DlgTitle, "Quantize All Tracks");

    QuantizeLabel = YCreateLabel(DlgTitle, QuantizeTopBox);

    QuantizeForm = YCreateShadedWidget("Quantize Form", formWidgetClass,
                                            QuantizePane, LightShade);

    QuantizeNotePosToggle = YCreateToggle("Quantize Note Positions",
                                  QuantizeForm, Midi_TrackQuantizeButtonsCB);

    QuantizeNotePosResMenuButton = YCreateMenuButton(
                                 "Note Position Menu Button", QuantizeForm);

    QuantizeNoteDurToggle = YCreateToggle("Quantize Note Durations",
                                 QuantizeForm, Midi_TrackQuantizeButtonsCB);

    QuantizeNoteDurResMenuButton = YCreateMenuButton(
                                 "Note Duration Menu Button", QuantizeForm);

    QuantizeBottomBox = YCreateShadedWidget("Quantize Button Box",
                    boxWidgetClass, QuantizePane, MediumShade);

    QuantizeOK = YCreateCommand("OK", QuantizeBottomBox);
    QuantizeCancel  = YCreateCommand("Cancel", QuantizeBottomBox);

    if (appData.interlockWindow)
    {
        QuantizeHelp = YCreateCommand("Help", QuantizeBottomBox);
        XtAddCallback(QuantizeHelp,   XtNcallback, Midi_HelpCallback,
                                                     "Track - Quantize");
    }

    XtAddCallback(QuantizeOK, XtNcallback, Midi_TrackQuantizeOKCB, NULL);
    XtAddCallback(QuantizeCancel, XtNcallback,
                                      Midi_TrackQuantizeCancelCB, NULL);

    YSetValue(QuantizeNoteDurResMenuButton, XtNlabel, "Hemidemisemiquaver");
    YSetValue(QuantizeNotePosResMenuButton, XtNlabel, "Hemidemisemiquaver");

    QuantizeNoteDurResMenu = (YMenuElement *)XtMalloc(sizeof(QuantizeMenu));
    memcpy(QuantizeNoteDurResMenu, QuantizeMenu, sizeof(QuantizeMenu));

    QuantizeNotePosResMenu = (YMenuElement *)XtMalloc(sizeof(QuantizeMenu));
    memcpy(QuantizeNotePosResMenu, QuantizeMenu, sizeof(QuantizeMenu));

    QuantizeNotePosMenuId = YCreateMenu(QuantizeNotePosResMenuButton,
                                            "Note Position Menu",
                                             XtNumber(QuantizeMenu),
                                             QuantizeNotePosResMenu);


    QuantizeNoteDurMenuId = YCreateMenu(QuantizeNoteDurResMenuButton,
                                            "Note Duration Menu",
                                             XtNumber(QuantizeMenu),
                                             QuantizeNoteDurResMenu);

    YSetValue(XtParent(QuantizeNotePosResMenuButton), XtNfromHoriz,
                                           XtParent(QuantizeNoteDurToggle));

    YSetValue(XtParent(QuantizeNoteDurToggle), XtNfromVert,
                                           XtParent(QuantizeNotePosToggle));

    YSetValue(XtParent(QuantizeNoteDurResMenuButton), XtNfromHoriz,
                                           XtParent(QuantizeNoteDurToggle));

    YSetValue(XtParent(QuantizeNoteDurResMenuButton), XtNfromVert,
                                           XtParent(QuantizeNotePosToggle));

    YSetValue(QuantizeNotePosResMenuButton, XtNsensitive, False);
    YSetValue(QuantizeNoteDurResMenuButton, XtNsensitive, False);

    QuantizeByDuration = False;
    QuantizeByPosition = False;

    QuantizeDurRes = MIDI_HEMIDEMISEMIQUAVER;
    QuantizePosRes = MIDI_HEMIDEMISEMIQUAVER;

    QuantizeTrack = TrackNum;

    op = YPlacePopupAndWarp(QuantizeDlg, XtGrabNonexclusive,
                                    QuantizeOK, QuantizeCancel);

    YAssertDialogueActions(QuantizeDlg, QuantizeOK,
                                    QuantizeCancel, QuantizeHelp);
END;
}


void Midi_TrackChangeChannelDlg(int);


void Midi_TrackChangeChannelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackChangeChannelCB");

    Midi_TrackChangeChannelDlg(MIDISelectedTrack);

END;
}



void Midi_TrackChangeChannelCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackChangeChannelCancelCB");

    YPopdown(ChannelDlg);
    XtDestroyWidget(ChannelDlg);

END;
}



void Midi_TrackChangeChannelOKCB(Widget w, XtPointer a, XtPointer b)
{
char     *FromStr;
char     *ToStr;

byte      From;
byte        To;
EventList OldTrack;

BEGIN("Midi_TrackChangeChannelOKCB");

    YGetValue(ChannelFromText, XtNstring, &FromStr);
    YGetValue(ChannelToText,   XtNstring, &ToStr);

    From = (byte)atoi(FromStr);
    To   = (byte)atoi(ToStr);

    if (From > 15 || To > 15) /* byte is unsigned, can't be < 0 */
    {
        YQuery(topLevel, "Channel out of range (0-15)", 1, 0, 0, "Continue", NULL);
    }
    else 
    {
        OldTrack = MIDITracks[ChannelTrackNum];
        MIDITracks[ChannelTrackNum] = Midi_TrackChangeChannel(MIDITracks[ChannelTrackNum], From, To);
        Midi_UndoRecordOperation(OldTrack, NULL, ChannelTrackNum);
        YPopdown(ChannelDlg);
        XtDestroyWidget(ChannelDlg);
        Midi_UpdateEventListWindow(ChannelTrackNum);
        Midi_UpdatePianoRollWindow(ChannelTrackNum);
    }
    Midi_SetFileModified(True);
END;
}


void Midi_TrackChangeChannelDlg(int TrackNum)
{
Widget ChannelPane;

Widget ChannelTopBox;
Widget ChannelTitle;

Widget ChannelForm;

Widget ChannelFromLabel;

Widget ChannelToLabel;

Widget ChannelBottomBox;
Widget ChannelOK;
Widget ChannelCancel;
Widget ChannelHelp = NULL;

char DlgTitle[32];

XPoint        op;

BEGIN("Midi_TrackChangeChannelDlg");

    if (TrackNum == -1) END;

    ChannelTrackNum = TrackNum;

    ChannelDlg = XtCreatePopupShell("Channel", transientShellWidgetClass, topLevel, NULL, 0);

    ChannelPane = YCreateWidget("Channel Pane", panedWidgetClass, 
                        ChannelDlg);

    ChannelTopBox = YCreateShadedWidget("Channel Title Box", boxWidgetClass,
                          ChannelPane, MediumShade);

    sprintf(DlgTitle, "Change channel for track %d", TrackNum);
    
    ChannelTitle = YCreateLabel(DlgTitle, ChannelTopBox);

    ChannelForm = YCreateShadedWidget("Channel Form", formWidgetClass,
                       ChannelPane,    LightShade);

    ChannelFromLabel = YCreateLabel("Move events on channel:", ChannelForm);

    ChannelFromText = YCreateSurroundedWidget("From Channel", asciiTextWidgetClass,
                          ChannelForm, NoShade, NoShade);

    ChannelToLabel = YCreateLabel("To channel:", ChannelForm);

    ChannelToText  = YCreateSurroundedWidget("To Channel", asciiTextWidgetClass,
                          ChannelForm, NoShade, NoShade);

    ChannelBottomBox = YCreateShadedWidget("Channel Bottom Box", boxWidgetClass,
                           ChannelPane, MediumShade);

    ChannelOK     = YCreateCommand("OK", ChannelBottomBox);
    ChannelCancel = YCreateCommand("Cancel", ChannelBottomBox);

    /******************/
    /* Add callbacks. */
    /******************/

    XtAddCallback(ChannelOK,     XtNcallback, Midi_TrackChangeChannelOKCB,     NULL);
    XtAddCallback(ChannelCancel, XtNcallback, Midi_TrackChangeChannelCancelCB, NULL);

    if (appData.interlockWindow)
    {
        ChannelHelp   = YCreateCommand("Help", ChannelBottomBox);
        XtAddCallback(ChannelHelp,   XtNcallback, Midi_HelpCallback, "Track - Change Channel");
    }
    
    /************************************/
    /* Format the form widget contents. */
    /************************************/

    YSetValue(XtParent(ChannelFromText), XtNfromHoriz, XtParent(ChannelFromLabel));
    YSetValue(XtParent(ChannelToLabel),  XtNfromVert,  XtParent(ChannelFromLabel));
    YSetValue(XtParent(ChannelToText),   XtNfromHoriz, XtParent(ChannelFromLabel));
    YSetValue(XtParent(ChannelToText),   XtNfromVert,  XtParent(ChannelFromLabel));

    YSetValue(ChannelFromText, XtNeditType, XawtextEdit);
    YSetValue(ChannelToText,   XtNeditType, XawtextEdit);

    op = YPlacePopupAndWarp(ChannelDlg, XtGrabNonexclusive,
                ChannelOK, ChannelCancel);

    YAssertDialogueActions(ChannelDlg, ChannelOK, ChannelCancel,
                   ChannelHelp);
END;
}




void Midi_TrackFilterByPitchCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackFilterByPitchCB");

    Midi_TrackFilterByPitchDlg(MIDISelectedTrack);

END;
}



void Midi_TrackFilterByPitchDirCB(Widget w, XtPointer a, XtPointer b)
{
Dimension ButtonWidth;

BEGIN("Midi_TrackFilterByPitchDirCB");

    YGetValue(w, XtNwidth, &ButtonWidth);

    FilterByPitchDirection = !FilterByPitchDirection;

    if (FilterByPitchDirection)
    {
        YSetValue(w, XtNlabel, "Above");
    }
    else YSetValue(w, XtNlabel, "Below");

    YSetValue(w, XtNwidth, ButtonWidth);

END;
}

void Midi_TrackFilterByPitchCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_TrackFilterByPitchCancelCB");

    YPopdown(FilterByPitchDlg);
    XtDestroyWidget(FilterByPitchDlg);

END;
}

void Midi_TrackFilterByPitchOKCB(Widget w, XtPointer a, XtPointer b)
{
char      *OctaveStr;
byte       Octave;
char      *NoteStr;
byte       Note;
EventList OldTrack;

BEGIN("Midi_TrackFilterByPitchOKCB");

    YGetValue(FilterByPitchOctaveField, XtNstring, &OctaveStr);

    Octave = (char)atoi(OctaveStr);

    YGetValue(FilterByPitchNoteField, XtNstring, &NoteStr);

    for (Note = 0; Note < 13; ++Note)
    {
        if (!strcasecmp(NoteStr, Notes[Note])) break;
    }

    if (Note == 13)
    {
        YQuery(FilterByPitchDlg, "Invalid Note. Please re-enter", 1, 0, 0, "Continue", NULL);
        END;
    }
    
    Note = Octave * 12 + Note;

    if (Note > 127)        /* byte is unsigned */
    {
        YQuery(FilterByPitchDlg, 
               "Note outside of valid MIDI range. Please re-enter", 
               1, 0, 0, "Continue", NULL);
        END;
    }

    OldTrack = MIDITracks[FilterByPitchTrackNum];

    MIDITracks[FilterByPitchTrackNum] = Midi_TrackFilterByPitch(MIDITracks[FilterByPitchTrackNum], 
                                    Note,
                                    FilterByPitchDirection);
     Midi_UpdateEventListWindow(FilterByPitchTrackNum);
    Midi_UpdatePianoRollWindow(FilterByPitchTrackNum);
    Midi_UndoRecordOperation(OldTrack, NULL, FilterByPitchTrackNum);
    
    YPopdown(FilterByPitchDlg);
    XtDestroyWidget(FilterByPitchDlg);
END;
}


void Midi_TrackFilterByPitchDlg(int TrackNum)
{
Widget FilterByPitchPane;
Widget FilterByPitchTopBox;
Widget FilterByPitchTitle;
Widget FilterByPitchForm;

Widget FilterByPitchDir;
Widget FilterByPitchNoteLabel;
Widget FilterByPitchOctaveLabel;

Widget FilterByPitchBottomBox;
Widget FilterByPitchOK;
Widget FilterByPitchCancel;
Widget FilterByPitchHelp = NULL;

XPoint op;
char DlgTitle[64];

BEGIN("Midi_TrackFilterByPitchDlg");

    if (TrackNum == -1) END;

    FilterByPitchTrackNum = TrackNum;

    FilterByPitchDlg = XtCreatePopupShell("FilterByPitch", transientShellWidgetClass, topLevel, NULL, 0);

    FilterByPitchPane = YCreateWidget("FilterByPitch Pane", panedWidgetClass, 
                        FilterByPitchDlg);

    FilterByPitchTopBox = YCreateShadedWidget("FilterByPitch Title Box", boxWidgetClass,
                          FilterByPitchPane, MediumShade);

    sprintf(DlgTitle, "Filter By Pitch for track %d", TrackNum);
    
    FilterByPitchTitle = YCreateLabel(DlgTitle, FilterByPitchTopBox);

    FilterByPitchForm = YCreateShadedWidget("FilterByPitch Form", formWidgetClass,
                       FilterByPitchPane,    LightShade);

    FilterByPitchDir = YCreateCommand("Above", FilterByPitchForm);

    FilterByPitchNoteLabel    = YCreateLabel("Note:",    FilterByPitchForm);
    FilterByPitchOctaveLabel   = YCreateLabel("Octave:",   FilterByPitchForm);

    FilterByPitchNoteField = YCreateSurroundedWidget("Note Pitch", asciiTextWidgetClass,
                              FilterByPitchForm, NoShade, NoShade);

    FilterByPitchOctaveField = YCreateSurroundedWidget("Note Octave", asciiTextWidgetClass,
                               FilterByPitchForm, NoShade, NoShade);


    FilterByPitchBottomBox = YCreateShadedWidget("Note Event Button Box", formWidgetClass,
                         FilterByPitchPane, MediumShade);

    FilterByPitchOK     = YCreateCommand("OK", FilterByPitchBottomBox);
    FilterByPitchCancel = YCreateCommand("Cancel", FilterByPitchBottomBox);

    YSetValue(XtParent(FilterByPitchCancel), XtNfromHoriz, XtParent(FilterByPitchOK));

    XtAddCallback(FilterByPitchOK,     XtNcallback, Midi_TrackFilterByPitchOKCB, NULL);
    XtAddCallback(FilterByPitchCancel, XtNcallback, Midi_TrackFilterByPitchCancelCB, NULL);
    XtAddCallback(FilterByPitchDir,    XtNcallback, Midi_TrackFilterByPitchDirCB, NULL);

    if (appData.interlockWindow)
    {
        FilterByPitchHelp   = YCreateCommand("Help", FilterByPitchBottomBox);
        XtAddCallback(FilterByPitchHelp,   XtNcallback, Midi_HelpCallback, "Track - Filter By Pitch");
        YSetValue(XtParent(FilterByPitchHelp),   XtNfromHoriz, XtParent(FilterByPitchCancel));
    }

    YSetValue(XtParent(FilterByPitchNoteLabel), XtNfromVert, XtParent(FilterByPitchDir));

    YSetValue(XtParent(FilterByPitchNoteField), XtNfromVert, XtParent(FilterByPitchDir));
    YSetValue(XtParent(FilterByPitchNoteField), XtNfromHoriz, XtParent(FilterByPitchOctaveLabel));

    YSetValue(XtParent(FilterByPitchOctaveLabel), XtNfromVert, XtParent(FilterByPitchNoteLabel));

    YSetValue(XtParent(FilterByPitchOctaveField), XtNfromVert, XtParent(FilterByPitchNoteField));
    YSetValue(XtParent(FilterByPitchOctaveField), XtNfromHoriz, XtParent(FilterByPitchOctaveLabel));

    YSetValue(FilterByPitchNoteField,   XtNeditType, XawtextEdit);
    YSetValue(FilterByPitchOctaveField, XtNeditType, XawtextEdit);
    
    FilterByPitchDirection = True;

    op = YPlacePopupAndWarp(FilterByPitchDlg, XtGrabNonexclusive,
                FilterByPitchOK, FilterByPitchCancel);

    YAssertDialogueActions(FilterByPitchDlg, FilterByPitchOK,
                   FilterByPitchCancel, FilterByPitchHelp);
END;
}
