//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher, Inova Europe (mbrasher@bmc.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusRepository_ObjectCache_h
#define PegasusRepository_ObjectCache_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Repository/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

Uint32 ObjectCacheHash(const String& str);

template<class OBJECT>
class PEGASUS_REPOSITORY_LINKAGE ObjectCache
{
public:

    ObjectCache(size_t maxEntries);

    void put(const String& path, OBJECT& object);

    bool get(const String& path, OBJECT& object);

    bool evict(const String& path);

private:

    static Uint32 _hash(const String& s)
    {
	return ObjectCacheHash(s);
    }

    static bool _equal(const String& s1, const String& s2)
    {
        return String::equalNoCase(s1, s2);
    }

    struct Entry
    {
	Uint32 code;
	String path;
	OBJECT object;
	Entry* hashNext;
	Entry* queueNext;
	Entry* queuePrev;

	Entry(Uint32 code_, const String& path_, OBJECT& object_) :
	    code(code_), path(path_), object(object_.clone()) { }
    };

    enum { NUM_CHAINS = 128 };

    Entry* _chains[NUM_CHAINS];
    Entry* _front;
    Entry* _back;
    size_t _numEntries;
    size_t _maxEntries;
    Mutex _mutex;
};

template<class OBJECT>
ObjectCache<OBJECT>::ObjectCache(size_t maxEntries) : 
    _front(0), _back(0), _numEntries(0), _maxEntries(maxEntries)
{
    memset(_chains, 0, sizeof(_chains));
}

template<class OBJECT>
void ObjectCache<OBJECT>::put(const String& path, OBJECT& object)
{
    if (_maxEntries == 0)
	return;

    _mutex.lock(pegasus_thread_self());

    //// Update object if it is already in cache:

    Uint32 code = _hash(path);
    Uint32 index = code % NUM_CHAINS;

    for (Entry* p = _chains[index]; p; p = p->hashNext)
    {
        if (code == p->code && _equal(p->path, path))
	{
	    // Update the repository.
	    p->object = object.clone();
	    _mutex.unlock();
	    return;
	}
    }

    //// Add to hash table:

    Entry* entry = new Entry(code, path, object);
    entry->hashNext = _chains[index];
    _chains[index] = entry;

    //// Add to back of FIFO queue:

    entry->queueNext = 0;

    if (_back)
    {
	_back->queueNext = entry;
	entry->queuePrev = _back;
	_back = entry;
    }
    else
    {
	_front = entry;
	_back = entry;
	entry->queuePrev = 0;
    }

    _numEntries++;

    //// Evict LRU entry if necessary (from front).

    if (_numEntries > _maxEntries)
    {
	Entry* entry = _front;

	//// Remove from hash table first.

	Uint32 index = entry->code % NUM_CHAINS;
	Entry* hashPrev = 0;

	for (Entry* p = _chains[index]; p; p = p->hashNext)
	{
	    if (p->code == entry->code && _equal(p->path, entry->path))
	    {
		if (hashPrev)
		    hashPrev->hashNext = p->hashNext;
		else
		    _chains[index] = p->hashNext;

		break;
	    }

	    hashPrev = p;
	}

	//// Now remove from queue:

	_front = entry->queueNext;

	if (_front)
	    _front->queuePrev = 0;

	delete entry;
	_numEntries--;
    }

    _mutex.unlock();
    return;
}

template<class OBJECT>
bool ObjectCache<OBJECT>::get(const String& path, OBJECT& object)
{
    if (_maxEntries == 0)
	return false;

    _mutex.lock(pegasus_thread_self());

    //// Search cache for object.

    Uint32 code = _hash(path);
    Uint32 index = code % NUM_CHAINS;

    for (Entry* p = _chains[index]; p; p = p->hashNext)
    {
        if (code == p->code && _equal(p->path, path))
	{
	    object = p->object.clone();
	    _mutex.unlock();
	    return true;
	}
    }

    /// Not found!

    _mutex.unlock();
    return false;
}

template<class OBJECT>
bool ObjectCache<OBJECT>::evict(const String& path)
{
    if (_maxEntries == 0)
	return false;

    _mutex.lock(pegasus_thread_self());

    //// Find and remove the given element.

    Uint32 code = _hash(path);
    Uint32 index = code % NUM_CHAINS;
    Entry* hashPrev = 0;

    for (Entry* p = _chains[index]; p; p = p->hashNext)
    {
        if (code == p->code && _equal(p->path, path))
	{
	    // Remove from hash chain:

            if (hashPrev)
                hashPrev->hashNext = p->hashNext;
            else
                _chains[index] = p->hashNext;

	    // Remove from queue:

	    if (p->queuePrev)
		p->queuePrev->queueNext = p->queueNext;
	    else
		_front = p->queueNext;

	    if (p->queueNext)
		p->queueNext->queuePrev = p->queuePrev;
	    else
		_back = p->queuePrev;

	    // Delete the entry and update the number of entries.

	    delete p;
	    _numEntries--;

	    _mutex.unlock();
	    return true;
	}

	hashPrev = p;
    }

    //// Not found!

    _mutex.unlock();
    return false;
}

PEGASUS_NAMESPACE_END

#endif /* PegasusRepository_ObjectCache_h */
