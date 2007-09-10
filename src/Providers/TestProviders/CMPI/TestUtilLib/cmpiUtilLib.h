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
#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Providers/TestProviders/CMPI/TestUtilLib/Linkage.h>

PEGASUS_CMPI_TESTPRV_UTILLIB_LINKAGE extern const CMPIBoolean CMPI_true;
PEGASUS_CMPI_TESTPRV_UTILLIB_LINKAGE extern const CMPIBoolean CMPI_false;

CMPI_EXTERN_C void
PROV_LOG (const char *fmt, ...);

CMPI_EXTERN_C void
PROV_LOG_CLOSE ();

CMPI_EXTERN_C void
PROV_LOG_OPEN (const char *file, const char *location);

CMPI_EXTERN_C const char *
strCMPIValueState (CMPIValueState state);

CMPI_EXTERN_C const char *
strCMPIPredOp (CMPIPredOp op);

CMPI_EXTERN_C const char *
strCMPIStatus (CMPIStatus stat);

CMPI_EXTERN_C int _CMSameType( CMPIData value1, CMPIData value2 );
CMPI_EXTERN_C int _CMSameValue( CMPIData value1, CMPIData value2 );
CMPI_EXTERN_C int _CMSameObject(const CMPIObjectPath * object1,
                                const CMPIObjectPath * object2 );
CMPI_EXTERN_C int _CMSameInstance(const CMPIInstance * instance1,
                                  const CMPIInstance * instance2 );
CMPI_EXTERN_C const char * _CMPIrcName ( CMPIrc rc );
CMPI_EXTERN_C const char * _CMPITypeName (CMPIType type);
CMPI_EXTERN_C char * _CMPIValueToString (CMPIData data);

CMPI_EXTERN_C void check_CMPIStatus (CMPIStatus rc);

CMPI_EXTERN_C CMPIBoolean
evalute_selectcond (const CMPISelectCond * cond, 
                    CMPIAccessor *accessor,
                    void *parm);

CMPI_EXTERN_C CMPIBoolean
evaluate(const CMPISelectExp *se, 
         const CMPIInstance *inst,
         CMPIAccessor *inst_accessor,
         void *parm );

CMPI_EXTERN_C CMPIData
instance_accessor (const char *name, void *param);

