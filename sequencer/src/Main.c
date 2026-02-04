/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Main.c
 *
 *    Description:    Main function.
 *
 *    Author:         AJG
 *
 * History:
 *
 *
 * Update       Date            Programmer      Comments
 * ======       ====            ==========      ========
 * 001          24/01/94        AJG             File Created.
 * 002          30/05/96        rwb             Initialisation of Play/Record
 *                                              devices occurs here.  Currently.
 *
 */

#include <SysDeps.h>
#include <ILClient.h>
#include "Mapper.h"

#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <MidiVarLenNums.h>

#include "Sequence.h"
#include "Types.h"
#include "Consts.h"
#include "Resources.h"
#include "Fallbacks.h"
#include "MidiConsts.h"
#include "Globals.h"
#include "Menu.h"
#include "Main.h"
#include "MainWindow.h"
#include "TrackList.h"
#include "PianoRoll.h"
#include "EventListWindow.h"
#include "Tracking.h"

#include <Debug.h>

#include <hourglass.xbm>
#include <hourglass_mask.xbm>

#include "hour0.xbm"
#include "hour1.xbm"
#include "hour2.xbm"
#include "hour3.xbm"
#include "hour4.xbm"
#include "hour5.xbm"
#include "hour6.xbm"
#include "hour7.xbm"

Display	       *display;
Widget	 	topLevel 	= NULL;
XtAppContext	appContext	= NULL;
Window		InterlockServer	= NULL;
AppData		appData;
Pixmap		HourglassPixmap;
Pixmap		HourglassMask;
Cursor		HourglassCursor;
Boolean		MIDIinServitude  = False;
Boolean		MIDIfileModified = False;
Boolean		MIDIneverSaved   = True;

Pixmap	HourglassAnim[HOUR_FRAMES];
Cursor  HourglassAnimCur[HOUR_FRAMES];

/********************************************/
/* Midi_ExitCleanly: Tidy up and go home... */
/********************************************/

void Midi_ExitCleanly(void)
{
BEGIN("Midi_ExitCleanly");

    if (topLevel && XtIsManaged(topLevel)) XtUnmapWidget(topLevel);

    if (MIDIinServitude)
    {
        IL_AcknowledgeRequest(ILS_SEQUENCE_SERVICE, IL_SERVICE_OK);
    }

    if (appData.interlockWindow)
    {
        IL_DeRegisterWithServer(ILS_PLAY_SERVICE);
        IL_DeRegisterWithServer(ILS_SEQUENCE_SERVICE);
        IL_DeRegisterWithServer(ILS_FOLLOWREADY_SERVICE);
    }

    if (appContext) XtDestroyApplicationContext(appContext);
    YCleanUp();
    exit(0);
}


/*******************************************/
/* Midi_ExitQuickly: Panic and run away... */
/*******************************************/

void Midi_ExitQuickly(char *msg)
{
BEGIN("Midi_ExitQuickly");


	if (appData.interlockWindow)
	{
		IL_DeRegisterWithServer(ILS_PLAY_SERVICE);
		IL_DeRegisterWithServer(ILS_SEQUENCE_SERVICE);
		IL_DeRegisterWithServer(ILS_FOLLOWREADY_SERVICE);
	}

	fprintf(stderr, "%s Sequencer: %s.\n", ProgramName, msg);
	/*if (appData.interlockWindow) kill(getppid(), SIGHUP);*/
	kill(0, SIGHUP);

 abort();			/* temporary, for core file */

  	exit(-1);
}

void Midi_QuitNicely(void)
{
char message[200];

BEGIN("Midi_QuitNicely");

	sprintf(message, "Exit the %s Sequencer: Are you sure?", ProgramName);
  	if (YQuery(topLevel, message, 2, 1, 1, "Yes", "No", "Sequencer File - Exit") == 0) Midi_ExitCleanly();

END;
}

void Midi_QuitNicelyAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
char message[200];

BEGIN("Midi_QuitNicely");

	sprintf(message, "Exit the %s Sequencer: Are you sure?", ProgramName);
  	if (YQuery(topLevel, message, 2, 1, 1, "Yes", "No", "Sequencer File - Exit") == 0) Midi_ExitCleanly();

END;
}

void Midi_IgnoreQuit(void)
{
BEGIN("Midi_IgnoreQuit");

END;
}

void Midi_IgnoreQuitAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
BEGIN("Midi_IgnoreQuit");

END;
}



void Midi_QuitCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_QuitCB");

  	Midi_QuitNicely();
END;
}



void Midi_ILQuit(Boolean confirm)
{
BEGIN("Midi_ILQuit");

	if (!confirm) appData.interlockWindow = NULL;	     /* don't deregister */
  	Midi_ExitCleanly();

END;
}


