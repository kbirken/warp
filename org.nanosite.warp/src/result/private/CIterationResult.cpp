/*
 * CIterationResult.cpp
 *
 * Author:  Klaus Birken (itemis AG)
 * Created: 18.04.2016
 */

#include "result/CIterationResult.h"

#include <stdio.h>

#include <map>
using namespace std;

using namespace warp::result;

CIterationResult::CIterationResult(int tDelta) :
	_tDelta(tDelta)
{
}

CIterationResult::~CIterationResult() {
}

void CIterationResult::addResourceUsage(shared_ptr<model::Resource> resource, vector<int> usingSteps) {
	_resources.push_back(resource);
	_nUsers.push_back(usingSteps.size());
	_usingSteps.push_back(usingSteps);
}

void CIterationResult::writeToFile(FILE* outfile) {
	fprintf(outfile, "DT %d\n", _tDelta);

	fprintf(outfile, "RU");
	for(int i=0; i<_nUsers.size(); i++) {
		fprintf(outfile, " %d", _nUsers[i]);
	}
	fprintf(outfile, "\n");

	fprintf(outfile, "ST");
	for(int i=0; i<_usingSteps.size(); i++) {
		for(int s=0; s<_usingSteps[i].size(); s++) {
			fprintf(outfile, " %d", _usingSteps[i][s]);
		}
		fprintf(outfile, " ;");
	}
	fprintf(outfile, "\n");
}

void CIterationResult::dump() {
	printf("  tDelta = %d\n", _tDelta);
	printf(" ");
	for(int i=0; i<_resources.size(); i++) {
		printf(" %s=%d", _resources[i]->getName(), _nUsers[i]);
	}
	printf("\n");
}
