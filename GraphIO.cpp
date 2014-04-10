#include "stdafx.h"
#include "GraphIO.h"
#define _USE_MATH_DEFINES
#include <math.h>


const double GraphIO::nodesize = 0.3;

GraphIO::GraphIO(int x , int y, int w, int h, Uint32 winflags, Uint32 rendflags) {

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // Initiate SDL	
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
        
    win = SDL_CreateWindow("GraphViewer", x, y, w, h, winflags); //Create SDL Window
    if (win == NULL) {    
       std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    }

    rend = SDL_CreateRenderer(win, -1, rendflags);
    if (rend == NULL){	
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    }

	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	currentGA = GraphAttributes();
	//currentCopy = GraphCopy();

	m_pGraph = NULL;
	m_pGA = NULL;
	m_pGG = NULL;
	lastGC = NULL;
	lastGA = NULL;
	lastGG = NULL;
	GridMode = false;
				
	//animTargetCopy = GraphCopy();
	animTargetGA = GraphAttributes();

	selNode = NULL;
	zoom = 1.0;
	Gscale = 1.0;

	offset = DPoint();
	temp_offset = DPoint();
	MouseDownCoord = DPoint();
	event = SDL_Event();

	tempChange = 0;
	animStart = 0;
	animEnd = 0;
	brushSize = 50;
	winh = h;
	winw = w;
	pause = false;
	key_T = false;
	key_KP_plus = false;
	key_KP_minus = false;
	key_shift = false;
	key_strg = false;
	mouse_LB = false;
	mouse_RB = false;
	animating = false;
}

