//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: XmlReader.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:32  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "Name.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "Qualifier.h"
#include "QualifierDecl.h"
#include "ClassDecl.h"
#include "InstanceDecl.h"

PEGASUS_NAMESPACE_BEGIN

static const Uint32 MESSAGE_SIZE = 128;

//------------------------------------------------------------------------------
//
// expectXmlDeclaration()
//
//------------------------------------------------------------------------------

void XmlReader::expectXmlDeclaration(
    XmlParser& parser, 
    XmlEntry& entry)
{
    if (!parser.next(entry) ||
	entry.type != XmlEntry::XML_DECLARATION ||
	strcmp(entry.text, "xml") != 0)
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected <?xml ... ?> style declaration");
    }
}

//------------------------------------------------------------------------------
//
// expectStartTag()
//
//------------------------------------------------------------------------------

void XmlReader::expectStartTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	entry.type != XmlEntry::START_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Expected open of %s element", tagName);
	throw XmlValidationError(parser.getLine(), message);
    }
}

//------------------------------------------------------------------------------
//
// expectEndTag()
//
//------------------------------------------------------------------------------

void XmlReader::expectEndTag(XmlParser& parser, const char* tagName)
{
    XmlEntry entry;

    if (!parser.next(entry) ||
	entry.type != XmlEntry::END_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Expected close of %s element", tagName);
	throw XmlValidationError(parser.getLine(), message);
    }
}

//------------------------------------------------------------------------------
//
// expectStartTagOrEmptyTag()
//
//------------------------------------------------------------------------------

void XmlReader::expectStartTagOrEmptyTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::START_TAG &&
	entry.type != XmlEntry::EMPTY_TAG) ||
	strcmp(entry.text, tagName) != 0)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, 
	    "Expected either open or open/close %s element", tagName);
	throw XmlValidationError(parser.getLine(), message);
    }
}

//------------------------------------------------------------------------------
//
// expectContentOrCData()
//
//------------------------------------------------------------------------------

Boolean XmlReader::expectContentOrCData(
    XmlParser& parser, 
    XmlEntry& entry)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::CONTENT &&
	entry.type != XmlEntry::CDATA))
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected content of CDATA");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testStartTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	entry.type != XmlEntry::START_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testEndTag>()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testEndTag(XmlParser& parser, const char* tagName)
{
    XmlEntry entry;

    if (!parser.next(entry) ||
	entry.type != XmlEntry::END_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testStartTagOrEmptyTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testStartTagOrEmptyTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::START_TAG &&
	entry.type != XmlEntry::EMPTY_TAG) ||
	strcmp(entry.text, tagName) != 0)
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testContentOrCData()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testContentOrCData(
    XmlParser& parser, 
    XmlEntry& entry)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::CONTENT &&
	entry.type != XmlEntry::CDATA))
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testCimStartTag()
//
//     <!ELEMENT CIM (MESSAGE|DECLARATION)>
//     <!ATTRLIST CIM 
//         CIMVERSION CDATA #REQUIRED
//         DTDVERSION CDATA #REQUIRED>
//
//------------------------------------------------------------------------------

