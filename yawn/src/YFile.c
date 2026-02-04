
/*
   A natty popup window for file selection purposes.

   Chris Cannam, Berlin May 1993
   Revised by Chris Cannam, Bath February 1994 - London, 1996
*/

/*
   Using It
   ========

   There    is one  external  function:  YFileGetFileInformation(), as
   follows:

   Boolean YFileGetFileInformation(
     Widget  parent,
     String  purpose,
     String  error,
     String  path,
     String  type,
     Boolean testp,
     Boolean openp,
     Boolean backupp,
     void (* callback)(String, FILE *),
     String  helptag,
     void (* helpcb)(Widget, XtPointer, XtPointer));

   parent -- Widget  which the  file   selector window's  outer  shell
   widget should use as a parent.

   purpose -- String to display in  the Apply button's label.  You may
   wish this to say Save, Load or whatever.  If you leave it NULL, the
   button will be labelled Apply.

   error -- String  to display as an  error message in the event  that
   there is nothing from  which to choose --  ie. if none of  the path
   directories  (see "path") exist, or   if none of  the  choices in a
   selection file  are  available.  If  the error  message is NULL,  a
   generic message will be used instead.

   path -- This string may be one of the following:

   [1] the name of  a directory, in which case   the contents of  that
   directory are shown in the selection box;

   [2] a  list of  directory names,  separated  by colons,  where  the
   contents  of the selection  box will be those of  the  first in the
   path which proves both to exist and to be a real directory;

   [3] the name of a file, in which case that  file is taken as a list
   of  possibilities   from which to    choose, in  a file  format  as
   described below;

   [4]  a list of  file names, separated  by pipe symbols (`|'), where
   the first file in the  list found to exist  and be readable will be
   used to provide the list in the same manner as in [3];

   [5] NULL, in which case the current directory is assumed.

   Note that in all cases environment  variable expansion is performed
   on  the  path names, and  names  not beginning  with a  slash after
   expansion are taken relative to the current directory.

   Notice also that if there is only one element in the list (case [1]
   or  [3] above), the   resulting  behaviour will be  dependent  upon
   whether  it  pointed  to a  directory   or  file;  if  several, the
   behaviour will depend on whether the first dividing character was a
   colon or a  pipe.  Passing a  string containing both colon and pipe
   characters is not sensible.

   returnPath -- If non-NULL, points to a char* variable which will be
   set upon successful return to point  to a new heap-allocated string
   giving  the directory the  file   browser was browsing when  popped
   down.

   type -- This is  a string similar to  the  type argument passed  to
   fopen(3),  only without support for  binary modes or anything fancy
   like that.   Hence the valid strings  are "r", "w", "a", "r+", "w+"
   or "a+".  See the man page for fopen(3).  If this is NULL, "r" will
   be assumed.

   testp -- Flag  to   declare   whether destructive, creative      or
   infeasible filename requests should  be queried.  If True, a  query
   box will be popped up if one of the following conditions holds:

   [1] the type is "a", "a+" or "r+" and the file does not exist;

   [2] the type is "w", "w+" or "r+" and the file exists.

   If the  type  is "r", the  user  will not  be allowed to  choose  a
   nonexistent file, whether  testp is True or  not; likewise,  if the
   relevant permissions are unavailable on the file or directory or if
   for some other reason the file cannot be opened, the choice will be
   denied.

   openp -- Flag  to declare whether to open  the file.  If False, the
   filename will be only tested for suitability and  the file will not
   be opened (or created).   Otherwise it will be  opened and a  FILE*
   stream associated with it to pass to the callback (below).

   backupp -- Flag  to declare whether  to  make backups  of files for
   writing.  If True, files for writing will be renamed by appending a
   tilde to their filename.  This will  happen regardless of the value
   of openp; be warned that if a filename is requested for writing but
   is not opened, making a backup could be very destructive.

   callback --  Function to  be   called when the filename   has  been
   obtained.  It is assumed to take two arguments,  which are a string
   (the name   of the file,  either   absolute or  relative,  variable
   expansions already performed) and a FILE* stream pointer.  If openp
   was  False and the file   has not been opened,  the  stream will be
   NULL;  if no filename  was obtained,  the file string  will also be
   NULL.  If the callback function given is  NULL, no callback will be
   called.  (But what's  the point?)   Note  that the filename  string
   passed is obtained from a call to XtMalloc  and may safely be freed
   by calling XtFree.

   helptag -- String  to be passed  as the client-data argument to the
   help callback when the help button is pressed on  the file box.  If
   NULL, there will be no help button.

   helpcb -- Callback to be called when the  help button is pressed on
   the file box.  If NULL, or if `helptag' was NULL,  there will be no
   help button.  The  value  of `helptag'  will be  passed in to   the
   callback as its client data (second) argument.

   If a file is given as the path argument, it should contain a series
   of entries, all  referring to a  single filename, and  separated by
   single carriage returns, of the following format:

   <desc>::<file>::<name>

   <desc> a textual  description of the  file.  This is the text  that
             will appear as a selection in the Selection Box.

   <file> the pathname of a file  (on which variable expansion will be
	     carried out),  given purely  for  testing  purposes.  The
	     <desc> text will only  be listed as  an option if  one of
	     the following conditions is met:

             [1] the type is "r" and <file> exists and is readable;

	     [2] the type  is "w", "a",  "w+", "r+" or "a+" and <file>
	     does not exist  but can  be  created with the   necessary
	     permissions;

	     [3] the   type is "w"  or  "a" and  <file>  exists and is
	     writable;

	     [4] the type is "r+", "w+"  or "a+" and <file> exists and
	     is both readable and writable.

	     Note that if  the type is "w" or  "w+", an  existing file
	     can be overwritten without complaint.   The testp file is
	     not referred  to here;   the behaviour is  as  if  it was
	     False.   The openp flag, however,  is  used; if True, the
	     file is opened with the relevant type, and a file pointer
	     passed as normal to the callback; note, however, that the
	     name  passed to the callback will  not in  general be the
	     name of the opened file.

   <name>  a string  to identify  the  selection.   When the  callback
	     function is called, its  arguments will be (respectively)
	     this string, and  NULL.  If no selection was successfully
	     made, the first argument will also be NULL.

   No  entry (in the total   of its <desc>::<file>::<name>  structure)
   should be longer than 512 characters; this  is likely to be less of
   a practical drawback than the fact that it must all fit on one line
   in the file.

   So as an example, XReduce might use a file containing lines similar
   to this one (only flush-left):

      The Taylor Series Package::$reduce/fasl/taylor.b::taylor

   then "The Taylor Series  Package" would be  listed as an option, if
   the file  $reduce/binary/taylor.b was available (presuming the type
   given was "r"),  and the string  "taylor" would be returned to  the
   callback if this was selected.

   The normal return value from this function is True.  If none of the
   directories in the  directory path exist,  or if a file is referred
   to and none of its choices are  possible, or if something else goes
   wrong somewhere, then instead  of the selection window a  complaint
   window  will be popped  up, containing  the specified error message
   and a   Cancel button;  then the  function   returns False and  the
   callback, which is called when Cancel is pressed, receives two NULL
   arguments.
*/

