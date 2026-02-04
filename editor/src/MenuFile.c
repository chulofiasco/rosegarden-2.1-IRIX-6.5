
/* MenuFile.c */

/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Handlers for the File Menu, February 1994        */

/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "IO.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StaveCursor.h"
#include "Undo.h"

#include <Yawn.h>
#include <YSmeBSB.h>

#include "buffer.xbm"
#include "toolbar/new.xbm"
#include "toolbar/open.xbm"
#include "toolbar/save.xbm"
#include "toolbar/sequence.xbm"

/* }}} */
/* {{{ Prototypes */

void FileMenuNew     (Widget, XtPointer, XtPointer);
void FileMenuLoad    (Widget, XtPointer, XtPointer);
void FileMenuSave    (Widget, XtPointer, XtPointer);
void FileMenuSaveAs  (Widget, XtPointer, XtPointer);
void FileMenuClose   (Widget, XtPointer, XtPointer);
void FileChangeStave (Widget, XtPointer, XtPointer);

/* }}} */
/* {{{ File format macros */

#define FILE_HEADER "#!Rosegarden\n#\n#  Musical Notation File\n#\n\nRV21\n\n"
#define FILE_FOOTER "\n\nEnd\n"

/* }}} */

/* {{{ Declarations */

typedef struct _FileMenuStaveListElement{
  ListElement typeless;
  String      name;
  String      filename;
  MajorStave  sp;
  Boolean     changed;
} FileMenuStaveListElement, *FileMenuStaveList;

FileMenuStaveList fileMenuStaveList = NULL;

/* }}} */
/* {{{ Menu definition */

#define FileMenuInclude Unimplemented
#define FileMenuPrint Unimplemented
#define FileMenuPlay Unimplemented

static YMenuElement baseFileMenu[] = {
  { "New . . .",         SlaveToSequencerMode,                FileMenuNew,           new_bits, NULL, },
  { "Open . . .",        SlaveToSequencerMode,                FileMenuLoad,          open_bits, NULL, },
  YMenuDivider,
  /*  { "Include . . .",     FileNotLoadedMode | SlaveToSequencerMode | CursorNotPlacedMode,
                                               FileMenuInclude,       NULL, NULL, },*/
  { "Save",           FileNotLoadedMode | SlaveToSequencerMode | NoFilenameToSaveInMode,
                                               FileMenuSave,          save_bits, NULL, },
  { "Save As . . .",     FileNotLoadedMode | SlaveToSequencerMode,       FileMenuSaveAs,        NULL, NULL, },
  { "Close",          FileNotLoadedMode | SlaveToSequencerMode,       FileMenuClose,         NULL, NULL, },
  /*  { "Print . . .",       FileNotLoadedMode | SlaveToSequencerMode,       FileMenuPrint,         NULL, NULL, },*/
  /*  { "Play MIDI",      FileNotLoadedMode | SlaveToSequencerMode | SequencerRunningMode,
                                               FileMenuPlayMidi,     NULL, NULL, },*/
  YMenuDivider,
  { "Import MIDI . . .", NullMode | SlaveToSequencerMode,                FileMenuImportMidi,   NULL, NULL, },
  { "Export MIDI . . .", FileNotLoadedMode | SlaveToSequencerMode,       FileMenuWriteMidi,    NULL, NULL, },
  { "Export MusicTeX . . .",FileNotLoadedMode | SlaveToSequencerMode,       FileMenuExportMusicTeX,    NULL, NULL, },
  { "Export OpusTeX . . .",FileNotLoadedMode | SlaveToSequencerMode,       FileMenuExportOpusTeX,    NULL, NULL, },
  { "Export PMX . . .",FileNotLoadedMode | SlaveToSequencerMode,       FileMenuExportPMX,    NULL, NULL, },
  YMenuDivider,
  { "Sequence!",       FileNotLoadedMode | SlaveToSequencerMode | SequencerRunningMode,
                                               FileMenuSequence,     sequence_bits, NULL, },
  /*  { "Preferences . . .", NullMode | SlaveToSequencerMode,                FileMenuPreferences,  NULL, NULL, },*/
  { "Exit",           NullMode,                QuitNicelyCallback,    NULL, NULL, },
};

