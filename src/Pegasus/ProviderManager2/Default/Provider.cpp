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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Dan Gorey, IBM djgorey@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Provider.h"

#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN


// set current operations to 1 to prevent an unload
// until the provider has had a chance to initialize
Provider::Provider(const String & name,
		   ProviderModule *module,
		   CIMProvider *pr)
   : Base(pr), _module(module), _cimom_handle(0), _name(name),
     _no_unload(0), _status(UNINITIALIZED)
{
   _current_operations = 1;
}


Provider::~Provider(void)
{

}

CIMProvider *Provider::getCIMProvider()
{
   return _provider;
}

Provider::Status Provider::getStatus(void)
{
    AutoMutex lock(_statusMutex);
    return(_status);
}

ProviderModule *Provider::getModule(void) const
{
    return(_module);
}

void Provider::set(ProviderModule *module,
                    CIMProvider *cimProvider,
                    CIMOMHandle *cimomHandle)
{
    _module = module;
    _provider = cimProvider;
    _cimom_handle = cimomHandle;
}

void Provider::reset()
{
    _module = 0;
    _cimom_handle = 0;
    _no_unload = 0;
    _status = UNINITIALIZED;
}

String Provider::getName(void) const
{
    return(_name);
}

void Provider::initialize(CIMOMHandle & cimom)
{

    if(_status == UNINITIALIZED)
    {

    try
    {
	// yield before a potentially lengthy operation.
	pegasus_yield();

	ProviderFacade::initialize(cimom);
    }
    catch(...)
    {
  _current_operations = 0;
  _current_ind_operations = 0;
	throw;
    }

    _status = INITIALIZED;
    _current_operations = 0;
    _current_ind_operations = 0;
 }
}

void Provider::terminate(void)
{
  if(_status == INITIALIZED)
  {
// yield before a potentially lengthy operation.
	pegasus_yield();
	try
       {
	ProviderFacade::terminate();
       }
       catch(...)
       {
	  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
			       "Exception caught in ProviderFacade::Terminate for " +
			       _name);
       }
	

	throw;
    }
    
    _status = UNINITIALIZED;

}

Boolean Provider::operator == (const void *key) const
{
   if( (void *)this == key)
      return true;
   return false;
}

Boolean Provider::operator == (const Provider &prov) const
{
   if(String::equalNoCase(_name, prov._name))
      return true;
   return false;
}

void Provider::get_idle_timer(struct timeval *t)
{
   if(t && _cimom_handle)
      _cimom_handle->get_idle_timer(t);
}

void Provider::update_idle_timer(void)
{
   if(_cimom_handle)
      _cimom_handle->update_idle_timer();
}

Boolean Provider::pending_operation(void)
{
   if(_cimom_handle)
      return _cimom_handle->pending_operation();
   return false;
}


Boolean Provider::unload_ok(void)
{
   if(_no_unload.value() )
      return false;

   if(_cimom_handle)
      return _cimom_handle->unload_ok();
   return true;
}

//   force provider manager to keep in memory
void Provider::protect(void)
{
   _no_unload++;
}

// allow provider manager to unload when idle
void Provider::unprotect(void)
{
   _no_unload--;
}

PEGASUS_NAMESPACE_END