/* 
   First include various things -- standard headers, plus X stuff.
   Do not include anything specific to the host application.
*/

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif

#include <SysDeps.h>

#ifndef POSIX_PLEASE
#ifndef NO_SYS_ERRLIST
#define __USE_BSD 1	    /* Linux errno.h recognises this for sys_errlist */
#endif
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

#include "Yawn.h"
#include <Debug.h>

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

#ifndef NAME_MAX
#define NAME_MAX 512
#endif

#ifdef NO_VIEWPORT_SET_FUNCTIONS
#define FileViewportSetHeight(v,c,y)
#else
#define FileViewportSetHeight(v,c,y) { XawViewportSetCoordinates((v),0,(y)); }
#endif

#define Try(x)          { if ( (x) == False ) { goto oops; } else {}; }

#define MAXNROFFILES 512

static char **fileFilterOptions = 0;

static Widget filePopup = NULL;
static Widget filePane;
static Widget fileDir;
static Widget fileViewport;
static Widget fileList;
static Widget fileName;
static Widget fileButtonBox;
static Widget fileApply;
static Widget fileFilter;
static Widget fileCancel;
static Widget fileHelp;
static Widget fileParent;

typedef struct _ftstruct {
  char  * name;			/* Filename                  */
  char  * call;			/* Name as listed on window  */
  char  * retn;			/* Name to return (if index) */
  mode_t  mode;			/* Mode of file              */
  uid_t   uid;			/* UID and GID of file       */
  gid_t   gid;			/* (not used if index file)  */
} ftstruct;

static char     *directory = NULL;
static char    **filecall  = NULL;
static ftstruct *filelist  = NULL;
static char     *filename  = NULL;
static int       filecount = 0;

static char    **ReturnPath = NULL;

static char     *suffix = 0;

static char     *Error;
static char     *Type;
static Boolean   Testp;
static Boolean   Openp;
static Boolean   Backupp;
static Boolean   FilterSuffix;/* True if we should display only suffix types */
static Boolean   GlobDots;    /* True if we should display dotfiles and such */
static Boolean   ShowBackups; /* True if we should list backup files &c      */
static Boolean   File;	      /* True if we're using an index file, in which */
			      /* case, cunningly, `directory' will be a file */
static Boolean   Editable;    /* True if we should allow the user to edit    */
                              /* the filename (ie. if not opening read-only) */

static void    (*Callback)(String, FILE *);

static YMenuId  fileDirMenuId = NULL;


void FileList          (Widget, XtPointer, XtPointer);
void FileName          (Widget, XtPointer, XtPointer);
void FileApply         (Widget, XtPointer, XtPointer);
void FileFilter        (Widget, XtPointer, XtPointer);
void FileCancel        (Widget, XtPointer, XtPointer);
void FileRescan        (Widget, XtPointer, XtPointer);
void FileAbandon       (void);
void FileAbandonAction (Widget, XEvent *, String *, Cardinal *);

Boolean FileSetDirLabel     (void);
Boolean FileSetFileLabel    (void);
Boolean FileSetFileNameList (void);
Boolean FileGetFileList     (void);
Boolean FileChDir           (String);



void FileError(String message, String arg, Boolean useErrNo)
{
  char *msg;

#ifdef NO_SYS_ERRLIST

  if (arg) {

      msg = (char *)XtMalloc(strlen(message) + strlen(arg) + 100);
      strcpy(msg, "File Handler: ");
      sprintf(msg + strlen(msg), message, arg);
      perror(msg);

  } else {

      msg = (char *)XtMalloc(strlen(message) + 100);
      strcpy(msg, "File Handler: ");
      strcpy(msg + strlen(msg), message);
      perror(msg);
  }

#else

  if (useErrNo && errno < sys_nerr)

    if (arg) {

      msg = (char *)XtMalloc(strlen(message) + strlen(arg) + 200);
      sprintf(msg, message, arg);

      strcpy(msg + strlen(msg), ": ");
      strcpy(msg + strlen(msg), sys_errlist[errno]);
      /*      strcpy(msg + strlen(msg), ")");*/

    } else {

      msg = (char *)XtMalloc(strlen(message) + 200);
      sprintf(msg, "%s: %s", message, sys_errlist[errno]);
    }

  else 

#endif /* NO_SYS_ERRLIST */

    if (arg) {

      msg = (char *)XtMalloc(strlen(message) + strlen(arg) + 2);
      sprintf(msg, message, arg);

    } else msg = XtNewString(message);

  (void)YQuery(fileParent, msg, 1, 0, 0, "OK", NULL);

  XtFree(msg);
}
  

void FileQuery(String message, void (*callback)(Boolean))
{
  char *msg;
  int   answer;

  if (filename) {

    msg = (char *)XtMalloc(strlen(message) + strlen(filename) + 2);
    sprintf(msg, message, filename);

  } else msg = XtNewString(message);

  answer = YQuery(fileParent, msg, 2, 1, 1, "Yes", "No", NULL);

  XtFree(msg);
  callback((answer == 0));
}


Boolean FileDestroyWidgets(void)
{
  if (filePopup) {
    YDestroyOptionMenu(fileFilter);
    XtDestroyWidget(filePopup);
  }
  filePopup = NULL;
  return True;
}


