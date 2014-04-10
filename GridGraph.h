#ifndef GRIDGRAPH_H
#define GRIDGRAPH_H

#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/Stack.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>
#include <OGDF/basic/Array.h>
#include <Lattice.h>
#include <OLDGrid.h>



using namespace ogdf;

/**
Open Problems:
Calculation of the outline.
Getting all points inside of the outline.
Finding out whether moving a node changes the outline and calculating that change.
finding clusters
(idea: use IntersectionRect for outline instead of/in addition to IPolyline)
simulated annealing
*/


/*
This is the main datastructure that SA_Grid operates on.
It contains graph where each node represents a set of nodes of the original graph.
Nodes of degree two can be removed and a reference to them saved in the resulting edge.
The outline of the GridGraph is saved and each node has a position and a shape.
Every node also represents another GridGraph which provides the shape of the node with its outline.
Gridgraph supports the finding and grouping together of suitable node sets.
The simulated annealing algorithm can then be called recursively on the sub-GridGraphs to find
the optimal shapes for the nodes.
Moving nodes and finding optimal edges is implemented using the Grid class.
*/




class GridGraph: public Graph
{
protected:
	const Graph *m_pGraph;   //!< The original graph.
	
	NodeArray<List<node>> m_vOrig; //!< The list of corresponding nodes in the original graph.
	EdgeArray<List<edge>> m_eOrig; //!< The corresponding path stored in the edges of the graph	
	NodeArray<node> m_vCopy; //!< The corresponding node in the GridGraph. (returns NULL if the node is saved in an edge or not part of this GG)
		
	EdgeArray<edge> m_eCopy; //! The corresponding edge in the GridGraph. (returns NULL if the node is not part of this GG)
	
	static unsigned int instanceCount; //counts the # of GridGraph instances to assign unique ids
	unsigned int m_id; // id of the object instance, used for comparing objects.
	

	Grid m_Grid; //!< the Grid object associated with the GridGraph
	EdgeArray<IPolyline> m_edgeline; //!< startingpos, list of bend points and endpos of an edge 
	EdgeArray<DPolyline> m_dedgeline; //!< startingpos, list of bend points and endpos of an edge 
	EdgeArray<int> m_eSpaces; //!< list of available spaces on the edge where you can re-insert a node that is saved in m_eOrig
	
	List<edge> m_eOutgoing; //!< list of outgoing edges of the graph. The edges in this list belong to the original Graph.
	List<node> m_vConnect; //!< list of dummynodes on the outline that specify the position of the connection for the outgoing edges. The order must be the same as m_eOutgoing.
	node getConnectNode(edge e);
	bool isDummy(node v); //!< returns true if v is a dummynode for the connection 
	List<node> m_nonDummy; //! list of all non-dummy nodes

	NodeArray<GridGraph *> m_vGridGraph; //!< Pointer to the corresponding GridGraph element that the nodes represent, NULL if node is atomic
	List<GridGraph> m_GGList; //List where the sub GridGraphs are saved.
	
	IPoint m_pos; //position of this GG's origin in coordinates of the topmost GG
	DPoint m_dPos; //position of this GG's origin in coordinates of the topmost GG
	NodeArray<int> m_x; //x-coord of the upperleft corner of the nodes 
	NodeArray<int> m_y; //y-coord of the upperleft corner of the nodes
	NodeArray<double> m_dx; //double x-coord of the upperleft corner of the nodes 
	NodeArray<double> m_dy; //double y-coord of the upperleft corner of the nodes
	NodeArray<char> m_vRotation; // values of 0 - 3, rotation CCW in multiples of 90°
	NodeArray<bool> m_vXMirror;
	NodeArray<bool> m_vYMirror;
	NodeArray<double> m_vDRotation; //continuous rotation of v for drawing animation frames
	NodeArray<double> m_vXScale; //continous realization of X-Mirror for drawing animation frames
	NodeArray<double> m_vYScale; //continous realization of Y-Mirror for drawing animation frames
	NodeArray<IPolyline> m_vOutline; //outline of v in relation to pos of v accounting for rotation and mirroring.

	//IPolyline m_outline; //The Shape of the Gridgraph (not necessarily maintained throughout annealing)
	Lattice m_lattice;
	
	NodeArray<char> m_vState; //returns -1 if node is invisible, 1 if node is temporary, 0 else 
	EdgeArray<char> m_eState; //returns -1 if edge is invisible, 1 if edge is temporary, 0 else
	
	bool m_IOprep; //shows if GG has been prepared by GraphIO for GUI output;
	/*starting with node v find a cluster of nodes and merge to a single node and GridGraph. 
	The parameter p indicates how close the cluster should be. 
	*/	

public:
	void findCluster(node v, int p); 
	List<node> findClusterRecurse(List<node> cluster, int p);
	void moveToCluster(GridGraph &GGw, node w, node v); //merges w (node of GGw) to v and updates the list of original nodes, the list of corresponding nodes and the list of gridgraphs
	void moveToCluster(GridGraph &GGw, node v); //merges w to v and updates the list of original nodes, the list of corresponding nodes and the list of gridgraphs

	List<node> GridGraph::trimCluster(List<node> U, node v);
private:

	
	
public:
	List<edge> eoutgoing(){return m_eOutgoing;};
	GridGraph(); //standard constructor, just make it throw an error to make sure it's not used
	GridGraph(const GridGraph &GG); // construct a GridGraph taht is a copy of GG
	GridGraph(const Graph &G, node orig); // Construct a Gridgraph that represents a single node v
	GridGraph(const Graph &G); //Costruct a GridGraph that represents a Graph G
	GridGraph& operator=(const GridGraph& GG);
	bool operator==(const GridGraph &GG) const{return (this->id() == GG.id());};
	//A destructor might prove neccessary in the end
	
