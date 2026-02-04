
/* {{{ Includes */

#include "General.h"
#include "Tags.h"
#include "Visuals.h"
#include "Palette.h"
#include "GC.h"
#include "Notes.h"
#include "Classes.h"
#include "Yawn.h"
#include "Stave.h"
#include "StaveEdit.h"
#include "StaveCursor.h"

#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Paned.h>

/* }}} */
/* {{{ Palette Mod records */

#define PaletteModCount 4

typedef struct _PaletteModRec {
  String        name;
  Dimension     width;
  Dimension     height;
  Pixmap       *pixmap;
  unsigned long availablePalettes;
} PaletteModRec;

static PaletteModRec paletteMods[] = {
  {
    "Sharp",   NoteModWidth, NoteModHeight,
    &(noteModVisuals[0].pixmap), 1L << PaletteNotes,
  }, {
    "Flat",    NoteModWidth, NoteModHeight,
    &(noteModVisuals[1].pixmap), 1L << PaletteNotes,
  }, {
    "Natural", NoteModWidth, NoteModHeight,
    &(noteModVisuals[2].pixmap), 1L << PaletteNotes,
  }, {
    "Dot",     DotWidth,     NoteHeight,
    &(noteDotMap), (1L << PaletteNotes) | (1L << PaletteRests),
  },
};

/* }}} */
/* {{{ Local variable declarations */

static Widget paletteModWidgets[PaletteModCount];

static Widget paletteShellLocal;
static Widget paletteForm;
static Widget paletteNoteForm;
static Widget paletteRestForm;
static Widget paletteClefForm;

static Widget **paletteWidgets = 0;

static Widget paletteFollowToggle;
static Boolean paletteFollowKey = True;

/* }}} */

/* {{{ Note mod manipulations */

Boolean PaletteModDottedQuery(void)
{
  Boolean rtn = False;
  
  Begin("PaletteModDottedQuery");

  if (paletteModWidgets[3])
    YGetValue(paletteModWidgets[3], XtNstate, &rtn);

  Return(rtn);
}

static Boolean prevDot;
void PalettePushDot(Boolean dot)
{
  if (paletteModWidgets[3]) {
    YGetValue(paletteModWidgets[3], XtNstate, &prevDot);
    YSetValue(paletteModWidgets[3], XtNstate, dot);
  }
}
void PalettePopDot(void)
{
  if (paletteModWidgets[3]) {
    YSetValue(paletteModWidgets[3], XtNstate, prevDot);
  }
}

static Boolean prevSharp;
void PalettePushSharp(Boolean sharp)
{
  if (paletteModWidgets[0]) {
    YGetValue(paletteModWidgets[0], XtNstate, &prevSharp);
    YSetValue(paletteModWidgets[0], XtNstate, sharp);
  }
}
void PalettePopSharp(void)
{
  if (paletteModWidgets[0]) {
    YSetValue(paletteModWidgets[0], XtNstate, prevSharp);
  }
}

static Boolean prevFlat;
void PalettePushFlat(Boolean flat)
{
  if (paletteModWidgets[1]) {
    YGetValue(paletteModWidgets[1], XtNstate, &prevFlat);
    YSetValue(paletteModWidgets[1], XtNstate, flat);
  }
}
void PalettePopFlat(void)
{
  if (paletteModWidgets[1]) {
    YSetValue(paletteModWidgets[1], XtNstate, prevFlat);
  }
}

static Boolean prevNatural;
void PalettePushNatural(Boolean natural)
{
  if (paletteModWidgets[2]) {
    YGetValue(paletteModWidgets[2], XtNstate, &prevNatural);
    YSetValue(paletteModWidgets[2], XtNstate, natural);
  }
}
void PalettePopNatural(void)
{
  if (paletteModWidgets[2]) {
    YSetValue(paletteModWidgets[2], XtNstate, prevNatural);
  }
}


NoteMods PaletteGetNoteMods(void)
{
  int      i;
  Boolean  curr;
  NoteMods rtn = ModNone;

  Begin("PaletteGetNoteMods");

  for (i = 0; i < 3; ++i) {

    curr = False;

    if (paletteModWidgets[i])
      YGetValue(paletteModWidgets[i], XtNstate, &curr);

    if (curr) rtn |= 1 << i;
  }

  Return(rtn);
}

/* }}} */
/* {{{ Drawing notes, rests & clefs */

