#ifndef COMBINED_EDGE_LENGTH
#define COMBINED_EDGE_LENGTH

#include <Grid_EnergyFunction.h>
#include <ogdf/basic/Graph.h>

namespace ogdf {

	class CombinedEdgeLength: public Grid_EnergyFunction {
	public:
		CombinedEdgeLength(const String &funcname, GridGraph &GG);
		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy();
	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken();

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy();
	private:
		int calcEdgeLength(edge e);    

	};

}// namespace ogdf

#endif
