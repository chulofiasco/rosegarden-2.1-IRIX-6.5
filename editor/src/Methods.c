
/* Methods.c */

/* Musical Notation Editor for X, Chris Cannam 1994    */
/* Class methods, excluding constructors, Draw methods */
/* and all methods within the MajorStave               */

/* {{{ Includes */

#include <Lists.h>

#include "General.h"
#include "Tags.h"
#include "Visuals.h"
#include "Classes.h"
#include "Notes.h"
#include "GC.h"
#include "ItemList.h"
#include "Menu.h"

/* }}} */

/* {{{ Method pointer initialisations */

Methods itemMethods = {
  GetZeroWidth,   GetLongestChord,  GetLowestNote,
  GetHighestNote, GetZeroLength,    CloneItem,
  WriteNothing,   MidiWriteNothing, DrawNothing,
  DrawMTNothing,  DrawOTNothing,  DestroyItem,  
};

Methods metronomeMethods = {
  GetZeroWidth,   GetLongestChord,  GetLowestNote,
  GetHighestNote, GetZeroLength,    CloneMetronome,
  WriteMetronome, MidiWriteNothing, DrawMetronome,
  DrawMTMetronome,DrawOTMetronome,  DestroyMetronome,
};

Methods clefMethods = {
  GetClefWidth,   GetLongestChord,  GetLowestNote,
  GetHighestNote, GetZeroLength,    CloneClef,
  WriteClef,      MidiWriteNothing, DrawClef,
  DrawMTClef,     DrawOTClef,       DestroyClef,  
};

Methods keyMethods = {
  GetKeyWidth,    GetLongestChord,  GetLowestNote,
  GetHighestNote, GetZeroLength,    CloneKey,
  WriteKey,       MidiWriteKey,     DrawKey,
  DrawMTKey,      DrawOTKey,        DestroyKey,   
};

Methods textMethods = {
  GetZeroWidth,   GetLongestChord,  GetLowestNote,
  GetHighestNote, GetZeroLength,    CloneText,
  WriteText,      MidiWriteText,    DrawText,
  DrawMTText,     DrawOTText,       DestroyText,  
};

Methods phraseMethods = {
  GetZeroWidth,   GetLongestChord,  GetLowestNote,
  GetHighestNote, GetZeroLength,    ClonePhrase,
  WriteNothing,   MidiWriteNothing, DrawNothing,
  DrawMTNothing,  DrawOTNothing,    DestroyPhrase,
};

/* rests are written to MIDI even though they're empty, so as to */
/* enable emphasis deduced from their existence, or something:   */

Methods restMethods = {
  GetRestWidth,   GetRestShortest,  GetLowestNote,
  GetHighestNote, GetRestLength,    CloneRest,
  WriteRest,      MidiWriteRest,    DrawRest,
  DrawMTRest,     DrawOTRest,       DestroyRest,  
};

Methods chordMethods = {
  GetChordWidth,  GetChordShortest, GetChordHighest,
  GetChordLowest, GetChordLength,   CloneChord,
  WriteChord,     MidiWriteChord,   DrawChord,
  DrawMTChord,    DrawOTChord,      DestroyChord,  
};

Methods groupMethods = {
  GetGroupWidth,  GetGroupShortest, GetGroupHighest,
  GetGroupLowest, GetGroupLength,   CloneGroup,
  WriteGroup,     MidiWriteNothing, DrawGroup,
  DrawMTGroup,    DrawOTGroup,      DestroyGroup,  
};

Methods barMethods = {
  GetZeroWidth,   GetGroupShortest, GetGroupHighest,
  GetGroupLowest, GetGroupLength,   CloneBar,
  WriteNothing,   MidiWriteNothing, DrawNothing,
  DrawMTBar,      DrawOTBar,        DestroyBar,
};

/* }}} */
/* {{{ Degenerate highest, lowest, shortest, longest definitions */

NoteVoice highestNoteVoice =
{
  (Pitch)20, ModSharp, ModSharp,
};

NoteVoice lowestNoteVoice =
{
  (Pitch)-10, ModFlat, ModFlat,
};

NoteVoice metronomeNoteVoice =
{
  (Pitch)3, ModNone, ModNone,
};

TimeSignature defaultTimeSignature =
{
  4, 4, 64L,
};

Chord longestChord;
Clef  defaultClef;
Key   defaultKey;
Text  chordText;


