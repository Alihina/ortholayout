#include "stdafx.h"

#include <Grid.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>
#include <ogdf/basic/Queue.h>
#include "IPointComparer.h"  //einkommentieren, ich hab die datei grad nicht da.

using namespace ogdf;

//[TODO] also write registerOutline and isFree (outline), copy anettes part for that.


Gridpoint::Gridpoint() {
	v_node = NULL; //the node for the vertical grid
	h_node = NULL; //the node for the horizontal grid
	changelayer = NULL; //the edge connecting v_node and h_node

	//pretty sure none of them need to be pointers, as they already are.
	occNode = NULL; //reference to the node that lies on this gridpoint
	v_edge = NULL; //reference to the edge that goes through this gridpoint vertically
	h_edge = NULL; //reference to the edge that goes through this gridpoint horizonatlly	
}

Grid::Grid() {
	m_Grid = Array2D<Gridpoint> (1,1,1,1);
	m_Grid(1,1).v_node = newNode();
	m_Grid(1,1).h_node = newNode();
	m_Grid(1,1).changelayer = newEdge(m_Grid(1,1).v_node,m_Grid(1,1).h_node);

}

Grid::Grid(int a, int b, int c, int d) {
	std::cout << a << " " << b << " " << c << " " << d << std::endl;
	m_Grid = Array2D<Gridpoint> (a,b,c,d);
	NodeArray<int> x_coord(*this);
	NodeArray<int> y_coord(*this);
	//make all nodes and changelayer edges
	for (int i = a; i <= b; ++i) {
		for (int j = c; j <= d; ++j) {
			m_Grid(i,j).v_node = newNode();
			m_Grid(i,1).h_node = newNode();
			m_Grid(i,j).changelayer = newEdge(m_Grid(i,j).v_node,m_Grid(i,j).h_node);
			x_coord[m_Grid(i,j).h_node] = i;
			y_coord[m_Grid(i,j).h_node] = j;
			x_coord[m_Grid(i,j).v_node] = i;
			y_coord[m_Grid(i,j).v_node] = j;
		}
	}
	//make horizontal edges
	for (int i = a; i < b; ++i) {
		for (int j = c; j <= d; ++j) {
			newEdge(m_Grid(i,j).h_node,m_Grid(i+1,j).h_node);
			//m_Grid(i,j).Left = &m_Grid(i+1,j); //not using these
			//m_Grid(i+1,j).Right = &m_Grid(i,j);
		}
	}
	//make vertical edges.
	for (int i = a; i <= b; ++i) {
		for (int j = c; j < d; ++j) {
			newEdge(m_Grid(i,j).v_node,m_Grid(i,j+1).v_node);
			//m_Grid(i,j).Down = &m_Grid(i,j+1);
			//m_Grid(i,j+1).Up = &m_Grid(i,j);
		}
	}
}

bool Grid::findEdge(IPoint A, IPoint B, IPolyline &line) { // The IPolyline contains the first and last points.
	line.clear();
	Queue<node> bfsqueue;
	NodeArray<bool> visited(*this,false);
	NodeArray<node> parent(*this,NULL);
	visited[m_Grid(A.m_x,A.m_y).v_node] = true;
	visited[m_Grid(A.m_x,A.m_y).h_node] = true;
	bool found = false;
	IPoint c; //the cursor that tracks where we are while finding back
	node n,m;
	n = NULL;
	edge e;
	while (!bfsqueue.empty()) { //bfsloop
		n = bfsqueue.pop();
		if (n==m_Grid(B.m_x,B.m_y).h_node){
			found = true;
			break; //we're done!
		}
		if (n==m_Grid(B.m_x,B.m_y).v_node){
			found = true;
			break; //we're done!
		}
		forall_adj_edges(e,n) {
			m = e->opposite(n);
			if (visited[m] == false) {
				parent[m] = n;
				bfsqueue.append(m);
				visited[m]=true;
			}
		}
	}
	if (!found){
		//we're in deep shit, no way possible.
		return false; 
	}

	//find the way back now!
	
	while (n!=NULL){
		c = IPoint(x_coord[n],y_coord[n]);
		if (c != line.back()) {
			line.pushBack(c);
		}
		n=parent[n];
	}
	
	//normalize the line, so we don't have a point everywhen.
	ListIterator<IPoint> iter, next, onext;
	for (iter = line.begin(); iter.valid(); ++iter) {
		for( ; ; ) {
			next  = iter; next++;
			if (!next.valid()) break;
			onext = next, onext++;
			if (!onext.valid()) break;

			int d1=0, d2=0;

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
				line.del(next);
			else
				break; /* while */
		}
	}

	line.reverse(); //line is now pointing in the correct direction
	return true;
}

