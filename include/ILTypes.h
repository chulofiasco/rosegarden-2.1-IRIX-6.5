/*
---------------------------------------------------------------------------------------------
	    Interlock Tool Communication Framework - Final Year Project, A.J. Green
---------------------------------------------------------------------------------------------

File Name:	ILTypes.h	

Description:	Type definitions for Interlock Framework.

Author:		AJG

History:

Update	Date		Programmer	Comments
======	====		==========	========
001	08/02/94	AJG		File Created.
002	27/03/94	AJG		Added service activation and de-activation 
					callback types.
--------------------------------------------------------------------------------------------
*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Core.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <Lists.h>

#ifndef __IL_TYPES__
#define __IL_TYPES__

/**************************************************************/
/* IL_Bool - Single byte boolean type to ensure maximum space */
/* is available in IL_Request messages.			      */
/**************************************************************/

typedef unsigned char IL_Bool;

#define IL_FALSE 	0
#define IL_TRUE 	!IL_FALSE

#define IL_MESSAGE_DATA_CHUNK_SIZE	(20 - sizeof(char) * 4 - sizeof(Window) - sizeof(IL_Bool))

/******************************************************/
/* Return values - these indicate whether the service */
/* has been correctly processed. 		      */
/******************************************************/

typedef enum
{
	IL_SERVICE_OK,
	IL_SERVICE_BUSY,
	IL_SERVICE_FAILED,
	IL_NO_SUCH_SERVICE
}
IL_ReturnCode;


/*****************************************************************/
/* Interlock Service Registration message - allows the server to */
/* associate a window Id with the service descriptor provided.   */
/*****************************************************************/

#define IL_REGISTER "Interlock Registration Message"

typedef struct
{
	char 	Service[4];
	Window 	WindowId;
}
IL_Registration;


/*******************************************************************/
/* Interlock De-Registration message - signals to the server that  */
/* a client application is terminating, allowing the server to     */
/* clear any outstanding service requests with negative responses. */
/*******************************************************************/

#define IL_DEREGISTER "Interlock DeRegistration Message"

typedef IL_Registration IL_DeRegistration;



/*****************************************************************/
/* Interlock Request message - limited to a size of twenty bytes */
/* by the structure of the X client message type. Data longer    */
/* than fifteen bytes is transmitted in fifteen byte chunks with */
/* the MoreToFollow flag set to true on all bar the last chunk.  */
/*****************************************************************/

#define IL_REQUEST "Interlock Request Message"

typedef struct
{
	char 	Service[4];
	Window	Originator;
	IL_Bool MoreToFollow;
	char 	Data[IL_MESSAGE_DATA_CHUNK_SIZE];
}
IL_Request;



/******************************************************/
/* Interlock Response message - returns the result of */
/* the service request to the client window.	      */
/******************************************************/

#define IL_ACKNOWLEDGE "Interlock Response Message"

typedef struct
{
	char 		Service[4];
	IL_ReturnCode	Result;
}
IL_Response;


/*************************************************************************/
/* Interlock Termination Request message - requests that a client task   */
/* terminate itself cleanly. Contains a boolean value indicating whether */
/* the client should send de-registration messages for all the services  */
/* it has registered.							 */
/*************************************************************************/

#define IL_TERMINATE "Interlock Termination Request Message"

typedef struct
{
	Boolean	Noisy;
}
IL_Terminate;

/******************************************************************/
/* Callback definition for a service function. Service functions  */
/* are passed a pointer to the data that has been read in from    */
/* the request message(s). This data is allocated on the heap and */
/* must be freed explicitly by the client application after use.  */
/******************************************************************/

typedef void (*IL_ServiceCB)(char *);



/**********************************************************************/
/* Callback definition for Acknowledgement functions. These functions */
/* receive the acknowledgement message that come back after a service */
/* has been processed or has failed for some reason.		      */
/**********************************************************************/

typedef void (*IL_AckCB)(IL_ReturnCode);



/************************************************************************/
/* De-Registration callback - called when a de-registration messages is */
/* received from a client. This is available to provide a hook whereby  */
/* programs that use the Server module can update their internal state  */
/* after a service providing task has died (cleanly at least).		*/
/************************************************************************/

typedef IL_ServiceCB IL_DeRegCB;



/*****************************************************************************/
/* Service activation callback - this is called in the server when a request */
/* for a service has been passed to the appropriate client. The service Id   */
/* is passed out to indicate which service has been activated.		     */
/*****************************************************************************/

typedef IL_ServiceCB IL_ServiceActivateCB;



/***************************************************************************/
/* Service de-activation callback - this is called in the server when an   */
/* acknowledgement message is received for a previously activated service. */
/* The service Id and the response code are passed out for the delectation */
/* and delight of the callback function.				   */
/***************************************************************************/

typedef void (*IL_ServiceDeactivateCB)(char *, IL_ReturnCode);




/**************************************************************************/
/* Termination Request callback - called in the client when a termination */
/* request has been received from the server.				  */
/**************************************************************************/

typedef void (*IL_TerminateCB)(Boolean);



/****************************************************/
/* Service association structure for Client Module. */
/****************************************************/

typedef struct
{
	char 		ServiceId[4];
	IL_ServiceCB	ServiceFunc;
}
IL_ClientServiceAssoc;

typedef struct
{
	ListElement 		Base;
	IL_ClientServiceAssoc 	Service;
}
IL_ClientServiceAssocListElt, *IL_ClientServiceAssocList;




/****************************************************/
/* Service association structure for Server Module. */
/****************************************************/

typedef struct
{
	char 	ServiceId[4];
	Window	WindowId;
}
IL_ServerServiceAssoc;

typedef struct
{
	ListElement 		Base;
	IL_ServerServiceAssoc 	Service;
}
IL_ServerServiceAssocListElt, *IL_ServerServiceAssocList;




/*********************************************************************/
/* Server outstanding request structure - used to maintain a list of */
/* outstanding requests to enable clean shutdown and stuff.	     */
/*********************************************************************/

typedef struct
{
	char	ServiceId[4];
	Window 	WindowId;
	IL_Bool MsgInTransit;
}
IL_OutstandingRequest;



/********************************************************************************/
/* Structure to store a list of services and the executables that provide them. */
/********************************************************************************/

typedef struct
{
	ListElement		Base;
	char			ServiceId[4];
	char		       *ExecutablePath;
}
IL_ServiceTaskListElt, *IL_ServiceTaskList;



/**************************************************************************/
/* Structure to store outstanding requests so that we know which services */
/* are in use at any one time.						  */
/**************************************************************************/

typedef struct
{
	ListElement		Base;
	IL_OutstandingRequest	Request;
}
IL_OutstandingRequestListElt, *IL_OutstandingRequestList;




/************************************************************************/
/* Structure to store X events while waiting for a service to register. */
/************************************************************************/

typedef struct
{
	ListElement	Base;
	XEvent		Msg;
}
IL_XEventListElt, *IL_XEventList;


#endif

