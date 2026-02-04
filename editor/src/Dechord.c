
/*
   Dechord -- analyse chord name and generate chord

   This file by Guillaume Laurent, glaurent@worldnet.fr
   Copyright 1996 Guillaume Laurent

   Used in Rosegarden under the same redistribution
   rights as the rest of the Rosegarden source

	27 Sept 1996	Chris Frantz (frantzc@nachos.bork.com)
		Added (no 3rd) functionality to allow specification
		of chords without thirds (besides "power chords"), such
		as C7(no 3rd) or E6(no 3rd).

		 Generalized this to 5th, 7th, 9th, 11th, 13th
		 G. Laurent.

		 Added chord names support

		 Added new feature : lower case chord names
		 yield single notes
*/

#include "Dechord.h"
#ifdef USE_POSIX_REGEXP

typedef regex_t Regexp;
#define NB_OF_REGEXPS 15
#define N_MATCH 10

regex_t *chord_regs[NB_OF_REGEXPS];

regmatch_t pmatch[N_MATCH];

#define Regexec(REG, STR) regexec(REG, STR, N_MATCH, pmatch, 0)

void Regsub(char matchedString[], int matchNb, char destString[])
{
  strncpy(destString,
	  matchedString+pmatch[matchNb].rm_so,
	  pmatch[matchNb].rm_eo - pmatch[matchNb].rm_so);
}

#define REG_MATCHED REG_NOERROR

#else /* H. Spencer's regexp */

typedef regexp Regexp;
#define Regexec regexec
#define REG_MATCHED 1

#endif

static Regexp *GetRoot, *IsMinor, *GetSus, *IsDim, *IsAug, *IsPowc,
  *Get7, *GetAdd, *GetAlt5, *Get6, *GetAlt9, *GetAlt11, *GetAlt13,
  *GetAltBass, *NoX;

static char workBuffer[255];

#define RESET_WORK_BUFFER memset(workBuffer, 0, 255);

#ifdef USE_POSIX_REGEXP

void InitRegs(void)
{
  int i;

  for(i=0; i < NB_OF_REGEXPS; i++)
    {
      chord_regs[i]=malloc(sizeof(regex_t));
      chord_regs[i]->buffer=malloc(sizeof(char));
      chord_regs[i]->allocated=sizeof(char);
      chord_regs[i]->translate=NULL;
      chord_regs[i]->fastmap=malloc(256);
    }

  GetRoot = chord_regs[0] ;
  IsMinor = chord_regs[1];
  GetSus = chord_regs[2];
  IsDim = chord_regs[3];
  IsAug = chord_regs[4];
  IsPowc = chord_regs[5];
  Get7 = chord_regs[6];
  GetAdd = chord_regs[7];
  GetAlt5 = chord_regs[8];
  Get6 = chord_regs[9];
  GetAlt9 = chord_regs[10];
  GetAlt11 = chord_regs[11];  
  GetAlt13 = chord_regs[12];
  GetAltBass = chord_regs[13];
  NoX = chord_regs[14];

  regcomp(GetRoot, GET_ROOT, REG_EXTENDED);
  regcomp(IsMinor, IS_MINOR, REG_EXTENDED);
  regcomp(GetSus, GET_SUS, REG_EXTENDED);
  regcomp(IsDim, IS_DIM, REG_EXTENDED);
  regcomp(IsAug, IS_AUG, REG_EXTENDED);
  regcomp(IsPowc, IS_POWC, REG_EXTENDED);
  regcomp(Get7, GET_7TH, REG_EXTENDED); 
  regcomp(GetAdd, GET_ADD, REG_EXTENDED);
  regcomp(GetAlt5, GET_ALT5, REG_EXTENDED);
  regcomp(Get6, GET_6, REG_EXTENDED);
  regcomp(GetAlt9, GET_ALT9, REG_EXTENDED);
  regcomp(GetAlt11, GET_ALT11, REG_EXTENDED);
  regcomp(GetAlt13, GET_ALT13, REG_EXTENDED);
  regcomp(GetAltBass, GET_ALTBASS, REG_EXTENDED);
  regcomp(NoX, NO_x, REG_EXTENDED);
}

