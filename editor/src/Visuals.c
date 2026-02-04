
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Visual record setup functions */


/*
   For many of the Visual lists in this file, the order in which the
   entries appear _is_ important.  Frequently, other functions simply
   reference these by index, and would begin to get things wrong if
   you changed the ordering.  If you must do so, check very carefully
   for references in other files before you do.
*/

/* {{{ Includes */

#include "General.h"
#include "Visuals.h"
#include "Yawn.h"

#include "rose.xbm"
#include "rose_mask.xbm"
#include "grey.xbm"
#include "lightgrey.xbm"
#include "up.xbm"
#include "down.xbm"

#include "note_bodyfilled.xbm"
#include "note_bodyempty.xbm"

#include "dot.xbm"

#include "tail_up_1.xbm"
#include "tail_up_2.xbm"
#include "tail_up_3.xbm"
#include "tail_up_4.xbm"

#include "tail_down_1.xbm"
#include "tail_down_2.xbm"
#include "tail_down_3.xbm"
#include "tail_down_4.xbm"

#include "rest_hemidemisemi.xbm"
#include "rest_demisemi.xbm"
#include "rest_semiquaver.xbm"
#include "rest_quaver.xbm"
#include "rest_crotchet.xbm"
#include "rest_minim.xbm"
#include "rest_semibreve.xbm"
#include "rest_breve.xbm"

#include "notemod_sharp.xbm"
#include "notemod_flat.xbm"
#include "notemod_natural.xbm"

#include "chordmod_dot.xbm"
#include "chordmod_accent.xbm"
#include "chordmod_legato.xbm"
#include "chordmod_sfz.xbm"
#include "chordmod_rfz.xbm"
#include "chordmod_trill.xbm"
#include "chordmod_turn.xbm"
#include "chordmod_pause.xbm"

#include "clef_treble.xbm"
#include "clef_tenor.xbm"
#include "clef_alto.xbm"
#include "clef_bass.xbm"

/* }}} */

/* {{{ Basic definitions */

Dimension roseHeight;

Pixmap roseMap         = 0;
Pixmap roseMaskMap     = 0;
Pixmap greyMap         = 0;
Pixmap lightGreyMap    = 0;
Pixmap noteDotMap      = 0;
Pixmap upMap           = 0;
Pixmap downMap         = 0;

Pixmap tailDownMap[4]  = { 0, 0, 0, 0 };
Pixmap tailUpMap[4]    = { 0, 0, 0, 0 };

/* }}} */
/* {{{ Notes */

NoteVisualCompound noteVisuals[] = {
  {
    { Hemidemisemiquaver, "Hemidemisemiquaver", 0, True,  False, 4, 1 },
    { Hemidemisemiquaver, "Dotted hemidemisemiquaver",
	                                        0, True,  True,  4, 1 },
  },{
    { Demisemiquaver, "Demisemiquaver",         0, True,  False, 3, 1 },
    { Demisemiquaver, "Dotted demisemiquaver",  0, True,  True,  3, 1 },
  },{  
    { Semiquaver, "Semiquaver",                 0, True,  False, 2, 1 },
    { Semiquaver, "Dotted semiquaver",          0, True,  True,  2, 1 },
  },{
    { Quaver, "Quaver",                         0, True,  False, 1, 4 },
    { Quaver, "Dotted quaver",                  0, True,  True,  1, 4 },
  },{
    { Crotchet, "Crotchet",                     0, True,  False, 0, 12 },
    { Crotchet, "Dotted crotchet",              0, True,  True,  0, 12 },
  },{
    { Minim, "Minim",                           0, True,  False, 0, 24 },
    { Minim, "Dotted minim",                    0, True,  True,  0, 24 },
  },{
    { Semibreve, "Semibreve",                   0, False, False, 0, 40 },
    { Semibreve, "Dotted semibreve",            0, False, True,  0, 40 },
  },{
    { Breve, "Breve",                           0, False, False, 0, 60 },
    { Breve, "Dotted breve",                    0, False, True,  0, 60 },
  },
};

int noteVisualCount = XtNumber(noteVisuals);

/* }}} */
/* {{{ Rests */

