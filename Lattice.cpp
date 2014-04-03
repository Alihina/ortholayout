#include "stdafx.h"

#include <Lattice.h>


Lattice::Lattice(){
	nodePos.init(*this,IPoint(-1,-1));
	height.init(*this,-1);
	start.init(*this,-1);
	end.init(*this,-1);
}

int Lattice::getHeight(IPoint p1, IPoint p2){
	if (p1.m_x == p2.m_x) return p1.m_x;
	if (p1.m_y == p2.m_y) return p1.m_y;
	return 0; //this should never happen
};


ListIterator<edge> Lattice::findST(List<edge> &edgelist, int pos){
	ListIterator<edge> it;
	ListIterator<edge> value = edgelist.begin();
	forall_nonconst_listiterators(edge, it, edgelist){
		if (height[*it] >= pos) return value;
		value = it;
	}
	return it;
}

ListIterator<edge> Lattice::findBT(List<edge> &edgelist, int pos){
	ListIterator<edge> it;
	ListIterator<edge> value = edgelist.begin();
	forall_nonconst_listiterators(edge, it, edgelist){
		value = it;
		if (height[*it] > pos) return value;		
	}
	return it;
}

char Lattice::adjPos(node v, edge e){
	node v1 = e->source();
	node v2 = e->target();
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	char val = -1;
	if (v == v1){
		if (p1.m_x == p2.m_x){ //edge is vertical
			if (p1.m_y > p2.m_y) val = 3;
			else val = 7;
		}else{ //edge is horizontal
			if (p1.m_x > p2.m_x) val = 5;
			else val = 1;
		}
	}
	if (v == v2){
		if (p1.m_x == p2.m_x){ //edge is vertical
			if (p1.m_y > p2.m_y) val = 0;
			else val = 4;
		}else{ //edge is horizontal
			if (p1.m_x > p2.m_x) val = 2;
			else val = 6;
		}
	}
	return val;
}

void Lattice::adjSort(node v, adjEntry newAdj){
	List<adjEntry> adjList;
	this->adjEntries(v,adjList);
	if (adjList.size() == 1) return;	
	adjEntry pred = adjList.popBackRet(); //remove the newly added AdjEntry
	char newPos = adjPos(v,newAdj->theEdge());
	char predPos = 8;
	while(adjList.empty() == false){
		pred = adjList.popBackRet();
		predPos = adjPos(v,pred->theEdge());
		if (predPos < newPos){
			this->moveAdjAfter(newAdj,pred);
			return;
		}
	}
	this->moveAdjBefore(newAdj,pred);
}

edge Lattice::connect(node v1, node v2){
	edge e = this->searchEdge(v1,v2);
	if (e == NULL) e = this->searchEdge(v2,v1);
	if (e == NULL) e = newEdge(v1,v2);
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	if (v1 == e->source()){
		adjSort(v1,e->adjSource());
		adjSort(v2,e->adjTarget());	
	}else{
		adjSort(v2,e->adjSource());
		adjSort(v1,e->adjTarget());	
	}
	return e;
}

node Lattice::getNode(IPoint pos){ 
	node v;
	forall_nodes(v,*this){
		if (nodePos[v] == pos){
			return v;
			std::cout << "NODE EXISTS" << std::endl;
		}
	}
	node w = newNode();
	nodePos[w] = pos;
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
List<edge> Lattice::edges(IPoint p1, IPoint p2){
	int pos = getHeight(p1,p2);
	List<edge> crossings;
	if (p1.m_x == p2.m_x){
		if(p1.m_y > p2.m_y){
			int a = p2.m_y;
			int b = p1.m_y;
			forall_listiterators(edge, it, hor){
				if (height[*it] > a){
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						crossings.pushFront(*it);
					}
				}
			}
		}else{
			int a = p1.m_y;
			int b = p2.m_y;
			forall_listiterators(edge, it, hor){
				if (height[*it] > a){
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						crossings.pushBack(*it);
					}
				}
			}
		}
	}
	if (p1.m_y == p2.m_y){
		if(p1.m_x > p2.m_x){
			int a = p2.m_x;
			int b = p1.m_x;
			forall_listiterators(edge, it, vert){
				if (height[*it] > a){
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						crossings.pushFront(*it);
					}
				}
			}
		}else{
			int a = p1.m_x;
			int b = p2.m_x;
			forall_listiterators(edge, it, vert){
				if (height[*it] > a){
					if (height[*it] > b) break;
					if (start[*it] <= pos && end[*it] >= pos){
						crossings.pushBack(*it);
					}
				}
			}
		}
	}
	return crossings;
}