void FreeRegs(void)
{
  regfree(GetRoot); regfree(IsMinor); regfree(GetSus);
  regfree(IsDim); regfree(IsAug); regfree(IsPowc); regfree(Get7);
  regfree(GetAdd); regfree(GetAlt5); regfree(Get6);
  regfree(GetAlt9); regfree(GetAlt11); regfree(GetAlt13); regfree(GetAltBass);
  regfree(NoX);
}

#else

void InitRegs(void)
{
  Begin("InitRegs");
  GetRoot = regcomp(GET_ROOT);
  IsMinor = regcomp(IS_MINOR);
  GetSus = regcomp(GET_SUS);
  IsDim = regcomp(IS_DIM);
  IsAug = regcomp(IS_AUG);
  IsPowc = regcomp(IS_POWC);
  Get7 = regcomp(GET_7TH);
  GetAdd = regcomp(GET_ADD);
  GetAlt5 = regcomp(GET_ALT5);
  Get6 = regcomp(GET_6);
  GetAlt9 = regcomp(GET_ALT9);
  GetAlt11 = regcomp(GET_ALT11);
  GetAlt13 = regcomp(GET_ALT13);
  GetAltBass = regcomp(GET_ALTBASS);
  NoX = regcomp(NO_x);
  /* gee, what a nice piece of code this is */
  End;
}

void FreeRegs(void)
{
  Begin("FreeRegs");
  free(GetRoot);
  free(IsMinor);
  free(GetSus);
  free(IsDim); free(IsAug); free(IsPowc); free(Get7);
  free(GetAdd); free(GetAlt5); free(Get6);
  free(GetAlt9); free(GetAlt11); free(GetAlt13); free(GetAltBass);
  free(NoX);
  End;
}

#endif

NoteVoice TransposeVoice(NoteVoice root, int interval,
			  ClefTag currentClef, Boolean sharpsOrFlats)
{
  Begin("TransposeVoice");

  return (MidiPitchToVoice(VoiceToMidiPitch(&root, currentClef)
			   + interval, sharpsOrFlats));
}

Pitch NoteToPitch(char Note, ClefTag currentClef)
{
  Pitch p;
  Begin("NoteToPitch");

  Note = toupper((int)Note);
  if((Note < 'A') && (Note > 'G')) Error("wrong note");

  p = Note - 'E' /* - 7 ?? */ + ClefPitchOffset(currentClef);
  Return(p);
}

Chord* ArrayToChord(char ChordArray[],
		   NoteVoice voicesBuf[], int lastNote,
		   ClefTag currentClef, Boolean sharpsOrFlats)
{
  int i;
  NoteVoice *voices, root = voicesBuf[0];
  Begin("ArrayToChord");

  for(i=lastNote; i < CHORDLENGTH; i++) {
    if(ChordArray[i])
      voicesBuf[lastNote++] = TransposeVoice(root, i, currentClef,
					     sharpsOrFlats);
  }

  voices = (NoteVoice*)XtMalloc(lastNote * sizeof(NoteVoice));
  memcpy(voices, voicesBuf, (size_t)(lastNote * sizeof(NoteVoice)));

  return(NewChord(NULL, voices, lastNote,
		  ModNone, Crotchet, 0));
}

int CompleteChord(int code, char ChordArray[])
{
  Begin("CompleteChord");
  switch(code) {

  case 13:
    if((!ChordArray[DIM_ELEVENTH]) &&
       (!ChordArray[ELEVENTH]) &&
       (!ChordArray[AUG_ELEVENTH]))
      ChordArray[ELEVENTH] = 1;

  case 11:
    if((!ChordArray[DIM_NINTH]) &&
       (!ChordArray[NINTH]) &&
       (!ChordArray[AUG_NINTH]))
      ChordArray[NINTH] = 1;
    
  case 9:
    if((!ChordArray[MINOR_SEVENTH]) &&
       (!ChordArray[MAJOR_SEVENTH]))
      ChordArray[MINOR_SEVENTH] = 1;
  }

  Return(UNCOMPLETE);
}


