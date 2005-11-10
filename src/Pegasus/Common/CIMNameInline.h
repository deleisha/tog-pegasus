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
//%/////////////////////////////////////////////////////////////////////////////

/* If neither of these defined, suppress compilation of this header. */
#if !defined(PEGASUS_INTERNALONLY) && !defined(Pegasus_CIMNameInline_cxx)
# define Pegasus_CIMNameInline_h
#endif

#ifndef Pegasus_CIMNameInline_h
#define Pegasus_CIMNameInline_h

#include <Pegasus/Common/CIMName.h>

#ifdef Pegasus_CIMNameInline_cxx
# define PEGASUS_CIMNAME_INLINE
#else
# define PEGASUS_CIMNAME_INLINE inline
#endif

PEGASUS_NAMESPACE_BEGIN

PEGASUS_CIMNAME_INLINE CIMName::CIMName()
{
}

PEGASUS_CIMNAME_INLINE CIMName& CIMName::operator=(const CIMName& name)
{
    cimName = name.cimName;
    return *this;
}

PEGASUS_CIMNAME_INLINE const String& CIMName::getString() const
{
    return cimName;
}

PEGASUS_CIMNAME_INLINE Boolean CIMName::isNull() const
{
    return (cimName.size() == 0);
}

PEGASUS_CIMNAME_INLINE void CIMName::clear()
{
    cimName.clear();
}

PEGASUS_CIMNAME_INLINE Boolean CIMName::equal(const CIMName& name) const
{
    return String::equalNoCase(cimName, name.cimName);
}

PEGASUS_CIMNAME_INLINE Boolean CIMName::equal(const char* name) const
{
    return String::equalNoCase(cimName, name);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const CIMName& name1, const CIMName& name2)
{
    return name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const CIMName& name1, const char* name2)
{
    return name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator==(
    const char* name1, const CIMName& name2)
{
    return name2.equal(name1);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const CIMName& name1, const CIMName& name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const CIMName& name1, const char* name2)
{
    return !name1.equal(name2);
}

PEGASUS_CIMNAME_INLINE Boolean operator!=(
    const char* name1, const CIMName& name2)
{
    return !name2.equal(name1);
}

PEGASUS_CIMNAME_INLINE CIMNamespaceName::CIMNamespaceName()
{
}

PEGASUS_CIMNAME_INLINE const String& CIMNamespaceName::getString() const
{
    return cimNamespaceName;
}

PEGASUS_CIMNAME_INLINE Boolean CIMNamespaceName::isNull() const
{
    return (cimNamespaceName.size() == 0);
}

PEGASUS_CIMNAME_INLINE void CIMNamespaceName::clear()
{
    cimNamespaceName.clear();
}

PEGASUS_CIMNAME_INLINE Boolean CIMNamespaceName::equal(
    const CIMNamespaceName& name) const
{
    return String::equalNoCase(cimNamespaceName, name.cimNamespaceName);
}

PEGASUS_CIMNAME_INLINE Boolean CIMNamespaceName::equal(const char* name) const
{
    return String::equalNoCase(cimNamespaceName, name);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMNameInline_h */
