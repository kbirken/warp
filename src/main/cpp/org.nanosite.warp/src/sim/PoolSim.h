/*
 * PoolSim.h
 *
 *  Created on: 2010-03-29
 *      Author: kbirken
 */

#ifndef WARP_SIM_POOL_H_
#define WARP_SIM_POOL_H_

#include "simulation/ILogger.h"

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
		int getNOverflows() const  { return _nOverflows; }
		int getNUnderflows() const  { return _nUnderflows; }

		// init for simulation run
		void init (void);

		void handleRequest(int amount, int index, ILogger& logger);

	private:
		bool isStopped() const;
		bool mayAlloc (int amount) const;
		void handleOverflow (int amount);
		void handleUnderflow (int amount);
		void alloc (int amount);

	private:
		const model::Pool& _modelItem;

		int _allocated;

		int _nOverflows;
		int _nUnderflows;
	};

} /* namespace sim */
} /* namespace warp */

#endif /* WARP_SIM_POOL_H_ */
