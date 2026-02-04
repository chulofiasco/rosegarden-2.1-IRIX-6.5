
/* MidiIn.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Midi input functions, using libMidi.a from the Sequencer development */

/* {{{ Includes */

#include "MidiIn.h"
#include "General.h"
#include "Tags.h"
#include "Classes.h"
#include "Widgets.h"
#include "Notes.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "ItemList.h"
#include "Menu.h"
#include "GC.h"

#include <Yawn.h>

#include <MidiFile.h>
#include <MidiTrack.h>

#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>

/* }}} */
/* {{{ Handy macros */

#define DeltaToMTimeNumbers(a,b) (((a)*16L)/(b))
#define MTimeNumbersToDelta(a,b) (((a)*(b))/16L)

/* }}} */
/* {{{ Local declarations */

static EventList *events;

typedef struct _TimeSigReadRec {
  int staveNo;
  ItemList item;                /* left of time sig mark */
  TimeSignature time;
} TimeSigReadRec;

static TimeSigReadRec *timeSigsRead;
static int timeSigCount;

/* }}} */

/* {{{ Auxiliary functions for MIDI EventList manipulation */

/* Insert an event at a given delta into an aggregated-delta event list */
/* and return a pointer to the list element containing the new element, */
/* or at least to one no further on in the list.  This return strategy  */
/* is exploited in MidiSpewNonNoteEventsAcrossTracks, where this is     */
/* called repeatedly when a spewable event is found, updating the       */
/* spewed-to event list with the returned value every time so as to     */
/* reduce the seek time for the next spewed event.                      */

EventList InsertEventInList(EventList list, MIDIEvent event, long delta)
{
  EventList i;

  Begin("InsertEventInList");

  /* Both of these Trues modified from Falses by jonyi@mit.edu, */
  /* and I think he's right -- cc, 2/95                         */

  if (delta == 0)
    Return((EventList)First(Insert(Midi_EventCreateList(event, True), list)));

  for (i = list; i; i = (EventList)Next(i))
    if (i->Event.DeltaTime >= delta)
      Return((EventList)Insert(Midi_EventCreateList(event, True), i));

  Return(list);         /* if it's after the end of the list, ignore it */
}

/* }}} */
/* {{{ ItemList and Clef heuristics */

ClefTag GuessItemListClef(ItemList list)
{
  int   i;
  int   count;
  long  total;
  Pitch average;

  Begin("GuessItemListClef");

  for (list = (ItemList)First(list), total = 0L, count = 0;
       list; list = iNext(list)) {

    if (list->item->object_class == ChordClass)
      for (i = 0; i < ((Chord *)list->item)->chord.voice_count; ++i) {

        total += (long)((Chord *)list->item)->chord.voices[i].pitch;
        count ++;
      }
  }

  if (count == 0) Return(TrebleClef);

  average = total / count;

  if      (average < -6) Return(  BassClef);
  else if (average < -3) Return( TenorClef);
  else if (average <  1) Return(  AltoClef);
  else                   Return(TrebleClef);
}


ItemList PutItemListInClef(ItemList list)
{
  ItemList       i;
  Pitch          offset;
  Clef          *clef;
  int            n;
  
  Begin("PutItemListInClef");

  clef = NewClef(NULL, GuessItemListClef(list));

  switch (clef->clef.clef) {

  case  TrebleClef: offset =  0; break;
  case   TenorClef: offset =  7; break;
  case    AltoClef: offset =  7; break;
  case    BassClef: offset = 14; break;
  case InvalidClef: offset =  0; break;
  }

  list = (ItemList)First(list);

  for (i = list; i; i = iNext(i)) {

    if (i->item->object_class == ChordClass)
      for (n = 0; n < ((Chord *)i->item)->chord.voice_count; ++n)
        ((Chord *)i->item)->chord.voices[n].pitch += offset;
  }

  Return((ItemList)First(Insert(NewItemList((Item *)clef), list)));
}

