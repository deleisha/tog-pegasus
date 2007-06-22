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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////
*/

#include <Executor/Path.h>
#include <Executor/Strlcpy.h>
#include <Executor/Strlcat.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static void UnsetEnvironmentVariable(const char* name)
{
    int namelen = strlen(name);
    int i;

    for (i = 0; environ[i]; i++)
    {
        if ((strncmp(name, environ[i], namelen) == 0) &&
            (environ[i][namelen] == '='))
        {
            break;
        }
    }

    if (environ[i])
    {
        int j = i;
        while (environ[++j])
            ;
        environ[i] = environ[j-1];
        environ[j-1] = 0;
    }
}

static int test(const char* path, const char* expect)
{
    char buffer[EXECUTOR_BUFFER_SIZE];
    DirName(path, buffer);
    return strcmp(buffer, expect) == 0 ? 0 : -1;
}

int main()
{
    /* DirName() */
    {
        assert(test("/aaa", "/") == 0);
        assert(test("/aaa/", "/") == 0);
        assert(test("/aaa/bbb", "/aaa") == 0);
        assert(test("/aaa/bbb/ccc", "/aaa/bbb") == 0);
        assert(test("aaa/bbb", "aaa") == 0);
        assert(test("aaa", ".") == 0);
        assert(test("aaa/", ".") == 0);
        assert(test("", ".") == 0);
        assert(test("/", "/") == 0);
        assert(test("////", "/") == 0);
        assert(test("/etc/passwd", "/etc") == 0);
    }

    /* GetHomedPath() */
    {
        char expect[EXECUTOR_BUFFER_SIZE];
        char actual[EXECUTOR_BUFFER_SIZE];
        const char* home;

        assert((home = getenv("PEGASUS_HOME")) != NULL);

        /* Test relative path */
        Strlcpy(expect, home, sizeof(expect));
        Strlcat(expect, "/somefile", sizeof(expect));

        assert(GetHomedPath("somefile", actual) == 0);
        assert(strcmp(expect, actual) == 0);

        /* Test absolute path */
        memset(actual, 0, sizeof(actual));
        assert(GetHomedPath(expect, actual) == 0);
        assert(strcmp(expect, actual) == 0);

        /* Test null path */
        memset(actual, 0, sizeof(actual));
        assert(GetHomedPath(NULL, actual) == 0);
        assert(strcmp(home, actual) == 0);
    }

    /* GetPegasusInternalBinDir() */
    {
        char expect[EXECUTOR_BUFFER_SIZE];
        char actual[EXECUTOR_BUFFER_SIZE];
        const char* home;

        assert((home = getenv("PEGASUS_HOME")) != NULL);
        Strlcpy(expect, home, sizeof(expect));
        Strlcat(expect, "/bin", sizeof(expect));

        assert(GetPegasusInternalBinDir(actual) == 0);
        assert(strcmp(expect, actual) == 0);
    }

    /* Remove PEGASUS_HOME from the environment */

    UnsetEnvironmentVariable("PEGASUS_HOME");

    /* GetHomedPath() with no PEGASUS_HOME defined */
    {
        char buffer[EXECUTOR_BUFFER_SIZE];
        assert(GetHomedPath("somefile", buffer) != 0);
    }

    /* GetPegasusInternalBinDir() with no PEGASUS_HOME defined */
    {
        char buffer[EXECUTOR_BUFFER_SIZE];
        assert(GetPegasusInternalBinDir(buffer) != 0);
    }

    printf("+++++ passed all tests\n");

    return 0;
}
