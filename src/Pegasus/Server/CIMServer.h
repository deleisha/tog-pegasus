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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Mike Day (mdday@us.ibm.com)
//	   Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Server_h
#define Pegasus_Server_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMServerState.h>

PEGASUS_NAMESPACE_BEGIN

struct ServerRep;

class Monitor;
class CIMOperationRequestDispatcher;
class CIMOperationResponseEncoder;
class CIMOperationRequestDecoder;
class CIMExportRequestDispatcher;
class CIMExportResponseEncoder;
class CIMExportRequestDecoder;
class HTTPAcceptor;

class PEGASUS_SERVER_LINKAGE CIMServer
{
public:

    enum Protocol { PROPRIETARY, STANDARD };

    /** Constructor - Creates a CIMServer object.
        The CIM Server objects establishes a repository object,
        a dispatcher object, and creates a channnel factory and
        acceptor for the Server.
        @param monitor	  monitor object for the server.
        @param rootPath Directory path to the repository.
        @exception - ATTN
    */
    CIMServer(
	Monitor* monitor,
	const String& rootPath,
        Boolean useSSL);

    ~CIMServer();

    /** bind Binds the port address to the Server.
	@param address char* to the port address for TCP.
	@exception - This function may receive exceptions from
	Channel specific subfunctions.
    */
    void bind(Uint32 port);

    /** runForever Main runloop for the server.
    */
    void runForever();

    /** Call to gracefully shutdown the server.  The server connection socket
        will be closed to disable new connections from clients.
    */
    void stopClientConnection(); 

    /** Call to gracefully shutdown the server.  It is called when the server
        has been stopped and is ready to be shutdown.  Next time runForever() 
        is called, the server shuts down.
    */
    void shutdownServer();

    /** Return true if the server has shutdown, false otherwise.
    */
    Boolean terminated() { return _dieNow; };

    /** Call to resume the sever. 
    */
    void resumeServer();

    Uint32 getRequestCount();

    CIMOperationRequestDispatcher* getDispatcher();

private:

    Boolean _dieNow;

    String _rootPath;
    String _repositoryRootPath;

    Boolean _useSSL;

    Monitor* _monitor;
    CIMOperationRequestDispatcher* _cimOperationRequestDispatcher;
    CIMOperationResponseEncoder* _cimOperationResponseEncoder;
    CIMOperationRequestDecoder* _cimOperationRequestDecoder;

    CIMExportRequestDispatcher* _cimExportRequestDispatcher;
    CIMExportResponseEncoder* _cimExportResponseEncoder;
    CIMExportRequestDecoder* _cimExportRequestDecoder;

    HTTPAcceptor*   _acceptor;
    CIMServerState* _serverState;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Server_h */
