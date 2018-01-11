// ResourceVector.cpp: implementation of the CResourceVector class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <cstdint>
#include <limits>

#include "simulation/CResourceSlotVector.h"
#include "simulation/CIntAccuracy.h"


//////////////////////////////////////////////////////////////////////

unsigned CResourceVector::_nResources = std::numeric_limits<std::int32_t>::max();


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CResourceVector::CResourceVector (int dflt)
{
	for(unsigned r=0; r<_nResources; r++) {
		_values.push_back(dflt);
	}
}


CResourceVector::CResourceVector (const vector<int>& from)
{
	if (_nResources != from.size()) {
		// todo: error handling
		printf("ERROR: invalid vector size in CResourceVector!");
		return;
	}

	for(unsigned r=0; r<_nResources; r++) {
		_values.push_back(from[r]);
	}
}


CResourceVector::CResourceVector (const CResourceVector& rhs)
{
	_values = rhs._values;
}

CResourceVector& CResourceVector::operator= (const CResourceVector& rhs)
{
	_values = rhs._values;
	return *this;
}

CResourceVector::~CResourceVector()
{

}


void CResourceVector::print (void) const
{
	for(unsigned i=0; i<_values.size(); i++) {
		printf("%7d", CIntAccuracy::toPrint(_values[i]));
	}
	printf("\n");
}


/*
bool CResourceVector::cumulate (const CResourceVector& from, vector<int>& counts, bool verbose)
{
	bool isReady = true;

	if (verbose) {
		printf("\t\t\t\t\t\t");
	}
	for(unsigned i=0; i<_values.size(); i++) {
		if (from[i]>0) {
			_values[i] += from[i];
			counts[i]++;
			isReady=false;
		}
		if (verbose) {
			printf("%d/%07d/%d ", i, _values[i], counts[i]);
		}
	}
	if (verbose) {
		printf("\n");
	}

	return isReady;
}
*/


