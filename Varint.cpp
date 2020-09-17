#include "Varint.h"
#define MAXBITS 8

template <class T>
Varint<T>::Varint(T _input) : inputBytes(_input)
{
	setStartIndexes();
	processBytes();
}

template <class T>
void Varint<T>::setStartIndexes()
{
	startIndexes.push_back(0);
	for (size_t i = 0; i < inputBytes.size(); i++) {
		if (inputBytes[i] <= 0x7F && i != (inputBytes.size() - 1)) {
			startIndexes.push_back(i + 1);
		}
	}
}

template <class T>
void Varint<T>::decode(size_t start, std::vector<unsigned char>& result)
{
	std::vector<unsigned char> tmp;
	for (size_t i = start, nBytes = 0; i < inputBytes.size(); i++) {
		nBytes++;
		if (inputBytes[i] <= 0x7F) {
			tmp.push_back(inputBytes[i] & (0xFF >> 1));
			break;
		}
		tmp.push_back((inputBytes[i] & (0xFF >> 1)) + 1);
	}
	base128To256(tmp, result);
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
