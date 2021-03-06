#//%LICENSE////////////////////////////////////////////////////////////////
#//
#// Licensed to The Open Group (TOG) under one or more contributor license
#// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
#// this work for additional information regarding copyright ownership.
#// Each contributor licenses this file to you under the OpenPegasus Open
#// Source License; you may not use this file except in compliance with the
#// License.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a
#// copy of this software and associated documentation files (the "Software"),
#// to deal in the Software without restriction, including without limitation
#// the rights to use, copy, modify, merge, publish, distribute, sublicense,
#// and/or sell copies of the Software, and to permit persons to whom the
#// Software is furnished to do so, subject to the following conditions:
#//
#// The above copyright notice and this permission notice shall be included
#// in all copies or substantial portions of the Software.
#//
#// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//////////////////////////////////////////////////////////////////////////
ROOT = ../../../../..

DIR = Pegasus/HandlerService/tests/ReliableIndicationDisableEnable

include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     peglistener \
     pegclient \
     pegexportserver \
     peggeneral \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS),VMS)
 LOCAL_DEFINES = -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)"""
else
 LOCAL_DEFINES = -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"
endif

LOCAL_DEFINES += -DPEGASUS_INTERNALONLY

PROGRAM = ReliableIndicationDisableEnableTest
PROGRAM2 = $(PROGRAM) IndicationStressTestClass test/TestProvider

SOURCES = ReliableIndicationDisableEnableTest.cpp

include $(ROOT)/mak/program.mak

include $(ROOT)/mak/test.mak


testreliablehttp:
	$(PROGRAM2) setupCL WQL HTTP
	$(PROGRAM2) run 1 1 
	@$(ECHO) "Disabling the reliable indiaction ...."
	@cimconfig -s maxIndicationDeliveryRetryAttempts=0 
	$(PROGRAM2) run 1 1
	$(PROGRAM2) cleanup
	@$(ECHO) "Set the Defult value  maxIndicationDeliveryRetryAttempts=3 ...."
	@cimconfig -s maxIndicationDeliveryRetryAttempts=3	

testreliablehttps:
ifdef PEGASUS_HAS_SSL
	$(PROGRAM2) setupCL WQL HTTPS
	$(PROGRAM2) run 1 1
	@$(ECHO) "Disabling the reliable indiaction ...."
	@cimconfig -s maxIndicationDeliveryRetryAttempts=0                                 
	$(PROGRAM2) run 1 1
	$(PROGRAM2) cleanup
	@$(ECHO) "Set the Defult value  maxIndicationDeliveryRetryAttempts=3 ...."
	@cimconfig -s maxIndicationDeliveryRetryAttempts=3
	
endif

tests:
poststarttests:
