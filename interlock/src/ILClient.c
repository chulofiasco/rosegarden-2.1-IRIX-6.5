/*
---------------------------------------------------------------------------------------------
	    Interlock Tool Communication Framework - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	ILClient.c	

Description:	Function Prototypes for Interlock client functions.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	08/02/94	AJG		File Created.
002	27/03/94	AJG		Tidied up handling of NULL callbacks.

--------------------------------------------------------------------------------------------
*/

#include "ILClient.h"
#include <Debug.h>
#include <string.h>

/************************************************************************************/
/* Global variables to store the Server task window ID and service assocation list. */
/************************************************************************************/

Window 			  ILC_Server_Task = 0;
Widget			  ILC_This_Task;
IL_ClientServiceAssocList ILC_Services;
IL_AckCB		  ILC_Current_AckHandler;
IL_TerminateCB		  ILC_KillFunc;
IL_Bool			  ILC_Task_Busy;
IL_Bool			  ILC_Expecting_Response;

Atom ILC_RegisterMsg, ILC_DeRegisterMsg, ILC_RequestMsg, ILC_ResponseMsg, ILC_TerminateMsg;

/********************************/
/* Private function prototypes. */
/********************************/

void IL_EventHandler(Widget w, XtPointer closure, XEvent *event, Boolean *cont);

/*************************************************************/
/* Constructor function for client service association list. */
/*************************************************************/

IL_ClientServiceAssocList IL_CreateServiceList(IL_ClientServiceAssoc NewService)
{
IL_ClientServiceAssocList NewServiceList;

BEGIN("IL_CreateServiceList");

	NewServiceList = (IL_ClientServiceAssocList)NewList(sizeof(IL_ClientServiceAssocListElt));
	NewServiceList->Service = NewService;

RETURN_PTR(NewServiceList);
}
	


/***************************************************************************************/
/* This function handles incoming ClientMessage event. All other events fall through.  */
/* If the incoming client message is not a service request or response then it is also */
/* ignored.									       */
/***************************************************************************************/

void IL_EventHandler(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
IL_ClientServiceAssocList  ServiceList;
IL_Request		  *Msg;
static char 		  *DataBuffer = NULL;
static int		   DataCount  = 0;

BEGIN("IL_EventHandler");

	if (cont) *cont = True;  /* cc '95 */

	if (event->type != ClientMessage) END;

	if (event->xclient.message_type == ILC_RequestMsg)
 	{

		Msg = (IL_Request *)&event->xclient.data.b;

		if (ILC_Task_Busy)
		{
			IL_AcknowledgeRequest(Msg->Service, IL_SERVICE_BUSY);
			END;
		}

		ServiceList = ILC_Services;

		if (DataBuffer) {  /* cc 95 */
		      DataBuffer = (char *)realloc(DataBuffer, 
						   DataCount + IL_MESSAGE_DATA_CHUNK_SIZE);
		  } else {
		      DataBuffer = (char *)malloc(DataCount + IL_MESSAGE_DATA_CHUNK_SIZE);
		  }
		
		if (DataBuffer == NULL) /* cc 95 during bug tracing */
		  {
		      fprintf(stderr, "Interlock Internal Error: "
			      "Malloc failed on %d bytes\n",
			      DataCount + IL_MESSAGE_DATA_CHUNK_SIZE);
		      END;
		  }

		memcpy(DataBuffer + DataCount, Msg->Data, IL_MESSAGE_DATA_CHUNK_SIZE);
		DataCount += IL_MESSAGE_DATA_CHUNK_SIZE;

		if (Msg->MoreToFollow) END;

		while(ServiceList != NULL)
		{
			if (!strncmp(Msg->Service, ServiceList->Service.ServiceId, 4))
			{
				ILC_Task_Busy = True;
				(ServiceList->Service.ServiceFunc)((char *)DataBuffer);
				DataCount = 0;
				DataBuffer = NULL;
				break;
			}

			ServiceList = (IL_ClientServiceAssocList) Next(ServiceList);
		}
	}

	else if (event->xclient.message_type == ILC_ResponseMsg)
	{
		IL_Response *RespMsg = (IL_Response *)&event->xclient.data.b;

		if (!ILC_Expecting_Response) END;

		ILC_Expecting_Response = False;
		if (ILC_Current_AckHandler) (ILC_Current_AckHandler)(RespMsg->Result);
	}
	else if (event->xclient.message_type == ILC_TerminateMsg && ILC_KillFunc)
	{
		IL_Terminate *TerminateMsg = (IL_Terminate *)event->xclient.data.b;
		if (ILC_KillFunc)
		{
			(ILC_KillFunc)(TerminateMsg->Noisy);
		}
		else exit(0);
	}

END;
}
		

