#include "stdafx.h"
#include "NodeDensity.h"


using namespace ogdf;


//BoundingBoxSize::BoundingBoxSize(const String &funcname, GridGraph &GG){m_BoundingBox = IPoint(INT_MAX,INT_MAX) ;}
/**/ //INT_MIN std::

//! computes energy for the layout at the beginning of the optimization process
void NodeDensity::computeEnergy()
{
	m_energy = NodeDensity::OutlineArea(m_GG.getBox(),m_GG.getOutline())/m_GG.numberOfNodes();
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void NodeDensity::compCandEnergy()
{
	m_candidateEnergy=NodeDensity::OutlineArea(m_GG.getBox(),m_GG.getOutline())/m_GG.numberOfNodes();;
}

void NodeDensity::internalCandidateTaken() {
	;
}