void Midi_DemandHelp(char *HelpTopic)
{
BEGIN("Midi_DemandHelp");

	if (appData.interlockWindow)
	{
		IL_RequestService(ILS_HELP_SERVICE, Midi_AcknowledgeHelp,
				  HelpTopic, strlen(HelpTopic) + 1);
	}

END;
}

void Midi_HelpCallback(Widget w, XtPointer HelpTopic, XtPointer a)
{
BEGIN("Midi_HelpCallback");

	Midi_DemandHelp((char *)HelpTopic);

END;
}

void Midi_PlayService(char *Nothing)
{}

void Midi_SequenceService(char *FileName)
{
	BEGIN("Midi_SequenceService");

	if (MIDIHeaderBuffer.Format != MIDI_NO_FILE_LOADED)
	  if (!Midi_CloseFile()) {
	    IL_AcknowledgeRequest(ILS_SEQUENCE_SERVICE, IL_SERVICE_BUSY);
	    END;
	  }

	Midi_EventListDeleteAllWindows();
	Midi_PianoRollDeleteAllWindows();

	XMapRaised(XtDisplay(topLevel), XtWindow(topLevel));

	MIDIinServitude = True;
	Midi_LoadFile(FileName, False);
	IL_AcknowledgeRequest(ILS_SEQUENCE_SERVICE, IL_SERVICE_OK);
	MIDIinServitude  = False;

	/* Next three lines are only true if this service was
	   requested from the Editor.  If it was requested from the
	   topbox (originating at the command-line), we should use
	   Midi_SetFileModified(False), MIDIneverSaved = False and
	   MIDIFileName = FileName.  Unfortunately we don't have any
	   way to distinguish between the two cases. --cc */

	Midi_SetFileModified(True);
	MIDIneverSaved   = True;
	MIDIFileName     = NULL;

	Midi_SetTitleBar();
	END;
}

/****************************************************/
/* Midi_ErrorXNonFatal: Report a non-fatal X Error. */
/****************************************************/

int Midi_ErrorXNonFatal(Display *d, XErrorEvent *event)
{
char  buffer[500];
char *dispname;

BEGIN("ErrorXNonFatal");

	XGetErrorText(d, event->error_code, buffer, 500);
  	dispname = XDisplayName(NULL);

  	fprintf(stderr, "%s: X Warning: %s on display `%s'\n",
		ProgramName, buffer, dispname);

	fprintf(stderr, "      [ serial number: 0x%08lx ]  [ major op code: 0x%08x ]\n",
	  	event->serial, event->request_code);
 
 	fprintf(stderr, "      [ resource iden: 0x%08lx ]  [ minor op code: 0x%08x ]\n",
	  	event->resourceid, event->minor_code);

  RETURN_INT(0);
}




/******************************************************************/
/* Midi_ErrorXIO: Handler for the fatal X error (connection lost) */
/******************************************************************/

int Midi_ErrorXIO(Display *d)
{
BEGIN("Midi_ErrorXIO");

  	fprintf(stderr,"%s: The X server connection has been cut, goodbye!\n",
		ProgramName);
  	if (appContext) XtDestroyApplicationContext(appContext);
  	exit(1);

RETURN_INT(-1);			/* no return is fatal to some C++ compilers*/
}


/**********************************************************/
/* Midi_ErrorXt: Handler for Toolkit errors, always fatal */
/**********************************************************/

void Midi_ErrorXt(char *msg)
{
BEGIN("Midi_ErrorXt");

 	fprintf(stderr,"%s: X Toolkit Fatal Error: %s\n", ProgramName, msg);
  	if (appContext) XtDestroyApplicationContext(appContext);
  	exit(1);
}




/***********************************/
/* SigBus: Handler for Bus errors. */
/***********************************/

void SigBus(void)
{
BEGIN("SigBus");

  	Midi_ExitQuickly("Bus Error: this shouldn't happen, sorry");
END;
}



/*********************************************/
/* SigSegV: Handler for Segmentation faults. */
/*********************************************/

void SigSegV(void)
{
BEGIN("SigSegV");

	Midi_ExitQuickly("Segmentation Violation: this shouldn't happen, sorry");
END;
}



/*********************************************/
/* SigIll: Handler for Illegal Instructions. */
/*********************************************/

void SigIll(void)
{
BEGIN("SigIll");

	Midi_ExitQuickly("Illegal Instruction: this shouldn't happen, sorry");
 
END;
}



/********************************************/
/* SigOther: Handler for all other signals. */
/********************************************/

void SigOther(void)
{
BEGIN("SigOther");

	Midi_ExitCleanly();

END;
}



/***************************************************************/
/* InstallSignalHandlers: Set up the signal handling functions */
/***************************************************************/

