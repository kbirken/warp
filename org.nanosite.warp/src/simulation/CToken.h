// CToken.h: interface for the CToken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TOKEN_H_INCLUDED_)
#define _TOKEN_H_INCLUDED_

#include <string>
using namespace std;

//#include "CBehavior.h"

// forward decl
class CTokenFactory;

class CToken
{
public:
	typedef int Id;

	virtual ~CToken();

	Id getId() const  { return _id; }

	string getInfo() const;
	string getName() const;

	// reference counting for proper deallocation
	void addRef();
	void removeRef();

	void print() const;

private:
	friend class CTokenFactory;
	CToken (Id id, string info, CToken* parent=0);

	string getNameRecursive() const;

private:
	Id _id;
	CToken* _parent;
	string _info;

	// reference count for proper deallocation
	int _referrers;
};


#endif // !defined(_TOKEN_H_INCLUDED_)
