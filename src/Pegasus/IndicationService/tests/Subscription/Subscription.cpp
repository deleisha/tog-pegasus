//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName SOURCENAMESPACE = 
    CIMNamespaceName ("root/SampleProvider");

Boolean verbose;

void _createModuleInstance 
    (CIMClient & client, 
     const String & name,
     const String & location)
{
    CIMInstance moduleInstance (PEGASUS_CLASSNAME_PROVIDERMODULE);
    moduleInstance.addProperty (CIMProperty (CIMName ("Name"), name));
    moduleInstance.addProperty (CIMProperty (CIMName ("Vendor"), 
        String ("Hewlett-Packard Company")));
    moduleInstance.addProperty (CIMProperty (CIMName ("Version"), 
        String ("2.0")));
    moduleInstance.addProperty (CIMProperty (CIMName ("InterfaceType"),
        String ("C++Default")));
    moduleInstance.addProperty (CIMProperty (CIMName ("InterfaceVersion"), 
        String ("2.2.0")));
    moduleInstance.addProperty (CIMProperty (CIMName ("Location"), location));

    CIMObjectPath path = client.createInstance (NAMESPACE, moduleInstance);
}

void _createProviderInstance 
    (CIMClient & client, 
     const String & name,
     const String & providerModuleName)
{
    CIMInstance providerInstance (PEGASUS_CLASSNAME_PROVIDER);
    providerInstance.addProperty (CIMProperty (CIMName ("Name"), name));
    providerInstance.addProperty (CIMProperty (CIMName ("ProviderModuleName"), 
        providerModuleName));

    CIMObjectPath path = client.createInstance (NAMESPACE, providerInstance);
}

void _createCapabilityInstance 
    (CIMClient & client, 
     const String & providerModuleName,
     const String & providerName,
     const String & capabilityID,
     const String & className,
     const Array <String> & namespaces,
     const Array <Uint16> & providerTypes,
     const CIMPropertyList & supportedProperties)
{
    CIMInstance capabilityInstance (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES);
    capabilityInstance.addProperty (CIMProperty (CIMName ("ProviderModuleName"),
        providerModuleName));
    capabilityInstance.addProperty (CIMProperty (CIMName ("ProviderName"),
        providerName));
    capabilityInstance.addProperty (CIMProperty (CIMName ("CapabilityID"),
        capabilityID));
    capabilityInstance.addProperty (CIMProperty (CIMName ("ClassName"), 
        className));
    capabilityInstance.addProperty (CIMProperty (CIMName ("Namespaces"), 
        namespaces));
    capabilityInstance.addProperty (CIMProperty (CIMName ("ProviderType"),
        providerTypes));
    if (!supportedProperties.isNull ())
    {
        Array <String> propertyNameStrings;
        for (Uint32 i = 0; i < supportedProperties.size (); i++)
        {
            propertyNameStrings.append (supportedProperties [i].getString ());
        }
        capabilityInstance.addProperty (CIMProperty 
            (CIMName ("supportedProperties"), propertyNameStrings));
    }

    CIMObjectPath path = client.createInstance (NAMESPACE, capabilityInstance);
}

void _modifyCapabilityInstance 
    (CIMClient & client, 
     const String & providerModuleName,
     const String & providerName,
     const String & capabilityID,
     const CIMPropertyList & supportedProperties)
{
    CIMInstance capabilityInstance (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES);
    if (!supportedProperties.isNull ())
    {
        Array <String> propertyNameStrings;
        for (Uint32 i = 0; i < supportedProperties.size (); i++)
        {
            propertyNameStrings.append (supportedProperties [i].getString ());
        }
        capabilityInstance.addProperty (CIMProperty 
            (CIMName ("SupportedProperties"), propertyNameStrings));
    }

    CIMObjectPath path;
    Array <CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("ProviderModuleName", providerModuleName,
        CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("ProviderName", providerName, 
        CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("CapabilityID", capabilityID, 
        CIMKeyBinding::STRING));
    path.setClassName (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES);
    path.setKeyBindings (keyBindings);
    capabilityInstance.setPath (path);

    Array <CIMName> propertyNames;
    propertyNames.append (CIMName ("SupportedProperties"));
    CIMPropertyList properties (propertyNames);
    client.modifyInstance (NAMESPACE, capabilityInstance, false, properties);
}

void _addStringProperty
    (CIMInstance & instance,
     const String & name,
     const String & value,
     Boolean null = false,
     Boolean isArray = false)
{
    if (null)
    {
        instance.addProperty (CIMProperty (CIMName (name), 
            CIMValue (CIMTYPE_STRING, false)));
    }
    else
    {
        if (isArray)
        {
            Array <String> values;
            values.append (value);
            instance.addProperty (CIMProperty (CIMName (name), values));
        }
        else
        {
            instance.addProperty (CIMProperty (CIMName (name), value));
        }
    }
}

void _addUint16Property
    (CIMInstance & instance,
     const String & name,
     Uint16 value,
     Boolean null = false,
     Boolean isArray = false)
{
    if (null)
    {
        instance.addProperty (CIMProperty (CIMName (name), 
            CIMValue (CIMTYPE_UINT16, false)));
    }
    else
    {
        if (isArray)
        {
            Array <Uint16> values;
            values.append (value);
            instance.addProperty (CIMProperty (CIMName (name), values));
        }
        else
        {
            instance.addProperty (CIMProperty (CIMName (name), value));
        }
    }
}

void _addUint32Property
    (CIMInstance & instance,
     const String & name,
     Uint32 value,
     Boolean null = false,
     Boolean isArray = false)
{
    if (null)
    {
        instance.addProperty (CIMProperty (CIMName (name), 
            CIMValue (CIMTYPE_UINT32, false)));
    }
    else
    {
        if (isArray)
        {
            Array <Uint32> values;
            values.append (value);
            instance.addProperty (CIMProperty (CIMName (name), values));
        }
        else
        {
            instance.addProperty (CIMProperty (CIMName (name), value));
        }
    }
}

void _addUint64Property
    (CIMInstance & instance,
     const String & name,
     Uint64 value,
     Boolean null = false,
     Boolean isArray = false)
{
    if (null)
    {
        instance.addProperty (CIMProperty (CIMName (name), 
            CIMValue (CIMTYPE_UINT64, false)));
    }
    else
    {
        if (isArray)
        {
            Array <Uint64> values;
            values.append (value);
            instance.addProperty (CIMProperty (CIMName (name), values));
        }
        else
        {
            instance.addProperty (CIMProperty (CIMName (name), value));
        }
    }
}

CIMObjectPath _buildFilterOrHandlerPath
    (const CIMName & className,
     const String & name)
{
    CIMObjectPath path;

    Array <CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        System::getSystemCreationClassName (), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("SystemName",
        System::getFullyQualifiedHostName (), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("CreationClassName",
        className.getString(), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("Name", name, CIMKeyBinding::STRING));
    path.setClassName (className);
    path.setKeyBindings (keyBindings);

    return path;
}

CIMObjectPath _buildSubscriptionPath 
    (const String & filterName,
     const CIMName & handlerClass,
     const String & handlerName)
{
    CIMObjectPath filterPath = _buildFilterOrHandlerPath 
        (PEGASUS_CLASSNAME_INDFILTER, filterName);

    CIMObjectPath handlerPath = _buildFilterOrHandlerPath (handlerClass, 
        handlerName);

    Array<CIMKeyBinding> subscriptionKeyBindings;
    subscriptionKeyBindings.append (CIMKeyBinding ("Filter",
        filterPath.toString (), CIMKeyBinding::REFERENCE));
    subscriptionKeyBindings.append (CIMKeyBinding ("Handler",
        handlerPath.toString (), CIMKeyBinding::REFERENCE));
    CIMObjectPath subscriptionPath ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDSUBSCRIPTION, subscriptionKeyBindings);

    return subscriptionPath;
}

CIMInstance _buildSubscriptionInstance
    (const CIMObjectPath & filterPath,
     const CIMName & handlerClass,
     const CIMObjectPath & handlerPath)
{
    CIMInstance subscriptionInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);

    subscriptionInstance.addProperty (CIMProperty (CIMName ("Filter"),
        filterPath, 0, PEGASUS_CLASSNAME_INDFILTER));
    subscriptionInstance.addProperty (CIMProperty (CIMName ("Handler"),
        handlerPath, 0, handlerClass));

    return subscriptionInstance;
}

