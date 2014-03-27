#ifndef NODE_DENSITY
#define NODE_DENSITY

#include "Grid_EnergyFunction.h"

namespace ogdf {

	class NodeDensity: public Grid_EnergyFunction {
	public:
		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy()=0;

	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken() = 0;

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy()=0;//will be continued if there is enough time left

	private: 
		int OutlineArea(IPolyline Boundingbox,IPolyline Outline);//this function implements Larisa

	};

}// namespace ogdf

#endif
