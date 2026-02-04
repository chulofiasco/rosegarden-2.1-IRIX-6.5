
/* Mild attempt at chord naming */
/* G. Laurent, 09/96 */

#include "Rechord.h"

#ifdef DEBUG
#include <assert.h>
void DumpIntervals(ChordIntervals *chordIntervals)
{
  fprintf(stderr, "Intervals :\nintervals[Second] : %u\n"
          "intervals[Third] : %u\n"
          "intervals[Fourth] : %u\n"
          "intervals[Fifth] : %u\n"
          "intervals[Sixth] : %u\n"
          "intervals[Seventh] : %u\n"
          "intervals[Octave] : %u\n"
          "intervals[Ninth] : %u\n"
          "intervals[Eleventh] : %u\n"
          "intervals[Thirteenth] : %u\n",
          chordIntervals->intervals[Second],
          chordIntervals->intervals[Third], chordIntervals->intervals[Fourth],
          chordIntervals->intervals[Fifth], chordIntervals->intervals[Sixth],
          chordIntervals->intervals[Seventh], chordIntervals->intervals[Octave],
          chordIntervals->intervals[Ninth], chordIntervals->intervals[Eleventh],
          chordIntervals->intervals[Thirteenth]);
  fprintf(stderr, "Complexity : %d\n", chordIntervals->complexity);
}
#endif

void VoiceToNote(NoteVoice voice, char *noteName)
{
  Pitch pitch = voice.pitch % 7;

  Begin("VoiceToNote");

  switch(pitch) {
  case -6: noteName[0] = 'F'; break;
  case -5: noteName[0] = 'G'; break;
  case -4: noteName[0] = 'A'; break;
  case -3: noteName[0] = 'B'; break;
  case -2: noteName[0] = 'C'; break;
  case -1: noteName[0] = 'D'; break;
  case  0: noteName[0] = 'E'; break;
  case  1: noteName[0] = 'F'; break;
  case  2: noteName[0] = 'G'; break;
  case  3: noteName[0] = 'A'; break;
  case  4: noteName[0] = 'B'; break;
  case  5: noteName[0] = 'C'; break;
  case  6: noteName[0] = 'D'; break;
  }
  /* Yes, I could have devised a clever math formula to do the above,
     but it would have been much less readable (waaaay much less) */
  if(voice.modifiers & ModSharp) noteName[1] = '#';
  else if(voice.modifiers & ModFlat) noteName[1] = 'b';
  else noteName[1] = '\0';

  End;
}

int SemiTonesBetween(NoteVoice voice1, NoteVoice voice2) {
  static int semiTonesPattern[] = { 1, 2, 2, 2, 1, 2, 2 };
  /* E->F, F->G, G->A, A->B, B->C, C->D, D->E */
  int semiTones = 0;
  Pitch p, pitch1 = voice1.pitch, pitch2 = voice2.pitch;
  NoteVoice *highVoice, *lowVoice;

  Begin("SemiTonesBetween");

  if(pitch1 == pitch2) return 0;

  if(pitch1 < pitch2) {
    lowVoice = &voice1;
    highVoice = &voice2;
  } else { 
    lowVoice = &voice2;
    highVoice = &voice1;
  }

  pitch1 += 77; pitch2 += 77; /* To ensure pitches are above 0 */

  while(pitch2 < pitch1) pitch2 += 7;
  /* so the loop below always makes sense */

  for(p = pitch1; p < pitch2; p++)
    semiTones += semiTonesPattern[p % 7];

  if(lowVoice->modifiers & ModSharp) semiTones--;
  else if(lowVoice->modifiers & ModFlat) semiTones++;

  if(highVoice->modifiers & ModSharp) semiTones++;
  else if(highVoice->modifiers & ModFlat) semiTones--;

  Return(semiTones);
}