void XmlReader::testCimStartTag(XmlParser& parser)
{
    XmlEntry entry;
    XmlReader::expectStartTag(parser, entry, "CIM");

    const char* cimVersion;

    if (!entry.getAttributeValue("CIMVERSION", cimVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing CIM.CIMVERSION attribute");

    if (strcmp(cimVersion, "2.0") != 0)
	throw XmlValidationError(parser.getLine(), 
	    "CIM.CIMVERSION attribute must be \"2.0\"");

    const char* dtdVersion;

    if (!entry.getAttributeValue("DTDVERSION", dtdVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing CIM.DTDVERSION attribute");

    if (strcmp(dtdVersion, "2.0") != 0)
	throw XmlValidationError(parser.getLine(), 
	    "CIM.DTDVERSION attribute must be \"2.0\"");
}

//------------------------------------------------------------------------------
//
// getIsArrayAttribute()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIsArrayAttribute(
    Uint32 lineNumber,
    const XmlEntry& entry,
    const char* tagName,
    Boolean& value)
{
    const char* tmp;

    if (!entry.getAttributeValue("ISARRAY", tmp))
	return false;

    if (strcmp(tmp, "true") == 0)
    {
	value = true;
	return true;
    }
    else if (strcmp(tmp, "false") == 0)
    {
	value = false;
	return true;
    }

    char buffer[62];
    sprintf(buffer, "Bad %s.%s attribute value", "ISARRAY", tagName);
    throw XmlSemanticError(lineNumber, buffer);
    return false;
}

//------------------------------------------------------------------------------
//
// getCimNameAttribute()
//
//     <!ENTITY % CIMName "NAME CDATA #REQUIRED">
//
//------------------------------------------------------------------------------

String XmlReader::getCimNameAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName,
    Boolean acceptNull)
{
    String name;

    if (!entry.getAttributeValue("NAME", name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, "missing %s.NAME attribute", elementName);
	throw XmlValidationError(lineNumber, buffer);
    }

    if (acceptNull && name.getLength() == 0)
	return name;

    if (!Name::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, "Illegal value for %s.NAME attribute", elementName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getClassNameAttribute()
//
//     <!ENTITY % CIMName "CLASSNAME CDATA #REQUIRED">
//
//------------------------------------------------------------------------------

String XmlReader::getClassNameAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String name;

    if (!entry.getAttributeValue("CLASSNAME", name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, "missing %s.CLASSNAME attribute", elementName);
	throw XmlValidationError(lineNumber, buffer);
    }

    if (!Name::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, 
	    "Illegal value for %s.CLASSNAME attribute", elementName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getClassOriginAttribute()
//
//     <!ENTITY % ClassOrigin "CLASSORIGIN CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

String XmlReader::getClassOriginAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* tagName)
{
    String name;

    if (!entry.getAttributeValue("CLASSORIGIN", name))
	return String();

    if (!Name::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, 
	    "Illegal value for %s.CLASSORIGIN attribute", tagName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getReferenceClassAttribute()
//
//     <!ENTITY % ReferenceClass "REFERENCECLASS CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

String XmlReader::getReferenceClassAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String name;

    if (!entry.getAttributeValue("REFERENCECLASS", name))
	return String();

    if (!Name::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, 
	    "Illegal value for %s.REFERENCECLASS attribute", elementName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getSuperClassAttribute()
//
//     <!ENTITY % SuperClass "SUPERCLASS CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

String XmlReader::getSuperClassAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* tagName)
{
    String superClass;

    if (!entry.getAttributeValue("SUPERCLASS", superClass))
	return String();

    if (!Name::legal(superClass))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(
	    buffer, "Illegal value for %s.SUPERCLASS attribute", tagName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return superClass;
}

//------------------------------------------------------------------------------
//
// getCimTypeAttribute()
//
//     <!ENTITY % CIMType "TYPE (boolean|string|char16|uint8|sint8|uint16
//         |sint16|uint32|sint32|uint64|sint64|datetime|real32|real64)">
//
//------------------------------------------------------------------------------

Type XmlReader::getCimTypeAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry, 
    const char* tagName)
{
    const char* typeName;

    if (!entry.getAttributeValue("TYPE", typeName))
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "missing %s.TYPE attribute", tagName);
	throw XmlValidationError(lineNumber, message);
    }

    Type type = Type::NONE;

    if (strcmp(typeName, "boolean") == 0)
	type = Type::BOOLEAN;
    else if (strcmp(typeName, "string") == 0)
	type = Type::STRING;
    else if (strcmp(typeName, "char16") == 0)
	type = Type::CHAR16;
    else if (strcmp(typeName, "uint8") == 0)
	type = Type::UINT8;
    else if (strcmp(typeName, "sint8") == 0)
	type = Type::SINT8;
    else if (strcmp(typeName, "uint16") == 0)
	type = Type::UINT16;
    else if (strcmp(typeName, "sint16") == 0)
	type = Type::SINT16;
    else if (strcmp(typeName, "uint32") == 0)
	type = Type::UINT32;
    else if (strcmp(typeName, "sint32") == 0)
	type = Type::SINT32;
    else if (strcmp(typeName, "uint64") == 0)
	type = Type::UINT64;
    else if (strcmp(typeName, "sint64") == 0)
	type = Type::SINT64;
    else if (strcmp(typeName, "datetime") == 0)
	type = Type::DATETIME;
    else if (strcmp(typeName, "real32") == 0)
	type = Type::REAL32;
    else if (strcmp(typeName, "real64") == 0)
	type = Type::REAL64;
    else if (strcmp(typeName, "reference") == 0)
	type = Type::REFERENCE;

    // ATTN: "reference" is not legal according to the DTD; however, it is
    // used by the XML version of the CIM schema.

    if (type == Type::NONE)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Illegal value for %s.TYPE attribute", tagName);
	throw XmlSemanticError(lineNumber, message);
    }

    return type;
}

//------------------------------------------------------------------------------
//
// getCimBooleanAttribute()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getCimBooleanAttribute(
    Uint32 lineNumber,
    const XmlEntry& entry,
    const char* tagName,
    const char* attributeName,
    Boolean defaultValue,
    Boolean required)
{
    const char* tmp;

    if (!entry.getAttributeValue(attributeName, tmp))
    {
	if (!required)
	    return defaultValue;

	char buffer[62];
	sprintf(buffer, "missing required %s.%s attribute", 
	    attributeName, tagName);

	throw XmlValidationError(lineNumber, buffer);
    }

    if (strcmp(tmp, "true") == 0)
	return true;
    else if (strcmp(tmp, "false") == 0)
	return false;

    char buffer[62];
    sprintf(buffer, "Bad %s.%s attribute value", attributeName, tagName);
    throw XmlSemanticError(lineNumber, buffer);
    return false;
}

//------------------------------------------------------------------------------
//
// SringToReal()
//
//	[ "+" | "-" ] *decimalDigit "." 1*decimalDigit 
//	    [ ( "e" | "E" ) [ "+" | "-" ] 1*decimalDigit ]
//
//------------------------------------------------------------------------------

Boolean XmlReader::stringToReal(const char* stringValue, Real64& x)
{
    const char* p = stringValue;

    if (!*p)
	return false;

    // Skip optional sign:

    if (*p == '+' || *p  == '-')
	p++;

    // Skip optional first set of digits:

    while (isdigit(*p))
	p++;

    // Test required dot:

    if (*p++ != '.')
	return false;

    // One or more digits required:

    if (!isdigit(*p++))
	return false;

    while (isdigit(*p))
	p++;

    // If there is an exponent now:

    if (*p)
    {
	// Test exponent:

	if (*p != 'e' && *p != 'E')
	    return false;

	p++;

	// Skip optional sign:

	if (*p == '+' || *p  == '-')
	    p++;

	// One or more digits required:

	if (!isdigit(*p++))
	    return false;

	while (isdigit(*p))
	    p++;
    }

    if (*p)
	return false;

    char* end;
    x = strtod(stringValue, &end);
    return true;
}

//------------------------------------------------------------------------------
//
// stringToSignedInteger
//
//	[ "+" | "-" ] ( positiveDecimalDigit *decimalDigit | "0" )
//
// ATTN-B: handle conversion from hexadecimal.
//------------------------------------------------------------------------------

Boolean XmlReader::stringToSignedInteger(
    const char* stringValue, 
    Sint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!*p)
	return false;

    // Skip optional sign:

    Boolean negative = *p == '-';

    if (negative || *p == '+')
	p++;

    // If the next thing is a zero, then it must be the last:

    if (*p == '0')
	return p[1] == '\0';

    // Expect a positive decimal digit:

    const char* first = p;

    if (!isdigit(*p) || *p == '0')
	return false;

    p++;

    // Expect zero or more digits:

    while (isdigit(*p))
	p++;

    if (*p)
	return false;

    const char* last = p;

    while (first != last)
	x = 10 * x + (*first++ - '0');

    if (negative)
	x = -x;

    return true;
}

//------------------------------------------------------------------------------
//
// stringToUnsignedInteger
//
//	[ "+" | "-" ] ( positiveDecimalDigit *decimalDigit | "0" )
//
// ATTN-B: handle conversion from hexadecimal.
//------------------------------------------------------------------------------

Boolean XmlReader::stringToUnsignedInteger(
    const char* stringValue, 
    Uint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!*p)
	return false;

    // Skip optional sign:

    if (*p == '-')
	return false;

    if (*p == '+')
	p++;

    // If the next thing is a zero, then it must be the last:

    if (*p == '0')
	return p[1] == '\0';

    // Expect a positive decimal digit:

    const char* first = p;

    if (!isdigit(*p) || *p == '0')
	return false;

    p++;

    // Expect zero or more digits:

    while (isdigit(*p))
	p++;

    if (*p)
	return false;

    const char* last = p;

    while (first != last)
	x = 10 * x + (*first++ - '0');

    return true;
}

//------------------------------------------------------------------------------
//
// stringToValue()
//
// ATTN-C: note that integers are truncated without warning. What should be 
// done in this case? In C they are truncated without warning by design.
//
//------------------------------------------------------------------------------

