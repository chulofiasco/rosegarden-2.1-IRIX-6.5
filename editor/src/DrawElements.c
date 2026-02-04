
/* DrawElements.c */
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Atomic element and Chord drawing methods */
/* In this file, "lengths" refer to time and "widths" to on-screen
   space */

/* {{{ Includes */

#include "Draw.h"
#include "chordmod_pause.xbm"

/* }}} */

/* {{{ Trivial things */

Dimension DrawNothing(MusicObject obj, Drawable drawable, Position x,
		      Position y, Pitch p, Dimension width, LineEquation eqn)
{
  Begin("DrawNothing");
  Return(width);
}

/* }}} */
/* {{{ Chord and its components */

Dimension DrawNoteMod(NoteMods mod, Drawable drawable,
		      Position x, Position y, Pitch pitch)
{
  int n;
  Begin("DrawNoteMod");

  for (n = 0; !(noteModVisuals[n].type & mod); ++n);
  if  (n > 2) Return(0);

  CopyArea(noteModVisuals[n].pixmap, drawable, 0, 0,
	   NoteModWidth - 1, NoteModHeight, x,
	   y + STAVE_Y_COORD(pitch) - (NoteModHeight-NoteHeight)/2 -
	   ((mod & ModFlat) ? 2 : 0));

  Return(NoteModWidth);
}



/* Draws the body of a note, given its visual; this is perhaps misnamed, */
/* as a NoteVoice is only a record of the pitch and modifiers associated */
/* with a sound, and as such it doesn't really have a visual presence.   */

/* The x coord passed to this function should _already_ have had compen- */
/* sation added to it for the width of the NoteMods; this function won't */
/* compensate, it'll just draw the modifiers to the left of the x coord. */

/* virtualX and dotX are: X coord we think head is at for purpose of     */
/* drawing the modifiers (in case of shifted note); and X coord we think */
/* it's at for purpose of drawing the dots.  Degenerate pass in 0 for    */
/* same as X.                                                            */

Dimension DrawNoteVoice(NoteVoice *voice, Drawable drawable, Position x,
			Position y, Pitch offset, Dimension width,
			NoteVisual visual, Position virtualX, Position dotX)
{
  Position   ny;
  Pitch      p;
  NoteMods   mods;
  int        modno = 0;
  int        modco = 0;
  Dimension  rwidth;
  Pitch      pitch = voice->pitch + offset;
  Boolean    breve = (visual->type == Breve);

  Begin("DrawNoteVoice");

  ny = y + STAVE_Y_COORD(pitch);          /* work out height of body */

  if (!virtualX) virtualX = x;
  if (!dotX)         dotX = x;

  CopyArea(visual->body, drawable,                      /* draw body */
	   0, 0, NoteWidth, NoteHeight, x, ny);

  if (breve) {
    XDrawLine(display, drawable, drawingGC, x-5, ny, x-5, ny+NoteHeight-1);
    XDrawLine(display, drawable, drawingGC, x-2, ny, x-2, ny+NoteHeight-1);
    XDrawLine(display, drawable, drawingGC,
	      x+NoteWidth, ny, x+NoteWidth, ny+NoteHeight-1);
    XDrawLine(display, drawable, drawingGC,
	      x+NoteWidth+3, ny, x+NoteWidth+3, ny+NoteHeight-1);
  }

  if (visual->dotted) {

    /* hack with actual numeric values here */
    CopyArea(noteDotMap, drawable, 1, 3, DotWidth, 2,
	     dotX + NoteWidth + (breve? 5 : 1), ny + 3);
  }

  for (mods = voice->display_mods; mods; mods >>= 1, ++ modco)
    if (mods & 1)
      DrawNoteMod(1 << modco, drawable,         /* modify if necessary */
		  virtualX - (breve? 5 : 0) - NoteModWidth * ++modno, y, pitch);

  if (pitch < -1) {                              /* need leger lines */

    p = -pitch;
    if (p/2 != (p+1)/2) p -= 1;
    p = -p;

    for (ny = y + STAVE_Y_COORD(p) + NoteHeight/2 - 1; p < -1;
	 p += 2, ny -= NoteHeight + 1)
      XDrawLine(display, drawable, drawingGC, x-1, ny, x+NoteWidth-1, ny);

  } else if (pitch > 9) {

    p = pitch;
    if (p/2 != (p+1)/2) p -= 1;

    for (ny = y + STAVE_Y_COORD(p) + NoteHeight/2; p > 9;
	 p -= 2, ny += NoteHeight + 1)
      XDrawLine(display, drawable, drawingGC, x-1, ny, x+NoteWidth-1, ny);
  } 

  rwidth = NoteWidth + (visual->dotted ? DotWidth : 0) + (breve ? 4 : 0);

  if (width > 0) Return(width);
  else Return(rwidth);
}



