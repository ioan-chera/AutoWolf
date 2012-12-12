/*
 *  List.h
 *  AutoWolf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#ifndef LIST_H_
#define LIST_H_

//
// List
//
// A mutable list of objects
//
template <typename T> class List
{
	struct Link
	{
		T obj;
		Link *next;
	} *base, *current;
	
public:
	void add(T what)
	{
		current = base;
		base = new Link;
		base->obj = what;
		base->next = current;
	}
	void remove(T what)
	{
		Link *oldcur = 0;
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
	void removeAll()
	{
		Link *next;
		for(current = base; current; current = next)
		{
			next = current->next;
			delete current;
		}
		base = current = 0;
	}
	
	List()
	{
		base = 0;
	}
	~List()
	{
		removeAll();
	}
	
	T firstObject()
	{
		current = base;
		return current ? current->obj : 0;
	}
	T nextObject()
	{
		current = current->next;
		return current ? current->obj : 0;
	}
};

#endif
