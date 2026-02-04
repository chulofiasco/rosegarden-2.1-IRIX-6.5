
/* DrawOTGroups.c */
/* Musical Notation Editor for X, Chris Cannam 1994-95 */

/* Drawing methods for Groups and Bars to OpusTeX stream */

#include "Draw.h"

/* In this file, "lengths" refer to time and "widths" to on-screen
   space.  The width passed in is from 0 (\notes) to 5 (\NOTES). */
/* this should get adapted to opustex wider range */ 

static void StartBeam(FILE *f, int beams, int staff, Boolean down) {
  int i; 
  
  Begin("StartBeam");

  fprintf(f, "\\i"); 
  for (i = 0; i < beams; ++i) putc('b', f);

  if (down) putc('d', f);
  else      putc('u', f);

  fprintf(f, "%d", staff); /* Outch, this should be the
                              beam-refernce... but because rosegarden
                              (up to now) only handels one beam per
                              staff, this goes well. */

  End;
}

static void IncBeam(FILE *f, int beams, int staff) {
  int i; 
  
  Begin("IncBeam");

  fprintf(f, "\\n"); 
  for (i = 0; i < beams; ++i) putc('b', f);
  fprintf(f, "%d", staff); /* Outch, this should be the
                              beam-refernce... but because rosegarden
                              (up to now) only handels one beam per
                              staff, this goes well. */
  End;
}

static void TerminateBeam(FILE *f, int beams, int staff, Boolean shiftr) {
  int i; 
  
  Begin("TerminateBeam");

  fprintf(f, "\\t"); 
  if (shiftr)   fprintf(f, "r"); 
  for (i = 0; i < beams; ++i) putc('b', f);
  fprintf(f, "%d", staff); /* Outch, this should be the
                              beam-refernce... but because rosegarden
                              (up to now) only handels one beam per
                              staff, this goes well. */
  End;
}

/* Draw one item from a group.  We can't draw the whole group at once, */
/* because OpusTeX just doesn't work like that.  Take an argument for  */
/* which item to draw; then we pretend to draw the whole group but     */
/* really only output the single item.  Implies a traversal of the     */
/* group for every item in it; we're not too bothered for the moment.  */

/* We can't store information about the group between invocations,     */
/* because this must be re-entrant -- we may want to use it for groups */
/* active simultaneously on different staffs                           */

