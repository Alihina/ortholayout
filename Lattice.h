#ifndef LATTICE_H
#define LATTICE_H

#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>
#include <OGDF/basic/Array.h>

using namespace ogdf;



class Lattice: public Graph
{
private:
	NodeArray<IPoint> nodePos;
	EdgeArray<int> height;
	EdgeArray<int> start;
	EdgeArray<int> end;
	List<edge> hor;
	List<edge> vert;
	List<node> Nodes; //lexicographically sorted array of nodes

	List<edge> m_outline;
	IPolyline m_pOutline;
	node getNode(IPoint pos); //get point at pos, create it otherwise
	bool isOutside(IPoint pos); 
	List<edge> edges(IPoint p1, IPoint p2); //return list of edges between p1 and p2 (open intervall)
	node splitEdge(edge e, int pos);
	int getHeight(IPoint p1, IPoint p2);
	edge connect(node v1, node v2); // insert edge between v1 v2, sort in the new edge (rot`?)
	bool onOutline(node v);
	int getRot(edge e1, edge e2);  //get rotation between adjacent edges
	int getRot(node v1, node v2); //get rotation along outline
	void flip(List<edge> &edges); //flip each edge and reverse order of list
	void addToOutline(List<edge> &newOutline); //adds newOutline to outline, removes old outline
	void addToOutlineSimple(List<edge> &newOutline); //adds newOutline to outline
	edge getprevEdge(node v); //return edge on outline that points to v	
	ListIterator<edge> findST(List<edge> &edgelist, int pos);
	ListIterator<edge> findBT(List<edge> &edgelist, int pos);
	void merge(node v1, node v2); //merge two nodes that share position
	char adjPos(node v, edge e);// returns the direction of e going from N-in = 0 CCW to N-out = 7
	void adjSort(node v, adjEntry newAdj);
	edge getRight(edge e); //get the edge that's to the right of e, create a new edge if direction is wrong;

public:	
	Lattice();
	void addLine(IPolyline line); //adds Polyline to Lattice and updates BBox and outline
	void removeLine(IPolyline line);
	IPolyline outline();
	IPolyline CalcOutline();


};
#endif