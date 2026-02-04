
/* YAWN modal menu management code */
/* Chris Cannam, 1994              */

/* Accelerator code is now dependent on -DUSE_MENU_ACCELERATORS */

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>

#include <Yawn.h>

/*#include <YSmeBSB.h>*/
#include <X11/Xaw/SmeBSB.h>
#define ysmeBSBObjectClass smeBSBObjectClass

#include <Debug.h>
#include <SysDeps.h>
#include "YToolbar.h"

#include "darkgrey.xbm"

#define FUNCTION_NAME "menu-function"

typedef struct __YMenuAccRec {
  String         keyname;
  String         actionArg;
  YMenuElement  *menuElt;
  Pixmap         sensitiveBitmap;
  Pixmap         insensitiveBitmap;
  Dimension      mapWidth;
} _YMenuAccRec;

typedef struct _YMenuRec {
  unsigned long  mode;
  Widget         menuButton;
  Widget         menuShell;
  int            number;
  _YMenuAccRec **accelerators;
  YMenuElement  *elts;
} YMenuRec;

static Widget        refw;

static _YMenuAccRec *menuAccRec   = NULL;
static int           menuAccCount = 0;
static Pixmap        stippleMap   = 0;
static GC            stippleGC    = 0;

extern GC           _yClearGC;
extern GC           _yDrawingGC;
extern Boolean      _yInitialised;

static void CreateStippleMap(Widget w)
{
  Begin("CreateStippleMap");

  stippleMap = YCreatePixmapFromData(darkgrey_bits,
				     darkgrey_width,
				     darkgrey_height,
				     NoShade);
  End;
}


static String GetKeyName(String s, String keyname)
{
  int i, j;
  int length;

  Begin("GetKeyName");

  while (*s && isspace(*s)) ++s;

  for (i = 0; s[i] && s[i] != '<'; ++i);
  if (!s[i]) Return(NULL);

  if (i > 0) {

    if (!strncmp("Mod1", s, 4)) strncpy(keyname, "Alt", length = 3);
    else                        strncpy(keyname,     s, length = i);

    keyname[length++] = '-';

  } else length = 0;

  if (strncasecmp(s + ++i, "key", 3)) Return(NULL);

  while (s[i] && s[i] != '>') ++i;
  if (!s[i] || !s[i+1]) Return(NULL);

  j = i + 1;

  while (s[i] && s[i] != ':') ++i;
  if (!s[i] || !s[i+1]) Return(NULL);

  strncpy(keyname + length, s + j, i - j);

  keyname[length + i - j] = '\0';

  Return(s + i + 1);
}


static String GetInvocationArgument(String s, String argument)
{
  int i, j;

  Begin("GetInvocationArgument");
  
  for (i = 0; s[i] && isspace(s[i]); ++i);
  if (!s[i] || strncmp(s + i, FUNCTION_NAME, strlen(FUNCTION_NAME)))
    Return(NULL);

  while (s[i] && s[i] != '(') ++i;
  if (!s[i] || !s[i+1]) Return(NULL);

  j = i + 1;

  while (s[i] && s[i] != ')' && !isspace(s[i])) ++i;
  if (!s[i]) Return(NULL);

  strncpy(argument, s + j, i - j);

  for (s += i; *s; ++s);
  Return(++s);
}