GraphIO::~GraphIO() {
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

void GraphIO::clrscr() {
    Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color
    SDL_SetRenderDrawColor(rend,255,255,255,255); //set render color to white    
    SDL_RenderClear(rend);
    SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}

void GraphIO::recalcGscale() { //get graph scale-to-fit ratio
	float winw_b; //Window size augmented by border
	float winh_b;

	winw_b = winw * (100/float(border)); //apply border
	winh_b = winh * (100/float(border));
	
	DRect graphBox;
	if (GridMode){
		IPolyline box = currentGG.getBox();		
		graphBox = DRect(box.front().m_x, box.front().m_y, box.back().m_x, box.back().m_y);
	}else graphBox = currentGA.boundingBox();

	float scalex = winw_b / graphBox.width(); 
	float scaley = winh_b / graphBox.height();
	Gscale = min(scalex, scaley);
}

double GraphIO::g2v(double a) { // Convert global coordinates to viewport coordinates
	return a*zoom;
}

DPoint GraphIO::g2v(DPoint g) { // Convert global coordinates to viewport coordinates	
	DPoint v = g - offset ;
	v.m_x *= zoom;
	v.m_y *= zoom;
	return v;
}

DPoint GraphIO::v2g(DPoint v) { // Convert Onscreen coordinates to global coordinates	
	v.m_x /= zoom;
	v.m_y /= zoom;
	DPoint g = v + offset ;
	
	return g;
}

DPoint GraphIO::g2v(IPoint g) { // Convert global coordinates to viewport coordinates	
	DPoint v = DPoint(g.m_x - offset.m_x, g.m_y - offset.m_y) ;
	v.m_x *= zoom;
	v.m_y *= zoom;
	return v;
}

DPoint GraphIO::v2g(IPoint v) { // Convert Onscreen coordinates to global coordinates	
	DPoint d = DPoint(v.m_x, v.m_y);
	d.m_x /= zoom;
	d.m_y /= zoom;
	DPoint g = d + offset ;
	
	return g;
}

double GraphIO::v2g(double a) { // Convert Onscreen coordinates to global coordinates	
	return a/zoom;
}
node GraphIO::getNode(int x, int y){	
if (GridMode){
	//const Graph &G = currentGG.constGraph();

	node v;

	forall_nodes(v,currentGG){
		node hitTest;// = hitNode( v,x,y ); // *operator converts the iterator returned by get() to reference to its node element
	
	
		float Vw = nodesize; //Get Width, height and top-left point for node rect in global coordinates
		float Vh = nodesize;
		float Vx = currentGG.x(v) - (Vw/2);
		float Vy = currentGG.y(v) - (Vh/2);
		DPoint M = v2g(DPoint(x,y));
		if ((Vx < M.m_x) && (M.m_x < Vx+Vw) && (Vy < M.m_y) && (M.m_y < Vy+Vh)){
			hitTest = v;
		}else{
			hitTest = NULL;
		}
		if (hitTest != NULL) {
			return hitTest;
		}
	}
	return NULL;	

}else{
	const Graph &G = currentGA.constGraph(); //Get the Graph underlying GA	
	node v;

	forall_nodes(v,G){
		node hitTest;// = hitNode( v,x,y ); // *operator converts the iterator returned by get() to reference to its node element
		GraphAttributes &GA = currentGA;
	
		float Vw = GA.width(v); //Get Width, height and top-left point for node rect in global coordinates
		float Vh = GA.height(v);
		float Vx = GA.x(v) - (Vw/2);
		float Vy = GA.y(v) - (Vh/2);
		DPoint M = v2g(DPoint(x,y));
		if ((Vx < M.m_x) && (M.m_x < Vx+Vw) && (Vy < M.m_y) && (M.m_y < Vy+Vh)){
			hitTest = v;
		}else{
			hitTest = NULL;
		}
		if (hitTest != NULL) {
			return hitTest;
		}
	}
	return NULL;	
}
}

void GraphIO:: addToSelection(node v){
	if (selNodes.search(v) == -1){
		selNodes.pushFront(v);
		//const Graph &G = currentGA.constGraph();
		node w;
		edge e = NULL;
		ListIterator<node> it;
		forall_listiterators(node,it,selNodes){
			w = (*it);
			//e = G.searchEdge(v,w);
			adjEntry adj;
			e = NULL;
			forall_adj(adj,v) {
				if(adj->twinNode() == w) e = adj->twin()->theEdge();				
			}//forall_adj
			if (e) if (selEdges.search(e) == -1) {
				selEdges.pushBack(e);				
				testLattice.addLine(currentGG.edgeline(e));
				//std::cout << "Edge Selected" << std::endl;
			}
		}//forall_listiterators
	}//if
}

void GraphIO::removeFromSelection(node v){
	int v_pos = selNodes.search(v);
	if (v_pos != -1){
		//const Graph &G = currentGA.constGraph();
		node w;
		edge e;
		ListIterator<node> it;
		forall_listiterators(node,it,selNodes){
			w = (*it)
				;
			adjEntry adj;
			e = NULL;
			forall_adj(adj,v) {
				if(adj->twinNode() == w) e = adj->twin()->theEdge();				
			}

			if (e) {
				int e_pos = selEdges.search(e);
				selEdges.del(selEdges.get(e_pos));
				testLattice.removeLine(currentGG.edgeline(e));
			}
		}
		selNodes.del(selNodes.get(v_pos));		
	}
}

void GraphIO::clearSelection(){
	selNodes.clear();
	forall_listiterators(edge, it, selEdges){
		testLattice.removeLine(currentGG.edgeline(*it));
	}
	selEdges.clear();
}



void GraphIO::brushSelect(){
	int x,y;
	SDL_GetMouseState(&x,&y);
	DPoint v_mouse(x,y);
	DPoint g_mouse = v2g(v_mouse);

	if (GridMode){
		//const Graph &G = currentGG.constGraph();
		node v;
		forall_nodes(v,currentGG){
			if (dist(g_mouse,v) < v2g(brushSize)) 
				addToSelection(v);
		}
	}else{
		const Graph &G = currentGA.constGraph();
		node v;
		forall_nodes(v,G){
			if (dist(g_mouse,v) < v2g(brushSize)) 
				addToSelection(v);
		}
	}
	
}

void GraphIO::brushDeselect(){
	int x,y;
	SDL_GetMouseState(&x,&y);
	DPoint v_mouse(x,y);
	DPoint g_mouse = v2g(v_mouse);
	if (GridMode){
		//const Graph &G = currentGG.constGraph();
		node v;
		forall_nodes(v,currentGG){
			if (dist(g_mouse,v) < v2g(brushSize)){
				removeFromSelection(v);
			}
		}
	}else{
		const Graph &G = currentGA.constGraph();
		node v;
		forall_nodes(v,G){
			if (dist(g_mouse,v) < v2g(brushSize)){
				removeFromSelection(v);
			}
		}
	}
}

List<node> GraphIO::brushFind(){
	int x,y;
	SDL_GetMouseState(&x,&y);
	DPoint v_mouse(x,y);
	DPoint g_mouse = v2g(v_mouse);
	List<node> foundNodes;
	Graph G = currentGA.constGraph();
	node v;
	forall_nodes(v,G){
		if (dist(g_mouse,v) < v2g(brushSize)) foundNodes.pushFront(v);
	}
	return foundNodes;
}

void GraphIO::breadthSelect(){
	if (selNodes.empty()) return;
	const Graph &G = currentGA.constGraph();
	node v,v2;
	List<edge> edges;
	List<node> newNodes;
	ListIterator<node> it,it2;
	forall_listiterators(node,it,selNodes){
		v = *it;
		G.outEdges(v,edges);
		forall_listiterators(edge,it2,edges){
			v2 = (*it2)->target();
			if (newNodes.search(v2) == -1) addToSelection(v2);		
		}
		G.inEdges(v,edges);
		forall_listiterators(edge,it2,edges){
			v2 = (*it2)->source();
			if (newNodes.search(v2) == -1) addToSelection(v2);
		}//edgeloop
	}// node loop
	selNodes.conc(newNodes);
}


void GraphIO::getGA(GraphAttributes &GA){
	GA = currentGA;
}



bool GraphIO::checkIdConsistency(){
	 bool consistent = true;
	 const Graph &G = currentGA.constGraph();
	 int id = -1;
	 List <int> ids;
	 node v;
	 std::cout << "checking node id consistency" << std::endl;
	 forall_nodes(v,G){
		 id = currentGA.idNode(v);
		 //std::cout << id << std::endl;
		 if (ids.search(id) != -1) {
			 std::cout << "DOUBLE NODE ID :" << id << std::endl;
			 consistent = false;
		 }
		 ids.pushFront(id);
	 }
	 return consistent;
}


void GraphIO::copyAttributes(GraphAttributes &srcGA, GraphCopy &srcGC, GraphAttributes &trgGA, GraphCopy &trgGC){

	const Graph &trgG = trgGC.original();
	const Graph &srcG = srcGC.original();
	if (&srcG != &trgG){
		std::cout << "ERROR in copyAttributes: srcGC and trgGC are copies of different graphs" << std::endl;
		throw -1;
	}

	node v;
	forall_nodes(v,trgGC){
		trgGA.x(v) = srcGA.x(srcGC.copy(trgGC.original(v)));
		trgGA.y(v) = srcGA.y(srcGC.copy(trgGC.original(v)));
		trgGA.width(v) = srcGA.width(srcGC.copy(trgGC.original(v)));
		trgGA.height(v) = srcGA.height(srcGC.copy(trgGC.original(v)));
		trgGA.idNode(v) = srcGA.idNode(srcGC.copy(trgGC.original(v)));
		//trgGA.colorNode(v) = srcGA.colorNode(srcGC.copy(trgGC.original(v)));
	}
}

void GraphIO::setGA(GraphAttributes &newGA) {
		
	const Graph &G = newGA.constGraph();
	m_pGA = &newGA;
	m_pGraph = &G;
	lastGC = NULL;
	lastGA = &newGA;

	if (G.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to set empty graph" << std::endl;
		throw -1;
	}
	GridMode = false;

	currentCopy = GraphCopy(G);
	valpha.init(currentCopy, 255);
	ealpha.init(currentCopy, 255);

	currentGA = GraphAttributes(currentCopy, 
		GraphAttributes::nodeGraphics |	
		GraphAttributes::edgeGraphics | 
		GraphAttributes::nodeLabel | 
		GraphAttributes::edgeType |
		GraphAttributes::nodeColor |
		GraphAttributes::nodeId | 
		GraphAttributes::nodeStyle | 
		GraphAttributes::nodeType | 
		GraphAttributes::nodeTemplate);

	node v;
	forall_nodes(v,currentCopy){
		currentGA.x(v) = m_pGA->x(currentCopy.original(v));
		currentGA.y(v) = m_pGA->y(currentCopy.original(v));
		currentGA.width(v) = m_pGA->width(currentCopy.original(v));
		currentGA.height(v) = m_pGA->height(currentCopy.original(v));
		currentGA.idNode(v) = m_pGA->idNode(currentCopy.original(v));
		//currentGA.colorNode(v) = m_pGA->colorNode(currentCopy.original(v));
	}
	
		
		
	bool consistent = checkIdConsistency();
	if (!consistent) std::cout << "WARNING: Node ids are not consistent!" << std::endl;

	// Recalculate offset so that the center of the Window is on the Center of the Graph
	DPoint VCenter = DPoint(winw/2, winh/2); //Get the Center of the Window
	DRect graphBox = currentGA.boundingBox();
	DPoint diag = graphBox.p2()-graphBox.p1(); //Get the Graph Center
	diag.m_x /= 2;
	diag.m_y /= 2;
	DPoint GCenter = graphBox.p1()+diag;

	offset = GCenter - VCenter; //Set the offset 
	recalcGscale(); // get scale to fit factor	
	zoomTo(Gscale); // scale to fit	

	std::cout << "graph set" << std::endl;

}

void GraphIO::prepareGG(GridGraph &GG){
	node v;
	GG.IOprep() = true;	
	std::cout << "preparing GG# " << GG.id() << std::endl;
	forall_nodes(v,GG){
		GG.dx(v) = GG.x(v);
		GG.dy(v) = GG.y(v);
		GG.dRot(v) = M_PI_2*GG.rot(v);
		if (GG.xMirror(v)) GG.xScale(v) = -1;
		else GG.xScale(v) = 1;
		if (GG.yMirror(v)) GG.yScale(v) = -1;
		else GG.yScale(v) = 1;				
		DPoint pos(GG.getPos().m_x, GG.getPos().m_y); 
		GG.dPos() = pos;
		if (GG.subGG(v)){
			//DPoint nPos;
			//nPos.m_x = pos.m_x + GG.dx(v);
			//nPos.m_y = pos.m_y + GG.dy(v);
			prepareGG(*GG.subGG(v));
		}
	}
	edge e;
	forall_edges(e,GG){		
		forall_listiterators(IPoint,it,GG.edgeline(e)){
			GG.dedgeline(e).pushBack(DPoint((*it).m_x,(*it).m_y));
		}
	}
}

void GraphIO::setGG(GridGraph &newGG) {
		
	const Graph &G = newGG.constGraph();
	m_pGG = &newGG;
	m_pGraph = &G;
	lastGC = NULL;
	lastGG = &newGG;

	if (G.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to set empty graph" << std::endl;
		throw -1;
	}

	GridMode = true;
	currentCopy = GraphCopy(G);
	valpha.init(currentCopy, 255);
	ealpha.init(currentCopy, 255);
	//std::cout << "setting currentGG to newGG... " ;
	currentGG = newGG;
	
	adjEntry adj;
	node v = currentGG.firstNode();
	std::cout << "adjacent edgelines setGG: " << std::endl;
	forall_adj(adj,v){
		IPolyline line = currentGG.edgeline(adj->theEdge());
		/*std::cout << adj->theNode() << "->" << adj->twinNode() << " = ";*/
		std::cout << adj->theEdge() << ":  {";
		forall_listiterators(IPoint,it,line){
			std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
		}
		std::cout << std::endl;
	}

	//std::cout << "done" << std::endl;
	prepareGG(currentGG);

	IPolyline box = currentGG.getBox();

	// Recalculate offset so that the center of the Window is on the Center of the Graph
	DPoint VCenter = DPoint(winw/2, winh/2); //Get the Center of the Window
	DRect graphBox(box.front().m_x, box.front().m_y, box.back().m_x, box.back().m_y);
	DPoint diag = graphBox.p2()-graphBox.p1(); //Get the Graph Center
	diag.m_x /= 2;
	diag.m_y /= 2;
	DPoint GCenter = graphBox.p1()+diag;

	offset = GCenter - VCenter; //Set the offset 
	recalcGscale(); // get scale to fit factor	
	zoomTo(Gscale); // scale to fit	

	std::cout << "Gridgraph set" << std::endl;

}

void GraphIO::animateTo(GraphAttributes &nextGA, GraphCopy &GC, unsigned int t){ 
	
	const Graph &G = nextGA.constGraph();
	const Graph &GC_orig = GC.original();

	if (G.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to animate to empty graph" << std::endl;
		throw -1;
	}
	if (t == 0){
		std::cout << "ERROR: attempting to animate in 0ms" << std::endl;
		throw -1;
	}
	if (&GC_orig != m_pGraph){
		std::cout << "ERROR: attempting to animate to copy of a different Graph" << std::endl;
		throw -1;
	}

	animating = true;
	lastGC = &GC;
	lastGA = &nextGA;
	animTargetCopy = GraphCopy(GC);
	animTargetGA = 	GraphAttributes(animTargetCopy, 
		GraphAttributes::nodeGraphics |	
		GraphAttributes::edgeGraphics | 
		GraphAttributes::nodeLabel | 
		GraphAttributes::edgeType |
		GraphAttributes::nodeColor |
		GraphAttributes::nodeId | 
		GraphAttributes::nodeStyle | 
		GraphAttributes::nodeType | 
		GraphAttributes::nodeTemplate);

	animSourceCopy = GraphCopy(currentCopy);
	animSourceGA = 	GraphAttributes(animSourceCopy, 
		GraphAttributes::nodeGraphics |	
		GraphAttributes::edgeGraphics | 
		GraphAttributes::nodeLabel | 
		GraphAttributes::edgeType |
		GraphAttributes::nodeColor |
		GraphAttributes::nodeId | 
		GraphAttributes::nodeStyle | 
		GraphAttributes::nodeType | 
		GraphAttributes::nodeTemplate);
	
	copyAttributes(nextGA,GC,animTargetGA,animTargetCopy);
	copyAttributes(currentGA,currentCopy,animSourceGA,animSourceCopy);

	animStart = SDL_GetTicks();	
	animEnd = animStart + t;

}

void GraphIO::animateTo(GraphAttributes &nextGA, unsigned int t){ 

	//std::cout << "GraphIO::animateTo" << std::endl;
	const Graph &G = nextGA.constGraph();

	if (G.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to animate to empty graph" << std::endl;
		throw -1;
	}
	if (t == 0){
		std::cout << "ERROR: attempting to animate in 0ms" << std::endl;
		throw -1;
	}
	if ( (lastGC == NULL && &G != m_pGraph) | (lastGC != NULL && &G != lastGC ) ){
		std::cout << "ERROR: attempting to animate to GA of a different Graph" << std::endl;
		throw -1;
	}
	animating = true;
	lastGA = &nextGA;

	animTargetCopy = GraphCopy(currentCopy);
	animTargetGA = GraphAttributes(animTargetCopy, 
		GraphAttributes::nodeGraphics |	
		GraphAttributes::edgeGraphics | 
		GraphAttributes::nodeLabel | 
		GraphAttributes::edgeType |
		GraphAttributes::nodeColor |
		GraphAttributes::nodeId | 
		GraphAttributes::nodeStyle | 
		GraphAttributes::nodeType | 
		GraphAttributes::nodeTemplate);

	animSourceCopy = GraphCopy(currentCopy);
	animSourceGA = GraphAttributes(animSourceCopy, 
		GraphAttributes::nodeGraphics |	
		GraphAttributes::edgeGraphics | 
		GraphAttributes::nodeLabel | 
		GraphAttributes::edgeType |
		GraphAttributes::nodeColor |
		GraphAttributes::nodeId | 
		GraphAttributes::nodeStyle | 
		GraphAttributes::nodeType | 
		GraphAttributes::nodeTemplate);
	
	node v;

	if (lastGC){
		forall_nodes(v,animTargetCopy){
			animTargetGA.x(v) = nextGA.x(lastGC->copy(animTargetCopy.original(v)));
			animTargetGA.y(v) = nextGA.y(lastGC->copy(animTargetCopy.original(v)));
			animTargetGA.width(v) = nextGA.width(lastGC->copy(animTargetCopy.original(v)));
			animTargetGA.height(v) = nextGA.height(lastGC->copy(animTargetCopy.original(v)));
			animTargetGA.idNode(v) = nextGA.idNode(lastGC->copy(animTargetCopy.original(v)));
			//animTargetGA.colorNode(v) = nextGA.colorNode(lastGC->copy(animTargetCopy.original(v)));
		}
		forall_nodes(v, animSourceCopy){
			animSourceGA.x(v) = currentGA.x(lastGC->copy(animSourceCopy.original(v)));
			animSourceGA.y(v) = currentGA.y(lastGC->copy(animSourceCopy.original(v)));
			animSourceGA.width(v) = currentGA.width(lastGC->copy(animSourceCopy.original(v)));
			animSourceGA.height(v) = currentGA.height(lastGC->copy(animSourceCopy.original(v)));
			animSourceGA.idNode(v) = currentGA.idNode(lastGC->copy(animSourceCopy.original(v)));
		}
	}else{
		forall_nodes(v,animTargetCopy){
			animTargetGA.x(v) = nextGA.x(animTargetCopy.original(v));
			animTargetGA.y(v) = nextGA.y(animTargetCopy.original(v));
			animTargetGA.width(v) = nextGA.width(animTargetCopy.original(v));
			animTargetGA.height(v) = nextGA.height(animTargetCopy.original(v));
			animTargetGA.idNode(v) = nextGA.idNode(animTargetCopy.original(v));
			//animTargetGA.colorNode(v) = nextGA.colorNode(animTargetCopy.original(v));
		}

		forall_nodes(v, animSourceCopy){
			animSourceGA.x(v) = currentGA.x(animSourceCopy.original(v));
			animSourceGA.y(v) = currentGA.y(animSourceCopy.original(v));
			animSourceGA.width(v) = currentGA.width(animSourceCopy.original(v));
			animSourceGA.height(v) = currentGA.height(animSourceCopy.original(v));
			animSourceGA.idNode(v) = currentGA.idNode(animSourceCopy.original(v));
		}
	}

	animStart = SDL_GetTicks();	
	animEnd = animStart + t;
	
}

void GraphIO::update(unsigned int t){
	std::cout << "UPDATE NOT YET IMPLEMENTED" << std::endl;
}

void GraphIO::animate(){
	unsigned int now = SDL_GetTicks();
	if (now < animEnd){		
		drawTarget();	    
		node v;
		float t = float(now - animStart)/float(animEnd - animStart); // 0 < t < 1;						
		forall_nodes(v,currentCopy) animateNode(v,t);		
		//std::cout<< t <<std::endl;
	}else{		
		if (animating){
			currentGA = animTargetGA;
			currentCopy = animTargetCopy;		
		}
		animating = false;

	}
}

void GraphIO::drawTarget(){
	
	GraphAttributes &GA = animTargetGA;
	GraphCopy &GC = animTargetCopy;
	
	node v;
	edge e;

	if (GC.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to draw empty Target" << std::endl;
		throw -1;
	}
	//std::cout << "drawing " << G.numberOfNodes() << " nodes" << std::endl;

	forall_edges(e,GC) drawEdge(GA,e,30);
	forall_nodes(v,GC) drawNode(GA,v,30);
}

void GraphIO::animateNode(node v, float t){
	if (selNode && selNodes.search(v) != -1) return;
	
	node w = animTargetCopy.copy(currentCopy.original(v));
	node s = animSourceCopy.copy(currentCopy.original(v));
	if (w == NULL){
		valpha[v] = 255*(1-t);
		edge e;
		forall_adj_edges(e,v){
			ealpha[e] = 255*(1-t);
		}
		
	}else{	

		float delta_x = (animTargetGA.x(w) - animSourceGA.x(s)) * f(t);
		float delta_y = (animTargetGA.y(w) - animSourceGA.y(s)) * f(t);
		//float width = (animTarget.width(v) - animSource.width(v)) * f(t);
		//float height = (animTarget.height(v) - animSource.height(v)) * f(t);
		currentGA.x(v) = animSourceGA.x(s) + delta_x;
		currentGA.y(v) = animSourceGA.y(s) + delta_y;
		//currentGA.width(v) = width;
		//currentGA.height(v) = height;
		//if (selNodes.search(v) != -1) std::cout << "distanceX: " << (animTargetGA.x(w) - currentGA.x(v)) << "  delta_x: " << delta_x << "   t: " <<  t << std::endl;

	}
}

float GraphIO::f(float t){ 
	if (0 <= t && t <= 1){
		if (t < 0.5) {
			t = 2*t;
			t = t*t*t;
			t = t/2;
		}else{
			t = t-0.5;
			t = 2*t;
			t = 1-t;
			t = t*t*t;
			t = t/2;
			t = 1-t;
		}
		return t;
	}else{
		std::cout << "ERROR: t = " << t << ", should be between 0 and 1" << std::endl;
		throw -1;
	}
}


void GraphIO::addParents(node v, const NodeArray<List<node>> &parentarray, List<node> &found){
	//if (found.search(v) != -1) {		--

	//	std::cout << "v already found" << std::endl;
	//	return; //if v is already found, return (will this ever occur? YES! is it a problem?)
	//}
	if (parentarray[v].search(v) != -1) { //if the root has been reached
		if (found.search(v) == -1) found.pushFront(v); 
		//std::cout << "r";
		return; //addParents is done
	}
	ListIterator<node> it;
	//if (parentarray[v].size() > 1) std::cout << "BAM!!!";
	forall_listiterators(node, it, parentarray[v]){ //for all parents of v ! ! ! TWO PARENTS -> POSSIBLE INFT-LOOP!!! 
		//std::cout << "+" << *it;
		if (found.search(*it) == -1) found.pushFront(*it);
		addParents(*it,parentarray,found); //add its parents
	}

}

void GraphIO::bfs_recurse(node v, 
		int dist,
		int p,
		List<node> &searched, 		
		NodeArray<List<node>> &parentarray, 		
		NodeArray<List<NodeArray<List<node>>*>> &registeredArrays){
	
	adjEntry adj;
	dist++;
	//std::cout << dist;
	if (dist > p ) return; //stop BFS if maxDist has been reached
	forall_adj(adj,v){ //for every
		node w = adj->twinNode(); //neighboring node w		
		if (parentarray[v].search(w) == -1){ //that is not where BFS just came from			
			if (searched.search(w) == -1) searched.pushFront(w); // mark w as searched and
			if (!hasAncestor(v,w,parentarray)){ //if continuing with w doesn't create a parentloop
				parentarray[w].pushFront(v); // make v w's parent,
				if (registeredArrays[w].search(&parentarray) == -1) registeredArrays[w].pushFront(&parentarray); //register the parentarray and
				bfs_recurse(w,dist, p,searched, parentarray, registeredArrays); //continue the BFS 
			}
		}
	}// every neighbor has been searched


	/* All this is probably unnecessary*/
	//	if (searched.search(w) == -1) { //if node w has not yet been visited at all
	//		parentarray[w].pushFront(v); // make v w's parent
	//		searched.pushFront(w);
	//		bfs_recurse(w,dist, p,searched, found,parentarray, parentarrays); //continue bfs
	//	}else{ //if has been visited before 
	//		if (parentarray[w].empty()){ // if the BFS has not been here before				
	//			//std::cout << "!";
	//			parentarray[w].pushFront(v); // make v w's parent and
	//			found.pushFront(w); //add the node to found and		
	//			//std::cout << "PARENTARRAY: ";
	//			//displayParentarray(parentarray);
	//			addParents(w,parentarray,found); //its parents
	//			ListIterator<NodeArray<List<node>>> it;							
	//			forall_listiterators(NodeArray<List<node>>, it, parentarrays){	//for all other BFSs
	//				if (!(*it)[w].empty()){
	//					//std::cout << "OTHER PARENTARRAY: "; 
	//					//displayParentarray(*it);
	//					addParents(w,(*it),found);
	//				}	//add parents for appropriate BFS paths.											
	//			}				
	//			/* NOTE: Don't continues this BFS if another has been found to save time and avoid leaves */
	//		}else{ //if the BFS has been here before								
	//			if (!hasAncestor(v,w,parentarray)){ //don't continue if it would result in a parent-selfloop 
	//				parentarray[w].pushFront(v); // just make v w's parent and
	//				bfs_recurse(w,dist, p,searched, found,parentarray, parentarrays); //continue bfs (this is necessarry)
	//			}
	//		}
	//	}		
	//}
}

bool GraphIO::hasAncestor(node v, node w, const NodeArray<List<node>> &parentarray){
	if (v == w) return true;

	ListIterator<node> it;
	bool value = false;
	forall_listiterators(node,it,parentarray[v]){
		if (*it == v) return false;
		if (hasAncestor(*it,w,parentarray)) value = true;
	}
	return value;

}
void GraphIO::displayParentarray(const NodeArray<List<node>> &parentarray){
	node v;
	ListIterator<node> it;
	forall_nodes(v,currentCopy){
		//std::cout <<std::endl << "[" << v <<": "<< parentarray[v].size();
		if (!parentarray[v].empty()){
			std::cout << "[" << v << "->";
			forall_listiterators(node, it, parentarray[v]){
				std::cout << *it << "|";
			}
			std::cout << "]";
		}
	}
	std::cout << ::endl;

};

void GraphIO::clusterize(int p){
	currentGG.clusterize(p);

	//NodeArray<List<NodeArray<List<node>>*>> registeredArrays; //For every node in currentCopy save a list of pointers to the parentarrays of the BFS searches that visited the node.
	//registeredArrays.init(currentCopy);
	//List<NodeArray<List<node>>> parentarrays; //for every BFS save the Nodearray that point each node to its parent.
	//ListIterator<NodeArray<List<node>>> itPA;
	//ListIterator<node> it;
	//ListIterator<NodeArray<List<node>>*> itPAp;
	//List<node> found; //the list of nodes that will be added to the selection
	//List<node> searched; //the list of nodes that have been covered by at least one BFS
	//List<edge> hidden; //the list of edges that have been hidden
	//ListIterator<edge> ite;
	//List<node> neighbors; //the list of nodes that neighbor the current selection

	//node v = NULL;
	//node w = NULL;
	//edge e = NULL;
	//
	///*parentarrays.clear();
	//searched.clear();
	//found.clear();*/

	//std::cout << "clustering " << selNodes.size() << " nodes with parameter " << p<< std::endl;

	//
	//forall_listiterators(node, it, selNodes){ // compile a list of neighboring nodes
	//	std::cout << "search cluster for " << *it << std::endl;
	//	v = (*it);		
	//	//std::cout << "v is " << v << std::endl;
	//	adjEntry adj;				
	//	List<adjEntry> adjList;
	//	ListIterator<adjEntry> itadj;
	//	forall_adj(adj,v){ //compile list of adjEntries because hiding edges during forall_adj breaks everything
	//		//std::cout << "adj is " << adj << std::endl;
	//		adjList.pushFront(adj);
	//	}
	//	forall_listiterators(adjEntry,itadj,adjList){ 
	//		adj = *itadj;
	//		w = adj->twinNode();
	//		e = adj->theEdge();
	//		if (selNodes.search(w) == -1) {
	//			neighbors.pushFront(w);
	//			hidden.pushFront(e);
	//			currentCopy.hideEdge(e);
	//		}
	//	}
	//}
	//forall_listiterators(node,it,neighbors){ //for every neighbor of the current selection
	//	v = (*it);
	//	if (searched.search(v) == -1) searched.pushFront(v);
	//	NodeArray<List<node>> parentarray2(currentCopy,List<node>());
	//	parentarrays.pushFront(parentarray2);
	//	NodeArray<List<node>> &parentarray = parentarrays.front(); //I'm not entirely sure why, but this works and results in pointers that do what they're supposed to.
	//	registeredArrays[v].pushFront(&parentarray);
	//	parentarray[v].pushFront(v);
	//	bfs_recurse(v,0, p,searched, parentarray, registeredArrays); //construct a BFS Parentarray
	//	
	//	if (&parentarrays.front() != registeredArrays[v].front()) std::cout << std::endl<< std::endl<< std::endl<< std::endl<<"===============SOMETHING WENT WRONG==============="<< std::endl<< std::endl<< std::endl<< std::endl<< std::endl;
	//}
	//
	//forall_listiterators(edge,ite,hidden){ // restores the edges
	//	currentCopy.restoreEdge(*ite);
	//}

	//
	//forall_listiterators(NodeArray<List<node>>, itPA, parentarrays){
	//		int pos = parentarrays.pos(itPA);
	//		std::cout << "parrentarray nr."<< pos << ": ";
	//		displayParentarray(*itPA);	
	//}
	//
	////forall_nodes(v,currentCopy){
	////	std::cout << v << ": " << registeredArrays[v].size() << std::endl;
	////}
	////
	//forall_nodes(v,currentCopy){ //for all nodes
	//	if (registeredArrays[v].size() > 1){ //if node has been coverd by at least two BFSs
	//		if (found.search(v) == -1) found.pushFront(v); //add the node
	//		forall_listiterators(NodeArray<List<node>>*,itPAp,registeredArrays[v]){	//and all parents for every parentarray
	//			addParents(v,*(*itPAp),found);
	//		}
	//	}
	//}
	////std::cout << "found nodes: ";
	//forall_listiterators(node,it,found){
	//	//std::cout << *it << " ";
	//	addToSelection(*it);
	//}


	////	forall_listiterators(adjEntry,itadj,adjlist){
	////		adj = (*itadj);
	////		node w = adj->twinNode();
	////		if (selNodes.search(w) == -1){
	////			int dist = 0;				
	////			NodeArray<List<node>> parentarray(currentCopy);
	////			currentCopy.hideEdge(adj->theEdge());
	////			
	////			if (searched.search(w)){ //if the node has been covered
	////				forall_listiterators(NodeArray<List<node>>, it, parentarrays)//for all other BFSs
	////					if (!(*it)[w].empty()) addParents(w,(*it),found); //add the parrents																
	////				parentarray[w].pushFront(v); // make v w's parent (although this should be unnecessary) and
	////				found.pushFront(w); //add the node to found 								
	////				ListIterator<NodeArray<List<node>>> it;											
	////				/* NOTE: Don't start this BFS if another has been found to save time and avoid leaves */
	////			}else{ //if the node has not been found by other bfs			
	////				searched.pushFront(w);
	////				parentarray[w].pushFront(w);					
	////				bfs_recurse(w,dist, p,searched, found,parentarray, parentarrays);
	////				parentarrays.pushFront(parentarray);					
	////			}
	////			currentCopy.restoreEdge(adj->theEdge());			

	////		}else std::cout << w << " is already selected" << std::endl;			
	////		
	////		
	////	}//forall_listiterators AdjEntries
	////	
	////	forall_listiterators(node, it, found){ // add found nodes to selection
	////		std::cout << "adding "<< *it << " to selection" << std::endl;
	////		addToSelection(*it);
	////	}

	////}
	////
}

void GraphIO::OnKeydown(){
	node e1 = currentCopy.chooseNode();
	node e2 = currentCopy.chooseNode();
	ListIterator<node> it;
	ListIterator<edge> it2;
	node v;
	const Graph &G = currentGA.constGraph();
	int j ;
	switch(event.key.keysym.sym){
		case SDLK_1: //scale to fit
			//zoomTo(Gscale);
			clusterize(1);
			break;
		case SDLK_2: //scale to original size
			//zoomTo(1.0);
			clusterize(2);
			break;
		case SDLK_3: 
			clusterize(3);
			break;
		case SDLK_4: 
			clusterize(4);
			break;
		case SDLK_5: 
			clusterize(5);
			break;
		case SDLK_6: 
			clusterize(6);
			break;
		case SDLK_7: 
			clusterize(7);
			break;
		case SDLK_8: 
			clusterize(8);
			break;
		case SDLK_9: 
			clusterize(9);
			break;
		case SDLK_0: 
			clusterize(10);
			break;
		case SDLK_c:
			if (selNode){
				//selNodes = currentGG.trimCluster(selNodes,selNode);
				//currentGG.moveToCluster(selNode->firstAdj()->twinNode(),selNode);
				forall_listiterators(node,it,selNodes){
					if(*it != selNode){
						currentGG.moveToCluster(*it, selNode);
					}
				}
			}
			break;
		case SDLK_SPACE: //pause and unpause
			pause = !pause;
			break;			
		case SDLK_t: //pause and unpause
			key_T = true;
			break;
		case SDLK_KP_PLUS:
			if (key_T && !key_KP_plus){
				++tempChange;
				key_KP_plus = true;
			}else{
				forall_listiterators(node,it,selNodes){
					v = (*it)->succ();
					if (v != NULL){
						std::cout << v << std::endl;
						removeFromSelection((*it));
						addToSelection(v);
					   
					}
				}
			}
			break;		
		case SDLK_KP_MINUS:
			if (key_T && !key_KP_minus){
				--tempChange;
				key_KP_minus = true;
			}else{
				forall_listiterators(node,it,selNodes){
					v = (*it)->pred();
					if (v){
						removeFromSelection((*it));
						addToSelection(v);
					
					}

				}
			}
			break;		
		case SDLK_LSHIFT:
			key_shift = true;
			break;
		case SDLK_b:
			breadthSelect();
			break;

		case SDLK_p:			
			std::cout << selNodes.size() << std::endl;
			break;
		case SDLK_s:
			std::cout << "selected: [";
			forall_listiterators(edge,it2,selEdges){
			std::cout << (*it2) << ";";
			}
			std::cout << "]" << endl;
			break;
		case SDLK_LCTRL:
			key_strg = true;
			break;
		case SDLK_n:
			std::cout << " asdf  " << std::endl;
			forall_listiterators(edge,it2,selEdges){
				std::cout << *it2 << std::endl;
				node dummy = currentCopy.newNode();
				currentCopy.newEdge((*it2)->source(), dummy);
				currentCopy.newEdge(dummy, (*it2)->target());
				currentGA.height(dummy) = 10;
				currentGA.width(dummy) = 10;
				currentGA.x(dummy) = (currentGA.x((*it2)->source())+currentGA.x((*it2)->target()) )/2;
				currentGA.y(dummy) = (currentGA.y((*it2)->source())+currentGA.y((*it2)->target()) )/2;
				currentCopy.delEdge((*it2));
				
			}
			selEdges.clear();
			break;


		}
}

void GraphIO::OnKeyUp(){
	switch(event.key.keysym.sym){
		case SDLK_t: 
			key_T = false;
			break;
		case SDLK_KP_PLUS:
				key_KP_plus = false;
			break;		
		case SDLK_KP_MINUS:
			key_KP_minus = false;	
			break;
		case SDLK_LSHIFT:
			key_shift = false;
			break;
		case SDLK_LCTRL:
			key_strg = false;
			break;
	}
}

void GraphIO::displayNodeInformation(node v){
if (GridMode){
	
	adjEntry adj;
	node v3 = currentGG.firstNode();

	std::cout << "=================================================================" << std::endl;
	std::cout << "Node Attributes" << std::endl;
	std::cout << "=================================================================" << std::endl;
	std::cout << "node: " << v ;
	std::cout << " [" << currentGG.x(v) << ":" << currentGG.y(v) << "]";
	std::cout << " (" << currentGG.dx(v) << ":" << currentGG.dy(v) << ")" << std::endl;	
	std::cout << "adjacent nodes: " << std::endl;

	forall_adj(adj,v){
		edge e = adj->theEdge();
		std::cout << adj->twinNode();
		std::cout << " [" << currentGG.x(adj->twinNode()) << ":" << currentGG.y(adj->twinNode()) << "]";
		std::cout << " (" << currentGG.dx(adj->twinNode()) << ":" << currentGG.dy(adj->twinNode()) << ")" << std::endl;
		/*std::cout << "edge: " << e << std::endl;
		std::cout << "source: " << e->source() << " ";
		std::cout << " [" << currentGG.edgeline(e).front().m_x << ":" << currentGG.edgeline(e).front().m_y << "]" << std::endl; */

	}
	if (currentGG.GridGraph_of(v)) currentGG.GridGraph_of(v)->displayDebug();
	//std::cout << "adjacent edgelines with forall_adj_edges: " << std::endl;
	//edge e;
	//forall_adj_edges(e,v){
	//	IPolyline line = currentGG.edgeline(e);
	//	std::cout << e << "  ";
	//	forall_listiterators(IPoint,it,line){
	//		std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
	//	}
	//	std::cout << std::endl;
	//}




	//std::cout << "adjacent edgelines: " << std::endl;
	//forall_adj(adj,v){
	//	IPolyline line = currentGG.edgeline(adj->theEdge());
	//	DPolyline dline = currentGG.dedgeline(adj->theEdge());
	//	std::cout << adj->theNode() << "->" << adj->twinNode() << " = ";
	//	std::cout << adj->theEdge() << ":  {";
	//	forall_listiterators(IPoint,it,line){
	//		std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
	//	}
	//	/*std::cout << "}    {";
	//	forall_listiterators(DPoint,it,dline){
	//		std::cout << "(" << (*it).m_x << ":" << (*it).m_y << ")";
	//	}*/
	//	std::cout << "}" << std::endl;

	//	
	//	edge e2 = currentGG.searchEdge(adj->theNode(),adj->twinNode());
	//	std::cout << "Searchedge: " << std::endl;
	//	if (e2 == NULL) e2 = currentGG.searchEdge(adj->twinNode(),adj->theNode());
	//	std::cout << e2 << " ";
	//	forall_listiterators(IPoint,it,currentGG.edgeline(e2)){
	//		std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
	//	}
	//	std::cout << std::endl;

	//}
	//std::cout << std::endl;
	//std::cout << "adjEntry is Now: " << adj << std::endl;

	//edge e2;	
	//std::cout << "all edges:" << std::endl;
	//forall_edges(e2,currentGG){
	//	std::cout << e2 << " ";
	//	forall_listiterators(IPoint,it,currentGG.edgeline(e2)){
	//		std::cout << "[" << (*it).m_x << ":" << (*it).m_y << "]";
	//	}
	//	std::cout << std::endl;
	//}
}else{


//	String ogdfS = currentGA->colorNode(*v);
//	char * end = &ogdfS[0];
//	const char * CS = ogdfS.cstr();
//	std::cout << "C++ String: " << CS << std::endl;
//	std::cout << "int: " << std::strtol(CS,NULL,16) << std::endl;

	float x = currentGA.x(v);
	float y = currentGA.y(v);
	DPoint OnScreen = g2v(DPoint(x,y));
	std::cout << "=================================================================" << std::endl;
	std::cout << "Node Attributes" << std::endl;
	std::cout << "=================================================================" << std::endl;
//	std::cout << "OnScreen Coordinates: " << OnScreen.m_x << ":" << OnScreen.m_y << std::endl;
//	std::cout << "Coordinates: " << x << ":" << y << std::endl;
	std::cout << "idnode: " << currentGA.idNode(v) << std::endl;
//	std::cout << "node: " << v << std::endl;
	std::cout << "index: " << v->index() << std::endl;
//	std::cout << "Size: " << currentGA.width(v) << ":" << currentGA.height(v) << std::endl;
//	std::cout << "Color: " << currentGA.colorNode(v) << std::endl;
//	std::cout << "ImageAlignmentNode: " << currentGA.imageAlignmentNode(v) << std::endl;
//	std::cout << "ImageStyleNode: " << currentGA.imageStyleNode(v) << std::endl;
//	std::cout << "ImageUriStyle: " << currentGA.imageUriNode(*v) << std::endl;
//	std::cout << "ImageWidthiStyle: " << currentGA.imageWidthNode(*v) << std::endl;
//	std::cout << "LabelNode: " << currentGA.labelNode(*v) << std::endl;
//	std::cout << "level: " << currentGA.level(*v) << std::endl;
//	std::cout << "lineWidthNode: " << currentGA.lineWidthNode(*v) << std::endl;
//	std::cout << "shapeNode: " << currentGA.shapeNode(v) << std::endl;
//	std::cout << "styleNode: " << currentGA.styleNode(v) << std::endl;
//	std::cout << "templateNode: " << currentGA.templateNode(*v) << std::endl;
//	std::cout << "weight: " << currentGA.weight(*v) << std::endl;	
	
	std::cout << "adjacent nodes: [";	
	edge e;
	node w;
	adjEntry adj;
	forall_adj(adj,v){
		std::cout << adj->twinNode() << ";";
		if (adj == v->lastAdj()) break;
	}
	std::cout << "]" << std::endl;





	std::cout << std::endl;
	std::cout << std::endl;

}
}

void GraphIO::OnMousedown(){	
	node v;

	switch (event.button.button) {
	case SDL_BUTTON_LEFT:
		mouse_LB = true;		
		MouseDownCoord.m_x = event.button.x;
		MouseDownCoord.m_y = event.button.y;
		selNode = getNode(event.button.x,event.button.y);		

		if (!key_shift && !key_strg && selNodes.search(selNode) == -1 ) clearSelection();

		if (selNode) {
			addToSelection(selNode);
			initNodemove();
			displayNodeInformation(selNode);
		}
		if (key_strg) brushSelect();
		break;
	case SDL_BUTTON_RIGHT:		//init panning
		mouse_RB = true;
		MouseDownCoord.m_x = event.button.x;
		MouseDownCoord.m_y = event.button.y;
		temp_offset = offset;
		break;					
	}
}

void GraphIO::initNodemove(){
if (GridMode){
	node v;
	selNodesOff.clear();
	ListIterator<node> it;
	double x,y;
	forall_listiterators(node,it,selNodes){
		x = currentGG.dx((*it)) - currentGG.dx(selNode);
		y = currentGG.dy((*it)) - currentGG.dy(selNode);
		selNodesOff.pushFront(DPoint(x,y));
	}

}else{
	node v;
	selNodesOff.clear();
	ListIterator<node> it;
	double x,y;
	forall_listiterators(node,it,selNodes){
		x = currentGA.x((*it)) - currentGA.x(selNode);
		y = currentGA.y((*it)) - currentGA.y(selNode);
		selNodesOff.pushFront(DPoint(x,y));
	}
}
}

void GraphIO::OnMouseup(){
	switch (event.button.button) {
	case SDL_BUTTON_LEFT:
		mouse_LB = false;
		if (selNode){
			ListIterator<node> it;
			node v;
			if (GridMode){
			}else{
				forall_listiterators(node,it,selNodes){
					movedNodes.pushFront(currentGA.idNode((*it)));
					movedNodeCoord.pushFront( DPoint(currentGA.x( (*it) ),currentGA.y( (*it) ) ) );		
					if (animating)  {
						node w = animTargetCopy.copy(currentCopy.original(*it));
						if (w){
							animTargetGA.x( w ) = currentGA.x( (*it) );
							animTargetGA.y( w ) = currentGA.y( (*it) );
						}
					}
				}
			}				
			selNode = NULL;			
	}
		break;
	case SDL_BUTTON_RIGHT:
		mouse_RB = false;
		if (selNode){
			
		}
		break;					
	}
}

void GraphIO::OnMousewheel(){
	float a = pow(1.07177,event.wheel.y);
	int x;
	int y;
	SDL_GetMouseState(&x,&y);

	if (key_strg){
		brushSize *= a;
	}else{
	zoomView(a, DPoint(x,y) );
	}
}


void GraphIO::zoomTo(float newZoom){
	DPoint VCenter = DPoint(winw/2, winh/2); //Get the Center of the Window
	float a = newZoom/zoom;
	zoomView(a, VCenter);
}

void GraphIO::zoomView(float a, DPoint center){	
	DPoint b = v2g(center)-offset;
	zoom = zoom*a;
	//std::cout << zoom << std::endl;
	DPoint deltaOffset = DPoint( (1-(1/a))*b.m_x , (1-(1/a))*b.m_y);
	offset = offset + deltaOffset;
}




void GraphIO::OnMouseMotion(){
	if (key_strg){
		switch(event.motion.state) {
		case  SDL_BUTTON_RMASK:
			brushDeselect();
			break;
		case  SDL_BUTTON_LMASK:
			brushSelect();
		}
	}else{
		if (mouse_RB) { //Panning
			offset.m_x = temp_offset.m_x  - (event.motion.x - MouseDownCoord.m_x) * (1/zoom);
			offset.m_y = temp_offset.m_y - (event.motion.y - MouseDownCoord.m_y) * (1/zoom);							
		}
		if (selNode != NULL){ // move selected nodes		
			DPoint Gmouse = v2g(DPoint(event.motion.x , event.motion.y));			
			ListIterator<node> it;
			ListIterator<DPoint> it2;
			it2 = selNodesOff.begin();
			forall_listiterators(node,it,selNodes){				
				OGDF_ASSERT(it2.valid())
				node v = *it;
				DPoint coord = *it2;
				coord = coord + Gmouse;
				if (GridMode){
					currentGG.dx(v) = coord.m_x;
					currentGG.dy(v) = coord.m_y;				
				}else{
					currentGA.x(v) = coord.m_x;
					currentGA.y(v) = coord.m_y;				
				}
					it2++;
			}
		}else if (mouse_LB && !key_strg) {//Box selection
			if (GridMode){
				const Graph &G = currentGG.constGraph();
				node v;
				DPoint Vmouse = DPoint(event.motion.x , event.motion.y);	
				DPoint Gmouse = v2g(Vmouse);	
				IntersectionRectangle box(v2g(MouseDownCoord), Gmouse);
				forall_nodes(v,G){
					DPoint p(currentGG.dx(v), currentGG.dy(v));
					int pos =selNodes.search(v);
					if (box.inside(p)) addToSelection(v);
					else if (pos != -1)	selNodes.del(selNodes.get(pos));
				}
			}else{
				const Graph &G = currentGG.constGraph();
				node v;
				DPoint Vmouse = DPoint(event.motion.x , event.motion.y);	
				DPoint Gmouse = v2g(Vmouse);	
				IntersectionRectangle box(v2g(MouseDownCoord), Gmouse);
				forall_nodes(v,G){
					DPoint p(currentGA.x(v), currentGA.y(v));
					int pos =selNodes.search(v);
					if (box.inside(p)) addToSelection(v);
					else if (pos != -1)	selNodes.del(selNodes.get(pos));
			}			
				
			}
		}
	}
}

void GraphIO::OnWindowResized(){	
	DPoint WC1 = v2g(DPoint(winw/2, winh/2)); //Get old center of the Window in global coord
	SDL_GetWindowSize(win,&winw,&winh); //Get new Window Size 
	DPoint WC2 = v2g(DPoint(winw/2, winh/2)); //Get new center of the Window in global coord
	offset = offset - (WC2 - WC1);
	recalcGscale();					

}

bool GraphIO::handleEvents(){
	bool done = false;		
	while(SDL_PollEvent(&event)) {	
		switch(event.type){
		case SDL_KEYDOWN:
			//std::cout << "KEYDOWN" << std::endl;
			OnKeydown();						
			break;

		case SDL_KEYUP:
			//std::cout << "KEYUp" << std::endl;
			OnKeyUp();						
			break;

		case SDL_MOUSEBUTTONDOWN:
			//std::cout << "MOUSEDOWN" << std::endl;
			OnMousedown();
			break;

		case SDL_MOUSEBUTTONUP:
			//std::cout << "MOUSEUP" << std::endl;
			OnMouseup();
			break;

		case SDL_MOUSEMOTION:
			OnMouseMotion();
			break;

		case SDL_MOUSEWHEEL:
			//std::cout << "WHEEL" << std::endl;
			OnMousewheel();
			break;
		
		case SDL_WINDOWEVENT:
			//std::cout << "WINDOWRESIZE" << std::endl;
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				OnWindowResized();
			}
			break;

		case SDL_QUIT:	
			//std::cout << "BYE!" << std::endl;
			done = true;
			break;			
		}			
	}

    return done;
}

