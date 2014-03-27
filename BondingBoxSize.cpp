#include "stdafx.h"
#include <ogdf/basic/geometry.h>
#include <BondingBoxSize.h>


using namespace ogdf;


//BondingBoxSize::BondingBoxSize(const String &funcname, GridGraph &GG){m_bondingBox = IPoint(INT_MAX,INT_MAX) ;}
/**/ //INT_MIN std::

//! computes energy for the layout at the beginning of the optimization process
void BondingBoxSize::computeEnergy()
{
	IPolyline Box;      
    Box = m_GG.getBox();
	m_energy = BondingBoxSize::calcBoxArea(Box);//Fläche der Box
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void BondingBoxSize::compCandEnergy()
{
	IPolyline Box;      
    Box = m_GG.getBox();
	m_candidateEnergy = BondingBoxSize::calcBoxArea(Box);
}

void BondingBoxSize::internalCandidateTaken() 
{
	;
}

double BondingBoxSize::calcBoxArea(IPolyline &Box)
{
	return (abs(Box.back().m_x-Box.front().m_x)+1)
		*(abs(Box.back().m_y-Box.front().m_y)+1);
}