extern void InitialiseStaticObjects()
{
  Begin("InitialiseStaticObjects");

  NewChord(&longestChord, &lowestNoteVoice, 1, ModNone, LongestNote, True);
  NewClef(&defaultClef, TrebleClef);
  NewKey(&defaultKey, KeyC);
  NewText(&chordText, "no name", TextChordName);

  End;
}

/* }}} */
/* {{{ Degenerate methods */

Dimension GetZeroWidth(MusicObject item)
{
  Begin("GetZeroWidth");
  Return(0);
}

MTime GetZeroLength(MusicObject item)
{
  Begin("GetZeroLength");
  Return(zeroTime);
}

NoteVoice *GetHighestNote(MusicObject item)
{
  Begin("GetHighestNote");
  Return(&highestNoteVoice);
}

NoteVoice *GetLowestNote(MusicObject item)
{
  Begin("GetLowestNote");
  Return (&lowestNoteVoice);
}

MusicObject GetLongestChord(MusicObject item)
{
  Begin("GetLongestChord");
  Return((MusicObject)&longestChord);
}

/* }}} */

/* {{{ Chord methods */

Dimension GetChordWidth(MusicObject p)
{
  Chord *chord = (Chord *)p;
  Dimension width;
  Begin("GetChordWidth");
  if (!chord) return 0;

  width = NoteWidth +
    (chord->chord.note_modifier_width) +
    (chord->chord.visual->dotted? DotWidth : 0);

  /* if we've got any second intervals, we're gonna want more width;
     should we try to deal with that here?  hmm. */

  Return(width);
}


char *Rechord(Chord);

String GetChordDisplayedName(Chord *chord)
{
  Begin("GetChordDisplayedName");

  if (!chord) Return(0);

  /* assume these four modes are exclusive */

  if (QueryMenuMode(ShowingNoChordNamesMode)) {

    if (chord->chord.chord_name && !chord->chord.chord_named) {
      NameChord(chord, NULL, False);
    }

    Return(0);

  } else if (QueryMenuMode(ShowingChordNamesMode)) {

    if (!chord->chord.chord_name ||
	(strlen(chord->chord.chord_name) == 0)) Return(0);

    if (!chord->chord.chord_named) {
      NameChord(chord, NULL, False);
      Return(0);
    }

  } else if (QueryMenuMode(ShowingAllChordNamesMode)) {

    if (chord->chord.voice_count < 2) Return(0);

  } else if (QueryMenuMode(ShowingAllNoteNamesMode)) {

    /* no problem, name the thing */

  } else {

    Return(0);
  }

  if (!chord->chord.chord_name) {
    String str = Rechord(*chord);
    NameChord(chord, str, False);
    free(str);
  }

  Return(chord->chord.chord_name);
}


Dimension GetChordNameExtraWidth(Chord *chord)
{
  String name;
  Dimension w, w2;
  Begin("GetChordNameExtraWidth");

  name = GetChordDisplayedName(chord);
  if (!name) Return(0);

  w = GetChordWidth((MusicObject)chord);
  w2 = XTextWidth(chordFont,
		  chord->chord.chord_name, strlen(chord->chord.chord_name));

  if (w2 > w) Return(w2 - w);
  Return(0);
}


MTime GetChordLength(MusicObject chord)
{
  Begin("GetChordLength");
  Return(chord ? ((Chord *)chord)->chord.length : zeroTime);
}


MusicObject GetChordShortest(MusicObject obj)
{
  Begin("GetChordShortest");
  Return(obj ? obj : (MusicObject)&longestChord);
}


NoteVoice *GetChordHighest(MusicObject obj)
{
  Chord *c = (Chord *)obj;
  Begin("GetChordHighest");

  if (c && c->chord.voice_count > 0) {
    Return(&c->chord.voices[c->chord.voice_count-1]);
  } else {
    Return(GetLowestNote((MusicObject)NULL));
  }
}


/* uses Clef if given, offset otherwise; returns True for Up */

Boolean GetChordStemDirection(Chord *c, Clef *clef, int offset)
{
  Boolean up;
  Pitch highest, lowest;
  Begin("GetChordStemDirection");
  
  if (clef) offset = ClefPitchOffset(clef->clef.clef);
  
  highest = c->methods->get_highest((MusicObject)c)->pitch + offset;
  lowest =  c->methods->get_lowest ((MusicObject)c)->pitch + offset;

  if  (highest > 4)
    if (lowest > 4) up = False;
    else            up = ((highest - 4) < (5 - lowest));
  else              up = True;

  Return(up);
}


