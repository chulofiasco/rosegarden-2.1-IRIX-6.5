
/* MidiOut.c */

/* Musical Notation Editor for X, Chris Cannam 1994    */
/* Some Midi output fns.  Thanks to Andy for midi code */


/* {{{ Includes */

#include "General.h"
#include "Tags.h"
#include "Classes.h"
#include "Notes.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "Format.h"
#include "ItemList.h"

#include <Yawn.h>
#include <MidiFile.h>
#include <MidiTrack.h>

/* }}} */
/* {{{ Handy macro */

#define MTimeToDelta(x) ((10L * MTimeToNumber(x)) \
                         / TagToNumber(Hemidemisemiquaver, False))

/* }}} */
/* {{{ Classes and local variables for Dynamic records */

static double midiBaseAmp;
static double midiAmpGradient;
static int midiBarStart = 0;	/* JPff */
int ampBarEmphasis;	/* JPff */

/* DynamicAssoc and AmplitudeAssoc must both begin with is_dynamic & delta */

typedef struct _DynamicAssoc
{
  Boolean is_dynamic;
  long delta;
  long delta_end;
  Boolean crescendo;
  int amp_start;
  int amp_end;
} DynamicAssoc;

typedef struct _AmplitudeAssoc
{
  Boolean is_dynamic;
  long delta;
  int amp;
} AmplitudeAssoc;

/* Note that we rely on being able to copy arrays of AmpChangeAssocs
   with memcpy(), so no pointers please */

typedef union _AmpChangeAssoc
{
  Boolean is_dynamic;
  DynamicAssoc dynamic;
  AmplitudeAssoc amplitude;
} AmpChangeAssoc;

static AmpChangeAssoc **amps = NULL;
static int *ampCount, *ampNext;

/* }}} */


/* {{{ Write track zero (opening text and metronome events) */

static void MidiWriteTrackZero(MajorStave sp, void *fp, String name)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  MIDIFileHandle file = (MIDIFileHandle) fp;
  ItemList       items;
  EventList      events;
  MIDIEvent      endEvent;
  long           delta = 0;
  EventList      iTempo;

  Begin("MidiWriteTrackZero");

  events = Midi_EventCreateList
    (Midi_EventCreateTextEvt(MIDI_TRACK_NAME, 0, name), False);

  Nconc(events, Midi_EventCreateList
	(Midi_EventCreateTextEvt
	 (MIDI_TEXT_MARKER, 0,
	  "Created by the Rosegarden editor"), False));

  Nconc(events, iTempo =
	Midi_EventCreateList(Midi_EventCreateTempoEvt(0, 120), False));

  items = (ItemList)First(mstave->music[0]);

  while (items) {

    switch (items->item->object_class) {

    case MetronomeClass:
      Nconc(events, Midi_EventCreateList
	    (Midi_EventCreateTempoEvt
	     (delta, (long)
	      (((Metronome *)items->item)->metronome.setting *
	       MTimeToNumber(((Metronome *)items->item)->metronome.beat_length)
	       / TagToNumber(Crotchet, False))), False));

      if (iTempo && delta == 0) { Remove(iTempo); iTempo = NULL; }
      break;

    default: break;
    }

    delta += MTimeToDelta(items->item->methods->get_length(items->item));
    items = iNext(items);
  }

  endEvent = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));
  endEvent->DeltaTime = delta;
  endEvent->EventCode = MIDI_FILE_META_EVENT;
  endEvent->EventData.MetaEvent.MetaEventCode = MIDI_END_OF_TRACK;
  endEvent->EventData.MetaEvent.NBytes = 0;

  Nconc(events, Midi_EventCreateList(endEvent, False));
  Midi_FileWriteTrack(file, events);
  Midi_TrackDelete(events);

  End;
}

/* }}} */
/* {{{ Write a track */

