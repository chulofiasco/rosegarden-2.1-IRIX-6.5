/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Types.h
 *
 *    Description:    Definitions of common types.
 *
 *    Author:         AJG
 *
 *    History:
 *
 *    Update    Date            Programmer      Comments
 *    ======    ====            ==========      ========
 *    001       24/01/94        AJG             File Created.
 *
 *
 */

#ifndef	__MIDI_TYPES__
#define __MIDI_TYPES__

#include <MidiXInclude.h>


typedef struct
{
    String    interlockWindow;
    String    aboutTextFont;
    Boolean   foundDefaults;
    String    musicDirectory;
    String    acceleratorTable;
    String    midiPortName;
    String    externalPlayer;
    String    filtersDirectory;
    Boolean   shouldWarpPointer;
    String    midiFmPatchFile;
    String    midiFmDrumPFile;
    Boolean   scorePlayTracking;
    int       defaultDevice;
} 
AppData, *AppDataPtr;

#endif
