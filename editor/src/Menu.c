/* Menu.c */

/*
   Musical Notation Editor for X, Chris Cannam 1994-97
   Menu handling
*/

/* {{{ Includes */

#include "General.h"
#include "IO.h"
#include "Widgets.h"
#include "Visuals.h"
#include "Menu.h"
#include "Undo.h"

#include "text_above.xbm"
#include "text_above_large.xbm"
#include "text_below.xbm"
#include "text_below_italic.xbm"

#include <Yawn.h>
#include <YSmeBSB.h>
#include <ILClient.h>

#include "toolbar/undo.xbm"
#include "toolbar/redo.xbm"
#include "toolbar/ninja_cross.xbm"
#include "toolbar/cut.xbm"
#include "toolbar/copy.xbm"
#include "toolbar/paste.xbm"
#include "toolbar/edit.xbm"
#include "toolbar/chord.xbm"
#include "toolbar/slur.xbm"
#include "toolbar/tie.xbm"
#include "toolbar/beam.xbm"
#include "toolbar/tuplet.xbm"
#include "toolbar/break.xbm"
#include "toolbar/text1.xbm"
#include "toolbar/text2.xbm"
#include "toolbar/text3.xbm"
#include "toolbar/text4.xbm"
#include "toolbar/dynamic.xbm"
#include "toolbar/crescendo.xbm"
#include "toolbar/decrescendo.xbm"

#include "chordmod_pause.xbm"

/* }}} */

/* {{{ Menu structure definitions */

#define GroupMenuGrace Unimplemented

static YMenuElement editMenu[] = {
  { "Undo",           FileNotLoadedMode | UndoUnavailableMode | SlaveToSequencerMode,
  EditMenuUndo,          undo_bits, NULL, },
  { "Redo",           FileNotLoadedMode | RedoUnavailableMode | SlaveToSequencerMode,
  EditMenuRedo,          redo_bits, NULL, },
  YMenuDivider,  
  { "Delete",         NoAreaSweptMode | SlaveToSequencerMode,         EditMenuDelete,        ninja_cross_bits, NULL, },
  YMenuDivider,
  { "Cut",            NoAreaSweptMode | SlaveToSequencerMode,         EditMenuCut,           cut_bits, NULL, },
  { "Copy",           NoAreaSweptMode | SlaveToSequencerMode,         EditMenuCopy,          copy_bits, NULL, },
  { "Paste",          CursorNotPlacedMode | SlaveToSequencerMode,     EditMenuPaste,         paste_bits, NULL, },
  { "Show Clipboard", NullMode | SlaveToSequencerMode,                EditMenuShowClipboard, NULL, NULL, },
  YMenuDivider,
  { "Select this Bar",     CursorNotPlacedMode | SlaveToSequencerMode,     EditMenuSelectBar,    NULL, NULL, },
  { "Select this Staff",   CursorNotPlacedMode | SlaveToSequencerMode,     EditMenuSelectStaff,  NULL, NULL, },
};

static YMenuId editMenuId = NULL;


