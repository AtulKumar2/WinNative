#include <vector>
#include <string>
#include <iostream>

#include "AllFunctions.h"


class STLTestReal
{
	void StringInVector();
public:
	void Run();
};

void STLTest::Run()
{
	(new STLTestReal())->Run();
	return;
}

void STLTestReal::Run()
{
	StringInVector();
	return;
}

void STLTestReal::StringInVector()
{
	using std::wcout;
	using std::endl;
	using std::vector;
	using std::wstring;
	
	vector<wstring> RandomVect;
	RandomVect.push_back(L"abc");
	RandomVect.push_back(L"def");

	for (unsigned int i=0; i<RandomVect.size(); i++)
	{
		wcout << RandomVect[i].c_str() << endl;
	}

	return;
}

