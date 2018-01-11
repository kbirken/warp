/*
 * CSimulationResult.h
 *
 * Author:  Klaus Birken (itemis AG)
 * Created: 18.04.2016
 */

#ifndef RESULT_CSIMULATIONRESULT_H_
#define RESULT_CSIMULATIONRESULT_H_

#include <vector>
using namespace std;

namespace warp {
namespace result {

// forward declaration
class CIterationResult;

class CSimulationResult {
public:
	CSimulationResult();
	virtual ~CSimulationResult();

	void clear();

	void add(CIterationResult* iterationResult);

	bool writeToFile(const char* filename);

	void dump();

private:
	typedef vector<CIterationResult*> Iterations;
	Iterations _iterations;
};

} /* namespace result */
} /* namespace warp */

#endif /* RESULT_CSIMULATIONRESULT_H_ */
