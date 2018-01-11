// CTokenFactory: implementation of the CTokenFactory class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//#include <algorithm>
//using namespace std;

#include "simulation/CTokenFactory.h"
#include "simulation/CToken.h"



// init singleton pointer
CTokenFactory* CTokenFactory::_instance = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTokenFactory::CTokenFactory () :
	_printInfos(false),
	_next_id(1)
{
}


CTokenFactory::~CTokenFactory()
{
}


CTokenFactory* CTokenFactory::instance()
{
	if (_instance==0) {
		// create singleton instance on the fly
		_instance = new CTokenFactory();
	}

	return _instance;
}


void CTokenFactory::setPrintInfos()
{
	_printInfos = true;
}


CToken* CTokenFactory::createToken (string info, CToken* parent)
{
	int id = _next_id++;
	CToken* token = new CToken(id, info, parent);

	if (_printInfos) {
		printf("          TOKINFO   %s is %s\n", token->getName().c_str(), token->getInfo().c_str());
	}

	return token;
}


void CTokenFactory::print() const
{
	printf("TokenFactory next_id=%d\n", _next_id);
	//printf("%4d %s\n\t\t", _id, getQualifiedName().c_str());
}

