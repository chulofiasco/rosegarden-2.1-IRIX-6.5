
/* {{{ Includes */

#include "General.h"
#include "Widgets.h"
#include "Menu.h"
#include "StavePrivate.h"
#include "ItemList.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

/* }}} */
/* {{{ Menu declarations */

static void ApplyFilterFromFile(String);
void FilterMenuApplySelectedFilter(Widget, XtPointer, XtPointer);

static YMenuElement baseFilterMenu[] = {
  { "Apply Petal Filter . . .",   CursorNotPlacedMode | SlaveToSequencerMode,     FilterMenuApplyFilter,  NULL, NULL, },
  /*  { "Rescan Filters . . .",   CursorNotPlacedMode | SlaveToSequencerMode,     FilterMenuRescan,  NULL, NULL, },*/
  YMenuDivider,
  { "Dump this Staff to Petal",   CursorNotPlacedMode | SlaveToSequencerMode,     FilterMenuDumpStaff,  NULL, NULL, },
  { "Dump all Staffs to Petal",   FileNotLoadedMode | SlaveToSequencerMode,     FilterMenuDumpAllStaves,  NULL, NULL, },
  { "Read new Staff from Petal",   FileNotLoadedMode | SlaveToSequencerMode,     FilterMenuReadStaff,  NULL, NULL, },
};

typedef struct _FilterAssoc {
  String name;
  String file;
} FilterAssoc;

static FilterAssoc  *filters = NULL;
static int           filterCount = 0;

static YMenuElement *filterMenu = NULL;
static int           filterMenuCount = 0;
YMenuId              filterMenuId = NULL;

/* }}} */

/* {{{ Install and clean up */

Boolean ScanDirForPetalLib(String dirname)
{
  Boolean res = False;
  struct stat buf;
  struct dirent *direntry;
  DIR *dir;

  Begin("ScanDir");

  if(!stat(dirname, &buf) && S_ISDIR(buf.st_mode)) {
    dir = opendir(dirname);

    while(direntry = readdir(dir))
      if(!strcmp(direntry->d_name, "Petal.so")) {
       res = True;
       break;
      }

    closedir(dir);
  }

  Return(res);
}

Boolean PetalIsPresent(void)
{
  String tmp, tcllibpath, dirname;
  Boolean res = False;

  Begin("PetalIsPresent");

  tcllibpath = getenv("TCLLIBPATH");

  if(!tcllibpath) Return(False);

  tmp = XtNewString(tcllibpath);

  dirname = strtok(tmp, " ");

  if(dirname)
    if(ScanDirForPetalLib(dirname))
      res = True;
    else
      while(dirname = strtok(NULL, " "))
       if(ScanDirForPetalLib(dirname)) res = True;

  XtFree(tmp);
  Return(res);
}

static void GetFilters(void);

void InstallFilterMenu(Widget w)
{
  int i, j;
  Begin("InstallFilterMenu");

  if(PetalIsPresent() == False) {
    char *t;
    if(t = getenv("TCLLIBPATH"))
      fprintf(stderr, "Warning : Petal library could not be found in TCLLIBPATH (%s)\n", t);
    else
      fprintf(stderr, "Warning : Petal library could not be found, TCLLIBPATH is not set\n");

    fprintf(stderr,"Not installing Filter Menu\n");
    filterMenuCount = 0;

  } else {

    GetFilters();

    filterMenuCount = filterCount + XtNumber(baseFilterMenu);
    if (filterCount > 0) ++filterMenuCount;

    filterMenu =
      (YMenuElement *)XtCalloc(filterMenuCount, sizeof(YMenuElement));

    for (i = 0; i < filterCount; ++i) {

      filterMenu[i].label = XtNewString(filters[i].name);
      filterMenu[i].insensitive_mode_mask =
       CursorNotPlacedMode | SlaveToSequencerMode;
      filterMenu[i].callback = FilterMenuApplySelectedFilter;
    }

    if (i > 0) {
      filterMenu[i].label = NULL;
      ++i;
    }

    for (j = 0; i < filterMenuCount; ++i, ++j) {
      filterMenu[i] = baseFilterMenu[j];
    }

  }

  filterMenuId =
    YCreateMenu(w, "Filter Menu", filterMenuCount, filterMenu);

  if(filterMenuCount == 0) XtSetSensitive(w, False);

  End;
}


