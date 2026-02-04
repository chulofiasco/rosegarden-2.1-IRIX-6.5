
/*
   YAWN :  Yet Another Widget Neatener
   -----------------------------------

   Module for simple widget management.  The header and library
   together provide the following Xt/Xaw-based conveniences:

     -- Widget creation macro

     -- Resource setting and getting macros

     -- Utility functions for creating pixmaps, querying fonts and
        saving and restoring pointer positions

     -- Widget-with-background-pixmap creation function

     -- Functions for creating pretty Label, Command, Repeater and
        MenuButton widgets, with leftBitmaps and lined surrounds

     -- Toggle buttons with tick marks

     -- Text display code

     -- Question Box code

     -- User Input Box code

     -- Modal Menu code

     -- File Selection code

     -- Help code (in YHelp.h and libYHelp.a)

   See the man page for more documentation.
   You'll need my Debug.h header, my Lists library and the
   appropriate Yawn bitmaps in order to compile this.

   Chris Cannam, 1994
*/


#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Repeater.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>

#include <Yawn.h>
#include <SysDeps.h>
#include <Debug.h>

#include "YPop.h"

#include        "grey.xbm"
#include   "lightgrey.xbm"
#include        "down.xbm"
#include       "right.xbm"
#include  "left_arrow.xbm"
#include "right_arrow.xbm"
#include    "up_arrow.xbm"
#include  "down_arrow.xbm"
#include      "ticked.xbm"
#include    "unticked.xbm"


#define DefaultFont "fixed"


Widget _Y_Wdg;
Arg    _Y_Arg;

static Display *display;
static Window   root;
static Widget   refw;
static unsigned depth;

GC             _yClearGC;
GC             _yDrawingGC;
GC             _yMessageGC;

Boolean        _yInitialised     = False;
Boolean        _yFileInitialised = False;
Boolean        _yMsgsInitialised = False;

static Pixel    fg;
static Pixel    bg;
static Pixel    lightFg;
static Pixel    lightBg;
static Pixel    mediumFg;
static Pixel    mediumBg;
static Pixel    surroundFg;
static Pixel    surroundBg;
static Pixel    highlightFg;
static Pixel    highlightBg;
static Pixel    xorFg;
static Pixel    xorBg;

static Pixmap       yGreyMap       = 0;
static Pixmap       yLightGreyMap  = 0;

Pixmap yToggleOnMap;
Pixmap yToggleOffMap;

static Pixmap       yMessageBitmap = 0;
static XFontStruct *yMessageRFont  = NULL;
static XFontStruct *yMessageIFont  = NULL;
static XFontStruct *yMessageBFont  = NULL;
static XFontStruct *yMessageEFont  = NULL;

static YHelpCallback yHelpCallback = NULL;

static String  yFileReadDir     = NULL;
static String  yFileWriteDir    = NULL;
static Boolean yFileAsk         = True;
static Boolean yFileBak         = True;
static String  yFileReadLabel   = NULL;
static String  yFileWriteLabel  = NULL;
static String  yFileAppendLabel = NULL;

static String  yFileName;
static FILE  * yFileFile;

static XPoint  yPointerPosition;
static Boolean yHavePosition = False;

Boolean _yIsXaw3d;

static char    yFontErrorMsg[400];
extern void   _YMenuCleanUp(void);

static void YGenericHelpAction(Widget, XEvent *, String *, Cardinal *);



/* Initialisation and cleaning up */


Boolean YIsInitialised(void)
{
  Begin("YIsInitialised");
  Return(_yInitialised);
}


void YCleanUp(void)
{
  Begin("YCleanUp");

  if (!_yInitialised) End;

  XFreeGC(display, _yClearGC);
  XFreeGC(display, _yDrawingGC);
  XFreeGC(display, _yMessageGC);

  if (yToggleOffMap) XFreePixmap(display, yToggleOffMap);
  if (yToggleOnMap)  XFreePixmap(display, yToggleOnMap);

  YCreateUnmanagedWidget (NULL, NULL, NULL, NoShade);
  YCreateCommand         (NULL, NULL);
  YCreateMenuButton      (NULL, NULL);
  YCreateRepeater        (NULL, NULL);
  YCreateArrowButton     (NULL, NULL, YArrowLeft);

  if (yMessageRFont) {
    XFreeFont(display, yMessageRFont);
    yMessageRFont = NULL;
  }

  if (yMessageBFont) {
    XFreeFont(display, yMessageBFont);
    yMessageBFont = NULL;
  }

  if (yMessageIFont) {
    XFreeFont(display, yMessageIFont);
    yMessageIFont = NULL;
  }

  if (yMessageEFont) {
    XFreeFont(display, yMessageEFont);
    yMessageEFont = NULL;
  }

  if (_yFileInitialised) {

    XtFree(yFileReadDir);
    XtFree(yFileWriteDir);
    XtFree(yFileReadLabel);
    XtFree(yFileWriteLabel);
    XtFree(yFileAppendLabel);
  }

  _YMenuCleanUp();

  End;
}


