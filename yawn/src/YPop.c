
/* YAWN callback/action management for */
/* popup dialogue boxes, C Cannam 3/95 */

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Yawn.h>
#include <SysDeps.h>
#include <Lists.h>
#include <Debug.h>

#include "YPop.h"

Boolean _yShouldWarp;		/* should be set by YInitialise */

void _YDialogueDefaultAction (Widget, XEvent *, String *, Cardinal *);
void _YDialogueCancelAction  (Widget, XEvent *, String *, Cardinal *);
void _YDialogueHelpAction    (Widget, XEvent *, String *, Cardinal *);


/* Some code to associate a Shell with buttons of some sort.  This    */
/* will be used later to implement popdowns; when the Shell's window- */
/* manager popdown action is called, the cancel button's callbacks    */
/* will be called by it; similarly the cancel and help keyboard       */
/* accelerators.                                                      */

typedef enum {
    _yCBAdd,
    _yCBRemove,
    _yCBQuery
} _YCBRequest;

typedef enum {
    _yApplyCB,
    _yCancelCB,
    _yHelpCB
} _YCBType;

/* Make a general structure for associating shells with buttons */

typedef struct __yCBButtonStruct {
    Widget   shell;
    Widget   button;
    _YCBType type;
} _YCBButtonStruct;

typedef struct __yCBListElement {
    ListElement        generic;
    _YCBButtonStruct   item;
} _YCBListElement, *_YCBList;

_YCBList _YNewCBList(_YCBButtonStruct s)
{
    _YCBList list;
    Begin("_YNewCBList");

    list = (_YCBList)NewList(sizeof(_YCBListElement));
    list->item = s;

    Return(list);
}


static void _YHandlePopupCallback(Widget shell, _YCBType type,
				  _YCBRequest request, Widget *button)
{
    _YCBList         listItr;
    _YCBButtonStruct newElt;
    static _YCBList cbList = 0;

    Begin("_YHandlePopupCallback");

    switch (request) {

      case _yCBAdd:

	_YHandlePopupCallback(shell, type, _yCBRemove, NULL);

	newElt.type  = type;
	newElt.shell = shell;
	if (button) newElt.button = *button;
	else        newElt.button = NULL;

	cbList = (_YCBList)Nconc(cbList, _YNewCBList(newElt));
	break;

      case _yCBRemove:
	
	for (listItr = (_YCBList)First(cbList);
	     listItr; listItr = (_YCBList)Next(listItr)) {
	    if (listItr->item.type == type &&
		listItr->item.shell == shell) {
		cbList = (_YCBList)Remove(listItr);
		break;
	    }
	}
	break;

      case _yCBQuery:
	
	if (button) {
	    *button = NULL;
	    for (listItr = (_YCBList)First(cbList);
		 listItr; listItr = (_YCBList)Next(listItr)) {
		if (listItr->item.type == type &&
		    listItr->item.shell == shell) {
		    *button = listItr->item.button;
		    break;
		}
	    }
	}
	break;
    }

    End;
}


static void _YDismissAction(Widget shell, XEvent *e, String *s, Cardinal *c)
{
    Widget button;

    _YHandlePopupCallback(shell, _yCancelCB, _yCBQuery, &button);

    /* Notice we don't want to warp the pointer back if we're popping */
    /* down a window because the user's called the wm Close function  */

    if (button) {
	YPushPointerPosition();
	XtCallCallbacks(button, XtNcallback, 0);
	YRetractShellDismissButton(shell);
    } else {
	YPopdown(shell);
    }
}


/* Handy positioning function */