Boolean FileCreateWidgets(String purpose, String helptag,
			  void (*helpcb)(Widget, XtPointer, XtPointer))
{
  filePopup = XtCreatePopupShell("File Selector",
				 transientShellWidgetClass,
				 fileParent, NULL, 0);

  YInitialise(filePopup, NULL);

  filePane      = YCreateWidget("File Pane", panedWidgetClass, filePopup);

  fileDir       = YCreateMenuButton
    ( "File Dir", YCreateShadedWidget
     ("File Dir",  boxWidgetClass, filePane,  MediumShade));

  fileViewport  = YCreateWidget
    ( "File View", viewportWidgetClass, YCreateShadedWidget
     ("File View", formWidgetClass, filePane, LightShade));

  if (fileFilterOptions[0]) {
    fileFilter = YCreateOptionMenu(XtParent(fileViewport), fileFilterOptions,
				   4, 0, FileFilter, 0);
  } else {
    fileFilter = YCreateOptionMenu(XtParent(fileViewport), fileFilterOptions+1,
				   3, 0, FileFilter, 0);
  }

  fileList = YCreateWidget("File List", listWidgetClass, fileViewport);

  /*  if (Editable) {*/

    fileName = YCreateCommand
      ( "File Name", YCreateShadedWidget
       ("File Name", boxWidgetClass, filePane, MediumShade));
    /*
  } else {

   fileName = YCreateLabel
      ( "File Name", YCreateShadedWidget
       ("File Name", boxWidgetClass, filePane, MediumShade));
  }
  */

  fileButtonBox = YCreateShadedWidget
    ("File Buttons", boxWidgetClass, filePane, MediumShade);

  fileApply     = YCreateCommand(purpose,  fileButtonBox);
  fileCancel    = YCreateCommand("Cancel", fileButtonBox);

  if (helptag && helpcb) {
    fileHelp    = YCreateCommand("Help",   fileButtonBox);
    XtAddCallback(fileHelp, XtNcallback, helpcb, (XtPointer)helptag);
  } else fileHelp = NULL;

  XtAddCallback(fileList,   XtNcallback, FileList,   0);
  XtAddCallback(fileApply,  XtNcallback, FileApply,  0);
  XtAddCallback(fileCancel, XtNcallback, FileCancel, 0);

  /*  if (Editable)*/
    XtAddCallback(fileName, XtNcallback, FileName,   0);

  fileDirMenuId = NULL;

  return True;
}


Boolean FileContourAndInstallWidgets(void)
{
  Dimension   h;
  Dimension   h2;
  Dimension   w;
  Dimension   sWd;
  Dimension   sHt;
  Position    rxr;
  Position    ryr;
  Atom        wmPrtcls[2];

  static XtActionsRec yFileActions[] = {
    { "y-file-wm-quit", FileAbandonAction },
  };

  YSetValue(fileViewport,  XtNallowVert,          True);
  YSetValue(fileViewport,  XtNforceBars,          True);
  YSetValue(fileName,      XtNjustify,   XtJustifyLeft);
  YSetValue(fileName,      XtNeditType,    XawtextEdit);

  YSetValue(XtParent(fileFilter), XtNfromVert, fileViewport);
  YSetValue(XtParent(fileFilter), XtNtop,    XawChainBottom);
  YSetValue(XtParent(fileFilter), XtNbottom, XawChainBottom);

  YSetValue(fileViewport, XtNtop,       XawChainTop);
  YSetValue(fileViewport, XtNbottom, XawChainBottom);
  YSetValue(fileViewport, XtNleft,     XawChainLeft);
  YSetValue(fileViewport, XtNright,   XawChainRight);

  Try(FileGetFileList());

  if ((filecount == 0) && File) {

    FileError(Error, NULL, False);
    FileAbandon();
    return False;
  }

  Try(FileSetDirLabel());
  Try(FileSetFileLabel());
  Try(FileSetFileNameList());

  XtRealizeWidget(filePopup);
  YSetViewportScrollbarPixmaps(fileViewport);
  XtTranslateCoords(fileParent, 150, -15, &rxr, &ryr);

  YGetValue(XtParent(fileApply),    XtNheight, &h);
  YGetValue(XtParent(fileName),     XtNheight, &h2);
  YGetValue(filePopup,              XtNwidth,  &w);

  if (h2 > h) h = h2;

  sWd =  DisplayWidth(XtDisplay(fileParent),
		      DefaultScreen(XtDisplay(fileParent)));
  sHt = DisplayHeight(XtDisplay(fileParent),
		      DefaultScreen(XtDisplay(fileParent)));

  if (rxr + 400 > sWd) rxr = sWd - 400;
  if (ryr + 400 > sHt) ryr = sHt - 400;
  if (rxr < 0) rxr = 0;
  if (ryr < 0) ryr = 0;

  YSetValue(filePopup, XtNx, rxr);
  YSetValue(filePopup, XtNy, ryr);

  YSetValue(XtParent(XtParent(fileDir)),   XtNmin, h + 10);
  YSetValue(XtParent(XtParent(fileDir)),   XtNmax, h + 10);
  YSetValue(XtParent(XtParent(fileName)),  XtNmin, h + 10);
  YSetValue(XtParent(XtParent(fileName)),  XtNmax, h + 10);
  YSetValue(XtParent(XtParent(fileApply)), XtNmin, h + 10);
  YSetValue(XtParent(XtParent(fileApply)), XtNmax, h + 10);

  XtPopup(filePopup, XtGrabNonexclusive);
  XtAppAddActions(XtWidgetToApplicationContext(filePopup),
		  yFileActions, XtNumber(yFileActions));

  YFixOptionMenuLabel(fileFilter);

  YAssertDialogueActions(filePopup, fileApply, fileCancel, fileHelp);

  wmPrtcls[0] =
    XInternAtom(XtDisplay(filePopup), "WM_DELETE_WINDOW", False);
  wmPrtcls[1] =
    XInternAtom(XtDisplay(filePopup), "WM_SAVE_YOURSELF", False);

  XtOverrideTranslations
    (filePopup,
     XtParseTranslationTable("<Message>WM_PROTOCOLS: y-file-wm-quit()"));

  XSetWMProtocols(XtDisplay(filePopup), XtWindow(filePopup), wmPrtcls, 2);

  return True;

 oops: return False;
}