Dimension DrawChord(MusicObject obj, Drawable drawable, Position x,
		    Position y, Pitch offset, Dimension width,
		    LineEquation eqn)
{
  Position     nx, ay, by, virtualX = 0, dotX = 0;
  Pitch        lowest  = highestNoteVoice.pitch;
  Pitch        highest =  lowestNoteVoice.pitch;
  Boolean      up = True;
  Dimension    rwidth = NoteWidth;
  Chord       *chord  = (Chord *)obj;
  NoteVoice   *voices;
  int          voiceno, voiceCount;
  ChordMods    mods;
  int          modno = 0;
  int          modco = 0;
  int          extent;
  NoteTag      type;
  short        groupXcoord;
  Boolean      shifted = False;
  Boolean      chordHasShift = False;

  Begin("DrawChord");

  if (!chord) Return(width);

  x      += chord->chord.note_modifier_width;
  rwidth += chord->chord.note_modifier_width;
  type    = chord->chord.visual->type;
  chord->item.x = x;

  if (chord->chord.visual->dotted) rwidth += DotWidth;
 
  if (eqn && eqn->eqn_present) {
    groupXcoord = EVIL_HACK_GROUP_X_COORD_FROM_EQN(eqn); 
  } else {
    groupXcoord = x;
  }

  highest =
    chord->methods->get_highest((MusicObject)chord)->pitch + offset;
  lowest =
    chord->methods->get_lowest((MusicObject)chord)->pitch + offset;

  /* up or down? */
  if (chord->chord.visual->stalked) {

    if (eqn && eqn->eqn_present) {
      up = EQN(eqn, groupXcoord, x) < (STAVE_Y_COORD(lowest));
      if (eqn->reverse) { up = !up; eqn = NULL; }
    } else {
      up = GetChordStemDirection(chord, NULL, offset);
    }
  }

  /* deal with the note heads first */

  voices = chord->chord.voices;
  voiceCount = chord->chord.voice_count;

  /* check for a second interval */

  for (voiceno = 0; voiceno < voiceCount-1; ++voiceno) {
    if (voices[voiceno].pitch == voices[voiceno+1].pitch - 1) {
      
      chordHasShift = True;	/* use this in a minute */

      /* if tail down and we've got second intervals, we'll want to start
	 the chord right a bit to make room for the left-shifted head. We
	 can only do this if we're not part of a beamed group, 'cos if we
	 are, the beam has already been drawn so we can't move the stem */

      if (!up && !(eqn && eqn->eqn_present)) {

	x += NoteWidth - 3;
	rwidth += NoteWidth - 3;
	break;
      }
    }
  }

  if (chordHasShift) {
    virtualX = up ? x : x - NoteWidth + 2;
    /*    dotX = up ? x + NoteWidth - 2 : x;*/
  }

  /* bottom to top if stem up, otherwise top to bottom */

  for (voiceno = (up ? 0 : voiceCount-1) ;
       up ? (voiceno < voiceCount) : (voiceno >= 0) ;
       voiceno += (up ? 1 : -1) ) {

    if (up) {
      shifted = (voiceno > 0 && !shifted &&
		 (voices[voiceno-1].pitch == voices[voiceno].pitch - 1));
    } else {
      shifted = (voiceno < voiceCount - 1 && !shifted &&
		 (voices[voiceno+1].pitch == voices[voiceno].pitch + 1));
    }

    if (shifted) {
      DrawNoteVoice(&voices[voiceno], drawable,
		    up ? (x + NoteWidth - 2) : (x - NoteWidth + 2),
		    y, offset, width, chord->chord.visual, virtualX, dotX);
    } else {
      DrawNoteVoice(&voices[voiceno], drawable,
		    x, y, offset, width, chord->chord.visual, virtualX, dotX);
    }
  }

  /* Draw the stalk, if there is one */

  if (chord->chord.visual->stalked) {

    /* because eqn is actually relative to the stem, not the note head: */
    if (up) groupXcoord += chord->methods->get_min_width(chord);
    
    nx = x + NoteWidth - 2;
    ay = y + STAVE_Y_COORD(lowest);
    by = y + STAVE_Y_COORD(highest);

    extent =
      (eqn && eqn->eqn_present) ? 0 : (type < Quaver ? 4*(Quaver-type) : 0);

    if (up) {

      XDrawLine
	(display, drawable, drawingGC, nx, ay + NoteHeight/2, (int)nx,
	 (int)((!eqn || !eqn->eqn_present) ?
	       by- NoteHeight*5/2 - extent : (EQN(eqn, groupXcoord, nx) + y)));

    } else {
      XDrawLine
	(display, drawable, drawingGC, x, by + NoteHeight/2, (int)x,
	 (int)((!eqn || !eqn->eqn_present) ?
	       ay+ NoteHeight*7/2 + extent : (EQN(eqn, groupXcoord, x) + y)));
    }

    /* Add note tails if there's no line equation for a beam */

    if ((!eqn || !eqn->eqn_present) &&
	type < Crotchet && chord->chord.visual->stalked)
      if (up) {
	CopyArea(tailDownMap[type],
		 drawable, 0, 0, TailWidth, TailHeight + 6*(Quaver-type),
		 nx + 1, by - NoteHeight*5/2 - extent + 1);
      } else {
	CopyArea(tailUpMap[type],
		 drawable, 0, 0, TailWidth, TailHeight + 6*(Quaver-type),
		 x + 1, ay + extent + NoteHeight*7/2 -
		 TailHeight - 6*(Quaver-type));
      }
  }
  
  /* Decorate with modifiers if necessary */

  if (up) {
    ay = y + NoteHeight + STAVE_Y_COORD(lowest);
    if (lowest  > 1) lowest  = 1;
    by = y + NoteHeight + STAVE_Y_COORD(lowest);
  } else {
    ay = y - NoteHeight + STAVE_Y_COORD(highest);
    if (highest < 7) highest = 7;
    by = y - NoteHeight + STAVE_Y_COORD(highest);
  }

  for (modno = 0, mods = chord->chord.modifiers; mods; mods >>= 1, ++ modco)
    if (mods & 1L)
      switch(modco) {

      case ModPausePower:
	{
	  int useY = up ? y - NoteHeight + STAVE_Y_COORD(highest) : ay;
	  
	  CopyArea
	    (chordModVisuals[modco].pixmap, drawable, 0, 0,
	     chordmod_pause_width, chordmod_pause_height, (int)x-2,
	     useY < y-NoteHeight ?
	     (int)useY - chordmod_pause_height*modno++ - 3:
	     (int)   y - chordmod_pause_height*modno++ - NoteHeight);
	}
	break;

      case ModDotPower: case ModLegatoPower:
	CopyArea
	  (chordModVisuals[modco].pixmap, drawable, 0, 0, NoteWidth,
	   ChordModHeight, (int)x,
	   up ? (int)ay + ChordModHeight*modno++ :
	        (int)ay - ChordModHeight*modno++ );
	break;

      default:
	CopyArea
	  (chordModVisuals[modco].pixmap, drawable, 0, 0, NoteWidth,
	   ChordModHeight, (int)x,
	   up ? (int)by + ChordModHeight*modno++ + 3:
	        (int)by - ChordModHeight*modno++ - 3);
	break;
      }

  /* Finally, check out the possibility of a tie */

  if (chord->phrase.tied_backward && !tie[0].present) {
    tie[0].present = True;
    tie[0].x       = x + NoteWidth/3;
    tie[0].above   = !(lowest < 4);
  }

  if (chord->phrase.tied_forward) {
    tie[1].present = True;
    tie[1].above   = !(lowest < 4);
    tie[1].x       = x + (2*NoteWidth)/3;
  }

  {
    String name = GetChordDisplayedName(chord); 
    if (name && strlen(name) > 0) {
      chordText.text.text = name;
      (void)DrawText
	((MusicObject)&chordText, drawable, x, y, offset, width, eqn);
    }
  }

  if (width > 0) Return(width);
  else Return(rwidth);
}


