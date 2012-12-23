//
//  Dictionary.cpp
//  Wolf4SDL
//
//  Created by Ioan on 21.12.2012.
//
//

#include <string.h>
#include "Dictionary.h"

//
// Dictionary::~Dictionary
//
// Destructor
//
Dictionary::~Dictionary()
{
	KeyVal *entry, *next;
	for(entry = entries.firstObject(); entry; entry = next)
	{
		next = entries.nextObject();
		delete entry->key;
	}
	// list deletion will happen subsequently
}

//
// Dictionary::setIntegerForKey
//
// Set key for value, creating pair if not existing
//
void Dictionary::setIntegerForKey(const char *key, int val)
{
	KeyVal *entry;
	for(entry = entries.firstObject(); entry; entry = entries.nextObject())
	{
		// look for entry with same key
		if(!strcmp(entry->key, key))
		{
			// found one
			entry->val = val;
			return;
		}
	}
	// not found any
	KeyVal *newEntry = new KeyVal;
	newEntry->key = strdup(key);
	newEntry->val = val;
	entries.add(newEntry);
}

//
// Dictionary::getIntegerForKey
//
// Obtain value. If nothing, assume 0!
//
int Dictionary::getIntegerForKey(const char *key)
{
	KeyVal *entry;
	for(entry = entries.firstObject(); entry; entry = entries.nextObject())
	{
		// look for entry with same key
		if(!strcmp(entry->key, key))
		{
			// found one
			return entry-> val;
		}
	}
	
	return 0;
}

//
// Dictionary::hasKey
//
// Verify if it has the key
//
bool Dictionary::hasKey(const char *key)
{
	KeyVal *entry;
	for(entry = entries.firstObject(); entry; entry = entries.nextObject())
	{
		// look for entry with same key
		if(!strcmp(entry->key, key))
		{
			// found one
			return true;
		}
	}
	
	return false;
}