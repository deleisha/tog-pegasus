#%2005////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
# Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
# Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
# IBM Corp.; EMC Corporation, The Open Group.
# Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
# IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
# Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
# EMC Corporation; VERITAS Software Corporation; The Open Group.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
# ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
# "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#==============================================================================
#%/////////////////////////////////////////////////////////////////////////////
#
# tog-pegasus.spec
#
#%/////////////////////////////////////////////////////////////////////////////

#
# WARNING: This file is automatically generated by executing 
#   make -f pegasus/Makefile.Release create_OpenPegasusRPMSpecFile
# Any changes made directly to this file will be lost the next 
# time this file is generated.
#

%define srcRelease 1
Version: 2.5Alpha1
Release: 1

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-intro.spec
Summary: OpenPegasus WBEM Services for Linux
Name: tog-pegasus
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/tog-pegasus-%{version}-%{srcRelease}.tar.gz
Requires: openssl >= 0.9.6 lsb >= 1.3
Provides: tog-pegasus-cimserver
BuildRequires: openssl-devel >= 0.9.6

%description
OpenPegasus WBEM Services for Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-intro.spec

%global OPENSSL_HOME /usr/include/openssl
%global OPENSSL_BIN /usr/bin
%global PEGASUS_PEM_DIR /etc/opt/tog-pegasus
%global PEGASUS_SSL_CERT_FILE server.pem
%global PEGASUS_SSL_KEY_FILE file.pem
%global PEGASUS_SSL_TRUSTSTORE client.pem
%global PAM_CONFIG_DIR /etc/pam.d
%global PEGASUS_CONFIG_DIR /etc/opt/tog-pegasus
%global PEGASUS_REPOSITORY_DIR /var/opt/tog-pegasus/repository
%global PEGASUS_PREV_REPOSITORY_DIR /var/opt/tog-pegasus/prev_repository
%global PEGASUS_SBIN_DIR /opt/tog-pegasus/sbin

%global PEGASUS_RPM_ROOT $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
%global PEGASUS_RPM_HOME $RPM_BUILD_ROOT/build/tog-pegasus
%global PEGASUS_INSTALL_LOG /var/opt/tog-pegasus/log/install.log

%ifarch ia64 x86_64
%global PEGASUS_HARDWARE_PLATFORM LINUX_IA64_GNU
%else
%ifarch ppc
%global PEGASUS_HARDWARE_PLATFORM LINUX_PPC_GNU
%else
%ifarch ppc64
%global PEGASUS_HARDWARE_PLATFORM LINUX_PPC64_GNU
%else
%ifarch s390
%global PEGASUS_HARDWARE_PLATFORM LINUX_ZSERIES_GNU
%else
%ifarch s390x zseries
%global PEGASUS_HARDWARE_PLATFORM LINUX_ZSERIES64_GNU
%else
%global PEGASUS_HARDWARE_PLATFORM LINUX_IX86_GNU
%endif
%endif
%endif
%endif
%endif

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-packages.spec
%package sdk
Summary: The OpenPegasus Software Development Kit
Group: Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= %{version}

%description sdk
The OpenPegasus WBEM Services for Linux SDK is the developer's kit for the OpenPegasus WBEM
Services for Linux release. It provides Linux C++ developers with the WBEM files required to
build WBEM Clients and Providers. It also supports C provider developers via the CMPI interface.

%package test
Summary: The OpenPegasus Tests
Group: Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= %{version} 

%description test
The OpenPegasus WBEM tests for the OpenPegasus %{version} Linux rpm.
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-packages.spec

%prep
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%setup -n %{name}-%{version}
export PEGASUS_ROOT=%PEGASUS_RPM_ROOT

%build

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-build.spec
export PEGASUS_ROOT=%PEGASUS_RPM_ROOT
export PEGASUS_HOME=%PEGASUS_RPM_HOME
export PEGASUS_PLATFORM=%PEGASUS_HARDWARE_PLATFORM
export PEGASUS_ENVVAR_FILE=$PEGASUS_ROOT/env_var_Linux.status

export OPENSSL_HOME=%OPENSSL_HOME
export OPENSSL_BIN=%OPENSSL_BIN
export LD_LIBRARY_PATH=$PEGASUS_HOME/lib
export PATH=$PEGASUS_HOME/bin:$PATH

export PEGASUS_EXTRA_C_FLAGS="-g $RPM_OPT_FLAGS"
export PEGASUS_EXTRA_CXX_FLAGS="$PEGASUS_EXTRA_C_FLAGS"

