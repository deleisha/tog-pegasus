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

void TestCreateHandlerInstances(CIMClient& client)
{
    CIMClass handlerClass = client.getClass(
	NAMESPACE, 
        "CIM_IndicationHandlerCIMXML",
	false);
    
    for (Uint8 i = 1; i <= 10; i++)
    {
	CIMInstance handlerInstance("CIM_IndicationHandlerCIMXML");
	handlerInstance.addQualifier(CIMQualifier("Handler", 
                                     "CIMxmlIndicationHandler"));
	handlerInstance.addProperty(CIMProperty("SystemCreationClassName", 
                                     "CIM_UnitaryComputerSystem"));
	handlerInstance.addProperty(CIMProperty("SystemName", 
                                     "bigbasin.hp.com"));
	handlerInstance.addProperty(CIMProperty("CreationClassName", 
                                     "CIM_IndicationHandlerCIMXML"));
	switch (i)
	{
	    case 1:
		handlerInstance.addProperty(CIMProperty("Name", "Handler1"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test1"));
		break;
	    case 2:
		handlerInstance.addProperty(CIMProperty("Name", "Handler2"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test2"));
		break;
	    case 3:
		handlerInstance.addProperty(CIMProperty("Name", "Handler3"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test1"));
		break;
	    case 4:
		handlerInstance.addProperty(CIMProperty("Name", "Handler4"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test2"));
		break;
	    case 5:
		handlerInstance.addProperty(CIMProperty("Name", "Handler5"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test3"));
		break;
	    case 6:
		handlerInstance.addProperty(CIMProperty("Name", "Handler6"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test1"));
		break;
	    case 7:
		handlerInstance.addProperty(CIMProperty("Name", "Handler7"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test1"));
		break;
	    case 8:
		handlerInstance.addProperty(CIMProperty("Name", "Handler8"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test1"));
		break;
	    case 9:
		handlerInstance.addProperty(CIMProperty("Name", "Handler9"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test1"));
		break;
	    case 10:
		handlerInstance.addProperty(CIMProperty("Name", "Handler10"));
	        handlerInstance.addProperty(CIMProperty("Destination", 
                                            "localhost:5988/test4"));
		break;
	}

	client.createInstance(NAMESPACE, handlerInstance);
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
        TestCreateHandlerInstances (client);
    }
    catch (Exception e)
    {
        PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                           << PEGASUS_STD (endl);
        PEGASUS_STD (cerr) << "create handler instances failed" 
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ handler instances created" 
                       << PEGASUS_STD (endl);
    exit (0);
}
