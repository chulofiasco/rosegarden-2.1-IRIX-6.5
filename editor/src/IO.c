
/* IO.c */

/*
   Musical Notation Editor, Chris Cannam 1994
   File I/O Functions and Methods
*/


/*
   We only need methods for reading in and writing out
   classes which there can actually be instances of --
   Metronome, Clef, Key, TimeSignature, Text, Rest,
   Group and Chord.  We don't need to be able to write
   out the Bar as it's a transient class that can be
   deduced from the others (using the Stave's format).
*/

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif

/* {{{ Includes */

#include <stdio.h>
#include <ctype.h>
#include <stddef.h>

#include "General.h"
#include "Tags.h"
#include "Classes.h"
#include "Notes.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "Widgets.h"
#include "Menu.h"
#include "Format.h"
#include "ItemList.h"
#include "IO.h"
#include "Marks.h"

#include "Yawn.h"

/* }}} */
/* {{{ Macros and Static Variable declarations */

#define TAG_LEN    50
#define STRING_LEN 500

#define Write(x) do{for(dc=0;dc<depth;++dc)fputc('\t',file);fprintf x;}while(0)

static int            dc;

static int            rInt;
static unsigned int   rUInt;
static unsigned long  rULong;
static short          rShort;
static char           rTag[TAG_LEN];
static char           rString[STRING_LEN];

String                musicIOErrorMessage = NULL;

static Boolean        readingGroup = False;
static int            potentialDynamics = 0;
static Boolean        format20 = False;

/* }}} */
/* {{{ Prototypes */

static Boolean        ReadWS(FILE *);
static Boolean        ReadInt(FILE *);
static Boolean        ReadUnsignedInt(FILE *);
static Boolean        ReadHexUnsignedInt(FILE *);
static Boolean        ReadHexUnsignedLong(FILE *);
static Boolean        ReadShort(FILE *);
static Boolean        ReadTag(FILE *);
static Boolean        ReadConstTag(String, FILE *);
static Boolean        ReadString(FILE *);

static Boolean        ReadMark(ItemList, FILE *);
static Boolean        ReadFixedBar(String, ItemList, FILE *);

static Boolean        BuildFileError(FILE *, String);
static Boolean        BuildFileError2s(FILE *, String, String);
static Boolean        BuildFileError3s(FILE *, String, String, String);

static Boolean        ReadItemList(ItemList *, FILE *);

/* }}} */
/* {{{ Read Function Records */

static struct {
  String     tag;
  size_t     size;
  Boolean (* read)(MusicObject, FILE *);
} itemTags[] = {
  { ":",             sizeof(Chord),         ReadChord,         },
  { "Group",         sizeof(Group),         ReadGroup,         },
  { "Rest",          sizeof(Rest),          ReadRest,          },
  { "Text",          sizeof(Text),          ReadText,          },
  { "Clef",          sizeof(Clef),          ReadClef,          },
  { "Key",           sizeof(Key),           ReadKey,           },
  { "Metronome",     sizeof(Metronome),     ReadMetronome,     },
  { "Chord",         sizeof(Chord),         ReadChord,         },
};

/* }}} */

/* {{{ Nothing */

void WriteNothing(MusicObject obj, FILE *file, int depth)
{
  Begin("WriteNothing");
  End;
}

/* }}} */
/* {{{ NoteVoice */

void WriteNoteVoice(NoteVoice *a, FILE *file, int depth)
{
  Begin("WriteNoteVoice");

  fprintf(file, " %hd %x", a->pitch, (int)(a->modifiers));

  End;
}


Boolean ReadNoteVoice(NoteVoice *a, FILE *file)
{
  Begin("ReadNoteVoice");

  if (!ReadShort(file))
    Return(BuildFileError(file, "First argument of Voice lost or corrupted"));

  a->pitch = rShort;

  if (!ReadHexUnsignedInt(file))
    Return(BuildFileError(file, "Second argument of Voice lost or corrupted"));

  a->modifiers = (NoteMods)(rUInt);

  Return(True);
}

/* }}} */
/* {{{ Metronome */

void WriteMetronome(MusicObject obj, FILE *file, int depth)
{
  MetronomePart m = ((Metronome *)obj)->metronome;

  Begin("WriteMetronome");

  Write((file, "Metronome\n"));
  Write((file, "\t%s %u\n", m.beat.chord.visual->name, m.setting));

  End;
}