Value XmlReader::stringToValue(
    Uint32 lineNumber, 
    const char* valueString, 
    Type type)
{
    // ATTN-B: accepting only UTF-8 for now! (affects string and char16):

    switch (type)
    {
	case Type::BOOLEAN:
	{
	    if (strcmp(valueString, "TRUE") == 0)
		return Value(true);
	    else if (strcmp(valueString, "FALSE") == 0)
		return Value(false);
	    else
		throw XmlSemanticError(
		    lineNumber, "Bad boolean value");
	}

	case Type::STRING:
	{
	    return Value(valueString);
	}

	case Type::CHAR16:
	{
	    if (strlen(valueString) != 1)
		throw XmlSemanticError(lineNumber, "Bad char16 value");

	    return Value(Char16(valueString[0]));
	}

	case Type::UINT8:
	case Type::UINT16:
	case Type::UINT32:
	case Type::UINT64:
	{
	    Uint64 x;

	    if (!stringToUnsignedInteger(valueString, x))
	    {
		throw XmlSemanticError(
		    lineNumber, "Bad unsigned integer value");
	    }

	    switch (type)
	    {
		case Type::UINT8: return Value(Uint8(x));
		case Type::UINT16: return Value(Uint16(x));
		case Type::UINT32: return Value(Uint32(x));
		case Type::UINT64: return Value(Uint64(x));
		default: break;
	    }
	}

	case Type::SINT8:
	case Type::SINT16:
	case Type::SINT32:
	case Type::SINT64:
	{
	    Sint64 x;

	    if (!stringToSignedInteger(valueString, x))
	    {
		throw XmlSemanticError(
		    lineNumber, "Bad signed integer value");
	    }

	    switch (type)
	    {
		case Type::SINT8: return Value(Sint8(x));
		case Type::SINT16: return Value(Sint16(x));
		case Type::SINT32: return Value(Sint32(x));
		case Type::SINT64: return Value(Sint64(x));
		default: break;
	    }
	}

	case Type::DATETIME:
	{
	    DateTime tmp;

	    try
	    {
		tmp.set(valueString);
	    }
	    catch (BadDateTimeFormat&)
	    {
		throw XmlSemanticError(lineNumber, "Bad datetime value");
	    }

	    return Value(tmp);
	}

	case Type::REAL32:
	{
	    Real64 x;

	    if (!stringToReal(valueString, x))
		throw XmlSemanticError(lineNumber, "Bad real value");

	    return Value(Real32(x));
	}

	case Type::REAL64:
	{
	    Real64 x;

	    if (!stringToReal(valueString, x))
		throw XmlSemanticError(lineNumber, "Bad real value");

	    return Value(x);
	}

	default:
	    break;
    }

    throw XmlSemanticError(lineNumber, "malformed XML");
    return false;
}

//------------------------------------------------------------------------------
//
// getValueElement()
//
//     <!ELEMENT VALUE (#PCDATA)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueElement(
    XmlParser& parser, 
    Type type, 
    Value& value)
{
    // Get VALUE start tag:

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "VALUE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get VALUE content:

    const char* valueString = "";

    if (!empty)
    {
	if (testContentOrCData(parser, entry))
	    valueString = entry.text;

	expectEndTag(parser, "VALUE");
    }

    value = stringToValue(parser.getLine(), valueString,type);
    return true;
}

//------------------------------------------------------------------------------
//
// stringArrayToValue()
//
//------------------------------------------------------------------------------

template<class T>
Value StringArrayToValueAux(
    Uint32 lineNumber, 
    const Array<const char*>& stringArray,
    Type type,
    T*)
{
    Array<T> array;

    for (Uint32 i = 0, n = stringArray.getSize(); i < n; i++)
    {
	Value value = XmlReader::stringToValue(
	    lineNumber, stringArray[i], type);

	T x;
	value.get(x);
	array.append(x);
    }

    return Value(array);
}

Value XmlReader::stringArrayToValue(
    Uint32 lineNumber, 
    const Array<const char*>& array, 
    Type type)
{
    switch (type)
    {
	case Type::BOOLEAN: 
	    return StringArrayToValueAux(lineNumber, array, type, (Boolean*)0);

	case Type::STRING:
	    return StringArrayToValueAux(lineNumber, array, type, (String*)0);

	case Type::CHAR16:
	    return StringArrayToValueAux(lineNumber, array, type, (Char16*)0);

	case Type::UINT8:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint8*)0);

	case Type::UINT16:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint16*)0);

	case Type::UINT32:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint32*)0);

	case Type::UINT64:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint64*)0);

	case Type::SINT8:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint8*)0);

	case Type::SINT16:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint16*)0);

	case Type::SINT32:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint32*)0);

	case Type::SINT64:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint64*)0);

	case Type::DATETIME:
	    return StringArrayToValueAux(lineNumber, array, type, (DateTime*)0);

	case Type::REAL32:
	    return StringArrayToValueAux(lineNumber, array, type, (Real32*)0);

	case Type::REAL64:
	    return StringArrayToValueAux(lineNumber, array, type, (Real64*)0);

	default:
	    break;
    }

    // Unreachable:
    return Value();
}

//------------------------------------------------------------------------------
//
// getValueArrayElement()
//
//     <!ELEMENT VALUE.ARRAY (VALUE*)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueArrayElement(
    XmlParser& parser, 
    Type type, 
    Value& value)
{
    value.clear();

    // Get VALUE.ARRAY open tag:

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "VALUE.ARRAY"))
	return false;

    if (entry.type == XmlEntry::EMPTY_TAG)
	return true;

    // For each VALUE element:

    Array<const char*> stringArray;

    while (testStartTagOrEmptyTag(parser, entry, "VALUE"))
    {
	if (entry.type == XmlEntry::EMPTY_TAG)
	{
	    stringArray.append("");
	    continue;
	}

	if (testContentOrCData(parser, entry))
	    stringArray.append(entry.text);
	else
	    stringArray.append("");

	expectEndTag(parser, "VALUE");
    }

    expectEndTag(parser, "VALUE.ARRAY");

    value = stringArrayToValue(parser.getLine(), stringArray, type);
    return true;
}

//------------------------------------------------------------------------------
//
// getFlavor()
//
//     <!ENTITY % QualifierFlavor 
//         "OVERRIDABLE (true|false) 'true'
//         TOSUBCLASS (true|false) 'true'
//         TOINSTANCE (true|false)  'false'
//         TRANSLATABLE (true|false)  'false'">
//
//------------------------------------------------------------------------------