make -f $PEGASUS_ROOT/Makefile.Release create_ProductVersionFile
make -f $PEGASUS_ROOT/Makefile.Release all
make -f $PEGASUS_ROOT/Makefile.Release repository
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-build.spec

%install

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-install.spec
export PEGASUS_ROOT=%PEGASUS_RPM_ROOT
export PEGASUS_HOME=%PEGASUS_RPM_HOME
export PEGASUS_PLATFORM=%PEGASUS_HARDWARE_PLATFORM
export PEGASUS_ENVVAR_FILE=$PEGASUS_ROOT/env_var_Linux.status

export OPENSSL_BIN=%OPENSSL_BIN
export LD_LIBRARY_PATH=$PEGASUS_HOME/lib
export PATH=$PEGASUS_HOME/bin:$PATH

export PEGASUS_STAGING_DIR=$RPM_BUILD_ROOT

make -f $PEGASUS_ROOT/Makefile.Release stage PEGASUS_STAGING_DIR=$PEGASUS_STAGING_DIR
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-install.spec
[ "$PEGASUS_STAGING_DIR" != "/" ] && [ -d $PEGASUS_STAGING_DIR ] &&  [ -d $PEGASUS_STAGING_DIR%PEGASUS_PEM_DIR ] &&  rm -f $PEGASUS_STAGING_DIR/%PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE  $PEGASUS_STAGING_DIR/%PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE  $PEGASUS_STAGING_DIR/%PEGASUS_PEM_DIR/%PEGASUS_SSL_KEY_FILE  $PEGASUS_STAGING_DIR/%PEGASUS_CONFIG_DIR/ssl.cnf;
[ "$PEGASUS_HOME" != "/" ] && [ -d $PEGASUS_HOME ] && rm -rf $PEGASUS_HOME;

%clean

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-clean.spec
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-clean.spec

%pre

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-pre.spec
if [ -d %PEGASUS_REPOSITORY_DIR"/root#PG_Internal" ]
then
  #
  # Save the current repository to prev_repository
  #
  if [[ -d %PEGASUS_REPOSITORY_DIR ]]
  then
    if [[ -d %PEGASUS_PREV_REPOSITORY_DIR ]]
    then
      rm -rf %PEGASUS_PREV_REPOSITORY_DIR
    fi
    mv %PEGASUS_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR
    mkdir %PEGASUS_REPOSITORY_DIR
  fi
fi
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-pre.spec

%post
if [ $1 -eq 1 ]; then
   echo `date` > %PEGASUS_INSTALL_LOG 2>&1
   %define PEGASUS_CONFIG_DIR /etc/opt/tog-pegasus
   %define PEGASUS_PEM_DIR /etc/opt/tog-pegasus
   %define PEGASUS_SSL_CERT_FILE server.pem
   %define PEGASUS_SSL_KEY_FILE file.pem
   %define PEGASUS_SSL_TRUSTSTORE client.pem

   # Create Symbolic Links for SDK Libraries
   #
   ln -sf libpegclient.so.1 /opt/tog-pegasus/lib/libpegclient.so
   ln -sf libpegcommon.so.1 /opt/tog-pegasus/lib/libpegcommon.so
   ln -sf libpegprovider.so.1 /opt/tog-pegasus/lib/libpegprovider.so
   ln -sf libDefaultProviderManager.so.1 /opt/tog-pegasus/lib/libDefaultProviderManager.so
   ln -sf libCIMxmlIndicationHandler.so.1 /opt/tog-pegasus/lib/libCIMxmlIndicationHandler.so
   ln -sf libCMPIProviderManager.so.1 /opt/tog-pegasus/lib/libCMPIProviderManager.so

   # Create Symbolic Links for Packaged Provider Libraries
   #
   ln -sf libComputerSystemProvider.so.1 /opt/tog-pegasus/providers/lib/libComputerSystemProvider.so
   ln -sf libOSProvider.so.1 /opt/tog-pegasus/providers/lib/libOSProvider.so
   ln -sf libProcessProvider.so.1 /opt/tog-pegasus/providers/lib/libProcessProvider.so


# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec
   
   # Create OpenSSL Configuration File (ssl.cnf)
   #
   if [ -f %PEGASUS_CONFIG_DIR/ssl.cnf ]
   then
      echo "WARNING: %PEGASUS_CONFIG_DIR/ssl.cnf already exists."
   else
      echo " Generating SSL Certificate..."
      echo "[ req ]" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "distinguished_name     = req_distinguished_name" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "prompt                 = no" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "[ req_distinguished_name ]" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "C                      = UK" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "ST                     = Berkshire" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "L                      = Reading" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "O                      = The Open Group" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "OU                     = The OpenPegasus Project" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      echo "CN                     = `host \`hostname\`|cut -d\" \" -f1`" >> %PEGASUS_CONFIG_DIR/ssl.cnf
      chmod 400 %PEGASUS_CONFIG_DIR/ssl.cnf
      chown root %PEGASUS_CONFIG_DIR/ssl.cnf
   fi

   %OPENSSL_BIN/openssl req -x509 -days 3650 -newkey rsa:2048 \
      -nodes -config %PEGASUS_CONFIG_DIR/ssl.cnf \
      -keyout %PEGASUS_PEM_DIR/key.pem -out %PEGASUS_PEM_DIR/cert.pem \
           2>>%PEGASUS_INSTALL_LOG

   cat %PEGASUS_PEM_DIR/key.pem > %PEGASUS_PEM_DIR/key-2048.pem
   chmod 400 %PEGASUS_PEM_DIR/key-2048.pem
   cat %PEGASUS_PEM_DIR/cert.pem > %PEGASUS_PEM_DIR/cert-2048.pem
   chmod 400 %PEGASUS_PEM_DIR/cert-2048.pem
   cat %PEGASUS_PEM_DIR/cert.pem > %PEGASUS_PEM_DIR/truststore-2048.pem
   chmod 400 %PEGASUS_PEM_DIR/truststore-2048.pem
   rm -f %PEGASUS_PEM_DIR/key.pem %PEGASUS_PEM_DIR/cert.pem

   if [ -f %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE ]
   then
   echo "WARNING: %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE SSL Certificate file already exists."
   else
      cp %PEGASUS_PEM_DIR/cert-2048.pem %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE
      chmod 444 %PEGASUS_PEM_DIR/%PEGASUS_SSL_CERT_FILE
      cp %PEGASUS_PEM_DIR/key-2048.pem %PEGASUS_PEM_DIR/%PEGASUS_SSL_KEY_FILE
      chmod 400 %PEGASUS_PEM_DIR/%PEGASUS_SSL_KEY_FILE
   fi

   if [ -f %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE ]
   then
       echo "WARNING: %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE SSL Certificate trust store already exists."
   else
       cp %PEGASUS_PEM_DIR/truststore-2048.pem %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE
       chmod 444 %PEGASUS_PEM_DIR/%PEGASUS_SSL_TRUSTSTORE
   fi

   echo " To start Pegasus manually:"
   echo " /etc/init.d/tog-pegasus start"
   echo " Stop it:"
   echo " /etc/init.d/tog-pegasus stop"
   echo " To set up PATH and MANPATH in /etc/profile"
   echo " run /opt/tog-pegasus/sbin/settogpath."
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec

fi
%preun
if [ $1 -eq 0 ]; then
   # Check if the cimserver is running
   isRunning=`ps -el | grep cimserver | grep -v "grep cimserver"`
   if [ "$isRunning" ]; then
      /opt/tog-pegasus/sbin/cimserver -s
   fi
   [ -f /var/opt/tog-pegasus/cimserver_current.conf ] &&  rm /var/opt/tog-pegasus/cimserver_current.conf;
   [ -f %PEGASUS_INSTALL_LOG ] && rm %PEGASUS_INSTALL_LOG;
   # Delete the Link to the rc.* Startup Directories
   /usr/lib/lsb/remove_initd /etc/init.d/tog-pegasus;
fi

%postun
if [ $1 -eq 0 ]; then
   [ -f %PEGASUS_PEM_DIR/key-2048.pem ] && rm %PEGASUS_PEM_DIR/key-2048.pem;
   [ -f %PEGASUS_PEM_DIR/cert-2048.pem ] && rm %PEGASUS_PEM_DIR/cert-2048.pem;
   [ -f %PEGASUS_PEM_DIR/truststore-2048.pem ] && rm %PEGASUS_PEM_DIR/truststore-2048.pem;
   export LC_ALL=C
fi

