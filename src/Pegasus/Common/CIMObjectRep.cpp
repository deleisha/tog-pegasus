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

#include "CIMObjectRep.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/MessageLoader.h>
#include "CIMPropertyRep.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

CIMObjectRep::CIMObjectRep(const CIMObjectRep& x)
    : Sharable(), _reference(x._reference)
{
    x._qualifiers.cloneTo(_qualifiers);

    _properties.reserveCapacity(x._properties.size());

    for (Uint32 i = 0, n = x._properties.size(); i < n; i++)
    {
        _properties.append(x._properties[i].clone());
    }
}

CIMObjectRep::CIMObjectRep(const CIMObjectPath& reference)
{
    // ensure the class name is not null
    if (reference.getClassName().isNull())
    {
        throw UninitializedObjectException();
    }

    _reference = reference;
}

CIMObjectRep::~CIMObjectRep()
{
}

void CIMObjectRep::addProperty(const CIMProperty& x)
{
    if (x.isUninitialized())
    {
        throw UninitializedObjectException();
    }

    // Reject duplicate property names:

    if (findProperty(x._rep->_name, x._rep->_nameTag) != PEG_NOT_FOUND)
    {
        MessageLoaderParms parms(
            "Common.CIMObjectRep.PROPERTY",
            "property \"$0\"",
            x.getName().getString());

        throw AlreadyExistsException(parms);
    }

    // Append property:

    _properties.append(x);
}

Boolean CIMObjectRep::identical(const CIMObjectRep* x) const
{
    // If the pointers are the same, the objects must be identical
    if (this == x)
    {
        return true;
    }

    if (!_reference.identical(x->_reference))
    {
        return false;
    }

    if (!_qualifiers.identical(x->_qualifiers))
    {
        return false;
    }

    // Compare properties:

    {
        const PropertySet& tmp1 = _properties;
        const PropertySet& tmp2 = x->_properties;

        if (tmp1.size() != tmp2.size())
        {
            return false;
        }

        for (Uint32 i = 0, n = tmp1.size(); i < n; i++)
        {
            if (!tmp1[i].identical(tmp2[i]))
            {
                return false;
            }
        }
    }

    return true;
}

void CIMObjectRep::setPath(const CIMObjectPath& path)
{
    // ensure the class name is not null
    if (path.getClassName().isNull())
    {
        throw UninitializedObjectException();
    }

    // prevent changing the class name (type) in when updating the object path
    if (!_reference.getClassName().equal(path.getClassName()))
    {
        throw TypeMismatchException();
    }

    _reference = path;
}

PEGASUS_NAMESPACE_END
