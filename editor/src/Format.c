
/* Format.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */

/* File containing methods specific to the working  */
/* of the Bar class: formatting notes into bars.    */

/* {{{ Includes */

#include "General.h"
#include "Notes.h"
#include "Classes.h"
#include "Format.h"
#include "ItemList.h"
#include "StavePrivate.h"

/* }}} */
/* {{{ Prototypes */

static void SplitChord  (ItemList, MTime, MTime);
static void SplitRest   (ItemList, MTime, MTime);

static void MergeChords (ItemList);
static void MergeRests  (ItemList);

/* }}} */
/* {{{ Static variables */

static Key *prevKey = NULL;
static KeyTag prevKeyTag = KeyC;
static NoteMods accidentalTable[7];
static NoteMods newAccidentalTable[7];

/* }}} */

/* {{{ Key and Accidental table */

void ResetStaffKey(Key *key)
{
  Begin("ResetStaffKey");
  prevKey = key;
  if (key) prevKeyTag = key->key.visual->key;
  else prevKeyTag = KeyC;
  End;
}


void InitialiseAccidentalTable(Key *key)
{
  int     n;
  Pitch   pitch;
  Boolean sharps;

  Begin("InitialiseAccidentalTable");

  if (prevKey != key) key->key.changing_from = prevKeyTag;
  prevKeyTag = key->key.key;
  prevKey = key;

  sharps = key->key.visual->sharps;
  pitch  = sharps ? 8 : 4;

  for (n = 0; n < 7; ++n) accidentalTable[n] = ModNone;

  for (n = 0; n < key->key.visual->number; ++n) {

    if (sharps) accidentalTable[(pitch + 70) % 7] |= ModSharp;
    else        accidentalTable[(pitch + 70) % 7] |= ModFlat;

    if (sharps) pitch -= 3;
    else        pitch += 3;
  }

  End;
}


void UpdateAccidentalTable(Chord *chord)
{
  int        n;
  int        p;
  int        mods;
  NoteMods   cmods;
  NoteVoice *v;
  Boolean    changed = False;

  Begin("UpdateAccidentalTable");

  /* need newAccidentalTable for double-buffering to prevent one
     accidental in achord aliasing another one on another octave in
     the same chord -- whole chord must be aliased according to the
     accidentalTable in force when it starts */

  for (n = 0; n < 7; ++n) newAccidentalTable[n] = accidentalTable[n];

  chord->chord.note_modifier_width = 0;

  for (n = 0, v = chord->chord.voices; n < chord->chord.voice_count; ++n, ++v) {

    p = (v->pitch + 70) % 7;

    if (accidentalTable[p] != ModNone) {

      if (v->modifiers == ModNone) {

	v->display_mods = ModNatural;
	newAccidentalTable[p] = ModNone;
	changed = True;

      } else if (v->modifiers & ModNatural) {

	v->display_mods = v->modifiers;
	newAccidentalTable[p] = v->modifiers ^ ModNatural;
	changed = True;

      } else if (v->modifiers == accidentalTable[p]) {

	v->display_mods = ModNone;

      } else {

	v->display_mods = v->modifiers | ModNatural;
	newAccidentalTable[p] = v->modifiers;
	changed = True;
      }

    } else {

      v->display_mods = v->modifiers;
      newAccidentalTable[p] |= v->modifiers;
      changed = True;
    }

    for (mods = 0, cmods = v->display_mods; cmods; cmods >>= 1)
      if (cmods & 1) ++mods;

    if (mods * NoteModWidth > chord->chord.note_modifier_width)
      chord->chord.note_modifier_width = mods * NoteModWidth;
  }

  chord->chord.note_modifier_width = chord->chord.note_modifier_width * 2 / 3;

  if (changed) {
    for (n = 0; n < 7; ++n) accidentalTable[n] = newAccidentalTable[n];
  }

  End;
}

/* }}} */

/* {{{ Split Chord */

static void SplitChord(ItemList items, MTime chordLength, MTime needed)
{
  int        i;
  Chord     *a, *b;
  NoteTag    tag1, tag2;
  Boolean    dotted1, dotted2;
  Boolean    tiedBack;
  MTime      secondLength;
  MarkList   marks;
  NoteVoice *voices;

  Begin("SplitChord");
  
  if (!(MTimeGreater(chordLength, needed))) End;
  secondLength = SubtractMTime(chordLength, needed);

  tag1 = MTimeToTag(needed,       &dotted1);
  tag2 = MTimeToTag(secondLength, &dotted2);

  if (!MTimeEqual(TagToMTime(tag1, dotted1), needed) ||
      !MTimeEqual(TagToMTime(tag2, dotted2), secondLength)) End;

  a = (Chord *)(items->item);
  tiedBack = a->phrase.tied_backward;
  marks = a->item.marks;

  (void)NewChord(a,
		 a->chord.voices,
		 a->chord.voice_count,
		 a->chord.modifiers,
		 tag1, dotted1);

  a->item.marks = marks;
  voices = (NoteVoice *)XtMalloc(a->chord.voice_count * sizeof(NoteVoice));
  for (i = 0; i < a->chord.voice_count; ++i) voices[i] = a->chord.voices[i];

  b   = NewChord(NULL,
		 voices,
		 a->chord.voice_count,
		 a->chord.modifiers,
		 tag2, dotted2);

  a->phrase.tied_forward  = True;
  b->phrase.tied_backward = True;
  a->phrase.tied_backward = tiedBack;

  if (Next(items)) Insert(NewItemList((Item *)b), Next(items));
  else       Nconc(items, NewItemList((Item *)b));

  End;
}