Uint32 XmlReader::getFlavor(
    XmlEntry& entry, 
    Uint32 lineNumber, 
    const char* tagName)
{
    // Get QUALIFIER.OVERRIDABLE

    Boolean overridable = getCimBooleanAttribute(
	lineNumber, entry, tagName, "OVERRIDABLE", true, false);

    // Get QUALIFIER.TOSUBCLASS

    Boolean toSubClass = getCimBooleanAttribute(
	lineNumber, entry, tagName, "TOSUBCLASS", true, false);

    // Get QUALIFIER.TOINSTANCE

    Boolean toInstance = getCimBooleanAttribute(
	lineNumber, entry, tagName, "TOINSTANCE", false, false);

    // Get QUALIFIER.TRANSLATABLE

    Boolean translatable = getCimBooleanAttribute(
	lineNumber, entry, tagName, "TRANSLATABLE", false, false);

    Uint32 flavor = 0;

    if (overridable)
	flavor |= Flavor::OVERRIDABLE;

    if (toSubClass)
	flavor |= Flavor::TOSUBCLASS;

    if (toInstance)
	flavor |= Flavor::TOINSTANCE;

    if (translatable)
	flavor |= Flavor::TRANSLATABLE;

    return flavor;
}

//------------------------------------------------------------------------------
//
// getOptionalScope()
//
//     DTD:
//         <!ELEMENT SCOPE EMPTY>
//         <!ATTLIST SCOPE 
//              CLASS (true|false) 'false'
//              ASSOCIATION (true|false) 'false'
//              REFERENCE (true|false) 'false'
//              PROPERTY (true|false) 'false'
//              METHOD (true|false) 'false'
//              PARAMETER (true|false) 'false'
//              INDICATION (true|false) 'false'>
//
//------------------------------------------------------------------------------

Uint32 XmlReader::getOptionalScope(XmlParser& parser)
{
    XmlEntry entry;

    if (!parser.next(entry))
	return false;

    Boolean isEmptyTag = entry.type == XmlEntry::EMPTY_TAG;

    if ((!isEmptyTag && 
	entry.type != XmlEntry::START_TAG) ||
	strcmp(entry.text, "SCOPE") != 0)
    {
	parser.putBack(entry);
	return 0;
    }

    Uint32 line = parser.getLine();
    Uint32 scope = 0;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "CLASS", false, false))
	scope |= Scope::CLASS;

    if (getCimBooleanAttribute(
	line, entry, "SCOPE", "ASSOCIATION", false, false))
	scope |= Scope::ASSOCIATION;

    if (getCimBooleanAttribute(
	line, entry, "SCOPE", "REFERENCE", false, false))
	scope |= Scope::REFERENCE;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "PROPERTY", false, false))
	scope |= Scope::PROPERTY;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "METHOD", false, false))
	scope |= Scope::METHOD;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "PARAMETER", false, false))
	scope |= Scope::PARAMETER;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "INDICATION",false, false))
	scope |= Scope::INDICATION;

    if (!isEmptyTag)
	expectEndTag(parser, "SCOPE");

    return scope;
}

//------------------------------------------------------------------------------
//
// getQualifierElement()
//
//     <!ELEMENT QUALIFIER (VALUE|VALUE.ARRAY)>
//     <!ATTLIST QUALIFIER
//         %CIMName;
//         %CIMType; #REQUIRED
//         %Propagated;
//         %QualifierFlavor;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getQualifierElement(
    XmlParser& parser, 
    Qualifier& qualifier)
{
    // Get QUALIFIER element:

    XmlEntry entry;
    if (!testStartTag(parser, entry, "QUALIFIER"))
	return false;

    // Get QUALIFIER.NAME attribute:

    String name = getCimNameAttribute(parser.getLine(), entry, "QUALIFIER");

    // Get QUALIFIER.TYPE attribute:

    Type type = getCimTypeAttribute(parser.getLine(), entry, "QUALIFIER");

    // Get QUALIFIER.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "QUALIFIER", "PROPAGATED", false, false);

    // Get flavor oriented attributes:

    Uint32 flavor = getFlavor(entry, parser.getLine(), "QUALIFIER");

    // Get VALUE or VALUE.ARRAY element:

    Value value;

    if (!getValueElement(parser, type, value) &&
	!getValueArrayElement(parser, type, value))
    {
	throw XmlSemanticError(parser.getLine(),
	    "Expected VALUE or VALUE.ARRAY element");
    }

    // Expect </QUALIFIER>:

    expectEndTag(parser, "QUALIFIER");

    // Build qualifier:

    qualifier = Qualifier(name, value, flavor, propagated);
    return true;
}

//------------------------------------------------------------------------------
//
// getQualifierElements()
//
//------------------------------------------------------------------------------

template<class CONTAINER>
void getQualifierElements(XmlParser& parser, CONTAINER& container)
{
    Qualifier qualifier;

    while (XmlReader::getQualifierElement(parser, qualifier))
    {
	try
	{
	    container.addQualifier(qualifier);
	}
	catch (AlreadyExists&)
	{
	    throw XmlSemanticError(parser.getLine(), "duplicate qualifier");
	}
    }
}

