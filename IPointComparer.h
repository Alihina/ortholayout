#ifndef IPOINT_COMPARER_H
#define IPOINT_COMPARER_H

#include <ogdf/basic/comparer.h>
#include <ogdf/basic/geometry.h>
#include <algorithm>

namespace ogdf{

	template<> class StdComparer<IPoint> {
	public:
		static bool less (const IPoint &a, const IPoint &b);
		static bool equal  (const IPoint &a, const IPoint &b);
		/*static IPoint geq  (const IPoint &a, const IPoint &b);
		static IPoint greater (const IPoint &a, const IPoint &b);
		static IPoint leq (const IPoint &a, const IPoint &b);*/
		
	};

}
#endif