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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Dave Rosckes (rosckes@us.ibm.com)
//		Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//		Adrian Schuur (schuur@de.ibm.com)
//		Seema Gupta (gseema@in.ibm.com) for PEP135
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationResponseHandler_h
#define Pegasus_OperationResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/ContentLanguages.h>  // l10n

#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Common/CIMValue.h>

#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/XmlWriter.h>

#include <Pegasus/ProviderManager2/SimpleResponseHandler.h>

#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_PPM_LINKAGE OperationResponseHandler
{
public:
    OperationResponseHandler(CIMRequestMessage * request, CIMResponseMessage * response)
        : _request(request), _response(response)
    {
    }

    virtual ~OperationResponseHandler(void)
    {
    }

    CIMRequestMessage * getRequest(void) const
    {
        return(_request);
    }

    CIMResponseMessage * getResponse(void) const
    {
        return(_response);
    }

    virtual void setStatus(const Uint32 code, const String & message = String::EMPTY)
    {
        _response->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
    }

    virtual void setStatus(const Uint32 code,
    						const ContentLanguages & langs,
    						const String & message = String::EMPTY)
    {
        _response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
        						langs,
        						CIMStatusCode(code),
        						 message);
    }

protected:
    CIMRequestMessage * _request;
    CIMResponseMessage * _response;

};


/* ------------------------------------------------------------------------- */
/* operation specific response handlers                                      */
/* ------------------------------------------------------------------------- */

class GetInstanceResponseHandler:  public OperationResponseHandler, public SimpleInstanceResponseHandler
{
public:
    GetInstanceResponseHandler(
        CIMGetInstanceRequestMessage * request,
        CIMGetInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        if(getObjects().size() == 0)
        {
            // error? provider claims success,
            // but did not deliver an instance.
            setStatus(CIM_ERR_NOT_FOUND);

            return;
        }

        static_cast<CIMGetInstanceResponseMessage *>(
            getResponse())->cimInstance = getObjects()[0];

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }
};

