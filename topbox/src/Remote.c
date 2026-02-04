
#include <SysDeps.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <ILClient.h>
#include <stdio.h>
#include <unistd.h>
#include <Debug.h>

#include "General.h"
#include "Remote.h"


static Boolean TextPropertySays(XTextProperty *text, String cmp)
{
  int count;
  char **list = 0;

  Begin("TextPropertySays");

  if (text->encoding == XA_STRING && text->format == 8 && text->nitems >= 1 &&
      XTextPropertyToStringList(text, &list, &count) &&
      count > 0 && !strcmp(cmp, list[0])) {

    XFreeStringList(list);
    Return(True);
  }
  
  if (list) XFreeStringList(list);
  Return(False);
}


static Window FindRosegarden(Display *d, Window w)
{
  int i;
  Window found;
  Window root, parent, *children = 0;
  unsigned int n;
  XTextProperty text;

  Begin("FindRosegarden");

  if (!XQueryTree(d, w, &root, &parent, &children, &n)) Return(0);
  if (n == 0 || !children) Return(0);

  for (i = 0; i < n; ++i) {

    if (XGetWMName(d, children[i], &text) &&
	TextPropertySays(&text, ProgramName) &&
	XGetWMIconName(d, children[i], &text) &&
	TextPropertySays(&text, ProgramName)) {

      found = children[i];
      XFree(children);
      Return(found);
    }

    if ((found = FindRosegarden(d, children[i]))) {
      XFree(children);
      Return(found);
    }
  }

  if (children) XFree(children);
  Return(0);
}


static Boolean done = False;


static void RemoteNullTimeOut(XtPointer a, XtIntervalId *id) { done = True; }
static void RemoteNullCallback(String x) { done = True; }


static void RemoteAcknowledgeCallback(IL_ReturnCode rtn)
{
  Begin("RemoteAcknowledgeCallback");

  IL_DeRegisterWithServer(ILS_NULL_SERVICE);

  switch(rtn) {

  case IL_SERVICE_OK: break;

  case IL_SERVICE_BUSY:
    fprintf(stderr, "%s: Sorry, the existing session is busy.\n",
	    ProgramName);
    break;

  case IL_SERVICE_FAILED:
    /*    fprintf(stderr, "Couldn't open file.  Sorry.\n");*/
    break;

  case IL_NO_SUCH_SERVICE:
    fprintf(stderr, "%s: I'm confused.\n", ProgramName);
    break;
  }

  done = True;
  End;
}


extern void Goodbye(int);


extern Boolean OpenRemote(Widget w, String file, Window server)
{
  char tag[5];
  char cwd[1000];
  char fullfname[1200];
  XtAppContext context;
  FILE *fd;
  char *service;

  Begin("OpenRemote");

  if (file[0] == '/') {
    sprintf(fullfname, "%s", file);
  } else if (!getcwd(cwd, 1000)) {
    perror("OpenRemote");
    Return(False);
  } else {
    sprintf(fullfname, "%s/%s", cwd, file);
  }

  if ((fd = fopen(fullfname, "r")) == NULL) {

    char errmsg[1000];
    sprintf(errmsg, "%s: %s", ProgramName, fullfname);
    perror(errmsg);
    Goodbye(1);
    Return(True);

  } else {

    fgets(tag, 5, fd);
    fclose(fd);

    if (!strcmp(tag, "#!Ro")) {
      service = ILS_EDIT_SERVICE;
    } else {
      service = ILS_SEQUENCE_SERVICE;
    }
  }

  if (!server) {

    fprintf(stderr,
	    "%s: Searching for an existing %s session on this display...\n",
	    ProgramName, ProgramName);

    server = FindRosegarden(XtDisplay(w), RootWindowOfScreen(XtScreen(w)));
  }

  if (!server) {
    fprintf(stderr, "%s: Couldn't locate a %s session: starting one up...\n",
	    ProgramName, ProgramName);
    Return(False);
  }

  IL_ClientInit(server, w, NULL);
  IL_RegisterWithServer(ILS_NULL_SERVICE, RemoteNullCallback);

  fprintf(stderr, "%s: Trying to open file...\n", ProgramName);

  done = False;
  context = XtWidgetToApplicationContext(w);
  XtAppAddTimeOut(context, 10000, RemoteNullTimeOut, 0);

  IL_RequestService(service, RemoteAcknowledgeCallback,
		    fullfname, strlen(fullfname) + 1);

  while (!done && XtAppPending(context)) XtAppProcessEvent(context, XtIMAll);

  Return(True);
}

