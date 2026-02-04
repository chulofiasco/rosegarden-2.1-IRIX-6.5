
/* Rosegarden Top Box, widget creation stuff */

#include "General.h"
#include "Widgets.h"
#include "ILTypes.h"
#include "ILServer.h"
#include "ILClient.h"
#include "Yawn.h"
#include "YHelp.h"
#include "Visuals.h"

#include <Version.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>

Widget aboutButton;

static Widget editButton;
static Widget midiButton;
static Widget helpButton;

Boolean serverStarted = False;

YMessageString aboutText[] = {
  { "Rosegarden",                    YMessageBold,       },
  { " ",                             YMessageNormal,     },
  { "MIDI Sequencer",                YMessageNormal,     },
  { " ",                             YMessageNormal,     },
  { ROSEGARDEN_VERSION,              YMessageNormal,     },
  { "Andy Green & Richard Brown",    YMessageBold,       },
  { " ",                             YMessageNormal,     },
  { "IRIX 6.5 fixes: @chulofiasco, SGUG",  YMessageBold,       },
  { "with thanks to ctc, jpff and others", YMessageItalic, },
};


void ActivateILClient(String tag)
{
  Begin("ActivateILClient");

  if (!strncmp(tag, ILS_SEQUENCE_SERVICE, 4) ||
      !strncmp(tag, ILS_PLAY_SERVICE, 4) ||
      !strncmp(tag, ILS_FOLLOWREADY_SERVICE, 4)) {

    SetSigHupForClient();
    XtSetSensitive(midiButton, False);

  } else if (!strncmp(tag, ILS_EDIT_SERVICE, 4) ||
	     !strncmp(tag, ILS_FOLLOW_SERVICE, 4)) {

    SetSigHupForClient();
    XtSetSensitive(editButton, False);

  }  else if (!strncmp(tag, ILS_HELP_SERVICE, 4)) {

    XtSetSensitive(helpButton, False);
  }

  End;
}


void InitialiseILServer(void)
{
  struct stat buf;

  Begin("InitialiseILServer");

  IL_ServerInit(topLevel, ProgramName, ClientFinished, ActivateILClient, NULL);

  serverStarted = True;
  
  if (stat(appData.editorName, &buf)) {
    fprintf(stderr,"%s: Cannot open editor service\n", ProgramName);
    appData.editorName = NULL;
  }

  if (stat(appData.sequencerName, &buf)) {
    fprintf(stderr,"%s: Cannot open sequencer service\n", ProgramName);
    appData.sequencerName = NULL;
  }

  if (appData.editorName) {
    IL_AddService(ILS_EDIT_SERVICE, appData.editorName);
    IL_AddService(ILS_FOLLOW_SERVICE, appData.editorName);
  } else XtSetSensitive(editButton, False);

  if (appData.sequencerName) {
    IL_AddService(ILS_PLAY_SERVICE, appData.sequencerName);
    IL_AddService(ILS_SEQUENCE_SERVICE, appData.sequencerName);
    IL_AddService(ILS_FOLLOWREADY_SERVICE, appData.sequencerName);
  } else XtSetSensitive(midiButton, False);

  End;
}


void HelpILCallback(char *chunk)
{
  Begin("HelpILCallback");
  if (strcmp(chunk, "Nothing")) YHelpSetTopic(chunk);
  YHelpInstallHelp();
  IL_AcknowledgeRequest(ILS_HELP_SERVICE, IL_SERVICE_OK);
  XtSetSensitive(helpButton, False);
  End;
}


void ILQuit(Boolean confirm)
{
  Begin("ILQuit");
  if (confirm) IL_DeRegisterWithServer("Help");
  YHelpClose();
  End;
}


/* This application is both server and client.  Interlock can't handle */
/* the case where the server window is the same as one of the clients  */
/* (it has multiple-messaging problems, things get sent to both client */
/* and server), so we have to use a different window for the client id */

void InitialiseILClient(void)
{
  Begin("InitialiseILClient");

  IL_ClientInit(XtWindow(topLevel), helpButton, ILQuit);
  IL_RegisterWithServer(ILS_HELP_SERVICE, HelpILCallback);

  End;
}


void EditButton(Widget w, XtPointer a, XtPointer b)
{
  Begin("EditButton");
  IL_StartTask(ILS_EDIT_SERVICE);
  YDarkenWidget(w);
  XtSetSensitive(w, False);
  SetSigHupForClient();
  End;
}