void DrawOTGroupItem(MusicObject obj, FILE *f, Dimension width,
                     int staff, ClefTag clef, ItemList ilist)
{
  Boolean     down;
  Boolean     beamed;
  int         beamCount;
  int         maxBeamCount;
  int         prevBeamCount;
  int         nextBeamCount;
  Pitch       imaginaryNoteHead;
  Dimension   probableWidth;
  Group     * group = (Group *)obj;
  ItemList    list, tempList;
  Item      * item;

  Begin("DrawOTGroupItem");

  /* we request the cache info given a reasonable-looking width for
     the group (based upon the passed width).  All we're needing is
     the beaming heuristic, which we hope won't be *too* far out */

  /* for tupled groups, just draw as beamed ones for now (hope it works) */

  beamCount = 0;
  maxBeamCount = 0;
  probableWidth = 0;

  GetGroupLayoutCacheInfo(group, ClefPitchOffset(clef), probableWidth);
  down = group->group.stems_down;

  beamed = ((group->group.type == GroupBeamed ||
             group->group.type == GroupTupled) &&
            group->group.eqn.eqn_present && group->group.need_beam);

  imaginaryNoteHead = down ?
    group->methods->get_lowest(group)->pitch  + 2 :
    group->methods->get_highest(group)->pitch - 2;

  /* we need: (1) beam up or down; (2) pitch of note-head that beam
     should be relative to (only the actual note-head if there's only
     going to be one beam throughout group); (3) gradient; (4)
     reference number -- use staff number as there can only be one
     beamed group per staff in Rosegarden; (5) initial no of beams */

  for (ItemList_ITERATE_GROUP(list, group)) {

    item = list->item;

    if (beamed &&
        item->object_class == ChordClass &&
        (Crotchet - ((Chord *)item)->chord.visual->type) > maxBeamCount) {

      maxBeamCount = Crotchet - ((Chord *)item)->chord.visual->type;

      if (beamCount == 0) {     /* first beamable */

        beamCount = maxBeamCount;

        if (down) {
          if (item->methods->get_lowest(item)->pitch < imaginaryNoteHead) {
            imaginaryNoteHead = item->methods->get_lowest(item)->pitch;
          }
        } else {
          if (item->methods->get_highest(item)->pitch >imaginaryNoteHead) {
            imaginaryNoteHead = item->methods->get_highest(item)->pitch;
          }
        }
      }
    }

    probableWidth += item->methods->get_min_width(item) + width*2 + 2;
  }
#ifdef NOT_DEFINED
  if (ilist == group->group.end) {
    group->group.cache_valid = False; /* anticipating the next "proper" plot */
  }
#endif
  if (beamed) {

    Boolean firstChord = False;

    for (ItemList_ITERATE_GROUP(list, group)) {
      if (list->item->object_class == ChordClass) {
        firstChord = (list == ilist);
        break;
      }
    }

    if (firstChord /*ilist == group->group.start*/) {

      if (down) imaginaryNoteHead -= maxBeamCount - 1;
      else      imaginaryNoteHead += maxBeamCount - 1;

      /*    fprintf(f, "\\relax%% beamed group\n");*/
      StartBeam(f, beamCount, staff, down);
      WriteOpusTeXPitchCode(f, imaginaryNoteHead, clef);
      fprintf(f, "{%d}",
              (group->group.eqn.m > 0.0) ?
              -(int)( group->group.eqn.m * 21.0) :
              (int)(-group->group.eqn.m * 21.0));
    }
  }

  prevBeamCount = 0;

  for (ItemList_ITERATE_GROUP(list, group)) {

    item = list->item;

#define TAILED(i) ((i)->object_class == ChordClass && \
                   ((Chord *)(i))->chord.visual->type < Crotchet)

    if (beamed && TAILED(item)) {

      /* beamCount is no of beams for current item; scan forward to
         next tailed Chord to find the nextBeamCount (yes, it's
         inefficient) */

      beamCount = Crotchet - ((Chord *)item)->chord.visual->type;
      nextBeamCount = 0;

      if (list != group->group.end) {

        for (tempList = iNext(list); tempList;
             tempList = (tempList == group->group.end ? NULL :
                         iNext(tempList))) {

          if (TAILED(tempList->item)) {
            nextBeamCount =
              Crotchet - ((Chord *)(tempList->item))->chord.visual->type;
            break;
          }
        }
      }

      /* We want to: open any beams which are more than the previous
         note needed but are *not* more than the next one will; and
         terminate any beams which are more than the next note needs,
         regardless of whether the previous note needed them or not.
         (But we've got to bear in mind different coding for the first
         note in the group.) */

      /* Notice that we only actually write anything for the item
         we're considering at the moment */

      if (prevBeamCount) {      /* not the first note */

        if (beamCount > prevBeamCount && beamCount <= nextBeamCount) {

          if (list == ilist) {
            IncBeam(f, beamCount, staff);
          }

        } else if (beamCount > nextBeamCount) {

          if (list == ilist) {

            /* note we always terminate for final note with "tbl" or
               "tb", after possibly using "tbb", "tbbu" etc */

            TerminateBeam(f, beamCount, staff,0);

            if (beamCount > 1 && nextBeamCount == 0) {
              TerminateBeam(f, 1, staff,0);
            }
          }
        }
      } else {                  /* first note */

        if (nextBeamCount > 0 && beamCount > nextBeamCount) {

          /* introduce new beam-lines which will then end immediately after
             the stem */

          if (list == ilist) {
            TerminateBeam(f, beamCount, staff, 1);
          }
        }
      }

      prevBeamCount = beamCount;
    }

    /* now if this is the right item, write it and quit */

    if (list == ilist) {
      item->methods->draw_opusTeX(item, f, width, staff, down, clef,
				  beamed); /* not: beamed&&TAILED(item) */

      /* problem -- the clef change won't be propagated back to the
         calling function */
      if (item->object_class == ClefClass)
        clef = ((Clef *)item)->clef.clef;

      break;
    }
  }

  /*  fprintf(f, "\\relax\n");*/
}

void DrawOTGroup(MusicObject obj, FILE *f, Dimension width,
                 int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Begin("DrawOTGroup");

  fprintf(f, "%% Rosegarden internal error: DrawOTGroup called, should\n");
  fprintf(f, "%% have been dealt with by repeated use of DrawOTGroupItem\n");

  End;
}

void DrawOTBar(MusicObject obj, FILE *f, Dimension width,
               int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Begin("DrawOTBar");

  fprintf(f, "%% Rosegarden internal error: DrawOTBar called, should\n");
  fprintf(f, "%% have been dealt with by Stave methods\n");

  End;
}


