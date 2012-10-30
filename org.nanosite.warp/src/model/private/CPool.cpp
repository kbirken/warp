/*
 * CPool.cpp
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#include "model/CPool.h"


CPool::CPool (const char* name, int maxAmount) :
	_name(name),
	_maxAmount(maxAmount),
	_allocated(0)
{
}


CPool::~CPool()
{
}


// init for simulation run
void CPool::init (void)
{
	_allocated = 0;
}


bool CPool::mayAlloc (int amount) const
{
	if (_allocated+amount > _maxAmount) {
		// overflow
		return false;
	}

	if (_allocated+amount < 0) {
		// underflow
		return false;
	}

	return true;
}


void CPool::alloc (int amount)
{
	_allocated += amount;
}


