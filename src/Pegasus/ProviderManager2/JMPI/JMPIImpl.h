//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef _Provider_JMPIImpl_h
#define _Provider_JMPIImpl_h

#include <jni.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>

PEGASUS_NAMESPACE_BEGIN

#define NULL_CHECK0(e) if ((e) == 0) return 0 //added by Andy
#define NULL_CHECK(e) if ((e) == 0) return //added by Andy

#define Catch(jEnv) \
   catch(CIMException & e) { \
      JMPIjvm::cacheIDs(jEnv); \
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[33],(jint)e.getCode(),jEnv->NewStringUTF(e.getMessage().getCString())); \
      jEnv->Throw((jthrowable)ev); \
   } \
   catch(Exception & e) { \
      JMPIjvm::cacheIDs(jEnv);\
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[33],(jint)1,jEnv->NewStringUTF(e.getMessage().getCString())); \
      jEnv->Throw((jthrowable)ev); \
   } \
   catch(...)  { \
      JMPIjvm::cacheIDs(jEnv); \
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[33],(jint)1,"Exception: Unknown"); \
      jEnv->Throw((jthrowable)ev); \
   }

typedef struct jvmVector {
   int       initRc; 
   JavaVM    *jvm;
   JNIEnv    *env;
   jclass    *classRefs;
   jmethodID *instMethodIDs;
   jmethodID *classMethodIDs;
} JvmVector;

class JMPIjvm {
 public:
   static int trace;
   static JavaVM *jvm;
   static JvmVector jv;
   JMPIjvm();
  ~JMPIjvm();

   static JNIEnv* attachThread(JvmVector **jvp);
   static void detachThread();
   static jobject getProvider(JNIEnv *env, const char *cn, jclass *cls) ;
   static jobject getProvider(JNIEnv *env, String jar, String cls, const char *cn, jclass *cls) ;
   static void checkException(JNIEnv *env);
   static jstring NewPlatformString(JNIEnv *env,char *s);
   static jobjectArray NewPlatformStringArray(JNIEnv *env,char **strv, int strc);
   static int cacheIDs(JNIEnv *env);
 private:
   static jclass getGlobalClassRef(JNIEnv *env, const char* name);
   static int initJVM();
};

class _nameSpace {
 public:
   _nameSpace();
   _nameSpace(String hn);
   _nameSpace(String hn, String ns);
   
   int port();
   String hostName();
   String nameSpace();
   int port_;
   String protocol_;
   String hostName_;
   String nameSpace_;
};

static int pTypeToJType[]=
 /* CIMTYPE_BOOLEAN,   public static final int BOOLEAN  = 10;
    CIMTYPE_UINT8,     public static final int UINT8    = 1;
    CIMTYPE_SINT8,     public static final int SINT8    = 2;
    CIMTYPE_UINT16,    public static final int UINT16   = 3;
    CIMTYPE_SINT16,    public static final int SINT16   = 4;
    CIMTYPE_UINT32,    public static final int UINT32   = 5;
    CIMTYPE_SINT32,    public static final int SINT32   = 6;
    CIMTYPE_UINT64,    public static final int UINT64   = 7;
    CIMTYPE_SINT64,    public static final int SINT64   = 8;
    CIMTYPE_REAL32,    public static final int REAL32   = 11;
    CIMTYPE_REAL64,    public static final int REAL64   = 12;
    CIMTYPE_CHAR16,    public static final int CHAR16   = 14;
    CIMTYPE_STRING,    public static final int STRING   = 9;
    CIMTYPE_DATETIME,  public static final int DATETIME = 13;
    CIMTYPE_REFERENCE  public static final int REFERENCE = 0x32+1;
*/
   {10,1,2,3,4,5,6,7,8,11,12,14,9,13,0x32+1};

   static char *jTypeToChars[]= {
        NULL,
        "uint8",
        "sint8",
        "uint16",
        "sint16",
        "uint32",
        "sint32",
        "uint64",
        "sint64",
        "string",
        "boolean",
        "real32",
        "real64",
        "datetime",
        "char16" };


class _dataType {
  public:
   int     _type,_size;
   Boolean _reference;
   Boolean _null;
   Boolean _array;
   String _refClass;
   Boolean _fromProperty;

   _dataType(int type, int size, Boolean reference,
             Boolean null, Boolean array, String &refClass, Boolean fromProperty) :
      _type(type), _size(size), _reference(reference), _array(array),
      _refClass(refClass), _fromProperty(fromProperty) {}
   _dataType(int type) :
             _type(type), _size(1), _reference(false), _array(false),
       _refClass(String::EMPTY), _fromProperty(false) {}
   _dataType(int type, int size) :
       _type(type), _size(size), _reference(false), _array(true),
       _refClass(String::EMPTY), _fromProperty(true) {}
   _dataType(int type, const String ref) :
     _type(type), _size(0), _reference(true), _array(false),
     _refClass(ref), _fromProperty(true) {}
};

#define VectorClassRef 		classRefs[0]
#define CIMOMHandleClassRef 	classRefs[16]
#define CIMExceptionClassRef 	classRefs[13]
#define CIMClassClassRef 	classRefs[17]
#define CIMInstanceClassRef 	classRefs[18]
#define CIMObjectPathClassRef 	classRefs[12]
#define CIMPropertyClassRef 	classRefs[15]
#define CIMValueClassRef 	classRefs[19]
#define CIMObjectClassRef 	classRefs[20]
#define ThrowableClassRef 	classRefs[21]

#define VectorNew 		instMethodIDs[0]
#define VectorAddElement 	instMethodIDs[15]
#define VectorElementAt 	instMethodIDs[16]
#define VectorSize	 	instMethodIDs[28]
#define CIMOMHandleNewI		instMethodIDs[17]
#define CIMExceptionNewS	instMethodIDs[18]
#define CIMClassNewI		instMethodIDs[19]
#define CIMInstanceNewI		instMethodIDs[20]
#define CIMObjectPathNewI	instMethodIDs[21]
#define CIMObjectPathCInst	instMethodIDs[22]
#define CIMInstanceCInst	instMethodIDs[23]
#define ObjectToString		instMethodIDs[25]
#define ThrowableGetMessage	instMethodIDs[26]
#define CIMExceptionGetID	instMethodIDs[27]
#define CIMExceptionGetCode     instMethodIDs[34]
#define PropertyCInst   	instMethodIDs[29]
#define VectorRemoveElementAt   instMethodIDs[31]
#define CIMPropertyNewI		instMethodIDs[14]
#define CIMValueCInst		instMethodIDs[32]

//extern "C" JNIEnv* attachThread(JvmVector**);
//extern "C" void detachThread();
//extern "C" jobject getProvider(JNIEnv*,const char*,jclass*);
//extern "C" void checkException(JNIEnv *env);

typedef JNIEnv* (*JvmAttach)(JvmVector**);
typedef void (*JvmDetach)();

PEGASUS_NAMESPACE_END

#endif

