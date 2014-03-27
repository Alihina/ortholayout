#include "stdafx.h"
#include "RectangleShape.h"
#include "BoundingBoxSize.h"

using namespace ogdf;


//BoundingBoxSize::BoundingBoxSize(const String &funcname, GridGraph &GG){m_BoundingBox = IPoint(INT_MAX,INT_MAX) ;}
/**/ //INT_MIN std::

//! computes energy for the layout at the beginning of the optimization process
void Rectangleshape::computeEnergy()
{
	m_energy = Rectangleshape::OutlineArea(m_GG.getBox(),m_GG.getOutline())-m_GG.numberOfNodes();
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void Rectangleshape::compCandEnergy()
{
	IPolyline Box;      
    Box = m_GG.getBox();
	m_candidateEnergy=BoundingBoxSize::calcBoxArea(Box);
}

void Rectangleshape::internalCandidateTaken()
{
	;
}


