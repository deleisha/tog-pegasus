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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day IBM Corporation (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*****

<< Fri Oct 11 16:26:12 2002 mdd >>

DO NOT delete the provider handle UnloadProvider() 

// << Fri Oct 11 16:32:11 2002 mdd >>

// CHANGED this algorithm to FIRST terminate each provider THEN remove each provider. 

// The array traversal was not working as expected originally when all of the processing
// was in one loop. providers were having their terminate functions cal

****/


#include "ProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

PEGASUS_NAMESPACE_BEGIN

ProviderManager *my_instance = 0;


ProviderManager::ProviderManager(void)
   : _idle_timeout(300), _unload_idle_flag(1)
{
   my_instance = this;
   
}

ProviderManager::~ProviderManager(void)
{
   Uint32 ccode;
   
   _provider_ctrl(UNLOAD_ALL_PROVIDERS, this, &ccode);

}


Sint32 ProviderManager::_provider_ctrl(CTRL code, void *parm, void *ret)
{

   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_provider_ctrl");
   auto_mutex monitor(&_mut);

   Sint32 ccode = 0;
   CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);
   try 
   {
      
      switch(code)
      {

	 case GET_PROVIDER:
	 {

	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::GET_PROVIDER");
	 
	    Provider *pr;
	 
	    if(true == _providers.lookup(*(parms->providerName), *(reinterpret_cast<Provider * *>(ret))))
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Found Provider " + *(parms->providerName) + " in Provider Manager Cache");
	    
	       (*(reinterpret_cast<Provider * *>(ret)))->update_idle_timer();
	       break;
	    }
	 
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			     "Creating Provider " + *(parms->providerName) );
	 

	    ProviderModule *module;
	    if( false  == _modules.lookup(*(parms->fileName), module) )
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
				"Creating Provider Module " + *(parms->fileName) );
	    
	       module = new ProviderModule(*(parms->fileName));
	       _modules.insert((*parms->fileName), module);
	    }
	    else 
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
				"Using Cached  Provider Module " + *(parms->fileName) );
	    }
	 
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
			     "Loading/Linking Provider Module " + *(parms->fileName) );
	    
	    CIMBaseProvider *base = module->load(*(parms->providerName));
	    

	    // create provider module
   
	    MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
	    PEGASUS_ASSERT(queue != 0);
	    MessageQueueService * service = dynamic_cast<MessageQueueService *>(queue);
	    PEGASUS_ASSERT(service != 0);
	    pr = new Provider(*(parms->providerName), module, base);
	    if(pr == 0)
	       throw NullPointer();
	    
	    if(0 == (pr->_cimom_handle =  new CIMOMHandle()))
	       throw NullPointer();

	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "Loading Provider " +  pr->_name);
	 
	    pr->initialize(*(pr->_cimom_handle));
	    pr->update_idle_timer();
	    _providers.insert(*(parms->providerName), pr);
	    *(reinterpret_cast<Provider * *>(ret)) = pr;
	    break;
	 }
      
	 case UNLOAD_PROVIDER:
	 {

	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::UNLOAD_PROVIDER");
	    CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);
	    Provider *pr;
	    if(true == _providers.lookup(*(parms->providerName), pr ))
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Unloading Provider " + pr->_name );
	       if( pr->_current_operations.value())
	       {
		  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				   "Provider cannot be unloaded due to pending operations: " + 
				   pr->_name );
		  break;
	       }
	       _providers.remove(pr->_name);
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Terminating Provider " + pr->_name );
	       pr->terminate();
	       if((pr->_module != 0 ) && pr->_module->_ref_count.value() == 0)
	       {
		  pr->_module->unloadModule();
		  _modules.remove(pr->_module->_fileName);
		  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				   "Destroying Provider " + pr->_name );
		  delete pr->_module;
	       }
	    
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Destroying Provider's CIMOM Handle " + pr->_name );
	       delete pr->_cimom_handle;
	       delete pr;
	    }
	    else 
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
				"Unable to find Provider in cache: " + 
				*(parms->providerName));
	    }
	 }

	 case LOOKUP_PROVIDER:
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::LOOKUP_PROVIDER");
	 
	    if(true == _providers.lookup(*(parms->providerName), 
					 *(reinterpret_cast<Provider * *>(ret))))
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Found Provider in cache: " + 
				*(parms->providerName));
	    
	       (*(reinterpret_cast<Provider * *>(ret)))->update_idle_timer();
	    }
	    else
	    {
	    
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Could not find  Provider in cache: " + 
				*(parms->providerName));
	       ccode = -1;
	    }
			  
	    break;
	 }
      
	 case LOOKUP_MODULE:
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::LOOKUP_MODULE");

	    if(false  == _modules.lookup(*(parms->fileName), 
					 *(reinterpret_cast<ProviderModule * *>(ret))))
	    {
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Could not find  Provider Module in cache: " + 
				*(parms->fileName));
	       ccode = -1;
	    }
	 
	    break;
	 }

	 case INSERT_PROVIDER: 
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::INSERT_PROVIDER");
	    if( false  == _providers.insert(
		   *(parms->providerName), 
		   *reinterpret_cast<Provider * *>(parm)) )
	       ccode = -1;
	    break;
	 }
	 case INSERT_MODULE: 
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::INSERT_MODULE");
	    if( false  == _modules.insert(
		   *(parms->fileName), 
		   *reinterpret_cast<ProviderModule * *>(parm)) )
	       ccode = -1;
	    break;
	 }
	 case REMOVE_PROVIDER:
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::REMOVE_PROVIDER");
	    if(false == _providers.remove(*(parms->providerName)))
	       ccode = -1;
	    break;
	 }
	 case REMOVE_MODULE:
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::REMOVE_MODULE");
	    if(false == _modules.remove(*(parms->fileName)))
	       ccode = -1;
	    break;
	 }

	 case UNLOAD_ALL_PROVIDERS:
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::UNLOAD_ALL_PROVIDERS");
	    ProviderManager *myself = reinterpret_cast<ProviderManager *>(parm);
	    Provider * provider;
	    ProviderTable::Iterator i = myself->_providers.start();
	    try 
	    {
	    
	       for(; i ; i++)
	       {
		  provider = i.value();
		  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				   "Terminating Provider " + provider->getName());
		  try 
		  {
		     provider->terminate();
		  }
		  catch(ObjectBusyException &)
		  {
		     PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				      "Provider Busy, cannot terminate" + 
				      provider->getName());
		     continue;
		  }
		  catch(...)
		  {
		     PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				      "Exception terminating " + 
				      provider->getName());
		     continue;
		  }
		  if((provider->_module != 0 ) &&  
		     provider->_module->_ref_count.value() == 0)
		  {

		     if(true == _modules.lookup(provider->_module->_fileName, provider->_module))
		     {
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Removing Provider's Module " + provider->getName());
			_modules.remove(provider->_module->_fileName);
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Destroying Provider's Module " + provider->getName());
			delete provider->_module;
		     }
		  }
		  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				   "Destroying Provider's CIMOM Handle: " + provider->getName());
		  delete provider->_cimom_handle;
		  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				   "Destroying Provider: " + provider->getName());
		  delete provider;
	       }
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Clearing Provider Cache" );
	       myself->_providers.clear();
	       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				"Clearing Module Cache");
	       myself->_modules.clear();
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
	    ProviderManager *myself = reinterpret_cast<ProviderManager *>(parm);
	    Provider * provider;
	    if(myself->_providers.size())
	    {
	       try
	       {
	       
		  struct timeval now;
		  gettimeofday(&now, NULL);
		  ProviderTable::Iterator i = myself->_providers.start();
		  for(; i ; i++)
		  {
		     provider = i.value();
		     if( provider->_current_operations.value() ) 
		     {
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Provider has pending operations: " + 
					 provider->getName());
			continue;
		     }

		     PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
				      "Checking timeout data for Provider: " + 
				      provider->getName());
		     struct timeval timeout = {0,0};
		     provider->get_idle_timer(&timeout);
		     if(( now.tv_sec - timeout.tv_sec) > (Sint32)myself->_idle_timeout )
		     {

			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Terminating Provider " + provider->getName());
			try
			{
			   provider->terminate();
			}
			catch(ObjectBusyException &)
			{
			   PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					    "Provider Busy, cannot terminate" + 
					    provider->getName());
			   continue;
			}
			catch(...)
			{
			   PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					    "Exception terminating " + 
					    provider->getName());
			   continue;
			}
		  
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Removing Provider " + provider->getName());
			myself->_providers.remove(provider->_name);
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 provider->_name + " Removed, looking at Module" );
			if(provider->_module != 0 )
			{
			   PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					    "Module ref: " + provider->_module->_ref_count.value() );
			   if( provider->_module->_ref_count.value() == 0)
			   {
			      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					       "Removing Module " + provider->_module->_fileName);
			      _modules.remove(provider->_module->_fileName);
			      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					       "Destroying Module " + provider->_module->_fileName);
			      delete provider->_module;
			   }
			}
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Destroying Provider's CIMOM Handle: " + 
					 provider->getName());
			delete provider->_cimom_handle;
			PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
					 "Destroying Provider: " + 
					 provider->getName());
			delete provider;
			i = myself->_providers.start();
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

	 case UNLOAD_IDLE_MODULES:
	 {
	    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			     "_provider_ctrl::UNLOAD_IDLE_MODULES");
	 
	    ProviderManager *myself = reinterpret_cast<ProviderManager *>(parm);
	    ProviderModule *module;
	    ModuleTable::Iterator i = myself->_modules.start();
	    for( ; i ; i++)
	    {
	       module = i.value();
	       if( module->_ref_count.value() == 0)
	       {
		  myself->_modules.remove(module->_fileName);
		  module->unloadModule();
		  delete module;
		  i = myself->_modules.start();
	       }
	    }
	    break;
	 }
      
	 default:
	    ccode = -1;
	    break;
      }
   }
   catch(...)
   {
      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
		       "_provider_ctrl::Unexpected Exception");
      throw;
      
   }
   PEG_METHOD_EXIT();
   return ccode;
}