void GraphIO::drawGrid(){
	SDL_SetRenderDrawColor(rend, 0,0,0,40);
	DPoint src = v2g(DPoint(0,0));
	int x = (floor((src.m_x)/gridScale)+1)*gridScale;
	int y = (floor((src.m_y)/gridScale)+1)*gridScale;
	//std::cout << offset <<"   " << x << ":" << y << std::endl;
	for ( int i = 0; i < v2g(max(winw,winh)); i+=gridScale){
		DPoint px = g2v(DPoint(x+i,0));
		DPoint py = g2v(DPoint(0,y+i));
		SDL_RenderDrawLine(rend, px.m_x , 0, px.m_x ,winh);
		SDL_RenderDrawLine(rend, 0, py.m_y, winw, py.m_y);		
		//SDL_RenderDrawLine(rend, g2v(x+i),0,g2v(x+i),winh);
		//SDL_RenderDrawLine(rend, 0,g2v(y+i),winw,g2v(y+i));		
	}
}

bool GraphIO::think() {     
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	clrscr();
	bool quit = handleEvents();
	drawGrid();
	animate();
	
	if (key_strg){
	int x,y;
		SDL_GetMouseState(&x,&y);
		draw_circle(x, y, brushSize);
	}
	drawSelected();
	if (mouse_LB && selNode == NULL && !key_strg) {
			int x,y; 
			SDL_GetMouseState(&x,&y);
			int w = (int)abs(MouseDownCoord.m_x - x);
			int h = (int)abs(MouseDownCoord.m_y - y);
			x = (int)min((int)MouseDownCoord.m_x, x);
			y = (int)min((int)MouseDownCoord.m_y, y);
			
			SDL_Rect rect = {x,y,w,h};
			SDL_SetRenderDrawColor(rend,0,0,0,125);
			SDL_RenderDrawRect(rend, &rect);
	}
	drawGraph();
    //drawSelected();
	IPolyline outline = testLattice.outline();
	if (outline.size() > 0) drawOutline( outline,DPoint(0,0));

	SDL_RenderPresent(rend);

	return quit;
}

	
void GraphIO::drawSelected() {
	bool isEmpty = selNodes.empty();
	if (!isEmpty){		
		forall_listiterators(edge,it,selEdges){
			drawSelectedEdge(*it);
		}
		forall_listiterators(node,it,selNodes){
			drawSelectedNode(*it);
		}

	}
}

