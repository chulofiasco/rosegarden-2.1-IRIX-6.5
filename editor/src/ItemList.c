
/* ItemList.c */

/* Musical Notation Editor for X, Chris Cannam 1994       */
/* Functions for dealing with manipulations of ItemLists. */
/* This is not as easy as just dealing with Lists; the    */
/* presence of Groups within the ItemList, with their     */
/* accompanying pointer inconsistency, makes things much  */
/* more complicated.                                      */

/* {{{ Includes */

#include "Classes.h"
#include "Lists.h"
#include "ItemList.h"
#include "Marks.h"

/* }}} */

/* {{{ General ItemList manipulations */

ItemList ItemListRemoveItems(ItemList start, ItemList end)
{
  ItemList first;
  Begin("ItemListRemoveItems");

  if (!start && !end) {
    fprintf(stderr, "Warning: ItemListRemoveItems: start and end both NULL\n");
    Return(NULL);
  }

  if (start) {
    first = (ItemList)First(start);
    start = iNext(start);
  } else {
    first = (ItemList)First(end);
    start = first;
  }

  while (start) {

    if (start == first) first = iNext(start);
    start->item->methods->destroy(start->item);

    if (start == end) {
      Remove(start);
      break;
    } else {
      start = (ItemList)Remove(start);
    }
  }

  Return(first);
}


ItemList ItemListDuplicate(ItemList list)
{
  Begin("ItemListDuplicate");
  Return(ItemListDuplicateSublist((ItemList)First(list), (ItemList)Last(list)));
}


/* "first" is first item in sublist, *not* item to left of start */

ItemList ItemListDuplicateSublist(ItemList first, ItemList last)
{
  Item *newItem;
  ItemList list, subList, matchList, returnList;
  MarkList mlist;
  Begin("ItemListDuplicateSublist");

  list = first;
  returnList = NULL;

  while (list) {

    newItem = (Item *)list->item->methods->clone(list->item);
    returnList = (ItemList)Nconc(returnList, NewItemList(newItem));

    if (list != first) {	/* check for marks */

      for (mlist = (MarkList)First(list->item->item.marks); mlist;
	   mlist = (MarkList)Next(mlist)) {

	if (mlist->mark->start) continue;

	/* if we're at the end of a mark, track back along the
           original and copy lists in step until we reach the start of
           the mark in the original list; then mark up this point in
           the copy list */

	for (subList = iPrev(list), matchList = iPrev(Last(returnList));
	     subList && (iNext(subList) != first);
	     subList = iPrev(subList), matchList = iPrev(matchList)) {

	  if (FindPairMark(mlist->mark, subList->item->item.marks)) {
	    MarkItems(mlist->mark->type, matchList, (ItemList)Last(returnList));
	  }
	}
      }
    }

    if (list == last) break;
    list = iNext(list);
  }

  Return(returnList);
}

/* }}} */
/* {{{ EnsureIntegrity -- postprocessor for newly-read lists */

/* ItemListEnsureIntegrity: call this on a newly-read itemlist, in  */
/* a format using Group items to point at separate contained lists. */
/* This function strips out the Groups and rebuilds the ItemList in */
/* the usual format. */

ItemList ItemListEnsureIntegrity(ItemList i)
{
  Group *g;
  ItemList oi, ii;
  ItemList rtn;
  short uLength;

  Begin("ItemListEnsureIntegrity");

  i = rtn = (ItemList)First(i);

  while (i) {

    if (i->item->object_class == GroupClass) {
      
      g = (Group *)i->item;
      oi = i;
      uLength = 0;

      if (Next(i)) {

	i = (ItemList)Insert(((Group *)i->item)->group.start, Next(i));

      } else {

	Nconc(i, ((Group *)i->item)->group.start);
	i = iNext(i);
      }

      for (ItemList_ITERATE_GROUP(ii, g)) {

	ii->item->item.grouping.none.type = g->group.type;
	ii->item->item.grouping.beamed.start = (ii == g->group.start);
	ii->item->item.grouping.beamed.end   = (ii == g->group.end);

	if (g->group.type == GroupTupled) {

	  uLength += ii->item->methods->get_length(ii->item);
	  ii->item->item.grouping.tupled.tupled_length = g->group.tupled_length;
	  ii->item->item.grouping.tupled.tupled_count  = g->group.tupled_count;
	}
      }

      if (g->group.type == GroupTupled) {
	for (ItemList_ITERATE_GROUP(ii, g)) {
	  ii->item->item.grouping.tupled.untupled_length = uLength;
	}
      }

      Remove(oi);

    } else i = iNext(i);
  }

  Return(rtn);
}