/* }}} */
/* {{{ Rest Lists, for expressing gaps in the music(!) */

/* Create a list containing only rests, of length `lengthD' midi units in   */
/* time division `division'.  There ought to be a general itemlist function */
/* for this: then I could use it to implement `fill to end' as well.        */

/* There are two functions here: the first (MidiMakeRestList, the lower of  */
/* the two) does up to 2 list fills using the second (MidiMakeRestListSub): */
/* one to fill up to the next crotchet (or, in 6/8 or some other such time, */
/* the next dotted crotchet) and the other to fill in crotchet or dotted-   */
/* crotchet leaps until the end of the section needing filling.             */

ItemList MidiMakeRestListSub(long lengthD, short division, Boolean dottedTime)
{
  long    length = DeltaToMTimeNumbers(lengthD, division);
  long    curr;
  NoteTag tag;

  Begin("MidiMakeRestList");

  if (length == 0) Return((ItemList)NULL);

  if ((curr = TagToNumber(Crotchet, dottedTime)) <= length) {
    
      Return((ItemList)Nconc
             (NewItemList
              ((Item *)NewRest(NULL, Crotchet, dottedTime)),
              MidiMakeRestListSub(MTimeNumbersToDelta
                                  (length - curr, division),
                                  division, dottedTime)));
  }

  for (tag = ShortestNote + 1; tag <= Crotchet; ++tag) {

    if (TagToNumber(tag, False) > length) {

      Return((ItemList)Nconc
             (NewItemList
              ((Item *)NewRest(NULL, tag - 1, False)),
              MidiMakeRestListSub(MTimeNumbersToDelta
                                  (length - TagToNumber(tag - 1, False),
                                   division), division, dottedTime)));
    }
  }

  /* should only be reached in dotted time for lengths between crotchet
     and dotted crotchet: */

  Return((ItemList)Nconc
         (NewItemList((Item *)NewRest(NULL, Crotchet, False)),
          MidiMakeRestListSub(MTimeNumbersToDelta
                              (length - TagToNumber(Crotchet, False),
                               division), division, dottedTime)));
}


ItemList MidiMakeRestList(long delta, long lengthD,
                          short division, TimeSignature *time)
{
  Boolean  dottedTime = False;
  long     distanceToNextBeat;
  long     beatLength;

  Begin("MidiMakeRestList");

  if (time && !(time->numerator % 3) &&
      !MTimeLesser(time->bar_length, TagToMTime(Crotchet, True)))
    dottedTime = True;

  beatLength = TagToNumber(Crotchet, dottedTime);
  distanceToNextBeat =
    beatLength - (DeltaToMTimeNumbers(delta, division) % beatLength);

  if (distanceToNextBeat > DeltaToMTimeNumbers(lengthD, division)) {

    Return(MidiMakeRestListSub(lengthD, division, dottedTime));

  } else {

    Return((ItemList)Nconc

           (MidiMakeRestListSub
            (MTimeNumbersToDelta(distanceToNextBeat, division),
             division, dottedTime),

            MidiMakeRestListSub
            (lengthD - MTimeNumbersToDelta(distanceToNextBeat, division),
             division, dottedTime)));
  }
}

/* }}} */

/* {{{ Chord manufacturing */

Chord *MidiConvertEventToChord(MIDIEvent event, short division, Boolean sharps)
{
  NoteVoice *voice;
  NoteTag    tag;
  Boolean    dotted;

  Begin("MidiConvertEventToChord");

  voice  = (NoteVoice *)XtMalloc(sizeof(NoteVoice));
  *voice = MidiPitchToVoice(event->EventData.Note.Note, sharps);

  tag    = NumberToTag(DeltaToMTimeNumbers
                       (event->EventData.Note.Duration, division), &dotted);

  Return(NewChord(NULL, voice, 1, ModNone, tag, dotted));
}



