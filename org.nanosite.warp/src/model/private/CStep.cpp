// Step.cpp: implementation of the CStep class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include <algorithm>
using namespace std;

#include "model/CStep.h"
#include "model/CFunctionBlock.h"
#include "simulation/CResourceSlotVector.h"
#include "simulation/ISimEventAcceptor.h"
#include "simulation/ILogger.h"
#include "simulation/CIntAccuracy.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStep::CStep (int id, CBehaviour* bhvr, string name,
		int milestone, unsigned int cpu, unsigned int partition,
		CResourceVector* rv,
		CResourceVector* averageCST,
		const CPoolVector::Values& poolRequests
) :
	_id(id),
	_bhvr(bhvr),
	_name(name),
	_milestone(milestone),
	_cpu(cpu),
	_partition(partition),
	_isFirst(false),
	_isLast(false),
	_initialResourceNeeds(rv),
	_averageCSTVector(averageCST),
	_poolRequests(poolRequests),
	_directPredecessor(0),
	_dotInstance(1000),
	_resourceNeeds(0)
{
}


CStep::~CStep()
{
	delete(_resourceNeeds);
	delete(_initialResourceNeeds);
	delete(_averageCSTVector);
}


void CStep::addSuccessor (CBehaviour* behaviour)
{
	//printf("succ %s => %s\n", getQualifiedName().c_str(), behaviour->getQualifiedName().c_str());
	_successors.push_back(behaviour);
}

void CStep::addSuccessor (CStep* step)
{
	//printf("succ %s => %s\n", getQualifiedName().c_str(), step->getQualifiedName().c_str());
	_successors.push_back(step);

	if (_bhvr == step->_bhvr) {
		// remember as direct predecessor in behaviour
		// will be added to _waitFor during prepareExecution()
		step->_directPredecessor = this;
	} else {
		// this is a precondition or some other condition
		step->waitFor(this);
	}
}


const CBehaviour* CStep::getBehaviour (void) const
{
	return _bhvr;
}


void CStep::waitFor (CStep* step)
{
	//printf("CStep::waitFor %s <= %s\n", getQualifiedName().c_str(), step->getQualifiedName().c_str());
	_waitFor.push_back(step);
}


void CStep::prepareExecution (void)
{
	//printf("CStep::prepareExecution %s\n", getQualifiedName().c_str());	fflush(stdout);

	delete(_resourceNeeds);
	_resourceNeeds = new CResourceVector(*_initialResourceNeeds);

	if (_directPredecessor) {
		// on restart of the behaviour, we add the direct predecessor as only waitFor element
		_waitFor.push_back(_directPredecessor);
	}
}


void CStep::done (const CStep* step, ISimEventAcceptor& eventAcceptor)
{
	//printf("CStep::done %s: %s is done - waitfor=%d\n", getQualifiedName().c_str(), step->getQualifiedName().c_str(), _waitFor.size());

	Vector::iterator iter = find(_waitFor.begin(), _waitFor.end(), step);
	if (iter==_waitFor.end()) {
		// todo: error handling
		printf("ERROR in step '%s' at CStep::done('%s'): inconsistent data model\n",
					getQualifiedName().c_str(),
					step->getQualifiedName().c_str());
		exit(1);
	}
	else {
		// we are no more waiting for this step, because it is done
		_waitFor.erase(iter);
	}

	bool runNow = false;
	if (_waitFor.size()==0) {
		// waiting is over
		if (_isFirst) {
			// first step has to ask behaviour if trigger has been received yet
			if (_bhvr->isRunning()) {
				runNow = true;
			}
		} else {
			// non-first steps can run immediately
			runNow = true;
		}
	}
	if (runNow) {
		eventAcceptor.setReady(this);
	}
	eventAcceptor.signalReady(step, this, runNow);
}


void CStep::print() const
{
	printf("%4d %s\n", _id, getQualifiedName().c_str());
	if (_resourceNeeds) {
		printf("\t\t");
		_resourceNeeds->print();
	}
}


void CStep::printWaitingList() const
{
	for(Vector::const_iterator it=_waitFor.begin(); it!=_waitFor.end(); it++) {
		printf("\t%s\n", (*it)->getQualifiedName().c_str());
	}
}


string CStep::getQualifiedName() const
{
	return _bhvr->getQualifiedName() + "::" + _name;
}

string CStep::getPrevDotId () const
{
	char txt[32];

	sprintf(txt, "_%d", _dotInstance-1);
	return _bhvr->getDotId() + "_" + _name + txt;
}

string CStep::getDotId (bool next) const
{
	char txt[32];

	sprintf(txt, "_%d", next ? _dotInstance+1 : _dotInstance);
	return _bhvr->getDotId() + "_" + _name + txt;
}

string CStep::getDotLabel() const
{
	return _bhvr->getQualifiedName() + "\\n" + _name;
}

bool CStep::isInternalMilestone() const
{
	return _milestone==1;
}

bool CStep::isCustomerMilestone() const
{
	return _milestone==2;
}