static void GetMenuBitmaps(Pixmap *map1, Pixmap *map2,
			   Dimension *width, Widget w, String name)
{
  int                 i;
  int                 asc, desc, dir;
  Display            *display   = XtDisplay(w);
  Drawable            root      = RootWindowOfScreen(XtScreen(w));
  unsigned            depth     = DefaultDepthOfScreen(XtScreen(w));
  static Widget       tempSme   = NULL;
  static XFontStruct *font      = NULL;
  XGCValues           values;
  XCharStruct         charInfo;
  Widget              ww;

  Begin("GetMenuBitmaps");

  if (!map1 && !map2) {		/* signal to clean up */
    if (stippleGC) {

      XFreeGC(display, stippleGC);
      if (stippleMap) XFreePixmap(display, stippleMap);

      stippleGC  = 0;
      stippleMap = 0;
    }
    End;
  }

  if (!stippleGC || !font || !tempSme) {

    CreateStippleMap(w);

    /* this is a hack, attempting to discover what font is set for    */
    /* objects under SimpleMenu in the resources.  It may fail badly. */

    for (ww = w; XtParent(ww); ww = XtParent(ww)) if (XtIsComposite(ww)) break;
    if (!XtParent(ww)) ww = w;	/* that one will probably fail */

    tempSme = XtCreateWidget("SimpleMenu", labelWidgetClass, ww, NULL, 0);
    YGetValue(tempSme, XtNfont, &font);
    XtDestroyWidget(tempSme);

    values.font = font->fid;
    XChangeGC(display, _yDrawingGC, GCFont, &values);

    XGetGCValues(display, _yDrawingGC, GCFunction | GCPlaneMask |
		 GCForeground | GCBackground | GCFont, &values);

    values.fill_style = FillTiled;
    values.tile       = stippleMap;

    stippleGC =
      XCreateGC(display, root, GCFunction | GCPlaneMask | GCForeground |
		GCBackground | GCFont | GCFillStyle | GCTile, &values);
  }

  /* purify complains of "name" uninitialized -- I think it's at least
     partly right but I haven't got around to investigating it yet */

  XTextExtents(font, name, strlen(name), &dir, &asc, &desc, &charInfo);

  /* if there's a dash and single-character keyname after */
  /* it, try to line up the dash with any other like it   */

  for (i = 0; name[i] && name[i] != '-'; ++i);
  *width = charInfo.width + 26;

  if (name[i] == '-' && name[i+1] && !name[i+2])
    *width += XTextWidth(font, "W", 1) - XTextWidth(font, name + i + 1, 1) - 1;
  else *width += 3;

  *map1  = XCreatePixmap(display, root, *width, asc + desc + 4, depth);
  *map2  = XCreatePixmap(display, root, *width, asc + desc + 4, depth);

  XFillRectangle(display, *map1, _yClearGC, 0, 0, *width, asc + desc + 4);
  XDrawString   (display, *map1, _yDrawingGC, 23, asc + 2, name, strlen(name));

  XFillRectangle(display, *map2, _yClearGC, 0, 0, *width, asc + desc + 4);
  XDrawString   (display, *map2,   stippleGC, 23, asc + 2, name, strlen(name));

  End;  
}


static void AddAcceleratorDisplay(Widget menuButton, _YMenuAccRec **acc,
				  Widget gadget, String label,
				  YMenuElement *elt, String name)
{
#ifdef USE_MENU_ACCELERATORS
  int         i;
  static char tempName[100];
#endif

  Begin("AddAcceleratorDisplay");

#ifdef USE_MENU_ACCELERATORS
  strcpy(tempName, name);
  strcpy(tempName + (i = 1 + strlen(name)), label);
  tempName[i-1] = '-';

  for (i += strlen(label); i > 0; --i) {
    if (tempName[i-1] == ' ') tempName[i-1] = '-';
    else if (tempName[i-1] == '.') tempName[i-1] = '\0';
  }

  for (i = 0; i < menuAccCount; ++i)
    if (!strcmp(tempName, menuAccRec[i].actionArg)) break;

  if (i >= menuAccCount) End;

  /* Record in the menu's accelerator table list, for the relevant */
  /* menu entry, a pointer to the found accelerator table entry.   */

  *acc = &menuAccRec[i];

  menuAccRec[i].menuElt = elt;

  if (elt->widget) {
    YSetValue(elt->widget, XtNrightBitmap, menuAccRec[i].sensitiveBitmap);
    YSetValue(elt->widget, XtNrightMargin, menuAccRec[i].mapWidth);
  }
#endif

  End;
}


