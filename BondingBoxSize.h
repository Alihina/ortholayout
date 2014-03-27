#ifndef BONDING_BOX_SIZE
#define BONDING_BOX_SIZE

#include <Grid_EnergyFunction.h>
#include <ogdf/basic/geometry.h>

namespace ogdf {

	class BondingBoxSize: public Grid_EnergyFunction {
	public:
		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy()=0;
		static double calcBoxArea(IPolyline &Box);
	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken() = 0;

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy()=0;

	};

}// namespace ogdf

#endif