YMenuElement chordMenu[] = {
  { "Create Chord", NoAreaSweptMode | SlaveToSequencerMode, ChordMenuChord, chord_bits, NULL, },
  { "Create by Name . . .", CursorNotPlacedMode | SlaveToSequencerMode, ChordMenuCreateChord, NULL, NULL },
  { "Edit Chord . . .", NoAreaSweptMode | SlaveToSequencerMode | MultipleItemsSweptMode, ChordMenuChangeChord, edit_bits, NULL },
  { "Pitch Search and Replace . . .", NoAreaSweptMode, ChordMenuSearchReplace, NULL, NULL },
  YMenuDivider,
  /*  { "Rename Chords", NoAreaSweptMode | SlaveToSequencerMode, ChordMenuNameChord, NULL, NULL },*/
  { "Guess Chord Names", NoAreaSweptMode | SlaveToSequencerMode, ChordMenuNameChord, NULL, NULL },
  /*  { "Restore Original Names", NoAreaSweptMode | SlaveToSequencerMode, ChordMenuUnnameChord, NULL, NULL },*/
  { "Clear Chord Names", NoAreaSweptMode | SlaveToSequencerMode, ChordMenuUnnameChord, NULL, NULL },
  YMenuDivider,
  { "Label No Chords", FileNotLoadedMode | SlaveToSequencerMode, ChordMenuHideNames, NULL, NULL },
  { "Label Named Chords Only", FileNotLoadedMode | SlaveToSequencerMode, ChordMenuShowNames, NULL, NULL },
  { "Label All Chords", FileNotLoadedMode | SlaveToSequencerMode, ChordMenuShowAllNames, NULL, NULL },
  { "Label Chords and Notes", FileNotLoadedMode | SlaveToSequencerMode, ChordMenuShowAllNoteNames, NULL, NULL },
  YMenuDivider,
  { "Break Chord", NoAreaSweptMode | SlaveToSequencerMode, ChordMenuUnchord, NULL, NULL, },
};

static YMenuId chordMenuId = NULL;
int chordMenuFirstChoiceIndex = 8;


YMenuElement staveMenu[] = {
  { "Add Staff . . .",   FileNotLoadedMode | SlaveToSequencerMode,       StaveMenuNewStave,     NULL, NULL, },
  { "Delete Staff",   CursorNotPlacedMode | SlaveToSequencerMode,     StaveMenuDeleteStave,  NULL, NULL, },
  { "Empty Staff",    CursorNotPlacedMode | SlaveToSequencerMode,     StaveMenuEmptyStave,   NULL, NULL, },
  { "Rename Staff . . .", CursorNotPlacedMode | SlaveToSequencerMode,    StaveMenuRenameStave,  NULL, NULL, },
  YMenuDivider,
  { "Show MIDI Patches", FileNotLoadedMode | SlaveToSequencerMode,    StaveMenuShowPatches,  NULL, NULL, },
  { "Assign MIDI Patch . . .", CursorNotPlacedMode | SlaveToSequencerMode,    StaveMenuAssignPatch,  NULL, NULL, },
  YMenuDivider,
  { "Bracket with Next",  CursorNotPlacedMode | SlaveToSequencerMode,     StaveMenuConnectBelow, NULL, NULL, },
  { "Unbracket",     CursorNotPlacedMode | SlaveToSequencerMode,     StaveMenuDisconnect,   NULL, NULL, },
  YMenuDivider,
  { "Fill to End",     CursorNotPlacedMode | SlaveToSequencerMode,     StaveMenuFillToEnd,   NULL, NULL, },
};

static YMenuId staveMenuId = NULL;
int staveMenuShowPatchesIndex = 5;


