// SimulatorCore.h: interface for the CSimulatorCore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SIMULATORCORE_H_INCLUDED_)
#define _SIMULATORCORE_H_INCLUDED_

#include <stdarg.h>
#include <stdio.h>  // for FILE*

#include <vector>
#include <map>
using namespace std;

#include "simulation/ISimEventAcceptor.h"
#include "simulation/ILogger.h"
#include "simulation/CResourceSlotVector.h"
#include "simulation/CIntAccuracy.h"
#include "model/Resource.h"
#include "model/CStep.h"

namespace warp {

// forward declarations
class CAPSScheduler;


class CSimulatorCore : public ISimEventAcceptor, public ILogger
{
public:
	CSimulatorCore(int verbose);
	virtual ~CSimulatorCore();

	// ISimEventAcceptor interface
	void setReady (CStep* step);
	void signalReady (const CStep* from, const CStep* to, bool unblocks);
	void signalSend (const CStep* from, const CStep* to, bool startImmediately);
	void signalUnless (const CStep* from, const CStep* to);

	// run actual simulation
	void run (
			const model::Resource::Vector& resources,
			const model::Resource::Vector& resourceInterfaces,
			sim::PoolSimVector& pools,
			bool withLoadfile,
			string dotFileName);

	// print out current state of SimulatorCore (for debugging reasons)
	void print();

	// ILogger interface
	// print one line to trace log, including timestamp
	void fatal (const char* fmt, ...);
	void log (const char* action, const char* fmt, ...);
	int verbose() const  { return _verbose; }

	long getTimeReady(CStep* step) {
		return CIntAccuracy::toPrint(_readyMap[step]);
	}

	long getTimeRunning(CStep* step) {
		return CIntAccuracy::toPrint(_runningMap[step]);
	}

	long getTimeDone(CStep* step) {
		return CIntAccuracy::toPrint(_doneMap[step]);
	}

private:
	typedef map<unsigned int, CAPSScheduler*> Schedulers;

	// progress one iteration during simulation run
	bool iteration (
			const model::Resource::Vector& resources,
			sim::PoolSimVector& pools,
			Schedulers& scheds,
			vector<bool> isLimited,
			FILE* loadfile);

	void drawNode (CStep* step);
	void drawDotNode (CStep* step, string color, string more);

	void stepDone (CStep* step);

private:
	int _verbose;

	CStep::Vector _ready;
	CStep::Vector _running;

	bool _healthy;
	int _time;

	static int _timeWindowDiscrete;
	int _timeDiscrete;
	int _timeslotDiscrete;

	// dotfile output
	FILE* _dotfile;
	typedef map<int,CStep::Vector> PhaseMap;
	static const int PHASE_LEN_MS = 1000;
	PhaseMap _phases;

	// timing results (per model step)
	typedef map<CStep*,int> TimeMap;
	TimeMap _readyMap;
	TimeMap _runningMap;
	TimeMap _doneMap;
};

} /* namespace warp */

#endif // !defined(_SIMULATORCORE_H_INCLUDED_)
