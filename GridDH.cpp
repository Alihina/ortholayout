
#include <GridDH.h>
#include <ogdf/basic/Math.h>
#include <time.h>

//TODO: in addition to the layout size, node sizes should be used in
//the initial radius computation in case of "all central" layouts with
//huge nodes
//the combinations for parameters should be checked: its only useful
//to have a slow shrinking if you have enough time to shrink down to
//small radius

namespace ogdf {

	const int GridDH::m_defaultTemp = 1000;
	const double GridDH::m_defaultRadius = 100.0; //this seems way too high? nevermind, it isn't actually used
	const int GridDH::m_iterationMultiplier = 25;  //best//30;ori
	const double GridDH::m_coolingFactor = 0.80;  //0.75;ori
	const double GridDH::m_shrinkFactor = 0.8;

	//initializes internal data and the random number generator
	GridDH::GridDH():
	m_temperature(m_defaultTemp),
	m_shrinkingFactor(m_shrinkFactor),
	m_diskRadius(m_defaultRadius),
	m_energy(0.0),
	m_numberOfIterations(0)
	{
		srand((unsigned)time(NULL));
	}


	//allow resetting in between subsequent calls
	void GridDH::initParameters()
	{
		m_diskRadius = m_defaultRadius;
		m_energy = 0.0;
		//m_numberOfIterations = 0; //is set in member function
		m_shrinkingFactor = m_shrinkFactor;

	}


	void GridDH::setStartTemperature(int startTemp)
	{
		OGDF_ASSERT(startTemp >= 0);
		m_temperature=startTemp;
	}

	void GridDH::setNumberOfIterations(int steps)
	{
		OGDF_ASSERT(steps >= 0);
		m_numberOfIterations = steps;
	}

	//whenever an energy function is added, the initial energy of the new function
	//is computed and added to the initial energy of the layout
	void GridDH::addEnergyFunction(Grid_EnergyFunction *F, double weight)
	{
		m_energyFunctions.pushBack(F);
		OGDF_ASSERT(weight >= 0);  //um, really?
		m_weightsOfEnergyFunctions.pushBack(weight);
		F->computeEnergy();
		m_energy += F->energy();
	}

	List<String> GridDH::returnEnergyFunctionNames()
	{
		List<String> names;
		ListIterator<Grid_EnergyFunction*> it;
		for(it = m_energyFunctions.begin(); it.valid(); it = it.succ())
			names.pushBack((*it)->getName());
		return names;
	}

	List<double> GridDH::returnEnergyFunctionWeights()
	{
		List<double> weights;
		ListIterator<double> it;
		for(it = m_weightsOfEnergyFunctions.begin(); it.valid(); it = it.succ())
			weights.pushBack(*it);
		return weights;
	}

	//newVal is the energy value of a candidate layout. It is accepted if it is lower
	//than the previous energy of the layout or if m_fineTune is not tpFine and
	//the difference to the old energy divided by the temperature is smaller than a
	//random number between zero and one
	bool GridDH::testEnergyValue(double newVal)
	{
		bool accepted = true;
		if(newVal > m_energy) {
			accepted = false;

			double testval = exp((m_energy-newVal)/ m_temperature);
			double compareVal = randNum(); // number between 0 and 1

			if(compareVal < testval)
				accepted = true;

		}
		return accepted;
	}

	//divides number returned by rand by RAND_MAX to get number between zero and one
	inline double GridDH::randNum() const
	{
		double val = rand();
		val /= RAND_MAX;
		return val;
	}

	//chooses random vertex and a new random position for it on a circle with radius m_diskRadius
	//around its previous position 
	//returned value is the node that should be moved,
	//newPos is the place it is moved to.
	//TODO: this function retries fiddling things until it finds a change that doesn't collide.
	node GridDH::computeCandidateLayout(
	const GridGraph &GG,
	IPoint &newPos) const
	{
		while(true) {//no way this can go wrong, maybe add a counter? or just continue in main algo?
//int randomPos = randomNumber(0,m_nonIsolatedNodes.size()-1);
			node v = GG.getNonDummyNode();
				
//*(m_nonIsolatedNodes.get(randomPos)); //getting that node and calculating a new point for it
			double oldx = GG.x(v); //TODO: this needs to be reformatted that it works
			double oldy = GG.y(v);
			double randomAngle = randNum() * 2.0 * Math::pi;
			newPos.m_y = int(oldy+sin(randomAngle)*m_diskRadius+0.5); 
			newPos.m_x = int(oldx+cos(randomAngle)*m_diskRadius+0.5); 
			//TODO: randomly rotate or flip the vertex if it is not trivial
#ifdef OGDF_DEBUG
			double dist = sqrt((newPos.m_x - oldx)*(newPos.m_x - oldx)+(newPos.m_y-oldy)*(newPos.m_y-oldy));
			OGDF_ASSERT(dist > 0.99 * m_diskRadius && dist < 1.01 * m_diskRadius);
#endif
			if ("we don't collide") { //TODO THIS
				return v;
			}
		}
	}