NoteVoice *GetChordLowest(MusicObject obj)
{
  Chord *c = (Chord *)obj;
  Begin("GetChordLowest");

  if (c && c->chord.voice_count > 0) {
    Return(&c->chord.voices[0]);
  } else {
    Return(GetHighestNote((MusicObject)NULL));
  }
}


int VoiceToMidiPitch(NoteVoice *voice, ClefTag clef)
{
  int rtn;
  int octave = 5;
  Pitch pitch = voice->pitch;

  Begin("VoiceToMidiPitch");

  while (pitch < 0) { octave -= 1; pitch += 7; }
  while (pitch > 7) { octave += 1; pitch -= 7; }

  if (pitch > 4) ++octave;

  switch(pitch) {

  case 0: rtn =  4; break;	/* bottom line, treble clef: E */
  case 1: rtn =  5; break;	/* F */
  case 2: rtn =  7; break;	/* G */
  case 3: rtn =  9; break;	/* A, in next octave */
  case 4: rtn = 11; break;	/* B, likewise*/
  case 5: rtn =  0; break;	/* C, moved up an octave (see above) */
  case 6: rtn =  2; break;	/* D, likewise */
  case 7: rtn =  4; break;	/* E, likewise */
  }

  if (voice->modifiers & ModSharp) ++ rtn;
  if (voice->modifiers & ModFlat)  -- rtn;

  switch(clef) {

  case  TrebleClef: break;
  case   TenorClef: octave -= 1; break;
  case    AltoClef: octave -= 1; break;
  case    BassClef: octave -= 2; break;
  case InvalidClef: break;
  }

  rtn += 12 * octave;

  Return(rtn);
}


/* Try to get a Rosegarden pitch for the MIDI pitch.  Assume the */
/* Rosegarden pitch is in the treble clef, for the moment.  Thus */
/* pitch 0 is an E, in octave 5 (midi pitch 64).                 */

NoteVoice MidiPitchToVoice(int mp, Boolean sharps)
{
  NoteMods  mods = ModNone;
  NoteVoice rtn;
  Pitch     pitch;
  int       octave;

  Begin("MidiPitchToVoice");

  octave = mp / 12;
  mp     = mp % 12;

  switch(mp) {

  case  0: pitch = -2; break;	                 /* C  */
  case  1: pitch = -2; mods = ModSharp; break;   /* C# */
  case  2: pitch = -1; break;                    /* D  */
  case  3: pitch = -1; mods = ModSharp; break;   /* D# */
  case  4: pitch =  0; break;                    /* E  */
  case  5: pitch =  1; break;                    /* F  */
  case  6: pitch =  1; mods = ModSharp; break;   /* F# */
  case  7: pitch =  2; break;                    /* G  */
  case  8: pitch =  2; mods = ModSharp; break;   /* G# */
  case  9: pitch =  3; break;                    /* A  */
  case 10: pitch =  3; mods = ModSharp; break;   /* A# */
  case 11: pitch =  4; break;                    /* B  */
  }

  if (mods && !sharps) { ++pitch; mods = ModFlat; }
  pitch += (octave - 5) * 7;

  (void)NewNoteVoice(&rtn, pitch, mods);

  Return(rtn);
}

/* }}} */
/* {{{ Clef methods */

Pitch ClefPitchOffset(ClefTag clef)
{
  Begin("ClefPitchOffset");

  switch(clef) {

  case  TrebleClef: Return( 0);
  case    BassClef: Return(-2);
  case    AltoClef: Return(-1);
  case   TenorClef: Return( 1);
  case InvalidClef: Return( 0);
  }

  Return(0);
}

Dimension GetClefWidth(MusicObject clef)
{
  Begin("GetClefWidth");
  Return(clef ? ClefWidth + 3 : 0);
}

/* }}} */
/* {{{ Key methods */

Dimension GetKeyWidth(MusicObject obj)
{
  Key *key = (Key *)obj;
  Dimension width = 0;
  KeyVisual prevV;
  Boolean sharps;
  int n, number;

  Begin("GetKeyWidth");

  sharps = key->key.visual->sharps;
  number = key->key.visual->number;

  for (n = 0; n < keyVisualCount; ++n) {
    if (keyVisuals[n].key == key->key.changing_from) prevV = &keyVisuals[n];
  }

  if (prevV->sharps == sharps && number < prevV->number) {
    width = (prevV->number - number) * (NoteModWidth - 2) + 4;
  } else {
    if (prevV->sharps != sharps) {
      width = prevV->number * (NoteModWidth - 2) + 4;
    }
  }

  width += number * (NoteModWidth - 2);

  Return(width + 3);

  /*  Return(key ? ((Key *)key)->key.visual->number * (NoteModWidth - 2) + 3 : 0);*/
}

