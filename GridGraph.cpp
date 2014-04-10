#include "stdafx.h"



#include <GridGraph.h>
#include "IPointComparer.h"//zur OutlineBerechnung

unsigned int GridGraph::instanceCount = 0;

GridGraph::GridGraph(){
	m_id = instanceCount;
	instanceCount++;
}; //DONE!

GridGraph::GridGraph(const Graph &G, node orig){ //DONE!
	m_id = instanceCount;
	instanceCount++;
	
	std::cout << "SINGLENODE GGid: " << m_id << std::endl;
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
		std::cout << "crt conn " << adj->theEdge()<<std::endl;
		node connect = this->newNode();

		m_vConnect.pushFront(connect);				
		edge e = newEdge(v,connect);
		std::cout << "outgoing list:" << std::endl;
		forall_listiterators(edge,it,m_eOutgoing){
			std::cout << "edge: "<<*it<<" id: " << (*it)->index() << std::endl;
		}



	}



};

GridGraph::GridGraph(const Graph &G){ // DONE!
	m_id = instanceCount;
	instanceCount++;
	std::cout << "WHOLEGRAPH GGid: " << m_id << std::endl;
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
	m_id = instanceCount;
	instanceCount++;
	std::cout << "GGCOPY GGid: " << m_id << std::endl;
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
			if (w2 != NULL){ //if w2 is already in GG
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
	m_eOutgoing = GG.m_eOutgoing;
	m_vConnect = GG.m_vConnect;
	m_lattice = GG.m_lattice;
	

};


GridGraph& GridGraph::operator=(const GridGraph& GG){
	this->clear();
	//Standard GridGraph initialization
	std::cout << "ASSIGNMENT: " << m_id << std::endl;
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
				std::cout << e2 << " ";
				forall_listiterators(IPoint,it,GG.m_edgeline[e1]){
					std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
				}
				std::cout << std::endl;
				m_edgeline[e2] = GG.m_edgeline[e1];
				m_eSpaces[e2] = GG.m_eSpaces[e1];
				m_eState[e2] = GG.m_eState[e1];				
			}			
		}
	}
	//copy all general members
	m_Grid = GG.m_Grid;
	m_dPos = GG.m_dPos;

	//Debug information
	adjEntry adj;
	node v = this->firstNode();
	std::cout << "adjacent edgelines Copy: " << std::endl;
	forall_adj(adj,v){
		IPolyline line = this->edgeline(adj->theEdge());
		DPolyline dline = this->dedgeline(adj->theEdge());
		/*std::cout << adj->theNode() << "->" << adj->twinNode() << " = ";*/
		std::cout << adj->theEdge() << ":  {";
		forall_listiterators(IPoint,it,line){
			std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
		}
		std::cout << std::endl;
	}

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

List<node> GridGraph::findClusterRecurse(List<node> cluster, int p){
	if (p == 0) return cluster;
	List<node> add;
	forall_listiterators(node, it, cluster){
		node v = *it;
		adjEntry adj;
		forall_adj(adj, v){
			node w = adj->twinNode();
			if (cluster.search(w) == -1){
				if (add.search(w) == -1) add.pushFront(w);
			}
		}
	}
	cluster.conc(add);
	return findClusterRecurse(cluster, p-1);

}

void GridGraph::findCluster(node v, int p){
	List<node> cluster;
	//DEBUG
	List<node> nodes;
	this->allNodes(nodes);
	// /DEBUG

	cluster.pushFront(v);
	std::cout << "cluster initially:" << std::endl;
	forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	cluster = findClusterRecurse(cluster, p);
	std::cout << "cluster after BFS:" << std::endl;
	forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	cluster = trimCluster(cluster,v);
	std::cout << "cluster after trim:"<<  std::endl;
	forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	forall_listiterators(node,it,cluster){
		if (*it != v){		
			moveToCluster(*it,v);
		}
	}

}; 

