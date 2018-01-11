// CBehavior: implementation of the CBehavior class.
//
//////////////////////////////////////////////////////////////////////

#include <model/CBehavior.h>
#include "model/CFunctionBlock.h"
#include "model/CStep.h"
#include "simulation/CToken.h"
#include "simulation/CTokenFactory.h"
#include "simulation/ISimEventAcceptor.h"
#include "simulation/ILogger.h"

#include <stdio.h>

#include <algorithm>
#include <string>
using namespace std;

namespace warp {

CBehavior::CBehavior (const CFunctionBlock& fb, string name, int type, int p, bool addToken) :
	_fb(fb),
	_name(name),
	_type(type),
	_p(p),
	_addToken(addToken),
	_unless_condition(0),
	_finished_once(false),
	_current_msg(0),
	_global_iteration(0),
	_iteration(0),
	_current_unless_condition(false)
{
	// TODO: rename p (its semantics depends on _type)
}


CBehavior::~CBehavior()
{
}

int CBehavior::compare(const CBehavior &other) const {
	int fb = _fb.compare(other._fb);
	if (fb!=0)
		return fb;

	int name = _name.compare(other._name);
	if (name!=0)
		return name;

	return 0;
}

string CBehavior::getQualifiedName() const
{
	return _fb.getName() + "::" + _name;
}

string CBehavior::getDotId() const
{
	return _fb.getDotId() + "_" + _name;
}



void CBehavior::addStep (CStep* step)
{
	_steps.push_back(step);
}


void CBehavior::addSendTrigger (CBehavior* bhvr)
{
	_send_triggers.push_back(bhvr);
}

void CBehavior::setUnlessCondition (CStep* step)
{
	_unless_condition = step;
	_current_unless_condition = false;

	// add to successors of step in order to be notified if unless-condition is active
	step->addSuccessor(this);
}

bool CBehavior::hasUnlessCondition() const
{
	return _type==LOOP_TYPE_UNLESS;
}

bool CBehavior::getUnlessCondition() const
{
	return _current_unless_condition;
}


void CBehavior::prepareExecution (void)
{
	//printf("CBehavior::prepareExecution %s\n", getQualifiedName().c_str());

	// prepare all steps for this execution
	for(unsigned int i=0; i<_steps.size(); i++) {
		_steps[i]->prepareExecution();
	}
}


// ***********************************************************************

void CBehavior::receiveTrigger (const CStep* from, CMessage* msg, ISimEventAcceptor& eventAcceptor, ILogger& logger)
{
	log(logger, 2, msg, "RECV ");

	if (_current_msg) {
		// we are busy, just queue this message
		_queue.push(msg);
		return;
	}

	// iteration count for repeat-loops
	_iteration = 0;

	_current_msg = msg;
	log(logger, 2, _current_msg, "START");
	handleTrigger(from, eventAcceptor, logger);
}


void CBehavior::lastStepDone (const CStep* from, ISimEventAcceptor& eventAcceptor, ILogger& logger)
{
	_finished_once = true;

	// last step is done, send triggers
	sendTriggers(from, eventAcceptor, logger);

	// prepare steps for next iteration or trigger
	prepareExecution();

	_global_iteration++;
	_iteration++;

	// TODO: fully implement "repeat" handling based on _type
	switch (_type) {
	case LOOP_TYPE_ONCE:
		// simple behavior, one execution per trigger
		break;
	case LOOP_TYPE_REPEAT:
		// repeat-loop (_p is loop count)
		if (_iteration<_p) {
			handleTrigger(from, eventAcceptor, logger);
			return;
		}
		break;
	case LOOP_TYPE_UNTIL:
		// NIY
		logger.fatal("invalid behavior %s - type %d not yet implemented\n", getQualifiedName().c_str(), _type);
		break;
	case LOOP_TYPE_UNLESS:
		if (! _current_unless_condition) {
			/*
			logger.log("INFO", "still waiting for unless condition in behavior %s: %s",
					getQualifiedName().c_str(),
					_unless_condition->getQualifiedName().c_str());
			*/

			// unless condition still false, do another loop
			handleTrigger(from, eventAcceptor, logger);
			return;
		} else {
			// unless condition is active, loop ends here
			eventAcceptor.signalUnless(_unless_condition, from);
		}
		break;
	default:
		// shouldn't happen
		logger.fatal("invalid behavior %s - unknown type %d\n", getQualifiedName().c_str(), _type);
	}

	// prepare for next incoming message
	_iteration = 0;
	closeAction(logger);

	// check if next message is already waiting
	if (! _queue.isEmpty()) {
		_current_msg = _queue.pop();
		log(logger, 2, _current_msg, "START");
		handleTrigger(_steps.back(), eventAcceptor, logger);
	}
}


// this is called to set 'unless' conditions
void CBehavior::done (const CStep* from, ISimEventAcceptor& eventAcceptor)
{
	// we simply clear unless condition (forever)
	// this will be checked before any execution of the loop
	_current_unless_condition = true;
}


void CBehavior::handleTrigger (const CStep* from, ISimEventAcceptor& eventAcceptor, ILogger& logger)
{
	if (_steps.size()>0) {
		CStep* first = _steps[0];

		if (first->isWaiting()) {
			// this step is waiting for preconditions and will be started later
			if (! (_type==LOOP_TYPE_UNLESS && _iteration>0)) {
				eventAcceptor.signalSend(from, first, false);
			}
		} else {
			// immediately run first step of this behavior
			eventAcceptor.setReady(first);
			if (! (_type==LOOP_TYPE_UNLESS && _iteration>0)) {
				eventAcceptor.signalSend(from, first, true);
			}
		}
	} else {
		// there are no steps in this behavior, recursively call send triggers
		int n = 1;
		if (_type==LOOP_TYPE_REPEAT) {
			n = _p;
		}
		if (_type==LOOP_TYPE_UNLESS) {
			logger.fatal("invalid behavior %s: unless-condition given, but no steps", getQualifiedName().c_str());
		}
		for(int i=0; i<n; i++) {
			sendTriggers(from, eventAcceptor, logger);
		}

		// prepare for next incoming message
		closeAction(logger);
	}
}


void CBehavior::closeAction (ILogger& logger)
{
	log(logger, 1, _current_msg, "READY");

	delete _current_msg;
	_current_msg = 0;
}


void CBehavior::sendTriggers (const CStep* from, ISimEventAcceptor& eventAcceptor, ILogger& logger)
{
	// send triggers to successor behaviors
	CToken* token = _current_msg->getToken();
	for(Vector::iterator it = _send_triggers.begin(); it!=_send_triggers.end(); it++) {
		if (_addToken) {
			// this behavior generates its own tokens
			token = genToken(token, *it);
		}
		CMessage* msg = new CMessage(token);
		(*it)->receiveTrigger(from, msg, eventAcceptor, logger);
	}
}


CToken* CBehavior::genToken (CToken* parent, CBehavior* next) const
{
	static char buf[20];

	string info(next->getQualifiedName());
	if (_type!=LOOP_TYPE_ONCE) {
		sprintf(buf, "%d", _iteration);
		info += "%" + string(buf);
	}

	return CTokenFactory::instance()->createToken(info, parent);
}


void CBehavior::log (ILogger& logger, int level, CMessage* msg, string action) const
{
	if (logger.verbose() > level) {
		logger.log("TOKEN", "%s %s at %s\n",
				msg->getName().c_str(),
				action.c_str(),
				getQualifiedName().c_str()
		);
	}

}


void CBehavior::print() const
{
	printf("Behavior %s\n", _name.c_str());
}

} /* namespace warp */

