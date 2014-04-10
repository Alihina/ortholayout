#ifndef RECTANGLE_SHAPE
#define RECTANGLE_SHAPE
#include <Grid_EnergyFunction.h>

namespace ogdf {

	class RectangleShape: public Grid_EnergyFunction {
	public:
		RectangleShape(const String &funcname, GridGraph &GG);
		//! computes energy for the layout at the beginning of the optimization process
		void computeEnergy();

	protected:

		//! changes the data of a specific energy function if the candidate was taken
		void internalCandidateTaken();

		//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
		void compCandEnergy();//will be continued if there is enough time left
	private: 
		IPolyline m_Outline;
	};

}// namespace ogdf

#endif