List<node> GridGraph::trimCluster(List<node> U, node v) {
	//pseudocode:
	//Do DFS:
	Stack<edge> DFSstack;
	Stack<node> DFSparent; //always push and pop both!
	edge e;
	edge f;
	node n;
	node m;
	forall_adj_edges(e,v) {
		DFSstack.push(e);
		DFSparent.push(v);
	}
	EdgeArray<bool> e_visited(*this,false);
	EdgeArray<node> direct(*this);
	NodeArray<bool> v_visited(*this,false);
	NodeArray<node> parent(*this);
	List<edge> backedges;
	parent[v] = 0;
	v_visited[v] = true;

	while (!DFSstack.empty()) {
		e = DFSstack.pop();
		n = DFSparent.pop();
		m = e->opposite(n);
		if (v_visited[m] == false){ 
			if (U.search(m)==-1) {
				continue;
			}
			else {
				direct[e] = n;
				e_visited[e] = true;
				parent[m] = n;
				forall_adj_edges(f,m) {
					if (e_visited[f]==false) {
						DFSstack.push(f);
						DFSparent.push(m);
					}
				}
			}
		}
		else {
			e_visited[e] = true;
			direct[e] = n;
			backedges.pushBack(e);
		}
	}

	bool done = false;
	NodeArray<bool> marked(*this,false);
	ListIterator<edge> i;
	marked[v] = true;
	while (!done) {
		done = true;
		for( ListIterator<edge> it = backedges.begin(); it.valid(); ++it) {
			if ( marked[(*it)->opposite(direct[(*it)])] ) {
				n = direct[(*it)];
				while (marked[n]==false) {
					marked[n] = true;
					n = parent[n];
				}
				i = it;
				++it; //oder --it? macht glaub ich nichts aus.
				backedges.del(i);
				done = false;
			}
		}
	}
	List<node> component;
	forall_nodes(n,*this) {
		if (marked[n]) {
			component.pushBack(n);
		}
	}
	return component;
}

node GridGraph::getConnectNode(edge e){
		int pos = m_eOutgoing.search(e);
		//std::cout << "outgoing list:" << std::endl;
		//forall_listiterators(edge,it,m_eOutgoing){
		//	std::cout << "OLedge: "<<*it<<" id: " << (*it)->index()  << std::endl;
		//}
		//std::cout << "edge: "<<e<<"pos: "<< pos << " id: " << e->index() << std::endl;
		return *(m_vConnect.get(pos));
	}

