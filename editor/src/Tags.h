
#ifndef _MUSIC_TAGS_
#define _MUSIC_TAGS_

/* These are referred to in Notes.h */

typedef unsigned int  NoteMods;
typedef unsigned long ChordMods;

#define ModNone      (0)

#define ModSharp     (1<<0)
#define ModFlat      (1<<1)
#define ModNatural   (1<<2)

#define ModDot       (1L<<0)
#define ModLegato    (1L<<1)
#define ModAccent    (1L<<2)
#define ModSfz       (1L<<3)
#define ModRfz       (1L<<4)
#define ModTrill     (1L<<5)
#define ModTurn      (1L<<6)
#define ModPause     (1L<<7)

#define ModDotPower    0
#define ModLegatoPower 1
#define ModAccentPower 2
#define ModSfzPower    3
#define ModRfzPower    4
#define ModTrillPower  5
#define ModTurnPower   6
#define ModPausePower  7


typedef short NoteTag;

#define Hemidemisemiquaver 0
#define Demisemiquaver     1
#define Semiquaver         2
#define Quaver             3
#define Crotchet           4
#define Minim              5
#define Semibreve          6
#define Breve              7

#define ShortestNote       0
#define LongestNote        7
#define NoteCount          8


typedef enum {
  Tie, Slur, Crescendo, Decrescendo
} MarkTag;


typedef char ClassTag;

#define ItemClass           ((char) 0)
#define MetronomeClass      ((char) 1)
#define ClefClass           ((char) 2)
#define KeyClass            ((char) 3)
#define TextClass           ((char) 4)
#define PhraseClass         ((char) 5)
#define RestClass           ((char) 6)
#define GroupClass          ((char) 7)
#define ChordClass          ((char) 8)
#define BarClass            ((char) 9)



typedef unsigned long MenuMode;

#define NullMode                 (0L)
#define AreaSweptMode            (1L<<0)
#define NoAreaSweptMode          (1L<<1)
#define SequencerRunningMode     (1L<<2)
#define SequencerNotRunningMode  (1L<<3)
#define FileLoadedMode           (1L<<4)
#define FileNotLoadedMode        (1L<<5)
#define CursorPlacedMode         (1L<<6)
#define CursorNotPlacedMode      (1L<<7)
#define NoFilenameToSaveInMode   (1L<<8)
#define SingleItemSweptMode      (1L<<9)
#define MultipleItemsSweptMode   (1L<<10)
#define SlaveToSequencerMode     (1L<<11)
#define ShowingNoChordNamesMode  (1L<<12)
#define ShowingChordNamesMode    (1L<<13)
#define ShowingAllChordNamesMode (1L<<14)
#define ShowingAllNoteNamesMode  (1L<<15)
#define UndoUnavailableMode      (1L<<16)
#define RedoUnavailableMode      (1L<<17)


typedef enum {
  TrebleClef, TenorClef, AltoClef, BassClef, InvalidClef
} ClefTag;

typedef enum {
  KeyA, KeyAflat, KeyB, KeyBflat, KeyC, KeyCflat, KeyCsharp,
  KeyD, KeyDflat, KeyE, KeyEflat, KeyF, KeyFsharp, KeyG, KeyGflat, InvalidKey
} KeyTag;

typedef enum {
  TextAboveStave, TextAboveStaveLarge, TextAboveBarLine,
  TextBelowStave, TextBelowStaveItalic, TextChordName, TextDynamic
} TextPosnTag;

extern int textDynamicCount;
extern String textDynamics[];

typedef enum {
  GroupNone, GroupNoDecoration, GroupBeamed, GroupTupled, GroupDeGrace
} GroupTag;

typedef enum {
  PaletteNotes, PaletteRests, PaletteClefs
} PaletteTag;


/* A bar begins with style X if the item before the first one in the
bar is marked with bar_tag X, and ends with style X if the last item
in it is marked with bar_tag X.  Styles for the start of the first bar
(and end of the last, in some particularly degenerate cases) are held
in the StaveEltList, but are always Double or Repeat. */

typedef enum {
  /* RepeatLeft is for repeat at the start of a bar; RepeatRight for
     end of a bar; RepeatBoth for -- guess! -- both */
  NoFixedBar, DoubleBar, RepeatRightBar, RepeatLeftBar, RepeatBothBar,
  PlainBar, NoBarAtAll
} BarTag;

typedef struct _StaveBarTag {
  BarTag precedes;
  BarTag follows;
} StaveBarTag;

#endif /* _MUSIC_TAGS_ */

