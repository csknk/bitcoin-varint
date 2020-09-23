#include <iostream>
#include "varint.h"
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
	utilities::printToHex(test);

	// Initialise an object to access it's Varints by passing in a vector of bytes
	Varint t(test);

	// Initialise a vector to hold the first result
	std::vector<unsigned char> first;

	// Decode the first Varint
	t.decode(0, first);

	// The first Varint in this context represents the block height and coinbase status.
	// The final bit of the final byte in the decoded varint indicates coinbase status.
	bool coinbase = first.back() & 1 ? true : false;

	// The bits preceding the least significant bit need to be shifted right to yield the block height.
	Varint<std::vector<unsigned char>>::shiftAllBytesRight(first, 1);

	// Convert block height represented by first (a base 256 encoded vector of bytes) to a string 
	// for display purposes.
	std::string blockHeight;
	utilities::bytesToDecimal(first, blockHeight);
	std::cout << "block height: " << blockHeight << "\n";
	std::cout << "coinbase: " << (coinbase ? "true" : "false") << "\n";

	// The next Varint represents the (compressed) amount.
	std::vector<unsigned char> rawAmount;

	// The decode() method returns the index of the following byte
	ssize_t scriptStart = t.decode(1, rawAmount);

	// No right shift for the amount
	std::string amountDecimalStr;
	
	// Make a decimal string for presentation purposes.
	utilities::bytesToDecimal(rawAmount, amountDecimalStr);
	std::cout << "amount, compressed, decimal: " <<std::dec << amountDecimalStr << "\n";

	// Convert to a uint64_t type to allow for decompression and further numeric processing
	// (sats to BTC conversion).
	char* pEnd;
	long int a = strtol(amountDecimalStr.c_str(), &pEnd, 10);

	uint64_t sats = DecompressAmount((uint64_t)a);
	std::cout << "amount (Satoshis):\t" << sats << "\n"; 
	std::cout << "amount (Bitcoin):\t" << sats / 100'000'000. << "\n"; 

	// Remaining bytes relate to ScriptPubKey
	std::vector<unsigned char> script;
	t.remainingBytesFromIndex((size_t) scriptStart, script);
	utilities::printToHex(script);

	return 0;
}
