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
// Author: Paulo F. Borges (pfborges@wowmail.com)
//        
//
// Modified By: Jair Francisco T. dos Santos (t.dos.santos.francisco@non.hp.com)
//==============================================================================
// This template was created from DNSAdminDomainProvider.h
//%/////////////////////////////////////////////////////////////////////////
#ifndef _NTPPROVIDER_H
#define _NTPPROVIDER_H

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//Pegasus includes
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>    
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>

//------------------------------------------------------------------------------
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//------------------------------------------------------------------------------
// Class [NTPAdminDomainProvider] Definition
//------------------------------------------------------------------------------
class NTPAdminDomainProvider: public CIMInstanceProvider,
                            public CIMMethodProvider
{
    public:
        //Constructor/Destructor
        NTPAdminDomainProvider(void);
        ~NTPAdminDomainProvider(void);

    public:

          //-- CIMInstanceProvider methods
          /** Given a reference to an instance of the CIM class, fills in the data
           *  elements of the class with the details gleaned from the system. */
        void getInstance(const OperationContext & context,
                         const CIMObjectPath & ref,
                         const Boolean includeQualifiers,
                         const Boolean includeClassOrigin,
                         const CIMPropertyList & propertyList,
                         InstanceResponseHandler & handler);

          /** Returns filled instances for all instances of the CIM class detected
           *  on the system. */
        void enumerateInstances(const OperationContext & context,
                                const CIMObjectPath & ref,
                                const Boolean includeQualifiers,
                                const Boolean includeClassOrigin,
                                const CIMPropertyList & propertyList,
                                InstanceResponseHandler & handler);

          /** Produces a list of references to all instances of the CIM class
           *  detected on the system, but does not fill the instances
           *  themselves. */
        void enumerateInstanceNames(const OperationContext & context,
                                    const CIMObjectPath & ref,
                                    ObjectPathResponseHandler & handler);

          /** Currently unimplemented in the NTPAdminDomain provider, 
        *  this is a no-op here. */
        void modifyInstance(const OperationContext & context,
                            const CIMObjectPath & ref,
                            const CIMInstance & obj,
                            const Boolean includeQualifiers,
                            const CIMPropertyList & propertyList,
                            ResponseHandler & handler);

          /** Currently unimplemented in the NTPAdminDomain provider, 
        *  this is a no-op here. */
        void createInstance(const OperationContext & context,
                            const CIMObjectPath & ref,
                            const CIMInstance & obj,
                            ObjectPathResponseHandler & handler);

          /** Currently unimplemented in the NTPAdminDomain provider, 
        *  this is a no-op here. */
        void deleteInstance(const OperationContext & context,
                            const CIMObjectPath & ref,
                            ResponseHandler & handler);

        /** Currently unimplemented in the NTPAdminDomain provider, 
        *  this is a no-op here. */
          void invokeMethod(const OperationContext& context,
                          const CIMObjectPath& objectReference,
                          const CIMName& methodName,
                          const Array<CIMParamValue>& inParameters,
                          MethodResultResponseHandler& handler);

        /** Currently unimplemented in the NTPAdminDomain provider, 
        *  this is a no-op here. */
          void initialize(CIMOMHandle & cimom);

        /** frees the object itself loaded */
          void terminate(void);

    private:
          // Builds a reference (a set of Key,Value pairs)
        CIMObjectPath _fill_reference(const String &nameSpace,
                                       const String &className);
          // Builds a filled-in instance.
        CIMInstance    _build_instance(const String & classname,
                                    const String & nameSpace,
                                    const Array<KeyBinding> keys);
};
#endif
