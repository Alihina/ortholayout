#include "StdAfx.h"
#include "IPointComparer.h"
#include <ogdf/basic/geometry.h>

using namespace ogdf;

bool StdComparer<IPoint>::less(const IPoint &a, const IPoint &b) 
{
			return ( (a.m_y < b.m_y) || //Vergleich der y-Koordinaten
			( (a.m_y == b.m_y) && (a.m_x < b.m_x) ) );//vergleich der x-Koordinaten, falls y-Koordinaten gleich sind
}


