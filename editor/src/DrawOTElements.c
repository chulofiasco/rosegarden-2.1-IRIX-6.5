
/* DrawOTElements.c */
/* Musical Notation Editor for X, Chris Cannam 1994-95 */

/* Atomic element and Chord drawing to OpusTeX stream */

/*#define DEBUG 1*/

#include <ctype.h>
#include "Draw.h"
#include <SysDeps.h>
#include <Debug.h>

/* In this file, "lengths" refer to time and "widths" to on-screen
   space */

static char *instrumentNumerals[] = {
  "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix",
};

static char *noteNames[] = {
  "cccc", "ccc", "cc", "c", "q", "h", "w",
};


void DrawOTNothing(MusicObject obj, FILE *f, Dimension width,
                   int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Begin("DrawOTNothing");
  End;
}

void DrawOTClef(MusicObject obj, FILE *f, Dimension width,
                int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Clef *c = (Clef *)obj;
  Begin("DrawOTClef");

  fprintf(f, "%% change to %s clef\n", c->clef.visual->name);
/*  fprintf(f, "%% (if this is after a barline... then it should probably be "
          "before it)\n"); */
  fprintf(f, "\\setclef%d%s", staff+1,
          c->clef.clef == TrebleClef ? "\\treble" :
          c->clef.clef ==  TenorClef ? "{4}" :
          c->clef.clef ==   AltoClef ? "\\alto" : "\\bass");
  End;
}

void DrawOTKey(MusicObject obj, FILE *f, Dimension width,
               int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Key *key = (Key *)obj;
  Begin("DrawOTKey");

  fprintf(f, "\\setsign%d{%d}\\relax %% %s\n", staff+1,
          key->key.visual->sharps ?
          key->key.visual->number : -key->key.visual->number,
          key->key.visual->name);
  End;
}

/* !! I think its a bad idea to have this piece of code twice ! */ 
/* cf DrawOTStave.c */
void DrawOTTimeSignature(TimeSignature *sig, FILE *f, int staff)
{
  Begin("DrawOTTimeSignature");

  fprintf(f, "%% change time signature to %d/%d\n", sig->numerator,
          sig->denominator);
  fprintf(f, "\\setmeter%d{{\\meterfrac{%d}{%d}}}%%\n",
          staff+1, sig->numerator,
          sig->denominator);
  End;  
}

void DrawOTText(MusicObject obj, FILE *f, Dimension width,
                int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Text *text = (Text *)obj;
  TextPosnTag pos = text->text.position;
  Begin("DrawOTText");

  /* This used to end with %%\n, but I replaced it with \\relax so as
     to allow use from DrawPMX, which doesn't allow \n at this point */

  fprintf(f, "\\%s%s}\\relax",
          pos == TextAboveStave      ? "Uptext{" :
          pos == TextAboveStaveLarge ? "Uptext{" :
          pos == TextAboveBarLine    ? "Uptext{" :
          pos == TextBelowStave      ? "zcharnote{-6}{" : "zcharnote{-6}{\\it ",
          text->text.text);
  End;  
}

void DrawOTRest(MusicObject obj, FILE *f, Dimension width, int staff,
                Boolean down, ClefTag clef, Boolean beamed)
{
  Rest *rest = (Rest *)obj;
  NoteTag type = rest->rest.visual->type;

  static char *restTable[] = {
    "eeeer", "eeer", "eer", "er", "qr", "hr", "wr", "Dwr", "Qwr",
  };

  /* !! Dwr and Qwr will never be used cause type <= LongestNote = 6 */ 

  Begin("DrawOTRest");

  fprintf(f, "\\%s", restTable[(int)type]);
  if (rest->rest.visual->dotted) fprintf(f, "p"); /* adding a p to the
                                                     does it _most of
                                                     the time */
  /* except for Dwr and Qwr who are never called */ 
  

  End;
}

