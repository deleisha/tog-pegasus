//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company 
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Message.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


Uint32 message_mask::type_legacy =          0x00000000;
Uint32 message_mask::type_CIMClass =        0x00000001;
Uint32 message_mask::type_CIMInstance =     0x00000002;
Uint32 message_mask::type_CIMIndication =   0x00000004;
Uint32 message_mask::type_CIMQualifier =    0x00000008;
Uint32 message_mask::type_CIMSubscription = 0x00000010;
Uint32 message_mask::type_socket =          0x00000020;
Uint32 message_mask::type_connection =      0x00000040;
Uint32 message_mask::type_http =            0x00000080;
Uint32 message_mask::type_cimom =           0x00000100;

Uint32 message_mask::type_request =         0x20000000;
Uint32 message_mask::type_resply =          0x40000000;
Uint32 message_mask::type_control =         0x80000000;

Uint32 Message::_nextKey = 0;

Message::~Message() 
{ 
}

void Message::print(ostream& os) const
{
    os << "[" << MessageTypeToString(_type) << ", " << _key << "]" << endl;
}

static const char* _MESSAGE_TYPE_STRINGS[] =
{
    "CIM_GET_CLASS_REQUEST_MESSAGE",
    "CIM_GET_INSTANCE_REQUEST_MESSAGE",
    "CIM_EXPORT_INDICATION_REQUEST_MESSAGE",
    "CIM_DELETE_CLASS_REQUEST_MESSAGE",
    "CIM_DELETE_INSTANCE_REQUEST_MESSAGE",
    "CIM_CREATE_CLASS_REQUEST_MESSAGE",
    "CIM_CREATE_INSTANCE_REQUEST_MESSAGE",
    "CIM_MODIFY_CLASS_REQUEST_MESSAGE",
    "CIM_MODIFY_INSTANCE_REQUEST_MESSAGE",
    "CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE",
    "CIM_EXEC_QUERY_REQUEST_MESSAGE",
    "CIM_ASSOCIATORS_REQUEST_MESSAGE",
    "CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE",
    "CIM_REFERENCES_REQUEST_MESSAGE",
    "CIM_REFERENCE_NAMES_REQUEST_MESSAGE",
    "CIM_GET_PROPERTY_REQUEST_MESSAGE",
    "CIM_SET_PROPERTY_REQUEST_MESSAGE",
    "CIM_GET_QUALIFIER_REQUEST_MESSAGE",
    "CIM_SET_QUALIFIER_REQUEST_MESSAGE",
    "CIM_DELETE_QUALIFIER_REQUEST_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE",
    "CIM_INVOKE_METHOD_REQUEST_MESSAGE",
    "CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE",
    "CIM_GET_CLASS_RESPONSE_MESSAGE",
    "CIM_GET_INSTANCE_RESPONSE_MESSAGE",
    "CIM_EXPORT_INDICATION_RESPONSE_MESSAGE",
    "CIM_DELETE_CLASS_RESPONSE_MESSAGE",
    "CIM_DELETE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_CREATE_CLASS_RESPONSE_MESSAGE",
    "CIM_CREATE_INSTANCE_RESPONSE_MESSAGE",
    "CIM_MODIFY_CLASS_RESPONSE_MESSAGE",
    "CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE",
    "CIM_EXEC_QUERY_RESPONSE_MESSAGE",
    "CIM_ASSOCIATORS_RESPONSE_MESSAGE",
    "CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE",
    "CIM_REFERENCES_RESPONSE_MESSAGE",
    "CIM_REFERENCE_NAMES_RESPONSE_MESSAGE",
    "CIM_GET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_SET_PROPERTY_RESPONSE_MESSAGE",
    "CIM_GET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_SET_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE",
    "CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE",
    "CIM_INVOKE_METHOD_RESPONSE_MESSAGE",
    "CIM_ENABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_MODIFY_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "CIM_DISABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE",
    "SOCKET_MESSAGE",
    "CLOSE_CONNECTION_MESSAGE",
    "HTTP_MESSAGE",
};

const char* MessageTypeToString(Uint32 messageType)
{
    if (messageType > DUMMY_MESSAGE && messageType < NUMBER_OF_MESSAGES)
	return _MESSAGE_TYPE_STRINGS[messageType - 1];

    return "Unknown message type";
}

PEGASUS_NAMESPACE_END
