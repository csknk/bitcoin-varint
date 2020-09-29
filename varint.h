#ifndef VARINT_H
#define VARINT_H 

#include <iostream>
#include <vector>
#include <iomanip>
#include "utilities.h"

template <class T>
class Varint {
private:
	T inputBytes;
	std::vector<size_t> startIndexes;
	std::vector<unsigned char> result;
	void setStartIndexes();
	void processBytes();
	void base128To256(const T& b128, T& b256);

public:
	Varint(T _inputCollection);
	Varint();
	Varint<T>& operator=(const Varint<T>& other);
	Varint<T> getInputBytes();	
	void outputResult();
	ssize_t decode(size_t start, std::vector<unsigned char>& result);
	void remainingBytesFromIndex(size_t start, std::vector<unsigned char>& result);
	void shiftAllBytesRight(size_t shift);
	void static shiftAllBytesRight(T& bytes, size_t shift);
};

class UTXO {
private:
	Varint<std::vector<unsigned char>> inputValue;
	void setHeight();
public:
	UTXO();
	UTXO(Varint<std::vector<unsigned char>>& _inputValue);
	void printUTXO();
	std::vector<unsigned char> scriptPubKey;
	int vout;
	bool coinbase;
	uint64_t height;
	short scriptType;

	friend std::ostream& operator<<(std::ostream& os, UTXO& utxo);
};

#endif /* VARINT_H */
