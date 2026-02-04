/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Resources.h
 *
 *    Description:    Definitions of application resources.
 *
 *    Author:         AJG
 *
 * History:
 *
 * Update       Date            Programmer      Comments
 * ======       ====            ==========      ========
 * 001          24/01/94        AJG             File Created.
 * 002          02/10/94        JPff            Added resource for midiPort
 * 003          30/05/96        rwb             Changed default midiPort
 * 004          24/07/96	cc		Added midiFmPatchFile
 */


#define XtNaboutTextFont     "aboutTextFont"
#define XtCAboutTextFont     "AboutTextFont"
#define XtNinterlockWindow   "interlockWindow"
#define XtCInterlockWindow   "InterlockWindow"
#define XtNfoundDefaults     "foundDefaults"
#define XtCFoundDefaults     "FoundDefaults"
#define XtNmusicDirectory    "musicDirectory"
#define XtCmusicDirectory    "MusicDirectory"
#define XtNacceleratorTable  "acceleratorTable"
#define XtCacceleratorTable  "AcceleratorTable"
#define XtNmidiPort	     "midiPort"
#define XtCmidiPort	     "MidiPort"
#define XtNexternalPlayer    "externalPlayer"
#define XtCexternalPlayer    "ExternalPlayer"
#define XtNshouldWarpPointer "shouldWarpPointer"
#define XtCShouldWarpPointer "ShouldWarpPointer"
#define XtNmidiFmPatchFile   "midiFmPatchFile"
#define XtCMidiFmPatchFile   "MidiFmPatchFile"
#define XtNmidiFmDrumPFile   "midiFmDrumPFile"
#define XtCMidiFmDrumPFile   "MidiFmDrumPFile"
#define XtNscorePlayTracking "scorePlayTracking"
#define XtCScorePlayTracking "ScorePlayTracking"
#define XtNfiltersDirectory  "filtersDirectory"
#define XtCFiltersDirectory  "FiltersDirectory"

static XtResource Midi_Resources[] = 
{
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
    		XtNfoundDefaults,
    		XtCFoundDefaults,
    		XtRBoolean,
    		sizeof(Boolean),
    		XtOffset(AppDataPtr, foundDefaults),
    		XtRImmediate,
    		(XtPointer)False,
  	},
	{
	  	XtNaboutTextFont,
    		XtCAboutTextFont,
    		XtRString,
    		sizeof(char *),
    		XtOffset(AppDataPtr, aboutTextFont),
    		XtRImmediate,
    		(XtPointer)"-*-lucida-medium-r-*-*-18-*-*-*-*-*-*-*",
  	},
	{
		XtNmusicDirectory,
		XtCmusicDirectory,
		XtRString,
		sizeof(char *),
		XtOffset(AppDataPtr, musicDirectory),
		(XtPointer)NULL,
	},
 	{
 	    	XtNmidiPort,
     		XtCmidiPort,
     		XtRString,
     		sizeof(char *),
     		XtOffset(AppDataPtr, midiPortName),
     		XtRImmediate,
#ifdef SYSTEM_OSS
     		(XtPointer)"/dev/sequencer",
#else
#ifdef SYSTEM_SGI
		(XtPointer)"internal",
#else
#ifdef SYSTEM_ZILOG
                (XtPointer)"/dev/ttyd2",
#else
                (XtPointer)"/dev/null",
#endif
#endif
#endif
   	},
	{
	    	XtNacceleratorTable,
    		XtCacceleratorTable,
    		XtRString,
    		sizeof(char *),
    		XtOffset(AppDataPtr, acceleratorTable),
    		XtRImmediate,
    		(XtPointer)NULL,
  	},
	{
	    	XtNexternalPlayer,
    		XtCexternalPlayer,
    		XtRString,
    		sizeof(char *),
    		XtOffset(AppDataPtr, externalPlayer),
    		XtRImmediate,
    		(XtPointer)"",
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
		XtNmidiFmPatchFile,
		XtCMidiFmPatchFile,
		XtRString,
		sizeof(String),
		XtOffset(AppDataPtr, midiFmPatchFile),
		XtRImmediate,
		(XtPointer)"/usr/local/lib/rosegarden/std.sb",
	},
	{
		XtNmidiFmDrumPFile,
		XtCMidiFmDrumPFile,
		XtRString,
		sizeof(String),
		XtOffset(AppDataPtr, midiFmDrumPFile),
		XtRImmediate,
		(XtPointer)"/usr/local/lib/rosegarden/drums.sb",
	},
	{
	        XtNscorePlayTracking,
	        XtCScorePlayTracking,
	        XtRBoolean,
	        sizeof(Boolean),
	        XtOffset(AppDataPtr, scorePlayTracking),
	        XtRImmediate,
	        (XtPointer)True,
	},
        {
           XtNfiltersDirectory,
           XtCFiltersDirectory,
           XtRString,
           sizeof(char *),
           XtOffset(AppDataPtr, filtersDirectory),
           XtRImmediate,
           (XtPointer)"/usr/local/lib/rosegarden/rosepetal-filters",
        }

};


static XrmOptionDescRec Midi_CommandOptions[] = 
{
    { "-aboutfn",     "*aboutTextFont",     XrmoptionSepArg, NULL },
    { "-ILTopBoxWin", "*interlockWindow",   XrmoptionSepArg, NULL },
    { "-midiport",    "*midiPort",          XrmoptionSepArg, NULL },
    { "-midipatchfile","*midiFmPatchFile",  XrmoptionSepArg, NULL },
    { "-mididrumpfile","*midiFmDrumPFile",  XrmoptionSepArg, NULL },
    { "-filtersdir",  "*filtersDirectory",  XrmoptionSepArg, NULL },
};