void DrawNoteOnPaletteWidget(int i, Widget w)
{
  Pixmap map;
  NoteVoice voice;
  Chord *chord;

  Begin("DrawNoteOnPaletteWidget");

  (void)NewNoteVoice(&voice, 1, ModNone);
  chord = NewChord(NULL, &voice, 1, ModNone,
		   noteVisuals[i].undotted.type, False);

  map = XCreatePixmap(display, RootWindowOfScreen(XtScreen(topLevel)),
		      NoteWidth + 40, StaveHeight + 80 + NoteHeight,
		      DefaultDepthOfScreen(XtScreen(topLevel)));

  XFillRectangle(display, map, clearGC, 0, 0, NoteWidth + 40,
		 StaveHeight + 80 + NoteHeight);

  chord->methods->draw(chord, map, 20, 38 + (i>5 ? 0 : NoteHeight), 0, 0, NULL);

  YSetValue(w, XtNbitmap, map);
  YSetValue(w, XtNlabel, NULL);
  YSetValue(w, XtNborderWidth, 0);
  XtFree((void *)chord);     /* not DestroyChord, 'cos voice is on stack */

  End;
}


void DrawRestOnPaletteWidget(int i, Widget w)
{
  Pixmap map;
  Rest *rest;

  Begin("DrawRestOnPaletteWidget");

  rest = NewRest(NULL, restVisuals[i].undotted.type, False);

  map = XCreatePixmap(display, RootWindowOfScreen(XtScreen(topLevel)),
		      NoteWidth + 40, StaveHeight + 80 + NoteHeight,
		      DefaultDepthOfScreen(XtScreen(topLevel)));

  XFillRectangle(display, map, clearGC, 0, 0, NoteWidth + 40,
		 StaveHeight + 80 + NoteHeight);

  rest->methods->draw(rest, map, 20, 40 + NoteHeight/2, 0, 0, NULL);

  YSetValue(w, XtNbitmap, map);
  YSetValue(w, XtNlabel, NULL);
  YSetValue(w, XtNborderWidth, 0);
  DestroyRest(rest);

  End;
}


void DrawClefOnPaletteWidget(int i, Widget w)
{
  Pixmap map;
  Clef *clef;
  int y;

  Begin("DrawClefOnPaletteWidget");

  clef = NewClef(NULL, clefVisuals[i].type);

  map = XCreatePixmap(display, RootWindowOfScreen(XtScreen(topLevel)),
		      ClefWidth + 4, StaveHeight + 40 + 4*NoteHeight,
		      DefaultDepthOfScreen(XtScreen(topLevel)));

  XFillRectangle(display, map, clearGC, 0, 0, ClefWidth + 4,
		 StaveHeight + 40 + 4*NoteHeight);

  clef->methods->draw(clef, map, 2, 20 + 2*NoteHeight, 0, 0, NULL);

  for (y = 0; y < StaveHeight; y += NoteHeight + 1)
    XDrawLine(display, map, drawingGC,
	      0, y + 20 + 2*NoteHeight, ClefWidth + 3, y + 20 + 2*NoteHeight);
  
  YSetValue(w, XtNbitmap, map);
  YSetValue(w, XtNlabel, NULL);
  YSetValue(w, XtNinternalWidth, 0);
  YSetValue(w, XtNborderWidth, 0);
  DestroyClef(clef);

  End;
}

/* }}} */
/* {{{ Changing mode */

void PaletteCallback(Widget, XtPointer, XtPointer);
static int lastPaletteSelection = 4;

void PaletteChangeMode(Boolean insertMode)
{
  int i, j;
  Boolean state;
  Begin("SwitchPaletteMode");

  StaveCursorRemove(stave);

  if (insertMode) {

    PaletteChangeMode(False);
    j = lastPaletteSelection / 20;
    i = lastPaletteSelection % 20;

    YSetValue(paletteWidgets[j][i], XtNstate, True);
    PaletteCallback(NULL, (XtPointer)(j * noteVisualCount + i), NULL);
    StaveEditEnterInsertMode();

  } else {

    for (j = 0; j < 3; ++j) {
      for (i = 0; i < (j == 0 ? noteVisualCount :
		       j == 1 ? restVisualCount :
		       clefVisualCount); ++i) {
	YGetValue(paletteWidgets[j][i], XtNstate, &state);
	if (state) {
	  lastPaletteSelection = i + j*20;
	  YSetValue(paletteWidgets[j][i], XtNstate, False);
	}
      }
    }

    StaveEditEnterEditMode();
  }

  End;
}

/* }}} */
/* {{{ Callbacks */

Boolean PaletteFollowKey(void)
{
  Begin("PaletteFollowKey");
  Return(paletteFollowKey);
}


