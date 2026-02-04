
#include <stdio.h>
#include "Format.h"
#include "StavePrivate.h"
#include "ItemList.h"
#include "Marks.h"

#include <SysDeps.h>
#include <Debug.h>


void VoiceToNameAndPMXOctave(NoteVoice *voice, Clef *clef,
                             char *noteNameReturn, NoteMods *modReturn,
                             int *octaveReturn)
{
  int mp;
  int octave;
  char note;
  NoteMods mods;
  Begin("VoiceToNameAndPMXOctave");

  mods = voice->display_mods;

  octave = 5;
  mp = voice->pitch;

  switch (clef->clef.clef) {
  case  TrebleClef: break;
  case   TenorClef: octave -= 1; break;
  case    AltoClef: octave -= 1; break;
  case    BassClef: octave -= 2; break;
  case InvalidClef: break;
  }

  mp -= 3;
  while (mp < 2) { mp += 7; --octave; }
  while (mp > 8) { mp -= 7; ++octave; }
  if (mp > 6) mp -= 7;
  note = 'a' + mp;

  if (octave < 0) octave = 0;
  if (octave > 9) octave = 9;

  if (noteNameReturn) *noteNameReturn = note;
  if (modReturn) *modReturn = mods;
  if (octaveReturn) *octaveReturn = octave;

  End;
}


static Boolean **slurPresent = 0;	/* slurPresent[staff][0] is True
					   if s-slur is active on staff;
					   slurPresent[staff][1] if t-slur */

void WriteSlurs(FILE *file, int staff, Phrase *phrase, Boolean inTuplet)
{
  int i;
  MarkList mlist;
  Begin("WriteSlurs");

  if (!slurPresent) End;	/* something wrong! */

  /* we'll stick to making this sort of tie a t-slur.  That means if
     there's already a t-slur present, you won't get the tie.  Note
     this only applies to auto-ties at the ends of bars, not
     user-specified ties (which are Marks, & will work like Slurs) */

  if (phrase->phrase.tied_backward && slurPresent[staff][1] && !inTuplet) {
    fprintf(file, "t "); slurPresent[staff][1] = False;
  }
  if (phrase->phrase.tied_forward && !slurPresent[staff][1] && !inTuplet) {
    fprintf(file, "t "); slurPresent[staff][1] = True;
  }

  /* this is pretty gross! the intention's very simple; but there are
     several conditions, and I've taken the most improper step of
     using the "ilist" temporary storage in the other_end Mark to
     indicate which kind of slur (s or t) it's supposed to close,
     hence the disgusting type-casts. */

  for (i = 0; i < 2; ++i) {

    /* do Ties first: if we have both, we want the Tie to be closer to
       the note than the Slur */
    int checkingType = i==0 ? Tie : Slur;

    for (mlist = (MarkList)First(phrase->item.marks); mlist;
	 mlist = (MarkList)Next(mlist)) {

      if (mlist->mark->type != checkingType) continue;

      if (mlist->mark->start) {
	if (slurPresent[staff][0]) {
	  if (slurPresent[staff][1] || inTuplet) {
	    mlist->mark->other_end->ilist = 0;
	  } else {
	    fprintf(file, "t ");
	    mlist->mark->other_end->ilist = (MusicObject)'t';
	    slurPresent[staff][1] = True;
	  }
	} else {
	  fprintf(file, "s ");
	  mlist->mark->other_end->ilist = (MusicObject)'s';
	  slurPresent[staff][0] = True;
	}
      } else {            /* end of slur */
	char c = (char)(((unsigned long)mlist->mark->ilist) & 0xff);
	if (c == 't') {
	  if (inTuplet) {		/* uh-oh, now we're screwed! */
	    fprintf
	      (stderr,
	       "Warning: PMX output: End of t-slur found in tuplet group!\n"
	       "PMX can't do this case; expect to have to edit the file\n");
	  }
	  fprintf(file, "t ");
	  slurPresent[staff][1] = False;
	} else if (c == 's') {
	  fprintf(file, "s ");
	  slurPresent[staff][0] = False;
	}
      }
    }
  }

  End;
}


void WriteOrnaments(FILE *file, Chord *chord)
{
  ChordMods mods;
  Begin("WriteOrnaments");

  mods = chord->chord.modifiers;

  if (mods & ModDot) fprintf(file, "o. ");
  if (mods & ModLegato) fprintf(file, "o_ ");
  if (mods & ModAccent) ; /* doesn't seem to be an accent in PMX !? */
  if (mods & ModSfz) ;
  if (mods & ModRfz) ;
  if (mods & ModTrill) fprintf(file, "oT0 ");
  if (mods & ModTurn) fprintf(file, "ot "); /* er, that's a shake, but hey */
  if (mods & ModPause) fprintf(file, "of ");

  End;
}


