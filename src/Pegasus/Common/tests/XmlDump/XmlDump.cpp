//BEGIN_LICENSE
//
//
// Copyright (c) 2000 The Open Group, BMC Software
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
// $Log: XmlDump.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:50  mike
// Pegasus import
//
//
//END_HISTORY

#include <fstream>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/FileSystem.h>

using namespace Pegasus;
using namespace std;

static void _processFile(const char* fileName)
{
    Array<char> text;
    FileSystem::loadFileToMemory(text, fileName);
    text.append('\0');

    XmlParser parser((char*)text.getData());
    XmlEntry entry;

    while (parser.next(entry))
	entry.print();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " xml-filename" << endl;
	exit(1);
    }

    try 
    { 
	_processFile(argv[1]); 
    }
    catch(Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;	
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
