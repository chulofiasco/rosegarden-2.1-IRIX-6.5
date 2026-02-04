/*
-------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
-------------------------------------------------------------------------------

File Name:	Hooks.h

Description:	Prototypes for hook functions that must be provided to allow access to MIDI
		ports or emulation for specific UNIX system. 

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	18/02/94	AJG		File Created.

-------------------------------------------------------------------------------
*/

#ifndef _MIDI_HOOKS_H_
#define _MIDI_HOOKS_H_

void Midi_PortInSetupHook(void);
void Midi_PortOutSetupHook(void);
void Midi_PortReadByteHook(void);
void Midi_PortWriteByteHook(void);

#endif

