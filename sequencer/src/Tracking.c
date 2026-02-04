
/* For requesting score tracking from the editor --cc,96 */

/* To use: create a menu with Midi_PlayTrackingMenuButton; user can
   choose the tracking level with this.  (Remember to call
   YFixOptionMenuLabel on the returned menu button after you pop up
   the containing dialog, and YDestroyOptionMenu after use.)

   Then call Midi_PlayTrackingOpen before you start playing,
   Midi_PlayTrackingClose after, and Midi_PlayTrackingJump every time
   you get a new deltatime.  (These functions will decide whether to
   track or not -- don't bother discriminating, just call them
   whenever.  You can call the Close function more than once if it's
   too much trouble to determine, but don't call Open more than once.)

   Don't leave the tracking menu operable during playback -- if the
   user switches tracking on or off during play it will almost
   certainly fail. */

#include <SysDeps.h>
#include <ILClient.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include <MidiFile.h>
#include <MidiErrorHandler.h>
#include <MidiVarLenNums.h>
#include <MidiConsts.h>

#include "Menu.h"
#include "Types.h"
#include "Consts.h"
#include "Globals.h"
#include "Main.h"
#include "Tracking.h"

#include <Debug.h>


String trackLevels[] = {
  "No Score Tracking", "Track Bars Only", "Track Every Note"
};

static Widget trackButton = 0;
static int trackLevel = 0;

static void TrackMenuCallback(Widget w, XtPointer client, XtPointer call)
{
  Begin("TrackMenuCallback");
  trackLevel = YGetCurrentOption(trackButton);
  End;
}


Widget Midi_PlayTrackingMenuButton(Widget parent)
{
  Begin("Midi_PlayTrackingMenuButton");
  
  trackButton = YCreateOptionMenu(parent, trackLevels, XtNumber(trackLevels),
				  trackLevel, TrackMenuCallback, 0);

  Return(trackButton);
}
  


/* for interacting with the editor during playback */

static float prevBeat = 0.0;
static Boolean haveFollowService = False;
static Window trackingTargetWindow = 0;

void Midi_PlayTrackingOpen(void)
{
  int i;
  char tempfile[50];
  XtAppContext appContext;
  Begin("Midi_PlayTrackingOpen");

  if (!appData.interlockWindow || trackLevel == 0) End;

  strcpy(tempfile, tmpnam(NULL));
  for (i = strlen(tempfile)-1; i > 0; --i) {
    if (tempfile[i] == '/') { ++i; break; }
  }

  sprintf(tempfile + i, "rose%ld", (long)appData.interlockWindow);
  Midi_SaveFile(tempfile);

  haveFollowService = False;
  trackingTargetWindow = 1;

  IL_RequestService(ILS_FOLLOW_SERVICE, NULL, tempfile, strlen(tempfile) + 1);

  appContext = XtWidgetToApplicationContext(topLevel);
  while ((XtAppPending(appContext) || !haveFollowService) &&
	 (trackingTargetWindow == 1)) {
    XtAppProcessEvent(appContext, XtIMAll);
  }

  (void)unlink(tempfile);
  prevBeat = 0.0;
  End;
}

static void SendToTrackingTarget(String chunk)
{
  static XEvent e;
  static Atom atom = 0;
  Begin("SendToTrackingTarget");

  if (trackingTargetWindow && trackingTargetWindow != 1) {

    if (atom == 0) {
      atom = XInternAtom(display, "Rosegarden Score Tracking Request", False);
      e.type = ClientMessage;
      e.xclient.display = display;
      e.xclient.message_type = atom;
      e.xclient.format = 8;
    }

    strcpy(e.xclient.data.b, chunk);
    e.xclient.window = trackingTargetWindow;
    XSendEvent(display, trackingTargetWindow, False, NoEventMask, &e);
    XFlush(display);

  } else {

    /*    fprintf(stderr,"no target window! sending as IL request\n");*/
    IL_RequestService(ILS_FOLLOW_SERVICE, NULL, chunk, strlen(chunk) + 1);
    XFlush(display);
  }

  End;
}

void Midi_PlayTrackingClose(void)
{
  XtAppContext appContext;
  Begin("Midi_PlayTrackingClose");
  if (!appData.interlockWindow || !haveFollowService || trackLevel == 0) End;

  appContext = XtWidgetToApplicationContext(topLevel);
  while(XtAppPending(appContext)) {
    XtAppProcessEvent(appContext, XtIMAll);
  }

  SendToTrackingTarget(ILS_FOLLOW_UNLOCK);
  trackingTargetWindow = 0;

  End;
}

void Midi_PlayTrackingJump(long delta)
{
  static char btext[20];
  float beat = Midi_TimeToBeat(delta);
  Begin("Midi_PlayTrackingOpen");

  if (!appData.interlockWindow || !haveFollowService || trackLevel == 0) End;

  if ((beat < 0.01) ||
      (trackLevel == 1 && (beat - prevBeat > 0.999)) ||
      (trackLevel == 2 && (beat - prevBeat > 0.01))) {
    /*
        fprintf(stderr, "sending delta %ld, beat %f\n", delta, beat);
	*/
    sprintf(btext, "%s%f", trackLevel == 1 ? "-" : "", beat);
    SendToTrackingTarget(btext);
    prevBeat = beat;
  }

  End;
}


void Midi_FollowReadyService(String chunk)
{
  Begin("Midi_FollowReadyService");

  if (!strcmp(chunk, ILS_FOLLOW_DEAD)) {

    haveFollowService = False;
    /*    trackingTargetWindow = 0;*/

  } else {

    haveFollowService = True;
    trackingTargetWindow = atol(chunk);
  }

  IL_AcknowledgeRequest(ILS_FOLLOWREADY_SERVICE, IL_SERVICE_OK);
  End;
}
