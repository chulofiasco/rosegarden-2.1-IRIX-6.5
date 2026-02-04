
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Midi Output functions header                     */


#ifndef _MUSIC_MIDIIN_
#define _MUSIC_MIDIIN_

#include "Stave.h"


extern MajorStave MidiReadStave(String, String *, Boolean, int, Boolean);
extern Result     MidiChooseQuantizeLevel(String, int *, int *);



#endif /* _MUSIC_MIDIIN_ */

