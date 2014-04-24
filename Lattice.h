#ifndef LATTICE_H
#define LATTICE_H

#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>
#include <OGDF/basic/Array.h>

using namespace ogdf;



class Lattice: public Graph //FixFlag: add references to original edges to all edges so crossings can be conveyed to the calling function.
{
private:
	static unsigned int instanceCount; //counts the # of GridGraph instances to assign unique ids
	unsigned int m_id;
	unsigned int id() const{return m_id;}
	NodeArray<IPoint> nodePos;
	EdgeArray<int> height;
	EdgeArray<int> start;
	EdgeArray<int> end;
	EdgeArray<node> srcN; //init to this
	EdgeArray<edge> srcE; //init to this
	NodeArray<List<edge>> lineN; //init to GG
	EdgeArray<List<edge>> lineE; //init to GG

	List<edge> hor;
	List<edge> vert;
	List<node> Nodes; //lexicographically sorted array of nodes

	List<edge> m_outline;
	IPolyline m_pOutline;
	node getNode(IPoint pos); //get point at pos, create it otherwise
	bool isOutside(IPoint pos); 
	bool isOutside(DPoint pos); 
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
	void adjSort(adjEntry newAdj);
	edge getRight(edge e); //get the edge that's to the right of e, create a new edge if direction is wrong;
	void sortIn(edge e); //sorts e into hor and vert;
	void displayNodeInformation(node v);
public:		
	Lattice();
	Lattice(Graph &G);
	Lattice(const Lattice &L);
	Lattice& operator=(const Lattice &L);
	bool addLine(IPolyline line,node N); //adds outline line of N to Lattice, returns false and removes line again if there is a node inside.
	bool addLine(IPolyline line,node N,List<edge> &EList); //adds outline line of N to Lattice, returns false if there is a node inside.
	void addLine(IPolyline line,edge E); //adds Polyline to Lattice 
	void addLine(IPolyline line,edge E, List<edge> &EList); //adds Polyline to Lattice, gives back a list of crossed edges.
	void removeLine(IPolyline line);
	void removeLine(edge E);
	void removeLine(node N);
	List<edge> edgesOn(IPoint p); //returns the edge that lies on p if it exists. (might be 2 even 4)) 
	node clusterOn(IPoint p); //returns the cluster that p is inside of.
	void nodeInfoOn(IPoint p);
	bool isInside(IPoint p);
	bool isInside(DPoint p);
	IPolyline outline();
	IPolyline CalcOutline();
	void checkConsistency();
	void getConnectedElements(node N, List<edge> &edges, List<node> &nodes); 
	void getConnectedElements(edge E, List<edge> &edges, List<node> &nodes);
	void displayLatticeInformation();

};
#endif