void FilterMenuCleanUp()
{
  Begin("FilterMenuCleanUp");
  End;
}

/* }}} */
/* {{{ Collating filter names & locations */

static void GetFiltersFromDirectory(String dirName)
{
  int i;
  char c;
  DIR *dir;
  FILE *file;
  struct dirent *entry;
  struct stat st;
  String fileName;
  char buffer[80];
  char filterName[80];

  Begin("GetFiltersFromDirectory");

  dir = opendir(dirName);
  if (!dir) {
    fprintf(stderr,"Rosegarden: no filter directory `%s'\n",dirName);
    End;
  }

  fileName = (String)XtMalloc(strlen(dirName) + 1000);

  while ((entry = readdir(dir)) != NULL) {

    sprintf(fileName, "%s/%s", dirName, entry->d_name);
    if (stat(fileName, &st) == -1) continue;
    if (!S_ISREG(st.st_mode) || !(st.st_mode & 0111)) continue;

    if (strlen(fileName) < 5 ||
	strcmp(fileName + strlen(fileName) - 4, ".tcl")) continue;

    file = fopen(fileName, "r");
    if (!fgets(buffer, 79, file)) continue;
    /*    if (!fgets(buffer, 79, file)) continue;*/
    
    if (fscanf(file, "# FilterName%c", &c) != 1 || c != ':') continue;
    fscanf(file, " ");
    if (!fgets(filterName, 79, file)) continue;
    if (!filterName || !filterName[0]) continue;

    if (!filters) filters = (FilterAssoc *)XtMalloc(sizeof(FilterAssoc));
    else filters = (FilterAssoc *)XtRealloc
	   ((void *)filters, (filterCount+1) * sizeof(FilterAssoc));

    if (filterName[strlen(filterName)-1] == '\n') {
      filterName[strlen(filterName)-1] = '\0';
    }

    printf(" ... found \"%s\" in %s\n", filterName, fileName);

    /* check we haven't already got this one */
    for (i = 0; i < filterCount; ++i) {
      if (!strcmp(filterName, filters[i].name)) break;
    }
    if (i < filterCount) {
      printf("     ... (ignoring duplicate)\n");
      continue;
    }

    filters[filterCount].name = XtNewString(filterName);
    filters[filterCount].file = XtNewString(fileName);

    ++filterCount;
  }

  End;
}


static int FilterAssocComparator(const void *a, const void *b)
{
  return strcasecmp(((FilterAssoc *)a)->name, ((FilterAssoc *)b)->name);
}


static String homeFilterDir = NULL;

static void GetFilters(void)
{
  String homeDir, sysFilterDir;
  Begin("GetFilters");

  homeDir = getenv("HOME");
  if (!homeDir) {
    fprintf(stderr, "Warning: couldn't get value of $HOME\n");
    homeDir = ".";
  }

  homeFilterDir = (String)XtMalloc(strlen(homeDir) + 23);
  sprintf(homeFilterDir, "%s/.rosepetal-filters", homeDir);

  printf("Rosegarden: Looking up Petal filters for Editor...\n");

  GetFiltersFromDirectory(homeFilterDir);
  GetFiltersFromDirectory(appData.filtersDirectory);

  if (filterCount > 0) {
    qsort((void *)filters, filterCount, sizeof(FilterAssoc),
	  FilterAssocComparator);
  } else {
    printf(" ... none found\n");
  }

  End;
}

/* }}} */

/* {{{ Dump Staffs to Petal */

