#include "stdafx.h"
#include "RectangleShape.h"
#include "BoundingBoxSize.h"
#include "GridGraph.h"

using namespace ogdf;



//! computes energy for the layout at the beginning of the optimization process
void RectangleShape::computeEnergy()
{
	m_energy = BoundingBoxSize::calcBoxArea(m_GG.getBox())-GridGraph::outlineArea(m_GG.getBox(), m_GG.getOutline());
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void RectangleShape::compCandEnergy()
{
	IPolyline Box;      
    Box = m_GG.getBox();
	m_candidateEnergy=BoundingBoxSize::calcBoxArea(Box);
}

void RectangleShape::internalCandidateTaken()
{
	;
}


