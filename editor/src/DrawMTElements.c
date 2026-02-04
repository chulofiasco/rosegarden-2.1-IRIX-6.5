
/* DrawMTElements.c */
/* Musical Notation Editor for X, Chris Cannam 1994-95 */

/* Atomic element and Chord drawing to MusicTeX stream */

/*#define DEBUG 1*/

#include "Draw.h"
#include <SysDeps.h>
#include <Debug.h>

/* In this file, "lengths" refer to time and "widths" to on-screen
   space */

static char *instrumentNumerals[] = {
  "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix",
};

static char *noteNames[] = {
  "cccc", "ccc", "cc", "c", "q", "h", "wh",
};


void DrawMTNothing(MusicObject obj, FILE *f, Dimension width,
		   int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Begin("DrawMTNothing");
  End;
}

void DrawMTClef(MusicObject obj, FILE *f, Dimension width,
		int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Clef *c = (Clef *)obj;
  Begin("DrawMTClef");

  fprintf(f, "%% change to %s clef\n", c->clef.visual->name);
  fprintf(f, "%% (if this is after a barline... then it should probably be "
	  "before it)\n");
  fprintf(f, "\\cleftoks%s={%d000}%%\n", instrumentNumerals[staff],
	  c->clef.clef == TrebleClef ? 0 :
	  c->clef.clef ==  TenorClef ? 4 :
	  c->clef.clef ==   AltoClef ? 3 : 6);

  End;
}

void DrawMTKey(MusicObject obj, FILE *f, Dimension width,
	       int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Key *key = (Key *)obj;
  Begin("DrawMTKey");

  fprintf(f, "\\sign%s=%d\\relax %% %s\n", instrumentNumerals[staff],
	  key->key.visual->sharps ?
	  key->key.visual->number : -key->key.visual->number,
	  key->key.visual->name);
  End;
}

void DrawMTTimeSignature(TimeSignature *sig, FILE *f, int staff)
{
  Begin("DrawMTTimeSignature");

  fprintf(f, "%% change time signature to %d/%d\n", sig->numerator,
	  sig->denominator);
  fprintf(f, "\\metertoks%s={{\\meterfrac{%d}{%d}}{}{}{}}%%\n",
	  instrumentNumerals[staff], sig->numerator,
	  sig->denominator);
  End;  
}

void DrawMTText(MusicObject obj, FILE *f, Dimension width,
		int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Text *text = (Text *)obj;
  TextPosnTag pos = text->text.position;
  Begin("DrawMTText");

  /* This used to end with %%\n, but I replaced it with \\relax so as
     to allow use from DrawPMX, which doesn't allow \n at this point */

  fprintf(f, "\\%s%s}\\relax",
	  pos == TextAboveStave       ? "Uptext{" :
	  pos == TextAboveStaveLarge  ? "Uptext{" :
	  pos == TextAboveBarLine     ? "Uptext{" :
	  pos == TextBelowStave       ? "zcharnote{-6}{" :
	  pos == TextBelowStaveItalic ? "zcharnote{-6}{\\it " :
	  pos == TextChordName        ? "Uptext{" :
	  pos == TextDynamic          ? "zcharnote{-6}{\\it " : "Uptext{",
	  text->text.text);
  End;  
}

void DrawMTRest(MusicObject obj, FILE *f, Dimension width, int staff,
		Boolean down, ClefTag clef, Boolean beamed)
{
  Rest *rest = (Rest *)obj;
  NoteTag type = rest->rest.visual->type;

  static char *restTable[] = {
    "qqs", "hs", "qs", "ds", "qp", "hpause", "pause",
  };

  Begin("DrawMTRest");

  if (rest->rest.visual->dotted) fprintf(f, "\\pt{4}");
  fprintf(f, "\\%s", restTable[(int)type]);

  End;
}

