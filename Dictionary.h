//
//  Dictionary.h
//  Wolf4SDL
//
//  Created by Ioan on 21.12.2012.
//
//

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include "List.h"

//
// Dictionary
//
// A list of string keys with variant values
// FIXME: make it more efficient
//
class Dictionary
{
protected:
	struct KeyVal
	{
		char *key;
		int val;
	};
	
	List <KeyVal *> entries;
public:
	// Destructor
	~Dictionary();
	// Set key for value, creating pair if not existing
	void setIntegerForKey(const char *key, int val);
	// Obtain value. If nothing, assume 0!
	int getIntegerForKey(const char *key);
	// Verify if it has the key
	bool hasKey(const char *key);
};

#endif

