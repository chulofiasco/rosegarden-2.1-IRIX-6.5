
/*
                      ===========================
                          TeXinfo Help Module
                      ===========================

     This module is an  X implementation of the  Info help system.  It
     is heavily  based on the ascii  terminal help  program by Herbert
     Melenk, used by ascii Reduce.

     The intended utility of this module is:

 [0] Initialise     the help  facility,    by   calling  the  function
     YHelpInitialise(parent,  x,    y,  name, callback,  exitcallback,
     helponhelp), where parent is  a widget that  the help  window can
     use as a parent,  x and y are  the  desired width and  height (in
     pixels) of the help window, name is  the desired name of the help
     window,  callback  is a pointer  to  a function   which returns a
     context string as may be used in the Help Guess Context method --
     described below--  or NULL if you  want no  Guess Context button,
     exitcallback is  a function to  be  called, taking  no arguments,
     when Help has exited, and helponhelp is the name of the help node
     (in the as-yet unopened  help  file) which  contains help  on the
     help itself   (or  NULL if    there  is  none).  You   can   call
     YHelpInitialise more than once  if you want,  though it is unwise
     to do so while Help is open.

 [1] Install      an initial   help   file,      with   a  call     to
     YHelpSetHelpFile(filename);  filename  should be a full pathname,
     and it must  be readable and seekable.  This  call  will open the
     help file, if possible, and close any previous help file; it also
     identifies and opens, if possible, the search-index file (usually
     the same   name as the  help file  but with a   ".hix" extension.
     YHelpSetHelpFile returns non-zero to indicate failure.

 [2] Request  an   initial  help  topic,   if  required,  by   calling
     YHelpSetTopic(topic),  where topic is  the  name of  the required
     help node.  If a NULL topic is requested, or  if no initial topic
     is  requested, the topmost  node  will  be used.    YHelpSetTopic
     returns non - zero  to indicate failure.  Note that YHelpSetTopic
     should only be called after YHelpSetHelpFile.

 [3] If you wish, set the fonts you want the help  window to use, with
     calls to   the  functions YHelpSetTextFont(fn),  YHelpSetXrefFont
     (fn), YHelpSetTitleFont(fn) and YHelpSetVerbatimFont(fn),   where
     arguments are   strings  giving, respectively,   fonts for normal
     text, highlighting cross-references, buttons, titles and verbatim
     environments.  Note that  a non-proportional font  is a good idea
     for verbatim environments.   If no fonts  are set, defaults (ugly
     ones) will be  used  instead.  If  you already  have  XFontStruct
     structures for your fonts and  wish to use them without requiring
     them to be reloaded, you can instead call YHelpSetTextFontStruct,
     YHelpSetXrefFontStruct,         YHelpSetTitleFontStruct       and
     YHelpSetVerbatimFontStruct, passing in the XFontStruct* argument.

 [4] When  the  help  facility  is called    for,   call the  function
     YHelpInstallHelp(), without  args.  This  should  cause the  help
     window to pop up and operate as a  non-grabbing subshell until it
     is finished  with.  It returns  non-zero to indicate failure, and
     it   will   happily  do  so  if,   for example,   the helpfile is
     unavailable or not set.

 [5] If the parent program wishes to close  the help window before the
     help facility would otherwise do so, it should call YHelpClose(),
     with no arguments.  HelpClose() returns no value; if it is called
     when the help window is already closed, it does nothing.

 [6] The current help filename can be obtained by calling the function
     YHelpGetHelpFile(). This returns a  shared string, or NULL  if no
     help file is currently set.

 [7] Calls to YHelpSetHelpFile() & YHelpSetTopic() may  be made at any
     time, although doing so may cause something of a context jump for
     the user.

 [8] Finally, before  the parent program exits,  if it has ever called
     YHelpSetHelpFile it  should call the function YHelpCloseHelpFile,
     to make sure that the file is closed  and any changes to the help
     index file are saved. It is safe to call this function even if no
     help file has been opened.   This function also calls YHelpClose,
     to ensure that the help window is properly closed.

     The help index  file  (extension ".hnx") used for  searching, and
     for speeding the  initial location of  the Table  of Contents, is
     made by this module, from the help  file.  A call to the function
     YHelpWriteIndexFile(), after opening a help file, will create and
     write  the index  file if    possible,  returning non- zero   for
     failure.  This takes a while.  If the help module is used without
     the index file's  presence, there will  not be a  Search facility
     available.

     The callback given to    YHelpInitialise  for use by  the   Guess
     Context facility should be a  pointer to a nullary function which
     examines the contextual status of the  parent program and returns
     a string which  Help can use to search  for likely references for
     help on  the  user's current dilemma.   It  should always  try to
     return something, even if only "Top"; if  it doesn't, the attempt
     is abandoned, and no indication of why  is given except an opaque
     warning message.

     In Reduce's case the context callback will probably just return a
     concatenation of any likely-looking words in the last few inputs;
     your callback  should probably do  something vaguely  similar.  I
     know this is a bit basic.

     The name given to  YHelpInitialise  will  be duplicated by   the
     function and may be  freed; the  string  returned by  the context
     callback should  be in space obtained from  XtMalloc as the Guess
     Context code will try to free it.

     It is my hope and intention that this help  module may be of some
     use  to programs other than XReduce.  Before that, however, it is
     my hope and intention that this help module may be made to work.

                        =============================
                        Chris Cannam, Berlin Jan 1993
                        =============================

			Revised Bath 1994 London 1996
*/


/*

 ===============================================
 Include lots of relevant and irrelevant headers
 ===============================================
 
   Include several useful  standard string and
   file managing headers, and all the relevant
   X header  information;  do  not include any
   headers specific to the application running
   the help module, so as to keep it modular.
*/


#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <SysDeps.h>   /* my header */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Viewport.h>

#include "Debug.h"

#include "Yawn.h"
#include "YHelp.h"

#include "right.xbm"
#include "left.xbm"
#include "up.xbm"

/* from Yawn.c; these should really be exported */
extern Boolean _yInitialised;
extern Boolean _yIsXaw3d;


/*

 ================================================
 Define global constants and filescoped variables
 ================================================

*/

#define FILE_TAG     "File:"
#define NODE_TAG     "Node:"
#define NEXT_TAG     "Next:"
#define PREV_TAG     "Prev:"
#define   UP_TAG     "Up:"
#define  TOC_TAG     "TagTable:"
#define DEFAULT_FONT "fixed"

#define END_OF_PAGE 0x1f
#define END_OF_LINE 0x0a
#define MAX_NODE    1000

/* unused
#define MAX_TABLE   1000
*/

#define MAX_LABELS  1000
#define MAX_BUFFER  10000
#define NAME_LENGTH 200		/* Was 100, upgraded because of overflows */
#define TAG_LENGTH  50
#define LABEL_SPACE 2

#define Try(x)          { if ((x)==-1 ) { goto oops; } else {}; }
#define TrL(x)          { if ((x)==-1L) { goto oops; } else {}; }
#define MenuP           ( helpMode == YHelpMenuMode )

typedef struct _YHelpSearchPage {
  char                   *page;
  char                   *comment;
  int                     frequency;
  struct _YHelpSearchPage *next;
} YHelpSearchPage;

typedef struct _YHelpSearchTree {
  char                   *string;
  YHelpSearchPage         *references;
  struct _YHelpSearchTree *inferior;
  struct _YHelpSearchTree *superior;
} YHelpSearchTree;

typedef enum {
  YHelpHeadMode,
  YHelpTextMode,
  YHelpXmplMode,
  YHelpMenuMode
} YHelpMode;

typedef enum {
  YHelpTextDisplay,
  YHelpSearchDisplay,
  YHelpRouteDisplay
} YHelpDisplayMode;

static Widget             helpSh;
static Widget             helpPane;

/* not currently used
static Widget             infoBox;
static Widget             nodeLabel;
*/

static Widget             commandBox;
static Widget             aboutButton;
static Widget             quitButton;
static Widget             helpButton;
static Widget             searchButton;
static Widget             showButton;
static Widget             buttonBox;
static Widget             prevButton;
static Widget             upButton;
static Widget             nextButton;
static Widget             textViewport;
static Widget             textBox;
static Widget             textForm;
static Widget             textEmpty;
static Widget             textLabels[MAX_LABELS];
static Widget             menuViewport;
static Widget             menuBox;
static Widget             menuForm;
static Widget             menuEmpty;
static Widget             menuLabels[MAX_LABELS];
static Widget             searchLabel1;
static Widget             searchLabel2;
static Widget             searchText;
static Widget             searchGo;
static Widget             searchGuess;
static Widget             referenceWidget;
static int                helpPaneHt;	/* in number of lines */
static int                menuPaneHt;
static Boolean            helpInstalled = False;
static Boolean            writingSearch = False;
static Boolean            lastSearch    = False;
static Boolean            guessMode     = False;
static YHelpDisplayMode   displayMode   = YHelpTextDisplay;
static YHelpMode          helpMode      = YHelpTextMode;
static Dimension          helpTextWd;
static FILE             * helpFile = NULL;
static FILE             * helpSFil = NULL;
static char             * hefiName = NULL;
static char             * hesfName = NULL;
static char             * helpName = NULL;
static YHelpSearchPage  * helpRout = NULL;
static YHelpSearchPage  * srchList = NULL;
static YHelpSearchPage  * comments = NULL;
static YHelpSearchTree  * searchBT = NULL;
static char             * helpHelp = NULL;
static char         *  (* guess)(void) = NULL;
static void            (* exitcallback)(void) = NULL;
static Pixmap             yHelpIdentifyingBitmap = 0;
static Pixmap             rMap;
static Pixmap             lMap;
static Pixmap             uMap;

XFontStruct             * yHelpTextFont = NULL;
XFontStruct             * yHelpXrefFont = NULL;
XFontStruct             * yHelpHeadFont = NULL;
XFontStruct             * yHelpXmplFont = NULL;

static int           nrTable;
static long          nrNodes;

/* these aren't actually used, though probably should be:

static int           nrLabels;
static int           nrMenu;
*/

static int           bPoint;
static long          topNode = -1;
static long          actNode = -1;
static long          TOCNode = -1;

static char    tag[  TAG_LENGTH ];
static char   node[ NAME_LENGTH ];
static char   next[ NAME_LENGTH ];
static char   prev[ NAME_LENGTH ];
static char     up[ NAME_LENGTH ];
static char *nname[  MAX_NODE   ];
static long  naddr[  MAX_NODE   ];
static char buffer[  MAX_BUFFER ];

/* unused; I can't even remember what it's for:

static long  table[  MAX_TABLE  ];
*/

