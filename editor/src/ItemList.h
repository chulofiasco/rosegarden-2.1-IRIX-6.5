
/* Musical Notation Editor for X, Chris Cannam 1994 */

#ifndef _MUSIC_ITEMLISTS_
#define _MUSIC_ITEMLISTS_

/* ItemList type itself is declared in Classes.h, because it's used by
   some other classes; this header declares some useful operations */

#include "Lists.h"
#include "Classes.h"

#define iNext(i) ((ItemList)Next(i))
#define iPrev(i) ((ItemList)Prev(i))

extern ItemList ItemListRemoveItems      (ItemList, ItemList);  /* start, end */
extern ItemList ItemListDuplicate        (ItemList); /* also copies Tie marks */
extern ItemList ItemListDuplicateSublist (ItemList, ItemList);    /* likewise */

extern void ItemListTranspose(ItemList, ItemList, int); /* first, last, semis */

extern ItemList ItemListEnGroup          (GroupTag, ItemList, ItemList);
extern ItemList ItemListEnTuplet         (ItemList, ItemList,
					  short, short, short);
extern ItemList ItemListUnGroup          (ItemList);
extern ItemList ItemListEnsureIntegrity  (ItemList);
extern ItemList ItemListAutoBeam         (TimeSignature *, ItemList, ItemList);


/* Define some iterators for use in "for" loops -- use as */
/*      for (ItemList_ITERATE(i, items))       { ... }    */
/* or   for (ItemList_ITERATE_GROUP(i, group)) { ... }    */

/* NOTE: THESE ITERATORS ARE DEPRECATED IN MOST CASES,
   BECAUSE GROUPS ARE NO LONGER FIRST-CLASS ITEMS */

/* x is iteration variable, y is list start: */

#define ItemList_ITERATE(x,y) \
             x = (y); (x); \
             x = (x)->item->object_class == GroupClass ? \
                 iNext(((Group *)(x)->item)->group.end) : \
                 iNext((x))

#define ItemList_ITERATE_SUBLIST(x,y,z) \
	     x = (y); (x); \
             x = ((x) == (z)) ? NULL : iNext((x))

/* x is iteration variable, y is group pointer; this */
/* works even where y is actually a bar object       */

#define ItemList_ITERATE_GROUP(x,y) \
             x = (y)->group.start; (x); \
             x = (x) == (y)->group.end ? NULL : \
                 (x)->item->object_class == GroupClass ? \
                 iNext(((Group *)(x)->item)->group.end) : \
                 iNext((x))

#define TIED_BACKWARD(i) \
             (((i)->object_class == ChordClass  || \
               (i)->object_class ==  RestClass  || \
               (i)->object_class == GroupClass) && \
              ((Phrase *)(i))->phrase.tied_backward)

#define TIED_FORWARD(i) \
             (((i)->object_class == ChordClass  || \
               (i)->object_class ==  RestClass  || \
               (i)->object_class == GroupClass) && \
              ((Phrase *)(i))->phrase.tied_forward)

#endif /* _MUSIC_ITEMLISTS_ */

