/*
 *  ObjectSet.h
 *  AutoWOlf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#ifndef OBJECTSET_H_
#define OBJECTSET_H_

//
// ObjectSet
//
// A mutable list of objects
//
class ObjectSet
{
	struct SetComponent
	{
		void *obj;
		SetComponent *next;
	} *base, *current;
public:
	ObjectSet() : base(0)
	{
	}
	~ObjectSet();
	void addObject(void *what);
	void removeObject(void *what);
	void removeAllObjects();
	void *firstObject()
	{
		current = base;
		return current ? current->obj : 0;
	}
	void *nextObject()
	{
		current = current->next;
		return current ? current->obj : 0;
	}
};

#endif