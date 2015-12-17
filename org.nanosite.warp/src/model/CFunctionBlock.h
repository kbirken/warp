// CFunctionBlock.h: interface for the CFunctionBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_FUNCTION_BLOCK_H_INCLUDED_)
#define _FUNCTION_BLOCK_H_INCLUDED_

#include <string>
#include <vector>
using namespace std;

#include "model/CBehaviour.h"

// forward decl
//class ISimEventAcceptor;

class CFunctionBlock
{
public:
	typedef vector<CFunctionBlock*> Vector;

	CFunctionBlock(string name, unsigned int cpu, unsigned int partition);
	virtual ~CFunctionBlock();

	int compare(const CFunctionBlock &other) const;
	bool operator==(const CFunctionBlock &other) const { return compare(other)==0; }
	bool operator!=(const CFunctionBlock &other) const { return compare(other)!=0; }

	void addBehaviour (CBehaviour* bhvr);
	CBehaviour* getBehaviour (unsigned int i);
	const CBehaviour::Vector& getBehaviours() const  { return _behaviours; };

	void prepareExecution (void);

	unsigned int getCPU() const  { return _cpu; }
	unsigned int getPartition() const  { return _partition; }
	string getName() const  { return _name; }
	string getDotId() const  { return _name; }

	void print() const;

private:
	string _name;
	unsigned int _cpu;
	unsigned int _partition;

	// list of CBehaviours, we are responsible for its memory
	CBehaviour::Vector _behaviours;
};


#endif // !defined(_FUNCTION_BLOCK_H_INCLUDED_)
