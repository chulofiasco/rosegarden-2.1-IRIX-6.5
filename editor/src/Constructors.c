
/* Constructors.c */

/* Music Editor             */
/* Constructor/Destructor   */
/* Functions for Classes &c */
/* Chris Cannam, Jan 1994   */

/* Destructors should only  */
/* be called for objects    */
/* constructed by passing   */
/* null space into the new  */
/* function and letting it  */
/* allocate space itself    */

/* {{{ Includes */

#include <Lists.h>

#include "General.h"
#include "Tags.h"
#include "Notes.h"
#include "Classes.h"
#include "Stave.h"
#include "Visuals.h"
#include "ItemList.h"

/* }}} */
/* {{{ Macros */

#define SPACE(x,y)  { if (!(x)) (x) = (y *)malloc((Cardinal)sizeof(y)); \
                      if (!(x)) Error("Couldn't perform malloc"); }
#define CLASS(x,y)  { (x)->object_class = (y); }

/* }}} */

/* The paradigm for constructors is as follows.  The first argument */
/* is always a pointer to the type to be constructed.  If this is   */
/* NULL, space will be allocated using malloc, a new instance put   */
/* in it and a pointer to it returned.  If it is non-NULL, it will  */
/* be initialised with the new instance.  In either case, a pointer */
/* to the newly initialised space is returned.                      */

/* The exception is lists, which are opaque pointers anyway; their  */
/* constructors always use malloc, and don't take the initial arg.  */

/* Strings and lists passed as arguments to constructors will be    */
/* shared (no new copy will be made); destructors will likewise not */
/* attempt to destroy them.                                         */

/* NewMTime is in MTime.c */

/* {{{ NoteVoice */

NoteVoice *NewNoteVoice(NoteVoice *p, Pitch pitch, NoteMods modifiers)
{
  Begin("NewNoteVoice");

  SPACE(p, NoteVoice);

  p->pitch             = pitch;
  p->modifiers         = modifiers;
  p->display_mods      = modifiers;

  Return(p);
}


void DestroyNoteVoice(NoteVoice *p)
{
  Begin("DestroyNoteVoice");

  free((void *)p);

  End;
}

/* }}} */
/* {{{ Item */

/* CLASSES */


Item *NewItem(Item *p)
{
  Begin("NewItem");

  SPACE(p, Item);

  p->methods    = &itemMethods;
  p->item.marks = 0;
  p->item.x     = 0;

  p->item.bar_tag = NoFixedBar;
  p->item.grouping.none.type = GroupNone;

  CLASS(p, ItemClass);
  Return(p);
}


MusicObject CloneItem(MusicObject p)
{
  Item *newItem;
  Item *i = (Item *)p;
  Begin("CloneItem");

  newItem = NewItem(NULL);
  newItem->item.grouping = i->item.grouping;

  Return((MusicObject)newItem);
}


void DestroyItem(MusicObject p)
{
  Item *item = (Item *)p;
  Begin("DestroyItem");

  if (item->item.marks) DestroyMarkList(item->item.marks);
  free((void *)p);

  End;
}

/* }}} */
/* {{{ Metronome */

Metronome *NewMetronome(Metronome *p, NoteTag beat,
			Boolean dotted, unsigned int setting)
{
  Begin("NewMetronome");

  SPACE(p, Metronome);
  (void)NewItem((Item *)p);

  (void)NewChord(&(p->metronome.beat),
		 &metronomeNoteVoice, 1, ModNone, beat, dotted);

  p->metronome.beat_length = TagToMTime(beat, dotted);
  p->methods               = &metronomeMethods;
  p->metronome.setting     = setting;

  CLASS(p, MetronomeClass);
  Return(p);
}


MusicObject CloneMetronome(MusicObject p)
{
  Begin("CloneMetronome");
  Return((MusicObject)NewMetronome
	 (NULL, ((Metronome *)p)->metronome.beat.chord.visual->type,
	  ((Metronome *)p)->metronome.beat.chord.visual->dotted,
	  ((Metronome *)p)->metronome.setting));
}


