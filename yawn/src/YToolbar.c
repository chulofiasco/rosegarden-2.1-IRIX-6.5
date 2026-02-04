
/* YAWN toolbar code  */
/* Chris Cannam, 1996 */

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif

#include <SysDeps.h>

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>

#include <Debug.h>

#include "YToolbar.h"
#include "YawnP.h"

extern GC _yDrawingGC;
extern Boolean _yIsXaw3d;

typedef struct _yToolbarButtonRec {
  String         label;
  Widget         button;
  Widget         menu_button;
  Pixmap         map;
  Pixmap         insensitive_map;
  unsigned long  insensitive_mode_mask;
} yToolbarButtonRec;

typedef struct _yToolbarRec {
  Widget shell;
  Widget toolbar;
  yToolbarButtonRec *buttons;
  int button_count;
} yToolbarRec;

static yToolbarRec *toolbarRecs;
static int toolbarRecCount = 0;
static int floatingCaptionDelay = 900; /* this literal appears elsewhere too */

static Window _yCaptionWindow = 0;
static XtIntervalId _yCaptionTimeout = 0;
static String _yCaption = 0;

void _YCaptionToolbarButton(Widget, XtPointer, XEvent *, Boolean *);
void _YStopCaptioningToolbarButton(Widget, XtPointer, XEvent *, Boolean *);


/*
void _YCaptionToolbarWindow(Widget toolbar, Position x, Position y)
{
  int i, tbNo;
  Position bx, by;
  Dimension wd, ht;
  Begin("_YCaptionToolbarWindow");

  for (i = 0; i < toolbarRecCount; ++i) {
    if (toolbarRecs[i].toolbar == toolbar) break;
  }

  if (i >= toolbarRecCount) End;
  tbNo = i;

  for (i = 0; i < toolbarRecs[tbNo].button_count; ++i) {

    YGetValue(XtParent(toolbarRecs[tbNo].buttons[i].button), XtNx, &bx);
    YGetValue(XtParent(toolbarRecs[tbNo].buttons[i].button), XtNy, &by);

    if (x >= bx && y >= by) {

      YGetValue(XtParent(toolbarRecs[tbNo].buttons[i].button), XtNwidth,  &wd);
      YGetValue(XtParent(toolbarRecs[tbNo].buttons[i].button), XtNheight, &ht);

      if (x <= bx + wd && y <= by + ht) {

	_YCaptionToolbarButton(toolbarRecs[tbNo].buttons[i].button,
			       (XtPointer)toolbarRecs[tbNo].buttons[i].label,
			       NULL, NULL);
	End;
      }
    }
  }

  End;
}
*/

void _YEnterToolbarHandler(Widget w, XtPointer c, XEvent *e, Boolean *b)
{
  Begin("_YEnterToolbarHandler");

/*
  _YStopCaptioningToolbarButton(w, 0, 0, 0);
*/

  End;
}

void _YLeaveToolbarHandler(Widget w, XtPointer c, XEvent *e, Boolean *b)
{
  Dimension wd, ht;
  Begin("_YLeaveToolbarHandler");

  YGetValue(w, XtNwidth, &wd);
  YGetValue(w, XtNheight, &ht);

  /* if we've left the toolbar window to somewhere other than just one
     of the toolbar buttons, we reset the delay */

  if (e->xcrossing.x <= 0 || e->xcrossing.x >= wd ||
      e->xcrossing.y <= 0 || e->xcrossing.y >= ht) {
    
    floatingCaptionDelay = 900;

  } else {
/*    
    _YCaptionToolbarWindow(w, e->xcrossing.x, e->xcrossing.y);
*/
  }

  End;
}


Widget YCreateToolbar(Widget parent)
{
  int i;
  Widget shell;
  Widget toolbar;
  Begin("YCreateToolbar");

  for (shell = parent; shell && !XtIsShell(shell); shell = XtParent(shell));

  if (!shell) XtAppError
		(XtWidgetToApplicationContext(shell),
		 "YCreateToolbar: Unable to find a shell (don't know why)");

  for (i = 0; i < toolbarRecCount; ++i) {
    if (toolbarRecs[i].shell == shell) {
      fprintf (stderr, "YCreateToolbar: Warning: Attempt to create "
	       "multiple toolbars in shell %p\n", shell);
      Return(0);
    }
  }

  toolbar = YCreateShadedWidget("Toolbar", formWidgetClass, parent, LightShade);

  XtAddEventHandler(toolbar, EnterWindowMask, False, _YEnterToolbarHandler, 0);
  XtAddEventHandler(toolbar, LeaveWindowMask, False, _YLeaveToolbarHandler, 0);

  if (toolbarRecCount == 0) {
    toolbarRecs = (yToolbarRec *)XtMalloc(sizeof(yToolbarRec));
  } else {
    toolbarRecs = (yToolbarRec *)XtRealloc
      ((char *)toolbarRecs, (toolbarRecCount + 1) * sizeof(yToolbarRec));
  }

  toolbarRecs[toolbarRecCount].shell   = shell;
  toolbarRecs[toolbarRecCount].toolbar = toolbar;
  toolbarRecs[toolbarRecCount].buttons = NULL;
  toolbarRecs[toolbarRecCount].button_count = 0;
  toolbarRecCount++;

  Return(toolbar);
}


