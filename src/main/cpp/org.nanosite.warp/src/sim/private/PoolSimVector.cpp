/*
 * PoolSimVector.cpp
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#include "sim/PoolSimVector.h"

#include "sim/PoolSim.h"
#include "simulation/ILogger.h"

#include "model/CStep.h"

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


bool PoolSimVector::apply (PoolSimVector::Values requests, warp::CStep* step, ILogger& logger)
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
			PoolSim* p = _pools[i];
			p->handleRequest(r, i, logger);

			// store current state of this pool in step, for result retrieval
			step->storePoolState(i,
					p->getAllocated(),
					p->getNOverflows() > 0,
					p->getNUnderflows() > 0
			);
		}
	}

	return true;
}

} /* namespace sim */
} /* namespace warp */