static Widget        fileMenuButton;
static YMenuElement *fileMenu   = NULL;
static int           fileMenuCount;
YMenuId              fileMenuId = NULL;

/* }}} */

/* {{{ Internal manipulation functions */

static FileMenuStaveList NewFileMenuStaveList(String name, String filename,
					      MajorStave sp)
{
  FileMenuStaveList list;

  Begin("NewFileMenuStaveList");

  list = (FileMenuStaveList)NewList(sizeof(FileMenuStaveListElement));
  list->name     = name;
  list->filename = filename;
  list->sp       = sp;
  list->changed  = False;

  Return(list);
}
  

static void MakeFileMenuFromBase(String changeTo)
{
  int               i, j = -1;
  int               len, orig;
  FileMenuStaveList list;
  static Pixmap     map = 0;

  Begin("MakeFileMenuFromBase");

  if (!map)
    map = XCreateBitmapFromData
      (display, RootWindowOfScreen(XtScreen(topLevel)),
       buffer_bits, buffer_width, buffer_height);

  len = Length(fileMenuStaveList);

  if (fileMenuId) YDestroyMenu(fileMenuId);
  if (fileMenu) XtFree((void *)fileMenu);

  fileMenu = (YMenuElement *)XtMalloc
    (((orig = XtNumber(baseFileMenu)) + len + 1) * sizeof(YMenuElement));

  for (i = 0; i < orig; ++i) {
    fileMenu[i]        = baseFileMenu[i];
    fileMenu[i].widget = NULL;
  }

  if (len > 0) {

    fileMenu[i].label                 = NULL;
    fileMenu[i].insensitive_mode_mask = SlaveToSequencerMode;
    fileMenu[i].callback              = NULL;
    fileMenu[i].widget                = NULL;

    orig ++;
  }

  for (i = 0, list = (FileMenuStaveList)First(fileMenuStaveList); i < len;
       ++i, list = (FileMenuStaveList)Next(list)) {

    fileMenu[orig + i].label                 = list->name;
    fileMenu[orig + i].insensitive_mode_mask = SlaveToSequencerMode;
    fileMenu[orig + i].callback              = FileChangeStave;
    fileMenu[orig + i].toolbar_bitmap        = NULL;
    fileMenu[orig + i].widget                = NULL;

    if (changeTo == list->name) j = orig + i;
  }

  fileMenuCount = orig + len;
  fileMenuId =
    YCreateMenu(fileMenuButton, "File Menu", fileMenuCount, fileMenu);

  for (i = 0; i < len; ++i) {

    YSetValue(fileMenu[orig + i].widget, XtNleftMargin, buffer_width + 7);
    YSetValue(fileMenu[orig + i].widget, XtNleftBitmap, map);
  }

  if (j != -1)
    FileChangeStave(fileMenuButton, (XtPointer)&(fileMenu[j]), NULL);

  End;
}


void InstallFileMenu(Widget file)
{
  Begin("InstallFileMenu");

  fileMenuButton = file;
  fileMenuStaveList = NULL;

  MakeFileMenuFromBase(NULL);

  End;
}


String GetStaveName(MajorStave sp)
{
  FileMenuStaveList list;

  Begin("GetStaveName");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list)) if (list->sp == sp) break;

  if (!list) Return(NULL);

  Return(list->name);
}


String GetStaveFileName(MajorStave sp)
{
  FileMenuStaveList list;

  Begin("GetStaveName");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list)) if (list->sp == sp) break;

  if (!list) Return(NULL);

  Return(list->filename);
}


void PutStaveFileName(MajorStave sp, String fname)
{
  FileMenuStaveList list;

  Begin("GetStaveName");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list)) if (list->sp == sp) break;

  if (list) {

    if (list->filename) XtFree(list->filename);
    list->filename = XtNewString(fname);
  }

  End;
}


static void FileChangeToSomeStave(void)
{
  Begin("FileChangeToSomeStave");

  if (!fileMenuStaveList) {
    ChangeTitleBar("No file open", False);
    End;
  }

  if (stave) StaveUnmap(stave);

  stave = fileMenuStaveList->sp;
  StaveRefresh(stave, -1);

  LeaveMenuMode(AreaSweptMode);
  EnterMenuMode(NoAreaSweptMode);
  ChangeTitleBar(GetStaveName(stave), fileMenuStaveList->changed);
  UndoInstallLabelsForStave(stave);

  End;
}


