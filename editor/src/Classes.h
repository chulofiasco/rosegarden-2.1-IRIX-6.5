
#ifndef _MUSIC_CLASSES_
#define _MUSIC_CLASSES_

#include <stdio.h>

#include "Notes.h"
#include "Lists.h"
#include "Tags.h"

extern void InitialiseStaticObjects();


extern Dimension    GetZeroWidth    (MusicObject);
extern MTime        GetZeroLength   (MusicObject);
extern MusicObject  GetLongestChord (MusicObject); /* Chord not yet defined */
extern NoteVoice   *GetHighestNote  (MusicObject);
extern NoteVoice   *GetLowestNote   (MusicObject);
extern void         WriteNothing    (MusicObject, FILE *, int);
extern void         MidiWriteNothing(MusicObject, List, long*, int, ClefTag);
extern Dimension    DrawNothing     (MusicObject, Drawable, Position,
				     Position, Pitch, Dimension, LineEquation);
extern void         DrawMTNothing   (MusicObject, FILE *,
				     Dimension, int, Boolean, ClefTag, Boolean);
extern void         DrawOTNothing   (MusicObject, FILE *,
				     Dimension, int, Boolean, ClefTag, Boolean);


typedef struct _Methods {
  Dimension     (* get_min_width) (MusicObject);
  MusicObject   (* get_shortest)  (MusicObject);
  NoteVoice   * (* get_highest)   (MusicObject);
  NoteVoice   * (* get_lowest)    (MusicObject);
  MTime         (* get_length)    (MusicObject);
  MusicObject   (* clone)         (MusicObject);
  void          (* write)         (MusicObject, FILE *, int);

  void          (* write_midi)    (MusicObject, List, long*, int, ClefTag);
  /* item, EventList, delta (+delta rtn), channel, clef for offset */

  Dimension     (* draw)          (MusicObject, Drawable, Position, Position,
				   Pitch, Dimension, LineEquation);
  /* item, pixmap, x & y coordinates, clef offset, width, grouping line eqn */

  void          (* draw_musicTeX) (MusicObject, FILE *, Dimension,
				   int, Boolean, ClefTag, Boolean);
  /* item, stream, width, staff number, down flag, clef, beamed flag */

  void          (* draw_opusTeX)  (MusicObject, FILE *, Dimension,
				   int, Boolean, ClefTag, Boolean);
  /* item, stream, width, staff number, down flag, clef, beamed flag */

  void          (* destroy)       (MusicObject);
} Methods;


/* Grouping information within the Item structures */
/* Vital that all these structs begin with "type", and all but "None"
   begin with "type", "start" and "end" */

typedef struct _ItemGroupNoneRec {
  GroupTag type;
} ItemGroupNoneRec;

typedef struct _ItemGroupBeamedRec {
  GroupTag type;
  Boolean start;
  Boolean end;
} ItemGroupBeamedRec;

typedef struct _ItemGroupTupledRec {
  GroupTag type;
  Boolean start;
  Boolean end;
  short untupled_length;
  short tupled_length;
  short tupled_count;
} ItemGroupTupledRec;

typedef struct _ItemGroupGraceRec {
  GroupTag type;
  Boolean start;
  Boolean end;
} ItemGroupGraceRec;

typedef union _ItemGroupRec {
  ItemGroupNoneRec   none;
  ItemGroupBeamedRec beamed;
  ItemGroupTupledRec tupled;
  ItemGroupGraceRec  grace;
} ItemGroupRec;

typedef struct _ItemPart {
  MarkList      marks;
  short         x;
  BarTag        bar_tag;	/* Type of bar that *follows* this item */
  ItemGroupRec  grouping;
} ItemPart;

typedef struct _Item {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
} Item;

#define GROUPING_TYPE(x) ((x)->item.grouping.none.type)

extern Methods    itemMethods;
extern Boolean    ReadItem(MusicObject *, FILE *);
extern Item      *NewItem(Item *);
extern void       DestroyItem(MusicObject);
extern MusicObject CloneItem(MusicObject);

typedef struct _ItemListElement {
  ListElement   typeless;
  Item         *item;
} ItemListElement, *ItemList;

extern ItemList NewItemList(Item *); /* one-item list */
extern void     DestroyItemList(ItemList);


typedef struct _ClefPart {
  ClefTag       clef;
  ClefVisual    visual;
} ClefPart;

typedef struct _Clef {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  ClefPart      clef;
} Clef;  

