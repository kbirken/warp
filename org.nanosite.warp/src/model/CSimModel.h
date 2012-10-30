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
#include "model/CResource.h"
#include "model/CPoolVector.h"


class CSimModel {
public:
	CSimModel();
	virtual ~CSimModel();

	bool readFile (const char* modelFilename, bool verbose);

	int getNResources() const  { return _resources.size(); }
	const CResource::Vector& getResources() const  { return _resources; }

	int getNSlots() const  { return _slots.size(); }
	const CResource::Vector& getResourceSlots() const  { return _slots; }
	CPoolVector& getPools()  { return _pools; }

	// inject initial steps into simulation engine
	void addInitials (ISimEventAcceptor& eventAcceptor, ILogger& logger);

	// final checks
	int checkRemnants (void) const;

private:
	CBehaviour* getBehaviour (unsigned int fbIndex, int bhvrIndex) const;

	static void err (const char* fmt, ...);

private:
	// list of CResources, we are responsible for its memory
	CResource::Vector _resources;

	// a ResourceSlot is a request from a resource (or its ResourceInterface)
	CResource::Vector _slots;

	// resource pools
	CPoolVector _pools;

	// list of CFunctionBlocks, we are responsible for its memory
	CFunctionBlock::Vector _fbs;

	// list of CSteps, we are responsible for its memory
	CStep::Vector _steps;

	// subset of CBehaviours containing all initially triggered behaviours
	CBehaviour::Vector _initials;
};


#endif /* SIMMODEL_H_ */
