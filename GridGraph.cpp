#include "stdafx.h"



#include <GridGraph.h>



GridGraph::GridGraph(){}; //DONE!

GridGraph::GridGraph(const Graph &G, node orig){ //DONE!
	m_pGraph = &G;
	m_vOrig.init(*this);	
	m_eOrig.init(*this);
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	
	m_vState.init(*this, 0);
	m_eState.init(*this, 0);

	m_edgeline.init(*this);
	m_dedgeline.init(*this);
	m_eSpaces.init(*this, -1);

	m_x.init(*this,-1);
	m_y.init(*this,-1);
	m_dx.init(*this,-1);
	m_dy.init(*this,-1);
	m_vRotation.init(*this,0);
	m_vXMirror.init(*this,false);
	m_vYMirror.init(*this,false);
	m_vDRotation.init(*this,0);
	m_vXScale.init(*this,1);
	m_vYScale.init(*this,1);
	m_vOutline.init(*this);
	m_vGridGraph.init(*this, NULL);	

	node v = this->newNode();
	m_nonDummy.pushFront(v);

	m_vCopy[orig] = v; 
	m_vOrig[v].pushFront(orig);

	
	adjEntry adj;
	forall_adj(adj,orig){ //for all adjacent 
		m_eOutgoing.pushFront(adj->theEdge()); //new outgoing edge
		node connect = this->newNode();

		m_vConnect.pushFront(connect);				
		edge e = newEdge(v,connect);



	}



};

GridGraph::GridGraph(const Graph &G){ // DONE!
	m_pGraph = &G;
	m_vOrig.init(*this);	
	m_eOrig.init(*this);	
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	m_vState.init(*this, 0);
	m_eState.init(*this, 0);

	m_edgeline.init(*this);
	m_dedgeline.init(*this);
	m_eSpaces.init(*this, -1);
	
	m_x.init(*this,-1);
	m_y.init(*this,-1);
	m_dx.init(*this,-1);
	m_dy.init(*this,-1);
	m_vRotation.init(*this,0);
	m_vXMirror.init(*this,false);
	m_vYMirror.init(*this,false);
	m_vDRotation.init(*this,0);
	m_vXScale.init(*this,1);
	m_vYScale.init(*this,1);
	m_vOutline.init(*this);
	m_vGridGraph.init(*this, NULL);
			
	node orig;
	forall_nodes(orig,*m_pGraph){ //for all original nodes
		node v = this->newNode(); //create a new GG node
		m_nonDummy.pushFront(v);

		m_vCopy[orig] = v;  //connect original and GG
		m_vOrig[v].pushFront(orig);		

		adjEntry adj;
		forall_adj(adj,orig){ //for all adjacent original nodes w
			node w = m_vCopy[adj->twinNode()]; 
			if (w){ //if w is already in GG
				edge e = this->newEdge(v,w); //create the new edge
				m_eCopy[adj->theEdge()] = e; //connect original and GG
				m_eOrig[e].pushFront(adj->theEdge());				
			}			
		}
	}
};

