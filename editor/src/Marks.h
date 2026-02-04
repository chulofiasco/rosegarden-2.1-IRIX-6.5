
#ifndef _MUSIC_MARKS_
#define _MUSIC_MARKS_

#include "Notes.h"
#include "Classes.h"


/* stuff we don't want to be *quite* as widespread as Notes.h is */

extern void     CollateMarksInBar(Bar *, MarkList *);
extern void     DrawMarks(Drawable, Position, Dimension, int, MarkList *);
extern MarkList FindMarkType(MarkTag, MarkList);
extern MarkList FindPairMark(Mark *, MarkList);
extern void     MarkItems(MarkTag, ItemList, ItemList);
extern void     ClearMarks(ItemList, ItemList, ItemList);
extern void     ClearMarkType(ItemList, ItemList, ItemList, MarkTag);
extern void     EnsureMarkIntegrity(ItemList);


#endif /* _MUSIC_MARKS_ */