void FileDirCallback(Widget w, XtPointer a, XtPointer b)
{
  Try(FileChDir(((YMenuElement *)a)->label));
  return;

 oops: return;
}


Boolean FileSetDirLabel(void)
{
  int                   i, j;
  int                   len;
  char                 *dn;
  static char          *dl = NULL;
  static int            ec = 0;
  static YMenuElement  *elts = NULL;

  XtUnmanageChild(XtParent(fileDir));
  XtUnmanageChild(fileDir);

  if (directory) {

    if (dl) XtFree(dl);
    if (directory[len = strlen(directory)] == '/') directory[len] = 0;

    for (i = len; i > 0 && directory[i-1] != '/'; --i);

    if (i < len && i > 0) dl = XtNewString(directory + i);
    else dl = XtNewString(directory);
    YSetValue(fileDir, XtNlabel, dl);

  } else YSetValue(fileDir, XtNlabel, "<none>");

  XtSetSensitive(fileDir, directory ? True : False);
  XtManageChild(fileDir);
  XtManageChild(XtParent(fileDir));

  if (directory) {

    if (fileDirMenuId) YDestroyMenu(fileDirMenuId);

    /* this used to be before the "if (directory)" line, but then
       YDestroyMenu would be destroying widgets from elts after this
       loop had already freed them */
    if (elts) {
      for (i = 0; i < ec; ++i) XtFree(elts[i].label);
      XtFree((char *)elts);
      elts = NULL;
    }

    for (i = len, ec = 1; i > 0; --i) if (directory[i-1] == '/') ++ec;
    if (len == 1 && ec == 2) ec = 1;

    dn = XtNewString(directory);
    elts = (YMenuElement *)XtCalloc(ec, sizeof(YMenuElement));

    for (i = 0; i < ec; ++i) {

      elts[i].label                 = XtNewString(dn);
      elts[i].insensitive_mode_mask = 0L;
      elts[i].callback              = (XtCallbackProc)FileDirCallback;
      elts[i].widget                = NULL;

      for (j = strlen(dn); j > 0; --j)
	if (dn[j-1] == '/') { dn[j>1 ? j-1 : j] = '\0'; break; }
    }

    XtFree(dn);
    fileDirMenuId = YCreateMenu(fileDir, "Directories", ec, elts);
  }

  return True;
}


/* Scan a string, expanding all environment variables found in it (ie.  */
/* anything beginning with `$'.  Return a new string, in space obtained */
/* from XtMalloc, containing the expanded version.  If any environment  */
/* variable is unobtainable, complain to stderr (only the first time it */
/* happens), and then return NULL.  Non-NULL returns can be XtFree'd.   */

char *FileExpandForEnvironment(String str)
{
  char          *vv;
  int            i,j,k;
  static char    vn[NAME_MAX + 1];
  static char    wk[NAME_MAX + PATH_MAX + 1];
  static Boolean complained = False;

  for (i = k = 0; str[i]; ) {

    if (str[i] == '$') {

      for (j = ++i; str[j] && j-i < NAME_MAX &&
	   (isalnum(str[j]) || str[j] == '_'); ++j) vn[j-i] = str[j];

      vn[j-i] = 0;

      if ((vv = getenv(vn))) { strcpy(wk + k, vv); k += strlen(vv); }
      else {
	if (!complained) {
	  fprintf(stderr,
        "File Handler: Couldn't get value of environment variable $%s\n", vn);
	  complained = True;
	} return NULL;
      }

      i = j;

    } else wk[k++] = str[i++];
  }

  wk[k] = 0;

  return XtNewString(wk);
}



Boolean FileSetFileLabel(void)
{
  XtUnmanageChild(fileName);
  YSetValue(fileName, XtNlabel, filename ? filename : "         ");
  XtManageChild(fileName);
  return True;
}


Boolean FileSetFileNameList(void)
{
  Arg            arg[4];
  int            i;
  int            dir;
  int            asc;
  int            dsc;
  int            mwd = 0;
  static Boolean fileNLSet = False;
  XCharStruct    info;
  XFontStruct   *font;

  YGetValue(fileList, XtNfont, &font);

  for (i = 0; i < filecount && filecall[i]; ++i) {

    XTextExtents(font, filecall[i], strlen(filecall[i]),
		 &dir, &asc, &dsc, &info);

    if (info.width > mwd) mwd = info.width;
  }

  if (!fileNLSet) {

    i = 0;
    XtSetArg(arg[i], XtNlist,          filecall);  i++;
    XtSetArg(arg[i], XtNnumberStrings, filecount); i++;
    XtSetArg(arg[i], XtNlongest,       mwd > 300 ? mwd+12 : 300);  i++;
    XtSetValues(fileList, arg, i);
    fileNLSet = True;

  } else {

    XawListChange(fileList, filecall, filecount, mwd > 300 ? mwd+6 : 300, True);
  }

  XawListUnhighlight(fileList);

  if (XtIsRealized(fileViewport) &&
      XtIsRealized(fileList)) {
    FileViewportSetHeight(fileViewport, fileList, 0);
  }

  return True;
}



char *FileDirCat(String a, String b)
{
  char *r;
  char *s;
  char *t;

  r = (char *)XtMalloc(strlen(a) + strlen(b) + 2);
  sprintf(r, "%s/%s", a, b);

  if (!(s = getcwd(NULL, PATH_MAX + NAME_MAX)))
    { XtFree(r); return NULL; }

  if (chdir(r))
    { XtFree(r); free(s); return NULL; }

  if (!(t = getcwd(NULL, PATH_MAX + NAME_MAX)))
    { XtFree(r); free(s); return NULL; }

  if (chdir(s))
    { XtFree(r); free(s); free(t); return NULL; }

  XtFree(r); free(s);
  r = XtNewString(t);
  free(t); return r;
}



/* This function makes the visible representation of the filename.     */
/* It is passed i, and assumes filename[i] and filetype[i] are present */
/* and correct in order to produce a name (to go into filecall[i]).    */