node GridGraph::addNode(node orig){ 
	node v = this->newNode();
	m_nonDummy.pushFront(v);
	m_vOrig[v].pushFront(orig);
	m_vCopy[orig] = v;
	//std::cout << "AddNodeGGid: " << m_id << std::endl;
	//std::cout << "outgoing list:" << std::endl;
	//
	//forall_listiterators(edge,it,m_eOutgoing){
	//			std::cout << "OLedge: "<<*it<<" id: " << (*it)->index()  << std::endl;
	//}

	adjEntry adj;
	forall_adj(adj,orig){
		node w = m_vCopy[adj->twinNode()];
		/*std::cout << "debug" << w;*/
		if (w) {
		/*	forall_listiterators(edge,it,m_eOutgoing){
				std::cout << "OLedge: "<<*it<<" id: " << (*it)->index()  << std::endl;
			}*/
			node conn = getConnectNode(adj->theEdge()); //should always work because v->w has been outgoing edge
			m_eOutgoing.del(m_eOutgoing.get(m_eOutgoing.search(adj->theEdge())));
			m_vConnect.del(m_vConnect.get(m_vConnect.search(conn)));
			delNode(conn);

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



void GridGraph::moveToCluster(node w, node v){ // should be done I HOPE!!
	/*if (m_vOrig[w].size() != 1) {
		std::cout << "ERROR: Trying to move whole cluster to another cluster (not implemented!)" << std::endl;
		throw -1;
	}*/
	std::cout << "ATTEMPTING TO MOVE NODE " << w << " INTO CLUSTER " << v << std::endl;
	std::cout << "w is " << w << " degree "<< w->degree() << std::endl;
	List<node> origList = m_vOrig[w];
	//List<adjEntry> connAdj;
	//forall_listiterators(node,it,origList){ //get the list of adjacent nodes (adjEntries) of the cluster in original
	//	node orig = *it;
	//	adjEntry adj;
	//	forall_adj(adj,orig){
	//		if (origList.search(adj->twinNode()) == -1){
	//			if (connAdj.search(adj) == -1) connAdj.pushFront(adj);
	//		}
	//	}
	//}
	std::cout << "before new GG w is " << w << " degree "<< w->degree() << std::endl;
	if (GridGraph_of(v) == NULL) {
		std::cout << "stillBeforew is " << w << " degree "<< w->degree() << std::endl;
		GridGraph newGG = GridGraph(*m_pGraph, m_vOrig[v].front());
		std::cout << "beforeList is " << w << " degree "<< w->degree() << std::endl;
		m_GGList.pushFront(newGG);
		std::cout << "afterListw is " << w << " degree "<< w->degree() << std::endl;
		m_vGridGraph[v] = &m_GGList.front();
		std::cout << "w is " << w << " degree "<< w->degree() << std::endl;
	}
	std::cout << "after new GG w is " << w << " degree "<< w->degree() << std::endl;		


	GridGraph &GG = *GridGraph_of(v);
	//if (GG.eoutgoing().empty()) std::cout << "EMPTY" << std::endl;
	//else std::cout << "size: " << GG.eoutgoing().size() << std::endl;
	//std::cout << GG.numberOfNodes();
	//forall_listiterators(edge,it,GG.eoutgoing()){
	//		std::cout << "edge: "<<*it<<" id: " << (*it)->index() << std::endl;
	//	}
	GridGraph * p_srcGG = GridGraph_of(w);
	std::cout << "w is " << w << " degree "<< w->degree() << std::endl;
	removeFromLattice(GG.getOutline());
		

	std::cout << "w is " << w << " degree "<< w->degree() << std::endl;
	int loopcount = 0;
	forall_listiterators(node,it,origList){ //add all nodes from subGG of w
		std::cout << "LOOPCOUNT: " << loopcount++  <<  " w is " << w << std::endl;
		node orig = *it;		
		std::cout << "w is " << w << " degree "<< w->degree() << std::endl;
		node w2 = GG.addNode(orig); //create the new node aswell as adj edges and connections
		std::cout << "w is " << w << " degree "<< w->degree() << std::endl;
		m_vOrig[v].pushFront(orig);
		m_vCopy[orig] = v;
		
		std::cout << "w is " << w << std::endl;

		IPoint wPos(0,0);
		IPoint vPos(0,0);
		IPoint relPos(0,0);
		// if v has a position give the corr. node in GG a position.
		std::cout << "w is " << w << std::endl;
		GG.setPos(GG.Copy(orig), IPoint(0,0)); 	
		//if (m_x[w] != -1){ //if w has a position, give w2 in GG that position and calc outline
		if (p_srcGG) {
			wPos.m_x = this->getPos(w).m_x + GG.getPos(GG.Copy(orig)).m_x;
			wPos.m_y = this->getPos(w).m_y + GG.getPos(GG.Copy(orig)).m_y;
		}
		else wPos = this->getPos(w);
		std::cout << "w is " << w << std::endl;
		vPos = this->getPos(v);
		relPos = IPoint(wPos.m_x - vPos.m_x,wPos.m_y - vPos.m_y);
		GG.setPos(w2,relPos); // move it to position				
		edge e;
		edge e2;
		std::cout << "w is " << w << std::endl;
		forall_adj_edges(e,w2){
			if (e2 = this->Copy(GG.original(e).front())){
				GG.edgeline(e) = this->edgeline(e2);
			}
			if (p_srcGG){
				if (e2 = p_srcGG->Copy(GG.original(e).front())){
					GG.edgeline(e) = p_srcGG->edgeline(e2);
				}
			}
		}
		std::cout << "w is " << w << std::endl;

	}
	std::cout << "FINISHED ORIGLIST LOOPw is " << w << std::endl;
	//	forall_adj(adj,orig){ ALL THIS SHIT IS FOR PREDEFINED GEOMETRY, DON'T NEED THAT
	//		//node thisnode = m_vCopy[adj->twinNode()]; //get adjacent in this
	//		//edge thisedge = m_eCopy[adj->theEdge()]; 
	//		//adjEdges.pushFront(thisedge);
	//		//node nodeGG = GG.Copy(adj->twinNode()); // get adjacent in GG		
	//		//edge edgeGG = GG.Copy(adj->theEdge());
	//		
	//		if(GG.Copy(adj->twinNode())){//Case1: adjNode is in GG
	//		}
	//			//remove connection of GG
	//			//add Edge
	//			//Case 1a: adjNode is in origList == is in SrcGG
	//				//Copy edgepath from SrcGG
	//			//Case 1b: adjNode is not in origList{
	//				//Copy Edgepath from this
	//		//Case2: adjNode is not in this:
	//			//Case2a: adjNode is in origList == is in SrcGG
	//			edge e;
	//			if (thisedge->source() == w) e = newEdge(v,thisedge->target());				
	//			else e = newEdge(thisedge->source(),v);
	//			m_eCopy[adj->theEdge()] = e;
	//			ListIterator<edge> it;
	//			forall_listiterators(edge, it, m_eOrig[thisedge]){
	//				m_eOrig[e].pushBack(*it);
	//			}
	//			ListIterator<IPoint> it2;
	//			forall_listiterators(IPoint, it2, m_edgeline[thisedge]){ //get edgeline from this
	//				m_edgeline[e].pushBack(*it2);
	//			}						
	//		}
	//				
	//	}			
	//}
	//
	//
	//
	//
	//// TO DO: Update Outline
	//
	//adjEntry adj;
	//forall_adj(adj, orig){ //forall adjacent nodes in original
	//	node thisnode = m_vCopy[adj->twinNode()]; //get adjacent in this
	//	edge thisedge = m_eCopy[adj->theEdge()]; 
	//	adjEdges.pushFront(thisedge);
	//	node nodeGG = GG.Copy(adj->twinNode()); // get adjacent in GG		
	//	edge edgeGG = GG.Copy(adj->theEdge());
	//	if(nodeGG){ // if the adjacent node is already part of the cluster
	//		 // TODO: properly get edge geometry
	//		m_eCopy[adj->theEdge()] = NULL; //original edge don't have copy in this anymore
	//		IPolyline line;
	//		ListIterator<IPoint> it;
	//		forall_listiterators(IPoint,it,m_edgeline[thisedge]){ //copy edgepath (edge is not empty iff nodes have positions. i hope.)
	//			IPoint point = (*it);
	//			point.m_x -= vPos.m_x;
	//			point.m_y -= vPos.m_y;
	//			line.pushBack(point);
	//		}
	//		GG.addToLattice(line);
	//		removeFromLattice(line);			
	//	}else{ //else reroute edges in this
	//		edge e;
	//		if (thisedge->source() == w){
	//			e = newEdge(v,thisedge->target());				
	//		}else{
	//			e = newEdge(thisedge->source(),v);
	//		}
	//		
	//		m_eCopy[adj->theEdge()] = e;
	//		ListIterator<edge> it;
	//		forall_listiterators(edge, it, m_eOrig[thisedge]){
	//			m_eOrig[e].pushBack(*it);
	//		}
	//		ListIterator<IPoint> it2;
	//		forall_listiterators(IPoint, it2, m_edgeline[thisedge]){
	//			m_edgeline[e].pushBack(*it2);
	//		}						
	//	}			
	//	delEdge(thisedge);
	//}
	if (p_srcGG){
		m_GGList.del(m_GGList.get(m_GGList.search(*p_srcGG)));
	}
	std::cout << "w is " << w << std::endl;
	
	adjEntry adj;
	forall_adj(adj,w){ //reconnect edges that were previously connected to w
		if(adj->twinNode() != v ){		
			addEdge(adj->theEdge());
		}
	}
	std::cout << "w is " << w << std::endl;

	////delete w from this
	//m_vCopy[orig] = NULL;
	////m_vState[w] = 0;
	////m_vOrig[w].clear();
	int pos = m_nonDummy.search(w);
	std::cout << "w is " << w << std::endl;
	if (pos == -1){
		while(true){
			std::cout << "THIS SHOULD NEVER HAPPEN: " << w << std::endl;
			forall_listiterators(node,it,m_nonDummy){
				std::cout << *it << std::endl;
			}
			std::cout << "POS: " << m_nonDummy.search(w) << std::endl;
		}
	}
	m_nonDummy.del(m_nonDummy.get(pos));
	delNode(w);
	//addToLattice(GG.getOutline());

}; 			
void GridGraph::init(){};

bool GridGraph::tryMove(node v, IPoint pos, int rotation, int mirror){return false;};
IPoint GridGraph::getConnection(edge e){return IPoint(0,0);};

void GridGraph::clusterize(int p){	
	List<node> nodes;
	this->allNodes(nodes);
	forall_listiterators(node,it,nodes){
		List<node> nowNodes;
		this->allNodes(nowNodes);
		node v = *it;
		if (nowNodes.search(v) != -1){	//check to see if v is still in this graph
			int degree = v->indeg() + v->outdeg();
			if (degree > 2){
				std::cout << "Calling findcluster on " << v << std::endl;
				findCluster(v,p);
			}
		}
	}
};

bool GridGraph::isVisible(node v)//returns true if node is part of currently considered Layout
{
return true; //Fix Me
}
bool GridGraph::isVisible(edge v) //returns true if edge is part of currently considered Layout
{
return true; //Fix Me
}
bool GridGraph::isTemporary(node v) //returns true if node is currently being tested
{
return false; //Fix Me
}
bool GridGraph::isTemporary(edge e) //returns true if edge is currently being tested
{
return false; //Fix Me
}

bool GridGraph::isDummy(node v)
{
	return false; //Fix Me
}


//////////////////////////////////////////Funktionen zur Berechnung der OutlineArea//////////////////////
int GridGraph::deleteAntennas(IPolyline& E)
{	
	int area=0;
	ListIterator<IPoint> iter, next, onext, backiter;
	//if the pattern ABA emerges, kill B and deal with all points after A. then backtrack A
	for (iter = E.begin(); iter.valid(); ++iter) {
		for( ; ; ) {
			next  = iter; next++;
			if (!next.valid()) break;
			onext = next, onext++;
			if (!onext.valid()) break;

			if ((*iter) == (*onext)){
				// von hier
				area += (int)((*iter).distance((*next)));
				//area=area+abs((*iter)-(*next));
				// bis hier musst du was anderes machen: addiere so viele flächeneinheiten, wie der abstand
				// von (*iter) bis (*next) ist
				E.del(next);
				E.del(onext);
				--iter;
				if (!iter.valid()) { //we don't want to fall off at the beginning.
					iter = E.begin();
				}

			}
			else
				break;
		}
	} // antennae taken care of: outer antennae not belonging to the beginning or end.


	while (true) {
		iter = E.begin(); backiter = E.cyclicPred( E.begin());
		iter++;	backiter--;
		
		if ((*iter)==(*backiter)) {
			// handle the points!
			//area=area+abs(E.front()-*iter)
			area += (int)((E.front()).distance((*iter)));
			// hier das gleiche mit den punkten E.front() und (*iter)
			E.del(E.begin());
			E.del(E.cyclicPred(E.begin()));
			continue;
		} 
		break;
	}

	//TODO: maybe just make sure we got no consecutive duplicates in outline?? because that would mess things up

	List<IPolyline> outlines;
	ListIterator<IPoint> iiter, bbackiter;
	IPolyline templine1, templine2;
	for (iter = E.begin();iter != E.cyclicPred(E.begin());++iter) {
		for (backiter = iter.pred();backiter.valid();--backiter) {
			if ((*iter) == (*backiter)) { //we got something
				iiter = iter; bbackiter = backiter;
				while ((*(iiter.succ())) == (*(bbackiter.pred())) ) {
					iiter = iiter.succ();
					bbackiter = bbackiter.pred();
				}
				//handle the points here
				IPolyline handle;
				next = iter;
				handle.pushBack(*next);
				while (next != iiter) {
					++next;
					handle.pushBack(*next);
				}
				// "inneren" punkte der IPolyline handle verrechnen
				// (die Summe der Abstände) -1 (glaub ich?)
				ListIterator<IPoint> inner=handle.begin();
				while (inner.succ().valid())
				{ 
					area+=(int)((*inner).distance((*inner.succ())));
					inner++;
				}
				area=area-1;
				E.split(iiter, E,templine2,after);
				E.split(iter,E,templine1,after);
				E.split(backiter,E,templine1,after);
				templine1.pushFront(templine1.back());
				outlines.pushBack(templine1);
				E.split(bbackiter,E,templine1,after);
				E.conc(templine2);
				//iter = bbackiter;
				//--iter; not sure better go with
				iter = E.begin();

				break;
			}
		}
	}


	ListIterator<IPolyline> listiter;
	outlines.pushFront(E);

	// then we normalize, i.e. delete those points not on a bend.


	for (listiter = outlines.begin(); listiter.valid() ;++listiter ) {
		for (iter = (*listiter).begin(); iter.valid(); ++iter) {
			for( ; ; ) {
				next  = iter; next++;
				if (!next.valid()) break;
				onext = next, onext++;
				if (!onext.valid()) break;

				int d1 = 0 , d2 = 0;

				/*direction is as follows:
				0 - not on an axis to another
				1 - second point is to the right of first
				then ccw up to 4, which is below*/
				if ((*iter).m_x==(*next).m_x) {
					if ((*iter).m_y <(*next).m_y){ d1 = 4; }
					else { d1 = 2; }
				}
				else if ((*iter).m_y==(*next).m_y) {
					if ((*iter).m_x <(*next).m_x){ d1 = 1; }
					else { d1 = 3; }
				}

				if ((*next).m_x==(*onext).m_x) {
					if ((*next).m_y <(*onext).m_y){ d2 = 4; }
					else { d2 = 2; }
				}
				else if ((*next).m_y==(*onext).m_y) {
					if ((*next).m_x <(*onext).m_x){ d2 = 1; }
					else { d2 = 3; }
				}
				OGDF_ASSERT(d1>0 && d2>0);
				// is *next on the way from *iter to *onext?
				if (d1 == d2)
					(*listiter).del(next);
				else
					break; /* while */
			}
		}

		//      check that the endpoint is not an inner point.

		iter = (*listiter).begin(); backiter = (*listiter).cyclicPred((*listiter).begin());
		iiter = iter.succ(); bbackiter = backiter.pred();
		int d1 = 0, d2 = 0;

		if ((*iter).m_x==(*iiter).m_x) {
			if ((*iter).m_y <(*iiter).m_y){ d1 = 4; }
			else { d1 = 2; }
		}
		else if ((*iter).m_y==(*iiter).m_y) {
			if ((*iter).m_x <(*iiter).m_x){ d1 = 1; }
			else { d1 = 3; }
		}

		if ((*bbackiter).m_x==(*backiter).m_x) {
			if ((*bbackiter).m_y <(*backiter).m_y){ d2 = 4; }
			else { d2 = 2; }
		}
		else if ((*bbackiter).m_y==(*backiter).m_y) {
			if ((*bbackiter).m_x <(*backiter).m_x){ d2 = 1; }
			else { d2 = 3; }
		}

		if (d1 == d2) {
			(*listiter).del(iter);
			(*listiter).del(backiter);
		}
		else {
			//sanitize endpoints, only wanna have one in there.
			(*listiter).del((*listiter).cyclicPred((*listiter).begin()));
		}
	}


	//concatenate all the lists again
	listiter = outlines.begin();
	++listiter;
	while (listiter.valid()) {
		outlines.front().conc(*listiter);
		++listiter;
	}
	E = outlines.front();
	// Und ab hier stehen in E nur noch die gewünschten punkte, fertig zum sortieren
	return area;
}

List<int> GridGraph::unionLists(ogdf::List<int> prev, ogdf::List<int> curr)
{
        // calculates the union of two lists of ints that each describe a set of intervals
     
        List<int> u;
        ListIterator<int> p, c;
        p = prev.begin();
        c = curr.begin();
        int cursor;
        bool prev_on = false, curr_on = false, writing = false, was_p = false;
        //sanity check: everything even?
        if (prev.size()%2!=0 || curr.size()%2!=0) {
                //ya done goof'd
                OGDF_ASSERT (1==0);
                return u;
        }
        while (p.valid() || c.valid()) {
                if (!p.valid()) {
                        // c is valid
                        cursor = *c;
                        ++c;
                        was_p = false;
                }
                else if (!c.valid()) {
                        // p is valid
                        cursor = *p;
                        ++p;
                        was_p = true;
                }
                else {
                        //both are, interesting things happen.
                        if (*p <= *c) {//hier stand vorher *q
                                cursor = *p;
                                ++p;
                                was_p = true;
                        }
                        else {
                                cursor = *c;
                                ++c;
                                was_p = false;
                        }
                }
                if (was_p) {
                        if (prev_on) {
                                prev_on = false;
                                if (!curr_on) {
                                        writing = false;
                                        u.pushBack(cursor);
                                }
                        }
                        else { //!prev_on
                                prev_on = true;
                                if (!writing) {
                                        writing = true;
                                        u.pushBack(cursor);
                                }
                        }
                }
                else { // !was_p
                        if (curr_on) {
                                curr_on = false;
                                if (!prev_on) {
                                        writing = false;
                                        u.pushBack(cursor);
                                }
                        }
                        else { //!curr_on
                                curr_on = true;
                                if (!writing) {
                                        writing = true;
                                        u.pushBack(cursor);
                                }
                        }
                }
               
        } //while
        //sanitise, because doubles may happen.
        ListIterator<int> it = u.begin(), itt, ittt;
        while (it.valid()) {
                itt = it.succ();
                if (!itt.valid()) {break;}
                ittt = itt.succ();
                if (!ittt.valid()) {break;}
                if (*itt == *ittt) {
                        u.del(ittt); u.del(itt);
                }
                else {
                        ++it;
                }
        }
        OGDF_ASSERT(u.size()%2==0)
        return u;
}

int GridGraph::prevarea(ogdf::List<int> previousline, int y)
	//Berechne Anzahl Gitterpunkte von einer vereinigung von Boxen, die auf Vertikaler Linie y Punkte aufweist 
	//und deren x Koordinaten durch die previousline festgelegt sind
{   
	OGDF_ASSERT(previousline.size()%2==0);
	int area=0;
    ogdf::ListIterator<int> it;                      //getestet
	it=previousline.begin();  
	while ( it.valid())                         //precondition: gerade Anzahl Elemente in der Liste; aufeinanderfolgende Zahlen stellen Intervall da. 
	{											//diese müssen disjunkt sein.
		area=area+*it.succ()-*it+1;
		it++;it++;//gehe zwei Schritte Weiter und berechne neues Intervall dazu
	}
	area=area*y;	
	return area;
}  

int GridGraph::rowarea(ogdf::List<int> previousline, ogdf::List<int> currentline)
	//Berechne Fläche einer Zeile aus der Vereinigung der Intervalle
	//die in den beiden Listen abgespeichert sind
	//in currentline befindet sich nach Durchlauf der Whileschliefe die Vereinigung der Intervalle aus previousline und currentline
{  
	if (currentline.size()==0)
	{
		return prevarea(previousline, 1);
	}
	else
	{
	  return prevarea(unionLists(previousline, currentline) , 1);
	}
}

int GridGraph::outlineArea(IPolyline Outline)
{	int area=0; 
    area=area+deleteAntennas(Outline);
	StdComparer<IPoint> comp;//Outline nach y und danach nach x-Koordinate sortieren
	Outline.quicksort(comp); 	
	ogdf::List<int> currentList;
	ogdf::List<int> previousList;
	int current_y=Outline.front().m_y;
	int previous_y=current_y-1;//Wir tun so, als gäbe es eine nullte Zeile ohne Flächeninhalt
	int linearea=0;//area einer Zeile
    ogdf::ListIterator<IPoint> it;
	it=Outline.begin();


	while ( it.valid())
	{
		current_y=(*it).m_y;
		previousList=currentList;

		while ((*it).m_y==current_y)//aktuelle Liste erneuern; Iterator befindet sich anschließend auf Element mit nächstgrößerer y-Koordinate
		{
			bool done = false;
			for (ListIterator<int> iter = currentList.begin();iter.valid();++iter){
				if (*iter == (*it).m_x  ) {
					currentList.del(iter);
					done = true;
					break;
				}
				if (*iter > (*it).m_x) {
					currentList.insertBefore((*it).m_x,iter);
					done = true;
					break;
				}
			}
			if (!done) {
				currentList.pushBack((*it).m_x);
			}

			++it;
			if (!it.valid()) {
				break;
			}

		}

			area=area+rowarea(previousList,currentList);//bilde Vereinigung der Intervalle aus previousList und currentList und berechne Fläche der Vereinigung
			if (previous_y!=current_y-1)//Veränderung um mehr als eine Zeile
			{ 
				area=area+prevarea(previousList, current_y-previous_y-1);
			}
			previous_y=current_y;	
	}
	return area;
}

////////////////////////////////////////////////das waren alle zur OutlineArea benötigten Funktionen/////////////////////










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
