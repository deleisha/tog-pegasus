//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day IBM Corporation (mdday@us.ibm.com)
//              Adrian Schuur, schuur@de.ibm.com
//              Dan Gorey, IBM djgorey@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPILocalProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderModule.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CMPILocalProviderManager *my_instance = 0;

CMPILocalProviderManager::CMPILocalProviderManager(void)
    :  _idle_timeout(IDLE_LIMIT), _unload_idle_flag(1)
{
    my_instance = this;
}

CMPILocalProviderManager::~CMPILocalProviderManager(void)
{
    Uint32 ccode;

//    _provider_ctrl(UNLOAD_ALL_PROVIDERS, this, &ccode);

}

Sint32 CMPILocalProviderManager::_provider_ctrl(CTRL code, void *parm, void *ret)
{

    static Uint32 quantum;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_provider_ctrl");

    Sint32 ccode = 0;
    CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);

    switch(code)
    {

    case GET_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::GET_PROVIDER");

            String providerName = *(parms->providerName);
            String moduleFileName = *(parms->fileName);
            String interfaceName = *(parms->interfaceName);
            CMPIProvider *pr = 0;
            CMPIProvider::OpProviderHolder* ph = 
	          reinterpret_cast< CMPIProvider::OpProviderHolder* >( ret );

            pr = _lookupProvider(providerName);
            
            if(pr->getStatus() != CMPIProvider::INITIALIZED)
            {
                _initProvider(pr,moduleFileName, interfaceName);
            }

            if(pr->_status != CMPIProvider::INITIALIZED)
            {
                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,"provider initialization failed");
            }


            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Returning Provider" + providerName);

            
            ph->SetProvider( pr );
            ph->GetProvider().update_idle_timer();
            break;
        }

    case UNLOAD_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_PROVIDER");
            CMPIProvider *pr = _lookupProvider(*(parms->providerName));

                if((pr->getStatus() == CMPIProvider::INITIALIZED))
                {
                   

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Unloading CMPIProvider" + pr->_name );

                AutoMutex lock(_providerTableMutex);
                _unloadProvider(pr);
                }
            break;
        }

    case LOOKUP_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::LOOKUP_PROVIDER");

            AutoMutex lock(_providerTableMutex);

            if(true == _providers.lookup(*(parms->providerName),
                *(reinterpret_cast<CMPIProvider * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Found CMPIProvider in cache: " +
                    *(parms->providerName));

                (*(reinterpret_cast<CMPIProvider * *>(ret)))->update_idle_timer();
            }
            else
            {

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Could not find  CMPIProvider in cache: " +
                    *(parms->providerName));
                ccode = -1;
            }

            break;
        }

    case LOOKUP_MODULE:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::LOOKUP_MODULE");

             AutoMutex lock(_providerTableMutex);

            if(false  == _modules.lookup(*(parms->fileName),
                *(reinterpret_cast<CMPIProviderModule * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Could not find  CMPIProvider Module in cache: " +
                    *(parms->fileName));
                ccode = -1;
            }

            break;
        }

    case INSERT_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::INSERT_PROVIDER");

            AutoMutex lock(_providerTableMutex);
             
            if(false  == _providers.insert(
                *(parms->providerName),
                *reinterpret_cast<CMPIProvider * *>(parm)))
                ccode = -1;
            break;
        }
    case INSERT_MODULE:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::INSERT_MODULE");
            AutoMutex lock(_providerTableMutex);
            if(false  == _modules.insert(
                *(parms->fileName),
                *reinterpret_cast<CMPIProviderModule * *>(parm)))
                ccode = -1;
            break;
        }
    
    case UNLOAD_ALL_PROVIDERS:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_ALL_PROVIDERS");
            CMPILocalProviderManager *myself = reinterpret_cast<CMPILocalProviderManager *>(parm);
            CMPIProvider * provider;
            AutoMutex lock(_providerTableMutex);
            Tracer::trace(TRC_PROVIDERMANAGER,Tracer::LEVEL4,
                          "providers in cache = %d", myself->_providers.size());
            ProviderTable::Iterator i = myself->_providers.start();
            try
            {
                for(; i != 0; i++)
                {
                    provider = i.value();
                    PEGASUS_ASSERT(provider!=0);
                    if(provider->getStatus()==CMPIProvider::UNINITIALIZED)
                    {
                        continue;
                    }
                    else
                    {
                        _unloadProvider(provider);
                    }
                }
                    
            }
            catch(...)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Unexpected Exception in UNLOAD_ALL_PROVIDERS.");
            }
            break;
        }

    case UNLOAD_IDLE_PROVIDERS:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_IDLE_PROVIDERS");
            AutoMutex lock(_providerTableMutex);

            quantum++;
            CMPILocalProviderManager *myself = reinterpret_cast<CMPILocalProviderManager *>(parm);
            CMPIProvider * provider;

            if(myself->_providers.size())
            {
                try
                {
                    struct timeval now;
                    gettimeofday(&now, NULL);
                    ProviderTable::Iterator i = myself->_providers.start();
                    for(; i != 0 ; i++)
                    {
                        provider = i.value();
                        PEGASUS_ASSERT(provider != 0);
                        if(provider->getStatus() == CMPIProvider::UNINITIALIZED)
                        {
                            continue;
                        }

                        if(provider->_quantum == quantum)
                        {
                            continue;
                        }

                        provider->_quantum = quantum;

                        if(provider->_current_operations.value())
                        {
                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "CMPIProvider has pending operations: " +
                                provider->getName());
                            
                            continue;
                        }

                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                            "Checking timeout data for CMPIProvider: " +
                            provider->getName());
                        struct timeval timeout = {0,0};
                        provider->get_idle_timer(&timeout);
                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                            " provider->unload_ok() returns: " +
                            provider->unload_ok() ? "true" : "false" );

                       if(provider->unload_ok() == true &&
                            (  now.tv_sec - timeout.tv_sec) > ((Sint32)myself->_idle_timeout))
                        {
                            AutoMutex pr_lock(provider->_statusMutex);
                            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                                "ProviderManager::_provider_crtl -  Unload idle provider $0",
                                provider->getName());

                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "Trying to Terminate CMPIProvider " + provider->getName());
                            try
                            {
                                if(false == provider->tryTerminate())
                                {
                                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                        "CMPIProvider Refused Termination " + provider->getName());

                                    continue;
                                }
                                else
                                {
                                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                        "CMPIProvider terminated: " +  provider->getName());
                                }
                            }
                            catch(...)
                            {
                                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                    "Exception terminating " +
                                    provider->getName());
                              
                                continue;
                            }
                            PEGASUS_ASSERT(provider->_module!=0);
                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "unloading provider module " + provider->getName());
                                provider->_module->unloadModule();

                            
                                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                    "Destroying CMPIProvider's CIMOM Handle: " +
                                    provider->getName());
                                delete provider->_cimom_handle;
                                provider->reset();
                        }
                    }
                }
                catch(...)
                {
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Unexpected Exception in UNLOAD_IDLE_PROVIDERS.");
                }
            } // if there are any providers
            break;
        }

    default:
        ccode = -1;
        break;
    }
    PEG_METHOD_EXIT();
    return(ccode);
}