/* The advantage of not commenting code is that you can
   still remove whole functions at a time *like this*.

static Widget GetWidgetUnderPointer(Widget w)
{
  Widget   ww;
  Window   root, child;
  int      rxr, ryr, wxr, wyr;
  unsigned mask;
  Widget   shell;
  XWindowAttributes attr;

  Begin("GetWidgetUnderPointer");

  for (shell = w; XtParent(shell) && !XtIsSubclass(shell, shellWidgetClass);
       shell = XtParent(shell));

  if (!XQueryPointer(XtDisplay(w), XtWindow(shell),
		     &root, &child, &rxr, &ryr, &wxr, &wyr, &mask))
    Return(NULL);

  fprintf(stderr,"Root window: %p\nChild window: %p\nChild coords: %d,%d\n",
	  root, child, wxr, wyr);

  XGetWindowAttributes(XtDisplay(w), child, &attr);

  fprintf(stderr,"Child size: %d,%d\n", attr.width, attr.height);

  XFillRectangle(XtDisplay(w), child,
		 _yDrawingGC, 0, 0, attr.width, attr.height);

  ww = XtWindowToWidget(XtDisplay(w), child);

  fprintf(stderr,"Widget: %p\n", ww);

  Return(ww);
}

*/


static void _YMenuFunctionAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
  int    i;
  Widget shell;

  Begin("_YMenuFunctionAction");

  if (e->type != KeyPress || c == 0 || !s || !s[0]) End;

  for (i = 0; i < menuAccCount; ++i)
    if (!strcmp(s[0], menuAccRec[i].actionArg)) break;

  if (i >= menuAccCount) End;

  if (menuAccRec[i].menuElt && menuAccRec[i].menuElt->callback &&
      XtIsSensitive(menuAccRec[i].menuElt->widget)) {

    for (shell = w; XtParent(shell) && !XtIsSubclass(shell, shellWidgetClass);
	 shell = XtParent(shell));

    menuAccRec[i].menuElt->callback(menuAccRec[i].menuElt->widget,
				    (XtPointer)menuAccRec[i].menuElt,
				    (XtPointer)shell);
  }

  End;
}
  

void YMenuInitialise(Widget w, String accTable)	/* accs end up attached to w */
{
  int                 fnamelength = strlen(FUNCTION_NAME);
  int                 length = fnamelength + 1;
  int                 i, j, k;
  String              nextBit;
  XtAccelerators      parsedAccs;
  static XtActionsRec menuActions = { FUNCTION_NAME, _YMenuFunctionAction } ;

  Begin("YMenuInitialise");

  if (!_yInitialised)
    XtAppError(XtWidgetToApplicationContext(w),
	       "YMenuInitialise called before YInitialise");

  refw = w;

  if (!accTable || !(accTable[0])) { menuAccCount = 0; End; }

  parsedAccs = XtParseAcceleratorTable(accTable);
  if (!parsedAccs) End;

#ifdef USE_MENU_ACCELERATORS
  YSetValue(w, XtNaccelerators, parsedAccs);
#endif

  for (i = 0, menuAccCount = 1; accTable[i]; ++ i)
    if (accTable[i] == '\n') { ++ menuAccCount; accTable[i] = '\0'; }

  if (menuAccRec) XtFree((void *)menuAccRec);
  menuAccRec = (_YMenuAccRec *)XtMalloc(menuAccCount * sizeof(_YMenuAccRec));

  /* Parse the accelerator table (minimally): */

  for (j = menuAccCount, k = 0; j > 0; --j) {
    /* (perform j=menuAccCount at start, cause it could change during loop) */

    menuAccRec[k].keyname   = (String)XtMalloc(20);
    menuAccRec[k].menuElt   = NULL;
    menuAccRec[k].actionArg = NULL;

    /* Errors: GetKeyName fails, or the function name length is */
    /* too short to contain the right name                      */

    if (!(nextBit = GetKeyName(accTable, menuAccRec[k].keyname)) ||
	((length  = strlen(nextBit)) <= fnamelength)             ||
	!(nextBit = GetInvocationArgument(nextBit,
					  menuAccRec[k].actionArg =
					  (String)XtMalloc
					  (length - fnamelength)))) {

      /* Commented out for the moment, so we don't get warnings for
	 accelerators that aren't used anyway --cc,1/96
 
      if (length <= fnamelength)
	fprintf(stderr, "Unknown action in \"%s\", ignoring\n", accTable);
      else
	fprintf(stderr, "Malformed accelerator \"%s\", ignoring\n", accTable);
      */

      XtFree(menuAccRec[k].keyname);
      if    (menuAccRec[k].actionArg) XtFree(menuAccRec[k].actionArg);

      -- menuAccCount;
      while (*accTable) ++accTable;
      ++ accTable;
   
      continue;
    }

    GetMenuBitmaps(&menuAccRec[k].sensitiveBitmap,
		   &menuAccRec[k].insensitiveBitmap,
		   &menuAccRec[k].mapWidth, w, menuAccRec[k].keyname);

    menuAccRec[k].menuElt = NULL;
    accTable = nextBit;
    ++k;
  }

#ifdef USE_MENU_ACCELERATORS
  XtAppAddActions(XtWidgetToApplicationContext(w), &menuActions, 1);
#else
  (void)menuActions;		/* just to avoid compiler warnings */
#endif

  End;
}


