
/* Musical Notation Editor for X, Chris Cannam 1994-96 */
/* Drawing methods for entire stave to OpusTeX stream */

/* Fixes & suggestions by Scott Snyder, snyder@d0sgif.fnal.gov  -- thanks! */

/*#define DEBUG 1*/


#include <stdio.h>
#include "Format.h"
#include "StaveEdit.h"
#include "StavePrivate.h"
#include "Draw.h"
#include <Yawn.h>

#include <SysDeps.h>
#include <Debug.h>

/* This file exports its declarations in Stave.h */

/* Note a maximum of six staffs is hardcoded in some places, and a
   maximum of nine in others.  Could probably get nine just by editing
   the six limit, but I wouldn't swear to it */


static char *instrumentNumerals[] = {
  "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix",
};


typedef struct _StavePointerRec {
  ItemList current;
  MTime    time;
  Group   *group;
} StavePointerRec, *StavePointerSet;


void StaveOTStartColumn(FILE *file, int i, MTime minLength)
{
  int j;

  Begin("StaveOTStartColumn");

  fprintf(file, "\\%s",
	  MTimeToNumber(minLength) > 48 ? "NOTES" :
	  MTimeToNumber(minLength) > 32 ? "NOTEsp" :
	  MTimeToNumber(minLength) > 24 ? "NOTEs" :
	  MTimeToNumber(minLength) > 16 ? "NOTesp" :
	  MTimeToNumber(minLength) > 12 ? "NOTes" :
	  MTimeToNumber(minLength) >  8 ? "NOtesp" :
	  MTimeToNumber(minLength) >  6 ? "NOtes" :
	  MTimeToNumber(minLength) >  4 ? "Notesp" :
	  MTimeToNumber(minLength) >  3 ? "Notes" :
	  MTimeToNumber(minLength) >  2 ? "notesp" : "notes");

  for (j = 0; j < i; ++j) {
    fprintf(file, "&%%\n");
  }

  End;
}


void StaveOTEndColumn(FILE *file)
{
  Begin("StaveOTEndColumn");

  fprintf(file, "\\en\n");

  End;
}


static MTime PervertTupletTime(Group *group, MTime time)
{
  long length;
  double ratio;
  Begin("PervertTupletTime");

  if (group && group->group.type == GroupTupled) {

    group->group.type = GroupNoDecoration;
    length = MTimeToNumber(group->methods->get_length(group));
    group->group.type = GroupTupled;

    ratio = ((double)MTimeToNumber(group->methods->get_length(group))) /
      (double)length;

    length = (long)(ratio * (double)MTimeToNumber(time));

  } else length = MTimeToNumber(time);

  Return(NumberToMTime(length));
}


Group *MakeCacheGroupForOT(ItemList il)
{
  ItemList i, j;
  Begin("MakeCacheGroupForOT");

  for (i = il; !i->item->item.grouping.beamed.start; i = iPrev(i));
  for (j = il; !j->item->item.grouping.beamed.end;   j = iNext(j));

  Return(NewFakeGroup(i, j));
}


