//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/PG_Internal");


int main(int argc, char** argv)
{
    try
    {
        CIMClient client;
        client.connect("localhost", 5988, String::EMPTY, String::EMPTY);

        Array<CIMInstance> cimInstance;
        try
        {
            cimInstance = client.enumerateInstances(
                             NAMESPACE,
                             CIMName("PG_WBEMSLPTemplate"));
        }
        catch (Exception& e)
        {
            cout<<e.getMessage()<<endl;
        }

        // There should be one instance
        if (cimInstance.size() == 0)
        {
            cout << "+++++ Error: enumerateInstances on WBEMSLPTemplate"\
                   << endl;
            cout << "+++++ Test failed" << endl;
            return 1;
        }
        else
        {

            Array<CIMObjectPath> instanceNames = client.enumerateInstanceNames(
                                                  NAMESPACE,
                                                 CIMName("PG_WBEMSLPTemplate"));



            for (Uint32 i = 0; i < instanceNames.size(); i++)
            {
                CIMInstance cimInstance1 = client.getInstance(
                                              NAMESPACE,instanceNames[i]);
                Uint32 NumProperties;
                cout << "Getting all properties for WBEMSLPTemplate \
                instance ....  "<< i+1 <<"\n";

                NumProperties = cimInstance1.getPropertyCount();
                for(Uint32 j=0;j<NumProperties;j++)
                {
                    CIMProperty cimProperty=cimInstance1.getProperty(j);
                    CIMValue cimValue=cimProperty.getValue();
                    CIMName cimName=cimProperty.getName();
                    cout << cimName.getString() << " is "  << \
                                 cimValue.toString() << "\n";
                }
            }
         }
    }
    catch(Exception& e)
    {
        PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
        exit(1);
    }

     cout << "+++++ SLP Test Passed +++++"<< endl;
     return 0;
}
