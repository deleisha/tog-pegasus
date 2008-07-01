#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================

CLEAN_DIRS = $(DIRS) $(RECURSE_EXTRA_CLEAN_DIRS)

define NL


endef

##
## PEGASUS_SKIP_MOST_TEST_DIRS -- disables building of tests if "true".
##

ifdef PEGASUS_SKIP_MOST_TEST_DIRS
  ifeq ($(PEGASUS_SKIP_MOST_TEST_DIRS),false)
    DIRS += $(TEST_DIRS)
  else
    ifneq ($(PEGASUS_SKIP_MOST_TEST_DIRS),true)
      $(error PEGASUS_SKIP_MOST_TEST_DIRS ($(PEGASUS_SKIP_MOST_TEST_DIRS)) invalid, must be true or false)
    endif
  endif
else
  DIRS += $(TEST_DIRS)
endif

#
# In VMS, arguments that must be in uppercase,
#  need to be surrounded by double quotes.
#

all: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) $(NL) )

depend: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) depend $(NL) )

tests: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) tests $(NL) )

poststarttests: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) poststarttests $(NL) )

clean: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(CLEAN_DIRS), $(MAKESH) $(MAKE) "-SC" $(i) clean $(NL))

misc: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(CLEAN_DIRS), $(MAKESH) $(MAKE) "-SC" $(i) misc $(NL))

docxx: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(CLEAN_DIRS), $(MAKESH) $(MAKE) "-SC" $(i) docxx $(NL))

sub: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(CLEAN_DIRS), $(MAKESH) $(MAKE) "-SC" $(i) sub $(NL))

general: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) general OPTION=$(OPTION) $(NL))

strip-license: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) strip-license $(NL) )

prepend-license: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) prepend-license $(NL) )

#l10n
messages: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) "-SC" $(i) messages $(NL) )

