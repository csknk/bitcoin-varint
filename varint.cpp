#include "varint.h"
//#include "utilities.h"

#define MAXBITS 8

UTXO::UTXO(Varint<std::vector<unsigned char>>& _inputValue)
{
	inputValue = _inputValue;
	setHeight();
	setAmount();
	setScriptPubKey();
}

void UTXO::setHeight()
{
	std::vector<unsigned char> first;
	inputValue.decode(0, first);
	// The first Varint in this context represents the block height and coinbase status.
	// The final bit of the final byte in the decoded varint indicates coinbase status.
	coinbase = first.back() & 1 ? true : false;
	
	// The bits preceding the least significant bit need to be shifted right to yield the block height.
	// This is because the protocol reserves the least significant bit of this Varint as a boolean
	// indicator of the coinbase status of this UTXO.
	Varint<std::vector<unsigned char>>::shiftAllBytesRight(first, 1);

	// Convert block height represented by first (a base 256 encoded vector of bytes) to a string 
	// as an intermediate to avoid expensive math operations.
	std::string blockHeight;
	utilities::bytesToDecimal(first, blockHeight);

	// Convert to a uint64_t type
	char* pEnd;
	height = strtol(blockHeight.c_str(), &pEnd, 10);
}

void UTXO::setAmount()
{
	// The second Varint in the stored database value represents the (compressed) amount.
	std::vector<unsigned char> rawAmount;

	// The decode() method returns the index of the following byte
	scriptStart = inputValue.decode(1, rawAmount);

	std::string amountDecimalStr;
	
	// Make a decimal string.
	utilities::bytesToDecimal(rawAmount, amountDecimalStr);

	// Convert to a uint64_t type to allow for decompression and further numeric processing.
	char* pEnd;
	long int a = strtol(amountDecimalStr.c_str(), &pEnd, 10);
	amount = DecompressAmount((uint64_t)a);
}

void UTXO::setScriptPubKey()
{
	std::vector<unsigned char> in;
	inputValue.remainingBytesFromIndex((size_t) scriptStart, in);
	unsigned char nSize = in[0];

	switch(nSize) {

	// P2PKH Pay to Public Key Hash
	case 0x00:
		scriptPubKey.resize(25);
		scriptPubKey[0] = OP_DUP;
		scriptPubKey[1] = OP_HASH160;
		scriptPubKey[2] = 0x14;
		memcpy(&scriptPubKey[3], &in[1], 20);
		scriptPubKey[23] = OP_EQUALVERIFY;
		scriptPubKey[24] = OP_CHECKSIG;
		break;

	// P2SH Pay to Script Hash
	case 0x01:
		scriptPubKey.resize(23);
		scriptPubKey[0] = OP_HASH160;
		scriptPubKey[1] = 0x14;
		memcpy(&scriptPubKey[2], &in[1], 20);
		scriptPubKey[22] = OP_EQUAL;
		break;
	}
//		return true;

}

// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Bitcoin Core `src/compressor.cpp`: https://github.com/bitcoin/bitcoin/blob/0.20/src/compressor.cpp#L168
uint64_t UTXO::DecompressAmount(uint64_t x)
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

void UTXO::printUTXO()
{
	std::cout << "Block height:\t" << height << "\n";
	std::cout << "Coinbase:\t" << (coinbase ? "true" : "false") << "\n";
}

std::ostream& operator<<(std::ostream& os, UTXO& utxo)
{
	std::string scriptPubKeyString;
       	utilities::bytesToHexstring(utxo.scriptPubKey, scriptPubKeyString);
	os << "Amount (sats):\t" << utxo.amount << "\n";
	os << "Block height:\t" << utxo.height << "\n";
	os << "Coinbase:\t" << (utxo.coinbase ? "true" : "false") << "\n";
	os << "scriptPubKey:\t" << scriptPubKeyString << "\n";
	return os;
}

template <class T>
Varint<T>::Varint() {
	startIndexes = {};
	inputBytes = {};
}

template <class T>
Varint<T>::Varint(T _input) : inputBytes(_input)
{
	setStartIndexes();
	processBytes();
}