void FlagInterval(ChordIntervals *chordIntervals, int semiTones)
{
  Begin("FlagInterval");

  semiTones %= 24; /* Intervals of more than two octaves aren't relevant */

  switch(semiTones) {
  case 0: break;
  case 1: chordIntervals->intervals[Second] |= Minor; break;
  case 2: chordIntervals->intervals[Second] |= Major; break;

  case 3: chordIntervals->intervals[Third] |= Minor; break;
  case 4: chordIntervals->intervals[Third] |= Major; break;

  case 5: chordIntervals->intervals[Fourth] |= Perfect; break;

  case 6: chordIntervals->intervals[Fifth] |= Diminished; break;
  case 7: chordIntervals->intervals[Fifth] |= Perfect; break;
  case 8: chordIntervals->intervals[Fifth] |= Augmented; break;

  case 9: chordIntervals->intervals[Sixth] |= Major; break;

  case 10: chordIntervals->intervals[Seventh] |= Minor; break;
  case 11: chordIntervals->intervals[Seventh] |= Major; break;

  case 12: chordIntervals->intervals[Octave] |= Perfect; break;

  case 13: chordIntervals->intervals[Ninth] |= Minor; break;
  case 14: chordIntervals->intervals[Ninth] |= Major; break;

  case 15: chordIntervals->intervals[Ninth] |= Augmented; break;

    /* We don't have recourse to such pitiful trick anymore:

  case 15: chordIntervals->intervals[Third] |= Minor; break; 

  Now we can play 'Foxy Lady'.
  */

  case 16: chordIntervals->intervals[Third] |= Major; break;

  case 17: chordIntervals->intervals[Eleventh] |= Perfect; break;
  case 18: chordIntervals->intervals[Eleventh] |= Augmented; break;

  case 19: chordIntervals->intervals[Fifth] |= Perfect; break;

  case 20: chordIntervals->intervals[Fifth] |= Augmented; break;

    /* I'm not too sure of this last one. Minor thirteenth aren't very
       common (never saw any), but forcing it into a augmented 5th
       doesn't seem quite right either...
  case 20: chordIntervals->intervals[Thirteenth] |= Minor; break;
  */
  case 21: chordIntervals->intervals[Thirteenth] |= Major; break;
  case 22: chordIntervals->intervals[Seventh] |= Minor; break;
  case 23: chordIntervals->intervals[Seventh] |= Major;
  }

  /* Again, a bunch of 'if's would have been more efficient but not as
     readable */

  End;
}

Boolean HasMoreThanOneFlagSet(short interval)
{
  Boolean oneFlagAlreadyFound = False;
  int j;

  Begin("HasMoreThanOneFlagSet");

  if(interval == NonExistent) Return(False);

  for(j = Diminished; j <= Augmented; j <<= 1)
    if(interval & j)
      if(oneFlagAlreadyFound) Return(True);
      else oneFlagAlreadyFound = True;

  Return(False);
}

void EvaluateIntervalsComplexity(ChordIntervals *chordIntervals)
{
  int localComplexity = 0, i;
  short interval, *intervals = chordIntervals->intervals;

  Begin("EvaluateIntervalsComplexity");
  chordIntervals->complexity = 0;

  /* And now for something completely haphazard.
     That is, this 'complexity' stuff is purely arbitrary, and will
     most certainly need some tuning as it is tested in 'real life'
     situations. "Nose-metrics" as we say in french. */

  /* First, what increases the complexity */
  interval = intervals[Second];
  if(interval & Minor) localComplexity += BIG;
  else if(interval & Major) localComplexity += SMALL;


  interval = intervals[Third];
  if((interval & Minor) || (interval == NonExistent))
    localComplexity += VERY_SMALL;

  if(intervals[Fourth] != NonExistent) localComplexity += SMALL;

  interval = intervals[Fifth];
  if(interval == NonExistent) localComplexity += AVERAGE;
  else if(interval & (Diminished | Augmented)) localComplexity += AVERAGE;

  /* Increase complexity if we have a 6th or a 7th... 13th */
  for(i = Sixth; i <= Thirteenth; i++) {
    if(i == Octave) continue;
    else if(intervals[i] != NonExistent) localComplexity += SMALL;
  }

  /* Enormous complexity increase for members that are checked more than once,
     for instance a perfect 5th with a dimished 5th */
  for(i = Second; i <= Thirteenth; i++)
    if(HasMoreThanOneFlagSet(intervals[i])) localComplexity += VERY_BIG * VERY_BIG;

  for(i = Ninth; i <= Thirteenth; i++)
    if((intervals[i] != NonExistent) &&
       (intervals[i - Octave -1] != NonExistent) &&
       (intervals[i - Octave -1] != intervals[i]))
      /* Check if the 'same' interval one octave below is different
         (ie the 2nd for the 9th, the 4th for the 11th, the 6th for
         the 13th */
      localComplexity += VERY_BIG;

  /* Now what decreases the complexity */

  if(intervals[Third] != NonExistent) localComplexity -= AVERAGE;
  if(intervals[Fifth] != NonExistent) localComplexity -= BIG;
  if(intervals[Octave] != NonExistent) localComplexity -= VERY_BIG;

  chordIntervals->complexity = localComplexity;
#ifdef DEBUG
  DumpIntervals(chordIntervals);
#endif

  End;
}

