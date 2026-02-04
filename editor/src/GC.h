
#ifndef _MUSIC_GC_
#define _MUSIC_GC_

extern void CreateGCs(void);

extern GC     drawingGC;
extern GC      dashedGC;
extern GC         xorGC;
extern GC        beamGC;
extern GC    greyFillGC;
extern GC   lightFillGC;
extern GC       clearGC;
extern GC        copyGC;
extern GC     timeSigGC;
extern GC     bigTextGC;
extern GC  littleTextGC;
extern GC    tinyTextGC;
extern GC  italicTextGC;
extern GC dynamicTextGC;
extern GC   chordNameGC;

extern XFontStruct *chordFont;

extern void GCCleanUp(void);

#endif /* _MUSIC_GC_ */

