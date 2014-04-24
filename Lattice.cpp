#include "stdafx.h"

#include <Lattice.h>

unsigned int Lattice::instanceCount = 1;

Lattice::Lattice(){
	std::cout << "CREATED BROKEN LATTICE!" << std::endl;
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
	srcN.init(*this,NULL);
	srcE.init(*this,NULL);
}
Lattice::Lattice(Graph &G){
	m_id = instanceCount++;
	std::cout << "CREATED LATTICE NR " << id()  << std::endl;

	displayLatticeInformation();
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
	srcN.init(*this,NULL);
	srcE.init(*this,NULL);
	lineN.init(G);
	lineE.init(G);
}

Lattice::Lattice(const Lattice &L){
	m_id = instanceCount++;
	std::cout << "COPIED LATTICE NR " << L.id() << "AS LATTICE NR " << id() << std::endl;
	//std::cout << "COPIED LATTICE WITH " << L.numberOfNodes() << " NODES!" << std::endl;
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
	srcN.init(*this,NULL);
	srcE.init(*this,NULL);
	lineN.init(*L.lineN.graphOf());
	lineE.init(*L.lineE.graphOf());
	
	node v;
	NodeArray<node> srcCopy(*this,NULL);
	NodeArray<node> trgCopy(L,NULL);
	forall_nodes(v,L){
		node w = newNode();
		srcCopy[w] = v;
		trgCopy[v] = w;
		nodePos[w] = L.nodePos[v];
	}
	edge e;
	forall_edges(e,L){
		edge f = newEdge(trgCopy[e->source()], trgCopy[e->target()]);
		height[f] = L.height[e];
		start[f] = L.start[e];
		end[f] = L.end[e];
	}

}

Lattice& Lattice::operator=(const Lattice &L){
	//std::cout << "ASSIGNED LATTICE WITH " << L.numberOfNodes() << " NODES!" << std::endl;
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
	srcN.init(*this,NULL);
	srcE.init(*this,NULL);
	lineN.init(*L.lineN.graphOf());
	lineE.init(*L.lineE.graphOf());
	
	node v;
	NodeArray<node> srcCopy(*this,NULL);
	NodeArray<node> trgCopy(L,NULL);
	forall_nodes(v,L){
		node w = newNode();
		srcCopy[w] = v;
		trgCopy[v] = w;
		nodePos[w] = L.nodePos[v];
	}
	edge e;
	forall_edges(e,L){
		edge f = newEdge(trgCopy[e->source()], trgCopy[e->target()]);
		height[f] = L.height[e];
		start[f] = L.start[e];
		end[f] = L.end[e];
	}
	return *this;
}


int Lattice::getHeight(IPoint p1, IPoint p2){
	if (p1.m_x == p2.m_x) return p1.m_x;
	if (p1.m_y == p2.m_y) return p1.m_y;
	return 0; //this should never happen
};


ListIterator<edge> Lattice::findST(List<edge> &edgelist, int pos){
	ListIterator<edge> it;
	ListIterator<edge> value = ListIterator<edge>();
	//std::cout << "findST: " << pos << std::endl;
	//forall_nonconst_listiterators(edge, it, edgelist){		
	//	std::cout << height[*it];
	//	if (height[*it] >= pos){ std::cout << "RETURN VALUE"<< height[*value];
	//	std::cout << std::endl;
	//	value = it;
	//}
	ListIterator<edge>();
	forall_nonconst_listiterators(edge, it, edgelist){		
		if (height[*it] >= pos) {
			//std::cout << "FOUND ST: " << height[*it] << std::endl;
			return value;
		}
		value = it;
	}
	return value;
}

ListIterator<edge> Lattice::findBT(List<edge> &edgelist, int pos){
	ListIterator<edge> it;
	ListIterator<edge> value = ListIterator<edge>();
	//std::cout << "findBT: " << pos << std::endl;

	//forall_nonconst_listiterators(edge, it, edgelist){		
	//	std::cout << height[*it];				
	//	value = it;
	//	if (height[*it] >= pos) std::cout << "RETURN VALUE " << height[*value];
	//	std::cout << std::endl;
	//}

	forall_nonconst_listiterators(edge, it, edgelist){
		//std::cout << height[*it] << std::endl;
		value = it;
		if (height[*it] > pos) {
			//std::cout << "FOUND BT: " << height[*it] << std::endl;
			return value;		
		}
	}
	return ListIterator<edge>();
}

char Lattice::adjPos(node v, edge e){
	node v1 = e->source();
	node v2 = e->target();
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	char val = -1;
	if (v == v1){
		if (p1.m_x == p2.m_x){ //edge is vertical
			if (p1.m_y > p2.m_y) val = 7; //N-out
			else val = 3; //S-Out
		}else{ //edge is horizontal
			if (p1.m_x > p2.m_x) val = 1; // W-Out
			else val = 5; // E-Out
		}
	}
	if (v == v2){
		if (p1.m_x == p2.m_x){ //edge is vertical
			if (p1.m_y > p2.m_y) val = 4; // S-in
			else val = 0; //N-in
		}else{ //edge is horizontal
			if (p1.m_x > p2.m_x) val = 6; //E-In
			else val = 2; //W-In
		}
	}
	OGDF_ASSERT(val!=-1);
	return val;
}

void Lattice::adjSort(adjEntry newAdj){ //sorts adjList from small to large adjPos <=> in math-pos direction
	node v = newAdj->theNode();
		//std::cout << "sorting " << newAdj->theEdge() << " at " << v;
	List<adjEntry> adjList;
	this->adjEntries(v,adjList);
	if (adjList.size() == 1) {
		//std::cout <<  " done" << std::endl;
		return;	
	}
	adjEntry pred = adjList.popBackRet(); //remove the newly added AdjEntry
	char newPos = adjPos(v,newAdj->theEdge());
	char predPos = 8;
	//std::cout << "  l: " << adjList.size();
	while(adjList.empty() == false){
		pred = adjList.popBackRet();
		//std::cout << "  l: " << adjList.size();
		predPos = adjPos(v,pred->theEdge());
		if (predPos < newPos){
			this->moveAdjAfter(newAdj,pred);
			//std::cout << " done" << std::endl;
			return;
		}
	}
	this->moveAdjBefore(newAdj,pred);
	//std::cout << " done" << std::endl;
}

edge Lattice::connect(node v1, node v2){
	edge e = this->searchEdge(v1,v2);
	if (e != NULL) if(v1 == e->source()){		
		//std::cout << "EDGE ALREADY EXISTS" << std::endl;
		return e;
	}
	e = newEdge(v1,v2);
	//std::cout << "    created edge " << e->index() << std::endl;
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	height[e] = getHeight(p1,p2);
	adjSort(e->adjSource());
	adjSort(e->adjTarget());		
	
	/*if (p1.m_x == p2.m_x){
		start[e] = min(p1.m_y, p2.m_y);
		end[e] = max(p1.m_y, p2.m_y);				
	}
	if (p1.m_y == p2.m_y){
		start[e] = min(p1.m_x, p2.m_x);
		end[e] = max(p1.m_x, p2.m_x);		
	}*/
	
	return e;
}

node Lattice::getNode(IPoint pos){ 
	//std::cout << "getting Node" << std::endl;
	node v;
	forall_nodes(v,*this){
		if (nodePos[v] == pos){			
			//std::cout << "NODE EXISTS" << std::endl;
			return v;
		}
	}
	node w = newNode();
	//std::cout << "created Node "<< w << std::endl;
	//std::cout << "created node# " << w << ": " << pos<< std::endl;
	//std::cout << "setting Pos to "<< pos << std::endl;
	nodePos[w] = pos;
	//std::cout << "set Pos to "<< pos << std::endl;
	return w;
}

