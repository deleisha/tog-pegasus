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
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "Config.h"
#include "Strlcpy.h"
#include "Strlcat.h"
#include "Path.h"
#include "Defines.h"

/*
**==============================================================================
**
** ConfigParameter
**
**     The _config[] array below defines default values for the
**     various configuration items.
**
**==============================================================================
*/

struct ConfigParameter
{
    const char* name;
    const char* value;
};

static struct ConfigParameter _config[] =
{
#include <Pegasus/Config/FixedPropertyTable.h>
};

static size_t _configSize = 
    sizeof(_config) / sizeof(_config[0]);

/*
**==============================================================================
**
** GetConfigParamFromCommandLine()
**
**     Attempt to find a command line configuratin parameter of the form 
**     name=value. For example: 
**         cimservermain repositoryDir=/opt/pegasus/repository. 
**     Return 0 if found.
**
**==============================================================================
*/

int GetConfigParamFromCommandLine(
    int argc,
    char** argv,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    size_t n = strlen(name);
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], name, n) == 0 && argv[i][n] == '=')
        {
            const char* p = argv[i] + n + 1;
            Strlcpy(value, argv[i] + n + 1, EXECUTOR_BUFFER_SIZE);
            return 0;
        }
    }

    return -1;
}

/*
**==============================================================================
**
** GetConfigParamFromFile()
**
**     Attempt to find the named option in the configuration file. If found,
**     set value and return 0.
**
**==============================================================================
*/

int GetConfigParamFromFile(
    const char* path,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    char buffer[EXECUTOR_BUFFER_SIZE];
    FILE* is;
    size_t n;
    
    if ((is = fopen(path, "r")) == NULL)
        return -1;

    n = strlen(name);

    while (fgets(buffer, sizeof(buffer), is) != 0)
    {
        size_t r;

        /* Skip comments. */

        if (buffer[0] == '#')
            continue;

        /* Remove trailing whitespace. */

        r = strlen(buffer);

        while (r--)
        {
            if (isspace(buffer[r]))
                buffer[r] = '\0';
        }

        /* Skip blank lines. */

        if (buffer[0] == '\0')
            continue;

        /* Check option. */

        if (strncmp(buffer, name, n) == 0 &&  buffer[n] == '=')
        {
            Strlcpy(value, buffer + n + 1, EXECUTOR_BUFFER_SIZE);
            fclose(is);
            return 0;
        }
    }

    /* Not found! */
    fclose(is);
    return -1;
}

/*
**==============================================================================
**
** GetConfigParam()
**
**     Attempt to find a configuration setting for the given name. First,
**     search the command line and then the config file.
**
**==============================================================================
*/

int GetConfigParam(
    int argc,
    char** argv,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    char path[EXECUTOR_BUFFER_SIZE];
    size_t i;

    /* (1) First check command line. */

    if (GetConfigParamFromCommandLine(argc, argv, name, value) == 0)
        return 0;

    /* (2) Next check planned config file. */

    if (GetHomedPath(PEGASUS_PLANNED_CONFIG_FILE_PATH, path) == 0 &&
        GetConfigParamFromFile(path, name, value) == 0)
        return 0;

    /* (3) Finally check the default configuration table. */

    for (i = 0; i < _configSize; i++)
    {
        if (strcmp(_config[i].name, name) == 0)
        {
            Strlcpy(value, _config[i].value, EXECUTOR_BUFFER_SIZE);
            return 0;
        }
    }

    /* Not found! */
    return -1;
}