Boolean ReadMetronome(MusicObject obj, FILE *file)
{
  int     i;
  Boolean dotted;
  NoteTag tag;

  Begin("ReadMetronome");

  if (!ReadTag(file))
    Return(BuildFileError(file,
			  "Couldn't read Metronome Type or Dotted segment"));

  if (!strcasecmp(rTag, "Dotted")) {

    dotted = True;
    if (!ReadTag(file))
      Return(BuildFileError(file, "Couldn't read Metronome Type segment"));

  } else dotted = False;

  for (i = 0; i < noteVisualCount; ++i)
    if (!strncasecmp(noteVisuals[i].undotted.name, rTag, strlen(rTag))) {
      tag = noteVisuals[i].undotted.type;
      break;
    }

  if (!ReadUnsignedInt(file))
    Return(BuildFileError(file, "Couldn't read Metronome Setting segment"));

  (void)NewMetronome((Metronome *)obj, tag, dotted, rUInt);

  Return(True);
}

/* }}} */
/* {{{ Clef */

void WriteClef(MusicObject obj, FILE *file, int depth)
{
  ClefPart c = ((Clef *)obj)->clef;

  Begin("WriteClef");

  Write((file, "Clef\n"));
  Write((file, "\tClef Name %s\n", c.visual->name));

  End;
}


Boolean ReadClef(MusicObject obj, FILE *file)
{
  int i;

  Begin("ReadClef");

  if (!ReadConstTag("Clef", file) || !ReadConstTag("Name", file))
    Return(BuildFileError(file, "Expected Clef Name segment not found"));

  if (!ReadTag(file))
    Return(BuildFileError(file, "Couldn't read Clef Name segment"));

  for (i = 0; i < clefVisualCount; ++i)

    if (!strncasecmp(rTag, clefVisuals[i].name, strlen(rTag))) {

      (void)NewClef((Clef *)obj, clefVisuals[i].type);
      break;
    }

  if (i >= clefVisualCount)
    Return(BuildFileError2s(file, "Unknown Clef Name `%s'", rTag));

  Return(True);
}

/* }}} */
/* {{{ Key */

void WriteKey(MusicObject obj, FILE *file, int depth)
{
  KeyPart k = ((Key *)obj)->key;

  Begin("WriteKey");

  Write((file, "Key\n"));
  Write((file, "\tKey Name %s\n", k.visual->name));

  End;
}


Boolean ReadKey(MusicObject obj, FILE *file)
{
  int i;

  Begin("ReadKey");

  if (!ReadConstTag("Key", file) || !ReadConstTag("Name", file))
    Return(BuildFileError(file, "Expected Key Name segment not found"));

  if (!ReadString(file))
    Return(BuildFileError(file, "Couldn't read Key Name segment"));

  for (i = 0; i < keyVisualCount; ++i)
    if (!strcasecmp(rString, keyVisuals[i].name)) {
      (void)NewKey((Key *)obj, keyVisuals[i].key);
      break;
    }

  if (i >= keyVisualCount)
    Return(BuildFileError2s(file, "Unknown Key Name `%s'", rString));

  Return(True);
}

/* }}} */
/* {{{ Text */

void WriteText(MusicObject obj, FILE *file, int depth)
{
  TextPart t = ((Text *)obj)->text;

  Begin("WriteText");

  Write((file, "Text\n"));
  Write((file, "\tString %s\n", t.text));
  Write((file, "\tPosition %d\n", (int)(t.position)));

  End;
}


Boolean ReadText(MusicObject obj, FILE *file)
{
  int i;
  Text *text = (Text *)obj;
  Begin("ReadText");

  if (!ReadConstTag("String", file))
    Return(BuildFileError(file, "Expected Text String segment not found"));

  if (!ReadString(file))
    Return(BuildFileError(file, "Couldn't read Text String segment"));

  if (!ReadConstTag("Position", file))
    Return(BuildFileError(file, "Expected Text Position segment not found"));

  if (!ReadInt(file))
    Return(BuildFileError(file, "Couldn't read Text Position segment"));

  (void)NewText(text, rString, (TextPosnTag)rInt);

  if (format20 &&
      (text->text.position == TextBelowStave ||
       text->text.position == TextBelowStaveItalic)) {

    for (i = 0; i < textDynamicCount; ++i)
      if (!strcmp(text->text.text, textDynamics[i])) break;
    if (i < textDynamicCount - 1) ++potentialDynamics;
  }

  Return(True);
}

/* }}} */
/* {{{ Rest */

void WriteRest(MusicObject obj, FILE *file, int depth)
{
  RestPart r = ((Rest *)obj)->rest;

  Begin("WriteRest");

  Write((file, "Rest\n"));
  Write((file, "\t%s\n", r.visual->name));

#ifdef DEBUG
  if (file == stderr) {
    if (((Rest *)obj)->phrase.tied_forward)  Write((file,"\tTied Forward\n"));
    if (((Rest *)obj)->phrase.tied_backward) Write((file,"\tTied Backward\n"));
  }
#endif

  End;
}


