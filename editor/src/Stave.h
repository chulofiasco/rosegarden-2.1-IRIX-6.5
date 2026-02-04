
#ifndef _MUSIC_STAVE_
#define _MUSIC_STAVE_

#include <stdio.h>
#include "Classes.h"

typedef void *MajorStave;
extern Boolean staveChanged;
extern Boolean staveMoved;          /* cursor-drawing sets False,
				       scroll, load &c. True */

extern void       StaveInitialise          (Widget);
extern void       StaveInitialiseScrollbar (Widget);
extern void       StaveRefresh             (MajorStave, int);
extern void       StaveRefreshAsDisplayed  (MajorStave);
extern void       StaveResetFormatting     (MajorStave, int);
extern void       StaveReformatEverything  (MajorStave);
extern void       StaveUnmap               (MajorStave);
extern void       StaveScrollbarCallback   (Widget, XtPointer, XtPointer);
extern void       StaveLeftCallback        (Widget, XtPointer, XtPointer);
extern void       StaveRightCallback       (Widget, XtPointer, XtPointer);
extern void       StaveJumpCallback        (Widget, XtPointer, XtPointer);
extern void       StavePageCallback        (Widget, XtPointer, XtPointer);
extern void       StaveLeapToTime          (MajorStave, MTime, Boolean);
extern void       StaveScrollUpOrDownABit  (Boolean);
extern MajorStave NewStave                 (int, ItemList *);
extern void       StaveDestroy             (MajorStave, Boolean);
extern void       StaveSetEndBarTags       (MajorStave, int, BarTag, BarTag);
extern void       StaveSetConnection       (MajorStave, int, Boolean);
extern void       StaveSetMIDIPatch        (MajorStave, int, int);
extern void       StaveRenameStave         (MajorStave, int, String);
extern void       StaveAddAbsoluteMark     (MajorStave, int, int,
					    unsigned long, unsigned long);
extern void       StaveAddBarTime          (MajorStave, int, unsigned long,
					    int, int);
extern void       StaveWriteToFile         (MajorStave, FILE *);
extern void       StaveCleanUp             (void);


extern void       StaveSetScrollbarMarks   (Boolean, float, float);
extern void       StaveScrollbarExpose     (Widget, XtPointer,
					    XEvent *, Boolean *);


/* This is actually in MenuStave.c, and it's quite complicated */
extern void       StaveAddANewStave        (MajorStave, int);



/* These take stave, staff number and itemlist and return the time
   sig, key sig and clef in effect at that point.  returns are not
   duplicates, don't free them.  these may each take appreciable time */

extern TimeSignature *StaveItemToTimeSignature(MajorStave, int, ItemList);
extern Key           *StaveItemToKey          (MajorStave, int, ItemList);
extern Clef          *StaveItemToClef         (MajorStave, int, ItemList);
extern Bar           *StaveItemToBar          (MajorStave, int, ItemList);

/* These ones haven't been properly tested yet */
extern MTime          StaveItemToTime         (MajorStave, int, Bar*, ItemList);
extern ItemList       StaveTimeToItem         (MajorStave, int, Bar*, MTime);
extern Bar           *StaveTimeToBar          (MajorStave, int, MTime);


extern Window StaveTrackingTargetWindow(void);

extern void StaveLeapToBar(MajorStave, int);


/* This one is actually in DrawMTStave.c: */
extern Result StaveWriteMusicTeXToFile (MajorStave, Widget);

/* And this one is in DrawOTStave.c: */
extern Result StaveWriteOpusTeXToFile (MajorStave, Widget);

/* And this is in DrawPMX.c: */
extern Result StaveWritePMXToFile (MajorStave, Widget);

/* sets time sigs in all bars from that containing ItemList to end of
   music on staff; returns first bar time sig was set in */
extern Bar *StaveSetTimeSignatures(MajorStave, int, ItemList, TimeSignature *);

/* shouldn't really be here: */
extern void StaveFormatBars(MajorStave, int, int);


extern MajorStave stave;


#endif /* _MUSIC_STAVE_ */