/* }}} */
/* {{{ Grouping and ungrouping */

/* Pass grouping type and first and last items in prospective */
/* group's item list.  Returns new (group) item in list.      */

/* Can we ensure that all grouping requests pass through this function? */

ItemList ItemListEnGroup(GroupTag type, ItemList begin, ItemList end)
{
  ItemList list;
  Begin("ItemListEnGroup");

  if (!begin || !end)
    Error("ItemListEnGroup called with invalid argument");

  if (iPrev(begin) && GROUPING_TYPE(iPrev(begin)->item) != GroupNone) {
    iPrev(begin)->item->item.grouping.beamed.end = True;
  }

  if (iNext(end) && GROUPING_TYPE(iNext(end)->item) != GroupNone) {
    iNext(end)->item->item.grouping.beamed.start = True;
  }

  for (ItemList_ITERATE_SUBLIST(list, begin, end)) {

    list->item->item.grouping.none.type = type;

    if (type != GroupNone) {
      /* "beamed" is just a name for any of the union */
      list->item->item.grouping.beamed.start = (list == begin);
      list->item->item.grouping.beamed.end   = (list == end);
    }

    if (type == GroupTupled) {
      fprintf(stderr, "WARNING: Calling EnGroup instead of EnTuplet"
	      " for tupled groups\n");
    }
  }
  
  Return(begin);
}


ItemList ItemListEnTuplet(ItemList begin, ItemList end, short untupledLength,
			  short tupledLength, short tupledCount)
{
  ItemList list;
  Begin("ItemListEnTuplet");

  if (!begin || !end)
    Error("ItemListEnTuplet called with invalid argument");

  for (ItemList_ITERATE_SUBLIST(list, begin, end)) {

    list->item->item.grouping.none.type    = GroupTupled;
    list->item->item.grouping.tupled.start = (list == begin);
    list->item->item.grouping.tupled.end   = (list == end);
    list->item->item.grouping.tupled.untupled_length = untupledLength;
    list->item->item.grouping.tupled.tupled_length   = tupledLength;
    list->item->item.grouping.tupled.tupled_count    = tupledCount;

    fprintf(stderr,"made tupled group, length %d, count %d\n",
	    (int)list->item->item.grouping.tupled.tupled_length,
	    (int)list->item->item.grouping.tupled.tupled_count);
  }

  Return(begin);
}


/* Pass group item in list.  Returns list that was first in */
/* group's itemlist.                                        */

/* !!! Pass first grouped item, returns same !!!  (ie. return is OBSOLETE) */

ItemList ItemListUnGroup(ItemList groupList)
{
  ItemList list = groupList;
  GroupTag type;
  Begin("ItemListUnGroup");

  if (groupList->item->object_class == GroupClass) {
    fprintf(stderr, "WARNING: ItemListUnGroup called with old convention\n");
    Return(NULL);
  }

  type = GROUPING_TYPE(groupList->item);
  if (type == GroupNone) Return(groupList);

  while (list && GROUPING_TYPE(list->item) == type) {

    list->item->item.grouping.none.type = GroupNone;
    if (list->item->item.grouping.beamed.end) list = NULL;
    else list = iNext(list);
  }

  Return(groupList);
}

/* }}} */
/* {{{ Transpose */

void ItemListTranspose(ItemList from, ItemList to, int semitones)
{
  int         mp;		/* MIDI pitch, used as intermediate repn */
  ItemList    i, j;
  short       v;
  Chord      *c;
  Key        *k  = NULL;
  static Key *k0 = NULL;
  NoteVoice   voice;

  Begin("Transpose");

  for (j = from; j; j = iPrev(j)) /* bug fixed cc mid 94 */
    if (j->item->object_class == KeyClass) k = (Key *)(j->item);

  if (!k)
    if (!k0) k = k0 = NewKey(NULL, KeyC);
    else k = k0;

  for (i = from; i; i = i ? iNext(i) : i) {

    if (i->item->object_class == ChordClass)
      for (v = 0, c = (Chord *)(i->item); v < c->chord.voice_count; ++v) {

	mp = VoiceToMidiPitch(&c->chord.voices[v], TrebleClef);
	voice = MidiPitchToVoice(mp + semitones, k->key.visual->sharps);

	if (voice.pitch > highestNoteVoice.pitch)
	    voice.pitch = highestNoteVoice.pitch;

	if (voice.pitch < lowestNoteVoice.pitch)
	    voice.pitch = lowestNoteVoice.pitch;

	c->chord.voices[v] = voice;
      }

    if (i == to) i = NULL;
  }

  End;
}