void DrawOTMetronome(MusicObject obj, FILE *f, Dimension width,
                     int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  Metronome *m = (Metronome *)obj;
  Begin("DrawOTMetronome");

  fprintf(f, "\\relax\n%% metronome: %s = %d\n",
          m->metronome.beat.chord.visual->name, (int)m->metronome.setting);
  fprintf(f, "\\Uptext{\\metron{\\%su%s}{%d}}%%\n",
          noteNames[(int)m->metronome.beat.chord.visual->type],
          m->metronome.beat.chord.visual->dotted ? "p" : "",
          (int)m->metronome.setting);

  End;
}

void WriteOpusTeXPitchCode(FILE *f, Pitch p, ClefTag c)
{
  /* this code is not necessary, but i dont know Rosgardens
     Datastructure well enough to extract the neede infos so i
     calculate it on my own here */
  static char *octave[] = { "`", "", "'", "", "'", "''", "'''" };
  static char name[] = {'a','b','c','d','e','f','g'};
  int n;
  int o;
  int corr;
  
  Begin("WriteOpusTeXPitchCode");
  
  if (c == TrebleClef) {
    o = (p+25) / 7;
    n = (p+25) % 7;
  } else if (c== BassClef) {
    o = (p+11) / 7;
    n = (p+11) % 7;
  } else if (c== TenorClef) {
    o = (p+18) / 7;
    n = (p+18) % 7;
  } else { /* c== AltoClef */
    o = (p+18) / 7;
    n = (p+18) % 7;
    
  }
  fprintf(f,"{%s%c}",octave[o],(o<3 ? (char)toupper(name[n]) : name[n]));

#ifdef PITCH_ABSOLUTE
  corr = (c == TrebleClef ?  0 :
          c ==  TenorClef ? -1 :
          c ==   AltoClef ?  1 : 2);
  p = p - corr; 
  fprintf(f, "{%d}", p-corr);
#endif
  End;
}


