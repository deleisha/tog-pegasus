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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <Pegasus/Common/InternalException.h>
#include "CIMScope.h"

PEGASUS_NAMESPACE_BEGIN

const CIMScope CIMScope::NONE = 0;
const CIMScope CIMScope::CLASS = 1;
const CIMScope CIMScope::ASSOCIATION = 2;
const CIMScope CIMScope::INDICATION = 4;
const CIMScope CIMScope::PROPERTY = 8;
const CIMScope CIMScope::REFERENCE = 16;
const CIMScope CIMScope::METHOD = 32;
const CIMScope CIMScope::PARAMETER = 64;
const CIMScope CIMScope::ANY = CIMScope::CLASS + CIMScope::ASSOCIATION +
                               CIMScope::INDICATION + CIMScope::PROPERTY +
                               CIMScope::REFERENCE + CIMScope::METHOD +
                               CIMScope::PARAMETER;

CIMScope::CIMScope ()
    : cimScope (CIMScope::NONE.cimScope)
{
}

CIMScope::CIMScope (const CIMScope & scope)
    : cimScope (scope.cimScope)
{
}

CIMScope::CIMScope (const Uint32 scope)
    : cimScope (scope)
{
    PEGASUS_ASSERT (scope < 128);
}

CIMScope & CIMScope::operator= (const CIMScope & scope)
{
    this->cimScope = scope.cimScope;
    return *this;
}

void CIMScope::addScope (const CIMScope & scope)
{
    this->cimScope |= scope.cimScope;
}

Boolean CIMScope::hasScope (const CIMScope & scope) const
{
    return ((this->cimScope & scope.cimScope) == scope.cimScope);
}

Boolean CIMScope::equal (const CIMScope & scope) const
{
    return (this->cimScope == scope.cimScope);
}

CIMScope CIMScope::operator+ (const CIMScope & scope) const
{
    return CIMScope(this->cimScope | scope.cimScope);
}

String CIMScope::toString () const
{
    String tmp;

    if (this->hasScope (CIMScope::CLASS))
	tmp.append("CLASS ");

    if (this->hasScope (CIMScope::ASSOCIATION))
	tmp.append("ASSOCIATION ");

    if (this->hasScope (CIMScope::INDICATION))
	tmp.append("INDICATION ");

    if (this->hasScope (CIMScope::PROPERTY))
	tmp.append("PROPERTY ");

    if (this->hasScope (CIMScope::REFERENCE))
	tmp.append("REFERENCE ");

    if (this->hasScope (CIMScope::METHOD))
	tmp.append("METHOD ");

    if (this->hasScope (CIMScope::PARAMETER))
	tmp.append("PARAMETER ");

    if (tmp.size())
	tmp.remove(tmp.size() - 1);

    return tmp;
}

PEGASUS_NAMESPACE_END
