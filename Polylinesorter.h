#ifndef IPOINT_COMPARER_H
#define IPOINT_COMPARER_H

#include <ogdf/basic/comparer.h>
#include <ogdf/basic/geometry.h>
#include <algorithm>

namespace ogdf{

	bool comparePoints( IPoint& a, IPoint& b)
	{
		return ( (a.m_y < b.m_y) || //Vergleich der y-Koordinaten
			( (a.m_y == b.m_y) && (a.m_x < b.m_x) ) );//vergleich der x-Koordinaten, falls y-Koordinaten gleich sind
	}
	inline void sortPolyline( Polyline& line )
	{
		ListIterator<IPoint> begin,end;
		begin = line.begin();
		end = line.end();
		std::sort(begin,end,comparePoints);
	}
}
#endif 