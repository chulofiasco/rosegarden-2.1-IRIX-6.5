/*
---------------------------------------------------------------------------------------------
			MIDI Sequencer - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	ILServer.h

Description:	Prototypes for functions that provide the server module of the Interlock
		messaging system. These functions are to be used when writing the top-box
		for a set of interlock applications. Note that the top-box must have a 
		window, although this window need not actually be visible.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	11/03/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#include "ILTypes.h"

#ifndef __IL_SERVER__
#define __IL_SERVER__

void IL_ServerInit(Widget                 ThisTask, 
		   char                  *ServerName, 
		   IL_DeRegCB             DeRegFunc,
		   IL_ServiceActivateCB   ActivationFunc,
		   IL_ServiceDeactivateCB DeactivationFunc);

pid_t	IL_StartTask(char *ServiceId);
void	IL_AddService(char *ServiceId, char *Path);
void	IL_KillAllTasks(Boolean Noisy);

#endif
