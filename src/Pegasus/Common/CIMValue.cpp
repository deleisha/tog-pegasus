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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Karl Schopmeyer, (k.schopmeyer@opengroup.org)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cstdio>
#include <cassert>
#include <cctype>
#include "CIMValue.h"
#include "Union.h"
#include "Indentor.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMValue
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

template<class T>
inline void _Inc(ArrayRep<T>* rep)
{
    ArrayRep<T>::inc(rep);
}

template<class T>
inline void _Dec(ArrayRep<T>* rep)
{
    ArrayRep<T>::dec(rep);
}

////////////////////////////////////////////////////////////////////////////////
//
// _toString and _toMof routines:
//
////////////////////////////////////////////////////////////////////////////////

inline void _toString(Array<Sint8>& out, Boolean x)
{
    XmlWriter::append(out, x);
}

inline void _toMof(Array<Sint8>& out, Boolean x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Uint8 x) { XmlWriter::append(out, Uint32(x)); }
inline void _toMof(Array<Sint8>& out, Uint8 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Sint8 x) { XmlWriter::append(out, Sint32(x)); }
inline void _toMof(Array<Sint8>& out, Sint8 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Uint16 x) { XmlWriter::append(out, Uint32(x)); }
inline void _toMof(Array<Sint8>& out, Uint16 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Sint16 x) { XmlWriter::append(out, Sint32(x)); }
inline void _toMof(Array<Sint8>& out, Sint16 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Uint32 x) { XmlWriter::append(out, x); }
inline void _toMof(Array<Sint8>& out, Uint32 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Sint32 x) { XmlWriter::append(out, x); }
inline void _toMof(Array<Sint8>& out, Sint32 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Uint64 x) { XmlWriter::append(out, x); }
inline void _toMof(Array<Sint8>& out, Uint64 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Sint64 x) { XmlWriter::append(out, x); }
inline void _toMof(Array<Sint8>& out, Sint64 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Real32 x) { XmlWriter::append(out, Real64(x)); }
inline void _toMof(Array<Sint8>& out, Real32 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Real64 x) { XmlWriter::append(out, x); }
inline void _toMof(Array<Sint8>& out, Real64 x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, Char16 x)
{
    // ATTN: How to convert 16-bit characters to printable form?
    out.append(Sint8(x));
}
inline void _toMof(Array<Sint8>& out, Char16 x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _toString(Array<Sint8>& out, const String& x)
{
    out << x;
}

/** _toMof Internal function to convert the string back
    to MOF format and output it.
    The conversions are:
    \b // \x0008: backspace BS
    \t // \x0009: horizontal tab HT
    \n // \x000A: linefeed LF
    \f // \x000C: form feed FF
    \r // \x000D: carriage return CR
    \" // \x0022: double quote "
    \� // \x0027: single quote '
    \\ // \x005C: backslash \
    \x<hex> // where <hex> is one to four hex digits
    \X<hex> // where <hex> is one to four hex digits
*/
/* ATTN:KS - We need to account for characters greater than x'7f
*/
inline void _toMof(Array<Sint8>& out, const String& x)
{
    out << "\"";
    const Char16* tmp = x.getData();
    char c;
    while ((c = *tmp++))
    {
        switch (c)
        {
        case '\\':
                out.append("\\\\",2);
                break;

            case '\b':
                out.append("\\b",2);
                break;

            case '\t':
                out.append("\\t",2);
                break;

            case '\n':
                out.append("\\n",2);
                break;

            case '\f':
                out.append("\\f",2);
                break;

            case '\r':
                out.append("\\r",2);
                break;

           /* case '\'':
                out.append("\\'", 2);
                break;*/

            case '"':
                out.append("\\\"", 2);
                break;

            default:
                out.append(Sint8(c));
        }

    }
    out << "\"";
}

inline void _toString(Array<Sint8>& out, const CIMDateTime& x)
{
    out << x.getString();
}

inline void _toMof(Array<Sint8>& out, const CIMDateTime& x) { _toString(out, x); }

inline void _toString(Array<Sint8>& out, const CIMReference& x)
{
    out << x.toString();
}
inline void _toMof(Array<Sint8>& out, const CIMReference& x)
{
    x.toMof(out);
}

template<class T>
void _toString(Array<Sint8>& out, const T* p, Uint32 size)
{
    while (size--)
    {
        _toString(out, *p++);
        out << " ";
    }
}

/** _toMof Array -
    arrayInitializer  = "{" constantValue*( "," constantValue)"}"

*/
template<class T>
void _toMof(Array<Sint8>& out, const T* p, Uint32 size)
{
    Boolean isFirstEntry = true;
    // if there are any entries in the array output them
    if (size)
    {
        out << "{";
        while (size--)
        {
            // Put comma on all but first entry.
            if (!isFirstEntry)
            {
                out << ", ";
            }
            isFirstEntry = false;
            _toMof(out, *p++);
        }
        out << "}";

    }
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMValueRep
//
////////////////////////////////////////////////////////////////////////////////

class CIMValueRep
{
public:
    CIMValueRep()
    {
        reset();
    }

    ~CIMValueRep()
    {
    }

    void reset()
    {
        _type = CIMType::NONE;
        _isArray = false;
        _isNull = true;
        _u._voidPtr = 0;
    }

    CIMType _type;
    Boolean _isArray;
    Boolean _isNull;
    Union _u;
};


////////////////////////////////////////////////////////////////////////////////
//
// CIMValue
//
////////////////////////////////////////////////////////////////////////////////

CIMValue::CIMValue()
{
    _rep = new CIMValueRep();
}

CIMValue::CIMValue(CIMType type, Boolean isArray, Uint32 arraySize)
{
    _rep = new CIMValueRep();
    setNullValue(type, isArray, arraySize);
}

CIMValue::CIMValue(Boolean x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Uint8 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Sint8 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Uint16 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Sint16 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Uint32 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Sint32 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Uint64 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Sint64 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Real32 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(Real64 x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Char16& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const String& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const char* x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const CIMDateTime& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const CIMReference& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Boolean>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Uint8>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Sint8>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Uint16>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Sint16>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Uint32>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Sint32>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Uint64>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Sint64>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Real32>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Real64>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<Char16>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<String>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<CIMDateTime>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const Array<CIMReference>& x)
{
    _rep = new CIMValueRep();
    set(x);
}

CIMValue::CIMValue(const CIMValue& x)
{
    _rep = new CIMValueRep();
    assign(x);
}

CIMValue::~CIMValue()
{
    clear();
    delete _rep;
}

CIMValue& CIMValue::operator=(const CIMValue& x)
{
    assign(x);
    return *this;
}

void CIMValue::assign(const CIMValue& x)
{
    if (this == &x)
        return;

    clear();

    _rep->_type = x._rep->_type;
    _rep->_isArray = x._rep->_isArray;
    _rep->_isNull = x._rep->_isNull;
    _rep->_u._voidPtr = 0;

    if (_rep->_isArray)
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
                _Inc(_rep->_u._booleanArray = x._rep->_u._booleanArray);
                break;

            case CIMType::UINT8:
                _Inc(_rep->_u._uint8Array = x._rep->_u._uint8Array);
                break;

            case CIMType::SINT8:
                _Inc(_rep->_u._sint8Array = x._rep->_u._sint8Array);
                break;

            case CIMType::UINT16:
                _Inc(_rep->_u._uint16Array = x._rep->_u._uint16Array);
                break;

            case CIMType::SINT16:
                _Inc(_rep->_u._sint16Array = x._rep->_u._sint16Array);
                break;

            case CIMType::UINT32:
                _Inc(_rep->_u._uint32Array = x._rep->_u._uint32Array);
                break;

            case CIMType::SINT32:
                _Inc(_rep->_u._sint32Array = x._rep->_u._sint32Array);
                break;

            case CIMType::UINT64:
                _Inc(_rep->_u._uint64Array = x._rep->_u._uint64Array);
                break;

            case CIMType::SINT64:
                _Inc(_rep->_u._sint64Array = x._rep->_u._sint64Array);
                break;

            case CIMType::REAL32:
                _Inc(_rep->_u._real32Array = x._rep->_u._real32Array);
                break;

            case CIMType::REAL64:
                _Inc(_rep->_u._real64Array = x._rep->_u._real64Array);
                break;

            case CIMType::CHAR16:
                _Inc(_rep->_u._char16Array = x._rep->_u._char16Array);
                break;

            case CIMType::STRING:
                _Inc(_rep->_u._stringArray = x._rep->_u._stringArray);
                break;

            case CIMType::DATETIME:
                _Inc(_rep->_u._dateTimeArray = x._rep->_u._dateTimeArray);
                break;

            case CIMType::REFERENCE:
                _Inc(_rep->_u._referenceArray = x._rep->_u._referenceArray);
                break;
            default:
                throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (_rep->_type)
        {
            case CIMType::NONE:
                break;

            case CIMType::BOOLEAN:
                _rep->_u._booleanValue = x._rep->_u._booleanValue;
                break;

            case CIMType::UINT8:
                _rep->_u._uint8Value = x._rep->_u._uint8Value;
                break;

            case CIMType::SINT8:
                _rep->_u._sint8Value = x._rep->_u._sint8Value;
                break;

            case CIMType::UINT16:
                _rep->_u._uint16Value = x._rep->_u._uint16Value;
                break;

            case CIMType::SINT16:
                _rep->_u._sint16Value = x._rep->_u._sint16Value;
                break;

            case CIMType::UINT32:
                _rep->_u._uint32Value = x._rep->_u._uint32Value;
                break;

            case CIMType::SINT32:
                _rep->_u._sint32Value = x._rep->_u._sint32Value;
                break;

            case CIMType::UINT64:
                _rep->_u._uint64Value = x._rep->_u._uint64Value;
                break;

            case CIMType::SINT64:
                _rep->_u._sint64Value = x._rep->_u._sint64Value;
                break;

            case CIMType::REAL32:
                _rep->_u._real32Value = x._rep->_u._real32Value;
                break;

            case CIMType::REAL64:
                _rep->_u._real64Value = x._rep->_u._real64Value;
                break;

            case CIMType::CHAR16:
                _rep->_u._char16Value = x._rep->_u._char16Value;
                break;

            case CIMType::STRING:
                _rep->_u._stringValue = new String(*(x._rep->_u._stringValue));
                break;

            case CIMType::DATETIME:
                _rep->_u._dateTimeValue =
                    new CIMDateTime(*(x._rep->_u._dateTimeValue));
                break;

            case CIMType::REFERENCE:
                _rep->_u._referenceValue =
                    new CIMReference(*(x._rep->_u._referenceValue));
                break;
            
            // Should never get here. testing complete enum
            default:
                throw CIMValueInvalidType();
        }
    }
}

//ATTN: P1  KS Problem with Compiler when I added the defaults to clear, the compiler
// gets an exception very early.  Disabled the exceptions to keep compiler running for
// the minute. Note that the case statement is not complete. None missing.
void CIMValue::clear()
{
    if (_rep->_isArray)
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
                _Dec(_rep->_u._booleanArray);
                break;

            case CIMType::UINT8:
                _Dec(_rep->_u._uint8Array);
                break;

            case CIMType::SINT8:
                _Dec(_rep->_u._sint8Array);
                break;

            case CIMType::UINT16:
                _Dec(_rep->_u._uint16Array);
                break;

            case CIMType::SINT16:
                _Dec(_rep->_u._sint16Array);
                break;

            case CIMType::UINT32:
                _Dec(_rep->_u._uint32Array);
                break;

            case CIMType::SINT32:
                _Dec(_rep->_u._sint32Array);
                break;

            case CIMType::UINT64:
                _Dec(_rep->_u._uint64Array);
                break;

            case CIMType::SINT64:
                _Dec(_rep->_u._sint64Array);
                break;

            case CIMType::REAL32:
                _Dec(_rep->_u._real32Array);
                break;

            case CIMType::REAL64:
                _Dec(_rep->_u._real64Array);
                break;

            case CIMType::CHAR16:
                _Dec(_rep->_u._char16Array);
                break;

            case CIMType::STRING:
                _Dec(_rep->_u._stringArray);
                break;

            case CIMType::DATETIME:
                _Dec(_rep->_u._dateTimeArray);
                break;

            case CIMType::REFERENCE:
                _Dec(_rep->_u._referenceArray);
                break;

            //default:
                //throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
            case CIMType::UINT8:
            case CIMType::SINT8:
            case CIMType::UINT16:
            case CIMType::SINT16:
            case CIMType::UINT32:
            case CIMType::SINT32:
            case CIMType::UINT64:
            case CIMType::SINT64:
            case CIMType::REAL32:
            case CIMType::REAL64:
            case CIMType::CHAR16:
                break;

            case CIMType::STRING:
                delete _rep->_u._stringValue;
                break;

            case CIMType::DATETIME:
                delete _rep->_u._dateTimeValue;
                break;

            case CIMType::REFERENCE:
                delete _rep->_u._referenceValue;
                break;
            //default:
                //throw CIMValueInvalidType();
        }
    }

    _rep->reset();
}

