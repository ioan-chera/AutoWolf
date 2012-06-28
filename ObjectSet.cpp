/*
 *  ObjectSet.cpp
 *  AutoWolf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#include "ObjectSet.h"

ObjectSet::~ObjectSet()
{
	SetComponent *next;
	for(current = base; current; current = next)
	{
		next = current->next;
		delete current;
	}
}

void ObjectSet::addObject(void *what)
{
	current = base;
	base = new SetComponent;
	base->obj = what;
	base->next = current;
}

void ObjectSet::removeObject(void *what)
{
	SetComponent *oldcur = 0;
	for(current = base; current; current = current->next)
	{
		if(current->obj == what)
		{
			if(oldcur)
				oldcur->next = current->next;
			else {
				base = base->next;
			}
			delete current;
			return;
		}
		oldcur = current;
	}
}