static void MidiWriteTrack(String name, ItemList list,
			   StaveEltList barList, int staveNo,
			   int program, void *fp)
{
  MIDIFileHandle file = (MIDIFileHandle)fp;
  ClefTag        clef = TrebleClef;
  long           delta = 0, prevDelta, savedDelta = -1;
  EventList      events;
  EventList      events2p;
  MIDIEvent      programEvent;
  MIDIEvent      endEvent;
  ItemList       i;
  Bar           *bar;
  Bar           *nextBar;

  Begin("MidiWriteTrack");

  midiBaseAmp = 64.0;
  ampNext[staveNo] = 0;

  events = Midi_EventCreateList
    (Midi_EventCreateTextEvt(MIDI_TRACK_NAME, 0, name), False);

  if (program != 0) {
    programEvent = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));
    programEvent->EventCode = CreateMessageByte(MIDI_PROG_CHANGE, staveNo);
    programEvent->DeltaTime = 0;
    programEvent->EventData.ProgramChange.Program = program;
    Nconc(events, Midi_EventCreateList(programEvent, False));
  }

  bar = barList->bars[staveNo];
  nextBar = Next(barList) ? (((StaveEltList)Next(barList))->bars[staveNo]) : 0;

  /* First bar time signature */

  Nconc(events, Midi_EventCreateList
	(Midi_EventCreateTimeSigEvt
	 (0, bar->bar.time.numerator, bar->bar.time.denominator), False));

  /* Find the original clef */

  for (i = list; i; i = iNext(i))
    if (i->item->object_class == ClefClass) {
      clef = ((Clef *)i->item)->clef.clef;
      break;
    }

  /* Get the events */

  for (i = list; i; i = iNext(i)) {

    /* start of a new bar? */

    if (nextBar && nextBar->group.start == i) {

      if (!BarTimesEqual(bar, nextBar)) {
	Nconc(events, Midi_EventCreateList
	      (Midi_EventCreateTimeSigEvt(delta, nextBar->bar.time.numerator,
					  nextBar->bar.time.denominator),
	       False));
      }

      barList = (StaveEltList)Next(barList);
      if ((bar = nextBar)) {
	nextBar = Next(barList) ?
	  (((StaveEltList)Next(barList))->bars[staveNo]) : 0;
      }

      midiBarStart = ampBarEmphasis;
    }

    /* dynamics to consider? */

    /* first, hit the end of an old hairpin? */

    if (ampNext[staveNo] > 0 && amps[staveNo][ampNext[staveNo]-1].is_dynamic &&
	delta >= amps[staveNo][ampNext[staveNo]-1].dynamic.delta_end) {
      /*
      fprintf(stderr,"switching off, delta=%d & delta_end=%d\n",
	      delta, amps[staveNo][ampNext[staveNo]-1].dynamic.delta_end);
	      */
      midiAmpGradient = 0.0;
    }

    /* now -- new stuff? */

    if (ampNext[staveNo] < ampCount[staveNo] &&
	delta >= amps[staveNo][ampNext[staveNo]].dynamic.delta) {

      AmpChangeAssoc *amp = &amps[staveNo][ampNext[staveNo]];

      if (amp->is_dynamic) {

	midiBaseAmp = (double)amp->dynamic.amp_start;

	if (amp->dynamic.delta_end == amp->dynamic.delta) {
	  midiAmpGradient = 0.0;
	} else {
	  midiAmpGradient =
	    (double)(amp->dynamic.amp_end - amp->dynamic.amp_start) /
	    (double)(amp->dynamic.delta_end - amp->dynamic.delta);
	  /*
	  fprintf(stderr,"switching on, grad=%lf\n", midiAmpGradient);*/
	}

      } else {
	midiBaseAmp = (double)amp->amplitude.amp;
	midiAmpGradient = 0.0;
      }

      ++ampNext[staveNo];
    }

    if (i->item->object_class == ClefClass) clef= ((Clef *)i->item)->clef.clef;

    if (GROUPING_TYPE(i->item) == GroupTupled &&
	i->item->item.grouping.tupled.start) {

      savedDelta = delta;
    }

    prevDelta = delta;

    i->item->methods->write_midi(i->item, (List)events, &delta,
				 staveNo, clef);

    /*    fprintf(stderr,"modifying, grad=%lf\n", midiAmpGradient);*/
    midiBaseAmp += midiAmpGradient * (double)(delta - prevDelta);

    if (GROUPING_TYPE(i->item) == GroupTupled &&
	i->item->item.grouping.tupled.end && savedDelta >= 0) {

      delta = savedDelta +
	MTimeToDelta(i->item->item.grouping.tupled.tupled_length);
      savedDelta = -1;
    }

    events = (EventList)Last(events);
  }

  endEvent = (MIDIEvent)XtMalloc(sizeof(MIDIEventStruct));
  endEvent->DeltaTime = delta;
  endEvent->EventCode = MIDI_FILE_META_EVENT;
  endEvent->EventData.MetaEvent.MetaEventCode = MIDI_END_OF_TRACK;
  endEvent->EventData.MetaEvent.NBytes = 0;
  Nconc(events, Midi_EventCreateList(endEvent, False));
  events = (EventList)First(events);

  /* And write them out */

  events2p = Midi_TrackConvertToTwoPointRepresentation(events);
  Midi_FileWriteTrack(file, events2p);
  Midi_TrackDelete(events2p);
  Midi_TrackDelete(events);
}

