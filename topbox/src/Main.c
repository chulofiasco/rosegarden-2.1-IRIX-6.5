
/*

   Chris Cannam

   Final Year Project, 1994
   Musical Notation Editor for X

   Main File for Rosegarden Top Box

*/


#include <SysDeps.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Core.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <signal.h>
#include <sys/types.h>

#include "General.h"
#include "Resources.h"
#include "Fallbacks.h"
#include "Widgets.h"
#include "Visuals.h"
#include "Remote.h"

#include <Yawn.h>
#include <YHelp.h>
#include <ILServer.h>
/*
#include <X11/Wc/WcCreate.h>
#include <X11/Xp/Xp.h>
*/
Display     *display;
Widget       topLevel = NULL;
AppData      appData;
XtAppContext appContext = NULL;



/* Normal termination procedure */

void Goodbye(int code)
{
  Begin("Goodbye");

  CleanUpVisuals();
  YCleanUp();

  if (serverStarted) IL_KillAllTasks(False);
  if (topLevel && XtIsManaged(topLevel)) XtUnmapWidget(topLevel);
  if (appContext) XtDestroyApplicationContext(appContext);
  exit(code);
}



/* Abnormal termination procedure */

void ErrorFastExit(String msg)
{
  Begin("ErrorFastExit");
  
  fprintf(stderr, "%s Top Box: %s.\n", ProgramName, msg);
  signal(SIGHUP, SIG_DFL);
  kill(0, SIGHUP);
  exit(-1);
}



/* Abnormal termination procedure */

void Error(String msg)
{
  Begin("Error");
  
  fprintf(stderr, "%s Top Box: %s.\n", ProgramName, msg);
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
	  ProgramName, buffer, dispname);
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
	  ProgramName);
  Goodbye(1);

  Return(-1);			/* otherwise some compilers complain */
}



/* Handler for Toolkit errors, always fatal */

void ErrorXt(char *msg)
{
  Begin("ErrorXt");

  fprintf(stderr,"%s: X Toolkit Fatal Error: %s\n", ProgramName, msg);
  if (appContext) XtDestroyApplicationContext(appContext);
  Goodbye(1);
}




void SigBus(void)
{
  Begin("SigBus");
  ErrorFastExit("Bus Error: this shouldn't happen, sorry");
}



void SigSegV(void)
{
  Begin("SigSegV");
  ErrorFastExit("Segmentation Violation: this shouldn't happen, sorry");
}



void SigIll(void)
{
  Begin("SigIll");
  ErrorFastExit("Illegal Instruction: this shouldn't happen, sorry");
}



void SigChldAfterHup(void)
{
  Begin("SigChldAfterHup");

  (void)YQuery
    (topLevel, "\n"
     "  One of the Rosegarden programs has just died, horribly.  \n"
     "\n"
     "  You might like to send an email to cannam@zands.demon.co.uk  \n"
     "  describing what went wrong.  It'd be helpful if you could \n"
     "  reconstruct the exact actions which led to the crash.\n\n",
     1, 0, 0, "Curses! And just at a critical moment, too");

/*
   One of the Rosegarden programs has died a horrible and   \n\
   unnecessary death.\n\n"
*/
/*   I can apologise; but what is wrought, I can not undo.   \n\n", */
/*	       1, 0, 0, "My deepest sympathy", NULL); */

  signal(SIGCHLD, SIG_DFL);

  End;
}


void SigHup(void)
{
  Begin("SigHup");

  signal(SIGCHLD, SigChldAfterHup);

  End;
}


void SetSigHupForClient(void)
{
  Begin("SetSigHupForClient");

  signal(SIGHUP, SigHup);

  End;
}


void SigOther(void)
{
  Begin("SigOther");
  Goodbye(2);
}



/* Install the signal handlers. */

void InstallSignalHandlers(void)
{
  Begin("InstallSignalHandlers");

  signal(SIGBUS,  SigBus);
  signal(SIGSEGV, SigSegV);
  signal(SIGILL,  SigIll);
  signal(SIGHUP,  SigOther);
  signal(SIGINT,  SigOther);
  signal(SIGQUIT, SigOther);

  End;
}


/* Pop up a query box on WM Quit function  */