/************************************************************************************/
/* Initialise Interlock. The application must pass in a window ID for the Interlock */
/* server (passed in as a command line argument when the client is executed by the  */
/* server), the top level widget, and a callback function to handle termination     */
/* requests.									    */
/************************************************************************************/

void IL_ClientInit(Window WindowServer, Widget ThisTask, IL_TerminateCB KillFunc)
{
BEGIN("IL_ClientInit");

	/***************************************************/
	/* Initialise global variables to supplied values. */
	/***************************************************/

	ILC_Server_Task  = WindowServer;
	ILC_This_Task  	 = ThisTask;
	ILC_Services	 = NULL;
	ILC_KillFunc	 = KillFunc;

	/*****************************************************************/
	/* Add the Interlock event handler to intercept client messages. */
	/*****************************************************************/

	XtAddRawEventHandler(ThisTask, NoEventMask, True, IL_EventHandler, NULL);

	/*************************************************/
	/* Create atoms for the interlock message types. */
	/*************************************************/

	ILC_RegisterMsg   = XInternAtom(XtDisplay(ThisTask), IL_REGISTER, False);
	ILC_DeRegisterMsg = XInternAtom(XtDisplay(ThisTask), IL_DEREGISTER, False);
	ILC_RequestMsg    = XInternAtom(XtDisplay(ThisTask), IL_REQUEST, False);
	ILC_ResponseMsg   = XInternAtom(XtDisplay(ThisTask), IL_ACKNOWLEDGE, False);
	ILC_TerminateMsg  = XInternAtom(XtDisplay(ThisTask), IL_TERMINATE, False);

END;
}
	


/*************************************************************/
/* Function to register a service with the Interlock server. */
/*************************************************************/

void IL_RegisterWithServer(char *ServiceId, IL_ServiceCB ServiceFunc)
{
XEvent Msg;
IL_Registration		       *RegistrationMsg;
IL_ClientServiceAssoc		NewService;
IL_ClientServiceAssocList	NewServiceList;

BEGIN("IL_RegisterWithServer");

	/********************************/
	/* Build the X Event structure. */
	/********************************/

	Msg.type	 	 = ClientMessage;
	Msg.xclient.display	 = XtDisplay(ILC_This_Task);
	Msg.xclient.window 	 = ILC_Server_Task;
	Msg.xclient.message_type = ILC_RegisterMsg;
	Msg.xclient.format	 = 8;

	/************************************/
	/* Create the Registration Message. */
	/************************************/

	RegistrationMsg = (IL_Registration *)Msg.xclient.data.b;

	strncpy(RegistrationMsg->Service, ServiceId, 4);
	RegistrationMsg->WindowId = XtWindow(ILC_This_Task);

	/*********************************/
	/* Post the event to the server. */
	/*********************************/

	XSendEvent(XtDisplay(ILC_This_Task), ILC_Server_Task, False, NoEventMask, &Msg);

	/*********************************************************************/
	/* Add the newly registered service to the service association list. */
	/*********************************************************************/

	strncpy(NewService.ServiceId, ServiceId, 4);
	NewService.ServiceFunc = ServiceFunc;

	NewServiceList = IL_CreateServiceList(NewService);

	if (ILC_Services)
	{
		Nconc((List)ILC_Services, (List)NewServiceList);
	}
	else ILC_Services = NewServiceList;

END;
}



/**************************************************************************/
/* Function to DeRegister a service. This must be called for all services */
/* registered by the client before the client exits.			  */
/**************************************************************************/

