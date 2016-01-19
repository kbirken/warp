// SimulatorCore.cpp: implementation of the CSimulatorCore class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <limits.h>
#include <model/CBehavior.h>

#include "simulation/CSimCore.h"
#include "simulation/CAPSScheduler.h"
#include "simulation/CIntAccuracy.h"

namespace warp {

// integration window for load diagrams
int CSimulatorCore::_timeWindowDiscrete = 10000;  // [microsec]


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimulatorCore::CSimulatorCore (int verbose) :
	_verbose(verbose),
	_healthy(true),
	_time(0),
	_timeDiscrete(0),
	_timeslotDiscrete(0),
	_dotfile(0)
{
}


CSimulatorCore::~CSimulatorCore()
{
}


void CSimulatorCore::print ()
{
	printf("SimulatorCore - running:\n");
	for (CStep::Vector::const_iterator it = _running.begin(); it!=_running.end(); it++) {
		printf("\t");
		(*it)->print();
	}
}


void CSimulatorCore::fatal (const char* fmt, ...)
{
	_healthy = false;

	printf("%09d %-10s", CIntAccuracy::toPrint(_time), "FATAL");

	va_list ap;
	va_start(ap, fmt);
	/*int retval = */vprintf(fmt, ap);
	va_end(ap);
}


void CSimulatorCore::log (const char* action, const char* fmt, ...)
{
	printf("%09d %-10s", CIntAccuracy::toPrint(_time), action);

	va_list ap;
	va_start(ap, fmt);
	/*int retval = */vprintf(fmt, ap);
	va_end(ap);

	//fflush(stdout);
}


void CSimulatorCore::drawDotNode (CStep* step, string color, string more) {
	if (_dotfile) {
		int t1 = _readyMap[step];
		int t2 = _runningMap[step];
		char ts1[32], ts2[32], ts3[32];
		sprintf(ts3, "%d", CIntAccuracy::toPrint(_time/1000));
		string label = step->getDotLabel();

		if (more!="") {
			label += string("\\n") + more;
		}

		if (t2!=t1) {
			sprintf(ts1, "%d", CIntAccuracy::toPrint(t1/1000));
			label += string("\\nblocked ") + ts1;
		}
		sprintf(ts2, "%d", CIntAccuracy::toPrint(t2/1000));
		label += string("\\nrunning ") + ts2;

		label += string("\\ndone ") + ts3;

		fprintf(_dotfile, "\t%s [label=\"%s\", fillcolor=\"%s\"];\n", step->getDotId().c_str(), label.c_str(), color.c_str());

		int phase = CIntAccuracy::toPrint(_time/1000) / PHASE_LEN_MS;
		_phases[phase].push_back(step);
	}
}


void CSimulatorCore::drawNode (CStep* step) {
	string color = "lightgrey";
	string more = "";
	const CBehavior& bhvr = step->getBehavior();

	if (bhvr.hasUnlessCondition()) {
		color = "cyan";

		if (step->isLast()) {
			if (! bhvr.getUnlessCondition()) {
				// last step in this behavior will be drawn for last iteration
				return;
			} else {
				// some additional information for last step
				char txt[32];
				sprintf(txt, "%d", bhvr.getIterations());
				more = string(txt) + " iterations";
			}
		} else {
			if (bhvr.hasFinishedOnce()) {
				// all other steps will be drawn only the first time, don't draw now
				return;
			}
		}
	}
	if (step->isInternalMilestone()) {
		color = "orange";
	}
	if (step->isCustomerMilestone()) {
		color = "yellow";
	}

	drawDotNode(step, color, more);
	step->nextDotInstance();
}


void CSimulatorCore::setReady (CStep* step)
{
	log("READY", "%s\n", step->getQualifiedName().c_str());
	_ready.push_back(step);
	_readyMap[step] = _time;
}

void CSimulatorCore::signalReady (const CStep* from, const CStep* to, bool unblocks)
{
	if (_dotfile) {
		string col = "blue";
		const CBehavior& fromBhvr = from->getBehavior();
		if (fromBhvr==to->getBehavior()) {
			col = "black";
			if (fromBhvr.hasUnlessCondition() && fromBhvr.hasFinishedOnce()) {
				// we have drawn during first run, don't draw anymore
				return;
			}
		}
		fprintf(_dotfile, "\t%s -> %s [color=%s%s];\n",
			from->getDotId().c_str(),
			to->getDotId().c_str(),
			col.c_str(),
			unblocks ? ", penwidth=3.0" : "");
	}
}

void CSimulatorCore::signalSend (const CStep* from, const CStep* to, bool startImmediately)
{
	if (_dotfile) {
		string fromStr = "ROOT";
		if (from==0) {
			fprintf(_dotfile, "\t%s [label=\"ROOT\"];\n", fromStr.c_str());
		} else {
			fromStr = from->getDotId();
		}

		fprintf(_dotfile, "\t%s -> %s [color=orange%s];\n",
				fromStr.c_str(), to->getDotId(from==to).c_str(),
				startImmediately ? ", penwidth=3.0" : "");
	}
}

void CSimulatorCore::signalUnless (const CStep* from, const CStep* to)
{
	if (_dotfile) {
		string col = "cyan";
		fprintf(_dotfile, "\t%s -> %s [color=%s];\n",
			from->getPrevDotId().c_str(),
			to->getDotId().c_str(),
			col.c_str());
	}
}


// helper function to switch a step's state to 'done'
void CSimulatorCore::stepDone (CStep* step) {
	step->exitActions(*this, *this);
	log("DONE", "%s\n", step->getQualifiedName().c_str());
	drawNode(step);
}



void CSimulatorCore::run (
		const model::Resource::Vector& resources,
		const model::Resource::Vector& resourceInterfaces,
		sim::PoolSimVector& pools,
		bool withLoadfile,
		string dotFileName)
{
	int iter=0;
	//int n=35;
	int n=19999;

	vector<bool> isLimited;
	for(unsigned i=0; i<resourceInterfaces.size(); i++) {
		isLimited.push_back(resourceInterfaces[i]->isLimited());
	}

	// prepare loadfile and print headline
	FILE* loadfile = 0;
	if (withLoadfile) {
		loadfile = fopen("load.csv", "w");
		if (loadfile) {
			fprintf(loadfile, "time [%d microsec];", _timeWindowDiscrete);
			for(unsigned r=1 /* skip waittime */; r<resourceInterfaces.size(); r++) {
				fprintf(loadfile, "%s;", resourceInterfaces[r]->getName());
			}
			fprintf(loadfile, "\n");
		}
	}

	// prepare dotfile
	_dotfile = 0;
	if (dotFileName.size()>0) {
		_dotfile = fopen(dotFileName.c_str(), "w");
		if (_dotfile) {
			fprintf(_dotfile, "digraph feature_graph {\n");
			fprintf(_dotfile, "\tnode [style=filled,fillcolor=lightgray,shape=box,fontsize=10,fontname=\"Arial\"];\n");
		}
	}

	// prepare schedulers
	Schedulers scheds;
	for(unsigned int r=0; r<resources.size(); r++) {
		if (resources[r]->isSchedulerAPS()) {
			CAPSScheduler* sched = new CAPSScheduler(*resources[r]);
			scheds[r] = sched;
		}
	}

	// iterate through time
	_healthy = true;
	pools.init();
	while (_healthy && iter<=n && (_ready.size()>0 || _running.size()>0)) {
		if (_verbose>0) {
			log("INFO", "ITER %5d   (ready=%d  running=%d)\n", iter, _ready.size(), _running.size());
		}
		bool ok = iteration(resources, pools, scheds, isLimited, loadfile);
		if (!ok) {
			// error message is printed inside iteration()
			return;
		}
		iter++;
	}

	if (iter>=n) {
		log("INFO", "simulation ended due to max number of iterations (max=%d)\n", n);
	}

	// clean-up schedulers
	for(Schedulers::iterator si=scheds.begin(); si!=scheds.end(); ++si) {
		delete(si->second);
	}

	// close dotfile
	if (_dotfile) {
		// draw cluster for each phase
		if (0) {  // deactivated, graphs are not really nice...
			for(PhaseMap::const_iterator pi=_phases.begin(); pi!=_phases.end(); ++pi) {
				int phaseBegin = pi->first * PHASE_LEN_MS / 1000;
				int phaseEnd = (pi->first+1) * PHASE_LEN_MS / 1000;
				fprintf(_dotfile, "\tsubgraph cluster_%d {\n", pi->first);
				fprintf(_dotfile, "\t\tlabel=\"PHASE %3d-%3d\";\n", phaseBegin, phaseEnd);
				for(CStep::Vector::const_iterator si=pi->second.begin(); si!=pi->second.end(); ++si) {
					fprintf(_dotfile, "\t\t%s;\n", (*si)->getDotId().c_str());
				}
				fprintf(_dotfile, "\t}\n");
			}
		}

		// close file
		fprintf(_dotfile, "}\n");
		fclose(_dotfile);
		_dotfile = 0;
	}

	// close loadfile
	if (loadfile) {
		fclose(loadfile);
	}
}


bool CSimulatorCore::iteration (
	const model::Resource::Vector& resources,
	sim::PoolSimVector& pools,
	Schedulers& scheds,
	vector<bool> isLimited,
	FILE* loadfile
)
{
	// transfer ready steps to running
	// (this might lead to new ready steps in between)
	while(_ready.size()>0) {
		// copy vector because loop might insert new CSteps into _ready vector
		CStep::Vector todo(_ready.begin(), _ready.end());
		_ready.erase(_ready.begin(), _ready.end());

		// handle current set of ready steps
		for (CStep::Vector::const_iterator it = todo.begin(); it!=todo.end(); it++) {
			CStep* step = *it;
			log("RUNNING", "%s\n", step->getQualifiedName().c_str());
			_runningMap[step] = _time;

			// alloc/free pool resources of this step
			bool ok = pools.apply(step->getPoolRequests(), *this);
			if (!ok) {
				return false;
			}

			if (step->hasResourceNeeds()) {
				_running.push_back(step);
			} else {
				// this is a step with zero resource usage, it is already done
				stepDone(step);
			}
		}
	}


	// compute sum of requests of all 'running' steps for each ResourceSlot (absolute value and count)
	CResourceVector sums;
	vector<int> nRequests;
	nRequests.resize(sums.size());
	for (CStep::Vector::const_iterator it = _running.begin(); it!=_running.end(); it++) {
		for(unsigned i=0; i<sums.size(); i++) {
			CStep* step = *it;
			int rn = step->getCurrentResourceNeeds()[i];
			if (rn > 0) {
				sums[i] += rn;
				nRequests[i]++;
			}
		}
	}


	// for all APS-scheduled resources: compute #requests per partition
	for(Schedulers::iterator si=scheds.begin(); si!=scheds.end(); ++si) {
		si->second->clear();
	}
	for (CStep::Vector::const_iterator it = _running.begin(); it!=_running.end(); it++) {
		CStep* step = *it;
		if (step->getCurrentResourceNeeds()[step->getCPU()]) {
			int part = step->getPartition();
			Schedulers::iterator si = scheds.find(step->getCPU());
			if (si!=scheds.end()) {
				si->second->addRequest(part);
			}
		}
	}
	if (_verbose>1) {
		for(Schedulers::iterator si=scheds.begin(); si!=scheds.end(); ++si) {
			int r = si->first;
			CAPSScheduler* aps = si->second;
			const model::Resource& res = *(resources[r]);
			bool logstart = false;
			for(unsigned int p=0; p<res.getNPartitions(); p++) {
				if (aps->getNReqPerPartition(p)>0) {
					if (!logstart) {
						logstart = true;
						log("SCHED", "%s=APS: ", res.getName());
					}
					printf(" %s/%i/%d/%d",
							res.getPartitionName(p).c_str(), p,
							(res.getPartitionSize(p)*1000) / aps->getUsedPartitionsSize(),
							aps->getNReqPerPartition(p)
					);
				}
			}
			if (logstart) {
				printf("\n");
			}
		}
	}

	// determine next interesting time (= minimum delta of all steps)
	CResourceVector mins(INT_MAX);
	for (CStep::Vector::const_iterator it = _running.begin(); it!=_running.end(); it++) {
		int partNReqs = 0;
		int partSize = 1;
		int partAllSize = 1;
		int part = (*it)->getPartition();
		Schedulers::iterator si = scheds.find((*it)->getCPU());
		if (si!=scheds.end()) {
			CAPSScheduler* aps = si->second;
			partNReqs = aps->getNReqPerPartition(part);
			partSize = aps->getPartitionSize(part);
			partAllSize = aps->getUsedPartitionsSize();
		}

		bool ok = (*it)->checkSmallestRequests(nRequests, mins, partNReqs, partSize, partAllSize, *this);
		if (!ok) {
			return false;
		}
	}

	int overallMinDelta = 0;
	bool logstart = true;
	for(unsigned r=0; r<nRequests.size(); r++) {
		if (nRequests[r] > 0) {
			int minDelta = isLimited[r] ? (mins[r] * nRequests[r]) : mins[r];
			if (overallMinDelta==0 || overallMinDelta > minDelta) {
				overallMinDelta = minDelta;
			}
			if (_verbose>1) {
				if (logstart) {
					logstart = false;
					log("DEBUG", "");
				}
				printf("%s", resources[r]->getName());
				printf("/%d/%d>%d  ",
						nRequests[r],
						CIntAccuracy::toPrint(sums[r]), CIntAccuracy::toPrint(minDelta)
				);
			}
		}
	}
	if (_verbose>1) {
		printf("\n");
	}

	// output to loadfile
	int dt = CIntAccuracy::toCalc(_timeWindowDiscrete);
	if (loadfile) {
		// TODO: we are producing some inaccuracies at the iteration boundaries here
		while (_timeDiscrete+dt < _time+overallMinDelta) {
			fprintf(loadfile, "%06d;", _timeslotDiscrete);

			// TODO: preparing a 0/1 array once outside while() would be more efficient
			for(unsigned r=1 /* skip waittime */; r<nRequests.size(); r++) {
				int resLoad = 0;
				if (nRequests[r] > 0) {
					resLoad = 1;
				}
				fprintf(loadfile, "%d;", resLoad);
			}

			fprintf(loadfile, "\n");

			_timeDiscrete += dt;
			_timeslotDiscrete++;
		}
	}

	// progress in time
	if (_verbose>1) {
		string loads = "";
		// update book-keeping
		for(unsigned r=0; r<nRequests.size(); r++) {
			if (nRequests[r] > 0) {
				sums[r] -= overallMinDelta;

				// limit sum to positive values
				if (sums[r]<0) {
					sums[r] = 0;
				}
			}
		}
		for(unsigned r=1; r<nRequests.size(); r++) {
			if (nRequests[r] > 0) {
				static char txt[128];
				sprintf(txt, "%s/%d", resources[r]->getName(), CIntAccuracy::toPrint(sums[r]));
				loads += string(" ") + string(txt);
			}
		}
		log("INFO", "DELTA=%d %s\n", CIntAccuracy::toPrint(overallMinDelta), loads.c_str());
	}

	// overflow check
	if (overallMinDelta<0) {
		fatal("arithmetic overflow (DELTA=%d)\n", overallMinDelta);
		return false;
	}

	_time += overallMinDelta;
	// overflow check
	if (_time<0) {
		fatal("arithmetic overflow (TIME=%d)\n", _time);
		return false;
	}

	vector<int> nMaxRequests(nRequests.begin(), nRequests.end());
	{
		for (CStep::Vector::iterator it = _running.begin(); it!=_running.end();) {
			int partNReqs = 0;
			int partSize = 1;
			int partAllSize = 1;
			int part = (*it)->getPartition();
			Schedulers::iterator si = scheds.find((*it)->getCPU());
			if (si!=scheds.end()) {
				CAPSScheduler* aps = si->second;
				partNReqs = aps->getNReqPerPartition(part);
				partSize = aps->getPartitionSize(part);
				partAllSize = aps->getUsedPartitionsSize();
			}

			if ((*it)->consume(overallMinDelta, isLimited, nMaxRequests, partNReqs, partSize, partAllSize, *this)) {
				// this step is done and can be consumed
				stepDone(*it);
				it = _running.erase(it);
			}
			else {
				it++;
			}
		}
	}

	return true;
}

} /* namespace warp */