node Lattice::splitEdge(edge e, int pos){
	node v1 = e->source();
	node v2 = e->target();
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	node newV = newNode();
	edge e1 = newEdge(v1,newV);
	edge e2 = newEdge(newV,v2);
	int a = getHeight(p1,p2);
	height[e1] = a;
	height[e2] = a;
	int d = m_outline.search(e);
	if ( d != -1){
		ListIterator<edge> ito = m_outline.get(d);
		m_outline.insertAfter(e2, ito);
		m_outline.insertAfter(e1, ito);
		m_outline.del(ito);
	}
	if (p1.m_y == p2.m_y){
		ListIterator<edge> ite = vert.get(vert.search(e));
		nodePos[newV] = IPoint(a,pos);
		vert.insertAfter(e1, ite);
		vert.insertAfter(e2, ite);
		vert.del(ite);
	}
	if (p1.m_x == p2.m_x){
		ListIterator<edge> ite = vert.get(vert.search(e));
		nodePos[newV] = IPoint(pos,a);
		hor.insertAfter(e1, ite);
		hor.insertAfter(e2, ite);
		hor.del(ite);
	}
	delEdge(e);
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
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;	
		forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		int height = getHeight(p1,p2);
		node vSrc = getNode(p1);
		node vTrg = getNode(p2);		
		List<edge> crossings = edges(p1, p2);
		ListIterator<edge> ite = crossings.rend(); // I hope this does what I think it does
		node v1 = vSrc;
		node v2;
		do{
			if (ite.valid()) ite = ite.succ();
			if (ite.valid()) v2 = splitEdge(*ite,height);
			else v2 = vTrg;
			std::cout << " connect:" << nodePos[v1] << "->" << nodePos[v2] << "#nodes: " << this->numberOfNodes(); 
			edge e = connect(v1,v2);
		}while(ite.valid());
		std::cout << "  done"  << std::endl;
	}
	m_pOutline = CalcOutline();
};

edge Lattice::getRight(edge e){
	/*OGDF_ASSERT(e_in->target() == w);*/
	node w = e->target();
	/*adj,Entry adj;*/
	adjEntry inc = e->adjTarget();
	/*forall_adj(adj,w){
		if (adj->theEdge() == e_in) inc = adj;
	}*/
	adjEntry out = inc->cyclicPred();
	edge e_out = out->theEdge();
	if (e_out->source() == w) return e_out;
	else this->reverseEdge(e_out);
	return e_out;
}
void Lattice::removeLine(IPolyline line){
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

	forall_listiterators(IPoint, it, line){
		p1 = p2;
		p2 = *it;
		int height = getHeight(p1,p2);
		v1 = getNode(p1);
		v2 = getNode(p2);				
		e = NULL;
		e = searchEdge(v1,v2);
		if (e != NULL){
			delEdge(e);
		}
		e = searchEdge(v2,v1);
		if (e != NULL){
			delEdge(e);
		}//if (e != NULL)
		 if (v1->indeg() + v1->outdeg() == 0) delNode(v1);
		 if (v2->indeg() + v2->outdeg() == 0) delNode(v2);
	}
	m_pOutline = CalcOutline();
	std::cout << "line removed" << std::endl;
}
IPolyline Lattice::outline(){
	return m_pOutline;
}

IPolyline Lattice::CalcOutline(){
	if (this->empty()) return IPolyline();
	IPolyline ret;
	node v,w;
	w = this->chooseNode();
	forall_nodes(v,*this){
		if (nodePos[v].m_y <= nodePos[w].m_y){
			if (nodePos[v].m_y < nodePos[w].m_y) w = v;
			else if (nodePos[v].m_x < nodePos[w].m_x){
				w = v;
			}			
		}
	}
	//std::cout << "n:" << nodePos[w] << "  ";
	IPoint tempPos = nodePos[w];
	tempPos.m_y -= 1;
	node tempNode = getNode(tempPos);
	edge tempEdge = connect(tempNode, w);
	edge e = getRight(tempEdge);
	ret.pushBack(nodePos[e->source()]);
	ret.pushBack(nodePos[e->target()]);
	std::cout << "TERMINUS: " << nodePos[w] << std::endl;
	bool loop = false;
	while(e->target() != w){
		loop = true;
		std::cout << "[" << nodePos[e->source()] << " -> " << nodePos[e->target()] << "] ";
		e = getRight(e);
		ret.pushBack(nodePos[e->target()]);
	}
	if (loop) std::cout << std::endl;
	delNode(tempNode);

	return ret;
}