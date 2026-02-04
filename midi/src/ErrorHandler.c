/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	ErrorHandler.c

Description:	Generic Error-handling function for midi sequencer application.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	16/11/93	AJG		File Created.
002	14/12/93	AJG		Added code to display error message boxes (i.e. to
					properly handler NON_FATAL_REPORT_TO_MSGBOX errors)
003	16/12/93	AJG		Changed error message box from being modeless to modal.

--------------------------------------------------------------------------------------------
*/

#include <stdio.h>

#include <Yawn.h>

#include <MidiErrorHandler.h>
#include <MidiFile.h>
#include <Debug.h>

#ifdef YAWN_AVAILABLE

Widget ErrMsgParent = NULL;

void ErrorHandlerInitialise(Widget ErrTopLevel)
{
BEGIN("ErrorHandlerInitialise");

	ErrMsgParent = ErrTopLevel;

END;
}

#endif

void ErrorHandler(error_level  Seriousness,
		  char 	      *ErrorMsg,
		  int	       LineNumber,
		  char	      *FileName)
{
BEGIN("ErrorHandler");

	switch(Seriousness)
	{
	case NON_FATAL_REPORT_TO_MSGBOX:

#ifdef YAWN_AVAILABLE

		if (ErrMsgParent)
		{
			YQuery(ErrMsgParent, ErrorMsg, 1, 0, 0, "Continue", NULL);
			break;
		}
		break;
#endif

	case NON_FATAL_REPORT_TO_STDERR:
		
		fprintf(stderr, "Non-fatal Error: %s\n",
			ErrorMsg);

#ifdef REPORT_LINE_AND_FILE

		fprintf(stderr, "Occurred in %s at line %d.\n\n",
			FileName,
			LineNumber);
#endif

		END;

	case FATAL:

		fprintf(stderr, "Fatal Error: %s\n",
			ErrorMsg);

#ifdef REPORT_LINE_AND_FILE

		fprintf(stderr, "Occurred in %s at line %d.\n\n",
			FileName,
			LineNumber);
#endif
		exit(-1);

	default:

		Error(FATAL, "Invalid Error code");
	}
END;
}



