//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


// A main for the cimmof_parser.  It can be embedded elsewhere, too. 

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Compiler/mofCompilerOptions.h>
#include "cmdlineExceptions.h"
#include <Pegasus/Compiler/cimmofParser.h>
#include <Pegasus/Compiler/parserExceptions.h>

#include <Pegasus/Common/PegasusVersion.h>

#ifdef PEGASUS_OS_OS400
#include "qycmutiltyUtility.H"
#endif

PEGASUS_USING_STD;

#ifdef PEGASUS_HAVE_NAMESPACES
using namespace ParserExceptions;
#endif

// This is used by the parsing routines to control flow
// through include files
static mofCompilerOptions cmdline;


extern "C++" int processCmdLine(int, char **, mofCompilerOptions &, ostream &);

#define NAMESPACE_ROOT "root/cimv2"

int
main(int argc, char ** argv) {
  int ret = 0;

#ifdef PEGASUS_OS_OS400
  if(FALSE == ycmCheckCmdAuthorities())
  { 
    return -9;
  }
#endif

  try {
    ret = processCmdLine(argc, argv, cmdline, cerr);
  } catch (ArgumentErrorsException &e) {
    cerr << e.getMessage() << endl;
    ret =  -2;
  } catch (CmdlineNoRepository &e) {
    cerr << e.getMessage() << endl;
    ret = -3;
  } catch (CIMException &e) {
    cerr << "Unexpected condition: " << e.getMessage() << endl;
    ret = -4;
  }
  if (ret) {
    if (ret > 0) {
      cerr << "Unexpected result from processing command line: " << ret <<endl;
      cerr << "Compilation terminating." << endl;
    }
    return ret;
  }
  const Array<String>& filespecs = cmdline.get_filespec_list();

  // For most options, a real repository is required.  If we can't
  // create one and we need to, bail. 
  cimmofParser *p = cimmofParser::Instance(); 
  p->setCompilerOptions(&cmdline);
  if ( p->setRepository() ) {	
    p->setDefaultNamespacePath(NAMESPACE_ROOT);
  } else {
    cerr << "Failed to set DefaultNamespacePath." << endl;
    // ATTN: P3 BB 2001 Did not set namespace.  We may need to log an error here.
    return ret;
  }
  if (filespecs.size())    // user specified command line args
    for (unsigned int i = 0; i < filespecs.size(); i++) {
      if (p->setInputBufferFromName((const String &)filespecs[i]) == 0) {
	try {
	  ret = p->parse();
	} catch(ParserLexException &e) {
	  cerr << "Lexer error: " << e.getMessage() << endl;
	} catch(Exception &e) {
	  cerr << "Parsing error: " << e.getMessage() << endl;
	}
      } else {
        cerr << "Can't open file " << filespecs[i] << endl;
      }
    }
  else {
    try {
    int ret =  p->parse();
    } catch(ParserLexException &e) {
      cerr << "Lexer error: " << e.getMessage() << endl;
    } catch(Exception &e) {
      cerr << "Compiler general exception: " << e.getMessage() << endl;
    }
  }
  
  return ret;
}
