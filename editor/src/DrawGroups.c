
/* DrawGroups.c */
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Group and beam drawing methods */
/* In this file, "lengths" refer to time and "widths" to on-screen
   space */

/* {{{ Includes */

#include "Draw.h"

/* }}} */
/* {{{ Variables */

TieRec tie[2] = { { False, 0, False, }, { False, 0, False, }, };

#define IS_BEAMED(g) ((g)->group.type == GroupBeamed || \
		      (g)->group.type == GroupTupled)

/* }}} */

/* {{{ Beams */

typedef struct _BeamPoint {
  Position  x;
  Position  y;
  Dimension width;
  NoteTag   tag;
  Chord    *chord;
} BeamPoint;

#define BeamPointMax 1000

/* Making these global means that I */
/* can't have beamed groups within  */
/* beamed groups. I am unrepentant. */

static int       beampointcount = 0;
static BeamPoint beampoints[BeamPointMax+1];

#define TIE_DIST  (3*NoteHeight)


/* DrawBeam is entered with the beampoints array set up with an
   element for each item in the beamed group, indicating x and y
   positions for the *beam* at that note's stem (not for the note head
   itself), horizontal width between this note and the next, tag of
   this note, and a pointer to the relevant chord.  If you have more
   than BeamPointMax notes in a beamed group, the excess will appear
   unbeamed. */

void DrawBeamLine(Display *d, Drawable dr, int x1, int y1, int x2, int y2)
{
  int i;
  Begin("DrawBeamLine");

  /*
    XDrawLine(d, dr, beamGC, x1, y1, x2, y2);
    */

  for (i = 0; i < 3; ++i) {
    XDrawLine(d, dr, drawingGC, x1, y1-1+i, x2, y2-1+i);
  }

  End;
}

void DrawBeam(Drawable drawable, LineEquation eqn, Boolean down)
{
  NoteTag  longest = ShortestNote;
  NoteTag  curr, next, prev;
  Position y;
  int      x;
  int      dir;
  int      i;

  Begin("DrawBeam");

  dir = down ? -1 : 1;
  y = beampoints[0].y - EQN(eqn, beampoints[0].x, beampoints[0].x);

  /* draw the overall beam, from start to end with as many lines */
  /* as go continuously from one end of the group to the other */

  for (i = 0; i < beampointcount; ++i)
    if (beampoints[i].tag > longest) longest = beampoints[i].tag;

  for (x = 0; x < Crotchet - longest; ++ x)
    DrawBeamLine(display, drawable,
		 beampoints[0].x, beampoints[0].y + dir*x*5,
		 beampoints[beampointcount-1].x,
		 beampoints[beampointcount-1].y + dir*x*5);

  /* do the variations within the group */

  /* for NoteTags: */
#define NEEDS_MORE_BEAMS(a,b) ((a)<(b))

  for (i = 0; i < beampointcount; ++i) {

    if (beampoints[i].tag < longest) {

      /* this note needs extra beam(s) */

      curr = beampoints[i].tag;
      if (i > 0)                prev = beampoints[i-1].tag;
      if (i < beampointcount-1) next = beampoints[i+1].tag;

      if (i < beampointcount-1 && NEEDS_MORE_BEAMS(curr, next)) {

	if (i == 0 || NEEDS_MORE_BEAMS(curr,prev)) {
	  
	  /* this note needs no fewer lines than the following one,
             and more lines than the preceding one, so we introduce
             prev-curr new lines, drawn just to the left of the stem
             if i>0 or just to the right otherwise */

	  int partBeamWidth = i > 0 ?
	    beampoints[i-1].width/3 : beampoints[i].width*2/3;

	  if (partBeamWidth < 2) partBeamWidth = 2;
	  else if (partBeamWidth > 10) partBeamWidth = 10;
 
	  if (i > 0) {
	    for (x = curr; x < prev; ++x)
	      DrawBeamLine(display, drawable,
			   beampoints[i].x - partBeamWidth,
			   EQN(eqn, beampoints[0].x,
			       beampoints[i].x - partBeamWidth) + y +
			   dir*5*(Quaver-x),
			   beampoints[i].x,
			   beampoints[i].y + dir*5*(Quaver-x));
	  } else {
	    for (x = curr; x < next; ++x)
	      DrawBeamLine(display, drawable,
			   beampoints[i].x,
			   beampoints[i].y + dir*5*(Quaver-x),
			   beampoints[i].x + partBeamWidth,
			   EQN(eqn, beampoints[0].x,
			       beampoints[i].x + partBeamWidth) + y +
			   dir*5*(Quaver-x));
	  }
	}

	/* we now have the same number of beams as needed by the next
           note left to draw, so draw them all the way up to the next
           stem (note this is not the same as the next draw loop,
           which begins "for (x = curr...") */

	for (x = next; x < longest; ++x)
	  DrawBeamLine(display, drawable, beampoints[i].x,
		       beampoints[i].y + dir*5*(Quaver-x),
		       beampoints[i+1].x, beampoints[i+1].y+ dir*5*(Quaver-x));
	
      } else if (i < beampointcount-1) {

	/* we don't want any more beams than the next note, so draw in
           the ones we want, right up to the next stem, and leave it
           up to the next note whether to add any more */

	for (x = curr; x < longest; ++x)
	  DrawBeamLine(display, drawable, beampoints[i].x,
		       beampoints[i].y + dir*5*(Quaver-x),
		       beampoints[i+1].x, beampoints[i+1].y+ dir*5*(Quaver-x));
	
      } else if (i > 0 && NEEDS_MORE_BEAMS(curr, prev)) {
	
	/* we need extra beams which won't be wanted by the next note,
           so draw them just to the left of the stem */

	for (x = curr; x < prev; ++x) {

	  DrawBeamLine(display, drawable, beampoints[i].x,
		       beampoints[i].y + dir*5*(Quaver - x),
		       beampoints[i].x - beampoints[i].width*2/3,
		       EQN(eqn, beampoints[0].x,
			   beampoints[i].x - beampoints[i].width*2/3) + y +
		       dir*5*(Quaver - x));
	}
      }
    }
  }

  End;
}