class EnumerateInstancesResponseHandler : public OperationResponseHandler, public SimpleInstanceResponseHandler
{
public:
    EnumerateInstancesResponseHandler(
        CIMEnumerateInstancesRequestMessage * request,
        CIMEnumerateInstancesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        static_cast<CIMEnumerateInstancesResponseMessage *>(
            getResponse())->cimNamedInstances = getObjects();

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class EnumerateInstanceNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    EnumerateInstanceNamesResponseHandler(
        CIMEnumerateInstanceNamesRequestMessage * request,
        CIMEnumerateInstanceNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        static_cast<CIMEnumerateInstanceNamesResponseMessage *>(
            getResponse())->instanceNames = getObjects();

	// l10n
	getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class CreateInstanceResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    CreateInstanceResponseHandler(
        CIMCreateInstanceRequestMessage * request,
        CIMCreateInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        if(getObjects().size() == 0)
        {
            // ATTN: is it an error to not return instance name?
            return;
        }

        static_cast<CIMCreateInstanceResponseMessage *>(
            getResponse())->instanceName = getObjects()[0];

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class ModifyInstanceResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    ModifyInstanceResponseHandler(
        CIMModifyInstanceRequestMessage * request,
        CIMModifyInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

};

class DeleteInstanceResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    DeleteInstanceResponseHandler(
        CIMDeleteInstanceRequestMessage * request,
        CIMDeleteInstanceResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

};

class GetPropertyResponseHandler : public OperationResponseHandler, public SimpleValueResponseHandler
{
public:
    GetPropertyResponseHandler(
        CIMGetPropertyRequestMessage * request,
        CIMGetPropertyResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        if(getObjects().size() == 0)
        {
            // error? provider claims success,
            // but did not deliver an instance.
            setStatus(CIM_ERR_NOT_FOUND);

            return;
        }

        static_cast<CIMGetPropertyResponseMessage *>(
            getResponse())->value = getObjects()[0];

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }
};

class SetPropertyResponseHandler : public OperationResponseHandler, public SimpleResponseHandler
{
public:
    SetPropertyResponseHandler(
        CIMSetPropertyRequestMessage * request,
        CIMSetPropertyResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

};

class ExecQueryResponseHandler : public OperationResponseHandler, public SimpleInstance2ObjectResponseHandler
{
public:
    ExecQueryResponseHandler(
        CIMExecQueryRequestMessage * request,
        CIMExecQueryResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        static_cast<CIMExecQueryResponseMessage *>(
            getResponse())->cimObjects = getObjects();
  
	// l10n
	getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};


class AssociatorsResponseHandler : public OperationResponseHandler, public SimpleObjectResponseHandler
{
public:
    AssociatorsResponseHandler(
        CIMAssociatorsRequestMessage * request,
        CIMAssociatorsResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                "OperationResponseHandler: complete()");

        static_cast<CIMAssociatorsResponseMessage *>(
            getResponse())->cimObjects = getObjects();

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class AssociatorNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    AssociatorNamesResponseHandler(
        CIMAssociatorNamesRequestMessage * request,
        CIMAssociatorNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        static_cast<CIMAssociatorNamesResponseMessage *>(
            getResponse())->objectNames.appendArray(getObjects());

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class ReferencesResponseHandler : public OperationResponseHandler, public SimpleObjectResponseHandler
{
public:
    ReferencesResponseHandler(
        CIMReferencesRequestMessage * request,
        CIMReferencesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        static_cast<CIMReferencesResponseMessage *>(
            getResponse())->cimObjects = getObjects();

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class ReferenceNamesResponseHandler : public OperationResponseHandler, public SimpleObjectPathResponseHandler
{
public:
    ReferenceNamesResponseHandler(
        CIMReferenceNamesRequestMessage * request,
        CIMReferenceNamesResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: complete()");

        static_cast<CIMReferenceNamesResponseMessage *>(
            getResponse())->objectNames.appendArray(getObjects());

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};

class InvokeMethodResponseHandler : public OperationResponseHandler, public SimpleMethodResultResponseHandler
{
public:
    InvokeMethodResponseHandler(
        CIMInvokeMethodRequestMessage * request,
        CIMInvokeMethodResponseMessage * response)
    : OperationResponseHandler(request, response)
    {
    }

    virtual void complete()
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "OperationResponseHandler: complete()");

        // ATTN-RK-20020903: Is it legal for the return value to be null?
        //if(getReturnValue().isNull())
        //{
            // ATTN: error? provider claims success, but did not deliver a CIMValue.
        //    return;
        //}

        static_cast<CIMInvokeMethodResponseMessage *>(
            getResponse())->outParameters = getParamValues();

        static_cast<CIMInvokeMethodResponseMessage *>(
            getResponse())->retValue = getReturnValue();

		// l10n
		getResponse()->operationContext.set(ContentLanguageListContainer(getLanguages()));
    }

};


typedef void (*PEGASUS_INDICATION_CALLBACK)(
    CIMProcessIndicationRequestMessage*);

class EnableIndicationsResponseHandler : public OperationResponseHandler, public SimpleIndicationResponseHandler
{
public:
    EnableIndicationsResponseHandler(
        CIMEnableIndicationsRequestMessage * request,
        CIMEnableIndicationsResponseMessage * response,
        CIMInstance & provider,
        PEGASUS_INDICATION_CALLBACK indicationCallback)
    : OperationResponseHandler(request, response),
        _request_copy(*request),
        _response_copy(*response),
        _indicationCallback(indicationCallback)
    {
        _provider = provider;
    }

    virtual void deliver(const CIMIndication & cimIndication)
    {
        OperationContext context;

        Array<CIMObjectPath> subscriptionInstanceNames;

        context.insert(SubscriptionInstanceNamesContainer
            (subscriptionInstanceNames));

        deliver(context, cimIndication);
    }

    virtual void deliver(const OperationContext & context, const CIMIndication & cimIndication)
    {
    	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
    		    "OperationResponseHandler: deliver()");
        // ATTN: temporarily convert indication to instance
        CIMInstance cimInstance(cimIndication);

	//
        //  Get list of subscription instance names from context
        //
        Array<CIMObjectPath> subscriptionInstanceNames;
        try
        {
            SubscriptionInstanceNamesContainer container = context.get
                 (SubscriptionInstanceNamesContainer::NAME);

            subscriptionInstanceNames =
                container.getInstanceNames();
        }
        catch (Exception&)
        {
            subscriptionInstanceNames.clear();
        }

// l10n
        ContentLanguages contentLangs;
        try
        {
            // Get the Content-Language for this indication.  The provider
            // does not have to add specify a language for the indication.
            ContentLanguageListContainer langContainer = context.get
                (ContentLanguageListContainer::NAME);

            contentLangs = langContainer.getLanguages();
        }
        catch (Exception&)
        {
            // The provider did not explicitly set a Content-Language for
            // the indication.  Fall back to the lang set in this object.
            contentLangs = getLanguages();
        }
// l10n -end

        // create message
// l10n
        CIMProcessIndicationRequestMessage * request =
            new CIMProcessIndicationRequestMessage(
            XmlWriter::getNextMessageId(),
            cimInstance.getPath().getNameSpace(),
            cimInstance,
	    subscriptionInstanceNames,
            _provider,
            QueueIdStack()); // Must be filled in by the callback function
        request->operationContext = context;
		try
		{
			request->operationContext.set(ContentLanguageListContainer(contentLangs));
		}catch(Exception &e)
		{
			request->operationContext.insert(ContentLanguageListContainer(contentLangs));
		}

        _indicationCallback(request);
    }

    virtual void deliver(const Array<CIMIndication> & cimIndications)
    {
        OperationContext context;

        deliver(context, cimIndications);
    }

    virtual void deliver(const OperationContext & context, const Array<CIMIndication> & cimIndications)
    {
        for(Uint32 i = 0, n = cimIndications.size(); i < n; i++)
        {
            deliver(context, cimIndications[i]);
        }
    }

private:
    CIMEnableIndicationsRequestMessage _request_copy;
    CIMEnableIndicationsResponseMessage _response_copy;
    PEGASUS_INDICATION_CALLBACK _indicationCallback;
};

PEGASUS_NAMESPACE_END

#endif