static char *insignificantWords[] = {
  "the",   "a",    "and",    "but",      "note", "to",   "of",   "it",
  "when",  "how",  "why",    "however",  "on",   "this", "that", "there",
  "here",  "so",   "if",     "be",       "by",   "can",  "does", "done",
  "has",   "have", "is",     "examples", "its",  "may",  "must", "not",
  "only",  "or",   "syntax", "reduce",   "than", "you",  "as",   "rosegarden",
   NULL,
};

YMessageString yHelpAboutText[] = {
  { "This is a hypertext TeXinfo Help system.", YMessageNormal, },
  { "It was originally written by",             YMessageNormal, },
  { "Chris Cannam",                             YMessageBold,   },
  { "at the",                                   YMessageNormal, },
  { "Konrad-Zuse-Zentrum",                      YMessageBold,   },
  { "fuer Informationstechnik, Berlin",         YMessageBold,   },
  { "and has been further developed at",        YMessageNormal, },
  { "the University of Bath, UK.",              YMessageNormal, },
};

void YHelpClose(void);
void YHelpHelpButton(Widget, XtPointer, XtPointer);
void YHelpMenuButton(Widget, XtPointer, XtPointer);
void YHelpXrefButton(Widget, XtPointer, XtPointer);
void YHelpSearchButton(Widget, XtPointer, XtPointer);
void SearchGo(Widget, XtPointer, XtPointer);
void SearchGuess(Widget, XtPointer, XtPointer);
int DisplayPage(long, Boolean);



int YHelpError(String ermsg)
{
  fprintf(stderr,"Help Module: %s\n",ermsg);
  fflush(stderr);
  return -1;
}

int YHelpError2s(String ermsg, String arg)
{
  fprintf(stderr,"Help Module: %s \"%s\"\n", ermsg, arg);
  fflush(stderr);
  return -1;
}

int YHelpError2d(String ermsg, int arg)
{
  fprintf(stderr,"Help Module: %s {%d}\n", ermsg, arg);
  fflush(stderr);
  return -1;
}


XFontStruct *EnsureFont(XFontStruct *fnP)
{
  if (fnP == NULL) {

    YHelpError2s("Absent font requested: using default",DEFAULT_FONT);
    if (!(fnP = XLoadQueryFont(XtDisplay(referenceWidget), DEFAULT_FONT)))
      YHelpError2s("Couldn't load default font",DEFAULT_FONT);
  }
  return fnP;
}


static void AboutButton(Widget w, XtPointer a, XtPointer b)
{
  if (YMessageIsInitialised())
    YMessage(XtParent(w), "About Help",
	      "Enough!", yHelpAboutText, XtNumber(yHelpAboutText));
}


/* This is supposed to work out the right geometries regardless of
   whether or not yHelpIdentifyingBitmap is set, but I suspect it'll
   actually only work properly if it is */

int YHelpCreateWidgets(void)
{
  int       i;
  Position  x;
  Position  y;
  Widget    topBox;
  Widget    helpBox;

  if ((yHelpTextFont = EnsureFont(yHelpTextFont)) == NULL) goto oops;
  if ((yHelpXrefFont = EnsureFont(yHelpXrefFont)) == NULL) goto oops;
  if ((yHelpHeadFont = EnsureFont(yHelpHeadFont)) == NULL) goto oops;
  if ((yHelpXmplFont = EnsureFont(yHelpXmplFont)) == NULL) goto oops;

  helpSh     = XtCreatePopupShell("Help",
				  topLevelShellWidgetClass,
				  referenceWidget,
				  NULL, 0);
  helpPane =
    YCreateWidget("Help Pane", panedWidgetClass, helpSh);

  topBox = YCreateShadedWidget("Top Box", formWidgetClass,
			       helpPane, MediumShade);

  commandBox =
    YCreateShadedWidget("Command Box", boxWidgetClass, topBox, MediumShade);
  helpBox =
    YCreateShadedWidget("Help Box", boxWidgetClass, topBox, MediumShade);

  XtVaSetValues(topBox, XtNdefaultDistance, 0, NULL);
    
  XtVaSetValues(commandBox,
		XtNleft,   XawChainLeft,   XtNright,  XawChainRight,
		XtNtop,    XawChainTop,    XtNbottom, XawChainTop,
		XtNhorizDistance, 0,       XtNvertDistance, 0,
		XtNborderWidth, 0, NULL);

  XtVaSetValues(helpBox,
		XtNfromHoriz, commandBox,  XtNleft,   XawChainRight,
		XtNright,  XawChainRight,  XtNtop,    XawChainTop,
		XtNbottom, XawChainTop,    XtNhorizDistance, 0,
		XtNvertDistance, 0,        XtNborderWidth, 0, NULL);
    
#ifdef SHOW_CURRENT_NODE_LABEL
  infoBox =
    YCreateShadedWidget("Info Box",    boxWidgetClass, helpPane,  LightShade);
#endif

  buttonBox =
    YCreateShadedWidget("Button Box",  boxWidgetClass, helpPane,  LightShade);

  textViewport = 
    YCreateWidget("Text Viewport", viewportWidgetClass, helpPane);
  menuViewport =
    YCreateWidget("Menu Viewport", viewportWidgetClass, helpPane);

  if (yHelpIdentifyingBitmap) {

    Window rw;
    int xx, yy;
    unsigned ww, hh, bw, d;

    aboutButton = YCreateSurroundedWidget
      ("About", commandWidgetClass, commandBox, NoShade, NoShade);
    YSetValue(aboutButton, XtNbitmap, yHelpIdentifyingBitmap);
    YSetValue(aboutButton, "shadowWidth", 0);
    XtAddCallback(aboutButton, XtNcallback, AboutButton, NULL);

    XGetGeometry(XtDisplay(aboutButton), yHelpIdentifyingBitmap,
		 &rw, &xx, &yy, &ww, &hh, &bw, &d);

    XtVaSetValues(topBox, XtNmax, hh + 17, XtNmin, hh + 17, NULL);
  }

  searchButton  = YCreateCommand("Search",  commandBox);
  showButton    = YCreateCommand("History", commandBox);
  quitButton    = YCreateCommand("Close",   commandBox);

  if (helpHelp) {
    helpButton  = YCreateCommand("Help", helpBox);
    XtAddCallback(helpButton, XtNcallback, YHelpHelpButton, NULL);
  }

#ifdef SHOW_CURRENT_NODE_LABEL
  nodeLabel    = YCreateLabel("Node Label",   infoBox);
#endif

  prevButton   = YCreateCommand("Previous", buttonBox);
  upButton     = YCreateCommand("Up",       buttonBox);
  nextButton   = YCreateCommand("Next",     buttonBox);

  textBox   = YCreateWidget("Text Surround", boxWidgetClass, textViewport);
  textForm  = YCreateWidget("Text Form",    formWidgetClass, textBox);
  textEmpty = YCreateWidget("Empty",       labelWidgetClass, textForm);

  menuBox   = YCreateWidget("Menu Surround", boxWidgetClass, menuViewport);
  menuForm  = YCreateWidget("Menu Form",    formWidgetClass, menuBox);
  menuEmpty = YCreateWidget("Empty",       labelWidgetClass, menuForm);

  for (i = 0; i < MAX_LABELS; ++i) textLabels[i] = NULL;
  for (i = 0; i < MAX_LABELS; ++i) menuLabels[i] = NULL;

  XtTranslateCoords(referenceWidget, 150, 70, &x, &y);

  YSetValue(helpSh,       XtNtitle,              helpName);
#ifdef SHOW_CURRENT_NODE_LABEL
  YSetValue(infoBox,      XtNshowGrip,           False);
#endif
  YSetValue(commandBox,   XtNshowGrip,           False);
  YSetValue(buttonBox,    XtNshowGrip,           False);
  YSetValue(textViewport, XtNallowResize,        True);
  YSetValue(textViewport, XtNallowHoriz,         False);
  YSetValue(textViewport, XtNallowVert,          True);
  YSetValue(textViewport, XtNforceBars,          True);
  YSetValue(textViewport, XtNshowGrip,           False);
  YSetValue(menuViewport, XtNallowHoriz,         False);
  YSetValue(menuViewport, XtNallowVert,          True);
  YSetValue(menuViewport, XtNforceBars,          True);
  YSetValue(textEmpty,    XtNlabel,              " ");
  YSetValue(menuEmpty,    XtNlabel,              " ");
  YSetValue(textEmpty,    XtNborderWidth,        0);
  YSetValue(menuEmpty,    XtNborderWidth,        0);
  YSetValue(textForm,     XtNborderWidth,        0);
  YSetValue(menuForm,     XtNborderWidth,        0);
  YSetValue(prevButton,   XtNleftBitmap,         lMap);
  YSetValue(  upButton,   XtNleftBitmap,         uMap);
  YSetValue(nextButton,   XtNleftBitmap,         rMap);

  XtRealizeWidget(helpSh);

  YSetViewportScrollbarPixmaps(textViewport);
  YSetViewportScrollbarPixmaps(menuViewport);

  searchGuess  =
    YCreateCommand("Search Guess",  buttonBox);
  searchGo     =
    YCreateCommand("Search Go", buttonBox);

  searchLabel1 =
    XtCreateWidget("Search Label", labelWidgetClass, textForm, NULL, 0);
  searchLabel2 =
    XtCreateWidget("Search Label", labelWidgetClass, textForm, NULL, 0);
  searchText   =
    XtCreateWidget("Search Text", asciiTextWidgetClass, textForm, NULL, 0);

  YSetValue(searchLabel1, XtNborderWidth, 0);
  YSetValue(searchLabel2, XtNborderWidth, 0);
  YSetValue(searchText,   XtNborderWidth, 0);
  YSetValue(searchText,   XtNwidth, helpTextWd - 300);

  XtUnmanageChild(searchGuess);
  XtUnmanageChild(searchGo);
  
  XtUnmanageChild(XtParent(searchGuess));
  XtUnmanageChild(XtParent(searchGo));

  YSetValue(searchGo,    XtNlabel, "Execute Search");
  YSetValue(searchGuess, XtNlabel,  "Guess Context");
  
  XtAddCallback(searchGo,    XtNcallback, SearchGo,    0);
  XtAddCallback(searchGuess, XtNcallback, SearchGuess, 0);

  if (!helpSFil) XtUnmanageChild(XtParent(searchButton));

  return 0;

 oops:

  return YHelpError("Can't set up the help window as required");
}


int SkipChar(char u)
{
  char c;

  do {

    if (  feof(helpFile)) return YHelpError("Unexpected EOF in help file");
    if (ferror(helpFile)) return YHelpError("Error reading help file");

    fread (&c, 1, 1, helpFile);

  } while (c != u);

  return 0;
}


