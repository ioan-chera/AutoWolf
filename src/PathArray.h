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


#ifndef PATHARRAY_H_
#define PATHARRAY_H_

class PathArray
{
protected:
	struct Node
	{
		int tilex, tiley;
		int f_score, g_score, h_score;
		int prev, next;
		int diagonal;
		Boolean open;
	} *nodes;
	int numNodes, numNodesAlloc;
	Boolean pathexists;
	
	int addNode(const Node &node);
public:
	PathArray() : nodes(0), numNodes(0), numNodesAlloc(0), pathexists(false)
	{
	}
	~PathArray();
	void makeEmpty()
	{
		numNodes = 0;
	}
	int addStartNode(int tx, int ty);
	int addStartNode(int tx, int ty, int destx, int desty,
                     Boolean negate = false);
	int bestScoreIndex() const;
	void closeNode(int index)
	{
		nodes[index].open = false;
	}
	void finish(int index);
	Boolean exists() const
	{
		return pathexists;
	}
	int length() const
	{
		return numNodes;
	}
	void reset()
	{
		pathexists = false;
	}
	void getCoords(int index, int *tx, int *ty) const
	{
		*tx = nodes[index].tilex;
		*ty = nodes[index].tiley;
	}
	int getPrevIndex(int index) const
	{
		return index >= 0 ? nodes[index].prev : -1;
	}
	int getNextIndex(int index) const
	{
		return index >= 0 ? nodes[index].next : -1;
	}
	int openCoordsIndex(int cx, int cy) const;
	int pathCoordsIndex(int cx, int cy) const;
	int getGScore(int index) const
	{
		return nodes[index].g_score;
	}
	void updateNode(int ichange, int index, int cx, int cy, int dist);
	void updateNode(int ichange, int index, int cx, int cy, int dist,
                    int destx, int desty, Boolean negate = false);
};

#endif