GridGraph::GridGraph(const GridGraph &GG){ 

	//Standard GridGraph initialization
	m_pGraph = &GG.constGraph();
	m_vOrig.init(*this);	
	m_eOrig.init(*this);	
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	m_vState.init(*this, 0);
	m_eState.init(*this, 0);

	m_edgeline.init(*this);
	m_dedgeline.init(*this);
	m_eSpaces.init(*this, -1);
	
	m_x.init(*this,-1);
	m_y.init(*this,-1);
	m_dx.init(*this,-1);
	m_dy.init(*this,-1);
	m_vRotation.init(*this,0);
	m_vXMirror.init(*this,false);
	m_vYMirror.init(*this,false);
	m_vDRotation.init(*this,0);
	m_vXScale.init(*this,1);
	m_vYScale.init(*this,1);
	m_vOutline.init(*this);
	m_vGridGraph.init(*this, NULL);
	
			

	//Copying nodes, edges, subGGs and Attributes from source
	node v1;
	NodeArray<node> srcCopy(GG,NULL);
	NodeArray<node> trgCopy(*this,NULL);
	forall_nodes(v1,GG){ //for all original nodes of GG
		node v2 = this->newNode(); //create a copy node
		srcCopy[v2] = v1;
		trgCopy[v1] = v2;
		m_nonDummy.pushFront(v2);
		m_vOrig[v2] = GG.m_vOrig[v1]; //copy original node list
		
		ListIterator<node> it;
		forall_listiterators(node, it, m_vOrig[v2]){ //copy copy node list
			m_vCopy[*it] = v2;
		}				

		//copy all other node attributes		
		m_x[v2] = GG.m_x[v1];
		m_y[v2] = GG.m_y[v1];
		m_dx[v2] = GG.m_dx[v1];
		m_dy[v2] = GG.m_dy[v1];
		m_vRotation[v2] = GG.m_vRotation[v1];
		m_vXMirror[v2] = GG.m_vXMirror[v1];
		m_vYMirror[v2] = GG.m_vYMirror[v1];
		m_vDRotation[v2] = GG.m_vDRotation[v1];
		m_vXScale[v2] = GG.m_vXScale[v1];
		m_vYScale[v2] = GG.m_vYScale[v1];
		m_vOutline[v2] = GG.m_vOutline[v1];
		m_vState[v2] = GG.m_vState[v1];

		if (GG.m_vGridGraph[v1]){
			m_GGList.pushFront(GridGraph(*GG.m_vGridGraph[v1])); //make a copy of any sub-GG
			m_vGridGraph[v2] = &m_GGList.front();
		}


		adjEntry adj;
		forall_adj(adj,v1){ //for all adjacent source nodes 
			node w2 = trgCopy[adj->twinNode()];
			edge e1 = adj->theEdge();
			if (w2){ //if w2 is already in GG
				edge e2 = this->newEdge(v2,w2); //create the new edge
				ListIterator<edge> ite;
				m_eOrig[e2] = GG.m_eOrig[e1]; //copy original edge list
				forall_listiterators(edge, ite, m_eOrig[e2]){ // copy copy edge list
					m_eCopy[*ite] = e2;
				}
				//copy all other edge attributes
				m_edgeline[e2] = GG.m_edgeline[e1];
				m_eSpaces[e2] = GG.m_eSpaces[e1];
				m_eState[e2] = GG.m_eState[e1];				
			}			
		}
	}
	//copy all general members
	m_Grid = GG.m_Grid;
	m_dPos = GG.m_dPos;
	
};


GridGraph& GridGraph::operator=(const GridGraph& GG){
	this->clear();
	//Standard GridGraph initialization
	m_pGraph = &GG.constGraph();
	m_vOrig.init(*this);	
	m_eOrig.init(*this);	
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	m_vState.init(*this, 0);
	m_eState.init(*this, 0);

	m_edgeline.init(*this);
	m_dedgeline.init(*this);

	m_eSpaces.init(*this, -1);
	
	m_x.init(*this,-1);
	m_y.init(*this,-1);
	m_dx.init(*this,-1);
	m_dy.init(*this,-1);
	m_vRotation.init(*this,0);
	m_vXMirror.init(*this,false);
	m_vYMirror.init(*this,false);
	m_vDRotation.init(*this,0);
	m_vXScale.init(*this,1);
	m_vYScale.init(*this,1);
	m_vOutline.init(*this);
	m_vGridGraph.init(*this, NULL);
	
			

	//Copying nodes, edges, subGGs and Attributes from source
	node v1;
	NodeArray<node> srcCopy(*this,NULL); //get source of node
	NodeArray<node> trgCopy(GG,NULL); //get target of node
	forall_nodes(v1,GG){ //for all original nodes of GG
		node v2 = this->newNode(); //create a copy node
		srcCopy[v2] = v1;
		trgCopy[v1] = v2;
		m_nonDummy.pushFront(v2);
		m_vOrig[v2] = GG.m_vOrig[v1]; //copy original node list
		
		ListIterator<node> it;
		forall_listiterators(node, it, m_vOrig[v2]){ //copy copy node list
			m_vCopy[*it] = v2;
		}				

		//copy all other node attributes		
		m_x[v2] = GG.m_x[v1];
		m_y[v2] = GG.m_y[v1];
		m_dx[v2] = GG.m_dx[v1];
		m_dy[v2] = GG.m_dy[v1];
		m_vRotation[v2] = GG.m_vRotation[v1];
		m_vXMirror[v2] = GG.m_vXMirror[v1];
		m_vYMirror[v2] = GG.m_vYMirror[v1];
		m_vDRotation[v2] = GG.m_vDRotation[v1];
		m_vXScale[v2] = GG.m_vXScale[v1];
		m_vYScale[v2] = GG.m_vYScale[v1];
		m_vOutline[v2] = GG.m_vOutline[v1];
		m_vState[v2] = GG.m_vState[v1];
		if (GG.m_vGridGraph[v1]){
			m_GGList.pushFront(GridGraph(*GG.m_vGridGraph[v1])); //make a copy of any sub-GG
			m_vGridGraph[v2] = &m_GGList.front();
		}


		adjEntry adj;
		forall_adj(adj,v1){ //for all adjacent source nodes 

			node w2 = trgCopy[adj->twinNode()];			
			edge e1 = adj->theEdge();
			if (w2 != NULL){ //if w2 is already in new GG
				edge e2 = this->newEdge(v2,w2); //create the new edge
				ListIterator<edge> ite;
				m_eOrig[e2] = GG.m_eOrig[e1]; //copy original edge list
				forall_listiterators(edge, ite, m_eOrig[e2]){ // copy copy edge list
					m_eCopy[*ite] = e2;
				}
				//copy all other edge attributes
				m_edgeline[e2] = GG.m_edgeline[e1];
				m_eSpaces[e2] = GG.m_eSpaces[e1];
				m_eState[e2] = GG.m_eState[e1];				
			}			
		}
	}
	//copy all general members
	m_Grid = GG.m_Grid;
	m_dPos = GG.m_dPos;
	return *this;
}


