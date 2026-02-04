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
002	27/03/94	AJG		Tidied up handling of NULL callbacks.

--------------------------------------------------------------------------------------------
*/

#include "ILTypes.h"
#include "ILServer.h"
#include <Debug.h>

#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

Widget 				IL_This_Task;
pid_t				IL_ProcessId;
IL_ServerServiceAssocList	IL_ActiveServices;
IL_ServiceTaskList		IL_AvailableServices;
IL_OutstandingRequestList	IL_OutstandingRequests;
char 			       *IL_ServerName;
IL_XEventList			IL_TempMsgStore;
char				IL_WindowIdString[16];
IL_DeRegCB			IL_DeRegFunc;
IL_ServiceActivateCB		IL_ActivationFunc;
IL_ServiceDeactivateCB		IL_DeactivationFunc;
char			       *IL_CommandLineArgs[4] = { NULL, "-ILTopBoxWin", IL_WindowIdString, NULL };

Atom IL_RegisterMsg, IL_DeRegisterMsg, IL_RequestMsg, IL_ResponseMsg, IL_TerminateMsg;

/********************************/
/* Private function prototypes. */
/********************************/

void IL_RouteRequest(IL_Request *RequestMsg);
void IL_ServerEventHandler(Widget w, XtPointer closure, XEvent *event, Boolean *cont);

/***********************************************************/
/* Constructor function for service-task association list. */
/***********************************************************/

IL_ServiceTaskList IL_CreateServiceTaskList(char *Service, char *Path)
{
IL_ServiceTaskList NewTaskList;

BEGIN("IL_CreateServiceTaskList");

	NewTaskList = (IL_ServiceTaskList)NewList(sizeof(IL_ServiceTaskListElt));
	strncpy(NewTaskList->ServiceId, Service, 4);
	NewTaskList->ExecutablePath = strcpy((char *)malloc(strlen(Path) + 1), Path);

RETURN_PTR(NewTaskList);
}


/*****************************************************/
/* Constructor function for X Event list. 	     */
/* Used to store messages when deferring processing. */
/*****************************************************/

IL_XEventList IL_CreateXEventList(XEvent *Event)
{
IL_XEventList NewEventList;

BEGIN("IL_CreateXEventList");

	NewEventList = (IL_XEventList)NewList(sizeof(IL_XEventListElt));
	NewEventList->Msg = *Event;

RETURN_PTR(NewEventList);
}



/*************************************************************/
/* Constructor function for server service association list. */
/*************************************************************/

IL_ServerServiceAssocList IL_ServerCreateServiceList(IL_ServerServiceAssoc NewService)
{
IL_ServerServiceAssocList NewServiceList;

BEGIN("IL_ServerCreateServiceList");

	NewServiceList = (IL_ServerServiceAssocList)NewList(sizeof(IL_ServerServiceAssocListElt));
	NewServiceList->Service = NewService;

RETURN_PTR(NewServiceList);
}




/*************************************************************/
/* Constructor function for server service association list. */
/*************************************************************/

IL_OutstandingRequestList IL_CreateRequestList(IL_OutstandingRequest NewRequest)
{
IL_OutstandingRequestList NewRequestList;

BEGIN("IL_CreateRequestList");

	NewRequestList = (IL_OutstandingRequestList)NewList(sizeof(IL_OutstandingRequestListElt));
	NewRequestList->Request = NewRequest;

RETURN_PTR(NewRequestList);
}





/*****************************************************************/
/* Function to add a service to the list of available services.  */
/* This list is scanned when a service is requested and the task */
/* providing that service is not running in order to find the    */
/* path for the executable for that task.			 */
/*****************************************************************/

void IL_AddService(char *ServiceId, char *Path)
{
IL_ServiceTaskList	NewTask;

BEGIN("IL_AddService");

	NewTask = IL_CreateServiceTaskList(ServiceId, Path);

	if (IL_AvailableServices)
	{
		Nconc(IL_AvailableServices, NewTask);
	}
	else IL_AvailableServices = NewTask;

END;
}


/****************************************/
/* Spawn a new task - fun, fun, fun!!!! */
/****************************************/

