// grid version of DavidsonHarel to work on GridGraphs

//changes of Variable types
// GraphAttributes -> GridGraph
// Energyfunction -> Grid_Energyfunction

#ifdef _MSC_VER
#pragma once
#endif

#ifndef GRIDDH_H
#define GRIDDH_H


#include <ogdf/internal/energybased/EnergyFunction.h>
#include <GridGraph.h>

namespace ogdf {


//! The Davidson-Harel approach for drawing graphs; modified to work on a GridGraph
class  GridDH
{
public:

	//! Creates an instance of Davidsen-Harel base class.
	GridDH();

	~GridDH() { }

	//! Sets the start temperature to \a startTemp.
	void setStartTemperature(int startTemp);

	//! Sets the number of iterations for each temperature step to \a steps.
	void setNumberOfIterations(int steps);

	//! Adds an energy function \a F with a certain weight.
	void addEnergyFunction(Grid_EnergyFunction *F, double weight);

	//! Returns a list of the names of the energy functions.
	List<String> returnEnergyFunctionNames();

	//! Returns a list of the weights of the energy functions.
	List<double> returnEnergyFunctionWeights();

	//! Calls the Davidson-Harel method for graph \a GA.
	//this will need to be changed..
	void call(GridGraph &G);

private:
	//! The default starting temperature.
	const static int m_defaultTemp;
	//! The default starting radius.
	const static double m_defaultRadius;
	//! Per default, the number of iterations per temperature are set as a constant multiple of the number of vertices.
	const static int m_iterationMultiplier;
	//! The fraction by which the temperature is lowered after a temperature step is finished.
	const static double m_coolingFactor;
	//! the constant by which the radius of the circle around each vertex is shrunk when the temperature is lowered
	const static double m_shrinkFactor;

	int m_temperature;          //!< The temperature during the annealing process.
	double m_shrinkingFactor;   //!< The factor for radius.
	double m_diskRadius;        //!< The radius of the disk around the old position of a vertex where the new position will be.
	double m_energy;            //!< The current energy of the system.
	int m_numberOfIterations;   //!< The number of iterations per temperature step.

	List<Grid_EnergyFunction*> m_energyFunctions; //!< The list of the energy functions.
	List<double> m_weightsOfEnergyFunctions; //!< The list of the weights for the energy functions.

//List<node> m_nonIsolatedNodes; //!< The list of nodes with degree greater 0. //not using, all are

	//! Resets the parameters for subsequent runs.
	void initParameters();

	//! Randomly computes a node and a new position for that node.
	node computeCandidateLayout(const GridGraph &GG, IPoint &newPos) const;

	//! Tests if new energy value satisfies annealing property (only better if m_fineTune).
	bool testEnergyValue(double newVal);

	//! Computes a random number between zero and one
	double randNum() const;

	//! Computes the first disk radius as the half the diamter of the enclosing rectangle.
	void computeFirstRadius(const GridGraph &GG);

	//! Computes the energy of the initial layout and stores it in \a m_energy.
	void computeInitialEnergy();

	//! Computes positions for the vertices of degree zero.
	void placeIsolatedNodes(GridGraph &GG) const;

	//! Fake assignment operator (dummy to avoid copying)
	GridDH& operator=(const GridDH &dh);
	//! Fake copy constructor (dummy to avoid copying)
	GridDH(const GridDH &) { }
};

} //end namespace
#endif
