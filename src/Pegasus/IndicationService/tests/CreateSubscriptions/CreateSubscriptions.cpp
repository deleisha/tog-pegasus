//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/HTTPConnector.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";

void TestCreateSubscriptionInstances(CIMClient& client)
{
    Array<CIMReference> filterRef;
    Array<KeyBinding> filterKeys;

    Array<CIMReference> handlerRef;
    Array<KeyBinding> handlerKeys;

    handlerRef = client.enumerateInstanceNames(NAMESPACE, 
        "CIM_IndicationHandlerCIMXML");

    /*for (Uint8 i = 0; i < handlerRef.size(); i++)
	cout << handlerRef[i]<< endl;*/

    filterRef = client.enumerateInstanceNames(NAMESPACE, 
        "CIM_IndicationFilter");

    /*for (Uint8 i = 0; i < filterRef.size(); i++)
	cout << filterRef[i] << endl;*/

    CIMClass subscriptionClass = client.getClass(
	NAMESPACE, "CIM_IndicationSubscription");

    for (Uint8 i = 0; i < handlerRef.size(); i++)
    {
        CIMInstance subscriptionInstance("CIM_IndicationSubscription");

        subscriptionInstance.addProperty(CIMProperty("Filter", filterRef[i], 
            0, "CIM_IndicationFilter"));
        subscriptionInstance.addProperty(CIMProperty("Handler", handlerRef[i], 
            0, "CIM_IndicationHandlerCIMXML"));
        subscriptionInstance.addProperty (CIMProperty ("SubscriptionState", 
            CIMValue ((Uint16) 3)));

        client.createInstance(NAMESPACE, subscriptionInstance);
    }
}

int main(int argc, char** argv)
{
    Monitor* monitor = new Monitor;
    HTTPConnector* httpConnector = new HTTPConnector(monitor);
    CIMClient client(monitor, httpConnector);
    client.connect("localhost:5988");

    try
    {
        TestCreateSubscriptionInstances (client);
    }
    catch (Exception e)
    {
        PEGASUS_STD (cerr) << "Exception: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        PEGASUS_STD (cerr) << "+++++ create subscription instances failed" 
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ subscription instances created" 
                       << PEGASUS_STD (endl);
    exit (0);
}
