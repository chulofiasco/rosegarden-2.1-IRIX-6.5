/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           Message.c
 *
 *    Description:    Provides functions for manipulating the message
 *                    window in the sequencer interface.
 *    Author:         AJG
 *
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *
 *    001     18/02/94        AJG             File Created.
 *
 */

#include <MidiXInclude.h>
#include <MidiFile.h>
#include "Globals.h"
#include "Message.h"

#include <Debug.h>

void Midi_DisplayPermanentMessage(String NewMsg)
{
BEGIN("Midi_DisplayPermanentMessage");

	XtUnmanageChild(MsgLabel);

	YSetValue(MsgLabel, XtNlabel, NewMsg);
	YSetValue(MsgLabel, XtNwidth, MsgLabelWidth);

	XtManageChild(MsgLabel);

	XSync(display, False);

END;
}



