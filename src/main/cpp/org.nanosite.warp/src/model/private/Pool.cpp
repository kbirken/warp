/*
 * Pool.cpp
 *
 *  Created on: 2015-12-17
 *      Author: kbirken
 */

#include "model/Pool.h"

namespace warp {
namespace model {

Pool::Pool (const char* name, int maxAmount) :
	_name(name),
	_maxAmount(maxAmount),
	_onOverflow(ErrorAction::REJECT_AND_CONTINUE),
	_onUnderflow(ErrorAction::REJECT_AND_CONTINUE)
{
}

Pool::Pool (const char* name, int maxAmount, ErrorAction onOverflow, ErrorAction onUnderflow) :
	_name(name),
	_maxAmount(maxAmount),
	_onOverflow(onOverflow),
	_onUnderflow(onUnderflow)
{
}

Pool::~Pool()
{
}

} /* namespace model */
} /* namespace warp */
