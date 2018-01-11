// CMessageQueue.h: interface for the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MESSAGE_QUEUE_H_INCLUDED_)
#define _MESSAGE_QUEUE_H_INCLUDED_

//#include <string>
#include <queue>
using namespace std;

//#include "CBehavior.h"

// forward decl
class CMessage;

class CMessageQueue
{
public:

	CMessageQueue();
	virtual ~CMessageQueue();

	int isEmpty() const;
	void push (CMessage* msg);
	CMessage* pop();

	void print() const;

private:
	typedef queue<CMessage*> Queue;
	Queue _queue;
};


#endif // !defined(_MESSAGE_QUEUE_H_INCLUDED_)