bool Lattice::isOutside(IPoint pos){
	edge e = NULL;
	forall_listiterators(edge, it, hor){		
		if (height[*it] > pos.m_y){
			if (start[*it] <= pos.m_x && end[*it] >= pos.m_x){
				e = *it;
				break;
			}
		}
	}
	if (e){
		if (nodePos[e->source()].m_x > nodePos[e->target()].m_x) return true;
	}
	return false;
}
bool Lattice::isOutside(DPoint pos){
	edge e = NULL;
	bool higher = false;
	bool lower = false;
	forall_listiterators(edge, it, hor){		
		if (height[*it] > pos.m_y){
			if (start[*it] <= pos.m_x && end[*it] >= pos.m_x) lower = true;		
		}else{
			if (start[*it] <= pos.m_x && end[*it] >= pos.m_x) higher = true;		
		}
	}
	//if (e){
	//	if (nodePos[e->source()].m_x > nodePos[e->target()].m_x) return true;
	//}
	if (higher && lower) return false;
	return true;
}
List<edge> Lattice::edges(IPoint p1, IPoint p2){
	int pos = getHeight(p1,p2);
	List<edge> crossings;
	//std::cout << "getting crossings" << std::endl;
	if (p1.m_x == p2.m_x){
		//std::cout << "line is vertical" << std::endl;
		if(p1.m_y > p2.m_y){
			int a = p2.m_y;
			int b = p1.m_y;
			//std::cout << "searching h: "<< pos <<" in range (" << a << "," << b << ")" << std::endl;
			forall_listiterators(edge, it, hor){
				//std::cout << "edges: " << (*it)->index() << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
				if (height[*it] >= a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						//std::cout << hor.pos(it) << " CROSSING FOUND A " << (*it)->index() << std::endl;
						crossings.pushFront(*it);
					}
				}
			}
		}else{
			int a = p1.m_y;
			int b = p2.m_y;
			//std::cout << "searching h: "<< pos <<" in range (" << a << "," << b << ")" << std::endl;
			forall_listiterators(edge, it, hor){
				//std::cout << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
				if (height[*it] >= a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						
						//std::cout << hor.pos(it) << " CROSSING FOUND B " << (*it)->index() << std::endl;
						crossings.pushBack(*it);
					}
				}
			}
		}
	}
	if (p1.m_y == p2.m_y){
		//std::cout << "line is horizontal" << std::endl;
		if(p1.m_x > p2.m_x){
			int a = p2.m_x;
			int b = p1.m_x;
			//std::cout << "searching h: "<< pos <<" in range (" << a << "," << b << ")" << std::endl;

			forall_listiterators(edge, it, vert){
				if (height[*it] >= a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						//std::cout << vert.pos(it) << " CROSSING FOUND C " << (*it)->index() << std::endl;
						crossings.pushFront(*it);
					}
				}
			}
		}else{
			int a = p1.m_x;
			int b = p2.m_x;
			//std::cout << "searching h: "<< pos <<" in range (" << a << "," << b << ")" << std::endl;
			forall_listiterators(edge, it, vert){
				//std::cout << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
				if (height[*it] >= a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						//std::cout << vert.pos(it) << " CROSSING FOUND D " << (*it)->index() << std::endl;
						crossings.pushBack(*it);
					}
				}
			}
		}
	}
	//if (!crossings.empty()){
	//	//std::cout << "FOUND " << crossings.size() << " CROSSING EDGES: " << std::endl;
	//	forall_listiterators(edge, it, crossings){			
	//		std::cout << (*it)->index() << ": "<< nodePos[(*it)->source()] << "-> " << nodePos[(*it)->target()] <<std::endl;
	//		if (p1.m_y == p2.m_y){
	//			forall_listiterators(edge, it, vert){
	//				std::cout << "e: " << (*it)->index() << std::endl;
	//			}
	//		}
	//		if (p1.m_x == p2.m_x){
	//			forall_listiterators(edge, it, vert){
	//				std::cout << "e: " << (*it)->index() << std::endl;
	//			}
	//		}
	//	}
	//}
	return crossings;
}

node Lattice::splitEdge(edge e, int pos){
	node v1 = e->source();
	node v2 = e->target();	
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	edge sE = srcE[e];
	node sN = srcN[e];
	//std::cout << "  splitting edge " << e->index() << ": " << p1 << "->" << p2 << " at pos " << pos << std::endl;
	//if (sN) std::cout << "SPLITTING NODE BORDER OF " << sN << std::endl;
	
	if (start[e] == pos | end[e] == pos){
		if (p1.m_y == p2.m_y){
			if(p1.m_x == pos) return v1;
			else return v2;
		}
		if (p1.m_x == p2.m_x){
			if(p1.m_y == pos) return v1;
			else return v2;
		}
	}
	

	
	node newV = newNode();	
	int a = getHeight(p1,p2);
	if (p1.m_y == p2.m_y) nodePos[newV] = IPoint(pos, a);
	if (p1.m_x == p2.m_x) nodePos[newV] = IPoint(a,pos);

	edge e1 = connect(v1,newV);		
	sortIn(e1);
	edge e2 = connect(newV,v2);
	sortIn(e2);
	edge e3 = connect(v2,newV);
	edge e4 = connect(newV,v1);
	if (sE){
		srcE[e1] = sE;
		lineE[sE].pushBack(e1);
		srcE[e2] = sE;
		lineE[sE].pushBack(e2);
		srcE[e3] = sE;
		lineE[sE].pushBack(e3);
		srcE[e4] = sE;
		lineE[sE].pushBack(e4);
	}
	if (sN){
		srcN[e1] = sN;
		lineN[sN].pushBack(e1);
		srcN[e2] = sN;
		lineN[sN].pushBack(e2);
		srcN[e3] = sN;
		lineN[sN].pushBack(e3);
		srcN[e4] = sN;
		lineN[sN].pushBack(e4);
	}



	if (p1.m_y == p2.m_y){ //edge is horizontal
		//std::cout << "splitting horizontal edge" << std::endl;
		ListIterator<edge> ite = hor.get(hor.search(e));
		nodePos[newV] = IPoint(pos, a);		
		OGDF_ASSERT(ite.valid());		
		hor.del(ite);		
	}
	if (p1.m_x == p2.m_x){ //edges are vertical		
		ListIterator<edge> ite = vert.get(vert.search(e));
		nodePos[newV] = IPoint(a,pos);		
		OGDF_ASSERT(ite.valid());		
		vert.del(ite);
	}	
	int lpos;
	if (sE) {
		lpos = lineE[sE].search(e);
		if (lpos != -1) lineE[sE].del(lineE[sE].get(lpos));
	}
	if (sN){
		lpos = lineN[sN].search(e);	
		if (lpos != -1) lineN[sN].del(lineN[sN].get(lpos));
	}
	delEdge(e);

	edge e_alt = this->searchEdge(v2,v1);
	if (sE){
		lpos = lineE[sE].search(e_alt);
		if (lpos != -1)  lineE[sE].del(lineE[sE].get(lpos));
	}
	if (sN) {
		lpos = lineN[sN].search(e_alt);
		if (lpos != -1)lineN[sN].del(lineN[sN].get(lpos));
	}

	delEdge(e_alt);

	return newV;
}


bool Lattice::onOutline(node v){
	edge e;
	forall_adj_edges(e,v){
		if(m_outline.search(e) !=-1) return true;
	}
	return false;

}