void InstallSignalHandlers(void)
{
BEGIN("InstallSignalHandlers");

  	signal(SIGBUS,  SigBus);
  	signal(SIGSEGV, SigSegV);
  	signal(SIGILL,  SigIll);
  	signal(SIGHUP,  SIG_IGN);
  	signal(SIGINT,  SigOther);
  	signal(SIGQUIT, SigOther);
END;
}



/********************************************************/
/* LoadQueryFont: Convenience routine to load in a font */
/* falling back to a hardcoded default if the requested */
/* font is not available.				*/
/********************************************************/

XFontStruct *LoadQueryFont(Widget widget, char *fn)
{
XFontStruct *fontStructp;

BEGIN("LoadQueryFont");

	if ((fontStructp = XLoadQueryFont(XtDisplay(widget),fn)) == NULL) 
	{
    		fprintf(stderr,"%s: No font \"%s\", trying \"%s\"\n", 
			ProgramName, fn, DefaultFont);

		if ((fontStructp = XLoadQueryFont(XtDisplay(widget),DefaultFont)) == NULL) 
		{
			Error(FATAL, "Couldn't load default font");
		}
	}

RETURN_PTR(fontStructp);
}

void Midi_InstallActions(void)
{

	static XtActionsRec musicActions[] = 
	{
		{ "music-wm-quit",     Midi_QuitNicelyAction },
		{ "eventlist-wm-quit", Midi_EventListQuitAction },
		{ "pianoroll-wm-quit", Midi_PianoRollQuitAction },
	};

BEGIN("Midi_InstallActions");

	  XtAppAddActions(appContext, musicActions, XtNumber(musicActions));
END;
}


void Midi_SetFileModified(Boolean m)
{
  BEGIN("Midi_SetFileModified");
  MIDIfileModified = m;
  Midi_SetTitleBar();
  END;
}


/****************************************************************/
/* SetTitleBar: Function to set up the title bar for the window */
/* to include the program and host name.			*/
/****************************************************************/

void Midi_SetTitleBar(void)
{
  char  TitleBuffer[200];
  Arg   arg[2];
  char *FileName = 0;
  int i;

/* fix from Simon Kagedal - 10/97 */
#ifndef NO_GETHOSTNAME
  char  machine[100];
#endif

  BEGIN("Midi_SetTitleBar");

  /*  FileName = YFileGetLastFilename(True);*/
  
  if (MIDIFileName) {
    for (i = strlen(MIDIFileName); i >= 0; --i) {
      if (MIDIFileName[i] == '/') break;
    }
    FileName = MIDIFileName + i + (i >= 0);
  }
  
  if (FileName)
    {
#ifdef NO_GETHOSTNAME
      sprintf(TitleBuffer, "%s Sequencer  %s%s", ProgramName, FileName,
	      MIDIfileModified ? " (changed)" : "");
#else
      
      if (gethostname(machine, 99))
	{
	  sprintf(TitleBuffer, "%s Sequencer  %s%s", ProgramName, FileName,
		  MIDIfileModified ? " (changed)" : "");
	}
      else sprintf(TitleBuffer, "%s Sequencer  [%s]  %s%s", ProgramName, 
		   machine, FileName, MIDIfileModified ? " (changed)" : "");
      
#endif
    }
  else
    {
#ifdef NO_GETHOSTNAME
      
      sprintf(TitleBuffer, "%s Sequencer  MIDI file%s", ProgramName,
	      MIDIfileModified ? " (changed)" : "");
      
#else
      
      if (gethostname(machine, 99))
	{
	  sprintf(TitleBuffer, "%s Sequencer  MIDI file%s", ProgramName,
		  MIDIfileModified ? " (changed)" : "");
	}
      else sprintf(TitleBuffer, "%s Sequencer  [%s]  MIDI file%s",
		   ProgramName, machine, MIDIfileModified ? " (changed)" : "");
      
#endif
    }
  
  
  XtSetArg(arg[0], XtNtitle,    (XtArgVal)TitleBuffer);
  XtSetArg(arg[1], XtNiconName, (XtArgVal)"Sequencer");
  XtSetValues(topLevel, arg, 2);
  
  END;
}


/***********************************************/
/* DisplaySyntax: Print out the usage message. */
/***********************************************/

void DisplaySyntax(void)
{
BEGIN("DisplaySyntax");
  
	fprintf(stderr, "\n%s accepts all the generic X Toolkit options.\n\n",
	  	ProgramName);
	fflush(stderr);
	if (appContext) XtDestroyApplicationContext(appContext);
	exit(0);
}



void SetWMQuitProperties(void)
{
Atom wmProtocols[2];

BEGIN("SetWMQuitProperties");

	wmProtocols[0] = XInternAtom(XtDisplay(topLevel), "WM_DELETE_WINDOW", False);
/*  	wmProtocols[1] = XInternAtom(XtDisplay(topLevel), "WM_SAVE_YOURSELF", False); */

  	XtOverrideTranslations(topLevel, XtParseTranslationTable(
                        "<Message>WM_PROTOCOLS: music-wm-quit()"));
   
	XSetWMProtocols(XtDisplay(topLevel), XtWindow(topLevel),
                                                     wmProtocols, 1);

END;
}




