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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IPInfo.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMName CLASS_NAME = CIMName("PG_RemoteServiceAccessPoint");
static const CIMNamespaceName NAMESPACE = CIMNamespaceName("root/cimv2");
static const CIMName PROPERTY_ACCESS_INFO = CIMName("AccessInfo");
static const CIMName PROPERTY_INFO_FORMAT = CIMName("InfoFormat");
static const CIMName PROPERTY_OTHER_INFO_FORMAT_DESCRIPTION = CIMName(
    "OtherInfoFormatDescription");

#define HeaderFormat "%-31s %-10s  %-38s"


////////////////////////////////////////////////////////////////////////////////
//  Constructor for Remote Service Access Point  Info
////////////////////////////////////////////////////////////////////////////////
RSApInfo::RSApInfo(
    CIMClient &client, 
    Boolean enableDebug,
    ostream& outPrintWriter, 
    ostream& errPrintWriter)
{
    _enableDebug = enableDebug;

    try
    {
        Boolean deepInheritance = true;
        Boolean localOnly = true;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;
          
        Array<CIMInstance> cimInstances = client.enumerateInstances(
            NAMESPACE, 
            CLASS_NAME,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin);
 
        Uint32 numberInstances = cimInstances.size();

        if (_enableDebug)
        {
            outPrintWriter << numberInstances << " instances of " <<
                CLASS_NAME.getString() << endl;
        }

        if (numberInstances > 0)
        {
            _gatherProperties(cimInstances[0]);
            _outputHeader(outPrintWriter);

            for (Uint32 i = 0; i < numberInstances; i++)
            {
                _gatherProperties(cimInstances[i]);
                _outputInstance(outPrintWriter);

            }   // end for looping through instances.
        }
        else
        {
            outPrintWriter << "No instances of class " 
                << CLASS_NAME.getString() << endl;
        }

    }  // end try .
    catch(Exception&)
    {
        errPrintWriter << "Error getting instances of class " <<
            CLASS_NAME.getString() << endl;
    }

}

////////////////////////////////////////////////////////////////////////////////
//  Destructor for Remote Service Access Point Info
////////////////////////////////////////////////////////////////////////////////
RSApInfo::~RSApInfo(void)
{
}

////////////////////////////////////////////////////////////////////////////////
//  Gather Properities for Remote Service Access Point Info
////////////////////////////////////////////////////////////////////////////////
void RSApInfo::_gatherProperties(CIMInstance &inst)
{
    _accessInfo = String::EMPTY;

    _infoFormat = 1;
    _otherInfoFmtDesc = String("Not Available");

    for (Uint32 j=0; j < inst.getPropertyCount(); j++)
    {
        CIMName propertyName = inst.getProperty(j).getName();

        // Properties that are also keys
        if (propertyName.equal(PROPERTY_ACCESS_INFO))
        {
            inst.getProperty(j).getValue().get(_accessInfo);
        }
        // Other properties
        else if (propertyName.equal(PROPERTY_INFO_FORMAT))
        {
            inst.getProperty(j).getValue().get(_infoFormat); 
        }
        else if (propertyName.equal(PROPERTY_OTHER_INFO_FORMAT_DESCRIPTION))
        {
            _otherInfoFmtDesc.clear();
            inst.getProperty(j).getValue().get(_otherInfoFmtDesc); 
        }
   } // end for loop through properties

}


////////////////////////////////////////////////////////////////////////////////
//  Header Section for Remote Service Access Point Info
////////////////////////////////////////////////////////////////////////////////
void RSApInfo::_outputHeader(ostream &outPrintWriter)
{

    outPrintWriter << endl 
        << ">>>> Remote Service Access Point Information <<<<" 
        << endl << endl;

    char header[81];

    sprintf(
        header, 
        HeaderFormat, 
        "AccessInfo", 
        "InfoFormat", 
        "OtherInfoFormatDescription");

    outPrintWriter << endl << header << endl;
    
}

////////////////////////////////////////////////////////////////////////////////
//  Output an instance of an Remote Service Access Point Info
////////////////////////////////////////////////////////////////////////////////
void RSApInfo::_outputInstance(ostream &outPrintWriter)
{
    char row[81];

    if (_accessInfo.size() > 31)
    {
        sprintf(
            row, 
            HeaderFormat, 
            (const char *)_accessInfo.getCString(),
            "",
            "");

        outPrintWriter << row << endl;
        _accessInfo.clear();
    }

    char _if[10];
    sprintf(_if,"%d",_infoFormat);

    sprintf(
        row,
        HeaderFormat,
        (const char *)_accessInfo.getCString(),
        _if,
        (const char *)_otherInfoFmtDesc.getCString());

    outPrintWriter << row << endl << endl; 

}