ChordIntervals ChordToIntervals(Chord chord, int root)
{
  int i, semiTones;
  ChordPart chordPart = chord.chord;
  NoteVoice *voice, *rootVoice = &(chord.chord.voices[root]);
  Pitch pitch;
  ChordIntervals chordIntervals;

  Begin("ChordToIntervals");

  memset(chordIntervals.intervals, 0, MAX_INTERVALS_VALUE*sizeof(short));

  if(root >= chordPart.voice_count) {
    fprintf(stderr, "Oops : root : %u, voice_count : %u\n",
            root, chordPart.voice_count);
    Error("Root is out of bounds");
  }

  for(i=root + 1; i < chordPart.voice_count; i++) {
    /* Deal with the notes above the root... */
    voice = &(chordPart.voices[i]);
    pitch = voice->pitch;

    semiTones = SemiTonesBetween(*rootVoice, *voice);

    FlagInterval(&chordIntervals, semiTones);
  }

  if(root >= 2) { /* if root = 1, voices[0] is the alt. bass */
    for(i=root - 1; i > 0; i--) {
      /* ... then those below, if there are more than 2 of 'em. Note
         that we *DO NOT* check for the lowest one, since if we get
         here it's alreay kept as being the strAltBass */
      voice = &(chordPart.voices[i]);
      pitch = voice->pitch;

      semiTones = SemiTonesBetween(*rootVoice, *voice);

      FlagInterval(&chordIntervals, semiTones);
    }
  }

  EvaluateIntervalsComplexity(&chordIntervals);

  chordIntervals.voiceCount = chordPart.voice_count;

  Return(chordIntervals);
}

Boolean CheckForDim7(ChordIntervals chordIntervals)
{
  Begin("CheckForDim7");

  if((chordIntervals.intervals[Second] != NonExistent) ||
     (chordIntervals.intervals[Fourth] != NonExistent) ||
     (chordIntervals.intervals[Seventh] != NonExistent) ||
     (chordIntervals.intervals[Ninth] != NonExistent) ||
     (chordIntervals.intervals[Eleventh] != NonExistent) ||
     (chordIntervals.intervals[Thirteenth] != NonExistent))
    Return(chordIsDim7 = False);
  /* Maybe the 13th could be tolerated here... */

  if((chordIntervals.intervals[Third] & Minor) &&
     (chordIntervals.intervals[Fifth] & Diminished) &&
     (chordIntervals.intervals[Sixth] & Major)) {
    strcpy(memberThird.s, "dim");
    Return(chordIsDim7 = True);
  }
  Return(chordIsDim7 = False);
}


#define FOUND_NONE 0
#define FOUND_NATURAL  1
#define FOUND_ALTERED  2

/* None of the Do<x> functions will zero the string they're dealing
   with, this is done in Rechord1() */

int DoThird(ChordIntervals chordIntervals)
{
  int rc = FOUND_ALTERED;

  Begin("DoThird");

  if(chordIntervals.intervals[Second] & Major)
    strcpy(memberThird.s, "sus2");
  else if(chordIntervals.intervals[Fourth] & Perfect)
    strcpy(memberThird.s, "sus4");
  else if(chordIntervals.intervals[Third] & Minor)
    strcpy(memberThird.s, "m");
  else if(chordIntervals.intervals[Third] & Major)
    rc = FOUND_NATURAL;

  else if(chordIntervals.intervals[Third] == NonExistent) {
    rc = FOUND_NONE;
    if((chordIntervals.intervals[Fifth] == Perfect)
          && (chordIntervals.intervals[Sixth] == NonExistent)
          && (chordIntervals.intervals[Seventh] == NonExistent)
          && (chordIntervals.intervals[Ninth] == NonExistent)
          && (chordIntervals.intervals[Eleventh] == NonExistent)
          && (chordIntervals.intervals[Thirteenth] == NonExistent)
       ) /* Check for a power chord */
      strcpy(memberThird.s, "5");
    else if(chordIntervals.voiceCount > 2) strcpy(memberThird.s, "(no 3rd)");
  }

  Return(rc);
}

