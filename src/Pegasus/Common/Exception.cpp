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
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "Exception.h"
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

Exception::Exception(const String& message) : _message(message)
{
}

Exception::Exception(const char* message) : _message(message)
{
}

Exception::~Exception()
{
}


const char OutOfBounds::MSG[] = "out of bounds";

const char AlreadyExists::MSG[] = "already exists: ";

const char IllegalName::MSG[] = "illegal CIM name";

const char IllegalNamespaceName::MSG[] = "illegal CIM namespace name";

const char UninitializedHandle::MSG[] = "uninitialized handle";

const char UninitializedObject::MSG[] = "uninitialized object";

const char AddedReferenceToClass::MSG[] =
    "attempted to add reference to a non-association class: ";

const char TruncatedCharacter::MSG[] =
    "truncated character during conversion from Char16 to char";

const char ExpectedReferenceValue::MSG[] =
    "Expected CIMValue object to be of type reference "
    "in this context";

const char MissingReferenceClassName::MSG[] = "missing reference class name";

const char TypeMismatch::MSG[] = "type mismatch";

const char CIMValueInvalidType::MSG[] = "invalid CIMValue type";

const char DynamicCastFailed::MSG[] = "dynamic cast failed";

const char BadDateTimeFormat::MSG[] = "bad datetime format";

const char IncompatibleTypes::MSG[] = "incompatible types";

const char IllformedObjectName::MSG[] = "illformed object name: ";

const char BindFailedException::MSG[] = "Bind failed: ";

const char InvalidLocatorException::MSG[] = "Invalid locator: ";

const char CannotCreateSocketException::MSG[] = "Cannot create socket";

const char CannotConnectException::MSG[] = "Cannot connect to: ";

const char UnexpectedFailureException::MSG[] = "Unexpected failure";

const char AlreadyConnectedException::MSG[] = "already connected";

const char NotConnectedException::MSG[] = "not connected";

const char ConnectionTimeoutException::MSG[] = "connection timed out";

const char SSLException::MSG[] = "SSL Exception ";


////////////////////////////////////////////////////////////////////////////////
//
// CIMException
//
////////////////////////////////////////////////////////////////////////////////

//
// Creates a description without source file name and line number.
//
static String _makeCIMExceptionDescription(
    CIMStatusCode code,
    const String& message)
{
    String tmp;
    tmp.append(cimStatusCodeToString(code));
    if (message != String::EMPTY)
    {
        tmp.append(": \"");
        tmp.append(message);
        tmp.append("\"");
    }
    return tmp;
}

//
// Creates a description with source file name and line number.
//
static String _makeCIMExceptionDescription(
    CIMStatusCode code,
    const String& message,
    const char* file,
    Uint32 line)
{
    String tmp = file;
    tmp.append("(");
    char buffer[32];
    sprintf(buffer, "%d", line);
    tmp.append(buffer);
    tmp.append("): ");
    tmp.append(_makeCIMExceptionDescription(code, message));
    return tmp;
}

CIMException::CIMException(
    CIMStatusCode code,
    const String& message,
    const char* file,
    Uint32 line)
    :
    Exception(message),
    _code(code),
    _file(file),
    _line(line)
{

}

//
// Returns a description string fit for human consumption
//
String CIMException::getDescription() const
{
#ifdef DEBUG_CIMEXCEPTION
    return getTraceDescription();
#else
    return _makeCIMExceptionDescription(_code, getMessage());
#endif
}

//
// Returns a description string with filename and line number information
// specifically for tracing.
//
String CIMException::getTraceDescription() const
{
    String traceDescription =
        _makeCIMExceptionDescription(_code, getMessage(), _file, _line);

    return traceDescription;
}

PEGASUS_NAMESPACE_END
