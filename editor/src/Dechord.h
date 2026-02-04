
/*
   Dechord -- analyse chord name and generate chord

   This file by Guillaume Laurent, glaurent@worldnet.fr
   Copyright 1996 Guillaume Laurent

   Used in Rosegarden under the same redistribution
   rights as the rest of the Rosegarden source
*/

#ifndef _DECHORD_H_
#define _DECHORD_H_

#include "General.h"
#include "Tags.h"
#include "Notes.h"
#include "Classes.h"
#include "MidiIn.h"
#include "MidiOut.h"
#ifdef USE_POSIX_REGEXP
#include <regex.h>
#else
#include "Regexp.h" /* H. Spencer's library */
#endif
#include "Menu.h"

/* convenient const. declarations */

#define UseFlats 0
#define UseSharps 1

#define MAXNOTES 12  /* 12 notes in the chromatic scale */
#define SCALELENGTH 8 /* 8 notes in a 'regular' scale */
#define CHORDLENGTH 24 /* a chord has at most 24 semitones between lowest and
			  highest note */
#define NOTELENGTH 4 /* length of a string representing a note, including */
                     /* trailing \0 */

#define COMPLETE 1 /* return code meaning no further analysis of a 
		      chord is needed */
#define UNCOMPLETE 0 /* guess what this means :-) */

/* Some intervals, expressed in semi-tones */

#define ROOT 0

#define MINOR_SECOND 1
#define MAJOR_SECOND 2

#define MINOR_THIRD 3
#define MAJOR_THIRD 4

#define FOURTH 5

#define DIM_FIFTH 6
#define FIFTH 7
#define AUG_FIFTH 8

#define SIXTH 9

#define DIM_SEVENTH 9
#define MINOR_SEVENTH 10
#define MAJOR_SEVENTH 11

#define DIM_NINTH 13
#define NINTH 14
#define AUG_NINTH 15

#define DIM_ELEVENTH 16
#define ELEVENTH 17
#define AUG_ELEVENTH 18

#define DIM_THIRTEENTH 20
#define THIRTEENTH 21
#define AUG_THIRTEENTH 22

typedef enum {
  Root,
  MinorSecond, MajorSecond,
  MinorThird, MajorThird,
  PerfectFourth,
  DiminishedFifth, PerfectFifth, AugmentedFifth,
  MajorSixth,
  DiminishedSeventh = 9, MinorSeventh, MajorSeventh,
  MinorNinth, MajorNinth, AugmentedNinth,
  DiminishedEleventh, PerfectEleventh, AugmentedEleventh,
  MinorThirteenth, MajorThirteenth, AugmentedThirteenth
} dechordIntervals;

/* Regexps used within the program */

#define GET_ROOT "^[A-Ga-g](#|b)?"

#define IS_MINOR "^m(.([^j]|$)|$)"
#define NO_x "\\(no ([0-9]+)(rd|th)\\)"
#define GET_SUS "sus[24]?"
#define IS_DIM "dim$"
#define IS_AUG "aug$|\\+$" /* double escape needed for correct compilation : */
/* one for the C compiler, one for the regexp compiler */

/* The next regexps are defined considering that the root is removed
 from the string. No probs with "Ab5" for ex. It will be treated as an
 Ab power chord, not as an A chord with a flattened 5th */

#define IS_POWC "[^-+b#]5$"

#define GET_7TH "j?7|j"
#define GET_ADD "d(9|11|13)+"
#define GET_ALT5 "[-+#b]5|(-$)" 
#define GET_6 "6"
#define GET_ALT9 "[-+#b]?9"
#define GET_ALT11 "[-+#b]?11"
#define GET_ALT13 "[-+#b]?13"

#define GET_ALTBASS "/([A-G]([#b]?))$"

#endif