/* }}} */
/* {{{ Time Signature methods */

Dimension GetTimeSignatureWidth(MusicObject timesig)
{
  XGCValues          values;
  static int         dir, asc, dsc = -1;
  static XCharStruct info;

  Begin("GetTimeSignatureWidth");
  
  if (dsc == -1) {
    XGetGCValues(display, timeSigGC, GCFont, &values);
    XQueryTextExtents(display, values.font, "4", 1, &dir, &asc, &dsc, &info);
    dsc = 0;
  }

  if (((TimeSignature *)timesig)->numerator   > 9 ||
      ((TimeSignature *)timesig)->denominator > 9)
    Return(2 * info.width + 8);
  else  Return(info.width + 14); /* was 8 */
}

/* }}} */
/* {{{ Rest methods */

Dimension GetRestWidth(MusicObject rest)
{
  Begin("GetRestWidth");
  Return(rest ? ((Rest *)rest)->rest.visual->width : 0);
}


MTime GetRestLength(MusicObject rest)
{
  Begin("GetRestLength");
  Return(rest ? ((Rest *)rest)->rest.length : zeroTime);
}


/* bit of a hack here */

MusicObject GetRestShortest(MusicObject obj)
{
  static Chord restChords[NoteCount*2];
  Rest *rest = (Rest *)obj;
  int i;
  Begin("GetRestShortest");

  if (!rest) Return((MusicObject)&longestChord);

  i = rest->rest.visual->type + (rest->rest.visual->dotted ? NoteCount : 0);

  (void)NewChord(&restChords[i], &lowestNoteVoice, 0, ModNone,
		 rest->rest.visual->type, rest->rest.visual->dotted);

  Return((MusicObject)&restChords[i]);
}

/* }}} */
/* {{{ Group methods */

Dimension GetGroupWidth(MusicObject obj)
{
  Dimension  width = 0;
  Group     *group = (Group *)obj;
  ItemList   list;

  Begin("GetGroupWidth");

  if (!group) Return(0);

  for (ItemList_ITERATE_GROUP(list, group)) /* deprecated */
    width +=
      list->item->methods->get_min_width((MusicObject)(list->item));

  Return(width);
}


MTime GetGroupLength(MusicObject obj)
{
  MTime     curr;
  MTime     shortest;
  MTime     length = zeroTime;
  Group    *group  = (Group *)obj;
  ItemList  list;

  Begin("GetGroupLength");

  if (!group || (group->group.type == GroupDeGrace)) Return(zeroTime);

  shortest = longestChord.chord.length;

  if (group->group.type == GroupTupled) {
    length = group->group.tupled_length;
  } else {

    for (ItemList_ITERATE_GROUP(list, group)) { /* deprecated */
      curr   = list->item->methods->get_length((MusicObject)(list->item));
      length = AddMTime(length, curr);

      if (GROUPING_TYPE(list->item) == GroupTupled &&
	  (list == group->group.end || list->item->item.grouping.tupled.end)) {
	length += list->item->item.grouping.tupled.tupled_length;
	length -= list->item->item.grouping.tupled.untupled_length;
      }
    }
  }

  Return(length);
}


MusicObject GetGroupShortest(MusicObject obj)
{
  Chord   *curr;
  Chord   *shortest;
  Group   *group = (Group *)obj;
  ItemList list;

  Begin("GetGroupShortest");

  shortest = (Chord *)GetLongestChord((MusicObject)NULL);
  if (!obj || group->group.type == GroupDeGrace) Return(shortest);

  for (ItemList_ITERATE_GROUP(list, group)) { /* deprecated */

    curr = (Chord *)(list->item->methods->get_shortest
		     ((MusicObject)(list->item)));
    if (MTimeLesser(curr->chord.length, shortest->chord.length))
      shortest = curr;
  }

  Return((MusicObject)shortest);
}


NoteVoice *GetGroupHighest(MusicObject obj)
{
  NoteVoice *curr;
  NoteVoice *highest;
  Group     *group = (Group *)obj;
  ItemList   list;

  Begin("GetGroupHighest");

  highest = GetLowestNote((MusicObject)NULL);
  if (!obj || group->group.type == GroupDeGrace) Return(highest);

  for (ItemList_ITERATE_GROUP(list, group)) { /* deprecated */

    curr = list->item->methods->get_highest((MusicObject)(list->item));
    if (curr->pitch > highest->pitch) highest = curr;
  }

  Return(highest);
}


