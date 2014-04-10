#include "stdafx.h"
#include <ogdf/basic/geometry.h>
#include <BoundingBoxSize.h>


using namespace ogdf;


//BoundingBoxSize::BoundingBoxSize(const String &funcname, GridGraph &GG){m_BoundingBox = IPoint(INT_MAX,INT_MAX) ;}
/**/ //INT_MIN std::

//! computes energy for the layout at the beginning of the optimization process
BoundingBoxSize::BoundingBoxSize(const String &funcname, GridGraph &GG):
Grid_EnergyFunction(funcname,GG)
{
}

void BoundingBoxSize::computeEnergy()
{
	IPolyline Box;      
    Box = m_GG.getBox();
	m_energy = BoundingBoxSize::calcBoxArea(Box);//Fläche der Box
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void BoundingBoxSize::compCandEnergy()
{
	IPolyline Box;      
    Box = m_GG.getBox();
	m_candidateEnergy = BoundingBoxSize::calcBoxArea(Box);
}

void BoundingBoxSize::internalCandidateTaken() 
{
	;
}

double BoundingBoxSize::calcBoxArea(IPolyline &Box)
{
	return (abs(Box.back().m_x-Box.front().m_x)+1)
		*(abs(Box.back().m_y-Box.front().m_y)+1);
}
