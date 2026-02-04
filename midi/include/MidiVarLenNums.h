/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	VarLenNums.h

Description:	Prototypes for Variable-Length number manipulation functions.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	24/01/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

typedef long VarLengthNum;

typedef union
{
	VarLengthNum	Number;
	byte		ByteField[4];
}
VarLengthNumBuffer;

VarLengthNum Midi_ConvFixedToVariable( long FixedFormatValue );
long Midi_ConvVariableToFixed( VarLengthNum VarLengthValue );