	//chooses the initial radius of the disk as half the maximum of width and height of
	//the initial layout or depending on the value of m_fineTune
	void GridDH::computeFirstRadius(const GridGraph &GG)  //need to redo this
	{
		const Graph &G = GG.constGraph();
		node v = G.firstNode();
		double minX = GG.x(v);
		double minY = GG.y(v);
		double maxX = minX;
		double maxY = minY;
		forall_nodes(v,G) {
			minX = min(minX,GG.x(v));
			maxX = max(maxX,GG.x(v));
			minY = min(minY,GG.y(v));
			maxY = max(maxY,GG.y(v));
		}
		// compute bounding box of current layout
		// make values nonzero
		double w = maxX-minX+1.0;
		double h = maxY-minY+1.0;

		double ratio = h/w;

		double W = sqrt(G.numberOfNodes() / ratio);

		m_diskRadius = W / 5.0;//allow to move by a significant part of current layout size
		m_diskRadius=max(m_diskRadius,max(maxX-minX,maxY-minY)/5.0);

		//TODO: also use nodeg sizes
		/*
		double lengthSum(0.0);
		node v;
		forall_nodes(v,m_G) {
			const IntersectionRectangle &i = shape(v);
			lengthSum += i.width();
			lengthSum += i.width();
			}
			lengthSum /= (2*m_G.numberOfNodes());
			// lengthSum is now the average of all lengths and widths
		*/
		//change the initial radius depending on the settings
		//this is legacy crap
//		double divo = 2.0;
//		if (m_fineTune == tpCoarse) {
//			m_diskRadius = 1000.0;
//			divo = 0.5;
//		}
//		if (m_fineTune == tpFine) {
//			m_diskRadius = 10.0;
//            divo = 15.0;
//		}
//		//m_diskRadius=max(m_diskRadius,max(maxX-minX,maxY-minY));
//		m_diskRadius = max(maxX-minX,maxY-minY);
//		m_diskRadius /= divo;
	}

	//steps through all energy functions and adds the initial energy computed by each
	//function for the start layout //but the energy already gets added when the function is added?
	void GridDH::computeInitialEnergy()
	{
		OGDF_ASSERT(!m_energyFunctions.empty());
		ListIterator<Grid_EnergyFunction*> it;
		ListIterator<double> it2;
		it2 = m_weightsOfEnergyFunctions.begin();
		for(it = m_energyFunctions.begin(); it.valid() && it2.valid(); it=it.succ(), it2 = it2.succ())
			m_energy += (*it)->energy() * (*it2);
	}

