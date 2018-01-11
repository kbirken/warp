// CTokenFactory.h: interface for the CTokenFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TOKEN_FACTORY_H_INCLUDED_)
#define _TOKEN_FACTORY_H_INCLUDED_

#include <string>
using namespace std;

#include "simulation/CToken.h"

// forward decl
//class CToken;

class CTokenFactory
{
public:
	// this is a singleton
	static CTokenFactory* instance();

	// configuration
	void setPrintInfos();

	CToken* createToken (string info, CToken* parent=0);

	void print() const;

private:
	CTokenFactory();
	virtual ~CTokenFactory();

private:
	static CTokenFactory* _instance;
	bool _printInfos;

	CToken::Id _next_id;
};


#endif // !defined(_TOKEN_FACTORY_H_INCLUDED_)