int Lattice::getRot(edge e1, edge e2){
	if (e1 == NULL) return 0;
	node v1 = e1->source();
	node v2 = e1->target();
	node w1 = e2->source();
	node w2 = e2->target();
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	IPoint q1 = nodePos[w1];
	IPoint q2 = nodePos[w2];
	int a = 0;
	int b = 0;
	if (p1.m_x == p2.m_x){
		if (p1.m_y < p2.m_y) a = 0;
		else a = 2;
	}else{
		if (p1.m_x < p2.m_x) a = 3;
		else a = 1;
	}

	if (q1.m_x == q2.m_x){
		if (q1.m_y < q2.m_y) b = 0;
		else b = 2;
	}else{
		if (q1.m_x < q2.m_x) b = 3;
		else b = 1;
	}
	return b-a;
}
int Lattice::getRot(node v1, node v2){
	int rot = 0;
	ListConstIterator<edge> itBegin;
	ListConstIterator<edge> itEnd;
	forall_listiterators(edge,it,m_outline){
		if ((*it)->source() == v1) itBegin = it;
		if ((*it)->source() == v2){ 
			itEnd = it; //the one past last edge that will be looked at
			break;
		}
	}	
	if (!itBegin.valid()) return 0;
	edge e1;
	edge e2 = *itBegin;
	for(ListConstIterator<edge> ite = itBegin; ite != itEnd; ite = ite.succ()){
		if (!(ite.valid())) ite = m_outline.begin();
		e1 = e2;
		e2 = *ite;
		int delta = getRot(e1,e2);
		if (delta == 3) delta = -1;
		if (delta == -3) delta = 1;
		if (delta == 2 | delta == -2){			
			ListConstIterator<edge> it0;
			if (ite.pred().valid()) it0 = ite.pred();
			else it0 = m_outline.rbegin();
			if (it0.pred().valid()) it0 = it0.pred();
			else it0 = m_outline.rbegin();
			IPoint p1 = nodePos[(*it0)->source()];
			IPoint p2 = nodePos[(*it0)->target()];
			if (p1.m_x == p2.m_x){
				if (p1.m_y < p2.m_y){
					if (nodePos[e1->source()].m_x < nodePos[e1->target()].m_x) delta = 2;
					else delta = -2;
				}else{
					if (nodePos[e1->source()].m_x < nodePos[e1->target()].m_x) delta = -2;
					else delta = 2;
				}
			}else{
				if (p1.m_x < p2.m_x){
					if (nodePos[e1->source()].m_y < nodePos[e1->target()].m_y) delta = -2;
					else delta = 2;
				}else{
					if (nodePos[e1->source()].m_y < nodePos[e1->target()].m_y) delta = 2;
					else delta = -2;
				}
			}
		} // delta 2
		rot += delta;
	}
	return rot;
}
void Lattice::flip(List<edge> &edges){
	forall_listiterators(edge, it, edges) reverseEdge(*it);
	edges.reverse();
}

void Lattice::addToOutline(List<edge> &newOutline){
	ListIterator<edge> itStart, itEnd;
	forall_nonconst_listiterators(edge, it1, m_outline){
		if ((*it1)->source() == newOutline.front()->source()) {
			itStart = it1;
		}
		if ((*it1)->target() == newOutline.back()->target()){
			itEnd = it1;
		}
	}
	while(newOutline.empty() == false){
		if (itStart.valid()) m_outline.insertBefore(newOutline.popFrontRet(),itStart);
		else m_outline.pushFront(newOutline.popFrontRet());
	}
	for(ListIterator<edge> it2 = itStart; it2 != itEnd.succ(); it2 = it2.succ()){
		m_outline.del(it2); //this should work (?)
	}	
}

void Lattice::addToOutlineSimple(List<edge> &newOutline){
	ListIterator<edge> itStart, itEnd;
	forall_nonconst_listiterators(edge, it1, m_outline){
		if ((*it1)->target() == newOutline.front()->source()) {
			itStart = it1;
			break;
		}
	}
	while(newOutline.empty() == false){
		m_outline.insertAfter(newOutline.popBackRet(),itStart);
	}
}

edge Lattice::getprevEdge(node v){
	forall_listiterators(edge,it,m_outline){
		if ((*it)->target() == v) return (*it);
	}
	return NULL;
}

void Lattice::merge(node v1, node v2){
	edge e = NULL;
	e = this->searchEdge(v1,v2);
	if (e == NULL) e = connect(v1,v2);
	OGDF_ASSERT(nodePos[v1] == nodePos[v2]);

}

void Lattice::addLine(IPolyline line, edge E) {
	//std::cout << "adding lines of " << E << std::endl;
	if (line.empty()) return;
	//std::cout << "adding line:" << std::endl;
	//forall_listiterators(IPoint,it,line) std::cout << *it << "->";
	//std::cout << std::endl;
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;	
	forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		int h = getHeight(p1,p2);

		//node vSrc = getNode(p1);				
		List<edge> crossings = edges(p1, p2);
		if (crossings.empty()){
			node v1 = getNode(p1);
			node v2 = getNode(p2);
			edge e = connect(v1,v2);
			srcE[e] = E;
			lineE[E].pushBack(e); 
			sortIn(e);
			e = connect(v2,v1);
			srcE[e] = E;
			lineE[E].pushBack(e);
		}else{
			edge cross = crossings.popFrontRet();
			int prevH = height[cross];

			node v2 = splitEdge(cross,h);		
			node v1 = getNode(p1);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcE[e] = E;
				lineE[E].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcE[e] = E;
				lineE[E].pushBack(e);
			}									
			while(!crossings.empty()){	
				edge cross = crossings.popFrontRet();
				if (height[cross] == prevH) continue;
				prevH = height[cross];
				v1 = v2;
				v2 = splitEdge(cross,h);			
				edge e = connect(v1,v2);
				srcE[e] = E;			
				lineE[E].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcE[e] = E;			
				lineE[E].pushBack(e);			
			}
			v1 = v2;
			v2 = getNode(p2);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcE[e] = E;
				lineE[E].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcE[e] = E;
				lineE[E].pushBack(e);
			}	
		}
	}
	/*std::cout << "Added lineE[" << E->index() << "] : ";
	forall_listiterators(edge,it,lineE[E]) std::cout << (*it)->index() << ", ";
	std::cout << std::endl;
	forall_listiterators(edge,it,lineE[E]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << ";";
	std::cout << std::endl;*/
	//std::cout << "recalc ...";
	//std::cout << "added Eline ";
	//forall_listiterators(edge,it,lineE[E]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << ";";
	//std::cout << std::endl;
	checkConsistency();
	m_pOutline = CalcOutline();
	//std::cout << "done" << std::endl;
};

