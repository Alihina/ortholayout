#ifndef GRAPHIO_H
#define GRAPHIO_H

#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/GraphCopy.h>
#include <ogdf/basic/Thread.h>
#include <ogdf/basic/CriticalSection.h>
#include <ogdf/internal/energybased/IntersectionRectangle.h>
#include <ogdf/internal/energybased/AdjacencyOracle.h>
#include <GridGraph.h>
#include <Lattice.h>
#if defined(_MSC_VER)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

using namespace ogdf;


//!GUI class for displaying OGDF::GraphAttribute objects
/**
 * use start() from the OGDF::Thread base class to create a new thread
 * that handles input and output through the GraphIO class.
 */


class GUI: public Thread{
public:	
	//! Create a GUI object and define starting size and position for the renderin window aswell as flags for SDL
	GUI(int x = 100, 
		int y = 100, 
		int w = 640, 
		int h = 480, 
		Uint32 winflags = SDL_WINDOW_RESIZABLE, 
		Uint32 rendflags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	~GUI(){};
	
	//!wrapper for SDL_Delay
	void wait(unsigned int t){SDL_Delay(t);}; 
	//! Set the GraphAttributes object for GraphIO to handle. Scale and Offset for the viewport will be set so that the Graph is in center and entirely visible.
	void setGA(GraphAttributes &GA);
	//! Set the GridGraph object for GraphIO to display
	void setGG(GridGraph &GG);
	//! Set the next animation target and duration in ms
	void animateTo(GraphAttributes &GA, unsigned int t);
	// inits animation from currentGA to nextGA and takes note of the received GraphCopy
	void animateTo(GraphAttributes &nextGA, GraphCopy &GC, unsigned int t); 
	//inits animation from currentGA to the current state of the last received GA
	void update(unsigned int t); 

	//! Check if a termination request has been sent by the user
	bool isDone(){return m_done;};
	//! This value is True when the user requests the main programm to pause calculations.
	void waitForUnpause();

	int getTempChange();
	
	List<int> movedNodes; //node ids
	List<DPoint> movedNodeCoord;

	CriticalSection lock; //used to synchronize member access through multiple threads

	

private:
	void doWork();

	GraphAttributes *m_setGA;
	GridGraph *m_setGG;
	GraphAttributes *m_animateTo;
	GraphCopy *m_GC; //! What does this do again??
	
	bool setGG_flag;
	bool setGA_flag;
	bool animateTo_flag;
	bool update_flag;
	HANDLE pauseEvent;

	int m_tempChange;
	int m_x;
	int m_y; 
	int m_w;
	int m_h;
	int m_t;
	bool m_done;
	Uint32 m_winflags;
	Uint32 m_rendflags;

	
};


class GraphIO
{
public:
	
	//Create an SDL Window with position x,y and size w,h with flags winfalgs and renderer with rendflags
	GraphIO(int x , int y, int w, int h, Uint32 winflags, Uint32 rendflags);

	~GraphIO();
	
	bool think(); // handles events and such, returns true when drawing application should terminate

	void setGA(GraphAttributes &newGA);
	void setGG(GridGraph &newGG);
	void getGA(GraphAttributes &GA);

	void animateTo(GraphAttributes &nextGA, unsigned int t); // creates animation from currentGA to nextGA in t milliseconds if nextGA reference the same graph as previous animation target
	void animateTo(GraphAttributes &nextGA, GraphCopy &GC, unsigned int t); // inits animation from currentGA to nextGA and takes note of the received GraphCopy
	void update(unsigned int t); //inits animation from currentGA to the current state of the last received GA

	bool pause;
	int tempChange;

	List<int> movedNodes; //node ids
	List<DPoint> movedNodeCoord;
	
private:
//===================== PRIVATE MEMBER FUNCTIONS ==========================
	
	bool handleEvents(); //returns true when programm is to be terminated
	void OnKeydown();
	void OnKeyUp();
	void OnMousedown();
	void OnMouseup();
	void OnWindowResized();
	void OnMousewheel();
	void OnMouseMotion();

	void clusterize(int p);
	void displayParentarray(const NodeArray<List<node>> &parentarray);
	bool hasAncestor(node v, node w, const NodeArray<List<node>> &parentarray); //checks is w is an ancestor of v
	void bfs_recurse(node v, 
		int dist,
		int p,
		List<node> &searched, 		
		NodeArray<List<node>> &parentarray, 		
		NodeArray<List<NodeArray<List<node>>*>> &registeredArrays);
	void addParents(node v, const NodeArray<List<node>> &parentarray, List<node> &found);