void AddStaveToFileMenu(MajorStave sp, String name, String filename)
{
  int               i = 1;
  FileMenuStaveList list;
  String            newname;

  Begin("AddStaveToFileMenu");

  newname = (String)XtMalloc(strlen(name) + 8);
  strcpy(newname, name);

  for (list = (FileMenuStaveList)Last(fileMenuStaveList);
       list; list = (FileMenuStaveList)Prev(list))
    if (!strcmp(list->name, newname)) sprintf(newname, "%s <%d>", name, ++i);

  fileMenuStaveList = (FileMenuStaveList)
    First(Nconc(NewFileMenuStaveList(newname,
				     filename ? XtNewString(filename) : NULL,
				     sp),
		fileMenuStaveList));

  MakeFileMenuFromBase(newname);

  End;
}


static void RemoveStaveFromFileMenu(MajorStave sp)
{
  FileMenuStaveList list;
  
  Begin("RemoveStaveFromFileMenu");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list)) if (list->sp == sp) break;

  if (!list) End;

  XtFree(list->name);
  if (list->filename) XtFree(list->filename);
  fileMenuStaveList = (FileMenuStaveList)First(Remove(list));

  MakeFileMenuFromBase(NULL);

  End;
}

/* }}} */
/* {{{ Mark and Test for Changes */

void FileMenuMarkChanged(MajorStave sp, Boolean changed)
{
  FileMenuStaveList list;

  Begin("FileMenuMarkChanged");

  staveChanged = True;

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list)) {

    if (sp == list->sp) {

      if (list->changed != changed) {
	ChangeTitleBar(GetStaveName(list->sp), changed);
	list->changed = changed;
      }

      break;
    }
  }

  End;
}


Boolean FileMenuIsChanged(MajorStave sp)
{
  FileMenuStaveList list;

  Begin("FileMenuIsChanged");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list))
    if (sp == list->sp) Return(list->changed);

  Return(False);
}

/* }}} */
/* {{{ Save Changes */

static Boolean SaveChanges(MajorStave sp, Boolean cancellable)
{
  String msg;
  String fname;
  int    rtn;
  FILE  *file;

  Begin("SaveChanges");

  if (!sp || !FileMenuIsChanged(sp)) Return(False);

  msg = (String)XtMalloc(300);
  fname = GetStaveFileName(sp);

  if (fname) sprintf(msg, "Save changes to `%s' ?", fname);
  else sprintf(msg, "Save changes to buffer `%s' ?", GetStaveName(sp));

  if (cancellable)
    rtn = YQuery(topLevel, msg, 3, 1, 2, "Yes", "No", "Cancel", NULL);
  else rtn = YQuery(topLevel, msg, 2, 1, 1, "Yes", "No", NULL);
  XtFree(msg);

  if (rtn == 1) Return(False);
  if (rtn == 2) Return(True);

  if (!fname) {

    fname = YFileGetWriteFilename(topLevel, "Editor File - Save As",
				  ".rose", "Rosegarden");
    if (!fname) Return(cancellable);

    PutStaveFileName(sp, fname);
  }

  if (!(file = fopen(fname, "w"))) {

    msg = (String)XtMalloc(300);
    sprintf(msg, "I can't open `%s' for writing.", fname);

    if (cancellable)
      rtn = YQuery(topLevel, msg, 2, 1, 1, "Ignore", "Cancel", NULL);
    else rtn = YQuery(topLevel, msg, 1, 0, 0, "Continue", NULL);
    XtFree(msg);

    if (rtn == 0) Return(False);
    Return(True);
  }

  fprintf(file, FILE_HEADER);
  StaveWriteToFile(sp, file);
  fprintf(file, FILE_FOOTER);

  fclose(file);
  FileMenuMarkChanged(sp, False);

  Return(False);
}

/* }}} */

/* {{{ New File */

/* User-end functions. */