void MidiAugmentChord(MIDIEvent event,
                      short division, Boolean sharps, Chord *chord)
{
  NoteVoice *voices;
  int        voicecount;
  NoteTag    tag;
  NoteTag    tag1;
  NoteTag    tag2;
  Boolean    dotted;
  Boolean    dotted1;
  Boolean    dotted2;

  Begin("MidiAugmentChord");
  
  tag1       = NumberToTag(DeltaToMTimeNumbers
                           (event->EventData.Note.Duration, division),
                           &dotted1);

  tag2       = chord->chord.visual->type;
  dotted2    = chord->chord.visual->dotted;
  voicecount = chord->chord.voice_count;

  voices = (NoteVoice *)XtRealloc
    ((char *)chord->chord.voices, (voicecount + 1) * sizeof(NoteVoice));
  voices[voicecount] = MidiPitchToVoice(event->EventData.Note.Note, sharps);

  if (tag1 >  tag2)  { tag = tag2; dotted = dotted2; }
  else               { tag = tag1; dotted = dotted1; }

  if (tag1 == tag2) dotted = (dotted1 && dotted2);

  (void)NewChord(chord, voices, voicecount + 1, ModNone, tag, dotted);

  End;
}

/* }}} */
/* {{{ Text */

Text *MidiConvertEventToText(MIDIEvent event, short division)
{
  String text;

  Begin("MidiConvertEventToText");

  text = (String)XtMalloc(1 + event->EventData.MetaEvent.NBytes);

  memcpy(text, &event->EventData.MetaEvent.Bytes,
         event->EventData.MetaEvent.NBytes);

  text[event->EventData.MetaEvent.NBytes] = '\0';

  Return(NewText(NULL, text, TextBelowStaveItalic));
}

/* }}} */
/* {{{ Key signatures */

Key *MidiConvertEventToKey(MIDIEvent event,
                           short division, Boolean *sharps)
{
  byte    code;
  int     number;
  KeyTag  tag;
  int     i;

  Begin("MidiConvertEventToKey");

  code = event->EventData.MetaEvent.Bytes + (byte)7;

  *sharps = code >= 7;
  number  = *sharps ? code - 7 : 7 - code;

  tag = KeyC;  /* Default to C major if no match found */
  for (i = 0; i < keyVisualCount; ++i)
    if (keyVisuals[i].number == number && keyVisuals[i].sharps == *sharps)
      tag = keyVisuals[i].key;

  Return(NewKey(NULL, tag));
}

/* }}} */
/* {{{ Metronome marks */

Metronome *MidiConvertEventToMetronome(MIDIEvent event, short division)
{
  int bpm;

  Begin("MidiConvertEventToMetronome");

  bpm = (int)Midi_EventConvertTempoToBPM(event);

  Return(NewMetronome(NULL, Crotchet, False, bpm));
}

/* }}} */

/* {{{ Convert track to item list */

/* When converting a MIDI track to an item list, we should keep the  */
/* deltas updated by adding in the length of the acquired note every */
/* time, not the length of the MIDI event.  That way if we're forced */
/* to use a shorter note than we should, the delta will be shorter   */
/* too and so we'll know to fill with rests the next time around.    */