void DumpStaff(int staveNo, FILE *fp)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       start   = mstave->sweep.from.left;
  ItemList       list;
  Chord         *tmpChord;
  ClefTag        currentClef;
  KeyTag         currentKey;
  unsigned int i, c, t, time = 0;
  static char buf[256];

  currentKey = StaveItemToKey(stave, staveNo, start)->key.key;
  currentClef = StaveItemToClef(stave, staveNo, start)->clef.clef;

  for (list = (ItemList)First(mstave->music[staveNo]); 
       list != NULL; list = iNext(list)) {

    switch(list->item->object_class) {
    case ClefClass:
      currentClef = ((Clef*)list->item)->clef.clef; 
      break;
    case KeyClass:
      t = ((Key*)list->item)->key.key;
      fprintf(fp, "set staff%u(%u.9) %u ; # Key\n", staveNo, time - 1, t);
      break;
    case RestClass:
      t = ((Rest*)list->item)->rest.length;
      fprintf(fp, "set staff%u(%u) { %u }; # Rest\n", staveNo, time, t);
      time += t;
      break;
    case ChordClass:
      tmpChord = (Chord*)list->item;
      t = tmpChord->chord.length;
      for(c=i=0; i < tmpChord->chord.voice_count; i++)
	c += sprintf(buf+c,"%u ", VoiceToMidiPitch(&(tmpChord->chord.voices[i]),
						   currentClef));
      fprintf(fp, "set staff%u(%u) { %u %s} ; # Chord\n",
	      staveNo, time, t, buf);
      time += t;
      break;
    case BarClass: /* We'll never see this one */
      printf("Bar : %lu\n", ((Bar*)list->item)->bar.start_time);
      break;
    default: /* To shut -Wall up */
      break;
    }
  }
  End;
}

void FilterMenuDumpStaff(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            staveNo = mstave->sweep.stave;
  FILE *fp;

  Begin("FilterMenuDumpStaff");
  fp = fopen("/tmp/rosebowl.tcl", "w");
  DumpStaff(staveNo, fp);
  fclose(fp);

  End;
}


void FilterMenuDumpAllStaves(Widget w, XtPointer a, XtPointer b)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int            staveNo;
  FILE *fp;

  Begin("FilterMenuDumpAllStaves");
  fp = fopen("/tmp/rosebowl.tcl", "w");
  for(staveNo=0; staveNo < mstave->staves; staveNo++)
    DumpStaff(staveNo, fp);

  fprintf(fp, "\nset nbOfStaves %u\n", staveNo);
  fclose(fp);

  End;
}

/* }}} */
/* {{{ Read Staffs from Petal */

void ReadStaff(FILE *source)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  ItemList       list, newItemList;
  Chord         *tmpChord;
  NoteVoice   *voices;
  Clef        *tmpClef;
  Key         *tmpKey;
  Rest        *tmpRest;
  NoteTag     length;
  unsigned int i, itemCounter = 0, newStaveNb;
  int paramStack[20], *paramStackPtr, param, c;