char *FileMakeFileCall(int i)
{
  char *call;
  uid_t uid;
  gid_t gid;
  int   len;
  char  ind = 0;

  uid  = getuid();
  gid  = getgid();
  call = (char *)XtMalloc(4 + (len = strlen(filelist[i].name)));

  if      (S_ISDIR(filelist[i].mode))  ind = '/';
#ifdef S_ISLNK
  else if (S_ISLNK(filelist[i].mode))  ind = '@';
#endif
#ifdef S_ISSOCK
  else if (S_ISSOCK(filelist[i].mode)) ind = '=';
#endif
  else if ((filelist[i].uid == uid && (filelist[i].mode & S_IXUSR)) ||
	   (filelist[i].gid == gid && (filelist[i].mode & S_IXGRP)) ||
	   (filelist[i].uid != uid &&  filelist[i].gid != gid &&
	    (filelist[i].mode & S_IXOTH))) ind = '*';

#ifdef NOT_DEFINED
  if      ((filelist[i].mode & S_IFDIR)  == S_IFDIR)  ind = '/';
  else if ((filelist[i].mode & S_IFLNK)  == S_IFLNK)  ind = '@';
  else if ((filelist[i].mode & S_IFSOCK) == S_IFSOCK) ind = '=';
  else if ((filelist[i].uid == uid && (filelist[i].mode & S_IXUSR)) ||
	   (filelist[i].gid == gid && (filelist[i].mode & S_IXGRP)) ||
	   (filelist[i].uid != uid &&  filelist[i].gid != gid &&
	    (filelist[i].mode & S_IXOTH))) ind = '*';
#endif

  sprintf(call, " %s%c%s", filelist[i].name, ind ? ind : ' ', ind ? " " : "");
  return  call;
}


int FileStrCmpP(const void *a, const void *b)
{
  ftstruct *ca;
  ftstruct *cb;

  ca = (ftstruct *)a;
  cb = (ftstruct *)b;

  if (File) return strcmp(ca->call, cb->call);

  if (ca->name[0] == '.' && cb->name[0] != '.') return -1;
  if (cb->name[0] == '.' && ca->name[0] != '.') return  1;

  if   (S_ISDIR(ca->mode))
    if (S_ISDIR(cb->mode)) return strcmp(ca->name, cb->name);
    else return -1;
  else
    if (S_ISDIR(cb->mode)) return 1;

  return strcmp(ca->name, cb->name);
}


Boolean FileTestIndexFileStat(String np)
{
  struct stat bf;
  uid_t       uid;
  gid_t       gid;
  Boolean     rst;

  uid = getuid();
  gid = getgid();

  if (!stat(np, &bf)) {

    if (!strcmp(Type, "r")  || !strcmp(Type, "r+") ||
	!strcmp(Type, "w+") || !strcmp(Type, "a+")) {

      rst = ((bf.st_uid == uid && (bf.st_mode & S_IRUSR)) ||
	     (bf.st_gid == gid && (bf.st_mode & S_IRGRP)) ||
	     (bf.st_uid != uid &&
	      bf.st_gid != gid && (bf.st_mode & S_IROTH)));

      if (!rst || !strcmp(Type, "r")) return rst;
    }

    if (!strcmp(Type, "w")  || !strcmp(Type, "a")  ||
	!strcmp(Type, "r+") || !strcmp(Type, "w+") || !strcmp(Type, "a+")) {

      rst = ((bf.st_uid == uid && (bf.st_mode & S_IWUSR)) ||
	     (bf.st_gid == gid && (bf.st_mode & S_IWGRP)) ||
	     (bf.st_uid != uid &&
	      bf.st_gid != gid && (bf.st_mode & S_IWOTH)));

      return rst;
    }
  } else {

    if (errno == ENOENT &&
	(!strcmp(Type, "w")  || !strcmp(Type, "a")  ||
	 !strcmp(Type, "r+") || !strcmp(Type, "w+") || !strcmp(Type, "a+"))) {

      if (creat(np, S_IRUSR | S_IWUSR
#ifdef S_IFREG
		| S_IFREG
#endif
		) >= 0) {

	if (unlink(np)) perror("File Handler");
	return True;

      } else {

	perror("File Handler");
	return False;
      }
    } else return False;
  }

  return False;	     /* never reached, but SGI NCC complains if it's absent */
}



