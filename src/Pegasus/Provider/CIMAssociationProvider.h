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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMAssociationProvider_h
#define Pegasus_CIMAssociationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMProvider.h>

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Provider/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
This class defines the set of methods implemented by an association provider. A providers that derives
from this class must implement all methods. The minimal method implementation simply throw the
NotSupported exception.
*/
class PEGASUS_PROVIDER_LINKAGE CIMAssociationProvider : public virtual CIMProvider
{
public:
    CIMAssociationProvider(void);
    virtual ~CIMAssociationProvider(void);

    /**

    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param associationClass
    @param resultClass
    @param role
    @param resultRole
    @param includeQualifiers
    @param includeClassOrigin
    @param propertyList

    @param handler asynchronously processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void associators(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler) = 0;

    /**
    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param associationClass
    @param resultClass
    @param role
    @param resultRole

    @param handler asynchronously processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void associatorNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	ObjectPathResponseHandler & handler) = 0;

    /**
    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param resultClass
    @param role
    @param resultRole
    @param includeQualifiers
    @param includeClassOrigin
    @param propertyList

    @param handler asynchronously processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void references(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler) = 0;

    /**
    @param context contains security and locale information relevant for the lifetime
    of this operation.

    @param objectName
    @param resultClass
    @param role

    @param handler asynchronously processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void referenceNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	ObjectPathResponseHandler & handler) = 0;
};

PEGASUS_NAMESPACE_END

#endif