void Lattice::addLine(IPolyline line, edge E, List<edge> &EList) {
		if (line.empty()) return;
	//std::cout << "adding line:" << std::endl;
	//forall_listiterators(IPoint,it,line) std::cout << *it << "->";
	//std::cout << std::endl;
	EList.clear();
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;	
	forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		int h = getHeight(p1,p2);

		//node vSrc = getNode(p1);				
		List<edge> crossings = edges(p1, p2);

		if (crossings.empty()){
			node v1 = getNode(p1);
			node v2 = getNode(p2);
			edge e = connect(v1,v2);
			srcE[e] = E;
			lineE[E].pushBack(e); 
			sortIn(e);
			e = connect(v2,v1);
			srcE[e] = E;
			lineE[E].pushBack(e);
		}else{

			edge cross = crossings.popFrontRet();
			int prevH = height[cross];

			node v2 = splitEdge(cross,h);		
			node v1 = getNode(p1);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcE[e] = E;
				lineE[E].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcE[e] = E;
				lineE[E].pushBack(e);
			}									
			while(!crossings.empty()){	
				edge cross = crossings.popFrontRet();
				if (height[cross] == prevH) continue;

				edge srce = srcE[cross];
				if (srce != NULL) if (EList.search(srce) == -1) EList.pushBack(srce);

				prevH = height[cross];
				v1 = v2;
				v2 = splitEdge(cross,h);			
				edge e = connect(v1,v2);
				srcE[e] = E;			
				lineE[E].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcE[e] = E;			
				lineE[E].pushBack(e);			
			}
			v1 = v2;
			v2 = getNode(p2);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcE[e] = E;
				lineE[E].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcE[e] = E;
				lineE[E].pushBack(e);
			}	
		}
	}
	/*std::cout << "Added lineE[" << E->index() << "] : ";
	forall_listiterators(edge,it,lineE[E]) std::cout << (*it)->index() << ", ";
	std::cout << std::endl;
	forall_listiterators(edge,it,lineE[E]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << ";";
	std::cout << std::endl;*/
	//std::cout << "recalc ...";
	//std::cout << "added Eline ";
	//forall_listiterators(edge,it,lineE[E]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << ";";
	//std::cout << std::endl;
	checkConsistency();

	m_pOutline = CalcOutline();
	//std::cout << "done" << std::endl;
};

bool Lattice::addLine(IPolyline line, node N, List<edge> &Elist) {
	
	if (line.empty()) return true;
	if (!lineN[N].empty()) while (true) std::cout << N << " already registered in Lattice!";
	bool legal = true;
	Elist.clear();
	List<edge> singles;
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;	
		forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		int h = getHeight(p1,p2);
		int a = 0; int b = 0;
		if (p1.m_x == p2.m_x)
			a = p1.m_y; b = p2.m_y;
		if (p1.m_y == p2.m_y)
			a = p1.m_x; b = p2.m_x;
		

		List<edge> crossings = edges(p1, p2);
		if (crossings.empty()){
			node v1 = getNode(p1);
			node v2 = getNode(p2);
			edge e = connect(v1,v2);
			srcN[e] = N;
			lineN[N].pushBack(e); 
			sortIn(e);
			e = connect(v2,v1);
			srcN[e] = N;
			lineN[N].pushBack(e);
		}else{
			edge cross = crossings.popFrontRet();
			int prevH = height[cross];

			node v2 = splitEdge(cross,h);		
			node v1 = getNode(p1);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcN[e] = N;
				lineN[N].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcN[e] = N;
				lineN[N].pushBack(e);
			}									
			while(!crossings.empty()){	
				edge cross = crossings.popFrontRet();
				if (height[cross] == prevH) continue;
				prevH = height[cross];
				
				edge srce = srcE[cross];			

				if (height[cross] != a && height[cross] != b){ //this makes sure that a starting- or endpoint is not registered as a crossing
					if (srcN[cross] != NULL && srcN[cross] != N) {						
					//removeLine(N);
					//return false;
						legal = false;
				}	
					if (Elist.search(srce) == -1) Elist.pushBack(srce);
					else{
						int listpos = singles.search(srce);
						if (listpos == -1) singles.pushBack(srce);
						else singles.del(singles.get(listpos));
					}
				}


				v1 = v2;
				v2 = splitEdge(cross,h);			
				edge e = connect(v1,v2);
				srcN[e] = N;			
				lineN[N].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcN[e] = N;			
				lineN[N].pushBack(e);			
			} //while ! crossings.empty

			v1 = v2;
			v2 = getNode(p2);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcN[e] = N;
				lineN[N].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcN[e] = N;
				lineN[N].pushBack(e);
			}	
		}
	}
	//std::cout << "added Nline ";
	//forall_listiterators(edge,it,lineN[N]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << ";";
	//std::cout << std::endl;
	checkConsistency();

	if (!singles.empty()){
		legal = false;
	}
	m_pOutline = CalcOutline();	
	return legal;
	
};
void Lattice::displayLatticeInformation(){
	std::cout << "info Lattice Nr " << id() << std::endl;
	std::cout << "number of Nodes " << numberOfNodes() << std::endl;
	std::cout << "number of Edges " << numberOfEdges() << std::endl;
	checkConsistency();
}
void Lattice::checkConsistency(){
	//no double-nodes:
	//std::cout << "checking consistency of lattice " << id() << std::endl;
	node v,w;
	edge e;
	forall_edges(e,*this){
		edge f;
		node v1 = e->source();
		node v2 = e->target();
		forall_edges(f,*this){
			if (f->index() != e->index()){
			if (f->source() == e->source() && f->target() == e->target()){
				std::cout << id() <<" "<< "double edge ";
				std::cout << e->index() << "  " << nodePos[v1] << e << nodePos[v2] << " sE "<< srcE[e] << " sN "<< srcN[e] << std::endl;
				std::cout << "";
			}
			}
		}
	}
	forall_nodes(v,*this){
		List<edge> sE;
		List<node> sN;
		edge e;
		forall_adj_edges(e,v){
			edge srE = srcE[e];
			node srN = srcN[e];
			if (srE){ 
				if (sE.search(srE) == -1) sE.pushFront(srE);
				else sE.del(sE.get(sE.search(srE)));
			}
			if (srN){ 
				if (sN.search(srN) == -1) sN.pushFront(srN);
				else sN.del(sN.get(sN.search(srN)));
			}
		}
		forall_listiterators(edge,it,sE){
			std::cout << id() <<" "<< "faulty edgeline " << *it << " at " << nodePos[v] << ":"<< std::endl;
			displayNodeInformation(v);
			if (lineE[*it].empty()){
				std::cout << id() <<" "<< "edges registered but not ins list:" << std::endl;
				edge f;
				forall_edges(f, *this){
					if (srcE[f] == *it){
						std::cout << f << ":  " << nodePos[f->source()] << "->" << nodePos[f->target()] << std::endl;
					}
				}
			}
			forall_listiterators(edge,it2,lineE[*it]){
				edge f = *it2;
				//std::cout << f << std::endl;
				std::cout << nodePos[(*it2)->source()] << "->" << nodePos[(*it2)->target()] << std::endl;
			}
		}
		forall_listiterators(node,it,sN){
			std::cout << id() <<" "<< "faulty nodeline " << *it << " at " << nodePos[v] << ":"<< std::endl;
			if (lineN[*it].empty()){
				std::cout << id() <<" "<< "edges registered but not ins list:" << std::endl;
				edge f;
				forall_edges(f, *this){
					if (srcN[f] == *it){
						std::cout << f << ":  " << nodePos[f->source()] << "->" << nodePos[f->target()] << std::endl;
					}
				}
			}
			forall_listiterators(edge,it2,lineN[*it]){
				edge f = *it2;
				std::cout << nodePos[(*it2)->source()] << "->" << nodePos[(*it2)->target()] << std::endl;
			}
		}

		forall_nodes(w,*this){
			if (w != v){
				if (nodePos[w] == nodePos[v]){
					std::cout << id() <<" " << "nodes " << v << " and " << w << " share location " << nodePos[w] << std::endl;
					std::cout << "";
				}
			}
		}
		forall_edges(e,*this){
			IPoint p1 = nodePos[e->source()];
			IPoint p2 = nodePos[e->target()];
			IPoint q = nodePos[v];
			if (p1.m_y == p2.m_y){
				int a = min(p1.m_x, p2.m_x);
				int b = max(p1.m_x, p2.m_x);
				if (q.m_y == p1.m_y && a < q.m_x && q.m_x < b){
					std::cout << id() <<" " << "node " << v << " touches edges " << p1 << "->" << p2 << " of srcE: " << srcE[e] << " srcN: " << srcN[e] << std::endl;
					std::cout << "";
				}
			}
			if (p1.m_x == p2.m_x){
				int a = min(p1.m_y, p2.m_y);
				int b = max(p1.m_y, p2.m_y);
				if (q.m_x == p1.m_x && a < q.m_y && q.m_y < b){
					std::cout << id() <<" " << "node " << v << " touches edges " << p1 << "->" << p2 << " of srcE: " << srcE[e] << " srcN: " << srcN[e] << std::endl;
					std::cout << "";
				}
			}
		}
	}	
	forall_edges(e,*this){
		if (e->source() == e->target()){
			std::cout << id() <<" "<< "edge " << e->index() << " loops on node " << e->source() << " at location " << nodePos[e->source()] << std::endl;
			std::cout << "";
		}
		if (srcN[e] == NULL && srcE[e] == NULL){
			std::cout << id() <<" "<< "edge " << e->index() << ": " << nodePos[e->source()] << "->" << nodePos[e->target()] << "is not connected to src Noder or Edge " << std::endl;
			std::cout << "";
		}

	}

}