void WriteChordAsPMX(FILE *file, int staff, Clef *clef, ItemList il)
{
  int i;
  char note;
  int octave;
  NoteMods mods;
  char duration;
  Chord *c = (Chord *)il->item;
  Boolean dotted;
  Boolean inTuplet;
  Boolean startsTuplet;
  static int lastPitch = 0;
  static char *noteNames = "63184200";
  static Boolean sActive = False, tActive = False;
  Boolean up = False;
  Boolean shifted = False;
  Begin("WriteChordAsPMX");

  duration = noteNames[c->chord.visual->type];
  if (c->chord.visual->type == Breve) {
    fprintf(stderr, "Warning: Can't write breves in PMX.  You'll probably "
            "have to\nedit the file by hand.\n");
  }

  dotted = c->chord.visual->dotted;

  inTuplet = GROUPING_TYPE(c) == GroupTupled;
  startsTuplet = inTuplet && c->item.grouping.tupled.start;

  up = GetChordStemDirection(c, clef, 0);

  for (i = 0; i < c->chord.voice_count; ++i) {

    NoteVoice *voice;

    /* if the stem is pointing down, we want to make the topmost voice
       the primary (i.e. first) note-head, because this is the one PMX
       uses to work out the stem direction. */

    if (up) voice = &c->chord.voices[i];
    else    voice = &c->chord.voices[c->chord.voice_count - i - 1];

    VoiceToNameAndPMXOctave(voice, clef, &note, &mods, &octave);

    if (i == 0 && (startsTuplet || !inTuplet)) {

      if (startsTuplet) {
        duration = noteNames[NumberToTag(c->item.grouping.tupled.tupled_length,
                                         &dotted)];
      }

      lastPitch = voice->pitch;

      fprintf(file, "%c%s%s%c%d", note,
              dotted ? "d" : "",
              mods & ModSharp ?   "s" :
              mods & ModFlat  ?   "f" :
              mods & ModNatural ? "n" : "",
              duration, octave);

      if (startsTuplet) {
        int tupledCount = 1;
        while (il && GROUPING_TYPE(il->item) == GroupTupled &&
               !il->item->item.grouping.tupled.end) {
          ++tupledCount; il = iNext(il);
        }
        fprintf(file, "x%d ", tupledCount);
      } else {
        fprintf(file, " ");
      }

      WriteSlurs(file, staff, (Phrase *)c, inTuplet);

    } else {

      /* more duplication with DrawChord().  Could use a Chord method
	 that says whether a note-head is shifted or not */

      if (up) {
	shifted =
	  (i > 0 && !shifted &&
	   (c->chord.voices[i-1].pitch == voice->pitch - 1));
      } else {
	shifted =
	  (i > 0 && !shifted &&
	   (c->chord.voices[c->chord.voice_count-i].pitch == voice->pitch + 1));
      }

      fprintf(file, "%s%c%s%s", i == 0 ? "" : "z", note,
	      shifted ? up ? "r" : "e" : "",
              mods & ModSharp ?   "s" :
              mods & ModFlat  ?   "f" :
              mods & ModNatural ? "n" : "");

      if (voice->pitch > lastPitch) {
        while (voice->pitch > lastPitch + 4) {
          fprintf(file, "+"); lastPitch += 7;
        }
      } else {
        while (voice->pitch < lastPitch - 4) {
          fprintf(file, "-"); lastPitch -= 7;
        }
      }

      lastPitch = voice->pitch;

      fprintf(file, " ");
    }

    if (i == 0) {
      WriteOrnaments(file, c);
    }
  }

  End;
}


void WriteRestAsPMX(FILE *file, Rest *r)
{
  char duration;
  Boolean dotted;
  Begin("WriteRestAsPMX");

  duration = "63184200"[r->rest.visual->type];
  if (r->rest.visual->type == Breve) {
    fprintf(stderr,
	    "Warning: Can't write breve rests in PMX.  You'll probably"
            "have to\nedit the file by hand.\n");
  }

  dotted = r->rest.visual->dotted;

  fprintf(file, "r%s%c ", dotted ? "d" : "", duration);

  End;
}