ItemList MidiConvertTrackToItemList(EventList track, short division,
                                    String *name, int staveNo, int *program)
{
  long                  delta = 0;
  ItemList              items = NULL;
  EventList             eventlist;
  static Boolean        sharps;

  Begin("MidiConvertTrackToItemList");

  /* initialise for first track, carry over for the rest */
  if (track == events[0]) { sharps = True; }
 
  for (eventlist = (EventList)First(track);
       eventlist; eventlist = (EventList)Next(eventlist)) {

    if (eventlist->Event.DeltaTime > delta) {

      items = (ItemList)
        Last(Nconc(items, MidiMakeRestList
                   (delta, eventlist->Event.DeltaTime - delta,
                    division,/* &defaultTimeSignature*/ /* time*/ /*HACK*/
                    timeSigCount > 0 ? &timeSigsRead[timeSigCount-1].time :
                    &defaultTimeSignature))); /* BIG HACK */
    }

    delta = eventlist->Event.DeltaTime;

    switch(MessageType(eventlist->Event.EventCode)) {

    case MIDI_NOTE_ON:

      if (Next(eventlist) &&
          ((EventList)Next(eventlist))->Event.DeltaTime > delta &&
          ((EventList)Next(eventlist))->Event.DeltaTime <
          delta + eventlist->Event.EventData.Note.Duration) {

        eventlist->Event.EventData.Note.Duration =
          ((EventList)Next(eventlist))->Event.DeltaTime - delta;
      }

      if (Prev(eventlist) &&
          ((EventList)Prev(eventlist))->Event.DeltaTime == delta &&
          MessageType(((EventList)Prev(eventlist))->Event.EventCode) ==
          MIDI_NOTE_ON) {
          
        MidiAugmentChord(&eventlist->Event,
                         division, sharps, (Chord *)items->item);

      } else {

        items = (ItemList)Nconc
          (items, NewItemList((Item *)MidiConvertEventToChord
                              (&eventlist->Event, division, sharps)));
      }

      delta += MTimeNumbersToDelta
        (TagToNumber(((Chord *)items->item)->chord.visual->type,
                     ((Chord *)items->item)->chord.visual->dotted),
         division);

      break;

    case MIDI_PROG_CHANGE:

      if (program) *program = eventlist->Event.EventData.ProgramChange.Program;
      break;

    case MIDI_SYSTEM_MSG:
      
      if (eventlist->Event.EventCode == MIDI_FILE_META_EVENT) {

        switch(eventlist->Event.EventData.MetaEvent.MetaEventCode) {

        case MIDI_TRACK_NAME:

          if (!*name) {

            *name = (String)XtMalloc
              (1 + eventlist->Event.EventData.MetaEvent.NBytes);

            memcpy(*name, &eventlist->Event.EventData.MetaEvent.Bytes,
                   eventlist->Event.EventData.MetaEvent.NBytes);

            (*name)[eventlist->Event.EventData.MetaEvent.NBytes] = '\0';
          }

          break;

        case MIDI_TEXT_EVENT:
        case MIDI_LYRIC:
        case MIDI_TEXT_MARKER:
        case MIDI_CUE_POINT:

          items = (ItemList)Nconc
            (items, NewItemList((Item *)MidiConvertEventToText
                                (&eventlist->Event, division)));
          break;

        case MIDI_KEY_SIGNATURE:

          items = (ItemList)Nconc
            (items, NewItemList((Item *)MidiConvertEventToKey
                                (&eventlist->Event, division, &sharps)));
          break;

        case MIDI_SET_TEMPO:

          items = (ItemList)Nconc
            (items, NewItemList((Item *)MidiConvertEventToMetronome
                                (&eventlist->Event, division)));
          break;

        case MIDI_TIME_SIGNATURE:

          timeSigsRead = (TimeSigReadRec *)XtRealloc
            ((char *)timeSigsRead,
             (timeSigCount + 1) * sizeof(TimeSigReadRec));

          timeSigsRead[timeSigCount].staveNo = staveNo;
          timeSigsRead[timeSigCount].item = (ItemList)Last(items);

          (void)NewTimeSignature
            (&timeSigsRead[timeSigCount].time,
             ((byte *)(&eventlist->Event.EventData.MetaEvent.Bytes))[0],
             1 << ((byte *)(&eventlist->Event.EventData.MetaEvent.Bytes))[1]);

          ++timeSigCount;
          break;
          
        default: break;
        }
      }
    default: break;
    }
  }
  
  items = PutItemListInClef(items);
  Return(ItemListAutoBeam(&defaultTimeSignature, items, (ItemList)Last(items)));
}

/* }}} */
/* {{{ Insertion of key signatures, time sigs &c (messy) */

