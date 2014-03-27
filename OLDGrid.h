#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>
#include <OGDF/basic/Array.h>
#include <Lattice.h>

class Gridpoint
{
public:
	Gridpoint();
	node v_node; //the node for the vertical grid
	node h_node; //the node for the horizontal grid
	edge changelayer; //the edge connecting v_node and h_node
	node *occNode; //reference to the node that lies on this gridpoint
	edge *v_edge; //reference to the edge that goes through this gridpoint vertically
	edge *h_edge; //reference to the edge that goes through this gridpoint horizonatlly
	
	Gridpoint *Up; //pointer to the gridpoint above
	Gridpoint *Down; //pointer to the gridpoint below
	Gridpoint *Left; //pointer to the gridpoint to the left
	Gridpoint *Right; //pointer to the gridpoint to the right
};

/*
This class represnts the Grid as a 2-dim array of gridpoints, each of which
represents two nodes, one for horizontal and one for vertical gridlines with a connecting edge between those nodes.
I still need to figure out how to do arrays right and how to 
initialze a grid of predefined proportions and how to
resize in case a move operation leaves the grid, how to
search a shortest path and where if at all to save edgeweights
*/
class Grid: public Graph
{
protected:
	Array2D<Gridpoint> m_Grid; //The two-dimensional array of gridpoints
public:
	Grid(); //create a new grid with only one gridpoint
	
	
	/*returns the list of bendpoints for the shortest edge from A to B and 
	modifies the grid by deleting approriate h_nodes and v_nodes.
	After inserting the new edge into the grid, all edges that were crossed are informed that they have one less free space. 
	Edges that can't be crossed without losing the space necessary to reinsert the nodes saved in that edge modify the grid accordingly.
	The polyline of the edge is returned so that GridGraph can update the bendpoints of the edge.
	*/
	IPolyline findEdge(IPoint A, IPoint B);  
	void registerpoint(IPoint A);
	void registerline(IPolyline E);
	void registerfill(IPolyline E);
	void restorepoint(IPoint A); //removes nodes on point A and reinserts gridedges when appropriate
	void restoreline(IPolyline E); //reinserts gridedges and gridnodes along the line E
	void restorefill(IPolyline E); //repairs Grid in the area given by the outline E
	bool isFree(IPoint A); //Returns true if there is no node on gridpoint A
	bool isFree(IPolyline outline); //Returns true if there is no node on the grind inside the shape given by outline
	
	// EdgeArray<double> weight; //notwendig?
	
	/*
	some functions to resize the grid and make sure the data doesn't get corrupted?	
	*/

};




using namespace ogdf;

#endif