XPoint YPlacePopupAndWarp(Widget shell, XtGrabKind grab,
			  Widget warpTo, Widget dismissButton)
{
  XPoint    op;
  Position  px;
  Position  py;
  Dimension wd;
  Dimension ht;
  Position  cx;
  Position  cy;
  Widget    w;

  Begin("YPlacePopupAndWarp");

  op = YPushPointerPosition();
  XtRealizeWidget(shell);

  YGetValue(shell, XtNwidth,  &wd);
  YGetValue(shell, XtNheight, &ht);

  if (warpTo) {
      cx = cy = 0;
      for (w = warpTo; w && w != shell; w = XtParent(w)) {
	  YGetValue(w, XtNx, &px);
	  YGetValue(w, XtNy, &py);
	  cx += px; cy += py;
      }
      cx += 15;
      cy += 15;
  } else {
      cx = cy = 30;
  }

  if (op.x || op.y) {

    XtTranslateCoords(XtParent(shell), (Position)0, (Position)0, &px, &py);

    if ((op.x - cx) > px - wd) px = op.x - cx;
    else                       px = px - wd;
    if ((op.y - cy) > py - ht) py = op.y - cy;
    else                       py = py - ht;

    if (px >  WidthOfScreen(XtScreen(shell)) - wd - 50)
        px =  WidthOfScreen(XtScreen(shell)) - wd - 50;
    if (py > HeightOfScreen(XtScreen(shell)) - ht - 70)
        py = HeightOfScreen(XtScreen(shell)) - ht - 70;
    if (px < 0) px = 0;
    if (py < 0) py = 0;

    YSetValue(shell, XtNx, px);
    YSetValue(shell, XtNy, py);
  }

  for (w = XtParent(shell); w; w = XtParent(w)) {
    if (XtIsShell(w)) {
      XtMapWidget(w);
      break;
    }
  }

  XtPopup(shell, grab);

  if (warpTo && (op.x || op.y) && _yShouldWarp) {
      XWarpPointer(XtDisplay(warpTo), 0, XtWindow(warpTo), 0, 0, 0, 0, 15, 15);
  }

 /* else {
      (void)YPopPointerPosition();   NO! Client app will probably do this
  }*/

  YAssertShellDismissButton(shell, dismissButton);

  Return(op);
}


XPoint YPlaceAndPopup(Widget w, XtGrabKind g, Widget dismissButton)
{
    Begin("YPlaceAndPopup");
    Return(YPlacePopupAndWarp(w, g, NULL, dismissButton));
}


void YPopdown(Widget shell)
{
    Begin("YPopdown");

    YRetractShellDismissButton(shell);
    XtPopdown(shell);

    YPopPointerPosition();	/* the client may well do this too, but */
                                /* that shouldn't really matter         */
    End;
}


/* Queries */


void YAssertDialogueActions(Widget shell, Widget deft,
			    Widget cancel, Widget help)
{
  Begin("YAssertDialogueActions");

  _YHandlePopupCallback(0, _yApplyCB,  _yCBAdd, &deft);
  _YHandlePopupCallback(0, _yCancelCB, _yCBAdd, &cancel);
  _YHandlePopupCallback(0, _yHelpCB,   _yCBAdd, &help);

  YAssertShellDismissButton(shell, cancel);

  End;
}


void YRetractDialogueActions(Widget shell)
{
  Begin("YRetractDialogueActions");

  _YHandlePopupCallback(0, _yApplyCB,  _yCBRemove, NULL);
  _YHandlePopupCallback(0, _yCancelCB, _yCBRemove, NULL);
  _YHandlePopupCallback(0, _yHelpCB,   _yCBRemove, NULL);

  YRetractShellDismissButton(shell);

  End;
}


void YAssertShellDismissButton(Widget shell, Widget button)
{
  static Atom wmProtocols[1];
  static Boolean protocolsInterned = False;
  static XtActionsRec actions[] = {
    { "yawn-popdown-action", _YDismissAction },
  };

  Begin("YAssertShellDismissButton");

  if (!protocolsInterned) {
      wmProtocols[0] = XInternAtom(XtDisplay(shell),"WM_DELETE_WINDOW", False);
      XtAppAddActions(XtWidgetToApplicationContext(shell),
		      actions, XtNumber(actions));
  }

  XtOverrideTranslations
    (shell,
     XtParseTranslationTable("<Message>WM_PROTOCOLS: yawn-popdown-action()"));
  XSetWMProtocols(XtDisplay(shell), XtWindow(shell), wmProtocols, 1);

  _YHandlePopupCallback(shell, _yCancelCB, _yCBAdd, &button);
}


void YRetractShellDismissButton(Widget shell)
{
    _YHandlePopupCallback(shell, _yCancelCB, _yCBRemove, NULL);
}


void _YDialogueDefaultAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
    Widget button;
    Begin("YDialogueDefaultAction");

    _YHandlePopupCallback(0, _yApplyCB, _yCBQuery, &button);
    if (button) XtCallCallbacks(button, XtNcallback, NULL);

    End;
}


void _YDialogueCancelAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
    Widget button;
    Begin("YDialogueCancelAction");

    _YHandlePopupCallback(0, _yCancelCB, _yCBQuery, &button);
    if (button) XtCallCallbacks(button, XtNcallback, NULL);
    
    End;
}


void _YDialogueHelpAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
    Widget button;
    Begin("YDialogueHelpAction");

    _YHandlePopupCallback(0, _yHelpCB, _yCBQuery, &button);
    if (button) XtCallCallbacks(button, XtNcallback, NULL);

    End;
}

