#include "stdafx.h"
#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/geometry.h>
#include <OGDF/basic/Thread.h>
#include <Grid_EnergyFunction.h>
#include <GridGraph.h>




using namespace ogdf;


//class Grid_Energyfunction; //Grid version of the energyfunction class from Davidson Harel

/*
class grid energyfunctions derived from Energyfunction
possible functions:
Bounding Box size
area
average nodedensity
combined edge length

*/


Grid_EnergyFunction::Grid_EnergyFunction(const String &funcname, GridGraph &GG) :
	m_name(funcname),
	m_candidateEnergy(0),
	m_energy(0),
	m_GG(GG),
	m_testNode(NULL){ }


void Grid_EnergyFunction::candidateTaken()
{
	m_energy=m_candidateEnergy;
	m_candidateEnergy = 0.0;
	internalCandidateTaken();
	m_testNode=NULL;
}


	//! sets m_testNode and computes the energy for the new configuration
double Grid_EnergyFunction::computeCandidateEnergy(const node v)
{
	m_testNode = v;
	compCandEnergy();
	OGDF_ASSERT(m_candidateEnergy >= 0.0);
	return m_candidateEnergy;
}


#ifdef OGDF_DEBUG
void Grid_EnergyFunction::printStatus() const{
	cout << "\nEnergy function name: " << m_name;
	cout << "\nCurrent energy: " << m_energy;
	node v;
	cout << "\nPosition of nodes in current solution:";
	NodeArray<int> num(m_G);
	int count = 1;
	forall_nodes(v,m_G) num[v] = count ++;
	forall_nodes(v,m_G) {
		cout << "\nNode: " << num[v] << " Position: " << currentPos(v);
	}
	cout << "\nTest Node: " << m_testNode << " New coordinates: " << m_testPos;
	cout << "\nCandidate energy: " << m_candidateEnergy;
	printInternalData();
}
#endif

