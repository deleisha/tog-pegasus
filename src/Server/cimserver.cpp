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
// Author: Mike Brasher
//
//END_HISTORY

#include <iostream>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Selector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Server/CIMServer.h>

using namespace Pegasus;
using namespace std;

const char PEGASUS_VERSION[]  = "Pegasus CIM Server - Version 0.7";

void GetEnvironmentVariables(
    const char* arg0,
    String& pegasusHome)
{
    // Get environment variables:

    const char* tmp = getenv("PEGASUS_HOME");

    if (!tmp)
    {
	cerr << arg0 << ": PEGASUS_HOME environment variable undefined" << endl;
	exit(1);
    }

    pegasusHome = tmp;
    FileSystem::translateSlashes(pegasusHome);
}

void GetOptions(
    OptionManager& om,
    int& argc, 
    char** argv, 
    const String& pegasusHome)
{
    static struct OptionRow options[] =
    {
	{"port", "8888", false, Option::WHOLE_NUMBER, 0, 0, "port"},
	{"trace", "false", false, Option::BOOLEAN, 0, 0, "trace"},
	{"version", "false", false, Option::BOOLEAN, 0, 0, "v"},
	{"help", "false", false, Option::BOOLEAN, 0, 0, "h"}
    };
    const Uint32 NUM_OPTIONS = sizeof(options) / sizeof(options[0]);

    om.registerOptions(options, NUM_OPTIONS);

    String configFile = pegasusHome + "/cimserver.conf";

    if (FileSystem::exists(configFile))
	om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();
}

void PrintHelp(const char* arg0)
{
    cout << '\n';
    cout << PEGASUS_VERSION << endl;
    cout << '\n';
    cout << "Usage: " << arg0 << " [-port <port_num> -t -h -v]\n";
    cout << '\n';
    cout << "    -h - prints this help message\n";
    cout << "    -port - specifies port number to listen on\n";
    cout << "    -v - prints out the version number\n";
    cout << "    -t - turns on trace mode\n";
    cout << endl;
}

int main(int argc, char** argv)
{
    // Get environment variables:

    String pegasusHome;
    GetEnvironmentVariables(argv[0], pegasusHome);

    // Get options (from command line and from configuration file); this 
    // removes corresponding options and their arguments fromt he command
    // line.

    OptionManager om;

    try
    {
	GetOptions(om, argc, argv, pegasusHome);
	// om.print();
    }
    catch (Exception& e)
    {
	cerr << argv[0] << ": " << e.getMessage() << endl;
	exit(1);
    }

    // At this point, all options should have been extracted; print an
    // error if there are any remaining:

    if (argc != 1)
    {
	cerr << argv[0] << ": unrecognized options: ";

	for (Uint32 i = 1; i < argc; i++)
	    cerr << argv[i] << ' ';
	cout << endl;
	exit(1);
    }

    // Check to see if user asked for the version (-v otpion):

    String versionOption;

    if (om.lookupValue("version", versionOption) && versionOption == "true")
    {
	cerr << PEGASUS_VERSION << endl;
	exit(0);
    }

    // Check to see if user asked for help (-h otpion):

    String helpOption;

    if (om.lookupValue("help", helpOption) && helpOption == "true")
    {
	PrintHelp(argv[0]);
	exit(0);
    }

    // Grab the port otpion:

    String portOption;
    om.lookupValue("port", portOption);

    try
    {
	Selector selector;
	CIMServer server(&selector, pegasusHome);

	char* address = portOption.allocateCString();
	server.bind(address);
	delete [] address;
	server.runForever();
    }
    catch(Exception& e)
    {
	std::cerr << "Error: " << e.getMessage() << std::endl;
    }

    return 0;
}