int FindThird(char ChordString[], char ChordArray[])
{
  
  Begin("FindThird");
  RESET_WORK_BUFFER;

  ChordArray[ROOT] = 1; /* gotta have a root, no matter what */

  if(Regexec(IsDim, ChordString) == REG_MATCHED) { /* is it diminished ? */
    ChordArray[MINOR_THIRD] = 1;
    ChordArray[DIM_FIFTH] = 1;
    ChordArray[DIM_SEVENTH] = 1;
    Return(COMPLETE);
  }

  if(Regexec(IsAug, ChordString) == REG_MATCHED) { /* is it augmented ? */
    ChordArray[MAJOR_THIRD] = 1;
    ChordArray[AUG_FIFTH] = 1;
    Return(COMPLETE);
  }

  if(Regexec(IsPowc, ChordString) == REG_MATCHED) { /* is it a power chord ? */
    ChordArray[FIFTH] = 1;
    Return(COMPLETE);
  }


  if(Regexec(IsMinor, ChordString) == REG_MATCHED)
     ChordArray[MINOR_THIRD] = 1; /* it's a minor chord */
  else if(Regexec(GetSus, ChordString) == REG_MATCHED) {  /* it can be a suspended chord */
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(GetSus, "&", workBuffer);
#endif
    if(workBuffer[3] == '2') ChordArray[MAJOR_SECOND] = 1; /* sus2 */
    else ChordArray[FOURTH] = 1; /* sus4 */
  } else ChordArray[MAJOR_THIRD] = 1;

  Return(UNCOMPLETE);
}