void::GraphIO::drawGG(GridGraph &GG){
	//const Graph &G = precurrentGG.constGraph(); //Get the Graph underlying GA
	node v;
	edge e;
	if (!GG.IOprep()) prepareGG(GG);
	if (GG.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to draw empty graph" << std::endl;
		throw -1;
	}
	//std::cout << "drawing " << G.numberOfNodes() << " nodes" << std::endl;

	forall_edges(e,GG) drawEdge(currentGG,e,ealpha[e]);
	forall_listiterators(node,it,GG.nonDummyNodes()){
		v = *it;
		if (GG.subGG(v)) drawGG(*GG.subGG(v));// std::cout << "Do something here" << std::endl; // call drawOutline(IPolyline outline, DPoint pos);
		else drawNode(GG,v,valpha[v]);
	}
	
	
}

void GraphIO::drawGraph() { // draws the Graph 
if (GridMode){ //second draft: display nodes of first layer with outline if node is subGG
	drawGG(currentGG);	
}else{
	GraphAttributes &GA = currentGA;
	
	const Graph &G = GA.constGraph(); //Get the Graph underlying GA
	node v;
	edge e;

	if (G.numberOfNodes() == 0){
		std::cout << "ERROR: attempting to draw empty graph" << std::endl;
		throw -1;
	}
	//std::cout << "drawing " << G.numberOfNodes() << " nodes" << std::endl;

	forall_edges(e,G) drawEdge(GA,e,ealpha[e]);
	forall_nodes(v,G) drawNode(GA,v,valpha[v]);
}	
}

