
#ifndef _TOPBOX_GENERAL_
#define _TOPBOX_GENERAL_

#include <SysDeps.h>


/* Non-X libraries */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


/* Vital X libraries */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>


#include <Debug.h>
#include <ServiceNames.h>


/* Global defines, and functions in Main */

extern Display   *display;
extern Widget     topLevel;

extern Pixmap     greyMap;
extern Pixmap     lightGreyMap;
extern Pixmap     veryLightGreyMap;
extern Pixmap     roseMap;
extern Pixmap     roseMaskMap;

extern XFontStruct *LoadQueryFont(Widget, char *);
extern void Error(String);
extern void QuitNicely(void);
extern void QuitNicelyCallback(Widget, XtPointer, XtPointer);
extern void SetSigHupForClient(void);


/* Other defines */

#define ProgramName "Rosegarden"
#define DefaultFont "fixed"


/* Resource type */

typedef struct {
  String  editorName;
  String  sequencerName;
  String  helpFile;
  Boolean foundDefaults;
  String  helpTextFont;
  String  helpXrefFont;
  String  helpVerbatimFont;
  String  helpTitleFont;
  String  aboutTextFont;
  Boolean shouldWarpPointer;
} AppData, *AppDataPtr;

extern AppData appData;


typedef int Result;

#define Failed    0
#define Succeeded 1


/* Blit optimization macro */

extern Boolean oneD;

#define CopyArea(s,d,x,y,w,h,X,Y) { if (oneD) { \
    XCopyPlane (display,(s),(d),copyGC,(x),(y),(w),(h),(X),(Y),1); \
  } else { \
    XCopyArea  (display,(s),(d),copyGC,(x),(y),(w),(h),(X),(Y)); } }

#endif /* _TOPBOX_GENERAL_ */