void QuitNicely(void)
{
  char message[200];

  Begin("QuitNicely");

  sprintf(message, "This will finish your whole %s session.", ProgramName);
  if (YQuery(topLevel, message, 2, 1, 1, "Exit",
	     "Cancel", NULL) == 0) Goodbye(0);

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


/* Convenience routine to load up fonts. */

XFontStruct *LoadQueryFont(Widget widget, char *fn)
{
  XFontStruct *fontStructp;

  Begin("LoadQueryFont");

  if ((fontStructp = XLoadQueryFont(XtDisplay(widget),fn)) == NULL) {
    fprintf(stderr,"%s: No font \"%s\", trying \"%s\"\n", ProgramName,
	    fn, DefaultFont);
    if ((fontStructp = XLoadQueryFont(XtDisplay(widget),DefaultFont))
	== NULL) Error("Couldn't load default font");
  }
  Return(fontStructp);
}



void SetTitleBar(char **argv)
{
#ifndef NO_GETHOSTNAME
  char  machine[100];
#endif
  char  title[200];
  Arg   arg[2];

  Begin("SetTitleBar");

  /* Set title in title bar, with host name if possible */

#ifdef NO_GETHOSTNAME
  sprintf(title, "%s", ProgramName);
#else
  if (gethostname(machine, 99))
    sprintf(title, "%s", ProgramName);
  else
    sprintf(title, "%s  [%s]", ProgramName, machine);
#endif

  XtSetArg(arg[0], XtNtitle,    (XtArgVal)title);
  XtSetArg(arg[1], XtNiconName, (XtArgVal)ProgramName);
  XtSetValues(topLevel, arg, 2);

  End;
}



void DisplaySyntax(void)
{
  Begin("DisplaySyntax");
  
  fprintf
    (stderr,
     "\n%s accepts all of the generic X Toolkit command-line\n"
     "options, as well as the following.\n\n"
     "  filename        Attempt to open file in existing Rosegarden session\n"
     "  -editorname     Pathname of the Rosegarden editor executable\n"
     "  -sequencername  Pathname of the Rosegarden sequencer executable\n"
     "  -helpfile       Pathname of the Help Info text file\n"
     "  -aboutfn        Font used for text in the About boxes\n"
     "  -helptextfn     Font used for text in the Help display\n"
     "  -helpxreffn     Font used for hyperlinks in the Help display\n"
     "  -helptitlefn    Font used for titles in the Help display\n"
     "  -helpverbfn     Font used for verbatim text in the Help display\n\n"
     "Each of these options requires one argument.\n\n"
     "For further explanation of these options and of how to use and\n"
     "customise the Rosegarden top box, see the Rosegarden manual.\n",
     ProgramName);

  fprintf(stderr,"\n");
  if (appContext) XtDestroyApplicationContext(appContext);
  exit(0);
}



/* Record the Xt Actions table */

/* ARGSUSED */
void InstallActions(void)
{
  static XtActionsRec musicActions[] = {
    { "interlock-wm-quit", QuitNicelyAction },
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
     XtParseTranslationTable("<Message>WM_PROTOCOLS: interlock-wm-quit()"));
  XSetWMProtocols(XtDisplay(topLevel), XtWindow(topLevel), wmProtocols, 1);

  End;
}



/* ARGSUSED */
int main(int argc, char **argv, char **envp)
{
  char appName[100];
  char *fileName = 0;

  Begin("main");

  (void)setsid();		/* can't do anything about it if it fails */

  /* Do the Xt initialisation by hand, because we want   */
  /* the application name and class name to be different */

  XtToolkitInitialize();
  appContext = XtCreateApplicationContext();
  XtAppSetFallbackResources(appContext, fallbacks);

  sprintf(appName, "%s Top Box", ProgramName);

  if (!(display = XtOpenDisplay(appContext, NULL, appName, ProgramName,
				commandOptions, XtNumber(commandOptions),
				&argc, argv)))
    Error("Couldn't open display");

  topLevel = XtAppCreateShell(appName, ProgramName,
			      applicationShellWidgetClass, display, NULL, 0);

  XtGetApplicationResources(topLevel, &appData, resources,
			    XtNumber(resources), NULL, 0);

  display = XtDisplay(topLevel);

  /* Wcl */
  /*  XpRegisterAll(appContext);
  WcInitialize(topLevel);
  */
  /* Set up the error handlers */
  XSetErrorHandler(ErrorXNonFatal);
  XSetIOErrorHandler(ErrorXIO);
  XtAppSetErrorHandler(appContext, ErrorXt);
  InstallSignalHandlers();

  if (argc > 1) {

    if (strcmp(argv[1], "-writehelpix") == 0) {

      int ext;

      fprintf(stderr, "\nAttempting to write help index file.\n");

      YHelpInitialise(topLevel, "Rosegarden Help", NULL, NULL, NULL);
      YHelpSetHelpFile(appData.helpFile);
      ext = YHelpWriteIndexFile();
      YHelpCloseHelpFile();

      fprintf(stderr,"\nRosegarden Top Box exiting.\n");
      exit(ext);
    }

    if (strcmp(argv[1], "-help") == 0) DisplaySyntax();
    else if (argv[1][0] != '-') {

      if (OpenRemote(topLevel, argv[1], 0)) exit(0);
      else fileName = argv[1];

    } else {

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

  /* Set up and install the visible interface */

  SetTitleBar(argv);
  CreateApplicationWidgets();
  InstallActions();
  SetWMQuitProperties();

  if (fileName) OpenRemote(aboutButton, fileName, XtWindow(topLevel));

  XtAppMainLoop(appContext);
  fprintf(stderr,"%s: Panic!  X Toolkit Main Loop failed\n", ProgramName);
  Return(-1);
}