pid_t IL_StartTask(char *ServiceId)
{
pid_t			NewProcessId;
IL_ServiceTaskList	AvailableServices;
char		       *ServiceProvider;

BEGIN("IL_StartTask");

	ServiceProvider = NULL;

	AvailableServices = IL_AvailableServices;

	while(AvailableServices)
	{
		if (!strncmp(AvailableServices->ServiceId, ServiceId, 4))
		{
			ServiceProvider = AvailableServices->ExecutablePath;
			break;
		}
		AvailableServices = (IL_ServiceTaskList)Next(AvailableServices);
	}

	/***********************************************************/
	/* If the service is not described then send back an error */
	/* message to the originating client explaining this in    */
	/* great detail and apologising profusely.		   */
	/***********************************************************/

	if (ServiceProvider == NULL) RETURN_INT(0);

	NewProcessId = fork();

	if (NewProcessId < 0) exit(-1);

	if (NewProcessId) return NewProcessId;

	IL_CommandLineArgs[0] = ServiceProvider;

	execvp(ServiceProvider, IL_CommandLineArgs);

	perror("interlock: exec failed");

RETURN_INT(NewProcessId);
}

/**************************************************************************/
/* Function to process registration messages. The new service is added to */
/* the active services list - this means that the client application that */
/* is able to supply this service is up and running.			  */
/**************************************************************************/

void IL_RegisterNewService(IL_Registration *RegistrationMsg)
{
IL_ServerServiceAssoc		NewService;
IL_ServerServiceAssocList	NewServiceList;
IL_XEventList			StoredMsg, NextMsg;
IL_Request		       *Request;
BEGIN("IL_RegisterNewService");

	strncpy(NewService.ServiceId, RegistrationMsg->Service, 4);
	NewService.WindowId = RegistrationMsg->WindowId;

	NewServiceList = IL_ServerCreateServiceList(NewService);

	if (IL_ActiveServices)
	{
		Nconc(IL_ActiveServices, NewServiceList);
	}
	else IL_ActiveServices = NewServiceList;

	/***********************************************************************/
	/* Run through any stored messages to see if they can now be serviced. */
	/***********************************************************************/

	StoredMsg = IL_TempMsgStore;

	while(StoredMsg)
	{
		Request = (IL_Request *)StoredMsg->Msg.xclient.data.b;
		if (!strncmp(Request->Service, RegistrationMsg->Service, 4))
		{
			IL_RouteRequest(Request);
			NextMsg = (IL_XEventList)Next(StoredMsg);
			IL_TempMsgStore = (IL_XEventList)First(Remove(StoredMsg));
			StoredMsg = NextMsg;
		}
		else StoredMsg = (IL_XEventList)Next(StoredMsg);
	}
END;
}




/********************************************************************/
/* Remove a service from the active service lists after receiving a */
/* de-registration message.					    */
/********************************************************************/

void IL_DeRegisterService(IL_DeRegistration *DeRegMsg)
{
IL_ServerServiceAssocList ServiceList;
IL_OutstandingRequestList OutstandingRequests;
XEvent			  Msg;
IL_Response		 *Resp;

BEGIN("IL_DeRegisterService");

	ServiceList = IL_ActiveServices;

	/****************************************************/
	/* Remove the service from the active service list. */
	/****************************************************/

	while(ServiceList != NULL)
	{
		if (!strncmp(ServiceList->Service.ServiceId, DeRegMsg->Service, 4))
		{
			IL_ActiveServices = (IL_ServerServiceAssocList)First(Remove(ServiceList));
			if (IL_DeRegFunc) (IL_DeRegFunc)(DeRegMsg->Service);
			END;
		}

		ServiceList = (IL_ServerServiceAssocList)Next(ServiceList);
	}

	/**************************************************************************/
	/* Scan the outstanding requests list, deleting any outstanding requests  */
	/* for the service being de-registered and sending back IL_SERVICE_FAILED */
	/* replies to the originating task.					  */
	/**************************************************************************/

	OutstandingRequests = IL_OutstandingRequests;

	while (OutstandingRequests)
	{
		if (!strncmp(OutstandingRequests->Request.ServiceId, DeRegMsg->Service, 4))
		{
			Msg.type = ClientMessage;
			Msg.xclient.display	 = XtDisplay(IL_This_Task);
			Msg.xclient.window 	 = OutstandingRequests->Request.WindowId;
			Msg.xclient.message_type = IL_ResponseMsg;
			Msg.xclient.format	 = 8;

			Resp = (IL_Response *)Msg.xclient.data.b;
					
			strncpy(Resp->Service, DeRegMsg->Service, 4);
			Resp->Result = IL_SERVICE_FAILED;

			XSendEvent(XtDisplay(IL_This_Task),
				   OutstandingRequests->Request.WindowId,
				   False,
				   NoEventMask, &Msg);

			IL_OutstandingRequests = (IL_OutstandingRequestList)First(Remove(OutstandingRequests));

			END;
		}

		OutstandingRequests = (IL_OutstandingRequestList)Next(OutstandingRequests);
	}	
END;
}




