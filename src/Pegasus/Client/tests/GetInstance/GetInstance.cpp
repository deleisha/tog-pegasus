//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: GetInstance.cpp,v $
// Revision 1.3  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.2  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.1  2001/01/29 02:24:15  mike
// Added support for GetInstance.
//
// Revision 1.2  2001/01/25 02:12:05  mike
// Added meta-qualifiers to LoadRepository program.
//
// Revision 1.1.1.1  2001/01/14 19:50:33  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Client/CIMClient.h>

using namespace Pegasus;
using namespace std;

const String NAMESPACE = "root/cimv20";

int main(int argc, char** argv)
{
    try
    {
	CIMClient client;
	client.connect("localhost", 8888);

	String instanceName = "Process.pid=123456";

	CIMReference reference;
	CIMReference::instanceNameToReference(instanceName, reference);
	CIMInstance cimInstance = client.getInstance(NAMESPACE, reference);
	cimInstance.print();
    }
    catch(Exception& e)
    {
	std::cerr << "Error: " << e.getMessage() << std::endl;
	exit(1);
    }

    std::cout << "+++++ passed all tests" << std::endl;

    return 0;
}
