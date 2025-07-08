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


#ifndef IOAN_SECRET_H_
#define IOAN_SECRET_H_

#include <vector>
#include <utility>

namespace Secret
{
struct Push
{
	int from;
	int wallpos;
	int to;
	bool trivial;
	
	bool operator==(const Push& other) const noexcept
	{
		return from == other.from && 
		       wallpos == other.wallpos && 
		       to == other.to && 
		       trivial == other.trivial;
	}
	
	bool operator!=(const Push& other) const noexcept
	{
		return !(*this == other);
	}
};

struct PushTree
{
	void clear(const Push &push);
	bool SafeToPush(int tx, int ty, int txofs, int tyofs) const;
	void logStructure(int depth = 0) const;

	std::vector<Push> trivial;
	std::vector<std::pair<Push, PushTree>> nontrivial;
};

PushTree AnalyzeSecrets();

}

#endif