void GraphIO::drawNode(GraphAttributes &GA, node v, int alpha) {
	Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color

	float w = GA.width(v); //Get Width, height and top-left point for node rect in global coordinates
	float h = GA.height(v);
	float x = GA.x(v) - (w/2);
	float y = GA.y(v) - (h/2);
	DPoint Vnode = g2v(DPoint(x,y)); //Get Viewport coordinates for topleft point

	SDL_Rect nodeRect = {Vnode.m_x, Vnode.m_y,w*zoom,h*zoom};
	SDL_SetRenderDrawColor(rend, 200,200,255,alpha);
	
	

	SDL_RenderFillRect(rend, &nodeRect);
	
	SDL_SetRenderDrawColor(rend, 0,0,0,alpha);
	SDL_RenderDrawRect(rend, &nodeRect);

	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}

void GraphIO::drawNode(GridGraph &GG, node v, int alpha) {
	Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color

	float w = nodesize; //Get Width, height and top-left point for node rect in global coordinates
	float h = nodesize;
	float x = GG.getPos().m_x + GG.dx(v) - (w/2);
	float y =  GG.getPos().m_y + GG.dy(v) - (h/2);
	DPoint Vnode = g2v(DPoint(x,y)); //Get Viewport coordinates for topleft point

	SDL_Rect nodeRect = {Vnode.m_x, Vnode.m_y,w*zoom,h*zoom};
	//SDL_SetRenderDrawColor(rend, 200,200,255,alpha);
	Uint8 r,g,b;
	r = (GG.id()*177)%255;
	g = (GG.id()*47)%255;
	b = (GG.id()*200)%255;
	SDL_SetRenderDrawColor(rend, r,g,b,alpha);

	SDL_RenderFillRect(rend, &nodeRect);

	
	SDL_SetRenderDrawColor(rend, 0,0,0,alpha);
	SDL_RenderDrawRect(rend, &nodeRect);

	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}