//------------------------------------------------------------------------------
//
// getPropertyElement()
//
//     <!ELEMENT PROPERTY (QUALIFIER*,VALUE?)>
//     <!ATTLIST PROPERTY
//         %CIMName;
//         %ClassOrigin;
//         %Propagated;
//         %CIMType; #REQUIRED>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getPropertyElement(XmlParser& parser, Property& property)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PROPERTY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PROPERTY.NAME attribute:

    String name = getCimNameAttribute(parser.getLine(), entry, "PROPERTY");

    // Get PROPERTY.CLASSORIGIN attribute:

    String classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY");

    // Get PROPERTY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY", "PROPAGATED", false, false);

    // Get PROPERTY.TYPE attribute:

    Type type = getCimTypeAttribute(parser.getLine(), entry, "PROPERTY");

    // Create property:

    Value value;
    value.setNullValue(type, false);
    property = Property(
	name, value, 0, String(), classOrigin, propagated);

    if (!empty)
    {
	// Get qualifiers:

	getQualifierElements(parser, property);

	// Get value:

	if (getValueElement(parser, type, value))
	    property.setValue(value);

	expectEndTag(parser, "PROPERTY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getArraySizeAttribute()
//
//     Returns true if able to get array-size. Note that array size will
//     always be a positive integer.
//
//     <!ENTITY % ArraySize "ARRAYSIZE CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

Boolean XmlReader::getArraySizeAttribute(
    Uint32 lineNumber,
    const XmlEntry& entry,
    const char* tagName,
    Uint32& value)
{
    const char* tmp;

    if (!entry.getAttributeValue("ARRAYSIZE", tmp))
	return false;

    Uint64 arraySize;

    if (!stringToUnsignedInteger(tmp, arraySize) || arraySize == 0)
    {
	char message[128];
	sprintf(message, "Illegal value for %s.%s", tagName, "ARRAYSIZE");
	throw XmlSemanticError(lineNumber, message);
    }

    value = Uint32(arraySize);
    return true;
}

//------------------------------------------------------------------------------
//
// getPropertyArrayElement()
//
//     <!ELEMENT PROPERTY.ARRAY (QUALIFIER*,VALUE.ARRAY?)>
//     <!ATTLIST PROPERTY.ARRAY
//             %CIMName;
//             %CIMType; #REQUIRED
//             %ArraySize;
//             %ClassOrigin;
//             %Propagated;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getPropertyArrayElement(
    XmlParser& parser, 
    Property& property)
{
    // Get PROPERTY element:

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PROPERTY.ARRAY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PROPERTY.NAME attribute:

    String name = 
	getCimNameAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.TYPE attribute:

    Type type = getCimTypeAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PROPERTY.ARRAY", arraySize);

    // Get PROPERTY.CLASSORIGIN attribute:

    String classOrigin 
	= getClassOriginAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.ARRAY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY.ARRAY", "PROPAGATED", false, false);

    // Create property:

    Value nullValue;
    nullValue.setNullValue(type, true, arraySize);
    property = Property(
	name, nullValue, arraySize, String(), classOrigin, propagated);

    if (!empty)
    {
	// Get qualifiers:

	getQualifierElements(parser, property);

	// Get value:

	Value value;

	if (getValueArrayElement(parser, type, value))
	{
	    if (arraySize && arraySize != value.getArraySize())
	    {
		throw XmlSemanticError(parser.getLine(),
		    "ARRAYSIZE attribute and value-array size are different");
	    }

	    property.setValue(value);
	}

	expectEndTag(parser, "PROPERTY.ARRAY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getHostElement()
//
//     <!ELEMENT HOST (#PCDATA)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getHostElement(
    XmlParser& parser,
    String& host)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "HOST"))
	return false;

    if (!parser.next(entry) || entry.type != XmlEntry::CONTENT)
    {
	throw XmlValidationError(parser.getLine(),
	    "expected content of HOST element");
    }

    host = entry.text;

    expectEndTag(parser, "HOST");
    return true;
}

//------------------------------------------------------------------------------
//
// getNameSpaceElement()
//     
//     <!ELEMENT NAMESPACE EMPTY>
//     <!ATTLIST NAMESPACE %CIMName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getNameSpaceElement(
    XmlParser& parser,
    String& nameSpaceComponent)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "NAMESPACE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    nameSpaceComponent = getCimNameAttribute(
	parser.getLine(), entry, "NAMESPACE");

    if (!empty)
	expectEndTag(parser, "NAMESPACE");

    return true;
}

//------------------------------------------------------------------------------
//
// getLocalNameSpacePathElement()
//     
//     <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getLocalNameSpacePathElement(
    XmlParser& parser,
    String& nameSpace)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "LOCALNAMESPACEPATH"))
	return false;

    String nameSpaceComponent;

    while (getNameSpaceElement(parser, nameSpaceComponent))
    {
	if (nameSpace.getLength())
	    nameSpace += '/';

	nameSpace += nameSpaceComponent;
    }

    if (!nameSpace.getLength())
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected one or more NAMESPACE elements within "
	    "LOCALNAMESPACEPATH element");
    }

    expectEndTag(parser, "LOCALNAMESPACEPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getNameSpacePathElement()
//
//     <!ELEMENT NAMESPACEPATH (HOST,LOCALNAMESPACEPATH)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getNameSpacePathElement(
    XmlParser& parser,
    String& host,
    String& nameSpace)
{
    host.clear();
    nameSpace.clear();

    XmlEntry entry;

    if (!testStartTag(parser, entry, "NAMESPACEPATH"))
	return false;

    if (!getHostElement(parser, host))
	throw XmlValidationError(parser.getLine(), "expected HOST element");

    if (!getLocalNameSpacePathElement(parser, nameSpace))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected LOCALNAMESPACEPATH element");
    }

    expectEndTag(parser, "NAMESPACEPATH");

    return true;
}

//------------------------------------------------------------------------------
//
// getClassNameElement()
//
//     <!ELEMENT CLASSNAME EMPTY>
//     <!ATTLIST CLASSNAME %CIMName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getClassNameElement(
    XmlParser& parser,
    String& className,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "CLASSNAME"))
    {
	if (required)
	{
	    throw XmlValidationError(parser.getLine(),
		"expected CLASSNAME element");
	}
	else
	    return false;
    }

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    className = getCimNameAttribute(
	parser.getLine(), entry, "CLASSNAME", true);

    if (!empty)
	expectEndTag(parser, "CLASSNAME");

    return true;
}

//------------------------------------------------------------------------------
//
// getValueTypeAttribute()
//
//     VALUETYPE (string|boolean|numeric) 'string'
//
//------------------------------------------------------------------------------

KeyBinding::Type XmlReader::getValueTypeAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String tmp;

    if (!entry.getAttributeValue("VALUETYPE", tmp))
	return KeyBinding::STRING;

    if (tmp == "string")
	return KeyBinding::STRING;
    else if (tmp == "boolean")
	return KeyBinding::BOOLEAN;
    else if (tmp == "numeric")
	return KeyBinding::NUMERIC;

    char buffer[MESSAGE_SIZE];

    sprintf(buffer, 
	"Illegal value for %s.VALUETYPE attribute; "
	"Value must be one of \"string\", \"boolean\", or \"numeric\"",
	elementName);

    throw XmlSemanticError(lineNumber, buffer);
    return KeyBinding::BOOLEAN;
}

//------------------------------------------------------------------------------
//
// getKeyValueElement()
//
//     <!ELEMENT KEYVALUE (#PCDATA)>
//     <!ATTLIST KEYVALUE
//         VALUETYPE (string|boolean|numeric)  'string'>
//
// ATTN-B: VALUE.REFERENCE ignored above; can't understand why it is needed!
//
//------------------------------------------------------------------------------