void YDestroyToolbar(Widget toolbar)
{
  int i, j;
  Display *display;
  Begin("YDestroyToolbar");

  display = XtDisplay(toolbar);

  for (i = 0; i < toolbarRecCount; ++i) {
    if (toolbarRecs[i].toolbar == toolbar) {

      XtDestroyWidget(toolbar);

      for (j = 0; j < toolbarRecs[i].button_count; ++j) {
	XtFree(toolbarRecs[i].buttons[j].label);
	XFreePixmap(display, toolbarRecs[i].buttons[j].map);
	XFreePixmap(display, toolbarRecs[i].buttons[j].insensitive_map);
      }

      if (i < toolbarRecCount-1) {
	memcpy(&(toolbarRecs[i]), &(toolbarRecs[i+1]),
	       (toolbarRecCount-i-1) * sizeof(yToolbarRec));
      }
    }
  }

  --toolbarRecCount;

  End;
}


void _YStopCaptioningToolbarButton(Widget w, XtPointer c,
				   XEvent *e, Boolean *b)
{
  Begin("_YStopCaptioningToolbarButton");

  if (_yCaptionTimeout) {
    XtRemoveTimeOut(_yCaptionTimeout);
    _yCaptionTimeout = 0;
  }

  if (_yCaptionWindow) {
    XDestroyWindow(XtDisplay(w), _yCaptionWindow);
    _yCaptionWindow = 0;
  }

  if (b) *b = True;

  End;
}


void _YDrawToolbarButtonCaption(XtPointer ww, XtIntervalId *id)
{
  Widget w = (Widget)ww;
  Position x, y, sx, sy;
  XCharStruct info;
  int dir, asc, dsc;
  XGCValues values;
  Widget top;

  Begin("_YDrawToolbarButtonCaption");

  if (!_yCaption) End;
  _YStopCaptioningToolbarButton(w, 0, 0, 0);

  for (top = w; top && XtParent(top) && !XtIsShell(XtParent(top));
       top = XtParent(top));

  if (!top) End;

  XtTranslateCoords(w, 0, 0, &x, &y);
  XtTranslateCoords(top, 0, 0, &sx, &sy);

  x = x - sx;
  y = y - sy + 25;

  XGetGCValues(XtDisplay(w), _yDrawingGC,
	       GCFont | GCForeground | GCBackground, &values);

  XQueryTextExtents(XtDisplay(w), values.font, _yCaption, strlen(_yCaption),
		    &dir, &asc, &dsc, &info);
  
  _yCaptionWindow = XCreateSimpleWindow
    (XtDisplay(w), XtWindow(top), x, y, info.width + 4, asc + dsc + 4, 0,
     values.background, values.background);

  XMapWindow(XtDisplay(w), _yCaptionWindow);
  
  XDrawString(XtDisplay(w), _yCaptionWindow, _yDrawingGC, 2, asc + 2,
	      _yCaption, strlen(_yCaption));

  floatingCaptionDelay = 0;	/* reset to 900 by _YLeaveToolbarHandler */

  End;
}


void _YCaptionToolbarButton(Widget w, XtPointer c, XEvent *e, Boolean *b)
{
  Begin("_YCaptionToolbarButton");

  _yCaption = (String)c;
  _YStopCaptioningToolbarButton(w, 0, 0, 0);

  _yCaptionTimeout = XtAppAddTimeOut
    (XtWidgetToApplicationContext(w), floatingCaptionDelay,
     _YDrawToolbarButtonCaption, (XtPointer)w); 

  if (b) *b = True;

  End;
}