/* }}} */
/* {{{ Build amplitude lists from Dynamic marks */

static int ampLevels[] = {
  14, 24, 34, 44, 54, 64, 85, 105, 120, 127
};

AmpChangeAssoc *AddAmp(AmpChangeAssoc **array, int *count)
{
  Begin("AddAmp");

  /* whee!  this is fun. */

  ++ *count;

  if (*array) {
    *array = (AmpChangeAssoc *)
      XtRealloc((void *)*array, *count * sizeof(AmpChangeAssoc));
    return &(*array)[*count - 1];
  } else {
    *array = (AmpChangeAssoc *)XtMalloc(sizeof(AmpChangeAssoc));
    return *array;
  }

  End;
}


void GetAmpChanges(MajorStave sp, int staveNo)
{
  int i, diff;
  long delta = 0, savedDelta = -1;
  ItemList il;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  AmpChangeAssoc *newAssoc;
  MarkList mlist;
  Begin("GetAmpChanges");

  /* are we doing amp changes at all? */

  if (!appData.midiDynamics) End;

  /* if we're inheriting from the staff above, just copy 'em down (for
     the time being) */

  if (staveNo > 0 && mstave->connected_down[staveNo-1]) {

    if (amps[staveNo-1] && (ampCount[staveNo-1] > 0)) {

      amps[staveNo] = (AmpChangeAssoc *)
	XtMalloc(ampCount[staveNo-1] * sizeof(AmpChangeAssoc));
      memcpy(amps[staveNo], amps[staveNo-1],
	     ampCount[staveNo-1] * sizeof(AmpChangeAssoc));

      ampCount[staveNo] = ampCount[staveNo-1];
    }

    End;
  }

  midiBaseAmp = 64.0;

  /* otherwise, scan the track for dynamics */

  for (il = mstave->music[staveNo]; il; il = iNext(il)) {

    if (GROUPING_TYPE(il->item) == GroupTupled &&
	il->item->item.grouping.tupled.start) {

      savedDelta = delta;
    }

    /* hairpin marks...? */

    for (mlist = (MarkList)First(il->item->item.marks);
	 mlist; mlist = (MarkList)Next(mlist)) {

      if (mlist->mark->start &&
	  (mlist->mark->type == Crescendo ||
	   mlist->mark->type == Decrescendo)) {

	long deltaEnd = delta;
	ItemList subList = il;

	while (subList) {
	  deltaEnd += MTimeToDelta(il->item->methods->get_length(il->item));
	  if (FindPairMark(mlist->mark, subList->item->item.marks)) break;
	  subList = iNext(subList);
	}

	newAssoc = AddAmp(amps + staveNo, ampCount + staveNo);
	newAssoc->is_dynamic = True;
	newAssoc->dynamic.delta = delta;
	newAssoc->dynamic.delta_end = deltaEnd;
	newAssoc->dynamic.crescendo = (mlist->mark->type == Crescendo);
	newAssoc->dynamic.amp_start = (int)midiBaseAmp;
      }
    }

    /* textual dynamics */

    if (il->item->object_class == TextClass) {

      Text *text = (Text *)il->item;
      String s = text->text.text;

      if (text->text.position == TextDynamic) {

	for (i = 0; i < textDynamicCount; ++i) {
	  if (!strcmp(s, textDynamics[i])) break;
	}

	if (i >= textDynamicCount) {
	  fprintf(stderr, "Unrecognised dynamic `%s' in output\n", s);
	  End;
	}

	if (XtNumber(ampLevels) != textDynamicCount) {
	  fprintf(stderr,
		  "Warning: amplitude count not equal to dynamic count!\n");
	}

	/* Delta-time will be a bit approximate when writing something
           in the middle of a tupled group. */

	newAssoc = AddAmp(amps + staveNo, ampCount + staveNo);
	newAssoc->is_dynamic = False;
	newAssoc->amplitude.delta = delta;
	newAssoc->amplitude.amp = ampLevels[i];

	midiBaseAmp = (double)ampLevels[i];
      }
    }

    delta += MTimeToDelta(il->item->methods->get_length(il->item));

    if (GROUPING_TYPE(il->item) == GroupTupled &&
	il->item->item.grouping.tupled.end && savedDelta >= 0) {
      delta = savedDelta +
	MTimeToDelta(il->item->item.grouping.tupled.tupled_length);
      savedDelta = -1;
    }
  }

  /* now go through the dynamic list and compute amps and gradients
     for the hairpins */

  for (i = 0; i < ampCount[staveNo]; ++i) {

    if (amps[staveNo][i].is_dynamic) {

      DynamicAssoc *a = &(amps[staveNo][i].dynamic);
      int j, dCount = 0, ampEnd = -1;

      for (j = i + 1; j < ampCount[staveNo]; ++j) {

	if (amps[staveNo][j].is_dynamic) {
	  if (amps[staveNo][j].dynamic.crescendo == a->crescendo) {
	    ++dCount;
	  } else {
	    if (dCount > 0) --dCount;
	  }
	} else {
	  ampEnd = amps[staveNo][j].amplitude.amp;
	  break;
	}
      }
      
      if (ampEnd < 0) ampEnd = a->amp_start + 15;
      if (dCount > 0)
	ampEnd = a->amp_start + ((ampEnd - a->amp_start) / (dCount + 1));

      /* if we don't have a sensible ending amplitude, invent one --
         but check it for gradient, we don't want anything too sudden */

      diff = (int)(0.03 * (double)(a->delta_end - a->delta));
      if (diff > 15) diff = 15;

      if (a->crescendo) {
	if (ampEnd < a->amp_start) ampEnd = a->amp_start + diff;
      } else {
	if (ampEnd > a->amp_start) ampEnd = a->amp_start - diff;
      }
      
      if      (ampEnd > 127) ampEnd = 127;
      else if (ampEnd <= 10) ampEnd = 10;

      a->amp_end = ampEnd;

      /* if the next ampChange is a hairpin dynamic, we start it from
         the same amplitude as this one ended... */

      if (i < ampCount[staveNo] - 1 && amps[staveNo][i+1].is_dynamic) {

	amps[staveNo][i+1].dynamic.amp_start = ampEnd;

	/* ...and if it's going in the opposite direction and isn't
           followed by an explicit textual amplitude, end it instead
           at the amplitude that this one started at */

	if (amps[staveNo][i+1].dynamic.crescendo != a->crescendo &&
	    (i == ampCount[staveNo] - 2 || amps[staveNo][i+2].is_dynamic)) {
	  amps[staveNo][i+1].dynamic.amp_end = a->amp_start;
	  ++i;
	}
      }
    }
  }

  fprintf(stderr, "\nStaff %d\n", staveNo);
  for (i = 0; i < ampCount[staveNo]; ++i) {
    if (amps[staveNo][i].is_dynamic) {
      fprintf(stderr, "time %ld until %ld crescendo %d amp_start %d amp_end %d\n",
	      amps[staveNo][i].dynamic.delta,
	      amps[staveNo][i].dynamic.delta_end,
	      amps[staveNo][i].dynamic.crescendo,
	      amps[staveNo][i].dynamic.amp_start,
	      amps[staveNo][i].dynamic.amp_end);
    }
  }

  End;
}


