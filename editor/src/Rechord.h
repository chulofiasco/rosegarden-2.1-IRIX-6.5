#include "General.h"
#include "Tags.h"
#include "Notes.h"
#include "Classes.h"

#define MAX_INTERVALS_VALUE 12
typedef enum { 
  Second, /* 0 */
  Third,  /* 1 */
  Fourth, /* 2 */
  Fifth,  /* 3 */
  Sixth,  /* 4 */
  Seventh,/* 5 */
  Octave, /* 6 */
  Ninth,  /* 7 */
  Eleventh = 9,
  Thirteenth = 11
} Interval;

  enum { member3RD, member5TH, member6TH,
         member7TH,
         member9TH, member11TH, member13TH }; /* We really have a shitload
                                                 of those flashy enums all
                                                 throughout [DR]echord,
                                                 haven't we ? */

typedef enum {
  NonExistent = 0,
  Diminished = 1<<0,
  Minor = 1<<1,
  /* We don't need 2 seperate values for those, since they can't conflict */
  Major = 1<<2, Perfect = 1<<2,
  Augmented = 1<<3
} IntervalType;

typedef struct _ChordIntervals {
  short intervals[MAX_INTERVALS_VALUE];
  int complexity;
  unsigned int voiceCount;
} ChordIntervals;

typedef int (*ExtensionFunc)(ChordIntervals);

typedef struct _WeightedString {
  char *s;
  short w;
} WeightedString;

#define SIZEOF_TONIC 3
#define SIZEOF_MEMBER 15
#define MEMBER_LAST_ELEMENT SIZEOF_MEMBER - 1
#define SIZEOF_EXTENSION 40
#define SIZEOF_ALTBASS 5
static char strTonic[SIZEOF_TONIC];
static char strThird[SIZEOF_MEMBER];
static char strFifth[SIZEOF_MEMBER];
static char strSixth[SIZEOF_MEMBER];
static char strSeventh[SIZEOF_MEMBER];
static char strNinth[SIZEOF_MEMBER];
static char strEleventh[SIZEOF_MEMBER];
static char strThirteenth[SIZEOF_MEMBER];

static WeightedString memberThird = { strThird, 0 };
static WeightedString memberFifth = { strFifth, 0 };
static WeightedString memberSixth = { strSixth, 0 };
static WeightedString memberSeventh = { strSeventh, 0 };
static WeightedString memberNinth = { strNinth, 0 };
static WeightedString memberEleventh = { strEleventh, 0 };
static WeightedString memberThirteenth = { strThirteenth, 0 };
static char strExtension[SIZEOF_EXTENSION];
static char strAltBass[SIZEOF_ALTBASS];

static Boolean chordIsDim7 = False;

#define VERY_SMALL 1
#define SMALL 5
#define AVERAGE 10
#define BIG 20
#define VERY_BIG 30
#define MAX_TOLERABLE_COMPLEXITY 100 /* It isn't worth trying to find a name
                                        for a set of intervals which complexity
                                        is above this */

