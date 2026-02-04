
#ifndef _MUSIC_GENERAL_
#define _MUSIC_GENERAL_


#include <stdio.h>
#include <stdlib.h>
#include <SysDeps.h>
#include <Yawn.h>
#include <Debug.h>
#include <ServiceNames.h>


/* Global defines, and functions in Main */

extern Display   *display;
extern Widget     topLevel;
extern Widget     paletteShell;
extern Boolean    slaveMode;

extern Pixmap     greyMap;
extern Pixmap     lightGreyMap;
extern Pixmap     roseMap;
extern Pixmap     roseMaskMap;
extern Pixmap     upMap;
extern Pixmap     downMap;
extern Pixmap     leftMap;
extern Pixmap     rightMap;
extern Dimension  roseHeight;

extern void Error(String);
extern void QuitNicely(void);
extern void QuitNicelyCallback(Widget, XtPointer, XtPointer);
extern void ChangeTitleBar(String, Boolean); /* buffer name, changed */


/* Dimension defines */

#define DotWidth          4
#define NoteWidth         9
#define NoteHeight        8
#define ClefWidth         24
#define NoteModWidth      7
#define NoteModHeight     14
#define ChordModHeight    7
#define TailWidth         5
#define TailHeight        14
#define StaveHeight       37
#define StaveUpperGap     80
#define StaveLowerGap     40


/* Other defines */

#define ProgramName "Rosegarden"
#define ApplicationName "Rosegarden Editor"
#define DefaultFont "fixed"


/* Resource type */

typedef struct {
  String    timeSignatureFont;
  String    bigTextFont;
  String    littleTextFont;
  String    tinyTextFont;
  String    italicTextFont;
  String    dynamicTextFont;
  String    chordNameFont;
  String    aboutTextFont;
  String    interlockWindow;
  String    filtersDirectory;
  String    musicDirectory;
  String    acceleratorTable;
  int       midiBarEmphasis;
  Boolean   midiDynamics;
  Boolean   shouldWarpPointer;
  Boolean   foundDefaults;
} AppData, *AppDataPtr;

extern AppData appData;


/* Equation and general musical object types */

typedef void *MusicObject;


/* cc 11/95: changed eqn storage from int+double to short+float,
   because we're now cacheing one of these in every Group object;
   accuracy isn't all that important, because it never appeared to be
   all that good anyway */

typedef struct _LineEquationRec {
  short         c;		/* then eqn is "y = mx + c" */
  float         m;
  Boolean       eqn_present;
  Boolean       reverse;	/* point tail the "wrong" way? */
} LineEquationRec, *LineEquation;


/* Success/failure type */

typedef int Result;

#define Failed    0
#define Succeeded 1


#define STAVE_Y_COORD(p) (StaveHeight-1 - ((p)<0 ? -(NoteHeight/2+1) : 0) - \
			  (((p)/2==((p)+1)/2)? \
			   ((p)/2*(NoteHeight+1)+(NoteHeight/2)): \
			   ((p)/2*(NoteHeight+1)+ NoteHeight)))

/* Blit macro */

extern Boolean oneD;

#define CopyArea(s,d,x,y,w,h,X,Y)  if (oneD) \
   XCopyPlane(display,(s),(d),copyGC,(x),(y),(w),(h),(X),(Y),1L); \
   else XCopyArea(display,(s),(d),copyGC,(x),(y),(w),(h),(X),(Y))


#endif /* _MUSIC_GENERAL_ */

