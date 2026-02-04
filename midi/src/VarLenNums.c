/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	VarLenNums.c

Description:	Simple functions to encode/decode MIDI variable length number representation.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	24/01/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#include <Debug.h>

#include <MidiConsts.h>
#include <MidiVarLenNums.h>


/**************************************************************/
/* Function: Midi_ConvFixedToVariable. Convert a long integer */
/* into a midi variable length encoded integer.		      */
/**************************************************************/
			
VarLengthNum Midi_ConvFixedToVariable( long FixedFormatValue )
{
long Buffer;

BEGIN("Midi_ConvFixedToVariable");

	
	Buffer = FixedFormatValue & 0x7F;

	while ((FixedFormatValue >>= 7) > 0)
	{
		Buffer <<= 8;
		Buffer |= 0x80;
		Buffer += FixedFormatValue & 0x7F;
	}

RETURN_LONG(Buffer);
}




/****************************************************************/
/* Function: Midi_ConvVariableToFixed. Convert a MIDI variable- */
/* length encoded integer into a long integer.			*/
/****************************************************************/

long Midi_ConvVariableToFixed( VarLengthNum VarLengthValue )
{
long Buffer;

BEGIN("Midi_ConvVariableToFixed");

	if (VarLengthValue < 0x80)
	{
		RETURN_LONG(VarLengthValue);
	}

	Buffer = 0;

	while (VarLengthValue & 0x80)
	{
		Buffer = (Buffer << 7) | (VarLengthValue & 0x7F);
		VarLengthValue >>= 8;
	}

	Buffer = (Buffer << 7) | (VarLengthValue & 0x7F);
	
RETURN_LONG(Buffer);
}

