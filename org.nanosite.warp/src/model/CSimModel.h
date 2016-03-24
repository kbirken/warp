/*
 * CSimModel.h
 *
 *  Created on: 04.08.2009
 *      Author: kbirken
 */

#ifndef SIMMODEL_H_
#define SIMMODEL_H_

#include <stdarg.h>

#include "model/CFunctionBlock.h"
#include "model/CStep.h"
#include "model/Resource.h"
#include "sim/PoolSimVector.h"

namespace warp {
namespace model {

class CSimModel {
public:
	CSimModel();
	virtual ~CSimModel();

	bool readFile(const char* modelFilename, bool verbose);

	void init();
	void addResource(shared_ptr<Resource> res);
	void phase2();
	void addFunctionBlock(CFunctionBlock* fb);
	void addInitial(CBehavior* bhvr);
	void addStep(CStep* step);
	void finalize();

	int getNResources() const  { return _resources.size(); }
	const Resource::Vector& getResources() const  { return _resources; }

	int getNSlots() const  { return _slots.size(); }
	const Resource::Vector& getResourceSlots() const  { return _slots; }
	sim::PoolSimVector& getPools()  { return _pools; }

	// inject initial steps into simulation engine
	void addInitials (ISimEventAcceptor& eventAcceptor, ILogger& logger);

	// final checks
	int checkRemnants (void) const;

private:
	CBehavior* getBehavior (unsigned int fbIndex, int bhvrIndex) const;

	static void err (const char* fmt, ...);

private:
	// list of CResources, we are responsible for its memory
	Resource::Vector _resources;

	// a ResourceSlot is a request from a resource (or its ResourceInterface)
	Resource::Vector _slots;

	// resource pools
	sim::PoolSimVector _pools;

	// list of CFunctionBlocks, we are responsible for its memory
	CFunctionBlock::Vector _fbs;

	// list of CSteps, we are responsible for its memory
	CStep::Vector _steps;

	// subset of CBehaviors containing all initially triggered behaviors
	CBehavior::Vector _initials;
};

} /* namespace model */
} /* namespace warp */

#endif /* SIMMODEL_H_ */