void FileMenuNew(Widget w, XtPointer a, XtPointer b)
{
  int        staves;
  int        i;
  String     str;
  ItemList  *music;
  String    *names;
  MajorStave sp;

  Begin("FileMenuNew");

  if ((str = YGetUserInput(topLevel, "Staffs:", "1", YOrientHorizontal,
			   "Editor File - New")) == NULL) End;

  staves = atoi(str);

  if (staves < 1) {
    IssueMenuComplaint("There must be at least one staff.");
    End;
  }

  /* arbitrary limit -- in fact there can be as many staffs   */
  /* as you like, it'll just get horrifically slow quite fast */

  if (staves > 24) {
    IssueMenuComplaint("I can only handle 24 staffs in a single piece.");
    End;
  }

  music = (ItemList *)XtMalloc(staves * sizeof(ItemList));
  names = (String   *)XtMalloc(staves * sizeof(String  ));

  for (i = 0; i < staves; ++i) {

    music[i] = NewItemList
      ((Item *)NewClef(NULL,
		       staves-i <= staves/3 ||
		       (i == staves-1 && staves > 1) ? BassClef :
		       staves-i >  staves*2/3 ? TrebleClef : AltoClef));

    names[i] = (String)XtMalloc(10);
    sprintf(names[i], "Staff %d", i+1);
  }

  sp = NewStave(staves, music);
  for (i = 0; i < staves; ++i) StaveRenameStave(sp, i, names[i]);

  AddStaveToFileMenu(sp, "Composition", NULL);
  FileMenuMarkChanged(sp, False);

  staveMoved = True;
  staveChanged = True;
  LeaveMenuMode(FileNotLoadedMode);
  EnterMenuMode(FileLoadedMode);
  UndoInstallLabelsForStave(stave);

  End;
}

/* }}} */
/* {{{ Change Stave */

void FileChangeStave(Widget w, XtPointer a, XtPointer b)
{
  String str = ((YMenuElement *)a)->label;
  FileMenuStaveList list;
  
  Begin("FileChangeStave");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list))
    if (list->name == str) break;

  if (!list) End;
  if (list->sp == stave) End;

  if (stave) StaveUnmap(stave);

  stave = list->sp;
  StaveRefresh(stave, -1);

  if (list->filename) LeaveMenuMode(NoFilenameToSaveInMode);
  else                EnterMenuMode(NoFilenameToSaveInMode);

  staveChanged = True;
  LeaveMenuMode(AreaSweptMode);
  EnterMenuMode(NoAreaSweptMode);
  ChangeTitleBar(GetStaveName(stave), list->changed);
  UndoInstallLabelsForStave(stave);

  End;
}


Boolean FileStaveExists(String name)
{
  int i;
  Begin("FileStaveExists");

  for (i = 0; i < fileMenuCount; ++i) {
    if (fileMenu[i].callback == FileChangeStave &&
	!strcmp(fileMenu[i].label, name)) Return(True);
  }

  Return(False);
}


void FileChangeToStave(String name)
{
  int i;
  Begin("FileChangeToStave");

  for (i = 0; i < fileMenuCount; ++i) {
    if (fileMenu[i].callback == FileChangeStave &&
	!strcmp(fileMenu[i].label, name)) {
      FileChangeStave(0, &fileMenu[i], 0);
      End;
    }
  }

  End;
}

/* }}} */
/* {{{ Save, Save As */

void FileMenuSave(Widget w, XtPointer a, XtPointer b)
{
  FILE             *file;
  FileMenuStaveList list;
  String            message;

  Begin("FileMenuSave");

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list))
    if (list->sp == stave) break;

  if (!list || !list->filename) {
    IssueMenuComplaint
      ("Sorry, I haven't got a filename.  Try saving with a new filename.");
    End;
  }

  message = (String)XtMalloc(strlen(list->filename) + 17);
  sprintf(message, "Save file `%s' ?", list->filename);
  if (YQuery(topLevel, message, 2,0,1, "Yes", "No", "Editor File - Save") ==1) {
    XtFree(message); End;
  } else XtFree(message);

  if (!(file = fopen(list->filename, "w"))) {
    IssueMenuComplaint("Sorry, I can't open the file for writing.");
    End;
  }

  fprintf(file, FILE_HEADER);
  StaveWriteToFile(stave, file);
  fprintf(file, FILE_FOOTER);
  fclose(file);

  message = (String)XtMalloc(strlen(list->filename) + 12);
  sprintf(message, "Saved `%s'.", list->filename);
  (void)YQuery(topLevel, message, 1, 0, 0, "OK", NULL);
  XtFree(message);

  FileMenuMarkChanged(stave, False);

  End;
}


