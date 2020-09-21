#include <iostream>
#include "Varint.h"
#include "utilities.h"

uint64_t DecompressAmount(uint64_t x)
{
	// x = 0  OR  x = 1+10*(9*n + d - 1) + e  OR  x = 1+10*(n - 1) + 9
	if (x == 0)
		return 0;
	x--;
	// x = 10*(9*n + d - 1) + e
	int e = x % 10;
	x /= 10;
	uint64_t n = 0;
	if (e < 9) {
		// x = 9*n + d - 1
		int d = (x % 9) + 1;
		x /= 9;
		// x = n
		n = x*10 + d;
	} else {
		n = x+1;
	}
	while (e) {
		n *= 10;
		e--;
	}
	return n;
}

int main()
{
	std::vector <unsigned char> test;
	utilities::hexstringToBytes("805881d6a88f2c016e4529a080512843f3fbef5c3945376ed8e51b80", test);
//	utilities::hexstringToBytes("802d3203d926e0e02db441e560910b02ddaa1cb7538d0f5e87374b3dda18be3c0d456b74", test);
//	utilities::hexstringToBytes("c0842680ed5900a38f35518de4487c108e3810e6794fb68b189d8b", test);
	utilities::printToHex(test);
	Varint t(test);
	std::vector<unsigned char> first;

	t.decode(0, first);
	bool coinbase = first.back() & 1 ? true : false;
	Varint<std::vector<unsigned char>>::shiftAllBytesRight(first, 1);
	std::string blockHeight;
	utilities::bytesToDecimal(first, blockHeight);
	std::cout << "block height: " << blockHeight << "\n";
	std::cout << "coinbase: " << (coinbase ? "true" : "false") << "\n";

	std::vector<unsigned char> rawAmount;
	ssize_t scriptStart = t.decode(1, rawAmount);

	// No right shift for the amount
	std::string amountDecimalStr;
	utilities::printToHex(rawAmount);
	utilities::bytesToDecimal(rawAmount, amountDecimalStr);
	std::cout << "amount, compressed, decimal: " <<std::dec << amountDecimalStr << "\n";

	char* pEnd;
	long int a = strtol(amountDecimalStr.c_str(), &pEnd, 10);

	uint64_t sats = DecompressAmount((uint64_t)a);
	std::cout << "amount (Satoshis):\t" << sats << "\n"; 
	std::cout << "amount (Bitcoin):\t" << sats / 100'000'000. << "\n"; 

	
	std::vector<unsigned char> script;
	t.remainingBytesFromIndex((size_t) scriptStart, script);
	utilities::printToHex(script);

	return 0;
}