NoteVoice *GetGroupLowest(MusicObject obj)
{
  NoteVoice *curr;
  NoteVoice *lowest;
  Group     *group = (Group *)obj;
  ItemList   list;

  Begin("GetGroupLowest");

  lowest = GetHighestNote((MusicObject)NULL);
  if (!obj || group->group.type == GroupDeGrace) Return(lowest);

  for (ItemList_ITERATE_GROUP(list, group)) { /* deprecated */

    curr = list->item->methods->get_lowest((MusicObject)(list->item));
    if (curr->pitch < lowest->pitch) lowest = curr;
  }

  Return(lowest);
}

/* }}} */
/* {{{ Bar methods */

/* To find the minimum comfortable width of a whole bar, we need  */
/* the absolute minimum width of the bar (by "casting" it to a    */
/* Group, and getting the width of that), and then add on as much */
/* comfortable width as would be needed if the bar was made up    */
/* entirely of repetitions of its shortest note (this being the   */
/* scenario that would take most space).  This gap is the product */
/* of the number of the bar's shortest notes that will fit in the */
/* length of the bar, and the comfortable gap for each of those.  */

Dimension GetBarWidth(Bar *bar, Bar *prevBar)
{
  Dimension width = 0;
  Dimension minWidth;
  ItemList  list;
  Chord    *curr;
  Chord    *shortest;
  int       shortCount;
  MTime     length;
  MTime     totalLen;
  BarTag    start, end;
  Item     *item;

  Begin("GetBarWidth");

  if (!bar) Return(ClefWidth + NoteWidth);              /* arbitrary */

  /* this fails to cope with the stretching that StaveRefresh now does: */
  /*  if (bar->bar.still_as_drawn) Return(bar->bar.width); */

  shortest = (Chord *)GetLongestChord((MusicObject)NULL);
  totalLen = zeroTime;
  shortCount = 0;

  /* can't actually use get_min_width on the Group parent of the bar,
     because it may contain Groups itself */

  for (ItemList_ITERATE_GROUP(list, bar)) {

    item = list->item;
    length = item->methods->get_length((MusicObject)(item));
    totalLen += length;

    if (GROUPING_TYPE(item) == GroupTupled &&
	(list == bar->group.end || item->item.grouping.tupled.end)) {
      totalLen += item->item.grouping.tupled.tupled_length;
      totalLen -= item->item.grouping.tupled.untupled_length;
    }

    if (MTimeEqual(length, zeroTime)) {

      width += item->methods->get_min_width((MusicObject)(item));

    } else {

      if (IS_CHORD(item)) {
	width += GetChordNameExtraWidth((Chord *)item);
      }

      curr = (Chord *)(item->methods->get_shortest((MusicObject)(item)));

      if (MTimeLesser(curr->chord.length, shortest->chord.length)) {
	shortCount = 1;
	shortest = curr;
      } else if (MTimeEqual(curr->chord.length, shortest->chord.length)) {
	shortCount ++;
      }
    }
  }

  minWidth = shortest->methods->get_min_width((MusicObject)shortest);
  if (shortest->chord.note_modifier_width == 0) minWidth += NoteModWidth/2;

  /* if there aren't many of the shortest notes, we don't want to
     allow so much space to accommodate them */
  if (shortCount < 3) minWidth -= 3 - shortCount;

  width +=
  (MTimeToNumber(totalLen) *
    (shortest->chord.visual->comfortable_gap + minWidth) /
     MTimeToNumber(shortest->methods->get_length((MusicObject)shortest)));

  /* if we'll need to draw a timesig, add the width */
  if (!prevBar ||
      prevBar->bar.time.numerator != bar->bar.time.numerator ||
      prevBar->bar.time.denominator != bar->bar.time.denominator) {

    width += GetTimeSignatureWidth(&bar->bar.time);
  }

  start = BAR_START_TYPE(bar);
  width += ((start == NoFixedBar || start == PlainBar) ? 3 :
	    (start == RepeatLeftBar || start == RepeatBothBar) ?
	    5 + DotWidth : (start == NoBarAtAll) ? 0 : 5);

  end = BAR_END_TYPE(bar);
  width += ((end == NoFixedBar || end == PlainBar) ? 3 :
	    (end == RepeatRightBar || end == RepeatBothBar) ?
	    10 + DotWidth : (end == NoBarAtAll) ? 0 : 10);

  Return(width + 3);
}

/* }}} */

