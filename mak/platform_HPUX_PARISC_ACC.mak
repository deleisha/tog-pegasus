include $(ROOT)/mak/config-unix.mak

OS = hpux

ARCHITECTURE = parisc

COMPILER = acc

PLATFORM_VERSION_SUPPORTED = yes

ifeq ($(HPUX_IA64_VERSION), yes)
  SYS_INCLUDES = 
else
  SYS_INCLUDES = -I$(ROOT)/src/stdcxx/stream
endif

ifdef PEGASUS_CCOVER
 SYS_INCLUDES += -I/opt/ccover11/include
endif

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

ifdef PEGASUS_AGENT
 DEFINES += -DHPUX_EMANATE
endif

ifdef PEGASUS_CCOVER
 DEFINES += -DPEGASUS_CCOVER
endif

ifdef ENABLETIMEOUTWORKAROUNDHACK
 DEFINES += -DENABLETIMEOUTWORKAROUNDHACK
endif

ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
 DEFINES += -DPEGASUS_LOCAL_DOMAIN_SOCKET
endif

ifdef PEGASUS_PAM_AUTHENTICATION
 DEFINES += -DPEGASUS_PAM_AUTHENTICATION
endif

DEPEND_INCLUDES =


## Flags:
##     +Z - produces position independent code (PIC).
##     +DAportable generates code for any HP9000 architecture
##     -Wl, passes the following option to the linker
##       +s causes the linked image or shared lib to be able to
##          search for any referenced shared libs dynamically in
##          SHLIB_PATH (LD_LIBRARY_PATH on 64-bit HP9000)
##       +b enables dynamic search in the specified directory(ies)
##

ifeq ($(HPUX_IA64_VERSION), yes)
  DEFINES += -DPEGASUS_ARCHITECTURE_IA64
  FLAGS = +Z +DD64 -mt 
else
  FLAGS = +Z +DAportable -mt
endif

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
  FLAGS += -Wl,+s -Wl,+b/opt/wbem/lib
endif
ifdef PEGASUS_DEBUG
  FLAGS += -g
endif

SYS_LIBS = -lpthread -lrt

# PAM support
ifdef PEGASUS_PAM_AUTHENTICATION
SYS_LIBS += -lpam
endif

# SSL support
ifdef PEGASUS_HAS_SSL
 FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
 SYS_INCLUDES += -I$(OPENSSL_HOME)/include
 SYS_LIBS += -L$(OPENSSL_HOME)/lib -lcrypto -lssl
endif

CXX = aCC

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .sl