Boolean XmlReader::getKeyValueElement(
    XmlParser& parser,
    KeyBinding::Type& type,
    String& value)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "KEYVALUE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    type = getValueTypeAttribute(parser.getLine(), entry, "KEYVALUE");

    value.clear();

    if (!empty)
    {
	if (!parser.next(entry))
	    throw XmlException(XmlException::UNCLOSED_TAGS, parser.getLine());

	if (entry.type == XmlEntry::CONTENT)
	    value = entry.text;
	else
	    parser.putBack(entry);

	expectEndTag(parser, "KEYVALUE");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getKeyBindingElement()
//
//     <!ELEMENT KEYBINDING (KEYVALUE|VALUE.REFERENCE)>
//     <!ATTLIST KEYBINDING
//         %CIMName;>
//
// ATTN-B: VALUE.REFERENCE ignored above; can't understand why it is needed!
//
//------------------------------------------------------------------------------

Boolean XmlReader::getKeyBindingElement(
    XmlParser& parser,
    String& name,
    String& value,
    KeyBinding::Type& type)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "KEYBINDING"))
	return false;

    name = getCimNameAttribute(parser.getLine(), entry, "KEYBINDING");

    if (!getKeyValueElement(parser, type, value))
	throw XmlValidationError(parser.getLine(), "Expected KEYVALUE element");

    expectEndTag(parser, "KEYBINDING");
    return true;
}

//------------------------------------------------------------------------------
//
// getInstanceNameElement()
//
//     <!ELEMENT INSTANCENAME (KEYBINDING*|KEYVALUE?|VALUE.REFERENCE?)>
//     <!ATTLIST INSTANCENAME
//         %ClassName;>
//
// ATTN-B: VALUE.REFERENCE sub-element not accepted yet.
// ATTN-B: KEYVALUE sub-element nothandled yet.
//
//------------------------------------------------------------------------------

Boolean XmlReader::getInstanceNameElement(
    XmlParser& parser,
    String& className,
    Array<KeyBinding>& keyBindings)
{
    className.clear();
    keyBindings.clear();

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "INSTANCENAME"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    className = getClassNameAttribute(parser.getLine(), entry, "INSTANCENAME");

    if (!empty)
    {
	String name;
	KeyBinding::Type type;
	String value;

	while (getKeyBindingElement(parser, name, value, type))
	    keyBindings.append(KeyBinding(name, value, type));

	if (!empty)
	    expectEndTag(parser, "INSTANCENAME");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getInstancePathElement()
//
//     <!ELEMENT INSTANCEPATH (NAMESPACEPATH,INSTANCENAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getInstancePathElement(
    XmlParser& parser,
    Reference& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "INSTANCEPATH"))
	return false;

    String host;
    String nameSpace;

    if (!getNameSpacePathElement(parser, host, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected NAMESPACEPATH element");
    }

    String className;
    Array<KeyBinding> keyBindings;

    if (!getInstanceNameElement(parser, className, keyBindings))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected INSTANCENAME element");
    }

    reference.set(host, nameSpace, className, keyBindings);

    expectEndTag(parser, "INSTANCEPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getLocalInstancePathElement()
//
//     <!ELEMENT LOCALINSTANCEPATH (NAMESPACEPATH,INSTANCENAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getLocalInstancePathElement(
    XmlParser& parser,
    Reference& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "LOCALINSTANCEPATH"))
	return false;

    String nameSpace;

    if (!getLocalNameSpacePathElement(parser, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected LOCALNAMESPACEPATH element");
    }

    String className;
    Array<KeyBinding> keyBindings;

    if (!getInstanceNameElement(parser, className, keyBindings))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected INSTANCENAME element");
    }

    reference.set(String(), nameSpace, className, keyBindings);

    expectEndTag(parser, "LOCALINSTANCEPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getClassPathElement()
//
//     <!ELEMENT CLASSPATH (NAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getClassPathElement(
    XmlParser& parser,
    Reference& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "CLASSPATH"))
	return false;

    String host;
    String nameSpace;

    if (!getNameSpacePathElement(parser, host, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected NAMESPACEPATH element");
    }

    String className;

    if (!getClassNameElement(parser, className))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected CLASSNAME element");
    }

    reference.set(host, nameSpace, className);

    expectEndTag(parser, "CLASSPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getLocalClassPathElement()
//
//     <!ELEMENT LOCALCLASSPATH (LOCALNAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getLocalClassPathElement(
    XmlParser& parser,
    Reference& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "LOCALCLASSPATH"))
	return false;

    String nameSpace;

    if (!getLocalNameSpacePathElement(parser, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected LOCALNAMESPACEPATH element");
    }

    String className;

    if (!getClassNameElement(parser, className))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected CLASSNAME element");
    }

    reference.set(String(), nameSpace, className);

    expectEndTag(parser, "LOCALCLASSPATH");

    return true;
}

//------------------------------------------------------------------------------
//
// getValueReferenceElement()
//
//     <!ELEMENT VALUE.REFERENCE (CLASSPATH|LOCALCLASSPATH|CLASSNAME|
//         INSTANCEPATH|LOCALINSTANCEPATH|INSTANCENAME)>
//
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueReferenceElement(
    XmlParser& parser,
    Reference& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.REFERENCE"))
	return false;

    if (!parser.next(entry))
	throw XmlException(XmlException::UNCLOSED_TAGS, parser.getLine());

    if (entry.type != XmlEntry::START_TAG && 
	entry.type != XmlEntry::EMPTY_TAG)
    {
	throw XmlValidationError(parser.getLine(), 
	    "Expected one of the following start tags: "
	    "CLASSPATH, LOCALCLASSPATH, CLASSNAME, INSTANCEPATH, "
	    "LOCALINSTANCEPATH, INSTANCENAME");
    }

    if (strcmp(entry.text, "CLASSPATH") == 0)
    {
	parser.putBack(entry);
	getClassPathElement(parser, reference);
    }
    else if (strcmp(entry.text, "LOCALCLASSPATH") == 0)
    {
	parser.putBack(entry);
	getLocalClassPathElement(parser, reference);
    }
    else if (strcmp(entry.text, "CLASSNAME") == 0)
    {
	parser.putBack(entry);
	String className;
	getClassNameElement(parser, className);
	reference.set(String(), String(), className);
    }
    else if (strcmp(entry.text, "INSTANCEPATH") == 0)
    {
	parser.putBack(entry);
	getInstancePathElement(parser, reference);
    }
    else if (strcmp(entry.text, "LOCALINSTANCEPATH") == 0)
    {
	parser.putBack(entry);
	getLocalInstancePathElement(parser, reference);
    }
    else if (strcmp(entry.text, "INSTANCENAME") == 0)
    {
	parser.putBack(entry);
	String className;
	Array<KeyBinding> keyBindings;
	getInstanceNameElement(parser, className, keyBindings);
	reference.set(String(), String(), className, keyBindings);
    }

    expectEndTag(parser, "VALUE.REFERENCE");
    return true;
}