/* May modify some cache values in the Chord, notably display_mods and name */

char *Rechord(Chord);

void DrawClefAndChordOnSimpleDrawable(Clef *clef, Chord *chord,
				      Drawable d, Dimension wd, Dimension ht,
				      Boolean markAPitch, Pitch markPitch,
				      Boolean drawName)
{
  char *name = 0;
  int i, y, topY;

  Begin("DrawClefAndChordOnSimpleDrawable");

  for (i = 0; i < chord->chord.voice_count; ++i) {
    chord->chord.voices[i].display_mods =
      chord->chord.voices[i].modifiers;
  }

  if (drawName) {
    name = Rechord(*chord);
    if (chord->chord.chord_named) NameChord(chord, NULL, True);
  }

  XFillRectangle(display, d, clearGC, 0, 0, wd, ht);
  topY = (ht - StaveHeight) / 2;

  if (clef) {

    clef->methods->draw(clef, d, 10, topY, 0, 0, NULL);

    chord->methods->draw
      (chord, d, wd - 47 - NoteWidth,
       topY, ClefPitchOffset(clef->clef.clef), 0, NULL);

    if (markAPitch) {
      y = topY + NoteHeight/2 + 
	STAVE_Y_COORD(markPitch + ClefPitchOffset(clef->clef.clef));
    }

  } else {

    chord->methods->draw(chord, d, wd/2 - 10, topY, 0, 0, NULL);

    if (markAPitch) {
      y = topY + NoteHeight/2 + STAVE_Y_COORD(markPitch);
    }
  }

  if (markAPitch) {
    XDrawLine(display, d, drawingGC, wd - 13, y - 3, wd - 16, y);
    XDrawLine(display, d, drawingGC, wd - 16, y, wd - 8, y);
    XDrawLine(display, d, drawingGC, wd - 16, y, wd - 13, y + 3);
  }

  for (y = 0; y < StaveHeight;  y += NoteHeight + 1) {
    XDrawLine(display, d, drawingGC, 6, y + topY, wd - 24, y + topY);
  }

  if (drawName && name && name[0]) {
    chordText.text.text = name;	/* chordText is a weird global thing */
    (void)DrawText((MusicObject)&chordText, d, 4, ht + 33, 0, 0, 0);
    free(name); chordText.text.text = NULL;
  }

  End;
}

