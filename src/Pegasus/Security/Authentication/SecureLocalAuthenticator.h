//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SecureLocalAuthenticator_h
#define Pegasus_SecureLocalAuthenticator_h

#include "Linkage.h"
#include "LocalAuthenticator.h"


PEGASUS_NAMESPACE_BEGIN

/**
    Implements file based secure authentication mechanism. It extends the 
    LocalAuthenticator and provides implementation.
*/   
class PEGASUS_SECURITY_LINKAGE SecureLocalAuthenticator : public LocalAuthenticator
{
public:

    /** constructor. */ 
    SecureLocalAuthenticator();

    /** destructor. */ 
    ~SecureLocalAuthenticator();

    /** authenticate. */ 
    Boolean authenticate(String userName, String password);

    /** authenticate. */ 
    Boolean authenticate(
        String userName, 
        String secretReceived, 
        String secretKept);

    /** getAuthResponseHeader. */ 
    String getAuthResponseHeader(String requestHeader, String& challenge);
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_SecureLocalAuthenticator_h */