bool CStep::checkSmallestRequests (
	const vector<int>& nMaxRequests,
	CResourceVector& mins,
	int partNReqs, int partSize, int partAllSize,
	ILogger& logger)
{
	if (logger.verbose()>2) {
		logger.log("DEBUG", "\t");
	}

	for(unsigned r=0; r<nMaxRequests.size(); r++) {
		int rn = (*_resourceNeeds)[r];
		int val = rn;
		if (r==_cpu && partNReqs>0) {
			// this is the CPU where this step is running on ... and APS is being used
			// => compute the APS-based resource need of this step
			val *= partNReqs;
			// overflow check
			if (val<0) {
				logger.fatal("%s arithmetic overflow (rn=%d, partNReqs=%d, resource %d)\n",
						getQualifiedName().c_str(),
						rn, partNReqs, r);
				return false;
			}
			if (nMaxRequests[r] > 1) {
				val = CIntAccuracy::div(val, nMaxRequests[r]);
			}


			// we scale down everything by factor 10 to avoid overflow (only if necessary)
			static unsigned int valmax = 1000000; // corresponds to 100 msec cpu-load in one single step
			bool scaledown = false;
			if (val>=valmax) {
				if (partAllSize>99 || val>=5*valmax) {
					scaledown = true;
					val = CIntAccuracy::div(val, 10);
				}
			}
			val *= partAllSize;
			// overflow check
			if (val<0) {
				logger.fatal("%s arithmetic overflow (rn=%d, partNReqs=%d, partAllSize=%d, resource %d)\n",
						getQualifiedName().c_str(),
						rn, partNReqs, partAllSize, r);
				return false;
			}

			if (scaledown) {
				val = CIntAccuracy::div(val, partSize/10);
			} else {
				val = CIntAccuracy::div(val, partSize);
			}

		} else {
			// this is some other resource (should be no other CPU...), compute context-switching penalty (if any)
			if ((*_averageCSTVector)[r]>0 && nMaxRequests[r]>1) {
				// more than one request pending on this resource
				// => cst penalty has to be added

				// we scale down everything by factor 10 to avoid overflow
				int penalty = CIntAccuracy::div(val, 10) * (*_averageCSTVector)[r];

				// overflow check is still necessary...
				if (penalty<0) {
					logger.fatal("%s arithmetic overflow (resource %d)\n", getQualifiedName().c_str(), r);
					printf("\tval=%d cst[r]=%d\n", val, (*_averageCSTVector)[r]);
					return false;
				}
				val += CIntAccuracy::div(penalty, 100);
			}
		}
		if (val>0 && val < mins[r]) {
			mins[r] = val;
		}
		if (logger.verbose()>2) {
			printf(" %07d/%07d ", CIntAccuracy::toPrint((*_resourceNeeds)[r]), CIntAccuracy::toPrint(val));
			//printf(" %06d/%06d ", _resourceNeeds[r], val);
		}
	}

	if (logger.verbose()>2) {
		printf("\t%s\n", getQualifiedName().c_str());
	}

	return true;
}


bool CStep::hasResourceNeeds (void) const
{
	CResourceVector& rv = *_resourceNeeds;
	for(unsigned i=0; i<rv.size(); i++) {
		if (rv[i]>0) {
			return true;
		}
	}

	return false;
}


bool CStep::consume (int delta,
		const vector<bool>& isLimited,
		const vector<int>& nMaxRequests,
		int partNReqs, int partSize, int partAllSize,
		ILogger& logger)
{
	//printf("CStep::consume %s %d\n", getQualifiedName().c_str(), delta);

	// consume data from ResourceVector
	CResourceVector& rv = *_resourceNeeds;
	int remaining = 0;
	for(unsigned i=0; i<rv.size(); i++) {
		if (rv[i]>0) {
			int req = nMaxRequests[i];
			int dtNetto = 0;

			if (i==_cpu && partNReqs>0) {
				// this is the CPU where this step is running on ... and APS is being used
				// => compute the APS-based resource need of this step
				dtNetto = delta;
				dtNetto *= partSize;
				// overflow check
				if (dtNetto<0) {
					logger.fatal("%s arithmetic overflow (delta=%d, partSize=%d, cpu %d)\n",
							getQualifiedName().c_str(), delta, partSize, i);
					return false;
				}
				dtNetto = CIntAccuracy::div(dtNetto, partAllSize*partNReqs);
				//logger.log("APS", "step %s: res=%d delta=%d partAllSize=%d, partSize=%d, partNReqs=%d, dtNetto=%d\n",
				//		getQualifiedName().c_str(), i, delta, partAllSize, partSize, partNReqs, dtNetto);
			} else {
				int dt = isLimited[i] ? CIntAccuracy::div(delta, req) : delta;
				dtNetto = dt;
				int cstPer1000 = (*_averageCSTVector)[i];
				if (cstPer1000>0 && req>1) {
					// more than one client wants to access this resource, apply cst penalty

					// first operation with overflow check
					int dt1 = dt*100;
					if (dt1<0) {
						logger.fatal("%s: arithmetic overflow (dt=%d)\n",
									getQualifiedName().c_str(),
									CIntAccuracy::toPrint(dt)
								);
						return false;
					}

					dtNetto = CIntAccuracy::div(dt1, 1000+cstPer1000) * 10;
					// we implicitly rounded the last digit of dtNetto, add 5 to compensate this
					if (dtNetto<10) {
						dtNetto += 5;
					}
					if (logger.verbose() > 2) {
						logger.log("DEBUG", "%s: cst penalty, res %d: dt=%5d dtNetto=%5d cts=%5d\n",
									getQualifiedName().c_str(),
									i,
									CIntAccuracy::toPrint(dt),
									CIntAccuracy::toPrint(dtNetto),
									cstPer1000
								);
					}
				}
			}

			// remove this portion from current resource needs
			if (rv[i] >= dtNetto) {
				rv[i] -= dtNetto;
			} else {
				rv[i] = 0;
			}

			if (rv[i]>0) {
				remaining++;
			}
		}
	}

	return remaining==0;
}


void CStep::exitActions (ISimEventAcceptor& eventAcceptor, ILogger& logger)
{
	// all has been consumed => tell successors that we are ready
	for (Successors::iterator it = _successors.begin(); it!=_successors.end(); it++) {
		(*it)->done(this, eventAcceptor);
	}

	// if this is the last step in one behaviour, also tell behaviour that we are ready
	if (_isLast) {
		_bhvr->lastStepDone(this, eventAcceptor, logger);
	}
}