/* }}} */
/* {{{ Tupling lines */

/* If we have more than one beampoint, then there must be a beam;   */
/* so use the beam's gradient (from the eqn) but place on the other */
/* side of the note heads; if there are fewer than two beampoints,  */
/* take the gradient according to the pitch difference between end  */
/* points, and draw the tupline line above the note heads.          */

/* This must be called after the notes have been drawn (so they all */
/* know their x-coordinates already)                                */

void DrawTuplingLine(Drawable drawable, Group *group, Position y, Pitch offset)
{
  int       width;
  /*  int       number;*/
  char      a[10];
  Item    * shortest;
  MTime     length;
  Item    * ai;
  Item    * bi;
  Position  ax, bx, ay, by;
  Position  ay0, ayd;
  Position  midx;
  float     midy;
  Position  tempy;
  float     gradient;
  int       tick_offset;

  static XFontStruct *font;
  static Boolean haveFont = False;

  Begin("DrawTuplingLine");

  if (!haveFont) {
    XGCValues values;
    if (XGetGCValues(display, tinyTextGC, GCFont, &values) == 0)
      Error("Could not get tupling line graphics-context values");
    font = XQueryFont(display, values.font);
    haveFont = True;
  }

  group->group.type = GroupNoDecoration;
  length = group->methods->get_length(group);
  group->group.type = GroupTupled;

  shortest = (Item *)group->methods->get_shortest(group);
  /*
  number =
    MTimeToNumber(length) /
    MTimeToNumber(shortest->methods->get_length(shortest));
    */
  sprintf(a, "%d", /*number*/ group->group.tupled_count);
  width = XTextWidth(font, a, strlen(a));

  ai = group->group.start->item;
  bi = group->group.end->item;

  ax = ai->item.x + 2;
  bx = bi->item.x + bi->methods->get_min_width(bi) - 2;

  if (beampointcount > 1 && group->group.eqn.eqn_present) {

    /* we have an equation; use it */

    /* if y_0 is the height of the note head, and y_1 the height of
       the beam at the note head's x coordinate, then we want y_2 =
       2*y_0 - y_1 (height of a point the same distance away from the
       head as the beam, but on the other side of the head).  We take
       this computation at the left hand end, and then use the offset
       thus obtained for the right hand end so as to retain the given
       equation's gradient. */

    /* for multi-headed chords, y_0 is the geometrical middle. */

    ay0 = (ai->methods->get_lowest(ai)->pitch + offset +
	   ai->methods->get_highest(ai)->pitch + offset) / 2;

    ayd = EQN(&group->group.eqn, group->item.x, ai->item.x);
    ay  = 2*(STAVE_Y_COORD(ay0)) - ayd;
    ayd = ay - ayd;
    ay += y;

    by = y + ayd + EQN(&group->group.eqn, group->item.x, bi->item.x);

    /*    fprintf(stderr,"case 1: ay %d, by %d\n", (int)ay, (int)by);*/

    /* we assume (most unwisely) that the equation we had was sure to
       give us at least three note heads distance away from the notes
       themselves; so we can move a couple of heads distance closer,
       so long as it doesn't make us too close to the staff itself */

    if (group->group.stems_down) {
	ay += NoteHeight; by += NoteHeight;
    } else {
	ay -= NoteHeight; by -= NoteHeight;
    }

    tick_offset = group->group.stems_down ? 3 : -3;

  } else {

    /* use note pitches if we have them; disregard tail up/down info,
       as well as the presence of accents &c., and just place the
       tupling line above the staff */

    if (ai->object_class == ChordClass) {

      ay = y + STAVE_Y_COORD(ai->methods->get_highest(ai)->pitch + offset) -16;
      if (ay > y + STAVE_Y_COORD(11)) ay = y + STAVE_Y_COORD(11);

    } else ay = y + STAVE_Y_COORD(12);

    if (bi->object_class == ChordClass) {

      by = y + STAVE_Y_COORD(bi->methods->get_highest(bi)->pitch + offset) -16;
      if (by > y + STAVE_Y_COORD(11)) by = y + STAVE_Y_COORD(11);

    } else by = y + STAVE_Y_COORD(12);

    /*    fprintf(stderr,"case 2: ay %d, by %d\n", (int)ay, (int)by);*/

    tick_offset = 3;
  }

  gradient = ((float)(by-ay))/((float)(bx-ax)); /* only needed for offsets */

  midx = (ax + bx) / 2;
  midy = (float)(ay + by) / 2.0;

  XDrawLine(display, drawable, drawingGC, ax, ay + tick_offset, ax, ay);

  /* beamGC is probably too thick -- draw two lines of drawingGC instead */

  tempy = midy - (gradient * ((float)(width/2 + 4)));
  XDrawLine(display, drawable, drawingGC, ax, ay,   midx - width/2-4, tempy);
  XDrawLine(display, drawable, drawingGC, ax, ay+1, midx - width/2-4, tempy+1);

  tempy = midy + (gradient * ((float)(width/2 + 4)));
  XDrawLine(display, drawable, drawingGC, midx + width/2+4, tempy,   bx, by);
  XDrawLine(display, drawable, drawingGC, midx + width/2+4, tempy+1, bx, by+1);

  XDrawLine(display, drawable, drawingGC, bx, by + tick_offset, bx, by);

  XDrawString(display, drawable, tinyTextGC, midx - width/2, midy + 3,
	      a, strlen(a));

  End;
}

