// CFunctionBlock: implementation of the CFunctionBlock class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//#include <algorithm>
#include <string>
using namespace std;

#include "model/CFunctionBlock.h"
#include "model/CBehaviour.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFunctionBlock::CFunctionBlock (string name, unsigned int cpu, unsigned int partition) :
	_name(name),
	_cpu(cpu),
	_partition(partition)
{
}


CFunctionBlock::~CFunctionBlock()
{
	for (CBehaviour::Vector::iterator it = _behaviours.begin(); it!=_behaviours.end(); it++) {
		delete(*it);
	}
}


int CFunctionBlock::compare(const CFunctionBlock &other) const {
	int name = _name.compare(other._name);
	if (name!=0)
		return name;

	return 0;
}


void CFunctionBlock::addBehaviour (CBehaviour* bhvr)
{
	//printf("succ %s => %s\n", getQualifiedName().c_str(), step->getQualifiedName().c_str());
	_behaviours.push_back(bhvr);
}


CBehaviour* CFunctionBlock::getBehaviour (unsigned int i)
{
	if (i>=_behaviours.size()) {
		return 0;
	}

	return _behaviours[i];
}


void CFunctionBlock::prepareExecution (void)
{
	//printf("CFunctionBlock::prepareExecution %s\n", _name.c_str());

	// prepare all steps for this execution
	for(unsigned int i=0; i<_behaviours.size(); i++) {
		_behaviours[i]->prepareExecution();
	}
}


void CFunctionBlock::print() const
{
	printf("FB %s\n", _name.c_str());
	//printf("%4d %s\n\t\t", _id, getQualifiedName().c_str());
}
