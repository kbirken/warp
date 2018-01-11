/*
 * CTimeMonitor.h
 *
 *  Created on: 04.08.2009
 *      Author: kbirken
 */

#ifndef CTIMEMONITOR_H_
#define CTIMEMONITOR_H_

#include <vector>
#include <string>
using namespace std;

class CTimeMonitor {
public:
	CTimeMonitor();
	virtual ~CTimeMonitor();

	void done (const char* what);
	void report ();

private:
	void tick ();
	int getInterval (unsigned i);

private:
	vector<string> _what;
	vector<int> _ticks;
};

#endif /* CTIMEMONITOR_H_ */
