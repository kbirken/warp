/*
 * PoolSimVector.cpp
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#include "sim/PoolSimVector.h"

#include "sim/PoolSim.h"
#include "simulation/ILogger.h"

namespace warp {
namespace sim {

PoolSimVector::PoolSimVector()
{
}

PoolSimVector::~PoolSimVector()
{
	for (Elements::iterator i = _pools.begin(); i!=_pools.end(); i++) {
		delete(*i);
	}
}


void PoolSimVector::push_back (PoolSim* elem)
{
	_pools.push_back(elem);
}


int PoolSimVector::getNPools (void)
{
	return _pools.size();
}

void PoolSimVector::init (void)
{
	for(int i=0; i<_pools.size(); i++) {
		_pools[i]->init();
	}
}


bool PoolSimVector::apply (PoolSimVector::Values requests, ILogger& logger)
{
	if (requests.size() != _pools.size()) {
		// both vectors need to have same size
		logger.fatal("Invalid number %d of pool requests (%d pools defined)\n",
				requests.size(),
				_pools.size());
		return false;
	}

	// now do real alloc
	for(int i=0; i<_pools.size(); i++) {
		int r = requests[i];
		if (r != 0) {
			if (_pools[i]->mayAlloc(requests[i])) {
				// allocation in range, do it
				_pools[i]->alloc(requests[i]);
				logger.log("POOL", "pool %s/%d: request %d, now %d\n", _pools[i]->getName(), i, r, _pools[i]->getAllocated());
			}
			else {
				// overflow or underflow
				if (r>0) {
					logger.log("POOL", "pool %s/%d: overflow %d\n", _pools[i]->getName(), i, r);
				} else {
					logger.log("POOL", "pool %s/%d: underflow %d\n", _pools[i]->getName(), i, r);
				}
			}

		}
	}

	return true;
}

} /* namespace sim */
} /* namespace warp */

