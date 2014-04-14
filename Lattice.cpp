#include "stdafx.h"

#include <Lattice.h>


Lattice::Lattice(){
	//std::cout << "CREATED LATTICE!" << std::endl;
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
}

Lattice::Lattice(const Lattice &L){
	//std::cout << "COPIED LATTICE WITH " << L.numberOfNodes() << " NODES!" << std::endl;
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
	
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
				if (height[*it] > a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] < pos && end[*it] > pos){
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
				if (height[*it] > a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] < pos && end[*it] > pos){
						
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
				if (height[*it] > a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] < pos && end[*it] > pos){
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
				if (height[*it] > a){
					//std::cout << "!" << *it << " h: " << height[*it] << ": [" << start[*it] << ":" << end[*it] << "]" << std::endl;
					if (height[*it] > b) break;
					if (start[*it] < pos && end[*it] > pos){
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
	//std::cout << "  splitting edge " << e->index() << ": " << p1 << "->" << p2 << std::endl;
	
	
	node newV = newNode();	
	int a = getHeight(p1,p2);
	if (p1.m_y == p2.m_y) nodePos[newV] = IPoint(pos, a);
	if (p1.m_x == p2.m_x) nodePos[newV] = IPoint(a,pos);

	//std::cout << "created Node " << newV << std::endl;
	edge e1 = connect(v1,newV);	
	sortIn(e1);
	//std::cout << "created edge " << e1->index() << std::endl;
	edge e2 = connect(newV,v2);
	sortIn(e2);
	//std::cout << "created edge " << e2->index() << std::endl;
	edge e3 = connect(v2,newV);
	//std::cout << "created edge " << e3->index() << std::endl;
	edge e4 = connect(newV,v1);
	//std::cout << "created edge " << e4->index() << std::endl;
	
	//height[e1] = a;
	//height[e2] = a;	
	//height[e3] = a;
	//height[e4] = a;

	if (p1.m_y == p2.m_y){ //edge is horizontal
		//std::cout << "splitting horizontal edge" << std::endl;
		ListIterator<edge> ite = hor.get(hor.search(e));
		nodePos[newV] = IPoint(pos, a);
		//if (ite.valid()) //std::cout << "valid" << std::endl;
		//else //std::cout << "invalid" << std::endl;
		OGDF_ASSERT(ite.valid());
		/*hor.insertAfter(e1, ite);
		hor.insertAfter(e2, ite);*/
		hor.del(ite);		
	}
	if (p1.m_x == p2.m_x){ //edges are vertical
		//std::cout << "splitting vertical edge" << std::endl;
		ListIterator<edge> ite = vert.get(vert.search(e));
		nodePos[newV] = IPoint(a,pos);
		//if (ite.valid()) //std::cout << "valid" << std::endl;
		//else //std::cout << "invalid" << std::endl;
		OGDF_ASSERT(ite.valid());
		/*vert.insertAfter(e1, ite);
		vert.insertAfter(e2, ite);*/
		vert.del(ite);
	}
	//std::cout << "crossNode#" << newV << ": " << nodePos[newV] << std::endl;
	//std::cout << "deleting edge " << e << std::endl;
	delEdge(e);
	edge e_alt = this->searchEdge(v2,v1);
	//std::cout << "deleting edge " << e_alt << std::endl;
	delEdge(e_alt);
	//adjSort(e1->adjSource());
	//adjSort(e2->adjSource());
	//adjSort(e3->adjSource());
	//adjSort(e4->adjSource());
	//adjSort(e1->adjTarget());
	//adjSort(e2->adjTarget());
	//adjSort(e3->adjTarget());
	//adjSort(e4->adjTarget());
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

void Lattice::addLine(IPolyline line) {
	if (line.empty()) return;
	/*std::cout << "adding line : ";
	forall_listiterators(IPoint, it, line) std::cout << *it << "->";
	std::cout << std::endl;*/
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;	
		forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		//std::cout << "p1 " << p1 << " p2 " << p2 << std::endl;
		int height = getHeight(p1,p2);
		//std::cout << "height " << height << std::endl;
		node vSrc = getNode(p1);
		//std::cout << "vSrc :" << vSrc << std::endl;
		node vTrg = getNode(p2);		
		List<edge> crossings = edges(p1, p2);
		bool debug = false;
		debug = (!crossings.empty());
		//ListIterator<edge> ite = crossings.begin(); // I hope this does what I think it does ADDENDUM: IT DOESN'T!!
		//if (debug) std::cout << ite.valid() << " ()() " << ite.succ().valid() << std::endl;
		node v1 = vSrc;
		node v2;
		forall_listiterators(edge,ite,crossings){	
			v2 = splitEdge(*ite,height);			
			edge e = connect(v1,v2);
			sortIn(e);
			connect(v2,v1);
			v1 = v2;
		}
		v2 = vTrg;
		edge e = connect(v1,v2);
		sortIn(e);
		connect(v2,v1);

	}
	//std::cout << "recalc ...";
	m_pOutline = CalcOutline();
	//std::cout << "done" << std::endl;
};

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
		if (loopcount > 10) throw -1;
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
						delFromList.pushFront(it);
						node v1 = (*it)->source();
						node v2 = (*it)->target();
						//std::cout << "  deleting [" << nodePos[v1] << "->" << nodePos[v2] << "]" << std::endl;
						delEdge(*it);
						edge e = searchEdge(v1,v2);
						//std::cout << "  deleting [" << nodePos[e->source()] << "->" << nodePos[e->target()] << "]" << std::endl;
						delEdge(e);
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
		//	std::cout << "hor: " << std::endl;
			forall_nonconst_listiterators(edge, it, hor){
			//	std::cout << "remove :" << (*it)->index() << " h: " << height[*it] << ": [" << nodePos[(*it)->source()] << "->" << nodePos[(*it)->target()] << "]" << std::endl;
				if (height[(*it)] == p1.m_y){
					if (start[*it] >= a && end[*it] <= b){
						delFromList.pushFront(it);
						node v1 = (*it)->source();
						node v2 = (*it)->target();
						//std::cout << "  deleting [" << nodePos[v1] << "->" << nodePos[v2] << "]" << std::endl;
						delEdge(*it);
						edge e = searchEdge(v1,v2);
						//std::cout << "  deleting [" << nodePos[e->source()] << "->" << nodePos[e->target()] << "]" << std::endl;
						delEdge(e);
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
	m_pOutline = CalcOutline();
	//std::cout << "line removed" << std::endl;
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
	edge e = getRight(tempEdge);
	edge firstEdge = e;	
	ret.pushBack(nodePos[e->source()]);
	ret.pushBack(nodePos[e->target()]);
	e = getRight(e);	
	while(e != firstEdge){
		ret.pushBack(nodePos[e->target()]);
		e = getRight(e);
	}

	delNode(tempNode);

	return ret;
}