void _YMenuCleanUp(void)
{
  int i;
  
  Begin("_YMenuCleanUp");

  if (refw) GetMenuBitmaps(NULL, NULL, NULL, refw, NULL);
  if (menuAccCount == 0) End;

  /* Assume the menu records have already gone (that's the app's problem) */

  for (i = 0; i < menuAccCount; ++i) {

    if (menuAccRec[i].keyname)   XtFree(menuAccRec[i].keyname);
    if (menuAccRec[i].actionArg) XtFree(menuAccRec[i].actionArg);

    if (menuAccRec[i].sensitiveBitmap)
      XFreePixmap(XtDisplay(refw), menuAccRec[i].sensitiveBitmap);

    if (menuAccRec[i].insensitiveBitmap)
      XFreePixmap(XtDisplay(refw), menuAccRec[i].insensitiveBitmap);
  }

  XtFree((void *)menuAccRec);
  menuAccCount = 0;
}


YMenuId YCreateMenu(Widget menuButton, String name,
		    int number, YMenuElement *elts)
{
  int       x;
  Arg       arg;
  YMenuRec *rec;

  Begin("YCreateMenu");

  if (!_yInitialised)
    XtAppError(XtWidgetToApplicationContext(menuButton),
	       "YCreateMenu called before YInitialise");

  rec = (YMenuRec *)XtMalloc(sizeof(YMenuRec));
  
  XtSetArg(arg, XtNmenuName, name);
  XtSetValues(menuButton, &arg, 1);

  rec->mode         = 0L;
  rec->elts         = elts;
  rec->number       = number;
  rec->accelerators = (_YMenuAccRec **)XtCalloc(number, sizeof(_YMenuAccRec*));
  rec->menuButton   = menuButton;
  rec->menuShell    = XtCreatePopupShell(name, simpleMenuWidgetClass,
					 XtParent(menuButton), &arg, 1);

  (void)XtCreateManagedWidget
    ("Line", smeLineObjectClass, rec->menuShell, NULL, 0);
  (void)XtCreateManagedWidget
    ("Line", smeLineObjectClass, rec->menuShell, NULL, 0);
  
  for (x = 0; x < number; ++x) {
    
    if (elts[x].label) {

      if (!elts[x].widget) elts[x].widget =
	XtCreateManagedWidget
	  (elts[x].label, ysmeBSBObjectClass, rec->menuShell, NULL, 0);

      XtAddCallback(elts[x].widget, XtNcallback,
		    elts[x].callback, (XtPointer)&(elts[x]));

      AddAcceleratorDisplay(menuButton, &rec->accelerators[x],
			    elts[x].widget, elts[x].label, &elts[x], name);

      if (elts[x].toolbar_bitmap) {
	_YAddToolbarButton(elts[x].label, menuButton, elts[x].toolbar_bitmap,
			   elts[x].callback, (XtPointer)&(elts[x]),
			   elts[x].insensitive_mode_mask);
      }

    } else {

      (void)XtCreateManagedWidget
	("Line", smeLineObjectClass, rec->menuShell, NULL, 0);
      (void)XtCreateManagedWidget
	("Line", smeLineObjectClass, rec->menuShell, NULL, 0);
    }
  }

  (void)XtCreateManagedWidget
    ("Line", smeLineObjectClass, rec->menuShell, NULL, 0);

  Return((YMenuId)rec);
}