static YMenuElement markMenu[] = {
  { "Accent",         NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Staccato",       NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Legato",         NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Sforzando",      NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Rinforzando",    NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Trill",          NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Turn",           NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  { "Pause",          NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuMark,          NULL, NULL, },
  YMenuDivider,
  { "Unmark",         NoAreaSweptMode | SlaveToSequencerMode,         MarkMenuUnmark,        NULL, NULL, },
};

static YMenuId markMenuId = NULL;


static YMenuElement groupMenu[] = {
  { "Beam",           NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuBeam,         beam_bits, NULL, },
  /*  { "Grace",          NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuGrace,        NULL, NULL, },*/
  { "Tuplet . . .",   NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuTuplet,       NULL, NULL, },
  { "Simple Tuplet",  NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuSimpleTuplet, tuplet_bits, NULL, },
  { "Break Group",    NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuBreakGroup,   break_bits, NULL, },
  YMenuDivider,
  { "Tie",            NoAreaSweptMode | SlaveToSequencerMode | SingleItemSweptMode,         GroupMenuTie,         tie_bits, NULL, },
  { "Slur",           NoAreaSweptMode | SlaveToSequencerMode | SingleItemSweptMode,         GroupMenuSlur,         slur_bits, NULL, },
  { "Crescendo",      NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuCrescendo,    crescendo_bits, NULL, },
  { "Decrescendo",    NoAreaSweptMode | SlaveToSequencerMode,         GroupMenuDecrescendo,  decrescendo_bits, NULL, }, 
  { "Remove Indications", NoAreaSweptMode | SlaveToSequencerMode,      GroupMenuRemove,       NULL, NULL, },
  YMenuDivider,
  { "Auto-Beam",      FileNotLoadedMode | SlaveToSequencerMode,       GroupMenuAutoBeam,     NULL, NULL, },
  { "Transpose . . .",   FileNotLoadedMode | SlaveToSequencerMode,       GroupMenuTranspose,    NULL, NULL, },
  /* JPff insertion */
  { "Invert . . .",      FileNotLoadedMode | SlaveToSequencerMode,       GroupMenuInvert,       NULL, NULL, },
  { "Retrograde",  FileNotLoadedMode | SlaveToSequencerMode,       GroupMenuRetrograde,   NULL, NULL, },
};

static YMenuId groupMenuId = NULL;


static YMenuElement textMenu[] = {
  /* resource file adds the dots to the first four entries */
  { "Text Above",       CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuTextAbove,     text1_bits, NULL, },
  { "Text Above, Large",       CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuTextAboveBig,  text2_bits, NULL, },
  { "Text Below",       CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuTextBelow,     text3_bits, NULL, },
  { "Text Below, Italic",       CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuTextBelowI,    text4_bits, NULL, },
  YMenuDivider,
  { "Dynamic . . .",    CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuDynamic,  dynamic_bits, NULL, },
  YMenuDivider,
  { "Clear Group",    NoAreaSweptMode | SlaveToSequencerMode,         TextMenuUnlabelGroup,  NULL, NULL, },
  { "Clear Bar",      CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuUnlabelBar,    NULL, NULL, },
  { "Clear Staff",    CursorNotPlacedMode | SlaveToSequencerMode,     TextMenuUnlabelStaff,  NULL, NULL, },
};

static YMenuId textMenuId = NULL;

static YMenuElement barMenu[] = {
  { "Plain",          CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuPlain,          NULL, NULL, },
  { "Double Bar",     CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuDoubleBar,      NULL, NULL, },
  { "Repeat at Start", CursorNotPlacedMode | SlaveToSequencerMode,    BarMenuRepeatAtStart,  NULL, NULL, },
  { "Repeat at End",  CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuRepeatAtEnd,    NULL, NULL, },
  YMenuDivider,
  { "End Bar Here",  CursorNotPlacedMode | SlaveToSequencerMode,      BarMenuEndHere,    NULL, NULL, },
  { "Remove Barlines", FileNotLoadedMode | SlaveToSequencerMode, BarMenuRemoveBarlines, NULL, NULL, },
  { "Refresh Barlines", FileNotLoadedMode | SlaveToSequencerMode, BarMenuRefreshBarlines, NULL, NULL, },
  YMenuDivider,
  { "Label . . .",       CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuLabel,          NULL, NULL, },
  { "Remove Label",   CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuUnlabel,        NULL, NULL, },
  YMenuDivider,
  { "Metronome . . .",   CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuMetronome,      NULL, NULL, },
  { "Remove Metronome", CursorNotPlacedMode | SlaveToSequencerMode,   BarMenuRemoveMet,      NULL, NULL, },
  YMenuDivider,
  { "Key Signature . . .",   CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuKeySignature,      NULL, NULL, },
  { "Time Signature . . .", CursorNotPlacedMode | SlaveToSequencerMode,     BarMenuTimeSignature,  NULL, NULL, },
};

static YMenuId barMenuId = NULL;

extern YMenuId filterMenuId;

/* }}} */
/* {{{ Admin functions */

void IssueMenuComplaint(String s)
{
  Begin("IssueMenuComplaint");

  XBell(display, 70);
  (void)YQuery(topLevel, s, 1, 0, 0, "Okay", NULL);

  End;
}


void Unimplemented(Widget w, XtPointer a, XtPointer b)
{
  Begin("Unimplemented");
  (void)YQuery(topLevel, "Sorry, that function is not yet implemented.",
		1, 0, 0, "Continue", NULL);
  End;
}

void EnterMenuMode(MenuMode mode)
{
  Begin("EnterMenuMode");

  YEnterMenuMode(    fileMenuId, mode );
  YEnterMenuMode(    editMenuId, mode );
  YEnterMenuMode(   chordMenuId, mode );
  YEnterMenuMode(   groupMenuId, mode );
  YEnterMenuMode(   staveMenuId, mode );
  YEnterMenuMode(     barMenuId, mode );
  YEnterMenuMode(    markMenuId, mode );
  YEnterMenuMode(    textMenuId, mode );
  YEnterMenuMode(  filterMenuId, mode );

  End;
}


void LeaveMenuMode(MenuMode mode)
{
  Begin("LeaveMenuMode");
  
  YLeaveMenuMode(    fileMenuId, mode );
  YLeaveMenuMode(    editMenuId, mode );
  YLeaveMenuMode(   chordMenuId, mode );
  YLeaveMenuMode(   groupMenuId, mode );
  YLeaveMenuMode(   staveMenuId, mode );
  YLeaveMenuMode(     barMenuId, mode );
  YLeaveMenuMode(    markMenuId, mode );
  YLeaveMenuMode(    textMenuId, mode );
  YLeaveMenuMode(  filterMenuId, mode );

  End;
}


Boolean QueryMenuMode(MenuMode mode)
{
  Begin("QueryMenuMode");

  /* used to be queried on fileMenuId -- but that's remade every time
     you load a new file, so the mode wasn't persisting correctly */

  if (editMenuId) Return(YQueryMenuMode(editMenuId, mode));
  Return(False);
}


void MenuCleanUp(void)
{
  Begin("MenuCleanUp");

  FileMenuCleanUp();

  if (    editMenuId) YDestroyMenu(    editMenuId);
  if (   groupMenuId) YDestroyMenu(   groupMenuId);
  if (   chordMenuId) YDestroyMenu(   chordMenuId);
  if (     barMenuId) YDestroyMenu(     barMenuId);
  if (   staveMenuId) YDestroyMenu(   staveMenuId);
  if (    textMenuId) YDestroyMenu(    textMenuId);
  if (    markMenuId) YDestroyMenu(    markMenuId);

  End;
}

/* }}} */
/* {{{ Install Menu functions */

void InstallEditMenu(Widget edit)
{
  Begin("InstallEditMenu");

  editMenuId = YCreateMenu(edit, "Edit Menu", XtNumber(editMenu), editMenu);
  UndoSetUndoMenuButtons(editMenu[0].widget, editMenu[1].widget);

  End;
}


void InstallStaveMenu(Widget stavew)
{
  Widget w;
  Begin("InstallStaveMenu");

  staveMenuId =
    YCreateMenu(stavew, "Stave Menu", XtNumber(staveMenu), staveMenu);

  w = staveMenu[staveMenuShowPatchesIndex].widget;
  YSetValue(w, XtNleftMargin, 21);
  YSetValue(w, XtNleftBitmap, yToggleOnMap);

  End;
}


void InstallGroupMenu(Widget group)
{
  Begin("InstallGroupMenu");

  groupMenuId =
    YCreateMenu(group, "Group Menu", XtNumber(groupMenu), groupMenu);

  End;
}


void InstallMarkMenu(Widget mark)
{
  int    i, j;
  Pixmap map;

  Begin("InstallMarkMenu");

  markMenuId =
    YCreateMenu(mark, "Mark Menu", XtNumber(markMenu), markMenu);

  for (i = 0; i < XtNumber(markMenu) - 2; ++i) {

    YSetValue(markMenu[i].widget, XtNleftMargin, chordmod_pause_width + /*NoteWidth +*/ 12);

    for (j = 0; j < chordModVisualCount; ++j)
      if (!strncasecmp(chordModVisuals[j].name, markMenu[i].label,
		       strlen(markMenu[i].label))) break;

    if (j < chordModVisualCount) {

      /* Nasty dependence on Pause dimensions here -- should do
         something about this */

      if (chordModVisuals[j].type == ModPause) {
	map = XCreateBitmapFromData
	  (display, RootWindowOfScreen(XtScreen(topLevel)),
	   chordModVisuals[j].bitmap, chordmod_pause_width,
	   chordmod_pause_height);
      } else {
	map = XCreateBitmapFromData
	  (display, RootWindowOfScreen(XtScreen(topLevel)),
	   chordModVisuals[j].bitmap, NoteWidth, ChordModHeight);
      }

      YSetValue(markMenu[i].widget, XtNleftBitmap, map);
    }
  }

  End;
}
  


void InstallTextMenu(Widget text)
{
  int    i;
  Pixmap map;

  Begin("InstallTextMenu");

  textMenuId =
    YCreateMenu(text, "Text Menu", XtNumber(textMenu), textMenu);

  for (i = 0;
       i < XtNumber(textMenu) &&
       (textMenu[i].label ? strncmp(textMenu[i].label,"Text",4) : True); ++i);

  if (i < XtNumber(textMenu)) {

    map = XCreateBitmapFromData
      (display, RootWindowOfScreen(XtScreen(text)), text_above_bits,
       text_above_width, text_above_height);
    YSetValue(textMenu[i].widget, XtNleftMargin, text_above_width + 9);
    YSetValue(textMenu[i].widget, XtNleftBitmap, map); i++;

    map = XCreateBitmapFromData
      (display, RootWindowOfScreen(XtScreen(text)), text_above_large_bits,
       text_above_large_width, text_above_large_height);
    YSetValue(textMenu[i].widget, XtNleftMargin, text_above_large_width + 9);
    YSetValue(textMenu[i].widget, XtNleftBitmap, map); i++;

    map = XCreateBitmapFromData
      (display, RootWindowOfScreen(XtScreen(text)), text_below_bits,
       text_below_width, text_below_height);
    YSetValue(textMenu[i].widget, XtNleftMargin, text_below_width + 9);
    YSetValue(textMenu[i].widget, XtNleftBitmap, map); i++;

    map = XCreateBitmapFromData
      (display, RootWindowOfScreen(XtScreen(text)), text_below_italic_bits,
       text_below_italic_width, text_below_italic_height);
    YSetValue(textMenu[i].widget, XtNleftMargin, text_below_italic_width + 9);
    YSetValue(textMenu[i].widget, XtNleftBitmap, map);
  }

  End;
}


void InstallBarMenu(Widget bar)
{
  Begin("InstallBarMenu");

  barMenuId =
    YCreateMenu(bar, "Bar Menu", XtNumber(barMenu), barMenu);

  End;
}


void InstallChordMenu(Widget chord)
{
  int i;
  Widget w;
  Begin("InstallChordMenu");

  chordMenuId =
    YCreateMenu(chord, "Chord Menu", XtNumber(chordMenu), chordMenu);

  for (i = 0; i < 4; ++i) {
    w = chordMenu[chordMenuFirstChoiceIndex + i].widget;
    YSetValue(w, XtNleftMargin, 21);
    YSetValue(w, XtNleftBitmap, i == 1 ? yToggleOnMap : yToggleOffMap);
  }

  End;
}

/* }}} */
