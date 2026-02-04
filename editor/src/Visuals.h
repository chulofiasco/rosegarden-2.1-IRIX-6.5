
#ifndef _MUSIC_VISUALS_
#define _MUSIC_VISUALS_

#include "General.h"
#include "Tags.h"


extern Pixmap noteDotMap;
extern Pixmap tailUpMap[4];
extern Pixmap tailDownMap[4];

/*
extern Cursor insertCursor;
extern Cursor dragCursor;
*/

extern Result InitialiseVisuals(void);
extern void   VisualsCleanUp(void);


/* 1: Notes.  We only use a pixmap for the round body of the note. */

typedef struct _NoteVisualRec {
  NoteTag       type;
  String        name;
  Pixmap        body;
  Boolean       stalked;
  Boolean       dotted;
  int           tails;
  Dimension     comfortable_gap;
} NoteVisualRec, *NoteVisual;

typedef struct _NoteVisualCompound {
  NoteVisualRec undotted;
  NoteVisualRec   dotted;
} NoteVisualCompound;

extern NoteVisualCompound noteVisuals[];
extern int noteVisualCount;


/* 2: Rests.  Height equal to that of the stave. */

typedef struct _RestVisualRec {
  NoteTag       type;
  String        name;
  Boolean       dotted;
  Pixmap        pixmap;
  Dimension     width;
} RestVisualRec, *RestVisual;

typedef struct _RestVisualCompound {
  char         *bitmap;
  RestVisualRec undotted;
  RestVisualRec dotted;
} RestVisualCompound;

extern RestVisualCompound restVisuals[];
extern int restVisualCount;


/* 3: Note mods.  All have width NoteModWidth and height NoteModHeight. */

typedef struct _NoteModVisualRec {
  NoteMods      type;
  String        name;
  char         *bitmap;
  Pixmap        pixmap;
} NoteModVisualRec, *NoteModVisual;

extern NoteModVisualRec noteModVisuals[];
extern int noteModVisualCount;


/* 4: Chord mods.  All have width NoteWidth and height ChordModHeight. */

typedef struct _ChordModVisualRec {
  ChordMods     type;
  String        name;
  char         *bitmap;
  Pixmap        pixmap;
} ChordModVisualRec, *ChordModVisual;

extern ChordModVisualRec chordModVisuals[];
extern int chordModVisualCount;


/* 5: Clefs.  Have width ClefWidth and height (StaveHeight+2*NoteHeight) */

typedef struct _ClefVisualRec {
  ClefTag       type;
  String        name;
  char         *bitmap;
  Pixmap        pixmap;
} ClefVisualRec, *ClefVisual;

extern ClefVisualRec clefVisuals[];
extern int clefVisualCount;


/* 6: Keys. */

typedef struct _KeyVisualRec {
  KeyTag        key;
  String        name;
  Boolean       sharps;		/* or, if False, flats */
  int           number;		/* of sharps or flats  */
} KeyVisualRec, *KeyVisual;

extern KeyVisualRec keyVisuals[];
extern int keyVisualCount;


#endif /* _MUSIC_VISUALS_ */