void GraphIO::drawSelectedNode(node v) {
	Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color
if (GridMode){
		GridGraph &GG = currentGG;
	double a = 1.7;
	float w = nodesize*a;
	float h = nodesize*a;
	float x = GG.getPos().m_x + GG.dx(v) - (w/2);
	float y = GG.getPos().m_y + GG.dy(v) - (h/2);
	
	DPoint Vnode = g2v(DPoint(x,y)); //Get Viewport coordinates for topleft point

	SDL_Rect nodeRect = {Vnode.m_x, Vnode.m_y,(w*zoom),(h*zoom)};
	
	//SDL_SetRenderDrawColor(rend,40,255,40,255);
	SDL_SetRenderDrawColor(rend,255,40,40,255);
	SDL_RenderFillRect(rend, &nodeRect);
	//for (int i = 0; i < 4; i++){
	//	--nodeRect.x;
	//	--nodeRect.y;
	//	nodeRect.w += 2;
	//	nodeRect.h += 2;
	//	SDL_RenderDrawRect(rend, &nodeRect);
	//}

}else{

	GraphAttributes &GA = currentGA;

	float w = GA.width(v); //Get Width, height and top-left point for node rect in global coordinates
	float h = GA.height(v);
	float x = GA.x(v) - (w/2);
	float y = GA.y(v) - (h/2);
	DPoint Vnode = g2v(DPoint(x,y)); //Get Viewport coordinates for topleft point

	SDL_Rect nodeRect = {Vnode.m_x, Vnode.m_y,(w*zoom),(h*zoom)};
	SDL_SetRenderDrawColor(rend,40,255,40,255);
	for (int i = 0; i < 4; i++){
		--nodeRect.x;
		--nodeRect.y;
		nodeRect.w += 2;
		nodeRect.h += 2;
		SDL_RenderDrawRect(rend, &nodeRect);
	}	
}
	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}

