
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Header file for bar, group, itemlist formatting  */

#ifndef _MUSIC_FORMAT_
#define _MUSIC_FORMAT_


#include "Classes.h"


extern ItemList  FormatBar(Bar *, Boolean, Boolean *, ItemList,
			   /*TimeSignature *, TimeSignature **, */ Clef *,
			   Clef **, Key *, Key **, MTime *);

                      /* tied_backward, tied_forward_return, items, timesig,
		         timesig_return, clef, clef_return, key, key_return,
			 startTime (& returns startTime plus bar length) */

extern void ResetStaffKey(Key *);

extern void UnformatItemList(ItemList *, ItemList *); /* first, last (rtns) */
extern void PrintItemList(ItemList);



#endif /* _MUSIC_FORMAT_ */

