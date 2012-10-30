// CMessage: implementation of the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//#include <algorithm>
using namespace std;

#include "simulation/CMessage.h"
#include "simulation/CToken.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessage::CMessage (CToken* token) :
	_token(token)
{
	_token->addRef();
}


CMessage::~CMessage()
{
	_token->removeRef();
}


string CMessage::getName() const
{
	return _token->getName();
}


void CMessage::print() const
{
	printf("Message(%s)\n", _token->getName().c_str());
	//printf("%4d %s\n\t\t", _id, getQualifiedName().c_str());
}
