
/* Include file for DrawElements.c and DrawGroups.c -- should be
   included only by these two files , as all it provides is their
   shared declarations */

/*
   Note on Drawing Coordinates.

   The X coordinate specified is the coordinate of the start of
   the note associated with that object.  Functions are free to
   place their objects to the left or right of this coordinate
   as appropriate.

   The Y coordinate is that of the top line of the stave.  The
   function is always free to draw up to StaveUpperGap pixels
   above, and StaveHeight + StaveLowerGap pixels below, that
   coordinate.

   If a width is passed which is less than the minimum width
   of the object to be drawn, the minimum width will be assumed
   instead.

   All drawing functions return the drawn width of the object.
   If the passed width was wider than the minimum object width,
   the returned width will be equal to the passed width;
   otherwise the minimum width will be returned.
*/

#ifndef _DRAW_H_
#define _DRAW_H_

#include "General.h"
#include "Notes.h"
#include "Classes.h"
#include "Visuals.h"
#include "GC.h"
#include "ItemList.h"
#include "Spline.h"

/* tie[0] holds info for back-tie, tie[1] for forward */

typedef struct _TieRec {
  Boolean  present;
  Position x;
  Boolean  above;
} TieRec;

extern TieRec tie[2];

#define EQN(e,x,X) ((int)(((double)(e)->m)*((X)-(x)))+((int)(e)->c))

extern void GetGroupLayoutCacheInfo(Group *, Pitch, Dimension);
extern void WriteMusicTeXPitchCode(FILE *, Pitch, ClefTag);

/* I guess this is really a Group method */
extern void DrawMTGroupItem(MusicObject, FILE *, Dimension,
			    int, ClefTag, ItemList);
extern void DrawOTGroupItem(MusicObject, FILE *, Dimension,
			    int, ClefTag, ItemList);

/* Function used to draw chords on boxes, used in the Edit Chord stuff &c */
extern void DrawClefAndChordOnSimpleDrawable(Clef *, Chord *,
					     Drawable, Dimension, Dimension,
					     Boolean, Pitch, Boolean);

/* This is REALLY nasty.  We want to find the x coord recorded in the
   group structure that contains a certain equation, given a pointer
   to the equation record in the group structure.  This will obviously
   fail badly if the equation supplied comes from the wrong group (or
   no group at all) -- */

#define EVIL_HACK_GROUP_X_COORD_FROM_EQN(E) (((Group *)((((char *)(E)) - \
   XtOffset(GroupPart *, eqn)) - XtOffset(Group *, group)))->item.x)


#endif