Boolean FileGetFileList(void)
{
  int            i,j;
  int            len;
  struct stat    bf;
  char           path[PATH_MAX + NAME_MAX + 1];

  if (!directory) return False;

  if (filelist) {
    for (i = 0; i < MAXNROFFILES && filelist[i].name; ++i) {
      XtFree(filelist[i].name);
      if (filelist[i].retn) XtFree(filelist[i].retn);
    }
    XtFree((char *)filelist);
  }
  if (filecall) {
    for (i = 0; i < MAXNROFFILES && filecall[i]; ++i) XtFree(filecall[i]);
    XtFree((char *)filecall);
  }
  
  filecall = (char    **)XtCalloc(MAXNROFFILES, sizeof(char   *));
  filelist = (ftstruct *)XtCalloc(MAXNROFFILES, sizeof(ftstruct));

  if (!File) {

    struct dirent *dp;
    DIR           *dirP;

    if (!(dirP = opendir(directory))) return False;

    strcpy(path, directory);
    strcpy(path + (len = strlen(directory)), "/");

    for (dp = readdir(dirP), i = 0; dp != NULL && i < MAXNROFFILES;
	 dp = readdir(dirP),  ++ i) {
      
      char *name = dp->d_name;
      int nlen = strlen(name);

      if (!strcmp(name, ".")  ||
	  !strcmp(name, "..") ||
	  (!GlobDots && (name[0] == '.')) ||
	  (!ShowBackups && (name[j = nlen - 1] == '~' ||
			    name[j] == '#' ||
			    name[j] == '%'))) --i;
      else {
	strcpy(path + len + 1, name);

	if (stat(path, &bf)) --i;
	else if (FilterSuffix &&
		 (nlen < strlen(suffix) ||
		  strcmp(name + nlen - strlen(suffix), suffix)) &&
		 !S_ISDIR(bf.st_mode)) --i;
	else {
	  filelist[i].retn = NULL;
	  filelist[i].name = XtNewString(name);
	  filelist[i].mode = bf.st_mode;
	  filelist[i].uid  = bf.st_uid;
	  filelist[i].gid  = bf.st_gid;
	}
      }
    }

    if (closedir(dirP)) { perror("File Handler"); return False; }
    filecount = i;
    qsort(filelist, (size_t)filecount, sizeof(ftstruct), FileStrCmpP);

    for (i = 0; i < filecount; ++i) {

      filecall[i]      = FileMakeFileCall(i);
      filelist[i].call = filecall[i];
    }
    return True;

  } else {

    int   k, m;
    char *np;
    char  rn[512];
    FILE *fileP;

    if (!(fileP = fopen(directory, "r"))) return False;
    i = 0;

    while(fgets(rn, 511, fileP)) {
      
      for (j = k = 0; rn[j]; ++j) {
	
	if (rn[j] == ':' && rn[j+1] == ':') {

	  if (k == 0) { rn[m = j] = 0; ++k; m += 2; }
	  else {
	    
	    rn[j] = 0;
	
	    /* m was set in an earlier pass */
	    if ((np = FileExpandForEnvironment(rn + m))) {
	      
	      if (!(FileTestIndexFileStat(np))) { XtFree(np); break; }
	      
	      filelist[i].name = np;
	      filelist[i].retn = XtNewString(rn + j + 2);
	      filelist[i].call = XtNewString(rn);
	      
	      if ((len = strlen(filelist[i].retn)) > 0) {
		
		if (filelist[i].retn[len - 1] == '\n')
		  filelist[i].retn[len - 1] = 0;
		i++; break;
	      }
	    }
	  }
	}
      }
    }

    fclose(fileP);
    filecount = i;

    qsort(filelist, (size_t)filecount, sizeof(ftstruct), FileStrCmpP);
    for (i = 0; i < filecount; ++i) filecall[i] = filelist[i].call;

    return True;
  }
}


void FileAbandonAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
  FileAbandon();
}


void FileAbandon(void)
{
  int i;

  if (directory) XtFree(directory);
  directory = NULL;
  if (filename)  XtFree(filename);
  filename  = NULL;
  Callback  = NULL;

  if (filecall) {
    for (i = 0; i < MAXNROFFILES && filecall[i]; ++i) XtFree(filecall[i]);
    XtFree((char *)filecall);
  }
  filecall = NULL;

  if (filelist) {
    for (i = 0; i < filecount; ++i) {
      if (filelist[i].name) XtFree(filelist[i].name);
    } XtFree((char *)filelist);
  }
  filelist  = NULL;
  filecount = 0;

  if (filePopup) {
    YDestroyOptionMenu(fileFilter);
    XtPopdown(filePopup);
    XtDestroyWidget(filePopup);
    YRetractDialogueActions(filePopup);
    filePopup = NULL;
  }
}


/* ARGSUSED */
void FileCancel(Widget w, XtPointer a, XtPointer b)
{
  if (Callback) Callback(NULL, NULL);
  FileAbandon();
  return;
}


Boolean FileChDir(String newdir)
{
  char *nd;

  if (!(nd =
	(newdir[0] == '/') ? XtNewString(newdir) :
	FileDirCat(directory ? directory : ".", newdir))) goto oops;

  if (directory) XtFree(directory);
  directory = nd;

  Try(FileSetDirLabel());
  Try(FileGetFileList());
  Try(FileSetFileNameList());

  filename = NULL;
  Try(FileSetFileLabel());
  return True;

 oops:
  if (!nd && errno == EACCES)
    FileError("Permission denied on directory ``%s''", newdir, False);
  else
    FileError("Cannot read directory ``%s''", newdir, True);
  return False;
}


/* ARGSUSED */
void FileListCancelClick(XtPointer closure, XtIntervalId *id)
{
  int                  n;
  int                  i = 0;
  Arg                  arg[2];
  Dimension            vh;
  Dimension            bh;
  Position             by;
  XawListReturnStruct *d;

  if (closure) *((Boolean *)closure) = False;

  if (!filePopup) return;

  d = XawListShowCurrent(fileList);

  XtSetArg(arg[i], XtNheight,       &bh); i++;
  XtSetArg(arg[i], XtNnumberStrings, &n); i++;
  XtGetValues(fileList, arg, i);

  i = 0;
  XtSetArg(arg[i], XtNheight, &vh); i++;
  XtGetValues(fileViewport, arg, i);

  by = (Position)((((unsigned long int)((unsigned)bh))/n) * d->list_index);

  if (((Dimension)by + (bh/(2*n))) <= (vh/2)) {
    FileViewportSetHeight(fileViewport, fileList, 0);
  } else {
    FileViewportSetHeight(fileViewport, fileList,
			  (Dimension)by + (bh/(2*n)) - vh/2);
  }
}


/* ARGSUSED */
void FileList(Widget w, XtPointer a, XtPointer dd)
{
  static String        firstClicked = NULL;
  static Boolean       alreadyClicked = False;
  XawListReturnStruct *d = (XawListReturnStruct *)dd;

  if (S_ISDIR(filelist[d->list_index].mode)) {

    Try(FileChDir(filelist[d->list_index].name));
    return;

  } else {

    if (filename) XtFree(filename);
    if (File) filename = XtNewString(filelist[d->list_index].call);
    else      filename = XtNewString(filelist[d->list_index].name);
    Try(FileSetFileLabel());

    if (alreadyClicked && firstClicked && !strcmp(filename, firstClicked)) {

      alreadyClicked = False;
      FileApply(fileApply, NULL, NULL);

    } else {

      if (firstClicked) XtFree(firstClicked);
      firstClicked = XtNewString(filename);

      alreadyClicked = True;
      XtAppAddTimeOut(XtWidgetToApplicationContext(filePopup),
		      200, FileListCancelClick, &alreadyClicked);
    }

    return;
  }

 oops: return;
}


