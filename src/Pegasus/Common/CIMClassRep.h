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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMClassRep_h
#define Pegasus_CIMClassRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectRep.h>
#include <Pegasus/Common/CIMMethod.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMClass;
class CIMConstClass;
class CIMInstanceRep;

// REVIEW: document this class.

class PEGASUS_COMMON_LINKAGE CIMClassRep : public CIMObjectRep
{
public:

    CIMClassRep(
	const String& className,
	const String& superClassName);

    virtual ~CIMClassRep();

    Boolean isAssociation() const;

    Boolean isAbstract() const;

    const String& getSuperClassName() const { return _superClassName; }

    void setSuperClassName(const String& superClassName);

    virtual void addProperty(const CIMProperty& x);

    void addMethod(const CIMMethod& x);

    Uint32 findMethod(const String& name);

    Uint32 findMethod(const String& name) const
    {
	return ((CIMClassRep*)this)->findMethod(name);
    }

    Boolean existsMethod(const String& name);

    Boolean existsMethod(const String& name) const
    {
	return ((CIMClassRep*)this)->existsMethod(name);
    }

    CIMMethod getMethod(Uint32 pos);

    CIMConstMethod getMethod(Uint32 pos) const
    {
	return ((CIMClassRep*)this)->getMethod(pos);
    }

    void removeMethod(Uint32 pos);

    Uint32 getMethodCount() const;

    void resolve(
	DeclContext* context,
	const String& nameSpace);

    Boolean identical(const CIMClassRep* x) const;

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream)& os = PEGASUS_STD(cout)) const;

    void toMof(Array<Sint8>& out) const;

    void printMof(PEGASUS_STD(ostream)& os = PEGASUS_STD(cout)) const;

    virtual CIMObjectRep* clone() const
    {
	return new CIMClassRep(*this);
    }

    void getKeyNames(Array<String>& keyNames) const;

    Boolean hasKeys() const;

private:

    CIMClassRep();

    CIMClassRep(const CIMClassRep& x);

    CIMClassRep& operator=(const CIMClassRep& x)
    {
	return *this;
    }

    String _superClassName;
    Array<CIMMethod> _methods;

    friend class CIMClass;
    friend class CIMInstanceRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMClassRep_h */
