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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Mike Day (mdday@us.ibm.com)s
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <iostream>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/PegasusVersion.h>
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
# include <Pegasus/Common/Signal.h>
#endif

#include <Pegasus/Repository/CIMRepository.h>
#include "ProviderMessageFacade.h"
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/HandlerService/IndicationHandlerService.h>
#include <Pegasus/IndicationService/IndicationService.h>
#include <Pegasus/ProviderManager/ProviderManagerService.h>
#include "CIMServer.h"
#include "CIMOperationRequestDispatcher.h"
#include "CIMOperationResponseEncoder.h"
#include "CIMOperationRequestDecoder.h"
#include "CIMOperationRequestAuthorizer.h"
#include "HTTPAuthenticatorDelegator.h"
#include "ShutdownProvider.h"
#include "ShutdownService.h"
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/ControlProviders/ConfigSettingProvider/ConfigSettingProvider.h>
#include <Pegasus/ControlProviders/UserAuthProvider/UserAuthProvider.h>
#include <Pegasus/ControlProviders/ProviderRegistrationProvider/ProviderRegistrationProvider.h>
#include <Pegasus/ControlProviders/NamespaceProvider/NamespaceProvider.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// Need a static method to act as a callback for the control provider.
// This doesn't belong here, but I don't have a better place to put it.
static Message * controlProviderReceiveMessageCallback(
    Message * message,
    void * instance)
{
    ProviderMessageFacade * mpf =
        reinterpret_cast<ProviderMessageFacade *>(instance);
    return mpf->handleRequestMessage(message);
}

Boolean handleSigHup = false;
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
void sigHupHandler(int s_n, siginfo_t * s_info, void * sig)
{
    PEG_METHOD_ENTER(TRC_SERVER, "sigHupHandler");

    handleSigHup = true;

    PEG_METHOD_EXIT();
}
#endif


CIMServer::CIMServer(Monitor* monitor)
   : _dieNow(false)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::CIMServer()");

    String repositoryRootPath = String::EMPTY;

    // -- Save the monitor or create a new one:
    
    _monitor = monitor;

    repositoryRootPath =
	    ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("repositoryDir"));

    // -- Create a repository:

#ifdef DO_NOT_CREATE_REPOSITORY_ON_STARTUP
    // If this code is enable, the CIMServer will fail to start
    // if the repository directory does not exit. If called,
    // the Repository will create an empty repository.

    // This check has been disabled to allow cimmof to call 
    // the CIMServer to build the initial repository.
    if (!FileSystem::isDirectory(repositoryRootPath))
    {
        PEG_METHOD_EXIT();
	throw NoSuchDirectory(repositoryRootPath);
    }
#endif

    _repository = new CIMRepository(repositoryRootPath);

    // -- Create a UserManager object:

    UserManager* userManager = UserManager::getInstance(_repository);

    // -- Create a CIMServerState object:

    _serverState = new CIMServerState();

    _providerRegistrationManager = new ProviderRegistrationManager(_repository);

    // -- Create queue inter-connections:
    _providerManager = new ProviderManagerService(_providerRegistrationManager);
    _handlerService = new IndicationHandlerService(_repository);

    // Create the control service
    _controlService = new ModuleController(PEGASUS_QUEUENAME_CONTROLSERVICE);

    // Create the Configuration control provider
    ProviderMessageFacade * configProvider =
        new ProviderMessageFacade(new ConfigSettingProvider());
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_CONFIGPROVIDER,
                                      configProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

    // Create the User/Authorization control provider
    ProviderMessageFacade * userAuthProvider =
        new ProviderMessageFacade(new UserAuthProvider(_repository));
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_USERAUTHPROVIDER,
                                      userAuthProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

    // Create the Provider Registration control provider
    ProviderMessageFacade * provRegProvider = new ProviderMessageFacade(
        new ProviderRegistrationProvider(_providerRegistrationManager));
    ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                      PEGASUS_MODULENAME_PROVREGPROVIDER,
                                      provRegProvider,
                                      controlProviderReceiveMessageCallback,
                                      0, 0);

     // Create the Shutdown control provider
     ProviderMessageFacade * shutdownProvider =
         new ProviderMessageFacade(new ShutdownProvider(this));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_SHUTDOWNPROVIDER,
                                       shutdownProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);

     // Create the namespace control provider
     ProviderMessageFacade * namespaceProvider =
         new ProviderMessageFacade(new NamespaceProvider(_repository));
     ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE,
                                       PEGASUS_MODULENAME_NAMESPACEPROVIDER,
                                       namespaceProvider,
                                       controlProviderReceiveMessageCallback,
                                       0, 0);

    _cimOperationRequestDispatcher
	= new CIMOperationRequestDispatcher(_repository,
                                            _providerRegistrationManager);

    
    _cimOperationResponseEncoder
	= new CIMOperationResponseEncoder;

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;
    Boolean enableNamespaceAuthorization = false;

    if (String::equal(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    if (String::equal(
        configManager->getCurrentValue("enableNamespaceAuthorization"), "true"))
    {
        enableNamespaceAuthorization = true;
    }

    //
    // check if authentication and authorization are enabled
    //
    if ( enableAuthentication && enableNamespaceAuthorization )
    {
        //
        // Create Authorization queue only if authorization and
        // authentication are enabled
        //
        _cimOperationRequestAuthorizer = new CIMOperationRequestAuthorizer(
            _cimOperationRequestDispatcher);

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
            _cimOperationRequestAuthorizer,
            _cimOperationResponseEncoder->getQueueId());
    }
    else
    {
        _cimOperationRequestAuthorizer = 0;

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
// to test async cimom, substibute cimom for _cimOperationRequestDispatcher below
            _cimOperationRequestDispatcher,
// substitute the cimom as well for the _cimOperationResponseEncoder below
            _cimOperationResponseEncoder->getQueueId());

    }

    _cimExportRequestDispatcher
	= new CIMExportRequestDispatcher();

    _cimExportResponseEncoder
	= new CIMExportResponseEncoder;

    _cimExportRequestDecoder = new CIMExportRequestDecoder(
	_cimExportRequestDispatcher,
	_cimExportResponseEncoder->getQueueId());

    _httpAuthenticatorDelegator = new HTTPAuthenticatorDelegator(
        _cimOperationRequestDecoder->getQueueId(),
        _cimExportRequestDecoder->getQueueId());

    _sslcontext = 0;

    // IMPORTANT-NU-20020513: Indication service must start after ExportService
    // otherwise HandlerService started by indicationService will never
    // get ExportQueue to export indications for existing subscriptions

    _indicationService = 0;
    if (String::equal(
        configManager->getCurrentValue("enableIndicationService"), "true"))
    {
        _indicationService = new IndicationService
            (_repository, _providerRegistrationManager);
    }

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    // Enable the signal handler to shutdown gracefully on SIGHUP
    getSigHandle()->registerHandler(SIGHUP, sigHupHandler);
    getSigHandle()->activate(SIGHUP);