/********************************************************************************/
/* Send a request message on to a client that can provide the required service. */
/********************************************************************************/

void IL_RouteRequest(IL_Request *RequestMsg)
{
IL_ServerServiceAssocList  ServiceList;
IL_OutstandingRequestList  OutstandingRequest;
IL_OutstandingRequest	   NewRequest;
IL_OutstandingRequestList  NewRequestList;	
XEvent 			   Msg;
IL_Response 		  *Resp;
IL_Request		  *Req;
IL_XEventList		   StoredEvent;
IL_Bool			   TaskNeedsStarting;
pid_t			   ChildProcessId;

BEGIN("IL_RouteRequest");

	ChildProcessId = 0;

	/*************************************************************************/
	/* First we need to traverse the active service lists to discover if the */
	/* task providing the requested service is currently active, and if so   */
	/* whether or not the service is in use.				 */
	/*************************************************************************/

	ServiceList = IL_ActiveServices;

	while(ServiceList)
	{
		/*******************************************************/
		/* Find the correct service entry in the service list. */
		/*******************************************************/

		if (!strncmp(ServiceList->Service.ServiceId,
			     RequestMsg->Service, 4))
		{

			/*********************************************************/
			/* We have found the service in the active service list. */
			/* Now check if anyone is currently using the service... */
			/*********************************************************/

			OutstandingRequest = IL_OutstandingRequests;

			while(OutstandingRequest)
			{
				if (!strncmp(OutstandingRequest->Request.ServiceId,
			     		     RequestMsg->Service, 4))
				{
					/*************************************************************/
					/* If we find an outstanding request for the service in the  */
					/* outstanding request list then we need to check two        */
					/* conditions so as to cope with multiple event messages:    */
					/*							     */
					/* 1) If the requesting task is different from the task that */
					/*    has a request outstanding.			     */
					/*							     */
					/* 2) The currently outstanding request has been completely  */
					/*    transmitted, (i.e. this is a separate request from the */
					/*    one currently outstanding, and not a continuation of a */
					/*    multi-event message).				     */
					/*							     */
					/* If either of these conditions are true then we must       */
					/* refuse the service, sending an IL_Response back to the    */
					/* requesting task indicating that the service is busy.      */
					/* Otherwise the event is a continuation of a previous       */
					/* request, so just send it rolling on its way.		     */
					/*************************************************************/

					if (RequestMsg->Originator != 
						OutstandingRequest->Request.WindowId ||
					    !OutstandingRequest->Request.MsgInTransit)
					{
						Msg.type = ClientMessage;
						Msg.xclient.display	 = XtDisplay(IL_This_Task);
						Msg.xclient.window 	 = RequestMsg->Originator;
						Msg.xclient.message_type = IL_ResponseMsg;
						Msg.xclient.format	 = 8;

						Resp = (IL_Response *)Msg.xclient.data.b;
					
						strncpy(Resp->Service, RequestMsg->Service, 4);
						Resp->Result = IL_SERVICE_BUSY;
	
						XSendEvent(XtDisplay(IL_This_Task),
							   RequestMsg->Originator,
							   False,
							   NoEventMask, &Msg);
	
						END;
					}
					else
					{
						Msg.type	 	 = ClientMessage;
						Msg.xclient.display	 = XtDisplay(IL_This_Task);
						Msg.xclient.window 	 = ServiceList->Service.WindowId;
						Msg.xclient.message_type = IL_RequestMsg;
						Msg.xclient.format	 = 8;

						OutstandingRequest->Request.MsgInTransit = 
										RequestMsg->MoreToFollow;
	 
						memcpy(Msg.xclient.data.b, RequestMsg, sizeof(IL_Request));

						XSendEvent(XtDisplay(IL_This_Task), 
							   ServiceList->Service.WindowId, 
							   False, NoEventMask, &Msg);

						END;
					}
				}

				OutstandingRequest = (IL_OutstandingRequestList)
								Next(OutstandingRequest);
			}

			/***************************************************************/
			/* If we've got this far then we are dealing with a completely */
			/* NEW request, so sing five hosannas  and then pass the event */
			/* on to the appropriate client, making a note of this new and */
			/* interesting request in the outstanding request list.        */
			/***************************************************************/

			Msg.type	 	 = ClientMessage;
			Msg.xclient.display	 = XtDisplay(IL_This_Task);
			Msg.xclient.window 	 = ServiceList->Service.WindowId;
			Msg.xclient.message_type = IL_RequestMsg;
			Msg.xclient.format	 = 8;
	 
			memcpy(Msg.xclient.data.b, RequestMsg, sizeof(IL_Request));

			XSendEvent(XtDisplay(IL_This_Task), 
				   ServiceList->Service.WindowId, 
				   False, NoEventMask, &Msg);

			strncpy(NewRequest.ServiceId, RequestMsg->Service, 4);

			NewRequest.WindowId 	= RequestMsg->Originator;
			NewRequest.MsgInTransit = RequestMsg->MoreToFollow;
			NewRequestList	    	= IL_CreateRequestList(NewRequest);

			if (IL_OutstandingRequests)
			{
				Nconc(IL_OutstandingRequests, NewRequestList);
			}
			else IL_OutstandingRequests = NewRequestList;

			if (IL_ActivationFunc) (IL_ActivationFunc)(RequestMsg->Service);

			END;
		}

		ServiceList = (IL_ServerServiceAssocList)Next(ServiceList);
	}

	/****************************************************************************/
	/* We haven't found the service in the active list, so we may need to start */
	/* the task up all by ourself. However, if the event coming in is a         */
	/* continuation of a multi-event message then we don't really want to start */
	/* up another task, so before leaping straight in and setting things off    */
	/* like no tomorrow we'd better have a quick peek at the temporary event    */
	/* store to see if we've already done this one.				    */
	/****************************************************************************/

	StoredEvent	 	= IL_TempMsgStore;
	TaskNeedsStarting 	= True;
	
	while(StoredEvent)
	{
		Req = (IL_Request *)StoredEvent->Msg.xclient.data.b;
		if (!strncmp(Req->Service, RequestMsg->Service, 4))
		{
			TaskNeedsStarting = False;
			break;
		}
		StoredEvent = (IL_XEventList)Next(StoredEvent);
	}

	/********************************/
	/* Start the task if necessary. */
	/********************************/

	if (TaskNeedsStarting) ChildProcessId = IL_StartTask(RequestMsg->Service);

	if (ChildProcessId || !TaskNeedsStarting)
	{

		/********************************************/
		/* If the task started up OK then store the */
		/* mevent in the temporary message store    */
		/* so that we can send it on after the task */
		/* has registered the appropriate service.  */
		/********************************************/

		Msg.type	 	 = ClientMessage;
		Msg.xclient.display	 = XtDisplay(IL_This_Task);
		Msg.xclient.message_type = IL_RequestMsg;
		Msg.xclient.format	 = 8;
	 
		memcpy(Msg.xclient.data.b, RequestMsg, sizeof(IL_Request));

		StoredEvent = IL_CreateXEventList(&Msg);

		if (IL_TempMsgStore)
		{
			Nconc(IL_TempMsgStore, StoredEvent);
		}
		else IL_TempMsgStore = StoredEvent;
	}	
	else
	{
		/*******************************************************/
		/* Otherwise something has gone terribly wrong - so    */
		/* send back a cop-out message to the requesting task. */
		/*******************************************************/

		Msg.type		 = ClientMessage;
		Msg.xclient.display 	 = XtDisplay(IL_This_Task);
		Msg.xclient.window  	 = RequestMsg->Originator;
		Msg.xclient.message_type = IL_ResponseMsg;
		Msg.xclient.format 	 = 8;

		Resp = (IL_Response *)Msg.xclient.data.b;
		strncpy(Resp->Service, RequestMsg->Service, 4);
		Resp->Result = IL_NO_SUCH_SERVICE;

		XSendEvent(XtDisplay(IL_This_Task),
			   RequestMsg->Originator,
			   False,
			   NoEventMask, &Msg);
	}

END;
}