void _YAddToolbarButton(char *label,
			Widget menuButton, unsigned char *bitmap,
			XtCallbackProc callback, XtPointer client_data,
			unsigned long modemask)
{
  int i, size;
  Widget shell;
  Widget button;
  Widget toolbar;
  int tbNo;
  yToolbarButtonRec *thisButtonRec;
  Pixmap map, insensMap;
  Widget prevButton, prevMenuButton;
  char *insensBM;
  Pixel bg;

  Begin("_YAddToolbarButton");

  for (shell = menuButton; !XtIsShell(shell); shell = XtParent(shell));
  for (i = 0; i < toolbarRecCount; ++i) {
    if (toolbarRecs[i].shell == shell) {
      toolbar = toolbarRecs[i].toolbar;
      break;
    }
  }
  
  if (i >= toolbarRecCount) {
    fprintf(stderr, "_YAddToolbarButton: No toolbar declared for shell %p\n",
	    shell);
    End;
  }

  tbNo = i;

  /* if this button already exists, don't make it again! */
  for (i = 0; i < toolbarRecs[tbNo].button_count; ++i) {
    if (toolbarRecs[tbNo].buttons[i].menu_button == menuButton &&
	!strcmp(toolbarRecs[tbNo].buttons[i].label, label)) End;
  }

  if (toolbarRecs[tbNo].button_count == 0) {
    toolbarRecs[tbNo].buttons = (yToolbarButtonRec *)XtMalloc
      (sizeof(yToolbarButtonRec));
  } else {
    toolbarRecs[tbNo].buttons = (yToolbarButtonRec *)XtRealloc
      ((char *)toolbarRecs[tbNo].buttons,
       (toolbarRecs[tbNo].button_count + 1) * sizeof(yToolbarButtonRec));
  }

  prevButton = toolbarRecs[tbNo].button_count > 0 ?
    XtParent(toolbarRecs[tbNo].buttons[toolbarRecs[tbNo].button_count-1].button)
    : 0;

  prevMenuButton = toolbarRecs[tbNo].button_count > 0 ?
    toolbarRecs[tbNo].buttons[toolbarRecs[tbNo].button_count-1].menu_button :0;

  map = XCreateBitmapFromData
    (XtDisplay(shell), RootWindowOfScreen(XtScreen(shell)),
     (char *)bitmap, YToolbarBitmapSize, YToolbarBitmapSize);

  size = YToolbarBitmapSize * YToolbarBitmapSize / 8;
  insensBM = (char *)XtMalloc(size);
  memcpy(insensBM, bitmap, size);

  for (i = 0; i < size; ++i) {
    if ((i / (YToolbarBitmapSize / 8)) % 2 == 0) {
      insensBM[i] &= 0x55;
    } else {
      insensBM[i] &= 0xaa;
    }      
  }
  
  insensMap = XCreateBitmapFromData
    (XtDisplay(shell), RootWindowOfScreen(XtScreen(shell)),
     insensBM, YToolbarBitmapSize, YToolbarBitmapSize);

  XtFree(insensBM);

  button = YCreateShadedActionButton(label, toolbar,
				     0, commandWidgetClass, NoShade);

  /* This is all a bit experimental */

  XtVaGetValues(button, XtNbackground, &bg, NULL);

  XtVaSetValues
    (XtParent(button), XtNfromHoriz, prevButton,
     XtNleft, XawChainLeft, XtNright, XawChainLeft,
     XtNhSpace, 0, XtNvSpace, 0,
     NULL);

  if (prevButton) {
    XtVaSetValues(XtParent(button), XtNhorizDistance,
		  ((prevMenuButton == menuButton) ? 4 : 12) -
		  (_yIsXaw3d ? 4 : 0), NULL);
  }

  XtVaSetValues(button, XtNbitmap, map, XtNborderWidth, 0,
		XtNinternalWidth, 5, XtNinternalHeight, 3, NULL);

  XtAddCallback(button, XtNcallback, callback, client_data);

  thisButtonRec = &toolbarRecs[tbNo].buttons[toolbarRecs[tbNo].button_count];

  thisButtonRec->label = XtNewString(label);
  thisButtonRec->button = button;
  thisButtonRec->menu_button = menuButton;
  thisButtonRec->map = map;
  thisButtonRec->insensitive_map = insensMap;
  thisButtonRec->insensitive_mode_mask = modemask;

  XtAddEventHandler(button, EnterWindowMask, False, _YCaptionToolbarButton,
		    thisButtonRec->label);

  XtAddEventHandler(button, LeaveWindowMask, False,
		    _YStopCaptioningToolbarButton, NULL);

  XtAddEventHandler(button, ButtonPressMask, False,
		    _YStopCaptioningToolbarButton, NULL);

  toolbarRecs[tbNo].button_count++;
  End;
}


void _YToolbarMode(Widget menuButton, unsigned long mode)
{
  int i, j;
  Boolean sens;
  Widget button;
  Widget shell;
  Begin("_YToolbarMode");

  for (shell = menuButton; shell && !XtIsShell(shell); shell = XtParent(shell));
  if (!shell) End;

  for (j = 0; j < toolbarRecCount; ++j) {
    if (shell == toolbarRecs[j].shell) break;
  }

  if (j >= toolbarRecCount) End;

  for (i = 0; i < toolbarRecs[j].button_count; ++i) {

    if (menuButton != toolbarRecs[j].buttons[i].menu_button) continue;

    button = toolbarRecs[j].buttons[i].button;
    sens = !(toolbarRecs[j].buttons[i].insensitive_mode_mask & mode);

    _YStopCaptioningToolbarButton(button, 0, 0, 0);

    if (( sens && !XtIsSensitive(button)) ||
	(!sens &&  XtIsSensitive(button))) {

      YDarkenWidget(button);
      XtSetSensitive(button, sens);

      if (sens) {
	XtVaSetValues(button, XtNbitmap, toolbarRecs[j].buttons[i].map, NULL);
      } else {
	XtVaSetValues(button, XtNbitmap,
		      toolbarRecs[j].buttons[i].insensitive_map, NULL);
      }
    }
  }

  End;
}

