
/*
   Musical Notation Editor, application widget creation stuff
   Chris Cannam
*/

/* {{{ Includes */

#include "General.h"
#include "GC.h"
#include "Classes.h"
#include "Visuals.h"
#include "Menu.h"
#include "Yawn.h"
#include "ILClient.h"
#include "Stave.h"
#include "Palette.h"

#include <Version.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Repeater.h>
#include <X11/Xaw/Scrollbar.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "rewind.xbm"
#include "back.xbm"
#include "forward.xbm"
#include "ffwd.xbm"

/* }}} */
/* {{{ Basic declarations */

static Widget helpButton;

Pixmap  leftMap = 0;
Pixmap rightMap = 0;

Widget paletteShell  = NULL;
Widget musicViewport = NULL;
Widget pageButton    = NULL;

YMessageString aboutText[] = {
  { "Rosegarden",                    YMessageBold,       },
  { " ",                             YMessageNormal,     },
  { "Musical Notation Editor",       YMessageNormal,     },
  { " ",                             YMessageNormal,     },
  { ROSEGARDEN_VERSION,              YMessageNormal,     },
  { "Chris Cannam",                  YMessageBold,       },
  { " ",                             YMessageNormal,     },
  { "IRIX 6.5 fixes: @chulofiasco, SGUG",  YMessageBold,       },
  { "with thanks to ajg, jpff and others", YMessageItalic, },
};

/* }}} */
/* {{{ Help callbacks */

void HelpFinished(IL_ReturnCode rtn)
{
  Begin("HelpFinished");
  XtSetSensitive(helpButton, True);
  End;
}

void HelpButton(Widget w, XtPointer a, XtPointer b)
{
  Begin("HelpButton");
  IL_RequestService(ILS_HELP_SERVICE, HelpFinished, "Editor", 8);
  XtSetSensitive(helpButton, False);
  End;
}

void yHelpCallback(String helpTag)
{
  Begin("yHelpCallback");

  IL_RequestService(ILS_HELP_SERVICE, HelpFinished,
		    helpTag, strlen(helpTag) + 1);

  End;
}

void yHelpCallbackCallback(Widget w, XtPointer a, XtPointer b)
{
  String helpTag = (String)a;

  Begin("yHelpCallbackCallback");

  IL_RequestService(ILS_HELP_SERVICE, HelpFinished,
		    helpTag, strlen(helpTag) + 1);

  End;
}

/* }}} */
/* {{{ About button */

void AboutButton(Widget w, XtPointer a, XtPointer b)
{
  Begin("AboutButton");
  YMessage(XtParent(w), "About Rosegarden",
	    "Enough!", aboutText, XtNumber(aboutText));
  End;
}

/* }}} */
/* {{{ Main widget-creation function */

