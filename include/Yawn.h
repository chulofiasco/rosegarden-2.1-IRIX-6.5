
#ifndef _Y_WIDGET_STUFF_
#define _Y_WIDGET_STUFF_

#include <stdio.h>
#include <stdarg.h>
#include <X11/Intrinsic.h>


extern Widget _Y_Wdg;
extern Arg    _Y_Arg;



typedef enum {
  NoShade, SurroundShade, LightShade, MediumShade /* sorry, no DarkShade */
} YShade;

typedef struct  _YMessageString {
  String         text;
  Boolean        bold;
  Boolean        italic;
} YMessageString;

typedef enum { YOrientHorizontal, YOrientVertical } YOrientation;
typedef enum { YArrowLeft, YArrowRight, YArrowUp, YArrowDown } YArrowDirection;

typedef struct  _YMenuElement {
  String         label;
  unsigned long  insensitive_mode_mask;
  XtCallbackProc callback;
  unsigned char *toolbar_bitmap; /* must be YToolbarBitmapSize xbm, or NULL */
  Widget         widget;
} YMenuElement;

typedef void   * YMenuId;

typedef void  (* YHelpCallback)(String);



extern  void     YInitialise           (Widget, YHelpCallback);
extern  Boolean  YIsInitialised        (void);
extern  void     YCleanUp              (void);



extern  XFontStruct * YLoadQueryFont   (String);
extern  XFontStruct * YRequestFont     (String, Boolean, Boolean);



extern  GC       YCreateGC             (unsigned long, XGCValues *,
					YShade, Boolean);
extern  Pixmap   YCreatePixmapFromData (String, unsigned, unsigned, YShade);

extern  void     YSetScrollbarPixmap          (Widget);
extern  void     YSetViewportScrollbarPixmaps (Widget);



extern  Widget   YCreateUnmanagedWidget(String, WidgetClass, Widget, YShade);
extern  Widget   YCreateShadedWidget   (String, WidgetClass, Widget, YShade);
extern  Widget   YCreateCommand        (String, Widget);
extern  Widget   YCreateMenuButton     (String, Widget);
extern  Widget   YCreateRepeater       (String, Widget);
extern  Widget   YCreateArrowButton    (String, Widget, YArrowDirection);
extern  Widget   YCreateToggle         (String, Widget, XtCallbackProc);
extern  Boolean  YGetToggleValue       (Widget);
extern  void     YSetToggleValue       (Widget, Boolean);
extern  void     YLightenWidget        (Widget);
extern  void     YDarkenWidget         (Widget);

extern  Widget   YCreateTextEntryField (Widget, String, String);
extern  void     YGetTextEntryWidths(Widget, Dimension, Dimension);
extern  void     YSetTextEntryWidths(Widget, Dimension, Dimension);


extern  int      YQuery                (Widget, String,
					Cardinal, int, int, ...);
extern  String   YGetUserInput         (Widget, String,
					String, YOrientation, String);
extern  void     YAssertDialogueActions(Widget, Widget, Widget, Widget);
extern  void     YRetractDialogueActions(Widget);
extern  void     YAssertShellDismissButton(Widget, Widget);
extern  void     YRetractShellDismissButton(Widget);



extern  void     YMessageInitialise    (Pixmap, String);
extern  Boolean  YMessageIsInitialised (void);
extern  Pixmap   YGetMessagePixmap     (YMessageString *, int,
					Dimension, Dimension);
extern  void     YMessage              (Widget, String, String,
					YMessageString *, int);



extern  void     YMenuInitialise       (Widget, String);
extern  YMenuId  YCreateMenu           (Widget, String, int, YMenuElement *);
extern  void     YEnterMenuMode        (YMenuId, unsigned long);
extern  void     YLeaveMenuMode        (YMenuId, unsigned long);
extern  Boolean  YQueryMenuMode        (YMenuId, unsigned long);
extern  Widget   YGetMenuButtonFromMenu(YMenuId);
extern  void     YDestroyMenu          (YMenuId);
extern  Widget   YCreateToolbar        (Widget); /* parent */
extern  void     YDestroyToolbar       (Widget); /* toolbar */

#define YToolbarBitmapSize 16	/* bitmaps must be 16x16 */

extern  Widget   YCreateOptionMenu(Widget, String *, int, int,
				   XtCallbackProc, XtPointer);
       /* parent, opts, count, default, optional change-cb & client data;
	  returns button */

extern  int      YGetCurrentOption(Widget);            /* menu button */
extern  void     YSetCurrentOption(Widget, int);       /* menu button, option */
extern  void     YDestroyOptionMenu(Widget);	       /* menu button */
extern  void     YFixOptionMenuLabel(Widget); /* button -- you must call this */



extern  void     YShouldWarpPointer    (Boolean);
extern  XPoint   YPushPointerPosition  (void);
extern  XPoint   YPopPointerPosition   (void);
extern  XPoint   YPlaceAndPopup        (Widget, XtGrabKind, Widget);
extern  XPoint   YPlacePopupAndWarp    (Widget, XtGrabKind, Widget, Widget);
extern  void     YPopdown              (Widget);



extern  void     YFileInitialise       (String, Boolean, Boolean,
					String, String, String);
extern  Boolean  YFileIsInitialised    (void);

extern  String   YFileGetReadFilename  (Widget, String, String, String);
extern  String   YFileGetWriteFilename (Widget, String, String, String);
extern  String   YFileGetAppendFilename(Widget, String, String, String);
extern  FILE    *YFileGetReadFile      (Widget, String, String, String);
extern  FILE    *YFileGetWriteFile     (Widget, String, String, String);
extern  FILE    *YFileGetAppendFile    (Widget, String, String, String);
extern  String   YFileGetLastFilename  (Boolean);

extern  Boolean  YFileGetFileInformation(Widget, String, String,
					 String, String, String *,
					 Boolean, Boolean, Boolean,
					 void (*)(String, FILE *),
					 String, void (*)(Widget, XtPointer,
							  XtPointer),
					 String, String);



#define YCreateWidget(A,B,C) YCreateShadedWidget((A),(B),(C),NoShade)

#define YSetValue(A,B,C) do{XtSetArg(_Y_Arg,(B),(XtArgVal)(C)); \
            XtSetValues((A),&_Y_Arg,1);}while(0)

#define YGetValue(A,B,C) do{XtSetArg(_Y_Arg,(B),(XtArgVal)(C)), \
            XtGetValues((A),&_Y_Arg,1);}while(0)

#define YCreateSurroundedWidget(A,B,C,D,E) (_Y_Wdg= \
            _YCreateSurroundingBox((A),(C),(D)), _Y_Wdg= \
	    YCreateShadedWidget((A),(B),_Y_Wdg,(E)))

#define YCreateLabel(A,B) (_Y_Wdg= \
            _YCreateSurroundingBox((A),(B),SurroundShade), \
	    _Y_Wdg=YCreateWidget((A),labelWidgetClass,_Y_Wdg))

#define YMenuDivider   {NULL,0L,NULL,NULL,NULL}

#define YMessageNormal     False,False
#define YMessageBold       True,False
#define YMessageItalic     False,True
#define YMessageBoldItalic True,True

/* debatable! */
extern Pixmap yToggleOnMap;
extern Pixmap yToggleOffMap;


/* private: */
extern Widget _YCreateSurroundingBox(String, Widget, YShade);


#endif /* _Y_WIDGET_STUFF_ */