extern Methods    clefMethods;
extern Clef      *NewClef(Clef *, ClefTag);	/* clef */
extern Dimension  GetClefWidth(MusicObject);
extern void       WriteClef(MusicObject, FILE *, int);
extern Boolean    ReadClef(MusicObject, FILE *);
extern Dimension  DrawClef(MusicObject, Drawable, Position,
			   Position, Pitch, Dimension, LineEquation);
extern void       DrawMTClef(MusicObject, FILE *, 
			     Dimension, int, Boolean, ClefTag, Boolean);
extern void       DrawOTClef(MusicObject, FILE *, 
			     Dimension, int, Boolean, ClefTag, Boolean);
extern void       DestroyClef(MusicObject);
extern Pitch      ClefPitchOffset(ClefTag);
extern MusicObject CloneClef(MusicObject);

extern Clef       defaultClef;


typedef struct _KeyPart {
  KeyTag        key;
  KeyTag        changing_from;
  KeyVisual     visual;
} KeyPart;

typedef struct _Key {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  KeyPart       key;
} Key;

extern Methods    keyMethods;
extern Key       *NewKey(Key *, KeyTag);	/* key */
extern Dimension  GetKeyWidth(MusicObject);
extern void       WriteKey(MusicObject, FILE *, int);
extern void       MidiWriteKey(MusicObject, List, long*, int, ClefTag);
extern Boolean    ReadKey(MusicObject, FILE *);
extern Dimension  DrawKey(MusicObject, Drawable, Position,
			  Position, Pitch, Dimension, LineEquation);
extern void       DrawMTKey(MusicObject, FILE *, 
			    Dimension, int, Boolean, ClefTag, Boolean);
extern void       DrawOTKey(MusicObject, FILE *, 
			    Dimension, int, Boolean, ClefTag, Boolean);
extern void       DestroyKey(MusicObject);
extern MusicObject CloneKey(MusicObject);

extern Key defaultKey;


/* This is no longer an Item, and shouldn't appear in an ItemList;
   it's used internally by the Bar and nowhere else */

typedef struct _TimeSignature {
  unsigned short numerator;
  unsigned short denominator;
  MTime          bar_length;
} TimeSignature;

extern TimeSignature *NewTimeSignature(TimeSignature *, unsigned short,
				       unsigned short);
extern Dimension      GetTimeSignatureWidth(MusicObject);
extern Dimension      DrawTimeSignature(MusicObject, Drawable, Position,
					Position, Pitch, Dimension,
					LineEquation);
extern TimeSignature  defaultTimeSignature;

extern MTime GetTimeSigBeatLength(TimeSignature *);

#define TimeSignaturesEqual(a,b) \
  ((a).numerator == (b).numerator && (a).denominator == (b).denominator)


typedef struct _TextPart {
  String        text;
  TextPosnTag   position;
} TextPart;

typedef struct _Text {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  TextPart      text;
} Text;

extern Text       chordText;

extern Methods    textMethods;
extern Text      *NewText(Text *, String, TextPosnTag); /* text, position */
extern void       WriteText(MusicObject, FILE *, int);
extern void       MidiWriteText(MusicObject, List, long*, int, ClefTag);
extern Boolean    ReadText(MusicObject, FILE *);
extern Dimension  DrawText(MusicObject, Drawable, Position,
			   Position, Pitch, Dimension, LineEquation);
extern void       DrawMTText(MusicObject, FILE *, 
			     Dimension, int, Boolean, ClefTag, Boolean);
extern void       DrawOTText(MusicObject, FILE *, 
			     Dimension, int, Boolean, ClefTag, Boolean);
extern void       DestroyText(MusicObject);
extern MusicObject CloneText(MusicObject);


typedef struct _PhrasePart {
  Boolean       tied_forward;
  Boolean       tied_backward;
} PhrasePart;

typedef struct _Phrase {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  PhrasePart    phrase;
} Phrase;

extern Methods    phraseMethods;
extern Phrase    *NewPhrase(Phrase *);
extern void       DestroyPhrase(MusicObject);
extern MusicObject ClonePhrase(MusicObject);


typedef struct _RestPart {
  MTime         length;
  RestVisual    visual;
} RestPart;

typedef struct _Rest {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  PhrasePart    phrase;
  RestPart      rest;
} Rest;

