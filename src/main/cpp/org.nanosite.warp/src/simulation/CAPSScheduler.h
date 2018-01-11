/*
 * CAPSScheduler.h
 *
 *  Created on: 12.04.2010
 *      Author: KBirken
 */

#ifndef CAPSSCHEDULER_H_
#define CAPSSCHEDULER_H_

#include <vector>
using namespace std;

namespace warp {

// forward declarations
namespace model {
	class Resource;
}

class CAPSScheduler
{
public:
	CAPSScheduler(const model::Resource& res);
	virtual ~CAPSScheduler();

	void clear();
	void addRequest(int i);

	int getPartitionSize(int i) const;

	int getUsedPartitions() const  { return _nUsedPartitions; }
	int getUsedPartitionsSize() const  { return _usedPartitionsSize; }
	int getNReqPerPartition(int i)  const  { return _nReqPerPartition[i]; }

private:
	const model::Resource& _resource;

	// data changed during each iteration
	int _nUsedPartitions;
	int _usedPartitionsSize;
	int _nRequests;
	vector<int> _nReqPerPartition;

};

} /* namespace warp */

#endif /* CAPSSCHEDULER_H_ */
