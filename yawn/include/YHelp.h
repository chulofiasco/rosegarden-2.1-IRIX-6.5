
/*
   YHelp.h :  Widget Creation library Help system Header
   ------------------------------------------------------

   Functions as listed below, plus two actions (y-help-wm-quit and
   y-help-contextual).  More documentation in the top of YHelp.c.

   Chris Cannam, 1993-94
*/


#ifndef _Y_HELP_STUFF_
#define _Y_HELP_STUFF_

#include "Yawn.h"

extern int   YHelpSetTextFont(String);
extern int   YHelpSetXrefFont(String);
extern int   YHelpSetTitleFont(String);
extern int   YHelpSetVerbatimFont(String);

extern int   YHelpSetTextFontStruct(XFontStruct *);
extern int   YHelpSetXrefFontStruct(XFontStruct *);
extern int   YHelpSetTitleFontStruct(XFontStruct *);
extern int   YHelpSetVerbatimFontStruct(XFontStruct *);

extern int   YHelpInitialise(Widget, String, String (*)(void),
			     void (*)(void), String);

extern void  YHelpSetIdentifyingBitmap(Pixmap bitmap);
extern int   YHelpSetTopic(String topic);
extern int   YHelpInstallHelp(void);
extern void  YHelpClose(void);

extern int   YHelpSetHelpFile(String filename);
extern char *YHelpGetHelpFile(void);
extern int   YHelpCloseHelpFile(void);

extern void  YHelpDoneButtonAction(Widget, XEvent *, String *, Cardinal *);
extern void  YHelpContextHelpAction(Widget, XEvent *, String *, Cardinal *);

extern int   YHelpWriteIndexFile(void);

extern XFontStruct *yHelpTextFont;
extern XFontStruct *yHelpXrefFont;
extern XFontStruct *yHelpXmplFont;
extern XFontStruct *yHelpHeadFont;

#endif /* _Y_HELP_STUFF_ */