/********************************************************************/
/* Route an incoming response message back to the originating task. */
/********************************************************************/

void IL_RouteResponse(IL_Response *ResponseMsg)
{
XEvent			  Msg;
IL_OutstandingRequestList OutstandingRequests;
IL_Response		 *Resp;

BEGIN("IL_RouteResponse");

	OutstandingRequests = IL_OutstandingRequests;

	while(OutstandingRequests != NULL)
	{
		if (!strncmp(OutstandingRequests->Request.ServiceId, ResponseMsg->Service, 4))
		{

			Msg.type = ClientMessage;
			Msg.xclient.display	 = XtDisplay(IL_This_Task);
			Msg.xclient.window 	 = OutstandingRequests->Request.WindowId;
			Msg.xclient.message_type = IL_ResponseMsg;
			Msg.xclient.format	 = 8;

			Resp = (IL_Response *)Msg.xclient.data.b;
		
			strncpy(Resp->Service, ResponseMsg->Service, 4);
			Resp->Result = ResponseMsg->Result;
	
			XSendEvent(XtDisplay(IL_This_Task),
				   OutstandingRequests->Request.WindowId,
				   False,
				   NoEventMask, &Msg);
	
			IL_OutstandingRequests = (IL_OutstandingRequestList)First(Remove(OutstandingRequests));

			if (IL_DeactivationFunc) (IL_DeactivationFunc)(ResponseMsg->Service, 
								       ResponseMsg->Result);
			END;
		}

		OutstandingRequests = (IL_OutstandingRequestList)Next(OutstandingRequests);

	}
END;
}



