#include "stdafx.h"
#include "RectangleShape.h"
#include "BoundingBoxSize.h"
#include "GridGraph.h"

using namespace ogdf;

RectangleShape::RectangleShape(const String &funcname, GridGraph &GG):
Grid_EnergyFunction(funcname,GG)
{
	m_Outline=m_GG.getOutline();
}

//! computes energy for the layout at the beginning of the optimization process
void RectangleShape::computeEnergy()
{
	m_energy = BoundingBoxSize::calcBoxArea(m_GG.getBox())-GridGraph::outlineArea(m_GG.getOutline());
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void RectangleShape::compCandEnergy()
{
	if (m_GG.getOutline()!=m_Outline)
	{
		m_candidateEnergy=BoundingBoxSize::calcBoxArea(m_GG.getBox())-GridGraph::outlineArea(m_GG.getOutline());
	}

}

void RectangleShape::internalCandidateTaken()
{
	m_Outline=m_GG.getOutline();;
}


