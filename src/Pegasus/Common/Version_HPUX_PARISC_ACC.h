//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Denise Eckstein (denise_eckstein@hp.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#define PLATFORM_PRODUCT_NAME  "HP WBEM Services"
#define PLATFORM_PRODUCT_VERSION  "A.00.01"
#define PLATFORM_PRODUCT_STATUS  "Pre-Release" 

#ifndef PLATFORM_COMPONENT_NAME
#define PLATFORM_COMPONENT_NAME  "HP WBEM Services Component"
#endif

#ifndef PLATFORM_COMPONENT_VERSION
#define PLATFORM_COMPONENT_VERSION PLATFORM_PRODUCT_VERSION 
#endif

#ifndef PLATFORM_COMPONENT_STATUS
#define PLATFORM_COMPONENT_STATUS PLATFORM_PRODUCT_STATUS
#endif

const char PLATFORM_EMBEDDED_IDENTIFICATION_STRING[] = "@(#)"PLATFORM_PRODUCT_NAME" "PLATFORM_PRODUCT_VERSION" "PLATFORM_PRODUCT_STATUS"\n@(#)"PLATFORM_PRODUCT_NAME"-"PLATFORM_COMPONENT_NAME" "PLATFORM_COMPONENT_VERSION" "PLATFORM_COMPONENT_STATUS;