void FreeAmpChanges(int staves)
{
  int i;
  Begin("FreeAmpChanges");

  for (i = 0; i < staves; ++i) {
    XtFree((void *)amps[i]);
  }

  XtFree((void *)amps);
  XtFree((void *)ampCount);
  XtFree((void *)ampNext);

  End;
} 

/* }}} */
/* {{{ Write the stave */

Result MidiWriteStave(MajorStave sp, String fname, String title)
{
  MajorStaveRec  *mstave = (MajorStaveRec *)sp;
  MIDIHeaderChunk header;    
  MIDIFileHandle  file;
  String          msg;
  int             i;
  
  Begin("MidiWriteStave");

  header.Format = MIDI_SIMULTANEOUS_TRACK_FILE;
  header.NumTracks = mstave->staves + 1;
  header.Timing.Division = 160;

  StaveBusyStartCount(mstave->staves*2 + 1);
  StaveBusyMakeCount(0);

  if ((file = Midi_FileOpen(fname, &header, MIDI_WRITE)) == NULL) {
    
    msg = (String)XtMalloc(300);

    if (!title) sprintf(msg, "Sorry, I can't open `%s' for writing.", fname);
    else        sprintf(msg, "Sorry, I can't open a temporary file.");

    XBell(display, 70);
    (void)YQuery(topLevel, msg, 1, 0, 0, "Cancel", NULL);

    XtFree((void *)msg);
    Return(Failed);
  }

  if (title) {

    MidiWriteTrackZero(sp, (void *)file, title);

  } else {

    for (msg = fname + strlen(fname) - 1; msg >= fname; --msg)
      if (*msg == '/') break;

    if (msg <= fname) MidiWriteTrackZero(sp, (void *)file, fname);
    else              MidiWriteTrackZero(sp, (void *)file, ++msg);
  }

  ampBarEmphasis = appData.midiBarEmphasis;
  amps = (AmpChangeAssoc **)XtCalloc(mstave->staves, sizeof(AmpChangeAssoc *));
  ampCount = (int *)XtCalloc(mstave->staves, sizeof(int));
  ampNext  = (int *)XtCalloc(mstave->staves, sizeof(int));

  fprintf(stderr, "Bar emphasis %d.  %sriting dynamic information.\n",
	  ampBarEmphasis, appData.midiDynamics ? "W" : "Not w");

  for (i = 0; i < mstave->staves; ++i) {

    StaveBusyMakeCount(i*2 + 1);
    UnformatItemList(&mstave->music[i], NULL);
    GetAmpChanges(sp, i);

    StaveBusyMakeCount(i*2 + 2);
    MidiWriteTrack
      (mstave->names[i], mstave->music[i],
       (StaveEltList)First(mstave->bar_list), i,
       mstave->midi_patches[(i > 0 && mstave->connected_down[i-1]) ? i-1 : i],
       (void *)file);
    StaveResetFormatting(sp, i);
  }

  Midi_FileClose(file);

  FreeAmpChanges(mstave->staves);

  staveMoved = True;
  StaveRefresh(stave, -1);
  StaveBusyFinishCount();

  Return(Succeeded);
}