CMPIProvider * CMPILocalProviderManager::_getResolver
   (const String & fileName, const String & interfaceType)
{
    CMPIProvider *pr;
    String id(interfaceType+String("::")+fileName);
    if (true == _resolvers.lookup(id,pr)) {
       return pr;
    }
    return NULL;
}

CMPIResolverModule *CMPILocalProviderManager::_loadResolver
   (const String & fileName)
{
   CMPIResolverModule *rm=new CMPIResolverModule(fileName);
   rm->load();
   return rm;
}

CMPIProvider::OpProviderHolder CMPILocalProviderManager::getProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{
    CMPIProvider::OpProviderHolder ph;
    CTRL_STRINGS strings;
    Sint32 ccode;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::getProvider");
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    strings.interfaceName = &interfaceName;

    try {
        ccode = _provider_ctrl( GET_PROVIDER, &strings, &ph );
    }
    catch (Exception e) {
       cerr<<"--- loading proxy: "<<e.getMessage()<<endl;
       PEG_METHOD_EXIT();
        throw;
   }
    catch(...) {
        PEG_METHOD_EXIT();
        throw;
    }


    PEG_METHOD_EXIT();
    return(ph);

}

void CMPILocalProviderManager::unloadProvider(
    const String & fileName,
    const String & providerName)
{
    CTRL_STRINGS strings;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::unloadProvider");
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    _provider_ctrl(UNLOAD_PROVIDER, &strings, (void *)0);
    PEG_METHOD_EXIT();
}

void CMPILocalProviderManager::shutdownAllProviders(void)
{

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::shutdownAllProviders");
    _provider_ctrl(UNLOAD_ALL_PROVIDERS, (void *)this, (void *)0);
    PEG_METHOD_EXIT();
}


// << Tue Jul 29 16:51:25 2003 mdd >> change to run every 300 seconds
void CMPILocalProviderManager::unload_idle_providers(void)
{
    static struct timeval first = {0,0}, now, last = {0,0};
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::unload_idle_providers");
   
    if(first.tv_sec == 0)
        gettimeofday(&first, NULL);
    gettimeofday(&now, NULL);
    if(((now.tv_sec - first.tv_sec) > IDLE_LIMIT ) && ( (now.tv_sec - last.tv_sec) > IDLE_LIMIT))
    {
        gettimeofday(&last, NULL);
        if(_unload_idle_flag.value() == 1)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Unload Idle Flag Set: Starting CMPIProvider Monitor Thread");
            _unload_idle_flag = 0;
            MessageQueueService::get_thread_pool()->allocate_and_awaken(this, provider_monitor);

        }
    }
    PEG_METHOD_EXIT();
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL CMPILocalProviderManager::provider_monitor(void *parm)
{
    Thread *th = reinterpret_cast<Thread *>(parm);

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::provider_monitor");
    CMPILocalProviderManager *myself =    my_instance ;

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Checking for Idle providers to unload.");
    try
    {
        myself->_provider_ctrl(UNLOAD_IDLE_PROVIDERS, myself, (void *)0);
        myself->_unload_idle_flag = 1;
    }
    catch(...)
    {
    }
    PEG_METHOD_EXIT();
    return(0);
}