void PaletteFollowKeyCallback(Widget w, XtPointer cl, XtPointer ca)
{
  Begin("PaletteFollowKeyCallback");

  paletteFollowKey = YGetToggleValue(paletteFollowToggle);

  End;
}


void PaletteCallback(Widget w, XtPointer cl, XtPointer ca)
{
  int i = (int)cl, j;
  Boolean state;

  Begin("PaletteCallback");;

  if (i >= noteVisualCount + restVisualCount) {
    i -= noteVisualCount + restVisualCount;
    j = 2;
  } else if (i >= noteVisualCount) {
    i -= noteVisualCount;
    j = 1;
  } else j = 0;

  YGetValue(paletteWidgets[j][i], XtNstate, &state);

  if (state) {

    StaveEditEnterInsertMode();

    switch(j) {
    case 0:
      StaveEditAssertInsertVisual(PaletteNotes, noteVisuals, i); break;

    case 1:
      StaveEditAssertInsertVisual(PaletteRests, restVisuals, i); break;

    case 2:
      StaveEditAssertInsertVisual(PaletteClefs, clefVisuals, i); break;

    default: break;
    }
  } else {

    StaveEditEnterEditMode();
  }

  End;
}

/* }}} */
/* {{{ Move up/down keyboard callbacks */

void PaletteMoveDown(void)
{
  int n;
  int i, j;
  int ni, nj;
  Boolean state;
  Begin("PaletteMoveDown");

  for (j = 0; j < 2; ++j) {

    n = (j == 0 ? noteVisualCount : restVisualCount);

    for (i = 0; i < n; ++i) {

      YGetValue(paletteWidgets[j][i], XtNstate, &state);

      if (state) {
	YSetValue(paletteWidgets[j][i], XtNstate, False);

	if (i < n-1) { ni = i+1; nj =   j; }
	else         { ni =   0; nj = 1-j; }

	YSetValue(paletteWidgets[nj][ni], XtNstate, True);
	PaletteCallback(NULL, (XtPointer)(nj * noteVisualCount + ni), NULL);

	End;
      }
    }
  }

  End;
}


void PaletteMoveUp(void)
{
  int n;
  int i, j;
  int ni, nj;
  Boolean state;
  Begin("PaletteMoveUp");

  for (j = 0; j < 2; ++j) {

    n = (j == 0 ? noteVisualCount : restVisualCount);

    for (i = 0; i < n; ++i) {

      YGetValue(paletteWidgets[j][i], XtNstate, &state);

      if (state) {
	YSetValue(paletteWidgets[j][i], XtNstate, False);

	if (i > 0) { ni = i-1; nj = j; }
	else {
	  ni = (j == 1 ? noteVisualCount : restVisualCount) - 1;
	  nj = 1-j;
	}

	YSetValue(paletteWidgets[nj][ni], XtNstate, True);
	PaletteCallback(NULL, (XtPointer)(nj * noteVisualCount + ni), NULL);

	End;
      }
    }
  }

  End;
}


void PaletteSelectNote(NoteTag note)
{
  int n;
  int i, j;
  Boolean state;
  long nNote = note;
  Begin("PaletteSelectNote");

  for (j = 0; j < 2; ++j) {
    n = (j == 0 ? noteVisualCount : restVisualCount);

    for (i = 0; i < n; ++i) {
      YGetValue(paletteWidgets[j][i], XtNstate, &state);
      if (state) YSetValue(paletteWidgets[j][i], XtNstate, False);
    }
  }

  YSetValue(paletteWidgets[0][note], XtNstate, True);
  PaletteCallback(NULL, (XtPointer)nNote, NULL);

  End;
}


/* }}} */
/* {{{ Installation */

