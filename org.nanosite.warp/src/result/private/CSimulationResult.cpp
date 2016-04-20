/*
 * CSimulationResult.cpp
 *
 * Author:  Klaus Birken (itemis AG)
 * Created: 18.04.2016
 */

#include "result/CIterationResult.h"

#include "result/CSimulationResult.h"

#include <stdio.h>

using namespace warp::result;

CSimulationResult::CSimulationResult() {

}

CSimulationResult::~CSimulationResult() {
	for(Iterations::const_iterator it = _iterations.begin(); it!=_iterations.end(); it++) {
		delete *it;
	}
}

void CSimulationResult::clear() {
	_iterations.clear();
}

void CSimulationResult::add(CIterationResult* iterationResult) {
	_iterations.push_back(iterationResult);
}

void CSimulationResult::dump() {
	int i = 0;
	for(Iterations::const_iterator it = _iterations.begin(); it!=_iterations.end(); it++) {
		printf("iteration result: %d\n", i++);
		(*it)->dump();
	}
}

bool CSimulationResult::writeToFile(const char* filename) {

	// prepare loadfile and print headline
	FILE* outfile = fopen(filename, "w");
	if (outfile==NULL)
		return false;

	fprintf(outfile, "WARP SIM RESULT VERSION 1\n");

	int i = 0;
	for(Iterations::const_iterator it = _iterations.begin(); it!=_iterations.end(); it++) {
		fprintf(outfile, "I %06d\n", i++);
		(*it)->writeToFile(outfile);
	}

	fclose(outfile);
	return true;
}
