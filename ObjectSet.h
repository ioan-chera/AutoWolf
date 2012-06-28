/*
 *  ObjectSet.h
 *  AutoWOlf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

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
	void *firstObject()
	{
		return (current = base)->obj;
	}
	void *nextObject()
	{
		return (current = current->next)->obj;
	}
};