/* }}} */


/* Object writing methods: */

/* {{{ Nothing */

void MidiWriteNothing(MusicObject obj, List events, long *delta, int channel,
		      ClefTag clef)
{
  Begin("MidiWriteNothing");
  End;
}

/* }}} */
/* {{{ Rest */

/* So we can emphasise first note (JPff change, not finished) */
void MidiWriteRest(MusicObject obj, List events, long *delta, int channel,
		   ClefTag clef)
{
  Begin("MidiWriteRest");
  midiBarStart = 0;
  *delta += MTimeToDelta(((Rest *)obj)->methods->get_length(obj));
  End;
}

/* }}} */
/* {{{ Key */

void MidiWriteKey(MusicObject obj, List events, long *delta, int channel,
		  ClefTag clef)
{
  Key *key = (Key *)obj;
  long useDelta = *delta - 1L;
  Begin("MidiWriteKey");

  if (events) {
    if (((EventList)Last(events))->Event.DeltaTime >= useDelta) useDelta += 1L;
  }

  Nconc(events, Midi_EventCreateList
	(Midi_EventCreateKeySigEvt
	 (useDelta, (byte)
	  (key->key.visual->sharps ?
	   key->key.visual->number : - key->key.visual->number),
	  (byte)0), False));

  End;
}