void DrawMTMetronome(MusicObject obj, FILE *f, Dimension width,
		     int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Metronome *m = (Metronome *)obj;
  Begin("DrawMTMetronome");

  fprintf(f, "\\relax\n%% metronome: %s = %d\n",
	  m->metronome.beat.chord.visual->name, (int)m->metronome.setting);
  fprintf(f, "\\Uptext{\\metron{\\%su%s}{%d}}%%\n",
	  noteNames[(int)m->metronome.beat.chord.visual->type],
	  m->metronome.beat.chord.visual->dotted ? "p" : "",
	  (int)m->metronome.setting);

  End;
}

void DrawMTChord(MusicObject obj, FILE *f, Dimension width,
		 int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  ChordPart *chord = &(((Chord *)obj)->chord);
  char updowntag[2];
  Boolean shifted = False;
  int i;

  Begin("DrawMTChord");

  updowntag[1] = '\0';

  for (i = 0; i < chord->voice_count; ++i) {

    /* chords must have their display_mods up to date, which means
       they must have been run through the bar format method -- we're
       gonna have to reformat *everything*, and get all the group
       layout cache data, before we can start MusicTeX output */

    if (chord->voices[i].display_mods & ModSharp) {
      fprintf(f,"\\sh"); WriteMusicTeXPitchCode(f,chord->voices[i].pitch, clef);
    }
    if (chord->voices[i].display_mods & ModFlat) {
      fprintf(f,"\\fl"); WriteMusicTeXPitchCode(f,chord->voices[i].pitch, clef);
    }
    if (chord->voices[i].display_mods & ModNatural) {
      fprintf(f,"\\na"); WriteMusicTeXPitchCode(f,chord->voices[i].pitch, clef);
    }
  }

  /* let's not worry about second-intervals yet */

  for (i = 0; i < chord->voice_count; ++i) {

    Boolean doneFinal = False;
    
    if (down) {
      shifted = (i < chord->voice_count - 1 && !shifted &&
		 (chord->voices[i+1].pitch == chord->voices[i].pitch + 1));
    } else {
      shifted = (i > 0 && !shifted &&
		 (chord->voices[i-1].pitch == chord->voices[i].pitch - 1));
    }

    /* stem & space for last note only, except (a) penultimate note
       when last note will be "shifted, with stem up" does have a
       stem, and it has to write the final note before it */

    if ((i == chord->voice_count-1) ||
	(i == chord->voice_count-2 && !down && !shifted &&
	 chord->voices[i+1].pitch == chord->voices[i].pitch + 1)) {

      if (i == chord->voice_count-2) { /* this bit same as below */

	if (chord->visual->type < Minim) fprintf(f, "\\rq");
	else if (chord->visual->type < Semibreve) fprintf(f, "\\rh");
	else fprintf(f, "\\rw");

	if (chord->visual->dotted) fprintf(f, "p");
	WriteMusicTeXPitchCode(f, chord->voices[i+1].pitch, clef);

	doneFinal = True;
      }
      
      updowntag[0] =
	(down ?
	 (beamed ?
	  ((chord->visual->type < Crotchet) ? 'b' : 'u') : 'l') :
	 (beamed ?
	  ((chord->visual->type < Crotchet) ? 'h' : 'l') : 'u'));

      fprintf(f, "\\%s%s%s",
	      beamed ? "q" : noteNames[(int)chord->visual->type],
	      chord->visual->type < Semibreve ? updowntag : "",
	      chord->visual->dotted ? "p" : "");

      if (beamed) fprintf(f, "%d", staff);

    } else {

      char c = (shifted ? (down ? 'l' : 'r') : 'z');

      if (chord->visual->type < Minim) fprintf(f, "\\%cq", c);
      else if (chord->visual->type < Semibreve) fprintf(f, "\\%ch", c);
      else fprintf(f, "\\%cw", c);

      if (chord->visual->dotted) fprintf(f, "p");
    }

    WriteMusicTeXPitchCode(f, chord->voices[i].pitch, clef);
    if (doneFinal) break;
  }

  End;
}


void WriteMusicTeXPitchCode(FILE *f, Pitch p, ClefTag c)
{
  Begin("WriteMusicTeXPitchCode");

  p -= (c == TrebleClef ?  0 :
	c ==  TenorClef ? -1 :
	c ==   AltoClef ?  1 : 2);

  fprintf(f, "{%d}", p);

  End;
}

