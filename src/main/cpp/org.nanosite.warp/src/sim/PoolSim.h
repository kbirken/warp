/*
 * PoolSim.h
 *
 *  Created on: 2010-03-29
 *      Author: kbirken
 */

#ifndef WARP_SIM_POOL_H_
#define WARP_SIM_POOL_H_

#include <string>
using namespace std;

namespace warp {

namespace model {
	// forward declarations
	class Pool;
}

namespace sim {

	class PoolSim {
	public:
		PoolSim(const model::Pool& modelItem);
		virtual ~PoolSim();

		const char* getName() const;

		int getAllocated() const  { return _allocated; }

		// init for simulation run
		void init (void);

		bool mayAlloc (int amount) const;
		void alloc (int amount);

	private:
		const model::Pool& _modelItem;

		int _allocated;
	};

} /* namespace sim */
} /* namespace warp */

#endif /* WARP_SIM_POOL_H_ */
