/*
 * CAPSScheduler.cpp
 *
 *  Created on: 12.04.2010
 *      Author: KBirken
 */

#include "simulation/CAPSScheduler.h"
#include "model/Resource.h"

namespace warp {

CAPSScheduler::CAPSScheduler(const warp::model::Resource& res) :
	_resource(res),
	_nUsedPartitions(0),
	_usedPartitionsSize(0),
	_nRequests(0)
{
	_nReqPerPartition.resize(res.getNPartitions());
}


CAPSScheduler::~CAPSScheduler()
{
}


void CAPSScheduler::clear()
{
	_nUsedPartitions = 0;
	_usedPartitionsSize = 0;
	_nRequests = 0;

	for(vector<int>::iterator i=_nReqPerPartition.begin(); i!=_nReqPerPartition.end(); ++i) {
		*i = 0;
	}
}


void CAPSScheduler::addRequest (int i)
{
	if (_nReqPerPartition[i]==0) {
		// first request for this partition, add up partitionSizes
		_usedPartitionsSize += _resource.getPartitionSize(i);
		_nUsedPartitions++;
	}
	_nRequests++;
	_nReqPerPartition[i]++;
}


int CAPSScheduler::getPartitionSize(int i) const
{
	return _resource.getPartitionSize(i);
}

} /* namespace warp */