void GraphIO::drawEdge(GraphAttributes &GA, edge e, int alpha) {
	Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color

	SDL_SetRenderDrawColor(rend, 0,0,0,alpha);

	
	double x = GA.x(e->source());
	double y = GA.y(e->source());

	DPoint p2 = g2v(DPoint(x,y));
	DPoint p1;	

	for(int i = 0; i < GA.bends(e).size(); i++) {//go through bendpoints
		p1 = p2;
		p2 = g2v( *(GA.bends(e).get(i)) ); //get(i) returns an iterator, the * operator is needed to adress the element it points to
		SDL_RenderDrawLine(rend, p1.m_x, p1.m_y, p2.m_x, p2.m_y);	
	}
	
	p1 = p2;
	x = GA.x(e->target());
	y = GA.y(e->target());
	p2 = g2v(DPoint(x,y));
	
	SDL_RenderDrawLine(rend, p1.m_x, p1.m_y, p2.m_x, p2.m_y);			

	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}

void GraphIO::drawEdge(GridGraph &GG, edge e, int alpha) {
	if (GG.dedgeline(e).empty()){
		//std::cout << "undifined edge drawing "<< e << std::endl;
		return;
	}


	Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color

	//SDL_SetRenderDrawColor(rend, 0,0,0,alpha);

	Uint8 r,g,b;
	r = (GG.id()*177)%255;
	g = (GG.id()*47)%255;
	b = (GG.id()*200)%255;
	SDL_SetRenderDrawColor(rend, r,g,b,alpha);

	DPoint p2 = g2v(GG.dedgeline(e).front() + GG.dPos());
	DPoint p1;		
	for(ListIterator<DPoint> it = GG.dedgeline(e).begin().succ(); it.valid() ; it = it.succ()){ //go through bendpoints
		p1 = p2;
		p2 = g2v(*it + GG.dPos()); //get(i) returns an iterator, the * operator is needed to adress the element it points to
		SDL_RenderDrawLine(rend, p1.m_x, p1.m_y, p2.m_x, p2.m_y);	
	}

	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}

void GraphIO::drawOutline(IPolyline outline, DPoint pos){
		Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color
	//SDL_SetRenderDrawColor(rend,40,255,40,255);
	SDL_SetRenderDrawColor(rend,255,0,0,255);
	/*std::cout << testLattice.numberOfEdges() << std::endl;*/
	//std::cout << outline.size() << std::endl;
	
	DPoint q2 = g2v(outline.front()) + pos;
	DPoint q1;
	//std::cout << "drawing outline: " << outline.size() << std::endl;
	Uint8 inc = 255/outline.size();
	int count = 0;
	for(ListIterator<IPoint> it = outline.begin().succ(); it.valid() ; it = it.succ()){ //go through bendpoints
		count ++;
		Uint8 ldr, ldg, ldb, lda;
		SDL_GetRenderDrawColor(rend, &ldr, &ldg, &ldb, &lda); //Get current color
		SDL_SetRenderDrawColor(rend, ldr-inc, ldg, ldb+inc, lda); //Reset render color to previous value 	
		q1 = q2;
		q2 = g2v(*it) + pos;
		DPoint p1 = q1;
		DPoint p2 = q2;
		//std::cout << currentGG.dedgeline(e).front().m_x;
		double size = g2v(0.05);
		double off = g2v(0.2);
		if (p1.m_y == p2.m_y){
			if (p2.m_x > p1.m_x){ //
				p1.m_y += off;
				p2.m_y += off;
				//p1.m_x -= off;
				//p2.m_x += off;
			}else{
				p1.m_y -= off;
				p2.m_y -= off;
				//p1.m_x += off;
				//p2.m_x -= off;
			}
		}else{
			
			if (p2.m_y > p1.m_y){
				p1.m_x -= off;
				p2.m_x -= off;
				//p1.m_y -= off;
				//p2.m_y += off;
			}else{
				p1.m_x += off;
				p2.m_x += off;
				//p1.m_y += off;
				//p2.m_y -= off;
			}
		}

		if (p1.m_x < p2.m_x) {
			p1.m_x -= size;
			p2.m_x += size;
		}
		else{
			p2.m_x -= size;
			p1.m_x += size;
		}
		if (p1.m_y < p2.m_y) {
			p1.m_y -= size;
			p2.m_y += size;
		}
		else{
			p2.m_y -= size;
			p1.m_y += size;
		}



		SDL_Rect edgeRect = {p1.m_x, p1.m_y, p2.m_x-p1.m_x, p2.m_y-p1.m_y};
		/*SDL_Rect edgeRect = {20,20,200,200};*/
		SDL_RenderFillRect(rend, &edgeRect);
	}
	//std::cout << count << std::endl;
	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	

}

