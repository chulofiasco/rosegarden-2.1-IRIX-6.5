
/* Musical Notation Editor for X, Chris Cannam 1994 */

/* File IO function header */


#ifndef _MUSIC_IO_
#define _MUSIC_IO_

#include <stdio.h>
#include "Stave.h"

extern MajorStave LoadStaveFromFile(FILE *);
extern void WriteMarks(ItemList, FILE *, int);
extern void WriteFixedBars(ItemList, FILE *, int);

extern unsigned int markIOIndex;


#endif /* _MUSIC_IO_ */

