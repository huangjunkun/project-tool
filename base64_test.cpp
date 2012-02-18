//---------------------------------------------------------------------------
#include <iostream>
#include "base64.h"

using namespace std;

int main(int argc,char* argv[])
{
    { // test base64 endoce and decode.
    using namespace base64;
	const unsigned TEST_CASE_COUNT = 10;
	const string TEST_STR[TEST_CASE_COUNT] = { "", "1", "22", "333", "4444", "55555", "666666", "7777777", "88888888", "999999999" };
	cout << " test case count:" << TEST_CASE_COUNT << "\n";
	for (size_t i = 0; i < TEST_CASE_COUNT; ++i)
	{
		string encode = base64_encrypt(TEST_STR[i]);
		string decode = base64_decrypt(encode);
		std::cout << " string:" << TEST_STR[i] << "\n";
		std::cout << " encode:" << encode << "\n";
		std::cout << " decode:" << decode << "\n";
		std::cout << " -- end --\n";
	}
    }
    cout << " Hello, World !" <<endl;
    return 0;
}


//---------------------------------------------------------------------------