bool Grid::findEdge(IPoint A, IPolyline outline, IPolyline &line) {
	line.clear();
	//check what the outline is.
	List<IPoint> outpoints;
	ListIterator<IPoint> it = outline.begin();
	outpoints.pushBack((*it));
	IPoint last = *it;
	++it;
	OGDF_ASSERT(last != *it);
	while (it.valid()) {
		outpoints.pushBack((*it));
		if (last.m_x == (*it).m_x) { //same x, so vertical line.
			int i, j;
			if (last.m_y < (*it).m_y) {
				i = last.m_y + 1;
				j = (*it).m_y;
			}
			else {
				i = (*it).m_y + 1;
				j = last.m_y;
			}
			for (; i < j;++i) {
				outpoints.pushBack(IPoint(last.m_x,i));
			}
		}
		else if (last.m_y == (*it).m_y) { //same y, so horizontal line.
			int i, j;
			if (last.m_x < (*it).m_x) {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			else {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			for (; i < j; ++i) {
				outpoints.pushBack(IPoint(i,last.m_y));
			}
		}
		else {
			OGDF_ASSERT(1==0);
		}
		IPoint last = *it;
		++it;
	}
	//we now have the outpoints
	//check if we're on the outline, if so return true
	for (it = outpoints.begin();it.valid();++it) {
		if (A == *it) { return true; } //line is still empty, as it should.
	}
	Queue<node> bfsqueue;
	NodeArray<bool> visited(*this,false);
	NodeArray<node> parent(*this,NULL);
	visited[m_Grid(A.m_x,A.m_y).v_node] = true;
	visited[m_Grid(A.m_x,A.m_y).h_node] = true;
	bool found = false;
	IPoint c; //the cursor that tracks where we are while finding back
	node n,m;
	n = NULL;
	edge e;
	while (!bfsqueue.empty()) { //bfsloop
		n = bfsqueue.pop();

		for (it = outpoints.begin();it.valid();++it){
			if (n==m_Grid((*it).m_x,(*it).m_y).h_node){
				found = true;
				break; //we're done!
			}
			if (n==m_Grid((*it).m_x,(*it).m_y).v_node){
				found = true;
				break; //we're done!
			}
		}
		
		forall_adj_edges(e,n) {
			m = e->opposite(n);
			if (visited[m] == false) {
				parent[m] = n;
				bfsqueue.append(m);
				visited[m]=true;
			}
		}
	}
	if (!found){
		//we're in deep shit, no way possible.
		return false; 
	}

	//find the way back now!
	
	while (n!=NULL){
		c = IPoint(x_coord[n],y_coord[n]);
		if (c != line.back()) {
			line.pushBack(c);
		}
		n=parent[n];
	}
	
	//normalize the line, so we don't have a point everywhen.
	ListIterator<IPoint> iter, next, onext;
	for (iter = line.begin(); iter.valid(); ++iter) {
		for( ; ; ) {
			next  = iter; next++;
			if (!next.valid()) break;
			onext = next, onext++;
			if (!onext.valid()) break;

			int d1=0, d2=0;

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
				line.del(next);
			else
				break; /* while */
		}
	}

	line.reverse(); //line is now pointing in the correct direction
	return true;

}

void Grid::registerPoint(IPoint A) {
	delNode(m_Grid(A.m_x,A.m_y).v_node);
	delNode(m_Grid(A.m_x,A.m_y).h_node);
	//delEdge(m_Grid(A.m_x,A.m_y).changelayer) //prolly unneccecary.
	m_Grid(A.m_x,A.m_y).v_node = NULL;
	m_Grid(A.m_x,A.m_y).h_node = NULL;
	m_Grid(A.m_x,A.m_y).changelayer = NULL;
}

void Grid::registerLine(IPolyline E) {
	if (E.empty()) return;
	ListIterator<IPoint> it = E.begin();
	registerPoint(*it);
	IPoint last = *it;
	++it;
	OGDF_ASSERT(last != *it); //why here
	while (it.valid()) {
		registerPoint(*it);
		if (last.m_x == (*it).m_x) { //same x, so vertical line.
			int i, j;
			if (last.m_y < (*it).m_y) {
				i = last.m_y + 1;
				j = (*it).m_y;
			}
			else {
				i = (*it).m_y + 1;
				j = last.m_y;
			}
			for (; i < j;++i) {
				if (m_Grid(last.m_x,i).v_node != NULL ){
					delNode(m_Grid(last.m_x,i).v_node);
					m_Grid(last.m_x,i).v_node = NULL;
				}
			}
		}
		else if (last.m_y == (*it).m_y) { //same y, so horizontal line.
			int i, j;
			if (last.m_x < (*it).m_x) {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			else {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			for (; i < j; ++i) {
				if (m_Grid(i,last.m_y).h_node != NULL) {
					delNode(m_Grid(i,last.m_y).h_node);
					m_Grid(i,last.m_y).h_node = NULL;
				}
			}
		}
		else {
			OGDF_ASSERT(1==0);
		}
		IPoint last = *it;
		++it;
	}
}

void Grid::registerFill(IPolyline E) {
	if (E.empty()) return;
		ListIterator<IPoint> iter, next, onext, backiter;
	//if the pattern ABA emerges, kill B and deal with all points after A. then backtrack A
	for (iter = E.begin(); iter.valid(); ++iter) {
		for( ; ; ) {
			next  = iter; next++;
			if (!next.valid()) break;
			onext = next, onext++;
			if (!onext.valid()) break;

			if ((*iter) == (*onext)){
				// HANDLE THE POINTS 
				//careful with fenceposts when counting
				IPolyline handle;
				handle.pushBack(*iter);
				handle.pushBack(*next);
				registerLine(handle);


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
	} // antennae killed: outer antennae not belonging to the beginning or end.
	
	while (true) {
		iter = E.begin(); backiter = E.cyclicPred(E.begin());
		iter++;backiter--;
		if ((*iter)==(*backiter)) {
			// handle the points!
			IPolyline handle;
			handle.pushBack(E.front());
			handle.pushBack(*iter);
			registerLine(handle);
			E.del(E.begin());
			E.del(E.cyclicPred(E.begin()));
			continue;
		}
		break;
	}

	//TODO: maybe just fucking make sure we got no consecutive duplicates in outline?? because that would fuck shit up

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
				registerLine(handle);

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

		//	check that the endpoint is not an inner point.

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
	//now all antennae are dead, commence handling the bulk.

	StdComparer<IPoint> comp;
	E.quicksort(comp);
	ogdf::List<int> currentList;
	ogdf::List<int> previousList;
	
	int current_y=E.front().m_y;
	int previous_y=current_y-1;
	iter = E.begin();

	while (iter.valid()) {
		current_y = (*iter).m_y;
		previousList = currentList;
		//currentList.clear();
		while ((*iter).m_y==current_y) {
			bool done = false;
			for (ListIterator<int> it = currentList.begin(); it.valid();++it) {
				if (*it == (*iter).m_x) {
					currentList.del(it);
					done = true;
					break;
				}
				if (*it > (*iter).m_x) {
					currentList.insertBefore((*iter).m_x,it);
					done = true;
					break;
				}
			}
			if (!done) {
				currentList.pushBack((*iter).m_x);
			}

			++iter;
			if (!iter.valid()) {
				break;
			}
		}
		List <int> u = unionLists(previousList, currentList);
		ListIterator<int> it = u.begin();

		while (it.valid()) {
			int from = *it; ++it; int to = *it; ++it;
			for (;from <=to;++from) {
				registerPoint(IPoint(from,current_y));
			}
		}
		
		OGDF_ASSERT(previousList.size()%2==0);
		for(int y = previous_y+1;y<current_y;++y) {
			it = previousList.begin();
			while (it.valid()){
				int from = *it; ++it; int to = *it; ++it;
				for (;from <=to;++from) {
					registerPoint(IPoint(from,y));
				}
			}
		}
		previous_y = current_y;
	}

}

void Grid::restorePoint(IPoint A) {
	if (m_Grid(A.m_x,A.m_y).v_node == NULL) {
		m_Grid(A.m_x,A.m_y).v_node = newNode();
	}
	if (m_Grid(A.m_x,A.m_y).h_node == NULL) {
		m_Grid(A.m_x,A.m_y).h_node = newNode();
	}
	x_coord[m_Grid(A.m_x,A.m_y).h_node] = A.m_x;
	y_coord[m_Grid(A.m_x,A.m_y).h_node] = A.m_y;
	x_coord[m_Grid(A.m_x,A.m_y).v_node] = A.m_x;
	y_coord[m_Grid(A.m_x,A.m_y).v_node] = A.m_y;

	if (m_Grid(A.m_x,A.m_y).changelayer == NULL && 
		(searchEdge(m_Grid(A.m_x,A.m_y).h_node,m_Grid(A.m_x,A.m_y).v_node) ||
		 searchEdge(m_Grid(A.m_x,A.m_y).v_node,m_Grid(A.m_x,A.m_y).h_node))) {
		m_Grid(A.m_x,A.m_y).changelayer = newEdge(m_Grid(A.m_x,A.m_y).h_node, m_Grid(A.m_x,A.m_y).v_node);
	}

	if (A.m_x < m_Grid.high1() && m_Grid(A.m_x + 1,A.m_y).h_node != NULL &&
		(searchEdge(m_Grid(A.m_x,A.m_y).h_node,m_Grid(A.m_x + 1,A.m_y).h_node) ==0 && 
		 searchEdge(m_Grid(A.m_x + 1,A.m_y).h_node,m_Grid(A.m_x,A.m_y).h_node) ==0 )) {
		newEdge(m_Grid(A.m_x,A.m_y).h_node,m_Grid(A.m_x + 1,A.m_y).h_node);
	}
	if (A.m_x > m_Grid.low1() && m_Grid(A.m_x - 1,A.m_y).h_node != NULL &&
		(searchEdge(m_Grid(A.m_x,A.m_y).h_node,m_Grid(A.m_x - 1,A.m_y).h_node) ==0 && 
		 searchEdge(m_Grid(A.m_x - 1,A.m_y).h_node,m_Grid(A.m_x,A.m_y).h_node) ==0 )) {
		newEdge(m_Grid(A.m_x,A.m_y).h_node,m_Grid(A.m_x - 1,A.m_y).h_node);
	}
	if (A.m_y < m_Grid.high2() && m_Grid(A.m_x,A.m_y + 1).v_node != NULL && 
		(searchEdge(m_Grid(A.m_x,A.m_y).v_node, m_Grid(A.m_x,A.m_y + 1).v_node) ==0 && 
		 searchEdge(m_Grid(A.m_x,A.m_y + 1).v_node, m_Grid(A.m_x,A.m_y).v_node) ==0 )) {
		newEdge(m_Grid(A.m_x,A.m_y).v_node, m_Grid(A.m_x,A.m_y + 1).v_node);
	}
	if (A.m_y > m_Grid.low2() && m_Grid(A.m_x,A.m_y - 1).v_node != NULL && 
		(searchEdge(m_Grid(A.m_x,A.m_y).v_node, m_Grid(A.m_x,A.m_y - 1).v_node) ==0 && 
		 searchEdge(m_Grid(A.m_x,A.m_y - 1).v_node, m_Grid(A.m_x,A.m_y).v_node) ==0 )) {
		newEdge(m_Grid(A.m_x,A.m_y).v_node, m_Grid(A.m_x,A.m_y - 1).v_node);
	}
} 

void Grid::restoreLine(IPolyline E, bool ends) {
	if (E.empty()) return;
	ListIterator<IPoint> it = E.begin();
	if (ends) {restorePoint(*it);}
	IPoint last = *it;
	++it;
	OGDF_ASSERT(last != *it);
	while (it.valid()) {
		if (it.succ().valid() || ends) {
			restorePoint(*it);
		}
		
		if (last.m_x == (*it).m_x) { //same x, so vertical line.
			int i, j;
			if (last.m_y < (*it).m_y) {
				i = last.m_y + 1;
				j = (*it).m_y;
			}
			else {
				i = (*it).m_y + 1;
				j = last.m_y;
			}
			for (; i < j;++i) {
				restorePoint(IPoint(last.m_x,i));
			}
		}
		else if (last.m_y == (*it).m_y) { //same y, so horizontal line.
			int i, j;
			if (last.m_x < (*it).m_x) {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			else {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			for (; i < j; ++i) {
				restorePoint(IPoint(i,last.m_y));
			}
		}
		else {
			OGDF_ASSERT(1==0);
		}
		IPoint last = *it;
		++it;
	}
}

List<int> Grid::unionLists(ogdf::List<int> prev, ogdf::List<int> curr) {
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
			if (*p <= *c) {
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

void Grid::restoreFill(IPolyline E) {
	if (E.empty()) return;
	//TODO ROBUSTNESS: check everywhere that we have an outline that is not a point
	// wtf happnes with a star for example?

	//we have the outline. the first and last points are the same. 
	//
	//	the algorithm goes as follows: kill all the antennae and operate on them.
	//		that's the outer and the inner antennae, 
	//	then comes the other part.
	ListIterator<IPoint> iter, next, onext, backiter;
	//if the pattern ABA emerges, kill B and deal with all points after A. then backtrack A
	for (iter = E.begin(); iter.valid(); ++iter) {
		for( ; ; ) {
			next  = iter; next++;
			if (!next.valid()) break;
			onext = next, onext++;
			if (!onext.valid()) break;

			if ((*iter) == (*onext)){
				// HANDLE THE POINTS 
				//careful with fenceposts when counting
				IPolyline handle;
				handle.pushBack(*iter);
				handle.pushBack(*next);
				restoreLine(handle);


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
	} // antennae killed: outer antennae not belonging to the beginning or end.
	
	while (true) {
		iter = E.begin(); backiter = E.cyclicPred(E.begin());
		iter++;backiter--;
		if ((*iter)==(*backiter)) {
			// handle the points!
			IPolyline handle;
			handle.pushBack(E.front());
			handle.pushBack(*iter);
			restoreLine(handle);
			E.del(E.begin());
			E.del(E.cyclicPred(E.begin()));
			continue;
		}
		break;
	}

	//TODO: maybe just fucking make sure we got no consecutive duplicates in outline?? because that would fuck shit up

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
				restoreLine(handle);

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

		//	check that the endpoint is not an inner point.

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
	//now all antennae are dead, commence handling the bulk.

	StdComparer<IPoint> comp;
	E.quicksort(comp);
	ogdf::List<int> currentList;
	ogdf::List<int> previousList;
	
	int current_y=E.front().m_y;
	int previous_y=current_y-1;
	iter = E.begin();

	while (iter.valid()) {
		current_y = (*iter).m_y;
		previousList = currentList;
		// currentList.clear(); nope.
		while ((*iter).m_y==current_y) {
			bool done = false;
			for (ListIterator<int> it = currentList.begin();it.valid();++it){
				if (*it == (*iter).m_x  ) {
					currentList.del(it);
					done = true;
					break;
				}
				if (*it > (*iter).m_x) {
					currentList.insertBefore((*iter).m_x,it);
					done = true;
					break;
				}
			}
			if (!done) {
				currentList.pushBack((*iter).m_x);
			}

			++iter;
			if (!iter.valid()) {
				break;
			}
		}
		List <int> u = unionLists(previousList, currentList);
		ListIterator<int> it = u.begin();

		while (it.valid()) {
			int from = *it; ++it; int to = *it; ++it;
			for (;from <=to;++from) {
				restorePoint(IPoint(from,current_y));
			}
		}
		
		OGDF_ASSERT(previousList.size()%2==0);
		for(int y = previous_y+1;y<current_y;++y) {
			it = previousList.begin();
			while (it.valid()){
				int from = *it; ++it; int to = *it; ++it;
				for (;from <=to;++from) {
					restorePoint(IPoint(from,y));
				}
			}
		}
		previous_y = current_y;
	}
}

bool Grid::isFree(IPoint A) { //returns true iff both v_node and h_node are "free", i.e. present
	if (m_Grid(A.m_x,A.m_y).h_edge != NULL && m_Grid(A.m_x,A.m_y).v_edge != NULL) {
		return true;
	}
	else {
		return false;
	}

}

bool Grid::isFreeLine(IPolyline E) {
	if (E.empty()) return true;
	ListIterator<IPoint> it = E.begin();
	if (!isFree(*it)) {return false;} 
	IPoint last = *it;
	++it;
	OGDF_ASSERT(last != *it);
	while (it.valid()) {
		if (!isFree(*it)) {return false;}
		if (last.m_x == (*it).m_x) { //same x, so vertical line.
			int i, j;
			if (last.m_y < (*it).m_y) {
				i = last.m_y + 1;
				j = (*it).m_y;
			}
			else {
				i = (*it).m_y + 1;
				j = last.m_y;
			}
			for (; i < j;++i) {
				if (!isFree(IPoint(last.m_x,i))) {return false;} 
			}
		}
		else if (last.m_y == (*it).m_y) { //same y, so horizontal line.
			int i, j;
			if (last.m_x < (*it).m_x) {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			else {
				i = last.m_x + 1;
				j = (*it).m_x;
			}
			for (; i < j; ++i) {
				if (!isFree(IPoint(i,last.m_y))) {return false;}
			}
		}
		else {
			OGDF_ASSERT(1==0);
		}
		IPoint last = *it;
		++it;
	}
	return true;
}

bool Grid::isFree(IPolyline E) { //returns true if every point in the area is free. 
	if (E.empty()) return true;	
	ListIterator<IPoint> iter, next, onext, backiter;
	//if the pattern ABA emerges, kill B and deal with all points after A. then backtrack A
	for (iter = E.begin(); iter.valid(); ++iter) {
		for( ; ; ) {
			next  = iter; next++;
			if (!next.valid()) break;
			onext = next, onext++;
			if (!onext.valid()) break;

			if ((*iter) == (*onext)){
				// HANDLE THE POINTS 
				//careful with fenceposts when counting
				IPolyline handle;
				handle.pushBack(*iter);
				handle.pushBack(*next);
				if (!isFreeLine(handle)) {return false;}


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
	} // antennae killed: outer antennae not belonging to the beginning or end.
	
	while (true) {
		iter = E.begin(); backiter = E.cyclicPred(E.begin());
		iter++;backiter--;
		if ((*iter)==(*backiter)) {
			// handle the points!
			IPolyline handle;
			handle.pushBack(E.front());
			handle.pushBack(*iter);
			if (!isFreeLine(handle)) {return false;}
			E.del(E.begin());
			E.del(E.cyclicPred(E.begin()));
			continue;
		}
		break;
	}

	//TODO: maybe just fucking make sure we got no consecutive duplicates in outline?? because that would fuck shit up

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
				if (!isFreeLine(handle)) {return false;}

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

		//	check that the endpoint is not an inner point.

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
	//now all antennae are dead, commence handling the bulk.

	StdComparer<IPoint> comp;
	E.quicksort(comp);
	ogdf::List<int> currentList;
	ogdf::List<int> previousList;
	
	int current_y=E.front().m_y;
	int previous_y=current_y-1;
	iter = E.begin();

	while (iter.valid()) {
		current_y = (*iter).m_y;
		previousList = currentList;
		// currentList.clear(); nope.
		while ((*iter).m_y==current_y) {
			bool done = false;
			for (ListIterator<int> it = currentList.begin();it.valid();++it){
				if (*it == (*iter).m_x  ) {
					currentList.del(it);
					done = true;
					break;
				}
				if (*it > (*iter).m_x) {
					currentList.insertBefore((*iter).m_x,it);
					done = true;
					break;
				}
			}
			if (!done) {
				currentList.pushBack((*iter).m_x);
			}

			++iter;
			if (!iter.valid()) {
				break;
			}
		}
		List <int> u = unionLists(previousList, currentList);
		ListIterator<int> it = u.begin();

		while (it.valid()) {
			int from = *it; ++it; int to = *it; ++it;
			for (;from <=to;++from) {
				if (!isFree(IPoint(from,current_y))) {return false;}
			}
		}
		
		OGDF_ASSERT(previousList.size()%2==0);
		for(int y = previous_y+1;y<current_y;++y) {
			it = previousList.begin();
			while (it.valid()){
				int from = *it; ++it; int to = *it; ++it;
				for (;from <=to;++from) {
					if (!isFree(IPoint(from,y))) {return false;}
				}
			}
		}
		previous_y = current_y;
	}
	return true; 
}



