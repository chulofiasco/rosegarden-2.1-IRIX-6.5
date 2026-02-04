
/* Main.c */

/*
   Chris Cannam
   Musical Notation Editor for X
   Main.c :  X Initialisation and Main Loop, plus housekeeping
*/

/* {{{ Includes */

#include <SysDeps.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Core.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <signal.h>
#include <sys/types.h>

#ifdef HAVE_SPECIALIST_MALLOC_LIBRARY
#include <malloc.h>
#endif

#include "General.h"
#include "Resources.h"
#include "Fallbacks.h"
#include "Widgets.h"
#include "Visuals.h"
#include "GC.h"
#include "Menu.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "Yawn.h"

#include <ILClient.h>

/* }}} */
/* {{{ Global definitions */

Display     *display    = NULL;
Widget       topLevel   = NULL;
AppData      appData;
XtAppContext appContext = NULL;

/* }}} */
/* {{{ Exit, Error and Signal code */

/* Normal termination procedure */

void Goodbye(int code)
{
  Begin("Goodbye");

  if (topLevel     && XtIsManaged(topLevel))     XtUnmapWidget(topLevel);
  if (paletteShell && XtIsManaged(paletteShell)) XtUnmapWidget(paletteShell);

   TrackingCleanUp ();
    VisualsCleanUp ();
  StaveEditCleanUp ();
       MenuCleanUp ();    /* also frees all staves */
      StaveCleanUp ();
         GCCleanUp ();
          YCleanUp ();

  if (appData.interlockWindow) {
    IL_DeRegisterWithServer(ILS_EDIT_SERVICE);
    IL_DeRegisterWithServer(ILS_FOLLOW_SERVICE);
  }

  if (appContext) XtDestroyApplicationContext(appContext);

  exit(code);
}



/* Abnormal termination procedure */

void ErrorFastExit(String msg)
{
  Begin("ErrorFastExit");

  fprintf(stderr, "%s: %s.\n", ApplicationName, msg);
  if (appData.interlockWindow) {
    IL_DeRegisterWithServer(ILS_EDIT_SERVICE);
    IL_DeRegisterWithServer(ILS_FOLLOW_SERVICE);
  }

  kill(0, SIGHUP);
  abort();
  /*  exit(-1);*/
}


/* Abnormal termination procedure */

void Error(String msg)
{
  Begin("Error");

  fprintf(stderr, "%s: %s.\n", ApplicationName, msg);
  Goodbye(1);

  End;
}



/* Handler for X warnings (fatal only if not handled) */

int ErrorXNonFatal(Display *d, XErrorEvent *event)
{
  char  buffer[500];
  char *dispname;

  Begin("ErrorXNonFatal");

  XGetErrorText(d, event->error_code, buffer, 500);
  dispname = XDisplayName(NULL);

  fprintf(stderr, "%s: X Warning: %s on display `%s'\n",
	  ApplicationName, buffer, dispname);
  fprintf(stderr,
	  "      [ serial number: 0x%08lx ]  [ major op code: 0x%08x ]\n",
	  event->serial, event->request_code);
  fprintf(stderr,
	  "      [ resource iden: 0x%08lx ]  [ minor op code: 0x%08x ]\n",
	  event->resourceid, event->minor_code);

  Return(0);
}



/* Handler for the fatal X error (connection lost) */

/* ARGSUSED */
int ErrorXIO(Display *d)
{
  Begin("ErrorXIO");

  fprintf(stderr,"%s: The X server connection has been cut, goodbye!\n",
	  ApplicationName);
  if (appContext) XtDestroyApplicationContext(appContext);
  exit(1);

  Return(-1);			/* else some compilers may complain */
}



/* Handler for Toolkit errors, always fatal */

void ErrorXt(char *msg)
{
  Begin("ErrorXt");

  fprintf(stderr,"%s: X Toolkit Fatal Error: %s\n", ApplicationName, msg);
  if (appContext) XtDestroyApplicationContext(appContext);
  if (appData.interlockWindow) {
    IL_DeRegisterWithServer(ILS_EDIT_SERVICE);
    IL_DeRegisterWithServer(ILS_FOLLOW_SERVICE);
  }
  exit(1);
}



void SigBus(void)
{
  Begin("SigBus");
  ErrorFastExit("Bus Error: this shouldn't happen, sorry");
  End;
}



