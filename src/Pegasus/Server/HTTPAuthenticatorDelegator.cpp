//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN 
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author:  Nag Boranna,   Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Config/ConfigManager.h>
#include "HTTPAuthenticatorDelegator.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN



HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator(
    Uint32 operationMessageQueueId,
    Uint32 exportMessageQueueId)
   : Base(PEGASUS_SERVICENAME_HTTPAUTHDELEGATOR,
          MessageQueue::getNextQueueId()),
    _operationMessageQueueId(operationMessageQueueId),
    _exportMessageQueueId(exportMessageQueueId)
{
    _authenticationManager = new AuthenticationManager();
}

HTTPAuthenticatorDelegator::~HTTPAuthenticatorDelegator()
{
    delete _authenticationManager;

}

void HTTPAuthenticatorDelegator::_sendResponse(
    Uint32 queueId,
    Array<Sint8>& message)
{
    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
        HTTPMessage* httpMessage = new HTTPMessage(message);
	httpMessage->dest = queue->getQueueId();
	
        queue->enqueue(httpMessage);
    }
}

void HTTPAuthenticatorDelegator::_sendChallenge(
    Uint32 queueId,
    const String& authResponse)
{
    //
    // build unauthorized (401) response message
    //

    Array<Sint8> message;
    XmlWriter::appendUnauthorizedResponseHeader(message, authResponse);

    _sendResponse(queueId, message);
}


void HTTPAuthenticatorDelegator::_sendError(
    Uint32 queueId,
    const String errorMessage)
{
    //
    // build erro response message
    //

    Array<Sint8> message;
    //
    //ATTN: Need an ErrorResponseHeader() in XmlWriter
    //
    //message = XmlWriter::formatErrorResponseHeader(errorMessage);

    _sendResponse(queueId, message);
}


void HTTPAuthenticatorDelegator::handleEnqueue(Message *message)
{

   
   if (!message)
        return;

   // Flag indicating whether the message should be deleted after handling.
   // This should be set to false by handleHTTPMessage when the message is
   // passed as is to another queue.
   Boolean deleteMessage = true;
   
    if (message->getType() == HTTP_MESSAGE)
    {
        handleHTTPMessage((HTTPMessage*)message, deleteMessage);
    }

    if (deleteMessage)
    {
        delete message;
    }
}

void HTTPAuthenticatorDelegator::handleEnqueue()
{
    Message* message = dequeue();
    if(message)
       handleEnqueue(message);
}

void HTTPAuthenticatorDelegator::handleHTTPMessage(
    HTTPMessage* httpMessage,
    Boolean & deleteMessage)
{  
    Boolean authenticated = false;
    deleteMessage = true;

    //
    // get the configured authentication flag
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean requireAuthentication = false;

    if (String::equal(
        configManager->getCurrentValue("requireAuthentication"), "true"))
    {
        requireAuthentication = true;
    }

    //
    // Save queueId:
    //
    Uint32 queueId = httpMessage->queueId;

    //
    // Parse the HTTP message:
    //
    String startLine;
    Array<HTTPHeader> headers;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, contentLength);
    
    //
    // Parse the request line:
    //
    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    //
    // Process M-POST and POST messages:
    //

    if (methodName == "M-POST" || methodName == "POST")
    {
	httpMessage->message.append('\0');

        //
        // Search for Authorization header:
        //
        String authorization = String::EMPTY;

        if ( HTTPMessage::lookupHeader(
             headers, "PegasusAuthorization", authorization, false) &&
             requireAuthentication
           )
        {
            //
            // Do pegasus/local authentication
            //
            authenticated = 
                _authenticationManager->performPegasusAuthentication(
                    authorization,
                    httpMessage->authInfo);

            if (!authenticated)
            {
                String authChallenge = String::EMPTY;
                String authResp = String::EMPTY;

                authResp = _authenticationManager->getPegasusAuthResponseHeader(
                    authorization,
                    httpMessage->authInfo);

                if (!String::equal(authResp, String::EMPTY))
                {
                    _sendChallenge(queueId, authResp);
                }
                else
                {
                    _sendError(queueId, "Invalid Request");
                }

                return;
            }
        }

        if ( HTTPMessage::lookupHeader(
             headers, "Authorization", authorization, false) &&
             requireAuthentication
           )
        {
            //
            // Do http authentication if not authenticated already
            //
            if (!authenticated)
            {
                authenticated =
                    _authenticationManager->performHttpAuthentication(
                        authorization,
                        httpMessage->authInfo);

                if (!authenticated)
                {
                    //ATTN: the number of challenges get sent for a 
                    //      request on a connection can be pre-set.

                    String authResp =
                        _authenticationManager->getHttpAuthResponseHeader();

                    if (!String::equal(authResp, String::EMPTY))
                    {
                        _sendChallenge(queueId, authResp);
                    }
                    else
                    {
                        _sendError(queueId, "Invalid Request");
                    }

                    return;
                }
            }
        }

        if ( authenticated || !requireAuthentication )
        {
            //
            // Search for "CIMOperation" header:
            //
            String cimOperation;

            if (HTTPMessage::lookupHeader(
                headers, "*CIMOperation", cimOperation, true))
            {
                MessageQueue* queue =
                    MessageQueue::lookup(_operationMessageQueueId);

                if (queue)
                {
		   httpMessage->dest = queue->getQueueId();
		   
                    queue->enqueue(httpMessage);
                    deleteMessage = false;
                }
            }
            else if (HTTPMessage::lookupHeader(
                headers, "*CIMExport", cimOperation, true))
            {
                MessageQueue* queue =
                    MessageQueue::lookup(_exportMessageQueueId);

                if (queue)
                {
		   httpMessage->dest = queue->getQueueId();

		   queue->enqueue(httpMessage);
		   deleteMessage = false;
                }
            }
            else
            {
                // We don't recognize this request message type

                // The Specification for CIM Operations over HTTP reads:
                //
                //     3.3.4. CIMOperation
                //
                //     If a CIM Server receives a CIM Operation request without
                //     this [CIMOperation] header, it MUST NOT process it as if
                //     it were a CIM Operation Request.  The status code
                //     returned by the CIM Server in response to such a request
                //     is outside of the scope of this specification.
                //
                //     3.3.5. CIMExport
                //
                //     If a CIM Listener receives a CIM Export request without
                //     this [CIMExport] header, it MUST NOT process it.  The
                //     status code returned by the CIM Listener in response to
                //     such a request is outside of the scope of this
                //     specification.
                //
                // The author has chosen to send a 400 Bad Request error, but
                // without the CIMError header since this request must not be
                // processed as a CIM request.

                Array<Sint8> message;
                XmlWriter::appendBadRequestResponseHeader(message);
                _sendResponse(queueId, message);
                return;
            }
        }
        else
        {
            String authResp =
                _authenticationManager->getHttpAuthResponseHeader();

            if (!String::equal(authResp, String::EMPTY))
            {
                _sendChallenge(queueId, authResp);
            }
            else
            {
                _sendError(queueId, "Invalid Request");
            }
        }
    }
}

const char* HTTPAuthenticatorDelegator::getQueueName() const
{
    return PEGASUS_SERVICENAME_HTTPAUTHDELEGATOR;
}

PEGASUS_NAMESPACE_END