int DoFifth(ChordIntervals chordIntervals)
{
  int rc = FOUND_NATURAL;

  Begin("DoFifth");

  if(chordIntervals.intervals[Fifth] == NonExistent) {
    if(chordIntervals.voiceCount > 2) strcpy(memberFifth.s, "(no 5th)");
    Return(FOUND_NONE);
  }

  if(chordIntervals.intervals[Fifth] != Major) {
    rc = FOUND_ALTERED;
    if(chordIntervals.intervals[Fifth] == Diminished)
      strcpy(memberFifth.s, "-5");
    else if(chordIntervals.intervals[Fifth] == Augmented)
      strcpy(memberFifth.s, "aug");
  }

  Return(rc);
}

int DoSixth(ChordIntervals chordIntervals)
{
  Begin("DoSixth");

  if(chordIntervals.intervals[Sixth]) {
    strcpy(memberSixth.s, "6");
    Return(FOUND_NATURAL);
  }

  Return(FOUND_NONE);
}

/* The general idea for DoSeventh, DoNinth and DoEleventh is :

   If nothing found, return (so far so good).

   If something fishy (altered and natural at the same time), put some
   funny things in the member string and return an error. This will
   eventually go away

   If altered note is found, put it in the string and return
   accordingly

   If natural is found, we have to take in account that an higher note
   may imply the one we're looking at. e.g. 'C9' means C major chord
   plus the 7th, Bb and the ninth D. Similarly, 'C13' means C plus
   7th, 9th, 11th and of course 13th.

   */

int DoSeventh(ChordIntervals chordIntervals)
{
  int rc = FOUND_ALTERED;

  Begin("DoSeventh");

  if(chordIntervals.intervals[Seventh] == NonExistent)
    Return(FOUND_NONE);

  if(chordIntervals.intervals[Seventh] & Major) {
    strcpy(memberSeventh.s, "maj");
    rc = FOUND_NATURAL;
  }

  if((chordIntervals.intervals[Ninth] != Major) && 
     (chordIntervals.intervals[Eleventh] != Perfect) &&
     (chordIntervals.intervals[Thirteenth] != Major))
    strcat(memberSeventh.s, "7");

  Return(rc);
}

int DoNinth(ChordIntervals chordIntervals)
{
  int rc = FOUND_ALTERED;
  Begin("DoNinth");

  if(chordIntervals.intervals[Ninth] == NonExistent)
    Return(FOUND_NONE);

  if((chordIntervals.intervals[Seventh] == NonExistent) &&
     (chordIntervals.intervals[Eleventh] == NonExistent))
    strcpy(memberNinth.s, "add");

  if(chordIntervals.intervals[Ninth] & Major) {
    rc = FOUND_NATURAL; /* There's a 9th alright... */
    if((chordIntervals.intervals[Eleventh] != Perfect) &&
     (chordIntervals.intervals[Thirteenth] != Major))
    strcat(memberNinth.s, "9"); /* But it must be shown only if no 11th nor 13th */
  }

  else if(chordIntervals.intervals[Ninth] & Minor)
    strcat(memberNinth.s, "b9");
  else if(chordIntervals.intervals[Ninth] & Augmented)
    strcat(memberNinth.s, "b9");
   
  Return(rc);
}

int DoEleventh(ChordIntervals chordIntervals)
{
  int rc = FOUND_ALTERED;
  Begin("DoEleventh");

  if(chordIntervals.intervals[Eleventh] == NonExistent)
    Return(FOUND_NONE);

  if((chordIntervals.intervals[Thirteenth] == NonExistent)
    && (chordIntervals.intervals[Ninth] == NonExistent))
    /* If no 9th and no 13th, the 11th is 'added' */
    strcpy(memberEleventh.s, "add");

  if(chordIntervals.intervals[Eleventh] & Perfect) {
    rc = FOUND_NATURAL;
    if(chordIntervals.intervals[Thirteenth] != Major) 
      strcat(memberEleventh.s, "11");
  }

  else if(chordIntervals.intervals[Eleventh] & Augmented)
    strcpy(memberEleventh.s, "#11");

  Return(rc);
}