Boolean ReadRest(MusicObject obj, FILE *file)
{
  int i;

  Begin("ReadRest");

  if (!ReadWS(file))
    Return(BuildFileError(file,
			  "Unexpected EOF (expecting Rest Name segment)"));
  
  if (!ReadString(file))
    Return(BuildFileError(file, "Couldn't read Rest Name segment"));

  for (i = 0; i < restVisualCount; ++i) {
    if (!strcasecmp(restVisuals[i].undotted.name, rString)) {
      (void)NewRest((Rest *)obj, restVisuals[i].undotted.type, False);
      break;
    }
    if (!strcasecmp(restVisuals[i].dotted.name, rString)) {
      (void)NewRest((Rest *)obj, restVisuals[i].dotted.type, True);
      break;
    }
  }

  if (i >= restVisualCount)
    Return(BuildFileError2s(file, "Unknown Rest Name `%s'", rString));

  Return(True);
}

/* }}} */
/* {{{ Chord */

void WriteChord(MusicObject obj, FILE *file, int depth)
{
  int       i;
  ChordPart c = ((Chord *)obj)->chord;

  Begin("WriteChord");

  Write((file, ": %s %lx %hd",
	 c.visual->name, c.modifiers, c.voice_count));
  for (i = 0; i < c.voice_count; ++i)
    WriteNoteVoice(&(c.voices[i]), file, depth+1);

  if (c.chord_name && (strlen(c.chord_name) > 0)) {
    fprintf(file, " %d \"%s\"\n", c.chord_named ? 1 : 0, c.chord_name);
  } else {
    fputc('\n', file);
  }

#ifdef DEBUG
  if (file == stderr) {
    if (((Chord *)obj)->phrase.tied_forward) Write((file,"\tTied Forward\n"));
    if (((Chord *)obj)->phrase.tied_backward)Write((file,"\tTied Backward\n"));
  }
#endif

  End;
}


Boolean ReadChord(MusicObject obj, FILE *file)
{
  int        i;
  short      n;
  NoteVoice *v;
  ChordMods  m;
  NoteTag    tag;
  Boolean    dotted;

  Begin("ReadChord");

  if (!ReadTag(file))
    Return(BuildFileError(file, "Couldn't read Chord Type or Dotted segment"));

  if (!strcasecmp(rTag, "Dotted")) {

    dotted = True;
    if (!ReadTag(file))
      Return(BuildFileError(file, "Couldn't read Chord Type segment"));

  } else dotted = False;

  for (i = 0; i < noteVisualCount; ++i)
    if (!strncasecmp(noteVisuals[i].undotted.name, rTag, strlen(rTag))) {
      tag = noteVisuals[i].undotted.type;
      break;
    }

  if (i > noteVisualCount)
    Return(BuildFileError2s(file, "Unknown Chord Type `%s'", rTag));

  if (!ReadHexUnsignedLong(file))
    Return(BuildFileError(file, "Expected Chord Modifier segment not found"));

  if (!ReadShort(file))
    Return(BuildFileError(file,
			  "Expected Chord Voice Count segment not found"));

  m = rULong;
  n = rShort;
  v = (NoteVoice *)XtMalloc(n * sizeof(NoteVoice));

  for (i = 0; i < n; ++i)
    if (!ReadNoteVoice(&(v[i]), file)) {
      XtFree((void *)v);
      Return(BuildFileError(file, "Too few Note Voices in Chord segment?"));
    }

  (void)NewChord((Chord *)obj, v, n, m, tag, dotted);

  {
    int c = 0;
    while (isspace(c = getc(file)));

    if (c != '0' && c != '1') {

      (void)ungetc(c, file);
      Return(True);

    } else {

      (void)ungetc(c, file);

      if (!ReadUnsignedInt(file) || rUInt > 1) {
	Return(BuildFileError
	       (file, "Absent or corrupted Chord explicitly-named tag"));
      }

      if (!ReadString(file) || !rString || !rString[0]) {
	Return(BuildFileError(file, "Corrupted Chord Name segment?"));
      }

      if (rString[strlen(rString)-1] == '\"') {
	rString[strlen(rString)-1] = '\0';
      }

      if (rString[0] == '\"') {
	NameChord((Chord *)obj, rString + 1, rUInt);
      } else {
	NameChord((Chord *)obj, rString, rUInt);
      }
    }
  }

  Return(True);
}

/* }}} */
/* {{{ Group */

void WriteGroup(MusicObject obj, FILE *file, int depth)
{
  Group   *g = (Group *)obj;
  GroupTag t = ((Group *)obj)->group.type;
  ItemList i;

  Begin("WriteGroup");

  Write((file, "Group\n"));
  Write((file, "\t%s",
	 t == GroupBeamed       ? "Beamed" :
	 t == GroupTupled       ? "Tupled" :
	 t == GroupDeGrace      ? "Grace"  : "Ordinary"));

  if (t == GroupTupled) {
    fprintf(file, " %hd %hd\n", g->group.tupled_length, g->group.tupled_count);
  } else {
    fputc('\n', file);
  }

#ifdef DEBUG
  if (file == stderr) {
    if (g->phrase.tied_forward)  Write((file,"\tTied Forward\n"));
    if (g->phrase.tied_backward) Write((file,"\tTied Backward\n"));
  }
#endif

  for (i = g->group.start; i; i = (i == g->group.end) ? NULL : iNext(i)) {
    i->item->methods->write((MusicObject)(i->item), file, depth+1);
    WriteMarks(i, file, depth+1); WriteFixedBars(i, file, depth+1);
  }

  Write((file, "\tEnd\n"));

  End;
}