int FindFifth(char ChordString[], char ChordArray[])
{
  int tlen = strlen(ChordString);
  Begin("FindFifth");
  RESET_WORK_BUFFER;

  if(!tlen ||
     ((tlen == 1) && (ChordString[0] == 'm'))) {
    /* if ChordString is empty, or if it contains just an 'm', then
       there's nothing more to find but the fifth */
    ChordArray[FIFTH] = 1; 
    Return(COMPLETE);
  }

  if(Regexec(GetAlt5, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(GetAlt5, "&", workBuffer);
#endif
    if((workBuffer[0] == 'b') || (workBuffer[0] == '-'))
      ChordArray[DIM_FIFTH] = 1;
    else ChordArray[AUG_FIFTH] = 1;
  }
  else ChordArray[FIFTH] = 1;

  Return(UNCOMPLETE);
}

int FindSixth(char ChordString[], char ChordArray[])
{
  Begin("FindSixth");
  if(Regexec(Get6, ChordString) == REG_MATCHED) ChordArray[SIXTH] = 1;
  Return(UNCOMPLETE);
}

int FindSeventh(char ChordString[], char ChordArray[])
{
  
  Begin("FindSeventh");
  RESET_WORK_BUFFER;

  if(Regexec(Get7, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(Get7, "&", workBuffer);
#endif
    if(workBuffer[0] == 'j') ChordArray[MAJOR_SEVENTH] = 1;
    else ChordArray[MINOR_SEVENTH] = 1;
  }
  Return(UNCOMPLETE);
}

int FindAdd(char ChordString[], char ChordArray[])
{
  
  Begin("FindAdd");
  RESET_WORK_BUFFER;

  if(Regexec(GetAdd, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(GetAdd, "&", workBuffer);
#endif
    if(workBuffer[1] == '9') ChordArray[NINTH] = 1;
    else if(workBuffer[2] == '1') ChordArray[ELEVENTH] = 1;
    else ChordArray[THIRTEENTH] = 1;
  }
  Return(UNCOMPLETE);
}

int FindNinth(char ChordString[], char ChordArray[])
{
  int retcode = UNCOMPLETE;
  
  Begin("FindNinth");
  RESET_WORK_BUFFER;

  /* if Chord[NINTH] Return(UNCOMPLETE); */ 
  /* not the function's job to check if it should have been called in the
     first place */
  
  if(Regexec(GetAlt9, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(GetAlt9, "&", workBuffer);
#endif
    if((workBuffer[0] == 'b') || (workBuffer[0] == '-')) ChordArray[DIM_NINTH] = 1;
    else if((workBuffer[0] == '#') || (workBuffer[0] == '+')) ChordArray[AUG_NINTH] = 1;
    else {
      ChordArray[NINTH] = 1;
      retcode = COMPLETE;
    }

    CompleteChord(9, ChordArray);

  }
  Return(retcode);
}

int FindEleventh(char ChordString[], char ChordArray[])
{
  int retcode = UNCOMPLETE;
  
  Begin("FindEleventh");
  RESET_WORK_BUFFER;

  if(Regexec(GetAlt11, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(GetAlt11, "&", workBuffer);
#endif
    if((workBuffer[0] == 'b') || (workBuffer[0] == '-')) ChordArray[DIM_ELEVENTH] = 1;
    else if((workBuffer[0] == '#') || (workBuffer[0] == '+')) ChordArray[AUG_ELEVENTH] = 1;
    else {
      ChordArray[ELEVENTH] = 1;
      retcode = COMPLETE;
    }

    CompleteChord(11, ChordArray);
  }
  Return(retcode);
}

int FindThirteenth(char ChordString[], char ChordArray[])
{
  int retcode = UNCOMPLETE;

  Begin("FindThirteenth");
  RESET_WORK_BUFFER;
 
  if(Regexec(GetAlt13, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 0, workBuffer);
#else
    regsub(GetAlt13, "&", workBuffer);
#endif
    if((workBuffer[0] == 'b') || (workBuffer[0] == '-')) ChordArray[DIM_THIRTEENTH] = 1;
    else if((workBuffer[0] == '#') || (workBuffer[0] == '+')) ChordArray[AUG_THIRTEENTH] = 1;
    else {
      ChordArray[THIRTEENTH] = 1;
      retcode = COMPLETE;
    }

    CompleteChord(13, ChordArray);

  }
  Return(retcode);
}

int FindSuppressedNote(char ChordString[], char ChordArray[])
{
  int suppressedNote;

  Begin("FindSuppressedNote");
  RESET_WORK_BUFFER;

  if(Regexec(NoX, ChordString) == REG_MATCHED) {
#ifdef USE_POSIX_REGEXP
    Regsub(ChordString, 1, workBuffer);
#else
    regsub(NoX, "\\1", workBuffer);
#endif
    suppressedNote = atoi(workBuffer);
    switch(suppressedNote) {
    case 3:
      ChordArray[MAJOR_THIRD] = 0; 
      /* We don't care about the minor 3rd, specifying "no 3rd" for a
         minor chord is a bit stupid, don't you think ? */
      break;
    case 5: 
      ChordArray[DIM_FIFTH] = ChordArray[FIFTH] = ChordArray[AUG_FIFTH] = 0;
      break;
    case 7:
      ChordArray[DIM_SEVENTH] = ChordArray[MINOR_SEVENTH] = ChordArray[MAJOR_SEVENTH] = 0;
      break;
    case 9:
      ChordArray[DIM_NINTH] = ChordArray[NINTH] = ChordArray[AUG_NINTH] = 0;
      break;
    case 11:
      ChordArray[DIM_ELEVENTH] = ChordArray[ELEVENTH] = ChordArray[AUG_ELEVENTH] = 0;
      break;
    case 13: /* Now this is even more stupid. Only way to get here is "X13 (no 13th)" */
      ChordArray[DIM_THIRTEENTH] = ChordArray[THIRTEENTH] = ChordArray[AUG_THIRTEENTH] = 0;
      break;
    }
  }
  return(COMPLETE);
}

Chord* SpellChord(char ChordString[], ClefTag currentClef, Boolean sharpsOrFlats)
{
  int match, IsComplete, lastNote = 0;
  static Boolean initWasDone = FALSE;
  Pitch rootPitch = 0, altBassPitch = 0;
  NoteMods modifiers = ModNone;
  char ChordArray[CHORDLENGTH], *subChordString;
  char RootNote[2], AltBass[3]; /* extra char is for leading slash */
  NoteVoice voicesBuf[CHORDLENGTH];
  Chord *newChord;
  Begin("SpellChord");


  if(!initWasDone) {
    InitRegs();
    initWasDone = TRUE;
  }

  /* Zero out a few variables */
  RootNote[1] = 0;
  lastNote = 0;
  memset(voicesBuf, 0, (size_t)CHORDLENGTH * sizeof(NoteVoice));
  memset(ChordArray, 0, (size_t)CHORDLENGTH * sizeof(char));
  
  match = Regexec(GetRoot, ChordString); /* Get Root Note (ie tonic) */

  if(match != REG_MATCHED) {
    IssueMenuComplaint("Can't recognize root note");
    Return(NULL);
  }

#ifdef USE_POSIX_REGEXP
  Regsub(ChordString, 0, RootNote);
#else
  regsub(GetRoot, "&", RootNote);
#endif

  rootPitch = NoteToPitch(RootNote[0], currentClef); 
  switch(RootNote[1])
    {
    case 'b' : modifiers = ModFlat; break;
    case '#' : modifiers = ModSharp; break;
    default : modifiers = ModNone;
    }

  NewNoteVoice(&(voicesBuf[0]), rootPitch, modifiers); lastNote++;

  subChordString = ChordString + strlen(RootNote);
  /* So the Find<x> functions will only see what is "after" the root,
     ie from "Cmaj7" we just want to see "maj7" */

  while(isspace(subChordString[0])) subChordString++;

  if(isupper((int)RootNote[0])) {

    if(Regexec(GetAltBass, ChordString) == REG_MATCHED) { /* Get possible altered bass */
#ifdef USE_POSIX_REGEXP
      Regsub(ChordString, 1, AltBass);
#else
      regsub(GetAltBass, "\\1", AltBass);
#endif
      altBassPitch = NoteToPitch(AltBass[0], currentClef) - 7; 
      switch(AltBass[1])
	{
	case 'b' : modifiers = ModFlat; break;
	case '#' : modifiers = ModSharp; break;
	default : modifiers = ModNone;
	}
      NewNoteVoice(&(voicesBuf[1]), altBassPitch, modifiers); lastNote++;
    }

    /* Now analyse the chord */
    IsComplete = FindThird(subChordString, ChordArray);
    if(!IsComplete)
      IsComplete = FindFifth(subChordString, ChordArray);
    if(!IsComplete)
      IsComplete = FindSixth(subChordString, ChordArray);
    if(!IsComplete)
      IsComplete = FindSeventh(subChordString, ChordArray);
    if(!IsComplete)
      IsComplete = FindAdd(subChordString, ChordArray);
    if((!IsComplete) && (!ChordArray[NINTH])) /* an added 9th 11th or 13th */
      IsComplete = FindNinth(subChordString, ChordArray); /* may be found by FindAdd */
    if((!IsComplete) && (!ChordArray[ELEVENTH]))
      IsComplete = FindEleventh(subChordString, ChordArray);
    if((!IsComplete) && (!ChordArray[THIRTEENTH]))
      IsComplete = FindThirteenth(subChordString, ChordArray);

    FindSuppressedNote(subChordString, ChordArray);

    /* FreeRegs(); Should be called on editor's exit */
  }

  newChord = ArrayToChord(ChordArray, voicesBuf, lastNote,
	       currentClef, sharpsOrFlats);

  NameChord(newChord, ChordString, True);

  Return(newChord);

}
