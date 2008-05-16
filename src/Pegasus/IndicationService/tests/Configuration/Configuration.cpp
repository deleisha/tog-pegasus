//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>

#include <Pegasus/IndicationService/IndicationConstants.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void _testPropertyValue(
    CIMInstance& instance,
    const CIMName& propertyName,
    const CIMValue & testValue)
{
    CIMValue value =
        instance.getProperty(instance.findProperty(propertyName)).getValue();
    PEGASUS_TEST_ASSERT(testValue == value);
}


void _testIndicationServiceInstance(CIMClient &client)
{
    // Get CIM_IndicationService instance names
    Array<CIMObjectPath> servicePaths = client.enumerateInstanceNames(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_CIM_INDICATIONSERVICE);
    PEGASUS_TEST_ASSERT(servicePaths.size() == 1);

    // Get CIM_IndicationService instances
    Array<CIMInstance> serviceInstances = client.enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_CIM_INDICATIONSERVICE);
    PEGASUS_TEST_ASSERT(serviceInstances.size() == 1);

    // Get CIM_IndicationService instance
    CIMInstance serinstance = client.getInstance(
        PEGASUS_NAMESPACENAME_INTEROP,
        servicePaths[0]);

    CIMInstance instance = serviceInstances[0];

    _testPropertyValue(
        instance,
        _PROPERTY_FILTERCREATIONENABLED,
        CIMValue(_PROPERTY_FILTERCREATIONENABLED_VALUE));

    _testPropertyValue(
        instance,
        _PROPERTY_SUBSCRIPTIONREMOVALACTION,
        CIMValue(_PROPERTY_SUBSCRIPTIONREMOVALACTION_VALUE));

    _testPropertyValue(
        instance,
        _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVAL,
        CIMValue(_PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVAL_VALUE));

    _testPropertyValue(
        instance,
        _PROPERTY_DELIVERYRETRYATTEMPTS,
        CIMValue(_PROPERTY_DELIVERYRETRYATTEMPTS_VALUE));

    _testPropertyValue(
        instance,
        _PROPERTY_DELIVERYRETRYINTERVAL,
        CIMValue(_PROPERTY_DELIVERYRETRYINTERVAL_VALUE));

    // Test GetInstance() on CIM_IndicationService,
    // with property list set.
    Array<CIMName> props;
    props.append(_PROPERTY_DELIVERYRETRYATTEMPTS);
    props.append(_PROPERTY_DELIVERYRETRYINTERVAL);
    CIMPropertyList propList(props);

    CIMInstance seInstance = client.getInstance(
        PEGASUS_NAMESPACENAME_INTEROP,
        servicePaths[0],
        false,
        true,
        true,
        propList);
    
    _testPropertyValue(
        seInstance,
        _PROPERTY_DELIVERYRETRYATTEMPTS,
        CIMValue(_PROPERTY_DELIVERYRETRYATTEMPTS_VALUE));

    _testPropertyValue(
        seInstance,
        _PROPERTY_DELIVERYRETRYINTERVAL,
        CIMValue(_PROPERTY_DELIVERYRETRYINTERVAL_VALUE));

    cout << "+++++ CIM_IndicationService instance"
                " test completed successfully." << endl;
}

void _testIndicationServiceCapabilitiesInstance(CIMClient &client)
{
    // Get CIM_IndicationServiceCapabilities instances
    Array<CIMInstance> capInstances = client.enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_CIM_INDICATIONSERVICECAPABILITIES);
    PEGASUS_TEST_ASSERT(capInstances.size() == 1);

    // Get CIM_IndicationServiceCapabilities instance names
    Array<CIMObjectPath> capPaths = client.enumerateInstanceNames(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_CIM_INDICATIONSERVICECAPABILITIES);
    PEGASUS_TEST_ASSERT(capPaths.size() == 1);

    // Get CIM_IndicationServiceCapabilities instance
    CIMInstance instance = client.getInstance(
        PEGASUS_NAMESPACENAME_INTEROP,
        capPaths[0]);

    instance = capInstances[0];

    _testPropertyValue(
        instance,
        _PROPERTY_FILTERCREATIONENABLEDISSETTABLE,
        CIMValue(Boolean(false)));

    _testPropertyValue(
        instance,
        _PROPERTY_DELIVERYRETRYATTEMPTSISSETTABLE,
        CIMValue(Boolean(false)));

    _testPropertyValue(
        instance,
        _PROPERTY_DELIVERYRETRYINTERVALISSETTABLE,
        CIMValue(Boolean(false)));

    _testPropertyValue(
        instance,
        _PROPERTY_SUBSCRIPTIONREMOVALACTIONISSETTABLE,
        CIMValue(Boolean(false)));

    _testPropertyValue(
        instance,
        _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVALISSETTABLE,
        CIMValue(Boolean(false)));

    CIMValue value(CIMTYPE_UINT32, false);

    _testPropertyValue(
        instance,
        _PROPERTY_MAXLISTENERDESTINATIONS,
        value);

    _testPropertyValue(
        instance,
        _PROPERTY_MAXACTIVESUBSCRIPTIONS,
        value);

    _testPropertyValue(
        instance,
        _PROPERTY_SUBSCRIPTIONSPERSISTED,
        CIMValue(Boolean(true)));

    // Test GetInstance() on CIM_IndicationServiceCapabilities,
    // with property list set.
    Array<CIMName> props;
    props.append(_PROPERTY_SUBSCRIPTIONREMOVALACTIONISSETTABLE);
    props.append(_PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVALISSETTABLE);
    CIMPropertyList propList(props);

    CIMInstance caInstance = client.getInstance(
        PEGASUS_NAMESPACENAME_INTEROP,
        capPaths[0],
        false,
        true,
        true,
        propList);

    _testPropertyValue(
        caInstance,
        _PROPERTY_SUBSCRIPTIONREMOVALACTIONISSETTABLE,
        CIMValue(Boolean(false)));

    _testPropertyValue(
        caInstance,
        _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVALISSETTABLE,
        CIMValue(Boolean(false)));

    cout << "+++++ CIM_IndicationServiceCapabilities instance"
                " test completed successfully." << endl;
}

int main(int argc, char** argv)
{
    CIMClient client;
    client.connectLocal();

    try
    {
        client.getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_CIM_INDICATIONSERVICE);
    }
    catch(CIMException &e)
    {
        PEGASUS_TEST_ASSERT(e.getCode() == CIM_ERR_NOT_FOUND);
        cout << argv[0]
             << ": Skipping the test, DMTF Indication Profile"
                    "classes can not be found in the repository." << endl;
        return 0;
    }
    catch(...)
    {
        PEGASUS_TEST_ASSERT(0);
    }

    try
    {
        _testIndicationServiceInstance(client);
        _testIndicationServiceCapabilitiesInstance(client);
    }
    catch (Exception& e)
    {
        cerr << e.getMessage() << endl;
        return -1;
    }
    cout << "+++++ Tests completed successfully" << endl;

    return 0;
}