bool Lattice::addLine(IPolyline line, node N) {
	//std::cout << "started addLine N" << std::endl;	
	if (line.empty()) return true;
	std::cout << "adding to lattice Node " << N << std::endl;
	if (!lineN[N].empty()) {std::cout << N << " already registered in Lattice!"; throw -1;}
	bool legal;
	List<edge> Elist;
	List<edge> singles;
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;	
		forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		int h = getHeight(p1,p2);
		int a = 0; int b = 0;
		if (p1.m_x == p2.m_x)
			a = p1.m_y; b = p2.m_y;
		if (p1.m_y == p2.m_y)
			a = p1.m_x; b = p2.m_x;
		//std::cout << "getting Crossings" << std::endl;
		List<edge> crossings = edges(p1, p2);
		if (crossings.empty()){
			node v1 = getNode(p1);
			node v2 = getNode(p2);
			edge e = connect(v1,v2);
			srcN[e] = N;
			lineN[N].pushBack(e); 
			sortIn(e);
			e = connect(v2,v1);
			srcN[e] = N;
			lineN[N].pushBack(e);
		}else{

			edge cross = crossings.popFrontRet();
			int prevH = height[cross];

			node v2 = splitEdge(cross,h);		
			node v1 = getNode(p1);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcN[e] = N;
				lineN[N].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcN[e] = N;
				lineN[N].pushBack(e);
			}									
			while(!crossings.empty()){	
				edge cross = crossings.popFrontRet();
				if (height[cross] == prevH) continue;
				prevH = height[cross];

				edge srce = srcE[cross];			

				if (height[cross] != a && height[cross] != b){ //this makes sure that a starting- or endpoint is not registered as a crossing
					if (srcN[cross] != NULL){						
						if (srcN[cross] != N) legal = false;
					}else{
						if (Elist.search(srce) == -1) Elist.pushBack(srce);
						else{
							int listpos = singles.search(srce);
							if (listpos == -1) singles.pushBack(srce);
							else singles.del(singles.get(listpos));
						}
					}
				}


				v1 = v2;
				v2 = splitEdge(cross,h);			
				edge e = connect(v1,v2);
				srcN[e] = N;			
				lineN[N].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcN[e] = N;			
				lineN[N].pushBack(e);			
			} //while ! crossings.empty

			v1 = v2;
			v2 = getNode(p2);
			if (v1 != v2){
				edge e = connect(v1,v2);
				srcN[e] = N;
				lineN[N].pushBack(e); 
				sortIn(e);
				e = connect(v2,v1);
				srcN[e] = N;
				lineN[N].pushBack(e);
			}	
		}
	}
	//std::cout << "added Nline ";
	//forall_listiterators(edge,it,lineN[N]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << ";";
	//std::cout << std::endl;
	checkConsistency();
	//std::cout << "checked consistency" << std::endl;

	if (!singles.empty()){
		legal = false;
	}
	m_pOutline = CalcOutline();	
	return legal;
};

List<edge> Lattice::edgesOn(IPoint p){
	int x = p.m_x;
	int y = p.m_y;
	List<edge> edges;
	forall_listiterators(edge,it,hor){
		edge e = *it;
		if (height[e] > y) break;
		if (height[e] == y && start[e] < x && x < end[e]) edges.pushBack(srcE[e]);					
	}
	forall_listiterators(edge,it,vert){
		edge e = *it;
		if (height[e] > x) break;
		if (height[e] == x && start[e] < y && y < end[e]) edges.pushBack(srcE[e]);					
	}

}

node Lattice::clusterOn(IPoint p){
	int x = p.m_x;
	int y = p.m_y;
	edge above = NULL;
	edge below = NULL;;
	forall_listiterators(edge,it,hor){
		edge e = *it;
		if (height[e] <= y && start[e] < x && x < end[e]) above = e;	
		if (height[e] >= y && start[e] < x && x < end[e]) {below = e; break;}
	}
	node a = NULL;
	node b = NULL;
	if (above) a = srcN[above];
	if (below) a = srcN[below];

	if (a != NULL && a == b) return a;
	else return NULL;
}


void Lattice::sortIn(edge e){	
	node v1 = e->source();
	node v2 = e->target();
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];	
	int pos = getHeight(p1,p2);
	//std::cout << "sorting in e# " << e->index() << ": " << pos << "  (" << height[e]<< ")" << std::endl;

	if (p1.m_x == p2.m_x){
		start[e] = min(p1.m_y, p2.m_y);
		end[e] = max(p1.m_y, p2.m_y);	
		//std::cout << "inserting v-edge " << e << ":" << start[e] << "->" << end[e] << "  h: " << pos << std::endl;
		if (vert.empty()) {
			vert.pushFront(e);
			//std::cout << "push " << e->index() << std::endl;
		}
		else{					
			if (p1.m_y > p2.m_y) {
				ListIterator<edge> ST = findST(vert, pos);
				if (ST.valid()) vert.insertAfter(e , ST);	
				else vert.pushFront(e);
			}
			else{
				ListIterator<edge> BT = findBT(vert, pos);
				if (BT.valid()) vert.insertBefore(e, BT);
				else vert.pushBack(e);
			}
		}
	}
	if (p1.m_y == p2.m_y){
		start[e] = min(p1.m_x, p2.m_x);
		end[e] = max(p1.m_x, p2.m_x);
		//std::cout << "inserting h-edge " << e << ":" << start[e] << "->" << end[e] << "  h: " << pos << std::endl;
		if (hor.empty()) hor.pushFront(e);
		else{			
			if (p1.m_x > p2.m_x){				
				ListIterator<edge> ST = findST(hor, pos);
				if (ST.valid()) hor.insertAfter(e , ST);	
				else hor.pushFront(e);
			}else{
				ListIterator<edge> BT = findBT(hor, pos);
				if (BT.valid()) hor.insertBefore(e, BT);
				else hor.pushBack(e);
			}
		}
	}
	//std::cout << "vert:" << std::endl;
	//forall_listiterators(edge,it,vert){
	//	edge e = *it;
	//	std::cout << "  #" << e->index() << " h: " << height[e] << std::endl;
	//}
	//std::cout << "hor:" << std::endl;
	//forall_listiterators(edge,it,hor){
	//	edge e = *it;
	//	std::cout << "  #" << e->index() << " h: " << height[e] << std::endl;	
	//}
	//std::cout << std::endl<< std::endl;
}

