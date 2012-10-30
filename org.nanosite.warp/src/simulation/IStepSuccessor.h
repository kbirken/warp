/*
 * IStepSuccessor.h
 *
 *  Created on: 24.03.2010
 *      Author: kbirken
 */

#ifndef ISTEPSUCCESSOR_H_
#define ISTEPSUCCESSOR_H_

// forward decls
class CStep;
class ISimEventAcceptor;

class IStepSuccessor {
public:
	virtual void done (const CStep* step, ISimEventAcceptor& acceptor) = 0;
};


#endif /* ISTEPSUCCESSOR_H_ */
