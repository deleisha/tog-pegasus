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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WQLOperation_h
#define Pegasus_WQLOperation_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/WQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

enum WQLOperation
{
    WQL_OR,
    WQL_AND,
    WQL_NOT,
    WQL_EQ,
    WQL_NE,
    WQL_LT,
    WQL_LE,
    WQL_GT,
    WQL_GE,
    WQL_IS_NULL,
    WQL_IS_TRUE,
    WQL_IS_FALSE,
    WQL_IS_NOT_NULL,
    WQL_IS_NOT_TRUE,
    WQL_IS_NOT_FALSE
};

PEGASUS_WQL_LINKAGE const char* WQLOperationToString(WQLOperation x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLOperation_h */
