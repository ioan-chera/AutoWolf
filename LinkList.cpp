/*
 *  LinkList.cpp
 *  AutoWolf
 *
 *  Created by MacBook on 28.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#include "LinkList.h"

LinkList::~LinkList()
{
	removeAllLinks();
}

void LinkList::addLink(void *what)
{
	current = base;
	base = new Link;
	base->obj = what;
	base->next = current;
}

void LinkList::removeLink(void *what)
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

void LinkList::removeAllLinks()
{
	Link *next;
	for(current = base; current; current = next)
	{
		next = current->next;
		delete current;
	}
	base = current = 0;
}