Result WriteStaveAsPMX(MajorStave sp, int staffs, FILE *file)
{
  int i;
  StaveEltList barList;
  MajorStaveRec *stave = (MajorStaveRec *)sp;
  ItemList il;
  Key *key = &defaultKey;
  Clef *clef = &defaultClef;
  Boolean beamOpen = False;
  Bar *bar = 0;
  int barNo = 1;
  int writeBeams = -1;
  int anacrusis = 0;
  static char *clefNames = "tnabt";

  Begin("WriteStaveAsPMX");

  barList = (StaveEltList)First(stave->bar_list);

  for (il = stave->music[0]; il; il = iNext(il)) {
    if (il->item->object_class == KeyClass) { key = (Key *)il->item; break; }
  }

  bar = barList->bars[0];
  if (!bar) Return(Failed);

  StaveBusyStartCount(stave->bars);
  StaveBusyMakeCount(0);

  /* Check time signature is the same for all bars at the start? */

  /* Cope with anacruses! please */

  if (bar->methods->get_length(bar) != bar->bar.time.bar_length) {
    anacrusis = bar->methods->get_length(bar) /
      GetTimeSigBeatLength(&bar->bar.time);
  }

  fprintf(file, "%% Generated by Rosegarden\n");
  fprintf(file, "%% PMX generation code: Chris Cannam, 1997\n");
  fprintf(file, "%% Bugs, fixes and other comments to cannam@netcomuk.co.uk\n");
  fprintf(file, "%%\n%%\n%% number of staffs\n%d\n"
          "%% total number of voices\n%d\n"
          "%% logical timesig numerator\n%d\n"
          "%% logical timesig denominator\n%d\n"
          "%% drawn timesig numerator\n%d\n"
          "%% drawn timesig denominator\n%d\n"
          "%% beats in anacrusis bar\n%d\n"
          "%% sharps (+ve) or flats (-ve) in key\n%d\n"
          "%% number of pages, or zero\n%d\n"
          "%% number of systems, or measures per system\n%d\n"
          "%% point size (16 or 20)\n%d\n"
          "%% indentation of first line, as decimal fraction\n%lf\n",
          staffs, staffs, bar->bar.time.numerator, bar->bar.time.denominator,
          bar->bar.time.numerator, bar->bar.time.denominator,
          anacrusis, key->key.visual->sharps ?
          key->key.visual->number : -key->key.visual->number, 0, 4, 16, 0.1);

  for (i = staffs-1; i >= 0; --i) {
    fprintf(file, "%s\n", stave->names[i]);
  }

  for (i = staffs-1; i >= 0; --i) {
    for (il = stave->music[i]; il; il = iNext(il)) {
      if (il->item->object_class == ClefClass) {
        clef = (Clef *)il->item; break;
      }
    }
    fprintf(file, "%c", clefNames[(int)clef->clef.clef]);
  }

  fprintf(file, "\n./\n");

  slurPresent = (Boolean **)XtMalloc(staffs * sizeof(Boolean *));
  for (i = 0; i < staffs; ++i) {
    slurPresent[i] = (Boolean *)XtCalloc(2, sizeof(Boolean));
  }

  while (barList) {

    for (i = staffs - 1; i >= 0; --i) {

      int depth = 0;
      Bar *bar = barList->bars[i];

      if (!bar) break; /* !!! deal with this properly, please! */

      if (i == staffs - 1) {

        fprintf(file, "%% Bar %d\n", barNo);

	if ((!Next(barList) || !((StaveEltList)Next(barList))->bars[i]) &&
	    bar->methods->get_length(bar) < bar->bar.time.bar_length) {
	  int num = bar->methods->get_length(bar)/TagToNumber(Semiquaver,False);
	  int denom = 16;
	  fprintf(stderr, "Incomplete final bar\n");
	  while (num > 16) { num /= 2; denom /= 2; }
	  if (num > 1)                                             /* ugh */
	    if (denom > 1) fprintf(file, "m%d%d00\n", num, denom);
	    else fprintf(file, "m%do00\n", num);
	  else
	    if (denom > 1) fprintf(file, "mo%d00\n", denom);
	    else fprintf(file, "moo00\n");

	} else {
 
	  if (Prev(barList) && !BarTimesEqual
	      (bar, ((StaveEltList)Prev(barList))->bars[i])) {
	    fprintf(file, "m%d%d%d%d\n", bar->bar.time.numerator,
		    bar->bar.time.denominator, bar->bar.time.numerator,
		    bar->bar.time.denominator);
	  }
	}
      }

      for (ItemList_ITERATE_GROUP(il, bar)) {

	MarkList mlist;

	/* I couldn't get \icresc...\tcresc dynamics to work right on
           first hack, so I'll stick with the lazy & ugly option for
           the moment and maybe do it right some other time: */

	if (mlist = FindMarkType(Crescendo, il->item->item.marks)) {
	  if (mlist->mark->start)
	    fprintf(file, "\n\\zcharnote{-6}{\\crescendo{36pt}}\\ \n  ");
	}
	if (mlist = FindMarkType(Decrescendo, il->item->item.marks)) {
	  if (mlist->mark->start)
	    fprintf(file, "\n\\zcharnote{-6}{\\decrescendo{36pt}}\\ \n  ");
	}

        if (GROUPING_TYPE(il->item) == GroupBeamed &&
            (il == barList->bars[i]->group.start ||
             il->item->item.grouping.beamed.start)) {

          if (writeBeams < 0) {
#ifdef ASK_ABOUT_BEAMS
            writeBeams = YQuery
              (topLevel, "Allow PMX to compute its own beam groups?", 2, 0, 1,
               "Yes - let PMX do it", "No - write explicit groups", NULL);
#else
	    writeBeams = False;
#endif
          }

          if (writeBeams) fprintf(file, "[ ");
          beamOpen = True;
        }

        switch (il->item->object_class) {

        case ChordClass:        /* StaveItemToClef is a bit slow, but... */
          WriteChordAsPMX(file, i, StaveItemToClef(sp, i, il), il);
          depth += ((Chord *)il->item)->chord.voice_count;
          break;

        case RestClass:
          WriteRestAsPMX(file, (Rest *)il->item);
          ++depth;
          break;

        case ClefClass:
          fprintf(file, "C%c ", clefNames[((Clef *)il->item)->clef.clef]);
          ++depth;
          break;

        case KeyClass:
          if (i < staffs - 1) break; /* can only key-change in first voice */
          fprintf(file, "K+0%c%1d ",
                  ((Key *)il->item)->key.visual->sharps ? '+' : '-',
                  ((Key *)il->item)->key.visual->number);
          break;

	case TextClass:
	  fprintf(file, "\n");
	  DrawMTText(il->item, file, 0, i, False, TrebleClef, False);
	  fprintf(file, "\\ \n  ");
        }

        if (GROUPING_TYPE(il->item) == GroupBeamed &&
            il->item->item.grouping.beamed.end) {
          if (beamOpen && writeBeams) {
            fprintf(file, "]\n  ");
            depth = 0;
          }
          beamOpen = False;
        }

        if (depth >= 8 && il != barList->bars[i]->group.end) {
          fprintf(file, "\n  ");
          depth = 0;
        }
      }

      if (beamOpen && writeBeams) fprintf(file, "] ");
      fprintf(file, "/\n");
    }

    StaveBusyMakeCount(barNo);

    barList = (StaveEltList)Next(barList);
    ++barNo;
  }

  StaveBusyFinishCount();

  XtFree(slurPresent);
  slurPresent = 0;

  Return(Succeeded);
}



