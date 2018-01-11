// CFunctionBlock: implementation of the CFunctionBlock class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//#include <algorithm>
#include <string>
using namespace std;

#include "model/CFunctionBlock.h"
#include <model/CBehavior.h>

namespace warp {

CFunctionBlock::CFunctionBlock (string name, unsigned int cpu, unsigned int partition) :
	_name(name),
	_cpu(cpu),
	_partition(partition)
{
}


CFunctionBlock::~CFunctionBlock()
{
	for (CBehavior::Vector::iterator it = _behaviors.begin(); it!=_behaviors.end(); it++) {
		delete(*it);
	}
}


int CFunctionBlock::compare(const CFunctionBlock &other) const {
	int name = _name.compare(other._name);
	if (name!=0)
		return name;

	return 0;
}


void CFunctionBlock::addBehavior (CBehavior* bhvr)
{
	//printf("succ %s => %s\n", getQualifiedName().c_str(), step->getQualifiedName().c_str());
	_behaviors.push_back(bhvr);
}


CBehavior* CFunctionBlock::getBehavior (unsigned int i)
{
	if (i>=_behaviors.size()) {
		return 0;
	}

	return _behaviors[i];
}


void CFunctionBlock::prepareExecution (void)
{
	//printf("CFunctionBlock::prepareExecution %s\n", _name.c_str());

	// prepare all steps for this execution
	for(unsigned int i=0; i<_behaviors.size(); i++) {
		_behaviors[i]->prepareExecution();
	}
}


void CFunctionBlock::print() const
{
	printf("FB %s\n", _name.c_str());
	//printf("%4d %s\n\t\t", _id, getQualifiedName().c_str());
}

} /* namespace warp */
