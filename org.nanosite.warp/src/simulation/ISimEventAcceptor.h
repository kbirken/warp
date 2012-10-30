/*
 * ISimEventAcceptor.h
 *
 *  Created on: 04.08.2009
 *      Author: kbirken
 */

#ifndef ISIMEVENTACCEPTOR_H_
#define ISIMEVENTACCEPTOR_H_

// forward decl
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


#endif /* ISIMEVENTACCEPTOR_H_ */