int DoThirteenth(ChordIntervals chordIntervals)
{
  int rc = FOUND_NATURAL;
  Begin("DoThirteenth");

  if(chordIntervals.intervals[Thirteenth] == NonExistent)
    Return(FOUND_NONE);

  if((chordIntervals.intervals[Eleventh] == NonExistent) &&
      (chordIntervals.intervals[Ninth] == NonExistent))
    /* If no 11th nor 9th, the 13th is 'added' */
    strcpy(memberThirteenth.s, "add");

  if(chordIntervals.intervals[Thirteenth] & Minor) {
    strcat(memberThirteenth.s, "b13");
    rc = FOUND_ALTERED;
    /* I'll probably never fall on this one */
  }
  else if(chordIntervals.intervals[Thirteenth] & Major)
    strcat(memberThirteenth.s, "13");

  Return(rc);
}

#define NBFUNC 7
#define SET_MEMBER_WEIGHT(X,Y) X.w = Y


int CompareMembers(const void* m1, const void* m2)
{
  WeightedString *member1, *member2;

  Begin("CompareMembers");
#ifdef DEBUG2
  assert(m1 && m2);
#endif
  member1 = (WeightedString *)m1; /* or, "assignment discards `const' ..." */
  member2 = (WeightedString *)m2;
#ifdef DEBUG2
  assert(member1 && member2);

  fprintf(stderr, "member1 : '%s' (weight %u), member2 : '%s', (weight %u)\n",
          member1->s, member1->w,
          member2->s, member2->w);
#endif
  Return((int)(member1->w - member2->w));
}

void WeighChordMembers(WeightedString strChordMembers[NBFUNC],
                          int chordMembers[NBFUNC])
{
  Begin("WeighStrChordMembers");

  /* Now we "weigh" every string member */
  if(chordMembers[member3RD] == FOUND_NONE) {
    if(memberThird.s[0] == '5') {
      SET_MEMBER_WEIGHT(memberThird, 0);
      End; /* It's a power chord, no more possible chord members */
    } else {
      SET_MEMBER_WEIGHT(memberThird, 30);
    }
  }

  if(chordMembers[member3RD] == FOUND_ALTERED) {
    if(memberThird.s[0] == 'm')
      SET_MEMBER_WEIGHT(memberThird, 0);
    else  /* 'susX' */
      SET_MEMBER_WEIGHT(memberThird, 20); 
  }

  if(chordMembers[member5TH] == FOUND_ALTERED)
    SET_MEMBER_WEIGHT(memberFifth, 13);
  else if(chordMembers[member5TH] == FOUND_NONE)
    SET_MEMBER_WEIGHT(memberFifth, 30);
    
  if(chordMembers[member6TH] == FOUND_NATURAL)
    SET_MEMBER_WEIGHT(memberSixth, 5) ;

  if(chordMembers[member13TH] == FOUND_NATURAL)
    SET_MEMBER_WEIGHT(memberThirteenth, 5);
  else if(chordMembers[member13TH] == FOUND_ALTERED)
    SET_MEMBER_WEIGHT(memberThirteenth, 10);

  if(chordMembers[member11TH] == FOUND_NATURAL)
    SET_MEMBER_WEIGHT(memberEleventh, 5);
  else if(chordMembers[member11TH] == FOUND_ALTERED)
    SET_MEMBER_WEIGHT(memberEleventh, 11);

  if(chordMembers[member9TH] == FOUND_NATURAL)
    SET_MEMBER_WEIGHT(memberNinth, 5);
  else if(chordMembers[member9TH] == FOUND_ALTERED)
    SET_MEMBER_WEIGHT(memberNinth, 12);

  End;
}

void DoExtension(ChordIntervals chordIntervals)
{
  /*  static int (*(DoOneExtension[NBFUNC]))(ChordIntervals) = { &DoThird,
      &DoFifth,
      &DoSixth,
      &DoSeventh,
      &DoNinth,
      &DoEleventh,
      &DoThirteenth };
      */

  static ExtensionFunc DoOneExtension[NBFUNC] = { &DoThird,
                                                  &DoFifth,
                                                  &DoSixth,
                                                  &DoSeventh,
                                                  &DoNinth,
                                                  &DoEleventh,
                                                  &DoThirteenth };

  WeightedString strChordMembers[NBFUNC];
  static int chordMembers[NBFUNC];

  short i;

  Begin("DoExtension");

  strChordMembers[0] = memberThird;
  strChordMembers[1] = memberFifth;
  strChordMembers[2] = memberSixth;
  strChordMembers[3] = memberSeventh;
  strChordMembers[4] = memberNinth;
  strChordMembers[5] = memberEleventh;
  strChordMembers[6] = memberThirteenth;
  
  /* Apply all the Do<x> functions */
  for(i=0; i < NBFUNC; i++)
    chordMembers[i] = (*(DoOneExtension[i]))(chordIntervals);
  
    /* Check that at least one of the members strings is not null */
  for(i = 0; (i < NBFUNC) && (strChordMembers[i].s[0] == 0); i++) {
#ifdef DEBUG2
    fprintf(stderr, "DoExtension : strChordMember[%u] = %s\n",
            i, strChordMembers[i].s);
#endif
  }

  if(i < NBFUNC) {
    WeighChordMembers(strChordMembers, chordMembers);
    qsort(strChordMembers, NBFUNC, sizeof(WeightedString), &CompareMembers);
  } else {
#ifdef DEBUG
    fprintf(stderr, "DoExtension : skipping qsort, all members are null\n");
#endif
  }

  /* Concatenate all the member strings */
  for(i=0; i < NBFUNC; i++) {
    if(strChordMembers[i].s[0])
      strcat(strExtension, strChordMembers[i].s);
  }

  End;
}


