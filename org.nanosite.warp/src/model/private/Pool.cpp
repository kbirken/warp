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
	_maxAmount(maxAmount)
{
}

Pool::~Pool()
{
}

} /* namespace model */
} /* namespace warp */
