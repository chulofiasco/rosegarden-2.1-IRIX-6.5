#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <tcl.h>

#define NB_MODES 7
#define NB_TONALITIES 17 /* names, actually */
#define NB_NOTES 12
#define NB_BUILTIN_SCALES 14

#define BUF_SIZE 1024

#define USAGE_ERROR -1

typedef unsigned char uchar;

static uchar scaleMajor[] = { 2, 2, 1, 2, 2, 2, 1, 0 },
  scaleMinor[] = { 2, 1, 2, 2, 1, 2, 2, 0 },
  scaleHarmonicMinor[] = { 2, 1, 2, 2, 1, 3, 1, 0 },
    scaleMelodicMinor[] = { 2, 1, 2, 2, 2, 2, 1, 0 },
    scaleWholeTone[] = { 2, 2, 2, 2, 2, 2, 0 },
      scaleDiminished[] = { 2, 1, 2, 1, 2, 1, 2, 1, 0 },
      scalePentatonic[] = { 2, 2, 3, 2, 3, 0 },
	scalePentatonicMinor[] = { 3, 2, 2, 3, 2, 0 },
	scaleBlues[] = { 3, 2, 1, 1, 3, 2, 0 },
	  scaleEnigmatic[] = { 1, 3, 2, 2, 2, 1, 1, 0 },
	  scaleNeapolitan[] = { 1, 2, 2, 2, 2, 2, 1, 0 },
	    scaleNeapolitanMinor[] = { 1, 2, 2, 2, 1, 3, 1, 0 },
	    scaleHungarian[] = { 2, 1, 3, 1, 1, 3, 1, 0 },
	      scaleChromatic[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	      scaleAny[NB_NOTES], scaleAny2[NB_NOTES];
/* This last ones for user-defined scales */

static char builtInScaleNames[NB_BUILTIN_SCALES][24] = { "scaleMajor",
							 "scaleMinor",
							 "scaleHarmonicMinor",
							 "scaleMelodicMinor",
							 "scaleWholeTone",
							 "scaleDiminished",
							 "scalePentatonic",
							 "scalePentatonicMinor",
							 "scaleBlues",
							 "scaleEnigmatic",
							 "scaleNeapolitan",
							 "scaleNeapolitanMinor",
							 "scaleHungarian",
							 "scaleChromatic" };

static char *builtInScales[NB_BUILTIN_SCALES] = { scaleMajor,
						  scaleMinor,
						  scaleHarmonicMinor,
						  scaleMelodicMinor,
						  scaleWholeTone,
						  scaleDiminished,
						  scalePentatonic,
						  scalePentatonicMinor,
						  scaleBlues,
						  scaleEnigmatic,
						  scaleNeapolitan,
						  scaleNeapolitanMinor,
						  scaleHungarian,
						  scaleChromatic };

static char modeNames[NB_MODES][16] = { "modeIonian",
					"modeDorian",
					"modePhrygian",
					"modeLydian",
					"modeMixolydian",
					"modeAeolian",
					"modeLocrian" };

static char tonalityNames[NB_TONALITIES][16] = { "tonalityC",
						 "tonalityCsharp", "tonalityDflat",
						 "tonalityD",
						 "tonalityDsharp", "tonalityEflat",
						 "tonalityE",
						 "tonalityF",
						 "tonalityFsharp", "tonalityGflat",
						 "tonalityG",
						 "tonalityGsharp", "tonalityAflat",
						 "tonalityA",
						 "tonalityAsharp", "tonalityBflat",
						 "tonalityB" };

static char noteNames[] = "C {C# Db } D {D# Eb} E F {F# Gb} G {G# Ab} A {A# Bb} B",
  noteSharps[] = "C C# D D# E F F# G G# A A# B",
  noteFlats[] = "C Db D Eb E F Gb G Ab A Bb B";

static char noteNamesArray[NB_NOTES][10] = { "C", "C# Db", "D", "D# Eb",
					     "E", "F", "F# Gb", "G",
					     "G# Ab", "A", "A# Bb", "B" };

typedef enum { modeIonian = 0, 
	       modeDorian = 2,
	       modePhrygian = 4,
	       modeLydian = 5,
	       modeMixolydian = 7,
	       modeAeolian = 9,
	       modeLocrian = 11
} harmonicMode;
static short modeValues[] = { modeIonian, modeDorian, modePhrygian,
			      modeLydian, modeMixolydian, modeAeolian,
			      modeLocrian };
/*
#define USE_SHARPS Ox00
#define USE_FLATS  0xF0
#define PITCH_MASK 0x0F
#define MOD_MASK   0xF0
*/

typedef enum { tonalityC = 0,
	       tonalityCsharp = 1, tonalityDflat = 1,
	       tonalityD = 2,
	       tonalityDsharp = 3, tonalityEflat = 3,
	       tonalityE = 4, 
	       tonalityF = 5, 
	       tonalityFsharp = 6, tonalityGflat = 6,
	       tonalityG = 7,
	       tonalityGsharp = 8, tonalityAflat = 8,
	       tonalityA = 9,
	       tonalityAsharp = 10, tonalityBflat = 10,
	       tonalityB = 11,
} tonality; /* Look Ma, those are midi pitches ! */

short tonalityToTonalityName[] = { 0, 1, 3, 4, 6, 7, 8, 10, 11, 13, 14, 16 };

tonality tonalityNameToTonalityTable[] = { tonalityC, tonalityCsharp, tonalityDflat, tonalityD, tonalityDsharp,
			    tonalityEflat, tonalityE, tonalityF, tonalityFsharp,
			    tonalityGflat, tonalityG, tonalityGsharp,
			    tonalityAflat, tonalityA, tonalityAsharp, tonalityBflat, tonalityB }; 


/* Internal global defaults */

static uchar *defaultScale = scaleMajor,
  defaultScaleName[BUF_SIZE]; /* For error recoveries */
harmonicMode defaultMode = modeIonian;
tonality defaultTonality = tonalityC;

static char gbuf[BUF_SIZE];

/* 

   - scaleName is the input name of the scale (e.g. scaleMinor)

   - scaleToSet is the var which we want to set to a scale (e.g. the
   defaultScale global)

   - scaleNameToSet, likewise, will contain a copy of the input name,
   if not null (we actually want one only when setting defaultScale,
   because we need to remember its value as a fallback in case of
   errors)

   - userScale will contain a possible user-defined scale (e.g. 2 2
   1...) if this is what scaleName points to, in which case scaleToSet
   will point to it.

   Any error message will be written in gbuf.

   */
char * CheckScale(Tcl_Interp *interp, char *scaleName,
		  uchar **scaleToSet, uchar userScale[NB_NOTES])
{
  int i, s;
  char *userScaleInterp;

  /* First, check if the scale is among our built-ins */
  for(s=0; s<NB_BUILTIN_SCALES; s++) {
    if(!strcasecmp(scaleName, builtInScaleNames[s])) {
      *scaleToSet = builtInScales[s];
      return NULL;
    }
  }

  /* Nope, looks like the user asks for a scale of his own, so we get the
     variable's value */
  if(!(userScaleInterp = Tcl_GetVar(interp, scaleName, TCL_GLOBAL_ONLY))) {
    sprintf(gbuf, "Define scale %s to a list of integers first.\n", scaleName);
    return gbuf;
  }

  /* DO NOT USE strtok on userScale here! It points to the actual
     var. value */
  for(i=0, s=0; i < strlen(userScale); i++) {
    if(isdigit(userScale[i])) {
      userScale[s] = userScaleInterp[i] - 0x30;
	/* printf("-> %u\n", scaleAny[s]); */
	s++; if(s>NB_NOTES) {
	  return "Too many notes in this scale, max. nb is 12";
	}
      }
    /* Check that the added intervals equal to 12 ? */
  }

  *scaleToSet = userScale;

  return NULL;

}

char* CheckTonality(Tcl_Interp *interp, char *tonalityName,
		    tonality *tonalityToSet)
{
  int tonality;
  static char errMsg[] = "tonality must be an integer between 0 and 11";

  if(sscanf(tonalityName, " %u", &tonality))
    if(tonality < NB_NOTES) { /* NOT NB_TONALITIES (aha!) */
      *tonalityToSet = tonality;
      return NULL;
    }
  return errMsg;
}

char* CheckMode(Tcl_Interp *interp, char *modeName, harmonicMode *modeToSet)
{
  int mode;
  static char errMsg[] = "mode must be an integer between 0 and 6";

  if(sscanf(modeName, " %u", &mode))
    if(mode < NB_MODES) {
      *modeToSet = mode;
      return NULL;
    }
  return errMsg;
}


/* Get the often used '[ -scale S ] [ -tonality K ] [ -mode M ]' options,
   and stores the found values in optScale, optTonality, optMode.

   If one of those parameters is NULL, which means the calling
   func. is not expecting the corresponding option, and the option is
   found nonetheless, returns USAGE_ERROR.

   Also returns USAGE_ERROR if the value found for the option doesn't
   pass the checks.

 */
int GetStandardOptions(Tcl_Interp *interp, int argc, char *argv[],
		       uchar **optScale,
		       tonality *optTonality, harmonicMode *optMode)
{
  int i;
  char *errMsg;

  for(i=1; i < argc - 1; i++) { /* argv[0] is the command name */

    if(argv[i][0] != '-') break; /* Stop on the 1st arg that isn't an option */

    if(!strcmp(argv[i], "-scale")) {
      if((!optScale) ||
	 (errMsg = CheckScale(interp, argv[++i], optScale, scaleAny2))) {
	Tcl_AppendResult(interp, errMsg, NULL);
	return USAGE_ERROR;
      }
    } else if(!strcmp(argv[i], "-tonality")) {
      if((!optTonality) ||
	 (errMsg = CheckTonality(interp, argv[++i], optTonality))) {
	Tcl_AppendResult(interp, errMsg, NULL);
	return USAGE_ERROR;
      }
    } else if(!strcmp(argv[i], "-mode")) {
      if((!optMode) ||
	 (errMsg = CheckMode(interp, argv[++i], optMode))) {
	Tcl_AppendResult(interp, errMsg, NULL);
	return USAGE_ERROR;
      }
    }

  }
  return i;
}

/*
  Returns a midi pitch (the lowest one) from a string representing a note
   e.g. "A", "C#", etc...
*/
int NoteToMidi(char *note)
{
  static int noteToMidiTable[] = { 9, 11, 0, 2, 4, 5, 7 };
  int rtn;

  if(!strstr(noteNames, note)) return -1;

  rtn = noteToMidiTable[note[0] - 'A'];

  if(note[1]) {
    if(note[1] == 'b') rtn--;
    else if(note[1] == '#') rtn++;
  }
  return rtn;
}

/*
  Returns a pointer to a note name (among the predefined ones in the
  noteNamesArray
*/
char* MidiToNote(int pitch)
{
  pitch %= NB_NOTES;
  return noteNamesArray[pitch];
}

int BuildScale(uchar *scale, tonality tonality,
	       harmonicMode mode, short pitches[])
{
  short i; 
  int scaleLen = strlen(scale);
  pitches[0] = (tonality + mode) % NB_NOTES;
  for(i=0; i < scaleLen; i++)
    pitches[i+1] = (pitches[i] + scale[(i + mode) % scaleLen]) % NB_NOTES;

  return 0; /* May be I'll return some useful value someday... */
}

int PitchIsInScale(uchar *scale, tonality tonality,
		   harmonicMode mode, unsigned int pitch)
{
  short i;
  static short pitches[NB_NOTES];

  pitch %= NB_NOTES;

  BuildScale(scale, tonality, mode, pitches); /* QUICK'N DIRTY */
  /* I REALLY SHOULD OPTIMIZE THIS !!!!!!! */

  for(i=0; i<strlen(scale); i++)
    if(pitch == pitches[i]) return i;

  return -1;
}

/*
 *                 New Tcl commands start here
 */

/*
  PetalInitInternals
  */
int PetalInitInternalsCmd(ClientData client_data, Tcl_Interp *interp, int argc,
	      char **argv)
{
  /* Just triggers the load of Petal.so and the call to Petal_Init */
  return TCL_OK;
}

/*
  noteIndex [-scale S] [-tonality K] [-mode M] note|midi_pitch
  */
int NoteIndexCmd(ClientData client_data, Tcl_Interp *interp, int argc,
	      char **argv)
{
  int argIdx, notePitch, i;
  uchar *optScale = defaultScale;
  tonality optTonality = defaultTonality;
  harmonicMode optMode = defaultMode;
  

  argIdx = GetStandardOptions(interp, argc, argv,
			      &optScale, &optTonality, &optMode);

  if((argIdx != argc - 1) || (argIdx < 0)) {
    sprintf(gbuf,
	    "Usage : %s [-scale S] [-tonality K] [-mode M] note|midi_pitch",
	    argv[0]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }

  /* Check note|midi_pitch argument */
  if(isdigit(argv[argIdx][0])) { /* That's a midi pitch */
    notePitch = atoi(argv[argIdx]);
  } else if((notePitch = NoteToMidi(argv[argIdx])) < 0) {
    sprintf(gbuf, "%s: wrong argument : %s, must be a midi pitch or a note name", 
	    argv[0], argv[argIdx]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }

  i = PitchIsInScale(optScale, optTonality, optMode, notePitch);
  if(i<0) {
    sprintf(gbuf, "%s: wrong argument : %s isn't in scale",
	    argv[0], argv[argIdx]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }
  sprintf(gbuf, "%u", i);
  Tcl_AppendResult(interp, gbuf, NULL);
  return TCL_OK;

}
/*
  ith [-scale S] [-tonality K] [-mode M] note|midi_pitch offset
*/
int IthCmd(ClientData client_data, Tcl_Interp *interp, int argc,
	      char **argv)
{
#define RESULT_NOTE_NAME 1
#define RESULT_MIDI_PITCH 2
  int notePitch, argIdx, i, offset, resultFormat = RESULT_NOTE_NAME;
  uchar *optScale = defaultScale;
  unsigned int scaleLen;
  tonality optTonality = defaultTonality;
  harmonicMode optMode = defaultMode;

  argIdx = GetStandardOptions(interp, argc, argv,
			      &optScale, &optTonality, &optMode);

  if((argIdx != argc - 2) || (argIdx < 0)) {
    sprintf(gbuf, "Usage : %s [-scale S] [-tonality K] [-mode M] note|midi_pitch offset",
	    argv[0]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }

  /* Check note|midi_pitch argument */
  if(isdigit(argv[argIdx][0])) { /* That's a midi pitch */
    notePitch = atoi(argv[argIdx]);
    resultFormat = RESULT_MIDI_PITCH; /* The result will also be a midi pitch */
  } else if((notePitch = NoteToMidi(argv[argIdx])) < 0) {
    sprintf(gbuf, "%s: wrong argument : %s, must be a midi pitch or a note name", 
	    argv[0], argv[argIdx]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }

  /* Check the offset argument */
  if(!isdigit(argv[argIdx+1][0])) {
    sprintf(gbuf, "%s: wrong argument : %s, must be an integer",
	    argv[0], argv[argIdx+1]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }

  offset = atoi(argv[argIdx+1]);
  scaleLen = strlen(optScale);

  i = PitchIsInScale(optScale, optTonality, optMode, notePitch);
  if(i<0) {
    sprintf(gbuf, "%s: wrong argument : %s isn't in scale",
	    argv[0], argv[argIdx]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }
    
  for(; offset > 0; offset--) {
    notePitch += optScale[i];
    i++; if(i >= scaleLen) i = 0; 
    /* Is this more efficient than i %= scaleLen ? */
  }

  switch(resultFormat) {
  case RESULT_MIDI_PITCH:
    sprintf(gbuf, "%u", notePitch);
    break;
  case RESULT_NOTE_NAME:
    sprintf(gbuf, "%s", MidiToNote(notePitch));
  }

  Tcl_SetResult(interp, gbuf, TCL_STATIC);
  return TCL_OK;
#undef RESULT_NOTE_NAME
#undef RESULT_MIDI_PITCH
}


/*
  isInScale [-scale S] [-tonality K] note|midi_pitch
*/
int IsInScaleCmd(ClientData client_data, Tcl_Interp *interp, int argc,
	      char **argv)
{
  int notePitch, argIdx;
  uchar *optScale = defaultScale;
  tonality optTonality = defaultTonality;

  argIdx = GetStandardOptions(interp, argc, argv,
			      &optScale, &optTonality, NULL);

  if(argIdx < 0) return TCL_ERROR;

  if(isdigit(argv[argIdx][0])) { /* That's a midi pitch */
    notePitch = atoi(argv[argIdx]) % NB_NOTES;
  } else if((notePitch = NoteToMidi(argv[argIdx])) < 0) {
    sprintf(gbuf, "%s wrong argument : %s, must be a midi pitch or a note name", 
	    argv[0], argv[argIdx]);
    Tcl_AppendResult(interp, gbuf, NULL);
    return TCL_ERROR;
  }

  if(PitchIsInScale(optScale, optTonality, 0, notePitch) >= 0) {
      Tcl_SetResult(interp, "1", TCL_STATIC);
      return TCL_OK;
    }

  Tcl_SetResult(interp, "0", TCL_STATIC);
  return TCL_OK;
}


/*
  This function wants the name of a variable containing a scale, ie. a
  list of integers which sum up to 12. It's more practical and
  readable to have a defaultScale set to "scaleMajor" than to
  "2 2 1 2 2 2 1"
*/
char* SetDefaultScale(ClientData clientData,
		      Tcl_Interp *interp,
		      char *name1, char *name2,
		      int flags)
{
  char *buf = Tcl_GetVar2(interp, name1, name2, TCL_GLOBAL_ONLY),
    *errMsg;

  if((errMsg = CheckScale(interp, buf, &defaultScale, scaleAny))) {
    /* Resetting defaultScale to previous value */
    Tcl_SetVar(interp, "defaultScale", defaultScaleName, TCL_GLOBAL_ONLY);
    return errMsg;
  }

  /* Backup scale's name (see above) */
  strncpy(defaultScaleName, buf, BUF_SIZE);

  return NULL;
}


/* Can we do something here to set a SharpsOrFlats flag ? */
/* Not counting foreign note names... */
char* SetDefaultTonality(ClientData clientData,
		    Tcl_Interp *interp,
		    char *name1, char *name2,
		    int flags)
{
  tonality newTonality;
  static tonality prevDefaultTonality;
  char *buf = Tcl_GetVar2(interp, name1, name2, TCL_GLOBAL_ONLY),
    *errMsg;

  if((errMsg = CheckTonality(interp, buf, &newTonality))) {
    /* Restore defaultTonality to previous value */
    sprintf(gbuf, "%u", prevDefaultTonality);
    Tcl_SetVar(interp, "defaultTonality", gbuf, TCL_GLOBAL_ONLY);
    return errMsg;
  }

  prevDefaultTonality = defaultTonality = newTonality;

  return NULL;
}



char* SetDefaultMode(ClientData clientData,
		     Tcl_Interp *interp,
		     char *name1, char *name2,
		     int flags)
{
  harmonicMode mode;
  static harmonicMode prevDefaultMode;
  char *buf = Tcl_GetVar2(interp, name1, name2, TCL_GLOBAL_ONLY),
    *errMsg;

  if((errMsg = CheckMode(interp, buf, &mode))) {
    /* Restore defaultMode to previous value */
    sprintf(gbuf, "%u", prevDefaultMode);
    Tcl_SetVar(interp, "defaultMode", gbuf, TCL_GLOBAL_ONLY);
    return errMsg;
  }

  prevDefaultMode = defaultMode = mode;

  return NULL;
}


int Petal_Init(Tcl_Interp *interp)
{
		 
  int i, j, c;
  
  /* Note names */
  Tcl_SetVar(interp, "noteNames", noteNames, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "noteSharps", noteSharps, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "noteFlats", noteFlats, TCL_GLOBAL_ONLY);

  /* Modes */
  for(i=0; i<NB_MODES; i++) {
    sprintf(gbuf, "%u", modeValues[i]);
    Tcl_SetVar(interp, modeNames[i], gbuf, TCL_GLOBAL_ONLY);
  }

  /* Tonalities */
  for(i=0; i<NB_TONALITIES; i++) {
    sprintf(gbuf, "%u", tonalityNameToTonalityTable[i]);
    Tcl_SetVar(interp, tonalityNames[i], gbuf, TCL_GLOBAL_ONLY);
  }

  /* Scales */
  for(i=0; i<NB_BUILTIN_SCALES; i++) {

    /* I'm proud of this pretty little bit */
    for(c=j=0; j < strlen(builtInScales[i]); j++)
      c += sprintf(gbuf + c, "%u ", builtInScales[i][j]);

    Tcl_SetVar(interp, builtInScaleNames[i], gbuf, TCL_GLOBAL_ONLY);
  }
  /* Various built-in defaults */ 
  Tcl_SetVar(interp, "defaultScale", builtInScaleNames[0], TCL_GLOBAL_ONLY);
  strcpy(defaultScaleName, builtInScaleNames[0]);

  Tcl_SetVar(interp, "defaultTonality", "0", TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "defaultMode", "0", TCL_GLOBAL_ONLY);

  /* ... and tracing of changes to those defaults */
  Tcl_TraceVar(interp, "defaultScale", TCL_TRACE_WRITES|TCL_GLOBAL_ONLY, 
	       SetDefaultScale, NULL);
  Tcl_TraceVar(interp, "defaultTonality", TCL_TRACE_WRITES|TCL_GLOBAL_ONLY,
	       SetDefaultTonality, NULL);
  Tcl_TraceVar(interp, "defaultMode", TCL_TRACE_WRITES|TCL_GLOBAL_ONLY, 
	       SetDefaultMode, NULL);

  Tcl_CreateCommand(interp, "petalInitInternals", PetalInitInternalsCmd, 0, NULL);
  Tcl_CreateCommand(interp, "isInScale", IsInScaleCmd, 0, NULL);
  Tcl_CreateCommand(interp, "ith", IthCmd, 0, NULL);
  Tcl_CreateCommand(interp, "noteIndex", NoteIndexCmd, 0, NULL);

  return Tcl_PkgProvide(interp, "Petal", "0.2");

}
