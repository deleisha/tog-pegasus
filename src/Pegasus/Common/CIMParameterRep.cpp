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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include "CIMParameter.h"
#include "CIMParameterRep.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

CIMParameterRep::CIMParameterRep(
    const CIMName& name, 
    CIMType type,
    Boolean isArray,
    Uint32 arraySize,
    const CIMName& referenceClassName) 
    : _name(name), _type(type), 
    _isArray(isArray), _arraySize(arraySize), 
    _referenceClassName(referenceClassName)
{
    if (_type == CIMTYPE_NONE)
	throw NullType();

    if (_arraySize && !_isArray)
	throw IncompatibleTypesException();

    if (!referenceClassName.isNull())
    {
	if (_type != CIMTYPE_REFERENCE)
	{
	    throw ExpectedReferenceValueException();
	}
    }
    else
    {
        // ATTN: revisit this later!
#if 0
	if (_type == CIMTYPE_REFERENCE)
	    throw MissingReferenceClassNameException();
#endif
    }
}

CIMParameterRep::~CIMParameterRep()
{

}

void CIMParameterRep::setName(const CIMName& name) 
{
    _name = name; 
}

void CIMParameterRep::removeQualifier(Uint32 pos)
{
    if (pos >= _qualifiers.getCount())
        throw IndexOutOfBoundsException();

    _qualifiers.removeQualifier (pos);
}

void CIMParameterRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace)
{
    // Validate the qualifiers of the method (according to
    // superClass's method with the same name). This method
    // will throw an exception if the validation fails.

    CIMQualifierList dummy;

    _qualifiers.resolve(
	declContext,
	nameSpace,
	CIMScope::PARAMETER,
	false,
	dummy,
	true);
}

void CIMParameterRep::toXml(Array<Sint8>& out) const
{
    if (_isArray)
    {
	out << "<PARAMETER.ARRAY";

	out << " NAME=\"" << _name << "\" ";

	out << " TYPE=\"" << cimTypeToString (_type) << "\"";

	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "%d", _arraySize);
	    out << " ARRAYSIZE=\"" << buffer << "\"";
	}

	out << ">\n";

	_qualifiers.toXml(out);

	out << "</PARAMETER.ARRAY>\n";
    }
    else if (_type == CIMTYPE_REFERENCE)
    {
	out << "<PARAMETER.REFERENCE";
	out << " NAME=\"" << _name << "\" ";
	out << " REFERENCECLASS=\"" << _referenceClassName << "\"";
	out << ">\n";

	_qualifiers.toXml(out);

	out << "</PARAMETER.REFERENCE>\n";
    }
    else
    {
	out << "<PARAMETER";
	out << " NAME=\"" << _name << "\" ";
	out << " TYPE=\"" << cimTypeToString (_type) << "\"";
	out << ">\n";

	_qualifiers.toXml(out);

	out << "</PARAMETER>\n";
    }
}

/** toMof - puts the Mof representation of teh Parameter object to
    the output parameter array
    The BNF for this conversion is:
    parameterList    = 	parameter *( "," parameter )

	parameter    = 	[ qualifierList ] (dataType|objectRef) parameterName
				[ array ]

	parameterName= 	IDENTIFIER
	
	array 	     = 	"[" [positiveDecimalValue] "]"
	
    Format on a single line.
    */
void CIMParameterRep::toMof(Array<Sint8>& out) const
{
    // Output the qualifiers for the parameter
    _qualifiers.toMof(out);

    if (_qualifiers.getCount())
	out << " ";

    // Output the data type and name
    out << cimTypeToString (_type) << " " <<  _name;

    if (_isArray)
    {
	//Output the array indicator "[ [arraysize] ]"
	if (_arraySize)
	{
	    char buffer[32];
	    sprintf(buffer, "[%d]", _arraySize);
	    out << buffer;
	}
	else
	    out << "[]";
    }
}


Boolean CIMParameterRep::identical(const CIMParameterRep* x) const
{
    if (_name != x->_name)
	return false;

    if (_type != x->_type)
	return false;

    if (_referenceClassName != x->_referenceClassName)
	return false;

    if (!_qualifiers.identical(x->_qualifiers))
	return false;

    return true;
}

CIMParameterRep::CIMParameterRep()
{

}

CIMParameterRep::CIMParameterRep(const CIMParameterRep& x) :
    Sharable(),
    _name(x._name),
    _type(x._type),
    _isArray(x._isArray),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName)
{
    x._qualifiers.cloneTo(_qualifiers);
}

void CIMParameterRep::setType(CIMType type)
{ 
    _type = type;

    if (_referenceClassName.isNull() && _type == CIMTYPE_REFERENCE)
    {
	throw MissingReferenceClassNameException();
    }
}

PEGASUS_NAMESPACE_END