void MidiSpewNonNoteEventsAcrossTracks(int n)
{
  int       i;
  EventList list;

  Begin("MidiSpewNonNoteEventsAcrossTracks");

  for (list = (EventList)First(events[0]);
       list; list = (EventList)Next(list)) {

    if (list->Event.EventCode == MIDI_FILE_META_EVENT) {

      switch(list->Event.EventData.MetaEvent.MetaEventCode) {

      case MIDI_KEY_SIGNATURE:
      case MIDI_TIME_SIGNATURE:

        for (i = 1; i < n; ++i)
          events[i] =
            InsertEventInList(events[i], &list->Event, list->Event.DeltaTime);

        break;

      case MIDI_SET_TEMPO:

        events[1] =
          InsertEventInList(events[1], &list->Event, list->Event.DeltaTime);

        break;

      default: break;
      }
    }
  }

  for (i = 1; i < n; ++i) events[i] = (EventList)First(events[i]);

  End;
}


void MidiInsertTimeSignatures(MajorStave sp, int destStave, int srcStave)
{
  int i;
  Bar *bar;
  /*  ItemList il = 0, a, b;*/
  Begin("MidiInsertTimeSignatures");

  for (i = 0; i < timeSigCount; ++i) {

    if (timeSigsRead[i].staveNo == srcStave) {

      StaveResetFormatting(sp, destStave);
      StaveFormatBars(sp, destStave, -1);

      /* very inefficient */

      bar = StaveSetTimeSignatures
        (sp, destStave, timeSigsRead[i].item ? iNext(timeSigsRead[i].item) : 0,
         &timeSigsRead[i].time);

      /*
      if (bar && !a) a = (ItemList)First(bar->group.start);

      if (bar) {
        ItemListAutoBeam(&bar->bar.time, bar->group.start,
                         (ItemList)Last(bar->group.start));
      }
      */
    }
  }

  End;
}

/* }}} */
/* {{{ MakeStave */

MajorStave MidiMakeStave(int n, short division, String *name)
{
  ItemList       *music;
  String         *names;
  int            *programs;
  Boolean         firstHasNotes;
  EventList       eventlist;
  MIDIEvent       event;
  int             i;
  MajorStave      sp;

  Begin("MidiMakeStave");

  firstHasNotes = (n == 1);
  *name = NULL;

  StaveBusyMakeCount(2*n);

  for (eventlist = (EventList)First(events[0]);
       eventlist; eventlist = (EventList)Next(eventlist)) {
    
    event = &eventlist->Event;

    if (MessageType(event->EventCode) == MIDI_NOTE_ON) firstHasNotes = True;

    if (event->EventCode == MIDI_FILE_META_EVENT &&
        event->EventData.MetaEvent.MetaEventCode == MIDI_TRACK_NAME &&
        !*name) {

      *name = (String)XtMalloc(1 + event->EventData.MetaEvent.NBytes);
        
      memcpy(*name, &event->EventData.MetaEvent.Bytes,
             event->EventData.MetaEvent.NBytes);
        
      (*name)[event->EventData.MetaEvent.NBytes] = '\0';
    }

    if (*name && firstHasNotes) break;
  }

  if (firstHasNotes) {

    if (*name) XtFree(*name);
    *name = XtNewString("Midi File");

  } else {

    if (!*name) *name = XtNewString("Midi File");

    if (n == 1) {

      (void)YQuery(topLevel,
                   "This file doesn't seem to contain any actual notes.",
                   1, 0, 0, "Cancel", NULL);
      
      if (*name) XtFree(*name);
      Midi_TrackDelete(events[0]);
      XtFree((void *)events);
      StaveBusyFinishCount();
      Return(NULL);
    }
  }

  music = (ItemList *)XtMalloc(n * sizeof(ItemList));
  names = (String *)XtMalloc(n * sizeof(String));
  programs = (int *)XtCalloc(n, sizeof(int));

  if (n > 1) MidiSpewNonNoteEventsAcrossTracks(n);
  timeSigCount = 0;
  timeSigsRead = 0;

  for (i = 0; i < n; ++i) {

    StaveBusyMakeCount(2*n + i + 1);

    names[i] = NULL;
    music[i] = MidiConvertTrackToItemList(events[i], division,
                                          &names[i], i, &programs[i]);

    Midi_TrackDelete(events[i]);
  }

  StaveBusyMakeCount(3*n + 1);

  if (firstHasNotes) {

    sp = NewStave(n, music);
    for (i = 0; i < n; ++i) {
      if (names[i]) StaveRenameStave(sp, i, names[i]);
      if (programs[i]) StaveSetMIDIPatch(sp, i, programs[i]);
      MidiInsertTimeSignatures(sp, i, i);
      StaveResetFormatting(sp, i);
    }
  } else {

    DestroyItemList(music[0]); sp = NewStave(n-1, music+1);
    for (i = 1; i < n; ++i) {
      if (names[i]) StaveRenameStave(sp, i-1, names[i]);
      if (programs[i]) StaveSetMIDIPatch(sp, i-1, programs[i]);
      MidiInsertTimeSignatures(sp, i-1, i);
      StaveResetFormatting(sp, i-1);
    }
  }

  if (timeSigsRead) XtFree((void *)timeSigsRead);
  XtFree((void *)events);
  
  FillStaffsToEnd(sp, 0, firstHasNotes ? n-1 : n-2);
  StaveBusyFinishCount();
  Return(sp);
}