void FileName(Widget w, XtPointer a, XtPointer b)
{
  int    i;
  String fn = NULL;
  String rtn, path = 0;
  String tail;
  struct stat bf;

  if (filename) {
    for (i = strlen(filename); i > 0 && filename[i] != '/'; --i);
    fn = XtNewString(filename + (filename[i] == '/' ? i+1 : i));
  }

  rtn = YGetUserInput(XtParent(w), "File name: ", fn, YOrientHorizontal, NULL);

  if (rtn) {
    
    path = (String)XtMalloc(strlen(directory) + strlen(rtn) + 2);

    if (rtn[0] == '/') {
      strcpy(path, rtn);
    } else {
      sprintf(path, "%s/%s", directory, rtn);
    }

    if (stat(path, &bf)) {	/* no luck with the file */
      if (!Editable || strchr(rtn, '/')) { /* if r/o, or explicitly directory */
	FileError("Can't find or open ``%s''", rtn, True);
	goto oops;
      }
    }

    if (S_ISDIR(bf.st_mode)) {

      Try(FileChDir(path));

    } else if ((tail = strrchr(path, '/'))) {

      *tail = '\0';
      Try(FileChDir(path));
      if (filename) XtFree(filename);
      filename = XtNewString(tail + 1);

    } else if (!strcmp(path, "..") || !strcmp(path, ".")) {

      Try(FileChDir(path));

    } else {
      if (filename) XtFree(filename);
      filename = XtNewString(path);
    }

    Try(FileSetFileLabel());
  }

 oops:
  if (fn) XtFree(fn);
  if (path) XtFree(path);
  return;
}


void FileFilter(Widget w, XtPointer a, XtPointer b)
{
  int answer = (int)b;

  FilterSuffix = fileFilterOptions[0] ? answer == 0 : False;
  GlobDots     = fileFilterOptions[0] ? answer >  1 : answer > 0;
  ShowBackups  = fileFilterOptions[0] ? answer == 3 : answer == 2;

  FileRescan(w, a, b);
  return;
}


/* ARGSUSED */
void FileRescan(Widget w, XtPointer a, XtPointer b)
{
  if (!directory) if (!(directory = FileDirCat(".","."))) goto oops;

  Try(FileSetDirLabel());
  Try(FileGetFileList());
  Try(FileSetFileNameList());
  return;

 oops:
  FileError("Couldn't reopen current directory. I'm confused.", NULL, False);
  return;
}


/* Attempt to open a single path element -- ie. a file or directory name, */
/* rather then a full path list (the sort with colons in it).  Expands    */
/* environment variables and makes the path absolute; if fileFlg is True  */
/* it will allow the path to be either file (in which case the file read  */
/* convention will be used) or directory (in which case the `directory'   */
/* variable will be set); otherwise only directories will be allowed.     */
/* If both fileFlg and forceFlg are True, *only* files will be allowed,   */
/* not directories.  Returns True for success and False for failure.      */

Boolean FileOpenPathElt(String path, Boolean fileFlg, Boolean forceFlg)
{
  char       *np;
  struct stat bf;

  if (!(np = FileExpandForEnvironment(path))) return False;

  if (stat(np, &bf)) return False;
  if (S_ISDIR(bf.st_mode) && !(fileFlg && forceFlg)) {

    if (np[0] == '/') directory = FileDirCat(np, ".");
    else              directory = FileDirCat(".", np);

    XtFree(np);

    if (directory) return True;
    else           return False;

  } else if (fileFlg && S_ISREG(bf.st_mode)) {

    directory = np;
    return (File = True);

  } else {

    XtFree(np);
    return False;
  }
}



/* Attempt to open a path and place the current directory in `directory'. */
/* Args are path string, a flag to say whether a file is acceptable as a  */
/* path -- if True, then the file reading convention will be used and a   */
/* list of names (rather than actual files) gleaned from it to present to */
/* the user --, and a flag which if True declares that if there is only a */
/* single element to the path, it must be of the type indicated by the    */
/* file flag (directory if file flag is False and file if True).  Returns */
/* True if successful, or False (and pops up an error box) for failure.   */

Boolean FileOpenPath(String path, Boolean fileFlg, Boolean forceFlg)
{
  int   i;
  int   len;
  int   sc = True;
  char *np = NULL;

  File = False;

  if (!path) {

    directory = FileDirCat(".","");
    if (!directory && !(directory = FileDirCat(".","."))) goto oops;

  } else {

    np = XtNewString(path);

    for (i = 0, len = strlen(np); i < len && np[i] != ':'; ++i);
    if  (i >= len)    for (i = 0; i < len && np[i] != '|'; ++i);
    if  (i >= len) sc = FileOpenPathElt(np, fileFlg, forceFlg);
    else {

      fileFlg  = (np[i] == '|'); np[i] = 0;
      if (!(sc = FileOpenPathElt(np,  fileFlg, True)) &&
	  !(sc = FileOpenPath(np+i+1, fileFlg, True))) goto oops;
    }

    XtFree(np);
    np = NULL;
  }

  if (sc) return True;
  FileError(Error, NULL, False);
  /* fall on through */

 oops:

  if (np) XtFree(np);
  return False;
}



Boolean YFileGetFileInformation(Widget  parent,
				String  purpose,
				String  error,
				String  path,
				String  type,
				String *returnPath,
				Boolean testp,
				Boolean openp,
				Boolean backupp,
				void (* callback)(String, FILE *),
				String  helptag,
				void (* helpcb)(Widget, XtPointer, XtPointer),
				String  sfx,
				String  sfxDescription)
{
  Error       = XtNewString(error);
  Type        = XtNewString(type);
  Testp       = testp;
  Openp       = openp;
  Backupp     = backupp;
  Callback    = callback;
  GlobDots    = False;
  ShowBackups = False;
  Editable    = strcmp(type,"r");
  ReturnPath  = returnPath;

  fileParent = parent;
  Try(FileOpenPath(path, True, False));
  if (!directory) return False;

  if (filename) XtFree(filename);
  filename = NULL;

  if (File) Editable = False;

  if (!fileFilterOptions) {
    fileFilterOptions = (char **)XtMalloc(4 * sizeof(char *));
    fileFilterOptions[0] = 0;
    fileFilterOptions[1] = "Show all normal files";
    fileFilterOptions[2] = "Show normal and dot files";
    fileFilterOptions[3] = "Show all files";
  }

  if (fileFilterOptions[0]) {
    XtFree(fileFilterOptions[0]);
    fileFilterOptions[0] = 0;
  }

  if (sfxDescription) {
    fileFilterOptions[0] = (char *)XtMalloc(strlen(sfxDescription) + 20);
    sprintf(fileFilterOptions[0], "Show %s files only", sfxDescription);
  } else if (sfx) {
    fileFilterOptions[0] = (char *)XtMalloc(strlen(sfx) + 20);
    sprintf(fileFilterOptions[0], "Show %s files only", sfx);
  }

  FilterSuffix = (fileFilterOptions[0] != 0);

  if (suffix) XtFree(suffix);
  if (sfx) suffix = XtNewString(sfx);
  else suffix = NULL;

  Try(FileCreateWidgets(purpose ? purpose : "Apply", helptag, helpcb));
  Try(FileContourAndInstallWidgets());

  return True;

 oops: return False;
}