MTime StaveWriteColumnToOTFile(FILE *file, MajorStave sp, StavePointerSet set,
			       ClefTag *clefs, Key **keys,
			       TimeSignature **timesigs, int n,
			       int shortestInBar)
{
  int i;
  MTime length, minLength = longestNoteTime;
  MTime thisStart  = longestMTime;
  MTime startTime  = longestMTime;
  MTime minEndTime = longestMTime;
  Boolean down;
  Boolean started;
  Boolean nonZero;
  Boolean foundSig, foundClef;

  Begin("StaveWriteColumnToOTFile");

  for (i = 0; i < n; ++i) {
    if (set[i].current) {
      thisStart = set[i].time;
      if (MTimeLesser(thisStart, startTime)) startTime = thisStart;
    }
  }

  /* return longestMTime as a failed/empty case */

  if (thisStart == longestMTime) Return(longestMTime);

  /* find shortest of candidate notes, skipping zero-length objects
     unless there's really nothing else */

  nonZero = False;

  for (i = 0; i < n; ++i) {
    if (set[i].current && set[i].time == startTime) {
      ItemList l = set[i].current;
      while (l && (l->item->object_class == GroupClass ||
 	     l->item->methods->get_length(l->item) == 0)) l = iNext(l);
      if (l) {
 	length = l->item->methods->get_length(l->item);
 	if (length != zeroTime && MTimeLesser(length, minLength)) {
	  minLength = length;
	  nonZero = True;
	}
      }
    }
  }

  if (!nonZero) minLength = zeroTime;
  started = False;

  /* check for key sig and clef changes before writing any other items */

  foundSig = foundClef = False;

	/*	if (!(IS_CLEF(set[i].current->item) &&
	      ((Clef *)set[i].current->item)->clef.clef == clefs[i]) &&
	    !(IS_KEY(set[i].current->item) &&
	      ((Key *)set[i].current->item)->key.key == keys[i]->key.key)) {
	      */
    
  for (i = n - 1; i >= 0; --i) {

    ClassTag ctag;

    while (set[i].current && set[i].time == startTime &&
	   ((ctag = set[i].current->item->object_class) == KeyClass ||
	    ctag == ClefClass)) {

      if (ctag == KeyClass) {
	if (((Key *)set[i].current->item)->key.key == keys[i]->key.key) {
	  set[i].current = iNext(set[i].current);
	  continue;
	} else foundSig = True;
      } else {
	if (((Clef *)set[i].current->item)->clef.clef == clefs[i]) {
	  set[i].current = iNext(set[i].current);
	  continue;
	} else foundClef = True;
      }

      set[i].current->item->methods->draw_opusTeX
	(set[i].current->item, file, 0, n - i - 1, down, clefs[i], False);

      set[i].current = iNext(set[i].current);
    }
  }

  if (foundSig) fprintf(file, "\\changesignature\n");
  if (foundClef) fprintf(file, "\\changeclefs\n");

  /* write only those items that actually start dead on startTime --
     note reversed order of counting, because OpusTeX counts staffs
     from the bottom up and I count them from the top down */

  for (i = n - 1; i >= 0; --i) {

    Boolean done_staff = False;

    while (!done_staff && set[i].current && set[i].time == startTime) {

      done_staff = True;
      down = False;

      /*      if (IS_GROUP(set[i].current->item)) {
	
	set[i].group = (Group *)set[i].current->item;
	set[i].current = set[i].group->group.start;
	done_staff = False;
	continue;
      }
      */

      if (IS_CHORD(set[i].current->item)) {
	
	/* Tail up if lowest is further below the centre line than
           highest is above it */
	
	if ((4 -
	     set[i].current->item->methods->get_lowest
	     (set[i].current->item)->pitch) <
	    (set[i].current->item->methods->get_highest
	     (set[i].current->item)->pitch - 4)) {
	  down = True;
	}
      }

      /* This is just so as to make code written for the old internal
         structures work with the new ones: we create & destroy a
         cache group and recompute its contents for every single
         grouped item.  Obviously this makes the cache a bit
         pointless, and as a result this code is incredibly slow.  All
         this OpusTeX stuff really needs to be rewritten by someone
         with lots of time, a good head for sensible data-structures
         and an extremely good knowledge of OpusTeX.  Any takers? */

      if (GROUPING_TYPE(set[i].current->item) != GroupNone) {
	set[i].group = MakeCacheGroupForOT(set[i].current);
      }

      if (set[i].group != NULL) {

	if (!started) {
	  StaveOTStartColumn(file, n - i - 1, minLength); started = True;
	}

	DrawOTGroupItem
	  (set[i].group, file,
	   (minLength > 32 ? 4 : minLength > 16 ? 2 : minLength > 4 ? 1 : 0),
	   n - i - 1, clefs[i], set[i].current);

      } else {

	/* if clef or key, ignore if it's the same one as we had
           anyway (we do this to avoid writing the very first example
           of each twice) */

	/*	if (!(IS_CLEF(set[i].current->item) &&
	      ((Clef *)set[i].current->item)->clef.clef == clefs[i]) &&
	    !(IS_KEY(set[i].current->item) &&
	      ((Key *)set[i].current->item)->key.key == keys[i]->key.key)) {
	      */
	  if (!started) {
	    StaveOTStartColumn(file, n - i - 1, minLength); started = True;
	  }

	  set[i].current->item->methods->draw_opusTeX
	    (set[i].current->item, file,
	     (minLength > 32 ? 4 : minLength > 16 ? 2 : minLength > 4 ? 1 : 0),
	     n - i - 1, down, clefs[i], False);
	  /*	}*/
      }      

      if (set[i].current->item->object_class == ClefClass) {

	clefs[i] = ((Clef *)set[i].current->item)->clef.clef;

      } else if (set[i].current->item->methods->get_length
		 (set[i].current->item) == zeroTime) {
	done_staff = False;
      }
      /*
      if (set[i].group && set[i].current == set[i].group->group.end) {
	set[i].group = NULL;
      }
      */

      if (IS_CHORD(set[i].current->item) && set[i].group &&
	  set[i].group->group.type == GroupTupled) {

	set[i].time = AddMTime
	  (set[i].time,
	   PervertTupletTime
	   (set[i].group,
	    set[i].current->item->methods->get_length(set[i].current->item)));

      } else {
      
	set[i].time = AddMTime
	  (set[i].time,
	   set[i].current->item->methods->get_length(set[i].current->item));
      }

      if (set[i].group) {
	XtFree((void *)set[i].group); /* ugh */
	set[i].group = NULL;
      }

      if (MTimeLesser(set[i].time, minEndTime)) minEndTime = set[i].time;      
      set[i].current = iNext(set[i].current);
    }
  
/*    if (i > 0 && started) fprintf(file, "\\relax\n&"); */
    if (i > 0 && started) fprintf(file, "%\n&");
  }

  if (started) StaveOTEndColumn(file);

  if (MTimeEqual(minEndTime, zeroTime)) Return(NumberToMTime(1));
  else Return(minEndTime);
}