#define NB_PITCHES (paramStackPtr - paramStack - 1) /* first param is length */
  Boolean reprocess, sharps=True, dotted;
  static char buf[256], itemType;
  enum { reading_item,
	 reading_clef_param,
	 reading_key_param,
	 reading_rest_param,
	 reading_note_param } state = reading_item;

  Begin("ReadStaff");

  StaveAddANewStave(stave, mstave->staves);

  newStaveNb = mstave->staves - 1;
  list = (ItemList)Last(mstave->music[newStaveNb]);

  while(!feof(source)) {
    c = fscanf(source, "%250s", buf);
    if(c<=0) { /* May be EOF, but we may have a note item left */
      if(state != reading_note_param) continue;
      else buf[0] = '\0';
    }

    do {
      reprocess = False;

      if(state == reading_clef_param ||
	 state == reading_key_param ||
	 state == reading_rest_param) {

	if(!sscanf(buf, "%d", &param)) {
	  printf("Syntax error : %s, expecting an int\n", buf);
	  exit(1);
	}

	switch(state) {
	case reading_clef_param:
	  tmpClef = NewClef(NULL, param);
	  newItemList = NewItemList((Item*)tmpClef);
	  break;
	case reading_key_param:
	  tmpKey = NewKey(NULL, param);
	  newItemList = NewItemList((Item*)tmpKey);
	  break;
	case reading_rest_param:
	  length = MTimeToTag(param, &dotted);
	  /* This is pretty stupid : NewRest calls TagToMTime()... */

	  tmpRest = NewRest(NULL, length, dotted);
	  newItemList = NewItemList((Item*)tmpRest);
	  break;
	default:/* We can't get here, this is just to shut -Wall up */
	  IssueMenuComplaint("You've just entered the Twilight Zone. Sorry");
	  
	}
	list = (ItemList)Nconc(list, newItemList);
	state = reading_item;

      } else {

	switch(state) {

	case reading_item:
	  itemType = buf[0]; itemCounter++;

	  switch(itemType) {
	  case 'C':
	    state = reading_clef_param; break;
	  case 'K':
	    state = reading_key_param; break;
	  case 'N':
	    state = reading_note_param;
	    paramStackPtr = paramStack;
	    break;
	  case 'R':
	    state = reading_rest_param; break;
	  default:
	    printf("Syntax error : %s, expecting an item\n", buf);
	  }
	  break;

	case reading_note_param:
	  c = sscanf(buf, "%d", &param);
	  if(c && c != EOF) {
	    *paramStackPtr = param;
	    paramStackPtr++;
	    if(NB_PITCHES > 18) {
	      sprintf(buf, "Too many notes for item %u", itemCounter);
	      IssueMenuComplaint(buf);
	      End;
	    }
	  } else {
	    length = MTimeToTag(paramStack[0], &dotted);
	    /* This is pretty stupid too, see above. */

	    state = reading_item;

#ifdef DEBUG
	    printf("Found Notes : ");
	    for(i = 1; i <= NB_PITCHES; i++)
	      /* First param is chord len */
	      printf("%u ", paramStack[i]);
	    putchar('\n');
#endif
	    if(c != EOF) reprocess = True;

	    voices = (NoteVoice*)XtMalloc((NB_PITCHES + 1) * sizeof(NoteVoice));
	    for(i = 0; i <= NB_PITCHES; i++)
	      voices[i] = MidiPitchToVoice(paramStack[i+1], sharps);
	    tmpChord = NewChord(NULL, voices, NB_PITCHES,
				ModNone, length, dotted);

	    paramStackPtr = paramStack; 
	    /* Must be reset AFTER we're through using NB_PITCHES */
	    newItemList = NewItemList((Item*)tmpChord);
	    Nconc(list, newItemList);

	  }
	  break;

	default:/* We can't get here either, see above */
	  IssueMenuComplaint("Your machine is defying basic laws of the Universe. Please Reboot");

	}
      }
    } while (reprocess == True);

  }

  /*
  UndoAssertNewContents("Read New Staff", stave, newStaveNb,
			start, iNext(start)); */

  FileMenuMarkChanged(stave, True);
  StaveResetFormatting(stave, newStaveNb);
  StaveRefreshAsDisplayed(stave);

  End;
#undef NB_PITCHES
}

void FilterMenuReadStaff(Widget w, XtPointer a, XtPointer b)
{
  FILE *fp;
  Begin("FilterMenuReadStaff");
  if(!(fp = fopen("/tmp/rose.petal", "r"))) {
    IssueMenuComplaint("Couldn't open /tmp/rose.petal");
    End;
  }

  ReadStaff(fp);
  fclose(fp);
  End;
}

/* }}} */
/* {{{ Apply Filter */

void FilterMenuApplySelectedFilter(Widget w, XtPointer a, XtPointer b)
{
  int i;
  String fileName;
  YMenuElement *element = (YMenuElement *)a;
  Begin("FilterMenuApplySelectedFilter");

  for (i = 0; i < filterCount; ++i) {
    if (!strcmp(filters[i].name, element->label)) {
      ApplyFilterFromFile(filters[i].file);
      break;
    }
  }

  End;
}

