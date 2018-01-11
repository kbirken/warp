// ResourceVector.h: interface for the CResourceVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_RESOURCEVECTOR_H_INCLUDED_)
#define _RESOURCEVECTOR_H_INCLUDED_

#include <vector>
using namespace std;

class CResourceVector  
{
public:
	CResourceVector(int dflt=0);
	CResourceVector(const vector<int>& from);

	CResourceVector(const CResourceVector& rhs);
	CResourceVector& operator=(const CResourceVector& rhs);

	virtual ~CResourceVector();

	static unsigned getNResources()  { return _nResources; }
	static void setNResources (unsigned n)  { _nResources = n; }

	unsigned size() const  { return _values.size(); }
	int& operator[] (unsigned idx)  { return _values[idx]; }
	int operator[] (unsigned idx) const  { return _values[idx]; }

	//bool cumulate (const CResourceVector& from, vector<int>& counts, bool verbose=false);

	void print() const;

private:
	static unsigned _nResources;
	vector<int> _values;
};

#endif // !defined(_RESOURCEVECTOR_H_INCLUDED_)