/* }}} */
/* {{{ Auto-beaming */

/* Auto-Beaming Heuristic:                                   */
/*                                                           */
/* First, we need the number of units to try to beam in.  If */
/* we're in 4/4, we probably beam quavers in twos (and semis */
/* in fours and so on); if 6/8, we beam quavers in threes;   */
/* if 2/4, in twos again; if something weird (5/8? 7/8?) we  */
/* probably just beam in fives or sevens.  Seems we just     */
/* want the lowest prime divisor for the number of quavers,  */
/* and to take semis and so on from there.                   */
/*                                                           */
/* Then we skip through the section, accumulating times.     */
/* On finding that the accumulated time is a multiple of the */
/* length of a beamed group (two quavers in 4/4, three in    */
/* 6/8, &c.), we check to see if there's a note ending       */
/* cleanly at the end of another beamed group's length, and  */
/* if so, and if there are at least two beamable items in    */
/* between here and there, we beam them up.  ("...Scotty")   */
/*                                                           */
/* This returns the item list; if `start' was the first item */
/* and has been engrouped, the return value will be the new  */
/* group item; otherwise it'll just be `start'.              */

ItemList ItemListAutoBeamSub(MTime avg, MTime min, MTime max,
			     ItemList start, ItemList end, MTime barLength)
{
  MTime    accumulator;		/* ah, for the days of *real* CPUs */
  MTime    prospective;
  MTime    temp;
  MTime    tmin;
  MTime    beamLength;
  ClassTag oclass;
  MTime    current;
  int      beamable;
  int      longerThanDemi;
  ItemList i, j, k;
  ItemList rtn;
  
  Begin("ItemListAutoBeamSub");

  rtn = start;

  for (i = start, accumulator = zeroTime;
       i && (iPrev(i) != end); i = iNext(i)) {

    oclass = i->item->object_class;

    if (MTimeToNumber(accumulator) % MTimeToNumber(barLength) == 0)
      accumulator = zeroTime;

    if ((MTimeToNumber(accumulator) % MTimeToNumber(avg) == 0) &&
	(oclass == ChordClass || oclass ==  RestClass) &&
	(i->item->methods->get_length(i->item) < TagToMTime(Crotchet, False))) {

      k              = NULL;
      tmin           = min;
      temp           = zeroTime;
      beamable       = 0;
      longerThanDemi = 0;

      for (j = i; j && (iPrev(j) != end); j = iNext(j)) {

	oclass  = j->item->object_class;
	current = j->item->methods->get_length(j->item);

	if (oclass == ChordClass) {

	  if (((Chord *)j->item)->chord.visual->type < Crotchet) ++beamable;
	  if (((Chord *)j->item)->chord.visual->type >= Semiquaver)
	    ++longerThanDemi;
	}

	temp = AddMTime(temp, current);

	if (MTimeGreater(temp, zeroTime) &&
	    MTimeToNumber(temp) % MTimeToNumber(tmin) == 0) {
	  k = j;
	  beamLength = temp;

	  /* Will this group have crossed a bar line?  If so, wrap */
	  /* around the prospective accumulator from the bar mark  */

	  prospective =
	    NumberToMTime(MTimeToNumber(AddMTime(accumulator, temp)) %
			  MTimeToNumber(barLength));

	  tmin = NumberToMTime(2 * MTimeToNumber(tmin));
	}

	/* Decide to stop scanning for items to join this beamed group. */
	/* We stop if we've got the maximum length of beamed group, if  */
	/* we've got more than 4 semis or quavers, if we're at the end  */
	/* of the piece or of a bar, if there's a rest ahead or if the  */
	/* next chord is longer than the current one.                   */

	/* All this, of course, results is an absolutely magnificent    */
	/* conditional.  Ha!  I spurn your puny temporary variables.    */

	if (!MTimeLesser(temp, max) || longerThanDemi >= 4 || !Next(j) ||
	    (k && !MTimeToNumber(prospective)) ||
	    (iNext(j))->item->object_class == RestClass ||
	    (oclass == ChordClass &&
	     (iNext(j))->item->object_class == ChordClass &&
	     MTimeGreater
	     ((iNext(j))->item->methods->get_length
	      ((iNext(j))->item), current))) {

	  if (k && beamable >= 2) {

	    if (i == start && !Prev(i))
	      rtn = ItemListEnGroup(GroupBeamed, i, k/*, False*/);
	    else
	      (void)ItemListEnGroup(GroupBeamed, i, k/*, False*/);
	  }

	  /* If this group is longer than the check threshold (`avg'), */
	  /* its length must be a multiple of the threshold and hence  */
	  /* we can keep scanning from the end of the group without    */
	  /* losing the modulo properties of the accumulator.  Other-  */
	  /* wise, we continue from where we were.  (The latter action */
	  /* must be safe -- we can't get another group starting half- */
	  /* way through the last one, because we know the last one    */
	  /* is shorter than the group start check threshold.)         */

	  if (k && !MTimeLesser(beamLength, avg)) {

	    i = k;
	    accumulator = prospective;

	  } else {

	    accumulator = AddMTime
	      (accumulator,
	       i->item->methods->get_length(i->item));
	  }

	  break;
	}
      }
    } else {

      accumulator = AddMTime
	(accumulator,
	 i->item->methods->get_length(i->item));
    }
  }

  Return(rtn);
}


