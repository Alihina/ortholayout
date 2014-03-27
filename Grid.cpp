

#include <GridGraph.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <OGDF/basic/Array2D.h>


using namespace ogdf;


Gridpoint::Gridpoint() {
	v_node = NULL; //the node for the vertical grid
	h_node = NULL; //the node for the horizontal grid
	changelayer = NULL; //the edge connecting v_node and h_node

	//pretty sure none of them need to be pointers, as they already are.
	occNode = NULL; //reference to the node that lies on this gridpoint
	v_edge = NULL; //reference to the edge that goes through this gridpoint vertically
	h_edge = NULL; //reference to the edge that goes through this gridpoint horizonatlly
	
	//do we need these? 
	Up = NULL; //pointer to the gridpoint above
	Down = NULL; //pointer to the gridpoint below
	Left = NULL; //pointer to the gridpoint to the left
	Right = NULL;
}





Grid::Grid() {
	m_Grid = Array2D<Gridpoint> (1,1,1,1);
	m_Grid[1][1].v_node = newNode();
	m_Grid[1][1].h_node = newNode();
	m_Grid[1][1].changelayer = newEdge(m_Grid[1][1].v_node,m_Grid[1][1].h_node);
}

Grid::Grid(int a, int b, int c, int d) {
	m_Grid = Array2D<Gridpoint> (a,b,c,d);
	//make all nodes and changelayer edges
	for (int i = a; i <= b; ++i) {
		for (int j = c; j <= d; ++j) {
			m_Grid[i][j].v_node = newNode();
			m_Grid[i][1j.h_node = newNode();
			m_Grid[i][j].changelayer = newEdge(m_Grid[i][j].v_node,m_Grid[i][j].h_node);
		}
	}
	//make horizontal edges
	for (int i = a; i < b; ++i) {
		for (int j = c; j <= d; ++j) {
			newEdge(m_Grid[i][j].h_node,m_Grid[i+1][j].h_node);
			m_Grid[i][j].Left = &m_Grid[i+1][j];
			m_Grid[i+1][j].Right = &m_Grid[i][j];
		}
	}
	//make vertical edges.
	for (int i = a; i <= b; ++i) {
		for (int j = c; j < d; ++j) {
			newEdge(m_Grid[i][j].v_node,m_Grid[i][j+1].v_node);
			m_Grid[i][j].Down = &m_Grid[i][j+1];
			m_Grid[i][j+1].Up = &m_Grid[i][j];
		}
	}
}

IPolyline Grid::findEdge(IPoint A, IPoint B) {
	//shit's complicated, yo. use an existing shortest path algo i guess.
}

void Grid::registerPoint(IPoint A) {
	delNode(m_Grid[A.m_x][A.m_y].v_node);
	delNode(m_Grid[A.m_x][A.m_y].h_node);
	//delEdge(m_Grid[A.m_x][A.m_y].changelayer) //prolly unneccecary.
	m_Grid[A.m_x][A.m_y].v_node = NULL;
	m_Grid[A.m_x][A.m_y].h_node = NULL;
	m_Grid[A.m_x][A.m_y].changelayer = NULL;
}

void Grid::registerLine(IPolyline E) {
	ListIterator<IPoint> it = E.begin();
	registerpoint(*it);
	IPoint last = *it;
	++it;
	OGDF_ASSERT(last != *it);
	while (it.valid()) {
		registerpoint(*it);
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
				if (m_Grid[last.m_x][i].v_node != NULL ){
					delNode(m_Grid[last.m_x][i].v_node);
					m_Grid[last.m_x][i].v_node = NULL;
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
				if (m_Grid[i][last.m_y].h_node != NULL) {
					delNode(m_Grid[i][last.m_y].h_node);
					m_Grid[i][last.m_y].h_node = NULL;
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
	//delete things 
}

void Grid::restorePoint(IPoint A) {
	if (m_Grid[A.m_x][A.m_y].v_node == NULL) {
		m_Grid[A.m_x][A.m_y].v_node = newNode();
	}
	if (m_Grid[A.m_x][A.m_y].h_node == NULL) {
		m_Grid[A.m_x][A.m_y].h_node = newNode();
	}
	if (m_Grid[A.m_x][A.m_y].changelayer == NULL && 
		(searchEdge(m_Grid[A.m_x][A.m_y].h_node,m_Grid[A.m_x][A.m_y].v_node) ||
		 searchEdge(m_Grid[A.m_x][A.m_y].v_node,m_Grid[A.m_x][A.m_y].h_node))) {
		m_Grid[A.m_x][A.m_y].changelayer = newEdge(m_Grid[A.m_x][A.m_y].h_node, m_Grid[A.m_x][A.m_y].v_node);
	}

	if (A.m_x < m_Grid.high1() && m_Grid[A.m_x + 1][A.m_y].h_node != NULL &&
		(searchEdge(m_Grid[A.m_x][A.m_y].h_node,m_Grid[A.m_x + 1][A.m_y].h_node) ==0 && 
		 searchEdge(m_Grid[A.m_x + 1][A.m_y].h_node,m_Grid[A.m_x][A.m_y].h_node) ==0 )) {
		newEdge(m_Grid[A.m_x][A.m_y].h_node,m_Grid[A.m_x + 1][A.m_y].h_node);
	}
	if (A.m_x > m_Grid.low1() && m_Grid[A.m_x - 1][A.m_y].h_node != NULL &&
		(searchEdge(m_Grid[A.m_x][A.m_y].h_node,m_Grid[A.m_x - 1][A.m_y].h_node) ==0 && 
		 searchEdge(m_Grid[A.m_x - 1][A.m_y].h_node,m_Grid[A.m_x][A.m_y].h_node) ==0 )) {
		newEdge(m_Grid[A.m_x][A.m_y].h_node,m_Grid[A.m_x - 1][A.m_y].h_node);
	}
	if (A.m_y < m_Grid.high2() && m_Grid[A.m_x][A.m_y + 1].v_node != NULL && 
		(searchEdge(m_Grid[A.m_x][A.m_y].v_node, m_Grid[A.m_x][A.m_y + 1].v_node) ==0 && 
		 searchEdge(m_Grid[A.m_x][A.m_y + 1].v_node, m_Grid[A.m_x][A.m_y].v_node) ==0 )) {
		newEdge(m_Grid[A.m_x][A.m_y].v_node, m_Grid[A.m_x][A.m_y + 1].v_node);
	}
	if (A.m_y > m_Grid.low2() && m_Grid[A.m_x][A.m_y - 1].v_node != NULL && 
		(searchEdge(m_Grid[A.m_x][A.m_y].v_node, m_Grid[A.m_x][A.m_y - 1].v_node) ==0 && 
		 searchEdge(m_Grid[A.m_x][A.m_y - 1].v_node, m_Grid[A.m_x][A.m_y].v_node) ==0 )) {
		newEdge(m_Grid[A.m_x][A.m_y].v_node, m_Grid[A.m_x][A.m_y - 1].v_node);
	}
} 

void Grid::restoreLine(IPolyline E) {

}

void Grid::restoreFill(IPolyline E) {

}

bool Grid::isFree(IPoint A) {

}

bool Grid::isFree(IPolyline outline) {

}

