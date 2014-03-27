#include "stdafx.h"

#include <OLDGrid.h>

Grid::Grid(){};
IPolyline Grid::findEdge(IPoint A, IPoint B){return IPolyline();};  
bool Grid::isFree(IPoint A){return false;}; 
bool Grid::isFree(IPolyline outline){return false;}; 
//List<Gridpoint> pointsIn(IPolyline outline){};