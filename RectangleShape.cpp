#include "stdafx.h"
#include <Rectangleshape.h>


using namespace ogdf;


//BondingBoxSize::BondingBoxSize(const String &funcname, GridGraph &GG){m_bondingBox = IPoint(INT_MAX,INT_MAX) ;}
/**/ //INT_MIN std::

//! computes energy for the layout at the beginning of the optimization process
void Rectangleshape::computeEnergy()
{m_energy = Rectangleshape::OutlineArea(m_GG.getBox(),m_GG.getOutline())-m_GG.numberOfNodes();
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void Rectangleshape::compCandEnergy()
{m_candidateEnergy=m_GG.boxarea();};

void Rectangleshape::internalCandidateTaken() {;}

