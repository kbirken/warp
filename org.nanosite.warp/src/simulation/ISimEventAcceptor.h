/*
 * ISimEventAcceptor.h
 *
 *  Created on: 2009-08-04
 *      Author: kbirken
 */

#ifndef WARP_ISIMEVENTACCEPTOR_H_
#define WARP_ISIMEVENTACCEPTOR_H_


namespace warp {

	// forward declarations
	class CStep;

	class ISimEventAcceptor {
	public:
		// actual simulation driver: set step to ready-state
		virtual void setReady (CStep* step) = 0;

		// these functions are for drawing result graph only
		// TODO: this is a bit clumsy...
		virtual void signalReady (const CStep* from, const CStep* to, bool unblocks) { }
		virtual void signalSend (const CStep* from, const CStep* to, bool startImmediately) { }
		virtual void signalUnless (const CStep* from, const CStep* to) { }
	};

} /* namespace warp */

#endif /* WARP_ISIMEVENTACCEPTOR_H_ */
