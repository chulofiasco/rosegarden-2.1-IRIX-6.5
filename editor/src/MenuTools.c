
/* MenuTools.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */

/* {{{ Includes */

#include "General.h"
#include "IO.h"
#include "Tags.h"
#include "MidiOut.h"
#include "MidiIn.h"
#include "Menu.h"
#include "StaveEdit.h"

#include <unistd.h>		/* for unlink */

#include <Yawn.h>
#include <ILClient.h>

/* }}} */
/* {{{ Sequence and Play */

static String filename = NULL;


void SequenceAcknowledgeCallback(IL_ReturnCode rtn)
{
  Begin("SequenceCompleteCallback");

  if (filename) (void)unlink(filename);	/* can't do anything about failure */

  StaveBusy(False);

  switch(rtn) {

  case IL_SERVICE_OK:

    LeaveMenuMode(SequencerRunningMode);
    EnterMenuMode(SequencerNotRunningMode);
    break;

  case IL_SERVICE_BUSY:
    
    (void)YQuery(topLevel, "The sequencer is busy.  Try again later.",
		 1, 0, 0, "Okay", NULL);

    LeaveMenuMode(SequencerRunningMode);
    EnterMenuMode(SequencerNotRunningMode);
    break;

  case IL_SERVICE_FAILED:

    (void)YQuery(topLevel, "Sorry, the sequencer doesn't seem to be working.",
		 1, 0, 0, "Okay", NULL);
    break;

  case IL_NO_SUCH_SERVICE:

    (void)YQuery(topLevel, "I can't invoke the sequencer.",
		 1, 0, 0, "Okay", NULL);
    break;
  }

  End;
}


void FileMenuSequence(Widget w, XtPointer a, XtPointer b)
{
  int len;
  String name;
  String sname;
  Begin("FileMenuSequence");

  if (!(filename = tmpnam(NULL))) {
    IssueMenuComplaint("Sorry, I couldn't get a temporary file name.");
    End;
  }

  sname = GetStaveName(stave);
  len = strlen(sname);
  name = (String)XtMalloc(len + 25);

  if (len > 13 && !strcmp(sname + len - 12, " (temporary)")) {
    sprintf(name, "%s", sname);
  } else {
    sprintf(name, "%s (temporary)", sname);
  }

  if (MidiWriteStave(stave, filename, name) == Failed) {
    XtFree(name);
    End;
  }

  XtFree(name);

  StaveBusy(True);
  IL_RequestService(ILS_SEQUENCE_SERVICE, SequenceAcknowledgeCallback,
		    filename, strlen(filename) + 1);

  LeaveMenuMode(SequencerNotRunningMode);
  EnterMenuMode(SequencerRunningMode);
  End;
}


void FileMenuPlayMidi(Widget w, XtPointer a, XtPointer b)
{
  char *text = "Hello World!  How are you?";

  Begin("FileMenuPlayMidi");

  StaveBusy(True);

  IL_RequestService(ILS_PLAY_SERVICE, SequenceAcknowledgeCallback,
		    text, strlen(text) + 1);

  LeaveMenuMode(SequencerNotRunningMode);
  EnterMenuMode(SequencerRunningMode);
  End;
}

/* }}} */
/* {{{ Export and Import MIDI */

void FileMenuWriteMidi(Widget w, XtPointer a, XtPointer b)
{
  String fname;
  String message;
  Begin("FileMenuWriteMidi");

  if ((fname = YFileGetWriteFilename(XtParent(XtParent(w)),
				     "Editor File - Export MIDI",
				     ".mid", "MIDI"))
      == NULL) End;

  (void)MidiWriteStave(stave, fname, NULL);

  message = (String)XtMalloc(strlen(fname) + 12);
  sprintf(message, "Wrote `%s'.", fname);
  (void)YQuery(topLevel, message, 1, 0, 0, "OK", NULL);
  XtFree(message);

  End;
}


void FileMenuImportMidi(Widget w, XtPointer a, XtPointer b)
{
  String     fname;
  String     name;
  MajorStave sp;

  Begin("FileMenuImportMidi");

  if ((fname = YFileGetReadFilename(XtParent(XtParent(w)),
				    "Editor File - Import MIDI",
				    ".mid", "MIDI"))
      == NULL) End;

  if ((sp = MidiReadStave(fname, &name, False, -1, True)) == NULL) {
    StaveBusy(False);
    End;
  }

  AddStaveToFileMenu(sp, name, NULL);
  FileMenuMarkChanged(sp, False);
  XtFree(name);

  LeaveMenuMode(FileNotLoadedMode);
  EnterMenuMode(FileLoadedMode);

  staveMoved = True;
  StaveReformatEverything(sp);

  End;
}

/* }}} */
/* {{{ Interlock and Tracking requests */