/* }}} */
/* {{{ Metronome */

Dimension DrawMetronome(MusicObject obj, Drawable drawable, Position x,
			Position y, Pitch offset, Dimension width,
			LineEquation eqn)
{
  Metronome *mnome = (Metronome *)obj;
  Dimension  off;
  char       string[20];

  Begin("DrawMetronome");

  if (!mnome) Return(width);

  mnome->item.x = x;

  off = DrawChord((MusicObject)&(mnome->metronome.beat),
		  drawable, x, 10, 0, 0, NULL);

  sprintf((char *)string, "= %d", mnome->metronome.setting);

  XDrawString(display, drawable, italicTextGC,
	      x + (int)off + 11, NoteHeight*7/2 + 8, string, strlen(string));

  Return(width);
}

/* }}} */
/* {{{ Clef */

Dimension DrawClef(MusicObject obj, Drawable drawable, Position x,
		   Position y, Pitch offset, Dimension width, LineEquation eqn)
{
  Begin("DrawClef");

  if (!obj) Return(width);

  ((Clef *)obj)->item.x = x;

  CopyArea(((Clef *)obj)->clef.visual->pixmap, drawable, 0, 0,
	   ClefWidth, StaveHeight + 2*NoteHeight, x, y - NoteHeight);

  if (width > 0) Return(width);
  else Return(ClefWidth + 3);
}

/* }}} */
/* {{{ Key */

Dimension DrawKey(MusicObject obj, Drawable drawable, Position x,
		  Position y, Pitch offset, Dimension width, LineEquation eqn)
{
  int          n;
  int          number;
  Dimension    off = 0;
  Pitch        pitch;
  Boolean      sharps;
  Key         *key = (Key *)obj;
  KeyVisual    prevV;

  Begin("DrawKey");

  if (!obj) Return(width);

  for (n = 0; n < keyVisualCount; ++n) {
    if (keyVisuals[n].key == key->key.changing_from) prevV = &keyVisuals[n];
  }

  key->item.x = x;
  sharps = key->key.visual->sharps;
  number = key->key.visual->number;

  if (prevV->sharps == sharps && /* may need to "reset" some pitches */
      number < prevV->number) {

    pitch = sharps ? 8 : 4;

    for (n = 0; n < prevV->number; ++n) {
      if (n >= number) {
	DrawNoteMod(ModNatural, drawable, x + off, y, pitch + offset);
	off += NoteModWidth - 2;
      }
      if (sharps) { pitch -= 3; if (pitch < 3) pitch += 7; }
      else        { pitch += 3; if (pitch > 7) pitch -= 7; }
    }

    off += 4;

  } else {

    if (prevV->sharps != sharps) { /* need to "reset" entire previous key */

      pitch = prevV->sharps ? 8 : 4;
      
      for (n = 0; n < prevV->number; ++n) {
	DrawNoteMod(ModNatural, drawable, x + off, y, pitch + offset);
	if (prevV->sharps) { pitch -= 3; if (pitch < 3) pitch += 7; }
	else               { pitch += 3; if (pitch > 7) pitch -= 7; }
	off += NoteModWidth - 2;
      }

      off += 4;
    }
  }

  pitch = sharps ? 8 : 4;

  for (n = 0; n < number; ++n) {
    DrawNoteMod(sharps ? ModSharp : ModFlat,
		drawable, x + off, y, pitch + offset);
    if (sharps) { pitch -= 3; if (pitch < 3) pitch += 7; }
    else        { pitch += 3; if (pitch > 7) pitch -= 7; }
    off += NoteModWidth - 2;
  }

  if (width > 0) Return(width);
  else Return(off + 3);
}