Boolean ReadGroup(MusicObject obj, FILE *file)
{
  ItemList       items;
  GroupTag       type; 
  short          tupled_length = 0;
  short          tupled_count = 0;

  Begin("ReadGroup");

  if (readingGroup) Return
    (BuildFileError
     (file, "This implementation does not support groups within groups"));

  if (!ReadTag(file))
    Return(BuildFileError(file, "Expected Group Type tag not found"));

  if      (!strcasecmp(rTag, "Beamed"  )) type = GroupBeamed;
  else if (!strcasecmp(rTag, "Tupled"  )) type = GroupTupled;
  else if (!strcasecmp(rTag, "Grace"   )) type = GroupDeGrace;
  else if (!strcasecmp(rTag, "Ordinary")) type = GroupNoDecoration;
  else Return(BuildFileError2s(file, "Unknown Group Type tag `%s'", rTag));

  if (type == GroupTupled) {

    if (!ReadInt(file))
      Return(BuildFileError(file, "Expected Group Tupled Length not found"));
    tupled_length = NumberToMTime(rInt);

    if (!ReadInt(file))
      Return(BuildFileError(file, "Expected Group Tupled Count not found"));
    tupled_count = NumberToMTime(rInt);
  }

  readingGroup = True;

  if (!ReadItemList(&items, file))
    Return(BuildFileError(file, "Couldn't read Item List for Group"));

  (void)NewGroup((Group *)obj, type, items, (ItemList)Last(items));
  ((Group *)obj)->group.tupled_length = tupled_length;
  ((Group *)obj)->group.tupled_count  = tupled_count;

  readingGroup = False;

  Return(True);
}

/* }}} */
/* {{{ Item (polymorphic) */

/* ReadItem is the only Read function to actually allocate its own
   space, as we don't know how big the Item will be beforehand -- this
   is a polymorphic function.  (Apart from ReadChord, that is, which
   allocates space for the NoteVoices itself, and ReadItemList, which
   calls for NewItemLists left, right and centre.) */

#define ReadItemFailed 0
#define ReadItemEndFound 1
#define ReadItemSucceeded 2

int ReadItemOrMark(IO MusicObject *obj, INPUT ItemList items, FILE *file)
{
  int         i;
  MusicObject item;

  Begin("ReadItemOrMark");

  if (!ReadTag(file))
    Return(BuildFileError(file,"Expected Item Discrimination Tag not found"));

  for (i = 0; i < XtNumber(itemTags); ++i)
    if (!strcmp(rTag, itemTags[i].tag)) break;

  if (i >= XtNumber(itemTags)) {

    if (!strcasecmp(rTag, "Mark")) {

      if (!items) {
	BuildFileError
	  (file,"Mark indicated before any items read, or on Group item");
	Return(ReadItemFailed);
      }

      if (!ReadMark(items, file)) {
	BuildFileError(file, "Couldn't read Mark associated with Item");
	Return(ReadItemFailed);
      }

      *obj = NULL;
      Return(ReadItemSucceeded);

    } else if (!strcasecmp(rTag, "Follows") || !strcasecmp(rTag, "Precedes")) {

      if (!items) {
	BuildFileError
	  (file,"Fixed Bar indicated before any items read, or on Group item");
	Return(ReadItemFailed);
      }

      if (!ReadFixedBar(rTag, items, file)) {
	BuildFileError(file, "Couldn't read Fixed Bar associated with Item");
	Return(ReadItemFailed);
      }

      *obj = NULL;
      Return(ReadItemSucceeded);

    } else if (!strcasecmp(rTag, "End")) {

      Return(ReadItemEndFound);

    } else {

      BuildFileError2s(file, "Unknown Item Discrimination Tag `%s'", rTag);
      Return(ReadItemFailed);
    }
  } else {

    item = (MusicObject)XtMalloc(itemTags[i].size);

    if (!itemTags[i].read(item, file)) {
      XtFree(item);
      BuildFileError2s(file, "Couldn't read indicated %s Item",itemTags[i].tag);
      Return(ReadItemFailed);
    }

    *obj = item;
    Return(ReadItemSucceeded);
  }
}

/* }}} */
/* {{{ Read ItemList */

