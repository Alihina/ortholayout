#include "stdafx.h"

#include <Lattice.h>


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

edge Lattice::connect(node v1, node v2){
	edge e = newEdge(v1,v2);
	IPoint p1 = nodePos[v1];
	IPoint p2 = nodePos[v2];
	int pos = getHeight(p1, p2);
	height[e] = pos;
	if (p1.m_x == p2.m_x){
		start[e] = min(p1.m_y, p2.m_y);
		end[e] = max(p1.m_y, p2.m_y);
		hor.insertAfter(e ,findST(hor, pos));	
	}
	if (p1.m_y == p2.m_y){
		start[e] = min(p1.m_x, p2.m_x);
		end[e] = max(p1.m_x, p2.m_x);
		vert.insertAfter(e ,findST(vert, pos));
	}
}

node Lattice::getNode(IPoint pos){ 
	node v;
	forall_nodes(v,*this){
		if (nodePos[v] == pos) return v;
	}
	v = newNode();
	nodePos[v] = pos;
	return v;
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
	int d = outline.search(e);
	if ( d != -1){
		ListIterator<edge> ito = outline.get(d);
		outline.insertAfter(e2, ito);
		outline.insertAfter(e1, ito);
		outline.del(ito);
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
}


bool Lattice::onOutline(node v){
	bool val = false;
	edge e;
	forall_adj_edges(e,v){
		if(outline.search(e) !=-1) val = true;
	}
	return val;

}
int Lattice::getRot(edge e1, edge e2){
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
	forall_listiterators(edge,it,outline){
		if ((*it)->source() == v1) itBegin = it;
		if ((*it)->source() == v2){ 
			itEnd = it; //the one past last edge that will be looked at
			break;
		}
	}	
	edge e1;
	edge e2 = *itBegin;
	for(ListConstIterator<edge> ite = itBegin; ite != itEnd; ite = ite.succ()){
		if (!(ite.valid())) ite = outline.begin();
		e1 = e2;
		e2 = *ite;
		int delta = getRot(e1,e2);
		if (delta == 3) delta = -1;
		if (delta == -3) delta = 1;
		if (delta == 2 | delta == -2){			
			ListConstIterator<edge> it0;
			if (ite.pred().valid()) it0 = ite.pred();
			else it0 = outline.rbegin();
			if (it0.pred().valid()) it0 = it0.pred();
			else it0 = outline.rbegin();
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
}
void Lattice::flip(List<edge> &edges){
	forall_listiterators(edge, it, edges) reverseEdge(*it);
	edges.reverse();
}

void Lattice::addToOutline(List<edge> &newOutline){
	ListIterator<edge> itStart, itEnd;
	forall_nonconst_listiterators(edge, it1, outline){
		if ((*it1)->source() == newOutline.front()->source()) {
			itStart = it1;
		}
		if ((*it1)->target() == newOutline.back()->target()){
			itEnd = it1;
		}
	}
	while(newOutline.empty() == false){
		outline.insertBefore(newOutline.popFrontRet(),itStart);
	}
	for(ListIterator<edge> it2 = itStart; it2 != itEnd.succ(); it2 = it2.succ()){
		outline.del(it2); //this should work (?)
	}
	
}

edge Lattice::getprevEdge(node v){
	forall_listiterators(edge,it,outline){
		if ((*it)->target() == v) return (*it);
	}
	return NULL;
}

void Lattice::addLine(IPolyline line){
	bool outside = isOutside(line.front());
	edge prevEdge = NULL;
	IPoint p1;
	IPoint p2 = line.popFrontRet();
	ListIterator<IPoint> it;
	List<edge> newOutline;
	int rot = 0;
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
			ite = ite.succ();
			if (ite.valid()) v2 = splitEdge(*ite,height);
			else v2 = vTrg;
			edge e = connect(v1,v2);
			//if (nodePos[v1] == nodePos[v2]) merge(v1,v2);
			if (outside){
				rot += getRot(prevEdge, e);
				newOutline.pushBack(e);
				prevEdge = e;
			}
			
			if (onOutline(v2)){
				if (outside){					
					if(onOutline(newOutline.front()->source())){
						rot += getRot(v2,newOutline.front()->source());
						if (rot == -4)	flip(newOutline);											
						addToOutline(newOutline); //this empties newOutline
					}else{
						ListIterator<edge> it2;
						List<edge> rnewOutline;						
						forall_rev_listiterators(edge, ite2, newOutline){
							edge e2 = *ite2;
							rnewOutline.pushBack(connect(e2->target(), e2->source()));
						}
						newOutline.concFront(rnewOutline);
						addToOutline(newOutline);
					}
					outside = false;
				}else{
					outside = true;
					prevEdge = getprevEdge(v2);
				}
			}

		}while(ite.valid());
	}
	if (outside){
		ListIterator<edge> it2;
		List<edge> rnewOutline;						
		forall_rev_listiterators(edge, ite2, newOutline){
			edge e2 = *ite2;
			rnewOutline.pushBack(connect(e2->target(), e2->source()));
		}
		newOutline.conc(rnewOutline);
		addToOutline(newOutline);
	}

};

//void Lattice::removeLine(IPolyline line){
//	bool outside = false;
//	edge oldEdge = NULL;
//	edge e = NULL;
//	IPoint p1;
//	IPoint p2 = line.popFrontRet();
//	ListIterator<IPoint> it;
//	List<edge> newOutline;
//	node vStart = NULL;
//	node vEnd = NULL;
//
//	forall_listiterators(IPoint, it, line){
//		p1 = p2;
//		p2 = *it;
//		int height = getHeight(p1,p2);
//		node v1 = getNode(p1);
//		node v2 = getNode(p2);				
//		e = NULL;
//		e = searchEdge(v1,v2);
//		if (e == NULL) e = searchEdge(v2,v1);
//		if (outline.search(e) != -1){
//			if (outside == false){			
//				vStart = v1;
//				outside = true;
//			}
//		}else{
//			if (outside == true){
//				vEnd = v2;
//				outside = false;
//				if (vEnd == e->source()){
//					node temp = vEnd;
//					vEnd = vStart;
//					vStart = temp;
//				}
//				edge e2;
//				node w1,w2;
//				for(;;){
//					adjEntry adj = e2->adjTarget();
//					adj = adj->cyclicPred();
//
//					forall_adj(adj,w2){
//
//					}
//				}
//			}
//		}
//	}
//
//}