extern Methods      restMethods;
extern Rest        *NewRest(Rest *, NoteTag, Boolean); 	/* type, dotted */
extern Dimension    GetRestWidth(MusicObject);
extern MTime        GetRestLength(MusicObject);
extern MusicObject  GetRestShortest(MusicObject);
extern void         WriteRest(MusicObject, FILE *, int);
extern void         MidiWriteRest(MusicObject, List, long*, int, ClefTag);
extern Boolean      ReadRest(MusicObject, FILE *);
extern Dimension    DrawRest(MusicObject, Drawable, Position,
			     Position, Pitch, Dimension, LineEquation);
extern void         DrawMTRest(MusicObject, FILE *, 
			       Dimension, int, Boolean, ClefTag, Boolean);
extern void         DrawOTRest(MusicObject, FILE *, 
			       Dimension, int, Boolean, ClefTag, Boolean);
extern void         DestroyRest(MusicObject);
extern MusicObject  CloneRest(MusicObject);


/* cache and bars only!  no longer in ItemList */

typedef struct _GroupPart {
  GroupTag        type;
  ItemList        start;
  ItemList        end;
  short           tupled_length; /* if 0, not tupled - should really be MTime */
  short           tupled_count;	 /* number to be drawn in tupling line */
  /* cache info for layout and beam generation: */
  LineEquationRec eqn;
  Dimension       shortest_width;
  Boolean         need_beam;
  Boolean         stems_down;
} GroupPart;

typedef struct _Group {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  PhrasePart    phrase;
  GroupPart     group;
} Group;

extern Methods      groupMethods;
extern Group       *NewGroup(Group *, GroupTag, ItemList, ItemList);
				/* ^-- type, start, end */
extern Group       *NewFakeGroup(ItemList, ItemList);
extern Dimension    GetGroupWidth(MusicObject);
extern MTime        GetGroupLength(MusicObject);
extern MusicObject  GetGroupShortest(MusicObject);
extern NoteVoice   *GetGroupLowest(MusicObject);
extern NoteVoice   *GetGroupHighest(MusicObject);
extern void         WriteGroup(MusicObject, FILE *, int);
extern Boolean      ReadGroup(MusicObject, FILE *);
extern Dimension    DrawGroup(MusicObject, Drawable, Position,
			      Position, Pitch, Dimension, LineEquation);
extern void         DrawMTGroup(MusicObject, FILE *, 
				Dimension, int, Boolean, ClefTag, Boolean);
extern void         DrawOTGroup(MusicObject, FILE *, 
				Dimension, int, Boolean, ClefTag, Boolean);
extern void         DestroyGroup(MusicObject);
extern MusicObject  CloneGroup(MusicObject);




typedef struct _BarPart {

  long           number;	/* -1 if invalidated */
  TimeSignature  time;		/* I own this */

  /* The following are instantiated by the format method ... */

  Clef          *clef;		/* bar methods should never try to free these */
  Key           *key;
  MTime          start_time;	/* since the start of the piece */

  /* This is manipulated sometimes by the Bar and sometimes by the Stave: */

  short          width;		                       /* from last drawing */

} BarPart;

typedef struct _Bar {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  PhrasePart    phrase;
  GroupPart     group;
  BarPart       bar;
} Bar;

#define BarValid(x) ((x)&&((x)->bar.number>=0))
#define BarTimesEqual(x,y) TimeSignaturesEqual((x)->bar.time, (y)->bar.time)

extern Methods    barMethods;
extern Bar       *NewBar(Bar *, unsigned long, unsigned short, unsigned short);
				/* number, num & denom for time sig */
extern void       DestroyBar(MusicObject);

extern Dimension GetBarWidth(Bar *, Bar *); /* bar, prev bar */

/* DrawBar has to be called explicitly; the Method is just DrawNothing */
extern Dimension DrawBar(Bar *, Bar *, StaveBarTag *, Drawable, Position,
			 Position, Pitch, Dimension);
/* args: this bar, previous bar (for time sig), stave end tags, drawable,
   x, y, offset, width */ 

extern void      DrawMTBar(MusicObject, FILE *, 
			   Dimension, int, Boolean, ClefTag, Boolean);
extern void      DrawOTBar(MusicObject, FILE *, 
			   Dimension, int, Boolean, ClefTag, Boolean);
extern MusicObject CloneBar(MusicObject);

#define ITEM_AFTER_BAR(x) ((x)->group.end->item->object_class == GroupClass ? \
			   iNext(((Group *)(x)->group.end->item)->group.end) :\
			   iNext((x)->group.end))

/* can't tell start-of-piece/end-of-piece styles, assume Double */