/* }}} */
/* {{{ Undecorated groups (ie. Bar contents) */

/* At the moment this method is only (!) used to draw the contents of
   bars -- it's also called from DrawGroup but it's not actually
   possible to make an undecorated group yet, except as a bar */

Dimension DrawUndecoratedGroup(MusicObject obj, Drawable drawable, Position x,
			       Position y, Pitch offset, Dimension width,
			       LineEquation eqn)
{
  Group       * group = (Group *)obj;
  ItemList      list;
  MTime         length;
  MTime         shortestLength;
  static MTime  crotchetTime = zeroTime;
  unsigned int  shortnum;
  Dimension     itemWidth;
  Chord       * shortest;
  Item        * item;
  int           px, cx;

  Begin("DrawUndecoratedGroup");

  if (!obj) Return(width);
  if (MTimeEqual(crotchetTime, zeroTime))
    (void)NewMTime(&crotchetTime, Crotchet, 1);

  group->item.x = x;

  /* initialise to degenerate end points */
  shortestLength = ((Chord *)(GetLongestChord(NULL)))->chord.length;
  shortest = (Chord *)group->methods->get_shortest((MusicObject)group);
  shortnum = MTimeToNumber(shortest->methods->get_length(shortest));

  /* draw the notes */

  cx = x;

  for (ItemList_ITERATE_GROUP(list, group)) {

    /* cx is current x (at which to draw this item, updated by the act
       of drawing), px is x of previous item */

    px = cx;
    item = list->item;

    if (item->object_class == GroupClass) {
      fprintf(stderr, "WARNING: found Group in ItemList during "
	      "DrawUndecoratedGroup\n");
    }

    if (GROUPING_TYPE(item) != GroupNone &&
	(list == group->group.start || item->item.grouping.beamed.start)) {

      Group *tGroup;
      GroupTag groupType = GROUPING_TYPE(item);
      ItemList endList = list;

      while (endList && GROUPING_TYPE(endList->item) == groupType &&
	     !endList->item->item.grouping.beamed.end) {

	if (endList == group->group.end) break;
	endList = iNext(endList);
      }

      if (!endList) endList = (ItemList)Last(list);
      if (GROUPING_TYPE(endList->item) != groupType) endList = iPrev(endList);

      tGroup = NewFakeGroup(list, endList);
      item = (Item *)tGroup;
    }
    
    length = item->methods->get_length((MusicObject)item);

    /*    fprintf(stderr, "length = %d\n", (int)length);*/

    if (MTimeEqual(length, zeroTime)) {

      /* non-sounding objects, fixed drawing widths */

      if (item->object_class == TextClass &&
	  ((Text *)item)->text.position == TextAboveBarLine) {

	cx += item->methods->draw
	  ((MusicObject)item, drawable, x - 15, y, offset, 0, eqn);

      } else {

	if (item->object_class == ClefClass)
	  offset = ClefPitchOffset(((Clef *)item)->clef.clef);

	cx += item->methods->draw
	  ((MusicObject)item, drawable, cx, y, offset, 0, eqn);
      }
    } else {

      /* sounding objects, must be assigned widths according to their
         lengths */

      itemWidth = group->group.shortest_width * MTimeToNumber(length) /
	shortnum;

      if (IS_CHORD(item) && !MTimeLesser(length, crotchetTime)) {

	if (itemWidth >
	    NoteWidth + ((Chord *)item)->chord.note_modifier_width + 4) {

	  /* untailed & unbeamed notes with large amounts of screen
             space should be drawn a bit further right than planned,
             so as to look balanced in their space */

	  cx        += (itemWidth > 20 ? 4 : (itemWidth / 5));
	  itemWidth -= cx - px;
	  px         = cx;
	}
      }

      if (IS_CHORD(item)) {
	itemWidth += GetChordNameExtraWidth((Chord *)item);

      } else if (IS_GROUP(item)) { /* yes, even now */

	ItemList subList;
	for (ItemList_ITERATE_GROUP(subList, ((Group *)item))) {
	  if (IS_CHORD(subList->item)) {
	    itemWidth += GetChordNameExtraWidth((Chord *)subList->item);
	  }
	}
      }

      if (itemWidth == 0) itemWidth = 1;

      (void)item->methods->draw
	((MusicObject)item, drawable, cx, y, offset, itemWidth, eqn);

      cx += itemWidth;
    }

    if (IS_GROUP(item)) {
      list = ((Group *)item)->group.end;
      DestroyGroup((Group *)item); /* cache only */
    }
  }

  if (width > 0) Return(width);
  else Return((Dimension)(cx - x));
}