void _checkFilterOrHandlerPath
    (const CIMObjectPath & path,
     const CIMName & className,
     const String & name)
{
    PEGASUS_ASSERT (path.getClassName () == className);
    Array <CIMKeyBinding> keyBindings = path.getKeyBindings ();
    Boolean SCCNfound = false;
    Boolean SNfound = false;
    Boolean CCNfound = false;
    Boolean Nfound = false;
    for (Uint32 i = 0; i < keyBindings.size (); i++)
    {
        if (keyBindings [i].getName ().equal ("SystemCreationClassName"))
        {
            SCCNfound = true;
            PEGASUS_ASSERT (keyBindings [i].getValue () == 
                System::getSystemCreationClassName ());
        }
        else if (keyBindings [i].getName ().equal ("SystemName"))
        {
            SNfound = true;
            PEGASUS_ASSERT (keyBindings [i].getValue () == 
                System::getFullyQualifiedHostName ());
        }
        else if (keyBindings [i].getName ().equal ("CreationClassName"))
        {
            CCNfound = true;
            PEGASUS_ASSERT (keyBindings [i].getValue () == 
                className.getString ());
        }
        else if (keyBindings [i].getName ().equal ("Name"))
        {
            Nfound = true;
            PEGASUS_ASSERT (keyBindings [i].getValue () == name);
        }
        else 
        {
            PEGASUS_ASSERT (false);
        }
    }

    PEGASUS_ASSERT (SCCNfound);
    PEGASUS_ASSERT (SNfound);
    PEGASUS_ASSERT (CCNfound);
    PEGASUS_ASSERT (Nfound);
}

void _checkSubscriptionPath
    (const CIMObjectPath & path,
     const String & filterName,
     const CIMName & handlerClass,
     const String & handlerName)
{
    PEGASUS_ASSERT (path.getClassName () == PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    Array <CIMKeyBinding> keyBindings = path.getKeyBindings ();
    Boolean filterFound = false;
    Boolean handlerFound = false;
    for (Uint32 i = 0; i < keyBindings.size (); i++)
    {
        if (keyBindings [i].getName ().equal ("Filter"))
        {
            filterFound = true;
            CIMObjectPath filterPath = _buildFilterOrHandlerPath 
                (PEGASUS_CLASSNAME_INDFILTER, filterName);
            PEGASUS_ASSERT (keyBindings [i].getValue () == 
                filterPath.toString ());
        }
        else if (keyBindings [i].getName ().equal ("Handler"))
        {
            handlerFound = true;
            CIMObjectPath handlerPath = _buildFilterOrHandlerPath 
                (handlerClass, handlerName);
            PEGASUS_ASSERT (keyBindings [i].getValue () == 
                handlerPath.toString ());
        }
        else 
        {
            PEGASUS_ASSERT (false);
        }
    }

    PEGASUS_ASSERT (filterFound);
    PEGASUS_ASSERT (handlerFound);
}

void _checkStringProperty
    (CIMInstance & instance,
     const String & name,
     const String & value,
     Boolean null = false)
{
    Uint32 pos = instance.findProperty (name);
    PEGASUS_ASSERT (pos != PEG_NOT_FOUND);

    CIMProperty theProperty = instance.getProperty (pos);
    CIMValue theValue = theProperty.getValue ();

    PEGASUS_ASSERT (theValue.getType () == CIMTYPE_STRING);
    PEGASUS_ASSERT (!theValue.isArray ());
    if (null)
    {
        PEGASUS_ASSERT (theValue.isNull ());
    }
    else
    {
        PEGASUS_ASSERT (!theValue.isNull ());
        String result;
        theValue.get (result);

        if (verbose)
        {
            if (result != value)
            {
                cerr << "Property value comparison failed.  ";
                cerr << "Expected " << value << "; ";
                cerr << "Actual property value was " << result << "." << endl;
            }
        }

        PEGASUS_ASSERT (result == value);
    }
}

void _checkUint16Property
    (CIMInstance & instance,
     const String & name,
     Uint16 value)
{
    Uint32 pos = instance.findProperty (name);
    PEGASUS_ASSERT (pos != PEG_NOT_FOUND);

    CIMProperty theProperty = instance.getProperty (pos);
    CIMValue theValue = theProperty.getValue ();

    PEGASUS_ASSERT (theValue.getType () == CIMTYPE_UINT16);
    PEGASUS_ASSERT (!theValue.isArray ());
    PEGASUS_ASSERT (!theValue.isNull ());
    Uint16 result;
    theValue.get (result);

    if (verbose)
    {
        if (result != value)
        {
            cerr << "Property value comparison failed.  ";
            cerr << "Expected " << value << "; ";
            cerr << "Actual property value was " << result << "." << endl;
        }
    }

    PEGASUS_ASSERT (result == value);
}

void _checkUint32Property
    (CIMInstance & instance,
     const String & name,
     Uint32 value)
{
    Uint32 pos = instance.findProperty (name);
    PEGASUS_ASSERT (pos != PEG_NOT_FOUND);

    CIMProperty theProperty = instance.getProperty (pos);
    CIMValue theValue = theProperty.getValue ();

    PEGASUS_ASSERT (theValue.getType () == CIMTYPE_UINT32);
    PEGASUS_ASSERT (!theValue.isArray ());
    PEGASUS_ASSERT (!theValue.isNull ());
    Uint32 result;
    theValue.get (result);

    if (verbose)
    {
        if (result != value)
        {
            cerr << "Property value comparison failed.  ";
            cerr << "Expected " << value << "; ";
            cerr << "Actual property value was " << result << "." << endl;
        }
    }

    PEGASUS_ASSERT (result == value);
}

void _checkUint64Property
    (CIMInstance & instance,
     const String & name,
     Uint64 value)
{
    Uint32 pos = instance.findProperty (name);
    PEGASUS_ASSERT (pos != PEG_NOT_FOUND);

    CIMProperty theProperty = instance.getProperty (pos);
    CIMValue theValue = theProperty.getValue ();

    PEGASUS_ASSERT (theValue.getType () == CIMTYPE_UINT64);
    PEGASUS_ASSERT (!theValue.isArray ());
    PEGASUS_ASSERT (!theValue.isNull ());
    Uint64 result;
    theValue.get (result);

    if (verbose)
    {
        if (result != value)
        {
            char buffer [32];  // Should need 21 chars max
            sprintf (buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", value);
            cerr << "Property value comparison failed.  ";
            cerr << "Expected " << buffer << "; ";
            sprintf (buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", result);
            cerr << "Actual property value was " << buffer << "." << endl;
        }
    }

    PEGASUS_ASSERT (result == value);
}

void _checkExceptionCode
    (const CIMException & e,
     const CIMStatusCode expectedCode)
{
    if (verbose)
    {
        if (e.getCode () != expectedCode)
        {
            cerr << "CIMException comparison failed.  ";
            cerr << "Expected " << cimStatusCodeToString (expectedCode) << "; ";
            cerr << "Actual exception was " << e.getMessage () << "." << endl;
        }
    }

    PEGASUS_ASSERT (e.getCode () == expectedCode);
}

void _deleteSubscriptionInstance 
    (CIMClient & client, 
     const String & filterName,
     const CIMName & handlerClass,
     const String & handlerName)
{
    CIMObjectPath subscriptionPath = _buildSubscriptionPath 
        (filterName, handlerClass, handlerName);
    client.deleteInstance (NAMESPACE, subscriptionPath);
}

void _deleteHandlerInstance 
    (CIMClient & client, 
     const CIMName & className,
     const String & name)
{
    CIMObjectPath path = _buildFilterOrHandlerPath (className, name);
    client.deleteInstance (NAMESPACE, path);
}

void _deleteFilterInstance 
    (CIMClient & client, 
     const String & name)
{
    CIMObjectPath path = _buildFilterOrHandlerPath 
        (PEGASUS_CLASSNAME_INDFILTER, name);
    client.deleteInstance (NAMESPACE, path);
}

void _deleteCapabilityInstance 
    (CIMClient & client, 
     const String & providerModuleName,
     const String & providerName,
     const String & capabilityID)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("ProviderModuleName",
        providerModuleName, CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("ProviderName",
        providerName, CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("CapabilityID",
        capabilityID, CIMKeyBinding::STRING));
    CIMObjectPath path ("", CIMNamespaceName (),
        CIMName ("PG_ProviderCapabilities"), keyBindings);
    client.deleteInstance (NAMESPACE, path);
}

void _deleteProviderInstance 
    (CIMClient & client, 
     const String & name,
     const String & providerModuleName)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("Name",
        name, CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("ProviderModuleName",
        providerModuleName, CIMKeyBinding::STRING));
    CIMObjectPath path ("", CIMNamespaceName (),
        CIMName ("PG_Provider"), keyBindings);
    client.deleteInstance (NAMESPACE, path);
}

void _deleteModuleInstance 
    (CIMClient & client, 
     const String & name)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("Name",
        name, CIMKeyBinding::STRING));
    CIMObjectPath path ("", CIMNamespaceName (),
        CIMName ("PG_ProviderModule"), keyBindings);
    client.deleteInstance (NAMESPACE, path);
}

