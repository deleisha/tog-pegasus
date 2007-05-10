/*
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
//%/////////////////////////////////////////////////////////////////////////////
*/

#ifndef _Executor_Socket_h
#define _Executor_Socket_h

#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include "Defines.h"

EXECUTOR_LINKAGE
int SetNonBlocking(int sock);

EXECUTOR_LINKAGE
int SetBlocking(int sock);

EXECUTOR_LINKAGE
ssize_t RecvNonBlock(int sock, void* buffer, size_t size);

EXECUTOR_LINKAGE
ssize_t SendNonBlock(int sock, const void* buffer, size_t size);

EXECUTOR_LINKAGE
int CloseOnExec(int fd);

EXECUTOR_LINKAGE
ssize_t SendDescriptorArray(int sock, int descriptors[], size_t count);

EXECUTOR_LINKAGE
int CreateSocketPair(int pair[2]);

static int RecvDescriptorArray(int sock, int descriptors[], size_t count)
{
    struct iovec iov[1];
    char dummy;
    struct msghdr mh;
    ssize_t n;
#if defined(HAVE_MSG_CONTROL)
    size_t size;
    char* data;
    struct cmsghdr* cmh = CMSG_FIRSTHDR(&mh);

    /*
     * This control data begins with a cmsghdr struct followed by the data
     * (a descriptor in this case). The union ensures that the data is 
     * aligned suitably for the leading cmsghdr struct. The descriptor 
     * itself is properly aligned since the cmsghdr ends on a boundary 
     * that is suitably aligned for any type (including int).
     *
     *     ControlData = [ cmsghdr | int ]
     */

    size = CMSG_SPACE(sizeof(int) * count);
    data = (char*)malloc(size);

    /* Define a msghdr that refers to the control data, which is filled in
     * by calling recvmsg() below.
     */

    memset(&mh, 0, sizeof(mh));
    mh.msg_control = data;
    mh.msg_controllen = size;

#else /* !defined(HAVE_MSG_CONTROL) */

    memset(&mh, 0, sizeof(mh));
    mh.msg_accrights = (caddr_t)descriptors;
    mh.msg_accrightslen = sizeof(int) * count;

#endif /* defined(HAVE_MSG_CONTROL) */

    /*
     * The other process sends a single-byte message. This byte is not
     * used since we only need the control data (the descriptor) but we
     * must request at least one byte from recvmsg().
     */

    memset(iov, 0, sizeof(iov));
    iov[0].iov_base = &dummy;
    iov[0].iov_len = 1;
    mh.msg_iov = iov;
    mh.msg_iovlen = 1;

    /* Receive the message from the other process. */

    n = recvmsg(sock, &mh, 0);

    if (n <= 0)
        return -1;

    /* Get a pointer to control message. Return if the header is null or 
     * does not contain what we expect.
     */

#if defined(HAVE_MSG_CONTROL)

    cmh = CMSG_FIRSTHDR(&mh);

    if (!cmh || 
        cmh->cmsg_len != CMSG_LEN(sizeof(int) * count) ||
        cmh->cmsg_level != SOL_SOCKET ||
        cmh->cmsg_type != SCM_RIGHTS)
    {
        return -1;
    }

    /* Copy the data: */

    memcpy(descriptors, CMSG_DATA(cmh), sizeof(int) * count);

#else /* !defined(HAVE_MSG_CONTROL) */

    if (mh.msg_accrightslen != sizeof(int) * count)
        return -1;

    memcpy(descriptors, mh.msg_accrights, sizeof(int) * count);

#endif /* defined(HAVE_MSG_CONTROL) */

    return 0;
}

#endif /* _Executor_Socket_h */
