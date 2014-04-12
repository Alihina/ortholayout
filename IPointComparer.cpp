#include "StdAfx.h"
#include "IPointComparer.h"
#include <ogdf/basic/geometry.h>

using namespace ogdf;

bool StdComparer<IPoint>::less(const IPoint &a, const IPoint &b) 
{
			return ( (a.m_y < b.m_y) || //Vergleich der y-Koordinaten
			( (a.m_y == b.m_y) && (a.m_x < b.m_x) ) );//vergleich der x-Koordinaten, falls y-Koordinaten gleich sind
}
/*bool StdComparer<IPoint>::geq  (const IPoint &a, const IPoint &b){return false}
bool StdComparer<IPoint>::greater (const IPoint &a, const IPoint &b){return false}
bool StdComparer<IPoint>::leq (const IPoint &a, const IPoint &b){return false}*/
bool StdComparer<IPoint>::equal(const IPoint &a, const IPoint &b){
	if(a.m_x == b.m_x && a.m_y == b.m_y) return true;
	return false;
}


static void sortPolyline( IPolyline& line )
	{
		StdComparer<IPoint> comp;
		line.quicksort(comp); 	
	}
