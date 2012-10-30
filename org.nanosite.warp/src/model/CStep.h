// Step.h: interface for the CStep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STEP_H_INCLUDED_)
#define _STEP_H_INCLUDED_

#include <string>
#include <vector>
using namespace std;


#include "simulation/CResourceSlotVector.h"
#include "simulation/IStepSuccessor.h"
#include "model/CPoolVector.h"

// forward decl
class ISimEventAcceptor;
class ILogger;
class CBehaviour;


class CStep : public IStepSuccessor {
public:
	typedef vector<CStep*> Vector;

	CStep(int id,
			CBehaviour* bhvr,
			string name,
			int milestone,
			unsigned int cpu,
			unsigned int partition,
			CResourceVector* rv,
			CResourceVector* averageCST,
			const CPoolVector::Values& poolRequests);

	virtual ~CStep();

	const CBehaviour* getBehaviour (void) const;
	unsigned int getCPU() const  { return _cpu; }
	unsigned int getPartition() const  { return _partition; }

	void setIsFirst()  { _isFirst = true; }
	void setIsLast()  { _isLast = true; }
	bool isLast() const  { return _isLast; }

	void addSuccessor (CBehaviour* behaviour);
	void addSuccessor (CStep* step);

	const CResourceVector& getCurrentResourceNeeds() const  { return *_resourceNeeds; }
	//const CResourceVector& getAverageCSTVector() const  { return *_averageCSTVector; }

	// prepare for next execution
	void prepareExecution();

	// triggers (called by predecessor steps)
	void waitFor (CStep* step);

	// IStepSuccessor interface
	void done (const CStep* step, ISimEventAcceptor& acceptor);

	bool isWaiting()  { return _waitFor.size()>0; }

	const CPoolVector::Values& getPoolRequests() const  { return _poolRequests; }

	string getQualifiedName() const;

	// some helpers for drawing graphviz diagrams
	void nextDotInstance()  { _dotInstance++; }
	string getPrevDotId () const;
	string getDotId (bool next=false) const;
	string getDotLabel() const;
	bool isInternalMilestone() const;
	bool isCustomerMilestone() const;

	bool checkSmallestRequests (
			const vector<int>& nMaxRequests,
			CResourceVector& mins,
			int partNReqs, int partSize, int partAllSize,
			ILogger& logger);

	bool hasResourceNeeds() const;

	bool consume (int delta,
			const vector<bool>& isLimited,
			const vector<int>& nMaxRequests,
			int partNReqs, int partSize, int partAllSize,
			ILogger& logger);

	void exitActions (ISimEventAcceptor& eventAcceptor, ILogger& logger);

	void print() const;
	void printWaitingList() const;

private:
	int _id;
	CBehaviour* _bhvr;
	string _name;
	int _milestone;
	unsigned int _cpu;  // same as in this step's functionblock
	unsigned int _partition;  // same as in this step's functionblock
	bool _isFirst, _isLast;
	CResourceVector* _initialResourceNeeds;
	CResourceVector* _averageCSTVector;
	CPoolVector::Values _poolRequests;

	typedef vector<IStepSuccessor*> Successors;
	Successors _successors;

	CStep* _directPredecessor;

	int _dotInstance;

	// this data is changed when the step is executed
	CResourceVector* _resourceNeeds;
	Vector _waitFor;
};


#endif // !defined(_STEP_H_INCLUDED_)
