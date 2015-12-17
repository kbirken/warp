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
	_allocated(0)
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


void PoolSim::alloc (int amount)
{
	_allocated += amount;
}

} /* namespace sim */
} /* namespace warp */

