#ifndef COMBINED_EDGE_LENGTH
#define COMBINED_EDGE_LENGTH

#include <Grid_EnergyFunction.h>
#include <ogdf/basic/Graph.h>

namespace ogdf {

	class CombinedEdgeLength: public Grid_EnergyFunction {
	public:
		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy()=0;
	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken() = 0;

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy()=0;
	private:
		int calcEdgeLength(edge e);    

	};

}// namespace ogdf

#endif