%files
%dir %attr(555,root,root) /opt/tog-pegasus
%dir %attr(555,root,root) /opt/tog-pegasus/share
%dir %attr(555,root,root) /opt/tog-pegasus/share/man
%dir %attr(555,root,root) /opt/tog-pegasus/share/man/man1
%dir %attr(555,root,root) /opt/tog-pegasus/share/man/man8
%dir %attr(555,root,root) /opt/tog-pegasus/lib
%dir %attr(555,root,root) /opt/tog-pegasus/providers
%dir %attr(555,root,root) /opt/tog-pegasus/providers/lib
%dir %attr(555,root,root) /opt/tog-pegasus/sbin
%dir %attr(555,root,root) /opt/tog-pegasus/bin
%dir %attr(555,root,root) /opt/tog-pegasus/mof
%dir %attr(555,root,root) /var/opt/tog-pegasus
%dir %attr(555,root,root) /var/opt/tog-pegasus/cache
%dir %attr(555,root,root) /var/opt/tog-pegasus/log
%dir %attr(555,root,root) /var/opt/tog-pegasus/cache/localauth
%dir %attr(555,root,root) /var/run/tog-pegasus
%dir %attr(555,root,root) /etc/opt/tog-pegasus
%dir %attr(1555,root,root) /var/run/tog-pegasus/socket
%dir %attr(555,root,root) /opt/tog-pegasus/mof/CIM28

%dir %attr(555,root,root) /opt/tog-pegasus/mof/Pegasus

/var/opt/tog-pegasus/repository

%doc %attr(444,root,root) /opt/tog-pegasus/license.txt
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man1/cimmof.1
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man1/cimprovider.1
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man1/osinfo.1
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man1/wbemexec.1
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man8/cimauth.8
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man8/cimconfig.8
%doc %attr(444,root,root) /opt/tog-pegasus/share/man/man8/cimserver.8

%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_AppRuntime.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_BIOS.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_CheckAction.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_DeploymentModel.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_InstalledProduct.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_J2eeAppServer.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_J2eeAppServerStats.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Application28_SystemSoftware.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Application28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Core28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Database28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Device28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Event28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Interop28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_IPsecPolicy28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Metrics28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Network28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Physical28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Policy28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Schema28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_Support28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_System28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/CIM_User28.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Capabilities.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Collection.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_CoreElements.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Device.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_MethodParms.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Physical.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_PowerMgmt.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_ProductFRU.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Qualifiers.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Redundancy.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Service.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Settings.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Software.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_Statistics.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Core28_StorageExtent.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Database28_Statistics.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Database28_Storage.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Database28_System.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Controller.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_CoolingAndPower.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_DeviceElements.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_DiskGroup.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_FC.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_IB.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Memory.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Modems.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_NetworkAdapter.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Ports.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Printing.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Processor.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_ProtocolController.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_SccExtents.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_Sensors.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_StorageDevices.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_StorageExtents.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_StorageLib.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_StorageServices.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_USB.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Device28_UserDevices.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Metrics28_BaseMetric.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Metrics28_UnitOfWork.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_BGP.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_Buffers.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_Collections.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_Filtering.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_IPsec.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_OSPF.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_Pipes.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_ProtocolEndpoints.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_QoS.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_Routes.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_RoutingForwarding.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_SNMP.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_SwitchingBridging.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_Systems.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Network28_VLAN.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_Component.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_Connector.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_Link.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_Misc.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_Package.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_PhysicalMedia.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/Physical28_StoragePackage.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_Boot.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_Diagnostics.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_FileElements.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_Logs.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_OperatingSystem.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_Processing.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_SystemElements.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_SystemResources.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_Time.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/System28_Unix.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_AccessControl.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Account.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_AuthenticationReqmt.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Credential.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Group.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Identity.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Kerberos.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Org.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_PrivilegeManagementService.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Privilege.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_PublicKey.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_Role.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_SecurityLevel.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_SecurityServices.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_SharedSecret.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_StorageHardwareID.mof
%attr(444,root,root) /opt/tog-pegasus/mof/CIM28/User28_UsersAccess.mof

%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_Authorization20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ConfigSetting20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_InternalSchema20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ShutdownService20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_User20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_Events20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_InterOpSchema20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ProviderModule20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ComputerSystem20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ComputerSystem20R.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ManagedSystemSchema20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_ManagedSystemSchema20R.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_OperatingSystem20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_OperatingSystem20R.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_UnixProcess20.mof
%attr(444,root,root) /opt/tog-pegasus/mof/Pegasus/PG_UnixProcess20R.mof

