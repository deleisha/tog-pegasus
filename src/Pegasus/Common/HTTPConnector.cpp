//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include <iostream>
#include "Constants.h"
#include "Socket.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <windows.h>
#else
# include <cctype>
# include <unistd.h>
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
#  include <sys/un.h>
# endif
#endif

#include "Socket.h"
#include "TLS.h"
#include "HTTPConnector.h"
#include "HTTPConnection.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _MakeAddress(
   const char* hostname, 
   int port, 
   sockaddr_in& address)
{
    if (!hostname)
      return false;
	
   struct hostent *entry;
   if (isalpha(hostname[0]))
      entry = gethostbyname((char *)hostname);
   else
   {
      // KS 20020926 Temp change to bypass the gethostbyaddress
      // and simply use whatever address was supplied.
      // This needs to be verified. Drops following two
      // lines and replace with memset through return lines.
#ifdef PEGASUS_SNIA_INTEROP_TEST
      memset(&address, 0, sizeof(address));
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = inet_addr((char *)hostname);
      address.sin_port = htons(port);
      return true;
#else     
      unsigned long tmp = inet_addr((char *)hostname);
      entry = gethostbyaddr((char *)&tmp, sizeof(tmp), AF_INET);
#endif      

   }

   if (!entry)
       return false;

   memset(&address, 0, sizeof(address));
   memcpy(&address.sin_addr, entry->h_addr, entry->h_length);
   address.sin_family = entry->h_addrtype;
   address.sin_port = htons(port);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnectorRep
//
////////////////////////////////////////////////////////////////////////////////

struct HTTPConnectorRep
{
      Array<HTTPConnection*> connections;
};

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnector
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnector::HTTPConnector(Monitor* monitor)
   : Base(PEGASUS_QUEUENAME_HTTPCONNECTOR),
     _monitor(monitor), _entry_index(-1)
{
   _rep = new HTTPConnectorRep;
   Socket::initializeInterface();
}

HTTPConnector::~HTTPConnector()
{
   delete _rep;
   Socket::uninitializeInterface();
}

void HTTPConnector::handleEnqueue(Message *message)
{

   if (!message)
      return;

   switch (message->getType())
   {
      // It might be useful to catch socket messages later to implement
      // asynchronous establishment of connections.

      case SOCKET_MESSAGE:
	 break;

      case CLOSE_CONNECTION_MESSAGE:
      {
	 CloseConnectionMessage* closeConnectionMessage 
	    = (CloseConnectionMessage*)message;

	 for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
	 {
	    HTTPConnection* connection = _rep->connections[i];	
	    Sint32 socket = connection->getSocket();

	    if (socket == closeConnectionMessage->socket)
	    {
	       _monitor->unsolicitSocketMessages(socket);
	       _rep->connections.remove(i);
	       delete connection;
	       break;
	    }
	 }
      }

      default:
	 // ATTN: need unexpected message error!
	 break;
   };

   delete message;
}


void HTTPConnector::handleEnqueue()
{

   Message* message = dequeue();

   if (!message)
      return;

   handleEnqueue(message);
}

HTTPConnection* HTTPConnector::connect(
   const String& host, 
   const Uint32 portNumber,
   SSLContext * sslContext,
   MessageQueue* outputMessageQueue)
{
   Sint32 socket;

#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
   if (host == String::EMPTY)
   {
      // Set up the domain socket for a local connection

      sockaddr_un address;
      address.sun_family = AF_UNIX;
      strcpy(address.sun_path, "/var/opt/wbem/cimxml.socket");

      socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
      if (socket < 0)
         throw CannotCreateSocketException();

      // Connect the socket to the address:

      if (::connect(socket,
                    reinterpret_cast<sockaddr*>(&address),
                    sizeof(address)) < 0)
      {
         throw CannotConnectException("local CIM server");
      }
   }
   else
   {
#endif

   // Make the internet address:

   sockaddr_in address;

   if (!_MakeAddress((const char*)host.getCString(), portNumber, address))
   {
      char portStr [32];
      sprintf (portStr, "%u", portNumber);
      throw InvalidLocatorException(host + ":" + portStr);
   }


   // Create the socket:

   socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (socket < 0)
      throw CannotCreateSocketException();



#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
   Sint16 priv_port=1023;
   sockaddr_in temp_address;
   temp_address.sin_addr.s_addr = INADDR_ANY;
   while( 1 )
   {
      temp_address.sin_port = htons(priv_port--);
      if( 0 == bind(socket, (struct sockaddr *)&temp_address, sizeof(sockaddr_in)))
	 break;
      if( priv_port == 513 )
      {
	 priv_port = 0;
	 break;
      }
   }
   
#endif


   // Conect the socket to the address:

   if (::connect(socket,
                 reinterpret_cast<sockaddr*>(&address),
                 sizeof(address)) < 0)
   {
      char portStr [32];
      sprintf (portStr, "%u", portNumber);
      throw CannotConnectException(host + ":" + portStr);
   }

#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
   }
#endif

   // Create HTTPConnection object:

   MP_Socket * mp_socket = new MP_Socket(socket, sslContext);
   if (mp_socket->connect() < 0) {
      char portStr [32];
      sprintf (portStr, "%u", portNumber);
      throw CannotConnectException(host + ":" + portStr);
   }
    
   HTTPConnection* connection = new HTTPConnection(_monitor, mp_socket,
						   this, static_cast<MessageQueueService *>(outputMessageQueue));

   // Solicit events on this new connection's socket:

   if (-1 == (_entry_index = _monitor->solicitSocketMessages(
	  socket,
	  SocketMessage::READ | SocketMessage::EXCEPTION,
	  connection->getQueueId(), Monitor::CONNECTOR)))
   {
      delete connection;
      Socket::close(socket);
   }

   // Save the socket for cleanup later:

   _rep->connections.append(connection);

   return connection;
}

void HTTPConnector::destroyConnections()
{
   // For each connection created by this object:

   for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
   {
      _deleteConnection(_rep->connections[i]);
   }

   _rep->connections.clear();
}

void HTTPConnector::disconnect(HTTPConnection* currentConnection)
{
    //
    // find and delete the specified connection
    //
    for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
    {
        if (currentConnection == _rep->connections[i])
        {
            _deleteConnection(_rep->connections[i]);

            return;
        }
    }
}

void HTTPConnector::_deleteConnection(HTTPConnection* httpConnection)
{
    Sint32 socket = httpConnection->getSocket();

    // Unsolicit SocketMessages:

    _monitor->unsolicitSocketMessages(socket);

    // Destroy the connection (causing it to close):

    delete httpConnection;
}

PEGASUS_NAMESPACE_END