/* }}} */
/* {{{ Group layout cacheing */

/* We have the following problems in drawing a beamed group:      */
/*                                                                */
/*   == choosing whether to arrange the beam above or below;      */
/*   == finding a suitable gradient for the beam;                 */
/*   == choosing the height of the beam;                          */
/*   == drawing the notes, with stems pointing correctly;         */
/*   == drawing the beam, with the correct number of tails        */
/*      for each note in the group.                               */
/*                                                                */
/* Here we give the first three tasks to GetGroupLayoutCacheInfo; */
/* the fourth task is performed by DrawGroup, which also builds   */
/* up beam point information for the fifth task, which is handled */
/* by DrawBeam (above).                                           */

void GetGroupLayoutCacheInfo(Group *g, Pitch offset, Dimension width)
{
  ItemList      list;
  MTime         length;
  MTime         shortestLength;
  MTime         lastLength;
  NoteTag       shortestTag;
  Dimension     activeWidth = width;
  int           tailedNoteCount = 0;
  Chord       * shortest;
  Item        * item;

  /* These 2-elt arrays are used because, as we don't know whether
     we'll be beaming above or below the notes until we've analysed
     the pitch data, we have to record data for both the top and
     bottom lines of notes in the group.  We use elt 0 for the top
     line and elt 1 for the bottom. */

  int           direction[2];	/* 1 up, -1 down, 0 no direction, -2 dunno */
  Pitch         thispitch[2];
  Pitch         prevpitch[2];
  Pitch         firstpitch[2];
  Pitch         lastpitch[2];

  Pitch         minpitch;
  Pitch         maxpitch;
  int           beamAbove = 0;
  int           beamBelow = 0;
  int           down;
  static double grads[] = { (double)(0.1), (double)(0.17), (double)(0.3) };
  double        grad;
  int           nearestY;
  int           nearestX;
  int           i;

  Begin("GetGroupLayoutCacheInfo");

  /* degenerate defaults */
  shortestLength = ((Chord *)(GetLongestChord(NULL)))->chord.length;
  direction[0]   = direction[1] = -2;
  prevpitch[0]   = prevpitch[1] = -100;

  minpitch =  16;
  maxpitch = -16;

  for (ItemList_ITERATE_GROUP(list, g)) {

    /* get pitch and length data for the item */

    item     = list->item;
    shortest = (Chord *)(item->methods->get_shortest((MusicObject)item));
    length   = shortest->chord.length;

    /* need to know if we'll have 2 or more tailed notes -- if not, we
       won't want a beam */

    if (tailedNoteCount < 2 && shortest->chord.visual->type < Crotchet &&
	item->object_class != RestClass) ++tailedNoteCount;

    /* calculations involving length */

    if (MTimeLesser(length, shortestLength)) {
      shortestLength = length;
      shortestTag    = shortest->chord.visual->type;
    }

    length = item->methods->get_length((MusicObject)(list->item));

    if (MTimeEqual(length, zeroTime)) {
      activeWidth -=
	item->methods->get_min_width((MusicObject)(list->item));
    } else if (IS_CHORD(item)) {
      activeWidth -= GetChordNameExtraWidth((Chord *)item);
    }

    lastLength = length;

    /* consider the pitch */

    if (item->object_class == ChordClass) {

      thispitch[0] =
	item->methods->get_highest((MusicObject)item)->pitch + offset;
      thispitch[1] =
	item->methods->get_lowest ((MusicObject)item)->pitch + offset;

      if (thispitch[1] < minpitch) minpitch = thispitch[1];
      if (thispitch[0] > maxpitch) maxpitch = thispitch[0];

      for (i = 0; i < 2; ++i) {

	if (prevpitch[i] == -100) prevpitch[i] = firstpitch[i] = thispitch[i];
	lastpitch[i] = thispitch[i];

	if (thispitch[i] < 5) beamAbove += (5-thispitch[i]);
	else                  beamBelow += (thispitch[i]-4);

	if      (thispitch[i] < prevpitch[i])
	  if (direction[i] ==  1 || direction[i] == 0) direction[i] =  0;
	  else                                         direction[i] = -1;
	else if (thispitch[i] > prevpitch[i])
	  if (direction[i] == -1 || direction[i] == 0) direction[i] =  0;
	  else                                         direction[i] =  1;

	prevpitch[i] = thispitch[i];
      }
    }
  }

  /* 2. Compute with the acquired length data. */

  if (g->group.type == GroupTupled) {

    if (!g->group.start || !g->group.start->item ||
	GROUPING_TYPE(g->group.start->item) != GroupTupled) {
      fprintf(stderr, "Warning: some failure in GetGroupLayoutCacheInfo for "
	      "tupled group; layout will\nprobably be wrong\n");
      length = g->methods->get_length(g);
    } else {
      length = g->group.start->item->item.grouping.tupled.untupled_length;
    }

  } else if (g->group.type == GroupDeGrace) {

    g->group.type = GroupNoDecoration;
    length = g->methods->get_length(g);
    g->group.type = GroupDeGrace;

  } else length = g->methods->get_length(g);

  /* g->group.shortest_width is the screen width of the shortest note
     in the group, used as a multiplier for widths; shortestLength is
     the length of that note, for the ratio */

  if (MTimeEqual(length, zeroTime)) {

    g->group.shortest_width = NoteWidth + 2;

  } else {
  
    g->group.shortest_width =
      (Dimension)((long)activeWidth*(MTimeToNumber(shortestLength)) /
		  MTimeToNumber(length));
  }

  /* 3. Compute with the acquired pitch data -- get above or    */
  /*    below, and gradient; have a guess at a suitable offset. */

  /*    A tupled group is assumed to be beamed as well; tupling line
        should generally be drawn on the opposite side of the note
        heads to the beam, but with the same gradient.  (See
        DrawTuplingLine.)  Fill in beampoints as for a beamed group.  */

  if (IS_BEAMED(g)) {

    /* if there were more notes above the middle line than below, draw
       the beam below */

    down = beamAbove > beamBelow ? 0 : 1;
/*    down = g->group.type == GroupTupled ? 1-down : down;*/ /* er, why? */

    /* assign a gradient based on direction and end pitches --
       direction[down] will be 0 if the group is neither nonincreasing
       nor nondecreasing, and -2 if there's only one unique pitch */

    if (direction[down] == 0 || direction[down] == -2) {
      if      (firstpitch[down] -  lastpitch[down] > 2) grad =  grads[0];
      else if ( lastpitch[down] - firstpitch[down] > 2) grad = -grads[0];
      else grad = (double)0;

    } else {		                 /* some overall direction */
      if      (firstpitch[down] -  lastpitch[down] > 4) grad =  grads[2];
      else if ( lastpitch[down] - firstpitch[down] > 4) grad = -grads[2];
      else if (firstpitch[down] -  lastpitch[down] > 3) grad =  grads[1];
      else if ( lastpitch[down] - firstpitch[down] > 3) grad = -grads[1];
      else grad = direction[down] > 0 ? -grads[0] : grads[0];
    }

    /* Offset should be such that the point on the line nearest   */
    /* to the note group should be at least NoteHeight away       */
    /* from the note nearest the beam.                            */

    if (down) {

      nearestY = STAVE_Y_COORD(minpitch) + NoteHeight*2;
      if (grad < (grads[1] - 0.01) && grad > (-grads[1] + 0.01))
	nearestY += NoteHeight;
      if (grad < (grads[0] - 0.01) && grad > (-grads[0] + 0.01))
	nearestY += NoteHeight/2;

      /* I think this computation of nearestX is at fault; I think it */
      /* should obtain the x coord of the lowest note head (similarly */
      /* in the other conditional branch, with the highest note head) */

      if (direction[down] > 0)
	nearestX = width -
	  (g->group.shortest_width *
	   MTimeToNumber(lastLength)/MTimeToNumber(shortestLength))
	  - NoteWidth/2;
      else nearestX = 0;

      if (shortestTag < Quaver) nearestY += 3*(Quaver-shortestTag);

    } else {

      /* reflection of the other branch of the conditional */

      nearestY = STAVE_Y_COORD(maxpitch) - NoteHeight;
      if (grad < (grads[1] - 0.01) && grad > (-grads[1] + 0.01))
	nearestY -= NoteHeight;
      if (grad < (grads[0] - 0.01) && grad > (-grads[0] + 0.01))
	nearestY -= NoteHeight/2;

      nearestX = g->group.start->item->methods->get_min_width
	((MusicObject)(g->group.start->item));

      if (direction[down] <= 0)
	nearestX +=
	  width - (g->group.shortest_width * MTimeToNumber(lastLength) /
		   MTimeToNumber(shortestLength));

      if (shortestTag < Quaver) nearestY -= 3*(Quaver-shortestTag);
    }

    g->group.stems_down = down;

    /* want "nearestY = m * nearestX + c"; we have m already. */
    /* we'd also like eqn independent of x&y-- expressed only */
    /* in terms of relative distance east of x -- hence the   */
    /* revisionist removal of x from the nearestX exprs above */

    g->group.eqn.m = grad;
    g->group.eqn.c = nearestY - (int)(grad * nearestX);
    g->group.eqn.eqn_present = True;
    g->group.eqn.reverse = False;

  } else {
    g->group.eqn.eqn_present = False;
  }

  g->group.need_beam = (IS_BEAMED(g) && tailedNoteCount > 1);
  /*  g->group.cache_valid = True;*/
  End;
}

