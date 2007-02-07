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
// Author:      Adrian Duta
//
// Modified By: Mark Hamzy, hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi;

public class CIMMethod
{
   private long cInst;

   private native int    _getType           (long cInst);
   private native String _getName           (long cInst);
   private native int    _findParameter     (long cInst, String name);
   private native long   _getParameter      (long cInst, int    iParameter);
   private native int    _getParameterCount (long cInst);
   private native void   _finalize          (long cInst);

   protected void finalize ()
   {
      _finalize (cInst);
   }

   protected long cInst ()
   {
      return cInst;
   }

   public CIMMethod (long ci)
   {
      cInst = ci;
   }

   public int getType ()
   {
      if (cInst != 0)
      {
         return _getType (cInst);
      }
      else
      {
         return 0;
      }
   }

   public String getName ()
   {
      if (cInst != 0)
      {
         return _getName (cInst);
      }
      else
      {
         return null;
      }
   }

   public int findParameter (String name)
   {
      if (cInst != 0)
      {
         return _findParameter (cInst, name);
      }
      else
      {
         return -1;
      }
   }

   public CIMParameter getParameter (int iParameter)
   {
      CIMParameter ret = null;

      if (cInst != 0)
      {
         long ciParameter = _getParameter (cInst, iParameter);

         if (ciParameter != 0)
         {
            ret = new CIMParameter (ciParameter);
         }
      }

      return ret;
   }

   public int getParameterCount ()
   {
      if (cInst != 0)
      {
         return _getParameterCount (cInst);
      }
      else
      {
         return 0;
      }
   }

   public String toString ()
   {
      return "@ CIMMethod.toString() not implemented yet!";
   }

   static {
      System.loadLibrary ("JMPIProviderManager");
   }
}