int main(int argc, char *argv[], char **envp)
{
char appName[100];
XColor Black, White, ThrowAway;
int i;

BEGIN("main");

    XtToolkitInitialize();

    appContext = XtCreateApplicationContext();
    XtAppSetFallbackResources(appContext, Midi_Fallbacks);

    sprintf(appName, "%s Sequencer", ProgramName);

    if (!(display = XtOpenDisplay(appContext, NULL, appName, ProgramName,
            Midi_CommandOptions, XtNumber(Midi_CommandOptions), &argc, argv)))
    {
        Error(FATAL, "Unable to open display.");
    }

    topLevel = XtAppCreateShell(appName, ProgramName, 
		            applicationShellWidgetClass, display, NULL, 0);

    XtGetApplicationResources(topLevel, &appData, Midi_Resources,
                            XtNumber(Midi_Resources), NULL, 0);

    XSetErrorHandler(Midi_ErrorXNonFatal);
    XSetIOErrorHandler(Midi_ErrorXIO);
    XtAppSetErrorHandler(appContext, Midi_ErrorXt);
    InstallSignalHandlers();

    if (argc > 1)
    {
        DisplaySyntax();
    }

    if (!appData.foundDefaults)
    {
        Error(FATAL, "Unable to open application-defaults file.");
    }

    HourglassPixmap =  XCreateBitmapFromData(display,
                            RootWindowOfScreen(XtScreen(topLevel)),
                            hourglass_bits,
                            hourglass_width,
                            hourglass_height);

    HourglassMask =   XCreateBitmapFromData(display,
                            RootWindowOfScreen(XtScreen(topLevel)),
                            hourglass_mask_bits,
                            hourglass_mask_width,
                            hourglass_mask_height);

    XLookupColor(display, DefaultColormapOfScreen(XtScreen(topLevel)),
                            "black", &ThrowAway, &Black);

    XLookupColor(display, DefaultColormapOfScreen(XtScreen(topLevel)),
                            "white", &ThrowAway, &White);

    HourglassCursor = XCreatePixmapCursor(display, HourglassPixmap,
                             HourglassMask,
                             &Black,
                             &White,
                             hourglass_x_hot, hourglass_y_hot);

    HourglassAnim[0] = XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
                             hour0_bits, 
                             hour0_width,   
                             hour0_height);
 
    HourglassAnim[1] =  XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
	                     hour1_bits, hour1_width,   hour1_height);

    HourglassAnim[2] = XCreateBitmapFromData(display,
	                     RootWindowOfScreen(XtScreen(topLevel)),
                             hour2_bits, hour2_width,   hour2_height);

    HourglassAnim[3] = XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
                             hour3_bits, hour3_width,   hour3_height);

    HourglassAnim[4] = XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
                             hour4_bits, hour4_width,   hour4_height);

    HourglassAnim[5] = XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
                             hour5_bits, hour5_width,   hour5_height);

    HourglassAnim[6] = XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
                             hour6_bits, hour6_width,   hour6_height);

    HourglassAnim[7] = XCreateBitmapFromData(display,
                             RootWindowOfScreen(XtScreen(topLevel)),
                             hour7_bits, hour7_width,   hour7_height);


    for(i = 0; i < HOUR_FRAMES; ++i)
    {
        HourglassAnimCur[i] =  XCreatePixmapCursor(display, HourglassAnim[i],
                                              HourglassMask,
                                              &Black,
                                              &White,
                                              hourglass_x_hot, hourglass_y_hot);
    }
	
    /***************************/
    /* Set up the main window. */
    /***************************/

    MIDIFileName = NULL;
    Midi_CreateMainWindow();
    Midi_SetTitleBar();
    Midi_InstallActions();
    SetWMQuitProperties();

    YFileInitialise(appData.musicDirectory, True, True,
		    "Open", "Save", "Append");

    if(Mapper_SetupDevices(appData.midiPortName) == False)
    {
        fprintf(stderr,"Rosegarden Sequencer : cannot poll device \"%s\"\n",
                appData.midiPortName);
    }

    if (appData.interlockWindow)
    {
        IL_ClientInit((Window)atol(appData.interlockWindow), topLevel,
                                                               Midi_ILQuit);
        IL_RegisterWithServer(ILS_PLAY_SERVICE, Midi_PlayService);
        IL_RegisterWithServer(ILS_SEQUENCE_SERVICE, Midi_SequenceService);
        IL_RegisterWithServer(ILS_FOLLOWREADY_SERVICE, Midi_FollowReadyService);
    }

    XtAppMainLoop(appContext);

RETURN_INT(-1);
}



