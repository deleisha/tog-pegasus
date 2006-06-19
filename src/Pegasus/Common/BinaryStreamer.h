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
// Author: Adrian Schuur (schuur@de.ibm.com) - PEP 164
//
// Modified By: 
//     David Dillard, VERITAS Software Corp.  (david.dillard@veritas.com)
//     Mike Brasher (mike-brasher@austin.rr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef BinaryStreamer_h
#define BinaryStreamer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ObjectStreamer.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>

#define BINREP_MARKER 0x11

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE BinaryStreamer : public ObjectStreamer 
{
public:

    BinaryStreamer() { }

    ~BinaryStreamer() { }

    PEGASUS_HIDDEN_LINKAGE
    void encode(Buffer& out, const CIMClass& x);

    PEGASUS_HIDDEN_LINKAGE
    void encode(Buffer& out, const CIMInstance& x);

    PEGASUS_HIDDEN_LINKAGE
    void encode(Buffer& out, const CIMQualifierDecl& x);

    PEGASUS_HIDDEN_LINKAGE
    void decode(const Buffer& in, unsigned int pos, CIMClass& x);

    PEGASUS_HIDDEN_LINKAGE
    void decode(const Buffer& in, unsigned int pos, CIMInstance& x);

    PEGASUS_HIDDEN_LINKAGE
    void decode(const Buffer& in, unsigned int pos, CIMQualifierDecl& x);

public:

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackQualifier(
	const Buffer& in, Uint32& pos, CIMQualifier& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packQualifier(Buffer& out, const CIMQualifier& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackProperty(
	const Buffer& in, Uint32& pos, CIMProperty& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackMethod(
	const Buffer& in, Uint32& pos, CIMMethod& x);

private:

    PEGASUS_HIDDEN_LINKAGE
    static void _packName(Buffer& out, const CIMName& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackName(const Buffer& in, Uint32& pos, CIMName& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packValue(Buffer& out, const CIMValue& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackValue(const Buffer& in, Uint32& pos, CIMValue& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packProperty(Buffer& out, const CIMProperty& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packParameter(Buffer& out, const CIMParameter& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackParameter(
	const Buffer& in, Uint32& pos, CIMParameter& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packParameters(Buffer& out, CIMMethodRep* rep);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackParameters(
	const Buffer& in, Uint32& pos, CIMMethod& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packMethod(Buffer& out, const CIMMethod& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packObjectPath(Buffer& out, const CIMObjectPath& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackObjectPath(
	const Buffer& in, Uint32& pos, CIMObjectPath& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packProperties(Buffer& out, CIMObjectRep* rep);

    PEGASUS_HIDDEN_LINKAGE
    static void _packMethods(Buffer& out, CIMClassRep* rep);

    PEGASUS_HIDDEN_LINKAGE
    static void _packScope(Buffer& out, const CIMScope& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackScope(
	const Buffer& in, Uint32& pos, CIMScope& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packFlavor(Buffer& out, const CIMFlavor& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackFlavor(
	const Buffer& in, Uint32& pos, CIMFlavor& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _packType(Buffer& out, const CIMType& x);

    PEGASUS_HIDDEN_LINKAGE
    static void _unpackType(
	const Buffer& in, Uint32& pos, CIMType& x);
};

class PEGASUS_COMMON_LINKAGE BinException : public Exception
{
public:
    BinException(const String& message) : Exception(message) { }
};

PEGASUS_NAMESPACE_END

#endif