void YInitialise(Widget widget, YHelpCallback hCallback)
{
  Widget        yawn;
  Widget        labelMono;
  Widget        labelNoShade = 0;
  Widget        labelLightShade;
  Widget        labelMediumShade;
  Widget        labelSurroundShade;
  Widget        labelHighlightShade;
  Widget        labelXorShade;
  int           threeDtester;
  XFontStruct  *font;
  XGCValues     values;
  unsigned long fullGCMask = GCFunction | GCPlaneMask |
    GCForeground | GCBackground | GCLineWidth | GCFont;

  static XtActionsRec yActions[] = {
    { "dialogue-default", _YDialogueDefaultAction },
    { "dialogue-cancel",  _YDialogueCancelAction  },
    { "dialogue-help",    _YDialogueHelpAction    },
    { "generic-help",     YGenericHelpAction     },
  };

  Begin("YInitialise");

  if (_yInitialised) End;

  refw    = widget;
  display = XtDisplay(widget);
  root    = RootWindowOfScreen(XtScreen(widget));
  depth   = DefaultDepthOfScreen(XtScreen(widget));

  if (hCallback) yHelpCallback = hCallback;

  yawn = XtCreateManagedWidget
    ("yawn", formWidgetClass,
     XtCreatePopupShell("yawn", transientShellWidgetClass, widget, 0,0), 0,0);

  if (depth == 1) {

    labelMono =
      XtCreateManagedWidget("mono", labelWidgetClass, yawn, 0, 0);

    YGetValue(labelMono, XtNforeground, &fg);
    YGetValue(labelMono, XtNbackground, &bg);

  } else {

    labelNoShade =
      XtCreateManagedWidget("noShade",       labelWidgetClass, yawn, 0, 0);
    labelLightShade =
      XtCreateManagedWidget("lightShade",    labelWidgetClass, yawn, 0, 0);
    labelMediumShade =
      XtCreateManagedWidget("mediumShade",   labelWidgetClass, yawn, 0, 0);
    labelSurroundShade =
      XtCreateManagedWidget("surroundShade", labelWidgetClass, yawn, 0, 0);
    labelHighlightShade =
      XtCreateManagedWidget("highlight",     labelWidgetClass, yawn, 0, 0);
    labelXorShade =
      XtCreateManagedWidget("xor",           labelWidgetClass, yawn, 0, 0);

    YGetValue(labelNoShade,        XtNforeground, &fg);
    YGetValue(labelNoShade,        XtNbackground, &bg);
    YGetValue(labelLightShade,     XtNforeground, &lightFg);
    YGetValue(labelLightShade,     XtNbackground, &lightBg);
    YGetValue(labelMediumShade,    XtNforeground, &mediumFg);
    YGetValue(labelMediumShade,    XtNbackground, &mediumBg);
    YGetValue(labelSurroundShade,  XtNforeground, &surroundFg);
    YGetValue(labelSurroundShade,  XtNbackground, &surroundBg);
    YGetValue(labelHighlightShade, XtNforeground, &highlightFg);
    YGetValue(labelHighlightShade, XtNbackground, &highlightBg);
    YGetValue(labelXorShade,       XtNforeground, &xorFg);
    YGetValue(labelXorShade,       XtNbackground, &xorBg);
  }

  if (!labelNoShade) labelNoShade = labelMono;

  /* This won't always work.  Why not?  To do with common strings? */
  /*  XtVaSetValues(labelNoShade, "shadowWidth", 16, NULL);*/
  threeDtester = 9999;
  XtVaGetValues(labelNoShade, "shadowWidth", &threeDtester, NULL);
  _yIsXaw3d = (threeDtester != 9999);

  YGetValue(labelNoShade, XtNfont, &font);
  XtDestroyWidget(yawn);

  if (XGetGCValues(display, DefaultGCOfScreen(XtScreen(widget)),
		   fullGCMask, &values) == 0)
    XtAppError(XtWidgetToApplicationContext(widget),
	       "Could not get default graphics context values");

  values.function   = GXcopy;
  values.foreground = bg;
  values.background = bg;
  values.line_width = 0;
  values.font       = font->fid;
  _yClearGC   = YCreateGC(fullGCMask, &values, NoShade, True);

  values.foreground = fg;
  _yDrawingGC = YCreateGC(fullGCMask, &values, NoShade, False);
  _yMessageGC = YCreateGC(fullGCMask, &values, NoShade, False);

  XtAppAddActions(XtWidgetToApplicationContext(widget),
		  yActions, XtNumber(yActions));

  _yShouldWarp = True;
  _yInitialised = True;
  End;
}




/* Pointer warping */

XPoint YPushPointerPosition(void)
{
  Boolean  qp;
  Window   rw, cw;
  int      wx, wy;
  int      ox, oy;
  unsigned mask;
  
  Begin("YPushPointerPosition");

  qp = XQueryPointer(display, root,
		     &rw, &cw, &ox, &oy, &wx, &wy, &mask);

  if (qp) {

    yPointerPosition.x = (short)ox;
    yPointerPosition.y = (short)oy;
    yHavePosition = True;

  } else {

    yPointerPosition.x = 0;
    yPointerPosition.y = 0;
    yHavePosition = False;
  }

  Return(yPointerPosition);
}


XPoint YPopPointerPosition(void)
{
  Begin("YPopPointerPosition");

  if (yHavePosition) {

    if (_yShouldWarp) {
      XWarpPointer(display, 0, root, 0, 0, 0, 0,
		   (int)(yPointerPosition.x), (int)(yPointerPosition.y));
    }

    yHavePosition = False;

  } else {

    yPointerPosition.x = 0;
    yPointerPosition.y = 0;
  }

  Return(yPointerPosition);
}


void YShouldWarpPointer(Boolean should)
{
  Begin("YShouldWarpPointer");

  if (!_yInitialised) {

    /* we'd like an X error, but have no handle on anything X from here */

    fprintf(stderr, "Yawn Toolkit Fatal Error: YShouldWarpPointer called "
	    "before YInitialise\n");
    exit(255);
  }

  _yShouldWarp = should;

  End;
}


GC YCreateGC(unsigned long mask, XGCValues *values,
	     YShade shade, Boolean clear)
{
  Begin("YCreateGC");

  if (shade == NoShade || depth == 1) {
 
    values->foreground = fg;
    values->background = bg;

  } else {

    if (values->function == GXxor) {

      values->foreground = xorFg;
      values->background = xorBg;

    } else {

      values->foreground = (shade == MediumShade ? mediumFg : lightFg);
      values->background = (shade == MediumShade ? mediumBg : lightBg);
    }
  }

  if (clear) {
    values->foreground = values->background;
  }

  Return(XCreateGC(display, root, mask | GCForeground | GCBackground, values));
}
  


Pixmap YCreatePixmapFromData(String data, unsigned width, unsigned height,
			     YShade shade)
{
  Pixmap map;

  Begin("YCreatePixmapFromData");

  if (depth > 1)
    map = XCreatePixmapFromBitmapData
      (display, root, data, width, height,
       shade == NoShade ? fg : shade == MediumShade ? mediumFg :
       shade == LightShade ? lightFg : surroundFg,
       shade == NoShade ? bg : shade == MediumShade ? mediumBg :
       shade == LightShade ? lightBg : surroundBg,
       depth);
  else
    map = XCreatePixmapFromBitmapData
      (display, root, data, width, height, fg, bg, depth);

  Return(map);
}



void _YLightenWindow(Widget w, XtPointer c, XEvent *e, Boolean *b)
{
    Begin("_YLightenWindow");

    YSetValue(w, XtNbackground, highlightBg);
    if (b) *b = True;

    End;
}


