#ifndef GRID
#define GRID



#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>




class Gridpoint
{
public:
	Gridpoint();
	node v_node; //the node for the vertical grid
	node h_node; //the node for the horizontal grid
	edge changelayer; //the edge connecting v_node and h_node


	//pretty sure none of them needed to be pointers, as they already are.
	//CHANGED: these were pointers twice
	node occNode; //reference to the node that lies on this gridpoint
	edge v_edge; //reference to the edge that goes through this gridpoint vertically
	edge h_edge; //reference to the edge that goes through this gridpoint horizonatlly
	

	//do we need these? 
	//CHANGED: commented them out.
	//Gridpoint *Up; //pointer to the gridpoint above
	//Gridpoint *Down; //pointer to the gridpoint below
	//Gridpoint *Left; //pointer to the gridpoint to the left
	//Gridpoint *Right; //pointer to the gridpoint to the right
};

class Grid: public Graph
{
protected:
	Array2D<Gridpoint> m_Grid; //The two-d	imensional array of gridpoints
public:
	Grid(); //create a new grid with only one gridpoint
	//CHANGED: added this second constructor
	Grid(int a, int b, int c, int d); //create new Grid with array [a..b]*[c..d] and initialise all the points.
	
	/*returns the list of bendpoints for the shortest edge from A to B and 
	modifies the grid by deleting approriate h_nodes and v_nodes.
	After inserting the new edge into the grid, all edges that were crossed are informed that they have one less free space. 
	Edges that can't be crossed without losing the space necessary to reinsert the nodes saved in that edge modify the grid accordingly.
	The polyline of the edge is returned so that GridGraph can update the bendpoints of the edge.
	*/
	IPolyline findEdge(IPoint A, IPoint B);  
	//CHANGED: camelcase'd these next six names
	void registerPoint(IPoint A);
	void registerLine(IPolyline E);
	void registerFill(IPolyline E);
	void restorePoint(IPoint A); //removes nodes on point A and reinserts gridedges when appropriate
	void restoreLine(IPolyline E); //reinserts gridedges and gridnodes along the line E
	void restoreFill(IPolyline E); //repairs Grid in the area given by the outline E
	bool isFree(IPoint A); //Returns true if there is no node on gridpoint A
	bool isFree(IPolyline outline); //Returns true if there is no node on the grind inside the shape given by outline
	
	// EdgeArray<double> weight; //notwendig?
	
	/*
	some functions to resize the grid and make sure the data doesn't get corrupted?	
	*/

};

#endif