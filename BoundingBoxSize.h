#ifndef Bounding_BOX_SIZE
#define Bounding_BOX_SIZE

#include <Grid_EnergyFunction.h>
#include <ogdf/basic/geometry.h>

namespace ogdf {

	class BoundingBoxSize: public Grid_EnergyFunction {
	public:
		BoundingBoxSize(const String &funcname, GridGraph &GG);
		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy();
		static double calcBoxArea(IPolyline &Box);
	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken();

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy();

	};

}// namespace ogdf

#endif
