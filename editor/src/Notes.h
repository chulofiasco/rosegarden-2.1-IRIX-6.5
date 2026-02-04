
#ifndef _MUSIC_NOTES_
#define _MUSIC_NOTES_

#include "General.h"
#include "Lists.h"
#include "Tags.h"
#include "Visuals.h"
#include "MTime.h"


typedef short Pitch;     /* 0 is bottom line, 1 next gap up &c.  Can be -ve */



typedef struct _NoteVoice {
  Pitch         pitch;
  NoteMods      modifiers;
  NoteMods      display_mods;
} NoteVoice;

extern NoteVoice *NewNoteVoice(NoteVoice *, Pitch, NoteMods);
				/* type, dotted, pitch, mods */

extern Dimension DrawNoteVoice(NoteVoice *, Drawable, Position,
			       Position, Pitch, Dimension, NoteVisual,
			       Position, Position);


extern NoteVoice highestNoteVoice;
extern NoteVoice lowestNoteVoice;
extern NoteVoice metronomeNoteVoice;

/* These are in Methods.c */
extern int VoiceToMidiPitch(NoteVoice *, ClefTag);
extern NoteVoice MidiPitchToVoice(int, Boolean);



typedef struct _Mark {
  MarkTag       type;
  Boolean       start;
  struct _Mark *other_end;
  MusicObject   ilist;		/* used in drawing; cache only, may change */
} Mark;

typedef struct _MarkListElement {
  ListElement   typeless;
  Mark         *mark;
} MarkListElement, *MarkList;

extern MarkList NewMarkList(Mark *);
extern void     DestroyMarkList(MarkList);
extern Mark    *NewMark(Mark *, MarkTag, Boolean, Mark *);


/* actually two NoteVoice* args, for qsort comparator */
extern int noteVoiceCompare(const void *, const void *);

#endif /* _MUSIC_NOTES_ */

