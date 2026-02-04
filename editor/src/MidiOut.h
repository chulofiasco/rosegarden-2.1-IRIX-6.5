
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Midi Output functions header                     */


#ifndef _MUSIC_MIDIOUT_
#define _MUSIC_MIDIOUT_

#include "Classes.h"
#include "Stave.h"


extern Result MidiWriteStave         (MajorStave, String, String);
extern void   MidiWriteItemList      (ItemList, void *);

/* extern void   MidiWriteNothing       (MusicObject, List, long, ClefTag); */
/* extern void   MidiWriteText          (MusicObject, List, long, ClefTag); */
/* extern void   MidiWriteChord         (MusicObject, List, long, ClefTag); */


#endif /* _MUSIC_MIDIOUT_ */