RestVisualCompound restVisuals[] = {
  {
    rest_hemidemisemi_bits,
    {
      Hemidemisemiquaver, "Hemidemisemiquaver rest",
      False, 0, (Dimension)rest_hemidemisemi_width,
    }, {
      Hemidemisemiquaver, "Dotted hemidemisemiquaver rest",
      True,  0, (Dimension)(rest_hemidemisemi_width + DotWidth),
    },
  },{
    rest_demisemi_bits,
    {
      Demisemiquaver, "Demisemiquaver rest",
      False, 0, (Dimension)rest_demisemi_width,
    }, {
      Demisemiquaver, "Dotted demisemiquaver rest",
      True,  0, (Dimension)(rest_demisemi_width + DotWidth),
    },
  },{
    rest_semiquaver_bits,
    {
      Semiquaver, "Semiquaver rest",
      False, 0, (Dimension)rest_semiquaver_width,
    }, {
      Semiquaver, "Dotted semiquaver rest",
      True,  0, (Dimension)(rest_semiquaver_width + DotWidth),
    },
  },{
    rest_quaver_bits,
    {
      Quaver, "Quaver rest",
      False, 0, (Dimension)rest_quaver_width,
    }, {
      Quaver, "Dotted quaver rest",
      True,  0, (Dimension)(rest_quaver_width + DotWidth),
    },
  },{
    rest_crotchet_bits,
    {
      Crotchet, "Crotchet rest",
      False, 0, (Dimension)rest_crotchet_width,
    }, {
      Crotchet, "Dotted crotchet rest",
      True,  0, (Dimension)(rest_crotchet_width + DotWidth),
    },
  },{
    rest_minim_bits,
    {
      Minim, "Minim rest",
      False, 0, (Dimension)rest_minim_width,
    }, {
      Minim, "Dotted minim rest",
      True,  0, (Dimension)(rest_minim_width + DotWidth),
    },
  },{
    rest_semibreve_bits,
    {
      Semibreve, "Semibreve rest",
      False, 0, (Dimension)rest_semibreve_width,
    }, {
      Semibreve, "Dotted semibreve rest",
      True,  0, (Dimension)(rest_semibreve_width + DotWidth),
    },
  },{
    rest_breve_bits,
    {
      Breve, "Breve rest",
      False, 0, (Dimension)rest_breve_width,
    }, {
      Breve, "Dotted breve rest",
      True,  0, (Dimension)(rest_breve_width + DotWidth),
    },
  },
};

int restVisualCount = XtNumber(restVisuals);

/* }}} */
/* {{{ Note Mods and Chord Mods */

NoteModVisualRec noteModVisuals[] = {
  { ModSharp,   "Sharp  ", notemod_sharp_bits,   0, },
  { ModFlat,    "Flat   ", notemod_flat_bits,    0, },
  { ModNatural, "Natural", notemod_natural_bits, 0, },
};

int noteModVisualCount = XtNumber(noteModVisuals);



ChordModVisualRec chordModVisuals[] = {
  { ModDot,     "Staccato   ", chordmod_dot_bits,     0, },
  { ModLegato,  "Legato     ", chordmod_legato_bits,  0, },
  { ModAccent,  "Accent     ", chordmod_accent_bits,  0, },
  { ModSfz,     "Sforzando  ", chordmod_sfz_bits,     0, },
  { ModRfz,     "Rinforzando", chordmod_rfz_bits,     0, },
  { ModTrill,   "Trill      ", chordmod_trill_bits,   0, },
  { ModTurn,    "Turn       ", chordmod_turn_bits,    0, },
  { ModPause,   "Pause      ", chordmod_pause_bits,   0, },
};

int chordModVisualCount = XtNumber(chordModVisuals);

/* }}} */
/* {{{ Clefs and Keys */

ClefVisualRec clefVisuals[] = {
  { TrebleClef, "Treble", clef_treble_bits,     0, },
  { TenorClef,  "Tenor ", clef_tenor_bits,      0, },
  { AltoClef,   "Alto  ", clef_alto_bits,       0, },
  { BassClef,   "Bass  ", clef_bass_bits,       0, },
};

int clefVisualCount = XtNumber(clefVisuals);



KeyVisualRec keyVisuals[] = {
  { KeyA,      "A  maj / F# min", True,  3 },
  { KeyAflat,  "Ab maj / F  min", False, 4 },
  { KeyB,      "B  maj / G# min", True,  5 },
  { KeyBflat,  "Bb maj / G  min", False, 2 },
  { KeyC,      "C  maj / A  min", True,  0 },
  { KeyCflat,  "Cb maj / Ab min", False, 7 },
  { KeyCsharp, "C# maj / A# min", True,  7 },
  { KeyD,      "D  maj / B  min", True,  2 },
  { KeyDflat,  "Db maj / Bb min", False, 5 },
  { KeyE,      "E  maj / C# min", True,  4 },
  { KeyEflat,  "Eb maj / C  min", False, 3 },
  { KeyF,      "F  maj / D  min", False, 1 },
  { KeyFsharp, "F# maj / D# min", True,  6 },
  { KeyG,      "G  maj / E  min", True,  1 },
  { KeyGflat,  "Gb maj / Eb min", False, 6 },
};

int keyVisualCount = XtNumber(keyVisuals);

/* }}} */
/* {{{ Initialisation and clean-up */

