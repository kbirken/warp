// CMessage.h: interface for the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MESSAGE_H_INCLUDED_)
#define _MESSAGE_H_INCLUDED_

#include <string>
//#include <vector>
using namespace std;

//#include "CBehaviour.h"

// forward decl
class CToken;


class CMessage
{
public:
	//typedef vector<CMessage*> Vector;

	CMessage(CToken* token);
	virtual ~CMessage();

	CToken* getToken() const  { return _token; }
	string getName() const;

	void print() const;

private:
	CToken* _token;
};


#endif // !defined(_MESSAGE_H_INCLUDED_)
