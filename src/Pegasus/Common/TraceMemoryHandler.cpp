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

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdarg.h>
#endif

#include <Pegasus/Common/TraceMemoryHandler.h>
#include <iostream>
#include <fstream>

#define PEGASUS_TRC_BUFFER_WRAP_MARKER ""
#define PEGASUS_TRC_BUFFER_EOT_MARKER "*EOTRACE*"
#define PEGASUS_TRC_BUFFER_EOT_MARKER_LEN 9


//#define DBG(output) output
#define DBG(output)

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//  Constructs TraceMemoryHandler with a default buffer size
////////////////////////////////////////////////////////////////////////////////
TraceMemoryHandler::TraceMemoryHandler()
{
    Uint32 traceAreaSize = PEGASUS_TRC_DEFAULT_BUFFER_SIZE_KB * 1024;
    
    _initialize(traceAreaSize);    
}

////////////////////////////////////////////////////////////////////////////////
//  Constructs TraceMemoryHandler with a custom buffer size
////////////////////////////////////////////////////////////////////////////////
TraceMemoryHandler::TraceMemoryHandler( Uint32 bufferSize )
{
    Uint32 traceAreaSize = bufferSize * 1024;
    
    _initialize(traceAreaSize);    
}

////////////////////////////////////////////////////////////////////////////////
//  Private method to (re-)initialize the memory buffer
////////////////////////////////////////////////////////////////////////////////
void TraceMemoryHandler::_initialize( Uint32 traceAreaSize )
{
    _dying = false;
    _inUseCounter = 0;
    _lockCounter  = 1;
    _contentionCount = 0;
    _numberOfLocksObtained = 0;
    _traceFileName = 0;


    _overflowBuffer = 0;
    _overflowBufferSize = 0;
    
    _traceArea = (struct traceArea_t*) new char[traceAreaSize];
    
    // The final buffer size is the size of the allocated area, less the
    // size of the header struct, less one byte reseved for a terminating 0
    _traceArea->bufferSize = traceAreaSize - sizeof(struct traceArea_t);
    _traceArea->bufferSize--;
    _traceArea->nextPos = 0;
    _leftBytesInBuffer = _traceArea->bufferSize-1;
    
    memcpy(_traceArea->eyeCatcher, 
           PEGASUS_TRC_BUFFER_EYE_CATCHER,
           PEGASUS_TRC_BUFFER_EYE_CATCHER_LEN);
           
    _appendMarker();       

    // The end of the trace buffer is always null terminated
    _traceArea->traceBuffer[_traceArea->bufferSize] = '\0';
}

////////////////////////////////////////////////////////////////////////////////
//  Destructs TraceMemoryHandler
////////////////////////////////////////////////////////////////////////////////
TraceMemoryHandler::~TraceMemoryHandler()
{
    // Signal to all callers and work in progress that this instance
    // will be destroyed soon. 
    // As from now, no other caller can get the the lock. They are blocked out.
    die();
    
    // Debug code for the time being
    // dumpTraceBuffer("cimserver.memorydump.trc");
                      
    // Wait until all users have left the critical section
    while ( _inUseCounter.get() > 0 )
    {
        // In any case, lock the buffer unconditional
        _lockCounter.set(0);        
        // Wait for 10ms, to give other therads to finish work.
        Threads::sleep(10);
    }

    delete[] _overflowBuffer;
    delete[] _traceArea;
    
    delete[] _traceFileName;
}