void YLightenWidget(Widget w)
{
    _YLightenWindow(w, NULL, NULL, NULL);
}


void _YDarkenWindow(Widget w, XtPointer c, XEvent *e, Boolean *b)
{
    Pixel background = (Pixel)c;
    
    Begin("_YDarkenWindow");

    YSetValue(w, XtNbackground, background);
    if (b) *b = True;

    End;
}


void YDarkenWidget(Widget w)
{
    _YDarkenWindow(w, (XtPointer)bg, NULL, NULL);
}



/* Widget creation */


Widget YCreateUnmanagedWidget(String name, WidgetClass wclass,
			      Widget parent, YShade shade)
{
    Widget ret;

    Begin("YCreateUnmanagedWidget");

    if (name == NULL) {

	if (yGreyMap)
	  { XFreePixmap(display, yGreyMap); yGreyMap = 0; }

	if (yLightGreyMap)
	  { XFreePixmap(display, yLightGreyMap); yLightGreyMap = 0; }

	Return(NULL);
    }

    if (depth == 1) {

	if (!yGreyMap) {
    
	    if (!_yInitialised)
	      XtAppError(XtWidgetToApplicationContext(parent),
			 "YCreateUnmanagedWidget called before YInitialise");

	    yGreyMap = YCreatePixmapFromData
	      (grey_bits, grey_width, grey_height, NoShade);

	    yLightGreyMap = YCreatePixmapFromData
	      (lightgrey_bits, lightgrey_width, lightgrey_height, NoShade);
	}

	ret = XtCreateWidget(name, wclass, parent, NULL, 0);

	YSetValue(ret, XtNbackground, bg);

	if (shade != NoShade && shade != SurroundShade &&
	    wclass != asciiTextWidgetClass)
	  YSetValue(ret, XtNbackgroundPixmap,
		    (shade == LightShade ? yLightGreyMap : yGreyMap));

	YSetValue(ret, XtNforeground, fg);
	YSetValue(ret, XtNborder,     fg);

    } else {

	Pixel background;

	ret = XtCreateWidget(name, wclass, parent, NULL, 0);
	if (wclass == asciiTextWidgetClass) Return(ret);

	YSetValue(ret, XtNforeground,
		  (shade ==    LightShade ?    lightFg :
		   shade ==   MediumShade ?   mediumFg : 
		   shade == SurroundShade ? surroundFg : fg));
    
	YSetValue(ret, XtNborder,
		  (shade ==    LightShade ?    lightFg :
		   shade ==   MediumShade ?   mediumFg : 
		   shade == SurroundShade ? surroundFg : fg));

	background = (shade ==    LightShade ?    lightBg :
		      shade ==   MediumShade ?   mediumBg : 
		      shade == SurroundShade ? surroundBg : bg);
	
	YSetValue(ret, XtNbackground, background);

	if (wclass == commandWidgetClass ||
	    wclass == menuButtonWidgetClass ||
	    wclass == toggleWidgetClass ||
	    wclass == repeaterWidgetClass) {

	    XtAddEventHandler(ret, EnterWindowMask, False,
			      _YLightenWindow, NULL);

	    XtAddEventHandler(ret, LeaveWindowMask, False,
			      _YDarkenWindow, (XtPointer)background);
	}
    }

    Return(ret);
}


Widget _YCreateSurroundingBox(String name, Widget parent, YShade shade)
{
  Widget     w;
  static Arg arg[] = {
    { XtNhSpace, 2 },
    { XtNvSpace, 1 },
    { XtNtop , XawChainTop  }, { XtNbottom, XawChainTop  },
    { XtNleft, XawChainLeft }, { XtNright,  XawChainLeft },
  };

  Begin("_YCreateSurroundingBox");

  w = YCreateShadedWidget(name, boxWidgetClass, parent, shade);
  XtSetValues(w, arg, XtNumber(arg));

  Return(w);
}


Widget YCreateShadedWidget(String name, WidgetClass wclass,
			   Widget parent, YShade shade)
{
  Widget w;

  Begin("YCreateShadedWidget");

  w = YCreateUnmanagedWidget(name, wclass, parent, shade);
  XtManageChild(w);

  Return(w);
}


Widget YCreateActionButton(String name, Widget parent,
			   Pixmap leftBitmap, WidgetClass c)
{
    Widget         button;
    static Pixel   background;
/*    static Boolean inited = False; */
    static Arg     larg   = { XtNleftBitmap, 0 };
    static Arg     barg[] = { { XtNvSpace, 1 }, { XtNhSpace, 2 }, };
    static Arg     bgarg  = { XtNbackground, (XtArgVal)&background };

    Begin("YCreateActionButton");

    button = XtCreateManagedWidget
      (name, c, _YCreateSurroundingBox(name, parent, SurroundShade), NULL, 0);

    XtGetValues(button, &bgarg, 1);

    XtAddEventHandler(button, EnterWindowMask, False, _YLightenWindow, NULL);
    XtAddEventHandler(button, LeaveWindowMask, False, _YDarkenWindow,
		      (XtPointer)background);

    larg.value = leftBitmap;
    XtSetValues(button, &larg, 1);
    XtSetValues(XtParent(button), barg, 2);
    
    Return(button);
}


Widget YCreateShadedActionButton(String name, Widget parent,
				 Pixmap leftBitmap, WidgetClass c,
				 YShade shade)
{
    Widget         button;
    static Pixel   background;
    static Arg     larg   = { XtNleftBitmap, 0 };
    static Arg     barg[] = { { XtNvSpace, 1 }, { XtNhSpace, 2 }, };
    static Arg     bgarg  = { XtNbackground, (XtArgVal)&background };

    Begin("YCreateActionButton");

    button = YCreateSurroundedWidget(name, c, parent, SurroundShade, shade);

    XtGetValues(button, &bgarg, 1);

    XtAddEventHandler(button, EnterWindowMask, False, _YLightenWindow, NULL);
    XtAddEventHandler(button, LeaveWindowMask, False, _YDarkenWindow,
		      (XtPointer)background);

    larg.value = leftBitmap;
    XtSetValues(button, &larg, 1);
    XtSetValues(XtParent(button), barg, 2);
    
    Return(button);
}