edge Lattice::getRight(edge e){ //returns the next OUTGOING edge in pos direction.
	//std::cout << "getRight(edge e) ";
	/*OGDF_ASSERT(e_in->target() == w);*/
	node w = e->target();
	/*adj,Entry adj;*/
	adjEntry inc = e->adjTarget();
	/*forall_adj(adj,w){
		if (adj->theEdge() == e_in) inc = adj;
	}*/
	List<adjEntry> adjList;
	this->adjEntries(w,adjList);
	//std::cout << "node " << w << " has " << adjList.size() << " adjEntries";
	adjEntry out = inc->cyclicSucc(); 
	edge e_out = out->theEdge();
	int loopcount = 0;
	
	while (e_out->source() != w){	
		loopcount ++;
		//if (loopcount > 10) throw -1; VERY BAD IDEA IDEA!!!!
		if (loopcount > 10) {
			std::cout << "exceeded normal adj limit "<< loopcount << " , node is:" << std::endl;
			displayNodeInformation(w);
		}
		out = out->cyclicSucc();
		//std::cout << out << std::endl;
		e_out = out->theEdge();
	}
	//std::cout << "return e_out";
	return e_out;
}
void Lattice::removeLine(IPolyline line){
	if (line.empty()) return;
	edge oldEdge = NULL;
	edge e = NULL;
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;
	List<edge> newOutline;
	node vStart = NULL;
	node vEnd = NULL;
	node v1 = NULL;
	node v2 = NULL;
	//std::cout << "Deleting line [" << p2;
	//forall_listiterators(IPoint, it, line){
	//	std::cout << "->" << (*it) ;
	//}
	//std::cout << "]" << std::endl;
	forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		//int height = getHeight(p1,p2);
		//v1 = getNode(p1);
		//v2 = getNode(p2);		
		List<ListIterator<edge>> delFromList;
		if (p1.m_x == p2.m_x){ //line is vertical		
			int a = min(p1.m_y, p2.m_y);
			int b = max(p1.m_y, p2.m_y);
			//std::cout << "vert: " << std::endl;
			forall_nonconst_listiterators(edge, it, vert){
				//std::cout << "remove :" << (*it)->index() << " h: " << height[*it] << ": [" << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << "]" << std::endl;
				if (height[*it] == p1.m_x){
					if (start[*it] >= a && end[*it] <= b){
						//delFromList.pushFront(it);
						node v1 = (*it)->source();
						node v2 = (*it)->target();
						//std::cout << "  deleting [" << nodePos[v1] << "->" << nodePos[v2] << "]" << std::endl;
						std::cout << "would delete " << (*it)->index()  << " [" << nodePos[v1] << "->" << nodePos[v2] << "]"<< std::endl;
						//delEdge(*it);
						edge e = searchEdge(v1,v2);
						v1 = e->source();
						v2 = e->target();
						std::cout << "would delete " << e->index()  << " [" << nodePos[v1] << "->" << nodePos[v2] << "]"<< std::endl;
						//std::cout << "  deleting [" << nodePos[e->source()] << "->" << nodePos[e->target()] << "]" << std::endl;
						//delEdge(e);
						if (v1->indeg() + v1->outdeg() == 0) delNode(v1);
						if (v2->indeg() + v2->outdeg() == 0) delNode(v2);
						//if (end[*it] >= b) break; doesn't work, List not sorted by start-end !!
					}
				}
				if (height[*it] > p1.m_x) break;

			}
			//std::cout << std::endl;
			forall_listiterators(ListIterator<edge>,itit,delFromList){
				vert.del(*itit);
			}
		}		
		if (p1.m_y == p2.m_y){ //line is horizontal
			int a = min(p1.m_x, p2.m_x);
			int b = max(p1.m_x, p2.m_x);
			//std::cout << "hor: " << std::endl;
			forall_nonconst_listiterators(edge, it, hor){
				//	std::cout << "remove :" << (*it)->index() << " h: " << height[*it] << ": [" << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << "]" << std::endl;
				if (height[(*it)] == p1.m_y){
					if (start[*it] >= a && end[*it] <= b){
						//delFromList.pushFront(it);
						node v1 = (*it)->source();
						node v2 = (*it)->target();
						//std::cout << "  deleting [" << nodePos[v1] << "->" << nodePos[v2] << "]" << std::endl;
						std::cout << "would delete " << (*it)->index()  << " [" << nodePos[v1] << "->" << nodePos[v2] << "]"<< std::endl;
						//delEdge(*it);
						edge e = searchEdge(v1,v2);
						v1 = e->source();
						v2 = e->target();
						std::cout << "would delete " << e->index()  << " [" << nodePos[v1] << "->" << nodePos[v2] << "]"<< std::endl;
						//std::cout << "  deleting [" << nodePos[e->source()] << "->" << nodePos[e->target()] << "]" << std::endl;
						//delEdge(e);
						if (v1->indeg() + v1->outdeg() == 0) delNode(v1);
						if (v2->indeg() + v2->outdeg() == 0) delNode(v2);
						/*if (end[*it] >= b) break;*/ //doesn't work, list not sorted by start-end!
					}
				}
				if (height[*it] > p1.m_y) break;
			}
			//std::cout << std::endl;
			forall_listiterators(ListIterator<edge>,itit,delFromList){
				hor.del(*itit);
			}
		}				
	}
	//forall_listiterators(edge,it,vert){
	//	edge e = *it;
	//	std::cout << "#" << e->index() << " " << nodePos[e->source()] << "->" << nodePos[e->target()] << std::endl;
	//}
	//m_pOutline = CalcOutline();
	//std::cout << "line removed" << std::endl;
}