void DestroyMetronome(MusicObject p)
{
  Begin("DestroyMetronome");

  DestroyItem(p);

  End;
}

/* }}} */
/* {{{ Clef */

Clef *NewClef(Clef *p, ClefTag tag)
{
  Begin("NewClef");
  
  SPACE(p, Clef);
  (void)NewItem((Item *)p);

  p->methods     = &clefMethods;
  p->clef.clef   = tag;
  p->clef.visual = &(clefVisuals[tag]);

  CLASS(p, ClefClass);
  Return(p);
}


MusicObject CloneClef(MusicObject p)
{
  Begin("CloneClef");
  Return(NewClef(NULL, ((Clef *)p)->clef.clef));
}


void DestroyClef(MusicObject p)
{
  Begin("DestroyClef");

  DestroyItem(p);

  End;
}

/* }}} */
/* {{{ Key */

Key *NewKey(Key *p, KeyTag tag)
{
  Begin("NewKey");

  SPACE(p, Key);
  (void)NewItem((Item *)p);

  p->methods = &keyMethods;
  p->key.key = tag;
  p->key.changing_from = KeyC;
  p->key.visual = &(keyVisuals[tag]);

  CLASS(p, KeyClass);
  Return(p);
}


MusicObject CloneKey(MusicObject p)
{
  Begin("CloneKey");
  Return(NewKey(NULL, ((Key *)p)->key.key));
}


void DestroyKey(MusicObject p)
{
  Begin("DestroyKey");

  DestroyItem(p);

  End;
}

/* }}} */
/* {{{ TimeSignature */

/* trying to use a sig whose denom is not a power of two may be troublesome */

TimeSignature *NewTimeSignature(TimeSignature *p,
				unsigned short numerator,
				unsigned short denominator)
{
  int count;

  Begin("NewTimeSignature");

  SPACE(p, TimeSignature);

  p->numerator   = numerator;
  p->denominator = denominator;
  p->bar_length  = NumberToMTime(numerator * GetTimeSigBeatLength(p));
  /*
  for (count = 0; denominator > 1; denominator = denominator/2, ++ count);
  (void)NewMTime(&(p->bar_length), Semibreve - count, numerator);
  */
  Return(p);
}

MTime GetTimeSigBeatLength(TimeSignature *sig)
{
  int count;
  int denominator = sig->denominator;
  Begin("GetTimeSigBeatLength");

  for (count = 0; denominator > 1; denominator = denominator/2, ++ count);

  Return(TagToMTime(Semibreve - count, False));
}


/* }}} */
/* {{{ Text */

Text *NewText(Text *p, String text, TextPosnTag tag)
{
  Begin("NewText");

  SPACE(p, Text);

  (void)NewItem((Item *)p);

  p->methods       = &textMethods;
  p->text.text     = XtNewString(text);
  p->text.position = tag;

  CLASS(p, TextClass);
  Return(p);
}


MusicObject CloneText(MusicObject p)
{
  Begin("CloneText");
  Return(NewText(NULL, XtNewString(((Text *)p)->text.text),
		 ((Text *)p)->text.position));
}


void DestroyText(MusicObject p)
{
  Begin("DestroyText");

  XtFree(((Text *)p)->text.text);
  DestroyItem(p);

  End;
}

/* }}} */
/* {{{ Phrase */

Phrase *NewPhrase(Phrase *p)
{
  Begin("NewPhrase");

  SPACE(p, Phrase);

  (void)NewItem((Item *)p);
  p->methods = &phraseMethods;

  p->phrase.tied_forward  = False;
  p->phrase.tied_backward = False;

  CLASS(p, PhraseClass);
  Return(p);
}


MusicObject ClonePhrase(MusicObject p)
{
  Begin("ClonePhrase");
  Return(NewPhrase(NULL));
}


void DestroyPhrase(MusicObject p)
{
  Begin("DestroyPhrase");

  DestroyItem(p);

  End;
}

