/*
 * CResource.h
 *
 *  Created on: 05.08.2009
 *      Author: kbirken
 */

#ifndef WARP_MODEL_RESOURCE_H_
#define WARP_MODEL_RESOURCE_H_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace warp {
namespace model {

	class Resource {
	public:
		typedef vector<shared_ptr<Resource> > Vector;

		typedef enum {
			SCHED_PLAIN = 0,
			SCHED_APS = 1,
			SCHED_MAX
		} Scheduling;

		Resource(const char* name, bool isLimited);
		Resource(const char* name, vector<int> cst, Scheduling scheduling);
		Resource(istream &in);
		virtual ~Resource();

		const char* getName() const  { return _name.c_str(); }
		bool isLimited() const  { return _isLimited; }
		bool isSchedulerAPS() const  { return _scheduling==SCHED_APS; }

		// additional slots may be induced by multiple interfaces (for non-CPUs) or >1 partitions (for CPUs)
		unsigned int getNSlots() const;

		unsigned int getNInterfaces() const;
		int getCST (int i) const  { return _contextSwitchingTimes[i]; }

		unsigned int getNPartitions() const;
		string getPartitionName(int i) const  { return _partitionNames[i]; }
		int getPartitionSize(int i) const  { return _partitionSizes[i]; }
		int getWholeSize() const { return _wholeSize; }

	private:
		string _name;
		bool _isLimited;
		Scheduling _scheduling;

		// for each ResourceInterface, there is one entry in the cst vector (for non-CPUs only)
		vector<int> _contextSwitchingTimes;

		// for each partition, there is one entry in the following vectors (for CPUs only)
		vector<string> _partitionNames;
		vector<int> _partitionSizes;
		int _wholeSize;  // whole CPU size (sum of all partitionSizes)
	};

} /* namespace model */
} /* namespace warp */

#endif /* WARP_MODEL_RESOURCE_H_ */