/* }}} */
/* {{{ Groups */

Dimension DrawGroup(MusicObject obj, Drawable drawable, Position x,
		    Position y, Pitch offset, Dimension width,
		    LineEquation eqn)
{
  Group       * group = (Group *)obj;
  ItemList      list;
  MTime         length;
  static MTime  crotchetTime = zeroTime;
  unsigned int  shortnum;
  Dimension     itemWidth;
  Chord       * shortest;
  int           px, cx;
  ItemList      barStart = 0;

  Begin("DrawGroup");

  if (!obj) Return(width);
  if (MTimeEqual(crotchetTime, zeroTime))
    (void)NewMTime(&crotchetTime, Crotchet, 1);

  group->item.x = x;
  GetGroupLayoutCacheInfo(group, offset, width); /* here or after next cond? */

  if (group->object_class == BarClass) {
    barStart = group->group.start;

    for (ItemList_ITERATE_GROUP(list, group)) {
      if (list->item->object_class != ClefClass &&
	  list->item->object_class != KeyClass &&
	  list->item->object_class != MetronomeClass &&
	  list->item->object_class != TextClass) break;
    }

    if (list) group->group.start = list;
    else Return(width);
  }

  /* we can't streamline the trivial case (start==end) for bars,
     because sometimes the last bar in a staff has start==end even
     though it may contain several items. I'm not _completely_ sure
     why this happens -- cc 11/95: */

  if (group->group.type != GroupDeGrace &&
      group->group.type != GroupNoDecoration &&
      group->group.start == group->group.end) {

    Return(group->group.start->item->methods->draw
	   ((MusicObject)(group->group.start->item),
	    drawable, x, y, offset, width, NULL /*??? &group->group.eqn*/));
  }

  if (group->group.type == GroupNoDecoration) {	/* true for bars */
    width = DrawUndecoratedGroup(obj, drawable, x, y, offset, width,
				 &group->group.eqn);
    if (barStart) group->group.start = barStart;
    Return(width);
  }

  if (group->group.type == GroupTupled &&
      (group->group.tupled_length == 0 || group->group.tupled_count == 0)) {
    group->group.type = GroupBeamed; /* ugh */
  }

  shortest = (Chord *)group->methods->get_shortest((MusicObject)group);
  shortnum = MTimeToNumber(shortest->methods->get_length(shortest));

  /* Having got the starting data (including group layout cache info),
     now draw the notes and beams */

  beampointcount = 0;
  cx = x;

  for (ItemList_ITERATE_GROUP(list, group)) {
    
    px = cx;
    
    length = list->item->methods->get_length((MusicObject)(list->item));

    if (MTimeEqual(length, zeroTime)) {

      if (list->item->object_class == ClefClass)
	offset = ClefPitchOffset(((Clef *)list->item)->clef.clef);

      cx += list->item->methods->draw
	((MusicObject)(list->item), drawable, cx, y, offset, 0,
	 &group->group.eqn);

    } else {

      itemWidth = group->group.shortest_width * MTimeToNumber(length) /
	shortnum;

      if (list->item->object_class == ChordClass) {

	itemWidth += GetChordNameExtraWidth((Chord *)list->item);
	
	if (!(MTimeLesser(length, crotchetTime))) {

	  if (itemWidth > NoteWidth +
	      ((Chord *)list->item)->chord.note_modifier_width + 4) {

	    /* untailed notes with lots of screen space should be
               moved right a bit to fill the space with more balance */

	    cx        += (itemWidth > 20 ? 4 : (itemWidth / 5));
	    itemWidth -= cx - px;
	    px         = cx;
	  }

	  /* if this is an untailed note in the middle of a tailed
             group, we should reverse its stem */

	  if (group->group.eqn.eqn_present && IS_BEAMED(group) &&
	      list != group->group.start && list != group->group.end)
	    group->group.eqn.reverse = True;

	} else {

	  if (!IS_BEAMED(group)) itemWidth += 3;
	}
      }

      if (itemWidth == 0) itemWidth = 1;

      (void)list->item->methods->draw
	((MusicObject)(list->item), drawable, cx, y, offset, itemWidth,
	 (IS_BEAMED(group) && group->group.need_beam)? &group->group.eqn :NULL);

      cx += itemWidth;

      /* establish that this is a note which will need a beampoint
         record, and if so assign one.  eqn will only be allocated if
         we're wanting a beam or something */

      if (group->group.eqn.eqn_present && group->group.need_beam &&
	   list->item->object_class == ChordClass) {

	if (IS_BEAMED(group) &&
	    ((Chord *)list->item)->chord.visual->type < Crotchet) {

	  BeamPoint *bpt = &(beampoints[beampointcount]);

	  if (group->group.stems_down) {
	    bpt->x = px +
	      ((Chord *)(list->item))->chord.note_modifier_width;
	  } else {
	    bpt->x = px + NoteWidth - 2 +
	      ((Chord *)(list->item))->chord.note_modifier_width;
	  }

	  bpt->y = EQN(&group->group.eqn, beampoints[0].x, bpt->x) + y;
	  bpt->chord = (Chord *)(list->item);
	  bpt->tag  = ((Chord *)(list->item))->chord.visual->type;
	  bpt->width = (Dimension)(group->group.shortest_width *
				   MTimeToNumber(length) / shortnum);

	  if (beampointcount < BeamPointMax) beampointcount ++;
	}
      }

      if (group->group.eqn.eqn_present) group->group.eqn.reverse = False;
    }
  }

  if (beampointcount > 0 && IS_BEAMED(group)) {
    DrawBeam(drawable, &group->group.eqn, group->group.stems_down);
  }

  if (group->group.type == GroupTupled) {
    DrawTuplingLine(drawable, group, y, offset);
  }

  if (width > 0) Return(width);
  else Return((Dimension)(cx - x));
}