Widget YCreateCommand(String name, Widget parent)
{
  static Pixmap buttonMap = 0;

  Begin("YCreateCommand");

  if (name == NULL) {

    if (buttonMap) XFreePixmap(display, buttonMap);
    buttonMap = 0;
    Return(0);
  }

  if (!buttonMap) {

    if (!_yInitialised)
      XtAppError(XtWidgetToApplicationContext(parent),
		 "YCreateCommand called before YInitialise");

    buttonMap = XCreateBitmapFromData
      (display, root, right_bits, right_width, right_height);
  }

  Return(YCreateActionButton(name, parent, buttonMap, commandWidgetClass));
}


Widget YCreateMenuButton(String name, Widget parent)
{
  static Pixmap buttonMap = 0;

  Begin("YCreateMenuButton");

  if (name == NULL) {

    if (buttonMap) XFreePixmap(display, buttonMap);
    buttonMap = 0;
    Return(0);
  }

  if (!buttonMap) {

    if (!_yInitialised)
      XtAppError(XtWidgetToApplicationContext(parent),
		 "YCreateMenuButton called before YInitialise");
    
    buttonMap = XCreateBitmapFromData
      (display, root, down_bits, down_width, down_height);
  }

  Return(YCreateActionButton(name, parent, buttonMap, menuButtonWidgetClass));
}


Widget YCreateRepeater(String name, Widget parent)
{
  static Pixmap buttonMap = 0;

  Begin("YCreateRepeater");

  if (name == NULL) {

    if (buttonMap) XFreePixmap(display, buttonMap);
    buttonMap = 0;
    Return(0);
  }

  if (!buttonMap) {

    if (!_yInitialised)
      XtAppError(XtWidgetToApplicationContext(parent),
		 "YCreateRepeater called before YInitialise");

    buttonMap = XCreateBitmapFromData
      (display, root, right_bits, right_width, right_height);
  }

  Return(YCreateActionButton(name, parent, buttonMap, repeaterWidgetClass));
}


Widget YCreateArrowButton(String name, Widget parent, YArrowDirection dir)
{
  static Pixmap    upMap = 0;
  static Pixmap  downMap = 0;
  static Pixmap  leftMap = 0;
  static Pixmap rightMap = 0;
  static Pixel   background;

  Widget        rtn;
  static Arg    bgarg  = { XtNbackground, (XtArgVal)&background };
  static Arg    barg[] = { { XtNvSpace, 1 }, { XtNhSpace, 2 }, };
  static Arg    carg[] = {
    { XtNforeground, 0 },
    { XtNbackground, 0 },
    { XtNborder,     0 },
  };

  Begin("YCreateArrowButton");

  if (name == NULL) {

    if (   upMap) XFreePixmap(display,    upMap);
    if ( downMap) XFreePixmap(display,  downMap);
    if ( leftMap) XFreePixmap(display,  leftMap);
    if (rightMap) XFreePixmap(display, rightMap);

    upMap = downMap = leftMap = rightMap = 0;

    Return(0);
  }

  if (!upMap) {

    if (!_yInitialised)
      XtAppError(XtWidgetToApplicationContext(parent),
		 "YCreateArrowButton called before YInitialise");

    upMap    = XCreateBitmapFromData
      (display, root,    up_arrow_bits,    up_arrow_width,    up_arrow_height);
    downMap  = XCreateBitmapFromData
      (display, root,  down_arrow_bits,  down_arrow_width,  down_arrow_height);
    leftMap  = XCreateBitmapFromData
      (display, root,  left_arrow_bits,  left_arrow_width,  left_arrow_height);
    rightMap = XCreateBitmapFromData
      (display, root, right_arrow_bits, right_arrow_width, right_arrow_height);

    carg[0].value = (XtArgVal)fg;
    carg[1].value = (XtArgVal)bg;
    carg[2].value = (XtArgVal)fg;
  }

  rtn = XtCreateManagedWidget
    (name, repeaterWidgetClass,
     _YCreateSurroundingBox(name, parent, SurroundShade), carg, 3);

  XtSetValues(XtParent(rtn), barg, 2);
  XtSetValues(XtParent(rtn), carg, 3);

  YSetValue(rtn, XtNbitmap,
	    dir == YArrowUp   ?   upMap :
	    dir == YArrowDown ? downMap :
	    dir == YArrowLeft ? leftMap : rightMap);

  XtGetValues(rtn, &bgarg, 1);

  XtAddEventHandler(rtn, EnterWindowMask, False, _YLightenWindow, NULL);
  XtAddEventHandler(rtn, LeaveWindowMask, False, _YDarkenWindow,
		    (XtPointer)background);


  Return(rtn);
}


void YSetToggleValue(Widget w, Boolean set)
{
  Begin("YSetToggleValue");

  YSetValue(w, XtNleftBitmap, set ? yToggleOnMap : yToggleOffMap);

  End;
}


Boolean YGetToggleValue(Widget w)
{
  Pixmap map;

  Begin("YGetToggleValue");

  YGetValue(w, XtNleftBitmap, &map);

  Return(map == yToggleOnMap);
}
 

static void YToggleCallback(Widget w, XtPointer a, XtPointer b)
{
  Boolean        set;
  XtCallbackProc callback = (XtCallbackProc)a;

  Begin("YToggleCallback");

  set = YGetToggleValue(w);
  YSetToggleValue(w, !set);

  if (callback) callback(w, (XtPointer)(!set), NULL);

  End;
} 


Widget YCreateToggle(String name, Widget parent, XtCallbackProc callback)
{
  Widget button;

  Begin("YCreateToggle");

  if (!yToggleOnMap) {

    if (!_yInitialised)
      XtAppError(XtWidgetToApplicationContext(parent),
		 "YCreateToggle called before YInitialise");

    yToggleOnMap = XCreateBitmapFromData
      (display, root, ticked_bits, ticked_width, ticked_height);

    yToggleOffMap = XCreateBitmapFromData
      (display, root, unticked_bits, unticked_width, unticked_height);
  }

  button =
    YCreateActionButton(name, parent, yToggleOffMap, commandWidgetClass);

  /*  if (callback) {*/
    XtAddCallback(button, XtNcallback, YToggleCallback, (XtPointer)callback);
    /*  }*/

  Return(button);
}