IPolyline GridGraph::getBox(){
	int min_x = 10000;
	int min_y = 10000;
	int max_x = -10000;
	int max_y = -10000;
	
	ListIterator<IPoint> it;
	node v;
	forall_nodes(v, *this){
		min_x = min(min_x, m_x[v]);
		max_x = max(max_x, m_x[v]);
		min_y = min(min_y, m_y[v]);
		max_y = max(max_y, m_y[v]);
		forall_listiterators(IPoint,it,m_vOutline[v]){
			IPoint p = *it;
			min_x = min(min_x, m_x[v]+p.m_x);
			max_x = max(max_x, m_x[v]+p.m_x);
			min_y = min(min_y, m_y[v]+p.m_y);
			max_y = max(max_y, m_y[v]+p.m_y);
		}				
	}
	edge e;
	forall_edges(e,*this){
		forall_listiterators(IPoint,it,m_edgeline[e]){
			IPoint p = *it;
			min_x = min(min_x, p.m_x);
			max_x = max(max_x, p.m_x);
			min_y = min(min_y, p.m_y);
			max_y = max(max_y, p.m_y);
		}				
	}
	IPolyline box;
	box.pushFront(IPoint(max_x,max_y));
	box.pushFront(IPoint(min_x,min_y));
	return box;
}

void GridGraph::findCluster(node v, int p){}; 

node GridGraph::addNode(node orig){ 
	node v = this->newNode();
	m_nonDummy.pushFront(v);
	m_vOrig[v].pushFront(orig);
	m_vCopy[orig] = v;

	adjEntry adj;
	forall_adj(adj,orig){
		node w = m_vCopy[adj->twinNode()];
		if (w) {
			edge e = newEdge(v,w);
			m_eOrig[e].pushFront(adj->theEdge());
			m_eCopy[adj->theEdge()] = e;
		}else{
			m_eOutgoing.pushFront(adj->theEdge());
			node connect = newNode();
			edge e = newEdge(v,connect);
			m_eCopy[adj->theEdge()] = e;
			m_eOrig[e].pushFront(adj->theEdge());
			m_vConnect.pushFront(connect);
		}
	}
	return v;
}

edge GridGraph::addEdge(edge orig){
	edge e = this->newEdge(Copy(orig->source()), Copy(orig->target()));
	m_eOrig[e].pushFront(orig);
	m_eCopy[orig] = e;
	return e;
}