//------------------------------------------------------------------------------
//
// getPropertyReferenceElement()
//
//     <!ELEMENT PROPERTY.REFERENCE (QUALIFIER*,(VALUE.REFERENCE)?)>
//     <!ATTLIST PROPERTY.REFERENCE
//         %CIMName;
//         %ReferenceClass;
//         %ClassOrigin;
//         %Propagated;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getPropertyReferenceElement(
    XmlParser& parser, 
    Property& property)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PROPERTY.REFERENCE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PROPERTY.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.REFERENCECLASS attribute:

    String referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.CLASSORIGIN attribute:

    String classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(parser.getLine(), entry, 
	"PROPERTY.REFERENCE", "PROPAGATED", false, false);

    // Create property:

    Value value;
    value.set(Reference());
    property = Property(
	name, value, 0, referenceClass, classOrigin, propagated);

    if (!empty)
    {
	getQualifierElements(parser, property);

	Reference reference;

	if (getValueReferenceElement(parser, reference))
	    property.setValue(reference);

	expectEndTag(parser, "PROPERTY.REFERENCE");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// GetPropertyElements()
//
//------------------------------------------------------------------------------

template<class CONTAINER>
void GetPropertyElements(XmlParser& parser, CONTAINER& container)
{
    Property property;

    while (XmlReader::getPropertyElement(parser, property) ||
	XmlReader::getPropertyArrayElement(parser, property) ||
	XmlReader::getPropertyReferenceElement(parser, property))
    {
	try
	{
	    container.addProperty(property);
	}
	catch (AlreadyExists&)
	{
	    throw XmlSemanticError(parser.getLine(), "duplicate property");
	}
    }
}

//------------------------------------------------------------------------------
//
// getParameterElement()
//
//     <!ELEMENT PARAMETER (QUALIFIER*)>
//     <!ATTLIST PARAMETER
//         %CIMName;
//         %CIMType; #REQUIRED>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterElement(
    XmlParser& parser, 
    Parameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.NAME attribute:

    String name = getCimNameAttribute(parser.getLine(), entry, "PARAMETER");

    // Get PARAMETER.TYPE attribute:

    Type type = getCimTypeAttribute(parser.getLine(), entry, "PARAMETER");

    // Create parameter:

    parameter = Parameter(name, type);

    if (!empty)
    {
	getQualifierElements(parser, parameter);

	expectEndTag(parser, "PARAMETER");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getParameterArrayElement()
//
//     <!ELEMENT PARAMETER.ARRAY (QUALIFIER*)>
//     <!ATTLIST PARAMETER.ARRAY
//         %CIMName;
//         %CIMType; #REQUIRED
//         %ArraySize;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterArrayElement(
    XmlParser& parser, 
    Parameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER.ARRAY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.ARRAY.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PARAMETER.ARRAY");

    // Get PARAMETER.ARRAY.TYPE attribute:

    Type type = getCimTypeAttribute(parser.getLine(), entry, "PARAMETER.ARRAY");

    // Get PARAMETER.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PARAMETER.ARRAY",arraySize);

    // Create parameter:

    parameter = Parameter(name, type, true, arraySize);

    if (!empty)
    {
	getQualifierElements(parser, parameter);

	expectEndTag(parser, "PARAMETER.ARRAY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getParameterReferenceElement()
//
//     <!ELEMENT PARAMETER.REFERENCE (QUALIFIER*)>
//     <!ATTLIST PARAMETER.REFERENCE
//         %CIMName;
//         %ReferenceClass;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterReferenceElement(
    XmlParser& parser, 
    Parameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER.REFERENCE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PARAMETER.REFERENCE");

    // Get PARAMETER.REFERENCECLASS attribute:

    String referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PARAMETER.REFERENCE");

    // Create parameter:

    parameter = Parameter(name, Type::REFERENCE, false, 0, referenceClass);

    if (!empty)
    {
	getQualifierElements(parser, parameter);
	expectEndTag(parser, "PARAMETER.REFERENCE");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// GetParameterElements()
//
//------------------------------------------------------------------------------

template<class CONTAINER>
void GetParameterElements(XmlParser& parser, CONTAINER& container)
{
    Parameter parameter;

    while (XmlReader::getParameterElement(parser, parameter) ||
	XmlReader::getParameterArrayElement(parser, parameter) ||
	XmlReader::getParameterReferenceElement(parser, parameter))
    {
	try
	{
	    container.addParameter(parameter);
	}
	catch (AlreadyExists&)
	{
	    throw XmlSemanticError(parser.getLine(), "duplicate parameter");
	}
    }
}

//------------------------------------------------------------------------------
//
// getQualifierDeclElement()
//
//     <!ELEMENT QUALIFIER.DECLARATION (SCOPE?,(VALUE|VALUE.ARRAY)?)>
//     <!ATTLIST QUALIFIER.DECLARATION 
//         %CIMName;               
//         %CIMType; #REQUIRED
//         ISARRAY (true|false) #IMPLIED
//         %ArraySize;
//         %QualifierFlavor;>
//         
//------------------------------------------------------------------------------

Boolean XmlReader::getQualifierDeclElement(
    XmlParser& parser, 
    QualifierDecl& qualifierDecl)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "QUALIFIER.DECLARATION"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "QUALIFIER.DECLARATION");

    // Get TYPE attribute:

    Type type = getCimTypeAttribute(
	parser.getLine(), entry, "QUALIFIER.DECLARATION");

    // Get ISARRAY attribute:

    Boolean isArray = false;
    getIsArrayAttribute(
	parser.getLine(), entry, "QUALIFIER.DECLARATION", isArray); 

    // Get ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    Boolean gotArraySize = getArraySizeAttribute(parser.getLine(),
	entry, "QUALIFIER.DECLARATION", arraySize);

    // Get flavor oriented attributes:

    Uint32 flavor = getFlavor(entry,parser.getLine(), "QUALIFIER.DECLARATION");

    // No need to look for interior elements if empty tag:

    Uint32 scope = Scope::NONE;
    Value value;

    if (!empty)
    {
	// Get the option SCOPE element:

	scope = getOptionalScope(parser);

	// Get VALUE or VALUE.ARRAY element:

	if (getValueArrayElement(parser, type, value))
	{
	    if (!isArray)
	    {
		throw XmlSemanticError(parser.getLine(),
		    "VALUE.ARRAY element encountered without "
		    "ISARRAY attribute");
	    }

	    if (arraySize && arraySize != value.getArraySize())
	    {
		throw XmlSemanticError(parser.getLine(),
		    "VALUE.ARRAY size is not the same as "
		    "ARRAYSIZE attribute");
	    }
	}
	else if (getValueElement(parser, type, value))
	{
	    if (isArray)
	    {
		throw XmlSemanticError(parser.getLine(),
		    "ISARRAY attribute used but VALUE element encountered");
	    }
	}

	// Now get the closing tag:

	expectEndTag(parser, "QUALIFIER.DECLARATION");
    }

    if (value.getType() == Type::NONE)
    {
	if (isArray)
	    value.setNullValue(type, true, arraySize);
	else
	    value.setNullValue(type, false);
    }

    QualifierDecl tmp(name, value, scope, flavor, arraySize);
    qualifierDecl = QualifierDecl(name, value, scope, flavor, arraySize);
    return true;
}

//------------------------------------------------------------------------------
// getMethodElement()
//
//     <!ELEMENT METHOD (QUALIFIER*,(PARAMETER|PARAMETER.REFERENCE|
//         PARAMETER.ARRAY|PARAMETER.REFARRAY)*)>
//     <!ATTLIST METHOD
//         %CIMName;
//         %CIMType; #IMPLIED
//         %ClassOrigin;
//         %Propagated;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getMethodElement(XmlParser& parser, Method& method)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "METHOD"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    String name = getCimNameAttribute(parser.getLine(), entry, "PROPERTY");

    Type type = getCimTypeAttribute(parser.getLine(), entry, "PROPERTY");

    String classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY");

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY", "PROPAGATED", false, false);

    method = Method(name, type, classOrigin, propagated);

    if (!empty)
    {
	getQualifierElements(parser, method);

	GetParameterElements(parser, method);

	expectEndTag(parser, "METHOD");
    }

    return true;
}

//------------------------------------------------------------------------------
// getClassElement()
//
//     <!ELEMENT CLASS (QUALIFIER*,
//         (PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*,METHOD*)>
//     <!ATTLIST CLASS %CIMName; %SuperClass;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getClassElement(XmlParser& parser, ClassDecl& classDecl)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "CLASS"))
	return false;

    String name = getCimNameAttribute(parser.getLine(), entry, "CLASS");

    String superClass = getSuperClassAttribute(parser.getLine(), entry,"CLASS");

    classDecl = ClassDecl(name, superClass);

    // Get QUALIFIER elements:

    getQualifierElements(parser, classDecl);

    // Get PROPERTY elements:

    GetPropertyElements(parser, classDecl);

    // Get METHOD elements:

    Method method;

    while (getMethodElement(parser, method))
	classDecl.addMethod(method);	

    // Get CLASS end tag:

    expectEndTag(parser, "CLASS");

    return true;
}

