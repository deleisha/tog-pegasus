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
// Modified By: Roger Kumpf, Hewlett Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include "CIMName.h"

static bool illegalNameException = false;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMName
//
////////////////////////////////////////////////////////////////////////////////

#define PEGASUS_ARRAY_T CIMName
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

CIMName::CIMName()
    : cimName(String::EMPTY)
{
}

CIMName::CIMName(const String& name)
    : cimName(name)
{
    if (!legal(name))
    {
        // ATTN: Does this clean up String memory properly?
        if (illegalNameException)
            throw InvalidNameException(name);
        else
            PEGASUS_STD(cout) << "KSTEST CIMName illegal 1 " << name << PEGASUS_STD(endl);
    }
}

CIMName::CIMName(const char* name)
    : cimName(name)
{
    if (!legal(name))
    {
        // ATTN: Does this clean up String memory properly?
        if (illegalNameException)
            throw InvalidNameException(name);
        else
            PEGASUS_STD(cout) << "KSTEST CIMName illegal 2 " << name << PEGASUS_STD(endl);

    }
}

CIMName& CIMName::operator=(const CIMName& name)
{
    cimName=name.cimName;
    return *this;
}

CIMName& CIMName::operator=(const String& name)
{
    if (!legal(name))
    {
        if (illegalNameException)
            throw InvalidNameException(name);
        else
            PEGASUS_STD(cout) << "KSTEST CIMName illegal 3 " << name << PEGASUS_STD(endl);

    }
    cimName=name;
    return *this;
}

#ifndef PEGASUS_REMOVE_DEPRECATED
CIMName& CIMName::operator=(const char* name)
{
    cimName=name;
    return *this;
}
#endif

const String& CIMName::getString() const
{
    return cimName;
}

#ifndef PEGASUS_REMOVE_DEPRECATED
CIMName::operator String() const
{
    return cimName;
}
#endif

Boolean CIMName::isNull() const
{
    return (cimName.size() == 0);
}

void CIMName::clear()
{
    cimName.clear();
}

Boolean CIMName::legal(const String& name)
{
    Uint32 length = name.size();

    if (length == 0 || !(isalpha(name[0]) || name[0] == '_'))
        return false;

    for (Uint32 i=1; i<length; i++)
    {
        // ATTN-RK-20020729: Is this check necessary?  Add it to namespace name?
        if (name[i] > PEGASUS_MAX_PRINTABLE_CHAR)
            return false;

        if (!(isalnum(name[i]) || name[i] == '_'))
            return false;
    }

    return true;
}

Boolean CIMName::equal(const CIMName& name) const
{
    return String::equalNoCase(cimName, name.cimName);
}

Boolean operator==(const CIMName& name1, const CIMName& name2)
{
    return name1.equal(name2);
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMNamespaceName
//
////////////////////////////////////////////////////////////////////////////////

#define PEGASUS_ARRAY_T CIMNamespaceName
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

CIMNamespaceName::CIMNamespaceName()
    : cimNamespaceName(String::EMPTY)
{
}

CIMNamespaceName::CIMNamespaceName(const String& name)
    : cimNamespaceName(name)
{
    if (!legal(name))
    {
        // ATTN: Does this clean up String memory properly?
        if (illegalNameException)
            throw InvalidNamespaceNameException(name);
        else
            PEGASUS_STD(cout) << "KSTEST CIMNamespaceName illegal 1 " << name << PEGASUS_STD(endl);

    }
}

CIMNamespaceName::CIMNamespaceName(const char* name)
    : cimNamespaceName(name)
{
    if (!legal(name))
    {
        // ATTN: Does this clean up String memory properly?
        if (illegalNameException)
            throw InvalidNamespaceNameException(name);
        else
            PEGASUS_STD(cout) << "KSTEST CIMNameNamespaceName illegal 2 " << name << PEGASUS_STD(endl);

    }
}

CIMNamespaceName& CIMNamespaceName::operator=(const CIMNamespaceName& name)
{
    cimNamespaceName=name.cimNamespaceName;
    return *this;
}

CIMNamespaceName& CIMNamespaceName::operator=(const String& name)
{
    if (!legal(name))
    {
        if (illegalNameException)
            throw InvalidNamespaceNameException(name);
        else
            PEGASUS_STD(cout) << "KSTEST CIMNameNamespaceName illegal 3 " << name << PEGASUS_STD(endl);

    }
    cimNamespaceName=name;
    return *this;
}

#ifndef PEGASUS_REMOVE_DEPRECATED
CIMNamespaceName& CIMNamespaceName::operator=(const char* name)
{
    cimNamespaceName=name;
    return *this;
}
#endif

const String& CIMNamespaceName::getString() const
{
    return cimNamespaceName;
}

#ifndef PEGASUS_REMOVE_DEPRECATED
CIMNamespaceName::operator String() const
{
    return cimNamespaceName;
}
#endif

Boolean CIMNamespaceName::isNull() const
{
    return (cimNamespaceName.size() == 0);
}

void CIMNamespaceName::clear()
{
    cimNamespaceName.clear();
}

Boolean CIMNamespaceName::legal(const String& name)
{
    Uint32 length = name.size();
    if (length == 0) return true;    // ATTN: Cheap hack!

    // ATTN-RK-20020729: Hack: Skip leading '/' because spec is ambiguous
    Uint32 start = 0;
    if (name[0] == '/')
    {
        start++;
    }

    for (Uint32 i=start; i<length; )
    {
        if (!name[i] || (!isalpha(name[i]) && name[i] != '_'))
        {
            return false;
        }

        i++;

        while (isalnum(name[i]) || name[i] == '_')
        {
            i++;
        }

        if (name[i] == '/')
        {
            i++;
        }
    }

    return true;

// Alternate implementation
#if 0
    String temp;

    // check each namespace segment (delimited by '/') for correctness

    for(Uint32 i = 0; i < name.size(); i += temp.size() + 1)
    {
        // isolate the segment beginning at i and ending at the first
        // ocurrance of '/' after i or eos

        temp = name.subString(i, name.subString(i).find('/'));

        // check segment for correctness

        if (!CIMName::legal(temp))
        {
            return false;
        }
    }

    return true;
#endif
}

Boolean CIMNamespaceName::equal(const CIMNamespaceName& name) const
{
    return String::equalNoCase(cimNamespaceName, name.cimNamespaceName);
}

Boolean operator==(const CIMNamespaceName& name1, const CIMNamespaceName& name2)
{
    return name1.equal(name2);
}

PEGASUS_NAMESPACE_END
