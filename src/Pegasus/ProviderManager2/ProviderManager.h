//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManager_h
#define Pegasus_ProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>

#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager2/ProviderRegistrar.h>

#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PPM_LINKAGE ProviderManager
{
public:
    ProviderManager(void);
    virtual ~ProviderManager(void);

    virtual Message * processMessage(Message * message);

    virtual ProviderName findProvider(const ProviderName & providerName);
    virtual ProviderName findProvider(const String & destinationPath);
    virtual Boolean insertProvider(const ProviderName & providerName);
    virtual Boolean removeProvider(const ProviderName & providerName);
	 virtual void unload_idle_providers()=0;
    static String _resolvePhysicalName(String physicalName);

protected:
    // ingredients for CIMOMHandle (binary message handler, repository, etc.)

private:
    ProviderRegistrar _registrar;

};

PEGASUS_NAMESPACE_END

#endif
