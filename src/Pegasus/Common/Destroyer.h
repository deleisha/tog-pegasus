//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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

#ifndef Pegasus_Destroyer_h
#define Pegasus_Destroyer_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class provides a convenient way of disposing of a heap object.
    It automatically deletes the enclosed pointer on destruction. For
    example:

    <pre>
	A* a = new A;
	Destroyer<A> dummy = a;
    </pre>

    When the destroyer object destructs, it frees the instance of A.
    This is particularly useful when a function has multiple returns.

    There are two classes here: Destroyer<> and ArrayDestroyer<>. The
    ArrayDestroyer<> class is used when a pointer must be deleted using the 
    array form as shown below:

    <pre>
	delete [] ptr;
    <pre>
*/
template<class T>
class Destroyer
{
public:

    Destroyer(T* ptr) : _ptr(ptr) { }

    ~Destroyer() { delete _ptr; }

    T* getPointer() { return _ptr; }

private:

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
    Destroyer(const Destroyer<T>&) { }

    Destroyer<T>& operator=(const Destroyer<T>&) { return *this; }
#else
    Destroyer(const Destroyer&) { }

    Destroyer& operator=(const Destroyer&) { return *this; }
#endif

    T* _ptr;
};

template<class T>
class ArrayDestroyer
{
public:

    ArrayDestroyer(T* ptr) : _ptr(ptr) { }

    ~ArrayDestroyer() { delete [] _ptr; }

    T* getPointer() { return _ptr; }

    ArrayDestroyer(const ArrayDestroyer<T>&) { }

    ArrayDestroyer<T>& operator=(const ArrayDestroyer<T>&) { return *this; }

private:

    T* _ptr;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Destroyer_h */