CMPIProvider* CMPILocalProviderManager::_initProvider(
    CMPIProvider * provider,
    const String & moduleFileName,
    const String & interfaceName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_initProvider");

    CMPIProviderModule *module = 0;
    ProviderVector base;

    {
	// lock the providerTable mutex
        AutoMutex lock(_providerTableMutex);

        // lookup provider module
	module = _lookupModule(moduleFileName, interfaceName);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Loading/Linking Provider Module " + moduleFileName);

        // load the provider
        try
        {
            base = module->load(provider->_name);
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                           "Exception caught Loading/Linking Provider Module " +
                           moduleFileName);
            PEG_METHOD_EXIT();
            throw;
        }
    }   // unlock the providerTable mutex

    // initialize the provider
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                     "Initializing Provider " +  provider->_name);

    //
    // Set the undoModuleLoad flag to true here, so that if the
    // initialize() throws an exception, we can unload the provider
    // module.
    //
    Boolean undoModuleLoad = true;

    {   // lock the provider mutex
        AutoMutex pr_lock(provider->_statusMutex);

        // check provider status
        if (provider->_status == CMPIProvider::UNINITIALIZED)
        {
            CIMOMHandle *cimomHandle =  new CIMOMHandle();
            provider->set(module, base, cimomHandle);
            provider->_quantum=0;

            try
            {
                provider->initialize(*(provider->_cimom_handle));
                undoModuleLoad = false;
            }
            catch(...)
            {
                // delete the cimom handle
	        delete provider->_cimom_handle;

	        // set provider status to UNINITIALIZED
                provider->reset();
            }
        }
    }  // unlock the provider mutex

    // if we did not initialize the provider, unload the provider module
    if (undoModuleLoad)
    {
	// lock the providerTable mutex
        AutoMutex lock(_providerTableMutex);

	// unload provider module
        module->unloadModule();
    }

    PEG_METHOD_EXIT();
    return(provider);
}


void CMPILocalProviderManager::_unloadProvider( CMPIProvider * provider)
{
    //
    // NOTE:  It is the caller's responsibility to make sure that
    // the ProviderTable mutex is locked before calling this method.
    //
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_unloadProvider");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                     "Unloading Provider " + provider->_name );

    if ( provider->_current_operations.value())
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Provider cannot be unloaded due to pending operations: " +
                         provider->_name );
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Terminating Provider " + provider->_name );

        // lock the provider mutex
        AutoMutex pr_lock(provider->_statusMutex);

        try
        {
            provider->terminate();
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                             "Error occured terminating CMPI provider " + provider->_name );
        }

        // delete the cimom handle
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Destroying CMPIProvider's CIMOM Handle " + provider->_name );
        delete provider->_cimom_handle;

        PEGASUS_ASSERT(provider->_module != 0);

        // unload provider module
        provider->_module->unloadModule();

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "CMPILocalProviderManager::_provider_crtl -  Unload provider $0",
            provider->getName());

        // set provider status to UNINITIALIZED
	provider->reset();
    }

    PEG_METHOD_EXIT();
}

CMPIProvider * CMPILocalProviderManager::_lookupProvider(
    const String & providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_lookupProvider");

    // lock the providerTable mutex
    AutoMutex lock(_providerTableMutex);

    // look up provider in cache
    CMPIProvider * pr = 0;
    if ( true == _providers.lookup(providerName, pr) )
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Found Provider " + providerName +
			 " in CMPI Provider Manager Cache");
    }
    else
    {
        // create provider
        pr = new CMPIProvider(providerName, 0, 0);

        // insert provider in provider table
        _providers.insert(providerName, pr);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Created provider " + pr->getName());
    }

    PEG_METHOD_EXIT();
    return (pr);
}


CMPIProviderModule * CMPILocalProviderManager::_lookupModule(
    const String & moduleFileName,
    const String & interfaceName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_lookupModule");

    // look up provider module in cache
    CMPIProviderModule * module = 0;

    if ( true == _modules.lookup(moduleFileName, module) )
    {
	// found provider module in cache
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Found Provider Module" + moduleFileName +
                         " in Provider Manager Cache");

   }
   else
   {
	// provider module not found in cache, create provider module
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Creating CMPI Provider Module " + moduleFileName);

        module = new CMPIProviderModule(moduleFileName, interfaceName);

	// insert provider module in module table
        _modules.insert(moduleFileName, module);
    }

    PEG_METHOD_EXIT();
    return (module);
}

PEGASUS_NAMESPACE_END