int ReadChar(void)
{
  char c;

  if (  feof(helpFile)) return YHelpError("Unexpected EOF in help file");
  if (ferror(helpFile)) return YHelpError("Error reading help file");

  fread (&c, 1, 1, helpFile);

  return c;
}


int ReadTag(void)
{
  int c;
  int i = 0;

  while (((c = ReadChar()) != -1) && (i < (TAG_LENGTH - 2)) &&
	       (isalnum((char)c) || isspace((char)c) || (c == 31) ||
		(i == 0 && c == END_OF_LINE)))
    if (c != END_OF_LINE && !isspace((char)c) && c != 31) tag[i++] = (char)c;

  tag[i+1] = (char)0;

  if      (c == ':')   tag[i] = (char)c;
  else if (c ==  -1) { tag[i] = (char)0;
		       return YHelpError2s("Tag read failed after",tag); }
  return 0;
}


int ReadString(String s)
{
  int c;
  int  i = 0;

  while (((c = ReadChar()) != (char)-1) &&
	 (isalnum(c) ||
	  c == ' ' || c == '?' || c == '_' ||  c == '-' ||
	  c == ':' || c == '.' || c == '(' ||  c == ')' ||
	  c == '%' || c == '^' || c == '!' ||
	  (c == '*' && i > 0 &&
	   (isprint(s[i-1]) && s[i-1] != '[')))) {

    s[i] = c; if (i != 0 || c != ' ') ++i;
  } s[i] = 0;

  if (c == -1) return YHelpError2s("String read failed after",s);
  else         return 0;
}
  

long ReadNumber(void)
{
  int c;
  long n = 0;

  while (((c = ReadChar()) != (char)-1) && isdigit(c))
    n = 10 * n + (c-'0');

  if (c == (char)(-1))
       return (long)(YHelpError2d("Number read failed: so far read", (int)n));
  else return n;
}


int FindTOC(void)
{
  if (TOCNode == -1) {

    nrNodes = 0;

    do {

      Try(SkipChar(END_OF_PAGE));
      Try(SkipChar(END_OF_LINE));
      Try(ReadTag());

    } while (strcmp(tag,TOC_TAG));

    Try(SkipChar(END_OF_LINE));
    TOCNode = ftell(helpFile);

  } else {

    if (fseek(helpFile, TOCNode, 0)) {
      YHelpError2d("Can't find TOC at", (int)TOCNode);
      TOCNode = -1;
      return FindTOC();
    }
  }

 loop: 

  Try(ReadTag());
  if (strcmp(tag,NODE_TAG)) return 0;

  Try(ReadString(buffer));
  nname[nrNodes] = XtNewString(buffer);
  TrL(naddr[nrNodes] = ReadNumber());

  if ((++nrNodes) > MAX_NODE) return YHelpError("Too many nodes in helpfile");

  goto loop;

 oops:
  return YHelpError("Unable to find Table of Contents");
}


long FindNode(String topic)
{
  int i;

  for (i = 0; i < nrNodes && strcmp(topic, nname[i]); ++i);

  if (i < nrNodes) return naddr[i];
  else {

    String s;

    s = (String)XtMalloc(strlen(topic) + 30);
    sprintf(s, "No such node as `%s'", topic);

    if (_yInitialised) {
      (void)YQuery(referenceWidget, s, 1, 0, 0, "Cancel", NULL);
    } else {
      fprintf(stderr, "%s\n", s);
    }

    return (long)(YHelpError(s));
  }
}



int YHelpSetHelpFile(String filename)
{
  char *spare;
  int   i, j;

  if (helpFile) fclose(helpFile);
  if (hefiName) XtFree(hefiName);

  if (!(helpFile = fopen(filename, "r")))
    return YHelpError2s("Cannot open help file",filename);
  hefiName = XtNewString(filename);

  if (helpSFil) fclose(helpSFil);
  if (hesfName) XtFree(hesfName);

  spare = (char *)XtMalloc((j = strlen(filename)) + 10);
  strcpy(spare, filename);
  for (i = j; i >= 0 && spare[i] != '.'; --i);
  if (spare[i] == '.') strcpy(spare + i+1, "hnx");
  else strcpy(spare + j, ".hnx");

  hesfName = XtNewString(spare);

  if   (!(helpSFil = fopen(spare, "r"))) {
    (void)YHelpError2s("Cannot open help index file",spare);
    (void)YHelpError("Sorry, there will be no search utility");
  } else {
    if (fscanf(helpSFil, "%ld\n", &TOCNode) != 1) {
      (void)YHelpError("Could not read TOC location from index file");
      TOCNode = -1;
    }
  }

  Try(FindTOC());
  TrL(topNode = FindNode("Top"));

  XtFree(spare);
  return 0;

 oops: 
  return YHelpError2s("Problem in reading help file",filename);
}


int YHelpCloseHelpFile(void)
{
  YHelpClose();

  if (helpFile) fclose(helpFile);
  if (hefiName) XtFree(hefiName);
  if (helpSFil) fclose(helpSFil);
  if (hesfName) XtFree(hesfName);

  return 0;
}


int YHelpSetTopic(String topic)
{
  long newNode = -1;

  if (!helpFile)
    return YHelpError("Attempt to set topic on nonexistent file");

  if (topic == NULL)  newNode = topNode;
  else                newNode = FindNode(topic);

  if (newNode == -1L) return YHelpError("Attempt to set nonexistent topic");

  if (helpInstalled &&
      (newNode != actNode || displayMode != YHelpTextDisplay)) {

    actNode = newNode; Try(DisplayPage(newNode, True));

  } else actNode = newNode;

  return 0;

 oops: return YHelpError("Cannot display requested topic's node");
}


int SizePanes(void)
{
  int       i;
  Arg       arg[2];
  int       ath;
  Dimension pah;
  Dimension vah;
  Dimension vbh;
  Dimension vth;

  i = 0; XtSetArg(arg[i], XtNheight, &vah); i++;
  XtGetValues(textViewport, arg, i);
  i = 0; XtSetArg(arg[i], XtNheight, &vbh); i++;
  XtGetValues(menuViewport, arg, i);

  ath = helpPaneHt + menuPaneHt;
  vth = vah + vbh;

  if      (helpPaneHt == 0) pah =       LABEL_SPACE + 1;
  else if (menuPaneHt == 0) pah = vth - LABEL_SPACE - 1;
  else {
    pah = (helpPaneHt * vth) / ath;
    if      (pah <  100)        pah = 100;
    else if (vth < (100 + pah)) pah = vth - 100;
  }

  i = 0; XtSetArg(arg[i], XtNheight, pah); i++;
  XtSetValues(textViewport, arg, i);

  i = 0; XtSetArg(arg[i], XtNheight, &vah); i++;
  XtGetValues(textForm, arg, i);
  i = 0; XtSetArg(arg[i], XtNheight,  vah + 2); i++;
  XtSetValues(textBox,  arg, i);
  i = 0; XtSetArg(arg[i], XtNheight, &vbh); i++;
  XtGetValues(menuForm, arg, i);
  i = 0; XtSetArg(arg[i], XtNheight,  vbh + 2); i++;
  XtSetValues(menuBox,  arg, i);

  XtMapWidget(textViewport);
  XtMapWidget(menuViewport);

  XSync(XtDisplay(textViewport), False);

  return 0;
}


int YHelpInstantiateNodeLabels(void)
{
  static char curL[NAME_LENGTH + 8];

  XtUnmanageChild( nextButton);
  XtUnmanageChild(   upButton);
  XtUnmanageChild( prevButton);

  XtUnmanageChild(XtParent( nextButton));
  XtUnmanageChild(XtParent(   upButton));
  XtUnmanageChild(XtParent( prevButton));

#ifdef SHOW_CURRENT_NODE_LABEL
  sprintf(curL, "Help on ``%s''", node);
  YSetValue(nodeLabel, XtNlabel, curL);
#endif

  if (strlen(prev) > 0) {

    sprintf(curL, "%s", prev);
    YSetValue(prevButton, XtNlabel, curL);

    XtManageChild(prevButton);
    XtManageChild(XtParent(prevButton));
  }

  if (strlen(up) > 0 && strcmp(up,"(dir)") != 0) {

    sprintf(curL, "%s", up);
    YSetValue(upButton, XtNlabel, curL);

    XtManageChild(upButton);
    XtManageChild(XtParent(upButton));
  }

  if (strlen(next) > 0) {

    sprintf(curL, "%s", next);
    YSetValue(nextButton, XtNlabel, curL);

    XtManageChild(nextButton);
    XtManageChild(XtParent(nextButton));
  }

  return 0;
}


int YHelpInstantiateRoutedNode(void)
{
  YHelpSearchPage *routEnd;
  YHelpSearchPage *routPre;
  char            comment[25];
  int             vC = 1;

  routEnd = helpRout;
  routPre = NULL;

  while (routEnd) {

    if (!strcmp(routEnd->page, node)) {

      vC = routEnd->frequency + 1;

      if (routPre) {
	routPre->next = routEnd->next;
	if    (routEnd->comment) XtFree(routEnd->comment);
	XtFree(routEnd->page);
	XtFree((char *)routEnd);
	routEnd  = routPre->next;

      } else {
	helpRout = routEnd->next;
	if    (routEnd->comment) XtFree(routEnd->comment);
	XtFree(routEnd->page);
	XtFree((char *)routEnd);
	routEnd  = helpRout;
      }
    } else {

      routPre = routEnd;
      routEnd = routEnd->next;
    }
  }

  routEnd = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));
  routEnd->page       = XtNewString(node);
  routEnd->frequency  = vC;
  routEnd->next       = NULL;

  if      (vC  < 2) routEnd->comment = XtNewString(" ");
  else if (vC == 2) routEnd->comment = XtNewString(" (Visited twice)");
  else {
    sprintf( comment," (Visited %d times)",vC);
    routEnd->comment = XtNewString(comment);
  }

  if (routPre) routPre->next = routEnd;
  else helpRout = routEnd;

  return 0;
}



XFontStruct *GetFontForMode(void)
{
  switch(helpMode) {

  case YHelpHeadMode:
    return yHelpHeadFont;

  case YHelpTextMode:
    return yHelpTextFont;

  case YHelpXmplMode:
    return yHelpXmplFont;

  case YHelpMenuMode:
    return yHelpTextFont;

  default:
    YHelpError2d("Warning: Unknown internal mode", helpMode);
    return yHelpTextFont;
  }
}


/* Create a label or command widget.  Arguments are: index of widget in  */
/* array; text to place in widget; flag for whether or not the widget    */
/* is a cross-reference (ie. highlighted and clickable); and flag for    */
/* whether or not a new line should be started for this widget (if not,  */
/* it will appear, we hope, after the previous widget on the same line.) */
/* Returns the new value of lab, or -1 to indicate failure.              */