void Lattice::removeLine(edge E ){
	List<edge> line = lineE[E];
	if (line.empty()) return;	
	/*std::cout << "del lineE[" << E->index() << "] : ";
	forall_listiterators(edge,it,lineE[E]) std::cout << (*it)->index() << ", ";
	std::cout << std::endl;*/
	edge e;
	//std::cout << "removing all lines of " << E << std::endl;
	forall_edges(e,*this){
		if (srcE[e] == E && line.search(e) == -1){
			std::cout << "FOUND ROGUE EDGE WHILE ATTEMPTING DELETION!" << std::endl;
			std::cout << nodePos[e->source()] << "->" << nodePos[e->target()] << std::endl;
		}
	}

	List<ListIterator<edge>> delFromList;
	forall_nonconst_listiterators(edge, it, vert) if (line.search(*it) != -1) delFromList.pushBack(it);
	forall_listiterators(ListIterator<edge>,itit,delFromList) vert.del(*itit);
	delFromList.clear();
	forall_nonconst_listiterators(edge, it, hor) if (line.search(*it) != -1) delFromList.pushBack(it);
	forall_listiterators(ListIterator<edge>,itit,delFromList) hor.del(*itit);
	while(!line.empty()){
		edge e = line.popFrontRet();
		node v1 = e->source();
		node v2 = e->target();
		
		//std::cout << "WILL DELETE: " << e->index() << " " << e  << " [" << nodePos[v1] << "->" << nodePos[v2] << "]"<< std::endl;
		delEdge(e);
		//std::cout << "checking if v1 " << v1 << " needs to be deleted" << std::endl;
		if (v1->degree() == 0) delNode(v1);
		else if (v1->degree() == 4){ //if deletion leaves a line with a node that could be merged, merge it.
			//std::cout << "blav1" << std::endl;
			node p = NULL;
			node q = NULL;
			adjEntry adj = v1->firstAdj();
			p = adj->twinNode();
			edge e1 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e2 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e3 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e4 = adj->theEdge();
			if (srcN[e1] == srcN[e2] && srcN[e1] == srcN[e3] && srcN[e1] == srcN[e4]){
				if (srcE[e1] == srcE[e2] && srcE[e1] == srcE[e3] && srcE[e1] == srcE[e4]){
					if(nodePos[p].m_x == nodePos[q].m_x){ //line is vertical
						//std::cout << "DELETING PATH NODE src vert" << std::endl;
						//displayNodeInformation(v1);
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = vert.search(e1);
						if (s1 != -1)vert.del(vert.get(s1));
						int s2 = vert.search(e2);
						if (s2 != -1) vert.del(vert.get(s2));
						int s3 = vert.search(e3);
						if (s3 != -1) vert.del(vert.get(s3));
						int s4 = vert.search(e4);
						if (s4 != -1) vert.del(vert.get(s4));

						delNode(v1);
					}
					if(nodePos[p].m_y == nodePos[q].m_y){ //line is horizontal
						//std::cout << "DELETING PATH NODE src hor" << std::endl;
						//displayNodeInformation(v1);
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = hor.search(e1);
						if (s1 != -1)hor.del(hor.get(s1));
						int s2 = hor.search(e2);
						if (s2 != -1) hor.del(hor.get(s2));
						int s3 = hor.search(e3);
						if (s3 != -1) hor.del(hor.get(s3));
						int s4 = hor.search(e4);
						if (s4 != -1) hor.del(hor.get(s4));

						delNode(v1);

					}
				}
			}						
		}//if deg(v1) == 4
		
		//std::cout << "checking if v2 " << v2 << "needs to be deleted" << std::endl;

		if (v2->degree() == 0) delNode(v2);
		else if (v2->degree() == 4){ //if deletion leaves a line with a node that could be merged, merge it.
			node p = NULL;
			node q = NULL;
			adjEntry adj = v2->firstAdj();
			p = adj->twinNode();
			edge e1 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e2 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e3 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e4 = adj->theEdge();
			if (srcN[e1] == srcN[e2] && srcN[e1] == srcN[e3] && srcN[e1] == srcN[e4]){
				if (srcE[e1] == srcE[e2] && srcE[e1] == srcE[e3] && srcE[e1] == srcE[e4]){
					if(nodePos[p].m_x == nodePos[q].m_x){ //line is vertical
						//std::cout << "DELETING PATH NODE trg vert" << std::endl;
						//displayNodeInformation(v2);
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){

							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = vert.search(e1);
						if (s1 != -1)vert.del(vert.get(s1));
						int s2 = vert.search(e2);
						if (s2 != -1) vert.del(vert.get(s2));
						int s3 = vert.search(e3);
						if (s3 != -1) vert.del(vert.get(s3));
						int s4 = vert.search(e4);
						if (s4 != -1) vert.del(vert.get(s4));

						delNode(v2);
					}
					if(nodePos[p].m_y == nodePos[q].m_y){ //line is horizontal
						//std::cout << "DELETING PATH NODE trg hor" << std::endl;
						//displayNodeInformation(v2);
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = hor.search(e1);
						if (s1 != -1)hor.del(hor.get(s1));
						int s2 = hor.search(e2);
						if (s2 != -1) hor.del(hor.get(s2));
						int s3 = hor.search(e3);
						if (s3 != -1) hor.del(hor.get(s3));
						int s4 = hor.search(e4);
						if (s4 != -1) hor.del(hor.get(s4));

						delNode(v2);

					}
				}
			}						
		}//if deg(v1) == 4

	}
	//std::cout << "delline e done" << std::endl;
		forall_edges(e,*this){
		if (srcE[e] == E){
			std::cout << "FOUND ROGUE EDGE AFTER ATTEMPTING DELETION!" << std::endl;
			std::cout << nodePos[e->source()] << "->" << nodePos[e->target()] << std::endl;
		}
	}
		lineE[E].clear();
	checkConsistency();
	//std::cout << "checking done" << std::endl;
	m_pOutline = CalcOutline();
	

	//std::cout << "recalced outline" << std::endl;

}

void Lattice::displayNodeInformation(node v){
	adjEntry adj;
	std::cout << "At " << nodePos[v] << " node " << v << std::endl;
	forall_adj(adj,v){
		edge e = adj->theEdge();
		node v1 = e->source();
		node v2 = e->target();
		std::cout << "edge: "<< e->index() << "  " << nodePos[v1] << e << nodePos[v2] << " sE "<< srcE[e] << " sN "<< srcN[e] << std::endl;
	}
}

void Lattice::nodeInfoOn(IPoint p){
	node v;
	node w = NULL;
	forall_nodes(v,*this){
		if (nodePos[v] == p){			
			//std::cout << "NODE EXISTS" << std::endl;
			w = v;
		}
	}
	if (w) displayNodeInformation(w);
}