static Boolean ReadItemList(ItemList *list, FILE *file)
{
  ItemList items = NULL;
  Item    *item = NULL;
  int      rtn;

  Begin("ReadItemList");

  while ((rtn = ReadItemOrMark((MusicObject *)&item, items, file)) ==
	 ReadItemSucceeded) {

    if (item) items = (ItemList)Nconc(items, NewItemList(item));
    /* otherwise it must have been a mark */
  }

  if (rtn == ReadItemFailed) {
    for (items = (ItemList)First(items);
	 items; items = iNext(items)) XtFree((char *)(items->item));
    DestroyList(items);
    Return(BuildFileError(file,"Couldn't read up to ItemList end tag"));
  }

  *list = (ItemList)First(items);
  (void)ItemListEnsureIntegrity(*list);

  Return(True);
}

/* }}} */

/* {{{ Main LoadStaveFromFile function */

MajorStave LoadStaveFromFile(FILE *file)
{
  int         i;
  char        t;
  int         staves;
  ItemList  * music = NULL;
  String    * names = NULL;
  static char message[256];
  Boolean     havePreamble = False;
  MajorStave  sp = NULL;

  Begin("LoadStaveFromFile");

  StaveBusyStartCount(0);
  StaveBusyMakeCount(0);
  readingGroup = False;
  format20 = False;

  if (musicIOErrorMessage) {
    XtFree(musicIOErrorMessage);
    musicIOErrorMessage = NULL;
  }

  if (fscanf(file,
	     "#!Rosegarden\n#\n#  Musical Notation File\n%c\n\n", &t) != 1 ||
      t != '#') {

    BuildFileError(file, "File Identifier Preamble missing or corrupted");
    goto failed;
  } else havePreamble = True;

  if (!ReadTag(file) || (strcmp(rTag, "RV20") && strcmp(rTag, "RV21"))) {
    YQuery(XtParent(musicViewport),
	   "Not a Rosegarden v2.0 or v2.1 file. This might be an old format;\n"
	   "if so, you will need version 2.0.1 or earlier to convert it.",
	   1, 0, 0, "OK", NULL);
    fclose(file);
    Return(NULL);
  }

  if (!strcmp(rTag, "RV20")) {
    format20 = True;
    potentialDynamics = 0;
    fprintf(stderr, "Rosegarden: loading v2.0 file\n");
  }

  if (!ReadConstTag("Staves", file)) {
    BuildFileError(file, "Expected Number-of-Staves Counter not found");
    goto failed;
  }

  if (!ReadInt(file)) {
    BuildFileError(file, "Couldn't read Number-of-Staves Counter value");
    goto failed;
  }

  staves = rInt;
  music  = (ItemList *)XtMalloc(staves * sizeof(ItemList));
  names  = (String   *)XtMalloc(staves * sizeof(String));

  StaveBusyStartCount(staves + 3);

  for (i = 0; i < staves; ++i) {
    music[i] = NULL;
    names[i] = NULL;
  }

  for (i = 0; i < staves; ++i) {

    StaveBusyMakeCount(i + 1);

    if (!ReadConstTag("Name", file)) {
      BuildFileError(file, "Expected Stave Name segment not found");
      goto failed;
    }

    if (!ReadString(file)) {
      BuildFileError(file, "Couldn't read Stave Name");
      goto failed;
    }

    names[i] = XtNewString(rString);

    if (!ReadItemList(&(music[i]), file)) {
      BuildFileError(file, "Couldn't read Item List for contents of Stave");
      goto failed;
    }
  }

  StaveBusyMakeCount(staves + 1);
  for (i = 0; i < staves; ++i) EnsureMarkIntegrity(music[i]);
  sp = NewStave(staves, music);
  for (i = 0; i < staves; ++i) {
    StaveRenameStave(sp, i, names[i]);
    StaveResetFormatting(sp, i);
    StaveFormatBars(sp, i, -1);
  }

 barStyles:

  StaveBusyMakeCount(staves + 2);

  if (!ReadTag(file)) {
    BuildFileError(file, "End or Bar Style marker apparently missing");
    goto failed;
  }

  if (!strcasecmp(rTag, "End")) goto succeeded;
  if (!strcasecmp(rTag, "Stave")) goto haveStaveTag;

  if (strcasecmp(rTag, "Bar")) {
    BuildFileError2s
      (file, "Unexpected tag `%s' (expecting `Stave', `Bar' or `End')", rTag);
    goto failed;
  }

  {
    int sno, num;

    if (!ReadInt(file)) {
      BuildFileError(file, "Couldn't read Bar Time staff number segment");
      goto failed;
    }
    sno = rInt;

    if (!ReadHexUnsignedLong(file)) {
      BuildFileError(file, "Couldn't read Bar Time bar number segment");
      goto failed;
    }

    if (!ReadConstTag("time", file) || !ReadInt(file)) {
      BuildFileError(file, "Couldn't read Bar Time numerator segment");
      goto failed;
    }
    num = rInt;

    if (!ReadInt(file)) {
      BuildFileError(file, "Couldn't read Bar Time denominator segment");
      goto failed;
    }

    StaveAddBarTime(sp, sno, rULong, num, rInt);
    goto barStyles;
  }

 haveStaveTag:

  {
    int sno, type;

    if (!ReadInt(file)) {
      BuildFileError(file, "Couldn't read Stave Style staff number segment");
      goto failed;
    }
    sno = rInt;
    
    if (!ReadConstTag("tags", file) || !ReadInt(file)) {
      BuildFileError(file, "Couldn't read Stave Style Precede marker");
      goto failed;
    }
    type = rInt;

    if (!ReadInt(file)) {
      BuildFileError(file, "Couldn't read Stave Style Follow marker");
      goto failed;
    }

    StaveSetEndBarTags(sp, sno, type, rInt);

    if (!format20) {

      if (!ReadConstTag("connected", file) || !ReadInt(file)) {
	fprintf(stderr, "Warning: couldn't read Stave Connection marker");
	goto succeeded; /* never mind */
      }
      StaveSetConnection(sp, sno, (Boolean)rInt);

      if (!ReadConstTag("program", file) || !ReadInt(file)) {
	BuildFileError(file, "Couldn't read Stave MIDI Program marker");
	goto succeeded; /* never mind */
      }
      StaveSetMIDIPatch(sp, sno, rInt);
    }      
  }

  goto barStyles;

 succeeded:

  fclose(file);
  if (format20 && potentialDynamics > 0)
    MaybeConvertDynamics(sp, potentialDynamics);
  StaveBusyFinishCount();
  Return(sp);

 failed:

  StaveBusyFinishCount();

  if (sp) StaveDestroy(sp, True);
  else {

    if (music) {
      for (i = 0; i < staves && music[i]; ++i) DestroyItemList(music[i]);
      XtFree((void *)music);
    }

    if (names) {
      for (i = 0; i < staves && names[i]; ++i) XtFree(names[i]);
      XtFree((void *)names);
    }
  }

  if (!havePreamble) {
    if (YFileGetLastFilename(False) == NULL)
      sprintf(message, "File is not a Rosegarden notation file.");
    else
      sprintf(message, "``%s'' is not a Rosegarden notation file.",
	      YFileGetLastFilename(False));
  } else {
    if (YFileGetLastFilename(False) == NULL)
      sprintf(message, "Cannot load file, sorry.");
    else
      sprintf(message, "Cannot load ``%s'', sorry.",
	      YFileGetLastFilename(False));
  }

  if (musicIOErrorMessage) {

    if (YQuery(XtParent(musicViewport), message, 2, 0, 0,
	       "Continue", "Show Error Report", NULL) == 1)
      (void)YQuery(XtParent(musicViewport),
		   musicIOErrorMessage, 1, 0, 0, "Continue", NULL);
  } else {

    (void)YQuery(XtParent(musicViewport), message, 1, 0, 0, "Continue", NULL);
  }

  fclose(file);
  Return(NULL);
}

