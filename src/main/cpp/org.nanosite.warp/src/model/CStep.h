// Step.h: interface for the CStep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STEP_H_INCLUDED_)
#define _STEP_H_INCLUDED_

#include <string>
#include <vector>
#include <map>
using namespace std;


#include "simulation/CResourceSlotVector.h"
#include "simulation/IStepSuccessor.h"
#include "sim/PoolSimVector.h"


// forward declarations
class ILogger;

namespace warp {

	// forward declarations
	class CBehavior;
	class ISimEventAcceptor;

	class CStep : public IStepSuccessor {
	public:
		typedef vector<CStep*> Vector;

		CStep(int id,
				/*const*/ CBehavior& bhvr,
				string name,
				int milestone,
				unsigned int cpu,
				unsigned int partition,
				CResourceVector* rv,
				CResourceVector* averageCST,
				const sim::PoolSimVector::Values& poolRequests);

		virtual ~CStep();

		int getID() const;

		const CBehavior& getBehavior (void) const;
		unsigned int getCPU() const  { return _cpu; }
		unsigned int getPartition() const  { return _partition; }

		void setIsFirst()  { _isFirst = true; }
		void setIsLast()  { _isLast = true; }
		bool isLast() const  { return _isLast; }

		void addSuccessor (CBehavior* behavior);
		void addSuccessor (CStep* step);

		const CResourceVector& getCurrentResourceNeeds() const  { return *_resourceNeeds; }
		//const CResourceVector& getAverageCSTVector() const  { return *_averageCSTVector; }

		// prepare for next execution
		void prepareExecution();

		// triggers (called by predecessor steps)
		void waitFor (CStep* step);

		// IStepSuccessor interface
		void done(const CStep* step, ISimEventAcceptor& acceptor);

		bool isWaiting()  { return _waitFor.size()>0; }

		const sim::PoolSimVector::Values& getPoolRequests() const  { return _poolRequests; }

		string getQualifiedName() const;

		// some helpers for drawing graphviz diagrams
		void nextDotInstance()  { _dotInstance++; }
		string getPrevDotId() const;
		string getDotId(bool next=false) const;
		string getDotLabel() const;
		bool isInternalMilestone() const;
		bool isCustomerMilestone() const;

		bool checkSmallestRequests(
				const vector<int>& nMaxRequests,
				CResourceVector& mins,
				int partNReqs, int partSize, int partAllSize,
				ILogger& logger);

		bool hasResourceNeeds() const;

		bool consume(int delta,
				const vector<bool>& isLimited,
				const vector<int>& nMaxRequests,
				int partNReqs, int partSize, int partAllSize,
				ILogger& logger);

		void exitActions(ISimEventAcceptor& eventAcceptor, ILogger& logger);

		void print() const;
		void printWaitingList() const;

		void storePoolState(int index, int amount, bool overflow, bool underflow);
		bool usedPool(int index);
		int getPoolUsage(int index);
		bool getPoolOverflow(int index);
		bool getPoolUnderflow(int index);

	private:
		int _id;
		/*const*/ CBehavior& _bhvr;
		string _name;
		int _milestone;
		unsigned int _cpu;  // same as in this step's functionblock
		unsigned int _partition;  // same as in this step's functionblock
		bool _isFirst, _isLast;
		CResourceVector* _initialResourceNeeds;
		CResourceVector* _averageCSTVector;
		sim::PoolSimVector::Values _poolRequests;

		typedef vector<IStepSuccessor*> Successors;
		Successors _successors;

		CStep* _directPredecessor;

		int _dotInstance;

		// this data is changed when the step is executed
		CResourceVector* _resourceNeeds;
		Vector _waitFor;

		// state of a pool after execution of this step
		struct PoolState {
			int _amount;
			bool _overflow;
			bool _underflow;

			PoolState() : _amount(0), _overflow(false), _underflow(false) { }
			PoolState(int amount, bool overflow, bool underflow) :
				_amount(amount),
				_overflow(overflow),
				_underflow(underflow)
			{ }
		};

		// we only store the last state for each pool
		// the map key is the global pool's index
		typedef map<int, PoolState> PoolStates;
		PoolStates _poolStates;
	};

} /* namespace warp */

#endif // !defined(_STEP_H_INCLUDED_)