////////////////////////////////////////////////////////////////////////////////
//  Request to lock the memory buffer for writing a trace message.
//
//  The locking of the memory buffer is implemented using a spinlock over
//  an atomic int. The lock is obtained when the atomic lock counter increment
//  results in a lock count of 1. Otherwise the lock count is decremented
//  and incremented again until we end up at 1.
//  To be able to replace in instance of the TraceMemoryHandler, two flags
//  are kept around to control the lock processing:
//  _dying: This flag indicates that the traceMemoryHandler will soon be 
//          destroyed and cannot be used any more. Active attempts to obtain
//          a lock are given up, leaving the spin loop.
//  _inUseCounter: Keeps track of how many callers are trying to obtain a lock
//                 or currently hold the lock. This allows the destructor to
//                 wait for all callers to complete before destroying the
//                 instance.
////////////////////////////////////////////////////////////////////////////////
inline Boolean TraceMemoryHandler::_lockBufferAccess()
{
    if ( _dying )
    {
        // The memory tracing is about to end. 
        // The caller will never get the lock.
        return false;
    }

    // Keep track of work in progress
    _inUseCounter.inc();
    
    // The lock is implemented as a spin loop, since the action to append
    // a trace message to the memory buffer is very short.
    while ( true )
    {
        if ( _dying )
        {
            // The memory tracing is about to end. 
            // The caller will never get the lock.
            _inUseCounter.dec();
            return false;
        }

        // If the lock counter not 1,an other caller is in the critical section.
        if ( _lockCounter.get() == 1 )
        {
            // Decrement the atomic lock counter and test if we do have lock:
            // _lockCounter == 0 
            if ( _lockCounter.decAndTestIfZero() )
            {
                // We do have lock! 
                _numberOfLocksObtained++;
                return true;
            }
        }
        // I did not get the lock. So signal the scheduer to change the active
        // thread to allow other threads to proceed. This also prevents from 
        // looping in a tight loop that causes a dead look due to the 
        // lock optaining thread does not get any time ot finsh his work.
        Threads::yield();
        _contentionCount.inc();
    } 
    
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//  Unlock the memory buffer when no longer used for writing.
////////////////////////////////////////////////////////////////////////////////
inline void TraceMemoryHandler::_unlockBufferAccess()
{
    // set the lock counter to 1 to allow one next user to enter 
    // the critical section.
    _lockCounter.set(1);
    _inUseCounter.dec();
}


////////////////////////////////////////////////////////////////////////////////
// Tells an instance of the traceMemoryHandler that it will be destructed
// soon and should accept no more requests for trace messages.
////////////////////////////////////////////////////////////////////////////////
inline void TraceMemoryHandler::die()
{
    _dying = true;
}

////////////////////////////////////////////////////////////////////////////////
// Appends a marker after the last trace message in the buffer
// The pointer for loction of the next message is not moved, 
// because the marker must be overwritten by the next message it self.
////////////////////////////////////////////////////////////////////////////////
void TraceMemoryHandler::_appendMarker()
{
    if (_leftBytesInBuffer > PEGASUS_TRC_BUFFER_EOT_MARKER_LEN )
    {
        // Marker does fit inot the buffer, so ...
        // ... append it the end of the last written message.
        memcpy(&(_traceArea->traceBuffer[_traceArea->nextPos]),
               PEGASUS_TRC_BUFFER_EOT_MARKER,
               PEGASUS_TRC_BUFFER_EOT_MARKER_LEN );
    }
    else
    {
        // Marker does not fit into the buffer, so ...
        // ... blank out the remainder of the buffer
        memset(&(_traceArea->traceBuffer[_traceArea->nextPos]),
               0,
               _leftBytesInBuffer);

        // ... and put marker to the front of the buffer
        memcpy(&(_traceArea->traceBuffer[0]),
              PEGASUS_TRC_BUFFER_EOT_MARKER,
              PEGASUS_TRC_BUFFER_EOT_MARKER_LEN );
    }
}

////////////////////////////////////////////////////////////////////////////////
//  Dumps the buffer to a given file.
//  This function is not fully implemented yet, but used in tests.
//  It will be revisited at implementing PEGAGAUS_ASSERT
////////////////////////////////////////////////////////////////////////////////
void TraceMemoryHandler::dumpTraceBuffer(const char* filename)
{
    cerr << "Number of lock contentions is <"<< _contentionCount.get()
         << ">" << endl;
    cerr << "Number of obtained locks is <"<< _numberOfLocksObtained
         << ">" << endl;

    ofstream ofile(filename,ios::app&ios::out);
    if( ofile.good() )
    {
        Boolean locked = _lockBufferAccess();
        ofile << _traceArea->traceBuffer << PEGASUS_STD(endl);
        if (locked )
        {
            _unlockBufferAccess();
        }

        ofile.close();    
   }
}


////////////////////////////////////////////////////////////////////////////////
//  Appends a simple fixed length message to the trace buffer
//  WARNING: This is a private method that does not lock the trace buffer.
//           Callers have to lock the buffer prior to calling this method.
////////////////////////////////////////////////////////////////////////////////
inline void TraceMemoryHandler::_appendSimpleMessage(
    const char* message, 
    Uint32 msgLen )
{
    if (_leftBytesInBuffer >= msgLen )
    {
        memcpy(&(_traceArea->traceBuffer[_traceArea->nextPos]),
               message,
               msgLen);
        
        _traceArea->nextPos += msgLen;
        _leftBytesInBuffer -= msgLen;
    }
    else
    {
        // Message doesn't completely fit into buffer, so we need to wrap
        // it around.
        
        // First fill the buffer till the end ...
        memcpy(&(_traceArea->traceBuffer[_traceArea->nextPos]),
               message,
               _leftBytesInBuffer);

        // ... and then add the rest at the beginning
        msgLen = msgLen - _leftBytesInBuffer;
        memcpy(&(_traceArea->traceBuffer[0]),
              message + _leftBytesInBuffer,
              msgLen );
        
        _traceArea->nextPos = msgLen;
        _leftBytesInBuffer = _traceArea->bufferSize - msgLen;
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//  Formats a trace message into the trace buffer
////////////////////////////////////////////////////////////////////////////////
void TraceMemoryHandler::handleMessage(
    const char *message,
    Uint32 msgLen,
    const char *fmt, va_list argList)
{
    if(!_lockBufferAccess())
    {
        // Give up, buffer is going to be destroyed
        return;
    }
    
    // Handle the static part of the message
    _appendSimpleMessage(message, msgLen);
    
    if (_leftBytesInBuffer == 0)
    {
        // Wrap the buffer
        _traceArea->nextPos = 0;
        _leftBytesInBuffer = _traceArea->bufferSize;
    }


    // In case the buffer is too short, we need to invoke vsnprintf twice and 
    // for this need a copy of the argList.
    va_list argListCopy;
    va_copy(argListCopy, argList);

    
    // We just use vsnprintf to format the variable right into the buffer,
    // up to the amount of bytes left.
#ifdef PEGASUS_OS_TYPE_WINDOWS
        // Windows until VC 8 does not support vsnprintf
        // need to use Windows equivalent function with the underscore
    int ttlMsgLen = 
        _vsnprintf(&(_traceArea->traceBuffer[_traceArea->nextPos]), 
                   _leftBytesInBuffer, 
                   fmt, 
                   argList);
#else
    int ttlMsgLen = 
        vsnprintf(&(_traceArea->traceBuffer[_traceArea->nextPos]), 
                  _leftBytesInBuffer, 
                  fmt, 
                  argList);
#endif        
    
    if (((Uint32)ttlMsgLen < _leftBytesInBuffer) &&
        (ttlMsgLen != -1))
    {
        ttlMsgLen++;  //Include the '/0'
        
        _traceArea->nextPos += ttlMsgLen;
        _leftBytesInBuffer -= ttlMsgLen;
    }
    else
    {
        // Reached end of buffer and need to wrap.
        // This is a bit ugly, since we can't just resume after what had
        // already been written, but have to start all over.
        //
        // To do this we format the message to the overflow buffer, and copy
        // the rest of the message from there to the beginning of the trace
        // buffer. 
        // To save memory allocations, the overflow buffer is kept around
        // until it becomes to small and needs to be reallocated.
        if ((Uint32)ttlMsgLen > _overflowBufferSize)
        {
            if (_overflowBuffer != NULL )
            {
                delete[] _overflowBuffer;
            }
            _overflowBufferSize = ttlMsgLen;
            _overflowBuffer = new char[_overflowBufferSize];
        }
        
#ifdef PEGASUS_OS_TYPE_WINDOWS
        // Windows until VC 8 does not support vsnprintf
        // need to use Windows equivalent function with the underscore
        ttlMsgLen = _vsnprintf(_overflowBuffer,
                               _overflowBufferSize,
                               fmt,
                               argListCopy);
#else
        ttlMsgLen = vsnprintf(_overflowBuffer,
                              _overflowBufferSize,
                              fmt,
                              argListCopy);
#endif        
    

        // Now calculate how much data we have to copy from the overflow
        // buffer back to the trace buffer.
        ttlMsgLen -= _leftBytesInBuffer;
        
        // Copy the remainder of the trace message to the trace buffer
        memcpy(&(_traceArea->traceBuffer[0]),
               &(_overflowBuffer[_leftBytesInBuffer]),
               ttlMsgLen );

        _traceArea->nextPos = ttlMsgLen+1;
        _leftBytesInBuffer = _traceArea->bufferSize - _traceArea->nextPos;
    }

    // replace null terminator with line break
    _traceArea->traceBuffer[_traceArea->nextPos-1] = '\n';

    _appendMarker();
    
    _unlockBufferAccess();
}

////////////////////////////////////////////////////////////////////////////////
//  Copies a simple trace message to trace buffer
////////////////////////////////////////////////////////////////////////////////
void TraceMemoryHandler::handleMessage(const char *message, Uint32 msgLen)
{
    if(!_lockBufferAccess())
    {
        // Give up, buffer is going to be destroyed
        return;
    }
    
    // We include the terminating 0 in the message for easier handling
    msgLen++;

    _appendSimpleMessage(message, msgLen);


    // replace null terminator with line break
    _traceArea->traceBuffer[_traceArea->nextPos-1] = '\n';

    _appendMarker();

    _unlockBufferAccess();
}

////////////////////////////////////////////////////////////////////////////////
//  Checks if a given message file is usable
////////////////////////////////////////////////////////////////////////////////
Boolean TraceMemoryHandler::isValidMessageDestination(const char* traceFileName)
{
    TraceFileHandler traceFileHandler;

    return traceFileHandler.isValidMessageDestination( traceFileName );
}


////////////////////////////////////////////////////////////////////////////////
//  Sets the message file in case we need to flush out the trace
////////////////////////////////////////////////////////////////////////////////
Uint32 TraceMemoryHandler::setMessageDestination(const char* destination)
{
    delete[] _traceFileName;
    
    _traceFileName = new char[strlen(destination)+1];
    strcpy(_traceFileName, destination);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  Flushes the trace
////////////////////////////////////////////////////////////////////////////////
void TraceMemoryHandler::flushTrace()
{
    dumpTraceBuffer(_traceFileName);
}

PEGASUS_NAMESPACE_END