void SigSegV(void)
{
  Begin("SigSegV");
  ErrorFastExit("Segmentation Violation: this shouldn't happen, sorry");
  End;
}



void SigIll(void)
{
  Begin("SigIll");
  ErrorFastExit("Illegal Instruction: this shouldn't happen, sorry");
  End;
}



void SigOther(void)
{
  Begin("SigOther");
  Goodbye(2);
  End;
}



/* Install the signal handlers */

void InstallSignalHandlers(void)
{
  Begin("InstallSignalHandlers");

  signal(SIGBUS,  SigBus);
  signal(SIGSEGV, SigSegV);
  signal(SIGILL,  SigIll);
  signal(SIGHUP,  SIG_IGN);
  signal(SIGINT,  SigOther);
  signal(SIGQUIT, SigOther);

  End;
}



void ILQuit(Boolean confirm)
{
  Begin("ILQuit");
  if (!confirm) appData.interlockWindow = NULL;	     /* don't deregister */
  Goodbye(0);
  End;
}



/* Pop up a query box on WM Quit function  */

void QuitNicely(void)
{
  char message[200];

  Begin("QuitNicely");

  sprintf(message, "Exit the %s: Are you sure?", ApplicationName);
  if (YQuery(topLevel, message, 2, 1, 1, "Yes", "No",
	     "Editor File - Exit") == 0)
    Goodbye(0);

  End;
}


void QuitNicelyAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
  Begin("QuitNicelyAction");
  QuitNicely();
  End;
}


void QuitNicelyCallback(Widget w, XtPointer a, XtPointer b)
{
  Begin("QuitNicelyCallback");
  QuitNicely();
  End;
}

/* }}} */
/* {{{ Title-bar */

void SetTitleBar(char **argv)
{
#ifndef NO_GETHOSTNAME
  char        machine[100];
#endif
  String      title;
  Arg         arg[2];

  Begin("SetTitleBar");

  /* Set title in title bar, with host name if possible */

  title = (String)XtMalloc(200);

#ifdef NO_GETHOSTNAME
  sprintf(title, "%s", ApplicationName);
#else
  if (gethostname(machine, 99))
    sprintf(title, "%s", ApplicationName);
  else
    sprintf(title, "%s  [%s]", ApplicationName, machine);
#endif

  XtSetArg(arg[0], XtNtitle,    (XtArgVal)title);
  XtSetArg(arg[1], XtNiconName, (XtArgVal)"Editor");
  XtSetValues(topLevel, arg, 2);

  End;
}


void ChangeTitleBar(String bname, Boolean changed)
{
  char         *temp;
  static String oldTitle = NULL;
  static char   newTitle[305];
  static char   bufferName[100];

  Begin("ChangeTitleBar");

  strncpy(bufferName, bname, 99);
  bufferName[99] = '\0';

  if (!oldTitle) {
    YGetValue(topLevel, XtNtitle, &temp);
    oldTitle = XtNewString(temp);
  }

  sprintf(newTitle, "%s  %s%s",
	  oldTitle, bufferName, changed ? " (changed)" : "");

  YSetValue(topLevel, XtNtitle,    newTitle);
  YSetValue(topLevel, XtNiconName, bufferName);

  End;
}

/* }}} */
/* {{{ Syntax display */

void DisplaySyntax(void)
{
  Begin("DisplaySyntax");

  fprintf
    (stderr,
     "\nThe %s editor should not normally be run on its own;\n"
     "instead you should start it up from the Rosegarden top-box.\n\n"
     "If, however, you really want to start it from the command-line,\n"
     "you can use any of the generic X Toolkit command-line options,\n"
     "as well as the following.\n\n"
     "  -musicdir       Default directory for loading and saving music\n"
     "  -bigfn          Font used for large above-the-staff text\n"
     "  -littlefn       Font used for small text\n"
     "  -tinyfn         Font used for very small text\n"
     "  -italicfn       Font used for italic below-the-staff text\n"
     "  -tsigfn         Font used when drawing the time signature\n"
     "  -aboutfn        Font used for text in the About boxes\n\n"
     "Each of these options requires one argument.\n\n"
     "For further explanation of these options and of how to use and\n"
     "customise the Rosegarden editor, see the Rosegarden manual.\n",
     ProgramName);

  fprintf(stderr,"\n");
  if (appContext) XtDestroyApplicationContext(appContext);
  exit(0);
}

/* }}} */
/* {{{ Interlock */