/* }}} */
/* {{{ Time signature */

Dimension DrawTimeSignature(MusicObject obj, Drawable drawable,
			    Position x, Position y, Pitch offset,
			    Dimension width, LineEquation eqn)
{
  char           a[5], b[5];
  TimeSignature *sig = (TimeSignature *)obj;

  Begin("DrawTimeSignature");

  if (!obj) Return(width);
  /*  sig->item.x = x;*/

  sprintf((char *)a, "%2d", sig->numerator);
  sprintf((char *)b, "%2d", sig->denominator);

  XDrawString(display, drawable, timeSigGC,
	      (int)x, (int)y + STAVE_Y_COORD(4) + NoteHeight/2 - 2,
	      a, strlen(a));
  XDrawString(display, drawable, timeSigGC,
	      (int)x, (int)y + STAVE_Y_COORD(0) + NoteHeight/2 - 2,
	      b, strlen(b));

  if (width > 0) Return(width);
  else Return(GetTimeSignatureWidth(obj));
}

/* }}} */
/* {{{ Text */

Dimension DrawText(MusicObject obj, Drawable drawable, Position x,
		   Position y, Pitch offset, Dimension width, LineEquation eqn)
{
  Text       *text = (Text *)obj;
  int         ty;
  int         dirn;
  int         dir, asc, desc;
  XCharStruct info;
  XGCValues   values;
  GC          gc;

  Begin("DrawText");

  if (!obj) Return(width);
  text->item.x = x;

  switch (text->text.position) {

  case TextAboveStave:
    /*    ty   = y - 10;*/
    ty   = y - 25;
    dirn = -1;
    gc   = littleTextGC;
    break;

  case TextAboveStaveLarge:
    ty   = y - StaveUpperGap + 42;
    dirn = -1;
    gc   = bigTextGC;
    break;

  case TextAboveBarLine:
    ty   = y + 2;
    dirn = -1;
    gc   = littleTextGC;
    break;

  case TextBelowStave:
    ty   = y + STAVE_Y_COORD(-5) + NoteHeight*7/2;
    dirn = 0;
    gc   = littleTextGC;
    break;

  case TextBelowStaveItalic:
    ty   = y + STAVE_Y_COORD(-5) + NoteHeight*7/2;
    dirn = 0;
    gc   = italicTextGC;
    break;

  case TextChordName:
    ty   = y - 25;
    dirn = -1;
    gc   = chordNameGC;
    break;

  case TextDynamic:
    ty   = y + STAVE_Y_COORD(-5) + NoteHeight*7/2;
    dirn = 0;
    gc   = dynamicTextGC;
    break;
  }

  if (dirn != 0) {

    if (XGetGCValues(display, timeSigGC, GCFont, &values) == 0)
      Error("Could not get text graphics-context values");

    XQueryTextExtents(display, values.font, text->text.text,
		      1, &dir, &asc, &desc, &info);
    ty += dirn * asc;
  }

  XDrawString(display, drawable, gc, x, ty,
	      text->text.text, strlen(text->text.text));

  Return(width);
}

/* }}} */
/* {{{ Rest */

Dimension DrawRest(MusicObject obj, Drawable drawable, Position x,
		   Position y, Pitch offset, Dimension width, LineEquation eqn)
{
  Rest *rest = (Rest *)obj;

  Begin("DrawRest");
  
  if (!obj) Return(width);
  rest->item.x = x;

  CopyArea(rest->rest.visual->pixmap, drawable, 0, 0,
	   rest->rest.visual->width, StaveHeight, x, y);

  if (rest->rest.visual->dotted)
    CopyArea(noteDotMap, drawable, 0, 0, DotWidth, NoteHeight,
	     x + rest->rest.visual->width, y + STAVE_Y_COORD(5));

  if (rest->phrase.tied_backward && !tie[0].present) {
    tie[0].present = True;
    tie[0].above   = True;
    tie[0].x       = x + rest->rest.visual->width/3;
  }

  if (rest->phrase.tied_forward) {
    tie[1].present = True;
    tie[1].above   = True;
    tie[1].x       = x + (2*rest->rest.visual->width)/3;
  }
  
  if (width > 0) Return(width);
  else Return(rest->rest.visual->width);
}

/* }}} */

