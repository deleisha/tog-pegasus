#ifndef Pegasus_IOPortProvider_h
#define Pegasus_IOPortProvider_h
//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//=============================================================================
//
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#define IOPORTPROVIDERNAME "LinuxIOPortProvider"
#define IOPORTCLASSNAME "Linux_PortResource"
#define IOMEMORYCLASSNAME "Linux_MemoryResource"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMInstance.h>
#include "IOPortInformation.h"

#include <iostream>
#include <vector>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

class LinuxIOPortProvider : public CIMInstanceProvider
{
   public:
      LinuxIOPortProvider();
      ~LinuxIOPortProvider();

      void getInstance(const OperationContext& context,
		       const CIMReference& ref,
		       const Uint32 flags,
		       const Array<String>& propertyList,
		       ResponseHandler<CIMInstance>& handler );

      void enumerateInstances(const OperationContext& context,
			      const CIMReference& ref,
			      const Uint32 flags,
			      const Array<String>& propertyList,
			      ResponseHandler<CIMInstance>& handler );

      void enumerateInstanceNames(const OperationContext& context,
			          const CIMReference& ref,
			          ResponseHandler<CIMReference>& handler );

      void modifyInstance(const OperationContext& context,
		          const CIMReference& ref,
		          const CIMInstance& instanceObject,
		          const Uint32 flags,
		          const Array<String>& propertyList,
		          ResponseHandler<CIMInstance>& handler );

      void createInstance(const OperationContext& context,
		          const CIMReference& ref,
		          const CIMInstance& instanceObject,
		          ResponseHandler<CIMReference>& handler );

      void deleteInstance(const OperationContext& context,
		          const CIMReference& ref,
		          ResponseHandler<CIMInstance>& handler );

      void initialize(CIMOMHandle& handle);
      void terminate(void);

   protected:
      CIMReference fill_reference(const String& nameSpace, 
	    			  const String& className,
			          const IOPortInformation* ptr);

      CIMInstance build_instance(const String& className, 
			         const IOPortInformation* ptr);

};


PEGASUS_NAMESPACE_END

#endif  /* Pegasus_LinuxIOPortProvider_h */
