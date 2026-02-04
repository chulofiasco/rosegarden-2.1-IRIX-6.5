
#ifndef _MUSIC_PALETTE_
#define _MUSIC_PALETTE_

#include "General.h"
#include "Visuals.h"


extern Boolean PaletteModDottedQuery(void);
extern NoteMods PaletteGetNoteMods(void);
extern void PaletteChangeMode(Boolean);
extern Boolean PaletteFollowKey(void);

extern void InstallPalettes(Widget);
extern void InstallPaletteMods(Widget);
extern void InstallPaletteFollowToggle(Widget);

extern void PalettePushDot(Boolean);
extern void PalettePushSharp(Boolean);
extern void PalettePushFlat(Boolean);
extern void PalettePushNatural(Boolean);

extern void PalettePopDot(void);
extern void PalettePopSharp(void);
extern void PalettePopFlat(void);
extern void PalettePopNatural(void);

extern void PaletteMoveUp(void);
extern void PaletteMoveDown(void);

extern void PaletteSelectNote(NoteTag);

#endif /* _MUSIC_PALETTE_ */