%config(noreplace) %attr(644,root,root)  /var/opt/tog-pegasus/cimserver_planned.conf
%config %attr(500,root,root) /etc/init.d/tog-pegasus
%config %attr(444,root,root) %PAM_CONFIG_DIR/wbem

%attr(500,root,root) /opt/tog-pegasus/sbin/cimauth
%attr(500,root,root) /opt/tog-pegasus/sbin/cimconfig
%attr(500,root,root) /opt/tog-pegasus/sbin/cimserver
%attr(500,root,root) /opt/tog-pegasus/sbin/cimuser
%attr(500,root,root) /opt/tog-pegasus/sbin/cimprovagt
%attr(500,root,root) /opt/tog-pegasus/sbin/cimservera
%attr(500,root,root) /opt/tog-pegasus/sbin/init_repository
%attr(500,root,root) /opt/tog-pegasus/sbin/settogpath

%attr(555,root,root) /opt/tog-pegasus/bin/cimmof
%attr(555,root,root) /opt/tog-pegasus/bin/cimmofl
%attr(555,root,root) /opt/tog-pegasus/bin/cimprovider
%attr(555,root,root) /opt/tog-pegasus/bin/osinfo
%attr(555,root,root) /opt/tog-pegasus/bin/wbemexec

%attr(555,root,root) /opt/tog-pegasus/lib/libCIMxmlIndicationHandler.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libConfigSettingProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libNamespaceProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libProviderRegistrationProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libUserAuthProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegauthentication.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegclient.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegcliutils.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegcommon.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegcompiler.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegconfig.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegexportclient.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegexportserver.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpeggetoopt.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpeghandlerservice.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegindicationservice.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpeglistener.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegprm.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegprovider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegprovidermanager.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegrepository.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegserver.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpeguser.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegwql.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libDefaultProviderManager.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libCertificateProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegqueryexpression.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegcql.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegquerycommon.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libCIMQueryCapabilitiesProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libInteropProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libCIMOMStatDataProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libpegservice.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libCMPIProviderManager.so.1
%attr(555,root,root) /opt/tog-pegasus/lib/libcmpiCppImpl.so.1
/opt/tog-pegasus/lib/libpegclient.so
/opt/tog-pegasus/lib/libpegcommon.so
/opt/tog-pegasus/lib/libpegprovider.so
/opt/tog-pegasus/lib/libDefaultProviderManager.so
/opt/tog-pegasus/lib/libCIMxmlIndicationHandler.so
/opt/tog-pegasus/lib/libCMPIProviderManager.so

%attr(555,root,root) /opt/tog-pegasus/providers/lib/libComputerSystemProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/providers/lib/libOSProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/providers/lib/libProcessProvider.so.1
/opt/tog-pegasus/providers/lib/libComputerSystemProvider.so
/opt/tog-pegasus/providers/lib/libOSProvider.so
/opt/tog-pegasus/providers/lib/libProcessProvider.so

%files sdk

# SDK Include Files
#
%dir %attr(555,root,root) /opt/tog-pegasus/include
%dir %attr(555,root,root) /opt/tog-pegasus/include/Pegasus
%dir %attr(555,root,root) /opt/tog-pegasus/include/Pegasus/Client
%dir %attr(555,root,root) /opt/tog-pegasus/include/Pegasus/Common
%dir %attr(555,root,root) /opt/tog-pegasus/include/Pegasus/Consumer
%dir %attr(555,root,root) /opt/tog-pegasus/include/Pegasus/Provider
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Client/CIMClientException.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Client/CIMClient.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Client/Linkage.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Array.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/ArrayInter.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Char16.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMClass.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMDateTime.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMFlavor.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMIndication.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMInstance.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMMethod.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMName.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMObject.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMObjectPath.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMParameter.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMParamValue.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMProperty.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMPropertyList.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMQualifierDecl.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMQualifier.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMScope.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMStatusCode.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMType.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMValue.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Config.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Exception.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Linkage.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/OperationContext.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/ResponseHandler.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/SSLContext.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/String.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Consumer/CIMIndicationConsumer.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Consumer/Linkage.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMAssociationProvider.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMIndicationConsumerProvider.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMIndicationProvider.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMInstanceProvider.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMMethodProvider.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMOMHandle.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/CIMProvider.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/Linkage.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Provider/ProviderException.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/AcceptLanguages.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/MessageLoader.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/ContentLanguages.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/LanguageElementContainer.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/AcceptLanguageElement.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Formatter.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/ContentLanguageElement.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/LanguageElement.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Client/ClientOpPerformanceDataHandler.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/CIMOperationType.h
%attr(444,root,root) /opt/tog-pegasus/include/Pegasus/Common/Platform_%PEGASUS_HARDWARE_PLATFORM.h