/* }}} */
/*       (note that writing a stave is done from Stave.c) */

/* {{{ Write Marks */

unsigned int markIOIndex;
static char *markIONames[] = { "Tie", "Slur", "Crescendo", "Decrescendo" };

void WriteMarks(ItemList i, FILE *file, int depth)
{
  MarkList mlist;
  Begin("WriteMarks");

  for (mlist = (MarkList)First(i->item->item.marks); mlist;
       mlist = (MarkList)Next(mlist)) {

    if (!mlist->mark->other_end) {
      fprintf(stderr, "Warning: Found mark (type %d) with invalid other-end "
	      "field; ignoring\n", mlist->mark->type);
      continue;
    }

    if (mlist->mark->start) {
    
      Write((file, "Mark start %u %s\n",
	     markIOIndex, markIONames[(int)mlist->mark->type]));
      mlist->mark->ilist = (MusicObject)markIOIndex;
      ++markIOIndex;

    } else {
      Write((file, "Mark end %u\n", (int)mlist->mark->other_end->ilist));
    }
  }

  End;
}

/* }}} */
/* {{{ Read Mark */

/* This one assumes the Mark tag has already been read */

static Boolean ReadMark(ItemList i, FILE *file)
{
  int type;
  Mark *newMark;
  Begin("ReadMarks");

  if (!ReadTag(file))
    Return(BuildFileError(file, "Couldn't read Mark Start/End segment"));

  if (!ReadUnsignedInt(file))
    Return(BuildFileError(file, "Couldn't read Mark Index segment"));

  if (!strcasecmp(rTag, "start")) {

    if (!ReadTag(file))
      Return(BuildFileError(file, "Couldn't read Mark Type segment"));

    for (type = 0; type < XtNumber(markIONames); ++type) {
      if (!strcasecmp(markIONames[type], rTag)) break;
    }

    if (type >= XtNumber(markIONames))
      Return(BuildFileError2s(file, "Unknown Mark Type tag %s", rTag));
    
    newMark = NewMark(NULL, (MarkTag)type, True, NULL);
    newMark->ilist = (MusicObject)rUInt;

    i->item->item.marks = (MarkList)Nconc
      (i->item->item.marks, NewMarkList(newMark));

  } else {

    newMark = NewMark(NULL, (MarkTag)1 /* ie. don't know yet */, False, NULL);
    newMark->ilist = (MusicObject)rUInt;

    i->item->item.marks = (MarkList)Nconc
      (i->item->item.marks, NewMarkList(newMark));
  }

  Return(True);
}

