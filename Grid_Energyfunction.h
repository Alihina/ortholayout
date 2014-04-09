#ifndef GRID_ENERGYFUNCTION_H
#define GRID_ENERGYFUNCTION_H

#include <GridGraph.h>
#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/String.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>

 //Grid version of the energyfunction class from Davidson Harel

/*
class grid energyfunctions derived from Energyfunction
possible functions:
Bounding Box size
area
average nodedensity
combined edge length
*/
/////////////////////////////////////////////////////How to use Grind_EnergyFunction//////////////////////////////////////////////
/* At first DavidsonHarel calls "computeEnergy()", this computes the initial energy of the layout.
Then DavidsonHarel hides one vertex and its incident edges. Then DavidsonHarel 
chooses a new position for this vertex and computes the positions of the incident edges. This changed vertex and its edges are saved as 'considered'.
Then DavidsonHarel calls "computecandidateEnergy()", this makes it that the varable "m_energy" is still the ernergy of the old configuration with the hidden vertex and 
the variable "m_candidateEnergy" is the energy of the new configuration with the considered vertex and its incident edges. 
Now DavidsonHarel calls "get_canditeteEnergy()" to decide if we want to change the old configuration to the new configuration. 
If we want the new configuration, DavidsonHarol calls "candidateTaken", that 
changes "m_energy" to "m_candidateEnergy" and sets the value of "m_candidateEnergy" to zero. Otherwise "m_energy" doesent change. 
*/

namespace ogdf {

class Grid_EnergyFunction {
public:
	//! Initializes data structures to speed up later computations
	Grid_EnergyFunction(const String &funcname, GridGraph &GG);
	virtual ~Grid_EnergyFunction() { }

	//! computes energy for the layout at the beginning of the optimization process
	virtual void computeEnergy()=0;

	//! sets m_testNode and computes the energy for the new configuration.
	//thereby m_testNode is the considered testnode of the DavidsonHarel-Algorithm. 
	//For computing the new energy we only must care about the considered m_testNode and his adjacent considered edges. 
	double computeCandidateEnergy(const node v);

	//! prints the name of the energy function
	String getName() const {return m_name;}

	//Sets m_energy to m_candidateEnergy and call internalCandidateTaken. so each energyfunction can also change owen variables.
	void candidateTaken();

	double get_energy(){return m_energy;};
	double get_candidateEnergy(){return m_candidateEnergy;};

#ifdef OGDF_DEBUG
	//! prints status information for debugging
	void printStatus() const;
#endif
	double energy() const {return m_energy;}
protected:
	GridGraph& m_GG;	//!< This stores the graph with its graphical attributes and the current positions for the vertices

	const String m_name;//!< name of the energy function

	double m_candidateEnergy;//!< the energy of the layout with the considered vertex and his considered incident edges

	double m_energy;//!< energy of the layout with the old hidden vertex an his hidden incident edges

	//! returns the current position of vertex v
	IPoint currentPos(const node v) const {return m_GG.getPos(v);}
	//IPoint currentPos(const node v) const; {return m_GG.getPos(node v);}

	//! returns the vertex that is under consideration in the current step
	node testNode() const {return m_testNode;}

	//! changes the data of a specific energy function if the candidate was taken
	virtual void internalCandidateTaken() = 0;

	//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
	virtual void compCandEnergy()=0;

#ifdef OGDF_DEBUG
	virtual void printInternalData() const = 0;
#endif
private:
	//! the copy constructor is fake and can not be used.
	Grid_EnergyFunction(const Grid_EnergyFunction &e):m_GG(e.m_GG),m_name(e.m_name) { } 
	//! the assignment operator is fake and can not be used.
	Grid_EnergyFunction& operator=(const Grid_EnergyFunction &e);
	node m_testNode;//!< The node that changed position in the candidate
};

}// end namespace
#endif



