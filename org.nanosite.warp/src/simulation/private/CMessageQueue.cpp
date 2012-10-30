// CMessageQueue: implementation of the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//#include <algorithm>
using namespace std;

#include "simulation/CMessageQueue.h"
#include "simulation/CMessage.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageQueue::CMessageQueue ()
{
}


CMessageQueue::~CMessageQueue()
{
}


int CMessageQueue::isEmpty() const
{
	return _queue.empty();
}


void CMessageQueue::push (CMessage* msg)
{
	_queue.push(msg);
}


CMessage* CMessageQueue::pop (void)
{
	CMessage* msg = _queue.front();
	_queue.pop();

	return(msg);
}


void CMessageQueue::print() const
{
	printf("MessageQueue %08x size=%d\n", (void*)this, _queue.size());
	//printf("%4d %s\n\t\t", _id, getQualifiedName().c_str());
}
