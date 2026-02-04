
#ifndef _MUSIC_STAVEPRIVATE_
#define _MUSIC_STAVEPRIVATE_

/* "Private" my arse.  This is included all over the place. */

#include <X11/Intrinsic.h>
#include "Classes.h"
#include "StaveCursor.h"
#include "Undo.h"

extern Widget  staveLabel;

/*
   What is a major stave?
   ======================

   A major stave, represented by the data type MajorStaveRec, is
   a description of an arrangement of musical notation in several
   staves, divided into bars, on a visible rectangular area.

   The major stave is equipped with the ability to highlight and
   edit parts of the musical text.
*/


typedef struct _StaveEltListElement {
  ListElement    typeless;
  Bar          **bars;		   /* array of pointers, not 2d array */
  Dimension     *widths;     /* widths[i] is width of bar at *bars[i] */
} StaveEltListElement, *StaveEltList;


/* Sweeping should be possible only from one chord (call it Item) to  */
/* another.  There should be no horizontal-band discrimination within */
/* a sweep, only rectangles the full height of the stave.  Functions  */
/* may be provided on the menu to handle dividing them if need be.    */
/* (Sounds a bit hopeful, surely?) */

typedef struct _StaveSweep {
  Boolean        swept;		       /* have we got an area at all? */
  int            stave;
  PointerRec     from;  /* from.left is item before first swept, or 0 */
  PointerRec     to;	                /* to.left is last swept item */
} StaveSweep;


typedef struct _StaveFormattingRec {
  int            next;	       /* first unformatted bar on that stave */
  Boolean        tied;
  Clef          *clef;
  Key           *key;
  ItemList       items;
} StaveFormattingRec;


typedef struct _MajorStaveRec {

  int                 staves;                           /* number of staves */
  int                 bars;	                        /* Length(bar_list) */
  int                 display_start;          /* first bar number displayed */
  int                 display_number; /* width of display, no of bars shown */
  MTime               total_length;                     /* approximate only */

  String             *names;
  int                *name_lengths;
  ItemList           *music;
  Boolean            *connected_down;
  int                *midi_patches;

  StaveBarTag        *bar_tags;	  /* "follows"->last bar, "precedes"->first */
  StaveEltList        bar_list;
  StaveFormattingRec *formats;	/* one record per stave (different keys &c) */

  StaveSweep          sweep;

  UndoStack           undo;

  Boolean             visible;

} MajorStaveRec;


extern Boolean      ShowingMIDIPatches;
extern StaveEltList NewStaveEltList(Bar **, BarTag *, Dimension *);
extern StaveSweep   NewSweep(void);
extern void         StaveForceUpdate(void);

extern void StaveFormatBars(MajorStave, int, int); /* stave, staffNo, endBar */


#define StaveTopHeight(x) ((x)*(StaveUpperGap+StaveHeight+StaveLowerGap) \
                           +StaveUpperGap)

#define StaveTotalHeight(x) (StaveTopHeight((x)->staves)- \
			     StaveUpperGap+StaveLowerGap)


#endif /* _MUSIC_STAVEPRIVATE_ */

