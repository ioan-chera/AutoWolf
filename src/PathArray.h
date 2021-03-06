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
    // NodeTableRef
    //
    // Node reference used by a hash table for quick lookup of x/y coordinates
    //
//    struct NodeTableRef
//    {
//        int coord;            // merged x-y coordinates, one for each 2-byte
//        int index;                      // index of targeted node
//        DLListItem<NodeTableRef> link;  // list link, for the EHashTable
//    };
    
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
		Boolean8 open;                   // node still open for search?
	} *nodes;                           // array of nodes here
    int numOpenNodes;                   // number of open nodes
	int numNodes, numNodesAlloc;        // number of nodes, and allocated amount
	Boolean8 pathexists;                 // whether the path has been built
//    std::unordered_map<int, int> pathNodes;  // set of used path nodes for easy
//                                             // look-up
//    EHashTable<NodeTableRef, EIntHashKey,
//    &NodeTableRef::coord, &NodeTableRef::link> pathNodes;
    int pathNodes[MAPSIZE][MAPSIZE];
	
	int addNode(const Node &node);
public:
	int addStartNode(int tx, int ty);
	int addStartNode(int tx, int ty, int destx, int desty,
                     Boolean8 negate = false);
	int bestScoreIndex() const;
	void finish(int index);
	int openCoordsIndex(int cx, int cy) const;
	int pathCoordsIndex(int cx, int cy) const;
	~PathArray();
	void updateNode(int ichange, int index, int cx, int cy, int dist);
	void updateNode(int ichange, int index, int cx, int cy, int dist,
                    int destx, int desty, Boolean8 negate = false);
    
    // Node closing accessor
	void closeNode(int index) {nodes[index].open = false; --numOpenNodes;}
    // pathexists accessor
	Boolean8 exists() const {return pathexists;}
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
	void makeEmpty()
    {
        numNodes = numOpenNodes = 0;
        memset(pathNodes, -1, sizeof(pathNodes));
    }
    //
    // Constructor
    //
	PathArray() : nodes(0), numOpenNodes(0), numNodes(0), numNodesAlloc(0),
    pathexists(false)
	{
        memset(pathNodes, -1, sizeof(pathNodes));
	}
    // pathexists disabling accessor
	void reset() {pathexists = false;}
};

#endif