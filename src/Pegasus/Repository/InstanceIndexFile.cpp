//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//              Ramnath Ravindran (Ramnath.Ravindran@compaq.com) 03/21/2002
//			replaced instances of "| ios::binary" with 
//			PEGASUS_OR_IOS_BINARY
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Tracer.h>
#include "InstanceIndexFile.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const Uint32 _MAX_FREE_COUNT = 16;

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

//
// Gets one line from the given file.
//

static Boolean _GetLine(fstream& fs, Array<char>& x)
{
    x.clear();
    x.reserve(1024);

    char c;

    while (fs.get(c) && c != '\n') 
        x.append(c);

    x.append('\0');

    return !!fs;
}

inline void _SkipWhitespace(char*& p)
{
    while (*p && isspace(*p))
        p++;
}

//
// Get an integer field from the character pointer and advance the
// pointer past the field.
//

Boolean _GetIntField(
    char*& ptr,
    Boolean& error,
    Uint32& value,
    int base)
{
    char* end = 0;
    value = strtoul(ptr, &end, base);

    error = false;

    if (!end)
    {
        error = true;
        return false;
    }

    _SkipWhitespace(end);

    if (*end == '\0')
    {
        error = true;
        return false;
    }

    ptr = end;
    return true;
}

//
// Gets the next record in the index file.
//

static Boolean _GetNextRecord(
    fstream& fs, 
    Array<char>& line,
    Uint32& freeFlag,
    Uint32& hashCode,
    Uint32& index,
    Uint32& size,
    const char*& instanceName,
    Boolean& error)
{
    error = false;

    //
    // Get next line:
    //

    if (!_GetLine(fs, line))
        return false;

    //
    // Get the free flag field:
    //

    char* end = (char*)line.getData();

    if (!_GetIntField(end, error, freeFlag, 10))
	return false;

    if (freeFlag != 0 && freeFlag != 1)
    {
	error = true;
	return false;
    }

    //
    // Get the hash-code field:
    //

    if (!_GetIntField(end, error, hashCode, 16))
	return false;

    //
    // Get index field:
    //

    if (!_GetIntField(end, error, index, 10))
	return false;

    //
    // Get size field:
    //

    if (!_GetIntField(end, error, size, 10))
	return false;

    //
    // Get instance name:
    //

    instanceName = end;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// InstanceIndexFile:
//
////////////////////////////////////////////////////////////////////////////////

Boolean InstanceIndexFile::lookupEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32& indexOut,
    Uint32& sizeOut)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::lookupEntry()");

    fstream fs;

    if (!_openFile(path, fs))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    Uint32 entryOffset = 0;

    Boolean result = _lookupEntry(
	fs, instanceName, indexOut, sizeOut, entryOffset);

    fs.close();

    PEG_METHOD_EXIT();
    return result;
}

Boolean InstanceIndexFile::createEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32 indexIn,
    Uint32 sizeIn)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::createEntry()");

    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(path, fs, true))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Return false if entry already exists:
    //

    Uint32 tmpIndex;
    Uint32 tmpSize;
    Uint32 tmpEntryOffset;

    if (InstanceIndexFile::_lookupEntry(
	fs, instanceName, tmpIndex, tmpSize, tmpEntryOffset))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Append the new entry to the end of the file:
    //

    if (!_appendEntry(fs, instanceName, indexIn, sizeIn))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Close the file:
    //

    fs.close();

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::deleteEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32& freeCount)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::deleteEntry()");

    freeCount = 0;

    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(path, fs))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Mark the entry as free:
    //

    if (!_markEntryFree(fs, instanceName))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Increment the free count:
    //

    freeCount = 0;

    if (!_incrementFreeCount(fs, freeCount))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Close the file:
    //

    fs.close();

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::modifyEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32 indexIn,
    Uint32 sizeIn,
    Uint32& freeCount)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::modifyEntry()");

    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(path, fs))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Mark the entry as free:
    //

    if (!_markEntryFree(fs, instanceName))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Append new entry:
    //

    if (!_appendEntry(fs, instanceName, indexIn, sizeIn))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Increment the free count:
    //

    freeCount = 0;

    if (!_incrementFreeCount(fs, freeCount))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Close the file:
    //

    fs.close();

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::enumerateEntries(
    const String& path,
    Array<Uint32>& freeFlags,
    Array<Uint32>& indices,
    Array<Uint32>& sizes,
    Array<CIMReference>& instanceNames,
    Boolean includeFreeEntries)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::enumerateEntries()");

    //
    // Reserve space for at least COUNT entries:
    //

    const Uint32 COUNT = 1024;

    freeFlags.reserve(COUNT);
    indices.reserve(COUNT);
    sizes.reserve(COUNT);
    instanceNames.reserve(COUNT);

    //
    // Open input file:
    //

    fstream fs;

    if (!_openFile(path, fs))
    {
        // file does not exist, just return with no instanceNames
        PEG_METHOD_EXIT();
        return true;
    }

    //
    // Iterate over all instances to build output arrays:
    //

    Array<char> line;
    Uint32 freeFlag;
    Uint32 hashCode;
    const char* instanceName;
    Uint32 index;
    Uint32 size;
    Boolean error;

    while (_GetNextRecord(
	fs, line, freeFlag, hashCode, index, size, instanceName, error))
    {
	if (!freeFlag || includeFreeEntries)
	{
	    freeFlags.append(freeFlag); 
	    indices.append(index); 
	    sizes.append(size);
	    instanceNames.append(instanceName);
	}
    }

    if (error)
    {
        PEG_METHOD_EXIT();
	return false;
    }

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::_incrementFreeCount(
    PEGASUS_STD(fstream)& fs,
    Uint32& freeCount)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::_incrementFreeCount()");

    //
    // Position file pointer to beginning of file (where free count is
    // located) and read the current free count.
    //

    fs.seekg(0);
    char hexString[9];
    fs.read(hexString, 8);

    if (!fs)
    {
        PEG_METHOD_EXIT();
	return false;
    }

    hexString[8] = '\0';

    //
    // Convert hex string to integer:
    //

    char* end = 0;
    long tmp = strtol(hexString, &end, 16);

    if (!end || *end != '\0' || tmp < 0)
    {
        PEG_METHOD_EXIT();
	return false;
    }

    freeCount = Uint32(tmp);

    //
    // Increment and rewrite the free count:
    //

    sprintf(hexString, "%08X", ++freeCount);
    fs.seekg(0);
    fs.write(hexString, 8);

    PEG_METHOD_EXIT();
    return !!fs;
}

