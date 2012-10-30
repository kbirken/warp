/*
 * CPool.h
 *
 *  Created on: 29.03.2010
 *      Author: kbirken
 */

#ifndef CPOOL_H_
#define CPOOL_H_

#include <string>
using namespace std;


class CPool {
public:
	CPool(const char* name, int maxAmount);
	virtual ~CPool();

	const char* getName() const  { return _name.c_str(); }
	int getMaxAmount() const  { return _maxAmount; }

	int getAllocated() const  { return _allocated; }

	// init for simulation run
	void init (void);

	bool mayAlloc (int amount) const;
	void alloc (int amount);

private:
	string _name;
	int _maxAmount;

	int _allocated;
};

#endif /* CPOOL_H_ */