/* }}} */
/* {{{ Rest */

Rest *NewRest(Rest *p, NoteTag tag, Boolean dotted)
{
  Begin("NewRest");

  SPACE(p, Rest);

  (void)NewPhrase((Phrase *)p);

  p->rest.length = TagToMTime(tag, dotted);

  if (dotted) p->rest.visual = &(restVisuals[tag].dotted);
  else        p->rest.visual = &(restVisuals[tag].undotted);

  p->methods = &restMethods;

  CLASS(p, RestClass);
  Return(p);
}


MusicObject CloneRest(MusicObject p)
{
  Rest *nr;
  Begin("CloneRest");

  nr = NewRest(NULL, ((Rest *)p)->rest.visual->type,
	       ((Rest *)p)->rest.visual->dotted);
  nr->item.grouping = ((Rest *)p)->item.grouping;

  Return(nr);
}


void DestroyRest(MusicObject p)
{
  Begin("DestroyRest");

  DestroyPhrase(p);

  End;
}

/* }}} */
/* {{{ Chord, including some name management functions */

/* Chord constructor and destructor assume you've got space  */
/* for the NoteVoice array using malloc or something, and    */
/* that it's allowed to free it up again when the destructor */
/* gets called.                                              */

/* We should sort NoteVoices by pitch, lowest up */
int noteVoiceCompare(const void *n, const void *m)
{
  NoteVoice *nn = (NoteVoice *)n;
  NoteVoice *mm = (NoteVoice *)m;

  if (nn->pitch == mm->pitch) {
    if (nn->modifiers & ModSharp) {
      if (mm->modifiers & ModSharp) return 0;
      else                          return 1;
    }
    if (nn->modifiers & ModFlat) {
      if (mm->modifiers & ModFlat)  return 0;
      else                          return -1;
    }
  }

  return nn->pitch - mm->pitch;
}

Chord *NewChord(Chord *p, NoteVoice *voices, int voiceCount,
		ChordMods modifiers, NoteTag tag, Boolean dotted)
{
  int i;
  Begin("NewChord");

  SPACE(p, Chord);

  (void)NewPhrase((Phrase *)p);

  if (voices && voiceCount > 1) {
    qsort(voices, voiceCount, sizeof(NoteVoice), noteVoiceCompare);

    for (i = 0; i < voiceCount-1; ++i) {

      /* if there are identical voices, remove all but one */
      while (i < voiceCount-1 &&
	     voices[i].pitch == voices[i+1].pitch &&
	     voices[i].modifiers == voices[i+1].modifiers) {

	memcpy(&voices[i], &voices[i+1], (voiceCount-i-1) * sizeof(NoteVoice));
	--voiceCount;
      }
    }
  }

  p->methods                   = &chordMethods;
  p->chord.voices              = voices;
  p->chord.voice_count         = voiceCount;
  p->chord.modifiers           = modifiers;
  p->chord.note_modifier_width = 0;
  p->chord.length              = TagToMTime(tag, dotted);
  p->chord.visual =
    dotted ? &(noteVisuals[tag].dotted) : &(noteVisuals[tag].undotted);

  p->chord.chord_name = 0;
  p->chord.chord_named = False;

  /*  NewText(&(p->chord.chord_name), "", TextChordName);*/

  CLASS(p, ChordClass);
  Return(p);
}


void NameChord(Chord *c, String name, Boolean explicit)
{
  if (!explicit && c->chord.chord_named && c->chord.chord_name) {
    fprintf(stderr, "Warning! replacing explicit with implicit chord name\n");
  }

  if (c->chord.chord_name) XtFree(c->chord.chord_name);
  c->chord.chord_name = name ? XtNewString(name) : 0;
  c->chord.chord_named = explicit;
}


