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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>

#include <sys/types.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>



PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

AtomicInt function_count;

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL work_func_blocking(void *parm)
{

#ifdef PEGASUS_ARCHITECTURE_IA64
   Uint32 sleep_interval = (Uint64)parm;
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
   unsigned long sleep_interval = (unsigned long)parm;
#else   
   Uint32 sleep_interval = (Uint32)parm;
#endif
   pegasus_sleep(sleep_interval);
   function_count++;
   return 0; 
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL work_func(void *parm)
{

#ifdef PEGASUS_ARCHITECTURE_IA64
   Uint32 sleep_interval = (Uint64)parm;
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
   unsigned long sleep_interval = (unsigned long)parm;
#else   
   Uint32 sleep_interval = (Uint32)parm;
#endif
   pegasus_sleep(sleep_interval);
   function_count++;
   if( ! (function_count.value() % 10) )
   {
#ifndef PEGASUS_DISABLE_KILLING_HUNG_THREADS
      while(1)
      { 
	 pegasus_sleep(1);
      }
#endif
   }
   
   return 0; 
}  

int main(int argc, char **argv) 
{ 
   
#if defined(PEGASUS_DEBUG)
   Tracer::setTraceComponents("All");
   Tracer::setTraceLevel(Tracer::LEVEL4);
   Tracer::setTraceFile("thread_pool_trace.txt");
#endif
   struct timeval await = { 0, 40000 };
   struct timeval dwait = { 1, 0 };
   struct timeval deadwait = { 0, 80000 }; 

   ThreadPool tp(0, "test pool ",  0, 6, await, dwait, deadwait);   
  
   int i ;
   
   for(i = 0 ; i < 10; i++)
   {  
      try 
      {
 	 tp.allocate_and_awaken((void *)1, work_func );
	 tp.allocate_and_awaken((void *)2, work_func );	 
	 tp.allocate_and_awaken((void *)3, work_func );
	 tp.allocate_and_awaken((void *)4 , work_func );	 
	 tp.allocate_and_awaken((void *)5, work_func );
	 tp.allocate_and_awaken((void *)6, work_func );
	 tp.allocate_and_awaken((void *)7, work_func );
	 tp.allocate_and_awaken((void *)8, work_func );
	 tp.allocate_and_awaken((void *)9, work_func );
	 tp.allocate_and_awaken((void *)1, work_func ); 
	 tp.allocate_and_awaken((void *)2, work_func );	 
	 tp.allocate_and_awaken((void *)3, work_func );
	 tp.allocate_and_awaken((void *)4 , work_func );	 
	 tp.allocate_and_awaken((void *)5, work_func );
	 tp.allocate_and_awaken((void *)6, work_func );
	 tp.allocate_and_awaken((void *)7, work_func );
	 tp.allocate_and_awaken((void *)8, work_func );
	 tp.allocate_and_awaken((void *)9, work_func );
	 tp.allocate_and_awaken((void *)1, work_func );
	 tp.allocate_and_awaken((void *)2, work_func );	 
	 tp.allocate_and_awaken((void *)3, work_func );
	 tp.allocate_and_awaken((void *)4 , work_func );	 
	 tp.allocate_and_awaken((void *)5, work_func );
	 tp.allocate_and_awaken((void *)6, work_func );
	 tp.allocate_and_awaken((void *)7, work_func );
	 tp.allocate_and_awaken((void *)8, work_func );
	 tp.allocate_and_awaken((void *)9, work_func );
	 tp.allocate_and_awaken((void *)1, work_func );
	 tp.allocate_and_awaken((void *)2, work_func );	 
	 tp.allocate_and_awaken((void *)3, work_func );
	 tp.allocate_and_awaken((void *)4 , work_func );	 
	 tp.allocate_and_awaken((void *)5, work_func );
	 tp.allocate_and_awaken((void *)6, work_func );
	 tp.allocate_and_awaken((void *)7, work_func );
	 tp.allocate_and_awaken((void *)8, work_func );
	 tp.allocate_and_awaken((void *)9, work_func );
	 tp.allocate_and_awaken((void *)1, work_func );
	 tp.allocate_and_awaken((void *)2, work_func );	 
	 tp.allocate_and_awaken((void *)3, work_func );
	 tp.allocate_and_awaken((void *)4 , work_func );	 
	 tp.allocate_and_awaken((void *)5, work_func );
	 tp.allocate_and_awaken((void *)6, work_func );
	 tp.allocate_and_awaken((void *)7, work_func );
	 tp.allocate_and_awaken((void *)8, work_func );
	 tp.allocate_and_awaken((void *)9, work_func );
	 tp.allocate_and_awaken((void *)1, work_func );
	 tp.allocate_and_awaken((void *)2, work_func );	 
	 tp.allocate_and_awaken((void *)3, work_func );
	 tp.allocate_and_awaken((void *)4 , work_func );	 
	 tp.allocate_and_awaken((void *)5, work_func );
	 tp.allocate_and_awaken((void *)6, work_func );
	 tp.allocate_and_awaken((void *)7, work_func );
	 tp.allocate_and_awaken((void *)8, work_func );
	 tp.allocate_and_awaken((void *)9, work_func );
      } 
       catch(Deadlock & )  
      {
      }
   }  
   cout << " sleeping ... " << endl;
   
   pegasus_sleep( 7000 ) ; 
   cout << " awakening" << endl;
   
   tp.kill_dead_threads( );
   tp.kill_dead_threads( );
   
   Semaphore blocking(0);
   Boolean success = true;
   
   do 
   {
      try 
      {
	 cout << " trying to allocate a new thread" << endl;
	 
	 tp.allocate_and_awaken((void *)16, work_func_blocking , &blocking);
      }
      catch(Deadlock & ) 
      { 
	 cout << " deadlock " << endl;
	 
	 success = false; 
	 pegasus_sleep(100);
      } 
   } while( success == false ); 
   
   
   
   blocking.wait();
//   tp.kill_dead_threads( ) ;

//    while(tp.running_count() )
//    {  
//       tp.kill_dead_threads();
//       pegasus_sleep(1);
//    }
   

   cout << argv[0] << " +++++ passed all tests" << endl;
  
   return(0);
   
}