Provider ProviderManager::getProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName) throw()
{

   Provider *provider = 0;
   CTRL_STRINGS strings;
   Sint32 ccode;
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::getProvider");
   strings.fileName = &fileName;
   strings.providerName = &providerName;
   strings.interfaceName = &interfaceName;
   try
   {
      ccode = _provider_ctrl(GET_PROVIDER, &strings, &provider);
   }
   catch(...)
   {
      PEG_METHOD_EXIT();
      throw;
   }
   
   
   PEG_METHOD_EXIT();
   return *provider;
   
}

void ProviderManager::unloadProvider(
    const String & fileName,
    const String & providerName) throw() 
{
   CTRL_STRINGS strings;
      PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::unloadProvider");
   strings.fileName = &fileName;
   strings.providerName = &providerName;
   _provider_ctrl(UNLOAD_PROVIDER, &strings, (void *)0);
   PEG_METHOD_EXIT();
}

void ProviderManager::shutdownAllProviders(void) throw()
{
 
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::shutdownAllProviders");
   _provider_ctrl(UNLOAD_ALL_PROVIDERS, (void *)this, (void *)0);
   PEG_METHOD_EXIT();
}

void ProviderManager::unload_idle_providers(void) throw()
{
   static struct timeval first = {0,0}, now, last = {0,0};
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::unload_idle_providers");
   if(first.tv_sec == 0)
      gettimeofday(&first, NULL);
   gettimeofday(&now, NULL);
   if(((now.tv_sec - first.tv_sec) > 60 ) && ( (now.tv_sec - last.tv_sec) > 60)) 
   {
      gettimeofday(&last, NULL);
      if(_unload_idle_flag.value() == 1 )
      { 
	 PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2, 
			  "Unload Idle Flag Set: Starting Provider Monitor Thread");
	 _unload_idle_flag = 0;
	 Thread th(provider_monitor, this , true);
	 th.run();
      }
   }
   PEG_METHOD_EXIT();
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManager::provider_monitor(void *parm)
{
   Thread *th = reinterpret_cast<Thread *>(parm);
   
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::provider_monitor");
   ProviderManager *myself =    my_instance ;
      
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
   exit_thread((PEGASUS_THREAD_RETURN)1);
   return 0;
}

PEGASUS_NAMESPACE_END