int MakeWidget(int lab, String text, Boolean hl, Boolean nl)
{
  static Arg      arg[15];
  static Widget   above = NULL;
  Widget        * labels;
  int             paneHt;
  int             i = 0;

  /*
  fprintf(stderr,"Making widget, wwd %d, text \"%s\"\n",wwd,text);
  */

  labels = MenuP ? menuLabels : textLabels;
  paneHt = MenuP ? menuPaneHt : helpPaneHt;

  if (lab > MAX_LABELS) return YHelpError("Maximum text per page exceeded");
  if (lab == 0) { above = NULL;          paneHt = 0; }
  else if (nl)  { above = labels[lab-1]; paneHt  ++; }

  if (helpMode == YHelpHeadMode && nl && lab > 0) {

    char * txt;

    XtSetArg(arg[0], XtNlabel, &txt);
    XtGetValues(labels[lab-1], arg, 1);

    while (*txt)
      if (isprint(*txt)) { helpMode = YHelpTextMode; break; }
      else txt++;
  }

  if (MenuP) menuPaneHt = paneHt;
  else       helpPaneHt = paneHt;

  if (!nl && !hl && lab > 1 && XtClass(labels[lab-1]) == labelWidgetClass) {

    char * prevText;
    char * totlText;
    int    prevLen;

    XtSetArg(arg[0], XtNlabel, &prevText);
    XtGetValues(labels[lab-1], arg, 1);

    totlText = (char *)XtMalloc((prevLen=strlen(prevText)) + strlen(text) + 1);
    strcpy(totlText,  prevText);
    strcpy(totlText + prevLen, text);

    XtSetArg(arg[0], XtNlabel, totlText);
    XtSetValues(labels[lab-1], arg, 1);

    XtFree((char *)totlText);
    return lab;

  } else {

    if (lab > 0) {
      XtSetArg(arg[i], XtNfromHoriz,     (nl ? 0 : labels[lab-1])); i++;
      XtSetArg(arg[i], XtNfromVert,                         above); i++;
    }
    
    XtSetArg(arg[i], XtNhorizDistance,        nl? 2*LABEL_SPACE:0); i++;
    XtSetArg(arg[i], XtNvertDistance,      above? 0:LABEL_SPACE*5); i++;
    XtSetArg(arg[i], XtNfont, (hl?yHelpXrefFont:GetFontForMode())); i++;
    XtSetArg(arg[i], XtNlabel,                               text); i++;
    XtSetArg(arg[i], XtNborderWidth,                            0); i++;
    XtSetArg(arg[i], XtNhighlightThickness,                     1); i++;
    XtSetArg(arg[i], XtNinternalHeight,             LABEL_SPACE/2); i++;
    XtSetArg(arg[i], XtNinternalWidth,          hl? LABEL_SPACE:0); i++;
    XtSetArg(arg[i], "shadowWidth",                             0); i++;

     if (labels[lab] &&
	XtClass(labels[lab]) != (hl ? commandWidgetClass : labelWidgetClass)) {
      XtDestroyWidget(labels[lab]); labels[lab] = NULL;
    }

    if (labels[lab]) XtSetValues(labels[lab], arg, 6);
    else {

      /* don't use the (more general) Y stuff: this is faster */

      labels[lab] = XtCreateWidget
	((hl? "helpXref" : "helpText"),
	 (hl? commandWidgetClass : labelWidgetClass),
	 (MenuP ? menuForm  : textForm), arg, i);

      if (hl) {
	if (MenuP)
	  XtAddCallback(labels[lab], XtNcallback, YHelpMenuButton, 0);
	else
	  XtAddCallback(labels[lab], XtNcallback, YHelpXrefButton, 0);
      }
    }
    return ++lab;
  }
}



/* Takes two arguments, and displays some text in the normal text window.    */
/* Returns 0 for success, or other for failure.  The first arg is the text;  */
/* the second is a Multi-Purpose Flag.  Normally it is True if the text is   */
/* a cross-reference (to be highlighted and clickable); if the text is NULL  */
/* then if the flag is True, the window is cleared and prepared for a new    */
/* page, and if False, the current page is finished and displayed.           */
/* The third arg is True if the text is to be placed in the Menu window and  */
/* False if it is to go in the normal text window.  This also has a bearing  */
/* upon the formatting of cross-references (in menus they always start a     */
/* new line).  Menu and Text strings mustn't be mixed; finish one lot first. */

/* Does not preserve text argument.  Always call with a new clean string.    */


int PlaceText(String text, Boolean hl)
{
  static int      lab = 0;      /* current label (or command) widget      */
  static Boolean  nln = False;	/* if next widget should start new line   */
  static unsigned pll = 0;	/* pixels so far on line                  */

  int         i;
  int         nwl;
  int         swl;
  int         dir;
  int         asc;
  int         dsc;
  unsigned    wwd;
  XCharStruct info;

  if (text == NULL)
    if (hl) {

      Widget *labels;
      labels = MenuP ? menuLabels : textLabels;

      for (lab = 0;
	   lab < MAX_LABELS && labels[lab] && XtIsManaged(labels[lab]); ++lab);
      if  (lab > 0) XtUnmanageChildren(labels, lab);

      pll  = 0;
      lab  = 0;
      return 0;

    } else {

      Widget *labels;
      labels = MenuP ? menuLabels : textLabels;

      if (MenuP) {

	Arg         arg[2];
	char       *txt;
	int         mwd = 0;

	for (i = 0; i < lab && menuLabels[i]; ++i) {

	  if (XtClass(menuLabels[i]) == commandWidgetClass) {

	    XtSetArg(arg[0], XtNlabel, &txt);
	    XtGetValues(menuLabels[i], arg, 1);
	    XTextExtents(yHelpXrefFont,txt,strlen(txt),&dir,&asc,&dsc,&info);

	    if (info.width > mwd) mwd = info.width;
	  }
	}

	XtSetArg(arg[0], XtNwidth, mwd + 10);
	XtSetArg(arg[1], XtNjustify, XtJustifyRight);

	if (mwd > 0)
	  for (i = 0; menuLabels[i]; ++i)
	    if (XtClass(menuLabels[i]) == commandWidgetClass)
	      XtSetValues(menuLabels[i], arg, 2);
      }

      for (i = lab;
	   i < MAX_LABELS && labels[i] && XtIsManaged(labels[i]); ++i);

      if (i > lab) XtUnmanageChildren(labels+lab, i-lab);
      if (lab > 0)   XtManageChildren(labels,       lab);

      return 0;
    }

  /*
  fprintf(stderr,"Text \"%s\", highlight flag %d and menuP %d\n",
  text, hl, MenuP);
  */

  if     ((nwl = strlen(text)) == 0) return 0;
  while   (nwl  > 0 && text[nwl-1] == END_OF_LINE) text[--nwl] = 0;
  if      (nwl == 0) { nln = True; return 0; }
  if      (MenuP) {
    while (*text == ' ' && *(text+1) == ' ') { text ++; nwl --; }
    nln = hl;
 
  } else if (nwl > 3 && (strchr(text,'_') != NULL)) {

    for (swl = nwl;
	 swl >  0  && (isspace(text[swl-1]) || text[swl-1] == '_'); --swl);
    if  (swl == 0)
      if (helpMode == YHelpXmplMode) { helpMode = YHelpTextMode; return 0; }
      else                           { helpMode = YHelpXmplMode; return 0; }
  }

  for (i = 0; text[i]; ++i) if (text[i] == END_OF_LINE) text[i] = ' ';

  if (!MenuP) {

    XTextExtents(hl ? yHelpXrefFont : GetFontForMode(),
		 text, nwl, &dir, &asc, &dsc, &info);

    wwd = (unsigned)(info.width) + (hl ? LABEL_SPACE : 0);

    if (pll + wwd > helpTextWd) nln = True;
    if (nln) pll  = wwd;
    else     pll += wwd;
  }

  Try(lab = MakeWidget(lab, text, hl, nln));
  nln = False;

  return 0;

 oops: return YHelpError("Cannot construct text display widgets");
}