int Rechord1(char *chordName, ChordIntervals *chordIntervals)
{
  Begin("Rechord1");

  memset(memberThird.s, 0, SIZEOF_MEMBER);
  memset(memberFifth.s, 0, SIZEOF_MEMBER);
  memset(memberSixth.s, 0, SIZEOF_MEMBER);
  memset(memberSeventh.s, 0, SIZEOF_MEMBER);
  memset(memberNinth.s, 0, SIZEOF_MEMBER);
  memset(memberEleventh.s, 0, SIZEOF_MEMBER);
  memset(memberThirteenth.s, 0, SIZEOF_MEMBER);

  memset(strExtension, 0, SIZEOF_EXTENSION);
  

  /* Note : We DO NOT reset strAltBass */
  /* because we want to keep it from a */
  /* chord name to the next     */

  /* First, "canonicalize" the intervals, which shouldn't be needed
     if they've been generated by ChordToIntervals, but we're not
     supposed to know that */
  if(chordIntervals->intervals[Fourth] & Augmented) {
    chordIntervals->intervals[Fifth] |= Diminished; /* augmented 4th = diminished 5th */
    chordIntervals->intervals[Fourth] = NonExistent;
  }

  if(chordIntervals->intervals[Sixth] & Minor) {
    chordIntervals->intervals[Fifth] |= Augmented; /* minor 6th = augmented 5th */
    chordIntervals->intervals[Sixth] = NonExistent;
  }

  if(chordIntervals->intervals[Seventh] & Diminished) {
    chordIntervals->intervals[Sixth] |= Major; /* diminished 7th = major 6th */
    chordIntervals->intervals[Seventh] = NonExistent;
  }

  /* Second, a few heuristics to solve invertions. What we do here is
     simply bump an interval an octave higher if it "conflicts" with
     another.

     Note that in doing so, since we OR the interval type with its
     alter ego on octave above, we may still end up with a similar
     "conflict", except it will be found an octave higher. The
     'interval complexity' stuff will catch it just the same and
     Rechord won't insist. */

  if(chordIntervals->intervals[Third] != NonExistent) {  
    /* A Third and a Second... */
    if(chordIntervals->intervals[Second] & Major) {
      chordIntervals->intervals[Ninth] |= chordIntervals->intervals[Second];
      chordIntervals->intervals[Second] = NonExistent;
    }
    /* ...or a Fourth */
    if(chordIntervals->intervals[Fourth] & Perfect) {
      chordIntervals->intervals[Eleventh] |= chordIntervals->intervals[Fourth];
      chordIntervals->intervals[Fourth] = NonExistent;
    }
  }

  if((chordIntervals->intervals[Sixth] != NonExistent) &&
     (chordIntervals->intervals[Seventh] != NonExistent)) { /* A 6th and a 7th */
      chordIntervals->intervals[Thirteenth] |= chordIntervals->intervals[Sixth];
      chordIntervals->intervals[Sixth] = NonExistent;
  }

  /* We first take care of the pesky dim7, which greatly simplifies
     the other functions */

  if(!CheckForDim7(*chordIntervals))
    DoExtension(*chordIntervals);

  if(!chordName) {
    chordName = (char*)malloc((strlen(strTonic)
                               + strlen(strExtension)
                               + strlen(strAltBass)
                               + 3));
    /* One for a possible '/', one for '\0' and one for the road */
  }

  chordName[0] = 0;
  strcat(chordName, strTonic);

  if(strExtension[0]) strcat(chordName, strExtension);

  if(strAltBass[0])
    sprintf(chordName + strlen(chordName), "/%s", strAltBass);
#ifdef DEBUG
  fprintf(stderr, "\nRechord1 : found chord name %s, len : %u\n", chordName, strlen(chordName));
  DumpIntervals(chordIntervals);
#endif
  Return(chordIntervals->complexity);
}