void YSetScrollbarPixmap(Widget scrollbar)
{
  Begin("YSetScrollbarPixmap");

  if (!yLightGreyMap) {
      yLightGreyMap = YCreatePixmapFromData
	(lightgrey_bits, lightgrey_width, lightgrey_height, NoShade);
  }

  YSetValue(scrollbar, XtNthumb, yLightGreyMap);

  End;
}


void YSetViewportScrollbarPixmaps(Widget viewport)
{
  Widget scrollbar;

  Begin("YSetViewportScrollbarPixmaps");

  scrollbar = XtNameToWidget(viewport, "vertical");
  if (scrollbar) YSetScrollbarPixmap(scrollbar);

  scrollbar = XtNameToWidget(viewport, "horizontal");
  if (scrollbar) YSetScrollbarPixmap(scrollbar);

  End;
}


Widget _YCreateTextEntryField(String name, Widget parent, Boolean numeric,
			      int max, Boolean updown)
{
  Widget box, label, text;  
  Begin("_YCreateTextEntryField");

  box = _YCreateSurroundingBox(name, parent, SurroundShade);

  label = YCreateShadedWidget(name, labelWidgetClass, box, SurroundShade);
  XtVaSetValues(label, XtNborderWidth, 0, NULL);

  text = YCreateShadedWidget(name, asciiTextWidgetClass, box, NoShade);

  Return(text);
}


/* Convenience routines to load up fonts. */


XFontStruct *YLoadQueryFont(String fn)
{
  XFontStruct *font;

  Begin("YLoadQueryFont");

  if (fn == NULL || (font = XLoadQueryFont(display, fn)) == NULL) {

    sprintf(yFontErrorMsg, "No font \"%s\", trying \"%s\"\n", fn, DefaultFont);

    if (fn)
      XtAppWarning(XtWidgetToApplicationContext(refw), yFontErrorMsg);

    if ((font = XLoadQueryFont(display, DefaultFont)) == NULL)
      XtAppError(XtWidgetToApplicationContext(refw),
		 "Couldn't load default font");
  }

  Return(font);
}


XFontStruct *YRequestFont(String fn,
			   Boolean bold, Boolean italic)
{
  int          i;
  String       str;
  String       name;
  String       oldfn;
  XFontStruct *font;
  String       demibold = NULL;
  String       oblique  = NULL;

  Begin("YRequestFont");

  if (!fn) Return(YLoadQueryFont(fn));

  oldfn    = XtNewString(fn);
  name     = (String)XtMalloc(strlen(fn) + 15);

  str = oblique = strstr(fn, "-o-");
  if (!str) str = strstr(fn, "-r-");
  if (!str) str = strstr(fn, "-i-");
  if ( str) {

    i = (int)(str-fn);
    (void)strncpy(name, fn, i);

    if (italic)
      if (oblique)
	strncpy(name+i, "-o-", 3);
      else strncpy(name+i, "-i-", 3);
    else strncpy(name+i, "-r-", 3);

    strcpy(name+i+3, str+3);

  } else strcpy(name, fn);

  fn = name;
  name = (String)XtMalloc(strlen(fn) + 15);

  str = demibold = strstr(fn, "-demibold-");
  if (!str)  str = strstr(fn, "-bold-");
  if (!str)  str = strstr(fn, "-medium-");
  if ( str) {

    i = (int)(str-fn);
    (void)strncpy(name, fn, i);

    if (bold)
      if (demibold)
	strncpy(name+i, "-demibold-", 10);
      else strncpy(name+i, "-bold-", 6);
    else strncpy(name+i, "-medium-", 8);

    strcpy(name + i + (bold ? (demibold ? 10 : 6) : 8),
	   str + (strstr(fn, "-bold-") ? 6 : (demibold ? 10 : 8)));

  } else strcpy(name, fn);

  if ((font = XLoadQueryFont(display, name)) == NULL) {

    if ((font = XLoadQueryFont(display, oldfn)) == NULL) {

      sprintf(yFontErrorMsg, "No font \"%s\" or \"%s\", trying \"%s\"\n",
	      name, oldfn, DefaultFont);

      XtAppWarning(XtWidgetToApplicationContext(refw), yFontErrorMsg);

      if ((font = XLoadQueryFont(display, DefaultFont)) == NULL)
	XtAppError(XtWidgetToApplicationContext(refw),
		   "Couldn't load default font");
    }
  }

  XtFree(name);
  XtFree(oldfn);
  XtFree(fn);

  Return(font);
}


Pixmap YGetMessagePixmap(YMessageString *text,
			 int number, Dimension width, Dimension height)
{
  int          i;
  Pixmap       map;
  int          asc, desc, dir;
  XCharStruct  info;
  XFontStruct *font;
  XGCValues    values;

  Begin("YGetMessagePixmap");

  map = XCreatePixmap(display, root, width, height, depth);

  XFillRectangle(display, map, _yClearGC, 0, 0, width, height);

  for (i = 0; i < number; ++i) {

    font = text[i].bold ?
      (text[i].italic ? yMessageEFont : yMessageBFont) :
	(text[i].italic ? yMessageIFont : yMessageRFont);

    values.font = font->fid;
    XChangeGC(display, _yMessageGC, GCFont, &values);

    XTextExtents(font, text[i].text, strlen(text[i].text),
		 &dir, &asc, &desc, &info);

    XDrawString(display, map, _yMessageGC, width/2 - info.width/2,
		(int)(((long)height*(long)(i+1))/(long)(number+1))+asc/2,
		text[i].text, strlen(text[i].text));
  }

  Return(map);
}
  

Boolean YMessageIsInitialised(void)
{
  Begin("YMessageIsInitialised");
  Return(_yInitialised ? _yMsgsInitialised : False);
}


void YMessageInitialise(Pixmap bitmap, String basefont)
{
  Begin("YMessageInitialise");

  _yMsgsInitialised = True;

  yMessageBitmap = bitmap;

  yMessageRFont = YRequestFont(basefont, False, False);
  yMessageBFont = YRequestFont(basefont,  True, False);
  yMessageIFont = YRequestFont(basefont, False,  True);
  yMessageEFont = YRequestFont(basefont,  True,  True);

  End;
}


void YMessageCallback(Widget w, XtPointer client, XtPointer call)
{
  Widget shell = (Widget)client;

  Begin("YMessageCallback");

  YMessage(XtParent(shell), NULL, NULL, NULL, 0);
  YPopdown(shell);
  YRetractDialogueActions(shell);
  XtDestroyWidget(shell);

  End;
}