/* }}} */

/* {{{ Write Fixed Bars */

void WriteFixedBars(ItemList i, FILE *file, int depth)
{
  int t;
  Begin("WriteFixedBars");

  t = i->item->item.bar_tag;

  if (t != NoFixedBar)
    Write((file, "Precedes %s\n",
	   t == DoubleBar      ? "Double"      :
	   t == RepeatLeftBar  ? "RepeatLeft"  :
	   t == RepeatRightBar ? "RepeatRight" :
	   t == RepeatBothBar  ? "RepeatBoth"  :
	   t == PlainBar       ? "Plain"       : "None"));

  End;
}

/* }}} */
/* {{{ Read Fixed Bar */

Boolean ReadFixedBar(String tag, ItemList i, FILE *file)
{
  Boolean follows;
  Begin("ReadFixedBar");

  follows = !strcasecmp(tag, "Follows");

  if (!ReadTag(file)) {
    Return(BuildFileError(file, "Couldn't read Fixed Bar Type segment"));
  }

  if (follows) {

    fprintf(stderr, "Rosegarden: Warning: Obsolete `Follows' bar-tag read ");

    i = iPrev(i);
    if (!i) {
      fprintf(stderr, "-- ignoring\n");
      Return(True);
    } else {
      fprintf(stderr, "-- improvising\n");

      /* Can't have RepeatBoth, RepeatLeft or RepeatRight, as they
         were added at the same time as Follows was removed */

      if (!strcasecmp(rTag, "Double")) {
	i->item->item.bar_tag = DoubleBar;
      } else if (!strcasecmp(rTag, "Repeat")) {
	if (i->item->item.bar_tag == RepeatRightBar) {
	  i->item->item.bar_tag = RepeatBothBar;
	} else {
	  i->item->item.bar_tag = RepeatLeftBar;
	}
      } else if (!strcasecmp(rTag, "Plain")) {
	i->item->item.bar_tag = PlainBar;
      }
    }
  }
  
  if (!strcasecmp(rTag, "Double")) {
    i->item->item.bar_tag = DoubleBar;
  } else if (!strcasecmp(rTag, "Repeat")) {
    i->item->item.bar_tag = RepeatRightBar;
  } else if (!strcasecmp(rTag, "RepeatLeft")) {
    i->item->item.bar_tag = RepeatLeftBar;
  } else if (!strcasecmp(rTag, "RepeatRight")) {
    i->item->item.bar_tag = RepeatRightBar;
  } else if (!strcasecmp(rTag, "RepeatBoth")) {
    i->item->item.bar_tag = RepeatBothBar;
  } else if (!strcasecmp(rTag, "Plain")) {
    i->item->item.bar_tag = PlainBar;
  } else if (!strcasecmp(rTag, "None")) {
    i->item->item.bar_tag = NoBarAtAll;
  } else {
    Return(BuildFileError2s
	   (file, "Unknown or unexpected Fixed Bar segment `%s'", rTag));
  }

  Return(True);
}

/* }}} */

/* {{{ Utility functions */

/* Utility error and reading functions. */


static Boolean ReadWS(FILE *file)
{
  Begin("ReadWS");

  (void)fscanf(file, " ");

  if (ferror(file))
    Return(BuildFileError(file, "File error while consuming whitespace"));

  if (feof(file))
    Return(BuildFileError(file, "Unexpected EOF"));

  Return(True);
}


static Boolean ReadInt(FILE *file)
{
  int i, j;

  Begin("ReadInt");

  i = fscanf(file, " %d", &j);

  if (i == EOF)
    Return(BuildFileError(file, "Unexpected EOF (expecting integer)"));
  if (i != 1)
    Return(BuildFileError(file, "Syntax Error (expecting integer)"));

  rInt = j;
  Return(True);
}