template <class T>
Varint<T>& Varint<T>::operator=(const Varint<T>& other)
{
	for (auto& el : other.inputBytes) {
		this->inputBytes.push_back(el);
	}
	this->setStartIndexes();
	this->processBytes();
	return *this;
}

template <class T>
Varint<T> Varint<T>::getInputBytes()
{
	return inputBytes;
}

template <class T>
void Varint<T>::setStartIndexes()
{
	startIndexes.push_back(0);
	for (size_t i = 0; i < inputBytes.size(); i++) {
		if ((inputBytes[i] & 0x80) == 0 && i != (inputBytes.size() - 1)) {
			startIndexes.push_back(i + 1);
		}
	}
}

template <class T>
void Varint<T>::remainingBytesFromIndex(size_t startIndex, std::vector<unsigned char>& result)
{
	for (size_t i = startIndex; i < inputBytes.size(); i++) {
		result.push_back(inputBytes[i]);
	}
}

template <class T>
ssize_t Varint<T>::decode(size_t start, std::vector<unsigned char>& result)
{
	std::vector<unsigned char> tmp;
	size_t nBytes = 0;
	for (size_t i = startIndexes[start]; i < inputBytes.size(); i++) {
		nBytes++;
		if ((inputBytes[i] & 0x80) == 0) {
			tmp.push_back(inputBytes[i] & (0xFF >> 1));
			break;
		}
		tmp.push_back((inputBytes[i] & (0xFF >> 1)) + 1);
	}
	base128To256(tmp, result);

	size_t finalIndex = startIndexes[start] + nBytes;
	return finalIndex < inputBytes.size() - 1 ? (ssize_t)finalIndex : -1;
}

template <class T>
void Varint<T>::base128To256(const T& b128, T& b256)
{
	size_t nBytes = b128.size();
	unsigned char current = 0, carry = 0;
	for (const auto& b : b128) {
		size_t offset = nBytes - 1;
		// Set least significant bits in current, then shift into correct position 
		current = (b & (0xFF << offset)) >> offset;
		// Most significant bits are the last iterations carry
		current |= carry;
		b256.push_back(current);
		
		// Set carry bits and shift into correct position for next iteration.
		carry = b & ~(0xFF << offset); // nBytes == 3, mask is 0000 0011
		carry <<= (MAXBITS - offset);
		nBytes--;
	}
}
	
template <class T>
void Varint<T>::processBytes()
{
	std::vector<unsigned char> tmp;
	size_t nBytes = 0;
	for (const auto& el : inputBytes) {
		nBytes++;
		// Most significant bit set zero denotes last byte
		if (el <= 0x7F) {
//		if ((el & (1 << 7)) == 0) {
			tmp.push_back(el & (0xff >> 1));
			break;
		}
		// For all bytes except the last, save the last 7 bits + 1
		tmp.push_back((el & (0xff >> 1)) + 1);
	}

	unsigned char current = 0, carry = 0;
	for (const auto& b : tmp) {
		size_t offset = nBytes - 1;
		// Set least significant bits in current, then shift into correct position 
		current = (b & (0xFF << offset)) >> offset;
		// Most significant bits are the last iterations carry
		current |= carry;
		result.push_back(current);
		
		// Set carry bits and shift into correct position for next iteration.
		carry = b & ~(0xFF << offset); // nBytes == 3, mask is 0000 0011
		carry <<= (MAXBITS - offset);
		nBytes--;
	}
	shiftAllBytesRight(1);
}

template <class T>
void Varint<T>::outputResult()
{
	for (const auto& el : result) {
		std::cout << (int) el << " ";
	}
	std::cout << "\n";
}

template <class T>
void Varint<T>::shiftAllBytesRight(size_t shift)
{
	for (auto& byte : result) {
		byte >>= shift;
	}
}	

template <class T>
void Varint<T>::shiftAllBytesRight(T& bytes, size_t shift)
{
	for (auto& byte : bytes) {
		byte >>= shift;
	}
}	

template class Varint<std::vector<unsigned char>>;
//template class UTXO<std::vector<unsigned char>>;