MusicObject CloneChord(MusicObject p)
{
  int        i;
  NoteVoice *v;
  Chord     *c = (Chord *)p;
  Chord     *nc;

  Begin("CloneChord");

  v = (NoteVoice *)XtMalloc(sizeof(NoteVoice) * c->chord.voice_count);
  for (i = 0; i < c->chord.voice_count; ++i) v[i] = c->chord.voices[i];

  nc = NewChord(NULL, v, i, c->chord.modifiers,
		c->chord.visual->type, c->chord.visual->dotted);

  NameChord(nc, c->chord.chord_name, c->chord.chord_named);
  nc->item.grouping = c->item.grouping;

  Return(nc);
}


void DestroyChord(MusicObject p)
{
  Begin("DestroyChord");

  free((void *)(((Chord *)p)->chord.voices));
  if (((Chord *)p)->chord.chord_name) XtFree(((Chord *)p)->chord.chord_name);
  DestroyPhrase(p);

  End;
}

/* }}} */
/* {{{ Group */

/* Notice that the list given to a group may be a sublist of another   */
/* list; no operations should use First, Last &c. on the group's list. */
/* The group's destructor does not deallocate the itemlist.            */

Group *NewGroup(Group *p, GroupTag type, ItemList start, ItemList end)
{
  Begin("NewGroup");

  SPACE(p, Group);

  (void)NewPhrase((Phrase *)p);

  p->methods               = &groupMethods;
  p->group.start           = start;
  p->group.end             = end;
  p->group.type            = type;
  p->group.eqn.eqn_present = False;
  p->group.tupled_length   = 0;	/* someone else must set this afterwards */
  p->group.tupled_count    = 0;

  CLASS(p, GroupClass);
  Return(p);
}


/* make temporary group enclosing certain items */

Group *NewFakeGroup(ItemList start, ItemList end)
{
  Group *tGroup;
  GroupTag groupType = GROUPING_TYPE(start->item);

  Begin("NewFakeGroup");
  tGroup = NewGroup(NULL, groupType, start, end);

  if (groupType == GroupTupled) {

    tGroup->group.tupled_length =
      start->item->item.grouping.tupled.tupled_length;

    tGroup->group.tupled_count =
      start->item->item.grouping.tupled.tupled_count;
  }

  Return(tGroup);
}


/* This doesn't do the right thing with its list, because that's quite */
/* difficult to do without knowing the context.  Take care.            */

MusicObject CloneGroup(MusicObject p)
{
  Begin("CloneGroup");

  Return(NewGroup(NULL, ((Group *)p)->group.type,
		  ((Group *)p)->group.start, ((Group *)p)->group.end));
}


void DestroyGroup(MusicObject p)
{
  Begin("DestroyGroup");

  DestroyPhrase(p);

  End;
}


ItemList NewItemList(Item *item)
{
  ItemList list;
  
  Begin("NewItemList");

  list = (ItemList)NewList(sizeof(ItemListElement));
  list->item = item;

  Return(list);
}

/* }}} */
/* {{{ ItemList */

void DestroyItemList(ItemList list)
{
  ItemList first = (ItemList)First(list);

  Begin("DestroyItemList");

  list = first;

  while (list) {

    list->item->methods->destroy((MusicObject)(list->item));
    list = iNext(list);
  }

  DestroyList(first);

  End;
}

/* }}} */
/* {{{ Bar */

Bar *NewBar(Bar *p, unsigned long number, unsigned short num,
	    unsigned short denom)
{
  Begin("NewBar");

  SPACE(p, Bar);

  (void)NewGroup((Group *)p, GroupNoDecoration, NULL, NULL);

  p->methods = &barMethods;
  p->bar.number = number;

  (void)NewTimeSignature(&p->bar.time, num, denom);

  CLASS(p, BarClass);
  Return(p);
}


/* This is basically wrong.  Please don't use it. */

MusicObject CloneBar(MusicObject p)
{
  Begin("CloneBar");
  Return(NewBar(NULL, ((Bar *)p)->bar.number, ((Bar *)p)->bar.time.numerator,
	 ((Bar *)p)->bar.time.denominator));
}


void DestroyBar(MusicObject p)
{
  Begin("DestroyBar");

  DestroyGroup(p);

  End;
}

/* }}} */