void InstallPalettes(Widget pane)
{
  int i;

  Begin("InstallPalettes");

  paletteForm = YCreateShadedWidget
    ("Palette Form", formWidgetClass, pane, LightShade);

  for (paletteShellLocal = paletteForm; XtParent(paletteShellLocal);
       paletteShellLocal = XtParent(paletteShellLocal));

  paletteWidgets = (Widget **)XtMalloc(3 * sizeof(Widget *));
  paletteWidgets[0] = (Widget *)XtMalloc(noteVisualCount * sizeof(Widget));
  paletteWidgets[1] = (Widget *)XtMalloc(restVisualCount * sizeof(Widget));
  paletteWidgets[2] = (Widget *)XtMalloc(clefVisualCount * sizeof(Widget));

  paletteNoteForm = YCreateShadedWidget
    ("paletteNoteForm", formWidgetClass, paletteForm, NoShade);

  paletteRestForm = YCreateShadedWidget
    ("paletteRestForm", formWidgetClass, paletteForm, NoShade);

  YSetValue(paletteRestForm, XtNfromHoriz, paletteNoteForm);

  paletteClefForm = YCreateShadedWidget
    ("paletteClefForm", formWidgetClass, pane, LightShade);

  YSetValue(paletteClefForm, XtNmin, 64);
  YSetValue(paletteClefForm, XtNmax, 64);
  YSetValue(paletteClefForm, XtNdefaultDistance, 0);

  for (i = 0; i < noteVisualCount; ++i) {

    paletteWidgets[0][i] =
      YCreateWidget("note", toggleWidgetClass, paletteNoteForm);

    if (i > 0) {
      YSetValue(paletteWidgets[0][i], XtNfromVert, paletteWidgets[0][i-1]);
      YSetValue(paletteWidgets[0][i], XtNradioGroup, paletteWidgets[0][0]);
      YSetValue(paletteWidgets[0][i], XtNradioData, i);
    }

    XtAddCallback(paletteWidgets[0][i], XtNcallback, PaletteCallback,
		  (XtPointer)i);
    DrawNoteOnPaletteWidget(i, paletteWidgets[0][i]);
  }

  for (i = 0; i < restVisualCount; ++i) {

    paletteWidgets[1][i] =
      YCreateWidget("rest", toggleWidgetClass, paletteRestForm);

    if (i > 0) {
      YSetValue(paletteWidgets[1][i], XtNfromVert, paletteWidgets[1][i-1]);
    }

    YSetValue(paletteWidgets[1][i], XtNradioGroup, paletteWidgets[0][0]);
    YSetValue(paletteWidgets[1][i], XtNradioData, noteVisualCount + i);

    XtAddCallback(paletteWidgets[1][i], XtNcallback, PaletteCallback,
		  (XtPointer)(noteVisualCount + i));
    DrawRestOnPaletteWidget(i, paletteWidgets[1][i]);
  }

  for (i = 0; i < clefVisualCount; ++i) {
    paletteWidgets[2][i] =
      YCreateWidget("clef", toggleWidgetClass, paletteClefForm);

    if (i > 0) {
      YSetValue(paletteWidgets[2][i], XtNfromHoriz, paletteWidgets[2][i-1]);
    }

    YSetValue(paletteWidgets[2][i], XtNradioGroup, paletteWidgets[0][0]);
    YSetValue(paletteWidgets[2][i], XtNradioData,
	      noteVisualCount + restVisualCount + i);

    XtAddCallback(paletteWidgets[2][i], XtNcallback, PaletteCallback,
		  (XtPointer)(noteVisualCount + restVisualCount + i));
    DrawClefOnPaletteWidget(i, paletteWidgets[2][i]);
  }

  End;
}


void InstallPaletteMods(Widget parent)
{
  int       i;
  Dimension wd = 0;
  Dimension ht = 0;
  Pixmap    bitmap;

  Begin("InstallPaletteMods");

  for (i = 0; i < PaletteModCount; ++i) {

    if (paletteMods[i].width  > wd) wd = paletteMods[i].width;
    if (paletteMods[i].height > ht) ht = paletteMods[i].height;
  }

  for (i = 0; i < PaletteModCount; ++i) {

    bitmap = XCreatePixmap(display, RootWindowOfScreen(XtScreen(topLevel)),
			   wd, ht, DefaultDepthOfScreen(XtScreen(topLevel)));

    XFillRectangle(display, bitmap, clearGC, 0, 0, wd, ht);

    CopyArea(*(paletteMods[i].pixmap), bitmap, 0, 0,
	     paletteMods[i].width, paletteMods[i].height,
	     (wd - paletteMods[i].width)/2, (ht - paletteMods[i].height)/2);

    paletteModWidgets[i] =
      YCreateWidget("Palette Mod Item", toggleWidgetClass, parent);

    YSetValue(paletteModWidgets[i], XtNbitmap, bitmap);
    YSetValue(paletteModWidgets[i], XtNlabel,    NULL);
    YSetValue(paletteModWidgets[i], XtNfromHoriz, paletteModWidgets[i-1]);
  }

  End;
}


void InstallPaletteFollowToggle(Widget parent)
{
  Begin("InstallPaletteFollowToggle");
    
  paletteFollowToggle =
    YCreateToggle("Follow key", parent, PaletteFollowKeyCallback);
  YSetToggleValue(paletteFollowToggle, paletteFollowKey);

  End;
}

/* }}} */