int DisplayPage(long adr, Boolean routeFlag)
{
  int     state;
  char    c;
  char    pch;
  char    ignore;
  char    newl[2];
  char   *sprBuf;
  Arg     arg;

  helpMode = YHelpHeadMode;

  Try(PlaceText(NULL, True));
  if (fseek(helpFile, adr, 0))
    return YHelpError2d("Can't find node at", (int)adr);

  pch      = 0;
  ignore   = 0;
  state    = 0;
  bPoint   = 0;
  nrTable  = 0;
  next[0]  = 0;
  prev[0]  = 0;
    up[0]  = 0;

  switch(displayMode) {

  case YHelpRouteDisplay:

    XtSetArg(arg, XtNlabel, "History");
    XtSetValues(showButton, &arg, 1);
    break;

  case YHelpSearchDisplay:

    XtUnmanageChild(searchLabel1);
    XtUnmanageChild(searchLabel2);
    XtUnmanageChild(searchText);

    XtUnmanageChild(searchGo);
    XtUnmanageChild(searchGuess);

    XtUnmanageChild(XtParent(searchGo));
    XtUnmanageChild(XtParent(searchGuess));

    XtManageChild(XtParent(prevButton));
    XtManageChild(XtParent(upButton));
    XtManageChild(XtParent(nextButton));

    XtSetArg(arg, XtNlabel, "Search");
    XtSetValues(searchButton, &arg, 1);
    break;

  case YHelpTextDisplay:
    break;
  }

  displayMode = YHelpTextDisplay;

  XtSetArg(arg, XtNwidth, &helpTextWd);
  XtGetValues(textViewport, &arg, 1);
  if  (helpTextWd > 100) helpTextWd -= 30 + 2*LABEL_SPACE;
  else helpTextWd = 60;

 loop:

  Try(ReadTag());

  if        (!strcmp(tag, FILE_TAG)) {
    Try(ReadString(tag )); goto loop;
  } else if (!strcmp(tag, NODE_TAG)) {
    Try(ReadString(node)); goto loop;
  } else if (!strcmp(tag, NEXT_TAG)) {
    Try(ReadString(next)); goto loop;
  } else if (!strcmp(tag, PREV_TAG)) {
    Try(ReadString(prev)); goto loop;
  } else if (!strcmp(tag,   UP_TAG)) {
    Try(ReadString(up  ));
  } else (void)YHelpError2s("Unknown or unexpected node tag:",tag);

  Try(YHelpInstantiateNodeLabels());
  if (routeFlag) Try(YHelpInstantiateRoutedNode());

  XtUnmapWidget(menuViewport);
  XtUnmapWidget(textViewport);

  XSync(XtDisplay(textViewport), False);

  while ((c = ReadChar()) != (char)-1 && c != END_OF_PAGE) {

    if (ignore && c == ignore) {
      if (c == '.') ignore = ']';
      else          ignore =  0;
      continue;
    }

    switch(c) {

    case '*':

      if (bPoint > 0 && 
	  (isprint(buffer[bPoint-1]) && buffer[bPoint-1] != '[')) {

	buffer[bPoint++] = c;
	break;

      } else {

	if (bPoint > 0)
	  if (buffer[bPoint-1] == '[') -- bPoint;
	  else if (isalnum(buffer[bPoint-1]) ||
		   buffer[bPoint-1] == '*'   ||
		   buffer[bPoint-1] == ')') {

	    buffer[bPoint++] = c;
	    state = 0;
	    break;
	  }

	buffer[bPoint] = 0;
	if (bPoint > 0) Try(PlaceText(buffer, False));

	state  = 2;
	bPoint = 0;
	break;
      }

    case ':':

      if      (state == 2) {
	state = 4;

	if (!MenuP &&
	    (!strncasecmp(buffer, "menu",bPoint) ||
	     !strncasecmp(buffer," menu",bPoint))) {

	  Try(PlaceText(NULL, False));
	  helpMode = YHelpMenuMode;
	  Try(PlaceText(NULL,  True));
	}
      }
      else if (state == 4) {

	sprBuf = buffer;
	buffer[bPoint] = 0;
	if (bPoint > 4 && !strncasecmp(buffer, "note ", 5)) sprBuf += 5;
	if (bPoint > (sprBuf - buffer)) Try(PlaceText(sprBuf, True));
	state  =  0;
	bPoint =  0;
	ignore = '.';
      }
      else buffer[bPoint++] = c;

      break;

    case END_OF_LINE:

      if ((pch      == END_OF_LINE)  ||
	  (state    == 0 && MenuP)   ||
	  (helpMode == YHelpXmplMode)) {

	buffer[bPoint] = 0;
	if (bPoint > 0) Try(PlaceText(buffer, False));
	  
	bPoint  = 0;	
	newl[0] = END_OF_LINE;
	newl[1] = 0;
	Try(PlaceText(newl, False));

	if (helpMode != YHelpXmplMode && !MenuP) {

	  newl[0] = ' ';
	  newl[1] = 0;
	  Try(PlaceText(newl, False));
	  newl[0] = END_OF_LINE;
	  newl[1] = 0;
	  Try(PlaceText(newl, False));
	}
	break;
	
      } else {

	if (state == 0) {
	  buffer[bPoint++] = ' ';
	  buffer[bPoint  ] = 0;
	  Try(PlaceText(buffer, False));
	  bPoint = 0;
	} else buffer[bPoint++] = ' ';
	break;
      }

    default:

      buffer[bPoint++] = c;

      if (!((state == 2 && bPoint < NAME_LENGTH) &&
	    (isalnum(c) ||
	     c == ' ' || c == '?' || c == '_' || c == '-' ||
	     c == ':' || c == '.' || c == '(' || c == ')' ||
	     c == '!' || c == '%' || c == '^' ||
	     (c == '*' && bPoint > 0 &&
	      (isprint(buffer[bPoint-1]) && buffer[bPoint-1] != '[')) ||
	     (c == ' ' && bPoint > 0)))) state = 0;

      if (isspace(c) && state == 0 &&
	  bPoint > 1 && !isspace(buffer[bPoint-2])) {

	buffer[bPoint] = 0;
	Try(PlaceText(buffer, False));
	bPoint = 0;
      }
    }
    pch = c;
  }

  if (bPoint > 0) Try(PlaceText(buffer, False));
  Try(PlaceText(NULL, False));
  if (!MenuP) {
    helpMode    = YHelpMenuMode;
    Try(PlaceText(NULL,  True));
    Try(PlaceText(NULL, False));
    menuPaneHt  = 0;
  }

  Try(SizePanes());

  return 0;

 oops:

  return YHelpError("Failed to read and display page data correctly");
}


int DisplayChoices(String comment, YHelpSearchPage *choice)
{
  YHelpSearchPage *current = 0;
  char            *text;

  helpMode   = YHelpHeadMode;

  Try(PlaceText(NULL,  True));
  Try(PlaceText(NULL, False));

  text       = XtNewString(comment);
  menuPaneHt = 0;
  helpMode   = YHelpMenuMode;

  Try(PlaceText(NULL,  True));
  Try(PlaceText(text, False));
  XtFree(text);

  for (current = choice; current; menuPaneHt ++, current = current->next) {

    text = XtNewString(current->page);
    Try(PlaceText(text, True));
    XtFree(text);

    text = XtNewString(current->comment);
    Try(PlaceText(text, False));
    XtFree(text);
  }

  Try(PlaceText(NULL, False));
  Try(SizePanes());
 return 0;

 oops:
  if (current) {
    return YHelpError2s("Choice list display failed, next item",current->page);
  } else {
    return YHelpError("Choice list display failed before any item displayed");
  }
}


/*ARGSUSED*/
void YHelpQuitButton(Widget w, XtPointer a, XtPointer b)
{
  int i;

  if (guessMode && displayMode == YHelpSearchDisplay) SearchGuess(w,a,b);

  XtPopdown(helpSh);

  for (i = 0; textLabels[i]; ++i) XtDestroyWidget(textLabels[i]);
  for (i = 0; menuLabels[i]; ++i) XtDestroyWidget(menuLabels[i]);

  XtDestroyWidget(helpSh);
  helpInstalled = False;
  if (exitcallback) exitcallback();
  return;
}


/*ARGSUSED*/
void YHelpDoneButtonAction(Widget w, XEvent *event, String *a, Cardinal *b)
{
  YHelpQuitButton(w, 0, 0);
}


/* ARGSUSED */
void YHelpHelpButton(Widget w, XtPointer a, XtPointer b)
{
  if (helpFile && helpInstalled) YHelpSetTopic(helpHelp);
}


/*ARGSUSED*/
void YHelpShowButton(Widget w, XtPointer a, XtPointer b)
{
  Arg arg;

  switch(displayMode) {

  case YHelpRouteDisplay:

    XtSetArg(arg, XtNlabel, "History");
    XtSetValues(showButton, &arg, 1);

    Try(DisplayPage(actNode, False));

    return;

  case YHelpSearchDisplay:

    XtUnmanageChild(searchLabel1);
    XtUnmanageChild(searchLabel2);
    XtUnmanageChild(searchText);

    XtUnmanageChild(searchGo);
    XtUnmanageChild(searchGuess);

    XtUnmanageChild(XtParent(searchGo));
    XtUnmanageChild(XtParent(searchGuess));

    XtManageChild(XtParent(prevButton));
    XtManageChild(XtParent(upButton));
    XtManageChild(XtParent(nextButton));

    XtSetArg(arg, XtNlabel, "Search");
    XtSetValues(searchButton, &arg, 1);

    /* deliberately fall through */

  case YHelpTextDisplay:

    displayMode = YHelpRouteDisplay;

    helpPaneHt = 0;
    Try(DisplayChoices("\nPages visited so far:\n\n",helpRout));

    XtSetArg(arg, XtNlabel, "Show Current Page");
    XtSetValues(showButton, &arg, 1);

    return;
  }

 oops:
  (void)YHelpError("Cannot display Show-Route page");
  return;
}


void YHelpClose(void)
{
  if (helpInstalled) YHelpQuitButton(quitButton, 0, 0);
  return;
}


/*ARGSUSED*/
void YHelpPrevButton(Widget w, XtPointer a, XtPointer b)
{
  if (strlen(prev) == 0) {

    XBell(XtDisplay(referenceWidget), 50);
    return;
  }

  TrL(actNode = FindNode(prev));
  Try(DisplayPage(actNode, True));
  return;

 oops: (void)YHelpError2s("Problem in displaying help page for",prev);
  return;
}


/*ARGSUSED*/
void YHelpNextButton(Widget w, XtPointer a, XtPointer b)
{
  if (strlen(next) == 0) {

    XBell(XtDisplay(referenceWidget), 50);
    return;
  }

  TrL(actNode = FindNode(next));
  Try(DisplayPage(actNode, True));
  return;

 oops: (void)YHelpError2s("Problem in displaying help page for",next);
  return;
}


/*ARGSUSED*/
void YHelpUpButton(Widget w, XtPointer a, XtPointer b)
{
  if (strlen(up) == 0 || !strcmp(up,"(dir)")) {

    XBell(XtDisplay(referenceWidget), 50);
    return;
  }

  TrL(actNode = FindNode(up));
  Try(DisplayPage(actNode, True));
  return;

 oops: (void)YHelpError2s("Problem in displaying help page for",up);
  return;
}


/*ARGSUSED*/
void YHelpMenuButton(Widget w, XtPointer a, XtPointer b)
{
  int   i;
  int   n = -1;
  Arg   arg;
  char *xref;

  for (i = 0; i < MAX_LABELS && menuLabels[i]; ++i)
    if (menuLabels[i] == w) { n = i; break; }

  if (n == -1) {
    (void)YHelpError("Unknown and undereferenceable menu button pressed");
    return;
  }

  XtSetArg(arg, XtNlabel, &xref);
  XtGetValues(menuLabels[n], &arg, 1);
  while (*xref && isspace(*xref)) xref ++;

  TrL(actNode = FindNode(xref));
  Try(DisplayPage(actNode, True));
  return;

 oops: (void)YHelpError2s("Problem in displaying help page for",xref);
  return;
}



/*ARGSUSED*/
void YHelpXrefButton(Widget w, XtPointer a, XtPointer b)
{
  int   i;
  int   n = -1;
  Arg   arg;
  char *xref;

  for (i = 0; i < MAX_LABELS && textLabels[i]; ++i)
    if (textLabels[i] == w) { n = i; break; }

  if (n == -1) {
    (void)YHelpError("Unknown and undereferenceable xref button pressed");
    return;
  }

  XtSetArg(arg, XtNlabel, &xref);
  XtGetValues(textLabels[n], &arg, 1);
  while (*xref && isspace(*xref)) xref ++;

  TrL(actNode = FindNode(xref));
  Try(DisplayPage(actNode, True));
  return;

 oops: (void)YHelpError2s("Problem in displaying help page for",xref);
  return;
}



