/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	ErrorHandler.h

Description:	Defines generic error handling for the midi sequencer application.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/12/93	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

typedef enum
{
	NON_FATAL_REPORT_TO_STDERR,
	NON_FATAL_REPORT_TO_MSGBOX,
	FATAL
}
error_level;

#define Error(X, Y) ErrorHandler(X, Y, __LINE__, __FILE__)

#ifdef YAWN_AVAILABLE

#include <MidiXInclude.h>

void ErrorHandlerInitialise(Widget ErrTopLevel);

#else

#define ErrorHandlerInitialise(x)

#endif

void ErrorHandler(error_level, char *, int, char *);


