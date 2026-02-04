
#ifndef _MUSIC_RESOURCES_
#define _MUSIC_RESOURCES_

#include "General.h"


#define XtNtimeSignatureFont "timeSignatureFont"
#define XtCTimeSignatureFont "TimeSignatureFont"
#define XtNbigTextFont       "bigTextFont"
#define XtCBigTextFont       "BigTextFont"
#define XtNlittleTextFont    "littleTextFont"
#define XtCLittleTextFont    "LittleTextFont"
#define XtNitalicTextFont    "italicTextFont"
#define XtCItalicTextFont    "ItalicTextFont"
#define XtNtinyTextFont      "tinyTextFont"
#define XtCTinyTextFont      "TinyTextFont"
#define XtNdynamicTextFont   "dynamicTextFont"
#define XtCDynamicTextFont   "DynamicTextFont"
#define XtNchordNameFont     "chordNameFont"
#define XtCChordNameFont     "ChordNameFont"
#define XtNaboutTextFont     "aboutTextFont"
#define XtCAboutTextFont     "AboutTextFont"
#define XtNinterlockWindow   "interlockWindow"
#define XtCInterlockWindow   "InterlockWindow"
#define XtNfiltersDirectory  "filtersDirectory"
#define XtCFiltersDirectory  "FiltersDirectory"
#define XtNmusicDirectory    "musicDirectory"
#define XtCMusicDirectory    "MusicDirectory"
#define XtNacceleratorTable  "acceleratorTable"
#define XtCAcceleratorTable  "AcceleratorTable"
#define XtNfoundDefaults     "foundDefaults"
#define XtCFoundDefaults     "FoundDefaults"
#define XtNmidiBarEmphasis   "midiBarEmphasis"
#define XtCMidiBarEmphasis   "MidiBarEmphasis"
#define XtNmidiDynamics      "midiDynamics"
#define XtCMidiDynamics      "MidiDynamics"
#define XtNshouldWarpPointer "shouldWarpPointer"
#define XtCShouldWarpPointer "ShouldWarpPointer"

static XtResource resources[] = {
  {
    XtNtimeSignatureFont,
    XtCTimeSignatureFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, timeSignatureFont),
    XtRImmediate,
    (XtPointer)"-*-new century schoolbook-medium-r-*--20-*-*-*-*-*-*-*",
  },
  {
    XtNbigTextFont,
    XtCBigTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, bigTextFont),
    XtRImmediate,
    (XtPointer)"-*-new century schoolbook-medium-r-*--18-*-*-*-*-*-*-*",
  },
  {
    XtNlittleTextFont,
    XtCLittleTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, littleTextFont),
    XtRImmediate,
    (XtPointer)"-*-new century schoolbook-medium-r-*--14-*-*-*-*-*-*-*",
  },
  {
    XtNitalicTextFont,
    XtCItalicTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, italicTextFont),
    XtRImmediate,
    (XtPointer)"-*-new century schoolbook-medium-i-*--14-*-*-*-*-*-*-*",
  },
  {
    XtNtinyTextFont,
    XtCTinyTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, tinyTextFont),
    XtRImmediate,
    (XtPointer)"-*-new century schoolbook-medium-i-*-*-12-*-*-*-*-*-*-*",
  },
  {
    XtNdynamicTextFont,
    XtCDynamicTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, dynamicTextFont),
    XtRImmediate,
    (XtPointer)"-*-new century schoolbook-bold-i-*-*-12-*-*-*-*-*-*-*",
  },
  {
    XtNchordNameFont,
    XtCChordNameFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, chordNameFont),
    XtRImmediate,
    (XtPointer)"-*-lucida-medium-r-*-*-12-*-*-*-*-*-*-*",
  },
  {
    XtNaboutTextFont,
    XtCAboutTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, aboutTextFont),
    XtRImmediate,
    (XtPointer)"-*-lucida-medium-r-*-*-18-*-75-*-*-*-*-*",
  },
  {
    XtNfiltersDirectory,
    XtCFiltersDirectory,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, filtersDirectory),
    XtRImmediate,
    (XtPointer)"/usr/local/lib/rosegarden/rosepetal-filters",
  },
  {
    XtNmusicDirectory,
    XtCMusicDirectory,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, musicDirectory),
    XtRImmediate,
    (XtPointer)".",
  },
  {
    XtNinterlockWindow,
    XtCInterlockWindow,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, interlockWindow),
    XtRImmediate,
    (XtPointer)NULL,
  },
  {
    XtNacceleratorTable,
    XtCAcceleratorTable,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, acceleratorTable),
    XtRImmediate,
    (XtPointer)NULL,
  },
  {
    XtNmidiBarEmphasis,
    XtCMidiBarEmphasis,
    XtRInt,
    sizeof(int),
    XtOffset(AppDataPtr, midiBarEmphasis),
    XtRImmediate,
    (XtPointer)15,
  },
  {
    XtNmidiDynamics,
    XtCMidiDynamics,
    XtRBoolean,
    sizeof(Boolean),
    XtOffset(AppDataPtr, midiDynamics),
    XtRImmediate,
    (XtPointer)False,
  },
  {
    XtNshouldWarpPointer,
    XtCShouldWarpPointer,
    XtRBoolean,
    sizeof(Boolean),
    XtOffset(AppDataPtr, shouldWarpPointer),
    XtRImmediate,
    (XtPointer)True,
  },
  {
    XtNfoundDefaults,
    XtCFoundDefaults,
    XtRBoolean,
    sizeof(Boolean),
    XtOffset(AppDataPtr, foundDefaults),
    XtRImmediate,
    (XtPointer)False,
  }
};


static XrmOptionDescRec commandOptions[] = {
  { "-musicdir",    "*musicDirectory",    XrmoptionSepArg, NULL },
  { "-filtersdir",  "*filtersDirectory",  XrmoptionSepArg, NULL },
  { "-tsigfn",      "*timeSignatureFont", XrmoptionSepArg, NULL },
  { "-bigfn",       "*bigTextFont",       XrmoptionSepArg, NULL },
  { "-littlefn",    "*littleTextFont",    XrmoptionSepArg, NULL },
  { "-tinyfn",      "*tinyTextFont",      XrmoptionSepArg, NULL },
  { "-italicfn",    "*italicTextFont",    XrmoptionSepArg, NULL },
  { "-dynamicfn",   "*dynamicTextFont",   XrmoptionSepArg, NULL },
  { "-aboutfn",     "*aboutTextFont",     XrmoptionSepArg, NULL },
  { "-accelerators","*acceleratorTable",  XrmoptionSepArg, NULL },
  { "-baremphasis", "*midiBarEmphasis",   XrmoptionSepArg, NULL },
  { "-dynamics",    "*midiDynamics",      XrmoptionSepArg, NULL },
  { "-warppointer", "*shouldWarpPointer", XrmoptionSepArg, NULL },
  { "-ILTopBoxWin", "*interlockWindow",   XrmoptionSepArg, NULL },
};


#endif /* _MUSIC_RESOURCES_ */
