/*
 * CTimeMonitor.cpp
 *
 *  Created on: 04.08.2009
 *      Author: kbirken
 */

#include "simulation/CTimeMonitor.h"

#include <time.h>

#include <windows.h>
#include <mmsystem.h>

CTimeMonitor::CTimeMonitor()
{
	tick();
}

CTimeMonitor::~CTimeMonitor() {
}


void CTimeMonitor::done (const char* what)
{
	_what.push_back(what);
	tick();
}

void CTimeMonitor::tick ()
{
	//DWORD t = timeGetTime();
	//printf("tick %ld\n", t);

	_ticks.push_back(clock());
}

int CTimeMonitor::getInterval (unsigned i)
{
	if (i+1 >= _ticks.size()) {
		return -1;
	}

	return _ticks[i+1] - _ticks[i];
}

void CTimeMonitor::report ()
{
	printf("Execution times:\n");
	for(unsigned i=0; i<_what.size(); i++) {
		printf("  %-10s: %06d msec\n", _what[i].c_str(), getInterval(i));
	}
}