int YHelpInstallHelp(void)
{
  Arg    arg;
  char  *fL;
  Atom   wmPrtcls[2];
  static XtActionsRec yHelpActions[] = {
    { "y-help-wm-quit",    YHelpDoneButtonAction  },
    { "y-help-contextual", YHelpContextHelpAction },
  };

  if (helpInstalled) {

    XMapRaised(XtDisplay(referenceWidget),XtWindow(helpSh));
    return 0;

  } else {

    Try(YHelpCreateWidgets());
    XtAddCallback(   quitButton, XtNcallback,   YHelpQuitButton, 0);
    XtAddCallback( searchButton, XtNcallback, YHelpSearchButton, 0);
    XtAddCallback(   showButton, XtNcallback,   YHelpShowButton, 0);
    XtAddCallback(   prevButton, XtNcallback,   YHelpPrevButton, 0);
    XtAddCallback(     upButton, XtNcallback,     YHelpUpButton, 0);
    XtAddCallback(   nextButton, XtNcallback,   YHelpNextButton, 0);

    XtAppAddActions(XtWidgetToApplicationContext(helpSh),
		    yHelpActions, XtNumber(yHelpActions));

    wmPrtcls[0] =
      XInternAtom(XtDisplay(helpSh), "WM_DELETE_WINDOW", False);
    wmPrtcls[1] =
      XInternAtom(XtDisplay(helpSh), "WM_SAVE_YOURSELF", False);
    XtOverrideTranslations
      (helpSh,
       XtParseTranslationTable("<Message>WM_PROTOCOLS: y-help-wm-quit()"));
    XSetWMProtocols(XtDisplay(helpSh), XtWindow(helpSh), wmPrtcls, 2);

    if (helpFile == NULL || topNode == -1) {

      if (_yInitialised) {
	(void)YQuery(referenceWidget,
		     "Sorry, help is not available at the moment.",
		     1, 0, 0, "Cancel", NULL);
      } else {
	/* not _quite_ sure how you'd get here, but... */
	fprintf(stderr, "Sorry, help is not available at the moment.\n");
      }

      YHelpClose();
      return YHelpError
	(helpFile ? "Can't find Top node\n" : "No help available\n");
    }

    helpInstalled = True;
    if (actNode == -1) actNode = topNode;

    XtPopup(helpSh, XtGrabNone);
    XMapRaised(XtDisplay(helpSh), XtWindow(helpSh));

    fL = (char *)XtMalloc(strlen(hefiName) + 10);
    sprintf(fL, "File: %s", hefiName);
    XtSetArg(arg, XtNlabel, fL);
    XtFree(fL);

    Try(DisplayPage(actNode, True));
    return 0;
  }

 oops: YHelpClose(); return YHelpError("Can't install Help facility");
}


int YHelpSetTextFont(String fn)
{
  if ((yHelpTextFont= EnsureFont(XLoadQueryFont(XtDisplay(referenceWidget),fn)))
      == NULL) goto oops;

  if (helpInstalled) Try(DisplayPage(actNode, True));
  return 0;

 oops: return YHelpError2s("Cannot load default font",DEFAULT_FONT);
}  


int YHelpSetXrefFont(String fn)
{
  if ((yHelpXrefFont=EnsureFont(XLoadQueryFont(XtDisplay(referenceWidget),fn)))
      == NULL) goto oops;

  if (helpInstalled) Try(DisplayPage(actNode, True));
  return 0;

 oops: return YHelpError2s("Cannot load default font",DEFAULT_FONT);
}  


int YHelpSetTitleFont(String fn)
{
  if ((yHelpHeadFont= EnsureFont(XLoadQueryFont(XtDisplay(referenceWidget),fn)))
      == NULL) goto oops;

  if (helpInstalled) Try(DisplayPage(actNode, True));
  return 0;

 oops: return YHelpError2s("Cannot load default font",DEFAULT_FONT);
}


int YHelpSetVerbatimFont(String fn)
{
  if ((yHelpXmplFont= EnsureFont(XLoadQueryFont(XtDisplay(referenceWidget),fn)))
      == NULL) goto oops;

  if (helpInstalled) Try(DisplayPage(actNode, True));
  return 0;

 oops: return YHelpError2s("Cannot load default font",DEFAULT_FONT);
}


int YHelpSetTextFontStruct(XFontStruct *fn)
{
  yHelpTextFont = fn;
  if (helpInstalled) Try(DisplayPage(actNode, True));
 oops: return 0;
}  


int YHelpSetXrefFontStruct(XFontStruct *fn)
{
  yHelpXrefFont = fn;
  if (helpInstalled) Try(DisplayPage(actNode, True));
 oops: return 0;
}  


int YHelpSetTitleFontStruct(XFontStruct *fn)
{
  yHelpHeadFont = fn;
  if (helpInstalled) Try(DisplayPage(actNode, True));
 oops: return 0;
}  


int YHelpSetVerbatimFontStruct(XFontStruct *fn)
{
  yHelpXmplFont = fn;
  if (helpInstalled) Try(DisplayPage(actNode, True));
 oops: return 0;
}  


char *YHelpGetHelpFile(void)
{
  return hefiName;
}


void YHelpSetIdentifyingBitmap(Pixmap bitmap)
{
  yHelpIdentifyingBitmap = bitmap;
}


int YHelpInitialise(Widget parent, String name, String (*callback)(void),
		    void (*exitcb)(void), String helponhelp)
{
  Display * d;
  Window    w;

  if (helpName) XtFree(helpName);
  if (helpHelp) XtFree(helpHelp);

  referenceWidget = parent;
  guess           = callback;
  exitcallback    = exitcb;
  helpName        = XtNewString(name);
  helpRout        = NULL;
  helpHelp        = helponhelp ? XtNewString(helponhelp) : NULL;

  d = XtDisplay(parent);
  w = RootWindowOfScreen(XtScreen(parent));

  rMap = XCreateBitmapFromData(d, w, right_bits, right_width, right_height);
  lMap = XCreateBitmapFromData(d, w,  left_bits,  left_width,  left_height);
  uMap = XCreateBitmapFromData(d, w,    up_bits,    up_width,    up_height);

  return 0;
}



/* The code that follows implements the Search facility.  We construct an  */
/* unbalanced binary search tree containing every significant word in the  */
/* Help file.  For each word there is a list of all nodes that refer to    */
/* the word, with a count of the number of times each node refers to it.   */
/* This list is kept in alphabetical order (not particularly useful!)      */
/* There is also a list of "insignificant words", which are to be ignored. */
/* The search is case-insensitive, and all words are stored in lower-case. */
/* We presume that the helpfile is known to be open, seekable &c.          */



YHelpSearchPage *MakeSearchPage(String cpage, int stress)
{
  YHelpSearchPage *page;

  page = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));
  page->frequency = stress;
  page->page      = XtNewString(cpage);
  page->next      = NULL;

  return page;
}


YHelpSearchTree *MakeSearchNode(String cpage, String word, int stress)
{
  YHelpSearchTree *n;
  int              copy;

  n =  (YHelpSearchTree *)XtMalloc(sizeof(YHelpSearchTree));
  n->string     = (char *)XtMalloc(strlen(word) + 1);
  n->references = MakeSearchPage(cpage,stress);
  n->inferior   = NULL;
  n->superior   = NULL;

  for (copy = 0; word[copy]; ++copy) {
    (n->string)[copy] =
      isupper(word[copy]) ? tolower(word[copy]) : word[copy];
  } (n->string)[copy] = 0;

  return n;
}


int StoreInGivenNode(String cpage, YHelpSearchTree *n, int stress)
{
  YHelpSearchPage *curr;
  YHelpSearchPage *pre;
  int              cmp;

  curr = n->references;
  pre = NULL;

  while (curr && ((cmp = strcmp(curr->page,cpage)) < 0)) {
    pre = curr;
    curr = curr->next;
  }

  if (curr && !cmp) curr->frequency += stress;

  else {

    if (pre) pre->next = MakeSearchPage(cpage,stress);
    else n->references = MakeSearchPage(cpage,stress);

    if (curr)
      if (pre) pre->next->next = curr;
      else n->references->next = curr;
  }

  return 0;
} 


int StoreInGivenTree(String cpage, String word,
		     YHelpSearchTree *tree, int stress)
{
  int cmp;

  if ((cmp = strcasecmp(word, tree->string)))
    if (cmp < 0)
      if (tree->inferior)
	return StoreInGivenTree(cpage,word,tree->inferior,stress);
      else return (tree->inferior = MakeSearchNode(cpage,word,stress), 0);
    else
      if (tree->superior)
	return StoreInGivenTree(cpage,word,tree->superior,stress);
      else return (tree->superior = MakeSearchNode(cpage,word,stress), 0);
  else
    return StoreInGivenNode(cpage,tree,stress);
}


int StoreMenuDescription(String cpage, String comment)
{
  YHelpSearchPage *current;
  YHelpSearchPage *pre;

  for (pre = NULL, current = comments; current;
       pre = pre ? pre->next : comments, current = current->next)
    if (!strcmp(cpage,current->page)) return 0;

  if (pre)
    current = pre->next= (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));
  else
    current = comments = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));

  current->page    = XtNewString(cpage);
  current->comment = XtNewString(comment);
  current->next    = NULL;

  return 0;
}


int StoreSearchWord(String cpage, String word, Boolean accinit,
		    int stress, Boolean menuP)
{
  static Boolean accept = True;
  static int     cix    = 0;
  static int     eix    = 0;
  static char    comment[NAME_LENGTH];
  static char    lastEnt[NAME_LENGTH];
  char         **currIW;
  int            i;
  int            j;

  /* If word only contains underscores (several), reject and flip accept */
  
  if (accinit) accept = True;
  else {

    for (i = j = 0; word[i] && word[i] == '_'; ++i, ++j);
    if (!word[i] && j > 3) {
      accept = !accept;
      return 0;
    }
  }

  if (!accept) return 0;

  /* If this is a menu item description, build it, or consider */
  /* storing a built description for saving later              */

  if (menuP)

    if (stress == 1) {

      if (cix + (i = strlen(word)) >= NAME_LENGTH - 2) {
	comment[cix = 0] = 0;
	return YHelpError2s("Menu description overflow, word",word);
      }

      if (cix > 0) comment[cix++] = ' ';
      strcpy(comment + cix, word);
      cix += i;

    } else {

      if (cix > 0) {
	Try(StoreMenuDescription(lastEnt,comment));
	comment[cix = 0] = 0;
	lastEnt[eix = 0] = 0;
      }

      if (eix + (i = strlen(word)) >= NAME_LENGTH - 2) {
	lastEnt[eix = 0] = 0;
	return YHelpError2s("Menu entry overflow, word",word);
      }

      if (i > 0) {
	if (eix > 0) lastEnt[eix++] = ' ';
	strcpy(lastEnt + eix, word);
	eix += i;
      }
    }

  /* Reject any word in the Insignificant Word List */

  for (currIW = insignificantWords; currIW && *currIW; ++currIW)
    if (!strcasecmp(word,*currIW)) return 0;

  /* Reject any word not containing alphabetical characters */

  for (i = 0; word[i] && !isalpha(word[i]); ++i);
  if (!word[i]) return 0;

  /* Store word */

  if (searchBT) {
    Try(StoreInGivenTree(cpage,word,searchBT,stress));
  } else searchBT = MakeSearchNode(cpage,word,stress);

  return 0;

 oops: return YHelpError("Couldn't store word information in search tree");
}