void FileMenuEditILCallback(String chunk)
{
  int          i;
  String       name;
  MajorStave   sp;
  XtAppContext appContext;
  FILE        *file;
  char         tag[5];
  String       fullname = NULL;

  Begin("FileMenuEditILCallback");

  appContext = XtWidgetToApplicationContext(topLevel);
  while (XtAppPending(appContext)) {
    XtAppProcessEvent(appContext, XtIMAll);
    XSync(display, False);
  }

  if ((file = fopen(chunk, "r")) == NULL) {

    char errmsg[1000];
    sprintf(errmsg, "%s: %s", ApplicationName, chunk);
    perror(errmsg);
    IL_AcknowledgeRequest(ILS_EDIT_SERVICE, IL_SERVICE_FAILED);
    End;

  } else {

    fgets(tag, 5, file);

    if (!strcmp(tag, "#!Ro")) {

      rewind(file);
      if ((sp = LoadStaveFromFile(file)) == NULL) {
	IL_AcknowledgeRequest(ILS_EDIT_SERVICE, IL_SERVICE_FAILED);
	End;
      } else {
	for (i = strlen(chunk)-1; i >= 0; --i) if (chunk[i] == '/') break;
	name = XtNewString(chunk + i + (i >= 0));
	fullname = XtNewString(chunk);
      }	

    } else {

      fclose(file);
      if ((sp = MidiReadStave(chunk, &name, False, -1, False)) == NULL) {
	IL_AcknowledgeRequest(ILS_EDIT_SERVICE, IL_SERVICE_FAILED);
	End;
      }
    }
  }

  XMapRaised(display, XtWindow(topLevel));
  XMapRaised(display, XtWindow(paletteShell));

  AddStaveToFileMenu(sp, name, fullname);
  FileMenuMarkChanged(sp, False);
  XtFree(name);

  LeaveMenuMode(FileNotLoadedMode);
  EnterMenuMode(FileLoadedMode);

  staveMoved = True;
  StaveReformatEverything(sp);

  IL_AcknowledgeRequest(ILS_EDIT_SERVICE, IL_SERVICE_OK);

  End;
}


/* request syntax: each request can be numeric, in which case it's a
   beat count from the sequencer (from Midi_TimeToBeat), or it can be
   LOCK or UNLOCK.  The filename is not transmitted but is obtained by
   the same means on both sides -- by using the topbox window id */

Boolean slaveMode;		/* external scope */
static String followFile = NULL;
#define TRACKING_BUFFER_NAME "(Playback Tracking)"

void FileMenuFollowILCallback(String chunk)
{
  float beat;
  MTime time;
  MajorStave sp;
  Boolean barOnly = False;
  XtAppContext appContext;
  Begin("FileMenuFollowILCallback");

  IL_AcknowledgeRequest(ILS_FOLLOW_SERVICE, IL_SERVICE_OK);

  if (!isdigit(chunk[0]) && chunk[0] != '-') {

    if (!strcmp(chunk, ILS_FOLLOW_UNLOCK)) {

      slaveMode = False;
      LeaveMenuMode(SlaveToSequencerMode);

      if (followFile) {
	FileCloseStave(TRACKING_BUFFER_NAME);
	XtFree(followFile);
	followFile = NULL;
      }

      IL_RequestService(ILS_FOLLOWREADY_SERVICE, NULL, ILS_FOLLOW_DEAD,
			strlen(ILS_FOLLOW_DEAD) + 1);

    } else {

      char *n;
      char  trw[20];

      followFile = XtNewString(chunk);
      FileCloseStave(TRACKING_BUFFER_NAME);

      if ((sp = MidiReadStave(followFile, &n, True, 4, False)) == NULL) {
	XtFree(followFile); followFile = NULL; 
	IL_RequestService(ILS_FOLLOWREADY_SERVICE, NULL, ILS_FOLLOW_DEAD,
			  strlen(ILS_FOLLOW_DEAD) + 1);
	End;
      }

      AddStaveToFileMenu(sp, TRACKING_BUFFER_NAME, NULL);
      FileMenuMarkChanged(sp, False);
      if (n) XtFree(n);

      StaveReformatEverything(sp);
      StaveLeapToTime(sp, zeroTime, False);
      XMapRaised(XtDisplay(topLevel), XtWindow(topLevel));

      slaveMode = True;
      EnterMenuMode(SlaveToSequencerMode);

      appContext = XtWidgetToApplicationContext(topLevel);
      while (XtAppPending(appContext)) {
	XtAppProcessEvent(appContext, XtIMAll);
	XSync(display, False);
      }

      sprintf(trw, "%ld", (long)StaveTrackingTargetWindow());
      IL_RequestService(ILS_FOLLOWREADY_SERVICE, NULL, trw, strlen(trw) + 1);
    }

    End;
  }

  if (!followFile) End;

  beat = atof(chunk);

  /*  fprintf(stderr, "beat is %f in IL handler\n", beat);*/

  if (beat < 0.0) {
    barOnly = True;
    beat = -beat;
  }

  time = NumberToMTime
    ((long)(beat * (float)TagToNumber(Crotchet, False) + 0.01));

  StaveLeapToTime(stave, time, barOnly);

  End;
}


void TrackingCleanUp(void)
{
  Begin("TrackingCleanUp");

  if (slaveMode) {

    slaveMode = False;

    if (followFile) {
      XtFree(followFile);
      followFile = NULL;
    }

    IL_RequestService(ILS_FOLLOWREADY_SERVICE, NULL, ILS_FOLLOW_DEAD,
		      strlen(ILS_FOLLOW_DEAD) + 1);
  }

  End;
}

/* }}} */
/* {{{ MusicTeX/OpusTeX export stubs */

void FileMenuExportMusicTeX(Widget w, XtPointer a, XtPointer b)
{
  Begin("FileMenuExportMusicTeX");

  StaveWriteMusicTeXToFile(stave, XtParent(XtParent(w)));

  End;
}

void FileMenuExportOpusTeX(Widget w, XtPointer a, XtPointer b)
{
  Begin("FileMenuExportOpusTeX");

  StaveWriteOpusTeXToFile(stave, XtParent(XtParent(w)));

  End;
}

void FileMenuExportPMX(Widget w, XtPointer a, XtPointer b)
{
  Begin("FileMenuExportPMX");

  StaveWritePMXToFile(stave, XtParent(XtParent(w)));

  End;
}

/* }}} */