Boolean CIMValue::typeCompatible(const CIMValue& x) const
{
    return _rep->_type == x._rep->_type && _rep->_isArray == x._rep->_isArray;
}

Boolean CIMValue::isArray() const
{
    return _rep->_isArray;
}

Boolean CIMValue::isNull() const
{
    return _rep->_isNull;
}

Uint32 CIMValue::getArraySize() const
{
    if (!_rep->_isArray)
        return 0;

    switch (_rep->_type)
    {
        case CIMType::NONE:
            return 0;
            break;

        case CIMType::BOOLEAN:
            return _rep->_u._booleanArray->size;
            break;

        case CIMType::UINT8:
            return _rep->_u._uint8Array->size;
            break;

        case CIMType::SINT8:
            return _rep->_u._sint8Array->size;
            break;

        case CIMType::UINT16:
            return _rep->_u._uint16Array->size;
            break;

        case CIMType::SINT16:
            return _rep->_u._sint16Array->size;
            break;

        case CIMType::UINT32:
            return _rep->_u._uint32Array->size;
            break;

        case CIMType::SINT32:
            return _rep->_u._sint32Array->size;
            break;

        case CIMType::UINT64:
            return _rep->_u._uint64Array->size;
            break;

        case CIMType::SINT64:
            return _rep->_u._sint64Array->size;
            break;

        case CIMType::REAL32:
            return _rep->_u._real32Array->size;
            break;

        case CIMType::REAL64:
            return _rep->_u._real64Array->size;
            break;

        case CIMType::CHAR16:
            return _rep->_u._char16Array->size;
            break;

        case CIMType::STRING:
            return _rep->_u._stringArray->size;
            break;

        case CIMType::DATETIME:
            return _rep->_u._dateTimeArray->size;
            break;

        case CIMType::REFERENCE:
            return _rep->_u._referenceArray->size;
            break;
        // Should never get here. switch on complete enum
        default:
            throw CIMValueInvalidType();
    }

    // Unreachable!
    PEGASUS_ASSERT(false);
    return 0;
}