void GraphIO::drawSelectedEdge(edge e) {
	Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(rend, &oldr, &oldg, &oldb, &olda); //Get current color
	SDL_SetRenderDrawColor(rend,40,255,40,255);
	//SDL_SetRenderDrawColor(rend,255,40,40,255);

	if (GridMode){			
	if (currentGG.dedgeline(e).empty()) return;
	DPoint q2 = g2v(currentGG.dedgeline(e).front());
	DPoint q1;
	for(ListIterator<DPoint> it = currentGG.dedgeline(e).begin().succ(); it.valid() ; it = it.succ()){ //go through bendpoints
		q1 = q2;
		q2 = g2v(*it);
		DPoint p1 = q1;
		DPoint p2 = q2;
		//std::cout << currentGG.dedgeline(e).front().m_x;
		int size = 3;
		if (p1.m_x < p2.m_x) {
			p1.m_x -= size;
			p2.m_x += size;
		}
		else{
			p2.m_x -= size;
			p1.m_x += size;
		}
		if (p1.m_y < p2.m_y) {
			p1.m_y -= size;
			p2.m_y += size;
		}
		else{
			p2.m_y -= size;
			p1.m_y += size;
		}
		SDL_Rect edgeRect = {p1.m_x, p1.m_y, p2.m_x-p1.m_x, p2.m_y-p1.m_y};
		/*SDL_Rect edgeRect = {20,20,200,200};*/
		SDL_RenderFillRect(rend, &edgeRect);
						
		//SDL_RenderDrawLine(rend, p1.m_x, p1.m_y, p2.m_x, p2.m_y);	
		//for (int i = 1; i < 3; i++)
		//{
		//	//std::cout << "*";
		//	SDL_RenderDrawLine(rend, p1.m_x-i, p1.m_y, p2.m_x-i, p2.m_y);	
		//	SDL_RenderDrawLine(rend, p1.m_x+i, p1.m_y, p2.m_x+i, p2.m_y);	
		//	SDL_RenderDrawLine(rend, p1.m_x, p1.m_y-i, p2.m_x, p2.m_y-i);	
		//	SDL_RenderDrawLine(rend, p1.m_x, p1.m_y+i, p2.m_x, p2.m_y+i);				
		//}
		
	}
		
	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	

	}else{	
	double x = currentGA.x(e->source());
	double y = currentGA.y(e->source());
	DPoint p2 = g2v(DPoint(x,y));
	DPoint p1;

	for(int i = 0; i < currentGA.bends(e).size(); i++) {//go through bendpoints
		p1 = p2;
		p2 = g2v( *(currentGA.bends(e).get(i)) ); //get(i) returns an iterator, the * operator is needed to adress the element it points to
		
		SDL_RenderDrawLine(rend, p1.m_x, p1.m_y, p2.m_x, p2.m_y);	
		for (int i = 1; i < 3; i++){
			SDL_RenderDrawLine(rend, p1.m_x-i, p1.m_y, p2.m_x-i, p2.m_y);	
			SDL_RenderDrawLine(rend, p1.m_x+i, p1.m_y, p2.m_x+i, p2.m_y);	
			SDL_RenderDrawLine(rend, p1.m_x, p1.m_y-i, p2.m_x, p2.m_y-i);	
			SDL_RenderDrawLine(rend, p1.m_x, p1.m_y+i, p2.m_x, p2.m_y+i);				
		}
		
	}
	
	p1 = p2;
	x = currentGA.x(e->target());
	y = currentGA.y(e->target());
	p2 = g2v(DPoint(x,y));
	
	SDL_RenderDrawLine(rend, p1.m_x, p1.m_y, p2.m_x, p2.m_y);	
	for (int i = 1; i < 3; i++){
		SDL_RenderDrawLine(rend, p1.m_x-i, p1.m_y, p2.m_x-i, p2.m_y);	
		SDL_RenderDrawLine(rend, p1.m_x+i, p1.m_y, p2.m_x+i, p2.m_y);	
		SDL_RenderDrawLine(rend, p1.m_x, p1.m_y-i, p2.m_x, p2.m_y-i);	
		SDL_RenderDrawLine(rend, p1.m_x, p1.m_y+i, p2.m_x, p2.m_y+i);				
	}
	SDL_SetRenderDrawColor(rend, oldr, oldg, oldb, olda); //Reset render color to previous value 	
}
}

void GraphIO::set_pixel(int x, int y){
	SDL_RenderDrawPoint(rend,x,y);
}

void GraphIO::draw_circle(int n_cx, int n_cy, int radius)
{
    // if the first pixel in the screen is represented by (0,0) (which is in sdl)
    // remember that the beginning of the circle is not in the middle of the pixel
    // but to the left-top from it:
 
    double error = (double)-radius;
    double x = (double)radius -0.5;
    double y = (double)0.5;
    double cx = n_cx - 0.5;
    double cy = n_cy - 0.5;
 
    while (x >= y)
    {
        set_pixel((int)(cx + x), (int)(cy + y));
        set_pixel((int)(cx + y), (int)(cy + x));
 
        if (x != 0)
        {
            set_pixel((int)(cx - x), (int)(cy + y));
            set_pixel((int)(cx + y), (int)(cy - x));
        }
 
        if (y != 0)
        {
            set_pixel((int)(cx + x), (int)(cy - y));
            set_pixel((int)(cx - y), (int)(cy + x));
        }
 
        if (x != 0 && y != 0)
        {
            set_pixel((int)(cx - x), (int)(cy - y));
            set_pixel((int)(cx - y), (int)(cy - x));
        }
 
        error += y;
        ++y;
        error += y;
 
        if (error >= 0)
        {
            --x;
            error -= x;
            error -= x;
        }
    }
}

GUI::GUI(int x, int y, int w, int h, Uint32 winflags, Uint32 rendflags){
	m_x = x;
	m_y = y;
	m_w = w; 
	m_h = h;
	m_winflags = winflags;
	m_rendflags = rendflags;
	m_done = false;
	setGA_flag = false;
	setGG_flag = false;
	animateTo_flag = false;
	update_flag = false;
	pauseEvent = CreateEvent(NULL,true,true,TEXT("pause"));
	m_tempChange = 0;
}

void GUI::setGA(GraphAttributes &GA){		
	lock.enter();
	m_setGA = &GA;
	m_animateTo = NULL;
	m_GC = NULL;
	update_flag = false;
	animateTo_flag = false;
	setGA_flag = true;
	lock.leave();
}

void GUI::setGG(GridGraph &GG){	
	lock.enter();
	m_setGG = &GG;
	m_animateTo = NULL;
	m_GC = NULL;
	update_flag = false;
	animateTo_flag = false;
	setGA_flag = false;
	setGG_flag = true;
	lock.leave();

}

void GUI::animateTo(GraphAttributes &GA, unsigned int t){
	lock.enter();	
	m_animateTo = &GA;
	m_GC = NULL;
	m_t = t;
	update_flag = false;
	animateTo_flag = true;
	lock.leave();
}

void GUI::animateTo(GraphAttributes &GA, GraphCopy &GC, unsigned int t){
	lock.enter();	
	m_animateTo = &GA;
	m_GC = &GC;
	m_t = t;
	update_flag = false;
	animateTo_flag = true;
	lock.leave();
}

void GUI::update(unsigned int t){
	animateTo_flag = false;
	update_flag = true;
	m_t = t;
}

void GUI::waitForUnpause(){
	WaitForSingleObject(pauseEvent, INFINITE);
}

int GUI::getTempChange(){
	lock.enter();
	int value = m_tempChange;
	m_tempChange = 0;
	lock.leave();
	return value;
}

void GUI::doWork(){
	std::cout << "thread started" << std::endl;
	GraphIO IO(m_x,m_y,m_w,m_h,m_winflags,m_rendflags);	
	bool done = false;

	while(!done) {	
		//============Handle In===============
		lock.enter();	
		if (setGA_flag) {
			IO.setGA(*m_setGA);
			m_setGA = NULL;
			setGA_flag = false;
		}
		if (setGG_flag) {
			IO.setGG(*m_setGG);
			m_setGG = NULL;
			setGG_flag = false;
		}
		if (animateTo_flag) {
			if (m_GC) IO.animateTo(*m_animateTo, *m_GC, m_t);
			else IO.animateTo(*m_animateTo, m_t);
			
			m_animateTo = NULL;
			m_GC = NULL;
			m_t = 0;
			animateTo_flag = false;
		}
		if (update_flag){
			IO.update(m_t);
			m_t = 0;
		}
		lock.leave();
		//============Main===============
		done = IO.think();
		//============Handle Out===============
		
		lock.enter();
		
		//Set Pause state
		if (IO.pause){
			ResetEvent(pauseEvent);
		}else{
			SetEvent(pauseEvent);
		}
		//get temperature change
		m_tempChange = IO.tempChange;
		IO.tempChange  = 0;
				
		//get list of moved nodes
		if (IO.movedNodes.size() > 0){
			movedNodes.pushFront(IO.movedNodes.popFrontRet() );
			movedNodeCoord.pushFront(IO.movedNodeCoord.popFrontRet() );
		}
		lock.leave();
		
	}	
	std::cout << "thread closed" << std::endl;
	m_done = true;
}

