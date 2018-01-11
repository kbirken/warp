/*
 * CIterationResult.h
 *
 * Author:  Klaus Birken (itemis AG)
 * Created: 18.04.2016
 */

#ifndef RESULT_CITERATIONRESULT_H_
#define RESULT_CITERATIONRESULT_H_

#include <vector>
using namespace std;

#include "model/Resource.h"

namespace warp {
namespace result {

class CIterationResult {
public:
	CIterationResult(int tDelta);
	virtual ~CIterationResult();

	void addResourceUsage(shared_ptr<model::Resource> resource, vector<int> usingSteps);

	void writeToFile(FILE* outfile);

	void dump();

private:
	int _tDelta;

	vector< shared_ptr<model::Resource> > _resources;
	vector<int> _nUsers;
	vector<vector<int> > _usingSteps;
};

} /* namespace result */
} /* namespace warp */

#endif /* RESULT_CITERATIONRESULT_H_ */