void CreateApplicationWidgets(void)
{
  Widget    outerPane;
  Widget    toptopBox;
  Widget    topBox;
  Widget    toolbar;
  Widget    helpBox;
  Widget    bottomBox;
  Widget    aboutButton;
  Widget    fileButton;
  Widget    editButton;
  Widget    chordButton;
  Widget    groupButton;
  Widget    textButton;
  Widget    barButton;
  Widget    staveButton;
  Widget    markButton;
  Widget    filterButton;
  Widget    rewindButton;
  Widget    backButton;
  Widget    forwardButton;
  Widget    ffwdButton;
  Widget    palettePane;
  Widget    paletteTopPane;
  Widget    paletteFollowBox;
  Widget    paletteTopBox;
  Widget    paletteSpace;
  Widget    scrollbar;
  Widget    tempButton;
  Window    w;
  char      paletteName[50];
  Pixmap    pixmap;
  Dimension h, hh;

  Begin("CreateApplicationWidgets");

  if (appData.interlockWindow) YInitialise(topLevel, yHelpCallback);
  else                         YInitialise(topLevel, NULL);

  YShouldWarpPointer(appData.shouldWarpPointer);

  YFileInitialise(appData.musicDirectory,
		  True, True, "Open", "Save", "Append");
  InitialiseVisuals();
  CreateGCs();

  YMessageInitialise(roseMap, appData.aboutTextFont);
  w = RootWindowOfScreen(XtScreen(topLevel));

  sprintf(paletteName, "Palette");
  paletteShell =
    XtAppCreateShell(paletteName, ProgramName,
		     applicationShellWidgetClass, display,  NULL, 0);

  outerPane     = YCreateShadedWidget
    ("Editor",        panedWidgetClass,  topLevel,     NoShade);
  toptopBox     = YCreateShadedWidget
    ("Top Top Box",    formWidgetClass, outerPane, MediumShade);
  topBox        = YCreateShadedWidget
    ("Top Box",         boxWidgetClass, toptopBox, MediumShade);
  helpBox       = YCreateShadedWidget
    ("Help Box",        boxWidgetClass, toptopBox, MediumShade);

  toolbar = YCreateToolbar(outerPane);

  musicViewport = YCreateShadedWidget
    ("Music View", viewportWidgetClass, outerPane,     NoShade);
  scrollbar     = YCreateShadedWidget
    ("Scrollbar", scrollbarWidgetClass, outerPane,     NoShade);
  bottomBox     = YCreateShadedWidget
    ("Bottom Box",      boxWidgetClass, outerPane, MediumShade);
  
  XtVaSetValues(toptopBox, XtNdefaultDistance, 0, NULL);

  XtVaSetValues(topBox,
		XtNleft,   XawChainLeft,   XtNright,  XawChainRight,
		XtNtop,    XawChainTop,    XtNbottom, XawChainTop,
		XtNhorizDistance, 0,       XtNvertDistance, 0,
		XtNborderWidth, 0, NULL);

  XtVaSetValues(helpBox,
		XtNfromHoriz, topBox,      XtNleft,   XawChainRight,
		XtNright,  XawChainRight,  XtNtop,    XawChainTop,
		XtNbottom, XawChainTop,    XtNhorizDistance, 0,
		XtNvertDistance, 0,        XtNborderWidth, 0, NULL);

  aboutButton   = YCreateSurroundedWidget
    ("About", commandWidgetClass, topBox, SurroundShade, NoShade); 
  YSetValue(aboutButton, "shadowWidth", 0); /* in case we're 3d */

     fileButton = YCreateMenuButton ("File",   topBox);
     editButton = YCreateMenuButton ("Edit",   topBox);
    chordButton = YCreateMenuButton ("Chord",  topBox);
    staveButton = YCreateMenuButton ("Staff",  topBox);
    groupButton = YCreateMenuButton ("Group",  topBox);
      barButton = YCreateMenuButton ("Bar",    topBox);
     textButton = YCreateMenuButton ("Words",  topBox);
     markButton = YCreateMenuButton ("Marks",  topBox);
   filterButton = YCreateMenuButton ("Filter", topBox);
     helpButton = YCreateCommand    ("Help",   helpBox);
  
  XtVaSetValues(topBox, XtNleft, XawChainLeft, XtNright, XawChainRight,
		XtNtop, XawChainTop, XtNbottom, XawChainTop, NULL);

  if (!appData.interlockWindow) XtSetSensitive(helpButton, False);

   rewindButton = YCreateSurroundedWidget("Fast Rewind",  repeaterWidgetClass,
					   bottomBox, SurroundShade, NoShade);
     backButton = YCreateSurroundedWidget("Scroll Left",  repeaterWidgetClass,
					   bottomBox, SurroundShade, NoShade);
     pageButton = YCreateSurroundedWidget("Bar 0000",      commandWidgetClass,
					   bottomBox, SurroundShade, NoShade);
  forwardButton = YCreateSurroundedWidget("Scroll Right", repeaterWidgetClass,
					   bottomBox, SurroundShade, NoShade);
     ffwdButton = YCreateSurroundedWidget("Fast Forward", repeaterWidgetClass,
					   bottomBox, SurroundShade, NoShade);

  palettePane    = YCreateShadedWidget
    ("Editor Palette", panedWidgetClass, paletteShell,    NoShade);
  paletteTopPane = YCreateShadedWidget
    ("Palette Top Pane", panedWidgetClass, palettePane,    NoShade);
  paletteFollowBox = YCreateShadedWidget
    ("Palette Follow Box", formWidgetClass, paletteTopPane, MediumShade);
  paletteTopBox  = YCreateShadedWidget
    ("Palette Top Box", formWidgetClass, paletteTopPane, MediumShade);

  YSetValue(  aboutButton,     XtNbitmap,     roseMap );
  YSetValue(     topLevel, XtNiconPixmap,     roseMap );
  YSetValue( paletteShell, XtNiconPixmap,     roseMap );
  YSetValue(     topLevel,   XtNiconMask, roseMaskMap );
  YSetValue( paletteShell,   XtNiconMask, roseMaskMap );

  YMenuInitialise(XtParent(aboutButton), appData.acceleratorTable);

  tempButton = YCreateCommand("Temp", paletteTopBox);

  InstallPaletteFollowToggle(paletteFollowBox);
  InstallPaletteMods(paletteTopBox);
  InstallPalettes(palettePane);

  paletteSpace = YCreateShadedWidget
    ("Palette Space", boxWidgetClass, palettePane, MediumShade);

  StaveInitialise(musicViewport);

  XtSetMappedWhenManaged(paletteShell, False);
  XtRealizeWidget(paletteShell);

  YGetValue(XtParent(tempButton), XtNheight, &h);
  YGetValue(paletteFollowBox, XtNheight, &hh);

  XtUnrealizeWidget(paletteShell);
  XtSetMappedWhenManaged(paletteShell, True);

  XtDestroyWidget(XtParent(tempButton));

  YSetValue(scrollbar, XtNorientation, XtorientHorizontal);
  YSetValue(scrollbar, XtNmin, 14);
  YSetValue(scrollbar, XtNmax, 14);

  YSetValue(paletteTopBox, XtNmin, roseHeight + 17 - hh);
  YSetValue(paletteTopBox, XtNmax, roseHeight + 17 - hh);
  YSetValue(paletteSpace,  XtNmax, h + 10);
  YSetValue(paletteSpace,  XtNmin, h + 10);
  XtRealizeWidget(paletteShell);

  pixmap =
    XCreateBitmapFromData(display,w,rewind_bits,rewind_width,rewind_height);
  YSetValue(  rewindButton, XtNbitmap, pixmap );

  rightMap =
    XCreateBitmapFromData(display,w,forward_bits,forward_width,forward_height);
  YSetValue( forwardButton, XtNbitmap, rightMap );

  leftMap = XCreateBitmapFromData(display,w,back_bits,back_width,back_height);
  YSetValue(    backButton, XtNbitmap, leftMap );

  pixmap = XCreateBitmapFromData(display,w,ffwd_bits,ffwd_width,ffwd_height);
  YSetValue(    ffwdButton, XtNbitmap, pixmap );

  XtAddCallback(helpButton,    XtNcallback,               HelpButton, NULL);
  XtAddCallback(aboutButton,   XtNcallback,              AboutButton, NULL);
  XtAddCallback(scrollbar,     XtNscrollProc, StaveScrollbarCallback, NULL);
  XtAddCallback(scrollbar,     XtNjumpProc,        StaveJumpCallback, NULL);

  XtAddEventHandler(scrollbar, ExposureMask, False, StaveScrollbarExpose, NULL);

  XtAddCallback(rewindButton,  XtNcallback,
		StaveLeftCallback,  (XtPointer)True);
  XtAddCallback(backButton,    XtNcallback,
		StaveLeftCallback,  (XtPointer)False);
  XtAddCallback(pageButton,    XtNcallback,
		StavePageCallback,  NULL);
  XtAddCallback(forwardButton, XtNcallback,
		StaveRightCallback, (XtPointer)False);
  XtAddCallback(ffwdButton,    XtNcallback,
		StaveRightCallback, (XtPointer)True);

    InstallFileMenu(   fileButton);
    InstallEditMenu(   editButton);
   InstallChordMenu(  chordButton);
   InstallStaveMenu(  staveButton);
   InstallGroupMenu(  groupButton);
    InstallTextMenu(   textButton);
    InstallMarkMenu(   markButton);
  InstallFilterMenu( filterButton);
     InstallBarMenu(    barButton);

  XtInstallAccelerators(musicViewport,  XtParent(aboutButton));
  XtInstallAccelerators(topBox,         XtParent(aboutButton));
  XtInstallAccelerators(bottomBox,      XtParent(aboutButton));
  XtInstallAccelerators(paletteTopBox,  XtParent(aboutButton));
  XtInstallAccelerators(paletteSpace,   XtParent(aboutButton));

  YSetValue(toptopBox,     XtNmin, roseHeight + 17);
  YSetValue(toptopBox,     XtNmax, roseHeight + 17);
  YSetValue(bottomBox,     XtNmax, h + 10);
  YSetValue(bottomBox,     XtNmin, h + 10);

  YSetValue(musicViewport, XtNallowVert,   True);
  YSetValue(musicViewport, XtNallowHoriz, False);
  YSetValue(musicViewport, XtNforceBars,   True);

  XtRealizeWidget(topLevel);

  StaveInitialiseScrollbar(scrollbar);
  YSetScrollbarPixmap(scrollbar);
  YSetViewportScrollbarPixmaps(musicViewport);

  stave = NULL;
  YSetValue(pageButton, XtNlabel, "Bar 0");
  XtSetSensitive(pageButton, False);

  EnterMenuMode(NoAreaSweptMode         |
		SequencerNotRunningMode |
		FileNotLoadedMode       |
		ShowingChordNamesMode   |
		UndoUnavailableMode     |
		RedoUnavailableMode     |
		CursorNotPlacedMode);

  End;
}

/* }}} */

