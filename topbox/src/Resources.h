
#ifndef _TOPBOX_RESOURCES_
#define _TOPBOX_RESOURCES_

#include "General.h"

#define XtNeditorName        "editorName"
#define XtCEditorName        "EditorName"
#define XtNsequencerName     "sequencerName"
#define XtCSequencerName     "SequencerName"
#define XtNhelpFile          "helpFile"
#define XtCHelpFile          "HelpFile"
#define XtNaboutTextFont     "aboutTextFont"
#define XtCAboutTextFont     "AboutTextFont"
#define XtNhelpTextFont      "helpTextFont"
#define XtCHelpTextFont      "HelpTextFont"
#define XtNhelpXrefFont      "helpXrefFont"
#define XtCHelpXrefFont      "HelpXrefFont"
#define XtNhelpVerbatimFont  "helpVerbatimFont"
#define XtCHelpVerbatimFont  "HelpVerbatimFont"
#define XtNhelpTitleFont     "helpTitleFont"
#define XtCHelpTitleFont     "HelpTitleFont"
#define XtNfoundDefaults     "foundDefaults"
#define XtCFoundDefaults     "FoundDefaults"
#define XtNshouldWarpPointer "shouldWarpPointer"
#define XtCShouldWarpPointer "ShouldWarpPointer"


static XtResource resources[] = {
  {
    XtNeditorName,
    XtCEditorName,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, editorName),
    XtRImmediate,
    (XtPointer)NULL,
  },
  {
    XtNsequencerName,
    XtCSequencerName,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr, sequencerName),
    XtRImmediate,
    (XtPointer)NULL,
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
    XtNhelpFile,
    XtCHelpFile,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr,helpFile),
    XtRImmediate,
    "/usr/local/lib/rosegarden/rosehelp.info",
  },
  {
    XtNhelpTextFont,
    XtCHelpTextFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr,helpTextFont),
    XtRImmediate,
    "-*-lucida-bold-r-*-*-14-*-75-*-*-*-*-*",
  },
  {
    XtNhelpXrefFont,
    XtCHelpXrefFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr,helpXrefFont),
    XtRImmediate,
    "-*-lucida-bold-i-*-*-14-*-75-*-*-*-*-*",
  },
  {
    XtNhelpVerbatimFont,
    XtCHelpVerbatimFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr,helpVerbatimFont),
    XtRImmediate,
    "-*-lucidatypewriter-medium-r-*-*-14-*-75-*-*-*-*-*",
  },
  {
    XtNhelpTitleFont,
    XtCHelpTitleFont,
    XtRString,
    sizeof(char *),
    XtOffset(AppDataPtr,helpTitleFont),
    XtRImmediate,
    "-*-lucida-medium-r-*-*-18-*-75-*-*-*-*-*",
  },
  {
    XtNshouldWarpPointer,
    XtCShouldWarpPointer,
    XtRBoolean,
    sizeof(Boolean),
    XtOffset(AppDataPtr, shouldWarpPointer),
    XtRImmediate,
    (XtPointer)False,
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
  { "-editorname",    "*editorName",       XrmoptionSepArg,         NULL },
  { "-sequencername", "*sequencerName",    XrmoptionSepArg,         NULL },
  { "-aboutfn",       "*aboutTextFont",    XrmoptionSepArg,         NULL },
  { "-helptextfn",    "*helpTextFont",     XrmoptionSepArg,         NULL },
  { "-helpxreffn",    "*helpXrefFont",     XrmoptionSepArg,         NULL },
  { "-helptitlefn",   "*helpTitleFont",    XrmoptionSepArg,         NULL },
  { "-helpverbfn",    "*helpVerbatimFont", XrmoptionSepArg,         NULL },
  { "-helpfile",      "*helpFile",         XrmoptionSepArg,         NULL },
  { "-warppointer",   "*shouldWarpPointer",XrmoptionSepArg,         NULL },
  { "-foundDefaults", "*foundDefaults",    XrmoptionNoArg, (XtPointer)True },
};


#endif /* _TOPBOX_RESOURCES_ */