void GridGraph::moveToCluster(node w, node v){ //not quite done! (?)
	if (m_vOrig[w].size() != 1) {
		std::cout << "ERROR: Trying to move whole cluster to another cluster (not implemented!)" << std::endl;
		throw -1;
	}
	node orig = m_vOrig[w].front();
	if (GridGraph_of(v) == NULL) {
		m_GGList.pushFront(GridGraph(*m_pGraph, m_vOrig[v].front() ));
		m_vGridGraph[v] = &m_GGList.front();
	}
	
	GridGraph &GG = *GridGraph_of(v);
	
	node w2 = GG.addNode(m_vOrig[w].front()); //create the new node


	IPoint wPos(0,0);
	IPoint vPos(0,0);
	IPoint relPos(0,0);
	if (m_x[v] != -1) { // if v has a position give the corr. node in GG a position.
		
		GG.setPos(GG.Copy(orig), IPoint(0,0)); 	

		if (m_x[w] != -1){ //if w has a position, give w2 in GG that position and calc outline
			wPos = this->getPos(w);
			vPos = this->getPos(v);
			relPos = IPoint(wPos.m_x - vPos.m_x,wPos.m_y - vPos.m_y);
		
			GG.setPos(w2,relPos); // move it to position
		}
	

	}
	// TO DO: Update Outline
	List<edge> adjEdges;
	adjEntry adj;
	forall_adj(adj, orig){ //forall adjacent nodes in original
		node thisnode = m_vCopy[adj->twinNode()]; //get adjacent in this
		edge thisedge = m_eCopy[adj->theEdge()]; 
		adjEdges.pushFront(thisedge);
		node nodeGG = GG.Copy(adj->twinNode()); // get adjacent in GG		
		edge edgeGG = GG.Copy(adj->theEdge());
		if(nodeGG){ // if the adjacent node is already part of the cluster
			 // TODO: properly get edge geometry
			m_eCopy[adj->theEdge()] = NULL;
			IPolyline line;
			ListIterator<IPoint> it;
			forall_listiterators(IPoint,it,m_edgeline[thisedge]){ //copy edgepath (edge is not empty iff nodes have positions. i hope.)
				IPoint point = (*it);
				point.m_x -= vPos.m_x;
				point.m_y -= vPos.m_y;
				line.pushBack(point);
			}
		}else{ //else reroute edges in this
			edge e;
			if (thisedge->source() == w){
				e = newEdge(v,thisedge->target());				
			}else{
				e = newEdge(thisedge->source(),v);
			}
			
			m_eCopy[adj->theEdge()] = e;
			ListIterator<edge> it;
			forall_listiterators(edge, it, m_eOrig[thisedge]){
				m_eOrig[e].pushBack(*it);
			}
			ListIterator<IPoint> it2;
			forall_listiterators(IPoint, it2, m_edgeline[thisedge]){
				m_edgeline[e].pushBack(*it2);
			}						
		}			
		delEdge(thisedge);
	}


	//delete w from this
	m_vCopy[orig] = NULL;
	//m_vState[w] = 0;
	//m_vOrig[w].clear();
	int pos = m_nonDummy.search(w);
	m_nonDummy.del(m_nonDummy.get(pos));
	delNode(w);


};
void GridGraph::init(){};

bool GridGraph::tryMove(node v, IPoint pos, int rotation, int mirror){return false;};
IPoint GridGraph::getConnection(edge e){return IPoint(0,0);};
void GridGraph::clusterize(int p){};

int GridGraph::outlineArea(IPolyline Boundingbox, IPolyline Outline)
{
}








//SA_Grid::SA_Grid(){
//	//set default settings
//}; 
//void SA_Grid::call(GridGraph &GG){
//	m_GG = &GG;
//	doWork();
//};
//void SA_Grid::doWork(){
//	node v;
//	ListIterator<SA_Grid> it;
//	List<SA_Grid> threads;
//	forall_nodes(v,*m_GG){
//		SA_Grid newthread;
//		threads.pushFront(newthread);
//		//maybe change some settings based on stuff like size of m_GG->Gridgraph(v) or something
//		newthread.call(m_GG->GridGraph_of(v));
//	}
//	//wait for all threads to finish, preferably using ogdf::Thread features
//	
//	//maybe output the recursiondepth and the calculation time;
//	startAnnealing();
//
//	
//}; 
//unsigned long SA_Grid::startAnnealing(){
//	//do fancy stuff
//	return 0;
//}; 
//
