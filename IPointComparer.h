#ifndef IPOINT_COMPARER_H
#define IPOINT_COMPARER_H

#include <ogdf/basic/comparer.h>
#include <ogdf/basic/geometry.h>
#include <algorithm>

namespace ogdf{

	template<> class StdComparer<IPolyline> {
	public:
		static IPolyline less (const IPolyline &a, const IPolyline &b);
	};

}
#endif