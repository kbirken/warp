/*
 * PoolSim.cpp
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#include "sim/PoolSim.h"

#include "model/Pool.h"

namespace warp {
namespace sim {

PoolSim::PoolSim (const model::Pool& modelItem) :
	_modelItem(modelItem),
	_allocated(0),
	_nOverflows(0),
	_nUnderflows(0)
{
}


PoolSim::~PoolSim()
{
}

const char* PoolSim::getName() const
{
	// delegate
	return _modelItem.getName();
}


// init for simulation run
void PoolSim::init (void)
{
	_allocated = 0;
}


void PoolSim::handleRequest(int amount, int index, ILogger& logger) {
	if (isStopped()) {
		// pool is stopped due to previous overflow/underflow
		logger.log("POOL", "pool %s/%d: stopped, ignoring request %d\n", getName(), index, amount);
	} else {
		// pool is working, try to allocate
		if (mayAlloc(amount)) {
			// allocation in range, do it
			alloc(amount);
			logger.log("POOL", "pool %s/%d: request %d, now %d\n", getName(), index, amount, getAllocated());
		}
		else {
			// overflow or underflow
			if (getAllocated()+amount < 0) {
				handleUnderflow(amount);
				logger.log("POOL", "pool %s/%d: underflow %d, now %d\n", getName(), index, amount, getAllocated());
			} else {
				handleOverflow(amount);
				logger.log("POOL", "pool %s/%d: overflow %d, now %d\n", getName(), index, amount, getAllocated());
			}
		}
	}
}


bool PoolSim::isStopped() const
{
	if (_nOverflows>0 && _modelItem.onOverflow()==model::Pool::ErrorAction::STOP_WORKING) {
		return true;
	}
	if (_nUnderflows>0 && _modelItem.onUnderflow()==model::Pool::ErrorAction::STOP_WORKING) {
		return true;
	}

	return false;
}

bool PoolSim::mayAlloc (int amount) const
{
	if (_allocated+amount > _modelItem.getMaxAmount()) {
		// overflow
		return false;
	}

	if (_allocated+amount < 0) {
		// underflow
		return false;
	}

	return true;
}


void PoolSim::handleOverflow (int amount)
{
	_nOverflows++;
	if (_modelItem.onOverflow()==model::Pool::ErrorAction::EXECUTE_AND_CONTINUE) {
		alloc(amount);
	}
}

void PoolSim::handleUnderflow (int amount)
{
	_nUnderflows++;
	if (_modelItem.onUnderflow()==model::Pool::ErrorAction::EXECUTE_AND_CONTINUE) {
		alloc(amount);
	}
}

void PoolSim::alloc (int amount)
{
	_allocated += amount;
}



} /* namespace sim */
} /* namespace warp */