/*

  Ha.  Another really deeply satisfying night where I go to bed early,
  relaxed and sober and then can't get to sleep for hours.  And wake up
  during the night, and wake up again too early in the morning.  Why?
  WHY?  I ask you.  (And all you can offer me is platitudes.  I don't know
  why I ever bother asking you.  You're a sorry, inconsequential cur who
  hasn't learnt the value of truth.)

  Anyway, I think I was probably just playing the wrong sort of music last
  night.  So today I want to celebrate "Montreal" by Autechre, because of
  its sleep-disturbing aura, because it sounds like the sort of music
  which would be going around in the gunman's head as he trains a laser
  sight into your bedroom through the narrow gap in your curtains and
  dances the little red dot around nervously on your wall.
  
*/


ItemList ItemListAutoBeam(TimeSignature *time, ItemList start, ItemList end)
{
  int   num;
  MTime avg;
  MTime min;
  MTime barLength;

  Begin("ItemListAutoBeam");

  if (time) {

    barLength = time->bar_length;

    /* Minimal number for grouping should ideally be smallest prime */
    /* divisor of bar's numerator.  We'll relax the "prime" bit,    */
    /* but ensure that it's at least 2.                             */

    /* Later comment: no, this isn't true.  If the denominator is   */
    /* 2 or 4, we should always beam in twos (in 3/4, 6/2 or stng.) */

    /* (Actually this isn't right either.  I think if the numerator */
    /* is prime, perhaps, we should beam up to the length of one    */
    /* beat at a time?  Something like that, anyway.  Leave it for  */
    /* a rainy day sometime.)                                       */

    if (time->denominator == 2 ||
	time->denominator == 4) {

      if (time->numerator % 3) {

	avg = NumberToMTime(TagToNumber(Quaver,     False));
	min = NumberToMTime(TagToNumber(Semiquaver, False));

      } else avg = min = NumberToMTime(TagToNumber(Semiquaver, False));

    } else {

      /* Special hack for 6/8.  This is getting dodgier by the minute */

      if (time->numerator   == 6 &&
	  time->denominator == 8) {

	avg = NumberToMTime(3 * TagToNumber(Quaver, False));
	min = NumberToMTime(MTimeToNumber(avg) / 2);

      } else {

	for (num = 2; time->numerator % num != 0; ++num);
	
	avg = NumberToMTime(num * TagToNumber(Semiquaver, False));
	min = NumberToMTime(MTimeToNumber(avg) / 2);
      }

      /* older code, works okay for most time signatures:

	 avg =
	 ((time->numerator== 4) ? (2 * TagToMTime(Semiquaver, False)) :
	 (time->numerator == 6) ? (3 * TagToMTime(Semiquaver, False)) :
	 (time->numerator * TagToMTime(Semiquaver, False)));

	 min =
	 ((time->numerator== 4) ? TagToMTime(Semiquaver, False) :
	 (time->numerator == 6) ? TagToMTime(Semiquaver,  True) :
	 TagToMTime(Semiquaver, False));
      */
    }
  } else {

    barLength = 4 * TagToMTime(Crotchet, False);

    avg = TagToMTime(Quaver, False);
    min = TagToMTime(Semiquaver, False);
  }

  if (time->denominator > 4) avg /= 2;

  Return(ItemListAutoBeamSub(avg, min,
			     NumberToMTime(4 * MTimeToNumber(avg)),
			     start, end, barLength));
}

/* }}} */