Boolean InstanceIndexFile::_openFile(
    const String& path, 
    PEGASUS_STD(fstream)& fs,
    Boolean create)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::_openFile()");

    const char ZERO_FREE_COUNT[] = "00000000\n";

    //
    // Open the file:
    //

    if (!FileSystem::openNoCase(fs, path, ios::in | ios::out PEGASUS_OR_IOS_BINARY))
    {
        if (create)
        {
	    //
	    // File does not exist so create it:
	    //

            ArrayDestroyer<char> p(path.allocateCString());
            fs.open(p.getPointer(), ios::out PEGASUS_OR_IOS_BINARY);

            if (!fs)
            {
                PEG_METHOD_EXIT();
	        return false;
            }

	    fs.write(ZERO_FREE_COUNT, sizeof(ZERO_FREE_COUNT) - 1);
	    fs.close();

	    //
	    // Reopen the file:
            //

	    if (!FileSystem::openNoCase(fs, path, ios::in | ios::out PEGASUS_OR_IOS_BINARY))
            {
                PEG_METHOD_EXIT();
	        return false;
            }
        }
        else
        {
            PEG_METHOD_EXIT();
            return false;
        }
    }

    //
    // Position the file pointer beyond the free count:
    //

    fs.seekg(sizeof(ZERO_FREE_COUNT) - 1);

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::_appendEntry(
    PEGASUS_STD(fstream)& fs,
    const CIMReference& instanceName,
    Uint32 indexIn,
    Uint32 sizeIn)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::_appendEntry()");

    //
    // Position the file at the end:
    //

    fs.seekg(0, ios::end);

    if (!fs)
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Write the entry:
    //

    Uint32 targetHashCode = instanceName.makeHashCode();

    char buffer[32];
    sprintf(buffer, "%08X", targetHashCode);

    fs << "0 " << buffer << ' ' << indexIn << ' ' << sizeIn << ' ';
    fs << instanceName << endl;

    PEG_METHOD_EXIT();
    return !!fs;
}

