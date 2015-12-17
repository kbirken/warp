// CBehavior: interface for the CBehavior class.
//
//////////////////////////////////////////////////////////////////////

#ifndef WARP_BEHAVIOR_H_
#define WARP_BEHAVIOR_H_

#include <string>
#include <vector>
using namespace std;

#include "model/CStep.h"
#include "simulation/CMessage.h"
#include "simulation/CMessageQueue.h"

namespace warp {

	// forward declarations
	class CFunctionBlock;
	class ISimEventAcceptor;

	class CBehavior : public IStepSuccessor
	{
	public:
		typedef vector<CBehavior*> Vector;

		enum {
			LOOP_TYPE_ONCE = 0,
			LOOP_TYPE_REPEAT = 1,
			LOOP_TYPE_UNTIL = 2,
			LOOP_TYPE_UNLESS = 3
		} LoopType;

		CBehavior(const CFunctionBlock& fb, string name, int type, int p, bool addToken);
		virtual ~CBehavior();

		int compare(const CBehavior &other) const;
		bool operator==(const CBehavior &other) const { return compare(other)==0; }
		bool operator!=(const CBehavior &other) const { return compare(other)!=0; }

		string getName() const  { return _name; }
		string getQualifiedName() const;
		string getDotId() const;

		void addStep (CStep* step);
		void addSendTrigger (CBehavior* bhvr);
		void setUnlessCondition (CStep* step);

		bool hasUnlessCondition() const;
		bool getUnlessCondition() const;

		bool hasFinishedOnce() const  { return _finished_once; }
		int getIterations() const  { return _global_iteration; }

		void prepareExecution (void);

		void receiveTrigger (const CStep* from, CMessage* msg, ISimEventAcceptor& eventAcceptor, ILogger& logger);

		// this will be called by the behavior's first step (if waiting for preconditions is over)
		bool isRunning() const  { return _current_msg!=0; }

		// this will be called by the behavior's last step (each time after it is done)
		void lastStepDone (const CStep* from, ISimEventAcceptor& eventAcceptor, ILogger& logger);

		// IStepSuccessor interface
		void done (const CStep* step, ISimEventAcceptor& acceptor);

		void print() const;

	private:
		void handleTrigger (const CStep* from, ISimEventAcceptor& eventAcceptor, ILogger& logger);
		void sendTriggers (const CStep* from, ISimEventAcceptor& eventAcceptor, ILogger& logger);

		void closeAction (ILogger& logger);

		CToken* genToken (CToken* parent, CBehavior* next) const;

		void log (ILogger& logger, int level, CMessage* msg, string action) const;

	private:
		const CFunctionBlock& _fb;
		string _name;
		int _type;
		int _p;
		bool _addToken;

		// list of contained CSteps
		CStep::Vector _steps;

		// condition for "unless" loop
		CStep* _unless_condition;

		// list of send target CBehaviors
		Vector _send_triggers;

		bool _finished_once;
		CMessage* _current_msg;
		int _global_iteration;
		int _iteration;
		CMessageQueue _queue;
		bool _current_unless_condition;
	};

} /* namespace warp */


#endif /* WARP_BEHAVIOR_H_ */
