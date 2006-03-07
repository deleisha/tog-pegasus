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

#ifndef _Pegasus_StringRep_h
#define _Pegasus_StringRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/AtomicInt.h>
#include <new>

PEGASUS_NAMESPACE_BEGIN

struct StringRep
{
    StringRep();

    ~StringRep();

    static StringRep* alloc(size_t cap);

    static void free(StringRep* rep);

    static StringRep* create(const Uint16* data, size_t size);

    static StringRep* create(const char* data, size_t size);

    static StringRep* copyOnWrite(StringRep* rep);

    static Uint32 length(const Uint16* str);

    static void ref(const StringRep* rep);

    static void unref(const StringRep* rep);

    static StringRep _emptyRep;

    // Number of characters in this string, excluding the null terminator.
    size_t size;

    // Number of characters this representation has room for. This is
    // greater or equal to size.
    size_t cap;

    // Number of string refering to this StringRep (1, 2, etc).
    AtomicInt refs;

    // The first character in the string. Extra space is allocated off the
    // end of this structure for additional characters.
    Uint16 data[1];
};

inline void StringRep::free(StringRep* rep)
{
    rep->refs.~AtomicInt();
    ::operator delete(rep);
}

inline StringRep::StringRep() : size(0), cap(0), refs(2)
{
    // Only called on _emptyRep. We set the reference count to two to
    // keep a String from modifying it (if the reference count were one,
    // a string would think it was the sole owner of the StringRep object).
    data[0] = 0;
}

inline StringRep::~StringRep()
{
    // Only called on _emptyRep.
}

inline void StringRep::ref(const StringRep* rep)
{
    if (rep != &StringRep::_emptyRep)
        ((StringRep*)rep)->refs++;
}

inline void StringRep::unref(const StringRep* rep)
{
    if (rep != &StringRep::_emptyRep && 
	((StringRep*)rep)->refs.decAndTestIfZero())
        StringRep::free((StringRep*)rep);
}

PEGASUS_COMMON_LINKAGE void StringThrowOutOfBounds();

PEGASUS_COMMON_LINKAGE void StringAppendCharAux(StringRep*& _rep);

PEGASUS_COMMON_LINKAGE Boolean StringEqualNoCase(
    const String& s1, const String& s2);

PEGASUS_COMMON_LINKAGE Uint32 StringFindAux(
    const StringRep* _rep, const Char16* s, Uint32 n);

inline void _checkBounds(size_t index, size_t size)
{
#ifndef PEGASUS_STRING_NO_THROW
    if (index > size)
        StringThrowOutOfBounds();
#endif
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_StringRep_h */