Boolean InstanceIndexFile::_markEntryFree(
    PEGASUS_STD(fstream)& fs,
    const CIMReference& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::_markEntryFree()");

    //
    // First look up the entry:
    //

    Uint32 index = 0;
    Uint32 size = 0;
    Uint32 entryOffset = 0;

    if (!InstanceIndexFile::_lookupEntry(
	fs, instanceName, index, size, entryOffset))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Now mark the entry as free (change the first character of the entry
    // from a '0' to a '1').
    //

    fs.seekg(entryOffset);

    if (!fs)
    {
        PEG_METHOD_EXIT();
	return false;
    }

    fs.write("1", 1);

    PEG_METHOD_EXIT();
    return !!fs;
}

Boolean InstanceIndexFile::_lookupEntry(
    PEGASUS_STD(fstream)& fs,
    const CIMReference& instanceName,
    Uint32& indexOut,
    Uint32& sizeOut,
    Uint32& entryOffset)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::_lookupEntry()");

    indexOut = 0;
    sizeOut = 0;
    entryOffset = 0;

    Uint32 targetHashCode = instanceName.makeHashCode();
    Array<char> line;
    Uint32 freeFlag;
    Uint32 hashCode;
    const char* instanceNameTmp;
    Uint32 index;
    Uint32 size;
    Boolean error;

    entryOffset = fs.tellp();

    while (_GetNextRecord(
	fs, line, freeFlag, hashCode, index, size, instanceNameTmp, error))
    {
	if (freeFlag == 0 &&
	    hashCode == targetHashCode &&
	    CIMReference(instanceNameTmp) == instanceName)
	{
	    indexOut = index;
	    sizeOut = size;
            PEG_METHOD_EXIT();
	    return true;
	}

	entryOffset = fs.tellp();
    }

    fs.clear();

    PEG_METHOD_EXIT();
    return false;
}

Boolean InstanceIndexFile::compact(
    const String& path)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::compact()");

    //
    // Open input file:
    //

    fstream fs;

    if (!_openFile(path, fs))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Open temporary file (delete it first):
    //

    fstream tmpFs;
    String tmpPath = path;
    tmpPath += ".tmp";

    FileSystem::removeFileNoCase(tmpPath);

    if (!_openFile(tmpPath, tmpFs, true))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Iterate over all instances to build output arrays:
    //

    Array<char> line;
    Uint32 freeFlag;
    Uint32 hashCode;
    const char* instanceName;
    Uint32 index;
    Uint32 size;
    Boolean error;
    Uint32 adjust = 0;

    while (_GetNextRecord(
	fs, line, freeFlag, hashCode, index, size, instanceName, error))
    {
	//
	// Copy the entry over to the temporary file if it is not free.
	// Otherwise, discard the entry and update subsequent indices to
	// compensate for removal of this block.
	//

	if (freeFlag)
	{
	    adjust += size;
	}
	else
	{
	    if (!_appendEntry(tmpFs, instanceName, index - adjust, size))
	    {
		error = true;
		break;
	    }
	}
    }

    //
    // Close both files:

    fs.close();
    tmpFs.close();

    //
    // If an error occurred, remove the temporary file and
    // return false.
    //

    if (error)
    {
	FileSystem::removeFileNoCase(tmpPath);
        PEG_METHOD_EXIT();
	return false;
    }

    //
    // Replace index file with temporary file:
    //

    if (!FileSystem::removeFileNoCase(path))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    if (!FileSystem::renameFile(tmpPath, path))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::hasNonFreeEntries(const String& path)
{
    //
    // If file does not exist, there are no instances:
    //

    if (!FileSystem::existsNoCase(path))
	return false;

    //
    // We must iterate all the entries looking for a non-free one:
    //

    Array<Uint32> freeFlags;
    Array<Uint32> indices;
    Array<Uint32> sizes;
    Array<CIMReference> instanceNames;

    if (!InstanceIndexFile::enumerateEntries(
	path, freeFlags, indices, sizes, instanceNames, false))
    {
	// This won't happen!
	return false;
    }

    return freeFlags.size() != 0;
}

Boolean InstanceIndexFile::beginTransaction(const String& path)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::beginTransaction()");

    //
    // Create a rollback file which is a copy of the index file. The
    // new filename is formed by appending ".rollback" to the name of
    // the index file.
    //

    String rollbackPath = path;
    rollbackPath += ".rollback";

    if (!FileSystem::copyFile(path, rollbackPath))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    PEG_METHOD_EXIT();
    return true;
}

Boolean InstanceIndexFile::rollbackTransaction(const String& path)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::rollbackTransaction()");

    //
    // If the rollback file does not exist, then everything is fine (nothing
    // to roll back).
    //

    if (!FileSystem::existsNoCase(path + ".rollback"))
    {
        PEG_METHOD_EXIT();
	return true;
    }

    //
    // To roll back, simply delete the index file and rename
    // the rollback file over it.
    //

    if (!FileSystem::removeFileNoCase(path))
    {
        PEG_METHOD_EXIT();
	return false;
    }

    PEG_METHOD_EXIT();
    return FileSystem::renameFileNoCase(path + ".rollback", path);
}

Boolean InstanceIndexFile::commitTransaction(const String& path)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "InstanceIndexFile::commitTransaction()");

    //
    // To commit, simply remove the rollback file:
    //

    String rollbackPath = path;
    rollbackPath += ".rollback";

    PEG_METHOD_EXIT();
    return FileSystem::removeFileNoCase(rollbackPath);
}

PEGASUS_NAMESPACE_END