/* }}} */
/* {{{ Bars */

/* 

   love is more thicker than forget
   more thinner than recall
   more seldom than a wave is met
   more frequent than to fail

   it is most mad and moonly
   and less it shall unbe
   than all the sea which only
   is deeper than the see

   love is less always than to win
   less never than alive
   less bigger than the least begin
   less littler than forgive

   it is most sane and sunly
   and more it cannot die
   than all the sky which only
   is higher than the sky


   (e.e.cummings)

*/



Dimension DrawBar(Bar *bar, Bar *prevBar, StaveBarTag *staveTags,
		  Drawable drawable, Position x, Position y,
		  Pitch offset, Dimension width)
{
  Position  sy;
  Dimension actwid;
  Position  off;
  BarTag    start, end;
  XPoint    tieControl;
  XPoint    tieStart;
  XPoint    tieEnd;
  ItemList  list;
  int       i;

  Begin("DrawBar");

  if (!bar) Return(width);

  actwid = width;
  off    = x;

  start = BAR_START_TYPE(bar);
  end = BAR_END_TYPE(bar);

  if (!iPrev(bar->group.start)) {
    start = staveTags ? staveTags->precedes : NoFixedBar;
  }

  if (!bar->group.end) {
    end = staveTags ? staveTags->follows : NoFixedBar;
  }

  if (tie[1].present) tie[0].above = tie[1].above;
  tie[0].present = tie[1].present = False;

  /* If required, draw the opening bar line.  We only need this if it's  */
  /* a double or opening-repeat bar (and then it goes after the clef and */
  /* key sig), or if this is the first bar in the line (in which case    */
  /* see above); otherwise, the ending line on the previous bar will do. */

  switch(start) {

  case RepeatLeftBar:
  case RepeatBothBar:
    XDrawLine(display, drawable, drawingGC, off, y, off, y+StaveHeight-1);
    XDrawLine(display, drawable, drawingGC, off+1, y, off+1, y+StaveHeight-1);
    XDrawLine(display, drawable, drawingGC, off+4, y, off+4, y+StaveHeight-1);

    CopyArea(noteDotMap, drawable, 0, 0,
	     DotWidth, NoteHeight, off + 5, y + STAVE_Y_COORD(3));
    CopyArea(noteDotMap, drawable, 0, 0,
	     DotWidth, NoteHeight, off + 5, y + STAVE_Y_COORD(5));

    off += 5 + DotWidth;
    break;

  case DoubleBar:
    XDrawLine(display, drawable, drawingGC, off, y, off, y+StaveHeight-1);
    XDrawLine(display, drawable, drawingGC, off+1, y, off+1, y+StaveHeight-1);
    XDrawLine(display, drawable, drawingGC, off+4, y, off+4, y+StaveHeight-1);

    off += 5;
    break;

  default:
    break;
  }

  if (!(bar->group.start &&
	bar->group.start->item->object_class == ClefClass)) off += 5;
  actwid -= off-x;

  /* Draw in the closing bar line */
    
  switch(end) {
      
  case NoFixedBar:
  case RepeatLeftBar:

    XDrawLine(display, drawable, drawingGC,
	      x + width-1, y, x + width-1, y + StaveHeight - 1);
    actwid -= 3;
    break;

  case PlainBar:

    XDrawLine(display, drawable, drawingGC,
	      x + width-1, y, x + width-1, y + StaveHeight - 1);
    XDrawLine(display, drawable, drawingGC,
	      x + width-1, y, x + width-4, y-3);
    XDrawLine(display, drawable, drawingGC,
	      x + width-1, y + StaveHeight - 1, x + width-4, y + StaveHeight+2);
    actwid -= 3;
    break;

  case RepeatRightBar:
  case RepeatBothBar:

    CopyArea(noteDotMap, drawable, 0, 0, DotWidth, NoteHeight,
	     x + width - 5 - DotWidth, y + STAVE_Y_COORD(3));
    CopyArea(noteDotMap, drawable, 0, 0, DotWidth, NoteHeight,
	     x + width - 5 - DotWidth, y + STAVE_Y_COORD(5));

    actwid -= DotWidth;

    /* fall through */

  case DoubleBar:
    
    XDrawLine(display, drawable, drawingGC,
	      x + width-1, y, x + width-1, y + StaveHeight - 1);
    XDrawLine(display, drawable, drawingGC,
	      x + width-2, y, x + width-2, y + StaveHeight - 1);
    XDrawLine(display, drawable, drawingGC,
	      x + width-5, y, x + width-5, y + StaveHeight - 1);

    actwid -= 7;
    break;

  case NoBarAtAll:
    actwid += 5;		/* why 5?  who knows */
    break;
  }

  /* Four-stage process for drawing the items within the bar.  First
     we draw clef and key if they appear at the start of the bar.
     Then if we have a new time signature to draw, place it next; then
     draw any text or metronome events from the starting zero-length-
     item block; then draw the rest of the bar.  (DrawGroup is clever
     enough to know that opening clefs and keys in bars should be
     ignored.)  */

  for (ItemList_ITERATE_GROUP(list, bar)) {

    if (IS_TEXT(list->item)) continue;

    if (!IS_METRONOME(list->item) &&
	!IS_CLEF(list->item) && !IS_KEY(list->item)) break;

    if (IS_CLEF(list->item))
      offset = ClefPitchOffset(((Clef *)list->item)->clef.clef);

    off += list->item->methods->draw
      (list->item, drawable, off + 2, y, offset, 0, NULL);
  }

  /* time signature? */

  if (start != NoBarAtAll && end != NoBarAtAll &&
      (!prevBar ||
       prevBar->bar.time.numerator   != bar->bar.time.numerator ||
       prevBar->bar.time.denominator != bar->bar.time.denominator ||
       BAR_START_TYPE(prevBar) == NoBarAtAll)) {

    int tsw;
    tsw = DrawTimeSignature(&bar->bar.time, drawable, off, y, offset, 0, NULL);
    off += tsw; actwid -= tsw;
  }

  for (ItemList_ITERATE_GROUP(list, bar)) {

    if (IS_CLEF(list->item))
      offset = ClefPitchOffset(((Clef *)list->item)->clef.clef);

    if (list->item->object_class == ClefClass ||
	list->item->object_class == KeyClass  ||
	list->item->object_class == MetronomeClass) continue;

    if (list->item->object_class != TextClass) break;

    off += list->item->methods->draw
      (list->item, drawable, off + 2, y, offset, 0, NULL);
  }

  /* Now fill in the contents of the bar */

  /*  DrawUndecoratedGroup */
  DrawGroup((MusicObject)bar, drawable, off + 2, y,
	    offset, actwid>3 ? actwid-3 : 2, NULL);

  /* Draw the stave lines */
    
  for (sy = 0 ; sy < StaveHeight; sy += NoteHeight + 1)
    XDrawLine(display, drawable, drawingGC, x, y + sy, x+width-1, y + sy);

  /* And finally, draw the ties if they're called for. (We sometimes
     seem to get ties beginning above the staff and ending below, or
     vice versa: gotta get around to dealing with this.) */

  for (i = 0; i < 2; ++i) {

    if ((i ? bar->phrase.tied_forward : bar->phrase.tied_backward) &&
	tie[i].present) {

      tieStart.x = i ? tie[i].x + 10 : tie[i].x - 10;
      tieEnd.x   = tieControl.x = tie[i].x;

      tieStart.y = tieControl.y =
	tie[i].above ?  y - TIE_DIST - 4 :
	  y + StaveHeight + TIE_DIST + 4;

      tieEnd.y = tieStart.y + (tie[i].above ? 4 : -4);
      
      XDrawLine  (display, drawable, drawingGC,
		  i ? x + width : x, tieStart.y, tieStart.x, tieStart.y);

      DrawSpline (display, drawable, drawingGC,
		  &tieControl, tieStart, tieEnd, 1, True);
    }
  }

  bar->item.x = x;
  Return(width);
}

/* }}} */

