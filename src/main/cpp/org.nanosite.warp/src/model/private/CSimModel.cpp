/*
 * CSimModel.cpp
 *
 *  Created on: 2009-08-04
 *      Author: kbirken
 */

#include "model/CBehavior.h"
#include "model/CSimModel.h"

#include "model/Pool.h"
#include "model/CStep.h"
#include "sim/PoolSim.h"
#include "simulation/ISimEventAcceptor.h"
#include "simulation/CTokenFactory.h"
#include "simulation/CIntAccuracy.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
using namespace std;

namespace warp {
namespace model {

// ***********************************************************************

CSimModel::CSimModel()
{
}


CSimModel::~CSimModel()
{
	// clean-up
	for (CStep::Vector::iterator it2 = _steps.begin(); it2!=_steps.end(); it2++) {
		delete(*it2);
	}
	for (CFunctionBlock::Vector::iterator it3 = _fbs.begin(); it3!=_fbs.end(); it3++) {
		delete(*it3);
	}
}


// ***********************************************************************

bool CSimModel::readFile (const char* modelFilename, bool verbose)
{
	ifstream in(modelFilename);
	if (!in) {
		err("Cannot open model file");
		return false;
	}

	// read version info
	static const int myVersion = 9;
	int version;
	in >> version;
	if (version!=myVersion) {
		err("unsupported version - file version is %d, expected %d", version, myVersion);
		return false;
	}

	// do basic initialization
	init();

	// read list of resources
	int nRes;
	in >> nRes;
	if (nRes<0 || nRes>100) {
		err("format error - invalid number of resources");
		return false;
	}

	printf("input file version %d - reading %d resources...\n", version, nRes);

	// add remaining resources
	for(int i=0; i<nRes; i++) {
		// plausibility check
		if (in.eof()) {
			err("premature end of file - resource definition expected");
			return false;
		}

		// NB: cpu id as given for each FunctionBlock later in file must be identical to index in _resources vector
		shared_ptr<Resource> res(new Resource(in));
		addResource(res);
	}

	phase2();

	// read list of pools
	int nPools;
	in >> nPools;
	if (nPools<0 || nPools>100) {
		err("format error - invalid number of resource pools");
		return false;
	}

	printf("reading %d resource pools...\n", nPools);

	for(int i=0; i<nPools; i++) {
		if (in.eof()) {
			err("premature end of file - resource pool definition expected");
			return false;
		}

		string poolName;
		int maxAmount;
		in >> poolName >> maxAmount;
		//printf("Pool %s has maximum amount of %d\n", poolName.c_str(), maxAmount);
		Pool *pool = new Pool(poolName.c_str(), maxAmount);
		sim::PoolSim *poolSim = new sim::PoolSim(*pool);
		_pools.push_back(poolSim);
	}


	// read list of function blocks and behaviors
	int nFBs;
	in >> nFBs;
	if (nFBs<0 || nFBs>1000) {
		err("format error - invalid number of function blocks");
		return false;
	}

	printf("reading %d function blocks...\n", nFBs);

	typedef pair<CBehavior*,unsigned int> UnlessCond;
	typedef vector<UnlessCond> UnlessVector;
	UnlessVector behaviorsWithUnless;
	for(int i=0; i<nFBs; i++) {
		if (in.eof()) {
			err("premature end of file - function block definition expected");
			return false;
		}

		string fbName;
		int cpu, partition, nBehaviors;
		in >> fbName >> cpu >> partition >> nBehaviors;
		if (nBehaviors<0 || nBehaviors>1000) {
			err("format error - invalid number of behavior");
			return false;
		}

		// partition counting in input file starts with 1, we start with 0 internally
		CFunctionBlock* fb = new CFunctionBlock(fbName, cpu, partition-1);
		_fbs.push_back(fb);

		// read behaviors for this fb
		for(int j=0; j<nBehaviors; j++) {
			if (in.eof()) {
				err("premature end of file - behavior definition expected");
				return false;
			}

			string bhvrName;
			int tok, type;
			int p = 0;

			in >> bhvrName >> tok >> type;
			if (type!=CBehavior::LOOP_TYPE_ONCE) {
				in >> p;
			}
			bool addToken = (tok>0);
			CBehavior* bhvr = new CBehavior(*fb, bhvrName, type, p, addToken);
			fb->addBehavior(bhvr);

			if (type==CBehavior::LOOP_TYPE_UNLESS) {
				behaviorsWithUnless.push_back(make_pair(bhvr, p));
			}
		}
	}

	// read list of steps
	int nSteps;
	in >> nSteps;
	if (nSteps<1 || nSteps>10000) {
		err("format error - invalid number of steps");
		return false;
	}

	int fbiLast = -1;
	int bhvriLast = -1;
	printf("reading %d steps...\n", nSteps);
	CStep* previousStep = NULL;
	for(int i=0; i<nSteps; i++) {
		if (in.eof()) {
			err("premature end of file - step definition expected");
			return false;
		}

		int id, fbi, bhvri, milestoneType;
		string s;
		in >> id >> fbi >> bhvri >> s >> milestoneType;

		if (i!=id) {
			err("format error - step id doesn't fit to index");
			return false;
		}

		if (milestoneType<0 || milestoneType>2) {
			err("format error - milestone type out of range (should be one of 0,1,2)");
			return false;
		}

		CBehavior* bhvr = getBehavior(fbi, bhvri);
		if (bhvr==0) {
			err("format error - invalid behavior id in definition of steps");
			return false;
		}
		CFunctionBlock* fb = _fbs[fbi];

		// read resource slots and cumulate them to resources
		vector<int> values;
		vector<int> averageCSTs;
		for(int r=0; r<getNResources(); r++) {
			const Resource& res = *(_resources[r]);
			//printf("  reading resource %d / %s\n", r, res.getName());

			int rv=0, cst=0;
			int n = res.getNSlots();
			int nRIs = res.getNInterfaces();
			//printf("    n=%d nRIs=%d\n", n, nRIs);
			for(int ri=0; ri<n; ri++) {
				int v;
				in >> v;

				rv += v;
				//printf("    ri=%d rv=%d v=%d\n", ri, rv, v);
				if (nRIs>0) {
					int cstRI = v * res.getCST(ri);
					//printf("     res.getCST(ri)=%d cstRI=%d\n", res.getCST(ri), cstRI);
					cst += cstRI;
					//printf("     cst=%d\n", cst);
					/*
					if (cst>0) {
						printf("  r=%d ri=%d n=%d v=%d rv=%d getCST=%d cst=%d\n",
								r, ri, n, v, rv, _resources[r]->getCST(ri), cst);
					}
					*/
					// check for overflow
					if (cstRI<0 || cst<0) {
						err("overflow error while computing average CST");
						return false;
					}
				}
			}
			values.push_back(CIntAccuracy::toCalc(rv));

			// CST is averaged across all IOActivities on one resource
			int averageCST = rv>0 ? (cst / rv) : 0;
			/*
			if (averageCST>0) {
				printf("averageCST(%s):\t%d\n", _resources[r]->getName(), averageCST);
			}
			*/
			averageCSTs.push_back(averageCST);
		}

		// read alloc/free amounts from pools
		sim::PoolSimVector::Values poolVals;
		for(int poolId=0; poolId<nPools; poolId++) {

			// will be <0 for free() and >0 for alloc()
			int poolAmount;
			in >> poolAmount;
			poolVals.push_back(poolAmount);
		}

		// create CStep and corresponding CResourceVector
		CStep* step =
				new CStep(id, *bhvr, s, milestoneType, fb->getCPU(), fb->getPartition(),
						new CResourceVector(values),
						new CResourceVector(averageCSTs),
						poolVals
				);
		if (verbose) {
			step->print();
		}
		_steps.push_back(step);
		bhvr->addStep(step);

		// set successor for previous step (if same FB and behavior)
		if (previousStep) {
			if (fbi==fbiLast && bhvri==bhvriLast) {
				previousStep->addSuccessor(step);
			} else {
				previousStep->setIsLast();
				step->setIsFirst();
			}
		}
		previousStep = step;
		fbiLast = fbi;
		bhvriLast = bhvri;
	}
	if (previousStep) {
		previousStep->setIsLast();
	}

	// steps have been read, now set unless conditions for some behaviors
	for(vector<UnlessCond>::const_iterator i=behaviorsWithUnless.begin(); i!=behaviorsWithUnless.end(); ++i) {
		CBehavior* bhvr = i->first;
		unsigned int stepIdx = i->second;
		if (stepIdx >= _steps.size()) {
			err("format error - invalid 'unless' condition %d for behavior %s",
					stepIdx, bhvr->getQualifiedName().c_str());
			return false;
		}
		CStep* step = _steps[stepIdx];
		//printf("unlessCond for behavior %s is %s\n", bhvr->getQualifiedName().c_str(), step->getQualifiedName().c_str());
		bhvr->setUnlessCondition(step);
	}

	// read list of initial triggers
	int nInitials;
	in >> nInitials;
	if (nInitials<1 || nInitials>100) {
		err("format error - invalid number of initial triggers");
		return false;
	}

	for(int i=0; i<nInitials; i++) {
		if (in.eof()) {
			err("premature end of file - initial trigger definition expected");
			return false;
		}

		int fbi, bhvri;
		in >> fbi >> bhvri;

		CBehavior* bhvr = getBehavior(fbi, bhvri);
		if (bhvr==0) {
			err("format error - invalid fb/behavior id in definition of initial triggers");
			return false;
		}

		_initials.push_back(bhvr);
	}

	// read list of send triggers for each behavior
	int nBhvr;
	in >> nBhvr;
	if (nBhvr<0 || nBhvr>1000) {
		err("format error - invalid number of behaviors in send triggers list");
		return false;
	}

	printf("reading send triggers for %d behaviors...\n", nBhvr);
	for(int i=0; i<nBhvr; i++) {
		if (in.eof()) {
			err("premature end of file - send trigger definition expected");
			return false;
		}

		int fbi, bhvri, nSends;
		in >> fbi >> bhvri >> nSends;

		CBehavior* bhvr = getBehavior(fbi, bhvri);
		if (bhvr==0) {
			err("format error - invalid fb/behavior id in send triggers list");
			return false;
		}

		if (nSends<0 || nSends>100) {
			err("format error - invalid number of sends in send triggers list");
			return false;
		}

		if (verbose) {
			if (nSends>0) {
				printf("  %s triggers:\n", bhvr->getQualifiedName().c_str());
			}
		}
		for(int j=0; j<nSends; j++) {
			int fbi2, bhvri2;
			in >> fbi2 >> bhvri2;

			CBehavior* bhvr2 = getBehavior(fbi2, bhvri2);
			if (bhvr2==0) {
				err("format error - invalid target fb/behavior id in send triggers list");
				return false;
			}

			bhvr->addSendTrigger(bhvr2);
			if (verbose) {
				printf("      %s\n", bhvr2->getQualifiedName().c_str());
			}
		}
	}

	// read list of preconditions
	int id;
	do {
		// plausibility check
		if (in.eof()) {
			err("premature end of file - step id expected");
			return false;
		}

		in >> id;
		if (id>=0) {
			int nPreconditions;
			in >> nPreconditions;

			//printf("%4d <= ", id);
			int nSteps = _steps.size();
			for(int p=0; p<nPreconditions; p++) {
				int pre;
				in >> pre;
				// plausibility check
				if (pre<0 || pre>nSteps) {
					err("format error - invalid definition of precondition %d for step %d", p, id);
					return false;
				}

				//printf("\t%3d", pre);
				_steps[pre]->addSuccessor(_steps[id]);
			}
			//printf("\n");
		}
	} while (id>=0);

	finalize();

	return true;
}


void CSimModel::addInitials (ISimEventAcceptor& eventAcceptor, ILogger& logger)
{
	for(CBehavior::Vector::iterator it = _initials.begin(); it!=_initials.end(); it++) {
		CBehavior* bhvr = *it;

		CToken* tok = CTokenFactory::instance()->createToken(bhvr->getQualifiedName());
		CMessage* msg = new CMessage(tok);
		bhvr->receiveTrigger(0, msg, eventAcceptor, logger);
	}
}


// ***********************************************************************

void CSimModel::init() {
	// add artificial resource for "wait time"
	// (this resource can be used unlimited)
	shared_ptr<Resource> res(new Resource("wait", false));
	_resources.push_back(res);
	_slots.push_back(res);
}

void CSimModel::addResource(shared_ptr<Resource> res) {
	// add to resources list
	_resources.push_back(res);

	// add slot(s) for this resource
	int n = res->getNSlots();
	for(int j=0; j<n; j++) {
		_slots.push_back(res);
	}
}

void CSimModel::phase2() {
	// TODO: this global setting is dangerous and should be replaced
	CResourceVector::setNResources(getNResources());
	printf("nResources=%d\n", getNResources());
}

void CSimModel::addFunctionBlock(CFunctionBlock* fb) {
	// add to list of function blocks
	_fbs.push_back(fb);
}

void CSimModel::addInitial(CBehavior* bhvr) {
	_initials.push_back(bhvr);
}

void CSimModel::addStep(CStep* step) {
	// add to list of steps
	_steps.push_back(step);
}

void CSimModel::finalize() {
	// prepare all FunctionBlocks for this execution
	for(unsigned int i=0; i<_fbs.size(); i++) {
		_fbs[i]->prepareExecution();
	}
}

// ***********************************************************************

int CSimModel::checkRemnants() const
{
	int nRemnants = 0;
	for (CFunctionBlock::Vector::const_iterator it = _fbs.begin(); it!=_fbs.end(); it++) {
		const CBehavior::Vector& bhvrs = (*it)->getBehaviors();
		for (CBehavior::Vector::const_iterator bhvr = bhvrs.begin(); bhvr!=bhvrs.end(); bhvr++) {
			if (! (*bhvr)->hasFinishedOnce()) {
				printf("Behavior hasn't been executed: %s.\n", (*bhvr)->getQualifiedName().c_str());
				nRemnants++;
			}
		}
	}

	return nRemnants;
}


// ***********************************************************************

CBehavior* CSimModel::getBehavior (unsigned int fbIndex, int bhvrIndex) const
{
	if (fbIndex>=_fbs.size()) {
		return 0;
	}
	CFunctionBlock* fb = _fbs[fbIndex];

	CBehavior* bhvr = fb->getBehavior(bhvrIndex);
	return bhvr;
}


void CSimModel::err (const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	printf("ERROR: ");

	/*int retval = */vprintf(fmt, ap);
	va_end(ap);

	printf("!\n");
}


// ***********************************************************************

} /* namespace model */
} /* namespace warp */