void FileApplyAux(Boolean success)
{
  struct stat bf;
  uid_t       uid;
  gid_t       gid;
  FILE       *fptr = NULL;

  if (!success) return;

  /* Right, we've checked that the file either exists (if the requested */
  /* type is "r", "a", "r+" or "a+") or doesn't (if the requested type  */
  /* is "w", "w+" or "r+"), or that the user doesn't mind this state of */
  /* affairs; now if we can't read or write (as appropriate) the file,  */
  /* we're still going to have to complain about it.                    */

  /* Of course, we can assume that a nonexistent file can now be safely */
  /* created in silence, as we've checked that below.                   */

  uid = getuid();
  gid = getgid();

  if (stat(filename, &bf)) {	/* stat failed */
    if (errno != ENOENT) {	/* some corruption */

      FileError("Cannot open ``%s''", filename, True);
      return;
    }
  } else {			/* file exists */

    if (!strcmp(Type, "a")  ||
	!strcmp(Type, "a+") ||
	!strcmp(Type, "r")  ||
	!strcmp(Type, "r+")) {

      if (!((bf.st_uid == uid && (bf.st_mode & S_IRUSR)) ||
	    (bf.st_gid == gid && (bf.st_mode & S_IRGRP)) ||
	    (bf.st_uid != uid &&  bf.st_gid != gid &&
	     (bf.st_mode & S_IROTH)))) {
	FileError("Read permission denied on ``%s''", filename, False);
	return;
      }
    }

    if (!strcmp(Type, "a")  ||
	!strcmp(Type, "a+") ||
	!strcmp(Type, "r+") ||
	!strcmp(Type, "w")  ||
	!strcmp(Type, "w+")) {

      if (!((bf.st_uid == uid && (bf.st_mode & S_IWUSR)) ||
	    (bf.st_gid == gid && (bf.st_mode & S_IWGRP)) ||
	    (bf.st_uid != uid &&  bf.st_gid != gid &&
	     (bf.st_mode & S_IWOTH)))) {
	FileError("Write permission denied on ``%s''", filename, False);
	return;
      }
    }

    if (Backupp &&
	(!strcmp(Type, "w") ||
	 !strcmp(Type, "w+"))) {

      char *newname;

      newname = (char *)XtMalloc(strlen(filename) + 250);
      sprintf(newname, "%s~", filename);

      if (rename(filename, newname) != 0) { /* failed to back up */

#ifndef NO_SYS_ERRLIST

	if (errno < sys_nerr) {
	  sprintf(newname, "Error in backing up ``%s''\n(%s)", filename,
		  sys_errlist[errno]);
	} else
#else
	  {
	    sprintf(newname, "Error in backing up ``%s''", filename);
	    perror("File Handler: Error in backup");
	  }
	
#endif /* NO_SYS_ERRLIST */

	if (YQuery(XtParent(fileApply), newname,
		   2, 0, 1, "Continue", "Cancel", NULL) == 1) {
	  XtFree(newname);
	  return;
	}
      }

      XtFree(newname);
    }
  }

  if (Openp && !(fptr = fopen(filename, Type))) {
    FileError("Cannot open ``%s''", filename, True);
    return;
  }

  if (Callback) {
    if (ReturnPath) *ReturnPath = XtNewString(directory);
    Callback(XtNewString(filename), fptr);
  }
  FileAbandon();
  return;
}



/* ARGSUSED */
void FileApply(Widget w, XtPointer a, XtPointer b)
{
  char       *nd;
  Boolean     exists = True;
  struct stat bf;

  if (!filename || strlen(filename) == 0) {
    if (Callback) Callback(NULL,NULL);
    FileAbandon();
    return;
  }

  if (File) {

    int   i;
    FILE *fptr;

    for (i = 0; i < MAXNROFFILES && filelist[i].name &&
	 strcmp(filename, filelist[i].call); i ++);

    if (i >= MAXNROFFILES || !(filelist[i].name)) Callback(NULL, NULL);
    else {

      if (ReturnPath) *ReturnPath = XtNewString(directory);

      if (Openp && (fptr = fopen(filelist[i].name, Type)))
	Callback(XtNewString(filelist[i].retn), fptr);
      else
	Callback(XtNewString(filelist[i].retn), NULL);
    }

    FileAbandon();
    return;

  } else {

    if (filename[0] != '/') {
      nd = (char *)XtMalloc(strlen(directory) + strlen(filename) + 2);
      sprintf(nd, "%s/%s", directory, filename);
      XtFree(filename);
      filename = nd;
    }

    if (stat(filename, &bf))
      if (errno == ENOENT) exists = False;
      else {
	FileError("Cannot open ``%s''", filename, True);
	return;
      }

    if      (Testp &&
	     (exists  &&
	      (!strcmp(Type, "w")    ||
	       !strcmp(Type, "w+")   ||
	       !strcmp(Type, "r+"))))
      FileQuery("File ``%s'' exists. Overwrite?", FileApplyAux);
    
    else if (Testp &&
	     (!exists &&
	      (!strcmp(Type, "a")    ||
	       !strcmp(Type, "a+")   ||
	       !strcmp(Type, "r+"))))
      FileQuery("File ``%s'' doesn't exist. Create it?", FileApplyAux);

    else FileApplyAux(True);
 
    return;
  }
}