void SigHandler(int sig)
{
  static Boolean hadPipe = False;

  if (sig == -1) {

    hadPipe = False;

  } else if (sig == SIGPIPE) {

    if (!hadPipe) {
      fprintf(stderr, "Rosegarden: broken pipe.  Cannot apply filter.\n");
      hadPipe = True;
    }

  } else if (sig == SIGCHLD) {

    int status;

    wait(&status);

    if (WIFEXITED(status)) {
      /* Problem here : staff is probably already read, bad bad... */
      if (WEXITSTATUS(status) != EXIT_SUCCESS)
	IssueMenuComplaint("Filter exited with a failure code!");
    } else {
      IssueMenuComplaint("The filter terminated abnormally");
    }
  }
}


static void ApplyFilterFromFile(String filterName)
{
  MajorStaveRec *mstave  = (MajorStaveRec *)stave;
  int staveNo, refStaff;
  int i;
  pid_t pid;
  int filterData[2], filterResult[2];
  char buf[256], *exeArgv[6];
  char convenienceArgv[4][20]; /* for argv[2..5] :
				  main staff nb, duplicate, ref staff nb */
  FILE *filterInput, *filterOutput;
  Begin("ApplyFilterFromFile");

  if (!mstave || !mstave->sweep.swept) {
    XBell(display, 70); End;
  }
  staveNo = mstave->sweep.stave;
  /*
  if (!getenv("TCLLIBPATH") && homeFilterDir) {
    char path[3000];
    fprintf(stderr, "Rosegarden: Environment variable TCLLIBPATH is not set; "
	    "guessing!\n");
    sprintf(path, "TCLLIBPATH=%s %s /usr/local/lib/rosegarden/petal",
	    homeFilterDir, appData.filtersDirectory);
    putenv(path);
  }
  */
  if (pipe(filterData) || pipe(filterResult)) {
    IssueMenuComplaint("pipe failed");
    End;
  }

  SigHandler(-1);		/* reset */
  signal(SIGCHLD, SigHandler);
  signal(SIGPIPE, SigHandler);

  if ((pid = fork()) == -1) {

    IssueMenuComplaint("Couldn't fork!");
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    End;

  } else if (pid) { /* Parent */

    StaveBusy(True);

    close(filterData[0]); close(filterResult[1]);
    filterInput = fdopen(filterData[1], "w");
    filterOutput = fdopen(filterResult[0], "r");

    for(i=0; i < mstave->staves; i++)
      DumpStaff(i, filterInput);
    fprintf(filterInput, "\nset nbOfStaves %u\n", mstave->staves);

    fclose(filterInput);

    ReadStaff(filterOutput);
    fclose(filterOutput);

    StaveBusy(False);

    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);

  } else { /* Child */

    close(filterData[1]); close(filterResult[0]);

    dup2(filterData[0], STDIN_FILENO);
    dup2(filterResult[1], STDOUT_FILENO);

    exeArgv[0] = filterName; exeArgv[1] = "EDITOR";
    for (i = 2; i < 5; i++) exeArgv[i] = convenienceArgv[i-1];
    exeArgv[5] = NULL;
    /* Don't want to malloc for such tiny strings */

    sprintf(exeArgv[2], "%u", staveNo); /* main staff nb */
    sprintf(exeArgv[3], "%u", 1); /* duplicate staff data or not */

    /* ref. staff nb */
    refStaff = staveNo + 1;
    if (refStaff >= mstave->staves) refStaff = 0;
    sprintf(exeArgv[4], "%u", refStaff);

    if (execvp(filterName, exeArgv)) exit(EXIT_FAILURE);
  }

  End;
}


void FilterMenuApplyFilter(Widget w, XtPointer a, XtPointer b)
{
  String filterName;
  Begin("FilterMenuApplyFilter");

  if ((filterName = YFileGetReadFilename
       (XtParent(XtParent(w)),
	"Editor Filter - Apply Petal Filter", ".tcl", "Petal Filters")) == NULL)
    End;

  ApplyFilterFromFile(filterName);

  End;
}

/* }}} */

