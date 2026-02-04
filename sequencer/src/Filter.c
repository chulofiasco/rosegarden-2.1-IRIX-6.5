
#include "Globals.h"
#include "Menu.h"
#include <Debug.h>

#include "MidiErrorHandler.h"

#include <Yawn.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

static void ApplyFilterFromFile(String);
void FilterMenuApplySelectedFilter(Widget, XtPointer, XtPointer);
void Midi_ApplyFilterCB(Widget, XtPointer, XtPointer);

static YMenuElement baseFilterMenu[] = {
  { "Apply Petal Filter . . .", NoFileLoadedMode | PlaybackMode, Midi_ApplyFilterCB, NULL, },
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

/* Loads of duplication with the Editor. Yum */

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
      filterMenu[i].insensitive_mode_mask = NoFileLoadedMode | PlaybackMode;
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

  printf("Rosegarden: Looking up Petal filters for Sequencer...\n");

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

    if(WIFEXITED(status)) {

      if (WEXITSTATUS(status) != EXIT_SUCCESS) {
	Error(NON_FATAL_REPORT_TO_MSGBOX, "Filter exited with a failure code!");
      }
    } else {
      Error(NON_FATAL_REPORT_TO_MSGBOX, "The filter exited abnormally");
    }
  }
}


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


void Midi_ApplyFilterCB(Widget w, XtPointer a, XtPointer b)
{
  String filterName;
  Begin("Midi_ApplyFilterCB");

  if ((filterName = YFileGetReadFilename
       (XtParent(XtParent(w)), "Sequencer Filter - Apply Petal Filter",
	".tcl", "Petal Filters")) == NULL) End;

  ApplyFilterFromFile(filterName);
  End;
}
  

void ApplyFilterFromFile(String filterName)
{
  pid_t pid;
  int filterData[2], filterResult[2], i;
  int useTrack, refTrack;
  FILE *filterDataFp, *filterResultFp;
  char buf[256], *exeArgv[6];
  char convenienceArgv[4][20]; /* for argv[2..5] :
				  main track nb, duplicate, ref track nb */
  String newFileName;
  MIDIFileHandle origFile, filteredFile;
  MIDIHeaderChunk OrigHeaderBuffer, FilteredHeaderBuffer;
  EventList ExpandedTrack;

  BEGIN("Midi_ApplyFilterCB");

  if (MIDISelectedTrack >= 0) useTrack = MIDISelectedTrack;
  else {
    String value = YGetUserInput
      (topLevel, "Main track for filter:", "0",
       YOrientHorizontal, "Sequencer Filter - Apply Petal Filter");
    if (!value) END;
    useTrack = atoi(value);
  }
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
    Error(NON_FATAL_REPORT_TO_MSGBOX, "pipe failed");
    END;
  }

  SigHandler(-1);		/* reset */
  signal(SIGCHLD, SigHandler);
  signal(SIGPIPE, SigHandler);

  if ((pid = fork()) == -1) { /* failure */

    Error(NON_FATAL_REPORT_TO_MSGBOX, "Couldn't fork!");
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    END;

  } else if(pid) { /* Parent */

    close(filterData[0]); close(filterResult[1]);

    filterDataFp = fdopen(filterData[1], "wb");
    filterResultFp = fdopen(filterResult[0], "rb");

    /* Dump midi file here */
    Midi_SaveFileToFP(filterDataFp);
    if (Midi_CloseFile()) Midi_LoadFileFromFP(filterResultFp, True);
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);

  } else { /* Child */

    close(filterData[1]); close(filterResult[0]);

    dup2(filterData[0], STDIN_FILENO);
    dup2(filterResult[1], STDOUT_FILENO);

    exeArgv[0] = filterName; exeArgv[1] = "MIDI";
    for (i=2; i < 5; i++) exeArgv[i] = convenienceArgv[i-1];
    exeArgv[5] = NULL;

    /* Don't want to malloc for such tiny strings */
    sprintf(exeArgv[2], "%u", useTrack); /* main track nb */
    sprintf(exeArgv[3], "%u", 1); /* duplicate or not */

    /* ref. track nb */
    refTrack = useTrack + 1;
    if (refTrack >= MIDIHeaderBuffer.NumTracks) 
      if (MIDIHeaderBuffer.NumTracks > 1) refTrack = 1;
      else refTrack = 0;

    sprintf(exeArgv[4], "%u", refTrack);

    if (execvp(filterName, exeArgv)) exit(EXIT_FAILURE);
  }

  END;
}