void InitialiseILClient(void)
{
  Begin("InitialiseILClient");

  if (!appData.interlockWindow) End;

  IL_ClientInit((Window)atol(appData.interlockWindow), topLevel, ILQuit);
  IL_RegisterWithServer(ILS_EDIT_SERVICE, FileMenuEditILCallback);
  IL_RegisterWithServer(ILS_FOLLOW_SERVICE, FileMenuFollowILCallback);

  End;
}

/* }}} */
/* {{{ Window manager stuff */

/* Record the Xt Actions table */

/* ARGSUSED */
void InstallActions(void)
{
  static XtActionsRec musicActions[] = {
    { "music-wm-quit",   QuitNicelyAction },
    { "palette-wm-quit", QuitNicelyAction },
  };

  Begin("InstallActions");
  XtAppAddActions(appContext, musicActions, XtNumber(musicActions));
  End;
}

void SetWMQuitProperties(void)
{
  Atom wmProtocols[2];

  Begin("SetWMQuitProperties");

  wmProtocols[0] = XInternAtom(XtDisplay(topLevel), "WM_DELETE_WINDOW", False);
/*  wmProtocols[1] = XInternAtom(XtDisplay(topLevel), "WM_SAVE_YOURSELF", False); */

  XtOverrideTranslations
    (topLevel,
     XtParseTranslationTable("<Message>WM_PROTOCOLS: music-wm-quit()"));
  XSetWMProtocols(XtDisplay(topLevel), XtWindow(topLevel), wmProtocols, 2);

  XtOverrideTranslations
    (paletteShell,
     XtParseTranslationTable("<Message>WM_PROTOCOLS: palette-wm-quit()"));
  XSetWMProtocols(XtDisplay(paletteShell), XtWindow(paletteShell),
		  wmProtocols, 1);

  End;
}

/* }}} */
/* {{{ Main */

/* ARGSUSED */
int main(int argc, char **argv, char **envp)
{
  char appName[100];

  Begin("main");

#ifdef HAVE_SPECIALIST_MALLOC_LIBRARY

  (void)mallopt(M_MXFAST, sizeof(Bar) + 2); /* largest common object */

#ifdef   DEBUG
#ifdef M_DEBUG

  fprintf(stderr,"Including malloc() debugging calls\n");
  (void)mallopt(M_DEBUG, 1);

#endif /* M_DEBUG */
#endif /*   DEBUG */
#endif /* HAVE_SPECIALIST_MALLOC_LIBRARY */

  /* Do the Xt initialisation by hand, because we want   */
  /* the application name and class name to be different */

  XtToolkitInitialize();
  appContext = XtCreateApplicationContext();
  XtAppSetFallbackResources(appContext, fallbacks);

  sprintf(appName, "%s", ApplicationName);

  if (!(display = XtOpenDisplay(appContext, NULL, appName, ProgramName,
				commandOptions, XtNumber(commandOptions),
				&argc, (String *)argv)))
    ErrorFastExit("Couldn't open display");

  topLevel = XtAppCreateShell(appName, ProgramName,
			      applicationShellWidgetClass, display, NULL, 0);

  XtGetApplicationResources(topLevel, (XtPointer)&appData, resources,
                            XtNumber(resources), NULL, 0);

  /* Set up the error handlers */
  XSetErrorHandler(ErrorXNonFatal);
  XSetIOErrorHandler(ErrorXIO);
  XtAppSetErrorHandler(appContext, ErrorXt);
  InstallSignalHandlers();

  if (argc > 1) {

    if (strcmp(argv[1], "-help") == 0) DisplaySyntax();
    else {

      fprintf(stderr, "%s: Unknown command line option %s.\n",
	      ProgramName, argv[1]);
      fprintf(stderr, "%s: Type %s -help for a list of recognised options\n",
	      ProgramName, argv[0]);
      if (appContext) XtDestroyApplicationContext(appContext);
      exit(2);
    }
  }

  if (!appData.foundDefaults)
    Error("Sorry, I can't open the application-default file");

  InitialiseStaticObjects();

  /* Set up and install the visible interface */

  SetTitleBar(argv);
  CreateApplicationWidgets();
  InstallActions();
  InitialiseILClient();
  SetWMQuitProperties();

  slaveMode = False;

  XtAppMainLoop(appContext);
  fprintf(stderr,"%s: Panic!  X Toolkit Main Loop failed\n", ProgramName);
  Return(-1);
}

/* }}} */


