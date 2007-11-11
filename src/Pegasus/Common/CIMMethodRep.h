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

#ifndef Pegasus_MethodRep_h
#define Pegasus_MethodRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/CIMParameterRep.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/OrderedSet.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstMethod;
class DeclContext;

class CIMMethodRep : public Sharable
{
public:

    CIMMethodRep(
        const CIMName& name,
        CIMType type,
        const CIMName& classOrigin,
        Boolean propagated);

    ~CIMMethodRep();

    virtual const CIMName& getName() const
    {
        return _name;
    }

    const Uint32 getNameTag() const
    {
        return _nameTag;
    }

    void increaseOwnerCount()
    {
        _ownerCount++;
        return;
    }

    void decreaseOwnerCount()
    {
        _ownerCount++;
        return;
    }
    
    void setName(const CIMName& name);

    CIMType getType() const
    {
        return _type;
    }

    void setType(CIMType type);

    const CIMName& getClassOrigin() const
    {
        return _classOrigin;
    }

    void setClassOrigin(const CIMName& classOrigin);

    Boolean getPropagated() const
    {
        return _propagated;
    }

    void setPropagated(Boolean propagated)
    {
        _propagated = propagated;
    }

    void addQualifier(const CIMQualifier& qualifier)
    {
        _qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const CIMName& name) const
    {
        return _qualifiers.find(name);
    }

    CIMQualifier getQualifier(Uint32 index)
    {
        return _qualifiers.getQualifier(index);
    }


    CIMConstQualifier getQualifier(Uint32 index) const
    {
        return _qualifiers.getQualifier(index);
    }

    void removeQualifier(Uint32 index)
    {
        _qualifiers.removeQualifier(index);
    }


    Uint32 getQualifierCount() const
    {
        return _qualifiers.getCount();
    }

    void addParameter(const CIMParameter& x);

    Uint32 findParameter(const CIMName& name) const;

    CIMParameter getParameter(Uint32 index);

    CIMConstParameter getParameter(Uint32 index) const
    {
        return ((CIMMethodRep*)this)->getParameter(index);
    }

    void removeParameter (Uint32 index);

    Uint32 getParameterCount() const;

    void resolve(
        DeclContext* declContext,
        const CIMNamespaceName& nameSpace,
        const CIMConstMethod& method);

    void resolve(
        DeclContext* declContext,
        const CIMNamespaceName& nameSpace);

    void toXml(Buffer& out) const;

    void toMof(Buffer& out) const;

    Boolean identical(const CIMMethodRep* x) const;

    CIMMethodRep* clone() const
    {
        return new CIMMethodRep(*this);
    }

private:

    CIMMethodRep(const CIMMethodRep& x);

    CIMMethodRep();    // Unimplemented
    CIMMethodRep& operator=(const CIMMethodRep& x);    // Unimplemented

    CIMName _name;
    CIMType _type;
    CIMName _classOrigin;
    Boolean _propagated;
    CIMQualifierList _qualifiers;
    Uint32 _nameTag;
    Uint32 _ownerCount;

    typedef OrderedSet<CIMParameter,
                       CIMParameterRep,
                       PEGASUS_PARAMETER_ORDEREDSET_HASHSIZE> ParameterSet;
    ParameterSet _parameters;

    friend class CIMClassRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MethodRep_h */