/* }}} */
/* {{{ Text */

void MidiWriteText(MusicObject obj, List events, long *delta, int channel,
		   ClefTag clef)
{
  Text *text = (Text *)obj;

  Begin("MidiWriteText");

  Nconc(events, Midi_EventCreateList
	(Midi_EventCreateTextEvt(MIDI_TEXT_EVENT, *delta, text->text.text),
	 False));

  End;

#ifdef NOT_DEFINED

  if (text->text.position == TextDynamic) {

    int i;
    String s = text->text.text;

    for (i = 0; i < textDynamicCount; ++i) {
      if (!strcmp(s, textDynamics[i])) break;
    }

    if (i >= textDynamicCount) {
      fprintf(stderr, "Unrecognised dynamic `%s' in output\n", s);
      End;
    }

    if (XtNumber(ampLevels) != textDynamicCount) {
      fprintf(stderr,"Warning: amplitude count not equal to dynamic count!\n");
    }

    /*    midiBaseAmp = ampLevels[i];*/
  }

  End;
#endif
}

/* }}} */
/* {{{ Chord */

static Boolean ThisChordTiedForward(Chord *c)
{
  MarkList mlist;
  Begin("ThisChordTiedForward");

  for (mlist = (MarkList)First(c->item.marks); mlist;
       mlist = (MarkList)Next(mlist)) {

    if (mlist->mark->type == Tie && mlist->mark->start) Return(True);
  }

  Return(False);
}


void MidiWriteChord(MusicObject obj, List events, long *delta, int channel,
		    ClefTag clef)
{
  int      i;
  Chord   *chord = (Chord *)obj;
  long     length;
  byte     pitch, amp;
  double   ratio = 1.0;

  static long rememberLength = 0L;

  Begin("MidiWriteChord");

  if (chord->item.grouping.none.type == GroupTupled) {

    ratio = ((double)(chord->item.grouping.tupled.tupled_length)) /
      ((double)(chord->item.grouping.tupled.untupled_length));

    length = (long)
      (ratio *
       (double)MTimeToDelta(chord->methods->get_length((MusicObject)chord)));

  } else {

    length = MTimeToDelta(chord->methods->get_length((MusicObject)chord));

    if (ThisChordTiedForward(chord)) {
      rememberLength = length;
      End;
    } else {
      length += rememberLength;
      rememberLength = 0L;
    }
  }

  amp = (byte)((int)(midiBaseAmp *
		     (chord->chord.modifiers & ModAccent ? 1.2 : 1.0)) +
	       midiBarStart);

  if (amp <  10) amp = 10;
  if (amp > 127) amp = 127;

  /*  fprintf(stderr,"amp is %d\n",(int)amp);*/
  
  for (i = 0; i < chord->chord.voice_count; ++i) {

    pitch = (byte)VoiceToMidiPitch(&chord->chord.voices[i], clef);

    Nconc(events, Midi_EventCreateList
	  (Midi_EventCreateNote(*delta, (byte)channel, pitch, amp, length),
	   False));
  }

  *delta += length;
  
  midiBarStart = 0;		/* Emphasis on first note of bar */
  End;
}

/* }}} */