void YDestroyMenu(YMenuId id)
{
  int       i;
  YMenuRec *rec = (YMenuRec *)id;

  Begin("YDestroyMenu");

  if (rec->accelerators) {

    for (i = 0; i < rec->number; ++i)
      if (rec->accelerators[i]) {
	rec->accelerators[i]->menuElt = NULL;
	rec->accelerators[i] = NULL;
      }

    XtFree((void *)rec->accelerators);
    rec->accelerators = NULL;
  }

  if (rec->menuShell) XtDestroyWidget(rec->menuShell);
  for (i = 0; i < rec->number; ++i) rec->elts[i].widget = NULL;
  XtFree(id);

  End;
}



Widget YGetMenuButtonFromMenu(YMenuId id)
{
  Begin("YGetMenuButtonFromMenu");
  Return(((YMenuRec *)id)->menuButton);
}



void YEnterMenuMode(YMenuId id, unsigned long mask)
{
  int        x;
  static Arg arg = { XtNrightBitmap, 0 };
  YMenuRec  *rec = (YMenuRec *)id;
  Boolean    sens;

  Begin("YEnterMenuMode");

  rec->mode |= mask;

  for (x = 0; x < rec->number; ++x)
    if (rec->elts[x].label) {

      sens = !(rec->elts[x].insensitive_mode_mask & rec->mode);
      XtSetSensitive(rec->elts[x].widget, sens);

      if (rec->accelerators[x]) {
	arg.value = sens ? rec->accelerators[x]->  sensitiveBitmap :
                           rec->accelerators[x]->insensitiveBitmap ;

#ifdef USE_MENU_ACCELERATORS
	XtSetValues(rec->elts[x].widget, &arg, 1);
#endif
      }
    }

  _YToolbarMode(rec->menuButton, rec->mode);

  End;
}


void YLeaveMenuMode(YMenuId id, unsigned long mask)
{
  int        x;
  static Arg arg = { XtNrightBitmap, 0 };
  YMenuRec  *rec = (YMenuRec *)id;
  Boolean    sens;

  Begin("YLeaveMenuMode");

  rec->mode &= ~mask;

  for (x = 0; x < rec->number; ++x)
    if (rec->elts[x].label) {

      sens = !(rec->elts[x].insensitive_mode_mask & rec->mode);
      XtSetSensitive(rec->elts[x].widget, sens);

      if (rec->accelerators[x]) {
	arg.value = sens ? rec->accelerators[x]->  sensitiveBitmap :
                           rec->accelerators[x]->insensitiveBitmap ;

#ifdef USE_MENU_ACCELERATORS
	XtSetValues(rec->elts[x].widget, &arg, 1);
#endif
      }
    }

  _YToolbarMode(rec->menuButton, rec->mode);

  End;
}


Boolean YQueryMenuMode(YMenuId id, unsigned long mask)
{
  YMenuRec *rec = (YMenuRec *)id;

  Begin("YQueryMenuMode");
  Return((rec->mode & mask) ? True : False);
}