void _usage ()
{
    cerr 
        << "Usage: TestSubscription "
        << "{register | test | unregister | cleanup }" 
        << endl;
}

void _register (CIMClient & client)
{
    try
    {
        Array <String> namespaces;
        Array <Uint16> providerTypes;
        namespaces.append (SOURCENAMESPACE.getString ());
        providerTypes.append (4);

        //
        //  Register the ProcessIndicationProvider
        //
        _createModuleInstance (client, 
            String ("ProcessIndicationProviderModule"), 
            String ("ProcessIndicationProvider"));
        _createProviderInstance (client, 
            String ("ProcessIndicationProvider"), 
            String ("ProcessIndicationProviderModule"));
        _createCapabilityInstance (client, 
            String ("ProcessIndicationProviderModule"), 
            String ("ProcessIndicationProvider"),
            String ("ProcessIndicationProviderCapability"),
            String ("CIM_ProcessIndication"),
            namespaces, 
            providerTypes,
            CIMPropertyList ());

        //
        //  Register the AlertIndicationProvider
        //
        _createModuleInstance (client, 
            String ("AlertIndicationProviderModule"), 
            String ("AlertIndicationProvider"));
        _createProviderInstance (client, 
            String ("AlertIndicationProvider"), 
            String ("AlertIndicationProviderModule"));
        _createCapabilityInstance (client, 
            String ("AlertIndicationProviderModule"), 
            String ("AlertIndicationProvider"),
            String ("AlertIndicationProviderCapability"),
            String ("CIM_AlertIndication"),
            namespaces, 
            providerTypes,
            CIMPropertyList ());
    }
    catch (Exception & e)
    {
        cerr << "register failed: " << e.getMessage () << endl;
        exit (-1);
    }

    cout << "+++++ register completed successfully" << endl;
}

