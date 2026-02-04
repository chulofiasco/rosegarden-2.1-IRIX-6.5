/*
---------------------------------------------------------------------------------------------
	    Interlock Tool Communication Framework - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	ILClient.h	

Description:	Function Prototypes for Interlock client functions.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	08/02/94	AJG		File Created.

--------------------------------------------------------------------------------------------
*/

#include "ILTypes.h"

#ifndef __IL_CLIENT__
#define __IL_CLIENT__

void IL_ClientInit(Window WindowServer, Widget ThisTask, IL_TerminateCB KillFunc);

void IL_RegisterWithServer(char *ServiceId, IL_ServiceCB ServiceFunc);
void IL_DeRegisterWithServer(char *ServiceId);

void IL_RequestService(char *ServiceId, IL_AckCB CBFunc, char *Data, int DataSize);
void IL_AcknowledgeRequest(char *ServiceId, IL_ReturnCode); 

#endif
