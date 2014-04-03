#include "stdafx.h"
#include "NodeDensity.h"
#include <ogdf/basic/geometry.h>

using namespace ogdf;


NodeDensity::NodeDensity(const String &funcname, GridGraph &GG):
Grid_EnergyFunction(funcname,GG)
{
	m_Outline=m_GG.getOutline();
}


//! computes energy for the layout at the beginning of the optimization process
void NodeDensity::computeEnergy()
{  
	m_energy = NodeDensity::OutlineArea(m_GG.getBox(),m_GG.getOutline())/m_GG.numberOfNodes();
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void NodeDensity::compCandEnergy()
{  
	if (m_GG.getOutline()!=m_Outline)
	{
		m_candidateEnergy=NodeDensity::OutlineArea(m_GG.getBox(),m_GG.getOutline())/m_GG.numberOfNodes();
	}
}

void NodeDensity::internalCandidateTaken() 
{
	m_Outline=m_GG.getOutline();
}

