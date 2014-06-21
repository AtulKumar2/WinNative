#include <vector>
#include <string>
#include <iostream>

#include "AllFunctions.h"

namespace NetworkTest
{
	class NetworkTest
	{
		void UnreachableHost();
	public:
		void Run();
	};

	void Run()
	{
		(new NetworkTest())->Run();
		return;
	}

	void NetworkTest::Run()
	{
		UnreachableHost();
		return;
	}

	void NetworkTest::UnreachableHost()
	{

	}

}