void DrawOTChord(MusicObject obj, FILE *f, Dimension width,
                 int staff, Boolean down, ClefTag clef, Boolean beamed)
{
  ChordPart *chord = &(((Chord *)obj)->chord);
  char updowntag[2];
  int shifted = True;
  int any_shifted = 0;
  int sek,ter;
  int dotted = chord->visual->dotted;
  int lowered;
  int i,bez,last,dir;
  int voices = chord->voice_count;
  int pitch;
  int l[10];
  char c[2];
    

  Begin("DrawOTChord");

  for (i = 0; i < chord->voice_count; ++i) {

    /* chords must have their display_mods up to date, which means
       they must have been run through the bar format method -- we're
       gonna have to reformat *everything*, and get all the group
       layout cache data, before we can start OpusTeX output */

    if (chord->voices[i].display_mods & ModSharp) {
      fprintf(f,"\\sh"); WriteOpusTeXPitchCode(f,chord->voices[i].pitch, clef);
    }
    if (chord->voices[i].display_mods & ModFlat) {
      fprintf(f,"\\fl"); WriteOpusTeXPitchCode(f,chord->voices[i].pitch, clef);
    }
    if (chord->voices[i].display_mods & ModNatural) {
      fprintf(f,"\\na"); WriteOpusTeXPitchCode(f,chord->voices[i].pitch, clef);
    }
  }
  /* let's not worry about second-intervals yet, ok not yet */
  
  bez = (down?voices-1:0);
  last = (down?-1:voices);
  dir = (down?-1:1);
  updowntag[1] = '\0';
  c[1] = '\0';

  lowered = 0;
  for (i = voices -1; i>=0;  i--) {
    pitch = chord->voices[i].pitch;
    sek = (i<voices-1 && (chord->voices[i+1].pitch == pitch + 1));
    ter = (i<voices-1 && (chord->voices[i+1].pitch == pitch + 2));
    any_shifted |= sek;
    if (sek && !(pitch % 2)) {
      lowered += 1;
    } else if (lowered>0 && (ter || sek)) {
      lowered += (ter ? 0: 1); /* (chord->voices[i].pitch % 2); */ 
    } else {
      lowered = 0;
    }
    l[i] = lowered;
  }
  if (!dotted)  for (i = voices -1; i>=0;  i--) l[i] = 0;
  
  shifted = 0;
  for (i=bez; i!=last; i+=dir) {
    pitch = chord->voices[i].pitch;
    sek = ((((i+dir!=last) && abs(chord->voices[i+dir].pitch - pitch) == 1)) || ((i!=bez) && abs(chord->voices[i-dir].pitch - pitch) == 1));
    c[0] = (shifted && sek ? (down ? 'l' : 'r'):'z');
    if (sek) {
      shifted = !shifted;
    } else {
      shifted = 0;
    }

    updowntag[0] =
      (down ?
       (beamed ?
        ((chord->visual->type < Crotchet) ? 'b' : 'u') : 'd') :
       (beamed ?
        ((chord->visual->type < Crotchet) ? 'b' : 'd') : 'u'));
    if (voices == 1 && !beamed) { 
      updowntag[0] = 'a';
    } else if ((i!=last-dir) && !beamed) { 
      /* dont draw several stems, opustex wants only one stemed note
         per accord, moreover this as to be the last note issued */
      updowntag[0] = '\0';
    }
    /* now the final showdown */ 
    if (dotted) {
      if (l[i]!=0) {
        if (any_shifted && !down) {
          fprintf(f,"\\roff{\\dpt");
          WriteOpusTeXPitchCode(f,pitch-l[i], clef);
          fprintf(f,"}");
        } else {
          fprintf(f,"\\dpt");
          WriteOpusTeXPitchCode(f,pitch-l[i], clef);
        }
      } else {
        if (any_shifted && !down) {
          fprintf(f,"\\rpt");
          WriteOpusTeXPitchCode(f,pitch, clef);
        } else if (c[0]!='z') {
          fprintf(f,"\\pt");
          WriteOpusTeXPitchCode(f,pitch, clef);
        } 
      }
      
      /* printing the note head */
      if ((i==last-dir) && !beamed && c[0]!='z' && (voices > 1)) {
        updowntag[0] = '\0';
      }
      fprintf(f, "\\%s%s%s", \
              (((c[0]=='z') && (!beamed && (int)chord->visual->type < 4) && (updowntag[0] != '\0')) ? "" : c),\
              beamed || ((updowntag[0] == '\0') && ((int)chord->visual->type < 4)) ? \
                 "q" : noteNames[(int)chord->visual->type],
              ((chord->visual->type < Semibreve) && (c[0] == 'z')) ? updowntag : "");
      if (c[0] == 'z' && !(any_shifted && !down)) fprintf(f,"p"); 
      if (beamed && c[0] == 'z') fprintf(f, "%d", staff);
      
    } else { /* not dotted */
      if ((i==last-dir) && !beamed && c[0]!='z' && (voices > 1)) {
        updowntag[0] = '\0';
      }
      fprintf(f, "\\%s%s%s",
              (((c[0]=='z') && (!beamed &&  (int)chord->visual->type < 4) && (updowntag[0] != '\0')) ? "" : c),\
              (beamed || ((updowntag[0] == '\0') && \
                          ((int)chord->visual->type < 4))) ? "q" : \
              noteNames[(int)chord->visual->type],\
              ((chord->visual->type < Semibreve) && \
               (c[0] == 'z')) ? updowntag : "");
      if (beamed && c[0] == 'z') fprintf(f, "%d", staff);

    }
    WriteOpusTeXPitchCode(f, chord->voices[i].pitch, clef);

    /* we have a problem when the last (stem length determining) note
       head is shifted: No note has issued a stemdirection command,
       in that case we redraw the penultimate note head with a stem
       direction. Yes, this is ugly but opustex is about to change
       these things anyway...  */
    if ((i==last-dir) && !beamed && c[0]!='z' && (voices > 1)) {
/*      fprintf(f,"%% last was shifted\n"); */
      updowntag[0] =
        (down ?
         (beamed ?
          ((chord->visual->type < Crotchet) ? 'b' : 'u') : 'd') :
         (beamed ?
          ((chord->visual->type < Crotchet) ? 'b' : 'd') : 'u'));
      fprintf(f,"\\%s%s%s", (((int)chord->visual->type < 4) ? "" : "z"),noteNames[(int)chord->visual->type], updowntag); 
      WriteOpusTeXPitchCode(f, chord->voices[i-dir].pitch, clef);
    }
  }
}

