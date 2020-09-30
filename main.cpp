#include <iostream>
#include "utilities.h"
#include "varint.h"

int main()
{
	std::cout << "This example takes a de-obfuscated value from the Bitcoin chainstate database and "
		"outputs: \n\n"
		"\t- First Varint: Block height, coinbase status\n"
		"\t- Second Varint: Amount\n"
		"\t- The remaining hexstring representing the ScriptPubKey\n\n";

	std::vector <unsigned char> test;

	// Mainnet example
	// ---------------
	// txid: a1f28c43f1f3d4821d0db42707737ea90616613099234f905dfc6ae2b4060000
	// vout: 1
	// tx data:
	// https://blockstream.info/tx/a1f28c43f1f3d4821d0db42707737ea90616613099234f905dfc6ae2b4060000
	// 
	// utilities::hexstringToBytes("c0842680ed5900a38f35518de4487c108e3810e6794fb68b189d8b", test);
	// ---------------
	
	// Regtest example
	// ---------------
	// For data relating to this transaction output, see the data directory in this repo.
	utilities::hexstringToBytes("805881d6a88f2c016e4529a080512843f3fbef5c3945376ed8e51b80", test);
	std::cout << "The starting value is: ";
	utilities::printToHex(test);

	// Initialise an object to access it's Varints by passing in a vector of bytes
	Varint t(test);
	
	// Initialise a UTXO object passing the Varint object as a parameter:
	UTXO u(t);
	std::cout << u;

	return 0;
}
