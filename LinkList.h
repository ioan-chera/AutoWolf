/*
 *  LinkList.h
 *  AutoWOlf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#ifndef LINKLIST_H_
#define LINKLIST_H_

//
// LinkList
//
// A mutable list of objects
//
class LinkList
{
	struct Link
	{
		void *obj;
		Link *next;
	} *base, *current;
	
public:
	LinkList() : base(0)
	{
	}
	~LinkList();
	
	void addLink(void *what);
	void removeLink(void *what);
	void removeAllLinks();
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