void FileMenuSaveAs(Widget w, XtPointer a, XtPointer b)
{
  FILE      *file;
  String     fname;
  MajorStave sp;

  Begin("FileMenuSaveAs");

  if ((file = YFileGetWriteFile(XtParent(XtParent(w)),
				"Editor File - Save As", ".rose",
				"Rosegarden"))
      == NULL) End;

  fprintf(file, FILE_HEADER);
  StaveWriteToFile(stave, file);
  fprintf(file, FILE_FOOTER);

  fclose(file);
  fname = YFileGetLastFilename(False);

  /* Now reopen the file with the new name */

  if (!(file = fopen(fname, "r"))) {
    YQuery(XtParent(w),
	   "Cannot reopen newly-saved file.", 1, 0, 0, "Continue", NULL);
    End;
  }

  RemoveStaveFromFileMenu(stave);

  StaveUnmap(stave);
  StaveDestroy(stave, True);
  stave = NULL;

  if ((sp = LoadStaveFromFile(file)) == NULL) End;

  AddStaveToFileMenu(sp, YFileGetLastFilename(True) ?
		     YFileGetLastFilename(True) : "[buffer]",
		     YFileGetLastFilename(False));
  FileMenuMarkChanged(sp, False);

  staveMoved = True;
  StaveReformatEverything(sp);

  End;
}

/* }}} */

/* {{{ Load */

void FileMenuLoad(Widget w, XtPointer a, XtPointer b)
{
  FILE *file;
  MajorStave sp;
  Begin("FileMenuLoad");

  if ((file = YFileGetReadFile(XtParent(XtParent(w)),
			       "Editor File - Open", ".rose",
			       "Rosegarden")) == NULL)
    End;

  if ((sp = LoadStaveFromFile(file)) == NULL) End;

  AddStaveToFileMenu(sp, YFileGetLastFilename(True) ?
		     YFileGetLastFilename(True) : "[buffer]",
		     YFileGetLastFilename(False));
  FileMenuMarkChanged(sp, False);

  LeaveMenuMode(FileNotLoadedMode);
  EnterMenuMode(FileLoadedMode);

  staveMoved = True;
  StaveReformatEverything(sp);
  UndoInstallLabelsForStave(stave);

  End;
}

/* }}} */
/* {{{ Close */

void FileMenuClose(Widget w, XtPointer a, XtPointer b)
{
  Begin("FileMenuClose");

  if (!stave) End;
  
  if (SaveChanges(stave, True)) End;
  RemoveStaveFromFileMenu(stave);

  StaveUnmap(stave);
  StaveDestroy(stave, True);
  stave = NULL;

  FileChangeToSomeStave();

  if (Length(fileMenuStaveList) == 0) {

    LeaveMenuMode(FileLoadedMode);
    EnterMenuMode(FileNotLoadedMode);

  } else {

    LeaveMenuMode(FileNotLoadedMode);
    EnterMenuMode(FileLoadedMode);
  }

  End;
}


void FileCloseStave(String name)
{
  int i;
  Begin("FileCloseStave");

  for (i = 0; i < fileMenuCount; ++i) {
    if (fileMenu[i].callback == FileChangeStave &&
	!strcmp(fileMenu[i].label, name)) FileMenuClose(0, &fileMenu[i], 0);
  }

  End;
}

/* }}} */

/* {{{ Clean up */

/* Like all clean-up functions, this can be called */
/* before any menu has been installed   <- poetry  */

void FileMenuCleanUp(void)
{
  FileMenuStaveList list;

  Begin("FileMenuCleanUp");

  (void)SaveChanges(stave, False);
  if (stave) {
    StaveUnmap(stave);
    StaveCursorCleanUp(stave);
  }

  for (list = (FileMenuStaveList)First(fileMenuStaveList);
       list; list = (FileMenuStaveList)Next(list)) {

    if (list->sp != stave) (void)SaveChanges(list->sp, False);

    StaveDestroy(list->sp, True);
    XtFree(list->name);
  }

  stave = NULL;

  if (fileMenuStaveList) DestroyList(fileMenuStaveList);
  if (fileMenuId) YDestroyMenu(fileMenuId);
  if (fileMenu) XtFree((void *)fileMenu);

  End;
}

/* }}} */

