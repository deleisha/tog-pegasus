//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////


#include "CIMInternalXmlEncoder.h"

PEGASUS_NAMESPACE_BEGIN

void _putXMLInstance(
    CIMBuffer& out,
    const CIMInstance& ci)
{
    if (ci.isUninitialized())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize instance as XML.
        {
            XmlWriter::appendInstanceElement(buf, ci);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());
            buf.clear();
        }

        // Serialize object path as XML.

        const CIMObjectPath& cop = ci.getPath();

        if (cop.getClassName().isNull())
        {
            out.putUint32(0);
            out.putString(String());
            out.putNamespaceName(CIMNamespaceName());
        }
        else
        {
            XmlWriter::appendValueReferenceElement(buf, cop, true);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());
            out.putString(cop.getHost());
            out.putNamespaceName(cop.getNameSpace());
        }
    }
}

void _putXMLNamedInstance(
    CIMBuffer& out,
    const CIMInstance& ci)
{
    if (ci.isUninitialized())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize instance as XML.
        {
            XmlWriter::appendInstanceElement(buf, ci);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());
            buf.clear();
        }

        // Serialize object path as XML.

        const CIMObjectPath& cop = ci.getPath();

        if (cop.getClassName().isNull())
        {
            out.putUint32(0);
            out.putString(String());
            out.putNamespaceName(CIMNamespaceName());
        }
        else
        {
            XmlWriter::appendInstanceNameElement(buf, cop);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());
            out.putString(cop.getHost());
            out.putNamespaceName(cop.getNameSpace());
        }
    }
}

void _putXMLInstanceName(
    CIMBuffer& out,
    const CIMObjectPath& cop)
{
    Buffer buf(4096);

    // Serialize object path as XML.

    if (cop.getClassName().isNull())
    {
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        XmlWriter::appendValueReferenceElement(buf, cop, true);
        buf.append('\0');

        out.putUint32(buf.size());
        out.putBytes(buf.getData(), buf.size());
        out.putString(cop.getHost());
        out.putNamespaceName(cop.getNameSpace());
    }
}

void _putXMLObject(
    CIMBuffer& out,
    const CIMObject& co)
{
    if (co.isUninitialized())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize instance as XML.
        {
            XmlWriter::appendObjectElement(buf, co);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());
            buf.clear();
        }

        // Serialize object path as XML.

        const CIMObjectPath& cop = co.getPath();

        if (cop.getClassName().isNull())
        {
            out.putUint32(0);
            out.putString(String());
            out.putNamespaceName(CIMNamespaceName());
        }
        else
        {
            XmlWriter::appendValueReferenceElement(buf, cop, true);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());
            out.putString(cop.getHost());
            out.putNamespaceName(cop.getNameSpace());
        }
    }
}

PEGASUS_NAMESPACE_END
