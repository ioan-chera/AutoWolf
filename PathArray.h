/*
 *  PathArray.h
 *  AutoWolf
 *
 *  Created by MacBook on 30.06.2012.
 *  Copyright 2012 Ioan Chera. Written under the GNU GPL license.
 *
 */

#ifndef PATHARRAY_H_
#define PATHARRAY_H_

#include "wl_def.h"

class PathArray
{
protected:
	struct Node
	{
		int tilex, tiley;
		int f_score, g_score, h_score;
		int prev, next;
		int diagonal;
		boolean open;
	} *nodes;
	int numNodes, numNodesAlloc;
	boolean pathexists;
	
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
	int addStartNode(int tx, int ty, int destx, int desty, boolean negate = false);
	int bestScoreIndex() const;
	void closeNode(int index)
	{
		nodes[index].open = false;
	}
	void finish(int index);
	boolean exists() const
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
	void updateNode(int ichange, int index, int cx, int cy, int dist, int destx, int desty, boolean negate = false);
};

#endif