/* }}} */
/* {{{ ReadStave */

/* We want an animated busy cursor.   How many frames?  For each track, */
/* we have one frame for reading, one for filtering/quantizing and one  */
/* for converting.  We then need frames for spewing and formatting.     */
/* Total: 3*tracks + 2.  (Which is ideal for multiples of 2 tracks.)    */

MajorStave MidiReadStave(String fname, String *name, Boolean autoQuantize,
                         int autoQuantLevel, Boolean useQuantName)
{
  MIDIHeaderChunk header;
  MIDIFileHandle  file;
  short           timingDivision;
  int             tracks;
  int             quantizePosition;
  int             quantizeDuration;
  EventList       temp;
  EventList       temp2;
  String          msg;
  int             i;

  Begin("MidiReadStave");

  if ((file = Midi_FileOpen(fname, &header, MIDI_READ)) == NULL ||
      header.Format == MIDI_SEQUENTIAL_TRACK_FILE) {

    msg = (String)XtMalloc(300);

    if (header.Format == MIDI_SEQUENTIAL_TRACK_FILE)
      sprintf(msg, "Sorry, I can't read sequential-track MIDI files.");
    else sprintf(msg, "`%s' doesn't seem to be a standard MIDI file.", fname);

    XBell(display, 70);
    (void)YQuery(topLevel, msg, 1, 0, 0, "Cancel", NULL);

    XtFree((void *)msg);
    Return(NULL);
  }

  if ((timingDivision = header.Timing.Division) < 0) {
    
    XBell(display, 70);
    (void)YQuery
      (topLevel,
       "Sorry, this file uses SMPTE timing data, which I'm not "
       "equipped to understand.", 1, 0, 0, "Cancel", NULL);

    Return(NULL);
  }

  if ((tracks = header.NumTracks) < 1) {

    XBell(display, 70);
    (void)YQuery
      (topLevel,
       "This file appears to contain no tracks.", 1, 0, 0, "Cancel", NULL);

    Return(NULL);
  }

  if (tracks > 25) {

    XBell(display, 70);
    (void)YQuery
      (topLevel, "Sorry, I can't handle more than 25-track files.",
       1, 0, 0, "Cancel", NULL);

    Return(NULL);
  }


  if (!autoQuantize) {

    msg = (String)XtMalloc(300);
    for (i = strlen(fname) - 2; i > 0; --i) {
      if (fname[i] == '/') break;
    }

    if (useQuantName) {
      sprintf(msg, "Quantization for MIDI file `%s'",
              fname[i] == '/' ? fname+i+1 : fname);
    } else {
      sprintf(msg, "Quantization for temporary MIDI file");
    } 
    
    if (MidiChooseQuantizeLevel(msg, &quantizePosition, &quantizeDuration)
        == Failed) {
      XtFree(msg);
      Return(NULL);
    } else {
      XtFree(msg);
    }
    
    if (quantizePosition == -1) quantizePosition = MIDI_HEMIDEMISEMIQUAVER;
    if (quantizeDuration == -1) quantizeDuration = MIDI_HEMIDEMISEMIQUAVER;
    
  } else {

    if (autoQuantLevel < 0) autoQuantLevel = MIDI_HEMIDEMISEMIQUAVER;
    quantizePosition = quantizeDuration = autoQuantLevel;
  }

  StaveBusyStartCount(3*tracks + 2);

  events = (EventList *)XtMalloc(tracks * sizeof(EventList));

  for (i = 0; i < tracks; ++i) {

    StaveBusyMakeCount(2*i);

    Midi_FileSkipToNextChunk(file, MIDI_TRACK_HEADER);
    temp = Midi_FileReadTrack(file);

    StaveBusyMakeCount(2*i + 1);

    Midi_TrackAggregateDeltas(temp);
    Midi_TrackConvertToOnePointRepresentation(temp);
    
    temp2 = Midi_TrackFilterByEvent
      (temp,
       MidiNoteOnEventMask        | MidiNoteOffEventMask       |
       MidiTextEventsMask         | MidiSetTempoEventMask      |
       MidiTimeSignatureEventMask | MidiKeySignatureEventMask  |
       MidiProgChangeEventMask);

    Midi_TrackDelete(temp);

    events[i] = Midi_TrackQuantize(temp2, &header,
                                   True, quantizePosition,
                                   True, quantizeDuration);

    Midi_TrackDelete(temp2);
  }

  Midi_FileClose(file);
  Return(MidiMakeStave(tracks, timingDivision, name));
}

