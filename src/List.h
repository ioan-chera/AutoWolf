//
// Copyright (C) 2013  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//


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
