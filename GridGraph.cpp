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
	//std::cout << "cluster initially:" << std::endl;
	//forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	cluster = findClusterRecurse(cluster, p);
	//std::cout << "cluster after BFS:" << std::endl;
	//forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	cluster = trimCluster(cluster,v);
	//std::cout << "cluster after trim:"<<  std::endl;
	//forall_listiterators(node,it,cluster) std::cout << "*it: "<< *it << " pos in Graph: " << nodes.search(*it) << std::endl;
	if (cluster.size() > 1) std::cout << "moving into cluster " << v << " nodes:" << std::endl;
	cluster.del(cluster.get(cluster.search(v)));
	//bool done = false;
	ListIterator<node> nNode;
	while (!cluster.empty()){
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
		moveToCluster(*nNode,v);
		cluster.del(nNode);
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
		std::cout << " pos: " << getPos(v) << std::endl;
	}
	std::cout << "Edgelist: " << std::endl;
	edge e;
	forall_edges(e,*this){
		std::cout << "  " << e << ": id:" << e->index() /*<< "  copy of" << original(e).front()*/ << std::endl;
	}
	std::cout << "non-dummy nodes: " << std::endl;
	forall_listiterators(node,it,m_nonDummy){
		std::cout << "  " << *it << ": degree: " << (*it)->degree() /*<< " copy of: " << original(v).front()*/
		<< " pos: " << getPos(*it) << std::endl;
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
			node conn = getConnectNode(adj->theEdge()); //should always work because v->w has been outgoing edge
			//std::cout << "got conn node " << conn << std::endl;
			int outpos = m_eOutgoing.search(adj->theEdge());
			//std::cout << "del from outgoing at " << outpos << std::endl;
			m_eOutgoing.del(m_eOutgoing.get(outpos));
			int connpos = m_vConnect.search(conn);
			//std::cout << "del from connNodes at " << connpos << std::endl;
			m_vConnect.del(m_vConnect.get(connpos));
			//std::cout << "del from Connect " << *m_vConnect.get(m_vConnect.search(conn)) << std::endl;

			edge f;
			//std::cout << "rightBeforeDeleteConn" << std::endl;
			//displayDebug();
			//std::cout << conn->degree() << std::endl;
			//forall_adj_edges(f,conn) std::cout << "conn_edge: " << f << std::endl;
			delNode(conn);
			//std::cout << "del conn" << std::endl;
			edge e = newEdge(v,w);
			m_eOrig[e].pushFront(adj->theEdge());
			m_eCopy[adj->theEdge()] = e;

		}else{
			//std::cout << "else1" << std::endl;
			//displayDebug();
			m_eOutgoing.pushBack(adj->theEdge());
			//std::cout << "else2" << std::endl;
			//displayDebug();
			node connect = newNode();
			//std::cout << "else3" << std::endl;
			//displayDebug();
			edge e = newEdge(v,connect);
			//std::cout << "else4" << std::endl;
			//displayDebug();
			m_eCopy[adj->theEdge()] = e;
			//std::cout << "else5" << std::endl;
			//displayDebug();
			m_eOrig[e].pushFront(adj->theEdge());
			//std::cout << "else6" << std::endl;
			//displayDebug();
			m_vConnect.pushBack(connect);
			//std::cout << "else7" << std::endl;
			//displayDebug();
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
	GridGraph &GG = *GridGraph_of(v); //this will always work if this function is only called by moveToCluster(node w, node v);
	List<node> nondummys = GGw.nonDummyNodes();
	while(!nondummys.empty()){
		ListIterator<node> itw;
		forall_nonconst_listiterators(node,it,nondummys){//pick nondummys whose original neighbor an original of v
			bool neighbor = false;
			List<node> originals = GGw.original(*it);
			forall_listiterators(node,it2,originals){ 
				adjEntry adj;
				forall_adj(adj,*it2){
					if (Copy(adj->twinNode()) == v) neighbor = true;
				}
				if (neighbor) break;
			}
			if (neighbor){
				itw = it;
				break;
			}
		}
	}
	forall_listiterators(node,it,GGw.nonDummyNodes()){
		node w = *it;		
		GridGraph * p_srcGG = GridGraph_of(w); //pointer to w's subGG	
		if (p_srcGG){ //if w is its own Cluster
			moveToCluster(*p_srcGG, v);  //that doesn't mean I don't have to consider all adjacent edges of w!
			GGw.m_GGList.del(GGw.m_GGList.get(GGw.m_GGList.search(*p_srcGG)));
		}else{ //if w is just a single node
			node orig = original(w).front(); //works because w isn't a dummy node
			node w2 = GG.addNode(orig); //the new node in cluster v (orig,copy,edge,connect taken care of by addNode)
			// WARNING: nodes actually connected to v will appear connected to a dummy node!
			// hhmm... or do they?
			//actually... they shouldn't, since I'm taking rerouting through addNode...
			//but then I have to fix the order in which nodes are added... lets do that then!


	
			m_vOrig[v].pushFront(orig);
			m_vCopy[orig] = v;
			IPoint wPos = GGw.getPos(w); //this is relative to GGw
			IPoint GGwPos = GGw.getPos(); //this is relative to top-level GridGraph
			IPoint vPos = getPos(v); // this is relative to this-level GridGraph
			//IPoint GGvPos = GG.getPos(); //this is relative to top-level GridGraph
			IPoint wPos_this;
			wPos_this.m_x = (wPos.m_x + GGwPos.m_x) - getPos().m_x;
			wPos_this.m_x = (wPos.m_y + GGwPos.m_y) - getPos().m_y;
			IPoint w2Pos(wPos_this.m_x - vPos.m_x, wPos_this.m_y - vPos.m_y); //pos of w2 relative to cluster v's center
			GG.setPos(w2, w2Pos);
		}
		


		//Consider all adj edges of w
		adjEntry adj;
		List<edge> delEdges;
		forall_adj(adj, w){ //note that only real edges to non-dummy nodes should be considered and all of them transferred to v
			edge e = adj->theEdge(); //the adjacent edge
			node n = adj->twinNode(); // the neighbor;
			if (!GGw.isDummy(n)){ // e must be deleted and its attributes transferred to the edge in cluster v
				delEdges.pushFront(e);
				IPoint vPos = getPos(v); //relative to this-level GridGraph;
				IPoint GGwPos = GGw.getPos(); //relative to top-level GridGraph;
				IPolyline e_edgeline = m_edgeline[e];			
				forall_nonconst_listiterators(IPoint, it, e_edgeline){
					(*it).m_x = ((*it).m_x +GGwPos.m_x) - getPos().m_x - vPos.m_x;
					(*it).m_y = ((*it).m_y +GGwPos.m_y) - getPos().m_y - vPos.m_y;					
				}
				GG.edgeline[GG.Copy(GGw.original(e).front())] = e_edgeline; //this should adress the correct edge in  cluster v
				forall_listiterators(edge,it,GGw.original(e)){ //delete reference to e from m_eCopy edgeArray;
					GGw.m_eCopy[*it] = NULL;
				}
			}
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
	}

}

void GridGraph::moveToCluster(node w, node v){ // should be done I HOPE!!		
	if (GridGraph_of(v) == NULL) { // create a new subGG if needed
		GridGraph newGG = GridGraph(*m_pGraph, m_vOrig[v].front());		
		m_GGList.pushFront(newGG);		
		m_vGridGraph[v] = &m_GGList.front();		
		(*GridGraph_of(v)).getPos().m_x = getPos(v).m_x + this->getPos().m_x;		
		(*GridGraph_of(v)).getPos().m_y = getPos(v).m_y + this->getPos().m_y;		
	}	

	GridGraph &GG = *GridGraph_of(v);		
	GridGraph * p_srcGG = GridGraph_of(w); //pointer to w's subGG	
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
	forall_adj(adj, w){ //note that w should be a neighbor of v!
		edge e = adj->theEdge(); //the adjacent edge
		node n = adj->twinNode(); // the neighbor;
		if (n == v){ // e must be deleted and its attributes transferred to the edge in cluster v
			delEdges.pushFront(e);
			IPoint vPos = getPos(v);
			IPolyline e_edgeline = m_edgeline[e];			
			forall_nonconst_listiterators(IPoint, it, e_edgeline){
				(*it).m_x -= vPos.m_x;
				(*it).m_y -= vPos.m_y;
			}
			GG.edgeline[GG.Copy(original(e).front())] = e_edgeline; //this should adress the correct edge in  cluster v
			forall_listiterators(edge,it,original(e)){ //delete reference to e from m_eCopy edgeArray;
				m_eCopy[*it] = NULL;
			}
		}else{ // e must be reconnected from w to v and its attributes transferred.			
			if (e->source() == w) moveSource(e,v);
			if (e->target() == w) moveTarget(e,v);
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
				//std::cout << "Calling findcluster on " << v << std::endl;
				findCluster(v,p);
			}
		}
	}
};












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
