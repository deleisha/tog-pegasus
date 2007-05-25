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

#ifndef Pegasus_HTTPMessage_h
#define Pegasus_HTTPMessage_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/Buffer.h>

PEGASUS_NAMESPACE_BEGIN

typedef Pair<String, String> HTTPHeader;

/** This message is sent from a connection to its output queue when
    a complete HTTP message is received.
*/
class PEGASUS_COMMON_LINKAGE HTTPMessage : public Message
{
public:

    typedef Message Base;

    HTTPMessage(
        const Buffer& message_,
        Uint32 queueId_ = 0,
        const CIMException* cimException_ = 0);

    HTTPMessage(const HTTPMessage & msg);

    Buffer message;
    Uint32 queueId;
    String ipAddress;
    AuthenticationInfo* authInfo;
    AcceptLanguageList acceptLanguages;
    ContentLanguageList contentLanguages;
    Boolean acceptLanguagesDecoded;
    Boolean contentLanguagesDecoded;
    CIMException cimException;

    void parse(
        String& startLine,
        Array<HTTPHeader>& headers,
        Uint32& contentLength) const;

#ifdef PEGASUS_DEBUG
    void printAll(PEGASUS_STD(ostream)& os) const;
#endif

    static void lookupHeaderPrefix(
        Array<HTTPHeader>& headers,
        const String& fieldName,
        String& prefix);

    static Boolean lookupHeader(
        Array<HTTPHeader>& headers,
        const String& fieldName,
        String& fieldValue,
        Boolean allowNamespacePrefix = false);

    static Boolean parseRequestLine(
        const String& startLine,
        String& methodName,
        String& requestUri,
        String& httpVersion);

    static Boolean parseStatusLine(
        const String& statusLine,
        String& httpVersion,
        Uint32& statusCode,
        String& reasonPhrase);

    /**
        Advances a given pointer to the first character that is not
        HTTP header whitespace (space or tabs).

        @param  str    Input string.
    */
    static void skipHeaderWhitespace(const char*& str);

    /**
        Looks for a token in the beginning of a given string.

        @param  str    input string to be searched
        @param  token  token to look for

        @return true   if the token is found in the string after skipping
                        the leading whitespaces if any.
        @return false  if the token is not found.
    */
    static Boolean expectHeaderToken(const char*& str, const char *token);

    /**
        Returns a pointer to the first CRLF or a LF separator.

        @param  data   input string.
        @param  size   size of the input string.

        @return pointer to the first CRLF or LF separator if any, else
                returns NULL.
    */
    static char* findSeparator(
        const char* data,
        Uint32 size);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPMessage_h */
