/*
 * PoolSimVector.h
 *
 *  Created on: 2010-03-29
 *      Author: kbirken
 */

#ifndef WARP_SIM_POOLSIM_VECTOR_H_
#define WARP_SIM_POOLSIM_VECTOR_H_

#include <vector>
using namespace std;

// forward declarations
class CPool;
class ILogger;

namespace warp {
namespace sim {

	// forward declarations
	class PoolSim;

	class PoolSimVector {
	public:
		typedef vector<int> Values;

		PoolSimVector();
		virtual ~PoolSimVector();

		// add new pool
		void push_back (PoolSim* elem);

		// get number of pools
		int getNPools();

		// init for simulation run
		void init (void);

		// allocate/free
		bool apply (PoolSimVector::Values requests, ILogger& logger);

	private:
		typedef vector<PoolSim*> Elements;

		// vector of pools, we are responsible for its memory
		Elements _pools;
	};

} /* namespace sim */
} /* namespace warp */

#endif /* WARP_SIM_POOLSIM_VECTOR_H_ */
