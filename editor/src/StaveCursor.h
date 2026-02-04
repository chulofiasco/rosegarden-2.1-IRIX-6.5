
#ifndef _MUSIC_STAVECURSOR_
#define _MUSIC_STAVECURSOR_

#include <X11/Intrinsic.h>
#include "Lists.h"
#include "Classes.h"
#include "Stave.h"


typedef struct _PointerRec {
  ItemList  left;		/* list giving item to left of pointer  */
  Bar     * bar;
  MTime     time;
} PointerRec;

#define TestNullRec(x) (!((x).left || (x).bar))
#define MakeNullRec(x) do { (x).left = NULL; (x).bar = NULL; } while(0)


/* the Lists here are actually StaveEltLists: */

extern int        StaveGetPointedStave (MajorStave, XPoint);
extern List       StaveGetPointedBar   (MajorStave, XPoint, int);
extern PointerRec StaveGetPointedItem  (MajorStave, XPoint, int, List);
extern Pitch      StaveGetPointedPitch (MajorStave, XPoint, int, ClefTag);

extern void       StaveCursorDrawX          (XPoint, XPoint);
extern void       StaveCursorExpose         (MajorStave);
extern void       StaveCursorRemove         (MajorStave);
extern void       StaveCursorMark           (MajorStave, XPoint);
extern void       StaveCursorExtend         (MajorStave, XPoint);
extern void       StaveCursorExplicitExtend (MajorStave, XPoint);
extern void       StaveCursorFinish         (MajorStave);
extern void       StaveCursorCleanUp        (MajorStave);


extern void       StaveCursorSelectSublist  (MajorStave, int,
					     ItemList, ItemList);
extern void       StaveCursorSelectBar      (MajorStave, XPoint, Boolean);
extern void       StaveCursorSelectStaff    (MajorStave, XPoint, Boolean);


#endif /* _MUSIC_STAVECURSOR_ */

