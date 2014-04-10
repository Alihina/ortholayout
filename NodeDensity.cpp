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
{   if (m_GG.nonDummyNodes().size()==0)
	{m_energy = 0;}
    else 
	{
		m_energy = GridGraph::outlineArea(m_GG.getOutline())/m_GG.nonDummyNodes().size();
		//m_energy = GridGraph::outlineArea(m_GG.getOutline())/m_GG.numberOfNodes();
	}
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void NodeDensity::compCandEnergy()
{  
	if (m_GG.getOutline()!=m_Outline)
	{
		m_candidateEnergy=GridGraph::outlineArea(m_GG.getOutline())/m_GG.nonDummyNodes().size();
	}
}

void NodeDensity::internalCandidateTaken() 
{
	m_Outline=m_GG.getOutline();
}

