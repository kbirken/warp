/*
 * IStepSuccessor.h
 *
 *  Created on: 24.03.2010
 *      Author: kbirken
 */

#ifndef WARP_ISTEPSUCCESSOR_H_
#define WARP_ISTEPSUCCESSOR_H_

namespace warp {

	// forward decls
	class CStep;
	class ISimEventAcceptor;

	class IStepSuccessor {
	public:
		virtual void done (const CStep* step, ISimEventAcceptor& acceptor) = 0;
	};

} /* namespace warp */


#endif /* WARP_ISTEPSUCCESSOR_H_ */
