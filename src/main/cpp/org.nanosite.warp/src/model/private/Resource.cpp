/*
 * Resource.cpp
 *
 *  Created on: 05.08.2009
 *      Author: kbirken
 */

#include "model/Resource.h"

namespace warp {
namespace model {

Resource::Resource(const char* name, bool isLimited) :
	_name(name),
	_isLimited(isLimited),
	_scheduling(SCHED_PLAIN),
	_wholeSize(0)
{
}

Resource::Resource(const char* name, vector<int> cst, Scheduling scheduling) :
	_name(name),
	_isLimited(true),
	_scheduling(scheduling),
	_wholeSize(0)
{
	// for cpus, nIfs will be 0
	// read list of context switching times (one for each resource interface)
	for(int ri=0; ri<cst.size(); ri++) {
		_contextSwitchingTimes.push_back(cst[ri]);
	}

	// TODO: handle additional configuration for scheduding!=SCHED_PLAIN (see constructor below)
}


Resource::Resource (istream &in) :
	_isLimited(true)  // resources read from file are limited by definition
{
	int nIfs;
	in >> _name >> nIfs;

	// for cpus, nIfs will be 0
	// read list of context switching times (one for each resource interface)
	for(int ri=0; ri<nIfs; ri++) {
		int cst;
		in >> cst;
		_contextSwitchingTimes.push_back(cst);
	}

	int sched;
	in >> sched;
	if (sched>=0 && sched<SCHED_MAX) {
		_scheduling = (Resource::Scheduling)sched;

		if (_scheduling!=SCHED_PLAIN) {
			// this is only valid for CPUs, other resources cannot (currently?) have partitions
			int nPartitions;
			in >> nPartitions;
			_wholeSize = 0;
			for(int pi=0; pi<nPartitions; pi++) {
				string partName;
				unsigned int partSize;
				in >> partName >> partSize;

				printf("Resource %s: partition %d/%d: %s\t%05d\n",
						_name.c_str(),
						pi, nPartitions,
						partName.c_str(), partSize
				);
				_partitionNames.push_back(partName);
				_partitionSizes.push_back(partSize);
				_wholeSize += partSize;
			}

			if (nPartitions==0) {
				// silently correct this, no APS without partitions
				_wholeSize = 1000;
				_partitionNames.push_back("ALL");
				_partitionSizes.push_back(_wholeSize);
			}
		}
	}
}

Resource::~Resource()
{
}


unsigned int Resource::getNSlots() const
{
	unsigned int n = _contextSwitchingTimes.size();
	return n==0 ? 1 : n;
}


unsigned int Resource::getNInterfaces() const
{
	return _contextSwitchingTimes.size();
}


unsigned int Resource::getNPartitions() const
{
	unsigned int n = _partitionSizes.size();
	return n==0 ? 1 : n;
}


} /* namespace model */
} /* namespace warp */

