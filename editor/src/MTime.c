
/* MTime.c */
/* MTime handling functions */

/* {{{ Includes */

#include "Notes.h"

/* }}} */
/* {{{ Macros */

#define SPACE(x,y) { if (!(x)) (x) = (y *)XtMalloc(sizeof(y)); }
#define MakeMTime(a,b) (((long)b)*(1L<<(a)))

/* }}} */

/* {{{ Constructor */

MTime *NewMTime(MTime *p, NoteTag base, unsigned long number)
{
  Begin("NewMTime");

  SPACE(p, MTime);

  *p = MakeMTime(base, number);

  Return(p);
}

/* }}} */
/* {{{ Converters */

MTime TagToMTime(NoteTag tag, Boolean dotted)
{
  MTime result;
  Begin("TagToMTime");

  if (tag <= ShortestNote) dotted = False;   /* can't dot shortest note */

  if (dotted) { result = MakeMTime(tag-1, 3); }
  else        { result = MakeMTime(tag,   1); }

  Return(result);
}


unsigned long TagToNumber(NoteTag tag, Boolean dotted)
{
  Begin("TagToNumber");
  Return((1L<<tag) + ((dotted && (tag>0)) ? (1L<<(tag-1)) : 0));
}


NoteTag MTimeToTag(MTime a, Boolean *dottedReturn)
{
  NoteTag tag;

  Begin("MTimeToTag");

  a += NumberToMTime(MTimeToNumber(a)/8);

  tag = LongestNote;
  *dottedReturn = True;
  
  while (MTimeGreater(TagToMTime(tag, *dottedReturn), a)) {

    if (!*dottedReturn) {

      if (tag == ShortestNote) Return(tag);
      if (tag >= ShortestNote + 1) *dottedReturn = True;

      --tag;

    } else {

      *dottedReturn = False;
    }
  }

  Return(tag);
}


NoteTag NumberToTag(unsigned long number, Boolean *dottedReturn)
{
  MTime time;
  Begin("NumberToTag");

  time = NumberToMTime(number);
  Return(MTimeToTag(time, dottedReturn));
}

/* }}} */
/* {{{ Comparator */

int CompareMTime(MTime a, MTime b)
{
  Begin("CompareMTime");

  if      (a == b) Return( 0);
  else if (a >  b) Return( 1);
  else             Return(-1);
}

/* }}} */