#define HOLD_THIS_CHORD_NAME t = testChordName; \
testChordName = holdShortest; holdShortest = t;

char* Rechord(Chord chord) {
  char *chordName;
  static char *testChordName = NULL, *holdShortest = NULL;
  char *t;
  int minComplex = 256, tpComplex, root;
  ChordIntervals chordIntervals;

  Begin("Rechord");

  if(!testChordName && !holdShortest) {
    /* Some inits to perform on 1st call only */
    testChordName = malloc(256); memset(testChordName, 0, 256);
    holdShortest = malloc(256); memset(holdShortest, 0, 256);
  }

  memset(strTonic, 0, SIZEOF_TONIC); /* We don't really need that */

  memset(strAltBass, 0, SIZEOF_ALTBASS);
  /* Other strings are reset in Rechord1, but this one we want to keep */

  /* No need to do that : it's done in ChordToIntervals */
  /* memset(chordIntervals.intervals, NonExistent, MAX_INTERVALS_VALUE*sizeof(short)); */


  /* Try the lowest note as being the root first... */
  VoiceToNote(chord.chord.voices[0], strTonic);
  chordIntervals = ChordToIntervals(chord, 0);
  minComplex = tpComplex = chordIntervals.complexity;
  if(tpComplex < MAX_TOLERABLE_COMPLEXITY)
    Rechord1(testChordName, &chordIntervals);
  else { testChordName[0] = '\0'; }

  if((tpComplex <= SMALL) ||
     ((chord.chord.voice_count == 4) && (chordIsDim7 == True))) {
    /* Plain major/minor chord, or dim : no need to search any further */
    chordName = malloc(strlen(testChordName));
    strcpy(chordName, testChordName);
    Return(chordName);
  }

  /* Save testChordName for possible final result */
  HOLD_THIS_CHORD_NAME;

  if(chord.chord.voice_count > 1) {
    /* ...Then the 2nd lowest, lowest one being alt. bass */
    strcpy(strAltBass, strTonic);
    VoiceToNote(chord.chord.voices[1], strTonic);
    chordIntervals = ChordToIntervals(chord, 1);
    tpComplex = chordIntervals.complexity;
    if(tpComplex < MAX_TOLERABLE_COMPLEXITY)
      Rechord1(testChordName, &chordIntervals);
    else { testChordName[0] = '\0'; }

    if((tpComplex <= SMALL) ||
       ((chord.chord.voice_count == 5) && (chordIsDim7 == True))) {
      /* Same test as above... */
      chordName = malloc(strlen(testChordName));
      strcpy(chordName, testChordName);
      Return(chordName);
    }

    if(tpComplex < minComplex + SMALL) { /* Add a penalty for alt. bass */
      minComplex = tpComplex;
      HOLD_THIS_CHORD_NAME;
#ifdef DEBUG
      fprintf(stderr, "\nHolding_1 chord name : %s, len : %u\n", holdShortest, minComplex);
#endif
    }
    /* From here we try every possible voice as being the root. Note
       that the strAltBass is kept. */

    for(root = 2; root < chord.chord.voice_count; root++) {
      VoiceToNote(chord.chord.voices[root], strTonic);
      chordIntervals = ChordToIntervals(chord, root);
      tpComplex = chordIntervals.complexity; 
      if(tpComplex < MAX_TOLERABLE_COMPLEXITY) {
        Rechord1(testChordName, &chordIntervals);
        if(tpComplex < minComplex) {
          minComplex = tpComplex;
          HOLD_THIS_CHORD_NAME;
#ifdef DEBUG
          fprintf(stderr, "\nHolding_2 chord name : %s, len : %u\n", holdShortest, minComplex);
#endif
        }
      }
    }


  }
  else {
    /* There's only one note in the chord, use lower case to name it */
    holdShortest[0] = (char)tolower((int)holdShortest[0]);
  }

  chordName = malloc(strlen(holdShortest));
  strcpy(chordName, holdShortest);
  Return(chordName);

}