Result StaveWritePMXToFile(MajorStave sp, Widget w)
{
  int i;
  MajorStaveRec *mstave = (MajorStaveRec *)sp;
  int staves = mstave->staves;
  String message;
  Result result;
  FILE *file;
  String name;
  Begin("StaveWritePMXToFile");

  if (staves > 7) {

    message = (String)XtMalloc(256);
    sprintf(message,
            "This piece has %d staffs; I can only handle seven.", staves);

    if (YQuery(topLevel, message, 2, 0, 1, "Write first 7 only", "Cancel",
               "Editor File - Export PMX") != 0) {
      XtFree(message);
      Return(Failed);
    } else {
      XtFree(message);
      staves = 7;
    }
  }

  if ((file = YFileGetWriteFile(w, "Editor File - Export PMX",
                                ".pmx", "PMX")) == NULL) {
    Return(Failed);
  }

  for (i = 0; i < staves; ++i) {
    UnformatItemList((ItemList *)&mstave->music[i], NULL);
    StaveResetFormatting(sp, i);
    StaveFormatBars(sp, i, -1);
  }

  result = WriteStaveAsPMX(sp, staves, file);
  fclose(file);

  name = YFileGetLastFilename(False);
  message = (String)XtMalloc(strlen(name) + 12);
  sprintf(message, "Wrote `%s'.", name);
  (void)YQuery(topLevel, message, 1, 0, 0, "OK", NULL);
  XtFree(message);

  Return(result);
}