/* }}} */
/* {{{ Split Rest */

static void SplitRest(ItemList items, MTime restLength, MTime needed)
{
  Rest      *a, *b;
  NoteTag    tag;
  Boolean    dotted;
  Boolean    tiedBack;
  MTime      secondLength;
  MarkList   marks;

  Begin("SplitRest");

  if (!(MTimeGreater(restLength, needed))) End;
  secondLength = SubtractMTime(restLength, needed);

  tag = MTimeToTag(needed, &dotted);
  a   = (Rest *)(items->item);

  tiedBack = a->phrase.tied_backward;
  marks = a->item.marks;

  (void)NewRest(a, tag, dotted);

  a->item.marks = marks;
  tag = MTimeToTag(secondLength, &dotted);
  b   = NewRest(NULL, tag, dotted);

  a->phrase.tied_forward  = True;
  b->phrase.tied_backward = True;
  a->phrase.tied_backward = tiedBack;

  if (Next(items)) Insert(NewItemList((Item *)b), Next(items));
  else       Nconc(items, NewItemList((Item *)b));

  End;
}

/* }}} */

/* {{{ Merge Chords */

static void MergeChords(ItemList items)
{
  NoteTag  tag;
  Boolean  dotted;
  Chord   *a, *b;
  MTime    atime, btime, total;
  MarkList marks;

  Begin("MergeChords");

  if (!Next(items)) {
    ((Chord *)(items->item))->phrase.tied_forward = False;
    End;
  }

  a = (Chord *)(items->item);
  b = (Chord *)((iNext(items))->item);

  if (b->object_class != ChordClass) return;

  atime = a->methods->get_length((MusicObject)a);
  btime = a->methods->get_length((MusicObject)b);
  total = AddMTime(atime, btime);

  if (MTimeGreater(total, longestChord.chord.length)) return;
  tag = MTimeToTag(total, &dotted);
  marks = a->item.marks;

  (void)NewChord(a,
		 a->chord.voices,
		 a->chord.voice_count,
		 a->chord.modifiers,
		 tag, dotted);

  a->item.marks = marks;
  DestroyChord((MusicObject)b);
  Remove(Next(items));

  End;
}

/* }}} */
/* {{{ Merge Rests */

static void MergeRests(ItemList items)
{
  NoteTag  tag;
  Boolean  dotted;
  Rest    *a, *b;
  MTime    atime, btime, total;
  MarkList marks;

  Begin("MergeRests");

  if (!Next(items)) {
    ((Rest *)(items->item))->phrase.tied_forward = False;
    End;
  }

  a = (Rest *)(items->item);
  b = (Rest *)((iNext(items))->item);

  if (b->object_class != RestClass) End;

  atime = a->methods->get_length((MusicObject)a);
  btime = a->methods->get_length((MusicObject)b);
  total = AddMTime(atime, btime);

  if (MTimeGreater(total, longestChord.chord.length)) End;
  tag = MTimeToTag(total, &dotted);
  marks = a->item.marks;

  (void)NewRest(a, tag, dotted);

  a->item.marks = marks;
  DestroyRest((MusicObject)b);
  Remove(Next(items));

  End;
}

/* }}} */

/* {{{ Format Bar */