//
//  Valid test cases: create, get, enumerate, modify, delete operations
//
void _valid (CIMClient & client, String& qlang)
{
    CIMObjectPath path;
    String query;
    CIMInstance retrievedInstance;
    Array <CIMInstance> instances;
    Array <CIMObjectPath> paths;

    //
    //  Create filter that selects all properties from CIM_ProcessIndication
    //
    CIMInstance filter01 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT * FROM CIM_ProcessIndication";
    _addStringProperty (filter01, "SystemCreationClassName", 
         System::getSystemCreationClassName ());
    _addStringProperty (filter01, "SystemName", 
         System::getFullyQualifiedHostName ());
    _addStringProperty (filter01, "CreationClassName", 
         PEGASUS_CLASSNAME_INDFILTER.getString ());
    _addStringProperty (filter01, "Name", "Filter01");
    _addStringProperty (filter01, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _addStringProperty (filter01, "Query", query);
    _addStringProperty (filter01, "QueryLanguage", qlang);
    path = client.createInstance (NAMESPACE, filter01);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter01");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", qlang);

    //
    //  Enumerate filters
    //
    instances = client.enumerateInstances 
        (NAMESPACE, PEGASUS_CLASSNAME_INDFILTER);
    PEGASUS_ASSERT (instances.size () == 1);
    _checkStringProperty (instances [0], "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (instances [0], "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (instances [0], "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (instances [0], "Name", "Filter01");
    _checkStringProperty (instances [0], "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (instances [0], "Query", query);
    _checkStringProperty (instances [0], "QueryLanguage", qlang);

    paths = client.enumerateInstanceNames
        (NAMESPACE, PEGASUS_CLASSNAME_INDFILTER);
    PEGASUS_ASSERT (paths.size () == 1);
    _checkFilterOrHandlerPath (paths [0], PEGASUS_CLASSNAME_INDFILTER, 
        "Filter01");

    //
    //  Create filter that selects some properties from 
    //  CIM_ProcessIndication
    //
    CIMInstance filter02 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT IndicationTime, IndicationIdentifier, "
        "CorrelatedIndications "
        "FROM CIM_ProcessIndication";
    _addStringProperty (filter02, "Name", "Filter02");
    _addStringProperty (filter02, "Query", query);
    _addStringProperty (filter02, "QueryLanguage", qlang);
    _addStringProperty (filter02, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter02);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter02");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter02");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", qlang);

    //
    //  Create filter that selects one property from CIM_ProcessIndication
    //
    CIMInstance filter03 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT IndicationTime FROM CIM_ProcessIndication";
    _addStringProperty (filter03, "Name", "Filter03");
    _addStringProperty (filter03, "Query", query);
    _addStringProperty (filter03, "QueryLanguage", qlang);
    _addStringProperty (filter03, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter03);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter03");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter03");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", qlang);

    //
    //  Create filter that selects properties from CIM_ProcessIndication
    //  and has a where clause condition
    //
    CIMInstance filter04 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT IndicationTime, IndicationIdentifier "
          "FROM CIM_ProcessIndication "
          "WHERE IndicationTime IS NOT NULL";
    _addStringProperty (filter04, "Name", "Filter04");
    _addStringProperty (filter04, "Query", query);
    _addStringProperty (filter04, "QueryLanguage", qlang);
    _addStringProperty (filter04, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter04);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter04");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter04");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", qlang);


#ifndef PEGASUS_DISABLE_CQL
    //
    //  Create filter that selects properties from CIM_ProcessIndication
    //  and has a where clause condition that includes an array property.
    //  Note: this is only allowed by CQL.
    //
    CIMInstance filter04a (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT IndicationTime, IndicationIdentifier "
          "FROM CIM_ProcessIndication "
          "WHERE IndicationTime IS NOT NULL AND CorrelatedIndications IS NOT NULL";
    _addStringProperty (filter04a, "Name", "Filter04a");
    _addStringProperty (filter04a, "Query", query);
    _addStringProperty (filter04a, "QueryLanguage", "CIM:CQL");  // hardcode to CQL
    _addStringProperty (filter04a, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter04a);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter04a");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter04a");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", "CIM:CQL"); // hardcode to CQL
#endif

    //
    //  Create filter that selects all properties from CIM_ProcessIndication
    //  and has a where clause condition
    //
    CIMInstance filter05 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT * FROM CIM_ProcessIndication "
        "WHERE IndicationTime IS NOT NULL";
    _addStringProperty (filter05, "Name", "Filter05");
    _addStringProperty (filter05, "Query", query);
    _addStringProperty (filter05, "QueryLanguage", qlang);
    _addStringProperty (filter05, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter05);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter05");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter05");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", qlang);

    //
    //  Create filter that selects all properties from CIM_AlertIndication
    //  and has a where clause condition
    //
    CIMInstance filter06 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT * FROM CIM_AlertIndication WHERE AlertType = 5";
    _addStringProperty (filter06, "Name", "Filter06");
    _addStringProperty (filter06, "Query", query);
    _addStringProperty (filter06, "QueryLanguage", qlang);
    _addStringProperty (filter06, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter06);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDFILTER, "Filter06");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Filter06");
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    _checkStringProperty (retrievedInstance, "Query", query);
    _checkStringProperty (retrievedInstance, "QueryLanguage", qlang);

    //
    //  Create persistent CIMXML handler
    //
    CIMInstance handler01 (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    _addStringProperty (handler01, "SystemCreationClassName", 
         System::getSystemCreationClassName ());
    _addStringProperty (handler01, "SystemName", 
         System::getFullyQualifiedHostName ());
    _addStringProperty (handler01, "CreationClassName", 
         PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ());
    _addStringProperty (handler01, "Name", "Handler01");
    _addStringProperty (handler01, "Owner", "an owner");
    _addUint16Property (handler01, "PersistenceType", 2);
    _addStringProperty (handler01, "OtherPersistenceType", String::EMPTY, true);
    _addStringProperty (handler01, "Destination", 
        "localhost/CIMListener/test1");
    path = client.createInstance (NAMESPACE, handler01);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Handler01");
    _checkStringProperty (retrievedInstance, "Owner", "an owner");
    _checkUint16Property (retrievedInstance, "PersistenceType", 2);
    _checkStringProperty (retrievedInstance, "OtherPersistenceType", 
        String::EMPTY, true);
    _checkStringProperty (retrievedInstance, "Destination", 
        "localhost/CIMListener/test1");

    //
    //  Enumerate handlers
    //
    instances = client.enumerateInstances 
        (NAMESPACE, PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    PEGASUS_ASSERT (instances.size () == 1);
    _checkStringProperty (instances [0], "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (instances [0], "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (instances [0], "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ());
    _checkStringProperty (instances [0], "Name", "Handler01");
    _checkStringProperty (instances [0], "Owner", "an owner");
    _checkUint16Property (instances [0], "PersistenceType", 2);
    _checkStringProperty (instances [0], "OtherPersistenceType", 
        String::EMPTY, true);
    _checkStringProperty (instances [0], "Destination", 
        "localhost/CIMListener/test1");

    //
    //  Create transient CIMXML handler
    //
    CIMInstance handler02 (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    _addStringProperty (handler02, "Name", "Handler02");
    _addUint16Property (handler02, "PersistenceType", 3);
    _addStringProperty (handler02, "Destination", 
        "localhost/CIMListener/test2");
    path = client.createInstance (NAMESPACE, handler02);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler02");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Handler02");
    _checkUint16Property (retrievedInstance, "PersistenceType", 3);
    _checkStringProperty (retrievedInstance, "Destination", 
        "localhost/CIMListener/test2");

    //
    //  Create persistent SNMP handler
    //
    CIMInstance handler03 (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
    _addStringProperty (handler03, "SystemCreationClassName", 
         System::getSystemCreationClassName ());
    _addStringProperty (handler03, "SystemName", 
         System::getFullyQualifiedHostName ());
    _addStringProperty (handler03, "CreationClassName", 
         PEGASUS_CLASSNAME_INDHANDLER_SNMP.getString ());
    _addStringProperty (handler03, "Name", "Handler03");
    _addStringProperty (handler03, "Owner", "an owner");
    _addUint16Property (handler03, "PersistenceType", 2);
    _addStringProperty (handler03, "OtherPersistenceType", String::EMPTY, true);
    _addStringProperty (handler03, "TargetHost", "localhost");
    _addUint16Property (handler03, "TargetHostFormat", 2);
    _addUint32Property (handler03, "PortNumber", 162);
    _addUint16Property (handler03, "SNMPVersion", 3);
    _addStringProperty (handler03, "SNMPSecurityName", "a name");
    _addStringProperty (handler03, "SNMPEngineID", "an ID");
    path = client.createInstance (NAMESPACE, handler03);

    _checkFilterOrHandlerPath (path, PEGASUS_CLASSNAME_INDHANDLER_SNMP, 
        "Handler03");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_SNMP.getString ());
    _checkStringProperty (retrievedInstance, "Name", "Handler03");
    _checkStringProperty (retrievedInstance, "Owner", "an owner");
    _checkUint16Property (retrievedInstance, "PersistenceType", 2);
    _checkStringProperty (retrievedInstance, "OtherPersistenceType", 
        String::EMPTY, true);
    _checkStringProperty (retrievedInstance, "TargetHost", "localhost");
    _checkUint16Property (retrievedInstance, "TargetHostFormat", 2);
    _checkUint32Property (retrievedInstance, "PortNumber", 162);
    _checkUint16Property (retrievedInstance, "SNMPVersion", 3);
    _checkStringProperty (retrievedInstance, "SNMPSecurityName", "a name");
    _checkStringProperty (retrievedInstance, "SNMPEngineID", "an ID");

    //
    //  Enumerate handlers
    //
    instances = client.enumerateInstances 
        (NAMESPACE, PEGASUS_CLASSNAME_INDHANDLER_SNMP);
    PEGASUS_ASSERT (instances.size () == 1);
    _checkStringProperty (instances [0], "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (instances [0], "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (instances [0], "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_SNMP.getString ());
    _checkStringProperty (instances [0], "Name", "Handler03");
    _checkStringProperty (instances [0], "Owner", "an owner");
    _checkUint16Property (instances [0], "PersistenceType", 2);
    _checkStringProperty (instances [0], "OtherPersistenceType", 
        String::EMPTY, true);
    _checkStringProperty (instances [0], "TargetHost", "localhost");
    _checkUint16Property (instances [0], "TargetHostFormat", 2);
    _checkUint32Property (instances [0], "PortNumber", 162);
    _checkUint16Property (instances [0], "SNMPVersion", 3);
    _checkStringProperty (instances [0], "SNMPSecurityName", "a name");
    _checkStringProperty (instances [0], "SNMPEngineID", "an ID");

    //
    //  Create subscriptions
    //
    CIMInstance subscription01 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter01"),
         PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
         _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             "Handler01"));
    _addUint16Property (subscription01, "OnFatalErrorPolicy", 2);
    _addStringProperty (subscription01, "OtherOnFatalErrorPolicy", 
        String::EMPTY, true);
    _addUint64Property (subscription01, "FailureTriggerTimeInterval", 60);
    _addUint16Property (subscription01, "SubscriptionState", 2);
    _addStringProperty (subscription01, "OtherSubscriptionState", String::EMPTY,
        true);
    _addUint64Property (subscription01, "SubscriptionDuration", 
        PEGASUS_UINT64_LITERAL(60000000000));
    _addUint16Property (subscription01, "RepeatNotificationPolicy", 1);
    _addStringProperty (subscription01, "OtherRepeatNotificationPolicy", 
        "another policy");
    _addUint64Property (subscription01, "RepeatNotificationInterval", 60);
    _addUint64Property (subscription01, "RepeatNotificationGap", 30);
    _addUint16Property (subscription01, "RepeatNotificationCount", 5);
    path = client.createInstance (NAMESPACE, subscription01);

    _checkSubscriptionPath (path, "Filter01", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkStringProperty (retrievedInstance, "OtherOnFatalErrorPolicy", 
        String::EMPTY, true);
    _checkUint64Property (retrievedInstance, "FailureTriggerTimeInterval", 60);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkStringProperty (retrievedInstance, "OtherSubscriptionState", 
        String::EMPTY, true);
    _checkUint64Property (retrievedInstance, "SubscriptionDuration", 
        PEGASUS_UINT64_LITERAL(60000000000));
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 1);
    _checkStringProperty (retrievedInstance, "OtherRepeatNotificationPolicy", 
        "another policy");
    _checkUint64Property (retrievedInstance, "RepeatNotificationInterval", 60);
    _checkUint64Property (retrievedInstance, "RepeatNotificationGap", 30);
    _checkUint16Property (retrievedInstance, "RepeatNotificationCount", 5);

    //
    //  Enumerate subscriptions
    //
    instances = client.enumerateInstances 
        (NAMESPACE, PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    PEGASUS_ASSERT (instances.size () == 1);
    _checkUint16Property (instances [0], "OnFatalErrorPolicy", 2);
    _checkStringProperty (instances [0], "OtherOnFatalErrorPolicy", 
        String::EMPTY, true);
    _checkUint64Property (instances [0], "FailureTriggerTimeInterval", 60);
    _checkUint16Property (instances [0], "SubscriptionState", 2);
    _checkStringProperty (instances [0], "OtherSubscriptionState", 
        String::EMPTY, true);
    _checkUint64Property (instances [0], "SubscriptionDuration", 
        PEGASUS_UINT64_LITERAL(60000000000));
    _checkUint16Property (instances [0], "RepeatNotificationPolicy", 1);
    _checkStringProperty (instances [0], "OtherRepeatNotificationPolicy", 
        "another policy");
    _checkUint64Property (instances [0], "RepeatNotificationInterval", 60);
    _checkUint64Property (instances [0], "RepeatNotificationGap", 30);
    _checkUint16Property (instances [0], "RepeatNotificationCount", 5);

    paths = client.enumerateInstanceNames
        (NAMESPACE, PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    PEGASUS_ASSERT (paths.size () == 1);
    _checkSubscriptionPath (path, "Filter01", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");

    CIMInstance subscription02 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter02"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
            "Handler01"));
    path = client.createInstance (NAMESPACE, subscription02);

    _checkSubscriptionPath (path, "Filter02", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    CIMInstance subscription03 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter03"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
            "Handler01"));
    path = client.createInstance (NAMESPACE, subscription03);

    _checkSubscriptionPath (path, "Filter03", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    CIMInstance subscription04 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter04"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
            "Handler01"));
    path = client.createInstance (NAMESPACE, subscription04);

    _checkSubscriptionPath (path, "Filter04", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    CIMInstance subscription05 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter05"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
            "Handler01"));
    path = client.createInstance (NAMESPACE, subscription05);

    _checkSubscriptionPath (path, "Filter05", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    CIMInstance subscription06 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter06"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
            "Handler01"));
    path = client.createInstance (NAMESPACE, subscription06);

    _checkSubscriptionPath (path, "Filter06", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    //
    //  Create subscription with transient handler
    //
    CIMInstance subscription07 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter01"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
            "Handler02"));
    path = client.createInstance (NAMESPACE, subscription07);

    _checkSubscriptionPath (path, "Filter01", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler02");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    //
    //  Delete transient handler - referencing subscription must be deleted
    //
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler02");
    try
    {
        retrievedInstance = client.getInstance (NAMESPACE, path);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_FOUND);
    }

    //
    //  Create subscription with SNMP handler
    //
    CIMInstance subscription08 = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter01"),
        PEGASUS_CLASSNAME_INDHANDLER_SNMP,
        _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_SNMP,
            "Handler03"));
    path = client.createInstance (NAMESPACE, subscription08);

    _checkSubscriptionPath (path, "Filter01", 
        PEGASUS_CLASSNAME_INDHANDLER_SNMP, "Handler03");
    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);

    //
    //  Enumerate filters
    //
    instances = client.enumerateInstances (NAMESPACE, 
        PEGASUS_CLASSNAME_INDFILTER);