	double GraphIO::dist(DPoint p1,DPoint p2){return (p1-p2).norm();};
	double GraphIO::dist(node v, DPoint p2){
		if(GridMode) return dist(DPoint(currentGG.dx(v),currentGG.dy(v)),p2);
		return dist(DPoint(currentGA.x(v),currentGA.y(v)),p2);
	};
	double GraphIO::dist(DPoint p1, node v){
		if (GridMode) return dist(p1, DPoint(currentGG.dx(v),currentGG.dy(v)));
		return dist(p1, DPoint(currentGA.x(v),currentGA.y(v)));};

	node getNode(int x, int y); //select node at view coordinates x,y
	void initNodemove();	
	void addToSelection(node v);
	void removeFromSelection(node v);
	void clearSelection();
	void brushSelect();
	void brushDeselect();
	List<node> brushFind();
	void breadthSelect();

	void displayNodeInformation(node v);
	bool checkIdConsistency();
	void draw_circle(int n_cx, int n_cy, int radius);
	void set_pixel(int x, int y);
	void drawGrid();

	void zoomView(float a, DPoint center);
	void zoomTo(float newZoom);


	void copyAttributes(GraphAttributes &srcGA, GraphCopy &srcGC, GraphAttributes &trgGA, GraphCopy &trgGC);	
	void prepareGG(GridGraph &GG,DPoint pos); //assigns the continuous variants pos and rot of GG


	void animate();	
	void drawTarget();
	void animateNode(node v, float t); // moves v to appropriate position on 100*t% animation	
	float f(float t); //animation response curve

	void drawSelected();
	void drawSelectedNode(node v);
	void drawSelectedEdge(edge e);
	void drawGraph();
	void drawNode(GraphAttributes &GA, node v, int alpha);
	void drawEdge(GraphAttributes &GA, edge e, int alpha);	
	void drawNode(GridGraph &GG, node v, int alpha);
	void drawEdge(GridGraph &GG, edge e, int alpha);
	void drawOutline(IPolyline outline, DPoint pos);

	void clrscr();

	void recalcGscale(); ///> Calculate the Scale-to-fit factor for the current Graph&Window
	DPoint g2v(DPoint g); //convert global coordinates to Onscreen coordinates
	DPoint v2g(DPoint v); //convert Onscreen coordinates to global coordinates
	DPoint g2v(IPoint g); //convert global coordinates to Onscreen coordinates
	DPoint v2g(IPoint v); //convert Onscreen coordinates to global coordinates
	double g2v(double a); //convert global length to Onscreen coordinates
	double v2g(double a); //convert Onscreen length to global coordinates
	

//===================== PRIVATE MEMBER VARIABLES ==========================
	
	SDL_Window *win;
	SDL_Renderer *rend;
	SDL_Event event;

    Lattice testLattice;
	const Graph *m_pGraph;
	const GraphAttributes *m_pGA;
	const GridGraph *m_pGG;
	const GraphCopy *lastGC;
	const GraphAttributes *lastGA;
	const GridGraph *lastGG;

	bool GridMode;
	
	GraphCopy currentCopy;
	GridGraph currentGG;
	GraphAttributes currentGA;
	
	GraphCopy animTargetCopy;
	GraphAttributes animTargetGA;

	GraphCopy animSourceCopy;
	GraphAttributes animSourceGA;

	NodeArray<int> valpha;
	EdgeArray<int> ealpha;


	unsigned int animStart;	
	unsigned int animEnd;

	bool key_T;
	bool key_KP_plus;
	bool key_KP_minus;
	bool key_shift;
	bool key_strg;
	bool mouse_LB;
	bool mouse_RB;

	bool animating;

	node selNode;
	List<node> selNodes;
	List<edge> selEdges;
	List<DPoint> selNodesOff;

	DPoint offset;  //viewport panning offset
	DPoint temp_offset; //used for scrolling
	float zoom; //current zoom factor
	float Gscale; //Scale-to-fit factor for the Graph
	DPoint MouseDownCoord;

	int brushSize;
	int winw; //window width
	int winh; //window height
	
	static const int border = 110; //size in percent of the Window in relation to graphsize for Graph Scale-to-fit variable
	static const int gridScale = 1; //size of the Grid
	static const double nodesize;
};


#endif