void Lattice::removeLine(node N ){
	List<edge> line = lineN[N];
	if (line.empty()) return;	
	//std::cout << "del lineN[" << N->index() << "] : ";
	//forall_listiterators(edge,it,lineN[N]) std::cout << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << " ; ";
	//std::cout << std::endl;

	List<ListIterator<edge>> delFromList;
	forall_nonconst_listiterators(edge, it, vert) if (line.search(*it) != -1) delFromList.pushBack(it);
	forall_listiterators(ListIterator<edge>,itit,delFromList) vert.del(*itit);
	delFromList.clear();
	forall_nonconst_listiterators(edge, it, hor) if (line.search(*it) != -1) delFromList.pushBack(it);
	forall_listiterators(ListIterator<edge>,itit,delFromList) hor.del(*itit);
	while(!line.empty()){
		edge e = line.popFrontRet();
		node v1 = e->source();
		node v2 = e->target();
		//std::cout << "WILL DELETE: " << e->index()  << " [" << nodePos[v1] << "->" << nodePos[v2] << "]"<< std::endl;
		delEdge(e);
		if (v1->degree() == 0) delNode(v1);
		if (v2->degree() == 0) delNode(v2);
		
		if (v1->degree() == 4){ //if deletion leaves a line with a node that could be merged, merge it.
			node p = NULL;
			node q = NULL;
			adjEntry adj = v1->firstAdj();
			p = adj->twinNode();
			edge e1 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e2 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e3 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e4 = adj->theEdge();
			if (srcN[e1] == srcN[e2] && srcN[e1] == srcN[e3] && srcN[e1] == srcN[e4]){
				if (srcE[e1] == srcE[e2] && srcE[e1] == srcE[e3] && srcE[e1] == srcE[e4]){
					if(nodePos[p].m_x == nodePos[q].m_x){ //line is vertical
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = vert.search(e1);
						if (s1 != -1)vert.del(vert.get(s1));
						int s2 = vert.search(e2);
						if (s2 != -1) vert.del(vert.get(s2));
						int s3 = vert.search(e3);
						if (s3 != -1) vert.del(vert.get(s3));
						int s4 = vert.search(e4);
						if (s4 != -1) vert.del(vert.get(s4));

						delNode(v1);
					}
					if(nodePos[p].m_y == nodePos[q].m_y){ //line is horizontal
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = hor.search(e1);
						if (s1 != -1)hor.del(hor.get(s1));
						int s2 = hor.search(e2);
						if (s2 != -1) hor.del(hor.get(s2));
						int s3 = hor.search(e3);
						if (s3 != -1) hor.del(hor.get(s3));
						int s4 = hor.search(e4);
						if (s4 != -1) hor.del(hor.get(s4));

						delNode(v1);

					}
				}
			}						
		}//if deg(v1) == 4

		if (v2->degree() == 4){ //if deletion leaves a line with a node that could be merged, merge it.
			node p = NULL;
			node q = NULL;
			adjEntry adj = v2->firstAdj();
			p = adj->twinNode();
			edge e1 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e2 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e3 = adj->theEdge();
			adj = adj->succ();
			if (adj->twinNode() != p) q = adj->twinNode();
			edge e4 = adj->theEdge();
			if (srcN[e1] == srcN[e2] && srcN[e1] == srcN[e3] && srcN[e1] == srcN[e4]){
				if (srcE[e1] == srcE[e2] && srcE[e1] == srcE[e3] && srcE[e1] == srcE[e4]){
					if(nodePos[p].m_x == nodePos[q].m_x){ //line is vertical
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = vert.search(e1);
						if (s1 != -1)vert.del(vert.get(s1));
						int s2 = vert.search(e2);
						if (s2 != -1) vert.del(vert.get(s2));
						int s3 = vert.search(e3);
						if (s3 != -1) vert.del(vert.get(s3));
						int s4 = vert.search(e4);
						if (s4 != -1) vert.del(vert.get(s4));

						delNode(v2);
					}
					if(nodePos[p].m_y == nodePos[q].m_y){ //line is horizontal
						edge sE = srcE[e1];
						node sN = srcN[e1];
						edge ne1 = connect (p,q);
						sortIn(ne1);
						edge ne2 = connect (q,p);
						
						srcE[ne1] = sE;
						srcN[ne1] = sN;
						srcE[ne2] = sE;
						srcN[ne2] = sN;

						if (sN){
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e1)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e2)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e3)));
							lineN[sN].del(lineN[sN].get(lineN[sN].search(e4)));
							lineN[sN].pushBack(ne1);
							lineN[sN].pushBack(ne2);
						}
						if (sE){
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e1)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e2)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e3)));
							lineE[sE].del(lineE[sE].get(lineE[sE].search(e4)));
							lineE[sE].pushBack(ne1);
							lineE[sE].pushBack(ne2);
						}

						int s1 = hor.search(e1);
						if (s1 != -1)hor.del(hor.get(s1));
						int s2 = hor.search(e2);
						if (s2 != -1) hor.del(hor.get(s2));
						int s3 = hor.search(e3);
						if (s3 != -1) hor.del(hor.get(s3));
						int s4 = hor.search(e4);
						if (s4 != -1) hor.del(hor.get(s4));

						delNode(v2);

					}
				}
			}						
		}//if deg(v1) == 4

	}
	checkConsistency();
	m_pOutline = CalcOutline();
	lineN[N].clear();
}
IPolyline Lattice::outline(){
	return m_pOutline;
}

bool Lattice::isInside(IPoint p){
	return (!isOutside(p));
}

bool Lattice::isInside(DPoint p){
	//std::cout << p;
	//if (isOutside(p)) std::cout << " is outside" << std::endl;
	//else std::cout << " is inside" << std::endl;
	return (!isOutside(p));
}
IPolyline Lattice::CalcOutline(){
	if (this->empty()) return IPolyline();
	IPolyline ret;
	node v,w;
	w = this->chooseNode();
	forall_nodes(v,*this){ //get the southernmost node
		if (nodePos[v].m_y >= nodePos[w].m_y){
			if (nodePos[v].m_y > nodePos[w].m_y) w = v;
			else if (nodePos[v].m_x < nodePos[w].m_x){
				w = v;
			}			
		}
	}
	IPoint tempPos = nodePos[w];
	tempPos.m_y += 1;
	node tempNode = getNode(tempPos);
	edge tempEdge = connect(tempNode, w);	

		//std::cout << "			TEMPEDGE edge " << tempEdge->index() << std::endl;
		
	edge e = getRight(tempEdge);
	//std::cout << "adding source of " << e->index() << " " << nodePos[e->source()] << "->" << nodePos[e->target()] << std::endl;		
	ret.pushBack(nodePos[e->source()]);
	//std::cout << "adding target of " << e->index() << " " << nodePos[e->source()] << "->" << nodePos[e->target()] << std::endl;		
	ret.pushBack(nodePos[e->target()]);
	edge firstEdge = e;		
	e = getRight(e);	
	
	bool strikeOne = false;
	while(e != firstEdge){
		//std::cout << e->index() << " " << e->source() << "-" << e->target() << " is part of outine" << std::endl;		
		//ret.pushBack(nodePos[e->target()]);
		//std::cout <<  "loop" << std::endl;
		//std::cout << "adding target of " << e->index() << " " << nodePos[e->source()] << "->" << nodePos[e->target()] << std::endl;		
		if (e->index() == 1095){
			//std::cout << "bubu" << std::endl;
			if (strikeOne){
				ret.clear();
				ret.pushBack(IPoint(0,0));				
				return ret;
			}
			strikeOne = true;
			
		}
		ret.pushBack(nodePos[e->target()]);
		//std::cout << "adding target of " << e->index() << " " << nodePos[e->source()] << "->" << nodePos[e->target()] << "done" << std::endl;		
		//debug
		edge prev = e;
		e = getRight(e);
		//std::cout<< "got next edge" << std::endl;
		/*if (prev == e){
			std::cout << "error" << std::endl;
		}*/
	}
	//std::cout << "done recalcing" << std::endl;
	delNode(tempNode);

	return ret;
}

void Lattice::getConnectedElements(edge E, List<edge> &edges, List<node> &nodes){
	List<edge> line = lineE[E];
	edges.clear();
	nodes.clear();
	if (line.empty()) return;

	forall_listiterators(edge, it, line){
		node v = (*it)->source();
		edge e;
		forall_adj_edges(e,v){
			edge sE = srcE[e];
			node sN = srcN[e];
			if (sE != E){
				if (sE != NULL && edges.search(sE) == -1) edges.pushBack(sE);
				if (sN != NULL && nodes.search(sN) == -1) nodes.pushBack(sN);
			}
		}
		v = (*it)->target();
		forall_adj_edges(e,v){
			edge sE = srcE[e];
			node sN = srcN[e];
			if (sE != E){
				if (sE != NULL && edges.search(sE) == -1) edges.pushBack(sE);
				if (sN != NULL && nodes.search(sN) == -1) nodes.pushBack(sN);
			}
		}
	}
}

void Lattice::getConnectedElements(node N, List<edge> &edges, List<node> &nodes){
	List<edge> line = lineN[N];
	edges.clear();
	nodes.clear();
	if (line.empty()) return;

	forall_listiterators(edge, it, line){
		node v = (*it)->source();
		edge e;
		forall_adj_edges(e,v){
			edge sE = srcE[e];
			node sN = srcN[e];
			if (sN != N){
				if (sE != NULL && edges.search(sE) == -1) edges.pushBack(sE);
				if (sN != NULL && nodes.search(sN) == -1) nodes.pushBack(sN);
			}
		}
		v = (*it)->target();
		forall_adj_edges(e,v){
			edge sE = srcE[e];
			node sN = srcN[e];
			if (sN != N){
				if (sE != NULL && edges.search(sE) == -1) edges.pushBack(sE);
				if (sN != NULL && nodes.search(sN) == -1) nodes.pushBack(sN);
			}
		}
	}
}