/* It's a crap bubble sort!! */

YHelpSearchPage *YHelpSortRefList(YHelpSearchPage *p)
{
  Boolean         finished = False;
  YHelpSearchPage *pre;
  YHelpSearchPage *cur;

  while (!finished)

    for  (cur = p, finished = True, pre = NULL;
	  cur && cur->next; pre = cur, cur = cur->next)

      if (cur->frequency < cur->next->frequency) {

	if (pre) pre->next = cur->next; else p = cur->next;
	cur->next = cur->next->next;
	if (pre) pre->next->next = cur; else p->next = cur;
	finished = False;
      }

  return p;
}


int YHelpTrimSearchTree(YHelpSearchTree *n)
{
  YHelpSearchPage *refs;
  YHelpSearchPage *prefs = NULL;

  if (n) {

    if (n->inferior) Try(YHelpTrimSearchTree(n->inferior));

    refs = n->references;

    while (refs)

      if (refs->frequency < 2) {

	if (prefs) prefs->next = refs->next;
	else  n->references = refs->next;

	XtFree(refs->page);
	XtFree((char *)refs);

	if (prefs) refs = prefs->next;
	else       refs = n->references;

      } else {

	prefs = refs;
	refs  = refs->next;
      }

    if (n->references) n->references= YHelpSortRefList(n->references);
    if (n->superior) Try(YHelpTrimSearchTree(n->superior));
  }

  return 0;

 oops: return YHelpError("Couldn't trim search tree");
}	  


int YHelpReinstallNode(YHelpSearchTree *n, YHelpSearchTree *parent)
{
  if (strcasecmp(n->string,parent->string) < 0)

    if  (parent->inferior) return YHelpReinstallNode(n,parent->inferior);
    else parent->inferior = n;

  else

    if  (parent->superior) return YHelpReinstallNode(n,parent->superior);
    else parent->superior = n;

  return 0;
}


int YHelpDeleteNode(YHelpSearchTree *n, YHelpSearchTree *parent)
{
  if (parent) {

    if (n == parent->inferior) parent->inferior = NULL;
    else                          parent->superior = NULL;

    if (n->inferior) Try(YHelpReinstallNode(n->inferior,parent));
    if (n->superior) Try(YHelpReinstallNode(n->superior,parent));

  } else {

    searchBT = n->inferior;

    if (n->superior) {
      searchBT = n->superior;
      if (n->inferior)
	Try(YHelpReinstallNode(n->inferior,n->superior));
    }
  }

  if (n->references) XtFree((char *)(n->references));
  XtFree(n->string);
  XtFree((char *)n);

  return 0;

 oops: return YHelpError("Couldn't delete node in search tree");
}


int YHelpDeleteEmptyNodes(YHelpSearchTree *n, YHelpSearchTree *parent)
{
  if (n) {

    if (n->inferior) Try(YHelpDeleteEmptyNodes(n->inferior,n));
    if (n->superior) Try(YHelpDeleteEmptyNodes(n->superior,n));
    if (n->references == NULL) Try(YHelpDeleteNode(n,parent));
  }

  return 0;

 oops: return YHelpError("Couldn't delete unwanted nodes from search tree");
}


/* A somewhat tagliatelle-alla-carbonaraesque function, possibly  */
/* accompanied by cheap Lambrusco.  Supposed to build up the tree */
/* of references for each word in the help file.  Relies largely  */
/* on the equally pasta-inclined function StoreSearchWord, some   */
/* pages overhead.  This whole machine badly needs rewriting;     */
/* at the moment it appears to be wobbly but working.             */

int YHelpBuildSearchTree(void)
{
  char    current[NAME_LENGTH];
  char    c;
  int     i, j;
  int     nextstress = 1;
  long    cct = 0;
  Boolean menuP;

  if (fseek(helpFile, 0, 0)) return YHelpError("Can't rewind help file");
  Try(SkipChar(END_OF_PAGE));

 getpage:

  Try(SkipChar(END_OF_LINE));

 gettags:

  Try(ReadTag());

  if        (!strcmp(tag,  TOC_TAG)) {
    Try(   YHelpTrimSearchTree(searchBT));
    Try( YHelpDeleteEmptyNodes(searchBT, NULL)); /* at least, I think NULL */
    return 0;
  }else if (!strcmp(tag, FILE_TAG)) {
    Try(ReadString(buffer));
    goto gettags;
  } else if (!strcmp(tag, NODE_TAG)) {
    Try(ReadString(current));
    for (i = j = 0; current[i]; ++i)
      if (isspace(current[i])) {
	if (i > j) {
	  c = current[i];
	  current[i] = 0;
	  Try(StoreSearchWord(current,current+j,True,nextstress,False));
	  current[i] = c;
	} j = i;
      }
  } else (void)YHelpError2s("Unknown or unexpected node tag:",tag);

  Try(SkipChar(END_OF_LINE));
  if (writingSearch) fprintf(stderr,"[%ld] ",++cct);
  menuP = False;

 getword:

  bPoint = 0;

  while (bPoint < MAX_BUFFER &&
	 (c = ReadChar()) != (char)-1 &&
	 (isalnum(c) || c == '!' || c == '\\' || c == '_' ||
	  (c == '-' && bPoint > 0) ||
	  (!isspace(c) && bPoint > 0 &&
	   (buffer[bPoint-1] == '!' ||
	    buffer[bPoint-1] == '\\')))) buffer[bPoint++] = c;

  while (bPoint > 0 &&
	 (buffer[bPoint-1] == '\\' || 
	  (buffer[bPoint-1] == '!' &&
	   (bPoint == 1 || buffer[bPoint-2] != '!')))) --bPoint;

  if (bPoint > 0) {
    buffer[bPoint] = 0;
    if (bPoint >= MAX_BUFFER) return YHelpError2s("Word too long:",buffer);
    Try(StoreSearchWord(current,buffer,False,nextstress,menuP));
  }

  if (nextstress == 2 && c == ':') nextstress = 1;

  if (bPoint == 0 && c == '*') {

    /* parentheses added around the last logical OR in the following
       conditional, cc-1/96 -- I'm not certain that that's what's
       intended; it isn't what the conditional actually said before */

    while (bPoint < MAX_BUFFER && (c = ReadChar()) != (char)-1 &&
	   (isalnum(c) || (!menuP && isspace(c) && bPoint == 0)))
      if (!isspace(c)) buffer[bPoint++] = c;

    if (menuP || !strncasecmp(buffer, "note", bPoint)) nextstress = 2;
    else     if (!strncasecmp(buffer, "menu", bPoint)) menuP = True;
  }

  if (c == END_OF_PAGE) {
    if (menuP) StoreSearchWord(current,"",True,2,menuP);
    goto getpage;
  } else goto getword;

 oops: return YHelpError("Couldn't build help search tree");
}


/* Write the help index file.  The format is as follows: the first line */
/* contains the fseek index of the table of contents in the help file;  */
/* the rest of the file contains entries formatted like this one:

vector
"MASS" 8
"G" 7
"INDEX" 6
"DOT_SIGN" 5
"High Energy Physics section" 3
"EPS" 2

   with succeeding entries separated by one or more blank lines.  The   */
/* fields present here show that the word "vector" is found in the page */
/* called "MASS", with a priority of 8, "G", with a priority of 7, and  */
/* so on.  The lines are sorted by priority number.                     */


int YHelpWriteTreeToIndexFile(YHelpSearchTree *tree)
{
  YHelpSearchPage *page;
  YHelpSearchPage *comm;
  Boolean         gotC;

  if (tree) {

    if (tree->inferior) Try(YHelpWriteTreeToIndexFile(tree->inferior));

    fprintf(helpSFil, "\n%s\n", tree->string);
    for (page = tree->references; page; page = page->next) {

      for (comm = comments, gotC = False; comm; comm = comm->next)
	if (!strcmp(page->page, comm->page)) {
	  fprintf(helpSFil, "\"%s\" \" %s\" %d\n",
		  page->page, comm->comment, page->frequency);
	  gotC = True;
	  break;
	}
      if (!gotC) fprintf(helpSFil, "\"%s\" \" \" %d\n",
			 page->page, page->frequency);
    }

    if (tree->superior) Try(YHelpWriteTreeToIndexFile(tree->superior));
  }

  return 0;

 oops: return YHelpError("Couldn't traverse tree properly");
}


int YHelpWriteIndexFile(void)
{
  if (TOCNode < 0) return YHelpError("No reference for TOC found");
  if (!hesfName)   return YHelpError("No name for help index file available");

  if (helpSFil) fclose(helpSFil);
  if (!(helpSFil = fopen(hesfName, "w")))
    return YHelpError2s("Cannot open help index file",hesfName);

  writingSearch = True;

  fprintf(stderr,"\nThis is the XR info-based help module.\n");
  fprintf(stderr,"\nWriting help index file \"%s\"...\n", hesfName);
  fprintf(stderr,"Building search tree from \"%s\"...\n", hefiName);

  Try(YHelpBuildSearchTree());

  fprintf(stderr,"\nWriting search tree to index file...\n");

  rewind(helpSFil);
  fprintf(helpSFil,"%ld\n",TOCNode);
  Try(YHelpWriteTreeToIndexFile(searchBT));
  fprintf(helpSFil,"\n\n::END::\n\n");
  fflush(helpSFil);

  fprintf(stderr,"Finished writing search tree.\n");
  writingSearch = False;

  if (!(helpSFil = fopen(hesfName, "r")))
    return YHelpError2s("Cannot open help index file",hesfName);

  return 0;

 oops: return YHelpError("Couldn't write index file");
}



YHelpSearchPage *Get1WordSearchPage(String word)
{
  char            c, p;
  char            check[NAME_LENGTH];
  char          **currIW;
  YHelpSearchPage *result = NULL;
  YHelpSearchPage *endres = NULL;

  for(currIW = insignificantWords; currIW && *currIW; ++currIW)
    if (!strcasecmp(word,*currIW)) return NULL;

  if (!helpSFil) {
    (void)YHelpError2s("No help index file; can't look up",word);
    return NULL;
  }

  rewind(helpSFil);

  for (check[0] = 0; strcasecmp(check, word); ) {

    for (c = p  = 0; c != END_OF_LINE || p != END_OF_LINE;
	 p = c, fread(&c, 1, 1, helpSFil))
      if (feof(helpSFil) || ferror(helpSFil)) return NULL;
    
    fscanf(helpSFil, "%s\n", check);
  }

 loop:

  fread(&c, 1, 1, helpSFil);
  if (feof(helpSFil) || ferror(helpSFil) || c != '"') return result;

  if (result) {
    endres->next    = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));
    endres = endres->next;
  } else
    endres = result = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));

  endres->page    = (char *)XtMalloc(NAME_LENGTH);
  endres->comment = (char *)XtMalloc(NAME_LENGTH);
  endres->next    = NULL;

  fscanf(helpSFil, "%[^\"]\" \"%[^\"]\" %d\n",
	 endres->page, endres->comment, &(endres->frequency));

  goto loop;
}



