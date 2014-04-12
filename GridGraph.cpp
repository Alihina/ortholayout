#include "stdafx.h"



#include <GridGraph.h>


unsigned int GridGraph::instanceCount = 0;

GridGraph::GridGraph(){
	m_id = instanceCount;
	instanceCount++;
}; //DONE!

GridGraph::GridGraph(const Graph &G, node orig){ //DONE!
	m_id = instanceCount;
	instanceCount++;
	
	//std::cout << "SINGLENODE GGid: " << m_id << std::endl;
	m_pGraph = &G;
	m_vOrig.init(*this);	
	m_eOrig.init(*this);
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	m_IOprep = false;
	
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

	m_x[v] = 0;
	m_y[v] = 0;

	
	adjEntry adj;
	forall_adj(adj,orig){ //for all adjacent 
		m_eOutgoing.pushBack(adj->theEdge()); //new outgoing edge
		//std::cout << "crt conn " << adj->theEdge()<<std::endl;
		node connect = this->newNode();
		//displayDebug();
		m_vConnect.pushBack(connect);				
		edge e = newEdge(v,connect);
		//std::cout << "outgoing list:" << std::endl;
		//forall_rev_listiterators(edge,it,m_eOutgoing){
		//	std::cout << "edge: "<<*it<<" id: " << (*it)->index() << std::endl;
		//}						
		
		m_eCopy[adj->theEdge()] = e;
		m_eOrig[e].pushFront(adj->theEdge());
		



	}



};

GridGraph::GridGraph(const Graph &G){ // DONE!
	m_id = instanceCount;
	instanceCount++;
	//std::cout << "WHOLEGRAPH GGid: " << m_id << std::endl;
	m_pGraph = &G;
	m_vOrig.init(*this);	
	m_eOrig.init(*this);	
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	m_vState.init(*this, 0);
	m_eState.init(*this, 0);
	m_IOprep = false;

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
	//std::cout << "GGCOPY GGid: " << m_id << std::endl;
	m_pGraph = &GG.constGraph();
	m_vOrig.init(*this);	
	m_eOrig.init(*this);	
	m_vCopy.init(*m_pGraph, NULL);
	m_eCopy.init(*m_pGraph, NULL);
	m_vState.init(*this, 0);
	m_eState.init(*this, 0);
	m_IOprep = false;

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
	m_eOutgoing = GG.m_eOutgoing; //THIS IS NOT WRONG!!
	//m_vConnect = GG.m_vConnect; //THIS IS WRONG!!
	//m_nonDummy == GG.m_nonDummy; //THIS IS WRONG!!
	forall_listiterators(node,it,GG.m_vConnect){
		m_vConnect.pushBack(trgCopy[*it]);
	}
	forall_listiterators(node,it,GG.m_nonDummy){
		m_nonDummy.pushBack(trgCopy[*it]);
	}
	m_lattice = GG.m_lattice;
	//std::cout << "copy constructor call" << std::endl;
	//displayDebug();

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
	m_IOprep = false;

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
				/*std::cout << e2 << " ";
				forall_listiterators(IPoint,it,GG.m_edgeline[e1]){
					std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
				}
				std::cout << std::endl;*/
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
	//std::cout << "adjacent edgelines Copy: " << std::endl;
	forall_adj(adj,v){
		IPolyline line = this->edgeline(adj->theEdge());
		DPolyline dline = this->dedgeline(adj->theEdge());
		/*std::cout << adj->theNode() << "->" << adj->twinNode() << " = ";*/
		std::cout << adj->theEdge() << ":  {";
		/*forall_listiterators(IPoint,it,line){
			std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
		}
		std::cout << std::endl;*/
	}

	return *this;
}

void GridGraph::acceptPos() {
	//we need to keep temporary
	for (ListIterator<node> it = nonDummyNodes().begin();it.valid();++it) {
		if (m_vState[(*it)]==1) { m_vState[(*it)]=0; }
	}
	edge e;
	for ((e)=firstEdge();(e);(e)=(e)->succ()) {
		if (m_eState[e]==1) { m_eState[e]=0; }
	}

	ListIterator<node> itt;
	//delete invisible
	for (ListIterator<node> it = nonDummyNodes().begin();it.valid();it=itt) {
		itt = it.succ();
		if (m_vState[(*it)]==-1) { delNode(*it);}
	}
	edge f;
	for ((e)=firstEdge();(e);(e)=(f)) {
		(f)= (e)->succ();
		if (m_eState[e]==-1) { delEdge(e); }
	}

}

void GridGraph::rejectPos() {
	////and delete temprary
	//ListIterator<node> itt;
	////delete invisible
	//for (ListIterator<node> it = nonDummyNodes().begin();it.valid();it=itt) {
	//	itt = it.succ();
	//	if (m_vState[(*it)]==1) { 
	//		m_Grid.restoreFill(outline von (*it));
	//		delNode(*it);
	//	}

	//}
	//edge f;
	//for ((e)=firstEdge();(e);(e)=(f)) {
	//	(f)= (e)->succ();
	//	if (m_eState[e]==1) {
	//		m_Grid.restoreline(outline von (e));
	//		delEdge(e); 
	//	}
	//}

	////keep invisible
	//for (ListIterator<node> it = nonDummyNodes().begin();it.valid();++it) {
	//	if (m_vState[(*it)]==-1) { 
	//		m_Grid.registerfill(outline von (*it));
	//		m_vState[(*it)]=0; 
	//	}
	//}
	//edge e;
	//for ((e)=firstEdge();(e);(e)=(e)->succ()) {
	//	if (m_eState[e]==-1) {
	//		m_Grid.registerline(outline von ((e)));
	//		m_eState[e]=0; 
	//	}
	//}
	
}

