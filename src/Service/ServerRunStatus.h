//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ServerRunStatus_h
#define Pegasus_ServerRunStatus_h

#ifdef PEGASUS_OS_TYPE_WINDOWS
#include <windows.h>
#endif
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Service/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

#if defined(PEGASUS_OS_TYPE_WINDOWS)
typedef int PEGASUS_PID_T;
#else
typedef pid_t PEGASUS_PID_T;
#endif

/**
    The ServerRunStatus class is used to manage the run status of a
    server.  It can be used to determine if an instance of the server is
    already running, to kill a running instance, and to indicate that an
    instance is being started.

    The implementation varies per platform.  On Unix platforms, a PID file
    is used to record the process ID of the running server.  On Windows,
    an event handle is used and the PID parameters are not meaningful.
*/
class PEGASUS_SERVICE_LINKAGE ServerRunStatus
{
public:

    /**
        Constructs a ServerRunStatus instance with the specified server
        parameters.
    */
    ServerRunStatus(
        const char* serverName,
        const char* pidFilePath);

    /**
        Destructs the ServerRunStatus instance.  On Windows, this
        indicates to the system that this instance of the server is no
        longer running.
    */
    ~ServerRunStatus();

    /**
        Determines whether the server is running, based on the parameters
        specified in the constructor.
    */
    Boolean isServerRunning();

    /**
        Indicates that this instance of the server is now running.
    */
    void setServerRunning();

    /**
        On platforms that use a PID file, this method allows a parent process
        ID to be specified.  This allows the isServerRunning() method to take
        the parent process into consideration.
    */
    void setParentPid(PEGASUS_PID_T parentPid);

    /**
        Sends a SIGKILL signal to the running server process on platforms
        that support signals.
    */
    Boolean kill();

private:

    /**
        Determines whether the specified process ID is an instance of the
        server specified in the constructor.
    */
    Boolean _isServerProcess(PEGASUS_PID_T pid);

    const char* _serverName;
    const char* _pidFilePath;
    PEGASUS_PID_T _parentPid;

#ifdef PEGASUS_OS_TYPE_WINDOWS
    HANDLE _event;
    Boolean _wasAlreadyRunning;
#endif
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_ServerRunStatus_h
