/*
 * CPoolVector.cpp
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#include "model/CPoolVector.h"
#include "model/CPool.h"
#include "simulation/ILogger.h"



CPoolVector::CPoolVector()
{
}

CPoolVector::~CPoolVector()
{
	for (Elements::iterator i = _pools.begin(); i!=_pools.end(); i++) {
		delete(*i);
	}
}


void CPoolVector::push_back (CPool* elem)
{
	_pools.push_back(elem);
}


void CPoolVector::init (void)
{
	for(int i=0; i<_pools.size(); i++) {
		_pools[i]->init();
	}
}


bool CPoolVector::apply (CPoolVector::Values requests, ILogger& logger)
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

