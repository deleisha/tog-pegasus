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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny.yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AssociationProvider_h
#define Pegasus_AssociationProvider_h

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>

PEGASUS_USING_PEGASUS;

class AssociationProvider : 
        public CIMInstanceProvider, public CIMAssociationProvider
{
public:
        AssociationProvider(void);
        virtual ~AssociationProvider(void);

        // CIMProvider interface
        virtual void initialize(CIMOMHandle & cimom);
        virtual void terminate(void);

        // CIMInstanceProvider interfaces
        virtual void getInstance(
                const OperationContext & context,
                const CIMObjectPath & ref,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler);

        virtual void enumerateInstances(
                const OperationContext & context,
                const CIMObjectPath & ref,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler);

        virtual void enumerateInstanceNames(
                const OperationContext & context,
                const CIMObjectPath & ref,
                ObjectPathResponseHandler & handler);

        virtual void modifyInstance(
                const OperationContext & context,
                const CIMObjectPath & ref,
                const CIMInstance & obj,
                const Boolean includeQualifiers,
                const CIMPropertyList & propertyList,
                ResponseHandler & handler);

        virtual void createInstance(
                const OperationContext & context,
                const CIMObjectPath & ref,
                const CIMInstance & obj,
                ObjectPathResponseHandler & handler);

        virtual void deleteInstance(
                const OperationContext & context,
                const CIMObjectPath & ref,
                ResponseHandler & handler);

        // CIMAssociationProvider interface
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
                ObjectResponseHandler & handler);

        virtual void associatorNames(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & associationClass,
                const CIMName & resultClass,
                const String & role,
                const String & resultRole,
                ObjectPathResponseHandler & handler);

        virtual void references(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & resultClass,
                const String & role,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                ObjectResponseHandler & handler);

        virtual void referenceNames(
                const OperationContext & context,
                const CIMObjectPath & objectName,
                const CIMName & resultClass,
                const String & role,
                ObjectPathResponseHandler & handler);

private:

        Array<CIMInstance>   _teacherInstances;
        Array<CIMInstance>   _studentInstances;
        Array<CIMInstance>   _TSassociationInstances;
        Array<CIMInstance>   _ASassociationInstances;

        void _createDefaultInstances();

        CIMInstance _createInstance(
            const CIMName& className, const String& name, Uint8 id);

        CIMInstance _createTSAssociationInstance(
            CIMObjectPath ref1, CIMObjectPath ref2);

        CIMInstance _createASAssociationInstance(
            CIMObjectPath ref1, CIMObjectPath ref2);

        Array<CIMInstance> _filterAssociationInstancesByRole(
            const Array<CIMInstance> & associationInstances,
            const CIMObjectPath & targetObjectPath,
            const String& role);

        Array<CIMObjectPath> _filterAssociationInstances(
            CIMInstance & associationInstance,
            const CIMObjectPath & sourceObjectPath,
            CIMName resultClass,
            String resultRole);

        void _getInstance( 
            const Array<CIMInstance> & instances,
            const CIMObjectPath & localReference,
            InstanceResponseHandler & handler);

        void _enumerateInstances( 
            const Array<CIMInstance> & instances,
            InstanceResponseHandler & handler);

        void _enumerateInstanceNames( 
            const Array<CIMInstance> & instances,
            ObjectPathResponseHandler & handler);

        void _associators(
            const Array<CIMInstance> & associationInstances,
            const CIMObjectPath & localReference,
            const String & role,
            const CIMName & resultClass,
            const String & resultRole,
            ObjectResponseHandler & handler);

        void _associatorNames(
            const Array<CIMInstance> & associationInstances,
            const CIMObjectPath & localReference,
            const String & role,
            const CIMName & resultClass,
            const String & resultRole,
            ObjectPathResponseHandler & handler);
};

#endif