CIMType CIMValue::getType() const
{
    return CIMType(_rep->_type);
}

void CIMValue::setNullValue(CIMType type, Boolean isArray, Uint32 arraySize)
{

    clear();

    if (isArray)
    {
        switch (type)
        {
            case CIMType::BOOLEAN:
                set(Array<Boolean>(arraySize));
                break;

            case CIMType::UINT8:
                set(Array<Uint8>(arraySize));
                break;

            case CIMType::SINT8:
                set(Array<Sint8>(arraySize));
                break;

            case CIMType::UINT16:
                set(Array<Uint16>(arraySize));
                break;

            case CIMType::SINT16:
                set(Array<Sint16>(arraySize));
                break;

            case CIMType::UINT32:
                set(Array<Uint32>(arraySize));
                break;

            case CIMType::SINT32:
                set(Array<Sint32>(arraySize));
                break;

            case CIMType::UINT64:
                set(Array<Uint64>(arraySize));
                break;

            case CIMType::SINT64:
                set(Array<Sint64>(arraySize));
                break;

            case CIMType::REAL32:
                set(Array<Real32>(arraySize));
                break;

            case CIMType::REAL64:
                set(Array<Real64>(arraySize));
                break;

            case CIMType::CHAR16:
                set(Array<Char16>(arraySize));
                break;

            case CIMType::STRING:
                set(Array<String>(arraySize));
                break;

            case CIMType::DATETIME:
                set(Array<CIMDateTime>(arraySize));
                break;

            case CIMType::REFERENCE:
                set(Array<CIMReference>(arraySize));
                break;
            default:
                throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (type)
        {
            case CIMType::BOOLEAN:
                set(false);
                break;

            case CIMType::UINT8:
                set(Uint8(0));
                break;

            case CIMType::SINT8:
                set(Sint8(0));
                break;

            case CIMType::UINT16:
                set(Uint16(0));
                break;

            case CIMType::SINT16:
                set(Sint16(0));
                break;

            case CIMType::UINT32:
                set(Uint32(0));
                break;

            case CIMType::SINT32:
                set(Sint32(0));
                break;

            case CIMType::UINT64:
                set(Uint64(0));
                break;

            case CIMType::SINT64:
                set(Sint64(0));
                break;

            case CIMType::REAL32:
                set(Real32(0.0));
                break;

            case CIMType::REAL64:
                set(Real64(0.0));
                break;

            case CIMType::CHAR16:
                set(Char16(0));
                break;

            case CIMType::STRING:
                set(String());
                break;

            case CIMType::DATETIME:
                set(CIMDateTime());
                break;

            case CIMType::REFERENCE:
                set(CIMReference());
                break;
            default:
                throw CIMValueInvalidType();
        }
    }

    // Set the Null attribute. Note that this must be after the set
    // because the set functions sets the _isNull.

    _rep->_isNull = true;
}

void CIMValue::set(Boolean x)
{
    clear();
    _rep->_u._booleanValue = (Uint8)x;
    _rep->_type = CIMType::BOOLEAN;
    _rep->_isNull = false;
}

void CIMValue::set(Uint8 x)
{
    clear();
    _rep->_u._uint8Value = x;
    _rep->_type = CIMType::UINT8;
    _rep->_isNull = false;
}

void CIMValue::set(Sint8 x)
{
    clear();
    _rep->_u._sint8Value = x;
    _rep->_type = CIMType::SINT8;
    _rep->_isNull = false;
}

void CIMValue::set(Uint16 x)
{
    clear();
    _rep->_u._uint16Value = x;
    _rep->_type = CIMType::UINT16;
    _rep->_isNull = false;
}

void CIMValue::set(Sint16 x)
{
    clear();
    _rep->_u._sint16Value = x;
    _rep->_type = CIMType::SINT16;
    _rep->_isNull = false;
}

void CIMValue::set(Uint32 x)
{
    clear();
    _rep->_u._uint32Value = x;
    _rep->_type = CIMType::UINT32;
    _rep->_isNull = false;
}

void CIMValue::set(Sint32 x)
{
    clear();
    _rep->_u._sint32Value = x;
    _rep->_type = CIMType::SINT32;
    _rep->_isNull = false;
}

void CIMValue::set(Uint64 x)
{
    clear();
    _rep->_u._uint64Value = x;
    _rep->_type = CIMType::UINT64;
    _rep->_isNull = false;
}

void CIMValue::set(Sint64 x)
{
    clear();
    _rep->_u._sint64Value = x;
    _rep->_type = CIMType::SINT64;
    _rep->_isNull = false;
}

void CIMValue::set(Real32 x)
{
    clear();
    _rep->_u._real32Value = x;
    _rep->_type = CIMType::REAL32;
    _rep->_isNull = false;
}

void CIMValue::set(Real64 x)
{
    clear();
    _rep->_u._real64Value = x;
    _rep->_type = CIMType::REAL64;
    _rep->_isNull = false;
}

void CIMValue::set(const Char16& x)
{
    clear();
    _rep->_u._char16Value = x;
    _rep->_type = CIMType::CHAR16;
    _rep->_isNull = false;
}

void CIMValue::set(const String& x)
{
    clear();
    _rep->_u._stringValue = new String(x);
    _rep->_type = CIMType::STRING;
    _rep->_isNull = false;
}

void CIMValue::set(const char* x)
{
    set(String(x));
    _rep->_isNull = false;
}

void CIMValue::set(const CIMDateTime& x)
{
    clear();
    _rep->_u._dateTimeValue = new CIMDateTime(x);
    _rep->_type = CIMType::DATETIME;
    _rep->_isNull = false;
}

void CIMValue::set(const CIMReference& x)
{
    clear();
    _rep->_u._referenceValue = new CIMReference(x);
    _rep->_type = CIMType::REFERENCE;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Boolean>& x)
{
    clear();
    _Inc(_rep->_u._booleanArray = x._rep);
    _rep->_type = CIMType::BOOLEAN;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Uint8>& x)
{
    clear();
    _Inc(_rep->_u._uint8Array = x._rep);
    _rep->_type = CIMType::UINT8;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Sint8>& x)
{
    clear();
    _Inc(_rep->_u._sint8Array = x._rep);
    _rep->_type = CIMType::SINT8;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Uint16>& x)
{
    clear();
    _Inc(_rep->_u._uint16Array = x._rep);
    _rep->_type = CIMType::UINT16;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Sint16>& x)
{
    clear();
    _Inc(_rep->_u._sint16Array = x._rep);
    _rep->_type = CIMType::SINT16;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Uint32>& x)
{
    clear();
    _Inc(_rep->_u._uint32Array = x._rep);
    _rep->_type = CIMType::UINT32;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Sint32>& x)
{
    clear();
    _Inc(_rep->_u._sint32Array = x._rep);
    _rep->_type = CIMType::SINT32;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Uint64>& x)
{
    clear();
    _Inc(_rep->_u._uint64Array = x._rep);
    _rep->_type = CIMType::UINT64;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Sint64>& x)
{
    clear();
    _Inc(_rep->_u._sint64Array = x._rep);
    _rep->_type = CIMType::SINT64;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Real32>& x)
{
    clear();
    _Inc(_rep->_u._real32Array = x._rep);
    _rep->_type = CIMType::REAL32;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Real64>& x)
{
    clear();
    _Inc(_rep->_u._real64Array = x._rep);
    _rep->_type = CIMType::REAL64;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<Char16>& x)
{
    clear();
    _Inc(_rep->_u._char16Array = x._rep);
    _rep->_type = CIMType::CHAR16;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<String>& x)
{
    clear();
    _Inc(_rep->_u._stringArray = x._rep);
    _rep->_type = CIMType::STRING;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<CIMDateTime>& x)
{
    clear();
    _Inc(_rep->_u._dateTimeArray = x._rep);
    _rep->_type = CIMType::DATETIME;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::set(const Array<CIMReference>& x)
{
    clear();
    _Inc(_rep->_u._referenceArray = x._rep);
    _rep->_type = CIMType::REFERENCE;
    _rep->_isArray = true;
    _rep->_isNull = false;
}

void CIMValue::get(Boolean& x) const
{
    if (_rep->_type != CIMType::BOOLEAN || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._booleanValue != 0;
}

void CIMValue::get(Uint8& x) const
{
    if (_rep->_type != CIMType::UINT8 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._uint8Value;
}

void CIMValue::get(Sint8& x) const
{
    if (_rep->_type != CIMType::SINT8 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._sint8Value;
}

void CIMValue::get(Uint16& x) const
{
    if (_rep->_type != CIMType::UINT16 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._uint16Value;
}

void CIMValue::get(Sint16& x) const
{
    if (_rep->_type != CIMType::SINT16 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._sint16Value;
}

void CIMValue::get(Uint32& x) const
{
    if (_rep->_type != CIMType::UINT32 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._uint32Value;
}

void CIMValue::get(Sint32& x) const
{
    if (_rep->_type != CIMType::SINT32 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._sint32Value;
}

void CIMValue::get(Uint64& x) const
{
    if (_rep->_type != CIMType::UINT64 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._uint64Value;
}

void CIMValue::get(Sint64& x) const
{
    if (_rep->_type != CIMType::SINT64 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._sint64Value;
}

void CIMValue::get(Real32& x) const
{
    if (_rep->_type != CIMType::REAL32 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._real32Value;
}

void CIMValue::get(Real64& x) const
{
    if (_rep->_type != CIMType::REAL64 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._real64Value;
}

void CIMValue::get(Char16& x) const
{
    if (_rep->_type != CIMType::CHAR16 || _rep->_isArray)
        throw TypeMismatch();

    x = _rep->_u._char16Value;
}

void CIMValue::get(String& x) const
{
    if (_rep->_type != CIMType::STRING || _rep->_isArray)
        throw TypeMismatch();

    x = *_rep->_u._stringValue;
}

void CIMValue::get(CIMDateTime& x) const
{
    if (_rep->_type != CIMType::DATETIME || _rep->_isArray)
        throw TypeMismatch();

    x = *_rep->_u._dateTimeValue;
}

void CIMValue::get(CIMReference& x) const
{
    if (_rep->_type != CIMType::REFERENCE || _rep->_isArray)
        throw TypeMismatch();

    x = *_rep->_u._referenceValue;
}

void CIMValue::get(Array<Boolean>& x) const
{
    if (_rep->_type != CIMType::BOOLEAN || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._booleanArray);
}

void CIMValue::get(Array<Uint8>& x) const
{
    if (_rep->_type != CIMType::UINT8 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._uint8Array);
}

void CIMValue::get(Array<Sint8>& x) const
{
    if (_rep->_type != CIMType::SINT8 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._sint8Array);
}

void CIMValue::get(Array<Uint16>& x) const
{
    if (_rep->_type != CIMType::UINT16 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._uint16Array);
}

void CIMValue::get(Array<Sint16>& x) const
{
    if (_rep->_type != CIMType::SINT16 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._sint16Array);
}

void CIMValue::get(Array<Uint32>& x) const
{
    if (_rep->_type != CIMType::UINT32 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._uint32Array);
}

void CIMValue::get(Array<Sint32>& x) const
{
    if (_rep->_type != CIMType::SINT32 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._sint32Array);
}

void CIMValue::get(Array<Uint64>& x) const
{
    if (_rep->_type != CIMType::UINT64 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._uint64Array);
}

void CIMValue::get(Array<Sint64>& x) const
{
    if (_rep->_type != CIMType::SINT64 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._sint64Array);
}

void CIMValue::get(Array<Real32>& x) const
{
    if (_rep->_type != CIMType::REAL32 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._real32Array);
}

void CIMValue::get(Array<Real64>& x) const
{
    if (_rep->_type != CIMType::REAL64 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._real64Array);
}

void CIMValue::get(Array<Char16>& x) const
{
    if (_rep->_type != CIMType::CHAR16 || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._char16Array);
}

void CIMValue::get(Array<String>& x) const
{
    if (_rep->_type != CIMType::STRING || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._stringArray);
}

void CIMValue::get(Array<CIMDateTime>& x) const
{
#ifdef CIMValueisNullexception
    if (_rep->_isNull)
        throw CIMValueIsNull();
#endif

    if (_rep->_type != CIMType::DATETIME || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._dateTimeArray);
}

void CIMValue::get(Array<CIMReference>& x) const
{
    if (_rep->_type != CIMType::REFERENCE || !_rep->_isArray)
        throw TypeMismatch();

    x.set(_rep->_u._referenceArray);
}

CIMValue CIMValue::clone() const
{
    return CIMValue(*this);
}

void CIMValue::toMof(Array<Sint8>& out) const
{
    // if the CIMValue is Null we return nothing.
    // The alternative is to return the Null indicator.
    if (_rep->_isNull)
    {
        out << "null";
        return ;
    }


    if (_rep->_isArray)
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
            {
                _toMof(out, _rep->_u._booleanArray->data(),
                            _rep->_u._booleanArray->size);
                break;
            }
            case CIMType::UINT8:
                _toMof(out, _rep->_u._uint8Array->data(),
                            _rep->_u._uint8Array->size);
                break;

            case CIMType::SINT8:
                _toMof(out, _rep->_u._sint8Array->data(),
                            _rep->_u._sint8Array->size);
                break;

            case CIMType::UINT16:
                _toMof(out, _rep->_u._uint16Array->data(),
                            _rep->_u._uint16Array->size);
                break;

            case CIMType::SINT16:
                _toMof(out, _rep->_u._sint16Array->data(),
                            _rep->_u._sint16Array->size);
                break;

            case CIMType::UINT32:
                _toMof(out, _rep->_u._uint32Array->data(),
                            _rep->_u._uint32Array->size);
                break;

            case CIMType::SINT32:
                _toMof(out, _rep->_u._sint32Array->data(),
                            _rep->_u._sint32Array->size);
                break;

            case CIMType::UINT64:
                _toMof(out, _rep->_u._uint64Array->data(),
                            _rep->_u._uint64Array->size);
                break;

            case CIMType::SINT64:
                _toMof(out, _rep->_u._sint64Array->data(),
                            _rep->_u._sint64Array->size);
                break;

            case CIMType::REAL32:
                _toMof(out, _rep->_u._real32Array->data(),
                            _rep->_u._real32Array->size);
                break;

            case CIMType::REAL64:
                _toMof(out, _rep->_u._real64Array->data(),
                            _rep->_u._real64Array->size);
                break;

            case CIMType::CHAR16:
                _toMof(out, _rep->_u._char16Array->data(),
                            _rep->_u._char16Array->size);
                break;

            case CIMType::STRING:
                _toMof(out, _rep->_u._stringArray->data(),
                            _rep->_u._stringArray->size);
                break;

            case CIMType::DATETIME:
                _toMof(out, _rep->_u._dateTimeArray->data(),
                            _rep->_u._dateTimeArray->size);
                break;

            case CIMType::REFERENCE:
                _toMof(out, _rep->_u._referenceArray->data(),
                            _rep->_u._referenceArray->size);
                break;

            default:
                throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
                _toMof(out, Boolean(_rep->_u._booleanValue != 0));
                break;

            case CIMType::UINT8:
                _toMof(out, _rep->_u._uint8Value);
                break;

            case CIMType::SINT8:
                _toMof(out, _rep->_u._sint8Value);
                break;

            case CIMType::UINT16:
                _toMof(out, _rep->_u._uint16Value);
                break;

            case CIMType::SINT16:
                _toMof(out, _rep->_u._sint16Value);
                break;

            case CIMType::UINT32:
                _toMof(out, _rep->_u._uint32Value);
                break;

            case CIMType::SINT32:
                _toMof(out, _rep->_u._sint32Value);
                break;

            case CIMType::UINT64:
                _toMof(out, _rep->_u._uint64Value);
                break;

            case CIMType::SINT64:
                _toMof(out, _rep->_u._sint64Value);
                break;

            case CIMType::REAL32:
                _toMof(out, _rep->_u._real32Value);
                break;

            case CIMType::REAL64:
                _toMof(out, _rep->_u._real64Value);
                break;

            case CIMType::CHAR16:
                _toMof(out, Char16(_rep->_u._char16Value));
                break;

            case CIMType::STRING:
                _toMof(out, *_rep->_u._stringValue);
                break;

            case CIMType::DATETIME:
                _toMof(out, *_rep->_u._dateTimeValue);
                break;

            case CIMType::REFERENCE:
                _toMof(out, *_rep->_u._referenceValue);
                break;

            default:
                throw CIMValueInvalidType();
        }
    }
}

String CIMValue::toString() const
{
    Array<Sint8> out;

    //ATTN: Not sure what we should do with getstring for Null CIMValues
    //Choice return empty string or exception out.
    if (_rep->_isNull)
        return String();

    if (_rep->_isArray)
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
            {
                Uint32 size = _rep->_u._booleanArray->size;
                for (Uint32 i = 0; i < size; i++)
                {
                    _toString(out, Boolean(_rep->_u._booleanArray->data()[i]));
                    out << " ";
                }
                break;
            }

            case CIMType::UINT8:
                _toString(out, _rep->_u._uint8Array->data(),
                               _rep->_u._uint8Array->size);
                break;

            case CIMType::SINT8:
                _toString(out, _rep->_u._sint8Array->data(),
                               _rep->_u._sint8Array->size);
                break;

            case CIMType::UINT16:
                _toString(out, _rep->_u._uint16Array->data(),
                               _rep->_u._uint16Array->size);
                break;

            case CIMType::SINT16:
                _toString(out, _rep->_u._sint16Array->data(),
                               _rep->_u._sint16Array->size);
                break;

            case CIMType::UINT32:
                _toString(out, _rep->_u._uint32Array->data(),
                               _rep->_u._uint32Array->size);
                break;

            case CIMType::SINT32:
                _toString(out, _rep->_u._sint32Array->data(),
                               _rep->_u._sint32Array->size);
                break;

            case CIMType::UINT64:
                _toString(out, _rep->_u._uint64Array->data(),
                               _rep->_u._uint64Array->size);
                break;

            case CIMType::SINT64:
                _toString(out, _rep->_u._sint64Array->data(),
                               _rep->_u._sint64Array->size);
                break;

            case CIMType::REAL32:
                _toString(out, _rep->_u._real32Array->data(),
                               _rep->_u._real32Array->size);
                break;

            case CIMType::REAL64:
                _toString(out, _rep->_u._real64Array->data(),
                               _rep->_u._real64Array->size);
                break;

            case CIMType::CHAR16:
                _toString(out, _rep->_u._char16Array->data(),
                               _rep->_u._char16Array->size);
                break;

            case CIMType::STRING:
                _toString(out, _rep->_u._stringArray->data(),
                               _rep->_u._stringArray->size);
                break;

            case CIMType::DATETIME:
                _toString(out, _rep->_u._dateTimeArray->data(),
                               _rep->_u._dateTimeArray->size);
                break;

            case CIMType::REFERENCE:
                _toString(out, _rep->_u._referenceArray->data(),
                               _rep->_u._referenceArray->size);
                break;

            default:
                throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (_rep->_type)
        {
            case CIMType::BOOLEAN:
                _toString(out, Boolean(_rep->_u._booleanValue != 0));
                break;

            case CIMType::UINT8:
                _toString(out, _rep->_u._uint8Value);
                break;

            case CIMType::SINT8:
                _toString(out, _rep->_u._sint8Value);
                break;

            case CIMType::UINT16:
                _toString(out, _rep->_u._uint16Value);
                break;

            case CIMType::SINT16:
                _toString(out, _rep->_u._sint16Value);
                break;

            case CIMType::UINT32:
                _toString(out, _rep->_u._uint32Value);
                break;

            case CIMType::SINT32:
                _toString(out, _rep->_u._sint32Value);
                break;

            case CIMType::UINT64:
                _toString(out, _rep->_u._uint64Value);
                break;

            case CIMType::SINT64:
                _toString(out, _rep->_u._sint64Value);
                break;

            case CIMType::REAL32:
                _toString(out, _rep->_u._real32Value);
                break;

            case CIMType::REAL64:
                _toString(out, _rep->_u._real64Value);
                break;

            case CIMType::CHAR16:
                _toString(out, Char16(_rep->_u._char16Value));
                break;

            case CIMType::STRING:
                _toString(out, *_rep->_u._stringValue);
                break;

            case CIMType::DATETIME:
                _toString(out, *_rep->_u._dateTimeValue);
                break;

            case CIMType::REFERENCE:
                _toString(out, *_rep->_u._referenceValue);
                break;

            default:
                throw CIMValueInvalidType();
        }
    }

    out.append('\0');
    return out.getData();
}


Boolean operator==(const CIMValue& x, const CIMValue& y)
{
    if (!x.typeCompatible(y))
        return false;

    if (x._rep->_isNull != y._rep->_isNull)
        return false;

    if (x._rep->_isArray)
    {
        switch (x._rep->_type)
        {
            case CIMType::BOOLEAN:
                return Array<Boolean>(x._rep->_u._booleanArray) ==
                    Array<Boolean>(y._rep->_u._booleanArray);

            case CIMType::UINT8:
                return Array<Uint8>(x._rep->_u._uint8Array) ==
                    Array<Uint8>(y._rep->_u._uint8Array);

            case CIMType::SINT8:
                return Array<Sint8>(x._rep->_u._sint8Array) ==
                    Array<Sint8>(y._rep->_u._sint8Array);

            case CIMType::UINT16:
                return Array<Uint16>(x._rep->_u._uint16Array) ==
                    Array<Uint16>(y._rep->_u._uint16Array);

            case CIMType::SINT16:
                return Array<Sint16>(x._rep->_u._sint16Array) ==
                    Array<Sint16>(y._rep->_u._sint16Array);

            case CIMType::UINT32:
                return Array<Uint32>(x._rep->_u._uint32Array) ==
                    Array<Uint32>(y._rep->_u._uint32Array);

            case CIMType::SINT32:
                return Array<Sint32>(x._rep->_u._sint32Array) ==
                    Array<Sint32>(y._rep->_u._sint32Array);

            case CIMType::UINT64:
                return Array<Uint64>(x._rep->_u._uint64Array) ==
                    Array<Uint64>(y._rep->_u._uint64Array);

            case CIMType::SINT64:
                return Array<Sint64>(x._rep->_u._sint64Array) ==
                    Array<Sint64>(y._rep->_u._sint64Array);

            case CIMType::REAL32:
                return Array<Real32>(x._rep->_u._real32Array) ==
                    Array<Real32>(y._rep->_u._real32Array);

            case CIMType::REAL64:
                return Array<Real64>(x._rep->_u._real64Array) ==
                    Array<Real64>(y._rep->_u._real64Array);

            case CIMType::CHAR16:
                return Array<Char16>(x._rep->_u._char16Array) ==
                    Array<Char16>(y._rep->_u._char16Array);

            case CIMType::STRING:
                return Array<String>(x._rep->_u._stringArray) ==
                    Array<String>(y._rep->_u._stringArray);

            case CIMType::DATETIME:
                return Array<CIMDateTime>(x._rep->_u._dateTimeArray) ==
                    Array<CIMDateTime>(y._rep->_u._dateTimeArray);

            case CIMType::REFERENCE:
                return Array<CIMReference>(x._rep->_u._referenceArray) ==
                    Array<CIMReference>(y._rep->_u._referenceArray);
            default:
                throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (x._rep->_type)
        {
            case CIMType::BOOLEAN:
                return x._rep->_u._booleanValue == y._rep->_u._booleanValue;

            case CIMType::UINT8:
                return x._rep->_u._uint8Value == y._rep->_u._uint8Value;

            case CIMType::SINT8:
                return x._rep->_u._sint8Value == y._rep->_u._sint8Value;

            case CIMType::UINT16:
                return x._rep->_u._uint16Value == y._rep->_u._uint16Value;

            case CIMType::SINT16:
                return x._rep->_u._sint16Value == y._rep->_u._sint16Value;

            case CIMType::UINT32:
                return x._rep->_u._uint32Value == y._rep->_u._uint32Value;

            case CIMType::SINT32:
                return x._rep->_u._sint32Value == y._rep->_u._sint32Value;

            case CIMType::UINT64:
                return x._rep->_u._uint64Value == y._rep->_u._uint64Value;

            case CIMType::SINT64:
                return x._rep->_u._sint64Value == y._rep->_u._sint64Value;

            case CIMType::REAL32:
                return x._rep->_u._real32Value == y._rep->_u._real32Value;

            case CIMType::REAL64:
                return x._rep->_u._real64Value == y._rep->_u._real64Value;

            case CIMType::CHAR16:
                return x._rep->_u._char16Value == y._rep->_u._char16Value;

            case CIMType::STRING:
                return String::equal(*x._rep->_u._stringValue,
                                     *y._rep->_u._stringValue);

            case CIMType::DATETIME:
                return *x._rep->_u._dateTimeValue == *y._rep->_u._dateTimeValue;

            case CIMType::REFERENCE:
                return *x._rep->_u._referenceValue ==
                    *y._rep->_u._referenceValue;

            default:
                throw CIMValueInvalidType();
        }
    }

    // Unreachable!
    return false;
}

Boolean operator!=(const CIMValue& x, const CIMValue& y)
{
    return !operator==(x, y);
}

PEGASUS_NAMESPACE_END
