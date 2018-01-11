/*
 * Pool.h
 *
 *  Created on: 2015-12-17
 *      Author: kbirken
 */

#ifndef WARP_MODEL_POOL_H_
#define WARP_MODEL_POOL_H_

#include <string>
using namespace std;

namespace warp {
namespace model {

	class Pool {
	public:
		Pool(const char* name, int maxAmount);
		virtual ~Pool();

		const char* getName() const  { return _name.c_str(); }
		int getMaxAmount() const  { return _maxAmount; }

	private:
		string _name;
		int _maxAmount;
	};

} /* namespace model */
} /* namespace warp */

#endif /* WARP_MODEL_POOL_H_ */
