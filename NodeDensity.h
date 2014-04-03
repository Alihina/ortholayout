#ifndef NODE_DENSITY
#define NODE_DENSITY

#include "Grid_EnergyFunction.h"
#include <ogdf/basic/geometry.h>

namespace ogdf {
	int outlineArea(IPolyline Boundingbox, IPolyline Outline);
	class NodeDensity: public Grid_EnergyFunction {
	public:
		NodeDensity(const String &funcname, GridGraph &GG);

		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy()=0;
		
	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken() = 0;

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy()=0;//will be continued if there is enough time left

	private: 

		IPolyline m_Outline;

	};

}// namespace ogdf

#endif
