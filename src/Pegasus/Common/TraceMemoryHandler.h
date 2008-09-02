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


#ifndef Pegasus_TraceMemoryHandler_h
#define Pegasus_TraceMemoryHandler_h

#include <cstdarg>
#include <cstdio>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/TraceHandler.h>
#include <Pegasus/Common/TraceFileHandler.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/AtomicInt.h>

PEGASUS_NAMESPACE_BEGIN

/** TraceMemoryHandler implements tracing of messages to a memory buffer
 */
#define PEGASUS_TRC_DEFAULT_BUFFER_SIZE_KB 10*1024
#define PEGASUS_TRC_BUFFER_EYE_CATCHER "PEGASUSMEMTRACE"
#define PEGASUS_TRC_BUFFER_EYE_CATCHER_LEN 16

class PEGASUS_COMMON_LINKAGE TraceMemoryHandler: public TraceHandler
{
public:

    /** Writes message with format string to the tracing facility
        @param    message  message to be written
        @param    msgLen   lenght of message without terminating '\0'
        @param    fmt      printf style format string
        @param    argList  variable argument list
     */
    virtual void handleMessage(const char* message,
                               Uint32 msgLen,
                               const char* fmt,
                               va_list argList);

    /** Writes simple message to the tracing facility.
        @param    message  message to be written
     */
    virtual void handleMessage(const char* message,
                               Uint32 msgLen);

    /** Validates the File Path for the trace File
        @param    filePath full path of the file
        @return   1        if the file path is valid
                  0        if the file path is invalid
     */
    virtual Boolean isValidMessageDestination(const char* traceFileName);

    /** Sets the filename for the trace handler. In this implementation
        we just keep the name around in case we have to flush the trace
        buffer later on to somewhere.
        @param    destination tracer destination, e.g. file
        @return   0           if the function is successful
                  1           if an error occurs
     */
    virtual Uint32 setMessageDestination(const char* destination);

    /** Flushes the trace
     */
    virtual void flushTrace();

    /** Dumps the complete content of the trace buffer to a file
        @param    filename  name of the file where to dump the trace buffer
     */
    void dumpTraceBuffer(const char* filename);

    /** Tells an instance of the traceMemoryHandler that it will be destructed
        soon and should accept no more requests for trace messages.
     */
    void die();

    /** Constructs a TraceMemoryHandler with a default buffer size.
     */
    TraceMemoryHandler();

    /** Constructs a TraceMemoryHandler with a custom buffer size.
        @param    bufferSize  size of the trace buffer in Kbyte
     */
    TraceMemoryHandler(Uint32 bufferSize);

    virtual ~TraceMemoryHandler();
    
private:

    /** The trace area is defined a struct to keep the following information
        together in memory, next the trace statementes:
        - eyecatcher to locate the trace buffer in a dump
        - the size of the trace buffer
        - the position after the last written trace statement
    */
    struct traceArea_t
    {
        char eyeCatcher[PEGASUS_TRC_BUFFER_EYE_CATCHER_LEN];
        Uint32 bufferSize;
        Uint32 nextPos;
        char* traceBuffer;
    };

    char* _overflowBuffer;
    Uint32 _overflowBufferSize;
    struct traceArea_t *_traceArea;
    Uint32 _leftBytesInBuffer;   

    // Members used for serialization
    AtomicInt _inUseCounter;
    AtomicInt _lockCounter;
    Boolean _dying;

    // Members used for statistics only
    AtomicInt _contentionCount;
    Uint32    _numberOfLocksObtained;
    
    // Name of a tracefile, in case we need to flush the buffer to a file
    char* _traceFileName;
    
    /** Request to lock the memory buffer for writing a trace message.
        @return 1        OK, you got the lock
                0        No lock was obtained, give up!!
    */
    Boolean _lockBufferAccess();

    /** Unlock the memory buffer when no longer used for writing.
    */
    void _unlockBufferAccess();

    /** Appends a fixed length message to be buffer
    */
    void _appendSimpleMessage(const char* message, Uint32 msgLen );
    
    /** Appends a marker after the last trace message in the buffer
    */
    void _appendMarker();
    
    /** Memory buffer initialization routine
        @param    traceAreaSize    the size of the used trace buffer
    */
    void _initialize( Uint32 traceAreaSize );
    
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TraceMemoryHandler_h */