Result InitialiseVisuals(void)
{
  int      i;
  NoteTag  x;
  Window   w;
  Pixmap   bodyFilledPixmap;
  Pixmap   bodyEmptyPixmap;

  Begin("InitialiseVisuals");

  w = RootWindowOfScreen(XtScreen(topLevel));

  bodyFilledPixmap =
    YCreatePixmapFromData(note_bodyfilled_bits, note_bodyfilled_width,
			  note_bodyfilled_height, NoShade);

  bodyEmptyPixmap =
    YCreatePixmapFromData(note_bodyempty_bits, note_bodyempty_width,
			  note_bodyempty_height, NoShade);
  
  for (x = ShortestNote; x <= LongestNote; ++x) {

    noteVisuals[x].undotted.body =
      noteVisuals[x].dotted.body =
	(x < Minim) ? bodyFilledPixmap : bodyEmptyPixmap;

    restVisuals[x].undotted.pixmap = restVisuals[x].dotted.pixmap =
      YCreatePixmapFromData(restVisuals[x].bitmap,
			    restVisuals[x].undotted.width,
			    StaveHeight, NoShade);
  }

  for (i = 0; i < XtNumber(noteModVisuals); ++i)
    noteModVisuals[i].pixmap =
      YCreatePixmapFromData(noteModVisuals[i].bitmap, NoteModWidth - 1,
			    NoteModHeight, NoShade);

  for (i = 0; i < XtNumber(chordModVisuals); ++i) {
    if (chordModVisuals[i].type == ModPause) { /* ugh! fix */
      chordModVisuals[i].pixmap =
	YCreatePixmapFromData(chordModVisuals[i].bitmap, chordmod_pause_width,
			      chordmod_pause_height, NoShade);
    } else {
      chordModVisuals[i].pixmap =
	YCreatePixmapFromData(chordModVisuals[i].bitmap, NoteWidth,
			      ChordModHeight, NoShade);
    }
  }

  for (i = 0; i < XtNumber(clefVisuals); ++i)
    clefVisuals[i].pixmap =
      YCreatePixmapFromData(clefVisuals[i].bitmap, ClefWidth,
			    StaveHeight + 2*NoteHeight, NoShade);

  roseHeight = rose_height;

  /* The following XCreateBitmapFromData calls were converted back */
  /* from YCreatePixmapFromData calls by cc, 2/95, so as to get    */
  /* better integrity in icon image and aboutButton highlighting   */
  /* If this breaks mono displays, I'd like to know about it       */

  roseMap = XCreateBitmapFromData
    (display, w, rose_bits, rose_width, rose_height);

  roseMaskMap = XCreateBitmapFromData
    (display, w, rose_mask_bits, rose_mask_width, rose_mask_height);

  greyMap =
    YCreatePixmapFromData(grey_bits, grey_width, grey_height, NoShade);

  lightGreyMap =
    YCreatePixmapFromData(lightgrey_bits, lightgrey_width,
			  lightgrey_height, NoShade);

  noteDotMap =
    YCreatePixmapFromData(dot_bits, dot_width, dot_height, NoShade);

  tailUpMap[3] = YCreatePixmapFromData
    (tail_up_1_bits, tail_up_1_width, tail_up_1_height, NoShade);
  tailUpMap[2] = YCreatePixmapFromData
    (tail_up_2_bits, tail_up_2_width, tail_up_2_height, NoShade);
  tailUpMap[1] = YCreatePixmapFromData
    (tail_up_3_bits, tail_up_3_width, tail_up_3_height, NoShade);
  tailUpMap[0] = YCreatePixmapFromData
    (tail_up_4_bits, tail_up_4_width, tail_up_4_height, NoShade);

  tailDownMap[3] = YCreatePixmapFromData
    (tail_down_1_bits, tail_down_1_width, tail_down_1_height, NoShade);
  tailDownMap[2] = YCreatePixmapFromData
    (tail_down_2_bits, tail_down_2_width, tail_down_2_height, NoShade);
  tailDownMap[1] = YCreatePixmapFromData
    (tail_down_3_bits, tail_down_3_width, tail_down_3_height, NoShade);
  tailDownMap[0] = YCreatePixmapFromData
    (tail_down_4_bits, tail_down_4_width, tail_down_4_height, NoShade);

  upMap = XCreateBitmapFromData
    (display, RootWindowOfScreen(XtScreen(topLevel)),
     up_bits, up_width, up_height);

  downMap = XCreateBitmapFromData
    (display, RootWindowOfScreen(XtScreen(topLevel)),
     down_bits, down_width, down_height);

  Return(Succeeded);
}


void VisualsCleanUp(void)
{
  int i;

  Begin("VisualsCleanUp");

  if (!display) End;

  if (      roseMap) XFreePixmap(display,      roseMap);
  if (  roseMaskMap) XFreePixmap(display,  roseMaskMap);
  if (      greyMap) XFreePixmap(display,      greyMap);
  if ( lightGreyMap) XFreePixmap(display, lightGreyMap);
  if (   noteDotMap) XFreePixmap(display,   noteDotMap);
  if (        upMap) XFreePixmap(display,        upMap);
  if (      downMap) XFreePixmap(display,      downMap);

  roseMap = roseMaskMap = greyMap = lightGreyMap = noteDotMap = 0;

  for (i = 0; i < 4; ++i) {

    if (tailDownMap[i]) XFreePixmap(display, tailDownMap[i]);
    if (tailUpMap[i]) XFreePixmap(display, tailUpMap[i]);

    tailDownMap[i] = tailUpMap[i] = 0;
  }

  End;
}

/* }}} */