void MidiButton(Widget w, XtPointer a, XtPointer b)
{
  Begin("MidiButton");
  IL_StartTask(ILS_SEQUENCE_SERVICE);
  YDarkenWidget(w);
  XtSetSensitive(w, False);
  SetSigHupForClient();
  End;
}


void AboutButton(Widget w, XtPointer a, XtPointer b)
{
  Begin("AboutButton");
  YMessage(XtParent(w), "About Rosegarden",
	   "Enough!", aboutText, XtNumber(aboutText));
  End;
}


void ClientFinished(char *name)
{
  Begin("ClientFinished");
  if (strncmp(name, ILS_EDIT_SERVICE, 4) == 0 ||
      strncmp(name, ILS_FOLLOW_SERVICE, 4) == 0)
    XtSetSensitive(editButton, True);
  else if (strncmp(name, ILS_SEQUENCE_SERVICE, 4) == 0 ||
	   strncmp(name, ILS_FOLLOWREADY_SERVICE, 4) == 0 ||
	   strncmp(name, ILS_PLAY_SERVICE, 4) == 0)
    XtSetSensitive(midiButton, True);
  else if (strncmp(name, ILS_HELP_SERVICE, 4) == 0)
    XtSetSensitive(helpButton, True);
  End;
}


void HelpFinished()
{
  XtSetSensitive(helpButton, True);
}


void HelpButton(Widget w, XtPointer a, XtPointer b)
{
  YHelpSetTopic("The Rosegarden Top Box");
  YHelpInstallHelp();
  IL_AcknowledgeRequest(ILS_HELP_SERVICE, IL_SERVICE_OK);
  YDarkenWidget(w);
  XtSetSensitive(helpButton, False);
}


void CreateApplicationWidgets(void)
{
  Widget outerForm;
  Widget quitButton;
  Window w;

  Begin("CreateApplicationWidgets");

  w = RootWindowOfScreen(XtScreen(topLevel));

  YInitialise(topLevel, NULL);
  YHelpInitialise(topLevel, "Rosegarden Help", NULL, HelpFinished,
		  "The Rosegarden Help System");
  YHelpSetHelpFile(appData.helpFile);

  YShouldWarpPointer(appData.shouldWarpPointer);

  InitialiseVisuals();

  YMessageInitialise(roseMap, appData.aboutTextFont);

  outerForm   = YCreateShadedWidget
    ("Top Box", formWidgetClass, topLevel, MediumShade);
  aboutButton = YCreateSurroundedWidget
    ("About", commandWidgetClass, outerForm, SurroundShade, NoShade);
  YSetValue(aboutButton, "shadowWidth", 0); /* in case we're 3d */

  YSetValue( aboutButton, XtNbitmap,        roseMap );
  YSetValue(    topLevel, XtNiconPixmap,    roseMap );
  YSetValue(    topLevel, XtNiconMask,  roseMaskMap );

  editButton = YCreateCommand("Editor",    outerForm);
  midiButton = YCreateCommand("Sequencer", outerForm);
  quitButton = YCreateCommand("Quit",      outerForm);
  helpButton = YCreateCommand("Help",      outerForm);


  YSetValue(XtParent(editButton), XtNfromHoriz, XtParent(aboutButton));
  YSetValue(XtParent(midiButton), XtNfromHoriz, XtParent(editButton));
  YSetValue(XtParent(quitButton), XtNfromHoriz, XtParent(midiButton));
  YSetValue(XtParent(helpButton), XtNfromHoriz, XtParent(quitButton));

  XtAddCallback(quitButton,  XtNcallback, QuitNicelyCallback, NULL);
  XtAddCallback(editButton,  XtNcallback,         EditButton, NULL);
  XtAddCallback(midiButton,  XtNcallback,         MidiButton, NULL);
  XtAddCallback(helpButton,  XtNcallback,         HelpButton, NULL);
  XtAddCallback(aboutButton, XtNcallback,        AboutButton, NULL);

  XtRealizeWidget(topLevel);

  YHelpSetIdentifyingBitmap(roseMap);
  YHelpSetXrefFont(appData.helpXrefFont);
  YHelpSetTitleFont(appData.helpTitleFont);
  YHelpSetTextFont(appData.helpTextFont);
  YHelpSetVerbatimFont(appData.helpVerbatimFont);

  InitialiseILServer();
  InitialiseILClient();
  End;
}