#ifndef PEGASUS_DISABLE_CQL
    PEGASUS_ASSERT (instances.size () == 7);
#else
    PEGASUS_ASSERT (instances.size () == 6);
#endif


    paths = client.enumerateInstanceNames (NAMESPACE, 
        PEGASUS_CLASSNAME_INDFILTER);
#ifndef PEGASUS_DISABLE_CQL
    PEGASUS_ASSERT (paths.size () == 7);
#else
    PEGASUS_ASSERT (paths.size () == 6);
#endif

    //
    //  Enumerate handlers
    //
    Array <CIMInstance> handlers = client.enumerateInstances 
        (NAMESPACE, PEGASUS_CLASSNAME_INDHANDLER);
    PEGASUS_ASSERT (handlers.size () == 2);

    Array <CIMObjectPath> cimxmlHandlerPaths = client.enumerateInstanceNames
        (NAMESPACE, PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    PEGASUS_ASSERT (cimxmlHandlerPaths.size () == 1);
    _checkFilterOrHandlerPath (cimxmlHandlerPaths [0], 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");

    Array <CIMObjectPath> snmpHandlerPaths = client.enumerateInstanceNames
        (NAMESPACE, PEGASUS_CLASSNAME_INDHANDLER_SNMP);
    PEGASUS_ASSERT (snmpHandlerPaths.size () == 1);
    _checkFilterOrHandlerPath (snmpHandlerPaths [0], 
        PEGASUS_CLASSNAME_INDHANDLER_SNMP, "Handler03");

    Array <CIMObjectPath> handlerPaths = client.enumerateInstanceNames
        (NAMESPACE, PEGASUS_CLASSNAME_INDHANDLER);
    PEGASUS_ASSERT (handlerPaths.size () == 2);

    //
    //  Enumerate subscriptions
    //
    Array <CIMInstance> subscriptions = client.enumerateInstances 
        (NAMESPACE, PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    PEGASUS_ASSERT (subscriptions.size () == 7);

    Array <CIMObjectPath> subscriptionPaths = client.enumerateInstanceNames
        (NAMESPACE, PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    PEGASUS_ASSERT (subscriptionPaths.size () == 7);

    //
    //  Modify subscription: set state to disabled
    //
    CIMInstance modifiedInstance01 (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    _addUint16Property (modifiedInstance01, "SubscriptionState", 4);
    path = _buildSubscriptionPath ("Filter01",
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    modifiedInstance01.setPath (path);
    Array <CIMName> propertyNames01;
    propertyNames01.append (CIMName ("SubscriptionState"));
    CIMPropertyList properties01 (propertyNames01);
    client.modifyInstance (NAMESPACE, modifiedInstance01, false, properties01);

    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 4);

    //
    //  Modify subscription: specify 0 properties to be modified
    //
    CIMInstance modifiedInstance02 (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    _addUint16Property (modifiedInstance02, "SubscriptionState", 2);
    path = _buildSubscriptionPath ("Filter01",
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    modifiedInstance02.setPath (path);
    Array <CIMName> propertyNames02;
    CIMPropertyList properties02 (propertyNames02);
    client.modifyInstance (NAMESPACE, modifiedInstance02, false, properties02);

    retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 4);

    //
    //  Provider registration changes: create, modify and delete operations
    //

    //
    //  Create a new CIM_ProcessIndication provider that supports only the
    //  IndicationTime property
    //
    Array <String> namespaces;
    Array <Uint16> providerTypes;
    Array <CIMName> propertyNames;
    namespaces.append (SOURCENAMESPACE.getString ());
    providerTypes.append (4);
    propertyNames.append (CIMName ("IndicationTime"));
    CIMPropertyList properties (propertyNames);
    _createProviderInstance (client, 
        String ("ProcessIndicationProvider2"), 
        String ("ProcessIndicationProviderModule"));
    _createCapabilityInstance (client, 
        String ("ProcessIndicationProviderModule"), 
        String ("ProcessIndicationProvider2"),
        String ("ProcessIndicationProviderCapability2"),
        String ("CIM_ProcessIndication"),
        namespaces, 
        providerTypes,
        properties);

    //
    //  Modify the CIM_ProcessIndication provider to support both the
    //  IndicationTime and IndicationIdentifier properties
    //
    propertyNames.clear ();
    propertyNames.append (CIMName ("IndicationTime"));
    propertyNames.append (CIMName ("IndicationIdentifier"));
    properties.clear ();
    properties.set (propertyNames);
    _modifyCapabilityInstance (client, 
        String ("ProcessIndicationProviderModule"), 
        String ("ProcessIndicationProvider2"),
        String ("ProcessIndicationProviderCapability2"),
        properties);

    //
    //  Modify the CIM_ProcessIndication provider to again support only the
    //  IndicationTime property
    //
    propertyNames.clear ();
    propertyNames.append (CIMName ("IndicationTime"));
    propertyNames.append (CIMName ("IndicationIdentifier"));
    properties.clear ();
    properties.set (propertyNames);
    _modifyCapabilityInstance (client, 
        String ("ProcessIndicationProviderModule"), 
        String ("ProcessIndicationProvider2"),
        String ("ProcessIndicationProviderCapability2"),
        properties);

    //
    //  Delete the provider 
    //
    _deleteCapabilityInstance (client, 
        "ProcessIndicationProviderModule", 
        "ProcessIndicationProvider2",
        "ProcessIndicationProviderCapability2");
    _deleteProviderInstance (client, 
        "ProcessIndicationProvider2", 
        "ProcessIndicationProviderModule");
}

//
//  Default value test cases: create and modify requests
//
void _default (CIMClient & client)
{
    // Note: since these testcases are used to test defaulted
    // properties on the filters, handlers, and subscriptions,
    // there is no need for CQL-specific tests.

    //
    //  Filter: Missing SystemCreationClassName must get default value
    //          Missing SystemName must get default value
    //          Missing CreationClassName must get default value
    //          Missing SourceNamespace must get default value
    //
    {
    CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
    String query = "SELECT * FROM CIM_ProcessIndication";
    _addStringProperty (filter, "Name", "Filter00");
    _addStringProperty (filter, "Query", query);
    _addStringProperty (filter, "QueryLanguage", "WQL");
    CIMObjectPath path = client.createInstance (NAMESPACE, filter);
    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        NAMESPACE.getString ());
    _deleteFilterInstance (client, "Filter00");
    }

    //
    //  Filter: Null SystemCreationClassName must get default value
    //          Null SystemName must get default value
    //          Null CreationClassName must get default value
    //          Null SourceNamespace must get default value
    //
    {
    CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
    String query = "SELECT * FROM CIM_ProcessIndication";
    _addStringProperty (filter, "Name", "Filter00");
    _addStringProperty (filter, "Query", query);
    _addStringProperty (filter, "QueryLanguage", "WQL");
    _addStringProperty (filter, "SourceNamespace", String::EMPTY, true);
    CIMObjectPath path = client.createInstance (NAMESPACE, filter);

    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString ());
    _checkStringProperty (retrievedInstance, "SourceNamespace", 
        NAMESPACE.getString ());
    _deleteFilterInstance (client, "Filter00");
    }

    //
    //  Handler: Missing SystemCreationClassName must get default value
    //           Missing SystemName must get default value
    //           Missing CreationClassName must get default value
    //           Missing PersistenceType must get default value
    //
    {
    CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    _addStringProperty (handler, "Name", "Handler00");
    _addStringProperty (handler, "Destination", "localhost/CIMListener/test1", 
        false);
    CIMObjectPath path = client.createInstance (NAMESPACE, handler);
    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ());
    _checkUint16Property (retrievedInstance, "PersistenceType", 2);
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler00");
    }

    //
    //  Handler: Null SystemCreationClassName must get default value
    //           Null SystemName must get default value
    //           Null CreationClassName must get default value
    //           Null PersistenceType must get default value
    //
    {
    CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    _addStringProperty (handler, "Name", "Handler00");
    _addStringProperty (handler, "Destination", "localhost/CIMListener/test1", 
        false);
    _addUint16Property (handler, "PersistenceType", 0, true);
    CIMObjectPath path = client.createInstance (NAMESPACE, handler);
    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkStringProperty (retrievedInstance, "SystemCreationClassName", 
        System::getSystemCreationClassName ());
    _checkStringProperty (retrievedInstance, "SystemName", 
        System::getFullyQualifiedHostName ());
    _checkStringProperty (retrievedInstance, "CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ());
    _checkUint16Property (retrievedInstance, "PersistenceType", 2);
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler00");
    }

    //
    //  Create filter and handler for testing subscriptions
    //
    CIMInstance filter00 (PEGASUS_CLASSNAME_INDFILTER);
    String query = "SELECT * FROM CIM_ProcessIndication";
    _addStringProperty (filter00, "Name", "Filter00");
    _addStringProperty (filter00, "Query", query);
    _addStringProperty (filter00, "QueryLanguage", "WQL");
    _addStringProperty (filter00, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    CIMObjectPath fPath = client.createInstance (NAMESPACE, filter00);

    CIMInstance handler00 (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    _addStringProperty (handler00, "Name", "Handler00");
    _addStringProperty (handler00, "Destination", "localhost/CIMListener/test0",
        false);
    CIMObjectPath hPath = client.createInstance (NAMESPACE, handler00);

    //
    //  Subscription: missing SubscriptionState must get default value
    //                missing OnFatalErrorPolicy must get default value
    //                missing RepeatNotificationPolicy must get default value
    //
    {
    CIMInstance subscription = _buildSubscriptionInstance
        (fPath, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, hPath);
    CIMObjectPath path = client.createInstance (NAMESPACE, subscription);
    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);
    _deleteSubscriptionInstance (client, "Filter00", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler00");
    }

    //
    //  Subscription: null SubscriptionState must get default value
    //                null OnFatalErrorPolicy must get default value
    //                null RepeatNotificationPolicy must get default value
    //
    {
    CIMInstance subscription = _buildSubscriptionInstance
        (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, "Filter00"),
         PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
         _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             "Handler00"));
    _addUint16Property (subscription, "SubscriptionState", 0, true);
    _addUint16Property (subscription, "OnFatalErrorPolicy", 0, true);
    _addUint16Property (subscription, "RepeatNotificationPolicy", 0, true);
    CIMObjectPath path = client.createInstance (NAMESPACE, subscription);
    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    _checkUint16Property (retrievedInstance, "OnFatalErrorPolicy", 2);
    _checkUint16Property (retrievedInstance, "RepeatNotificationPolicy", 2);
    _deleteSubscriptionInstance (client, "Filter00", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler00");
    }

    //
    //  Modify subscription: null SubscriptionState must get default value
    //
    {
    CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    _addUint16Property (modifiedInstance, "SubscriptionState", 0, true);
    CIMObjectPath path = _buildSubscriptionPath ("Filter01",
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    modifiedInstance.setPath (path);
    Array <CIMName> propertyNames;
    propertyNames.append (CIMName ("SubscriptionState"));
    CIMPropertyList properties (propertyNames);
    client.modifyInstance (NAMESPACE, modifiedInstance, false, properties);

    CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    _checkUint16Property (retrievedInstance, "SubscriptionState", 2);
    }

    //
    //  Delete filter and handler instances
    //
    _deleteFilterInstance (client, "Filter00");
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler00");
}

//
//  Error test cases: invalid queries
//
void _errorQueries (CIMClient & client, String& qlang)
{
    //
    //  Filter: Invalid query - invalid indication class name in FROM clause of
    //          CIM_IndicationFilter Query property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ManagedElement";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", qlang);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
#ifdef PEGASUS_DISABLE_CQL
        if (String::equalNoCase(qlang, "cim:cql"))
        {
          // If CQL is disabled, then a non-supported error
          // for invalid language is expected.
          _checkExceptionCode(e, CIM_ERR_NOT_SUPPORTED);

          return;
        }
        else
        {
          _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
        }
#else        
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
#endif
    }

    //
    //  Filter: Invalid query - parse error (join)
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = 
            "SELECT * FROM CIM_ProcessIndication, CIM_AlertIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", qlang);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Invalid query - Array property is not supported in the WQL 
    //          WHERE clause.
    //          Note: CQL allows array properties in the WHERE clause.
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = 
            "SELECT * FROM CIM_ProcessIndication "
            "WHERE CorrelatedIndications IS NOT NULL";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL"); // hardcode to WQL
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //          
    //  Filter: Invalid query - Property referenced in the WHERE clause not
    //          found in the indication class in the FROM clause
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = 
            "SELECT * FROM CIM_ProcessIndication "
            "WHERE AlertType = 3";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", qlang);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //          
    //  Filter: Invalid query - Property referenced in the SELECT clause 
    //          not found in the indication class in the FROM clause
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT PerceivedSeverity FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", qlang);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }
}

//
//  Error test cases: create, modify and delete requests
//
void _error (CIMClient & client)
{
    //
    //  Filter: Invalid SystemCreationClassName key property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "SystemCreationClassName", "invalidName", 
            false);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: SystemCreationClassName key property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addUint16Property (filter, "SystemCreationClassName", 1);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage",  "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: SystemCreationClassName key property of array type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "SystemCreationClassName", 
            System::getSystemCreationClassName (), false, true);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Invalid SystemName key property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "SystemName", "invalidName");
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: SystemName key property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addUint16Property (filter, "SystemName", 1);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Invalid CreationClassName key property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "CreationClassName", "invalidName");
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: CreationClassName key property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addUint16Property (filter, "CreationClassName", 1);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Missing required Name key property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Null required Name key property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", String::EMPTY, true);
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Required Name key property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addUint16Property (filter, "Name", 1);
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Required Name key property of array type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00", false, true);
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: SourceNamespace property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addUint16Property (filter, "SourceNamespace", 1);
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Missing required Query property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Null required Query property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", String::EMPTY, true);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Required Query property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        _addStringProperty (filter, "Name", "Filter00");
        _addUint16Property (filter, "Query", 1);
        _addStringProperty (filter, "QueryLanguage", "WQL");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Missing required QueryLanguage property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Null required QueryLanguage property
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", String::EMPTY, true);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Filter: Required QueryLanguage property unsupported value
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addStringProperty (filter, "QueryLanguage", "unknownQueryLanguage");
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Filter: Required QueryLanguage property of incorrect type
    //
    try
    {
        CIMInstance filter (PEGASUS_CLASSNAME_INDFILTER);
        String query = "SELECT * FROM CIM_ProcessIndication";
        _addStringProperty (filter, "Name", "Filter00");
        _addStringProperty (filter, "Query", query);
        _addUint16Property (filter, "QueryLanguage", 1);
        _addStringProperty (filter, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        CIMObjectPath path = client.createInstance (NAMESPACE, filter);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //          
    //  Filter: Attempt to delete a filter referenced by a subscription
    //          A Filter referenced by a subscription may not be deleted
    //
    try
    {
        _deleteFilterInstance (client, "Filter01");
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_FAILED);
    }

    //
    //  Handler: Invalid SystemCreationClassName key property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "SystemCreationClassName", "invalidName",
            false);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: SystemCreationClassName key property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addUint16Property (handler, "SystemCreationClassName", 1);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Invalid SystemName key property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "SystemName", "invalidName");
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: SystemName key property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addUint16Property (handler, "SystemName", 1);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Invalid CreationClassName key property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "CreationClassName", "invalidName");
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: CreationClassName key property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addUint16Property (handler, "CreationClassName", 1);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Missing required Name key property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Null required Name key property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", String::EMPTY, true);
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Required Name key property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addUint16Property (handler, "Name", 1);
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Unsupported value 1 (Other) for property PersistenceType
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addUint16Property (handler, "PersistenceType", 1);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Handler: OtherPersistenceType property present, but PersistenceType 
    //           value not 1 (Other)
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addUint16Property (handler, "PersistenceType", 2);
        _addStringProperty (handler, "OtherPersistenceType", "another type", 
            false);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Invalid value 0 for property PersistenceType
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addUint16Property (handler, "PersistenceType", 0);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Invalid value 4 for property PersistenceType
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addUint16Property (handler, "PersistenceType", 4);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: PersistenceType property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addStringProperty (handler, "PersistenceType", "invalid");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: PersistenceType property of array type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addUint16Property (handler, "PersistenceType", 2, false, true);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Owner property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addUint16Property (handler, "Owner", 1);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Handler: Owner property of array type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", 
            "localhost/CIMListener/test1");
        _addStringProperty (handler, "Owner", "arrayOwner", false, true);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  CIMXML Handler: Missing required Destination property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  CIMXML Handler: Null required Destination property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "Destination", String::EMPTY, true);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  CIMXML Handler: Required Destination property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (handler, "Name", "Handler00");
        _addUint16Property (handler, "Destination", 1);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Missing required TargetHost property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 3);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Null required TargetHost property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", String::EMPTY, true);
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 3);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Required TargetHost property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addUint16Property (handler, "TargetHost", 1);
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 3);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Missing required TargetHostFormet property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "SNMPVersion", 3);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Null required TargetHostFormet property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 0, true);
        _addUint16Property (handler, "SNMPVersion", 3);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Required TargetHostFormet property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addStringProperty (handler, "TargetHostFormat", "invalid");
        _addUint16Property (handler, "SNMPVersion", 3);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Missing required SNMPVersion property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 2);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Null required SNMPVersion property
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 0, true);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: Required SNMPVersion property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addStringProperty (handler, "SNMPVersion", "invalid");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: PortNumber property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 3);
        _addStringProperty (handler, "PortNumber", "invalid");
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: SNMPSecurityName property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 3);
        _addUint16Property (handler, "SNMPSecurityName", 1);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  SNMP Handler: SNMPEngineID property of incorrect type
    //
    try
    {
        CIMInstance handler (PEGASUS_CLASSNAME_INDHANDLER_SNMP);
        _addStringProperty (handler, "Name", "Handler00");
        _addStringProperty (handler, "TargetHost", "localhost");
        _addUint16Property (handler, "TargetHostFormat", 2);
        _addUint16Property (handler, "SNMPVersion", 3);
        _addUint16Property (handler, "SNMPEngineID", 1);
        CIMObjectPath path = client.createInstance (NAMESPACE, handler);

        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //          
    //  Handler: Attempt to delete a handler referenced by a subscription
    //           A Handler referenced by a subscription may not be deleted
    //
    try
    {
        _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
            "Handler01");
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_FAILED);
    }

    //
    //  Create filter and handler for subscription testing
    //
    String query;
    CIMObjectPath path;
    CIMInstance filter00 (PEGASUS_CLASSNAME_INDFILTER);
    query = "SELECT * FROM CIM_ProcessIndication";
    _addStringProperty (filter00, "Name", "Filter00");
    _addStringProperty (filter00, "Query", query);
    _addStringProperty (filter00, "QueryLanguage", "WQL");
    _addStringProperty (filter00, "SourceNamespace", 
        SOURCENAMESPACE.getString ());
    path = client.createInstance (NAMESPACE, filter00);

    CIMInstance handler00 (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    _addStringProperty (handler00, "Name", "Handler00");
    _addStringProperty (handler00, "Destination", "localhost/CIMListener/test0",
        false);
    path = client.createInstance (NAMESPACE, handler00);

    //
    //  Subscription: Missing required Filter key property
    //
    try
    {
        CIMInstance subscription (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        subscription.addProperty (CIMProperty (CIMName ("Handler"),
            _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"), 0, PEGASUS_CLASSNAME_INDHANDLER_CIMXML));
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Missing required Handler key property
    //
    try
    {
        CIMInstance subscription (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        subscription.addProperty (CIMProperty (CIMName ("Filter"),
            _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER,
                 "Filter00"), 0, PEGASUS_CLASSNAME_INDFILTER));
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Incorrect Host and Namespace in Filter reference
    //
    try
    {
        CIMObjectPath fPath = _buildFilterOrHandlerPath 
            (PEGASUS_CLASSNAME_INDFILTER, "Filter00");
        fPath.setHost ("somehost");
        fPath.setNameSpace (CIMNamespaceName ("root"));
        CIMInstance subscription = _buildSubscriptionInstance
            (fPath, PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_FOUND);
    }

    //
    //  Subscription: Incorrect Host and Namespace in Handler reference
    //
    try
    {
        CIMObjectPath hPath = _buildFilterOrHandlerPath 
            (PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler00");
        hPath.setHost ("somehost");
        hPath.setNameSpace (CIMNamespaceName ("root"));
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER,
                 "Filter00"), PEGASUS_CLASSNAME_INDHANDLER_CIMXML, hPath);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_FOUND);
    }

    //
    //  Subscription: Unsupported value 1 for property SubscriptionState
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "SubscriptionState", 1);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Subscription: OtherSubscriptionState property present, but 
    //                SubscriptionState value not 1
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "SubscriptionState", 2);
        _addStringProperty (subscription, "OtherSubscriptionState", 
            "another state");
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Invalid value for property SubscriptionState
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "SubscriptionState", 5);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Missing required OtherRepeatNotificationPolicy property
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "RepeatNotificationPolicy", 1);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Null required OtherRepeatNotificationPolicy property
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "RepeatNotificationPolicy", 1);
        _addStringProperty (subscription, "OtherRepeatNotificationPolicy", 
            String::EMPTY, true);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: OtherRepeatNotificationPolicy property present, but 
    //                RepeatNotificationPolicy value not 1
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "RepeatNotificationPolicy", 2);
        _addStringProperty (subscription, "OtherRepeatNotificationPolicy", 
            "another policy");
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: OtherRepeatNotificationPolicy property of incorrect type
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "RepeatNotificationPolicy", 1);
        _addUint16Property (subscription, "OtherRepeatNotificationPolicy", 1);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Invalid value for property RepeatNotificationPolicy
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "RepeatNotificationPolicy", 5);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Unsupported value 1 for property OnFatalErrorPolicy
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "OnFatalErrorPolicy", 1);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Subscription: OtherOnFatalErrorPolicy property present, but 
    //                OnFatalErrorPolicy value not 1
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "OnFatalErrorPolicy", 2);
        _addStringProperty (subscription, "OtherOnFatalErrorPolicy", 
            "another policy");
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Invalid value for property OnFatalErrorPolicy
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "OnFatalErrorPolicy", 0);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: Invalid value for property OnFatalErrorPolicy
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "OnFatalErrorPolicy", 5);
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: FailureTriggerTimeInterval property of incorrect type
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addStringProperty (subscription, "FailureTriggerTimeInterval", 
            "incorrect type");
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_INVALID_PARAMETER);
    }

    //
    //  Subscription: No providers for specified filter query
    //
    try
    {
        CIMInstance filter07 (PEGASUS_CLASSNAME_INDFILTER);
        query = "SELECT * FROM CIM_ClassIndication";
        _addStringProperty (filter07, "Name", "Filter07");
        _addStringProperty (filter07, "Query", query);
        _addStringProperty (filter07, "QueryLanguage", "WQL");
        _addStringProperty (filter07, "SourceNamespace", 
            SOURCENAMESPACE.getString ());
        path = client.createInstance (NAMESPACE, filter07);

        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter07"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        path = client.createInstance (NAMESPACE, subscription);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  if the subscription state is disabled, the create should succeed
    //
    try
    {
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter07"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint16Property (subscription, "SubscriptionState", 4);
        path = client.createInstance (NAMESPACE, subscription);
        CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
        _checkUint16Property (retrievedInstance, "SubscriptionState", 4);
        _deleteSubscriptionInstance (client, "Filter07", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler00");
    }
    catch (CIMException &)
    {
        PEGASUS_ASSERT (false);
    }
    _deleteFilterInstance (client, "Filter07");

    //
    //  Attempt to modify filter or handler instance
    //  Modification of filter or handler instances is not supported
    //
    try
    {
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDFILTER);
        _addStringProperty (modifiedInstance, "QueryLanguage", "WQL2");
        CIMObjectPath path = _buildFilterOrHandlerPath 
            (PEGASUS_CLASSNAME_INDFILTER, "Filter01");
        modifiedInstance.setPath (path);
        Array <CIMName> propertyNames;
        propertyNames.append (CIMName ("QueryLanguage"));
        CIMPropertyList properties (propertyNames);
        client.modifyInstance (NAMESPACE, modifiedInstance, false, properties);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    try
    {
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
        _addStringProperty (modifiedInstance, "Destination", 
            "localhost/CIMListener/test9");
        CIMObjectPath path = _buildFilterOrHandlerPath 
            (PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
        modifiedInstance.setPath (path);
        Array <CIMName> propertyNames;
        propertyNames.append (CIMName ("Destination"));
        CIMPropertyList properties (propertyNames);
        client.modifyInstance (NAMESPACE, modifiedInstance, false, properties);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Modify subscription: invalid value for SubscriptionState
    //
    try
    {
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        _addUint16Property (modifiedInstance, "SubscriptionState", 3);
        CIMObjectPath path = _buildSubscriptionPath ("Filter01",
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
        modifiedInstance.setPath (path);
        Array <CIMName> propertyNames;
        propertyNames.append (CIMName ("SubscriptionState"));
        CIMPropertyList properties (propertyNames);
        client.modifyInstance (NAMESPACE, modifiedInstance, false, properties);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Modify subscription: property list with more than 1 property
    //
    try
    {
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        _addUint16Property (modifiedInstance, "SubscriptionState", 2);
        CIMObjectPath path = _buildSubscriptionPath ("Filter01",
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
        modifiedInstance.setPath (path);
        Array <CIMName> propertyNames;
        propertyNames.append (CIMName ("SubscriptionState"));
        propertyNames.append (CIMName ("SubscriptionDuration"));
        CIMPropertyList properties (propertyNames);
        client.modifyInstance (NAMESPACE, modifiedInstance, false, properties);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Modify subscription: null property list (all properties)
    //
    try
    {
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        _addUint16Property (modifiedInstance, "SubscriptionState", 2);
        CIMObjectPath path = _buildSubscriptionPath ("Filter01",
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
        modifiedInstance.setPath (path);
        client.modifyInstance (NAMESPACE, modifiedInstance, false, 
            CIMPropertyList ());
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Modify subscription: includeQualifiers true
    //
    try
    {
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        _addUint16Property (modifiedInstance, "SubscriptionState", 2);
        CIMObjectPath path = _buildSubscriptionPath ("Filter01",
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
        modifiedInstance.setPath (path);
        Array <CIMName> propertyNames;
        propertyNames.append (CIMName ("SubscriptionState"));
        CIMPropertyList properties (propertyNames);
        client.modifyInstance (NAMESPACE, modifiedInstance, true, properties);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_SUPPORTED);
    }

    //
    //  Modify subscription: expired subscription
    //
    try
    {
        //
        //  Create subscription with short SubscriptionDuration
        //
        CIMInstance subscription = _buildSubscriptionInstance
            (_buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDFILTER, 
                 "Filter00"),
             PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
             _buildFilterOrHandlerPath (PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
                 "Handler00"));
        _addUint64Property (subscription, "SubscriptionDuration", 1);
        path = client.createInstance (NAMESPACE, subscription);

        //
        //  Sleep long enough for the subscription to expire
        //
        System::sleep (2);

        //
        //  Attempt to modify the expired subscription
        //
        CIMInstance modifiedInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
        _addUint16Property (modifiedInstance, "SubscriptionState", 4);
        modifiedInstance.setPath (path);
        Array <CIMName> propertyNames;
        propertyNames.append (CIMName ("SubscriptionState"));
        CIMPropertyList properties (propertyNames);
        client.modifyInstance (NAMESPACE, modifiedInstance, false, properties);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_FAILED);
    }
    
    //
    //  Verify the expired subscription has been deleted
    //
    try
    {
        CIMInstance retrievedInstance = client.getInstance (NAMESPACE, path);
    }
    catch (CIMException & e)
    {
        _checkExceptionCode (e, CIM_ERR_NOT_FOUND);
    }

    //
    //  Delete filter and handler instances
    //
    _deleteFilterInstance (client, "Filter00");
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler00");
}

void _delete (CIMClient & client)
{
    //
    //  Delete subscription instances
    //
    _deleteSubscriptionInstance (client, "Filter01", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    _deleteSubscriptionInstance (client, "Filter02", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    _deleteSubscriptionInstance (client, "Filter03", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    _deleteSubscriptionInstance (client, "Filter04", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    _deleteSubscriptionInstance (client, "Filter05", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    _deleteSubscriptionInstance (client, "Filter06", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    //_deleteSubscriptionInstance (client, "Filter01", 
        //PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler02");
    _deleteSubscriptionInstance (client, "Filter01", 
        PEGASUS_CLASSNAME_INDHANDLER_SNMP, "Handler03");

    //
    //  Delete handler instances
    //
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
        "Handler01");
    _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_SNMP, 
        "Handler03");

    //
    //  Delete filter instances
    //
    _deleteFilterInstance (client, "Filter01");
    _deleteFilterInstance (client, "Filter02");
    _deleteFilterInstance (client, "Filter03");
    _deleteFilterInstance (client, "Filter04");
#ifndef PEGASUS_DISABLE_CQL
    _deleteFilterInstance (client, "Filter04a");
#endif
    _deleteFilterInstance (client, "Filter05");
    _deleteFilterInstance (client, "Filter06");
}

void _test (CIMClient & client)
{
    try
    {
         String wql("WQL");
         String cql("CIM:CQL");

        _valid (client, wql);
        _default (client);
        _errorQueries(client, wql);
        _error (client);
        _delete (client);

#ifndef PEGASUS_DISABLE_CQL
        _valid (client, cql);
#endif
        _errorQueries(client, cql);
#ifndef PEGASUS_DISABLE_CQL
        _delete (client);
#endif
    }

    catch (Exception & e)
    {
        cerr << "test failed: " << e.getMessage () << endl;
        exit (-1);
    }

    cout << "+++++ test completed successfully" << endl;
}

void _cleanup (CIMClient & client)
{
    //
    //  Delete subscription instances
    //
    try
    {
        _deleteSubscriptionInstance (client, "Filter01", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter02", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter03", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter04", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter05", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter06", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter01", 
            PEGASUS_CLASSNAME_INDHANDLER_CIMXML, "Handler02");
    }
    catch (...)
    {
    }

    try
    {
        _deleteSubscriptionInstance (client, "Filter01", 
            PEGASUS_CLASSNAME_INDHANDLER_SNMP, "Handler03");
    }
    catch (...)
    {
    }

    //
    //  Delete handler instances
    //
    try
    {
        _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
            "Handler00");
    }
    catch (...)
    {
    }

    try
    {
        _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
            "Handler01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_CIMXML, 
            "Handler02");
    }
    catch (...)
    {
    }

    try
    {
        _deleteHandlerInstance (client, PEGASUS_CLASSNAME_INDHANDLER_SNMP, 
            "Handler03");
    }
    catch (...)
    {
    }

    //
    //  Delete filter instances
    //
    try
    {
        _deleteFilterInstance (client, "Filter00");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter01");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter02");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter03");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter04");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter04a");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter05");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter06");
    }
    catch (...)
    {
    }

    try
    {
        _deleteFilterInstance (client, "Filter07");
    }
    catch (...)
    {
    }

    //
    //  Delete provider registration instances
    //
    try
    {
        _deleteCapabilityInstance (client, 
            "AlertIndicationProviderModule", 
            "AlertIndicationProvider",
            "AlertIndicationProviderCapability");
    }
    catch (...)
    {
    }

    try
    {
        _deleteProviderInstance (client, 
            "AlertIndicationProvider", 
            "AlertIndicationProviderModule");
    }
    catch (...)
    {
    }

    try
    {
        _deleteModuleInstance (client, 
            "AlertIndicationProviderModule");
    }
    catch (...)
    {
    }

    try
    {
        _deleteCapabilityInstance (client, 
            "ProcessIndicationProviderModule", 
            "ProcessIndicationProvider",
            "ProcessIndicationProviderCapability");
    }
    catch (...)
    {
    }

    try
    {
        _deleteProviderInstance (client, 
            "ProcessIndicationProvider", 
            "ProcessIndicationProviderModule");
    }
    catch (...)
    {
    }

    try
    {
        _deleteModuleInstance (client, 
            "ProcessIndicationProviderModule");
    }
    catch (...)
    {
    }

    cout << "+++++ cleanup completed successfully" << endl;
}

void _unregister (CIMClient & client)
{
    try
    {
        _deleteCapabilityInstance (client, 
            "AlertIndicationProviderModule", 
            "AlertIndicationProvider",
            "AlertIndicationProviderCapability");
        _deleteProviderInstance (client, 
            "AlertIndicationProvider", 
            "AlertIndicationProviderModule");
        _deleteModuleInstance (client, 
            "AlertIndicationProviderModule");

        _deleteCapabilityInstance (client, 
            "ProcessIndicationProviderModule", 
            "ProcessIndicationProvider",
            "ProcessIndicationProviderCapability");
        _deleteProviderInstance (client, 
            "ProcessIndicationProvider", 
            "ProcessIndicationProviderModule");
        _deleteModuleInstance (client, 
            "ProcessIndicationProviderModule");
    }
    catch (Exception & e)
    {
        cerr << "unregister failed: " << e.getMessage ()
                           << endl;
        exit (-1);
    }

    cout << "+++++ unregister completed successfully"
                       << endl;
}

int main (int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;

    CIMClient client;
    try
    {
        client.connectLocal ();
    }
    catch (Exception & e)
    {
        cerr << e.getMessage () << endl;
        return -1;
    }

    if (argc != 2)
    {
        _usage ();
        return 1;
    }

    else
    {
        const char * opt = argv [1];

        if (String::equalNoCase (opt, "register"))
        {
            _register (client);
        }
        else if (String::equalNoCase (opt, "test"))
        {
            _test (client);
        }
        else if (String::equalNoCase (opt, "cleanup"))
        {
            _cleanup (client);
        }
        else if (String::equalNoCase (opt, "unregister"))
        {
            _unregister (client);
        }
        else
        {
            cerr << "Invalid option: " << opt 
                << endl;
            _usage ();
            return -1;
        }
    }

    return 0;
}