YHelpSearchPage *GetStringTitlePage(String str, int frequency)
{
  int             i;
  YHelpSearchPage *result;

  for (i = 0; i < nrNodes && strcasecmp(str, nname[i]); ++i);

  if (i >= nrNodes) return NULL;
  else {

    result = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));
    result->comment   = XtNewString(" ");
    result->page      = XtNewString(nname[i]);
    result->next      = NULL;
    result->frequency = frequency;

    return result;
  }
}    



/* Mergesort.  We have two page reference lists, which we know are each */
/* already sorted in descending order of frequency; we merge into one.  */
/* Discards and frees both the old lists completely.                    */

YHelpSearchPage *MergeSearchPages(YHelpSearchPage *a, YHelpSearchPage *b)
{
  YHelpSearchPage *result = NULL;
  YHelpSearchPage *endres = NULL;
  YHelpSearchPage *t;

  while (a || b) {

    if (result) {
      endres->next    = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));
      endres = endres->next;
    } else
      endres = result = (YHelpSearchPage *)XtMalloc(sizeof(YHelpSearchPage));

    if (a && (!b || (a->frequency > b->frequency))) {

      endres->page      = a->page;
      endres->frequency = a->frequency;
      endres->comment   = a->comment;

      t = a;
      a = a->next;
      XtFree((char *)t);

    } else {

      endres->page      = b->page;
      endres->frequency = b->frequency;
      endres->comment   = b->comment;

      t = b;
      b = b->next;
      XtFree((char *)t);
    }
  }

  if (endres) endres->next = NULL;
  return result;
}



/* We have a page list, and want to weed out repeated pages by summing the  */
/* frequency counts for the repetitions and putting the new larger count    */
/* in just the one of them.  We'll do this simply by doing lots of compares */
/* down the list; this isn't exactly a time-intensive bit of code.          */

/* This function suggests to me that either I should try and write it again */
/* only more neatly, or I should make the page-list doubly linked. Or both. */

YHelpSearchPage *CutRedundancies(YHelpSearchPage *p)
{
  YHelpSearchPage *q;
  YHelpSearchPage *r;
  YHelpSearchPage *i;
  Boolean         ch = False;

  for (q = p; q && q->next; q = q->next)

    for (i = q, r = q->next; r; )

      if (!strcmp(q->page, r->page)) {

	ch            = True;
	q->frequency += r->frequency;
        i->next       = r->next;

	XtFree(r->page);
	XtFree(r->comment);
	XtFree((char *)r);
	r = i->next;

      } else {

	r = r->next;
	i = i->next;
      }

  if (ch) p = YHelpSortRefList(p);
  return p;
}



YHelpSearchPage *GetXWordSearchPage(String ct)
{
  YHelpSearchPage *build = NULL;
  char           *text;
  int             i, j;
  char            c;

  text = XtNewString(ct);
  i = j = 0;
  while (text[i] && isspace(text[i])) ++i;
  if (!text[i]) return NULL; else j = i;

 loop:

  while ((c = text[i]) &&
	 (isalnum(c) || c == '!' || c == '\\' || c == '_' ||
	  (c == '-' && i > j) ||
	  (!isspace(c) && i > j &&
	   (text[i-1] == '!' || text[i-1] == '\\')))) ++i;

  text[i] = 0;

  if (i > j) {

    build = MergeSearchPages(build, Get1WordSearchPage(text + j));
    build = MergeSearchPages(build, GetStringTitlePage(text + j, 500));
  }

  if (c) {
    i = j = i+1;
    goto loop;
  } else {
    XtFree(text);
    return
      CutRedundancies(MergeSearchPages(build, GetStringTitlePage(ct, 1000)));
  }
}



/*ARGSUSED*/
void SearchGo(Widget w, XtPointer a, XtPointer b)
{
  char *word;
  Arg   arg;

  XtSetArg(arg, XtNstring, &word);
  XtGetValues(searchText, &arg, 1);
  lastSearch = True;
  srchList   = GetXWordSearchPage(word);

  if (srchList) {
    Try(DisplayChoices("\nPossible pages, in approximate order:", srchList));
  } else {

    char *comment;

    comment = (char *)XtMalloc(strlen(word) + 50);
    sprintf(comment,"\nNo references found for \"%s\"",word);
    Try(DisplayChoices(comment, srchList));
    XtFree(comment);
  }

 oops: return;
}



/* ARGSUSED */
void SearchGuess(Widget w, XtPointer a, XtPointer b)
{
  Arg          arg;
  static char *otherText;

  if (guessMode) {

    if (otherText) {

      XtSetArg(arg, XtNstring, otherText);
      XtSetValues(searchText, &arg, 1);
      XtFree(otherText);
      otherText = NULL;
    }

    XtSetArg(arg, XtNeditType, XawtextEdit);
    XtSetValues(searchText, &arg, 1);

    XtManageChild(searchGo);
    XtManageChild(XtParent(searchGo));

    XtSetArg(arg, XtNlabel, "Guess Context");
    XtSetValues(searchGuess, &arg, 1);

    guessMode = False;
    SearchGo (searchGo, a, b);
    return;

  } else {

    char *tempText;

    XtSetArg(arg, XtNstring, &tempText);
    XtGetValues(searchText, &arg, 1);
    otherText = XtNewString(tempText);

    if ((tempText = guess()) == NULL) {  /* get text from parent's callback */

      XtFree(otherText);
      otherText = NULL;
      (void)YHelpError("Can't get Search Context (sorry)");
      return;

    } else {

      XtUnmanageChild(searchGo);
      XtUnmanageChild(XtParent(searchGo));

      XtSetArg(arg, XtNstring, tempText);
      XtSetValues(searchText, &arg, 1);

      XtSetArg(arg, XtNeditType, XawtextRead);
      XtSetValues(searchText, &arg, 1);

      XtSetArg(arg, XtNlabel, "Return to User Search");
      XtSetValues(searchGuess, &arg, 1);

      XtFree(tempText);
      guessMode = True;
      SearchGo(searchGo, a, b);
      return;
    }
  }
}



/*ARGSUSED*/
void YHelpSearchButton(Widget w, XtPointer a, XtPointer b)
{
  Arg         arg;
  int         dir;
  int         asc;
  int         dsc;
  XCharStruct info;
  Dimension   slw;

  switch(displayMode) {

  case YHelpSearchDisplay:

    if (guess != NULL) {
 
      guessMode = False;

      XtUnmanageChild(XtParent(searchGuess));
      XtSetArg(arg, XtNlabel, "Guess Context");
      XtSetValues(searchGuess, &arg, 1);
    }

    XtUnmanageChild(searchLabel1);
    XtUnmanageChild(searchLabel2);
    XtUnmanageChild(searchText);
    XtUnmanageChild(searchGo);
    XtUnmanageChild(XtParent(searchGo));

    XtManageChild(XtParent(prevButton));
    XtManageChild(XtParent(upButton));
    XtManageChild(XtParent(nextButton));

    XtSetArg(arg, XtNlabel, "Search");
    XtSetValues(searchButton, &arg, 1);
    XtSetKeyboardFocus(textForm, textForm);

    Try(DisplayPage(actNode, False));

    return;

  case YHelpRouteDisplay:

    XtSetArg(arg, XtNlabel, "History");
    XtSetValues(showButton, &arg, 1);

    /* deliberately fall through */

  case YHelpTextDisplay:

    displayMode = YHelpSearchDisplay;

    YSetValue(searchLabel1, XtNfont,                  yHelpTextFont);
    YSetValue(searchLabel1, XtNlabel,              "Search string:");
    YSetValue(searchText,   XtNfont,                  yHelpTextFont);
    YSetValue(searchText,   XtNeditType,                XawtextEdit);
    YSetValue(searchText,   XtNresize,            XawtextResizeBoth);
    YSetValue(searchLabel2, XtNlabel,                           " ");
    YSetValue(searchButton, XtNlabel,         "Return to Help Page");
    YGetValue(searchLabel1, XtNwidth,                          &slw);

    YSetValue(searchLabel1, XtNvertDistance,                     20);
    YSetValue(searchText,   XtNfromHoriz,              searchLabel1);
    YSetValue(searchText,   XtNvertDistance,                     21);
    YSetValue(searchLabel2, XtNfromVert,               searchLabel1);

    XTextExtents(yHelpTextFont,"[(M0|W",6, &dir, &asc, &dsc, &info);
    YSetValue(searchText, XtNheight, info.ascent + info.descent + 30);
    YSetValue(searchText, XtNwidth, helpTextWd - 300);

    XtUnmanageChild(XtParent(prevButton));
    XtUnmanageChild(XtParent(nextButton));
    XtUnmanageChild(XtParent(upButton));

    if (guess != NULL) {
      XtManageChild(searchGuess);
      XtManageChild(XtParent(searchGuess));
    }

    XtManageChild(searchGo);
    XtManageChild(XtParent(searchGo));

    XtManageChild(searchLabel1);
    XtManageChild(searchLabel2);
    XtManageChild(searchText);
    XtSetKeyboardFocus(textForm, searchText);

    helpPaneHt = 6;

    if (srchList) {
      Try(DisplayChoices("\nPossible pages, in approximate order:\n\n",
			 srchList));
    } else {
      if (lastSearch) {
	Try(DisplayChoices("\nLast search unsuccessful.", srchList));
      } else {
	Try(DisplayChoices("\nNo previous search results.", srchList));
      }
    }

    return;
  }

 oops:
  (void)YHelpError("Cannot display search page");
  return;
}


/* ARGSUSED */
void YHelpContextHelpAction(Widget w, XEvent *event, String *b, Cardinal *c)
{
  if (!helpInstalled) Try(YHelpInstallHelp());
  if (displayMode != YHelpSearchDisplay)
    YHelpSearchButton(searchButton, 0, 0);
  if (guessMode)
    SearchGuess(searchGuess, 0, 0);
  SearchGuess(searchGuess, 0, 0);

  return;

 oops:
  (void)YHelpError("Couldn't get a contextual help page");
  return;
}