void YMessage(Widget parent, String name, String label,
	      YMessageString *text, int number)
{
  Widget     mShell;
  Widget     mPane;
  Widget     mTitleBox;
  Widget     mTextForm;
  Widget     mButtonBox;
  Widget     mLabel;
  Widget     mText;
  Widget     mConfirm;
  XPoint     op;
  Dimension  h1, h2;

  static Pixmap mMap = 0;

  Begin("YMessage");

  if (!_yMsgsInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YMessage called before YMessageInitialise");

  if (!label) {
    (void)YPopPointerPosition();
    if (mMap) { XFreePixmap(display, mMap); mMap = 0; }
    End;
  }

  mShell = XtCreatePopupShell("Message",
			      transientShellWidgetClass, parent, NULL, 0);

  if (name) YSetValue(mShell, XtNtitle, name);

  mPane = YCreateWidget("Message Pane", panedWidgetClass, mShell);

  if (yMessageBitmap) {

    mTitleBox = YCreateShadedWidget
      ("Message Title Box", boxWidgetClass, mPane, MediumShade);
    YSetValue(mTitleBox, XtNshowGrip, False);

    mLabel = YCreateLabel("Bitmap Label", mTitleBox);
    YSetValue(mLabel, XtNbitmap, yMessageBitmap);
  }

  if (text) {

    mTextForm = YCreateShadedWidget
      ("Message Text Form", formWidgetClass, mPane, LightShade);

    mMap = YGetMessagePixmap(text, number, 400, 200);
    mText = YCreateWidget("Message Text", labelWidgetClass, mTextForm);

    YSetValue(mTextForm, XtNshowGrip, False);
    YSetValue(mText, XtNbitmap, mMap);
  }

  mButtonBox = YCreateShadedWidget
    ("Message Button Box", boxWidgetClass, mPane, MediumShade);

  mConfirm = YCreateCommand("Message OK", mButtonBox);
  YSetValue(mConfirm, XtNlabel, label);

  XtAddCallback(mConfirm, XtNcallback, YMessageCallback, (XtPointer)mShell);

  XtRealizeWidget(mShell);

  YGetValue(mConfirm, XtNheight, &h1);

  if (yMessageBitmap) {
    YGetValue(mLabel, XtNheight, &h2);
  }

  XtUnrealizeWidget(mShell);

  YSetValue(mButtonBox, XtNmax, h1 + 15);
  YSetValue(mButtonBox, XtNmin, h1 + 15);

  if (yMessageBitmap) {
    YSetValue(mTitleBox,  XtNmax, h2 + 15);
    YSetValue(mTitleBox,  XtNmin, h2 + 15);
  }

  op = YPlacePopupAndWarp(mShell, XtGrabNone, mConfirm, mConfirm);
  YAssertDialogueActions(mShell, mConfirm, mConfirm, NULL);

  End;
}


static void YGenericHelpAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
  Begin("YGenericHelpAction");

  if (yHelpCallback)
    if (*c > 0) yHelpCallback(*s);
    else yHelpCallback(NULL);

  End;
}


static void YHelpButtonCallback(Widget w, XtPointer client, XtPointer call)
{
  String helpTag = (String)client;

  Begin("YHelpButtonCallback");

  if (yHelpCallback) yHelpCallback(helpTag);

  End;
}


static void YQueryCallback(Widget w, XtPointer client, XtPointer call)
{
  Widget *answer = (Widget *)client;

  Begin("YQueryCallback");
  *answer = w;
  End;
}


int YQuery(Widget parent, String message,
	   Cardinal buttons, int deft, int cancel, ...)

