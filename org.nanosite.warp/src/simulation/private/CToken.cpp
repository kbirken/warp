// CToken: implementation of the CToken class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

//#include <algorithm>
using namespace std;

#include "simulation/CToken.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToken::CToken (Id id, string info, CToken* parent) :
	_id(id),
	_info(info),
	_parent(parent),
	_referrers(0)
{
	if (_parent) {
		_parent->addRef();
	}
}

CToken::~CToken()
{
	if (_parent) {
		_parent->removeRef();
	}
}


void CToken::addRef()
{
	_referrers++;
}


void CToken::removeRef()
{
	_referrers--;
	if (_referrers<=0) {
		delete this;
	}
}


string CToken::getInfo() const
{
	string n = _info;
	if (_parent) {
		n = _parent->getInfo() + "/" + n;
	}
	return(n);
}


string CToken::getName() const
{
	return "$" + getNameRecursive() + "$";
}


string CToken::getNameRecursive() const
{
	static char buf[32];
	sprintf(buf, "%d", _id);
	string n(buf);
	if (_parent) {
		n = _parent->getNameRecursive() + "/" + n;
	}
	return(n);
}


void CToken::print() const
{
	printf("Token %d\n", _id);
	//printf("%4d %s\n\t\t", _id, getQualifiedName().c_str());
}