bool GridGraph::tryMove(node v, IPoint pos, int rotation, int mirror) {
	//find out the position to go to and see if it's free:
	//determine outline movingto
	std::cout << "implement me!" << std::endl;
	return false;
	//m_Grid.release the outline of v and its edges
	//if (!m_Grid.isFree(movingto)) {
		//return false;
	//}
	//m_Grid.register movingto
	//make the new vertex in out GridGraph
		//and fill all the nodeArrays and lists
	//then try to connect the edges, see if that works. 
		//if it does, register the line, make a new edge and fill all the EdgeArrays and stuff
	//also fail if any of the edges are too short to contain all the degree-2-verts
	//also somehow keep track of how many edges go through an edge.
	
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
	//std::cout << "cluster initially:" << std::endl;
	//forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	cluster = findClusterRecurse(cluster, p);
	//std::cout << "cluster after BFS:" << std::endl;
	//forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	cluster = trimCluster(cluster,v);
	//std::cout << "cluster after trim:"<<  std::endl;	
	//if (cluster.size() > 1) std::cout << "moving into cluster " << v << " nodes:" << std::endl;
	/*forall_listiterators(node,it,cluster) {
		std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << " adj nodes ";
		adjEntry adj;
		forall_adj(adj,*it){
			std::cout << adj->twinNode() << " ";
		}
		std::cout << std::endl;
		if (subGG(v)){
			subGG(v)->displayDebug();
		}
	}*/
	cluster.del(cluster.get(cluster.search(v)));
	//bool done = false;
	ListIterator<node> nNode;
	while (!cluster.empty()){
		nNode = ListIterator<node>();
		forall_nonconst_listiterators(node,it,cluster){ //find a neighbor of v in cluster
			bool neighbor = false;
			adjEntry adj;
			forall_adj(adj,*it){
				if (adj->twinNode() == v) neighbor = true;
			}
			if (neighbor){
				nNode = it;
				break;
			}
		}
		/*std::cout << "calling to move " << *nNode << " into " << v << std::endl;*/
		node test;
		//forall_nodes(test,*this) if (test == *nNode) std::cout << "*nNode is of this!" << std::endl;
		moveToCluster(*nNode,v);
		cluster.del(nNode);
		//std::cout << "deleted node, cluster is now: " << std::endl;
		/*forall_listiterators(node,it,cluster){
			std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << " adj ";	
			adjEntry adj;
			forall_adj(adj,*it){
			std::cout << adj->twinNode() << " ";
			}
			std::cout << std::endl; 
		}*/
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
	//std::cout << "trimming list:" << std::endl;
	//forall_listiterators(node,it,U){
//		std::cout << *it << std::endl;
//	}

	EdgeArray<bool> e_visited(*this,false);
	EdgeArray<node> direct(*this);
	NodeArray<bool> v_visited(*this,false);
	NodeArray<node> parent(*this);
	List<edge> backedges;

	forall_adj_edges(e,v) {
		//std::cout << "dfs push " << e << " parent " <<v<< std::endl;
		DFSstack.push(e);
		//e_visited[e] = true; DONT DO THAT!
		DFSparent.push(v);
	}

	parent[v] = 0;
	v_visited[v] = true;
	int prc = 0;
	while (!DFSstack.empty()) {		
		e = DFSstack.pop();
		n = DFSparent.pop();
		m = e->opposite(n);
		if (e_visited[e]) continue;
		//std::cout << prc++ <<": e = " << e << " n = " << n << " m = " << m << std::endl; 
		if (v_visited[m] == false){ 
			if (U.search(m)==-1) {
				continue;
			}
			else {
				direct[e] = n;
				//std::cout << "direction set: " << e << "direct: " <<direct[e] <<std::endl;
				e_visited[e] = true;
				v_visited[m] = true;
				parent[m] = n;
				forall_adj_edges(f,m) {
					if (e_visited[f]==false) {
						//std::cout << "dfs push " << f << " parent " << m << std::endl;
						DFSstack.push(f);
						DFSparent.push(m);						
					}
				}
			}
		}
		else {
			e_visited[e] = true;
			direct[e] = n;
			//std::cout << "direction set: " << e << "direct: " <<direct[e] <<std::endl;
			//std::cout << "found backedge " << e << "direct: " <<direct[e] <<std::endl;
			backedges.pushBack(e);
		}
	}
	
	//std::cout << "backedges list:" << std::endl;
	//forall_listiterators(edge,it,backedges){
	//	std::cout << *it << "direct: " <<direct[*it] << std::endl;
	//}

	bool done = false;
	NodeArray<bool> marked(*this,false);
	List<ListIterator<edge>> del;
	marked[v] = true;
	while (!done) {
		//std::cout << "mainloop" << std::endl;
		done = true;
		del.clear();
		//for( ListIterator<edge> it = backedges.begin(); it.valid(); ++it) {
		forall_nonconst_listiterators(edge, it, backedges){
			if ( marked[(*it)->opposite(direct[(*it)])] ) {
				//std::cout << "backloop" << std::endl;
				n = direct[(*it)];
				while (marked[n]==false) {
					marked[n] = true;
					n = parent[n];
					//std::cout << "markloop"<< std::endl;
				}
				del.pushBack(it);
				//++it; //oder --it? macht glaub ich nichts aus.
				done = false;
			}
		}
		forall_listiterators(ListIterator<edge>, itit,del){
			backedges.del(*itit);
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
		node v = *(m_vConnect.get(pos));
		if (v == NULL) while(true) std::cout << "CONNECTION MISSING" << std::endl;
		//std::cout << "getConnectNode " << v << std::endl;
		return v;
	}

void GridGraph::displayDebug(){
	std::cout << std::endl << std::endl;
	std::cout << "DEBUG INFORMATION FOR GRIDGRAPH #id: " << m_id << std::endl;
	//std::cout << "========================================" << std::endl;
	std::cout << "Number of Nodes: " << numberOfNodes() << std::endl;
	std::cout << "Number of Edges: " << numberOfEdges() << std::endl;
	std::cout << "Nodelist: " << std::endl;
	node v;
	forall_nodes(v,*this){
		std::cout << "  " << v << ": degree: " << v->degree();
		if (!original(v).empty()) std::cout << " copy of: " << original(v).front();
		std::cout << " pos: " << getPos(v);
		if (subGG(v)) {
			std::cout << " has subGG#";
			std::cout << subGG(v)->id();
			/*if (subGG(v)->id() == 19) {
				subGG(v)->displayDebug();
				return;
			}*/
		}
		std::cout << std::endl;
	}
	std::cout << "Edgelist: " << std::endl;
	edge e;
	forall_edges(e,*this){
		std::cout << "  " << e << ": id:" << e->index() << " edgeline : " ;
		forall_listiterators(IPoint,it,m_edgeline[e]){
			std::cout << *it << "->";
		}
		std::cout << std::endl;
	}
	std::cout << "non-dummy nodes: " << std::endl;
	forall_listiterators(node,it,m_nonDummy){
		std::cout << "  " << *it << ": degree: " << (*it)->degree() /*<< " copy of: " << original(v).front()*/
		<< " pos: " << getPos(*it);
		bool invader = true;
		node test;
		forall_nodes(test,*this) if (test == *it) invader = false;
		if (invader) std::cout << "   NODE DOES NOT BELONG TO THIS GRIDGRAPH!!!!";
		std::cout << std::endl;
	}
	std::cout << "Connect List:" << std::endl;
	forall_listiterators(node,it,m_vConnect){
		std::cout << *it << std::endl;
	}
	if (!m_vConnect.empty()){
		std::cout << "connectFront "<< m_vConnect.front() << std::endl;
		std::cout << "connectBack "<< m_vConnect.back() << std::endl;
	}
	std::cout << "Outgoing List:" << std::endl;
	forall_listiterators(edge,it,m_eOutgoing){
		std::cout << *it << std::endl;
	}

	std::cout << std::endl;
}

node GridGraph::addInnerNode(node orig, node src){ 
	//displayDebug();

	node v = this->newNode();
	m_nonDummy.pushFront(v);
	m_vOrig[v].pushFront(orig);
	m_vCopy[orig] = v;

	adjEntry adj;
	forall_adj(adj,orig){
		node w = m_vCopy[adj->twinNode()];		
		edge origedge = adj->theEdge();
		edge copyedge = Copy(origedge);
		if (copyedge){
			if (copyedge->source() == src) moveSource(copyedge,v);
			if (copyedge->target() == src) moveTarget(copyedge,v);
		}else{
			edge e = newEdge(v,w);
			m_eOrig[e].pushFront(adj->theEdge());
			m_eCopy[adj->theEdge()] = e;
		}
	}
	return v;
}

node GridGraph::addNode(node orig){ 
	//displayDebug();

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
	//std::cout << "adding node " << orig << std::endl;
	adjEntry adj;
	forall_adj(adj,orig){
		//std::cout << "adj orig " << adj->twinNode() << std::endl;
		//displayDebug();

		node w = m_vCopy[adj->twinNode()];
		/*std::cout << "debug" << w;*/
		if (w) {
			/*	forall_listiterators(edge,it,m_eOutgoing){
			std::cout << "OLedge: "<<*it<<" id: " << (*it)->index()  << std::endl;
			}*/
			edge e = newEdge(v,w);
			//should always work because v->w has been outgoing edge			
			node conn = getConnectNode(adj->theEdge()); 
			edge olde = conn->firstAdj()->theEdge(); //there should be exactly one adjacent edge
			m_edgeline[e] = m_edgeline[olde];


			int outpos = m_eOutgoing.search(adj->theEdge());			
			m_eOutgoing.del(m_eOutgoing.get(outpos));
			int connpos = m_vConnect.search(conn);			
			m_vConnect.del(m_vConnect.get(connpos));			
			delNode(conn);

			m_eOrig[e].pushFront(adj->theEdge());
			m_eCopy[adj->theEdge()] = e;

		}else{

			m_eOutgoing.pushBack(adj->theEdge());
			node connect = newNode();
			edge e = newEdge(v,connect);
			m_eCopy[adj->theEdge()] = e;
			m_eOrig[e].pushFront(adj->theEdge());
			m_vConnect.pushBack(connect);

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


void GridGraph::moveToCluster(GridGraph &GGw, node v){ 
/*	std::cout << "moving into " << v << "the following Cluster: " << std::endl;
	GGw.displayDebug();
	std::cout << "target Cluster: " << std::endl;*/	
	GridGraph &GG = *subGG(v); //this will always work if this function is only called by moveToCluster(node w, node v);	
	//GG.displayDebug();
	List<node> nondummys = GGw.nonDummyNodes();
	while(!nondummys.empty()){
		ListIterator<node> itw;
		bool neighbor = false;
		forall_nonconst_listiterators(node,it,nondummys){//pick nondummys whose original neighbor an original of v
			
			List<node> originals = GGw.original(*it);
			forall_listiterators(node,it2,originals){ 
				adjEntry adj;
				forall_adj(adj,*it2){
					if (this->Copy(adj->twinNode()) == v) neighbor = true;
				}
				if (neighbor) break;
			}
			if (neighbor){
				itw = it;
				break;
			}
		}
		if (!neighbor) while(true) std::cout << " oh no! "; //ERROR HANDLING LIKE A BOSS!
		node w = *itw;		
		//std::cout << "moving node " << w << std::endl;
		GridGraph * p_srcGG = GGw.subGG(w); //pointer to w's subGG	
		if (p_srcGG){ //if w is its own Cluster
			this->moveToCluster(*p_srcGG, v);  //that doesn't mean I don't have to consider all adjacent edges of w!
			GGw.m_GGList.del(GGw.m_GGList.get(GGw.m_GGList.search(*p_srcGG)));
		}else{ //if w is just a single node
			node orig = GGw.original(w).front(); //works because w isn't a dummy node
			/*IPolyline el1, el2, el3, el4;
			ListIterator<edge> adjE;
			List<edge> adjEs;
			constGraph().adjEdges(orig, adjEs);
			adjE = adjEs.begin();
			if (GGw.Copy(*adjE)) el1 = GGw.edgeline(GGw.Copy(*adjE));
			adjE++;
			if (adjE.valid()) if (GGw.Copy(*adjE)) el2 = GGw.edgeline(GGw.Copy(*adjE));			
			adjE++;
			if (adjE.valid()) if (GGw.Copy(*adjE)) el3 = GGw.edgeline(GGw.Copy(*adjE));			
			adjE++;
			if (adjE.valid()) if (GGw.Copy(*adjE)) el4 = GGw.edgeline(GGw.Copy(*adjE));
			adjE++;
			if (adjE.valid()) while(true) std::cout << " too many neighbors " << std::endl;
*/

			node w2 = GG.addNode(orig); //the new node in cluster v (orig,copy,edge,connect taken care of by addNode)
			
			//adjE = adjEs.begin();
			//if (GGw.Copy(*adjE)) GGw.edgeline(GGw.Copy(*adjE)) = el1;
			//adjE++;
			//if (adjE.valid()) if (GGw.Copy(*adjE)) GGw.edgeline(GGw.Copy(*adjE)) = el2;			
			//adjE++;
			//if (adjE.valid()) if (GGw.Copy(*adjE)) GGw.edgeline(GGw.Copy(*adjE)) = el3;			
			//adjE++;
			//if (adjE.valid()) if (GGw.Copy(*adjE)) GGw.edgeline(GGw.Copy(*adjE)) = el4;
			//adjE++;
			//if (adjE.valid()) while(true) std::cout << " too many neighbors " << std::endl;

	
			this->m_vOrig[v].pushFront(orig);
			this->m_vCopy[orig] = v;

			IPoint wPos = GGw.getPos(w); //this is relative to GGw
			IPoint GGwPos = GGw.getPos(); //this is relative to top-level GridGraph
			IPoint vPos = this->getPos(v); // this is relative to this-level GridGraph
			//IPoint GGvPos = GG.getPos(); //this is relative to top-level GridGraph
			IPoint wPos_this;
			wPos_this.m_x = (wPos.m_x + GGwPos.m_x) - getPos().m_x;
			wPos_this.m_y = (wPos.m_y + GGwPos.m_y) - getPos().m_y;
			IPoint w2Pos(wPos_this.m_x - vPos.m_x, wPos_this.m_y - vPos.m_y); //pos of w2 relative to cluster v's center
			GG.setPos(w2, w2Pos);
			//std::cout << "wPos :" << wPos << std::endl;
			//std::cout << "GGwPos :" << GGwPos << std::endl;
			//std::cout << "vPos :" << vPos << std::endl;
			//std::cout << "wPos_this :" << wPos_this << std::endl;
			//std::cout << "w2Pos :" << w2Pos << std::endl;
		}
		//GG.displayDebug();

		//Consider all adj edges of w
		adjEntry adj;
		List<edge> delEdges;
		forall_adj(adj, w){ //note that only real edges to non-dummy nodes should be considered and all of them transferred to v
			edge e = adj->theEdge(); //the adjacent edge
			node n = adj->twinNode(); // the neighbor;
			/*std::cout << "adj edge " << e << " neighbor "<< n << std::endl;*/
			if(GGw.isDummy(n)){
				//std::cout << "n is Dummy, will continue" << std::endl;				
				//std::cout << "original of w is " << GGw.original(w).front();
				//std::cout << "original of e is " << GGw.original(e).front();
				//std::cout << "neighbor in orig is: " << GGw.original(e).front()->opposite(GGw.original(w).front());
				//std::cout << "Copy in this is: " << Copy(GGw.original(e).front()->opposite(GGw.original(w).front()));

				if (!(Copy(GGw.original(e).front()->opposite(GGw.original(w).front())) == v ) )	continue;					
				//std::cout << "oops, dummy represents edge that connects to v, will not continue" << std::endl;
				// i.e. even if n is dummy, if the edge in this connected at n goes to v, continue
			}

			delEdges.pushFront(e);
			IPoint vPos = getPos(v); //relative to this-level GridGraph;
			IPoint GGwPos = GGw.getPos(); //relative to top-level GridGraph;
			IPolyline e_edgeline = GGw.m_edgeline[e];			
			//std::cout  << "copying edgeline ";
			forall_nonconst_listiterators(IPoint, it, e_edgeline){
				//std::cout << *it << "O";
				//std::cout << ((*it).m_x +GGwPos.m_x) - getPos().m_x - vPos.m_x << ",";
				//std::cout << ((*it).m_y +GGwPos.m_y) - getPos().m_y - vPos.m_y << " = ";	
				(*it).m_x = ((*it).m_x +GGwPos.m_x) - getPos().m_x - vPos.m_x;
				(*it).m_y = ((*it).m_y +GGwPos.m_y) - getPos().m_y - vPos.m_y;					
				//std::cout << (*it) << " -> "; 
				
			}
			//std::cout<<std::endl;
			//GG.displayDebug();
			//std::cout << "edge " << e << std::endl;
			edge origedge = GGw.original(e).front();
			//std::cout << "origedge " << origedge << std::endl;
			edge GGedge = GG.Copy(origedge);
			//std::cout << "GGedge " << GGedge << " id: " << GGedge->index() << std::endl;
			GG.m_edgeline[GGedge] = e_edgeline; //this should adress the correct edge in  cluster v
			GG.addToLattice(e_edgeline);
			//forall_listiterators(edge,it,GGw.original(e)){ //delete reference to e from m_eCopy edgeArray;
			//	GGw.m_eCopy[*it] = NULL;
			//}
			m_eCopy[GGw.original(e).front()] = NULL; //fixes the symptom, ignores the cause, could lead to trouble

		}
		forall_listiterators(edge,it,delEdges){
			GGw.delEdge(*it);
		}

		//int pos = GGw.m_nonDummy.search(w);
		//std::cout << "w is " << w << std::endl;
		/*if (pos == -1){
			while(true){
				std::cout << "THIS SHOULD NEVER HAPPEN: " << w << std::endl;
				forall_listiterators(node,it,m_nonDummy){
					std::cout << *it << std::endl;
				}
				std::cout << "POS: " << m_nonDummy.search(w) << std::endl;
			}
		}*/
		//GGw.m_nonDummy.del(m_nonDummy.get(pos)); //no reason to delete + it might mess up the forall_nodes loop
		//GGw.delNode(w);

		nondummys.del(itw);
	}

}

void GridGraph::moveToCluster(node w, node v){ // should be done I HOPE!!	
//	std:cout << "moving " << w << " into " << v << std::endl;
	node test;
	bool broken = true;
/*	forall_nodes(test,*this) if (test == v) std::cout << "v is of this!" << std::endl;
	forall_nodes(test,*this) if (test == w) {std::cout << "w is of this!" << std::endl; broken = false;}
	if (broken){
		std::cout << "well, fuck..." << std::endl;
	}*/

	if (subGG(v) == NULL) { // create a new subGG if needed
		GridGraph newGG = GridGraph(*m_pGraph, m_vOrig[v].front());		
		m_GGList.pushFront(newGG);		
		m_vGridGraph[v] = &m_GGList.front();		
		(*subGG(v)).getPos().m_x = getPos(v).m_x + this->getPos().m_x;		
		(*subGG(v)).getPos().m_y = getPos(v).m_y + this->getPos().m_y;		
	}	

	GridGraph &GG = *subGG(v);	
	//std:cout << "moving " << w << " into " << GG.id() << std::endl;
	GG.m_IOprep = false;
	GridGraph * p_srcGG = subGG(w); //pointer to w's subGG	
	if (p_srcGG){ //if w is its own Cluster
		moveToCluster(*p_srcGG, v);  //that doesn't mean I don't have to consider all adjacent edges of w!
		m_GGList.del(m_GGList.get(m_GGList.search(*p_srcGG)));
	}else{ //if w is just a single node
		node orig = original(w).front(); //works because w isn't a dummy node
		node w2 = GG.addNode(orig); //the new node in cluster v (orig,copy,edge,connect taken care of by addNode)

		m_vOrig[v].pushFront(orig);
		m_vCopy[orig] = v;
		IPoint wPos = getPos(w); //relative to this-level GridGraph
		IPoint vPos = getPos(v); //relative to this-level GridGraph
		IPoint w2Pos(wPos.m_x - vPos.m_x, wPos.m_y - vPos.m_y); //pos of w2 relative to cluster v's center
		GG.setPos(w2, w2Pos);
	}
	
	//Consider all adj edges of w
	adjEntry adj;
	List<edge> delEdges;
	//std::cout << "considering edges" << std::endl;
	//forall_adj(adj, w) std::cout << adj->theEdge() << std::endl;
	//std::cout << std::endl;
	List<edge> moveEdge;
	forall_adj(adj, w){ //note that w should be a neighbor of v!
		edge e = adj->theEdge(); //the adjacent edge
		node n = adj->twinNode(); // the neighbor;
		//std::cout << e;
		if (n == v){ // e must be deleted and its attributes transferred to the edge in cluster v
			//std::cout << " will be deleted" << std::endl;
			delEdges.pushFront(e);
			IPoint vPos = getPos(v);
			IPolyline e_edgeline = m_edgeline[e];			
			forall_nonconst_listiterators(IPoint, it, e_edgeline){
				(*it).m_x -= vPos.m_x;
				(*it).m_y -= vPos.m_y;
			}
			GG.m_edgeline[GG.Copy(original(e).front())] = e_edgeline; //this should adress the correct edge in  cluster v
			GG.addToLattice(e_edgeline);
			//forall_listiterators(edge,it,original(e)){ //delete reference to e from m_eCopy edgeArray;
			//	std::cout << "deleting reference in m_eCopy to " << *it << std::endl;
			//	m_eCopy[*it] = NULL;
			//}
			m_eCopy[original(e).front()] = NULL; //fixes the symptom, ignores the cause, could lead to trouble
		}else{ // e must be reconnected from w to v and its attributes transferred.			
			//std::cout  << " will be rerouted ";
			moveEdge.pushFront(e);
			//std::cout<<std::endl;
		}
	}
	//std::cout << "edge routings: " << std::endl;
	while(!moveEdge.empty()){
		edge moveE = moveEdge.popFrontRet();
		//std::cout << moveE << " becomes ";
		if (moveE->target() == w){
			moveTarget(moveE,v);
			//std::cout << moveE << "  " <<std::endl;
		}

		if (moveE->source() == w){
			moveSource(moveE,v);
			//std::cout << moveE << std::endl;
		}
	}
	forall_listiterators(edge,it,delEdges){
		delEdge(*it);
	}
	
	int pos = m_nonDummy.search(w);
	//std::cout << "w is " << w << std::endl;
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

}; 			

void GridGraph::contract(){ // This will break if GridGraph is a cycle, but that should never happen unless the original graph is a cycle.
	m_IOprep = false;
	//first take all paths and reverse edges such that every deg2 vertex has exactly one incoming and one outgoing edge;
	List<node> deg2nodes;
	List<ListIterator<node>> del;
	allNodes(deg2nodes);
	forall_nonconst_listiterators(node,it,deg2nodes){
		if ((*it)->degree() != 2) del.pushFront(it);
		else if (subGG(*it)) del.pushFront(it); //don't contact clusters!
	}		
	forall_listiterators(ListIterator<node>,itit,del){
		deg2nodes.del(*itit);
	}
	
	List<node> deg2nodesCopy = deg2nodes;
	while(!deg2nodes.empty()){
		node v = deg2nodes.chooseElement(); //take a random deg2 node
		adjEntry v_adj = v->firstAdj(); 
		adjEntry w_adj = v_adj->twin();
		node w = w_adj->theNode(); 	
		while (w->degree() == 2 && subGG(v) == NULL){
			v = w;
			v_adj = w_adj->cyclicSucc();
			w_adj = v_adj->twin();
			w = w_adj->theNode();
		}		
		//w is now the first node in the path
		edge e = v_adj->theEdge();
		if (e->target() != v) reverseEdge(e); //direct the path from w to v
		
		while(v->degree() == 2 && subGG(v) == NULL){ //move to next neighbor und fix path until end
			deg2nodes.del(deg2nodes.get(deg2nodes.search(v)));
			w = v;
			w_adj = v_adj->cyclicSucc();
			v_adj = w_adj->twin();
			v = v_adj->theNode();
			e = v_adj->theEdge();
			if (e->target() != v) reverseEdge(e);
		}
	}
	//all deg2Nodes have been fixed.
	forall_listiterators(node,it,deg2nodesCopy){
		contract(*it);
	}
}

edge GridGraph::contract(node v){
	//unsplit removes outgoing edge and reroutes incoming edge;
	
	List<edge> outgoingL;
	outEdges(v,outgoingL);
	edge out = outgoingL.front();

	List<edge> incomingL;
	inEdges(v,incomingL);
	edge in = incomingL.front();	
	
	List<edge> out_eOrig = m_eOrig[out];
	IPolyline out_edgeline = m_edgeline[out];
	int out_eSpaces = m_eSpaces[out];

	IPoint pos = getPos(v);
	//not copying m_eState, assuming standard state for all edges during contract.
	m_nonDummy.del(m_nonDummy.get(m_nonDummy.search(v)));
	unsplit(v);
	edge e = in;	
	m_eSpaces[e] += out_eSpaces;
	m_eOrig[e].conc(out_eOrig);

	if (out_edgeline.empty()) return e;
	if (m_edgeline[e].empty()){
		m_edgeline[e] = out_edgeline;
		return e;
	}
	if (pos == m_edgeline[e].back() && pos == out_edgeline.front()){		
		out_edgeline.popFront();
		m_edgeline[e].popBack();
		IPoint p1, p2;
		p1 = out_edgeline.front();
		p2 = m_edgeline[e].back();
		if (p1.m_x != p2.m_x && p1.m_y != p2.m_y) m_edgeline[e].pushBack(pos);
		m_edgeline[e].conc(out_edgeline);		
	}else if (pos == out_edgeline.back() && pos == m_edgeline[e].front()){				
		out_edgeline.popBack();
		m_edgeline[e].popFront();
		IPoint p1, p2;
		p1 = out_edgeline.back();
		p2 = m_edgeline[e].front();
		if (p1.m_x != p2.m_x && p1.m_y != p2.m_y) m_edgeline[e].pushFront(pos);		
		m_edgeline[e].concFront(out_edgeline);				
	}else if (pos == m_edgeline[e].back() && pos == out_edgeline.back()){		
		out_edgeline.popBack();
		m_edgeline[e].popBack();
		IPoint p1, p2;
		p1 = out_edgeline.back();
		p2 = m_edgeline[e].back();
		if (p1.m_x != p2.m_x && p1.m_y != p2.m_y) m_edgeline[e].pushBack(pos);		
		while (!out_edgeline.empty()){
			m_edgeline[e].pushBack(out_edgeline.popBackRet());
		}		
	}else if (pos == m_edgeline[e].front() && pos == out_edgeline.front()){		
		out_edgeline.popFront();
		m_edgeline[e].popFront();
		IPoint p1, p2;
		p1 = out_edgeline.front();
		p2 = m_edgeline[e].front();
		if (p1.m_x != p2.m_x && p1.m_y != p2.m_y) m_edgeline[e].pushFront(pos);

		while (!out_edgeline.empty()){
			m_edgeline[e].pushFront(out_edgeline.popFrontRet());
		}				
	}
	return e;	

}

void GridGraph::init(){};

//bool GridGraph::tryMove(node v, IPoint pos, int rotation, int mirror)
//{
//	grid machen, 
//};
IPoint GridGraph::getConnection(edge e){return IPoint(0,0);};

void GridGraph::clusterize(){
	int non2Nodes = 0;
	node v;
	int AllNodes = numberOfNodes();
	forall_nodes(v,*this) if (v->degree() != 2) non2Nodes++;
	//displayDebug();
	while(true){
		if (non2Nodes > 16){			
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(2);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(2);			
			}
		}else break;

		non2Nodes = 0;
		forall_nodes(v,*this) if (v->degree() != 2) non2Nodes++;
		if (non2Nodes > 18){
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(3);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(3);			
			}
		}else break;

		non2Nodes = 0;
		forall_nodes(v,*this) if (v->degree() != 2) non2Nodes++;
		if (non2Nodes > 20){
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(4);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(4);			
			}
		}else break;

		non2Nodes = 0;
		forall_nodes(v,*this) if (v->degree() != 2) non2Nodes++;
		if (non2Nodes > 30){
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(5);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(5);			
			}
		}else break;

		non2Nodes = 0;
		forall_nodes(v,*this) if (v->degree() != 2) non2Nodes++;
		if (non2Nodes > 50){
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(6);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(6);			
			}
		}else break;

		if (non2Nodes > 80){
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(7);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(7);			
			}
		}else break;

		if (non2Nodes > 120){
			int oldNoN;
			oldNoN = this->numberOfNodes();		
			clusterize(8);
			while (oldNoN != this->numberOfNodes()){
				oldNoN = this->numberOfNodes();
				clusterize(8);			
			}
		}else return;

		std::cout << "der graph ist zu h�sslich, SAGrid wird wahrscheinlich langsam sein oder nichts gutes finden" << std::endl;
		break;
	}
	/*forall_listiterators(GridGraph,it,GGList()){
		(*it).clusterize();
	}*/
	if (nonDummyNodes().size() == 1){
		eviscerate(nonDummyNodes().front());
	}
	//displayDebug();
	if (AllNodes != numberOfNodes()){
		std:cout << AllNodes << " became " << numberOfNodes() << std::endl;
		forall_nodes(v,*this){
			if (subGG(v)) {
				//std::cout << "calling clusterize on GridGraph #" << subGG(v)->id() << std::endl;
				//subGG(v)->displayDebug();
				subGG(v)->clusterize();
				//std::cout << "GridGraph id#" << subGG(v)->id() << " done" << std::endl;
				//subGG(v)->displayDebug();
				//std::cout << "pause" << std::endl;
			}
		}
	}
}

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
				//std::cout << "Calling findcluster on " << v << std::endl;
				findCluster(v,p);
			}
		}
	}
};


