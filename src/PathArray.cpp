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
////////////////////////////////////////////////////////////////////////////////
//
// Bot pathfinding
//
////////////////////////////////////////////////////////////////////////////////

#include "wl_def.h"

#include "i_system.h"
#include "ioan_bas.h"
#include "PathArray.h"

////////////////////////////////////////////////////////////////////////////////
//
// Static functions
//
////////////////////////////////////////////////////////////////////////////////

//
// OnePacked
//
// Packs the two values into one
//
//inline static int OnePacked(int cx, int cy)
//{
//    return cx + (cy << sizeof(int) * 4);
//}

////////////////////////////////////////////////////////////////////////////////
//
// Private methods
//
////////////////////////////////////////////////////////////////////////////////

//
// PathArray::addNode
//
// Adds a node to the nodes array. It allocates if needed.
//
int PathArray::addNode(const Node &node)
{
	if(++numNodes > numNodesAlloc)
	{
		numNodesAlloc = 2*numNodes;
		nodes = (Node*)I_CheckedRealloc(nodes, numNodesAlloc * sizeof(Node));
	}
	
	nodes[numNodes - 1] = node;
    numOpenNodes += (int)node.open;
    pathNodes[node.tilex][node.tiley] = numNodes - 1;
//    pathNodes[OnePacked(node.tilex, node.tiley)] = numNodes - 1;
	return numNodes - 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// Public methods
//
////////////////////////////////////////////////////////////////////////////////

//
// PathArray::addStartNode
//
// Adds the first node, on FindShortestPath initialization
//
int PathArray::addStartNode(int tx, int ty)
{
    // Normal case: no known destination or repellent
	return addStartNode(tx, ty, tx, ty);
}

int PathArray::addStartNode(int tx, int ty, int destx, int desty, Boolean8 negate)
{
	Node node;
    
    // May be inaccurate
	int dist = Basic::ApproxDist((destx - tx) << 8, (desty - ty) << 8);
	
	// This resets and creates new data with possible negative distance
	node.g_score = 0;
	node.h_score = negate ? -dist : dist;
	node.f_score = node.g_score + node.h_score;
	node.open = true;
	node.tilex = tx;
	node.tiley = ty;
	node.prev = -1;
	node.next = -1;
	node.diagonal = 0;
	
	return addNode(node);
}

//
// PathArray::bestScoreIndex
//
// Returns the node with the minimum (best) distance score
//
int PathArray::bestScoreIndex() const
{
	int fmin = INT_MAX, imin = -1, j = numOpenNodes;
//    printf("%d ", numOpenNodes);
	for(int i = numNodes - 1; i >= 0; --i)
	{
		if(nodes[i].open)
        {
//            printf("%c", 'o');
            --j;
            if(nodes[i].f_score < fmin)
            {
                fmin = nodes[i].f_score;
                imin = i;
            }
            if(!j)
            {
                break;
            }
        }

	}
	return imin;
}

//
// PathArray::finish
//
// Finalizes the path, preparing the .next fields
//
void PathArray::finish(int index)
{
	nodes[index].next = -1;
	int index2 = index;
	for(index = nodes[index].prev; index != -1;
        index2 = index, index = nodes[index].prev)
    {
		nodes[index].next = index2;
    }
	pathexists = true;
}

//
// PathArray::openCoordsIndex
//
// Returns the index of the cx/cy coordinate, if open. Returns -2 if closed or
// -1 if non-existent.
//
int PathArray::openCoordsIndex(int cx, int cy) const
{
    int i = pathNodes[cx][cy];
//    printf("%d ", i);
    if (i >= 0 && !nodes[i].open)
        return -2;
    return i;
    
//    int aggr = OnePacked(cx, cy);
//    if (pathNodes.count(aggr))
//    {
//        int i = pathNodes.at(aggr);
//        if (!nodes[i].open)
//            return -2;
//        else
//            return i;
//    }
//    return -1;
    
//	for(int i = numNodes - 1; i >= 0; --i)
//	{
//		if(nodes[i].tilex == cx && nodes[i].tiley == cy)
//		{
//			if(!nodes[i].open)
//            {
////                printf("%d\t%d\n", i, numNodes);
////                printf("@");
//				return -2;
//            }
//			else
//            {
////                printf("%d\t%d\n", i, numNodes);
////                printf("@");
//				return i;
//            }
//		}
//	}
//    printf("!");
//	return -1;
}

//
// PathArray::pathCoordsIndex
//
// Returns the index of the cx/cy coordinate, if it's on a path, or -1 else
//
int PathArray::pathCoordsIndex(int cx, int cy) const
{
	int prev = 0;
	for(int i = 0; i != -1; i = nodes[i].next)
	{
		if(nodes[i].tilex == cx && nodes[i].tiley == cy)
			return i;

		switch(nodes[i].diagonal)
		{
			case 1:	// up-right
				if(cx == nodes[prev].tilex && cy == nodes[prev].tiley - 1)
					return prev;
				if(cx == nodes[prev].tilex + 1 && cy == nodes[prev].tiley)
					return prev;
				break;
			case 2:	// up-left
				if(cx == nodes[prev].tilex && cy == nodes[prev].tiley - 1)
					return prev;
				if(cx == nodes[prev].tilex - 1 && cy == nodes[prev].tiley)
					return prev;
				break;
			case 3:	// down-left
				if(cx == nodes[prev].tilex && cy == nodes[prev].tiley + 1)
					return prev;
				if(cx == nodes[prev].tilex - 1 && cy == nodes[prev].tiley)
					return prev;
				break;
			case 4:	// down-right
				if(cx == nodes[prev].tilex && cy == nodes[prev].tiley + 1)
					return prev;
				if(cx == nodes[prev].tilex + 1 && cy == nodes[prev].tiley)
					return prev;
				break;
		}
		prev = i;
	}
	return -1;
}

//
// PathArray::~PathArray
//
// Destructor
//
PathArray::~PathArray()
{
	if(nodes)
		free(nodes);
}

//
// PathArray::updateNode
//
// Updates the score of the given node, or even creates a new one
//
void PathArray::updateNode(int ichange, int index, int cx, int cy, int dist)
{
	updateNode(ichange, index, cx, cy, dist, cx, cy);
}

void PathArray::updateNode(int ichange, int index, int cx, int cy, int dist,
                           int destx, int desty, Boolean8 negate)
{
	dist += nodes[index].g_score;
	if(ichange == -1)
	{
		Node node;
		int apdist = Basic::ApproxDist((destx - cx) << 8, (desty - cy) << 8);
		node.g_score = dist;
		node.h_score = negate ? -apdist : apdist;
		node.f_score = node.g_score + node.h_score;
		node.open = true;
		node.tilex = cx;
		node.tiley = cy;
		node.prev = index;
		node.diagonal = 0;
		if(index >= 0)
		{
			if(cx > nodes[index].tilex && cy < nodes[index].tiley)
				node.diagonal = 1;
			else if(cx < nodes[index].tilex && cy < nodes[index].tiley)
				node.diagonal = 2;
			else if(cx < nodes[index].tilex && cy > nodes[index].tiley)
				node.diagonal = 3;
			else if(cx > nodes[index].tilex && cy > nodes[index].tiley)
				node.diagonal = 4;

		}
			   
		addNode(node);
	}
	else if(dist < nodes[ichange].g_score)
	{
		nodes[ichange].g_score = dist;
		nodes[ichange].f_score = dist + nodes[ichange].h_score;
		nodes[ichange].prev = index;
		nodes[ichange].diagonal = 0;
		
		if(index >= 0)
		{
			cx = nodes[ichange].tilex;
			cy = nodes[ichange].tiley;
			if(cx > nodes[index].tilex && cy < nodes[index].tiley)
				nodes[ichange].diagonal = 1;
			else if(cx < nodes[index].tilex && cy < nodes[index].tiley)
				nodes[ichange].diagonal = 2;
			else if(cx < nodes[index].tilex && cy > nodes[index].tiley)
				nodes[ichange].diagonal = 3;
			else if(cx > nodes[index].tilex && cy > nodes[index].tiley)
				nodes[ichange].diagonal = 4;
			else {
				nodes[ichange].diagonal = 0;
			}
			
		}
	}
}