void StaveOTGetClefsTimesAndKeys(MajorStave sp, int i, ClefTag *clefs,
				 Key **keys, TimeSignature **timesigs)
{
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  ItemList list;
  Item *item;

  Begin("StaveOTGetClefsAndTimes");

  keys[i] = 0;
  clefs[i] = InvalidClef;
  timesigs[i] = NULL;

  if (BarValid(mstave->bar_list->bars[i])) { /* or just bar existence? */

    timesigs[i] = &mstave->bar_list->bars[i]->bar.time;

    /* naive iterator, don't care about groups */
    for (list = mstave->bar_list->bars[i]->group.start;
	 list && ((clefs[i] == InvalidClef) || (keys[i] == 0));
	 list = iNext(list)) {

      item = list->item;

      if (keys[i] == 0) {
	if (IS_KEY(item)) {
	  keys[i] = (Key *)item;
	} else if (IS_CHORD(item) || IS_REST(item) || IS_GROUP(item)) {
	  keys[i] = &defaultKey;
	}
      }

      if (clefs[i] == InvalidClef) {
	if (IS_CLEF(item)) {
	  clefs[i] = ((Clef *)item)->clef.clef;
	} else if (IS_CHORD(item) || IS_REST(item) || IS_GROUP(item)) {
	  clefs[i] = TrebleClef;
	}
      }
    }
  }

  if (keys[i] == 0) keys[i] = &defaultKey;
  if (clefs[i] == InvalidClef) clefs[i] = TrebleClef;

  End;
}


extern void DrawOTTimeSignature(TimeSignature *, FILE *, int);