/* }}} */
/* {{{ Quantization requests */

/* This little lot is very closely based on the equivalent code in
   sequencer/src/TrackList.c, only using the new YOptionMenu code */


void MidiQuantizeSetValue(Widget, int);
Widget qNotePosToggle, qNotePosResMenuButton;
Widget qNoteDurToggle, qNoteDurResMenuButton;
Boolean qByPosition, qByDuration;
int qPosRes, qDurRes;

void MidiQuantizeCancel(Widget w, XtPointer a, XtPointer b)
{
  Begin("MidiQuantizeCancel");

  *((int *)a) = 0;

  End;
}

void MidiQuantizeOK(Widget w, XtPointer a, XtPointer b)
{
  Begin("MidiQuantizeOK");

  *((int *)a) = 1;

  End;
}

void MidiQuantizeButtonsCB(Widget w, XtPointer a, XtPointer b)
{
  Begin("MidiQuantizeButtonsCB");

  if (w == qNotePosToggle) {
    qByPosition = !qByPosition;
    YSetValue(qNotePosResMenuButton, XtNsensitive, qByPosition);
  } else {
    qByDuration = !qByDuration;
    YSetValue(qNoteDurResMenuButton, XtNsensitive, qByDuration);
  }

  End;
}

String qLevels[] = {
  "Semibreve", "Minim", "Crotchet", "Quaver",
  "Semiquaver", "DemiSemiquaver", "HemiDemiSemiquaver"
};

int qLevelNumbers[] = {
  MIDI_SEMIBREVE, MIDI_MINIM, MIDI_CROTCHET, MIDI_QUAVER,
  MIDI_SEMIQUAVER, MIDI_DEMISEMIQUAVER, MIDI_HEMIDEMISEMIQUAVER
};