static Boolean ReadUnsignedInt(FILE *file)
{
  int          i;
  unsigned int j;

  Begin("ReadUnsignedInt");

  i = fscanf(file, " %u", &j);

  if (i == EOF)
    Return(BuildFileError(file, "Unexpected EOF (expecting unsigned int)"));
  if (i != 1)
    Return(BuildFileError(file, "Syntax Error (expecting unsigned int)"));

  rUInt = j;
  Return(True);
}


static Boolean ReadHexUnsignedInt(FILE *file)
{
  int          i;
  unsigned int j;

  Begin("ReadHexUnsignedInt");

  i = fscanf(file, " %x", &j);

  if (i == EOF)
    Return(BuildFileError(file,
			  "Unexpected EOF (expecting hex unsigned int)"));
  if (i != 1)
    Return(BuildFileError(file, "Syntax Error (expecting hex unsigned int)"));

  rUInt = j;
  Return(True);
}


static Boolean ReadHexUnsignedLong(FILE *file)
{
  int           i;
  unsigned long j;

  Begin("ReadHexUnsignedLong");

  i = fscanf(file, " %lx", &j);

  if (i == EOF)
    Return(BuildFileError(file,
			  "Unexpected EOF (expecting hex unsigned long int)"));
  if (i != 1)
    Return(BuildFileError(file,
			  "Syntax Error (expecting hex unsigned long int)"));

  rULong = j;
  Return(True);
}


static Boolean ReadShort(FILE *file)
{
  int   i;
  short j;

  Begin("ReadShort");

  i = fscanf(file, " %hd", &j);

  if (i == EOF)
    Return(BuildFileError(file, "Unexpected EOF (expecting short int)"));
  if (i != 1)
    Return(BuildFileError(file, "Syntax Error (expecting short int)"));

  rShort = j;
  Return(True);
}


static Boolean ReadTag(FILE *file)
{
  int         i;
  static char s[TAG_LEN];

  Begin("ReadTag");

  i = fscanf(file, " %s", s);

  if (i == EOF)
    Return(BuildFileError(file, "Unexpected EOF (expecting tag)"));
  if (i != 1)
    Return(BuildFileError(file, "Syntax Error (expecting tag)"));

  strcpy(rTag, s);
  Return(True);
}


static Boolean ReadConstTag(String tag, FILE *file)
{
  int         i;
  static char s[TAG_LEN];

  Begin("ReadTag");

  i = fscanf(file, " %s", s);

  if (i == EOF)
    Return(BuildFileError(file, "Unexpected EOF (expecting a tag)"));
  if (i != 1)
    Return(BuildFileError(file, "Syntax Error (expecting a tag)"));

  if (strcasecmp(s, tag) != 0)
    Return(BuildFileError3s(file, "Wrong tag (expecting `%s', got `%s')",
			    tag, s));

  Return(True);
}


static Boolean ReadString(FILE *file)
{
  int         i = 0;
  int         n = 0;
  static char s[STRING_LEN];

  Begin("ReadString");

  do { i = getc(file); } while (i != EOF && (i == ' ' || i == '\t'));

  if (i == EOF)
    Return(BuildFileError(file, "Unexpected EOF (expecting a string)"));

  if (i == '\n') { rString[0] = '\0'; Return(True); }
  if (i) { s[0] = (char)i; ++n; }

  if (fgets(s + n, STRING_LEN - 1 - n, file) == NULL)
    Return(BuildFileError(file, "Unexpected EOF (expecting a string)"));

  for (i = strlen(s); i > 0 && s[i-1] == '\n'; s[--i] = '\0');

  strcpy(rString, s);
  Return(True);
}



static Boolean BuildFileError(FILE *file, String message)
{
  int i;

  Begin("BuildFileError");

  if (musicIOErrorMessage) {

    i = strlen(musicIOErrorMessage);

    musicIOErrorMessage =
      (String)XtRealloc(musicIOErrorMessage, i + strlen(message) + 20);
    sprintf(musicIOErrorMessage+i-1, "[%ld]  %s\n\n", ftell(file), message);

  } else {

    musicIOErrorMessage = (String)XtMalloc(strlen(message) + 100);
    sprintf(musicIOErrorMessage, "\nRead Error Report\n\n[%ld]  %s\n\n",
	    ftell(file), message);
  }

  Return(False);
}


static Boolean BuildFileError2s(FILE *file, String message, String c)
{
  String s;

  Begin("BuildFileError2s");

  s = (String)XtMalloc(strlen(message) + strlen(c) + 3);
  sprintf(s, message, c);
  (void)BuildFileError(file, s);
  XtFree(s);

  Return(False);
}


static Boolean BuildFileError3s(FILE *file, String message, String c, String d)
{
  String s;

  Begin("BuildFileError3s");

  s = (String)XtMalloc(strlen(message) + strlen(c) + strlen(d) + 3);
  sprintf(s, message, c, d);
  (void)BuildFileError(file, s);
  XtFree(s);

  Return(False);
}

/* }}} */

