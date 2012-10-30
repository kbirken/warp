/*
 * CPool.h
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#ifndef CPOOL_VECTOR_H_
#define CPOOL_VECTOR_H_

#include <vector>
using namespace std;

//forward declarations
class CPool;
class ILogger;


class CPoolVector {
public:
	typedef vector<int> Values;

	CPoolVector();
	virtual ~CPoolVector();

	// add new pool
	void push_back (CPool* elem);

	// init for simulation run
	void init (void);

	// allocate/free
	bool apply (CPoolVector::Values requests, ILogger& logger);

private:
	typedef vector<CPool*> Elements;

	// vector of pools, we are responsible for its memory
	Elements _pools;
};

#endif /* CPOOL_VECTOR_H_ */