#endif

    PEG_METHOD_EXIT();
}

CIMServer::~CIMServer()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::~CIMServer()");

    // Note: do not delete the acceptor because it belongs to the Monitor
    // which takes care of disposing of it.

    if (_providerRegistrationManager)
    {
        delete _providerRegistrationManager;
    }

    if (_sslcontext)
	delete _sslcontext;

    PEG_METHOD_EXIT();
}

void CIMServer::addAcceptor(
    Boolean localConnection,
    Uint32 portNumber,
    Boolean useSSL)
{
    HTTPAcceptor* acceptor;
    acceptor = new HTTPAcceptor(_monitor,
                                _httpAuthenticatorDelegator,
                                localConnection,
                                portNumber,
                                useSSL ? _getSSLContext() : 0);

    _acceptors.append(acceptor);
}

void CIMServer::bind()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::bind()");

    if (_acceptors.size() == 0)
    {
        throw BindFailedException("No CIM Server connections are enabled.");
    }

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->bind();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::runForever()
{
   // Note: Trace code in this method will be invoked frequently.

   static int modulator = 0;
   
   if(!_dieNow)
   {
      if(false == _monitor->run(100))
      {
	 modulator++;
	 if( ! (modulator % 1000) )
	    ThreadPool::kill_idle_threads();
      }

      if (handleSigHup)
      {
         Tracer::trace(TRC_SERVER, Tracer::LEVEL3,
            "CIMServer::runForever - HUP signal received.  Shutting down.");

         ShutdownService::getInstance(this)->shutdown(true, 10, false);
         handleSigHup = false;
      }
   }
   
}

void CIMServer::stopClientConnection()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::stopClientConnection()");

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->closeConnectionSocket();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::shutdown()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::shutdown()");

    _dieNow = true;

    PEG_METHOD_EXIT();
}

void CIMServer::resume()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::resume()");

    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        _acceptors[i]->reopenConnectionSocket();
    }

    PEG_METHOD_EXIT();
}

void CIMServer::setState(Uint32 state)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::setState()");

    _serverState->setState(state);

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;
    Boolean enableNamespaceAuthorization = false;

    if (String::equal(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    if (String::equal(
        configManager->getCurrentValue("enableNamespaceAuthorization"), "true"))
    {
        enableNamespaceAuthorization = true;
    }

    if (state == CIMServerState::TERMINATING)
    {
        // tell decoder that CIMServer is terminating
        _cimOperationRequestDecoder->setServerTerminating(true);
        _cimExportRequestDecoder->setServerTerminating(true);

        // tell authorizer that CIMServer is terminating ONLY if
        // authentication and authorization are enabled
        //
        if ( enableAuthentication && enableNamespaceAuthorization )
        {
            _cimOperationRequestAuthorizer->setServerTerminating(true);
        }
    }
    else
    {
        // tell decoder that CIMServer is not terminating
        _cimOperationRequestDecoder->setServerTerminating(false);
        _cimExportRequestDecoder->setServerTerminating(false);

        // tell authorizer that CIMServer is terminating ONLY if
        // authentication and authorization are enabled
        //
        if ( enableAuthentication && enableNamespaceAuthorization )
        {
            _cimOperationRequestAuthorizer->setServerTerminating(false);
        }
    }
    PEG_METHOD_EXIT();
}

Uint32 CIMServer::getOutstandingRequestCount()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMServer::getOutstandingRequestCount()");

    Uint32 requestCount = 0;
    for (Uint32 i=0; i<_acceptors.size(); i++)
    {
        requestCount += _acceptors[i]->getOutstandingRequestCount();
    }

    PEG_METHOD_EXIT();
    return requestCount;
}

SSLContext* CIMServer::_getSSLContext()
{
    static String PROPERTY_NAME__SSLCERT_FILEPATH = "sslCertificateFilePath"; 

    if (_sslcontext == 0)
    {
        //
        // Get the CertificateFilePath property from the Config Manager.
        //
        String certPath;
        certPath = ConfigManager::getInstance()->getCurrentValue(
                               PROPERTY_NAME__SSLCERT_FILEPATH);

        String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
        // NOTE: It is technically not necessary to set up a random file on
        // the server side, but it is easier to use a consistent interface
        // on the client and server than to optimize out the random file on
        // the server side.
        randFile = ConfigManager::getHomedPath(PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

        _sslcontext = new SSLContext(certPath, 0, randFile);
    }

    return _sslcontext;
}

PEGASUS_NAMESPACE_END