Result StaveWriteOpusTeXToFile(MajorStave sp, Widget w) /*parent for filebox*/
{
  int i;
  int bar;
  FILE *file;
  String message;
  String name;

  Key **keys;
  ClefTag *clefs;
  TimeSignature **timesigs;
  StavePointerSet set;
  Boolean finished = False;
  MTime *barLengths;

  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  int staves = mstave->staves;
  MTime duration;
  StaveEltList barList;

  Item *shortestInBar, *shTemp;
  MTime shortestLength;

  Begin("StaveWriteOpusTeXToFile");

  if (staves > 6 /* 9 */) {

    message = (String)XtMalloc(256);
    sprintf(message,
	    "This piece has %d staffs; I can only handle six.", staves);

    if (YQuery(topLevel, message, 2, 0, 1, "Write first 6 only", "Cancel",
	       "Editor File - Export OpusTeX") != 0) {
      XtFree(message);
      Return(Failed);
    } else {
      XtFree(message);
      staves = 6;
    }
  }

  if ((file = YFileGetWriteFile(w, "Editor File - Export OpusTeX",
				".tex", "TeX")) == NULL) {
    fclose(file);
    Return(Failed);
  }

  StaveBusyStartCount(mstave->bars + 1);
  StaveBusyMakeCount(0);

  for (i = 0; i < staves; ++i) {
    UnformatItemList((ItemList *)&mstave->music[i], NULL);
    StaveResetFormatting(sp, i);
    StaveFormatBars(sp, i, -1);
  }

  keys = (Key **)XtMalloc(staves * sizeof(Key *));
  clefs = (ClefTag *)XtMalloc(staves * sizeof(ClefTag));
  timesigs = (TimeSignature **)XtMalloc(staves * sizeof(TimeSignature *));
  set = (StavePointerSet)XtMalloc(staves * sizeof(StavePointerRec));
  barLengths = (MTime *)XtMalloc (staves * sizeof (MTime));

  barList = mstave->bar_list;
  bar = 1;

  for (i = 0; i < staves; ++i) {
    StaveOTGetClefsTimesAndKeys(sp, i, clefs, keys, timesigs);
  }

  fprintf(file,
	  "%% Generated by Rosegarden\n"
	  "%% OpusTeX generation code: Chris Cannam, 1995; Erwin Achermann, 1997\n"
	  "%% Bugs, fixes and other comments to "
	  "cannam@netcomuk.co.uk\n%%\n"
	  "\\input opustex\n%%\n"
	  "\\instrumentnumber%d\\relax\n",
	  staves);

  for (i = 0; i < staves; ++i) {
    fprintf(file,
	    "%%\n\\setname%d{%s}%%\n"
	    "\\setstaffs%d{1}\n",
	    i+1,
	    mstave->names[staves-i-1], i+1);

    /* bit dubious, putting all this output-implementation here! */

    keys[staves-i-1]->methods->draw_opusTeX
      (keys[staves-i-1], file, 0, i, False, clefs[staves-i-1], False);

    fprintf(file,
	    "\\setclef%d%s%%\n"
	    "\\setmeter%d{{\\meterfrac{%d}{%d}}}%%\n",
	    i+1,
	    clefs[staves-i-1] == TrebleClef ? "\\treble" :            
	    clefs[staves-i-1] ==  TenorClef ? "{4}"  :                
	    clefs[staves-i-1] ==   AltoClef ? "\\alto"   : "\\bass" , 
	    i+1,
	    timesigs[staves-i-1]->numerator,
	    timesigs[staves-i-1]->denominator);
  }

  fprintf(file, "\\startpiece\\systemnumbers\n%%");

  while (barList && !finished) {
    Boolean timesig_changed = False;

    duration = zeroTime;
    finished = True;
    shortestInBar = (Item *)&longestChord;
    shortestLength = shortestInBar->methods->get_length(shortestInBar);

    for (i = 0; i < staves; ++i) {

      if (!BarValid(barList->bars[i])) { /* ??? */
	/*
	  fprintf(stderr,"bar invalid on staff %d\n",i);
*/
	set[i].current = NULL;
	timesigs[i] = NULL;
	barLengths[i] = zeroTime;

      } else {

	set[i].group = NULL;
	set[i].time = zeroTime;
	set[i].current = barList->bars[i]->group.start;
	barLengths[i] = barList->bars[i]->methods->get_length
	                 (barList->bars[i]);
	finished = False;

	shTemp =
	  (Item *)barList->bars[i]->methods->get_shortest(barList->bars[i]);
	if (MTimeLesser(shTemp->methods->get_length(shTemp), shortestLength)) {
	  shortestInBar  = shTemp;
	  shortestLength = shTemp->methods->get_length(shTemp);
	}

	if (!TimeSignaturesEqual(*timesigs[i], barList->bars[i]->bar.time)) {
	  DrawOTTimeSignature(&barList->bars[i]->bar.time, file, staves-i-1);
	  timesigs[i] = &barList->bars[i]->bar.time;
	  timesig_changed = True;
	}
      }
    }

    if (timesig_changed) {
      fprintf(file, "%% not guaranteed to be the right moment for this:\n");
      fprintf(file, "\\changecontext\n");
    } else {
      if (bar > 1 && bar < mstave->bars) fprintf(file, "\\bar\n");
    }

    fprintf(file, "%%\n%% bar %d\n%%\n", bar);

    do {

      duration = StaveWriteColumnToOTFile
	(file, sp, set, clefs, keys, timesigs, staves,
	 MTimeToNumber(shortestLength));

      for (i = 0; i < staves; ++i) {
	if (set[i].time >= barLengths[i]) set[i].current = NULL;
      }
      
      /* longestMTime is returned when all set[i].current are NULL: */

    } while MTimeLesser(duration, longestMTime);

    StaveBusyMakeCount(bar + 1);
    barList = (StaveEltList)Next(barList);
    bar ++;
  }

  fprintf(file, "%%\n\\Endpiece\\eject\\bye\n");

  staveMoved = True;
  StaveRefresh(mstave, -1);
  StaveBusyFinishCount();

  fclose(file);
  name = YFileGetLastFilename(False);
  message = (String)XtMalloc(strlen(name) + 12);
  sprintf(message, "Wrote `%s'.", name);
  (void)YQuery(topLevel, message, 1, 0, 0, "OK", NULL);
  XtFree(message);

  XtFree(keys);
  XtFree(clefs);
  XtFree(timesigs);
  XtFree(set);
  XtFree(barLengths);

  Return(Succeeded);
}