bool GridGraph::isDummy(node v){ //note: if you mess up and v is not even of this graph, it will com back as non-dummy
	forall_listiterators(node,it,m_vConnect){
		if (*it == v) return true;
	}
	return false;
}

void GridGraph::eviscerate(node v){
	//std::cout << "GRIDGRAPH GG#" << id() << " BEFORE EVISCERATION:";
	//displayDebug();
	m_IOprep = false;
	if (subGG(v) == NULL) return;
	GridGraph &GG = *subGG(v);
	List<edge> origEdges;
	List<IPolyline> origEdgelines;
	//std::cout << "eviscerating node " << v << " subGG#" << GG.id() << std::endl;	
	//GG.displayDebug();

	//bool invader = false;
	//node test;
	//forall_listiterators(node,it,GG.nonDummyNodes()){
	//	node w = *it;
	//	invader = true;
	//	forall_nodes(test,GG) if (test == w) invader = false;
	//	if (invader) break;
	//}
	//if (invader) 
	//	while(true) std::cout << "non-dummy nodes has been invaded" << std::endl; 

	edge e;
	forall_adj_edges(e,v){
		origEdges.pushFront(original(e).front());
		origEdgelines.pushFront(m_edgeline[e]);
	}
	forall_listiterators(node,it,GG.nonDummyNodes()){
		node w = *it;
		//std::cout << "extracting node " << w << std::endl;
		node test;
		//forall_nodes(test,GG) if (test == w) std::cout << "w is of GG!" << std::endl;

		if (GG.subGG(w)) 
			while(true) std::cout << "extracting non-single node, this shouldn't happen" << std::endl;
		node orig = GG.original(w).front();
		node w2 = addInnerNode(orig,v);		
		x(w2) = x(v)+GG.x(w);
		y(w2) = y(v)+GG.y(w);		
	}

	forall_edges(e,GG){
			IPolyline copyline = GG.edgeline(e);
			forall_nonconst_listiterators(IPoint,it,copyline){
				(*it).m_x = (*it).m_x + x(v);
				(*it).m_y = (*it).m_y + y(v);
			}
			edgeline(Copy(GG.original(e).front())) = copyline;
		}

	while(!origEdges.empty()){
		edge origEdge = origEdges.popFrontRet();
		IPolyline origLine = origEdgelines.popFrontRet();
		//std::cout << "copying edges previously attached to eviscerated node" << std::endl;
		forall_nonconst_listiterators(IPoint,it,origLine){
				(*it).m_x = (*it).m_x;
				(*it).m_y = (*it).m_y;
				//std::cout << *it << std::endl;
			}
		//std::cout << "to edge " << Copy(origEdge) << std::endl;
		edgeline(Copy(origEdge)) = origLine;
	}
	m_nonDummy.del(m_nonDummy.get(m_nonDummy.search(v)));
	m_GGList.del(m_GGList.get(m_GGList.search(*subGG(v))));
	m_vGridGraph[v] = NULL;
	delNode(v); 
	displayDebug();
}