ItemList FormatBar(Bar            *bar,
		   Boolean         tiedBackward,
		   Boolean        *tiedForwardReturn,
		   ItemList        items,
		   Clef           *clef,
		   Clef          **clefReturn,
		   Key            *key,
		   Key           **keyReturn,
		   MTime          *delta)
{
  ItemList  i;
  Item     *item;
  ClassTag  oclass;
  MTime     length;
  MTime     thisLength;
  Begin("FormatBar");
  
  bar->bar.clef = clef;
  bar->bar.key  = key;
  bar->bar.start_time = *delta;

  bar->phrase.tied_backward = tiedBackward;

  bar->group.type  = GroupNoDecoration;
  bar->group.start = items;

  if (tiedForwardReturn) *tiedForwardReturn = False;
  if (clefReturn)        *clefReturn = clef;
  if (keyReturn)         *keyReturn  = key;

  length = bar->bar.time.bar_length;
  InitialiseAccidentalTable(key);

  *delta = AddMTime(*delta, length);

  for (ItemList_ITERATE(i, items)) {              /* see ItemList.h */

#ifdef DEBUG
    fprintf(stderr, "0x%p --\t\t\t\tprev 0x%p next 0x%p\n",
	    i, Prev(i), Next(i));

    i->item->methods->write((MusicObject)(i->item), stderr, 0);
#endif

    bar->group.end = i;

    item   = i->item;
    oclass = item->object_class;

    if (GROUPING_TYPE(item) != GroupNone && !item->item.grouping.beamed.start &&
	(!iPrev(i) || GROUPING_TYPE(item) != GROUPING_TYPE(iPrev(i)->item))) {
      item->item.grouping.beamed.start = True;
    }

    if (GROUPING_TYPE(item) != GroupNone && !item->item.grouping.beamed.end &&
	(!iNext(i) || GROUPING_TYPE(item) != GROUPING_TYPE(iNext(i)->item))) {
      item->item.grouping.beamed.end = True;
    }

    /* !!! */
    if (oclass == GroupClass) {
      fprintf(stderr, "WARNING: found Group in ItemList during FormatBar\n");
      continue;
    }
    
    if (GROUPING_TYPE(item) == GroupTupled &&
	item->item.grouping.tupled.start) {

      if (item->item.grouping.tupled.untupled_length >
	  item->item.grouping.tupled.untupled_length) {

	length = SubtractMTime(length, NumberToMTime
			  ((int)(item->item.grouping.tupled.tupled_length -
				 item->item.grouping.tupled.untupled_length)));
      } else {

	length = AddMTime(length, NumberToMTime
			  ((int)(item->item.grouping.tupled.untupled_length -
				 item->item.grouping.tupled.tupled_length)));
      }
      /*
      fprintf(stderr,"adding %d to compensate for tuplet group\n",
	      (int)(item->item.grouping.tupled.untupled_length -
		    item->item.grouping.tupled.tupled_length));
		    */
    }
    
    if (oclass == ChordClass || oclass == RestClass) {

      if (((Phrase *)i->item)->phrase.tied_forward) {
	
	if (Next(i)) {

	  switch(oclass) {
	  case ChordClass: MergeChords(i); break;
	  case  RestClass: MergeRests (i); break;
	  }
	} else ((Phrase *)i->item)->phrase.tied_forward = False;
      }

      if (oclass == ChordClass) UpdateAccidentalTable((Chord *)i->item);

    } else {

      switch(oclass) {

      case ClefClass:
	if (clefReturn) *clefReturn = (Clef *)item;
	break;

      case KeyClass:
	InitialiseAccidentalTable((Key *)item);
	if (keyReturn) *keyReturn = (Key *)item;
	break;

      default: break;
      }
    }

    thisLength = item->methods->get_length((MusicObject)item);

    if (MTimeGreater(thisLength, length)) {

      switch(oclass) {
	
      case ChordClass: SplitChord(i, thisLength, length); break;
      case  RestClass: SplitRest (i, thisLength, length); break;
      }

      bar->phrase.tied_forward = True;
      if (tiedForwardReturn) *tiedForwardReturn = True;
      Return(iNext(i));
    }

    length = SubtractMTime(length, thisLength);
    
    if (!(MTimeGreater(length, zeroTime)) &&
	(GROUPING_TYPE(i->item) != GroupTupled ||
	 i->item->item.grouping.tupled.end)) {
      Return(iNext(i));
    }

    if (i->item->item.bar_tag != NoFixedBar &&
	i->item->item.bar_tag != NoBarAtAll) {
      Return(iNext(i));
    }
  }

  Return(NULL);
}

/* }}} */
/* {{{ Unformat ItemList */

void UnformatItemList(ItemList *begin, ItemList *end) /* these are also rtns */
{
  Item    *item;
  ItemList items;
  ClassTag oclass;
  Boolean  ending = False;

  Begin("UnformatItemList");

  for (ItemList_ITERATE(items, *begin)) {

    item = items->item;
    oclass = item->object_class;

#ifdef DEBUG
    fprintf(stderr,"Inspecting item at 0x%p [0x%p]\n", items, item);
#endif

    if (end && items == *end) ending = True;

    if ((oclass==ChordClass || oclass==RestClass) &&
	((Phrase *)(items->item))->phrase.tied_forward)

      if (Next(items)) {

	if (end && Next(items) == (List)*end) {
	  ending = True; *end = (ItemList)items;
	}

	switch (oclass) {
	case ChordClass: MergeChords(items); break;
	case  RestClass: MergeRests(items);  break;
	}

      } else ((Phrase *)(items->item))->phrase.tied_forward = False;

    if (ending) break;
  }

#ifdef DEBUG
  fprintf(stderr,"\n\nUnformatted item list, items are\n\n");
  PrintItemList(*begin);
#endif
  
  End;
}

/* }}} */

/* {{{ Print Item List (debugging function) */

void PrintItemList(ItemList items)
{
  ItemList i;

  Begin("PrintItemList");

  for (ItemList_ITERATE(i, items)) {

    fprintf(stderr, "0x%p --\t\t\t\tprev 0x%p next 0x%p\n",
	    i, Prev(i), Next(i));

    i->item->methods->write((MusicObject)(i->item), stderr, 0);
  }

  End;
}

/* }}} */

