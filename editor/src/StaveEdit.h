
#ifndef _MUSIC_STAVEEDIT_
#define _MUSIC_STAVEEDIT_

#include "Classes.h"
#include "Stave.h"

extern void StaveEditEnterEditMode(void);
extern void StaveEditEnterInsertMode(void);
extern void StaveEditAssertInsertVisual(PaletteTag, MusicObject, int);
extern void StaveEditInitialise(Widget);
extern void StaveEditCleanUp(void);

extern void StaveBusy(Boolean);
extern void StaveBusyStartCount(int);
extern void StaveBusyMakeCount(int);
extern void StaveBusyFinishCount(void);

#endif /* _MUSIC_STAVEEDIT_ */