bool GridGraph::isInside(IPoint p){
	return m_lattice.isInside(p);
}
bool GridGraph::isInside(DPoint p){
	return m_lattice.isInside(p);
}
//bool GridGraph::isVisible(node v)//returns true if node is part of currently considered Layout
//{
//return true; //Fix Me
//}
//bool GridGraph::isVisible(edge v) //returns true if edge is part of currently considered Layout
//{
//return true; //Fix Me
//}
//bool GridGraph::isTemporary(node v) //returns true if node is currently being tested
//{
//return false; //Fix Me
//}
//bool GridGraph::isTemporary(edge e) //returns true if edge is currently being tested
//{
//return false; //Fix Me
//}



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
				// bis hier musst du was anderes machen: addiere so viele fl�cheneinheiten, wie der abstand
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
				// (die Summe der Abst�nde) -1 (glaub ich?)
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
	// Und ab hier stehen in E nur noch die gew�nschten punkte, fertig zum sortieren
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
	{											//diese m�ssen disjunkt sein.
		area=area+*it.succ()-*it+1;
		it++;it++;//gehe zwei Schritte Weiter und berechne neues Intervall dazu
	}
	area=area*y;	
	return area;
}  

int GridGraph::rowarea(ogdf::List<int> previousline, ogdf::List<int> currentline)
	//Berechne Fl�che einer Zeile aus der Vereinigung der Intervalle
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
	int previous_y=current_y-1;//Wir tun so, als g�be es eine nullte Zeile ohne Fl�cheninhalt
	int linearea=0;//area einer Zeile
    ogdf::ListIterator<IPoint> it;
	it=Outline.begin();


	while ( it.valid())
	{
		current_y=(*it).m_y;
		previousList=currentList;

		while ((*it).m_y==current_y)//aktuelle Liste erneuern; Iterator befindet sich anschlie�end auf Element mit n�chstgr��erer y-Koordinate
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

			area=area+rowarea(previousList,currentList);//bilde Vereinigung der Intervalle aus previousList und currentList und berechne Fl�che der Vereinigung
			if (previous_y!=current_y-1)//Ver�nderung um mehr als eine Zeile
			{ 
				area=area+prevarea(previousList, current_y-previous_y-1);
			}
			previous_y=current_y;	
	}
	return area;
}

////////////////////////////////////////////////das waren alle zur OutlineArea ben�tigten Funktionen/////////////////////











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
//	GG.init()
//	GridDH dh();
//	Grid_EnergyFunction * blubb = new Art von energyfunction(); //und andere
//	dh.addEnergyFunction(blubb, weight.); //TODO gute weights raten.
//	dh.call(m_GG);
//	GG.finalise();
//	//do fancy stuff
//	return 0;
//}; 
//