# SDK Sample Files
#
%dir %attr(777,root,root) /opt/tog-pegasus/samples
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Clients
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/EnumInstances
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/InvokeMethod
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/SendTestIndications
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/IndicationProvider
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/SimpleDisplayConsumer
%dir %attr(777,root,root) /opt/tog-pegasus/samples/Providers/Load
%dir %attr(777,root,root) /opt/tog-pegasus/samples/mak
%attr(444,root,root) /opt/tog-pegasus/samples/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/mak/config.mak
%attr(444,root,root) /opt/tog-pegasus/samples/mak/program.mak
%attr(444,root,root) /opt/tog-pegasus/samples/mak/recurse.mak
%attr(444,root,root) /opt/tog-pegasus/samples/mak/library.mak
%attr(444,root,root) /opt/tog-pegasus/samples/mak/common.mak
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/EnumInstances/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/InvokeMethod/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/SendTestIndications/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/mak/%PEGASUS_HARDWARE_PLATFORM.mak
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/EnumInstances/EnumInstances.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/InvokeMethod/InvokeMethod.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Clients/DefaultC++/SendTestIndications/SendTestIndications.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Load/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Load/SampleProviderSchema.mof
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Load/IndicationProviderR.mof
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/IndicationProvider/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/IndicationProvider/IndicationProvider.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/IndicationProvider/IndicationProvider.h
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/IndicationProvider/IndicationProviderMain.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/SimpleDisplayConsumer/Makefile
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Load/InstanceProviderR.mof
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Load/MethodProviderR.mof
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/Load/SimpleDisplayConsumerR.mof
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider/InstanceProvider.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider/InstanceProviderMain.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider/InstanceProvider.h
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider/EnumerateInstancesrspgood.xml
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/InstanceProvider/EnumerateInstances.xml
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider/MethodProvider.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider/MethodProviderMain.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider/MethodProvider.h
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider/InvokeMethod.xml
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/MethodProvider/InvokeMethodrspgood.xml
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumerMain.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.cpp
%attr(444,root,root) /opt/tog-pegasus/samples/Providers/DefaultC++/SimpleDisplayConsumer/SimpleDisplayConsumer.h

# SDK Documentation
#
%attr(-,root,root) /opt/tog-pegasus/html

%files test

# Test Files
#
%dir %attr(555, root,root) /opt/tog-pegasus/test
%dir %attr(555, root,root) /opt/tog-pegasus/test/lib
%dir %attr(555, root,root) /opt/tog-pegasus/test/bin
%dir %attr(555, root,root) /opt/tog-pegasus/test/mak
%attr(444,root,root) /opt/tog-pegasus/test/mak/commands.mak
%attr(555,root,root) /opt/tog-pegasus/test/lib/libSampleInstanceProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/test/lib/libSampleMethodProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/test/lib/libSampleFamilyProvider.so.1
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestAbstract
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestArray
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestBase64
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestClassDecl
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestDateTime
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestFlavor
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestFormatter
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestHashTable
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestInstanceDecl
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestLogger
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestMethod
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestObject
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestOperationContext
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestParamValue
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestParameter
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestProperty
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestQualifier
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestQualifierDecl
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestQualifierList
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestQueue
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestReference
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestResolve
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestResponseHandler
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestScope
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestStack
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestStopwatch
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestStrToInstName
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestString
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestTimeValue
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestToMof
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestValidateClass
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestValue
%attr(555,root,root) /opt/tog-pegasus/test/bin/TracerTest
%attr(555,root,root) /opt/tog-pegasus/test/bin/IPC
%attr(555,root,root) /opt/tog-pegasus/test/bin/CompAssoc
%attr(555,root,root) /opt/tog-pegasus/test/bin/UserManagerTest
%attr(555,root,root) /opt/tog-pegasus/test/bin/TestClient
%attr(555,root,root) /opt/tog-pegasus/test/bin/OSTestClient
%attr(555,root,root) /opt/tog-pegasus/test/bin/InvokeMethod2
%attr(444,root,root) /opt/tog-pegasus/test/Makefile
/var/opt/tog-pegasus/testrepository