//------------------------------------------------------------------------------
// getInstanceElement()
//
//     <!ELEMENT INSTANCE (QUALIFIER*,
//         (PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*) >
//     <!ATTLIST INSTANCE
//         %ClassName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getInstanceElement(
    XmlParser& parser, 
    InstanceDecl& instanceDecl)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "INSTANCE"))
	return false;

    String className = getClassNameAttribute(
	parser.getLine(), entry, "INSTANCE");

    instanceDecl = InstanceDecl(className);

    // Get QUALIFIER elements:

    getQualifierElements(parser, instanceDecl);

    // Get PROPERTY elements:

    GetPropertyElements(parser, instanceDecl);

    // Get INSTANCE end tag:

    expectEndTag(parser, "INSTANCE");

    return true;
}

//------------------------------------------------------------------------------
//
// getMessageStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getMessageStartTag(
    XmlParser& parser, 
    Uint32& id,
    const char*& protocolVersion)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "MESSAGE"))
	return false;

    // Get MESSAGE.ID:

    if (!entry.getAttributeValue("ID", id))
	throw XmlValidationError(parser.getLine(), 
	    "Bad or missing MESSAGE.ID attribute");

    // Get MESSAGE.PROTOCOLVERSION:

    if (!entry.getAttributeValue("PROTOCOLVERSION", protocolVersion))
	throw XmlValidationError(parser.getLine(),
	    "Bad or missing MESSAGE.ID attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getIMethodCallStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIMethodCallStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "IMETHODCALL"))
	return false;

    // Get IMETHODCALL.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing IMETHODCALL.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getIMethodResponseStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIMethodResponseStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "IMETHODRESPONSE"))
	return false;

    // Get IMETHODRESPONSE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing IMETHODRESPONSE.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getIParamValueTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIParamValueTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "IPARAMVALUE"))
	return false;

    // Get IPARAMVALUE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing IPARAMVALUE.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getBooleanValueElement()
//
//     Get an elements like: "<VALUE>FALSE</VALUE>"
//
//------------------------------------------------------------------------------

Boolean XmlReader::getBooleanValueElement(
    XmlParser& parser, 
    Boolean& result,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE"))
    {
	if (required)
	{
	    throw XmlValidationError(parser.getLine(),
		"Expected VALUE element");
	}
	return false;
    }

    expectContentOrCData(parser, entry);

    if (strcmp(entry.text, "TRUE") == 0)
	result = true;
    else if (strcmp(entry.text, "FALSE") == 0)
	result = false;
    else
	throw XmlSemanticError(parser.getLine(), 
	    "Bad value for VALUE element: must be \"TRUE\" or \"FALSE\"");

    expectEndTag(parser, "VALUE");

    return true;
}

//------------------------------------------------------------------------------
//
// getErrorElement()
//
//     <!ELEMENT ERROR EMPTY>
//     <!ATTLIST ERROR 
//         CODE CDATA #REQUIRED
//         DESCRIPTION CDATA #IMPLIED>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getErrorElement(
    XmlParser& parser, 
    CimException::Code& code,
    const char*& description,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "ERROR"))
    {
	if (required)
	    throw XmlValidationError(parser.getLine(),"Expected ERROR element");
	return false;
    }

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get ERROR.CODE

    Uint32 tmpCode;

    if (!entry.getAttributeValue("CODE", tmpCode))
	throw XmlValidationError(
	    parser.getLine(), "missing ERROR.CODE attribute");

    code = CimException::Code(tmpCode);

    // Get ERROR.DESCRIPTION:

    description = "";
    entry.getAttributeValue("DESCRIPTION", description);

    if (!empty)
	expectEndTag(parser, "ERROR");

    return true;
}

PEGASUS_NAMESPACE_END
