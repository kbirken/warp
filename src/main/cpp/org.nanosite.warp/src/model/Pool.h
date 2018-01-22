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
		enum ErrorAction {
			EXECUTE_AND_CONTINUE,
			REJECT_AND_CONTINUE,
			STOP_WORKING
		};

		Pool(const char* name, int maxAmount);
		Pool(const char* name, int maxAmount, ErrorAction onOverflow, ErrorAction onUnderflow);
		virtual ~Pool();

		const char* getName() const  { return _name.c_str(); }
		int getMaxAmount() const  { return _maxAmount; }

		ErrorAction onOverflow() const { return _onOverflow; }
		ErrorAction onUnderflow() const { return _onUnderflow; }

	private:
		string _name;
		int _maxAmount;

		ErrorAction _onOverflow;
		ErrorAction _onUnderflow;
	};

} /* namespace model */
} /* namespace warp */

#endif /* WARP_MODEL_POOL_H_ */