	/*/the vertices with degree zero are placed below all other vertices on a horizontal
	// line centered with repect to the rest of the drawing  // we shouldn't need this.
	void GridDH::placeIsolatedNodes(GridGraph &GG) const {
		double minX = 0.0;
		double minY = 0.0;
		double maxX = 0.0;
		double maxY = 0.0;

		if(!m_nonIsolatedNodes.empty()) {
			//compute a rectangle that includes all non-isolated vertices
			node v = m_nonIsolatedNodes.front();
			minX = GG.x(v);
			minY = GG.y(v);
			maxX = minX;
			maxY = minY;
			ListConstIterator<node> it;
			for(it = m_nonIsolatedNodes.begin(); it.valid(); ++it) {
				v = *it;
				double xVal = GG.x(v);
				double yVal = GG.y(v);
				double halfHeight = AG.height(v) / 2.0;
				double halfWidth = AG.width(v) / 2.0;
				if(xVal - halfWidth < minX) minX = xVal - halfWidth;
				if(xVal + halfWidth > maxX) maxX = xVal + halfWidth;
				if(yVal - halfHeight < minY) minY = yVal - halfHeight;
				if(yVal + halfHeight > maxY) maxY = yVal + halfHeight;
			}
		}

		// compute the width and height of the largest isolated node
		List<node> isolated;
		node v;
		const Graph &G = AG.constGraph();
		double maxWidth = 0;
		double maxHeight = 0;
		forall_nodes(v,G) if(v->degree() == 0) {
			isolated.pushBack(v);
			if(AG.height(v) > maxHeight) maxHeight = AG.height(v);
			if(AG.width(v) > maxWidth) maxWidth = AG.width(v);
		}
		// The nodes are placed on a line in the middle under the non isolated vertices.
		// Each node gets a box sized 2 maxWidth.
		double boxWidth = 2.0*maxWidth;
		double commonYCoord = minY-(1.5*maxHeight);
		double XCenterOfDrawing = minX + ((maxX-minX)/2.0);
		double startXCoord = XCenterOfDrawing - 0.5*(isolated.size()*boxWidth);
		ListIterator<node> it;
		double xcoord = startXCoord;
		for(it = isolated.begin(); it.valid(); ++it) {
			v = *it;
			AG.x(v) = xcoord;
			AG.y(v) = commonYCoord;
			xcoord += boxWidth;
		}
	} //*/

	//this is the main optimization routine with the loop that lowers the temperature
	//and the disk radius geometrically until the temperature is zero. For each
	//temperature, a certain number of new positions for a random vertex are tried
	void GridDH::call(GridGraph &GG)
	{
		initParameters();

		m_shrinkingFactor = m_shrinkFactor; //why? that is done in initParameters!

		OGDF_ASSERT(!m_energyFunctions.empty());

		const Graph &G = GG.constGraph();
		//compute the list of vertices with degree greater than zero
		G.allNodes(m_nonIsolatedNodes); //this might as well be all nodes, our data set is connected
		ListIterator<node> it,itSucc;
		for(it = m_nonIsolatedNodes.begin(); it.valid(); it = itSucc) {
			itSucc = it.succ();
			if((*it)->degree() == 0) m_nonIsolatedNodes.del(it);
		}
		if(G.numberOfEdges() > 0) { //else only isolated nodes
			computeFirstRadius(GG);
			computeInitialEnergy();
			if(m_numberOfIterations == 0)
				m_numberOfIterations = m_nonIsolatedNodes.size() * m_iterationMultiplier;
			//this is the main optimization loop
			while(m_temperature > 0) {
				//iteration loop for each temperature
				for(int ic = 1; ic <= m_numberOfIterations; ic ++) {
					IPoint newPos;
					//choose random vertex and new position for vertex.
					node v = computeCandidateLayout(GG,newPos);
					//TODO here we tell the graph that the old vert and its edges are hidden and that the new ones are considered
					//compute candidate energy and decide if new layout is chosen
					ListIterator<Grid_EnergyFunction*> it;
					ListIterator<double> it2 = m_weightsOfEnergyFunctions.begin();
					double newEnergy = 0.0;
					for(it = m_energyFunctions.begin(); it.valid(); it = it.succ()) {
						newEnergy += (*it)->computeCandidateEnergy(v,newPos) * (*it2);
						it2 = it2.succ();
					}
					OGDF_ASSERT(newEnergy >= 0.0);
					//this tests if the new layout is accepted. If this is the case,
					//all energy functions are informed that the new layout is accepted
					if(testEnergyValue(newEnergy)) {
						for(it = m_energyFunctions.begin(); it.valid(); it = it.succ())
							(*it)->candidateTaken();
						//GG.x(v) = newPos.m_x;
						//GG.y(v) = newPos.m_y;
						m_energy = newEnergy;
						GG.acceptPos();
					}
					else {
						GG.rejectPos();
					}
				}
				//lower the temperature and decrease the disk radius
				m_temperature = (int)floor(m_temperature*m_coolingFactor);
				m_diskRadius *= m_shrinkingFactor;  
				//make sure the radius is at least 1 because rounding.
				if (m_diskRadius <= 1){
					m_diskRadius = 1;
				}

			}
		}
//if there are zero degree vertices, they are placed using placeIsolatedNodes //not gonna happen
//if(m_nonIsolatedNodes.size() != G.numberOfNodes())
placeIsolatedNodes(GG);

	}
} //namespace