{
  int           i;
  va_list       ap;

  /* arguments from va_list: */

  String       *labels;
  String        helpTag;

  /* widgets: */

  Widget        qShell;
  Widget        qForm;
  Widget        qLabel;
  Widget       *qButtons;
  Widget        qHelp  = NULL;
  Widget        answer = NULL;

  /* warping, moving, event-looping: */

  XPoint        op;
/*  Position      px;
  Position      py;*/
  int           dir, asc, desc;
  XCharStruct   info;
  XFontStruct  *font;
  XtAppContext  context;

  Begin("YQuery");

  va_start(ap, cancel);

  labels   = (String *)XtCalloc(buttons, sizeof(String));
  qButtons = (Widget *)XtCalloc(buttons, sizeof(Widget));

  for (i = 0; i < buttons; ++i)
    labels[i] = (String)(va_arg(ap, char *));

  helpTag = (String)(va_arg(ap, char *));

  va_end(ap);

  if (!_yInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YQuery called before YInitialise");

  qShell = XtCreatePopupShell
    ("Query", transientShellWidgetClass, parent, NULL, 0);

  qForm = YCreateSurroundedWidget
    ("Query Form", formWidgetClass, qShell, NoShade, MediumShade);

  if (message) {
    qLabel = YCreateLabel("Query Label", qForm);
    YSetValue(qLabel, XtNlabel, message);
  }

  for (i = 0; i < buttons; ++i) {

    qButtons[i] = YCreateCommand("Query Button", qForm);

    YSetValue(qButtons[i], XtNlabel, labels[i]);

    if (i > 0)
      YSetValue(XtParent(qButtons[i]), XtNfromHoriz, XtParent(qButtons[i-1]));
    if (message)
      YSetValue(XtParent(qButtons[i]), XtNfromVert,  XtParent(qLabel));

    XtAddCallback(qButtons[i], XtNcallback, YQueryCallback, &answer);
  }

  if (helpTag && yHelpCallback) {

    qHelp = YCreateCommand("Help", qForm);

    if (buttons > 0)
      YSetValue(XtParent(qHelp), XtNfromHoriz, XtParent(qButtons[buttons-1]));
    if (message)
      YSetValue(XtParent(qHelp), XtNfromVert,  XtParent(qLabel));

    XtAddCallback(qHelp, XtNcallback, YHelpButtonCallback, helpTag);
  }

  if (message) {

    YGetValue(qLabel, XtNfont, &font);
    XTextExtents(font, message, strlen(message), &dir, &asc, &desc, &info);

    if (info.width > 600) info.width = 600;

  } else info.width = 500;

  op = YPlacePopupAndWarp(qShell, XtGrabExclusive,
			  deft   >= 0 ? qButtons[deft]   : NULL,
			  cancel >= 0 ? qButtons[cancel] : NULL);

  YAssertDialogueActions(qShell,
			 deft   >= 0 ? qButtons[deft]   : NULL,
			 cancel >= 0 ? qButtons[cancel] : NULL, qHelp);

  context = XtWidgetToApplicationContext(qShell);
  while (!answer || XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);
  for (i = 0; i < buttons; ++i) if (answer == qButtons[i]) break;

  if (deft >= 0 && (op.x || op.y)) (void)YPopPointerPosition();

  YPopdown(qShell);
  XtDestroyWidget(qShell);
  XtFree((void *)labels);
  XtFree((void *)qButtons);
  YRetractDialogueActions(qShell);

  Return(i < buttons ? i : -1);
}



/* User input */


static void YInputCallback(Widget w, XtPointer client, XtPointer call)
{
  Widget *answer = (Widget *)client;

  Begin("YQueryCallback");
  *answer = w;
  End;
}


String YGetUserInput(Widget parent, String message,
		     String deft, YOrientation orientation, String helpTag)
{
  String        text;

  /* widgets: */

  Widget        iShell;
  Widget        iForm;
  Widget        iLabel;
  Widget        iText;
  Widget        iYesButton;
  Widget        iNoButton;
  Widget        iHelpButton = NULL;
  Widget        answer = NULL;

  /* warping, moving, event-looping: */

  XPoint        op;
/*  Position      px;
  Position      py; */
  int           dir, asc, desc;
  XCharStruct   info;
  XFontStruct  *font;
  XtAppContext  context;

  Begin("YGetUserInput");

  if (!_yInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YGetUserInput called before YInitialise");

  iShell = XtCreatePopupShell
    ("User Input", transientShellWidgetClass, parent, NULL, 0);

  iForm = YCreateSurroundedWidget
    ("Input Form", formWidgetClass, iShell, NoShade, MediumShade);

  if (message) {
    iLabel = YCreateLabel("Input Label", iForm);
    YSetValue(iLabel, XtNlabel, message);
  }

  iText = YCreateSurroundedWidget
    ("Input Text", asciiTextWidgetClass, iForm, LightShade, NoShade);

  if (deft) {
    YSetValue(iText, XtNstring, deft);
    YSetValue(iText, XtNinsertPosition, strlen(deft));
  } else {
    YSetValue(iText, XtNstring,   "");
    YSetValue(iText, XtNinsertPosition, 0);
  }

  iYesButton = YCreateCommand("Apply", iForm);
  iNoButton = YCreateCommand("Cancel", iForm);

  if (orientation == YOrientHorizontal)
    YSetValue(XtParent(iText), XtNfromHoriz, message? XtParent(iLabel) : NULL);
  else
    YSetValue(XtParent(iText), XtNfromVert,  message? XtParent(iLabel) : NULL);

  YSetValue(XtParent(  iYesButton ), XtNfromVert,    XtParent(iText));
  YSetValue(XtParent(   iNoButton ), XtNfromVert,    XtParent(iText));
  YSetValue(XtParent(   iNoButton ), XtNfromHoriz,   XtParent(iYesButton));

  XtAddCallback(  iYesButton, XtNcallback, YInputCallback,      &answer);
  XtAddCallback(   iNoButton, XtNcallback, YInputCallback,      &answer);

  if (helpTag && yHelpCallback) {

    iHelpButton = YCreateCommand("Help", iForm);
    YSetValue(XtParent( iHelpButton ), XtNfromVert,    XtParent(iText));
    YSetValue(XtParent( iHelpButton ), XtNfromHoriz,   XtParent(iNoButton));
    XtAddCallback( iHelpButton, XtNcallback, YHelpButtonCallback, helpTag);
  } 

  if (message) {

    YGetValue(iLabel, XtNfont, &font);
    XTextExtents(font, message, strlen(message), &dir, &asc, &desc, &info);

    if (orientation == YOrientHorizontal) {
      info.width += 80;
    } else {
      info.width += 180;
    }

  } else {

    if (orientation == YOrientHorizontal) {
      info.width = 200;
    } else {
      info.width = 400;
    }
  }

  YSetValue(iText, XtNwidth, info.width);
  YSetValue(iText, XtNeditType, XawtextEdit);

  op = YPlacePopupAndWarp(iShell, XtGrabExclusive, iYesButton, iNoButton);
  XtSetKeyboardFocus(iShell, iText);
  YAssertDialogueActions(iShell, iYesButton, iNoButton, iHelpButton);

  context = XtWidgetToApplicationContext(iShell);
  while (!answer || XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);

  if (answer == iYesButton) YGetValue(iText, XtNstring, &text);
  else text = NULL;

  if (op.x || op.y) (void)YPopPointerPosition();

  YPopdown(iShell);
  XtDestroyWidget(iShell);
  YRetractDialogueActions(iShell);

  Return(text);
}



/* Files -- these are mostly synchronising wrappers for the YFile.c code: */


Boolean YFileIsInitialised(void)
{
  Begin("YFileIsInitialised");
  Return(_yInitialised ? _yFileInitialised : False);
}


void YFileInitialise(String dir, Boolean query,
		     Boolean backup, String rl, String wl, String al)
{
  Begin("YFileInitialise");

  if (yFileReadDir)    XtFree(yFileReadDir);
  if (yFileWriteDir)   XtFree(yFileWriteDir);
  if (yFileReadLabel)  XtFree(yFileReadLabel);
  if (yFileWriteLabel) XtFree(yFileWriteLabel);
  if (yFileAppendLabel)XtFree(yFileAppendLabel);

  yFileReadDir       = XtNewString(dir);
  yFileWriteDir      = XtNewString(dir);

  if (rl) yFileReadLabel   = XtNewString(rl);
  else    yFileReadLabel   = XtNewString("Apply");
  if (wl) yFileWriteLabel  = XtNewString(wl);
  else    yFileWriteLabel  = XtNewString("Apply");
  if (al) yFileAppendLabel = XtNewString(al);
  else    yFileAppendLabel = XtNewString("Apply");

  yFileAsk         = query;
  yFileBak         = backup;

  _yFileInitialised = True;

  End;
}


static void YFileNameCallback(String name, FILE *file)
{
  Begin("YFileNameCallback");

  yFileName = name;

  End;
}

static void YFileFileCallback(String name, FILE *file)
{
  Begin("YFileFileCallback");

  yFileName = name;
  yFileFile = file;

  End;
}


String YFileGetReadFilename(Widget parent, String helpTag,
			    String suffix, String suffixDescription)
{
  XtAppContext context;
  String newDir = NULL;

  Begin("YFileGetReadFilename");

  if (!_yFileInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YFileGetReadFilename called before YFileInitialise");

  yFileName = (String)1;

  YFileGetFileInformation(parent, yFileReadLabel,
			  "Sorry, I can't get a list of files.",
			  yFileReadDir, "r", &newDir, yFileAsk,
			  False, yFileBak, YFileNameCallback,
			  yHelpCallback ? helpTag : NULL,
			  YHelpButtonCallback, suffix, suffixDescription);

  context = XtWidgetToApplicationContext(parent);
  while (yFileName == (String)1 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (yFileName == (String)1) yFileName = NULL;
  if (newDir) { XtFree(yFileReadDir); yFileReadDir = newDir; }

  Return(yFileName);
}


FILE *YFileGetReadFile(Widget parent, String helpTag,
		       String suffix, String suffixDescription)
{
  XtAppContext context;
  String newDir = NULL;

  Begin("YFileGetReadFile");

  if (!_yFileInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YFileGetReadFile called before YFileInitialise");

  yFileFile = (FILE *)1;

  YFileGetFileInformation(parent, yFileReadLabel,
			  "Sorry, I can't get a list of files.",
			  yFileReadDir, "r", &newDir, yFileAsk,
			  True, yFileBak, YFileFileCallback,
			  yHelpCallback ? helpTag : NULL,
			  YHelpButtonCallback, suffix, suffixDescription);

  context = XtWidgetToApplicationContext(parent);
  while (yFileFile == (FILE *)1 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (yFileFile == (FILE *)1) yFileFile = NULL;
  if (newDir) { XtFree(yFileReadDir); yFileReadDir = newDir; }

  Return(yFileFile);
}


String YFileGetWriteFilename(Widget parent, String helpTag,
			     String suffix, String suffixDescription)
{
  XtAppContext context;
  String newDir = NULL;

  Begin("YFileGetWriteFilename");

  if (!_yFileInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YFileGetWriteFilename called before YFileInitialise");

  yFileName = (String)1;

  YFileGetFileInformation(parent, yFileWriteLabel,
			  "Sorry, I can't get a list of files.",
			  yFileWriteDir, "w", &newDir, yFileAsk,
			  False, yFileBak, YFileNameCallback,
			  yHelpCallback ? helpTag : NULL,
			  YHelpButtonCallback, suffix, suffixDescription);

  context = XtWidgetToApplicationContext(parent);
  while (yFileName == (String)1 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (yFileName == (String)1) yFileName = NULL;
  if (newDir) { XtFree(yFileWriteDir); yFileWriteDir = newDir; }

  Return(yFileName);
}


FILE *YFileGetWriteFile(Widget parent, String helpTag,
			String suffix, String suffixDescription)
{
  XtAppContext context;
  String newDir = NULL;

  Begin("YFileGetWriteFile");

  if (!_yFileInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YFileGetWriteFile called before YFileInitialise");

  yFileFile = (FILE *)1;

  YFileGetFileInformation(parent, yFileWriteLabel,
			  "Sorry, I can't get a list of files.",
			  yFileWriteDir, "w", &newDir, yFileAsk,
			  True, yFileBak, YFileFileCallback,
			  yHelpCallback ? helpTag : NULL,
			  YHelpButtonCallback, suffix, suffixDescription);

  context = XtWidgetToApplicationContext(parent);
  while (yFileFile == (FILE *)1 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (yFileFile == (FILE *)1) yFileFile = NULL;
  if (newDir) { XtFree(yFileWriteDir); yFileWriteDir = newDir; }

  Return(yFileFile);
}


String YFileGetAppendFilename(Widget parent, String helpTag,
			      String suffix, String suffixDescription)
{
  XtAppContext context;
  String newDir = NULL;

  Begin("YFileGetAppendFilename");

  if (!_yFileInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YFileGetAppendFilename called before YFileInitialise");

  yFileName = (String)1;

  YFileGetFileInformation(parent, yFileAppendLabel,
			  "Sorry, I can't get a list of files.",
			  yFileWriteDir, "a", &newDir, yFileAsk,
			  False, yFileBak, YFileNameCallback,
			  yHelpCallback ? helpTag : NULL,
			  YHelpButtonCallback, suffix, suffixDescription);

  context = XtWidgetToApplicationContext(parent);
  while (yFileName == (String)1 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (yFileName == (String)1) yFileName = NULL;
  if (newDir) { XtFree(yFileWriteDir); yFileWriteDir = newDir; }

  Return(yFileName);
}


FILE *YFileGetAppendFile(Widget parent, String helpTag,
			 String suffix, String suffixDescription)
{
  XtAppContext context;
  String newDir = NULL;

  Begin("YFileGetAppendFile");

  if (!_yFileInitialised)
    XtAppError(XtWidgetToApplicationContext(parent),
	       "YFileGetAppendFile called before YFileInitialise");

  yFileFile = (FILE *)1;

  YFileGetFileInformation(parent, yFileAppendLabel,
			  "Sorry, I can't get a list of files.",
			  yFileWriteDir, "a", &newDir, yFileAsk,
			  True, yFileBak, YFileFileCallback,
			  yHelpCallback ? helpTag : NULL,
			  YHelpButtonCallback, suffix, suffixDescription);

  context = XtWidgetToApplicationContext(parent);
  while (yFileFile == (FILE *)1 || XtAppPending(context))
    XtAppProcessEvent(context, XtIMAll);

  if (yFileFile == (FILE *)1) yFileFile = NULL;
  if (newDir) { XtFree(yFileWriteDir); yFileWriteDir = newDir; }

  Return(yFileFile);
}


String YFileGetLastFilename(Boolean clip)
{
  int i = -1;

  Begin("YFileGetLastFilename");

  if (yFileName == (String)1 || yFileName == NULL) Return(NULL);

  if (clip)
    for (i = strlen(yFileName); i >= 0 && yFileName[i] != '/'; --i);

  Return(yFileName + ++i);
}
 
