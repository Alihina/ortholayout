#include "stdafx.h"
#include <ogdf/basic/geometry.h>
#include <ogdf/basic/List.h>
#include <CombinedEdgeLength.h>
#include <ogdf/basic/Graph.h>


using namespace ogdf;

//! computes energy for the layout at the beginning of the optimization process

int CombinedEdgeLength::calcEdgeLength(edge e)
{ //the length of an edge is sum of the eukledian length of the edgesegments.
	int result;
	result=0;
	IPolyline line;
	line = m_GG.edgeline(e); //edgeline gibt Referenz der edgeline zurück
	IPoint previousbend;
	previousbend = line.front();
	for(ListConstIterator< IPoint > it = (line).begin(); it.valid(); ++it)
	{	//in result is saved the length of the segments until the bend in *it
		result=result+abs((*it).m_x-previousbend.m_x)+abs((*it).m_y-previousbend.m_y);
		previousbend=*it;
	}
	return result;
}


void CombinedEdgeLength::computeEnergy()
{
	edge currentedge ;
	forall_edges(currentedge, m_GG)
	{
		if ((m_GG.isVisible(currentedge)==true)||(m_GG.isTemporary(currentedge)==true))
		{
			m_energy=m_energy+calcEdgeLength(currentedge);
		}

	}
}


//! computes the energy of the configuration with the considered testvertex and sets the value of m_candidateEnergy.
void CombinedEdgeLength::compCandEnergy()
{
	edge e;
	int oldedges=0;
	forall_adj_edges(e, m_GG.getHiddenNode())
	{
		oldedges=oldedges+calcEdgeLength(e);
	}
	int newedges=0;
	forall_adj_edges(e,testNode())
	{
		newedges=newedges+calcEdgeLength(e);
	}

	m_candidateEnergy=m_energy-oldedges+newedges;
}

void CombinedEdgeLength::internalCandidateTaken() 
{
	;
}