#define BAR_START_TYPE(x) \
  (iPrev((x)->group.start) ? \
   iPrev((x)->group.start)->item->item.bar_tag : DoubleBar)
/*
#define BAR_END_TYPE(x) \
  (ITEM_AFTER_BAR(x) ? \
   ITEM_AFTER_BAR(x)->item->item.bar_tag.follows : DoubleBar)
   */
#define BAR_END_TYPE(x) \
  (((x)->group.end) ? \
   ((x)->group.end)->item->item.bar_tag : DoubleBar)


/* Chord contains NoteVoices sorted by pitch, with the lowest at the
   start of the array and highest at the end */

typedef struct _ChordPart {
  MTime         length;
  ChordMods     modifiers;
  NoteVisual    visual;
  NoteVoice    *voices;
  short         voice_count;
  Dimension     note_modifier_width;
  String        chord_name;
  Boolean       chord_named;	/* this name given explicitly? */
} ChordPart;

typedef struct _Chord {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  PhrasePart    phrase;
  ChordPart     chord;
} Chord;

extern Methods      chordMethods;

extern Chord       *NewChord(Chord *, NoteVoice *, int,
			     ChordMods, NoteTag, Boolean);

				/* note voices, voice count, mods,
				   note length tag, dotted */

extern void         NameChord(Chord *, String, Boolean);

extern Dimension    GetChordWidth(MusicObject);
extern MTime        GetChordLength(MusicObject);
extern MusicObject  GetChordShortest(MusicObject);
extern NoteVoice   *GetChordLowest(MusicObject);
extern NoteVoice   *GetChordHighest(MusicObject);
extern void         WriteChord(MusicObject, FILE *, int);
extern void         MidiWriteChord(MusicObject, List, long*, int, ClefTag);
extern Boolean      ReadChord(MusicObject, FILE *);
extern Dimension    DrawChord(MusicObject, Drawable, Position,
			      Position, Pitch, Dimension, LineEquation);
extern void         DrawMTChord(MusicObject, FILE *, 
				Dimension, int, Boolean, ClefTag, Boolean);
extern void         DrawOTChord(MusicObject, FILE *, 
				Dimension, int, Boolean, ClefTag, Boolean);
extern void         DestroyChord(MusicObject);
extern MusicObject CloneChord(MusicObject);

extern String GetChordDisplayedName(Chord *);
extern Dimension GetChordNameExtraWidth(Chord *);
extern Boolean GetChordStemDirection(Chord *, Clef *, int);

extern Chord longestChord;
extern Chord restChord;



typedef struct _MetronomePart {
  Chord         beat;
  MTime         beat_length;
  unsigned int  setting;
} MetronomePart;

typedef struct _Metronome {
  ClassTag      object_class;
  Methods      *methods;
  ItemPart      item;
  MetronomePart metronome;
} Metronome;

extern Methods    metronomeMethods;
extern Metronome *NewMetronome(Metronome *, NoteTag, Boolean, unsigned int);
				                /* beat, dotted, setting */
extern void       WriteMetronome(MusicObject, FILE *, int);
extern void       MidiWriteMetronome(MusicObject, List, long*, int, ClefTag);
extern Boolean    ReadMetronome(MusicObject, FILE *);
extern Dimension  DrawMetronome(MusicObject, Drawable, Position,
				Position, Pitch, Dimension, LineEquation);
extern void       DrawMTMetronome(MusicObject, FILE *, 
				  Dimension, int, Boolean, ClefTag, Boolean);
extern void       DrawOTMetronome(MusicObject, FILE *, 
				  Dimension, int, Boolean, ClefTag, Boolean);
extern void       DestroyMetronome(MusicObject);
extern MusicObject CloneMetronome(MusicObject);


#define IS_ITEM(x)      ((x)->object_class == ItemClass)
#define IS_METRONOME(x) ((x)->object_class == MetronomeClass)
#define IS_CLEF(x)      ((x)->object_class == ClefClass)
#define IS_KEY(x)       ((x)->object_class == KeyClass)
#define IS_TEXT(x)      ((x)->object_class == TextClass)
#define IS_PHRASE(x)    ((x)->object_class == PhraseClass)
#define IS_REST(x)      ((x)->object_class == RestClass)
#define IS_GROUP(x)     ((x)->object_class == GroupClass)
#define IS_CHORD(x)     ((x)->object_class == ChordClass)
#define IS_BAR(x)       ((x)->object_class == BarClass)



#endif /* _MUSIC_CLASSES_ */