	node addNode(node orig); //adds a node, returns the new node, takes care of consequential edges and connections
	edge addEdge(edge orig); //never used (?)
	void displayDebug();
	//returns a pointer to the Gridgraph represented by node v;
	GridGraph * GridGraph_of(node v){return m_vGridGraph[v];};

	//returns a random NON-DUMMY node from GridGraph.
	node chooseNonDummy(){return m_nonDummy.chooseElement();};
	
	bool isVisible(node v); //returns true if node is part of currently considered Layout
	bool isVisible(edge v); //returns true if edge is part of currently considered Layout
	bool isTemporary(node v); //returns true if node is currently being tested
	bool isTemporary(edge e); //returns true if edge is currently being tested

	void acceptPos(); //deletes all invisible nodes and edges, finalizes temporary nodes and edges
	void rejectPos(); //deletes all temporary nodes and edges, restores invisibles, reverts Grid to original state.

	IPolyline getBox(); //returns bottemleft and upperright cornerpoint of GG
	IPolyline GridGraph::getOutline(){ return m_lattice.outline();};
	void addToLattice(IPolyline line){m_lattice.addLine(line);};
	void removeFromLattice(IPolyline line){m_lattice.removeLine(line);};
	

	/*Initiate the GridGraph with a valid starting layout, preferrably with enough free space to move nodes around.
	This layout could be found by randomly choosing starting positions for the nodes and adding edges and then 
	relocating a node for each edge that couldn't be connected.
	Initiation should only occur when the nodes are already grouped together with optimal outline.
	This method could also handling the simplification of paths to edges and initialization of the grid (size: sqrt(n)*4 square ?).*/
	void init(); 
	
	/*tries to move the node v to the new location pos and reconnecting all edges severed in the process. 
	If the operation succeeds, hides old nodes/edges and creates temporary new ones.
	The Move fails if either the new position is not free of nodes or if it's impossible to
	reconnect the edges.
	rotation values: 0-3* 90° CCW, mirror values: 0:no mirroring, 1 horizontal 2 vertical, */	
	bool tryMove(node v, IPoint pos, int rotation, int mirror); 
	//bool forceMove(nodev, IPoint pos); possible future addition, force the move by recursively relocating nodes that are in the way.	
		
	
	/*
	call findCluster on all nodes, starting with ones of high degree. The parameter p for the clusterizsation 
	could be found dynamically, either by choosing p according to the number of nodes or by increasing p
	until a suitably small number of nodes has been achieved (note: don't count nodes of degree 2, those are just edges)
	*/
	void clusterize(int p = 0); 
	
	/* 
	GETTTERS AND SETTERS	
	*/

	
	bool &IOprep(){return m_IOprep;};
	const Graph& constGraph() const {return *m_pGraph;}
	List<node> &nonDummyNodes(){return m_nonDummy;}

	unsigned int id() const{return m_id;};
	int &x(node v){return m_x[v];};
	int &y(node v){return m_y[v];};
	double &dx(node v){return m_dx[v];};
	double &dy(node v){return m_dy[v];};
			
	IPoint getPos(node v){return IPoint(m_x[v],m_y[v]);}; //!< position of node v (NOT NECESSARILY upper-left corner of the bounding box, just origin of GG coord-sys)
	IPoint &getPos(){return m_pos;};
	void setPos(node v, IPoint pos){m_x[v] = pos.m_x; m_y[v] = pos.m_y;}; //!< Set the position of node v without consideration for the Grid
	DPoint &dPos(){return m_dPos;}; //position of GG's origin in relation to global coord

	char &rot(node v){return m_vRotation[v];};	
	double &dRot(node v){return m_vDRotation[v];};

	bool &xMirror(node v){return m_vXMirror[v];};
	bool &yMirror(node v){return m_vYMirror[v];};
	double &xScale(node v){return m_vXScale[v];};
	double &yScale(node v){return m_vYScale[v];};


	IPolyline getEdgeline(edge e){return m_edgeline[e];}; //EIGENTLICH OBSOLET
	IPolyline &edgeline(edge e){return m_edgeline[e];};
	DPolyline &dedgeline(edge e){return m_dedgeline[e];};
	void setEdgeline(edge e, IPolyline line){m_edgeline[e] = line;}; // manually set the edgeline without consideration for the Grid	//EIGENTLICH OBSOLET
	//IPolyline &getOutline(){return m_outline;}; implemented above
	IPolyline &getOutline(node v){return m_vOutline[v];};

	List<GridGraph> &GGList(){return m_GGList;};
	GridGraph * subGG(node v){return m_vGridGraph[v];};
	IPoint getConnection(edge e); //!<given an outgoing edge e returns the position of the connection of the edge.

	List<node> original(node v){return m_vOrig[v];}; 
	const List<node> original(node v) const{return m_vOrig[v];}; 
	List<edge> original(edge e){return m_eOrig[e];}; 
	const List<edge> original(edge e) const{return m_eOrig[e];}; 
	node Copy(node v){return m_vCopy[v];};
	edge Copy(edge e){return m_eCopy[e];};
	node const Copy(node v) const{return m_vCopy[v];};
	edge const Copy(edge e) const{return m_eCopy[e];};
};

#endif