void IL_DeRegisterWithServer(char *ServiceId)
{
IL_ClientServiceAssocList ServiceList;
IL_DeRegistration	  DeRegMsg;
XEvent			  Msg;

BEGIN("IL_DeRegisterWithServer");

	ServiceList = ILC_Services;

	while(ServiceList != NULL)
	{
		if (!strncmp(ServiceList->Service.ServiceId, ServiceId, 4))
		{
			ILC_Services = (IL_ClientServiceAssocList)First(Remove(ServiceList));

			Msg.type	 	 = ClientMessage;
			Msg.xclient.display	 = XtDisplay(ILC_This_Task);
			Msg.xclient.window 	 = ILC_Server_Task;
			Msg.xclient.message_type = ILC_DeRegisterMsg;
			Msg.xclient.format	 = 8;


			memcpy(&DeRegMsg.Service, ServiceId, 4);
			DeRegMsg.WindowId = XtWindow(ILC_This_Task);

			/*****************************************************/
			/* Copy the registration message into the data field */
			/* of the client message.			     */
			/*****************************************************/

			memcpy(&Msg.xclient.data.b, &DeRegMsg, sizeof(IL_DeRegistration));

			/*********************************/
			/* Post the event to the server. */
			/*********************************/

			XSendEvent(XtDisplay(ILC_This_Task), 
				   ILC_Server_Task, 
				   False, NoEventMask, &Msg);

			XSync(XtDisplay(ILC_This_Task), False);
			
			END;
		}

		ServiceList = (IL_ClientServiceAssocList) Next(ServiceList);
	}
END;
}



/**************************************************/
/* Function to request a service from the server. */
/**************************************************/

void IL_RequestService(char *ServiceId, IL_AckCB CBFunc, char *Data, int DataSize)
{
char	   *DataPtr;
XEvent	    Msg;
IL_Request *Request;

BEGIN("IL_RequestService");

	if (ILC_Server_Task == 0) END;

	Msg.type	 	 = ClientMessage;
	Msg.xclient.display	 = XtDisplay(ILC_This_Task);
	Msg.xclient.window 	 = ILC_Server_Task;
	Msg.xclient.message_type = ILC_RequestMsg;
	Msg.xclient.format	 = 8;

	Request = (IL_Request *)Msg.xclient.data.b;

	strncpy(Request->Service, ServiceId, 4);
	Request->MoreToFollow = True;
	Request->Originator = XtWindow(ILC_This_Task);

	DataPtr = Data;

	while (DataSize > IL_MESSAGE_DATA_CHUNK_SIZE)
	{
		memcpy(Request->Data, DataPtr, 
		       (DataSize > IL_MESSAGE_DATA_CHUNK_SIZE) ? IL_MESSAGE_DATA_CHUNK_SIZE : 
								 DataSize);

		XSendEvent(XtDisplay(ILC_This_Task), 
			   ILC_Server_Task, 
			   False, NoEventMask, &Msg);

		DataPtr  += IL_MESSAGE_DATA_CHUNK_SIZE;
		DataSize -= IL_MESSAGE_DATA_CHUNK_SIZE;
	}

	Request->MoreToFollow = False;
	memcpy(Request->Data, DataPtr, DataSize);
	XSendEvent(XtDisplay(ILC_This_Task), ILC_Server_Task, False, NoEventMask, &Msg);

	ILC_Current_AckHandler = CBFunc;

	ILC_Expecting_Response = (CBFunc != NULL);

END;
}



/*******************************************************************/
/* Function to send a response message after processing a request. */
/*******************************************************************/

void IL_AcknowledgeRequest(char *ServiceId, IL_ReturnCode Result)
{
XEvent	     Msg;
IL_Response *Resp;
BEGIN("IL_AcknowledgeRequest");

	if (ILC_Server_Task == 0) END;

	Msg.type	 	 = ClientMessage;
	Msg.xclient.display	 = XtDisplay(ILC_This_Task);
	Msg.xclient.window 	 = ILC_Server_Task;
	Msg.xclient.message_type = ILC_ResponseMsg;
	Msg.xclient.format	 = 8;
	 
	Resp = (IL_Response *)Msg.xclient.data.b;

	strncpy(Resp->Service, ServiceId, 4);
	Resp->Result = Result;

	XSendEvent(XtDisplay(ILC_This_Task), 
		   ILC_Server_Task, 
		   False, NoEventMask, &Msg);

	ILC_Task_Busy = False;

END;
}