Result MidiChooseQuantizeLevel(String prompt, int *posResRtn, int *durResRtn)
{
  Widget qDlg, qPane, qTopBox, qLabel, qForm, qBottomBox, qOK, qCancel, qHelp;
  int result = -1;

  Begin("MidiChooseQuantizeLevel");

  qDlg = XtCreatePopupShell("Quantize", transientShellWidgetClass,
                            topLevel, NULL, 0);

  qPane = YCreateWidget("Quantize Pane", panedWidgetClass, qDlg);

  qTopBox = YCreateShadedWidget("Quantize Title Box", boxWidgetClass,
                                qPane, MediumShade);

  qLabel = YCreateLabel(prompt, qTopBox);

  qForm = YCreateShadedWidget("Quantize Form", formWidgetClass,
                              qPane, LightShade);
  
  qNotePosToggle = YCreateToggle("Quantize Note Positions", qForm,
                                 MidiQuantizeButtonsCB);

  qNotePosResMenuButton =
    YCreateOptionMenu(qForm, qLevels, XtNumber(qLevels),
                      XtNumber(qLevels)-1, NULL, NULL);

  qNoteDurToggle = YCreateToggle("Quantize Note Durations", qForm,
                                 MidiQuantizeButtonsCB);

  qNoteDurResMenuButton =
    YCreateOptionMenu(qForm, qLevels, XtNumber(qLevels),
                      XtNumber(qLevels)-1, NULL, NULL);

  qBottomBox = YCreateShadedWidget("Quantize Bottom Box", boxWidgetClass,
                                   qPane, MediumShade);

  qOK = YCreateCommand("OK", qBottomBox);
  XtAddCallback(qOK, XtNcallback, MidiQuantizeOK, (XtPointer)&result);
  qCancel = YCreateCommand("Cancel", qBottomBox);
  XtAddCallback(qCancel, XtNcallback, MidiQuantizeCancel, (XtPointer)&result);

  if (appData.interlockWindow) {
    qHelp = YCreateCommand("Help", qBottomBox);
    XtAddCallback(qHelp, XtNcallback, yHelpCallbackCallback,
                  (XtPointer)"Editor File - Import MIDI");
  } else {
    qHelp = NULL;
  }

  YSetValue(XtParent(qNotePosResMenuButton), XtNfromHoriz,
            XtParent(qNoteDurToggle));
  YSetValue(XtParent(qNoteDurToggle), XtNfromVert,
            XtParent(qNotePosToggle));
  YSetValue(XtParent(qNoteDurResMenuButton), XtNfromHoriz,
            XtParent(qNoteDurToggle));
  YSetValue(XtParent(qNoteDurResMenuButton), XtNfromVert,
            XtParent(qNotePosToggle));

  YSetValue(qNotePosResMenuButton, XtNsensitive, False);
  YSetValue(qNoteDurResMenuButton, XtNsensitive, False);
  qByPosition = qByDuration = False;
  qPosRes = qDurRes = MIDI_HEMIDEMISEMIQUAVER;

  YPushPointerPosition();
  YPlacePopupAndWarp(qDlg, XtGrabNonexclusive, qOK, NULL);
  YAssertDialogueActions(qDlg, qOK, NULL, NULL);

  while (result == -1 || XtAppPending(XtWidgetToApplicationContext(qOK))) {
    XtAppProcessEvent(XtWidgetToApplicationContext(qOK), XtIMAll);
  }

  YPopdown(qDlg);

  qPosRes = qLevelNumbers[YGetCurrentOption(qNotePosResMenuButton)];
  qDurRes = qLevelNumbers[YGetCurrentOption(qNoteDurResMenuButton)];

  YDestroyOptionMenu(qNotePosResMenuButton);
  YDestroyOptionMenu(qNoteDurResMenuButton);
  XtDestroyWidget(qDlg);
  YPopPointerPosition();
  
  if (qByPosition) *posResRtn = qPosRes;
  else             *posResRtn = -1;

  if (qByDuration) *durResRtn = qDurRes;
  else             *durResRtn = -1;

  Return(result == 1 ? Succeeded : Failed);
}

/* }}} */