void IL_KillAllTasks(Boolean Noisy)
{
IL_ServerServiceAssocList	Pruning, DoomedEntry;
Window				TaskWindow;
XEvent				Msg;
IL_Terminate		       *TerminateRequest;

BEGIN("IL_KillAllTasks");

	while (IL_ActiveServices)
	{
		Pruning	   = IL_ActiveServices;
		TaskWindow = Pruning->Service.WindowId;

		while(Pruning)
		{
			DoomedEntry = Pruning;
			Pruning = (IL_ServerServiceAssocList)Next(Pruning);

			if (DoomedEntry->Service.WindowId == TaskWindow)
			{
				Msg.type = ClientMessage;
				Msg.xclient.display	 = XtDisplay(IL_This_Task);
				Msg.xclient.window 	 = TaskWindow;
				Msg.xclient.message_type = IL_TerminateMsg;
				Msg.xclient.format	 = 8;

				TerminateRequest = (IL_Terminate *)Msg.xclient.data.b;
		
				TerminateRequest->Noisy = Noisy;
	
				XSendEvent(XtDisplay(IL_This_Task),
					   TaskWindow,
					   False,
					   NoEventMask, &Msg);
	
				IL_ActiveServices = (IL_ServerServiceAssocList)First(Remove(DoomedEntry));
			}
		}
	}

	XSync(XtDisplay(IL_This_Task), True);

END;
}
		
void IL_ServerEventHandler(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
BEGIN("IL_ServerEventHandler");

	if (cont) *cont = True; /* cc '95 */

	if (event->type != ClientMessage) END;

	if (event->xclient.message_type == IL_RegisterMsg)
	{
		IL_RegisterNewService((IL_Registration *)event->xclient.data.b);
		END;
	}

	if (event->xclient.message_type == IL_DeRegisterMsg)
	{
		IL_DeRegisterService((IL_DeRegistration *)event->xclient.data.b);
		END;
	}

	if (event->xclient.message_type == IL_RequestMsg)
	{
		IL_RouteRequest((IL_Request *)event->xclient.data.b);
		END;
	}

	if (event->xclient.message_type == IL_ResponseMsg)
	{
		IL_RouteResponse((IL_Response *)event->xclient.data.b);
		END;
	}

END;
}


void IL_ServerInit(Widget                 ThisTask, 
		   char                  *ServerName, 
		   IL_DeRegCB             DeRegFunc,
		   IL_ServiceActivateCB   ActivationFunc,
		   IL_ServiceDeactivateCB DeactivationFunc)
{
BEGIN("IL_ServerInit");

	IL_This_Task	     	= ThisTask;
	IL_ProcessId		= getpid();
	IL_ActiveServices     	= NULL;
	IL_AvailableServices	= NULL;
	IL_OutstandingRequests	= NULL;
	IL_TempMsgStore		= NULL;
	IL_DeRegFunc		= DeRegFunc;
	IL_ActivationFunc	= ActivationFunc;
	IL_DeactivationFunc	= DeactivationFunc;

	IL_ServerName = (char *)XtMalloc(strlen(ServerName) + 1);
	strcpy(IL_ServerName, ServerName);
	
	sprintf(IL_WindowIdString, "%ld", XtWindow(IL_This_Task));

	/*******************************/
	/* Register the event handler. */
	/*******************************/

	XtAddRawEventHandler(ThisTask, NoEventMask, True, IL_ServerEventHandler, NULL);

	/*************************************************/
	/* Create atoms for the interlock message types. */
	/*************************************************/

	IL_RegisterMsg   = XInternAtom(XtDisplay(ThisTask), IL_REGISTER, False);
	IL_DeRegisterMsg = XInternAtom(XtDisplay(ThisTask), IL_DEREGISTER, False);
	IL_RequestMsg    = XInternAtom(XtDisplay(ThisTask), IL_REQUEST, False);
	IL_ResponseMsg   = XInternAtom(XtDisplay(ThisTask), IL_ACKNOWLEDGE, False);
	IL_TerminateMsg  = XInternAtom(XtDisplay(ThisTask), IL_TERMINATE, False);

END;
}

