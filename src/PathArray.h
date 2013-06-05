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

//
// PathArray
//
// Class for bot's pathfinding in the Wolfenstein grid
//
class PathArray
{
protected:
    //
    // Node
    //
    // Dijkstra/A* search node
    //
	struct Node
	{
		int tilex, tiley;               // x, y tile coordinate
		int f_score, g_score, h_score;  // total, so-far, to-dest heuristics
		int prev, next;                 // previous and next index on path
		int diagonal;                   // diagonal direction, if applicable
		Boolean open;                   // node still open for search?
	} *nodes;                           // array of nodes here
	int numNodes, numNodesAlloc;        // number of nodes, and allocated amount
	Boolean pathexists;                 // whether the path has been built
	
	int addNode(const Node &node);
public:
	int addStartNode(int tx, int ty);
	int addStartNode(int tx, int ty, int destx, int desty,
                     Boolean negate = false);
	int bestScoreIndex() const;
	void finish(int index);
	int openCoordsIndex(int cx, int cy) const;
	int pathCoordsIndex(int cx, int cy) const;
	~PathArray();
	void updateNode(int ichange, int index, int cx, int cy, int dist);
	void updateNode(int ichange, int index, int cx, int cy, int dist,
                    int destx, int desty, Boolean negate = false);
    
    // Node closing accessor
	void closeNode(int index) {nodes[index].open = false;}
    // pathexists accessor
	Boolean exists() const {return pathexists;}
    // node coordinates accessor
	void getCoords(int index, int *tx, int *ty) const
	{
		*tx = nodes[index].tilex;
		*ty = nodes[index].tiley;
	}
    // previous index accessor (or -1)
	int getPrevIndex(int index) const
	{
		return index >= 0 ? nodes[index].prev : -1;
	}
    // next index accessor (or -1)
	int getNextIndex(int index) const
	{
		return index >= 0 ? nodes[index].next : -1;
	}
    // numNodes accessor
	int length() const {return numNodes;}
    // Empty the array (FIXME: merge with reset?)
	void makeEmpty() {numNodes = 0;}
    //
    // Constructor
    //
	PathArray() : nodes(0), numNodes(0), numNodesAlloc(0), pathexists(false)
	{
	}
    // pathexists disabling accessor
	void reset() {pathexists = false;}
};

#endif